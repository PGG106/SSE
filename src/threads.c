#include "threads.h"

#include "shims.h"

// If needed, define SIGCHLD for your architecture:
#define SIGCHLD 17

// Linux clone flags you want (example):
#define CLONE_VM      0x00000100
#define CLONE_FS      0x00000200
#define CLONE_FILES   0x00000400
#define CLONE_SIGHAND 0x00000800

// Example combine them:
#define MY_THREAD_FLAGS (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD)

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

// Minimal clone wrapper for demonstration:
static ssize_t clone_syscall(ssize_t flags, ssize_t stack,
    ssize_t ptid, ssize_t ctid, ssize_t tls)
{
    // Syscall #56 is clone on x86_64.
    return _sys6(56, flags, stack, ptid, ctid, tls);
}

int createThread(thread_fn fn, void* arg)
{
    // Allocate minimal stack:
    const size_t STACK_SIZE = 8192;
    char* stackMem = (char*)malloc(STACK_SIZE);
    if (!stackMem) {
        return -1;
    }

    // For downward-growing stack, pass the end address:
    char* stackTop = stackMem + STACK_SIZE;

    // Do the clone. 
    int ret = (int)clone_syscall(MY_THREAD_FLAGS, (ssize_t)stackTop, 0, 0, 0);
    if (ret == 0)
    {
        // Child path (clone returns 0 here).
        fn(arg);
        // If you used CLONE_THREAD plus other flags, you might want:
        //   _sys(231, 0, 0, 0); // SYS_exit for just this thread
        // If you used “thread-like” flags incorrectly, exit(0) might kill the process. 
        exit(0);
    }
    return ret;  // Parent gets child's TID, or error.
}