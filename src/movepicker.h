#pragma once
#include "search.h"

#include "shims.h"

struct MoveList;

enum {
    PICK_TT,
    GEN_NOISY,
    PICK_GOOD_NOISY,
    PICK_KILLER,
    GEN_QUIETS,
    PICK_QUIETS,
    GEN_BAD_NOISY,
    PICK_BAD_NOISY
};

enum MovepickerType {
    SEARCH,
    QSEARCH
};

struct Movepicker {
    enum MovepickerType movepickerType;
    struct Position* pos;
    struct SearchData* sd;
    struct MoveList moveList;
    struct MoveList badCaptureList;
    Move ttMove;
    Move killer;
    int idx;
    int stage;
};

#ifdef __cplusplus
extern "C" {
#endif
    void InitMP(struct Movepicker* mp, struct Position* pos, struct SearchData* sd, struct SearchStack* ss, const Move ttMove, const enum MovepickerType movepickerType, const bool rootNode);
    Move NextMove(struct Movepicker* mp, const bool skip);
#ifdef __cplusplus
}
#endif

