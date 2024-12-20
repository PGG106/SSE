#pragma once

#include "position.h"

// if we don't have enough material to mate consider the position a draw
bool MaterialDraw(const struct Position* pos);

int ScaleMaterial(const struct Position* pos, int eval);
int EvalPositionRaw(struct Position* pos);

// position evaluation
int EvalPosition(struct Position* pos);