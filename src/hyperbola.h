#pragma once

#include "types.h"

extern const Bitboard file_bbs[8];
extern const Bitboard rank_bbs[8];
extern const Bitboard diagonal_bbs[15];
extern const Bitboard antidiagonal_bbs[15];

Bitboard ReverseBits(Bitboard bitboard);
Bitboard MaskedSlide(const Bitboard allPieces, const Bitboard pieceBitboard, const Bitboard mask);
// get bishop attacks
Bitboard GetBishopAttacks(int square, Bitboard occupancy);
// get rook attacks
Bitboard GetRookAttacks(int square, Bitboard occupancy);
// get queen attacks
Bitboard GetQueenAttacks(const int square, Bitboard occupancy);
