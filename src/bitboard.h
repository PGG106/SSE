#pragma once

#include "shims.h"
#include "types.h"

// set/get/pop bit macros
static void set_bit(Bitboard *bitboard, const int square) { *bitboard |= (1ULL << square); }
inline int get_bit(const Bitboard bitboard, const int square) { return bitboard & (1ULL << square); }
inline void pop_bit(Bitboard *bitboard, const int square) { *bitboard &= ~(1ULL << square); }

static int GetLsbIndex(Bitboard bitboard) {
    assert(bitboard);
    return __builtin_ctzll(bitboard);;
}

static int popLsb(Bitboard *bitboard) {
    assert(bitboard);
    int square = GetLsbIndex(*bitboard);
    *bitboard &= *bitboard - 1;
    return square;
}

inline int CountBits(Bitboard bitboard) {
    return __builtin_popcountll(bitboard);
}
