/**
 * @file utils.h
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Utility functions used for Second Stage Loader
 *
 */
#ifndef BL_UTILS_H
#define BL_UTILS_H

#include "types.h"

/**
 * @brief Get value of DS register
 *
 * @return DS
 */
word_t
get_ds(void);

/**
 * @brief Calculate CRC32 checksum
 *
 * @param [in] buf Pointer to a buffer to be examined
 * @param [in] len Length of the buffer
 * @return CRC32 checksum
 */
uint32_t __check_ret
crc32(const byte_t* buf, size_t len);

/**
 * @brief Enable A20 line
 *
 * @return true on success
 * @return false on failure
 */
bool __check_ret
enable_A20(void);

/**
 * @brief Set the \ref GDT32_entry "GDT32 entry" object
 *
 * @param [out] entry Pointer to the \ref GDT32_entry "GDT32 entry" object to be
 * filled
 * @param [in] base Base address of the descriptor's segment
 * @param [in] limit Size of the descriptor's segment
 * @param [in] flags Flags of the descriptor
 * @param [in] access Access flags of the descriptor
 */
void
set_GDT32_entry(GDT32_entry* entry,
                dword_t base,
                dword_t limit,
                GDT_flags flags,
                GDT_access access);

/**
 * @brief Set the \ref GDTR32 "GDTR32" object
 *
 * @param [out] gdtr Pointer to \ref GDTR32 "GDTR32" object to be filled
 * @param [in] gdt32_table Pointer to the \ref GDT32_entry "GDT32"
 * @param [in] count Count of descriptor in gdt32_table
 */
void
set_GDTR32(GDTR32* gdtr, const GDT32_entry* gdt32_table, size_t count);

/**
 * @brief Load \ref GDTR32 "GDTR32" object
 *
 * @param [in] gdtr \ref GDTR32 "GDTR32" object
 */
void
load_GDT32(GDTR32 gdtr);

/**
 * @brief Turn on Unreal mode
 *
 * @param [in] data_segment_offset Offset to the data segment in loaded GDT
 */
void
enter_unreal(word_t data_segment_offset);

/**
 * @brief Get the partition array object
 *
 * @param [in] gpt_hdr Pointer to the \ref GPT_header "GPT header" object
 * @return  Pointer to the \ref GPT_partition_array "GPT Partition array"
 * object\n NULL on failure
 */
GPT_partition_array* __check_ret
get_partition_array(const GPT_header* gpt_hdr);

/**
 * @brief Find partition by GUID
 *
 * @param [in] partition_array
 * Pointer to the \ref GPT_partition_array "GPT Partition array" object
 * @param [in] GUID GUID of needed partition
 * @return  \ref GPT_partition_entry "GPT Partition entry" object\n
 *          NULL on failure
 */
GPT_partition_entry*
find_partition(const GPT_partition_array* partition_array, const byte_t* GUID);

/* Leave this undocumented */
#ifndef DOX_SKIP
#ifndef NDEBUG
extern void
_dump_heap(void);
extern void
_dump_memory_map(memory_map* mem_map);
#define dump_heap() _dump_heap()
#define dump_memory_map(mem_map) _dump_memory_map(mem_map)
#else
#define dump_heap()                                                            \
    do {                                                                       \
        continue;                                                              \
    } while (0)
#define dump_memory_map(mem_map)                                               \
    do {                                                                       \
        continue;                                                              \
    } while (0)
#endif /* NDEBUG */
#endif /* DOX_SKIP */

#endif /* BL_UTILS_H */
