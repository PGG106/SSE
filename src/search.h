#pragma once

#include "history.h"
#include "position.h"

#include "shims.h"

struct SearchStack {
    // don't init. search will init before entering the negamax method
    int16_t staticEval;
    Move excludedMove;
    Move move;
    int ply;
    Move searchKiller;
    int doubleExtensions;
};

struct SearchData {
    int searchHistory[2][64 * 64];
    int captHist[12 * 64][6];
    int pawnCorrHist[2][CORRHIST_SIZE];
    int whiteNonPawnCorrHist[2][CORRHIST_SIZE];
    int blackNonPawnCorrHist[2][CORRHIST_SIZE];
    int contHist[2][6 * 64][6 * 64];
};

// a collection of all the data a thread needs to conduct a search
struct ThreadData {
    struct Position pos;
    struct SearchData sd;
    struct SearchInfo info;
    int RootDepth;
    int nmpPlies;
    uint64_t nodeSpentTable[64 * 64];
};

extern Move return_bestmove;

void init_thread_data(struct ThreadData* td);

// ClearForSearch handles the cleaning of the thread data from a clean state
void ClearForSearch(struct ThreadData* td);

// Starts the search process, this is ideally the point where you can start a multithreaded search
void RootSearch(int depth, struct ThreadData* td);

// SearchPosition is the actual function that handles the search, it sets up the variables needed for the search , calls the Negamax function and handles the console output
void SearchPosition(int start_depth, int final_depth, struct ThreadData* td);

// Sets up aspiration windows and starts a Negamax search
int AspirationWindowSearch(int prev_eval, int depth, struct ThreadData* td);

int Negamax(int alpha, int beta, int depth, const bool cutNode, struct ThreadData* td, struct SearchStack* ss);

int Quiescence(int alpha, int beta, struct ThreadData* td, struct SearchStack* ss);

// inspired by the Weiss engine
bool SEE(const struct Position* pos, const int move, const int threshold);

// Checks if the current position is a draw
bool IsDraw(struct Position* pos);

bool IsRepetition(const struct Position* pos); // remove this later, make static
bool Is50MrDraw(struct Position* pos); // remove this later, make static
