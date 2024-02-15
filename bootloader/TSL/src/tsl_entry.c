/**
 * @file tsl_entry.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Third Stage Loader entry
 *
 */
#include <bl/defines.h>
#include <bl/io.h>
#include <bl/pe.h>
#include <bl/ramfs.h>
#include <bl/string.h>
#include <bl/types.h>
#include <bl/utils.h>

static qword_t pml4[512] __align(1 << 12);
static qword_t pml3[512] __align(1 << 12);
static qword_t pml2[512] __align(1 << 12);
static qword_t pml1[512] __align(1 << 12);

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
  size_t         i;
  pe_load_state* kernel;
  dword_t        pe_memory_end;

  /* Verify boot info size */
  if (boot_info->size != sizeof(boot_info_t)) {
    print_error("Boot info is outdated");
    goto halt;
  }

  /* Disable all PCI devices */
  disable_pci();

  /* Initialize RAMFS driver */
  if (!ramfs_init((void*)(uintptr_t)boot_info->RAMFS.address)) {
    print_error("Failed to initialize RAMFS");
    goto halt;
  }

  /* Initialize PE loader */
  if (!pe_loader_init((void*)align_page((dword_t)ramfs_get_end()))) {
    print_error("Failed to initialize PE loader");
    goto halt;
  }

  /* Load kernel image */
  if (!pe_load("ramfs/kernel.pe", &kernel)) {
    print_error("Failed to load kernel image");
    goto halt;
  }

  /* Enable Physical Address Extension */
  enable_PAE();

  /* Clear page tables */
  memset(pml4, 0, sizeof pml4);
  memset(pml3, 0, sizeof pml3);
  memset(pml2, 0, sizeof pml2);
  memset(pml1, 0, sizeof pml1);

  /* Identity map SSL */
  for (i = 0; i < 16; ++i) {
    qword_t addr     = 0x10000 + (i << 12);
    pml1[addr >> 12] = addr | 0x1;
  }

  /* Identity map TSL */
  for (i = 0; i < 16; ++i) {
    qword_t addr     = 0x20000 + (i << 12);
    pml1[addr >> 12] = addr | 0x1;
  }

  /* Identity map RAMFS and kernel image */
  pe_get_memory_range(NULL, &pe_memory_end);
  pe_memory_end = align_page(pe_memory_end);
  for (i = boot_info->RAMFS.address >> 12; i < pe_memory_end >> 12; ++i) {
    pml1[i] = (i << 12) | 0x1;
  }

  /* Identity map temporary stack */
  for (i = 0; i < align_page(kernel->stack_size) >> 12; ++i) {
    qword_t addr     = pe_memory_end + (i << 12);
    pml1[addr >> 12] = addr | 0x1;
  }

  pml2[0] = (qword_t)(uintptr_t)pml1 | 0x3;
  pml3[0] = (qword_t)(uintptr_t)pml2 | 0x3;
  pml4[0] = (qword_t)(uintptr_t)pml3 | 0x3;

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

  /* Load page table */
  load_page_table(pml4);

  /* Enable Long Mode */
  enable_long_mode();

  /* Enable Paging */
  enable_paging();

  /* Finalize TSL */
  __asm__ volatile(
      "pushw %[segment]\n"
      "pushl %[offset]\n"
      "movl %%esp, %%ebp\n"

      "movl %[tmp_stack], %%esp\n"
      "movl %[bootinfo], %%ecx\n"

      "lcall *(%%ebp)"
      :
      : [segment] "rmN"((word_t)3 << 3),
        [offset] "rm"((dword_t)kernel->entry),
        [bootinfo] "rmN"((dword_t)boot_info),
        [tmp_stack] "rmN"((dword_t)pe_memory_end + kernel->stack_size)
  );

halt:
  while (1) { __asm__ volatile("hlt"); }
}
