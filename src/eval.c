#include "eval.h"

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