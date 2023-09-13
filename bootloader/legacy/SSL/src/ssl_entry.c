/**
 * @file ssl_entry.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Second Stage Loader entry
 *
 */
#include <bl/bios.h>
#include <bl/defines.h>
#include <bl/io.h>
#include <bl/mem.h>
#include <bl/string.h>
#include <bl/types.h>
#include <bl/utils.h>

/* GDT for Protected mode */
static GDTR32 gdtr_pm;
static GDT32_entry gdt_pm[3];

/* GUID of booted drive */
static byte_t drive_GUID[16];

/* VolgaOS partition types */
static const byte_t kernel_partition_type[] = {
    0x98, 0xE5, 0xA9, 0x78, 0x38, 0x36, 0x67, 0x4D,
    0xB2, 0xEB, 0x01, 0x23, 0xD0, 0xAF, 0xBD, 0xBD
}; /* 78A9E598-3638-4D67-B2EB-0123D0AFBDBD */
static const byte_t tsl_partition_type[] = {
    0xC7, 0x0D, 0x6D, 0x87, 0x66, 0xCF, 0x63, 0x4C,
    0xBC, 0xEE, 0xBD, 0x79, 0xEE, 0x10, 0xF5, 0x93
}; /* 876D0DC7-CF66-4C63-BCEE-BD79EE10F593 */

/**
 * @brief Print error message to screen
 *
 * @param error_str Error message
 */
static void
print_error(const char* error_str) {
    printf("VLGBL Error: %s\n", error_str);
    serial_printf("VLGBL Error: %s\n", error_str);
}

/**
 * @brief Second Stage Loader entry
 *
 * @details `Second Stage Loader` (SSL) works in Real Mode, so it can use BIOS
 * interface.\n SSL goals are:\n
 *          - Load `Third Stage Loader` and kernel partitions to memory
 *          - Get GUID of the booted drive
 *          - Get memory map
 *          - Get video modes
 *          - // TODO: Enable graphics mode
 *          - Enable A20
 *          - Enable Protected Mode
 *          - Run `Third Stage Loader`
 */
void __noreturn
ssl_entry(void) {
    boot_info_t* boot_info;

    memory_map* mem_map;

    GPT_header* gpt_hdr;
    GPT_partition_array* partition_array;
    dword_t gpt_crc32, gpt_crc32_copy;

    GPT_partition_entry* tsl_partition;
    GPT_partition_entry* kernel_partition;

    DAP read_context;
    drive_parameteres drive_params;

    read_context.size = sizeof(DAP);
    read_context.rsv = 0;

    /* Print loading message */
    (void)printf("Loading VLGBL...\n");

    /* Initialize COM port */
    (void)bios_serial_init();

    /* Enable A20 line */
    if (!enable_A20()) {
        print_error("Failed to enable A20");
        goto halt;
    }

    /* Null descriptor */
    set_GDT32_entry(&gdt_pm[0], 0, 0, 0, 0);

    /* Protected mode 32bit code segment descriptor */
    set_GDT32_entry(&gdt_pm[1],
                    0x00000000,
                    0xFFFFFFFF,
                    GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE,
                    GDT_ACCESS_PRESENT | GDT_ACCESS_DPL0 | GDT_ACCESS_SEGMENT |
                      GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READABLE);

    /* Protected mode 32bit data segment descriptor */
    set_GDT32_entry(&gdt_pm[2],
                    0x00000000,
                    0xFFFFFFFF,
                    GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE,
                    GDT_ACCESS_PRESENT | GDT_ACCESS_DPL0 | GDT_ACCESS_SEGMENT |
                      GDT_ACCESS_WRITEABLE);

    /* Fill Protected mode GDTR */
    set_GDTR32(&gdtr_pm, gdt_pm, 3);

    /* Load Protected mode GDT */
    load_GDT32(gdtr_pm);

    /* Enable Unreal Mode */
    enter_unreal(2 * sizeof(GDT32_entry));

    /* Initialize allocator */
    if (!mem_init()) {
        print_error("Failed to initialize allocator");
        goto halt;
    }

    /* Get memory map */
    if ((mem_map = get_memory_map()) == NULL) {
        print_error("Failed to get memory map");
        goto halt;
    }

    /* Check drive logical sector size */
    drive_params.size = sizeof(drive_parameteres);
    if (!bios_get_drive_parameteres(&drive_params)) {
        print_error("Failed to get drive paramteres");
        goto halt;
    }
    if (drive_params.sector_size != SECTOR_SIZE) {
        print_error("Wrong sector size, aborting");
        goto halt;
    }

    /* Read GPT header */
    if ((gpt_hdr = malloc(SECTOR_SIZE)) ==
        NULL) { /* Allocate enough space for reading a sector */
        print_error("Failed to read GPT header.");
        goto halt;
    }

    read_context.sectors = 1;
    read_context.segment = get_ds();
    read_context.offset = (word_t)((uintptr_t)gpt_hdr & 0xFFFF);
    read_context.lba = 1; /* GPT header always located at LBA 1 */

    if (!bios_read_drive(&read_context)) {
        print_error("Failed to read GPT header.");
        goto halt;
    }
    if ((gpt_hdr = realloc(gpt_hdr, sizeof(GPT_header))) ==
        NULL) { /* GPT header size is less than sector size, so we should
                   reallocate it to reduce use of memory */
        print_error("Failed to read GPT header.");
        goto halt;
    }

    /* Check GPT Signature */
    if (memcmp(gpt_hdr->magic, "EFI PART", 8) != 0) {
        print_error("Drive is not GPT");
        goto halt;
    }

    /* Compare GPT checksum */
    gpt_crc32_copy = gpt_hdr->crc32;
    gpt_hdr->crc32 =
      0; /* GPT CRC32 checksum must be counted with CRC32 field being zero */
    gpt_crc32 = crc32((byte_t*)gpt_hdr, sizeof(GPT_header));
    if (gpt_crc32 != gpt_crc32_copy) {
        print_error("GPT header is corrupted. CRC32 mismatch");
        goto halt;
    }

    /* Save drive GUID */
    (void)memcpy(drive_GUID, gpt_hdr->guid, 16);

    /* Get partition table */
    if ((partition_array = get_partition_array(gpt_hdr)) == NULL) {
        print_error("Failed to get partition table");
        goto halt;
    }

    /* Find Third Stage Loader partition */
    if ((tsl_partition = find_partition(partition_array, tsl_partition_type)) ==
        NULL) {
        print_error("Failed to find Third Stage Loader partition");
        goto halt;
    }

    /* Find kernel partition */
    if ((kernel_partition =
           find_partition(partition_array, kernel_partition_type)) == NULL) {
        print_error("Failed to find kernel partition");
        goto halt;
    }

    /* Load Third Stage Loader */
    read_context.segment = TSL_SEG;
    read_context.offset = 0x0000;
    read_context.sectors =
      tsl_partition->end_lba - tsl_partition->start_lba + 1;
    read_context.lba = tsl_partition->start_lba;
    if (!bios_read_drive(&read_context)) {
        print_error("Failed to load Third Stage Loader");
        goto halt;
    }

    /* Create boot info */
    if ((boot_info = create_boot_info(drive_GUID, mem_map)) == NULL) {
        print_error("Failed to create boot info");
        goto halt;
    }

    /* Used for debug */
    dump_heap();
    dump_memory_map(mem_map);

    /* Finalize Second Stage Loader */
    __asm__ volatile(
      "cli\n"
      "movb $0xFF, %%al\n" /* Disable PIC */
      "outb %%al, $0xA1\n"
      "outb %%al, $0x21\n"

      "movl %%cr0, %%eax\n" /* Set CR0 PE bit */
      "orb $1, %%al\n"
      "movl %%eax, %%cr0\n"

      "jmp .pm\n" /* Clear instruction pipeline */
      ".pm:\n"

      "movw %[data_seg], %%ax\n" /* Set 32bit segments */
      "movw %%ax, %%ds\n"
      "movw %%ax, %%es\n"
      "movw %%ax, %%ss\n"
      "movw %%ax, %%fs\n"
      "movw %%ax, %%gs\n"

      "xorl %%eax, %%eax\n" /* Fix stack pointer */
      "movw %%cs, %%ax\n"
      "shll $4, %%eax\n"
      "andl $0xFFFF, %%esp\n"
      "addl %%eax, %%esp\n"

      "movb $0xE9, (0x0000)\n" /* Make jump stub */
      "movl %[jmp_addr], (0x0001)\n"

      "pushl %[boot_info]\n"
      "ljmp %[code_seg],$0x0000" /* Jump to Third Stage Loader */
      :
      : [data_seg] "rmN"((word_t)2 * sizeof(GDT32_entry)),
        [jmp_addr] "rmN"((dword_t)TSL_ADDR - 5),
        [code_seg] "N"((word_t)1 * sizeof(GDT32_entry)),
        [boot_info] "rmN"((dword_t)boot_info + ((dword_t)get_ds() << 4))
      : "eax");

halt:
    while (1) {
        __asm__ volatile("hlt");
    }
}
