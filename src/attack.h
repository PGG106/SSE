#pragma once

#include "types.h"

// generate pawn attacks
Bitboard MaskPawnAttacks(int side, int square);

// generate knight attacks
Bitboard MaskKnightAttacks(int square);

// generate king attacks
Bitboard MaskKingAttacks(int square);

// pawn attacks table [side][square]
extern Bitboard pawn_attacks[2][64];

// knight attacks table [square]
extern Bitboard knight_attacks[64];

// king attacks table [square]
extern Bitboard king_attacks[64];

Bitboard pieceAttacks(int piecetype, int pieceSquare, Bitboard occ);

// set occupancies
Bitboard SetOccupancy(int index, int bits_in_mask, Bitboard attack_mask);
