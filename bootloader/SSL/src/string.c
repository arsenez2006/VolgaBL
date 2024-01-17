/**
 * @file string.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Functions for manipulating c-strings and buffers
 *
 */
#include <bl/string.h>

size_t strlen(char const* start) {
  char const* end;
  for (end = start; *end != '\0'; ++end)
    ;
  return end - start;
}

int memcmp(void const* lhs, void const* rhs, size_t count) {
  while (count--) {
    if (*(byte_t*)lhs != *(byte_t*)rhs) {
      return (uintptr_t)lhs - (uintptr_t)rhs;
    }
    lhs = (byte_t*)lhs + 1;
    rhs = (byte_t*)rhs + 1;
  }
  return 0;
}

void* memcpy(void* dest, void const* src, size_t count) {
  size_t i;
  for (i = 0; i < count; ++i) { ((byte_t*)dest)[i] = ((byte_t*)src)[i]; }
  return dest;
}

void* memset(void* ptr, int val, size_t count) {
  byte_t* data = (byte_t*)ptr;
  while (count--) { *data++ = (byte_t)val; }
  return ptr;
}
