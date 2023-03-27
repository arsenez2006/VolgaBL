#ifndef BL_TYPES_H
#define BL_TYPES_H
#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include "defines.h"

typedef uint8_t     byte_t;
typedef uint16_t    word_t;
#ifdef _VLGBL_INT32
typedef uint32_t    dword_t;
#else
#define _VLGBL_CUSTOM_DWORDS
typedef struct _dword_t {
    word_t datal;
    word_t datah;
} dword_t;
#endif
#ifdef _VLGBL_INT64
typedef uint64_t    qword_t
#else
#define _VLGBL_CUSTOM_QWORDS
typedef struct _qword_t {
    dword_t datal;
    dword_t datah;
} qword_t;
#endif

#ifndef __bool_true_false_are_defined
#define __bool_true_false_are_defined
typedef enum { false, true } bool;
#endif

typedef struct _GPT_header {
    byte_t magic[8];
    dword_t revision;
    dword_t hdr_size;
    dword_t crc32;
    byte_t rsv0[4];
    byte_t hdr_lba[8];
    byte_t alt_hdr_lba[8];
    byte_t first_usable[8];
    byte_t last_usable[8];
    byte_t guid[16];
    byte_t partition_array[8];
    dword_t entries_count;
    dword_t entry_size;
    dword_t partition_array_crc32;
} GPT_header;

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

typedef struct _GDT_entry {
    byte_t data[8];
} GDT_entry;

typedef struct _GDTR48 {
    byte_t data[6];
} GDTR48;

typedef struct _GDTR80 {
    byte_t data[10];
} GDTR80;

#endif
