#pragma once

#include "types.h"

extern const Bitboard file_bbs[8];
extern const Bitboard rank_bbs[8];
extern const Bitboard diagonal_bbs[15];
extern const Bitboard antidiagonal_bbs[15];

static Bitboard ReverseBits(Bitboard bitboard)
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

static Bitboard MaskedSlide(const Bitboard allPieces, const Bitboard pieceBitboard, const Bitboard mask)
{
	const Bitboard left = ((allPieces & mask) - 2 * pieceBitboard);
	const Bitboard right = ReverseBits(ReverseBits(allPieces & mask) - 2 * ReverseBits(pieceBitboard));
	const Bitboard both = left ^ right;
	const Bitboard slide = both & mask;
	return slide;
}

// get bishop attacks
static Bitboard GetBishopAttacks(int square, Bitboard occupancy) {
	const Bitboard pieceBitboard = 1ULL << square;
	const Bitboard diagonal = MaskedSlide(occupancy, pieceBitboard, diagonal_bbs[square / 8 + square % 8]);
	const Bitboard antidiagonal = MaskedSlide(occupancy, pieceBitboard, antidiagonal_bbs[square / 8 + 7 - square % 8]);
	return diagonal | antidiagonal;
}

// get rook attacks
static Bitboard GetRookAttacks(int square, Bitboard occupancy) {
	const Bitboard pieceBitboard = 1ULL << square;
	const Bitboard horizontal = MaskedSlide(occupancy, pieceBitboard, rank_bbs[square / 8]);
	const Bitboard vertical = MaskedSlide(occupancy, pieceBitboard, file_bbs[square % 8]);
	return horizontal | vertical;
}

// get queen attacks
static Bitboard GetQueenAttacks(const int square, Bitboard occupancy) {
    return GetBishopAttacks(square, occupancy) | GetRookAttacks(square, occupancy);
}
