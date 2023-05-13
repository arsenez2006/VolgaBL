#include <bl/mem.h>
#include <bl/io.h>
#include <bl/string.h>

extern char _heap;
extern word_t _heap_size;

static struct {
    void* start;
    size_t max_alloc;
    size_t hdr_aligned_size;
} _mem_ctx;

typedef struct __block_hdr {
    bool free;
    size_t size;
    struct __block_hdr* next;
    struct __block_hdr* prev;
} _block_hdr;

static size_t _align16(size_t val) {
    return ((((uintptr_t)val - 1) / 16) * 16) + 16;
}

static void _allocate_block(_block_hdr* after, size_t size) {

    _block_hdr* new_block = (_block_hdr*)((byte_t*)after + after->size + _mem_ctx.hdr_aligned_size);
    new_block->free = true;
    new_block->size = size;

    new_block->prev = after;
    new_block->next = after->next;

    new_block->prev->next = new_block;
    if (new_block->next != NULL) {
        new_block->next->prev = new_block;
    }
}

static void _find_max_block() {
    _block_hdr* block;    

    _mem_ctx.max_alloc = 0;
    for (block = (_block_hdr*)_mem_ctx.start; block != NULL; block = block->next) {
        if (block->size > _mem_ctx.max_alloc) {
            _mem_ctx.max_alloc = block->size;
        }
    }
}

static _block_hdr* _merge_blocks(_block_hdr* up, _block_hdr* down) {
    _block_hdr* next = down->next;
    
    up->size += _mem_ctx.hdr_aligned_size + down->size;

    up->next = down->next;
    if (next != NULL) {
        next->prev = down->prev;
    }

    return up;
}

bool mem_init() {
    _mem_ctx.start = (void*)_align16((size_t)&_heap);
    _mem_ctx.hdr_aligned_size = _align16(sizeof(_block_hdr));

    _block_hdr* initial_block = (_block_hdr*)_mem_ctx.start;
    initial_block->free = true;
    initial_block->size = _heap_size - ((ptrdiff_t)_mem_ctx.start - (ptrdiff_t)&_heap) - _mem_ctx.hdr_aligned_size;
    initial_block->next = NULL;
    initial_block->prev = NULL;

    _mem_ctx.max_alloc = initial_block->size;

    return true;
}

void* malloc(size_t count) {
    _block_hdr* free_block;
    bool block_found;

    /* Align bytes count */
    count = _align16(count);
    if (count > _mem_ctx.max_alloc) {
        return NULL;
    }

    /* Find suitable free block */
    block_found = false;
    for (free_block = (_block_hdr*)_mem_ctx.start; free_block != NULL; free_block = free_block->next) {
        if (free_block->free && free_block->size >= count) {
            block_found = true;
            break;
        }
    }
    if (!block_found) {
        return NULL;
    }

    /* Try to allocate new free block */
    size_t block_left = free_block->size - count;
    free_block->free = false;
    free_block->size = count;
    if (block_left < _mem_ctx.hdr_aligned_size + 16) {
        free_block->size += block_left;
    } else {
        _allocate_block(free_block, block_left - _mem_ctx.hdr_aligned_size);
    }

    /* Find new max block */
    _find_max_block();
    
    return (byte_t*)free_block + _mem_ctx.hdr_aligned_size;
}

void* realloc(void* mem, size_t new_size) {
    _block_hdr* block = (_block_hdr*)((byte_t*)mem - _mem_ctx.hdr_aligned_size);
    new_size = _align16(new_size);

    if (block->size > new_size) {
        /* Decrease block size */
        size_t block_left = block->size - new_size;
        block->size = new_size;
        if (block_left < _mem_ctx.hdr_aligned_size + 16) {
            /* Cannot decrease block size */
            block->size += block_left;
            return mem;
        } else {
            /* Allocate new block from decreased size */
            _allocate_block(block, block_left - _mem_ctx.hdr_aligned_size);

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

void free(void *mem) {
    _block_hdr* block = (_block_hdr*)((byte_t*)mem - _mem_ctx.hdr_aligned_size);
    
    /* Mark block as free*/
    block->free = true;

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

void mem_dump() {
    size_t block_index = 0;
    _block_hdr* block;
    
    printf(
        "Start = %.4p\n"
        "Max alloc = %zu bytes\n"
        "Header size = %zu\n",
        _mem_ctx.start,
        _mem_ctx.max_alloc,
        _mem_ctx.hdr_aligned_size
    );

    for (block = (_block_hdr*)_mem_ctx.start; block != NULL; block = block->next) {
        printf(
            "Block %zu:\n"
            "    Free = %d\n"
            "    Block address = %.4p\n"
            "    Data address = %.4p\n"
            "    Size = %zu bytes\n"
            "    Prev = %.4p\n"
            "    Next = %.4p\n",
            block_index++,
            block->free,
            block,
            (byte_t*)block + _mem_ctx.hdr_aligned_size,
            block->size,
            block->prev,
            block->next
        );
    }
}
