#include <bl/string.h>

size_t strlen(const char *start)
{
    const char* end;
    for (end = start; *end != '\0'; ++end);
    return end - start;
}
