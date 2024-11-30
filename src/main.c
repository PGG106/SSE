#include "init.h"
#include "uci.h"

#if STDLIB
int main() {
#else
void _start() {
#endif

    // Tables for move generation and precompute reduction values
    InitAll();
    // connect to the GUI
    UciLoop();

#if !STDLIB
    exit(0);
#else
    return 0;
#endif
}
