#include "magic.h"
#include "hyperbola.h"

// get bishop attacks
Bitboard GetBishopAttacks(const int square, Bitboard occupancy) {
    // get bishop attacks assuming current board occupancy
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits;

    // return bishop attacks
    return bishop_attacks[square][occupancy];
}

// get queen attacks
Bitboard GetQueenAttacks(const int square, Bitboard occupancy) {
    return GetBishopAttacks(square, occupancy) | GetRookAttacks(square, occupancy);
}