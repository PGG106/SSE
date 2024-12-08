#pragma once
#include "position.h"

// print move (for UCI purposes)
void PrintMove(const Move move);
bool next_token(const char* str, int* index, char* token);
