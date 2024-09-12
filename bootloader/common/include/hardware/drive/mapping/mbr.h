#ifndef COMMON_HARDWARE_DRIVE_MAPPING_MBR_H
#define COMMON_HARDWARE_DRIVE_MAPPING_MBR_H
#include "../../../attributes.h"
#include "../../../cpu/integer.h"

typedef struct __packed _mbr_partition {
  byte_t  attributes;
  byte_t  start_chs[3];
  byte_t  type;
  byte_t  last_chs[3];
  dword_t lba_start;
  dword_t sectors_count;
} mbr_partition;

typedef struct __packed _mbr_mapping {
  byte_t        code[440];
  dword_t       disk_id;
  word_t        rsv0;
  mbr_partition partition_table[4];
  word_t        signature;
} mbr_mapping;

_Static_assert(sizeof(struct _mbr_partition) == 16, "Illegal structure size");
_Static_assert(sizeof(struct _mbr_mapping) == 512, "Illegal structure size");

#endif
