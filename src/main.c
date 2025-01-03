#include "all.h"

#if NOSTDLIB
SMALL void _start() {
#else
SMALL int main() {
#endif

    // Tables for move generation and precompute reduction values
    InitAll();
    // connect to the GUI
    UciLoop();

#if NOSTDLIB
    exit(0);
#else
    return 0;
#endif
}
