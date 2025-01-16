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

    TUNE_VALUES(RFP_MARGIN, 100, 40, 200, 10)
    TUNE_VALUES(NMP_REDUCTION_EVAL_DIVISOR, 169, 100, 400, 20)
    TUNE_VALUES(RAZORING_COEFF_0, 288, 100, 400, 20)
    TUNE_VALUES(DOUBLE_EXTENSION_MARGIN, 18, 5, 100, 1)
    TUNE_VALUES(HISTORY_QUIET_LMR_DIVISOR, 8278, 1, 16383, 100)
    TUNE_VALUES(HISTORY_NOISY_LMR_DIVISOR, 6196, 1, 16383, 100)
    TUNE_VALUES(DO_DEEPER_BASE_MARGIN, 65, 1, 200, 20)
    TUNE_VALUES(DO_DEEPER_DEPTH_MARGIN, 5, 1, 50, 2)
    TUNE_VALUES(QS_FUTILITY, 171, -500, 500, 25)
    // BATCH 2, do not lose track
    TUNE_VALUES(HISTORY_BONUS_MAX, 1565, 1, 4096, 256)
    TUNE_VALUES(LMR_QUIET_BASE, 97, 40, 150, 7)
    TUNE_VALUES(LMR_QUIET_DIVISOR, 223, 150, 500, 15)
    TUNE_VALUES(LMR_NOISY_BASE, -25, -70, 100, 7)
    TUNE_VALUES(LMR_NOISY_DIVISOR, 219, 150, 500, 15)
    // PART 3
    TUNE_VALUES(SEE_QUIET_MARGIN, -77, -150, -20, 5)
    TUNE_VALUES(SEE_NOISY_MARGIN, -30, -100, -1, 3)
    // part 4
    TUNE_VALUES(FUTILITY_COEFF_0, 245, 40, 300, 10)
    TUNE_VALUES(FUTILITY_COEFF_1, 155, 40, 200, 10)
    // part 5
    TUNE_VALUES(LMR_DEPTH_HISTORY_DIVISOR, 8197, 1, 16383, 100)
    TUNE_VALUES(DELTA_RESIZE, 146, 110, 160, 2)

};
