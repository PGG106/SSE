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

    TUNE_VALUES(RFP_MARGIN, 91, 40, 200, 10)
    TUNE_VALUES(NMP_REDUCTION_EVAL_DIVISOR, 200, 100, 400, 20)
    TUNE_VALUES(RAZORING_COEFF_0, 256, 100, 400, 20)
    TUNE_VALUES(DOUBLE_EXTENSION_MARGIN, 12, 5, 32)
    TUNE_VALUES(HISTORY_QUIET_LMR_DIVISOR, 8192, 1, 16383, 100)
    TUNE_VALUES(HISTORY_NOISY_LMR_DIVISOR, 6144, 1, 16383, 100)
    TUNE_VALUES(DO_DEEPER_BASE_MARGIN, 53, 1, 200, 20)
    TUNE_VALUES(DO_DEEPER_DEPTH_MARGIN, 2, 1, 50, 2)
    TUNE_VALUES(QS_FUTILITY, 192, -500, 500, 25)
};
