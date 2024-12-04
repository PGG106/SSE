#include "all.h"

int see_margin[64][2];

// Lookup to get the rank of a square
const uint8_t get_rank[64] = { 7, 7, 7, 7, 7, 7, 7, 7,
                               6, 6, 6, 6, 6, 6, 6, 6,
                               5, 5, 5, 5, 5, 5, 5, 5,
                               4, 4, 4, 4, 4, 4, 4, 4,
                               3, 3, 3, 3, 3, 3, 3, 3,
                               2, 2, 2, 2, 2, 2, 2, 2,
                               1, 1, 1, 1, 1, 1, 1, 1,
                               0, 0, 0, 0, 0, 0, 0, 0 };

// Lookup to get the file of a square
uint8_t get_file(const int square) {
    return square % 8;
}

// Lookup to get the diagonal of a square
const uint8_t get_diagonal[64] = { 14, 13, 12, 11, 10,  9,  8,  7,
                                   13, 12, 11, 10,  9,  8,  7,  6,
                                   12, 11, 10,  9,  8,  7,  6,  5,
                                   11, 10,  9,  8,  7,  6,  5,  4,
                                   10,  9,  8,  7,  6,  5,  4,  3,
                                    9,  8,  7,  6,  5,  4,  3,  2,
                                    8,  7,  6,  5,  4,  3,  2,  1,
                                    7,  6,  5,  4,  3,  2,  1,  0 };

// Lookup to get the color from a piece
const int Color[12] = { WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
                            BLACK, BLACK, BLACK, BLACK, BLACK, BLACK };

const int PieceType[12] = { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
                                PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };

// Contains the material Values of the pieces
const int SEEValue[15] = { 100, 422, 422, 642, 1015, 0,
                               100, 422, 422, 642, 1015, 0, 0, 0, 0 };
