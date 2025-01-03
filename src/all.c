#include "all.h"

// not A file constant
static Bitboard not_a_file = 18374403900871474942ULL;

// not H file constant
static Bitboard not_h_file = 9187201950435737471ULL;

// not HG file constant
static Bitboard not_hg_file = 4557430888798830399ULL;

// not AB file constant
static Bitboard not_ab_file = 18229723555195321596ULL;

// generate pawn attacks
SMALL Bitboard MaskPawnAttacks(int side, int square) {
    // result attacks bitboard
    Bitboard attacks = 0ULL;
    // piece bitboard
    Bitboard bitboard = 0ULL;
    // set piece on board
    set_bit(&bitboard, square);
    // white pawns
    if (!side) {
        // generate pawn attacks
        if ((bitboard >> 7) & not_a_file)
            attacks |= (bitboard >> 7);
        if ((bitboard >> 9) & not_h_file)
            attacks |= (bitboard >> 9);
    }
    // black pawns
    else {
        // generate pawn attacks
        if ((bitboard << 7) & not_h_file)
            attacks |= (bitboard << 7);
        if ((bitboard << 9) & not_a_file)
            attacks |= (bitboard << 9);
    }
    // return attack map
    return attacks;
}

// generate knight attacks
SMALL Bitboard MaskKnightAttacks(int square) {
    // result attacks bitboard
    Bitboard attacks = 0ULL;
    // piece bitboard
    Bitboard bitboard = 0ULL;
    // set piece on board
    set_bit(&bitboard, square);
    // generate knight attacks
    if ((bitboard >> 17) & not_h_file)
        attacks |= (bitboard >> 17);
    if ((bitboard >> 15) & not_a_file)
        attacks |= (bitboard >> 15);
    if ((bitboard >> 10) & not_hg_file)
        attacks |= (bitboard >> 10);
    if ((bitboard >> 6) & not_ab_file)
        attacks |= (bitboard >> 6);
    if ((bitboard << 17) & not_a_file)
        attacks |= (bitboard << 17);
    if ((bitboard << 15) & not_h_file)
        attacks |= (bitboard << 15);
    if ((bitboard << 10) & not_ab_file)
        attacks |= (bitboard << 10);
    if ((bitboard << 6) & not_hg_file)
        attacks |= (bitboard << 6);

    // return attack map
    return attacks;
}

// generate king attacks
SMALL Bitboard MaskKingAttacks(int square) {
    // result attacks bitboard
    Bitboard attacks = 0ULL;

    // piece bitboard
    Bitboard bitboard = 0ULL;

    // set piece on board
    set_bit(&bitboard, square);

    // generate king attacks
    if (bitboard >> 8)
        attacks |= (bitboard >> 8);
    if ((bitboard >> 9) & not_h_file)
        attacks |= (bitboard >> 9);
    if ((bitboard >> 7) & not_a_file)
        attacks |= (bitboard >> 7);
    if ((bitboard >> 1) & not_h_file)
        attacks |= (bitboard >> 1);
    if (bitboard << 8)
        attacks |= (bitboard << 8);
    if ((bitboard << 9) & not_a_file)
        attacks |= (bitboard << 9);
    if ((bitboard << 7) & not_h_file)
        attacks |= (bitboard << 7);
    if ((bitboard << 1) & not_a_file)
        attacks |= (bitboard << 1);

    // return attack map
    return attacks;
}

// retrieves attacks for a generic piece (except pawns and kings because that requires actual work)
Bitboard pieceAttacks(int piecetype, int pieceSquare, Bitboard occ) {
    assert(piecetype > PAWN && piecetype < KING);
    switch (piecetype) {
    case KNIGHT:
        return knight_attacks[pieceSquare];
    case BISHOP:
        return GetBishopAttacks(pieceSquare, occ);
    case ROOK:
        return GetRookAttacks(pieceSquare, occ);
    case QUEEN:
        return GetQueenAttacks(pieceSquare, occ);
    default:
        __builtin_unreachable();
    }
}

#if BENCH
// Benchmarks from Bitgenie
#define bench_count 52
const char* benchmarkfens[bench_count] = {
    "r3k2r/2pb1ppp/2pp1q2/p7/1nP1B3/1P2P3/P2N1PPP/R2QK2R w KQkq a6 0 14",
    "4rrk1/2p1b1p1/p1p3q1/4p3/2P2n1p/1P1NR2P/PB3PP1/3R1QK1 b - - 2 24",
    "r3qbrk/6p1/2b2pPp/p3pP1Q/PpPpP2P/3P1B2/2PB3K/R5R1 w - - 16 42",
    "6k1/1R3p2/6p1/2Bp3p/3P2q1/P7/1P2rQ1K/5R2 b - - 4 44",
    "8/8/1p2k1p1/3p3p/1p1P1P1P/1P2PK2/8/8 w - - 3 54",
    "7r/2p3k1/1p1p1qp1/1P1Bp3/p1P2r1P/P7/4R3/Q4RK1 w - - 0 36",
    "r1bq1rk1/pp2b1pp/n1pp1n2/3P1p2/2P1p3/2N1P2N/PP2BPPP/R1BQ1RK1 b - - 2 10",
    "3r3k/2r4p/1p1b3q/p4P2/P2Pp3/1B2P3/3BQ1RP/6K1 w - - 3 87",
    "2r4r/1p4k1/1Pnp4/3Qb1pq/8/4BpPp/5P2/2RR1BK1 w - - 0 42",
    "4q1bk/6b1/7p/p1p4p/PNPpP2P/KN4P1/3Q4/4R3 b - - 0 37",
    "2q3r1/1r2pk2/pp3pp1/2pP3p/P1Pb1BbP/1P4Q1/R3NPP1/4R1K1 w - - 2 34",
    "1r2r2k/1b4q1/pp5p/2pPp1p1/P3Pn2/1P1B1Q1P/2R3P1/4BR1K b - - 1 37",
    "r3kbbr/pp1n1p1P/3ppnp1/q5N1/1P1pP3/P1N1B3/2P1QP2/R3KB1R b KQkq b3 0 17",
    "8/6pk/2b1Rp2/3r4/1R1B2PP/P5K1/8/2r5 b - - 16 42",
    "1r4k1/4ppb1/2n1b1qp/pB4p1/1n1BP1P1/7P/2PNQPK1/3RN3 w - - 8 29",
    "8/p2B4/PkP5/4p1pK/4Pb1p/5P2/8/8 w - - 29 68",
    "3r4/ppq1ppkp/4bnp1/2pN4/2P1P3/1P4P1/PQ3PBP/R4K2 b - - 2 20",
    "5rr1/4n2k/4q2P/P1P2n2/3B1p2/4pP2/2N1P3/1RR1K2Q w - - 1 49",
    "1r5k/2pq2p1/3p3p/p1pP4/4QP2/PP1R3P/6PK/8 w - - 1 51",
    "q5k1/5ppp/1r3bn1/1B6/P1N2P2/BQ2P1P1/5K1P/8 b - - 2 34",
    "r1b2k1r/5n2/p4q2/1ppn1Pp1/3pp1p1/NP2P3/P1PPBK2/1RQN2R1 w - - 0 22",
    "r1bqk2r/pppp1ppp/5n2/4b3/4P3/P1N5/1PP2PPP/R1BQKB1R w KQkq - 0 5",
    "r1bqr1k1/pp1p1ppp/2p5/8/3N1Q2/P2BB3/1PP2PPP/R3K2n b Q - 1 12",
    "r1bq2k1/p4r1p/1pp2pp1/3p4/1P1B3Q/P2B1N2/2P3PP/4R1K1 b - - 2 19",
    "r4qk1/6r1/1p4p1/2ppBbN1/1p5Q/P7/2P3PP/5RK1 w - - 2 25",
    "r7/6k1/1p6/2pp1p2/7Q/8/p1P2K1P/8 w - - 0 32",
    "r3k2r/ppp1pp1p/2nqb1pn/3p4/4P3/2PP4/PP1NBPPP/R2QK1NR w KQkq - 1 5",
    "3r1rk1/1pp1pn1p/p1n1q1p1/3p4/Q3P3/2P5/PP1NBPPP/4RRK1 w - - 0 12",
    "5rk1/1pp1pn1p/p3Brp1/8/1n6/5N2/PP3PPP/2R2RK1 w - - 2 20",
    "8/1p2pk1p/p1p1r1p1/3n4/8/5R2/PP3PPP/4R1K1 b - - 3 27",
    "8/4pk2/1p1r2p1/p1p4p/Pn5P/3R4/1P3PP1/4RK2 w - - 1 33",
    "8/5k2/1pnrp1p1/p1p4p/P6P/4R1PK/1P3P2/4R3 b - - 1 38",
    "8/8/1p1kp1p1/p1pr1n1p/P6P/1R4P1/1P3PK1/1R6 b - - 15 45",
    "8/8/1p1k2p1/p1prp2p/P2n3P/6P1/1P1R1PK1/4R3 b - - 5 49",
    "8/8/1p4p1/p1p2k1p/P2npP1P/4K1P1/1P6/3R4 w - - 6 54",
    "8/8/1p4p1/p1p2k1p/P2n1P1P/4K1P1/1P6/6R1 b - - 6 59",
    "8/5k2/1p4p1/p1pK3p/P2n1P1P/6P1/1P6/4R3 b - - 14 63",
    "8/1R6/1p1K1kp1/p6p/P1p2P1P/6P1/1Pn5/8 w - - 0 67",
    "1rb1rn1k/p3q1bp/2p3p1/2p1p3/2P1P2N/PP1RQNP1/1B3P2/4R1K1 b - - 4 23",
    "4rrk1/pp1n1pp1/q5p1/P1pP4/2n3P1/7P/1P3PB1/R1BQ1RK1 w - - 3 22",
    "r2qr1k1/pb1nbppp/1pn1p3/2ppP3/3P4/2PB1NN1/PP3PPP/R1BQR1K1 w - - 4 12",
    "2r2k2/8/4P1R1/1p6/8/P4K1N/7b/2B5 b - - 0 55",
    "6k1/5pp1/8/2bKP2P/2P5/p4PNb/B7/8 b - - 1 44",
    "2rqr1k1/1p3p1p/p2p2p1/P1nPb3/2B1P3/5P2/1PQ2NPP/R1R4K w - - 3 25",
    "r1b2rk1/p1q1ppbp/6p1/2Q5/8/4BP2/PPP3PP/2KR1B1R b - - 2 14",
    "6r1/5k2/p1b1r2p/1pB1p1p1/1Pp3PP/2P1R1K1/2P2P2/3R4 w - - 1 36",
    "rnbqkb1r/pppppppp/5n2/8/2PP4/8/PP2PPPP/RNBQKBNR b KQkq c3 0 2",
    "2rr2k1/1p4bp/p1q1p1p1/4Pp1n/2PB4/1PN3P1/P3Q2P/2RR2K1 w - f6 0 20",
    "3br1k1/p1pn3p/1p3n2/5pNq/2P1p3/1PN3PP/P2Q1PB1/4R1K1 w - - 0 23",
    "2r2b2/5p2/5k2/p1r1pP2/P2pB3/1P3P2/K1P3R1/7R w - - 23 93" ,
    "8/P6p/2K1q1pk/2Q5/4p3/8/7P/8 w - - 4 44",
    "7k/8/7P/5B2/5K2/8/8/8 b - - 0 175"
};


void StartBench(int depth) {
    // init all
    struct ThreadData td;

    init_thread_data(&td);

    uint64_t totalNodes = 0;
    InitTT(64);
    InitNewGame(&td);
    size_t start = GetTimeMs();
    for (int positions = 0; positions < bench_count; positions++) {
        ParseFen(benchmarkfens[positions], &td.pos);
        printf("\nPosition: %d fen: %s \n", positions + 1, (const size_t)benchmarkfens[positions]);
        RootSearch(depth, &td);
        totalNodes += td.info.nodes;
    }
    size_t end = GetTimeMs();
    size_t totalTime = end - start;
    printf("\n%d nodes %d nps\n", totalNodes, (int)(totalNodes / (totalTime + 1) * 1000));
}

#else
void StartBench(int depth) {}
#endif

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define clamp(a,b,c) (((a) < (b)) ? (b) : ((a) > (c)) ? (c) : (a))

// if we don't have enough material to mate consider the position a draw
bool MaterialDraw(const struct Position* pos) {
    // If we only have kings on the board then it's a draw
    if (Position_PieceCount(pos) == 2)
        return true;
    // KN v K, KB v K
    else if (Position_PieceCount(pos) == 3 && ((CountBits(GetPieceBB(pos, KNIGHT)) == 1) || (CountBits(GetPieceBB(pos, BISHOP)) == 1)))
        return true;
    // If we have 4 pieces on the board
    else if (Position_PieceCount(pos) == 4) {
        // KNN v K, KN v KN
        if ((CountBits(GetPieceBB(pos, KNIGHT)) == 2))
            return true;
        // KB v KB
        else if (((CountBits(GetPieceBB(pos, BISHOP)) == 2)) && CountBits(Position_GetPieceColorBB(pos, BISHOP, WHITE)) == 1)
            return true;
    }

    return false;
}

int ScaleMaterial(const struct Position* pos, int eval) {
    const int knights = CountBits(GetPieceBB(pos, KNIGHT));
    const int bishops = CountBits(GetPieceBB(pos, BISHOP));
    const int rooks = CountBits(GetPieceBB(pos, ROOK));
    const int queens = CountBits(GetPieceBB(pos, QUEEN));
    const int phase = min(3 * knights + 3 * bishops + 5 * rooks + 10 * queens, 64);
    // Scale between [0.75, 1.00]
    return eval * (192 + phase) / 256;
}

int EvalPositionRaw(struct Position* pos) {
    // Update accumulators to ensure we are up to date on the current board state
    NNUE_update(Position_AccumulatorTop(pos), pos);
    const int pieceCount = Position_PieceCount(pos);
    const int outputBucket = min((63 - pieceCount) * (32 - pieceCount) / 225, 7);
    return NNUE_output(Position_AccumulatorTop(pos), pos->side, outputBucket);
}

// position evaluation
int EvalPosition(struct Position* pos) {
    int eval = EvalPositionRaw(pos);
    eval = ScaleMaterial(pos, eval);
    eval = eval * (200 - Position_get50MrCounter(pos)) / 200;
    eval = (eval / 16) * 16 - 1 + (pos->posKey & 0x2);
    // Clamp eval to avoid it somehow being a mate score
    eval = clamp(eval, -MATE_FOUND + 1, MATE_FOUND - 1);
    return eval;
}

#define min(a,b) (((a) < (b)) ? (a) : (b))
#define clamp(a,b,c) (((a) < (b)) ? (b) : ((a) > (c)) ? (c) : (a))
#define abs(x) ((x) < 0 ? -(x) : (x))


/* History updating works in the same way for all histories, we have 3 methods:
updateScore: this updates the score of a specific entry of <History-name> type
UpdateHistories : this performs a general update of all the heuristics, giving the bonus to the best move and a malus to all the other
GetScore: this is simply a getter for a specific entry of the history table
*/

int history_bonus(const int depth) {
    return min(16 * depth * depth + 32 * depth + 16, 1200);
}

void updateSingleCHScore(struct SearchStack* ss, const Move move, const int bonus, const int offset) {
    if ((ss - offset)->move) {
        // Scale bonus to fix it in a [-CH_MAX;CH_MAX] range
        const int scaledBonus = bonus - GetSingleCHScore(ss, move, offset) * abs(bonus) / CH_MAX;
        (*((ss - offset)->contHistEntry))[PieceTypeTo(move)] += scaledBonus;
    }
}

void updateCHScore(struct SearchStack* ss, const Move move, const int bonus) {
    // Update move score
    updateSingleCHScore(ss, move, bonus, 1);
    updateSingleCHScore(ss, move, bonus, 2);
    updateSingleCHScore(ss, move, bonus, 4);
}

void updateHHScore(const struct Position* pos, struct SearchData* sd, const Move move, const int bonus) {
    // Scale bonus to fix it in a [-HH_MAX;HH_MAX] range
    const int scaledBonus = bonus - GetHHScore(pos, sd, move) * abs(bonus) / HH_MAX;
    // Update move score
    sd->searchHistory[pos->side][FromTo(move)] += scaledBonus;
}

void updateCapthistScore(const struct Position* pos, struct SearchData* sd, const Move move, const int bonus) {
    // Scale bonus to fix it in a [-CAPTHIST_MAX;CAPTHIST_MAX] range
    const int scaledBonus = bonus - GetCapthistScore(pos, sd, move) * abs(bonus) / CAPTHIST_MAX;
    int capturedPiece = isEnpassant(move) ? PAWN : GetPieceType(Position_PieceOn(pos, To(move)));
    // If we captured an empty piece this means the move is a promotion, we can pretend we captured a pawn to use a slot of the table that would've otherwise went unused (you can't capture pawns on the 1st/8th rank)
    if (capturedPiece == EMPTY) capturedPiece = PAWN;
    // Update move score
    sd->captHist[PieceTo(move)][capturedPiece] += scaledBonus;
}

// Update all histories
void UpdateHistories(const struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const int depth, const Move bestMove, const struct MoveList* quietMoves, const struct MoveList* noisyMoves, const bool rootNode) {
    const int bonus = history_bonus(depth);
    if (!isTactical(bestMove))
    {
        // increase bestMove HH and CH score
        updateHHScore(pos, sd, bestMove, bonus);
        updateCHScore(ss, bestMove, bonus);

        // Loop through all the quiet moves
        for (int i = 0; i < quietMoves->count; i++) {
            // For all the quiets moves that didn't cause a cut-off decrease the HH score
            const Move move = quietMoves->moves[i].move;
            if (move == bestMove) continue;
            updateHHScore(pos, sd, move, -bonus);
            updateCHScore(ss, move, -bonus);
        }
    }
    else {
        // increase the bestMove Capthist score
        updateCapthistScore(pos, sd, bestMove, bonus);
    }
    // For all the noisy moves that didn't cause a cut-off, even is the bestMove wasn't a noisy move, decrease the capthist score
    for (int i = 0; i < noisyMoves->count; i++) {
        const Move move = noisyMoves->moves[i].move;
        if (move == bestMove) continue;
        updateCapthistScore(pos, sd, move, -bonus);
    }
}

// Returns the history score of a move
int GetHHScore(const struct Position* pos, const struct SearchData* sd, const Move move) {
    return sd->searchHistory[pos->side][FromTo(move)];
}

// Returns the history score of a move
int GetCHScore(const struct SearchStack* ss, const Move move) {
    return   GetSingleCHScore(ss, move, 1)
        + GetSingleCHScore(ss, move, 2)
        + GetSingleCHScore(ss, move, 4);
}

int GetSingleCHScore(const struct SearchStack* ss, const Move move, const int offset) {
    return (ss - offset)->move ? (*((ss - offset)->contHistEntry))[PieceTypeTo(move)]
        : 0;
}

// Returns the history score of a move
int GetCapthistScore(const struct Position* pos, const struct SearchData* sd, const Move move) {
    int capturedPiece = isEnpassant(move) ? PAWN : GetPieceType(Position_PieceOn(pos, To(move)));
    // If we captured an empty piece this means the move is a non capturing promotion, we can pretend we captured a pawn to use a slot of the table that would've otherwise went unused (you can't capture pawns on the 1st/8th rank)
    if (capturedPiece == EMPTY) capturedPiece = PAWN;
    return sd->captHist[PieceTo(move)][capturedPiece];
}

void updateSingleCorrHistScore(short* entry, const int scaledDiff, const int newWeight) {
    *entry = clamp((*entry * (CORRHIST_WEIGHT_SCALE - newWeight) + scaledDiff * newWeight) / CORRHIST_WEIGHT_SCALE, -CORRHIST_MAX, CORRHIST_MAX);
}

void updateCorrHistScore(const struct Position* pos, struct SearchData* sd, const struct SearchStack* ss, const int depth, const int diff) {
    const int scaledDiff = diff * CORRHIST_GRAIN;
    const int newWeight = min(depth * depth + 2 * depth + 1, 128);
    assert(newWeight <= CORRHIST_WEIGHT_SCALE);

    updateSingleCorrHistScore(&sd->pawnCorrHist[pos->side][pos->pawnKey % CORRHIST_SIZE], scaledDiff, newWeight);
    updateSingleCorrHistScore(&sd->whiteNonPawnCorrHist[pos->side][pos->whiteNonPawnKey % CORRHIST_SIZE], scaledDiff, newWeight);
    updateSingleCorrHistScore(&sd->blackNonPawnCorrHist[pos->side][pos->blackNonPawnKey % CORRHIST_SIZE], scaledDiff, newWeight);
}

int adjustEvalWithCorrHist(const struct Position* pos, const struct SearchData* sd, const int rawEval) {
    int adjustment = 0;

    adjustment += sd->pawnCorrHist[pos->side][pos->pawnKey % CORRHIST_SIZE];
    adjustment += sd->whiteNonPawnCorrHist[pos->side][pos->whiteNonPawnKey % CORRHIST_SIZE];
    adjustment += sd->blackNonPawnCorrHist[pos->side][pos->blackNonPawnKey % CORRHIST_SIZE];

    return clamp(rawEval + adjustment / CORRHIST_GRAIN, -MATE_FOUND + 1, MATE_FOUND - 1);
}

int GetHistoryScore(const struct Position* pos, const struct SearchData* sd, const Move move, const struct SearchStack* ss) {
    if (!isTactical(move))
        return GetHHScore(pos, sd, move) + GetCHScore(ss, move);
    else
        return GetCapthistScore(pos, sd, move);
}

// Resets the history tables
SMALL void CleanHistories(struct SearchData* sd) {
    memset(sd->searchHistory, 0, sizeof(sd->searchHistory));
    memset(sd->contHist, 0, sizeof(sd->contHist));
    memset(sd->captHist, 0, sizeof(sd->captHist));
    memset(sd->pawnCorrHist, 0, sizeof(sd->pawnCorrHist));
    memset(sd->whiteNonPawnCorrHist, 0, sizeof(sd->whiteNonPawnCorrHist));
    memset(sd->blackNonPawnCorrHist, 0, sizeof(sd->blackNonPawnCorrHist));
}

const Bitboard file_bbs[] =
{
    0x101010101010101ULL,
    0x202020202020202ULL,
    0x404040404040404ULL,
    0x808080808080808ULL,
    0x1010101010101010ULL,
    0x2020202020202020ULL,
    0x4040404040404040ULL,
    0x8080808080808080ULL
};

const Bitboard rank_bbs[] =
{
    0xFFULL,
    0xFF00ULL,
    0xFF0000ULL,
    0xFF000000ULL,
    0xFF00000000ULL,
    0xFF0000000000ULL,
    0xFF000000000000ULL,
    0xFF00000000000000ULL
};

const Bitboard diagonal_bbs[] =
{
    0x1ULL,
    0x102ULL,
    0x10204ULL,
    0x1020408ULL,
    0x102040810ULL,
    0x10204081020ULL,
    0x1020408102040ULL,
    0x102040810204080ULL,
    0x204081020408000ULL,
    0x408102040800000ULL,
    0x810204080000000ULL,
    0x1020408000000000ULL,
    0x2040800000000000ULL,
    0x4080000000000000ULL,
    0x8000000000000000ULL
};

const Bitboard antidiagonal_bbs[] =
{
    0x80ULL,
    0x8040ULL,
    0x804020ULL,
    0x80402010ULL,
    0x8040201008ULL,
    0x804020100804ULL,
    0x80402010080402ULL,
    0x8040201008040201ULL,
    0x4020100804020100ULL,
    0x2010080402010000ULL,
    0x1008040201000000ULL,
    0x804020100000000ULL,
    0x402010000000000ULL,
    0x201000000000000ULL,
    0x100000000000000ULL
};

Bitboard ReverseBits(Bitboard bitboard)
{
    const Bitboard temp1 = 0x5555555555555555ULL;
    const Bitboard temp2 = 0x3333333333333333ULL;
    const Bitboard temp3 = 0x0F0F0F0F0F0F0F0FULL;
    const Bitboard temp4 = 0x00FF00FF00FF00FFULL;
    const Bitboard temp5 = 0x0000FFFF0000FFFFULL;
    bitboard = ((bitboard >> 1) & temp1) | ((bitboard & temp1) << 1);
    bitboard = ((bitboard >> 2) & temp2) | ((bitboard & temp2) << 2);
    bitboard = ((bitboard >> 4) & temp3) | ((bitboard & temp3) << 4);
    bitboard = ((bitboard >> 8) & temp4) | ((bitboard & temp4) << 8);
    bitboard = ((bitboard >> 16) & temp5) | ((bitboard & temp5) << 16);
    bitboard = (bitboard >> 32) | (bitboard << 32);
    return bitboard;
}

Bitboard MaskedSlide(const Bitboard allPieces, const Bitboard pieceBitboard, const Bitboard mask)
{
    const Bitboard left = ((allPieces & mask) - 2 * pieceBitboard);
    const Bitboard right = ReverseBits(ReverseBits(allPieces & mask) - 2 * ReverseBits(pieceBitboard));
    const Bitboard both = left ^ right;
    const Bitboard slide = both & mask;
    return slide;
}

// get bishop attacks
Bitboard GetBishopAttacks(int square, Bitboard occupancy) {
    const Bitboard pieceBitboard = 1ULL << square;
    const Bitboard diagonal = MaskedSlide(occupancy, pieceBitboard, diagonal_bbs[square / 8 + square % 8]);
    const Bitboard antidiagonal = MaskedSlide(occupancy, pieceBitboard, antidiagonal_bbs[square / 8 + 7 - square % 8]);
    return diagonal | antidiagonal;
}

// get rook attacks
Bitboard GetRookAttacks(int square, Bitboard occupancy) {
    const Bitboard pieceBitboard = 1ULL << square;
    const Bitboard horizontal = MaskedSlide(occupancy, pieceBitboard, rank_bbs[square / 8]);
    const Bitboard vertical = MaskedSlide(occupancy, pieceBitboard, file_bbs[square % 8]);
    return horizontal | vertical;
}

// get queen attacks
Bitboard GetQueenAttacks(const int square, Bitboard occupancy) {
    return GetBishopAttacks(square, occupancy) | GetRookAttacks(square, occupancy);
}

Bitboard PieceKeys[12][64];
Bitboard enpassant_keys[64];
Bitboard SideKey;
Bitboard CastleKeys[16];

// pawn attacks table [side][square]
Bitboard pawn_attacks[2][64];

// knight attacks table [square]
Bitboard knight_attacks[64];

// king attacks table [square]
Bitboard king_attacks[64];

Bitboard SQUARES_BETWEEN_BB[64][64];

// pseudo random number state
static uint64_t random_state = 6379633040001738036;

// generate 64-bit pseudo legal numbers
SMALL static uint64_t GetRandomU64Number() {
    // get current state
    uint64_t number = random_state;

    // XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 7;
    number ^= number << 17;

    // update random number state
    random_state = number;

    // return random number
    return number;
}

// Initialize the Zobrist keys
SMALL void initHashKeys() {
    for (int Typeindex = WP; Typeindex <= BK; ++Typeindex) {
        for (int squareIndex = 0; squareIndex < 64; ++squareIndex) {
            PieceKeys[Typeindex][squareIndex] = GetRandomU64Number();
        }
    }
    // loop over board squares
    for (int square = 0; square < 64; square++)
        // init random enpassant keys
        enpassant_keys[square] = GetRandomU64Number();

    // loop over castling keys
    for (int index = 0; index < 16; index++)
        // init castling keys
        CastleKeys[index] = GetRandomU64Number();

    // init random side key
    SideKey = GetRandomU64Number();
}

// init attack tables for all the piece types, indexable by square
SMALL void InitAttackTables() {
    for (int square = 0; square < 64; square++) {
        // init pawn attacks
        pawn_attacks[WHITE][square] = MaskPawnAttacks(WHITE, square);
        pawn_attacks[BLACK][square] = MaskPawnAttacks(BLACK, square);

        // init knight attacks
        knight_attacks[square] = MaskKnightAttacks(square);

        // init king attacks
        king_attacks[square] = MaskKingAttacks(square);
    }
}

SMALL void initializeLookupTables() {
    // initialize squares between table
    Bitboard sqs;
    for (int sq1 = 0; sq1 < 64; ++sq1) {
        for (int sq2 = 0; sq2 < 64; ++sq2) {
            sqs = (1ULL << sq1) | (1ULL << sq2);
            if (get_file(sq1) == get_file(sq2) || get_rank[sq1] == get_rank[sq2])
                SQUARES_BETWEEN_BB[sq1][sq2] = GetRookAttacks(sq1, sqs) & GetRookAttacks(sq2, sqs);
            else if (get_diagonal[sq1] == get_diagonal[sq2] || get_antidiagonal(sq1) == get_antidiagonal(sq2))
                SQUARES_BETWEEN_BB[sq1][sq2] = GetBishopAttacks(sq1, sqs) & GetBishopAttacks(sq2, sqs);
        }
    }
}

// PreCalculate the logarithms used in the reduction calculation
SMALL void InitReductions() {

    for (int depth = 0; depth < 64; depth++) {

        see_margin[depth][1] = -80.0 * depth; // Quiet moves
        see_margin[depth][0] = -30.0 * depth * depth; // Non quiets

    }
}

SMALL void InitAll() {
    TT.pTable = NULL;
    // init attacks tables
    InitAttackTables();
    initializeLookupTables();
    initHashKeys();
    InitReductions();
    // Init TT
    InitTT(1);
    NNUE_init();
}

SMALL void InitNewGame(struct ThreadData* td) {
    // Extract data structures from ThreadData
    struct Position* pos = &td->pos;
    struct SearchData* sd = &td->sd;
    struct SearchInfo* info = &td->info;

    CleanHistories(sd);

    // Reset plies and search info
    info->starttime = GetTimeMs();
    info->stopped = 0;
    info->nodes = 0;
    info->seldepth = 0;
    // Clear TT
    ClearTT();

    // delete played moves hashes
    pos->played_positions_size = 0;

    // call parse position function
    ParsePosition("position startpos", pos);
}

SMALL void PrintMove(const Move move) {
    const char* from = square_to_coordinates[From(move)];
    const char* to = square_to_coordinates[To(move)];

    if (isPromo(move))
        printf("%s%s%c", (const size_t)from, (const size_t)to, promoted_pieces(getPromotedPiecetype(move)));
    else
        printf("%s%s", (const size_t)from, (const size_t)to);
}

SMALL bool next_token(const char* str, int* index, char* token)
{
    if (str[*index] == '\0')
    {
        return false;
    }

    int token_len = 0;
    while (true) {
        char current = str[*index];

        if (current != '\0') {
            (*index)++;
        }

        if (current == '\0' || current == ' ')
        {
            token[token_len] = '\0';
            return true;
        }

        token[token_len] = current;
        token_len++;
    }
}

#if NOSTDLIB
SMALL void _start() {
#else
SMALL int main() {
#endif

    // Tables for move generation and precompute reduction values
    InitAll();
    // connect to the GUI
    UciLoop();

#if NOSTDLIB
    exit(0);
#else
    return 0;
#endif
}

void HashKey(ZobristKey* originalKey, ZobristKey key) {
    *originalKey ^= key;
}

// Remove a piece from a square, the UPDATE params determines whether we want to update the NNUE weights or not
void ClearPiece(const bool UPDATE, const int piece, const int from, struct Position* pos) {
    assert(piece != EMPTY);
    // Do this first because if we happened to have moved the king we first need to get1lsb the king bitboard before removing it
    if (UPDATE) {
        bool flip[2] = { get_file(KingSQ(pos, WHITE)) > 3, get_file(KingSQ(pos, BLACK)) > 3 };
        int wkSq = KingSQ(pos, WHITE);
        int bkSq = KingSQ(pos, BLACK);
        Accumulator_AppendSubIndex(Position_AccumulatorTop(pos), piece, from, wkSq, bkSq, flip);
    }
    const int color = Color[piece];
    pop_bit(&pos->bitboards[piece], from);
    pop_bit(&pos->occupancies[color], from);
    pos->pieces[from] = EMPTY;
    HashKey(&pos->posKey, PieceKeys[piece][from]);
    if (GetPieceType(piece) == PAWN)
        HashKey(&pos->pawnKey, PieceKeys[piece][from]);
    else if (Color[piece] == WHITE)
        HashKey(&pos->whiteNonPawnKey, PieceKeys[piece][from]);
    else
        HashKey(&pos->blackNonPawnKey, PieceKeys[piece][from]);
}

// Add a piece to a square, the UPDATE params determines whether we want to update the NNUE weights or not
void AddPiece(const bool UPDATE, const int piece, const int to, struct Position* pos) {
    assert(piece != EMPTY);
    const int color = Color[piece];
    set_bit(&pos->bitboards[piece], to);
    set_bit(&pos->occupancies[color], to);
    pos->pieces[to] = piece;
    HashKey(&pos->posKey, PieceKeys[piece][to]);
    if (GetPieceType(piece) == PAWN)
        HashKey(&pos->pawnKey, PieceKeys[piece][to]);
    else if (Color[piece] == WHITE)
        HashKey(&pos->whiteNonPawnKey, PieceKeys[piece][to]);
    else
        HashKey(&pos->blackNonPawnKey, PieceKeys[piece][to]);
    // Do this last because if we happened to have moved the king we first need to re-add to the piece bitboards least we get1lsb an empty bitboard
    if (UPDATE) {
        bool flip[] = { get_file(KingSQ(pos, WHITE)) > 3, get_file(KingSQ(pos, BLACK)) > 3 };
        int wkSq = KingSQ(pos, WHITE);
        int bkSq = KingSQ(pos, BLACK);
        Accumulator_AppendAddIndex(Position_AccumulatorTop(pos), piece, to, wkSq, bkSq, flip);
    }
}

// Move a piece from the [to] square to the [from] square, the UPDATE params determines whether we want to update the NNUE weights or not
void MovePiece(const bool UPDATE, const int piece, const int from, const int to, struct Position* pos) {
    ClearPiece(UPDATE, piece, from, pos);
    AddPiece(UPDATE, piece, to, pos);
}

void UpdateCastlingPerms(struct Position* pos, int source_square, int target_square) {
    // Xor the old castling key from the zobrist key
    HashKey(&pos->posKey, CastleKeys[Position_getCastlingPerm(pos)]);
    // update castling rights
    pos->state.castlePerm &= castling_rights[source_square];
    pos->state.castlePerm &= castling_rights[target_square];
    // Xor the new one
    HashKey(&pos->posKey, CastleKeys[Position_getCastlingPerm(pos)]);
}

inline void resetEpSquare(struct Position* pos) {
    if (Position_getEpSquare(pos) != no_sq) {
        HashKey(&pos->posKey, enpassant_keys[Position_getEpSquare(pos)]);
        pos->state.enPas = no_sq;
    }
}

void MakeCastle(const bool UPDATE, const Move move, struct Position* pos) {
    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    // Remove the piece fom the square it moved from
    ClearPiece(UPDATE, piece, sourceSquare, pos);
    // Set the piece to the destination square, if it was a promotion we directly set the promoted piece
    AddPiece(UPDATE, piece, targetSquare, pos);

    resetEpSquare(pos);

    // move the rook
    switch (targetSquare) {
        // white castles king side
    case (g1):
        // move H rook
        MovePiece(UPDATE, WR, h1, f1, pos);
        break;

        // white castles queen side
    case (c1):
        // move A rook
        MovePiece(UPDATE, WR, a1, d1, pos);
        break;

        // black castles king side
    case (g8):
        // move H rook
        MovePiece(UPDATE, BR, h8, f8, pos);
        break;

        // black castles queen side
    case (c8):
        // move A rook
        MovePiece(UPDATE, BR, a8, d8, pos);
        break;
    }
    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakeEp(const bool UPDATE, const Move move, struct Position* pos) {
    pos->state.fiftyMove = 0;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    const int SOUTH = pos->side == WHITE ? 8 : -8;

    const int pieceCap = GetPiece(PAWN, pos->side ^ 1);
    pos->history[pos->historyStackHead].capture = pieceCap;
    const int capturedPieceLocation = targetSquare + SOUTH;
    ClearPiece(UPDATE, pieceCap, capturedPieceLocation, pos);

    // Remove the piece fom the square it moved from
    ClearPiece(UPDATE, piece, sourceSquare, pos);
    // Set the piece to the destination square
    AddPiece(UPDATE, piece, targetSquare, pos);

    // Reset EP square
    assert(Position_getEpSquare(pos) != no_sq);
    HashKey(&pos->posKey, enpassant_keys[Position_getEpSquare(pos)]);
    pos->state.enPas = no_sq;
}

void MakePromo(const bool UPDATE, const Move move, struct Position* pos) {
    pos->state.fiftyMove = 0;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    const int promotedPiece = GetPiece(getPromotedPiecetype(move), pos->side);

    // Remove the piece fom the square it moved from
    ClearPiece(UPDATE, piece, sourceSquare, pos);
    // Set the piece to the destination square, if it was a promotion we directly set the promoted piece
    AddPiece(UPDATE, promotedPiece, targetSquare, pos);

    resetEpSquare(pos);

    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakePromocapture(const bool UPDATE, const Move move, struct Position* pos) {
    pos->state.fiftyMove = 0;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    const int promotedPiece = GetPiece(getPromotedPiecetype(move), pos->side);

    const int pieceCap = Position_PieceOn(pos, targetSquare);
    assert(pieceCap != EMPTY);
    assert(GetPieceType(pieceCap) != KING);
    ClearPiece(UPDATE, pieceCap, targetSquare, pos);

    pos->history[pos->historyStackHead].capture = pieceCap;

    // Remove the piece fom the square it moved from
    ClearPiece(UPDATE, piece, sourceSquare, pos);
    // Set the piece to the destination square, if it was a promotion we directly set the promoted piece
    AddPiece(UPDATE, promotedPiece, targetSquare, pos);

    resetEpSquare(pos);

    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakeQuiet(const bool UPDATE, const Move move, struct Position* pos) {
    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);

    // if a pawn was moved or a capture was played reset the 50 move rule counter
    if (GetPieceType(piece) == PAWN)
        pos->state.fiftyMove = 0;

    MovePiece(UPDATE, piece, sourceSquare, targetSquare, pos);

    resetEpSquare(pos);

    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakeCapture(const bool UPDATE, const Move move, struct Position* pos) {
    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);

    pos->state.fiftyMove = 0;

    const int pieceCap = Position_PieceOn(pos, targetSquare);
    assert(pieceCap != EMPTY);
    assert(GetPieceType(pieceCap) != KING);
    ClearPiece(UPDATE, pieceCap, targetSquare, pos);
    pos->history[pos->historyStackHead].capture = pieceCap;

    MovePiece(UPDATE, piece, sourceSquare, targetSquare, pos);

    resetEpSquare(pos);

    UpdateCastlingPerms(pos, sourceSquare, targetSquare);
}

void MakeDP(const bool UPDATE, const Move move, struct Position* pos)
{
    pos->state.fiftyMove = 0;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);

    MovePiece(UPDATE, piece, sourceSquare, targetSquare, pos);

    resetEpSquare(pos);

    // Add new ep square
    const int SOUTH = pos->side == WHITE ? 8 : -8;
    int epSquareCandidate = targetSquare + SOUTH;
    if (!(pawn_attacks[pos->side][epSquareCandidate] & Position_GetPieceColorBB(pos, PAWN, pos->side ^ 1)))
        epSquareCandidate = no_sq;
    pos->state.enPas = epSquareCandidate;
    if (Position_getEpSquare(pos) != no_sq)
        HashKey(&pos->posKey, enpassant_keys[Position_getEpSquare(pos)]);
}

bool shouldFlip(int from, int to) {
    const bool prevFlipped = get_file(from) > 3;
    const bool flipped = get_file(to) > 3;

    if (prevFlipped != flipped)
        return true;

    return false;
}

// make move on chess board
void MakeMove(const bool UPDATE, const Move move, struct Position* pos) {
    if (UPDATE) {
        saveBoardState(pos);
        pos->accumStackHead++;
    }
    // Store position key in the array of searched position
    pos->played_positions[pos->played_positions_size++] = pos->posKey;

    // parse move flag
    const bool capture = isCapture(move);
    const bool doublePush = isDP(move);
    const bool enpass = isEnpassant(move);
    const bool castling = isCastle(move);
    const bool promotion = isPromo(move);
    // increment fifty move rule counter
    pos->state.fiftyMove++;
    pos->state.plyFromNull++;
    pos->hisPly++;

    if (castling) {
        MakeCastle(UPDATE, move, pos);
    }
    else if (doublePush) {
        MakeDP(UPDATE, move, pos);
    }
    else if (enpass) {
        MakeEp(UPDATE, move, pos);
    }
    else if (promotion && capture) {
        MakePromocapture(UPDATE, move, pos);
    }
    else if (promotion) {
        MakePromo(UPDATE, move, pos);
    }
    else if (!capture) {
        MakeQuiet(UPDATE, move, pos);
    }
    else {
        MakeCapture(UPDATE, move, pos);
    }

    if (UPDATE)
        pos->historyStackHead++;

    // change side
    Position_ChangeSide(pos);
    // Xor the new side into the key
    HashKey(&pos->posKey, SideKey);
    // Update pinmasks and checkers
    UpdatePinsAndCheckers(pos, pos->side);
    // If we are in check get the squares between the checking piece and the king
    if (Position_getCheckers(pos)) {
        const int kingSquare = KingSQ(pos, pos->side);
        const int pieceLocation = GetLsbIndex(Position_getCheckers(pos));
        pos->state.checkMask = (1ULL << pieceLocation) | RayBetween(pieceLocation, kingSquare);
    }
    else
        pos->state.checkMask = fullCheckmask;

    // Figure out if we need to refresh the accumulator
    if (UPDATE) {
        if (PieceType[Piece(move)] == KING) {
            int kingColor = Color[Piece(move)];
            if (shouldFlip(From(move), To(move))) {
                // tell the right accumulator it'll need a refresh
                pos->accumStack[pos->accumStackHead - 1].perspective[kingColor].needsRefresh = true;
            }
        }
    }

    // Make sure a freshly generated zobrist key matches the one we are incrementally updating
    assert(pos->posKey == GeneratePosKey(pos));
    assert(pos->pawnKey == GeneratePawnKey(pos));
}

void UnmakeMove(const Move move, struct Position* pos) {
    // quiet moves
    pos->hisPly--;
    pos->historyStackHead--;

    restorePreviousBoardState(pos);

    Accumulator_ClearAddIndex(Position_AccumulatorTop(pos));
    Accumulator_ClearSubIndex(Position_AccumulatorTop(pos));
    Position_AccumulatorTop(pos)->perspective[WHITE].needsRefresh = false;
    Position_AccumulatorTop(pos)->perspective[BLACK].needsRefresh = false;

    pos->accumStackHead--;

    // parse move
    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    // parse move flag
    const bool capture = isCapture(move);
    const bool enpass = isEnpassant(move);
    const bool castling = isCastle(move);
    const bool promotion = isPromo(move);

    const int piece = promotion ? GetPiece(getPromotedPiecetype(move), pos->side ^ 1) : Piece(move);

    // clear the piece on the target square
    ClearPiece(false, piece, targetSquare, pos);
    // no matter what add back the piece that was originally moved, ignoring any promotion
    AddPiece(false, Piece(move), sourceSquare, pos);

    // handle captures
    if (capture) {
        const int SOUTH = pos->side == WHITE ? -8 : 8;
        // Retrieve the captured piece we have to restore
        const int piececap = Position_getCapturedPiece(pos);
        const int capturedPieceLocation = enpass ? targetSquare + SOUTH : targetSquare;
        AddPiece(false, piececap, capturedPieceLocation, pos);
    }

    // handle castling moves
    if (castling) {
        // switch target square
        switch (targetSquare) {
            // white castles king side
        case (g1):
            // move H rook
            MovePiece(false, WR, f1, h1, pos);
            break;

            // white castles queen side
        case (c1):
            // move A rook
            MovePiece(false, WR, d1, a1, pos);
            break;

            // black castles king side
        case (g8):
            // move H rook
            MovePiece(false, BR, f8, h8, pos);
            break;

            // black castles queen side
        case (c8):
            // move A rook
            MovePiece(false, BR, d8, a8, pos);
            break;
        }
    }

    // change side
    Position_ChangeSide(pos);

    // restore zobrist key (done at the end to avoid overwriting the value while moving pieces back to their place)
    // we don't need to do the same for the pawn key because the unmake function correctly restores it already
    pos->posKey = pos->played_positions[--pos->played_positions_size];
}

// MakeNullMove handles the playing of a null move (a move that doesn't move any piece)
void MakeNullMove(struct Position* pos) {
    saveBoardState(pos);
    // Store position key in the array of searched position
    pos->played_positions[pos->played_positions_size++] = pos->posKey;
    // Update the zobrist key asap so we can prefetch
    resetEpSquare(pos);
    Position_ChangeSide(pos);
    HashKey(&pos->posKey, SideKey);
    TTPrefetch(Position_getPoskey(pos));

    pos->hisPly++;
    pos->historyStackHead++;
    pos->state.fiftyMove++;
    pos->state.plyFromNull = 0;

    // Update pinmasks and checkers
    UpdatePinsAndCheckers(pos, pos->side);
}

// Take back a null move
void TakeNullMove(struct Position* pos) {
    pos->hisPly--;
    pos->historyStackHead--;

    restorePreviousBoardState(pos);

    Position_ChangeSide(pos);
    pos->posKey = pos->played_positions[--pos->played_positions_size];
}
