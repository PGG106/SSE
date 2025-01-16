#pragma once

#define TUNE_PARAMETER(name) \
    int name;               \
    int name##_min;         \
    int name##_max;         \
    int name##_step;

struct Options {
    int Threads;

    TUNE_PARAMETER(RFP_MARGIN)
    TUNE_PARAMETER(NMP_REDUCTION_EVAL_DIVISOR)
    TUNE_PARAMETER(RAZORING_COEFF_0)
    TUNE_PARAMETER(DOUBLE_EXTENSION_MARGIN)
    TUNE_PARAMETER(HISTORY_QUIET_LMR_DIVISOR)
    TUNE_PARAMETER(HISTORY_NOISY_LMR_DIVISOR)
    TUNE_PARAMETER(DO_DEEPER_BASE_MARGIN)
    TUNE_PARAMETER(DO_DEEPER_DEPTH_MARGIN)
    TUNE_PARAMETER(QS_FUTILITY)
};

extern struct Options options;
