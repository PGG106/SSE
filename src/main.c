#include "init.h"
#include "uci.h"

#ifdef NOSTDLIB
SMALL __attribute__((naked)) void _start() {
#ifdef UCI
    register long* stack asm("rsp");
    int argc = (int)*stack;
    char** argv = (char**)(stack + 1);
#endif
#else
SMALL int main(int argc, char **argv) {
#endif

#if UCI
    if (argc > 1 && !strcmp(argv[1], "bench")) {
        puts("benching");
    }
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
