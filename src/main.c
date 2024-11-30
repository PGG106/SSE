#include "init.h"
#include "uci.h"

#if STDLIB
int main(int argc, char** argv) {
#else
void _start(int argc) {
#endif

    // Tables for move generation and precompute reduction values
    InitAll();
    // connect to the GUI
    UciLoop(argc);

#if !STDLIB
    exit(0);
#else
    return 0;
#endif
}
