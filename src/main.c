#include "init.h"
#include "search.h"
#include "threads.h"

#if NOSTDLIB
SMALL void _start() {
#else
SMALL int main() {
#endif
    struct ThreadData td;
    init_thread_data(&td);
    current_td = &td;

    StartUciThread();
    RunMainThread();

#if NOSTDLIB
    exit(0);
#else
    return 0;
#endif
}
