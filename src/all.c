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

#define abs(x) ((x) < 0 ? -(x) : (x))

// function that adds a (not yet scored) move to a move list
void AddMoveNonScored(const Move move, struct MoveList* list) {
    list->moves[list->count].move = move;
    list->count++;
}

// function that adds an (already-scored) move to a move list
void AddMoveScored(const Move move, const int score, struct MoveList* list) {
    list->moves[list->count].move = move;
    list->moves[list->count].score = score;
    list->count++;
}

static Bitboard NORTH(const Bitboard in, const int color) {
    return color == WHITE ? in >> 8 : in << 8;
}

void PseudoLegalPawnMoves(struct Position* pos, int color, struct MoveList* list, const enum MovegenType type) {
    const Bitboard enemy = Position_Occupancy(pos, color ^ 1);
    const Bitboard ourPawns = Position_GetPieceColorBB(pos, PAWN, color);
    const Bitboard rank4BB = color == WHITE ? 0x000000FF00000000ULL : 0x00000000FF000000ULL;
    const Bitboard freeSquares = ~Position_Occupancy(pos, BOTH);
    const int pawnType = GetPiece(PAWN, pos->side);
    const int north = color == WHITE ? -8 : 8;
    const bool genNoisy = type & MOVEGEN_NOISY;
    const bool genQuiet = type & MOVEGEN_QUIET;

    // Quiet moves (ie push/double-push)
    if (genQuiet) {
        Bitboard push = NORTH(ourPawns, color) & freeSquares & ~0xFF000000000000FFULL;
        Bitboard doublePushBb = NORTH(push, color) & freeSquares & rank4BB;
        push &= Position_getCheckmask(pos);
        doublePushBb &= Position_getCheckmask(pos);
        while (push) {
            const int to = popLsb(&push);
            AddMoveNonScored(encode_move(to - north, to, pawnType, Quiet), list);
        }
        while (doublePushBb) {
            const int to = popLsb(&doublePushBb);
            AddMoveNonScored(encode_move(to - north * 2, to, pawnType, doublePush), list);
        }
    }

    if (genNoisy) {
        // Push promotions
        Bitboard pushPromo = NORTH(ourPawns, color) & freeSquares & 0xFF000000000000FFULL & Position_getCheckmask(pos);
        while (pushPromo) {
            const int to = popLsb(&pushPromo);
            AddMoveNonScored(encode_move(to - north, to, pawnType, queenPromo | Quiet), list);
            AddMoveNonScored(encode_move(to - north, to, pawnType, rookPromo | Quiet), list);
            AddMoveNonScored(encode_move(to - north, to, pawnType, bishopPromo | Quiet), list);
            AddMoveNonScored(encode_move(to - north, to, pawnType, knightPromo | Quiet), list);
        }

        // Captures and capture-promotions
        Bitboard captBB1 = (NORTH(ourPawns, color) >> 1) & ~0x8080808080808080ULL & enemy & Position_getCheckmask(pos);
        Bitboard captBB2 = (NORTH(ourPawns, color) << 1) & ~0x101010101010101ULL & enemy & Position_getCheckmask(pos);
        while (captBB1) {
            const int to = popLsb(&captBB1);
            const int from = to - north + 1;
            if (get_rank[to] != (color == WHITE ? 7 : 0))
                AddMoveNonScored(encode_move(from, to, pawnType, Capture), list);
            else {
                AddMoveNonScored(encode_move(from, to, pawnType, queenPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, rookPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, bishopPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, knightPromo | Capture), list);
            }
        }

        while (captBB2) {
            const int to = popLsb(&captBB2);
            const int from = to - north - 1;
            if (get_rank[to] != (color == WHITE ? 7 : 0))
                AddMoveNonScored(encode_move(from, to, pawnType, Capture), list);
            else {
                AddMoveNonScored(encode_move(from, to, pawnType, queenPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, rookPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, bishopPromo | Capture), list);
                AddMoveNonScored(encode_move(from, to, pawnType, knightPromo | Capture), list);
            }
        }

        const int epSq = Position_getEpSquare(pos);
        if (epSq == no_sq)
            return;

        // En passant
        Bitboard epPieces = pawn_attacks[color ^ 1][epSq] & ourPawns;
        while (epPieces) {
            int from = popLsb(&epPieces);
            AddMoveNonScored(encode_move(from, epSq, pawnType, enPassant), list);
        }
    }
}

void PseudoLegalKnightMoves(struct Position* pos, int color, struct MoveList* list, const enum MovegenType type) {
    Bitboard knights = Position_GetPieceColorBB(pos, KNIGHT, color);
    const int knightType = GetPiece(KNIGHT, color);
    const bool genNoisy = type & MOVEGEN_NOISY;
    const bool genQuiet = type & MOVEGEN_QUIET;
    Bitboard moveMask = 0ULL; // We restrict the number of squares the knight can travel to

    // The type requested includes noisy moves
    if (genNoisy)
        moveMask |= Position_Occupancy(pos, color ^ 1);

    // The type requested includes quiet moves
    if (genQuiet)
        moveMask |= ~Position_Occupancy(pos, BOTH);

    while (knights) {
        const int from = popLsb(&knights);
        Bitboard possible_moves = knight_attacks[from] & moveMask & Position_getCheckmask(pos);
        while (possible_moves) {
            const int to = popLsb(&possible_moves);
            const enum Movetype movetype = Position_PieceOn(pos, to) != EMPTY ? Capture : Quiet;
            AddMoveNonScored(encode_move(from, to, knightType, movetype), list);
        }
    }
}

void PseudoLegalSlidersMoves(struct Position* pos, int color, struct MoveList* list, const enum MovegenType type) {
    const bool genNoisy = type & MOVEGEN_NOISY;
    const bool genQuiet = type & MOVEGEN_QUIET;
    Bitboard boardOccupancy = Position_Occupancy(pos, BOTH);
    Bitboard moveMask = 0ULL; // We restrict the number of squares the bishop can travel to

    // The type requested includes noisy moves
    if (genNoisy)
        moveMask |= Position_Occupancy(pos, color ^ 1);

    // The type requested includes quiet moves
    if (genQuiet)
        moveMask |= ~Position_Occupancy(pos, BOTH);

    for (int piecetype = BISHOP; piecetype <= QUEEN; piecetype++) {
        Bitboard pieces = Position_GetPieceColorBB(pos, piecetype, color);
        const int coloredPieceValue = GetPiece(piecetype, color);
        while (pieces) {
            const int from = popLsb(&pieces);
            Bitboard possible_moves =
                pieceAttacks(piecetype, from, boardOccupancy) & moveMask & Position_getCheckmask(pos);
            while (possible_moves) {
                const int to = popLsb(&possible_moves);
                const enum Movetype movetype = Position_PieceOn(pos, to) != EMPTY ? Capture : Quiet;
                AddMoveNonScored(encode_move(from, to, coloredPieceValue, movetype), list);
            }
        }
    }
}

void PseudoLegalKingMoves(struct Position* pos, int color, struct MoveList* list, const enum MovegenType type) {
    const int kingType = GetPiece(KING, color);
    const int from = KingSQ(pos, color);
    const bool genNoisy = type & MOVEGEN_NOISY;
    const bool genQuiet = type & MOVEGEN_QUIET;
    Bitboard moveMask = 0ULL; // We restrict the number of squares the king can travel to

    // The type requested includes noisy moves
    if (genNoisy)
        moveMask |= Position_Occupancy(pos, color ^ 1);

    // The type requested includes quiet moves
    if (genQuiet)
        moveMask |= ~Position_Occupancy(pos, BOTH);

    Bitboard possible_moves = king_attacks[from] & moveMask;
    while (possible_moves) {
        const int to = popLsb(&possible_moves);
        enum Movetype movetype = Position_PieceOn(pos, to) != EMPTY ? Capture : Quiet;
        AddMoveNonScored(encode_move(from, to, kingType, movetype), list);
    }

    // Only generate castling moves if we are generating quiets
    // Castling is illegal in check
    if (genQuiet && !Position_getCheckers(pos)) {
        const Bitboard occ = Position_Occupancy(pos, BOTH);
        const int castlePerms = Position_getCastlingPerm(pos);
        if (color == WHITE) {
            // king side castling is available
            if ((castlePerms & WKCA) && !(occ & 0x6000000000000000ULL))
                AddMoveNonScored(encode_move(e1, g1, WK, KSCastle), list);

            // queen side castling is available
            if ((castlePerms & WQCA) && !(occ & 0x0E00000000000000ULL))
                AddMoveNonScored(encode_move(e1, c1, WK, QSCastle), list);
        }
        else {
            // king side castling is available
            if ((castlePerms & BKCA) && !(occ & 0x0000000000000060ULL))
                AddMoveNonScored(encode_move(e8, g8, BK, KSCastle), list);

            // queen side castling is available
            if ((castlePerms & BQCA) && !(occ & 0x000000000000000EULL))
                AddMoveNonScored(encode_move(e8, c8, BK, QSCastle), list);
        }
    }
}

// generate moves
void GenerateMoves(struct MoveList* move_list, struct Position* pos, enum MovegenType type) {

    assert(type == MOVEGEN_ALL || type == MOVEGEN_NOISY || type == MOVEGEN_QUIET);

    const int checks = CountBits(Position_getCheckers(pos));
    if (checks < 2) {
        PseudoLegalPawnMoves(pos, pos->side, move_list, type);
        PseudoLegalKnightMoves(pos, pos->side, move_list, type);
        PseudoLegalSlidersMoves(pos, pos->side, move_list, type);
    }
    PseudoLegalKingMoves(pos, pos->side, move_list, type);
}

// Pseudo-legality test inspired by Koivisto
bool IsPseudoLegal(const struct Position* pos, const Move move) {

    if (move == NOMOVE)
        return false;

    const int from = From(move);
    const int to = To(move);
    const int movedPiece = Piece(move);
    const int pieceType = GetPieceType(movedPiece);

    if (from == to)
        return false;

    if (movedPiece == EMPTY)
        return false;

    if (Position_PieceOn(pos, from) != movedPiece)
        return false;

    if (Color[movedPiece] != pos->side)
        return false;

    if ((1ULL << to) & Position_Occupancy(pos, pos->side))
        return false;

    if ((!isCapture(move) || isEnpassant(move)) && Position_PieceOn(pos, to) != EMPTY)
        return false;

    if (isCapture(move) && !isEnpassant(move) && Position_PieceOn(pos, to) == EMPTY)
        return false;

    if ((isDP(move)
        || isPromo(move)
        || isEnpassant(move)) && pieceType != PAWN)
        return false;

    if (isCastle(move) && pieceType != KING)
        return false;

    if ((CountBits(Position_getCheckers(pos)) >= 2) && pieceType != KING)
        return false;

    const int NORTH = pos->side == WHITE ? -8 : 8;

    switch (pieceType) {
    case PAWN:
        if (isDP(move)) {
            if (from + NORTH + NORTH != to)
                return false;

            if (Position_PieceOn(pos, from + NORTH) != EMPTY)
                return false;

            if ((pos->side == WHITE && get_rank[from] != 1)
                || (pos->side == BLACK && get_rank[from] != 6))
                return false;
        }
        else if (!isCapture(move)) {
            if (from + NORTH != to)
                return false;
        }
        if (isEnpassant(move)) {
            if (to != Position_getEpSquare(pos))
                return false;

            if (!((1ULL << (to - NORTH)) & Position_GetPieceColorBB(pos, PAWN, pos->side ^ 1)))
                return false;
        }
        if (isCapture(move) && !(pawn_attacks[pos->side][from] & (1ULL << to)))
            return false;

        if (isPromo(move)) {
            if ((pos->side == WHITE && get_rank[from] != 6)
                || (pos->side == BLACK && get_rank[from] != 1))
                return false;

            if ((pos->side == WHITE && get_rank[to] != 7)
                || (pos->side == BLACK && get_rank[to] != 0))
                return false;
        }
        else {
            if ((pos->side == WHITE && get_rank[from] >= 6)
                || (pos->side == BLACK && get_rank[from] <= 1))
                return false;
        }
        break;

    case KNIGHT:
        if (!(knight_attacks[from] & (1ULL << to)))
            return false;

        break;

    case BISHOP:
        if (get_diagonal[from] != get_diagonal[to]
            && get_antidiagonal(from) != get_antidiagonal(to))
            return false;

        if (RayBetween(from, to) & Position_Occupancy(pos, BOTH))
            return false;

        break;

    case ROOK:
        if (get_file(from) != get_file(to)
            && get_rank[from] != get_rank[to])
            return false;

        if (RayBetween(from, to) & Position_Occupancy(pos, BOTH))
            return false;

        break;

    case QUEEN:
        if (get_file(from) != get_file(to)
            && get_rank[from] != get_rank[to]
            && get_diagonal[from] != get_diagonal[to]
            && get_antidiagonal(from) != get_antidiagonal(to))
            return false;

        if (RayBetween(from, to) & Position_Occupancy(pos, BOTH))
            return false;

        break;

    case KING:
        if (isCastle(move)) {
            if (Position_getCheckers(pos))
                return false;

            if (abs(to - from) != 2)
                return false;

            bool isKSCastle = GetMovetype(move) == KSCastle;

            Bitboard castleBlocked = Position_Occupancy(pos, BOTH) & (pos->side == WHITE ? isKSCastle ? 0x6000000000000000ULL
                : 0x0E00000000000000ULL
                : isKSCastle ? 0x0000000000000060ULL
                : 0x000000000000000EULL);
            int castleType = pos->side == WHITE ? isKSCastle ? WKCA
                : WQCA
                : isKSCastle ? BKCA
                : BQCA;

            if (!castleBlocked && (Position_getCastlingPerm(pos) & castleType))
                return true;

            return false;
        }
        if (!(king_attacks[from] & (1ULL << to)))
            return false;

        break;
    }
    return true;
}

bool IsLegal(struct Position* pos, const Move move) {

    const int color = pos->side;
    const int ksq = KingSQ(pos, color);
    const int from = From(move);
    const int to = To(move);
    const int movedPiece = Piece(move);
    const int pieceType = GetPieceType(movedPiece);

    if (isEnpassant(move)) {
        int offset = color == WHITE ? 8 : -8;
        int ourPawn = GetPiece(PAWN, color);
        int theirPawn = GetPiece(PAWN, color ^ 1);
        ClearPiece(false, ourPawn, from, pos);
        ClearPiece(false, theirPawn, to + offset, pos);
        AddPiece(false, ourPawn, to, pos);
        bool isLegal = !IsSquareAttacked(pos, ksq, color ^ 1);
        AddPiece(false, ourPawn, from, pos);
        AddPiece(false, theirPawn, to + offset, pos);
        ClearPiece(false, ourPawn, to, pos);
        return isLegal;
    }
    else if (isCastle(move)) {
        bool isKSCastle = GetMovetype(move) == KSCastle;
        if (isKSCastle) {
            return    !IsSquareAttacked(pos, color == WHITE ? f1 : f8, color ^ 1)
                && !IsSquareAttacked(pos, color == WHITE ? g1 : g8, color ^ 1);
        }
        else {
            return    !IsSquareAttacked(pos, color == WHITE ? d1 : d8, color ^ 1)
                && !IsSquareAttacked(pos, color == WHITE ? c1 : c8, color ^ 1);
        }
    }

    if (pieceType == KING) {
        int king = GetPiece(KING, color);
        ClearPiece(false, king, ksq, pos);
        bool isLegal = !IsSquareAttacked(pos, to, color ^ 1);
        AddPiece(false, king, ksq, pos);
        return isLegal;
    }
    else if (Position_getPinnedMask(pos) & (1ULL << from)) {
        return !Position_getCheckers(pos) && (((1ULL << to) & RayBetween(ksq, from)) || ((1ULL << from) & RayBetween(ksq, to)));
    }
    else if (Position_getCheckers(pos)) {
        return (1ULL << to) & Position_getCheckmask(pos);
    }
    else
        return true;
}

// is the square given in input attacked by the current given side
bool IsSquareAttacked(const struct Position* pos, const int square, const int side) {
    // Take the occupancies of both positions, encoding where all the pieces on the board reside
    const Bitboard occ = Position_Occupancy(pos, BOTH);
    // is the square attacked by pawns
    if (pawn_attacks[side ^ 1][square] & Position_GetPieceColorBB(pos, PAWN, side))
        return true;
    // is the square attacked by knights
    if (knight_attacks[square] & Position_GetPieceColorBB(pos, KNIGHT, side))
        return true;
    // is the square attacked by kings
    if (king_attacks[square] & Position_GetPieceColorBB(pos, KING, side))
        return true;
    // is the square attacked by bishops
    if (GetBishopAttacks(square, occ) & (Position_GetPieceColorBB(pos, BISHOP, side) | Position_GetPieceColorBB(pos, QUEEN, side)))
        return true;
    // is the square attacked by rooks
    if (GetRookAttacks(square, occ) & (Position_GetPieceColorBB(pos, ROOK, side) | Position_GetPieceColorBB(pos, QUEEN, side)))
        return true;
    // by default return false
    return false;
}

// ScoreMoves takes a list of move as an argument and assigns a score to each move
void ScoreMoves(struct Movepicker* mp) {
    struct MoveList* moveList = &mp->moveList;
    struct Position* pos = mp->pos;
    struct SearchData* sd = mp->sd;
    struct SearchStack* ss = mp->ss;
    // Loop through all the move in the movelist
    for (int i = mp->idx; i < moveList->count; i++) {
        const Move move = moveList->moves[i].move;
        if (isTactical(move)) {
            // Score by most valuable victim and capthist
            int capturedPiece = isEnpassant(move) ? PAWN : GetPieceType(Position_PieceOn(pos, To(move)));
            // If we captured an empty piece this means the move is a non capturing promotion, we can pretend we captured a pawn to use a slot of the table that would've otherwise went unused (you can't capture pawns on the 1st/8th rank)
            if (capturedPiece == EMPTY) capturedPiece = PAWN;
            moveList->moves[i].score = SEEValue[capturedPiece] * 16 + GetCapthistScore(pos, sd, move);
        }
        else {
            moveList->moves[i].score = GetHistoryScore(pos, sd, move, ss);
        }
    }
}

void partialInsertionSort(struct MoveList* moveList, const int moveNum) {
    int bestScore = moveList->moves[moveNum].score;
    int bestNum = moveNum;
    // starting at the number of the current move and stopping at the end of the list
    for (int index = moveNum + 1; index < moveList->count; ++index) {
        // if we find a move with a better score than our bestmove we use that as the new best move
        if (moveList->moves[index].score > bestScore) {
            bestScore = moveList->moves[index].score;
            bestNum = index;
        }
    }
    // swap the move with the best score with the move in place moveNum
    const struct ScoredMove temp = moveList->moves[moveNum];
    moveList->moves[moveNum] = moveList->moves[bestNum];
    moveList->moves[bestNum] = temp;
}

void InitMP(struct Movepicker* mp, struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const Move ttMove, const enum MovepickerType movepickerType, const bool rootNode) {
    mp->moveList.count = 0;
    mp->badCaptureList.count = 0;

    const Move killer = ss->searchKiller;
    mp->movepickerType = movepickerType;
    mp->pos = pos;
    mp->sd = sd;
    mp->ss = ss;
    mp->ttMove = ttMove;
    mp->idx = 0;
    mp->stage = mp->ttMove ? PICK_TT : GEN_NOISY;
    mp->killer = killer != ttMove ? killer : NOMOVE;
}

Move NextMove(struct Movepicker* mp, const bool skip) {
top:
    if (skip) {
        // In search, the skip variable is used to dictate whether we skip quiet moves
        if (mp->movepickerType == SEARCH
            && mp->stage > PICK_GOOD_NOISY
            && mp->stage < GEN_BAD_NOISY) {
            mp->stage = GEN_BAD_NOISY;
        }

        // In qsearch, the skip variable is used to dictate whether we skip quiet moves and bad captures
        if (mp->movepickerType == QSEARCH
            && mp->stage > PICK_GOOD_NOISY) {
            return NOMOVE;
        }
    }
    switch (mp->stage) {
    case PICK_TT:
        ++mp->stage;
        // If we are in qsearch and not in check, skip quiet TT moves
        if (mp->movepickerType == QSEARCH && skip && !isTactical(mp->ttMove))
            goto top;

        // If the TT move if not pseudo legal we skip it too
        if (!IsPseudoLegal(mp->pos, mp->ttMove))
            goto top;

        return mp->ttMove;

    case GEN_NOISY:
        GenerateMoves(&mp->moveList, mp->pos, MOVEGEN_NOISY);
        ScoreMoves(mp);
        ++mp->stage;
        goto top;

    case PICK_GOOD_NOISY:
        while (mp->idx < mp->moveList.count) {
            partialInsertionSort(&mp->moveList, mp->idx);
            const Move move = mp->moveList.moves[mp->idx].move;
            const int score = mp->moveList.moves[mp->idx].score;
            const int SEEThreshold = -score / 32 + 236;
            ++mp->idx;
            if (move == mp->ttMove)
                continue;

            if (!SEE(mp->pos, move, SEEThreshold)) {
                AddMoveScored(move, score, &mp->badCaptureList);
                continue;
            }

            assert(isTactical(move));

            return move;
        }
        ++mp->stage;
        goto top;

    case PICK_KILLER:
        ++mp->stage;
        if (IsPseudoLegal(mp->pos, mp->killer))
            return mp->killer;
        goto top;

    case GEN_QUIETS:
        GenerateMoves(&mp->moveList, mp->pos, MOVEGEN_QUIET);
        ScoreMoves(mp);
        ++mp->stage;
        goto top;

    case PICK_QUIETS:
        while (mp->idx < mp->moveList.count) {
            partialInsertionSort(&mp->moveList, mp->idx);
            const Move move = mp->moveList.moves[mp->idx].move;
            ++mp->idx;
            if (move == mp->ttMove
                || move == mp->killer)
                continue;

            assert(!isTactical(move));
            return move;
        }
        ++mp->stage;
        goto top;

    case GEN_BAD_NOISY:
        // Nothing to generate lol, just reset mp->idx
        mp->idx = 0;
        ++mp->stage;
        goto top;

    case PICK_BAD_NOISY:
        while (mp->idx < mp->badCaptureList.count) {
            const Move move = mp->badCaptureList.moves[mp->idx].move;
            ++mp->idx;
            if (move == mp->ttMove)
                continue;

            assert(isTactical(move));
            return move;
        }
        return NOMOVE;

    default:
        // we should never end up here because a movepicker stage should be always be valid and accounted for
        assert(false);
        __builtin_unreachable();
    }
}

struct Network net;

void NNUE_update(struct Accumulator* acc, struct Position* pos) {
    for (int pov = WHITE; pov <= BLACK; pov++) {
        struct Pov_Accumulator* povAccumulator = &(acc)->perspective[pov];
        struct Pov_Accumulator* previousPovAccumulator = &(acc - 1)->perspective[pov];

        // return early if we already updated this accumulator (aka it's "clean")
        if (Pov_Accumulator_isClean(povAccumulator))
            continue;
        // if the top accumulator needs a refresh, ignore all the other dirty accumulators and just refresh it
        else if (povAccumulator->needsRefresh) {
            Pov_Accumulator_refresh(povAccumulator, pos);
            continue;
        }
        // if the previous accumulator is clean, just UE on top of it and avoid the need to scan backwards
        else if (Pov_Accumulator_isClean(previousPovAccumulator)) {
            Pov_Accumulator_applyUpdate(povAccumulator, previousPovAccumulator);
            continue;
        }

        // if we can't update we need to start scanning backwards
        // if in our scan we'll find an accumulator that needs a refresh we'll just refresh the top one, otherwise we'll start an UE chain
        for (int UEableAccs = 1; UEableAccs < MAXPLY; UEableAccs++) {
            struct Pov_Accumulator* currentAcc = &(acc - UEableAccs)->perspective[pov];
            // check if the current acc should be refreshed
            if (currentAcc->needsRefresh) {
                Pov_Accumulator_refresh(povAccumulator, pos);
                break;
            }
            // If not check if it's a valid starting point for an UE chain
            else if (Pov_Accumulator_isClean(&(acc - UEableAccs - 1)->perspective[pov])) {
                for (int j = (pos->accumStackHead - 1 - UEableAccs); j <= pos->accumStackHead - 1; j++) {
                    Pov_Accumulator_applyUpdate(&pos->accumStack[j].perspective[pov], &pos->accumStack[j - 1].perspective[pov]);
                }
                break;
            }
        }
    }
}

void Pov_Accumulator_refresh(struct Pov_Accumulator* accumulator, struct Position* pos) {
    Pov_Accumulator_accumulate(accumulator, pos);
    // Reset the add and sub vectors for this accumulator, this will make it "clean" for future updates
    accumulator->NNUEAdd_size = 0;
    accumulator->NNUESub_size = 0;
    // mark any accumulator as refreshed
    accumulator->needsRefresh = false;
}

void Pov_Accumulator_addSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub) {
    const int16_t* const Add = &net.FTWeights[add * L1_SIZE];
    const int16_t* const Sub = &net.FTWeights[sub * L1_SIZE];
    for (int i = 0; i < L1_SIZE; i++) {
        accumulator->values[i] = prev_acc->values[i] - Sub[i] + Add[i];
    }
}

void Pov_Accumulator_addSubSub(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* prev_acc, size_t add, size_t sub1, size_t sub2) {
    const int16_t* const Add = &net.FTWeights[add * L1_SIZE];
    const int16_t* const Sub1 = &net.FTWeights[sub1 * L1_SIZE];
    const int16_t* const Sub2 = &net.FTWeights[sub2 * L1_SIZE];
    for (int i = 0; i < L1_SIZE; i++) {
        accumulator->values[i] = prev_acc->values[i] - Sub1[i] - Sub2[i] + Add[i];
    }
}

int32_t NNUE_output(struct Accumulator* const board_accumulator, const int stm, const int outputBucket) {
    // this function takes the net output for the current accumulators and returns the eval of the position
    // according to the net

    const int16_t* us = board_accumulator->perspective[stm].values;
    const int16_t* them = board_accumulator->perspective[stm ^ 1].values;
    const int32_t bucketOffset = 2 * L1_SIZE * outputBucket;
    return NNUE_ActivateFTAndAffineL1(us, them, &net.L1Weights[bucketOffset], net.L1Biases[outputBucket]);
}

void NNUE_accumulate(struct Accumulator* board_accumulator, struct Position* pos) {
    for (int i = 0; i < 2; i++) {
        Pov_Accumulator_accumulate(&board_accumulator->perspective[i], pos);
    }
}

void Pov_Accumulator_applyUpdate(struct Pov_Accumulator* accumulator, struct Pov_Accumulator* previousPovAccumulator) {

    assert(Pov_Accumulator_isClean(previousPovAccumulator));

    // return early if we already updated this accumulator (aka it's "clean")
    if (Pov_Accumulator_isClean(accumulator))
        return;

    // figure out what update we need to apply and do that
    const int adds = accumulator->NNUEAdd_size;
    const int subs = accumulator->NNUESub_size;

    // Quiets
    if (adds == 1 && subs == 1) {
        Pov_Accumulator_addSub(accumulator, previousPovAccumulator, accumulator->NNUEAdd[0], accumulator->NNUESub[0]);
    }
    // Captures
    else if (adds == 1 && subs == 2) {
        Pov_Accumulator_addSubSub(accumulator, previousPovAccumulator, accumulator->NNUEAdd[0], accumulator->NNUESub[0], accumulator->NNUESub[1]);
    }
    // Castling
    else {
        Pov_Accumulator_addSub(accumulator, previousPovAccumulator, accumulator->NNUEAdd[0], accumulator->NNUESub[0]);
        Pov_Accumulator_addSub(accumulator, accumulator, accumulator->NNUEAdd[1], accumulator->NNUESub[1]);
        // Note that for second addSub, we put acc instead of acc - 1 because we are updating on top of
        // the half-updated accumulator
    }

    // Reset the add and sub vectors for this accumulator, this will make it "clean" for future updates
    accumulator->NNUEAdd_size = 0;
    accumulator->NNUESub_size = 0;
}

void Pov_Accumulator_accumulate(struct Pov_Accumulator* accumulator, struct Position* pos) {
    for (int i = 0; i < L1_SIZE; i++) {
        accumulator->values[i] = net.FTBiases[i];
    }

    const int kingSq = KingSQ(pos, accumulator->pov);
    const bool flip = get_file(KingSQ(pos, accumulator->pov)) > 3;

    for (int square = 0; square < 64; square++) {
        const bool input = pos->pieces[square] != EMPTY;
        if (!input) continue;
        const int Idx = Pov_Accumulator_GetIndex(accumulator, Position_PieceOn(pos, square), square, kingSq, flip);
        const int16_t* const Add = &net.FTWeights[Idx * L1_SIZE];
        for (int j = 0; j < L1_SIZE; j++) {
            accumulator->values[j] += Add[j];
        }
    }
}

int Pov_Accumulator_GetIndex(const struct Pov_Accumulator* accumulator, const int piece, const int square, const int kingSq, bool flip) {
    const size_t COLOR_STRIDE = 64 * 6;
    const size_t PIECE_STRIDE = 64;
    const int piecetype = GetPieceType(piece);
    const int pieceColor = Color[piece];
    int pieceColorPov = accumulator->pov == WHITE ? pieceColor : (pieceColor ^ 1);
    // Get the final indexes of the updates, accounting for hm
    int squarePov = accumulator->pov == WHITE ? (square ^ 0b111000) : square;
    if (flip) squarePov ^= 0b000111;
    size_t Idx = pieceColorPov * COLOR_STRIDE + piecetype * PIECE_STRIDE + squarePov;
    return Idx;
}

int32_t NNUE_ActivateFTAndAffineL1(const int16_t* us, const int16_t* them, const int16_t* weights, const int16_t bias) {
#if defined(USE_SIMD) && !NOSTDLIB
    vepi32 sum = vec_zero_epi32();
    const vepi16 Zero = vec_zero_epi16();
    const vepi16 One = vec_set1_epi16(FT_QUANT);
    int weightOffset = 0;
    const int16_t* both[] = { us, them };
    for (int both_index = 0; both_index < 2; both_index++) {
        for (int i = 0; i < L1_SIZE; i += CHUNK_SIZE) {
            const vepi16 input = vec_loadu_epi((const vepi16*)&both[both_index][i]);
            const vepi16 weight = vec_loadu_epi((const vepi16*)&weights[i + weightOffset]);
            const vepi16 clipped = vec_min_epi16(vec_max_epi16(input, Zero), One);

            // In squared clipped relu, we want to do (clipped * clipped) * weight.
            // However, as clipped * clipped does not fit in an int16 while clipped * weight does,
            // we instead do mullo(clipped, weight) and then madd by clipped.
            const vepi32 product = vec_madd_epi16(vec_mullo_epi16(clipped, weight), clipped);
            sum = vec_add_epi32(sum, product);
        }
        weightOffset += L1_SIZE;
    }

    return (vec_reduce_add_epi32(sum) / FT_QUANT + bias) * NET_SCALE / (FT_QUANT * L1_QUANT);

#else
    int sum = 0;
    int weightOffset = 0;

    const int16_t* both[] = { us, them };

    for (int both_index = 0; both_index < 2; both_index++) {
        for (int i = 0; i < L1_SIZE; ++i) {
            const int16_t input = both[both_index][i];
            const int16_t weight = weights[i + weightOffset];
            const int16_t clipped = input < 0 ? 0 : input > FT_QUANT ? FT_QUANT : input; // clamp(input, int16_t(0), int16_t(FT_QUANT));
            sum += (int16_t)(clipped * weight) * clipped;
        }

        weightOffset += L1_SIZE;
    }

    return (sum / FT_QUANT + bias) * NET_SCALE / (FT_QUANT * L1_QUANT);
#endif
}


SMALL void NNUE_init() {
    // open the nn file

#if KAGGLE
    const char* nnpath = "//kaggle_simulations//agent//nn.net";
#else
    const char* nnpath = "nn.net";
#endif
    int nn = open(nnpath, 0, 0644); // Default permissions: -rw-r--r--
    if (nn < 0)
    {
        puts("Unable to open NN file");
        exit(1);
    }

    const size_t len = NUM_INPUTS * L1_SIZE + L1_SIZE + L1_SIZE * 2 * OUTPUT_BUCKETS + OUTPUT_BUCKETS;

    int8_t* ptr = mmap(NULL, len * sizeof(int16_t), 1, 2, nn, 0);

    const int ft_size = NUM_INPUTS * L1_SIZE;
    const int blockSize = L1_SIZE * 4;
    const int blockCount = ft_size / blockSize;

    int currentIndex = 0;
    for (int blockIndex = 0; blockIndex < blockCount; blockIndex++) {
        int8_t divisor = *ptr;
        ptr++;

        if (divisor == 0) {
            for (int i = 0; i < blockSize; i++) {
                net.FTWeights[currentIndex++] = 0;
            }
        }
        else {
            for (int i = 0; i < blockSize; i++) {
                net.FTWeights[currentIndex++] = (int16_t)((int16_t)(*ptr) * divisor);
                ptr++;
            }
        }
    }

    for (int i = 0; i < L1_SIZE; i++) {
        net.FTBiases[i] = *(int16_t*)ptr;
        ptr += 2;
    }
    for (int i = 0; i < L1_SIZE * 2 * OUTPUT_BUCKETS; i++) {
        net.L1Weights[i] = *ptr;
        ptr++;
    }
    for (int i = 0; i < OUTPUT_BUCKETS; i++) {
        net.L1Biases[i] = *(int16_t*)ptr;
        ptr += 2;
    }

    int16_t transposedL1Weights[L1_SIZE * 2 * OUTPUT_BUCKETS];
    for (int weight = 0; weight < 2 * L1_SIZE; ++weight)
    {
        for (int bucket = 0; bucket < OUTPUT_BUCKETS; ++bucket)
        {
            const int srcIdx = weight * OUTPUT_BUCKETS + bucket;
            const int dstIdx = bucket * 2 * L1_SIZE + weight;
            transposedL1Weights[dstIdx] = net.L1Weights[srcIdx];
        }
    }
    memcpy(net.L1Weights, transposedL1Weights, L1_SIZE * sizeof(int16_t) * 2 * OUTPUT_BUCKETS);
}

SMALL int char_pieces(const char ch) {
    switch (ch)
    {
    case 'P':
        return WP;
    case 'N':
        return WN;
    case 'B':
        return WB;
    case 'R':
        return WR;
    case 'Q':
        return WQ;
    case 'K':
        return WK;
    case 'p':
        return BP;
    case 'n':
        return BN;
    case 'b':
        return BB;
    case 'r':
        return BR;
    case 'q':
        return BQ;
    case 'k':
        return BK;
    default:
        __builtin_unreachable();
    }
}

// Map promoted piece to the corresponding ASCII character
SMALL char promoted_pieces(const int piece) {
    switch (piece)
    {
    case WQ:
        return 'q';
    case WR:
        return 'r';
    case WB:
        return 'b';
    case WN:
        return 'n';
    case BQ:
        return 'q';
    case BR:
        return 'r';
    case BB:
        return 'b';
    case BN:
        return 'n';
    default:
        __builtin_unreachable();
    }
}

const int castling_rights[64] = {
     7, 15, 15, 15,  3, 15, 15, 11,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15,
    13, 15, 15, 15, 12, 15, 15, 14,
};

const char* const square_to_coordinates[] = {
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
};

// Reset the position to a clean state
SMALL void ResetBoard(struct Position* const pos) {
    // reset board position (pos->pos->bitboards)
    memset(pos->bitboards, 0, sizeof(pos->bitboards));

    // reset pos->occupancies (pos->pos->bitboards)
    memset(pos->occupancies, 0, sizeof(pos->occupancies));

    memset(&pos->pieces, 0, sizeof(Bitboard) * 12);
    for (int index = 0; index < 64; ++index) {
        pos->pieces[index] = EMPTY;
    }
    pos->state.castlePerm = 0;
    pos->state.plyFromNull = 0;
}

SMALL void ResetInfo(struct SearchInfo* const info) {
    info->depth = 0;
    info->nodes = 0;
    info->starttime = 0;
    info->stoptimeOpt = 0;
    info->stoptimeMax = 0;
    info->stopped = false;
    info->timeset = false;
}

// Generates zobrist key from scratch
SMALL ZobristKey GeneratePosKey(const struct Position* const pos) {
    Bitboard finalkey = 0;
    // for every square
    for (int sq = 0; sq < 64; ++sq) {
        // get piece on that square
        const int piece = Position_PieceOn(pos, sq);
        // if it's not empty add that piece to the zobrist key
        if (piece != EMPTY) {
            assert(piece >= WP && piece <= BK);
            finalkey ^= PieceKeys[piece][sq];
        }
    }
    // include side in the key
    if (pos->side == BLACK) {
        finalkey ^= SideKey;
    }
    // include the ep square in the key
    if (Position_getEpSquare(pos) != no_sq) {
        assert(Position_getEpSquare(pos) >= 0 && Position_getEpSquare(pos) < 64);
        finalkey ^= enpassant_keys[Position_getEpSquare(pos)];
    }
    assert(Position_getCastlingPerm(pos) >= 0 && Position_getCastlingPerm(pos) <= 15);
    // add to the key the status of the castling permissions
    finalkey ^= CastleKeys[Position_getCastlingPerm(pos)];
    return finalkey;
}

// Generates zobrist key (for only the pawns) from scratch
SMALL ZobristKey GeneratePawnKey(const struct Position* pos) {
    Bitboard pawnKey = 0;
    for (int sq = 0; sq < 64; ++sq) {
        // get piece on that square
        const int piece = Position_PieceOn(pos, sq);
        if (piece == WP || piece == BP) {
            pawnKey ^= PieceKeys[piece][sq];
        }
    }
    return pawnKey;
}

// Generates zobrist key (for non-pawns) from scratch
SMALL ZobristKey GenerateNonPawnKey(const struct Position* pos, int side) {
    Bitboard nonPawnKey = 0;
    for (int sq = 0; sq < 64; ++sq) {
        // get piece on that square
        const int piece = Position_PieceOn(pos, sq);
        if (piece != EMPTY && piece != WP && piece != BP && Color[piece] == side) {
            nonPawnKey ^= PieceKeys[piece][sq];
        }
    }
    return nonPawnKey;
}

Bitboard RayBetween(int square1, int square2) {
    return SQUARES_BETWEEN_BB[square1][square2];
}

// Get on what square of the board the king of color c resides
int KingSQ(const struct Position* pos, const int c) {
    return GetLsbIndex(Position_GetPieceColorBB(pos, KING, c));
}

void UpdatePinsAndCheckers(struct Position* pos, const int side) {
    const Bitboard them = Position_Occupancy(pos, side ^ 1);
    const int kingSquare = KingSQ(pos, side);
    const Bitboard pawnCheckers = Position_GetPieceColorBB(pos, PAWN, side ^ 1) & pawn_attacks[side][kingSquare];
    const Bitboard knightCheckers = Position_GetPieceColorBB(pos, KNIGHT, side ^ 1) & knight_attacks[kingSquare];
    const Bitboard bishopsQueens = Position_GetPieceColorBB(pos, BISHOP, side ^ 1) | Position_GetPieceColorBB(pos, QUEEN, side ^ 1);
    const Bitboard rooksQueens = Position_GetPieceColorBB(pos, ROOK, side ^ 1) | Position_GetPieceColorBB(pos, QUEEN, side ^ 1);
    Bitboard sliderAttacks = (bishopsQueens & GetBishopAttacks(kingSquare, them)) | (rooksQueens & GetRookAttacks(kingSquare, them));
    pos->state.pinned = 0ULL;
    pos->state.checkers = pawnCheckers | knightCheckers;

    while (sliderAttacks) {
        const int sq = popLsb(&sliderAttacks);
        const Bitboard blockers = RayBetween(kingSquare, sq) & Position_Occupancy(pos, side);
        const int numBlockers = CountBits(blockers);
        if (!numBlockers)
            pos->state.checkers |= 1ULL << sq;
        else if (numBlockers == 1)
            pos->state.pinned |= blockers;
    }
}

// Returns the bitboard of a piecetype
Bitboard GetPieceBB(const struct Position* pos, const int piecetype) {
    return Position_GetPieceColorBB(pos, piecetype, WHITE) | Position_GetPieceColorBB(pos, piecetype, BLACK);
}

// Return a piece based on the piecetype and the color
int GetPiece(const int piecetype, const int color) {
    return piecetype + 6 * color;
}

// Returns the piecetype of a piece
int GetPieceType(const int piece) {
    if (piece == EMPTY)
        return EMPTY;
    return PieceType[piece];
}

// Returns true if side has at least one piece on the board that isn't a pawn or the king, false otherwise
bool BoardHasNonPawns(const struct Position* pos, const int side) {
    return Position_Occupancy(pos, side) ^ Position_GetPieceColorBB(pos, PAWN, side) ^ Position_GetPieceColorBB(pos, KING, side);
}

// Calculates what the key for position pos will be after move <move>, it's a rough estimate and will fail for "special" moves such as promotions and castling
ZobristKey keyAfter(const struct Position* pos, const Move move) {

    const int sourceSquare = From(move);
    const int targetSquare = To(move);
    const int piece = Piece(move);
    const int  captured = Position_PieceOn(pos, targetSquare);

    ZobristKey newKey = Position_getPoskey(pos) ^ SideKey ^ PieceKeys[piece][sourceSquare] ^ PieceKeys[piece][targetSquare];

    if (captured != EMPTY)
        newKey ^= PieceKeys[captured][targetSquare];

    return newKey;
}

// parse FEN string
SMALL void ParseFen(const char* command, struct Position* pos) {

    ResetBoard(pos);

    int token_index = 0;

    char pos_string[128];
    next_token(command, &token_index, pos_string);

    char turn[2];
    next_token(command, &token_index, turn);

    char castle_perm[5];
    next_token(command, &token_index, castle_perm);

    char ep_square[3];
    next_token(command, &token_index, ep_square);

    char fifty_move[8] = { 0 };
    char HisPly[8] = { 0 };

    // Keep fifty move and Hisply arguments optional
    if (next_token(command, &token_index, fifty_move)) {
        next_token(command, &token_index, HisPly);
    }

    int fen_counter = 0;
    for (int rank = 0; rank < 8; rank++) {
        // loop over board files
        for (int file = 0; file < 8; file++) {
            // init current square
            const int square = rank * 8 + file;
            const char current_char = pos_string[fen_counter];

            // match ascii pieces within FEN string
            if ((current_char >= 'a' && current_char <= 'z') || (current_char >= 'A' && current_char <= 'Z')) {
                // init piece type
                const int piece = char_pieces(current_char);
                if (piece != EMPTY) {
                    // set piece on corresponding bitboard
                    set_bit(&pos->bitboards[piece], square);
                    pos->pieces[square] = piece;
                }
                fen_counter++;
            }

            // match empty square numbers within FEN string
            if (current_char >= '0' && current_char <= '9') {
                // init offset (convert char 0 to int 0)
                const int offset = current_char - '0';

                const int piece = Position_PieceOn(pos, square);
                // on empty current square
                if (piece == EMPTY)
                    // decrement file
                    file--;

                // adjust file counter
                file += offset;

                // increment pointer to FEN string
                fen_counter++;
            }

            // match rank separator
            if (pos_string[fen_counter] == '/')
                // increment pointer to FEN string
                fen_counter++;
        }
    }

    // parse player turn
    pos->side = turn[0] == 'w' ? WHITE : BLACK;

    // Parse castling rights
    for (int i = 0;; i++) {
        const char c = castle_perm[i];
        if (c == '\0') {
            break;
        }

        switch (c) {
        case 'K':
            (pos->state.castlePerm) |= WKCA;
            break;
        case 'Q':
            (pos->state.castlePerm) |= WQCA;
            break;
        case 'k':
            (pos->state.castlePerm) |= BKCA;
            break;
        case 'q':
            (pos->state.castlePerm) |= BQCA;
            break;
        case '-':
            break;
        }
    }

    // parse enpassant square
    if (ep_square[0] != '\0' && ep_square[1] != '\0' && ep_square[0] != '-') {
        // parse enpassant file & rank
        const int file = ep_square[0] - 'a';
        const int rank = 8 - (ep_square[1] - '0');

        // init enpassant square
        pos->state.enPas = rank * 8 + file;
    }
    // no enpassant square
    else
        pos->state.enPas = no_sq;

    // Read fifty moves counter
    if (fifty_move[0] != '\0') {
        pos->state.fiftyMove = atoi(fifty_move);
    }
    else {
        pos->state.fiftyMove = 0;
    }
    // Read Hisply moves counter
    if (HisPly[0] != '\0') {
        pos->hisPly = atoi(HisPly);
    }
    else {
        pos->hisPly = 0;
    }

    for (int piece = WP; piece <= WK; piece++)
        // populate white occupancy bitboard
        pos->occupancies[WHITE] |= pos->bitboards[piece];

    for (int piece = BP; piece <= BK; piece++)
        // populate white occupancy bitboard
        pos->occupancies[BLACK] |= pos->bitboards[piece];

    pos->posKey = GeneratePosKey(pos);
    pos->pawnKey = GeneratePawnKey(pos);
    pos->whiteNonPawnKey = GenerateNonPawnKey(pos, WHITE);
    pos->blackNonPawnKey = GenerateNonPawnKey(pos, BLACK);

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

    // Update nnue accumulator to reflect board state
    NNUE_accumulate(&pos->accumStack[0], pos);
    pos->accumStackHead = 1;
}

void saveBoardState(struct Position* pos) {
    pos->history[pos->historyStackHead] = pos->state;
}

void restorePreviousBoardState(struct Position* pos)
{
    pos->state = pos->history[pos->historyStackHead];
}

SMALL void parse_moves(const char* moves, struct Position* pos) {
#if UCI
    char move_str[6];
    int token_index = 0;

    // loop over moves within a move string
    while (next_token(moves, &token_index, move_str)) {
        // parse next move
        const Move move = ParseMove(move_str, pos);
        // make move on the chess board
        MakeMove(false, move, pos);
    }
#endif
}

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
    printf("bestmove ");
    PrintMove(return_bestmove);
    printf("\n");
    fflush(stdout);

    // Hack for Kaggle for full repetition detection
#ifdef KAGGLE
    MakeMove(true, return_bestmove, &td->pos);
    ZobristKey opponent_hash = td->pos.posKey;
    UnmakeMove(return_bestmove, &td->pos);
    td->pos.played_positions[td->pos.played_positions_size++] = td->pos.posKey;
    td->pos.played_positions[td->pos.played_positions_size++] = opponent_hash;
#endif
#ifndef NOPONDER
    // start pondering
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
#endif
}

// Returns true if the position is a 2-fold repetition, false otherwise
bool IsRepetition(const struct Position* pos) {
    assert(pos->hisPly >= Position_get50MrCounter(pos));
    int counter = 0;
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

SMALL void init_thread_data(struct ThreadData* td)
{
    td->pos.side = -1;
    td->pos.hisPly = 0;
    td->pos.posKey = 0ULL;
    td->pos.pawnKey = 0ULL;
    td->pos.whiteNonPawnKey = 0ULL;
    td->pos.blackNonPawnKey = 0ULL;
    td->pos.historyStackHead = 0ULL;

    memset(&td->pos.bitboards, 0, sizeof(Bitboard) * 12);
    memset(&td->nodeSpentTable, 0, sizeof(td->nodeSpentTable));

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
        if (td->RootDepth > 7
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

        if (td->pondering && StdinHasData()) {
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
        delta *= 1.44;
    }
    return score;
}

static int get_complexity(const int eval, const int rawEval) {
    if (eval == 0 || rawEval == 0)
        return 0;

    return 100 * abs(eval - rawEval) / abs(eval);
}

static bool get_improving(const struct SearchStack* const ss, const bool inCheck) {
    if (inCheck)
        return false;
    else if ((ss - 2)->staticEval != SCORE_NONE)
        return ss->staticEval > (ss - 2)->staticEval;
    else if ((ss - 4)->staticEval != SCORE_NONE)
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

    if (td->pondering && info->nodes % 4096 == 0 && StdinHasData()) {
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
        eval = ss->staticEval = adjustEvalWithCorrHist(pos, sd, rawEval);

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
        eval = ss->staticEval = adjustEvalWithCorrHist(pos, sd, rawEval);
        // Save the eval into the TT
        StoreTTEntry(pos->posKey, NOMOVE, SCORE_NONE, rawEval, HFNONE, 0, pvNode, ttPv);
    }

    const int complexity = get_complexity(eval, rawEval);

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
            && eval - 91 * (depth - improving - canIIR) >= beta)
            return eval - 91 * (depth - improving - canIIR);

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
            const int R = 4 + depth / 3 + min((eval - beta) / 200, 3);
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
        if (depth <= 5 && eval + 256 * depth < alpha)
        {
            const int razorScore = Quiescence(alpha, beta, td, ss);
            if (razorScore <= alpha)
                return razorScore;
        }
    }

    // IIR by Ed Schroder (That i find out about in Berserk source code)
    // http://talkchess.com/forum3/viewtopic.php?f=7&t=74769&sid=64085e3396554f0fba414404445b3120
    // https://github.com/jhonnold/berserk/blob/dd1678c278412898561d40a31a7bd08d49565636/src/search.c#L379
    if (canIIR)
        depth -= 1;

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

            int depthReduction = isQuiet ? +1.00 + log(min(depth, 63)) * log(min(totalMoves, 63)) / 2.00
                : -0.25 + log(min(depth, 63)) * log(min(totalMoves, 63)) / 2.25;

            // lmrDepth is the current depth minus the reduction the move would undergo in lmr, this is helpful because it helps us discriminate the bad moves with more accuracy
            const int lmrDepth = max(0, depth - depthReduction + moveHistory / 8192);

            if (!skipQuiets) {

                const int lmplimit = improving ? 3.0 + 1.0 * depth * depth : 1.5 + 0.5 * depth * depth;

                // Movecount pruning: if we searched enough moves and we are not in check we skip the rest
                if (totalMoves > lmplimit) {
                    skipQuiets = true;
                }

                // Futility pruning: if the static eval is so low that even after adding a bonus we are still under alpha we can stop trying quiet moves
                if (!inCheck
                    && lmrDepth < 11
                    && ss->staticEval + 250 + 150 * lmrDepth <= alpha) {
                    skipQuiets = true;
                }
            }

            // See pruning: prune all the moves that have a SEE score that is lower than our threshold
            if (!SEE(pos, move, see_margin[min(lmrDepth, 63)][isQuiet]))
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

            int depthReduction = isQuiet ? +1.00 + log(min(depth, 63)) * log(min(totalMoves, 63)) / 2.00
                : -0.25 + log(min(depth, 63)) * log(min(totalMoves, 63)) / 2.25;

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

                if (complexity > 50)
                    depthReduction -= 1;

                // Decrease the reduction for moves that have a good history score and increase it for moves with a bad score
                depthReduction -= moveHistory / 8192;
            }
            else {
                // Fuck
                if (cutNode)
                    depthReduction += 2;

                // Decrease the reduction for moves that have a good history score and increase it for moves with a bad score
                depthReduction -= moveHistory / 6144;
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
                const bool doDeeperSearch = score > (bestScore + 53 + 2 * newDepth);
                const bool doShallowerSearch = score < (bestScore + newDepth);
                newDepth += doDeeperSearch - doShallowerSearch;
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

    if (td->pondering && info->nodes % 4096 == 0 && StdinHasData()) {
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
        ss->staticEval = bestScore = adjustEvalWithCorrHist(pos, sd, rawEval);

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
        bestScore = ss->staticEval = adjustEvalWithCorrHist(pos, sd, rawEval);
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
            const int futilityBase = ss->staticEval + 192;
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

#if !NOSTDLIB
inline uint64_t GetTimeMs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
#else

ssize_t _sys(ssize_t call, ssize_t arg1, ssize_t arg2, ssize_t arg3) {
    ssize_t ret;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(call), "D"(arg1), "S"(arg2), "d"(arg3)
        : "rcx", "r11", "memory");
    return ret;
}

SMALL void* memset(void* ptr, int value, size_t n) {
    unsigned char* p = (unsigned char*)ptr;
    while (n-- > 0) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

SMALL void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

SMALL void exit(const int returnCode) {
    _sys(60, returnCode, 0, 0);
}

SMALL int strlen(const char* const restrict string) {
    int length = 0;
    while (string[length]) {
        length++;
    }
    return length;
}

static void write(int fd, void* data, int count) {
    _sys(1, fd, (size_t)data, count);
}

void puts(const char* const restrict string) {
    write(stdout, string, strlen(string));
    write(stdout, "\n", 1);
}

void puts_nonewline(const char* const restrict string) {
    write(stdout, string, strlen(string));
}

void fflush(int fd) {

}

SMALL bool strcmp(const char* restrict lhs,
    const char* restrict rhs) {
    while (*lhs || *rhs) {
        if (*lhs != *rhs) {
            return true;
        }
        lhs++;
        rhs++;
    }
    return false;
}

SMALL int atoi(const char* restrict string) {
    size_t result = 0;
    while (true) {
        if (!*string) {
            return result;
        }
        result *= 10;
        result += *string - '0';
        string++;
    }
}

void _printf(const char* format, const size_t* args) {
    int value;
    char buffer[16], * string;

    while (true) {
        if (!*format) {
            break;
        }
        if (*format != '%') {
            write(stdout, format, 1);
            format++;
            continue;
        }

        format++;
        switch (*format++) {
        case 's':
            puts_nonewline((char*)*args);
            break;
        case 'c':
            write(1, args, 1);
            break;
        case 'd':
        case 'i':
            value = *args;
            if (value < 0) {
                puts_nonewline("-");
                value *= -1;
            }
            string = buffer + sizeof buffer - 1;
            *string-- = 0;
            for (;;) {
                *string = '0' + value % 10;
                value /= 10;
                if (!value) {
                    break;
                }
                string--;
            }
            puts_nonewline(string);
            break;
        }
        args++;
    }
}

struct timespec {
    ssize_t tv_sec;  // seconds
    ssize_t tv_nsec; // nanoseconds
};

size_t GetTimeMs() {
    struct timespec ts;
    _sys(228, 1, (ssize_t)&ts, 0);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

//inline void* mmap(size_t size, unsigned long fd)
SMALL void* mmap(void* addr, size_t len, size_t prot, size_t flags, size_t fd, size_t offset)
{
    unsigned long call = 9; // syscall number for mmap

    unsigned long ret;
    asm volatile(
        "mov %5, %%r10\n\t"
        "mov %6, %%r8\n\t"
        "mov %7, %%r9\n\t"
        "syscall"
        : "=a"(ret)  // Output operand: return value in rax
        : "a"(call), // Input operands
        "D"(addr),
        "S"(len),
        "d"(prot),
        "r"(flags),
        "r"(fd),
        "r"(offset)
        : "r10", "r8", "r9", "rcx", "r11", "memory"  // Clobbered registers
        );

    return (void*)ret;
}

SMALL void* malloc(size_t len)
{
    return mmap(NULL, len, 3, 0x22, -1, 0);
}

SMALL ssize_t open(const char* const restrict pathname, const int flags, const int mode) {
    return _sys(2, (ssize_t)pathname, flags, mode);
}

SMALL ssize_t fopen(const char* const restrict pathname, const char* const restrict mode) {
    int flags = 0;
    int file_mode = 0644; // Default permissions: -rw-r--r--

    return open(pathname, flags, file_mode);
}

SMALL char* strstr(const char* haystack, const char* needle) {
    // If needle is empty, return haystack
    if (*needle == '\0') {
        return (char*)haystack;
    }

    // Iterate through haystack
    for (; *haystack != '\0'; haystack++) {
        // Start matching needle with haystack
        const char* h = haystack;
        const char* n = needle;

        while (*n != '\0' && *h == *n) {
            h++;
            n++;
        }

        // If the entire needle is matched
        if (*n == '\0') {
            return (char*)haystack;
        }
    }

    // If no match is found, return NULL
    return NULL;
}

double log(double x) {
    const double M_SQRT1_2 = 0.70710678118654752440;
    const double M_SQRT2 = 1.41421356237309504880;

    // Extract bits of the double
    uint64_t bits = *(uint64_t*)&x;

    // Extract exponent and mantissa
    int exponent = (int)((bits >> 52) & 0x7FF) - 1023; // Unbiased exponent
    uint64_t mantissa = bits & 0xFFFFFFFFFFFFF; // 52 bits of mantissa

    // Normalize mantissa to [1, 2)
    double m = (double)(mantissa) / (double)(1ULL << 52) + 1.0;

    // Range reduction: m in [sqrt(0.5), sqrt(2)]
    if (m < M_SQRT1_2) {
        m *= 2.0;
        exponent -= 1;
    }
    else if (m > M_SQRT2) {
        m *= 0.5;
        exponent += 1;
    }

    // Polynomial approximation of log(m)
    double t = m - 1.0;
    double p = t * (1.0 - 0.5 * t + t * t * (0.33333333333333333 - 0.25 * t));
    double ln_m = p;

    // Combine results
    double ln2 = 0.6931471805599453;
    double result = ln_m + exponent * ln2;

    return result;
}

SMALL static int read(int fd, void* data, int count) {
    return _sys(0, fd, (size_t)data, count);
}

SMALL char* fgets(char* string0, int count, int file)
{
    char* string;
    int result;
    (void)count;

    string = string0;
    for (;;)
    {
        result = read(file, string, 1);
        if (result < 1)
        {
            if (string == string0) return NULL;
            break;
        }
        string++;
        if (string[-1] == '\n') break;
    }
    *string = 0;
    return string0;
}

int poll(struct pollfd* fds, unsigned int nfds, int timeout) {
    return (int)_sys(7, (ssize_t)fds, (ssize_t)nfds, timeout);
}

#endif

#define min(a,b) (((a) < (b)) ? (a) : (b))

// Calculate how much time to spend on searching a move
SMALL void Optimum(struct SearchInfo* info, int time, int inc) {
    // Reserve some time overhead to avoid timing out in the engine-gui communication process
    const int safety_overhead = min(600, time / 2);
    time -= safety_overhead;
    // else if we received wtime/btime we calculate an over and upper bound for the time usage based on fixed coefficients
    if (info->timeset) {
        int basetime = time * 0.044 + inc * 0.85;
        // Never use more than 76% of the total time left for a single move
        const double maxtimeBound = 0.70 * time;
        // optime is the time we use to stop if we just cleared a depth
        const double optime = min(0.70 * basetime, maxtimeBound);
        // maxtime is the absolute maximum time we can spend on a search (unless it is bigger than the bound)
        const double maxtime = min(2.64 * basetime, maxtimeBound);
        info->stoptimeMax = info->starttime + maxtime;
        info->stoptimeBaseOpt = optime;
        info->stoptimeOpt = info->starttime + info->stoptimeBaseOpt;
    }
}

bool StopEarly(const struct SearchInfo* info) {
    // check if we used all the nodes/movetime we had or if we used more than our lowerbound of time
    return (info->timeset) && GetTimeMs() > info->stoptimeOpt;
}

bool TimeOver(const struct SearchInfo* info) {
    // check if more than Maxtime passed and we have to stop
    return  ((info->timeset) && ((info->nodes & 1023) == 1023)
        && GetTimeMs() > info->stoptimeMax);
}

SMALL void ScaleTm(struct ThreadData* td) {
    const int bestmove = return_bestmove;
    // Calculate how many nodes were spent on checking the best move
    const double bestMoveNodesFraction = (double)(td->nodeSpentTable[FromTo(bestmove)]) / (double)(td->info.nodes);
    const double nodeScalingFactor = (1.52 - bestMoveNodesFraction) * 1.74;
    // Scale the search time based on how many nodes we spent and how the best move changed
    td->info.stoptimeOpt = min(td->info.starttime + td->info.stoptimeBaseOpt * nodeScalingFactor, td->info.stoptimeMax);
}

struct TTable TT;

void* AlignedMalloc(size_t size, size_t alignment) {
    return malloc(size);
}

void ClearTT() {
    for (uint64_t i = 0; i < TT.paddedSize / sizeof(struct TTBucket); ++i) {
        TTBucket_init(&TT.pTable[i]);
    }
    TT.age = 1;
}

void InitTT(uint64_t MB) {
    const uint64_t ONE_KB = 1024;
    const uint64_t ONE_MB = ONE_KB * 1024;
    const uint64_t hashSize = ONE_MB * MB;
    TT.numBuckets = (hashSize / sizeof(struct TTBucket)) - 3;

    // We align to 2MB on Linux (huge pages), otherwise assume that 4KB is the page size
#if defined(USE_MADVISE)
    const uint64_t alignment = 2 * ONE_MB;
#else
    const uint64_t alignment = 4 * ONE_KB;
#endif

    // Pad the TT by using a ceil div and a multiply to get the size to be a multiple of `alignment`
    TT.paddedSize = (TT.numBuckets * sizeof(struct TTBucket) + alignment - 1) / alignment * alignment;
    TT.pTable = (struct TTBucket*)(AlignedMalloc(TT.paddedSize, alignment));

    // On linux we request huge pages to make use of the 2MB alignment
#if defined(USE_MADVISE)
    madvise(TT.pTable, TT.paddedSize, MADV_HUGEPAGE);
#endif

    ClearTT();
}

void TTEntry_init(struct TTEntry* const tte) {
    tte->move = NOMOVE;
    tte->score = SCORE_NONE;
    tte->eval = SCORE_NONE;
    tte->ttKey = 0;
    tte->depth = 0;
    tte->ageBoundPV = HFNONE;
}

void TTBucket_init(struct TTBucket* const ttb) {
    for (int i = 0; i < ENTRIES_PER_BUCKET; i++) {
        TTEntry_init(&ttb->entries[i]);
    }
}

bool ProbeTTEntry(const ZobristKey posKey, struct TTEntry* tte) {
    const uint64_t index = Index(posKey);
    struct TTBucket* bucket = &TT.pTable[index];
    for (int i = 0; i < ENTRIES_PER_BUCKET; i++) {
        *tte = bucket->entries[i];
        if (tte->ttKey == (TTKey)posKey) {
            return true;
        }
    }
    return false;
}

void StoreTTEntry(const ZobristKey key, const PackedMove move, int score, int eval, const int bound, const int depth, const bool pv, const bool wasPV) {
    // Calculate index based on the position key and get the entry that already fills that index
    const uint64_t index = Index(key);
    const TTKey key16 = (TTKey)key;
    const uint8_t TTAge = TT.age;
    struct TTBucket* bucket = &TT.pTable[index];
    struct TTEntry* tte = &bucket->entries[0];
    for (int i = 0; i < ENTRIES_PER_BUCKET; i++) {
        struct TTEntry* entry = &bucket->entries[i];

        if (entry->ttKey == key16) {
            tte = entry;
            break;
        }

        if (tte->depth - ((MAX_AGE + TTAge - AgeFromTT(tte->ageBoundPV)) & AGE_MASK) * 4
    > entry->depth - ((MAX_AGE + TTAge - AgeFromTT(entry->ageBoundPV)) & AGE_MASK) * 4) {
            tte = entry;
        }
    }

    // Replacement strategy taken from Stockfish
    // Preserve any existing move for the same position
    if (move || key16 != tte->ttKey)
        tte->move = move;

    // Overwrite less valuable entries (cheapest checks first)
    if (bound == HFEXACT
        || key16 != tte->ttKey
        || depth + 5 + 2 * pv > tte->depth
        || AgeFromTT(tte->ageBoundPV) != TTAge) {
        tte->ttKey = key16;
        tte->ageBoundPV = PackToTT(bound, wasPV, TTAge);
        tte->score = (int16_t)score;
        tte->eval = (int16_t)eval;
        tte->depth = (uint8_t)depth;
    }
}

uint64_t Index(const ZobristKey posKey) {
#ifdef __SIZEOF_INT128__
    return (uint64_t)((((__uint128_t)(posKey) * (__uint128_t)(TT.numBuckets)) >> 64));
#else
    // Workaround to use the correct bits when indexing the TT on a platform with no int128 support, code provided by Nanopixel
    uint64_t xlo = (uint32_t)posKey;
    uint64_t xhi = posKey >> 32;
    uint64_t nlo = (uint32_t)TT.numBuckets;
    uint64_t nhi = TT.numBuckets >> 32;
    uint64_t c1 = (xlo * nlo) >> 32;
    uint64_t c2 = (xhi * nlo) + c1;
    uint64_t c3 = (xlo * nhi) + (uint32_t)c2;

    return xhi * nhi + (c2 >> 32) + (c3 >> 32);
#endif
}

// prefetches the data in the given address in l1/2 cache in a non blocking way.
void prefetch(const void* addr) {
#if defined(__INTEL_COMPILER) || defined(_MSC_VER)
    _mm_prefetch((char*)addr, _MM_HINT_T0);
#else
    __builtin_prefetch(addr);
#endif
}

void TTPrefetch(const ZobristKey posKey) {
    prefetch(&TT.pTable[Index(posKey)].entries[0]);
}


int ScoreToTT(int score, int ply) {
    if (score > MATE_FOUND)
        score += ply;
    else if (score < -MATE_FOUND)
        score -= ply;

    return score;
}

int ScoreFromTT(int score, int ply) {
    if (score > MATE_FOUND)
        score -= ply;
    else if (score < -MATE_FOUND)
        score += ply;

    return score;
}

PackedMove MoveToTT(Move move) {
    return (move & 0xffff);
}

Move MoveFromTT(struct Position* pos, PackedMove packed_move) {
    // It's important to preserve a move being null even it's being unpacked
    if (packed_move == NOMOVE)
        return NOMOVE;

    const int piece = Position_PieceOn(pos, From(packed_move));
    return packed_move | (piece << 16);
}

uint8_t BoundFromTT(uint8_t ageBoundPV) {
    return ageBoundPV & 0b00000011;
}

bool FormerPV(uint8_t ageBoundPV) {
    return ageBoundPV & 0b00000100;
}

uint8_t AgeFromTT(uint8_t ageBoundPV) {
    return (ageBoundPV & 0b11111000) >> 3;
}

uint8_t PackToTT(uint8_t bound, bool wasPV, uint8_t age) {
    return (uint8_t)(bound + (wasPV << 2) + (age << 3));
}

void UpdateTableAge() {
    TT.age = (TT.age + 1) & AGE_MASK;
}

int see_margin[64][2];

// Parse a move from algebraic notation to the engine's internal encoding
SMALL Move ParseMove(const char* moveString, struct Position* pos) {
    // create move list instance
    struct MoveList moveList;
    moveList.count = 0;

    // generate moves
    GenerateMoves(&moveList, pos, MOVEGEN_ALL);

    // parse source square
    const int sourceSquare = (moveString[0] - 'a') + (8 - (moveString[1] - '0')) * 8;

    // parse target square
    const int targetSquare = (moveString[2] - 'a') + (8 - (moveString[3] - '0')) * 8;

    // loop over the moves within a move list
    for (int move_count = 0; move_count < moveList.count; move_count++) {
        // init move
        const int move = moveList.moves[move_count].move;
        // make sure source & target squares are available within the generated move
        if (sourceSquare == From(move) &&
            targetSquare == To(move)) {
            // promoted piece is available
            if (isPromo(move)) {
                switch (getPromotedPiecetype(move)) {
                case QUEEN:
                    if (moveString[4] == 'q')
                        return move;
                    break;
                case ROOK:
                    if (moveString[4] == 'r')
                        return move;
                    break;
                case BISHOP:
                    if (moveString[4] == 'b')
                        return move;
                    break;
                case KNIGHT:
                    if (moveString[4] == 'n')
                        return move;
                    break;
                }
                // continue the loop on possible wrong promotions (e.g. "e7e8f")
                continue;
            }
            // return legal move
            return move;
        }
    }
    printf("Illegal move parsed: %s", (size_t)moveString);

    // return illegal move
    return NOMOVE;
}

// parse UCI "go" command, returns true if we have to search afterwards and false otherwise
SMALL bool ParseGo(const char* const line, struct SearchInfo* info, struct Position* pos) {
    ResetInfo(info);
    int depth = -1;
    int time = -1, inc = 0;

    char token[128];
    int token_index = 0;

    // loop over all the tokens and parse the commands
    while (next_token(line, &token_index, token)) {
        if (!strcmp(token, "binc") && pos->side == BLACK) {
            next_token(line, &token_index, token);
            inc = atoi(token);
        }

        if (!strcmp(token, "winc") && pos->side == WHITE) {
            next_token(line, &token_index, token);
            inc = atoi(token);
        }

        if (!strcmp(token, "wtime") && pos->side == WHITE) {
            next_token(line, &token_index, token);
            time = atoi(token);
            info->timeset = true;
        }
        if (!strcmp(token, "btime") && pos->side == BLACK) {
            next_token(line, &token_index, token);
            time = atoi(token);
            info->timeset = true;
        }
    }

    info->starttime = GetTimeMs();
    info->depth = depth;
    // calculate time allocation for the move
    Optimum(info, time, inc);

    return true;
}

// parse UCI "position" command
SMALL void ParsePosition(const char* command, struct Position* pos) {
    // parse UCI "startpos" command
    if (strstr(command, "startpos") != NULL) {
        // init chess board with start position
        ParseFen(start_position, pos);
    }

    // parse UCI "fen" command
    else {
        // if a "fen" command is available within command string
        if (strstr(command, "fen") != NULL) {
            // init chess board with position from FEN string
            const char* fen_start_ptr = strstr(command, "fen") + 4;
            ParseFen(fen_start_ptr, pos);
        }
    }

#if UCI
    // if there are moves to be played in the fen play them
    if (strstr(command, "moves") != NULL) {
        pos->played_positions_size = 0;
        //auto string_start = command.find("moves") + 6;
        const char* string_start_ptr = strstr(command, "moves") + 6;
        // Avoid looking for a moves that doesn't exist in the case of "position startpos moves <blank>" (Needed for Scid support)
        if (*string_start_ptr != '\0') {
            const char* moves_substr = string_start_ptr;
            parse_moves(moves_substr, pos);
        }
    }
#endif

    // Update accumulator state to reflect the new position
    NNUE_accumulate(&pos->accumStack[0], pos);
    pos->accumStackHead = 1;
}

// main UCI loop
SMALL void UciLoop() {
#if BENCH
    int benchDepth = 14;
    StartBench(benchDepth);
    return;
#endif

    struct ThreadData td;
    init_thread_data(&td);

    // main loop
    while (true) {
#if UCI
        char input[8192];
#else
        char input[256];
#endif

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            break;
        }

        // make sure input is available
        if (input[0] == '\0') {
            continue;
        }

        size_t len = strlen(input);
        if (input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }

        char token[128];
        int input_index = 0;
        next_token(input, &input_index, token);

        // parse UCI "position" command
        if (!strcmp(token, "position")) {
            // call parse position function
            ParsePosition(input, &td.pos);
        }

        // parse UCI "go" command
        else if (!strcmp(token, "go")) {
            // call parse go function
            bool search = ParseGo(input, &td.info, &td.pos);
            // Start search in a separate thread
            RootSearch(MAXDEPTH, &td);
        }

#if UCI
        // parse UCI "isready" command
        else if (!strcmp(token, "isready")) {
            puts("readyok");
            fflush(stdout);
            continue;
        }

        // parse UCI "uci" command
        else if (!strcmp(token, "uci")) {
            // print engine info
            puts("id name SSE 0.1.1");
            puts("id author Zuppa and Gedas based on Alexandria\n");
            puts("uciok");
            fflush(stdout);
        }

        // parse UCI "ucinewgame" command
        else if (!strcmp(token, "ucinewgame")) {
            InitNewGame(&td);
        }

        else if (!strcmp(token, "eval")) {
            // print position eval
            printf("Raw eval: %i\n", EvalPositionRaw(&td.pos));
            printf("Scaled eval: %i\n", EvalPosition(&td.pos));
        }
        else printf("Unknown command: %s\n", (size_t)input);
#endif
    }
}
