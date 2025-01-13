#include "hyperbola.h"

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

Bitboard GetRookAttacks(int square, Bitboard occupancy) {
    const Bitboard pieceBitboard = 1ULL << square;
    const Bitboard horizontal = MaskedSlide(occupancy, pieceBitboard, rank_bbs[square / 8]);
    const Bitboard vertical = MaskedSlide(occupancy, pieceBitboard, file_bbs[square % 8]);
    return horizontal | vertical;
}
