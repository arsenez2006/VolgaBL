/**
 * @file tsl_entry.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Third Stage Loader entry
 *
 */
#include <bl/defines.h>
#include <bl/io.h>
#include <bl/types.h>
#include <bl/utils.h>

/**
 * @brief Third Stage Loader entry
 *
 * @details `Third Stage Loader` works in Protected Mode. It's purpose is
 * getting system ready for loading kernel\n
 *
 */

static void print_error(const char* error_str) {
    /* TODO: Implement graphics printf */
    serial_printf("VLGBL Error: %s.\n", error_str);
}

void __noreturn
tsl_entry(void) {
    if (!check_cpuid()) {
        print_error("CPUID is not presented");
        goto halt;
    }

halt:
    while (1) {
        __asm__ volatile("hlt");
    }
}
