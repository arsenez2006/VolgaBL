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
#endif /* __bool_true_false_are_defined */

typedef struct __packed _memory_map_entry {
    qword_t base;
    qword_t limit;
    dword_t type;
    dword_t ACPI;
} memory_map_entry;

typedef struct _memory_map_node {
    struct _memory_map_node *next;
    struct _memory_map_node *prev;
    memory_map_entry entry;
} memory_map_node;

typedef struct _memory_map {
    size_t count;
    memory_map_node *list;
} memory_map;

typedef struct __packed _GPT_header {
    byte_t magic[8];
    dword_t revision;
    dword_t hdr_size;
    dword_t crc32;
    byte_t rsv0[4];
    qword_t hdr_lba;
    qword_t alt_hdr_lba;
    qword_t first_usable;
    qword_t last_usable;
    byte_t guid[16];
    qword_t partition_array;
    dword_t entries_count;
    dword_t entry_size;
    dword_t partition_array_crc32;
} GPT_header;

typedef struct __packed _GPT_partition_entry {
    byte_t type[16];
    byte_t uuid[16];
    qword_t start_lba;
    qword_t end_lba;
    qword_t attrs;
} GPT_partition_entry;

typedef struct _GPT_partition_array {
    size_t count;
    size_t entry_size;
    GPT_partition_entry* array;
} GPT_partition_array;

typedef enum {
    GDT_ACCESS_PRESENT      = (1 << 7),
    GDT_ACCESS_DPL0         = (0 << 5),
    GDT_ACCESS_DPL1         = (1 << 5),
    GDT_ACCESS_DPL2         = (2 << 5),
    GDT_ACCESS_DPL3         = (3 << 5),
    GDT_ACCESS_SEGMENT      = (1 << 4),
    GDT_ACCESS_EXECUTABLE   = (1 << 3),
    GDT_ACCESS_DIRECTION    = (1 << 2),
    GDT_ACCESS_CONFORMING   = (1 << 2),
    GDT_ACCESS_READABLE     = (1 << 1),
    GDT_ACCESS_WRITEABLE    = (1 << 1)
} GDT_access;

typedef enum {
    GDT_FLAG_GRANULARITY    = (1 << 3),
    GDT_FLAG_SIZE           = (1 << 2),
    GDT_FLAG_LONG           = (1 << 1)
} GDT_flags;

typedef struct __packed _GDT32_entry {
    byte_t data[8];
} GDT32_entry;

typedef struct __packed _GDTR32 {
    byte_t data[6];
} GDTR32;

#endif /* BL_TYPES_H */
