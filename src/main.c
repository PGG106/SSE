#include "init.h"
#include "uci.h"

#include "threads.h"

int printNumber(void* ptr)
{
    (void)ptr;
    while (true)
    printf("HELLO\n");
    return 0;
}

#if NOSTDLIB
SMALL void _start() {
#else
SMALL int main() {
#endif
    createThread(printNumber, (void*)0);

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
