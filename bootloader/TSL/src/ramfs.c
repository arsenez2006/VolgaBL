/**
 * @file ramfs.c
 * @author Arseny Lashkevicj (arsenez@cybercommunity.space)
 * @brief Functions for reading RAMFS
 *
 */

#include <bl/ramfs.h>
#include <bl/string.h>

static struct {
  void*  addr;
  size_t size;
} _ctx;

typedef struct posix_header {
  char name[100];
  char _unused0[24];
  char size[12];
  char _unused1[121];
  char magic[6];
  char _unused2[249];
} posix_header;

static size_t _str_oct_to_dec(char const* oct) {
  size_t ret = 0;
  while (*oct != 0) { ret = ret * 8 + *oct++ - '0'; }
  return ret;
}

static size_t _align512(size_t val) {
  return val == 0 ? 0 : ((((int)val - 1) / 512) * 512) + 512;
}

static posix_header* _next(posix_header* hdr) {
  return (posix_header*)((byte_t*)(hdr + 1) +
                         _align512(_str_oct_to_dec(hdr->size)));
}

bool ramfs_init(void* address) {
  posix_header* i;
  /* Set RAMFS base address */
  _ctx.addr = address;

  /* Get RAMFS size */
  for (i = _ctx.addr; memcmp("ustar", i->magic, 5) == 0; i = _next(i))
    ;
  _ctx.size = (char*)i - (char*)_ctx.addr;

  return true;
}

void* ramfs_get_end(void) { return (byte_t*)_ctx.addr + _ctx.size; }

void* ramfs_file(char const* name, size_t* size) {
  posix_header* hdr;
  for (hdr = _ctx.addr; memcmp("ustar", hdr->magic, 5) == 0; hdr = _next(hdr)) {
    if (memcmp(hdr->name, name, strlen(name)) == 0) {
      if (size != NULL) {
        *size = _str_oct_to_dec(hdr->size);
      }
      return hdr + 1;
    }
  }

  return NULL;
}
