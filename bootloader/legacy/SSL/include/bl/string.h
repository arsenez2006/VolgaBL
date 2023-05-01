#ifndef BL_STRING_H
#define BL_STRING_H
#include "types.h"

size_t strlen(const char* str);
int memcmp(const void* lhs, const void* rhs, size_t count);
void* memcpy(void* dest, const void* src, size_t count);
void* memset(void* ptr, int val, size_t count);

#endif
