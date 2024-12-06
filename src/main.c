#include "init.h"
#include "uci.h"

#ifdef NOSTDLIB
void _start() {
#else
int main() {
#endif

    // Tables for move generation and precompute reduction values
    InitAll();
    // connect to the GUI
    UciLoop();

#ifdef NOSTDLIB
    exit(0);
#else
    return 0;
#endif
}
