/**
 * @file tsl_entry.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Third Stage Loader entry
 *
 */
#include <bl/defines.h>
#include <bl/io.h>
#include <bl/ramfs.h>
#include <bl/string.h>
#include <bl/types.h>
#include <bl/utils.h>

static qword_t pml4[512] __align(1 << 12);
static qword_t pml3[512] __align(1 << 12);

/**
 * @brief Print error message to screen
 *
 * @param error_str Error message
 */
static void    print_error(char const* error_str) {
  serial_printf("VLGBL Error: %s.\n", error_str);
}

/**
 * @brief Third Stage Loader entry
 *
 * @details `Third Stage Loader` works in Protected Mode. It's purpose is
 * getting system ready for loading kernel\n
 *
 */
void __stdcall __noreturn tsl_entry(boot_info_t* boot_info) {
  size_t i;
  void*  kernel_image;
  size_t kernel_image_size;

  /* Verify boot info size */
  if (boot_info->size != sizeof(boot_info_t)) {
    print_error("Boot info is outdated");
    goto halt;
  }

  /* Initialize RAMFS driver */
  if (!ramfs_init((void*)(uintptr_t)boot_info->RAMFS.address)) {
    print_error("Failed to initialize RAMFS");
    goto halt;
  }

  /* Find kernel image */
  if ((kernel_image = ramfs_file("ramfs/kernel.pe", &kernel_image_size)) ==
      NULL) {
    print_error("Cannot find ramfs/kernel.pe");
    goto halt;
  }

  serial_printf("%p, %d\n", kernel_image, kernel_image_size);

  /* Enable Physical Address Extension */
  enable_PAE();

  /* Clear page tables */
  memset(pml4, 0, sizeof pml4);
  memset(pml3, 0, sizeof pml3);

  /* Identity map first 4GB of RAM */
  for (i = 0; i < 4; ++i) { pml3[i] = ((qword_t)i << 30) | 0x83; }
  pml4[0] = (qword_t)(uintptr_t)pml3 | 0x3;

  /* Load page table */
  load_page_table(pml4);

  /* Enable Long Mode */
  enable_long_mode();

  /* Enable Paging */
  enable_paging();

  /* Disable all PCI devices */
  disable_pci();

  /* Find ACPI RSDP table*/
  for (i = 0xE0000; i < 0xFFFFF; ++i) {
    if (!memcmp((void*)i, "RSD PTR ", 8)) {
      break;
    }
  }
  if (i == 0xFFFFF) {
    print_error("Failed to find ACPI tables");
  }
  boot_info->ACPI.rsdp = i;

halt:
  while (1) { __asm__ volatile("hlt"); }
}
