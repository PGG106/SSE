#include "bitboard.h"

void set_bit(Bitboard* bitboard, const int square) { *bitboard |= (1ULL << square); }