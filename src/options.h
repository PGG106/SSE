#pragma once

#define TUNE_PARAMETER(name) \
    int name;               \
    int name##_min;         \
    int name##_max;         \
    int name##_step;

struct Options {
    int Threads;

    TUNE_PARAMETER(aspirationDelta)
};

extern struct Options options;
