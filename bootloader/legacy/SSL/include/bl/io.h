#ifndef BL_IO_H
#define BL_IO_H

#include "types.h"
#include <stdarg.h>

/**
 * @brief Write formatted data from variable argument list to sized buffer
 *
 * @param [out] s Pointer to a buffer where the resulting C-string is stored
 * @param [in] n Maximum number of bytes to be used in the buffer
 * @param [in] format C-string that contains a format string
 * @param [in] arg A value identifying a variable arguments list initialized with va_start
 * @return The number of characters that would have been written if n had been sufficiently large,
 * not counting the terminating null character
 */
int
vsnprintf(char* s, size_t n, const char* format, va_list arg);

/**
 * @brief Write formatted output to sized buffer
 *
 * @param [out] s Pointer to a buffer where the resulting C-string is stored
 * @param [in] n Maximum number of bytes to be used in the buffer
 * @param [in] format C-string that contains a format string
 * @param [in out] ... Additional args
 * @return The number of characters that would have been written if n had been sufficiently large,
 * not counting the terminating null character
 */
int __print_fmt(3, 4) snprintf(char* s, size_t n, const char* format, ...);

/**
 * @brief Write formatted data to string
 *
 * @param [out] s Pointer to a buffer where the resulting C-string is stored
 * @param [in] format C-string that contains a format string
 * @param [in] ... Additional args
 * @return The number of characters that would have been written if n had been sufficiently large,
 * not counting the terminating null character
 */
int __print_fmt(2, 3) sprintf(char* s, const char* format, ...);

/**
 * @brief Print formatted data to terminal
 *
 * @param [in] format C-string that contains a format string
 * @param [in] ... Additional args
 * @return The number of characters that would have been written if n had been sufficiently large,
 * not counting the terminating null character
 */
int __print_fmt(1, 2) printf(const char* format, ...);

/**
 * @brief Print formatted data to COM port
 *
 * @param [in] format C-string that contains a format string
 * @param [in] ... Additional args
 * @return The number of characters that would have been written if n had been sufficiently large,
 * not counting the terminating null character
 */
int __print_fmt(1, 2) serial_printf(const char* format, ...);

#endif /* BL_IO_H */
