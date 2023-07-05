/**
 * @file tsl_entry.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Third Stage Loader entry
 *
 */
#include <bl/defines.h>
#include <bl/types.h>
#include <bl/utils.h>

/**
 * @brief Third Stage Loader entry
 *
 * @details `Third Stage Loader` works in Protected Mode. It's purpose is
 * getting system ready for loading kernel\n
 *
 */
void __noreturn
tsl_entry(void) {

    serial_putch('A');

    while (1) {
        __asm__ volatile("hlt");
    }
}
