#include "search.h"

#include "attack.h"
#include "movegen.h"
#include "eval.h"
#include "magic.h"
#include "hyperbola.h"
#include "makemove.h"
#include "ttable.h"
#include "io.h"
#include "options.h"
#include "movepicker.h"
#include "time_manager.h"

#include "shims.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define abs(x) ((x) < 0 ? -(x) : (x))
#define clamp(a,b,c) (((a) < (b)) ? (b) : ((a) > (c)) ? (c) : (a))

Move return_bestmove = NOMOVE;

// ClearForSearch handles the cleaning of the post and the info parameters to start search from a clean state
SMALL void ClearForSearch(struct ThreadData* td) {
    // Extract data structures from ThreadData
    struct SearchInfo* info = &td->info;
    memset(&td->nodeSpentTable, 0, sizeof(td->nodeSpentTable));

    // Reset plies and search info
    info->starttime = GetTimeMs();
    info->nodes = 0;
    info->seldepth = 0;
}

static bool StdinHasData()
{
    struct pollfd fds;
    fds.fd = 0;
    fds.events = POLLIN;
    return poll(&fds, 1, 0);
}

// Starts the search process, this is ideally the point where you can start a multithreaded search
SMALL void RootSearch(int depth, struct ThreadData* td) {
    // MainThread search
    SearchPosition(1, depth, td);
    puts_nonewline("bestmove ");
    PrintMove(return_bestmove);
    puts("");
    fflush(stdout);

    // Hack for Kaggle for full repetition detection
#ifdef KAGGLE
    MakeMove(true, return_bestmove, &td->pos);
    ZobristKey opponent_hash = td->pos.posKey;
    UnmakeMove(return_bestmove, &td->pos);
    td->pos.played_positions[td->pos.played_positions_size++] = td->pos.posKey;
    td->pos.played_positions[td->pos.played_positions_size++] = opponent_hash;
#endif
#ifdef UCI
    if (options.Threads == 2) {
#endif
        // start pondering
        NNUE_accumulate(&td->pos.accumStack[0], &td->pos);
        td->pos.accumStackHead = 1;

        MakeMove(true, return_bestmove, &td->pos);
        struct TTEntry tte;
        bool probed = ProbeTTEntry(td->pos.posKey, &tte);
        Move ponder_move = NOMOVE;
        if (probed)
            ponder_move = MoveFromTT(&td->pos, tte.move);
        if (IsPseudoLegal(&td->pos, ponder_move) && IsLegal(&td->pos, ponder_move)) {
            MakeMove(true, ponder_move, &td->pos);
            td->info.timeset = false;
            td->info.stopped = false;
            td->pondering = true;
            SearchPosition(1, MAXDEPTH, td);
            UnmakeMove(ponder_move, &td->pos);
        }
        UnmakeMove(return_bestmove, &td->pos);
        td->pondering = false;
#ifdef UCI
    }
#endif
}

// Returns true if the position is a 2-fold repetition, false otherwise
bool IsRepetition(const struct Position* pos) {
    assert(pos->hisPly >= Position_get50MrCounter(pos));
    // How many moves back should we look at most, aka our distance to the last irreversible move
    int distance = min(Position_get50MrCounter(pos), Position_getPlyFromNull(pos));
    // Get the point our search should start from
    int startingPoint = pos->played_positions_size;
    // Scan backwards from the first position where a repetition is possible (4 half moves ago) for at most distance steps
    for (int index = 4; index <= distance; index += 2)
        // if we found the same position hashkey as the current position
        if (pos->played_positions[startingPoint - index] == pos->posKey) {
            return true;
        }
    return false;
}

// Returns true if the position is a draw via the 50mr rule
bool Is50MrDraw(struct Position* pos) {
    if (Position_get50MrCounter(pos) >= 100) {
        // If there's no risk we are being checkmated return true
        if (!Position_getCheckers(pos))
            return true;

        // if we are in check make sure it's not checkmate 
        struct MoveList moveList;
        moveList.count = 0;
        // generate moves
        GenerateMoves(&moveList, pos, MOVEGEN_ALL);
        for (int i = 0; i < moveList.count; i++) {
            const Move move = moveList.moves[i].move;
            if (IsLegal(pos, move)) {
                return true; // We have at least one legal move, so it is a draw
            }
        }
        return false; // We have no legal moves, it is checkmate
    }

    return false;
}

// If we triggered any of the rules that forces a draw or we know the position is a draw return a draw score
bool IsDraw(struct Position* pos) {
    // if it's a 3-fold repetition, the fifty moves rule kicked in or there isn't enough material on the board to give checkmate then it's a draw
    return IsRepetition(pos)
        || Is50MrDraw(pos)
        || MaterialDraw(pos);
}

SMALL void init_thread_data(struct ThreadData* td) {
    td->pos.side = -1;
    td->pos.hisPly = 0;
    td->pos.posKey = 0ULL;
    td->pos.pawnKey = 0ULL;
    td->pos.whiteNonPawnKey = 0ULL;
    td->pos.blackNonPawnKey = 0ULL;
    td->pos.historyStackHead = 0ULL;

    memset(&td->pos.bitboards, 0, sizeof(Bitboard) * 12);
    memset(&td->nodeSpentTable,0,sizeof(td->nodeSpentTable));

    for (int i = 0; i < 2; i++) {
        td->pos.occupancies[i] = 0;
    }
    td->pos.played_positions_size = 0;

    td->pos.state.castlePerm = 15;
    td->pos.state.capture = EMPTY;
    td->pos.state.enPas = 0;
    td->pos.state.fiftyMove = 0;
    td->pos.state.plyFromNull = 0;
    td->pos.state.checkers = 0;
    td->pos.state.checkMask = fullCheckmask;

    td->info.starttime = 0;
    td->info.stoptimeBaseOpt = 0;
    td->info.stoptimeOpt = 0;
    td->info.stoptimeMax = 0;
    td->info.depth = -1;
    td->info.seldepth = -1;
    td->info.timeset = false;
    td->info.nodes = 0;
    td->info.stopped = false;

    td->nmpPlies = 0;

    td->pondering = false;

    memset(&td->sd, 0, sizeof(struct SearchData));

    for (int i = 0; i < MAXPLY; i++)
    {
        for (int j = 0; j < 2; j++) {
            td->pos.accumStack[i].perspective[j].pov = j;
            td->pos.accumStack[i].perspective[j].NNUEAdd_size = 0;
            td->pos.accumStack[i].perspective[j].NNUESub_size = 0;
            td->pos.accumStack[i].perspective[j].needsRefresh = false;
        }
    }
}

// returns a bitboard of all the attacks to a specific square
static inline Bitboard AttacksTo(const struct Position* pos, int to, Bitboard occ) {
    Bitboard attackingBishops = GetPieceBB(pos, BISHOP) | GetPieceBB(pos, QUEEN);
    Bitboard attackingRooks = GetPieceBB(pos, ROOK) | GetPieceBB(pos, QUEEN);

    return (pawn_attacks[WHITE][to] & Position_GetPieceColorBB(pos, PAWN, BLACK))
        | (pawn_attacks[BLACK][to] & Position_GetPieceColorBB(pos, PAWN, WHITE))
        | (knight_attacks[to] & GetPieceBB(pos, KNIGHT))
        | (king_attacks[to] & GetPieceBB(pos, KING))
        | (GetBishopAttacks(to, occ) & attackingBishops)
        | (GetRookAttacks(to, occ) & attackingRooks);
}

// inspired by the Weiss engine
bool SEE(const struct Position* pos, const int move, const int threshold) {

    // We can't win any material from castling, nor can we lose any
    if (isCastle(move))
        return threshold <= 0;

    int to = To(move);
    int from = From(move);

    int target = isEnpassant(move) ? PAWN : Position_PieceOn(pos, to);
    int promo = getPromotedPiecetype(move);
    int value = SEEValue[target] - threshold;

    // If we promote, we get the promoted piece and lose the pawn
    if (isPromo(move))
        value += SEEValue[promo] - SEEValue[PAWN];

    // If we can't beat the threshold despite capturing the piece,
    // it is impossible to beat the threshold
    if (value < 0)
        return false;

    int attacker = Position_PieceOn(pos, from);

    // If we get captured, we lose the moved piece,
    // or the promoted piece in the case of promotions
    value -= isPromo(move) ? SEEValue[promo] : SEEValue[attacker];

    // If we still beat the threshold after losing the piece,
    // we are guaranteed to beat the threshold
    if (value >= 0)
        return true;

    // It doesn't matter if the to square is occupied or not
    Bitboard occupied = Position_Occupancy(pos, BOTH) ^ (1ULL << from);
    if (isEnpassant(move))
        occupied ^= Position_getEpSquare(pos);

    Bitboard attackers = AttacksTo(pos, to, occupied);

    Bitboard bishops = GetPieceBB(pos, BISHOP) | GetPieceBB(pos, QUEEN);
    Bitboard rooks = GetPieceBB(pos, ROOK) | GetPieceBB(pos, QUEEN);

    int side = Color[attacker] ^ 1;

    // Make captures until one side runs out, or fail to beat threshold
    while (true) {
        // Remove used pieces from attackers
        attackers &= occupied;

        Bitboard myAttackers = attackers & Position_Occupancy(pos, side);
        if (!myAttackers) {
            break;
        }

        // Pick next least valuable piece to capture with
        int pt;
        for (pt = PAWN; pt < KING; ++pt)
            if (myAttackers & GetPieceBB(pos, pt))
                break;

        side ^= 1;

        value = -value - 1 - SEEValue[pt];

        // Value beats threshold, or can't beat threshold (negamaxed)
        if (value >= 0) {
            if (pt == KING && (attackers & Position_Occupancy(pos, side)))
                side ^= 1;
            break;
        }
        // Remove the used piece from occupied
        occupied ^= 1ULL << (GetLsbIndex(myAttackers & Position_GetPieceColorBB(pos, pt, side ^ 1)));

        if (pt == PAWN || pt == BISHOP || pt == QUEEN)
            attackers |= GetBishopAttacks(to, occupied) & bishops;
        if (pt == ROOK || pt == QUEEN)
            attackers |= GetRookAttacks(to, occupied) & rooks;
    }

    return side != Color[attacker];
}

// SearchPosition is the actual function that handles the search, it sets up the variables needed for the search, calls the AspirationWindowSearch function and handles the console output
SMALL void SearchPosition(int startDepth, int finalDepth, struct ThreadData* td) {
    // variable used to store the score of the best move found by the search (while the move itself can be retrieved from the triangular PV table)
    int score = 0;
    int averageScore = SCORE_NONE;

    // Clean the position and the search info to start search from a clean state
    ClearForSearch(td);
    UpdateTableAge();

    // Call the Negamax function in an iterative deepening framework
    for (int currentDepth = startDepth; currentDepth <= finalDepth; currentDepth++) {
        score = AspirationWindowSearch(averageScore, currentDepth, td);
        averageScore = averageScore == SCORE_NONE ? score : (averageScore + score) / 2;

        // use the previous search to adjust some of the time management parameters, do not scale movetime time controls
        if (   td->RootDepth > 7
               && td->info.timeset) {
            ScaleTm(td);
        }

        // check if we just cleared a depth and more than OptTime passed, or we used more than the give nodes
        if (StopEarly(&td->info))
            // Stop main-thread search
            td->info.stopped = true;

        // stop calculating and return best move so far
        if (td->info.stopped)
            break;
    }
}

SMALL int AspirationWindowSearch(int prev_eval, int depth, struct ThreadData* td) {
    int score;
    td->RootDepth = depth;
    struct SearchData* sd = &td->sd;
    struct SearchStack stack[MAXDEPTH + 4], * ss = stack + 4;
    // Explicitly clean stack
    for (int i = -4; i < MAXDEPTH; i++) {
        ss[i].move = NOMOVE;
        ss[i].excludedMove = NOMOVE;
        ss[i].searchKiller = NOMOVE;
        ss[i].staticEval = SCORE_NONE;
        ss[i].doubleExtensions = 0;
        ss[i].contHistEntry = &sd->contHist[td->pos.side ^ ((i + 4) % 2)][PieceTo(NOMOVE)];
    }
    for (int i = 0; i < MAXDEPTH; i++) {
        ss[i].ply = i;
    }
    // We set an expected window for the score at the next search depth, this window is not 100% accurate so we might need to try a bigger window and re-search the position
    int delta = 12;
    // define initial alpha beta bounds
    int alpha = -MAXSCORE;
    int beta = MAXSCORE;

    // only set up the windows is the search depth is bigger or equal than Aspiration_Depth to avoid using windows when the search isn't accurate enough
    if (depth >= 3) {
        alpha = max(-MAXSCORE, prev_eval - delta);
        beta = min(prev_eval + delta, MAXSCORE);
    }

    // Stay at current depth if we fail high/low because of the aspiration windows
    while (true) {
        score = Negamax(alpha, beta, depth, false, td, ss);

        // Check if more than Maxtime passed and we have to stop
        if (TimeOver(&td->info)) {
            td->info.stopped = true;
            break;
        }

        if( td->pondering && StdinHasData()){
            td->info.stopped = true;
            return 0;
        }

        // Stop calculating and return best move so far
        if (td->info.stopped) break;

        // We fell outside the window, so try again with a bigger window, since we failed low we can adjust beta to decrease the total window size
        if (score <= alpha) {
            beta = (alpha + beta) / 2;
            alpha = max(-MAXSCORE, score - delta);
            depth = td->RootDepth;
        }

        // We fell outside the window, so try again with a bigger window
        else if (score >= beta) {
            beta = min(score + delta, MAXSCORE);
            depth = max(depth - 1, td->RootDepth - 5);
        }
        else
            break;
        // Progressively increase how much the windows are increased by at each fail
        delta *= 1.46;
    }
    return score;
}

static bool get_improving(const struct SearchStack *const ss, const bool inCheck) {
    if (inCheck)
        return false;
    if ((ss - 2)->staticEval != SCORE_NONE)
        return ss->staticEval > (ss - 2)->staticEval;
    if ((ss - 4)->staticEval != SCORE_NONE)
        return ss->staticEval > (ss - 4)->staticEval;
    return true;
};

// Negamax alpha beta search
int Negamax(int alpha, int beta, int depth, const bool cutNode, struct ThreadData* td, struct SearchStack* ss) {
    // Extract data structures from ThreadData
    struct Position* pos = &td->pos;
    struct SearchData* sd = &td->sd;
    struct SearchInfo* info = &td->info;

    // Initialize the node
    const bool inCheck = Position_getCheckers(pos);
    const bool rootNode = (ss->ply == 0);
    int eval;
    int rawEval;
    int score = -MAXSCORE;
    struct TTEntry tte;
    TTEntry_init(&tte);

    const Move excludedMove = ss->excludedMove;

    bool pvNode = beta - alpha > 1;

    // Check for the highest depth reached in search to report it to the cli
    if (ss->ply > info->seldepth)
        info->seldepth = ss->ply;

    // Check for early return conditions
    if (!rootNode) {
        // If position is a draw return a draw score
        if (IsDraw(pos))
            return 0;

        // If we reached maxdepth we return a static evaluation of the position
        if (ss->ply >= MAXDEPTH - 1)
            return inCheck ? 0 : EvalPosition(pos);
    }

    // recursion escape condition
    if (depth <= 0)
        return Quiescence(alpha, beta, td, ss);

    // check if more than Maxtime passed and we have to stop
    if (TimeOver(&td->info)) {
        td->info.stopped = true;
        return 0;
    }

    if( td->pondering && info->nodes % 4096 == 0 && StdinHasData()){
        td->info.stopped = true;
        return 0;
    }

    if (!rootNode) {
        // Mate distance pruning
        alpha = max(alpha, -MATE_SCORE + ss->ply);
        beta = min(beta, MATE_SCORE - ss->ply - 1);
        if (alpha >= beta)
            return alpha;
    }

    // Probe the TT for useful previous search informations, we avoid doing so if we are searching a singular extension
    const bool ttHit = !excludedMove && ProbeTTEntry(Position_getPoskey(pos), &tte);
    const int ttScore = ttHit ? ScoreFromTT(tte.score, ss->ply) : SCORE_NONE;
    const Move ttMove = ttHit ? MoveFromTT(pos, tte.move) : NOMOVE;
    const uint8_t ttBound = ttHit ? BoundFromTT(tte.ageBoundPV) : (uint8_t)HFNONE;
    const uint8_t ttDepth = tte.depth;
    // If we found a value in the TT for this position, and the depth is equal or greater we can return it (pv nodes are excluded)
    if (!pvNode
        && ttScore != SCORE_NONE
        && ttDepth >= depth
        && ((ttBound == HFUPPER && ttScore <= alpha)
            || (ttBound == HFLOWER && ttScore >= beta)
            || ttBound == HFEXACT))
        return ttScore;

    const bool ttPv = pvNode || (ttHit && FormerPV(tte.ageBoundPV));

    const bool canIIR = depth >= 4 && ttBound == HFNONE;

    // clean killers and excluded move for the next ply
    (ss + 1)->excludedMove = NOMOVE;
    (ss + 1)->searchKiller = NOMOVE;

    // If we are in check or searching a singular extension we avoid pruning before the move loop
    if (inCheck) {
        eval = rawEval = ss->staticEval = SCORE_NONE;
    }
    else if (excludedMove) {
        eval = rawEval = ss->staticEval;
    }
    // get an evaluation of the position:
    else if (ttHit) {
        // If the value in the TT is valid we use that, otherwise we call the static evaluation function
        rawEval = tte.eval != SCORE_NONE ? tte.eval : EvalPosition(pos);
        eval = ss->staticEval = adjustEvalWithCorrHist(pos, sd, ss, rawEval);

        // We can also use the tt score as a more accurate form of eval
        if (ttScore != SCORE_NONE
            && ((ttBound == HFUPPER && ttScore < eval)
                || (ttBound == HFLOWER && ttScore > eval)
                || ttBound == HFEXACT))
            eval = ttScore;
    }
    else {
        // If we don't have anything in the TT we have to call evalposition
        rawEval = EvalPosition(pos);
        eval = ss->staticEval = adjustEvalWithCorrHist(pos, sd, ss, rawEval);
        // Save the eval into the TT
        StoreTTEntry(pos->posKey, NOMOVE, SCORE_NONE, rawEval, HFNONE, 0, pvNode, ttPv);
    }

    // Improving is a very important modifier to many heuristics. It checks if our static eval has improved since our last move.
    // As we don't evaluate in check, we look for the first ply we weren't in check between 2 and 4 plies ago. If we find that
    // static eval has improved, or that we were in check both 2 and 4 plies ago, we set improving to true.
    const bool improving = get_improving(ss, inCheck);

    if (!pvNode
        && !excludedMove
        && !inCheck) {
        // Reverse futility pruning
        if (depth < 10
            && abs(eval) < MATE_FOUND
            && (ttMove == NOMOVE || isTactical(ttMove))
            && eval - 125 * (depth - improving - canIIR) >= beta)
            return eval - 125 * (depth - improving - canIIR);

        // Null move pruning: If our position is so good that we can give the opponent a free move and still fail high,
        // return early. At higher depth we do a reduced search with null move pruning disabled (ie verification search)
        // to prevent falling into zugzwangs.
        if (eval >= ss->staticEval
            && eval >= beta
            && ss->staticEval >= beta - 30 * depth + 170
            && (ss - 1)->move != NOMOVE
            && depth >= 3
            && ss->ply >= td->nmpPlies
            && BoardHasNonPawns(pos, pos->side)) {

            ss->move = NOMOVE;
            const int R = 4 + depth / 3 + min((eval - beta) / 220, 3);
            ss->contHistEntry = &sd->contHist[!pos->side][PieceTo(NOMOVE)];

            MakeNullMove(pos);
            // Search moves at a reduced depth to find beta cutoffs.
            int nmpScore = -Negamax(-beta, -beta + 1, depth - R - canIIR, !cutNode, td, ss + 1);
            TakeNullMove(pos);

            ss->contHistEntry = &sd->contHist[pos->side][PieceTo(NOMOVE)];

            // fail-soft beta cutoff
            if (nmpScore >= beta) {
                // Don't return unproven mates but still return beta
                if (nmpScore > MATE_FOUND)
                    nmpScore = beta;

                return nmpScore;
            }
        }
        // Razoring
        if (depth <= 5 && eval + 280 * depth < alpha)
        {
            const int razorScore = Quiescence(alpha, beta, td, ss);
            if (razorScore <= alpha)
                return razorScore;
        }
    }

    // old value of alpha
    const int old_alpha = alpha;
    int bestScore = -MAXSCORE;
    Move move;
    Move bestMove = NOMOVE;

    int totalMoves = 0;
    bool skipQuiets = false;

    struct Movepicker mp;
    InitMP(&mp, pos, sd, ss, ttMove, SEARCH, rootNode);

    // Keep track of the played quiet and noisy moves
    struct MoveList quietMoves, noisyMoves;
    quietMoves.count = 0;
    noisyMoves.count = 0;

    // loop over moves within a movelist
    while ((move = NextMove(&mp, skipQuiets)) != NOMOVE) {

        if (move == excludedMove || !IsLegal(pos, move))
            continue;

        totalMoves++;

        const bool isQuiet = !isTactical(move);

        const int moveHistory = GetHistoryScore(pos, sd, move, ss);
        if (!rootNode
            && bestScore > -MATE_FOUND) {

            // lmrDepth is the current depth minus the reduction the move would undergo in lmr, this is helpful because it helps us discriminate the bad moves with more accuracy
            const int lmrDepth = max(0, depth - reductions[isQuiet][min(depth, 63)][min(totalMoves, 63)] + moveHistory / 8072);

            if (!skipQuiets) {

                const int lmplimit = improving ? 3.0 + 1.0 * depth * depth : 1.5 + 0.5 * depth * depth;

                // Movecount pruning: if we searched enough moves and we are not in check we skip the rest
                if (totalMoves > lmplimit) {
                    skipQuiets = true;
                }

                // Futility pruning: if the static eval is so low that even after adding a bonus we are still under alpha we can stop trying quiet moves
                if (!inCheck
                    && lmrDepth < 11
                    && ss->staticEval + 262 + 116 * lmrDepth <= alpha) {
                    skipQuiets = true;
                }
            }
            // See pruning: prune all the moves that have a SEE score that is lower than our threshold
            if (!SEE(pos, move, see_margin[lmrDepth][isQuiet]))
                continue;
        }

        int extension = 0;
        // Limit Extensions to try and curb search explosions
        if (ss->ply < td->RootDepth * 2) {
            // Singular Extensions
            if (!rootNode
                && depth >= 7
                && move == ttMove
                && !excludedMove
                && (ttBound & HFLOWER)
                && abs(ttScore) < MATE_FOUND
                && ttDepth >= depth - 3) {
                const int singularBeta = ttScore - depth;
                const int singularDepth = (depth - 1) / 2;

                ss->excludedMove = ttMove;
                int singularScore = Negamax(singularBeta - 1, singularBeta, singularDepth, cutNode, td, ss);
                ss->excludedMove = NOMOVE;

                if (singularScore < singularBeta) {
                    extension = 1;
                    // Avoid search explosion by limiting the number of double extensions
                    if (!pvNode
                        && singularScore < singularBeta - 17
                        && ss->doubleExtensions <= 11) {
                        extension = 2 + (!isTactical(ttMove) && singularScore < singularBeta - 100);
                        ss->doubleExtensions = (ss - 1)->doubleExtensions + 1;
                        depth += depth < 10;
                    }
                }
                else if (singularScore >= beta)
                    return singularScore;

                // If we didn't successfully extend and our TT score is above beta reduce the search depth
                else if (ttScore >= beta)
                    extension = -2;

                // If we are expecting a fail-high both based on search states from previous plies and based on TT bound
                // but our TT move is not singular and our TT score is failing low, reduce the search depth
                else if (cutNode)
                    extension = -1;
            }
        }
        // we adjust the search depth based on potential extensions
        int newDepth = depth - 1 + extension;
        // Speculative prefetch of the TT entry
        TTPrefetch(keyAfter(pos, move));
        ss->move = move;

        // Play the move
        MakeMove(true, move, pos);
        ss->contHistEntry = &sd->contHist[pos->side][PieceTypeTo(move)];
        // Add any played move to the matching list
        AddMoveNonScored(move, isQuiet ? &quietMoves : &noisyMoves);

        // increment nodes count
        info->nodes++;
        const uint64_t nodesBeforeSearch = info->nodes;
        // Conditions to consider LMR. Calculate how much we should reduce the search depth.
        if (totalMoves > 1 + pvNode && depth >= 3 && (isQuiet || !ttPv)) {
            int depthReduction = reductions[isQuiet][min(depth, 63)][min(totalMoves, 63)];
            if (isQuiet) {
                // Fuck
                if (cutNode)
                    depthReduction += 2;

                // Reduce more if we are not improving
                if (!improving)
                    depthReduction += 1;

                // Reduce less if the move is a refutation
                if (move == mp.killer)
                    depthReduction -= 1;

                // Decrease the reduction for moves that give check
                if (Position_getCheckers(pos))
                    depthReduction -= 1;

                // Reduce less if we have been on the PV
                if (ttPv)
                    depthReduction -= 1 + cutNode;

                // Decrease the reduction for moves that have a good history score and increase it for moves with a bad score
                depthReduction -= moveHistory / 8164;
            }
            else {
                // Fuck
                if (cutNode)
                    depthReduction += 2;

                // Decrease the reduction for moves that have a good history score and increase it for moves with a bad score
                depthReduction -= moveHistory / 6044;
            }

            // adjust the reduction so that we can't drop into Qsearch and to prevent extensions
            depthReduction = clamp(depthReduction, 0, newDepth - 1);

            int reducedDepth = newDepth - depthReduction;
            // search current move with reduced depth:
            score = -Negamax(-alpha - 1, -alpha, reducedDepth, true, td, ss + 1);

            // if we failed high on a reduced node we'll search with a reduced window and full depth
            if (score > alpha && newDepth > reducedDepth) {
                // Based on the value returned by our reduced search see if we should search deeper or shallower, 
                // this is an exact yoink of what SF does and frankly i don't care lmao
                const bool doDeeperSearch = score > (bestScore + 9 + 2 * newDepth);
                const bool doShallowerSearch = score < (bestScore + newDepth);
                newDepth += doDeeperSearch - doShallowerSearch; // fix
                if (newDepth > reducedDepth)
                    score = -Negamax(-alpha - 1, -alpha, newDepth, !cutNode, td, ss + 1);

                int bonus = score > alpha ? history_bonus(depth)
                                          : -history_bonus(depth);

                updateCHScore(ss, move, bonus);
            }
        }
        // If we skipped LMR and this isn't the first move of the node we'll search with a reduced window and full depth
        else if (!pvNode || totalMoves > 1) {
            score = -Negamax(-alpha - 1, -alpha, newDepth, !cutNode, td, ss + 1);
        }

        // PV Search: Search the first move and every move that beat alpha with full depth and a full window
        if (pvNode && (totalMoves == 1 || score > alpha))
            score = -Negamax(-beta, -alpha, newDepth, false, td, ss + 1);

        // take move back
        UnmakeMove(move, pos);
        if (rootNode)
            td->nodeSpentTable[FromTo(move)] += info->nodes - nodesBeforeSearch;

        if (info->stopped)
            return 0;

        // If the score of the current move is the best we've found until now
        if (score > bestScore) {
            // Update what the best score is
            bestScore = score;

            // Found a better move that raised alpha
            if (score > alpha) {
                bestMove = move;
                if (rootNode)
                    return_bestmove = bestMove;

                if (score >= beta) {
                    // If the move that caused the beta cutoff is quiet we have a killer move
                    if (isQuiet) {
                        ss->searchKiller = bestMove;
                    }
                    // Update the history heuristics based on the new best move
                    UpdateHistories(pos, sd, ss, depth + (eval <= alpha), bestMove, &quietMoves, &noisyMoves, rootNode);

                    // node (move) fails high
                    break;
                }
                // Update alpha iff alpha < beta
                alpha = score;
            }
        }
    }

    // We don't have any legal moves to make in the current postion. If we are in singular search, return -infinite.
    // Otherwise, if the king is in check, return a mate score, assuming closest distance to mating position.
    // If we are in neither of these 2 cases, it is stalemate.
    if (totalMoves == 0) {
        return excludedMove ? -MAXSCORE
            : inCheck ? -MATE_SCORE + ss->ply
            : 0;
    }
    // Set the TT bound based on whether we failed high or raised alpha
    int bound = bestScore >= beta ? HFLOWER : alpha != old_alpha ? HFEXACT : HFUPPER;

    if (!excludedMove) {
        if (!inCheck
            && (!bestMove || !isTactical(bestMove))
            && !(bound == HFLOWER && bestScore <= ss->staticEval)
            && !(bound == HFUPPER && bestScore >= ss->staticEval)) {
            updateCorrHistScore(pos, sd, ss, depth, bestScore - ss->staticEval);
        }
        StoreTTEntry(pos->posKey, MoveToTT(bestMove), ScoreToTT(bestScore, ss->ply), rawEval, bound, depth, pvNode, ttPv);
    }

    return bestScore;
}

// Quiescence search to avoid the horizon effect
int Quiescence(int alpha, int beta, struct ThreadData* td, struct SearchStack* ss) {
    struct Position* pos = &td->pos;
    struct SearchData* sd = &td->sd;
    struct SearchInfo* info = &td->info;
    const bool inCheck = Position_getCheckers(pos);
    bool pvNode = beta - alpha > 1;
    // tte is an TT entry, it will store the values fetched from the TT
    struct TTEntry tte;
    int bestScore;
    int rawEval;

    // check if more than Maxtime passed and we have to stop
    if (TimeOver(&td->info)) {
        td->info.stopped = true;
        return 0;
    }

    if(td->pondering && info->nodes % 4096 == 0 && StdinHasData()){
        td->info.stopped = true;
        return 0;
    }

    // If position is a draw return a draw score
    if (MaterialDraw(pos))
        return 0;

    // If we reached maxdepth we return a static evaluation of the position
    if (ss->ply >= MAXDEPTH - 1)
        return inCheck ? 0 : EvalPosition(pos);

    // ttHit is true if and only if we find something in the TT
    const bool ttHit = ProbeTTEntry(Position_getPoskey(pos), &tte);
    const int ttScore = ttHit ? ScoreFromTT(tte.score, ss->ply) : SCORE_NONE;
    const Move ttMove = ttHit ? MoveFromTT(pos, tte.move) : NOMOVE;
    const uint8_t ttBound = ttHit ? BoundFromTT(tte.ageBoundPV) : (uint8_t)HFNONE;
    // If we found a value in the TT for this position, we can return it (pv nodes are excluded)
    if (!pvNode
        && ttScore != SCORE_NONE
        && ((ttBound == HFUPPER && ttScore <= alpha)
            || (ttBound == HFLOWER && ttScore >= beta)
            || ttBound == HFEXACT))
        return ttScore;

    const bool ttPv = pvNode || (ttHit && FormerPV(tte.ageBoundPV));

    if (inCheck) {
        rawEval = ss->staticEval = SCORE_NONE;
        bestScore = -MAXSCORE;
    }
    // If we have a ttHit with a valid eval use that
    else if (ttHit) {

        // If the value in the TT is valid we use that, otherwise we call the static evaluation function
        rawEval = tte.eval != SCORE_NONE ? tte.eval : EvalPosition(pos);
        ss->staticEval = bestScore = adjustEvalWithCorrHist(pos, sd, ss, rawEval);

        // We can also use the TT score as a more accurate form of eval
        if (ttScore != SCORE_NONE
            && ((ttBound == HFUPPER && ttScore < bestScore)
                || (ttBound == HFLOWER && ttScore > bestScore)
                || ttBound == HFEXACT))
            bestScore = ttScore;
    }
    // If we don't have any useful info in the TT just call Evalpos
    else {
        rawEval = EvalPosition(pos);
        bestScore = ss->staticEval = adjustEvalWithCorrHist(pos, sd, ss, rawEval);
        // Save the eval into the TT
        StoreTTEntry(pos->posKey, NOMOVE, SCORE_NONE, rawEval, HFNONE, 0, pvNode, ttPv);
    }

    // Stand pat
    if (bestScore >= beta)
        return bestScore;

    // Adjust alpha based on eval
    alpha = max(alpha, bestScore);

    struct Movepicker mp;
    // If we aren't in check we generate just the captures, otherwise we generate all the moves
    InitMP(&mp, pos, sd, ss, ttMove, QSEARCH, false);

    Move bestmove = NOMOVE;
    Move move;
    int totalMoves = 0;

    // loop over moves within the movelist
    while ((move = NextMove(&mp, !inCheck || bestScore > -MATE_FOUND)) != NOMOVE) {

        if (!IsLegal(pos, move))
            continue;

        totalMoves++;

        // Futility pruning. If static eval is far below alpha, only search moves that win material.
        if (bestScore > -MATE_FOUND
            && !inCheck) {
            const int futilityBase = ss->staticEval + 215;
            if (futilityBase <= alpha && !SEE(pos, move, 1)) {
                bestScore = max(futilityBase, bestScore);
                continue;
            }
        }
        // Speculative prefetch of the TT entry
        TTPrefetch(keyAfter(pos, move));
        ss->move = move;
        // Play the move
        MakeMove(true, move, pos);
        // increment nodes count
        info->nodes++;
        // Call Quiescence search recursively
        const int score = -Quiescence(-beta, -alpha, td, ss + 1);

        // take move back
        UnmakeMove(move, pos);

        if (info->stopped)
            return 0;

        // If the score of the current move is the best we've found until now
        if (score > bestScore) {
            // Update  what the best score is
            bestScore = score;

            // if the score is better than alpha update our best move
            if (score > alpha) {
                bestmove = move;

                // if the score is better than or equal to beta break the loop because we failed high
                if (score >= beta)
                    break;

                // Update alpha iff alpha < beta
                alpha = score;
            }
        }
    }

    // return mate score (assuming closest distance to mating position)
    if (totalMoves == 0 && inCheck) {
        return -MATE_SCORE + ss->ply;
    }
    // Set the TT bound based on whether we failed high, for qsearch we never use the exact bound
    int bound = bestScore >= beta ? HFLOWER : HFUPPER;

    StoreTTEntry(pos->posKey, MoveToTT(bestmove), ScoreToTT(bestScore, ss->ply), rawEval, bound, 0, pvNode, ttPv);

    return bestScore;
}
