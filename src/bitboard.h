#pragma once
#ifdef __cplusplus
#include <cassert>
#else
#include "assert.h"
#endif
#include "types.h"

// set/get/pop bit macros
inline void set_bit(Bitboard *bitboard, const int square) { *bitboard |= (1ULL << square); }
inline void pop_bit(Bitboard *bitboard, const int square) { *bitboard &= ~(1ULL << square); }

inline int GetLsbIndex(Bitboard bitboard) {
    assert(bitboard);
    return __builtin_ctzll(bitboard);;
}

inline int popLsb(Bitboard *bitboard) {
    assert(bitboard);
    int square = GetLsbIndex(*bitboard);
    *bitboard &= *bitboard - 1;
    return square;
}

inline int CountBits(Bitboard bitboard) {
    return __builtin_popcountll(bitboard);
}
