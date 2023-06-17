#include <bl/defines.h>
#include <bl/types.h>
#include <bl/bios.h>
#include <bl/io.h>
#include <bl/utils.h>
#include <bl/mem.h>
#include <bl/string.h>

/* GDT for Protected mode */
static GDTR32 gdtr_pm;
static GDT32_entry gdt_pm[3];

/* GUID of booted drive */
static byte_t drive_GUID[16];

/* VolgaOS partition types */
static const byte_t kernel_partition_type[] = 
{ 0x98, 0xE5, 0xA9, 0x78, 0x38, 0x36, 0x67, 0x4D, 0xB2, 0xEB, 0x01, 0x23, 0xD0, 0xAF, 0xBD, 0xBD }; /* 78A9E598-3638-4D67-B2EB-0123D0AFBDBD */ 
static const byte_t tsl_partition_type[] = 
{ 0xC7, 0x0D, 0x6D, 0x87, 0x66, 0xCF, 0x63, 0x4C, 0xBC, 0xEE, 0xBD, 0x79, 0xEE, 0x10, 0xF5, 0x93 }; /* 876D0DC7-CF66-4C63-BCEE-BD79EE10F593 */ 

static int print_error(const char *error_str) {
    return printf("VLGBL Error: %s\n", error_str);
}

void __noreturn ssl_entry(void) {
    memory_map* mem_map;
    drive_parameteres drive_params;
    GPT_header* gpt_hdr;
    DAP read_context;
    dword_t gpt_crc32, gpt_crc32_copy;

    /* Print loading message */
    printf("Loading VLGBL...\n");

    /* Initialize COM port */
    bios_serial_init();

    /* Null descriptor */
    set_GDT32_entry(&gdt_pm[0], 0, 0, 0, 0);
    
    /* Protected mode 32bit code segment descriptor */
    set_GDT32_entry(
        &gdt_pm[1], 
        0x00000000, 
        0xFFFFFFFF, 
        GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE, 
        GDT_ACCESS_PRESENT | GDT_ACCESS_DPL0 | GDT_ACCESS_SEGMENT | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READABLE
    );

    /* Protected mode 32bit data segment descriptor */
    set_GDT32_entry(
        &gdt_pm[2], 
        0x00000000, 
        0xFFFFFFFF, 
        GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE, 
        GDT_ACCESS_PRESENT | GDT_ACCESS_DPL0 | GDT_ACCESS_SEGMENT | GDT_ACCESS_WRITEABLE
    );

    /* Fill Protected mode GDTR */
    set_GDTR32(&gdtr_pm, gdt_pm, 3);

    load_GDT32(gdtr_pm);
    
    enter_unreal(2 * sizeof(GDT32_entry));

    /* Initialize allocator */
    if(!mem_init()) {
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
    if(!bios_get_drive_parameteres(&drive_params)) {
        print_error("Failed to get drive paramteres");
        goto halt;
    }
    if (drive_params.sector_size != SECTOR_SIZE) {
        print_error("Wrong sector size, aborting");
        goto halt;
    }

    /* Read GPT header */
    if ((gpt_hdr = malloc(SECTOR_SIZE)) == NULL) {
        print_error("Failed to read GPT header.");
        goto halt;
    }

    read_context.size = sizeof(DAP);
    read_context.rsv = 0;

    read_context.sectors = 1;
    read_context.segment = get_ds();
    read_context.offset = (word_t)((uintptr_t)gpt_hdr & 0xFFFF);
    read_context.lba = 1;

    if(!bios_read_drive(&read_context)) {
        print_error("Failed to read GPT header.");
        goto halt;
    }
    if ((gpt_hdr = realloc(gpt_hdr, sizeof(GPT_header))) == NULL) {
        print_error("Failed to read GPT header.");
        goto halt;
    }

    /* Check GPT Signature */
    if(memcmp(gpt_hdr->magic, "EFI PART", 8) != 0) {
        print_error("Drive is not GPT");
        goto halt;
    }

    /* Compare GPT checksum */
    gpt_crc32_copy = gpt_hdr->crc32;
    gpt_hdr->crc32 = 0;
    gpt_crc32 = crc32((byte_t*)gpt_hdr, sizeof(GPT_header));
    if (gpt_crc32 != gpt_crc32_copy) {
        print_error("GPT header is corrupted. CRC32 mismatch");
        goto halt;
    }

    /* Save drive GUID */
    memcpy(drive_GUID, gpt_hdr->guid, 16);

    dump_heap();
    dump_memory_map(mem_map);

halt:
    while(1) {
        __asm__ volatile("hlt");
    }
}
