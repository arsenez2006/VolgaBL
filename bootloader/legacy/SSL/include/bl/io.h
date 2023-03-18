#ifndef BL_IO_H
#define BL_IO_H
#include "types.h"
#include <stdarg.h>

int vsnprintf(char* s, size_t n, const char* format, va_list arg);
int snprintf(char* s, size_t n, const char* format, ...);
int sprintf(char* s, const char* format, ...);
int printf(const char* format, ...);

#endif
