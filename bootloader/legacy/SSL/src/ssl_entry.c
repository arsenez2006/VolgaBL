#include <bl/types.h>
#include <bl/bios.h>
#include <bl/string.h>
#include <bl/io.h>
#include <bl/utils.h>
#include <bl/mem.h>

static int print_error(const char* error_str) {
    return printf("VLGBL Error: %s.", error_str);
}

/* GDT for Protected mode */
static GDTR48 gdtr_pm;
static GDT_entry gdt_pm[3];

word_t ssl_entry() {
    /* Print loading message*/
    printf("Loading VLGBL...\n");

    /* Initialize CRC32 polynom table */
    init_crc32();

    /* Null descriptor */
    set_GDT_entry(&gdt_pm[0], 0, 0, 0, 0, 0, 0);

    /* Protected mode 32bit code segment descriptor */
    set_GDT_entry(
        &gdt_pm[1], 
        0x0000, 0x0000, 
        0xFFFF, 0xFFFF, 
        GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE, 
        GDT_ACCESS_PRESENT | GDT_ACCESS_DPL0 | GDT_ACCESS_SEGMENT | GDT_ACCESS_EXECUTABLE | GDT_ACCESS_READABLE
    );

    /* Protected mode 32bit data segment descriptor */
    set_GDT_entry(
        &gdt_pm[2], 
        0x0000, 0x0000, 
        0xFFFF, 0xFFFF, 
        GDT_FLAG_GRANULARITY | GDT_FLAG_SIZE, 
        GDT_ACCESS_PRESENT | GDT_ACCESS_DPL0 | GDT_ACCESS_SEGMENT | GDT_ACCESS_WRITEABLE
    );

    /* Fill Protected mode GDTR */
    set_GDTR48(&gdtr_pm, gdt_pm, sizeof(gdt_pm));

    /* Load Protected mode GDT */
    load_GDT(gdtr_pm);

    /* Enter Unreal mode */
    enter_unreal(2 * sizeof(GDT_entry));

    /* Initialize allocator */
    if(!mem_init()) {
        print_error("Failed to initialize allocator");
        return 1;
    }

    /* Check drive logical sector size */
    drive_parameteres drive_params;
    drive_params.size = sizeof(drive_parameteres);
    if(!bios_get_drive_parameteres(&drive_params)) {
        print_error("Failed to get drive paramteres");
        return 1;
    }
    if (drive_params.sector_size != 512) {
        print_error("Wrong sector size, aborting");
        return 1;
    }

    /* Read GPT header */
    GPT_header* gpt_hdr;
    if ((gpt_hdr = malloc(512)) == NULL) {
        print_error("Failed to read GPT header.");
        return 1;
    }

    DAP read_context;
    read_context.size = sizeof(DAP);
    read_context.rsv = 0;

    read_context.sectors = 1;
    read_context.segment = get_ds();
    read_context.offset = (uintptr_t)gpt_hdr;
    assign_word_to_qword(read_context.lba, 1);

    if(!bios_read_drive(&read_context)) {
        print_error("Failed to read GPT header.");
        return 1;
    }

    /* Check GPT Signature */
    if(memcmp(gpt_hdr->magic, "EFI PART", 8) != 0) {
        print_error("Drive is not GPT");
    }

    /* Compare GPT checksum */
    dword_t gpt_crc32_copy = gpt_hdr->crc32;
    zero_dword(gpt_hdr->crc32);
    dword_t gpt_crc32;
    crc32(gpt_hdr, sizeof(GPT_header), &gpt_crc32);
    sub_dwords(gpt_crc32, gpt_crc32_copy, gpt_crc32);
    if(!dword_is_zero(gpt_crc32)) {
        print_error("GPT header is corrupted. CRC32 mismatch");
        return 1;
    }

    /* Save some information from GPT header */
    word_t partition_entry_size;
    assign_dword_to_word(partition_entry_size, gpt_hdr->entry_size);
    byte_t disk_guid[16];
    memcpy(disk_guid, gpt_hdr->guid, 16);

    /* Read partition table */
    dword_t partition_table_size;
    dword_t tmp;
    mul_dwords(gpt_hdr->entries_count, gpt_hdr->entry_size, partition_table_size);
    assign_word_to_dword(tmp, 512);
    div_dwords(partition_table_size, tmp, tmp);
    assign_dword_to_word(read_context.sectors, tmp);
    assign_word_to_dword(tmp, 512);
    mod_dwords(partition_table_size, tmp, tmp);
    if (!dword_is_zero(tmp)) {
        ++read_context.sectors;
    }
    read_context.lba = gpt_hdr->partition_array;
    free(gpt_hdr);

    GPT_partition_entry* partition_entry;
    if ((partition_entry = malloc(read_context.sectors * 512)) == NULL) {
        print_error("Failed to read partition table");
        return 1;
    }
    read_context.offset = (uintptr_t)partition_entry;
    if(!bios_read_drive(&read_context)) {
        print_error("Failed to read partition table");
        return 1;
    }
    

    mem_dump();

    return 0;
}
