#pragma once

#include "types.h"

// set/get/pop bit macros
void set_bit(Bitboard* bitboard, const int square);
int get_bit(const Bitboard bitboard, const int square);
void pop_bit(Bitboard* bitboard, const int square);
int GetLsbIndex(Bitboard bitboard);
int popLsb(Bitboard* bitboard);
int CountBits(Bitboard bitboard);
