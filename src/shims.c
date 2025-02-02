#include "shims.h"

#if !NOSTDLIB
inline uint64_t GetTimeMs() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

SMALL void puts_nonewline(const char* const restrict string) {
    fputs(string, stdout);
}
#else

ssize_t _sys(ssize_t call, ssize_t arg1, ssize_t arg2, ssize_t arg3) {
    ssize_t ret;
    asm volatile("syscall"
        : "=a"(ret)
        : "a"(call), "D"(arg1), "S"(arg2), "d"(arg3)
        : "rcx", "r11", "memory");
    return ret;
}

SMALL void* memset(void* ptr, int value, size_t n) {
    unsigned char* p = (unsigned char*)ptr;
    while (n-- > 0) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}

SMALL void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

SMALL void exit(const int returnCode) {
    _sys(60, returnCode, 0, 0);
}

SMALL int strlen(const char* const restrict string) {
    int length = 0;
    while (string[length]) {
        length++;
    }
    return length;
}

SMALL static void write(int fd, void* data, int count) {
    _sys(1, fd, (size_t)data, count);
}

SMALL void puts(const char* const restrict string) {
    write(stdout, string, strlen(string));
    write(stdout, "\n", 1);
}

SMALL void puts_nonewline(const char* const restrict string) {
    write(stdout, string, strlen(string));
}

SMALL void fflush(int fd) {

}

SMALL bool strcmp(const char* restrict lhs,
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

SMALL int atoi(const char* restrict string) {
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

SMALL void _printf(const char* format, const size_t* args) {
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
            puts_nonewline((char*)*args);
            break;
        case 'c':
            write(1, args, 1);
            break;
        case 'd':
        case 'i':
            value = *args;
            if (value < 0) {
                puts_nonewline("-");
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
            puts_nonewline(string);
            break;
        }
        args++;
    }
}

struct timespec {
    ssize_t tv_sec;  // seconds
    ssize_t tv_nsec; // nanoseconds
};

size_t GetTimeMs() {
    struct timespec ts;
    _sys(228, 1, (ssize_t)&ts, 0);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

//inline void* mmap(size_t size, unsigned long fd)
SMALL void* mmap(void* addr, size_t len, size_t prot, size_t flags, size_t fd, size_t offset)
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

SMALL void* malloc(size_t len)
{
    return mmap(NULL, len, 3, 0x22, -1, 0);
}

SMALL ssize_t open(const char* const restrict pathname, const int flags, const int mode) {
    return _sys(2, (ssize_t)pathname, flags, mode);
}

SMALL ssize_t fopen(const char* const restrict pathname, const char* const restrict mode) {
    int flags = 0;
    int file_mode = 0644; // Default permissions: -rw-r--r--

    return open(pathname, flags, file_mode);
}

SMALL char* strstr(const char* haystack, const char* needle) {
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

SMALL double log(double x) {
    const double M_SQRT1_2 = 0.70710678118654752440;
    const double M_SQRT2 = 1.41421356237309504880;

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

SMALL static int read(int fd, void* data, int count) {
    return _sys(0, fd, (size_t)data, count);
}

SMALL char* fgets(char* string0, int count, int file)
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

int poll(struct pollfd* fds, unsigned int nfds, int timeout) {
    return (int)_sys(7, (ssize_t)fds, (ssize_t)nfds, timeout);
}

#endif
