#pragma once

#if STDLIB
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/mman.h>

inline uint64_t GetTimeMs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}
#else

typedef char bool;
typedef char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;
typedef int64_t ssize_t;
typedef uint64_t size_t;

enum {
    stdin = 0,
    stdout = 1,
    stderr = 2
};

#define false 0
#define true 1
#define NULL ((void *)0)

#if !NDEBUG
#define assert(condition)                                                      \
  if (!(condition)) {                                                          \
    printf("Assert failed on line %i: ", __LINE__);                            \
    puts(#condition "\n");                                                     \
    _sys(60, 1, 0, 0);                                                         \
  }
#else
#define assert(condition)
#endif

inline static ssize_t _sys(ssize_t call, ssize_t arg1, ssize_t arg2, ssize_t arg3) {
    ssize_t ret;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(call), "D"(arg1), "S"(arg2), "d"(arg3)
        : "rcx", "r11", "memory");
    return ret;
}

inline static void exit(const int returnCode) {
    _sys(60, returnCode, 0, 0);
}

inline static int strlen(const char* const restrict string) {
    int length = 0;
    while (string[length]) {
        length++;
    }
    return length;
}

static void write(int fd, void* data, int count) {
    _sys(1, fd, (size_t)data, count);
}

inline static void puts(const char* const restrict string) {
    write(stdout, string, strlen(string));
}

inline static void fflush(int fd) {
    
}

inline static bool strcmp(const char* restrict lhs,
    const char* restrict rhs) {
    while (*lhs || *rhs) {
        if (*lhs != *rhs) {
            return true;
        }
        lhs++;
        rhs++;
    }
    return false;
}

inline static atoi(const char* restrict string) {
    size_t result = 0;
    while (true) {
        if (!*string) {
            return result;
        }
        result *= 10;
        result += *string - '0';
        string++;
    }
}

#define printf(format, ...) _printf(format, (size_t[]){__VA_ARGS__})

inline static void _printf(const char* format, const size_t* args) {
    int value;
    char buffer[16], * string;

    while (true) {
        if (!*format) {
            break;
        }
        if (*format != '%') {
            write(stdout, format, 1);
            format++;
            continue;
        }

        format++;
        switch (*format++) {
        case 's':
            puts((char*)*args);
            break;
        case 'c':
            write(1, args, 1);
            break;
        case 'i':
            value = *args;
            if (value < 0) {
                puts("-");
                value *= -1;
            }
            string = buffer + sizeof buffer - 1;
            *string-- = 0;
            for (;;) {
                *string = '0' + value % 10;
                value /= 10;
                if (!value) {
                    break;
                }
                string--;
            }
            puts(string);
            break;
        }
        args++;
    }
}

struct timespec {
    ssize_t tv_sec;  // seconds
    ssize_t tv_nsec; // nanoseconds
};

inline static size_t GetTimeMs() {
    struct timespec ts;
    _sys(228, 1, (ssize_t)&ts, 0);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

//inline void* mmap(size_t size, unsigned long fd)
inline static void* mmap(void* addr, size_t len, size_t prot, size_t flags, size_t fd, size_t offset)
{
    unsigned long call = 9; // syscall number for mmap

    unsigned long ret;
    asm volatile(
        "mov %5, %%r10\n\t"
        "mov %6, %%r8\n\t"
        "mov %7, %%r9\n\t"
        "syscall"
        : "=a"(ret)  // Output operand: return value in rax
        : "a"(call), // Input operands
        "D"(addr),
        "S"(len),
        "d"(prot),
        "r"(flags),
        "r"(fd),
        "r"(offset)
        : "r10", "r8", "r9", "rcx", "r11", "memory"  // Clobbered registers
        );

    return (void*)ret;
}

inline static void* malloc(size_t len)
{
    return mmap(NULL, len, 3, 0x22, -1, 0);
}

inline static ssize_t open(const char* const restrict pathname, const int flags, const int mode) {
    return _sys(2, (ssize_t)pathname, flags, mode);
}

inline static ssize_t fopen(const char* const restrict pathname, const char* const restrict mode) {
    int flags = 0;
    int file_mode = 0644; // Default permissions: -rw-r--r--

    return open(pathname, flags, file_mode);
}

inline static char* strstr(const char* haystack, const char* needle) {
    // If needle is empty, return haystack
    if (*needle == '\0') {
        return (char*)haystack;
    }

    // Iterate through haystack
    for (; *haystack != '\0'; haystack++) {
        // Start matching needle with haystack
        const char* h = haystack;
        const char* n = needle;

        while (*n != '\0' && *h == *n) {
            h++;
            n++;
        }

        // If the entire needle is matched
        if (*n == '\0') {
            return (char*)haystack;
        }
    }

    // If no match is found, return NULL
    return NULL;
}

void* memset(void* ptr, int value, size_t n);

inline static double log(double x) {
    const int M_SQRT1_2 = 0.7071067811865476;
    const int M_SQRT2 = 1.4142135623730951;

    // Extract bits of the double
    uint64_t bits = *(uint64_t*)&x;

    // Extract exponent and mantissa
    int exponent = (int)((bits >> 52) & 0x7FF) - 1023; // Unbiased exponent
    uint64_t mantissa = bits & 0xFFFFFFFFFFFFF; // 52 bits of mantissa

    // Normalize mantissa to [1, 2)
    double m = (double)(mantissa) / (double)(1ULL << 52) + 1.0;

    // Range reduction: m in [sqrt(0.5), sqrt(2)]
    if (m < M_SQRT1_2) {
        m *= 2.0;
        exponent -= 1;
    }
    else if (m > M_SQRT2) {
        m *= 0.5;
        exponent += 1;
    }

    // Polynomial approximation of log(m)
    double t = m - 1.0;
    double p = t * (1.0 - 0.5 * t + t * t * (0.33333333333333333 - 0.25 * t));
    double ln_m = p;

    // Combine results
    double ln2 = 0.6931471805599453;
    double result = ln_m + exponent * ln2;

    return result;
}

inline static int read(int fd, void* data, int count) {
    return _sys(0, fd, (size_t)data, count);
}

inline static char* fgets(char* string0, int count, int file)
{
    char* string;
    int result;
    (void)count;

    string = string0;
    for (;;)
    {
        result = read(file, string, 1);
        if (result < 1)
        {
            if (string == string0) return NULL;
            break;
        }
        string++;
        if (string[-1] == '\n') break;
    }
    *string = 0;
    return string0;
}

#endif