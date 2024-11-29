#pragma once

#include <stdint.h>
#include <time.h>
#include <poll.h>

inline uint64_t GetTimeMs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

inline bool InputWaiting()
{
#ifdef __linux__
    struct pollfd fds;
    fds.fd = 0; /* this is STDIN */
    fds.events = POLLIN;
    return poll(&fds, 1, 0);
#else
    return false;
#endif
}