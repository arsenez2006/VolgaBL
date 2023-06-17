#ifndef BL_UTILS_H
#define BL_UTILS_H

#include "types.h"

word_t get_ds(void);
uint32_t __check_ret crc32(const byte_t* buf, size_t len);
void set_GDT32_entry(GDT32_entry *entry, dword_t base, dword_t limit, GDT_flags flags, GDT_access access);
void set_GDTR32(GDTR32 *gdtr, const GDT32_entry *gdt32_table, size_t count);
void load_GDT32(GDTR32 gdtr);
void enter_unreal(word_t data_segment_offset);
void mem_dump(void);

#endif
