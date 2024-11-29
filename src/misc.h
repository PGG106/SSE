#pragma once

#include <stdint.h>
#include <time.h>

inline uint64_t GetTimeMs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
