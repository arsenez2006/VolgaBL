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

/**
 * @struct boot_info_t
 * @brief Boot info, passed to TSL and kernel
 *
 * @typedef boot_info_t
 * @brief boot_info_t type
 *
 */
typedef struct __packed boot_info_t {
    /**
     * @brief Size of this structure
     *
     */
    dword_t size;
    /**
     * @brief Booted drive info
     *
     */
    struct {
        /**
         * @brief Booted drive GUID
         *
         */
        byte_t GUID[16];
    } boot_drive;
    /**
     * @brief Memory map info
     *
     */
    struct {
        /**
         * @brief Count of memory map entries
         *
         */
        dword_t count;
        /**
         * @brief Size of each memory map entry
         *
         */
        dword_t entry_size;
        /**
         * @brief Physical address to memory map array
         *
         */
        qword_t address;
    } memory_map;

    /**
     * @brief Video info
     *
     */
    struct {
        /**
         * @brief Driver type
         *
         */
        dword_t type;
        /**
         * @brief Physical address to driver info
         *
         */
        qword_t address;
    } video_info;
} boot_info_t;

#endif /* BL_TYPES_H */
