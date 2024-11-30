//#if !STDLIB
#include "shims.h"

int stdin = 1;

void* memset(void* ptr, int value, size_t n) {
    // Cast the void pointer to an unsigned char pointer for byte-wise operations
    unsigned char* p = (unsigned char*)ptr;

    // Fill n bytes with the value
    while (n--) {
        *p++ = (unsigned char)value;
    }

    return ptr;
}
//#endif
