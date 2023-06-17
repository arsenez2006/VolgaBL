#ifndef BL_MEM_H
#define BL_MEM_H

#include "types.h"

bool __check_ret mem_init(void);

void __check_ret *malloc(size_t count);
void __check_ret *realloc(void *mem, size_t new_size);
void free(void *mem);

memory_map *get_memory_map(void);

#endif
