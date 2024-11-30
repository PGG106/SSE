#pragma once

#include "shims.h"

struct SearchInfo;
struct ThreadData;

#ifdef __cplusplus
extern "C" {
#endif

    void Optimum(struct SearchInfo* info, int time, int inc);
    bool StopEarly(const struct SearchInfo* info);
    bool TimeOver(const struct SearchInfo* info);

#ifdef __cplusplus
}
#endif
