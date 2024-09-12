#ifndef COMMON_HARDWARE_DRIVE_MAPPING_GPT_H
#define COMMON_HARDWARE_DRIVE_MAPPING_GPT_H

#include "../../../attributes.h"
#include "../../../cpu/integer.h"

typedef struct __packed _gpt_mapping {
  qword_t signature;
  dword_t revision;
  dword_t header_size;
  dword_t checksum;
  dword_t rsv0;
  qword_t this_lba;
  qword_t alt_lba;
  qword_t first_lba;
  qword_t last_lba;
  byte_t  disk_guid[16];
  qword_t partition_table_lba;
  dword_t partition_count;
  dword_t partition_entry_size;
  dword_t partition_table_checksum;
} gpt_mapping;

typedef struct __packed _gpt_partition {
  byte_t  type[16];
  byte_t  guid[16];
  qword_t start_lba;
  qword_t end_lba;
  qword_t attributes;
} gpt_partition;

#endif
