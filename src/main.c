#include "init.h"
#include "uci.h"

#ifdef NOSTDLIB
SMALL void _start() {
#ifdef UCI
    int argc;
    char** argv;
    char** envp;

    // Get the address of argc, argv, and envp from the stack
    asm volatile(
        "movq (%%rsp), %%rax\n"  // Read 8 bytes at RSP into RAX
        "movl %%eax, %0\n"       // Move the low 32 bits of RAX into 'argc'
        : "=r" (argc)            // output: put final value in a general register => then stored in argc
        : /* no inputs */
        : "rax", "memory"
        );

    argv = (char**)((uintptr_t)&argc + sizeof(argc)); // argv comes right after argc
#endif
#else
SMALL int main(int argc, char **argv) {
#endif

#if UCI
    printf("%i", argc);
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
