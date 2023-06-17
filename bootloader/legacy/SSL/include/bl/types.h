#ifndef BL_TYPES_H
#define BL_TYPES_H

#include "defines.h"
#include <stdint.h>
#include <stddef.h>

typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint32_t dword_t;
typedef uint64_t qword_t;

#ifndef __bool_true_false_are_defined
#define __bool_true_false_are_defined
typedef enum { false, true } bool;
#endif

#endif
