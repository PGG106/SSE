#include "options.h"

#define TUNE_VALUES(name, initial, min, max, step) \
    .name = initial, \
    .name##_min = min, \
    .name##_max = max, \
    .name##_step = step,

#define TUNE_PARAMETER(name) \
    int name; \
    int name##_min;         \
    int name##_max;         \
    int name##_step;


struct Options options = {
    .Threads = 1,
};
