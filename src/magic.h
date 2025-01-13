#pragma once

#include "attack.h"
#include "types.h"

// get bishop attacks
Bitboard GetBishopAttacks(const int square, Bitboard occupancy);

// get queen attacks
inline Bitboard GetQueenAttacks(const int square, Bitboard occupancy);