#if STDLIB
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#else

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
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

#define false 0;
#define true 1;

#define printf(format, ...) _printf(format, (size_t[]){__VA_ARGS__})

inline ssize_t _sys(ssize_t call, ssize_t arg1, ssize_t arg2, ssize_t arg3) {
    ssize_t ret;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(call), "D"(arg1), "S"(arg2), "d"(arg3)
        : "rcx", "r11", "memory");
    return ret;
}

inline void exit() {
    _sys(60, 0, 0, 0);
}

inline int strlen(const char* const restrict string) {
    int length = 0;
    while (string[length]) {
        length++;
    }
    return length;
}

inline void puts(const char* const restrict string) {
    _sys(1, stdout, (ssize_t)string, strlen(string));
}

inline bool strcmp(const char* restrict lhs,
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

inline atoi(const char* restrict string) {
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

inline void _printf(const char* format, const size_t* args) {
    int value;
    char buffer[16], * string;

    while (true) {
        if (!*format) {
            break;
        }
        if (*format != '%') {
            _sys(1, stdout, (ssize_t)format, 1);
            format++;
            continue;
        }

        format++;
        switch (*format++) {
        case 's':
            puts((char*)*args);
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

#endif