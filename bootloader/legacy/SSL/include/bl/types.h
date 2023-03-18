#ifndef BL_TYPES_H
#define BL_TYPES_H
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

typedef uint8_t byte_t;
typedef uint16_t word_t;
#ifdef __SMALLER_C_32__
typedef uint32_t dword_t
#endif

#ifndef __bool_true_false_are_defined
#define __bool_true_false_are_defined
typedef enum { false, true } bool;
#endif

#endif
