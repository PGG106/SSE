#pragma once

#include "types.h"

extern const Bitboard file_bbs[8];
extern const Bitboard rank_bbs[8];

Bitboard ReverseBits(Bitboard bitboard);
Bitboard MaskedSlide(const Bitboard allPieces, const Bitboard pieceBitboard, const Bitboard mask);
Bitboard GetRookAttacks(int square, Bitboard occupancy);
