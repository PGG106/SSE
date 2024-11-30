#if !STDLIB
#include "shims.h"

#pragma GCC push_options
#pragma GCC optimize("O2")
void* memset(void* ptr, int value, size_t n) {
    unsigned char* p = (unsigned char*)ptr;
    while (n-- > 0) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}
#pragma GCC pop_options
#endif
