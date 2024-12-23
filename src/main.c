#include "init.h"
#include "search.h"
#include "threads.h"

#if NOSTDLIB
SMALL void _start() {
#else
SMALL int main() {
#endif
    // Tables for move generation and precompute reduction values
    // connect to the GUI
    //UciLoop();
    do_search = false;
    StartUciThread();
    RunMainThread();

#if NOSTDLIB
    exit(0);
#else
    return 0;
#endif
}
