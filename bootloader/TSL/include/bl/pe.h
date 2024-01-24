#ifndef BL_PE_H
#define BL_PE_H

#include <bl/defines.h>
#include <bl/types.h>

typedef struct pe_load_state {
  dword_t load_addr;
  size_t  image_size;
  dword_t entry;
} pe_load_state;

/**
 * @brief Load PE into memory
 *
 * @param [in] pe_addr Address to PE file
 * @param [in] load_addr Address to load
 * @param [out] state Result of loading
 * @return true on success
 * @return false on failure
 */
bool pe_load(void* pe_addr, void* load_addr, pe_load_state* state);

#endif
