#pragma once

#include "types.h"

#include "shims.h"

struct Position;
struct SearchData;
struct SearchStack;
struct MoveList;

const static int HH_MAX = 8192;
const static int CH_MAX = 16384;
const static int CAPTHIST_MAX = 16384;
const static int CORRHIST_WEIGHT_SCALE = 1024;
const static int CORRHIST_GRAIN = 256;
#define CORRHIST_SIZE 16384
const static int CORRHIST_MAX = 16384;

// Functions used to update the history heuristics
void UpdateHistories(const struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const int depth, const Move bestMove, const struct MoveList* quietMoves, const struct MoveList* noisyMoves, const bool rootNode);

// Fuction that returns the history bonus
int history_bonus(const int depth);

// Getters for the history heuristics
int GetHHScore(const struct Position* pos, const struct SearchData* sd, const Move move);
int GetCHScore(const struct SearchStack* ss, const Move move);
int GetSingleCHScore(const struct SearchStack* ss, const Move move, const int offset);
int GetCapthistScore(const struct Position* pos, const struct SearchData* sd, const Move move);
int GetHistoryScore(const struct Position* pos, const struct SearchData* sd, const Move move, const struct SearchStack* ss);
// Clean all the history tables
void CleanHistories(struct SearchData* sd);
// Updates history heuristics for a single move
void updateHHScore(const struct Position* pos, struct SearchData* sd, const Move move, int bonus);
void updateCHScore(struct SearchStack* ss, const Move move, const int bonus);
void updateCapthistScore(const struct Position* pos, struct SearchData* sd, const Move move, int bonus);

// Corrhist stuff
void updateCorrHistScore(const struct Position* pos, struct SearchData* sd, const struct SearchStack* ss, const int depth, const int diff);
int adjustEvalWithCorrHist(const struct Position* pos, const struct SearchData* sd, const struct SearchStack* ss, const int rawEval);