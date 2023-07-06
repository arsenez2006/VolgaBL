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
 * @brief Print error message to screen
 *
 * @param error_str Error message
 */
static void
print_error(const char* error_str) {
    /* TODO: Implement graphics printf */
    serial_printf("VLGBL Error: %s.\n", error_str);
}

/**
 * @brief Third Stage Loader entry
 *
 * @details `Third Stage Loader` works in Protected Mode. It's purpose is
 * getting system ready for loading kernel\n
 *
 */
void __stdcall __noreturn
tsl_entry(boot_info_t* boot_info) {
    if (!check_cpuid()) {
        print_error("CPUID is not presented");
        goto halt;
    }

    serial_printf("boot info memory map count = %d\n",
                  boot_info->memory_map.count);

halt:
    while (1) {
        __asm__ volatile("hlt");
    }
}
