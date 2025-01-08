#include "init.h"
#include "bench.h"
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

#ifdef UCI
    bool bench = argc > 1 && !strcmp(argv[1], "bench");
#endif
    InitAll();
#ifdef UCI
    if (bench) {
        StartBench(14);
    } else {
#endif
        UciLoop();
#ifdef UCI
    }
#endif

#ifdef NOSTDLIB
    exit(0);
#else
    return 0;
#endif
}
