/**
 * @file types.h
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Typedefs, data structers and enums used in Third Stage Loader
 *
 */
#ifndef BL_TYPES_H
#define BL_TYPES_H

#include "defines.h"
#include <stddef.h>
#include <stdint.h>

/**
 * @typedef byte_t
 * @brief Byte type
 *
 */
typedef uint8_t byte_t;

/**
 * @typedef word_t
 * @brief Word type
 *
 */
typedef uint16_t word_t;

/**
 * @typedef dword_t
 * @brief Double word type
 *
 */
typedef uint32_t dword_t;

/**
 * @typedef qword_t
 * @brief Quad word type
 *
 */
typedef uint64_t qword_t;

#ifndef __bool_true_false_are_defined
#define __bool_true_false_are_defined
typedef enum { false, true } bool;
#endif /* __bool_true_false_are_defined */

#endif /* BL_TYPES_H */
