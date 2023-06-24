/**
 * @file mem.h
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Memory allocator
 * 
 */
#ifndef BL_MEM_H
#define BL_MEM_H

#include "types.h"

/**
 * @brief Initialize memory allocator
 *
 * @return true on success
 * @return false on failure
 */
bool __check_ret
mem_init(void);

/**
 * @brief Allocate memory
 *
 * @param [in] count Number of bytes to allocate
 * @return  Pointer to allocated block\n
 *          NULL on failure
 */
void* __check_ret
malloc(size_t count);

/**
 * @brief Reallocate memory
 *
 * @param [in] mem Pointer to block to reallocate
 * @param [in] new_size New size of the block
 * @return  Pointer to the reallocated block
 *          NULL on failure
 */
void* __check_ret
realloc(void* mem, size_t new_size);

/**
 * @brief Free memory block
 *
 * @param [in] mem Pointer to the block
 */
void
free(void* mem);

/**
 * @brief Get the \ref memory_map "memory map" object
 *
 * @return  Pointer to the \ref memory_map "memory map" object
 *          NULL on failure
 */
memory_map*
get_memory_map(void);

#endif /* BL_MEM_H */
