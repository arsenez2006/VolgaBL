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

/* TODO: Implement graphics printf */
#define printf serial_printf

/**
 * @brief Print error message to screen
 *
 * @param error_str Error message
 */
static void
print_error(const char* error_str) {
    printf("VLGBL Error: %s.\n", error_str);
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
    dword_t cpuid_max, cpuid_ext_max;
    const char* cpu_vendor;

    /* Verify boot info size */
    if (boot_info->size != sizeof(boot_info_t)) {
        print_error("Boot info is corrupted");
        goto halt;
    }

    /* Check CPUID presence */
    if (!check_cpuid()) {
        print_error("CPUID is not presented");
        goto halt;
    }

    /* Get CPU info */
    cpu_vendor = get_cpu_info(&cpuid_max, &cpuid_ext_max);
    printf("CPU vendor = %s, CPUID max = %#x, CPUID ext max = %#x\n",
           cpu_vendor,
           cpuid_max,
           cpuid_ext_max);

halt:
    while (1) {
        __asm__ volatile("hlt");
    }
}
