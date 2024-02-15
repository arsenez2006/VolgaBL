/**
 * @file mem.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Memory allocator
 *
 */
#include <bl/bios.h>
#include <bl/io.h>
#include <bl/mem.h>
#include <bl/string.h>
#include <bl/utils.h>

/* Leave this undocument */
#ifndef DOX_SKIP

/* From bootstrap.asm */
extern char   _heap;
extern word_t _heap_size;

/* Allocator info */
static struct {
  void*  start;
  size_t max_alloc;
} _mem_ctx;

/* Block header */
typedef struct __align(16) _block_hdr {
  bool               free;
  size_t             size;
  struct _block_hdr* next;
  struct _block_hdr* prev;
}

_block_hdr;

static size_t __inline__ _align16(size_t val) { return (val + 15) & -16; }

static void _allocate_block(_block_hdr* after, size_t size) {
  _block_hdr* new_block =
      (_block_hdr*)((byte_t*)after + after->size + sizeof(_block_hdr));
  new_block->free       = true;
  new_block->size       = size;

  new_block->prev       = after;
  new_block->next       = after->next;

  new_block->prev->next = new_block;
  if (new_block->next != NULL) {
    new_block->next->prev = new_block;
  }
}

static void _find_max_block(void) {
  _block_hdr* block;

  _mem_ctx.max_alloc = 0;
  for (block = (_block_hdr*)_mem_ctx.start; block != NULL;
       block = block->next) {
    if (block->size > _mem_ctx.max_alloc) {
      _mem_ctx.max_alloc = block->size;
    }
  }
}

static _block_hdr* _merge_blocks(_block_hdr* up, _block_hdr* down) {
  _block_hdr* next  = down->next;

  up->size         += sizeof(_block_hdr) + down->size;

  up->next          = down->next;
  if (next != NULL) {
    next->prev = down->prev;
  }

  return up;
}

#endif /* DOX_SKIP */

bool mem_init(void) {
  _block_hdr* initial_block;

  _mem_ctx.start      = (void*)_align16((size_t)&_heap);

  initial_block       = (_block_hdr*)_mem_ctx.start;
  initial_block->free = true;
  initial_block->size = _heap_size -
                        ((ptrdiff_t)_mem_ctx.start - (ptrdiff_t)&_heap) -
                        sizeof(_block_hdr);
  initial_block->next = NULL;
  initial_block->prev = NULL;

  _mem_ctx.max_alloc  = initial_block->size;

  return true;
}

void* malloc(size_t count) {
  _block_hdr* free_block;
  bool        block_found;
  size_t      block_left;

  /* Align bytes count */
  count = _align16(count);
  if (count > _mem_ctx.max_alloc) {
    return NULL;
  }

  /* Find suitable free block */
  block_found = false;
  for (free_block = (_block_hdr*)_mem_ctx.start; free_block != NULL;
       free_block = free_block->next) {
    if (free_block->free && free_block->size >= count) {
      block_found = true;
      break;
    }
  }
  if (!block_found) {
    return NULL;
  }

  /* Try to allocate new free block */
  block_left       = free_block->size - count;
  free_block->free = false;
  free_block->size = count;
  if (block_left < sizeof(_block_hdr) + 16) {
    free_block->size += block_left;
  } else {
    _allocate_block(free_block, block_left - sizeof(_block_hdr));
  }

  /* Find new max block */
  _find_max_block();

  return (byte_t*)free_block + sizeof(_block_hdr);
}

void* realloc(void* mem, size_t new_size) {
  _block_hdr* block = (_block_hdr*)((byte_t*)mem - sizeof(_block_hdr));
  new_size          = _align16(new_size);

  if (block->size > new_size) {
    /* Decrease block size */
    size_t block_left = block->size - new_size;
    block->size       = new_size;
    if (block_left < sizeof(_block_hdr) + 16) {
      /* Cannot decrease block size */
      block->size += block_left;
      return mem;
    } else {
      /* Allocate new block from decreased size */
      _allocate_block(block, block_left - sizeof(_block_hdr));

      /* Try to merge new block with it's next block */
      if (block->next->next && block->next->next->free) {
        _merge_blocks(block->next, block->next->next);
      }

      /* Find new max block */
      _find_max_block();
      return mem;
    }

  } else if (block->size < new_size) {
    /* Allocate new memory */
    void* new_mem;
    if ((new_mem = malloc(new_size)) == NULL) {
      /* Failed to allocate new memory */
      return NULL;
    }

    /* Copy data to new memory */
    memcpy(new_mem, mem, block->size);

    /* Free old memory */
    free(mem);

    return new_mem;
  } else {
    /* No reallocation needed */
    return mem;
  }
}

void free(void* mem) {
  _block_hdr* block = (_block_hdr*)((byte_t*)mem - sizeof(_block_hdr));

  /* Mark block as free*/
  block->free       = true;

  /* Try to merge free blocks */
  if (block->prev != NULL && block->prev->free) {
    block = _merge_blocks(block->prev, block);
  }
  if (block->next != NULL && block->next->free) {
    block = _merge_blocks(block, block->next);
  }

  /* Update max alloc size */
  if (block->size > _mem_ctx.max_alloc) {
    _mem_ctx.max_alloc = block->size;
  }
}

memory_map* get_memory_map(void) {
  memory_map*      mem_map;
  memory_map_node* node;
  dword_t          offset;

  /* Allocate struct */
  if ((mem_map = malloc(sizeof(memory_map))) == NULL) {
    return NULL;
  }

  /* Allocate first node */
  if ((node = malloc(sizeof(memory_map_node))) == NULL) {
    free(mem_map);
    return NULL;
  }
  memset(node, 0, sizeof(memory_map_node));
  mem_map->list  = node;
  mem_map->count = 1;

  /* Get memory map */
  offset         = 0;
  do {
    if (!bios_get_e820(&offset, sizeof(memory_map_entry), &node->entry)) {
      while (node->prev) {
        node = node->prev;
        free(node->next);
      }
      free(node);
      free(mem_map);
      return NULL;
    }
    if (offset != 0) {
      if ((node->next = malloc(sizeof(memory_map_node))) == NULL) {
        while (node->prev) {
          node = node->prev;
          free(node->next);
        }
        free(node);
        free(mem_map);
        return NULL;
      }
      memset(node->next, 0, sizeof(memory_map_node));
      node->next->prev = node;
      node             = node->next;
      ++mem_map->count;
    }
  } while (offset != 0);

  return mem_map;
}

/* Leave this undocument */
#ifndef DOX_SKIP
#  ifndef NDEBUG
void _dump_heap(void) {
  size_t      block_index = 0;
  _block_hdr* block;

  serial_printf(
      "HEAP DUMP:\n"
      "Start = %p\n"
      "Max alloc = %zu bytes\n"
      "Header size = %zu\n",
      _mem_ctx.start,
      _mem_ctx.max_alloc,
      sizeof(_block_hdr)
  );

  for (block = (_block_hdr*)_mem_ctx.start; block != NULL;
       block = block->next) {
    serial_printf(
        "Block %zu:\n"
        "    Free = %d\n"
        "    Block address = %p\n"
        "    Data address = %p\n"
        "    Size = %zu bytes\n"
        "    Prev = %p\n"
        "    Next = %p\n",
        block_index++,
        block->free,
        (void*)block,
        (void*)((byte_t*)block + sizeof(_block_hdr)),
        block->size,
        (void*)block->prev,
        (void*)block->next
    );
  }
}

void _dump_memory_map(memory_map* mem_map) {
  memory_map_node* node = mem_map->list;
  while (node) {
    serial_printf(
        "Base address = %#.16llx, Limit = %#.16llx, Type = %d, ACPI = %d\n",
        node->entry.base,
        node->entry.limit,
        node->entry.type,
        node->entry.ACPI
    );
    node = node->next;
  }
}

#  endif /* NDEBUG */
#endif   /* DOX_SKIP */
