#include <bl/types.h>
#include <bl/bios.h>
#include <bl/string.h>
#include <bl/io.h>
#include <bl/utils.h>
#include <bl/mem.h>

/* GDT for Protected mode */
static GDTR48 gdtr_pm;
static GDT_entry gdt_pm[3];

static byte_t disk_GUID[16];

static const byte_t null_partition_type[] = 
{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
static const byte_t kernel_partition_type[] = 
{ 0x98, 0xE5, 0xA9, 0x78, 0x38, 0x36, 0x67, 0x4D, 0xB2, 0xEB, 0x01, 0x23, 0xD0, 0xAF, 0xBD, 0xBD }; // 78A9E598-3638-4D67-B2EB-0123D0AFBDBD
static const byte_t tsl_partition_type[] = 
{ 0xC7, 0x0D, 0x6D, 0x87, 0x66, 0xCF, 0x63, 0x4C, 0xBC, 0xEE, 0xBD, 0x79, 0xEE, 0x10, 0xF5, 0x93 }; // 876D0DC7-CF66-4C63-BCEE-BD79EE10F593

static int print_error(const char* error_str) {
    return printf("VLGBL Error: %s.", error_str);
}

static memory_map* get_memory_map() {
    /* Allocate struct */
    memory_map* mem_map;
    if ((mem_map = malloc(sizeof(memory_map))) == NULL) {
        return NULL;
    }

    /* Allocate first node */
    memory_map_node* node;
    if ((node = malloc(sizeof(memory_map_node))) == NULL) {
        free(mem_map);
        return NULL;
    }
    memset(node, 0, sizeof(memory_map_node));
    mem_map->list = node;
    mem_map->count = 1;

    /* Get memory map */
    dword_t offset;
    zero_dword(offset);
    do {
        if (!bios_get_e820(&offset, sizeof(memory_map_entry), &node->entry)) {
            while(node->prev) {
                node = node->prev;
                free(node->next);
            }
            free(node);
            free(mem_map);
            return NULL;
        }
        if (!dword_is_zero(offset)) {
            if((node->next = malloc(sizeof(memory_map_node))) == NULL) {
                while(node->prev) {
                    node = node->prev;
                    free(node->next);
                }
                free(node);
                free(mem_map);
                return NULL;
            }
            memset(node->next, 0, sizeof(memory_map_node));
            node->next->prev = node;
            node = node->next;
            ++mem_map->count;
        }
    } while(!dword_is_zero(offset));
    
    return mem_map;
}

static GPT_partition_array* get_partition_array(const GPT_header* gpt_hdr) {
    /* Allocate struct */
    GPT_partition_array* partition_array;
    if ((partition_array = malloc(sizeof(GPT_partition_array))) == NULL) {
        return NULL;
    }
    assign_dword_to_word(partition_array->entry_size, gpt_hdr->entry_size);

    /* Prepare DAP */
    DAP read_context;
    memset(&read_context, 0, sizeof(DAP));
    read_context.size = sizeof(DAP);
    read_context.segment = get_ds();

    /* Calculate size of array */
    dword_t partition_table_size;
    dword_t tmp;
    mul_dwords(gpt_hdr->entry_size, gpt_hdr->entries_count, partition_table_size);

    /* Calculate count of sectors to read */
    assign_word_to_dword(tmp, 512);
    div_dwords(partition_table_size, tmp, tmp);
    assign_dword_to_word(read_context.sectors, tmp);
    assign_word_to_dword(tmp, 512);
    mod_dwords(partition_table_size, tmp, tmp);
    if(!dword_is_zero(tmp)) {
        ++read_context.sectors;
    }

    /* Allocate enough space for reading partition array */
    if ((partition_array->array = malloc(read_context.sectors * 512)) == NULL) {
        free(partition_array);
        return NULL;
    }

    /* Read partition array */
    read_context.offset = (uintptr_t)partition_array->array;
    read_context.lba = gpt_hdr->partition_array;
    if(!bios_read_drive(&read_context)) {
        free(partition_array->array);
        free(partition_array);
        return NULL;
    }

    /* Remove empty entries */
    word_t partition_table_count;
    assign_dword_to_word(partition_table_count, gpt_hdr->entries_count);
    
    size_t li = 0, ri;
    GPT_partition_entry *lp = partition_array->array, *rp;

    while (true) {
        /* Move left pointer to empty entry */
        while(li < partition_table_count && memcmp(lp->type, null_partition_type, 16) != 0) {
            ++li;
            lp = (GPT_partition_entry*)(((byte_t*)lp) + partition_array->entry_size);
        }

        /* Move right pointer to the first valid entry after left pointer */
        ri = li;
        rp = lp;
        while (ri < partition_table_count) {
            if (memcmp(rp->type, null_partition_type, 16) != 0) {
                break;
            }
            ++ri;
            rp = (GPT_partition_entry*)(((byte_t*)rp) + partition_array->entry_size);
        }

        /* Move entry from right pointer to left pointer */
        if (li < partition_table_count && ri < partition_table_count) {
            memcpy(lp, rp, partition_array->entry_size);
            memset(rp, 0, partition_array->entry_size);
        } else {
            break;
        }
    }
    partition_array->count = li;

    /* Reallocate array with less size */
    if ((partition_array->array = realloc(partition_array->array, partition_array->entry_size * partition_array->count)) == NULL) {
        free(partition_array->array);
        free(partition_array);
        return NULL;
    }

    return partition_array;
}

static GPT_partition_entry* find_partition(const GPT_partition_array* partition_array, const byte_t* GUID) {
    GPT_partition_entry* partition = partition_array->array;
    size_t count = partition_array->count;
    while (count--) {
        if (memcmp(partition->type, GUID, 16) == 0) {
            return partition;
        }
        partition = (GPT_partition_entry*)(((byte_t*)partition) + partition_array->entry_size);
    }
    return NULL;
}

word_t ssl_entry() {
    /* Print loading message*/
    printf("Loading VLGBL...\n");

    /* Initialize COM port */
    bios_serial_init();

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

    /* Get memory map */
    memory_map* mem_map;
    if ((mem_map = get_memory_map()) == NULL) {
        print_error("Failed to get memory map");
        return 1;
    }

    dump_mem_map(mem_map);

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
    if ((gpt_hdr = realloc(gpt_hdr, sizeof(GPT_header))) == NULL) {
        print_error("Failed to read GPT header.");
        return 1;
    }

    /* Check GPT Signature */
    if(memcmp(gpt_hdr->magic, "EFI PART", 8) != 0) {
        print_error("Drive is not GPT");
        return 1;
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

    /* Save disk GUID */
    memcpy(disk_GUID, gpt_hdr->guid, 16);

    /* Get partition table */
    GPT_partition_array* partition_array;
    if((partition_array = get_partition_array(gpt_hdr)) == NULL) {
        print_error("Failed to get partition table");
        return 1;
    }

    /* Find Third Stage Loader partition */
    GPT_partition_entry* tsl_partition;
    if ((tsl_partition = find_partition(partition_array, tsl_partition_type)) == NULL) {
        print_error("Failed to find Third Stage Loader partition");
        return 1;
    }

    /* Find kernel partition */
    GPT_partition_entry* kernel_partition;
    if ((kernel_partition = find_partition(partition_array, kernel_partition_type)) == NULL) {
        print_error("Failed to find kernel partition");
        return 1;
    }

    mem_dump();

    return 0;
}
