#include "bitboard.h"

#include "shims.h"

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
