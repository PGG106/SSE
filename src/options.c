#include "options.h"

#define TUNE_VALUES(name, initial, min, max) \
    .name = initial, \
    .name##_min = min, \
    .name##_max = max, \
    .name##_step = (min + max) / 20,

#define TUNE_PARAMETER(name) \
    int name; \
    int name##_min;         \
    int name##_max;         \
    int name##_step;


struct Options options = {
    .Threads = 1,

    TUNE_VALUES(aspirationDelta, 12, 8, 32)
};
