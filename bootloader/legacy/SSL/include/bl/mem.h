#ifndef BL_MEM_H
#define BL_MEM_H
#include "types.h"

bool mem_init();
void* malloc(size_t count);
void* realloc(void* mem, size_t new_size);
void free(void* mem);

void mem_dump();

#endif
