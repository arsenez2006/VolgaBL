#include <bl/defines.h>
#include <bl/types.h>
#include <bl/bios.h>
#include <bl/io.h>

void __noreturn ssl_entry(void) {
    /* Print loading message*/
    printf("Loading VLGBL...\n");

    /* Initialize COM port */
    (void)bios_serial_init();
    
    while(1) {
        __asm__ volatile("hlt");
    }
}
