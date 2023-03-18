#include <bl/types.h>
#include <bl/bios.h>
#include <bl/io.h>

int debug_var = 0;

void ssl_entry() {
    const char* test = "formatted string";
    printf("format string\n%020s\nNumber1: % .5d\nNumber2: % .5x\nNumber3: % "
           ".5X\nNumber4: % .5o\nCharacter: %05c\nssl_entry: %.4p\n",
           test, 228, 0xdead, 0xC0DE, 1337, 'A', ssl_entry);
    return;
}
