#include "threads.h"

#if !NOSTDLIB
#include <pthread.h>
#endif

#include "init.h"
#include "search.h"
#include "uci.h"

#define SIGCHLD 17
#define CLONE_VM      0x00000100
#define CLONE_FS      0x00000200
#define CLONE_FILES   0x00000400
#define CLONE_SIGHAND 0x00000800

#define MY_THREAD_FLAGS (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD)

typedef void* (*thread_fn)(void*);

// We define a new helper that can take 5 parameters plus the syscall number (6 total).
ssize_t _sys6(ssize_t call,
    ssize_t arg1, ssize_t arg2, ssize_t arg3,
    ssize_t arg4, ssize_t arg5)
{
    ssize_t ret;
    asm volatile(
        "mov %4, %%r10\n\t"  // Move arg4 into r10
        "mov %5, %%r8\n\t"   // Move arg5 into r8
        "syscall"
        : "=a"(ret)
        : "a"(call),
        "D"(arg1),
        "S"(arg2),
        "d"(arg3),
        "r"(arg4),
        "r"(arg5)
        : "rcx", "r11", "memory", "r10", "r8"
        );
    return ret;
}

int createThread(thread_fn fn, void* arg) {
#if NOSTDLIB
    const size_t STACK_SIZE = 16 * 1024;
    char* stackMem = (char*)malloc(STACK_SIZE);
    if (!stackMem) {
        return -1;
    }

    char* stackTop = stackMem + STACK_SIZE;

    // Do the clone. 
    int ret = (int)_sys6(56, MY_THREAD_FLAGS, (ssize_t)stackTop, 0, 0, 0);
    if (ret == 0)
    {
        fn(arg);
        exit(0);
    }
    return ret;
#else
    pthread_t t1;
    pthread_create(&t1, NULL, fn, NULL);
#endif
}

void* UciFn(void* arg)
{
    (void)arg;
    UciLoop();
    //while (true) puts("hi1");
    return NULL;
}

void StartUciThread() {
    createThread(UciFn, NULL);
}

void SpinLock(volatile bool* condition) {
    while (!*condition) {}
}

void RunMainThread() {
    while (true)
    {
        SpinLock(&do_search);
        RootSearch(MAXDEPTH, current_td);
        finished = true;
        do_search = false;
    }
}