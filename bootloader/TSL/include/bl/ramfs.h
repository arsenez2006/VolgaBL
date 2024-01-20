/**
 * @file ramfs.h
 * @author Arseny Lashkevicj (arsenez@cybercommunity.space)
 * @brief Functions for reading RAMFS
 *
 */
#ifndef BL_RAMFS_H
#define BL_RAMFS_H

#include "defines.h"
#include "types.h"

/**
 * @brief Initialize RAMFS driver
 *
 * @param [in] address Address of RAMFS
 * @return true on success
 */
bool  ramfs_init(void* address);

/**
 * @brief Get memory right after RAMFS
 *
 * @return Pointer to the first usable block of memory after RAMFS
 */
void* ramfs_get_end(void);

/**
 * @brief Get file from RAMFS
 *
 * @param [in] name Full filename
 * @param [out] size Actual file size
 * @return Pointer to the file in memory
 */
void* ramfs_file(char const* name, size_t* size);

#endif
