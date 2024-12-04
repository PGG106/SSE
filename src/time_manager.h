#pragma once

#include "all.h"

struct SearchInfo;
struct ThreadData;

void Optimum(struct SearchInfo* info, int time, int inc);
bool StopEarly(const struct SearchInfo* info);
bool TimeOver(const struct SearchInfo* info);
