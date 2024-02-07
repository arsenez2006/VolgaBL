#ifndef BL_PE_H
#define BL_PE_H

#include <bl/defines.h>
#include <bl/types.h>

typedef struct pe_load_state {
  char    name[256];
  dword_t load_addr;
  dword_t image_size;
  dword_t entry;
} pe_load_state;

/**
 * @brief Initialize PE loader
 *
 * @param [in] start Beginning of memory where PE images are put
 * @return true on success
 * @return false on failure
 */
bool pe_loader_init(void* start);

/**
 * @brief Get memory range used for loading PE images
 *
 * @param [out] begin Begin of memory range
 * @param [out] end End of memory range
 */
void pe_get_memory_range(void** begin, void** end);

/**
 * @brief Load PE into memory
 *
 * @param [in] file Full path to PE file in RAMFS
 * @param [out] state Result of loading
 * @return true on success
 * @return false on failure
 */
bool pe_load(char const* filename, pe_load_state** state);

#endif
