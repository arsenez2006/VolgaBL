/**
 * @file types.h
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Typedefs, data structers and enums used in Second Stage Loader
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
 * @struct memory_map_entry
 * @brief Memory map entry
 * @details Describes a memory region
 *
 * @typedef memory_map_entry
 * @brief memory_map_entry type
 *
 */
typedef struct __packed memory_map_entry {
    /**
     * @brief Start of memory region
     *
     */
    qword_t base;
    /**
     * @brief Size of memory region
     *
     */
    qword_t limit;
    /**
     * @brief Type of memory region
     *
     */
    dword_t type;
    /**
     * @brief ACPI info
     *
     */
    dword_t ACPI;
} memory_map_entry;

/**
 * @struct memory_map_node
 * @brief Memory map node
 * @details Describes a single node of memory map list
 *
 * @typedef memory_map_node
 * @brief memory_map_node type
 *
 */
typedef struct memory_map_node {
    /**
     * @brief Pointer to next node
     *
     */
    struct memory_map_node* next;
    /**
     * @brief Pointer to previous node
     *
     */
    struct memory_map_node* prev;
    /**
     * @brief Memory map entry
     *
     */
    memory_map_entry entry;
} memory_map_node;

/**
 * @struct memory_map
 * @brief Memory map
 * @details Holds a pointer to the memory map list and count of entries
 *
 * @typedef memory_map
 * @brief memory_map type
 *
 */
typedef struct memory_map {
    /**
     * @brief Count of entries
     *
     */
    size_t count;
    /**
     * @brief Pointer to the memory map list
     *
     */
    memory_map_node* list;
} memory_map;

/**
 * @struct GPT_header
 * @brief GPT header
 *
 * @typedef GPT_header
 * @brief GPT_header type
 *
 */
typedef struct __packed GPT_header {
    /**
     * @brief GPT Signature
     * @details Must be "EFI PART"
     *
     */
    byte_t magic[8];
    /**
     * @brief GPT Revision
     *
     */
    dword_t revision;
    /**
     * @brief GPT Header size
     *
     */
    dword_t hdr_size;
    /**
     * @brief CRC32 Checksum of GPT header
     * @details Calculated with \ref GPT_header::crc32 "CRC32 field" equal 0
     *
     */
    dword_t crc32;
    /**
     * @brief Reserved
     *
     */
    byte_t rsv0[4];
    /**
     * @brief LBA of the primary \ref GPT_header "GPT header"
     *
     */
    qword_t hdr_lba;
    /**
     * @brief LBA of the alternate \ref GPT_header "GPT header"
     *
     */
    qword_t alt_hdr_lba;
    /**
     * @brief First usable LBA
     *
     */
    qword_t first_usable;
    /**
     * @brief Last usable LBA
     *
     */
    qword_t last_usable;
    /**
     * @brief GUID of the drive
     *
     */
    byte_t guid[16];
    /**
     * @brief Starting LBA of \ref GPT_partition_entry "GPT partition array"
     *
     */
    qword_t partition_array;
    /**
     * @brief Count of entries in \ref GPT_partition_entry "GPT partition array"
     *
     */
    dword_t entries_count;
    /**
     * @brief Size of \ref GPT_partition_entry "GPT partition entry"
     *
     */
    dword_t entry_size;
    /**
     * @brief \ref GPT_partition_entry "GPT partition array" CRC32 Checksum
     *
     */
    dword_t partition_array_crc32;
} GPT_header;

/**
 * @struct GPT_partition_entry
 * @brief GPT partiion entry
 * @warning sizeof(\ref GPT_partition_entry) is not an actual size of entry.
 * Refer \ref GPT_header::entry_size "GPT Header" or \ref
 * GPT_partition_array::entry_size "GPT Partition array"
 *
 * @typedef GPT_partition_entry
 * @brief GPT_partition_entry type
 *
 */
typedef struct __packed GPT_partition_entry {
    /**
     * @brief Parition type
     *
     */
    byte_t type[16];
    /**
     * @brief Partition UUID
     *
     */
    byte_t uuid[16];
    /**
     * @brief Starting LBA of the partition
     *
     */
    qword_t start_lba;
    /**
     * @brief Ending LBA of the partition
     *
     */
    qword_t end_lba;
    /**
     * @brief Partition attributes
     *
     */
    qword_t attrs;
} GPT_partition_entry;

/**
 * @struct GPT_partition_array
 * @brief GPT partition array
 * @details Holds a pointer to the array, count of enries and size of each entry
 *
 * @typedef GPT_partition_array
 * @brief GPT_partition_array type
 *
 */
typedef struct GPT_partition_array {
    /**
     * @brief Count of entries
     *
     */
    size_t count;
    /**
     * @brief Size of single entry
     *
     */
    size_t entry_size;
    /**
     * @brief Pointer to the array
     *
     */
    GPT_partition_entry* array;
} GPT_partition_array;

/**
 * @enum GDT_access
 * @brief Access flags for GDT entry
 *
 */
typedef enum {
    /**
     * @brief Defines a valid segment
     *
     */
    GDT_ACCESS_PRESENT = (1 << 7),
    /**
     * @brief Set DPL = 0
     *
     */
    GDT_ACCESS_DPL0 = (0 << 5),
    /**
     * @brief Set DPL = 1
     *
     */
    GDT_ACCESS_DPL1 = (1 << 5),
    /**
     * @brief Set DPL = 2
     *
     */
    GDT_ACCESS_DPL2 = (2 << 5),
    /**
     * @brief Set DPL = 3
     *
     */
    GDT_ACCESS_DPL3 = (3 << 5),
    /**
     * @brief Defines normal, non-TSS segment
     *
     */
    GDT_ACCESS_SEGMENT = (1 << 4),
    /**
     * @brief Defines executable segment
     *
     */
    GDT_ACCESS_EXECUTABLE = (1 << 3),
    /**
     * @brief Defines a data segment, which grows down
     *
     */
    GDT_ACCESS_DIRECTION = (1 << 2),
    /**
     * @brief Defines a code segment, which can be executed from an equal or
     * lower privilege level
     *
     */
    GDT_ACCESS_CONFORMING = (1 << 2),
    /**
     * @brief Defines a readable code segment
     *
     */
    GDT_ACCESS_READABLE = (1 << 1),
    /**
     * @brief Defines a writeable data segment
     *
     */
    GDT_ACCESS_WRITEABLE = (1 << 1)
} GDT_access;

/**
 * @enum GDT_flags
 * @brief Flags for GDT entry
 *
 */
typedef enum {
    /**
     * @brief Granularity flag
     * @details Scales limit with page granularity (4KB)
     */
    GDT_FLAG_GRANULARITY = (1 << 3),
    /**
     * @brief Size flag
     * @details Defines 32bit segment
     */
    GDT_FLAG_SIZE = (1 << 2),
    /**
     * @brief Long mode code flag
     * @details Defines 64bit code segment.
     * @warning Don't use with \ref GDT_flags::GDT_FLAG_SIZE "Size flag"
     */
    GDT_FLAG_LONG = (1 << 1)
} GDT_flags;

/**
 * @struct GDT32_entry
 * @brief Protected mode GDT entry
 *
 * @typedef GDT32_entry
 * @brief GDT32_entry type
 *
 */
typedef struct __packed GDT32_entry {
    /**
     * @warning Should be filled with \ref set_GDT32_entry
     *
     */
    byte_t data[8];
} GDT32_entry;

/**
 * @struct GDTR32
 * @brief Protected mode GDTR
 *
 * @typedef GDTR32
 * @brief GDTR32 type
 *
 */
typedef struct __packed GDTR32 {
    /**
     * @warning Should be filled with \ref set_GDTR32
     *
     */
    byte_t data[6];
} GDTR32;

#endif /* BL_TYPES_H */
