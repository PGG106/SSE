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
Bitboard MaskPawnAttacks(int side, int square) {
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
Bitboard MaskKnightAttacks(int square) {
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
Bitboard MaskKingAttacks(int square) {
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

// set/get/pop bit macros
void set_bit(Bitboard* bitboard, const int square) { *bitboard |= (1ULL << square); }
int get_bit(const Bitboard bitboard, const int square) { return bitboard & (1ULL << square); }
void pop_bit(Bitboard* bitboard, const int square) { *bitboard &= ~(1ULL << square); }

int GetLsbIndex(Bitboard bitboard) {
    assert(bitboard);
    return __builtin_ctzll(bitboard);;
}

int popLsb(Bitboard* bitboard) {
    assert(bitboard);
    int square = GetLsbIndex(*bitboard);
    *bitboard &= *bitboard - 1;
    return square;
}

int CountBits(Bitboard bitboard) {
    return __builtin_popcountll(bitboard);
}

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
    return NNUE_output(Position_AccumulatorTop(pos), pos->side);
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

        // Loop through all the quiet moves
        for (int i = 0; i < quietMoves->count; i++) {
            // For all the quiets moves that didn't cause a cut-off decrease the HH score
            const Move move = quietMoves->moves[i].move;
            if (move == bestMove) continue;
            updateHHScore(pos, sd, move, -bonus);
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
int GetCapthistScore(const struct Position* pos, const struct SearchData* sd, const Move move) {
    int capturedPiece = isEnpassant(move) ? PAWN : GetPieceType(Position_PieceOn(pos, To(move)));
    // If we captured an empty piece this means the move is a non capturing promotion, we can pretend we captured a pawn to use a slot of the table that would've otherwise went unused (you can't capture pawns on the 1st/8th rank)
    if (capturedPiece == EMPTY) capturedPiece = PAWN;
    return sd->captHist[PieceTo(move)][capturedPiece];
}

void updateSingleCorrHistScore(int* entry, const int scaledDiff, const int newWeight) {
    *entry = (*entry * (CORRHIST_WEIGHT_SCALE - newWeight) + scaledDiff * newWeight) / CORRHIST_WEIGHT_SCALE;
    *entry = clamp(*entry, -CORRHIST_MAX, CORRHIST_MAX);
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

int GetHistoryScore(const struct Position* pos, const struct SearchData* sd, const Move move) {
    if (!isTactical(move))
        return GetHHScore(pos, sd, move);
    else
        return GetCapthistScore(pos, sd, move);
}

// Resets the history tables
void CleanHistories(struct SearchData* sd) {
    memset(sd->searchHistory, 0, sizeof(sd->searchHistory));
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
