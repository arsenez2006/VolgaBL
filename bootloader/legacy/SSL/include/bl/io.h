#ifndef BL_IO_H
#define BL_IO_H

#include "types.h"
#include <stdarg.h>

int vsnprintf(char* s, size_t n, const char* format, va_list arg);
int __print_fmt(3, 4) snprintf(char* s, size_t n, const char* format, ...);
int __print_fmt(2, 3) sprintf(char* s, const char* format, ...);
int __print_fmt(1, 2) printf(const char* format, ...);
int __print_fmt(1, 2) serial_printf(const char* format, ...);

#endif
