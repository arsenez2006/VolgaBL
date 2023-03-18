#include <bl/types.h>
#include <bl/bios.h>

const char* msg = "Hello, world!\r\n"; 
void ssl_entry()
{
    while (*msg != '\0')
        bios_putch(*msg++);
    return;
}
