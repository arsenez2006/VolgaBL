#include <bl/defines.h>
#include <bl/types.h>

/**
 * @brief Third Stage Loader entry
 *
 * @details `Third Stage Loader` works in Protected Mode. It's purpose is getting system ready for
 *          loading kernel\n
 *
 */
void __noreturn
tsl_entry(void) {
    while (1) {
        __asm__ volatile("hlt");
    }
}
