#include <bl/string.h>

size_t strlen(const char *start) {
    const char* end;
    for (end = start; *end != '\0'; ++end);
    return end - start;
}

int memcmp(const void *lhs, const void *rhs, size_t count) {
    while(count--) {
        if (*(byte_t*)lhs != *(byte_t*)rhs) {
            return (uintptr_t)lhs - (uintptr_t)rhs;
        }
        lhs = (byte_t*)lhs + 1;
        rhs = (byte_t*)rhs + 1;
    }
    return 0;
}

void* memcpy(void *dest, const void *src, size_t count) {
    size_t i;
    for (i = 0; i < count; ++i) {
        ((byte_t*)dest)[i] = ((byte_t*)src)[i];
    }
    return dest;
}
