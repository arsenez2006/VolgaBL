/**
 * @file utils.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Utility functions used for Second Stage Loader
 *
 */
#include <bl/bios.h>
#include <bl/mem.h>
#include <bl/string.h>
#include <bl/utils.h>

/* CRC32 polynom table */
static const uint32_t crc32table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/* Null partition type GUID */
static const byte_t null_partition_type[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

word_t
get_ds(void) {
    word_t ds;
    __asm__ volatile("movw %%ds, %[ds]" : [ds] "=r"(ds));
    return ds;
}

uint32_t
crc32(const byte_t* buf, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    while (len--) {
        crc = (crc >> 8) ^ crc32table[(crc ^ *buf++) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

/* Check if A20 is enabled */
static bool
_check_A20(void) {
    bool ret;
    const word_t test_offset = 0x7C00;
    const word_t failure_offset = (((dword_t)0xFFFF << 4) + test_offset) & 0xFFFF;
    /* Try to access byte at address FFFF:7C00
     * Physical address of FFFF:7C00 is (0xFFFF << 4) + 0x7C00 = 0x107BF0
     * If A20 is disabled 0x107BF0 becomes 0x7BF0
     */
    __asm__ volatile(
      "cli\n"

      "pushw %%es\n" /* Save ES and DS */
      "pushw %%ds\n"

      "xorw %%ax, %%ax\n"
      "movw %%ax, %%es\n" /* ES = 0x0000 */

      "notw %%ax\n"
      "movw %%ax, %%ds\n" /* DS = 0xFFFF */

      "movw %[test_offset], %%si\n"    /* SI = test_offset, test address = DS:SI */
      "movw %[failure_offset], %%di\n" /* DI = failure_offset, failure address = ES:DI */

      "movb %%ds:(%%si), %%al\n" /* Save byte at test address */
      "pushw %%ax\n"

      "movb %%es:(%%di), %%al\n" /* Save byte at failure address */
      "pushw %%ax\n"

      "movb $0x00, %%es:(%%di)\n" /* Place 0x00 at failure address */
      "movb $0xFF, %%ds:(%%si)\n" /* Place 0xFF at test address */

      "cmpb $0x00, %%es:(%%di)\n" /* Check if failure address is still 0x00 */

      "popw %%ax\n" /* Restore value from failure address */
      "movb %%al, %%es:(%%di)\n"

      "popw %%ax\n" /* Restore value from test address */
      "movb %%al, %%ds:(%%si)\n"

      "popw %%ds\n" /* Restore ES and DS */
      "popw %%es\n"

      "sti"
      : "=@ccz"(ret)
      : [test_offset] "rm"(test_offset), [failure_offset] "rm"(failure_offset)
      : "ax", "di", "si");
    return ret;
}

static byte_t __inline__ _inb(word_t port) {
    byte_t ret;
    __asm__ volatile("inb %[port], %[ret]" : [ret] "=a"(ret) : [port] "Nd"(port));
    return ret;
}

static void __inline__ _outb(word_t port, byte_t val) {
    __asm__ volatile("outb %[val], %[port]" : : [val] "a"(val), [port] "Nd"(port));
}

bool
enable_A20(void) {
    bool CF;
    byte_t kb_data;

    /* Check if A20 is already enabled */
    if (_check_A20()) {
        return true;
    }

    /* Try BIOS method */
    __asm__ volatile("int $0x15" : "=@ccc"(CF) : "a"((word_t)0x2401));
    if (!CF && _check_A20()) {
        return true;
    }

    /* Try keyboard controller method */
    while (_inb(0x64) & 2)
        continue;
    _outb(0x64, 0xAD); /* Disable PS/2 port */

    while (_inb(0x64) & 2)
        continue;
    _outb(0x64, 0xD0); /* Prepare to read data-output port */

    while (!(_inb(0x64) & 1))
        continue;
    kb_data = _inb(0x60); /* Read data-output port */

    while (_inb(0x64) & 2)
        continue;
    _outb(0x64, 0xD1); /* Prepare to write to data-output port */

    while (_inb(0x64) & 2)
        continue;
    _outb(0x60, kb_data | 2); /* Set A20 gate bit */

    while (_inb(0x64) & 2)
        continue;
    _outb(0x64, 0xAE); /* Enable PS/2 port */

    while (_inb(0x64) & 2)
        continue;

    if (_check_A20()) {
        return true;
    }

    /* Try Fast A20 method */
    _outb(0x92, _inb(0x92) | 2);
    if (_check_A20()) {
        return true;
    }

    return false;
}

void
set_GDT32_entry(GDT32_entry* entry,
                dword_t base,
                dword_t limit,
                GDT_flags flags,
                GDT_access access) {
    *(word_t*)&entry->data[0] = (word_t)limit;
    *(word_t*)&entry->data[2] = (word_t)base;
    *(byte_t*)&entry->data[4] = (byte_t)(base >> 16);
    *(byte_t*)&entry->data[5] = (byte_t)access;
    *(byte_t*)&entry->data[6] = (byte_t)((flags << 4) | ((limit >> 16) & 0x0F));
    *(byte_t*)&entry->data[7] = (byte_t)(base >> 24);
}

void
set_GDTR32(GDTR32* gdtr, const GDT32_entry* gdt32_table, size_t count) {
    *(word_t*)&gdtr->data[0] = sizeof(GDT32_entry) * count - 1;
    *(dword_t*)&gdtr->data[2] = ((dword_t)get_ds() << 4) | (dword_t)gdt32_table;
}

void
load_GDT32(GDTR32 gdtr) {
    __asm__ volatile("lgdt %[gdtr]" : : [gdtr] "m"(gdtr));
}

void
enter_unreal(word_t data_segment_offset) {
    __asm__ volatile("cli\n" /* Save real mode segments and disable interrupts */
                     "pushw %%ds\n"
                     "pushw %%es\n"

                     "movl %%cr0, %%eax\n" /* Enter Protected mode */
                     "orb $1, %%al\n"
                     "movl %%eax, %%cr0\n"

                     "jmp .pm\n" /* Clear CPU intstruction cache */
                     ".pm:\n"

                     "movw %[seg], %%bx\n" /* Load data descriptor */
                     "movw %%bx, %%ds\n"
                     "movw %%bx, %%es\n"

                     "andb $0xFE, %%al\n" /* Enter Unreal mode by disabling Protected mode */
                     "mov %%eax, %%cr0\n"

                     "popw %%es\n" /* Restore real mode segments and enable interrupts */
                     "popw %%ds\n"
                     "sti"
                     :
                     : [seg] "rm"(data_segment_offset)
                     : "eax", "bx");
}

GPT_partition_array*
get_partition_array(const GPT_header* gpt_hdr) {
    GPT_partition_array* partition_array;
    dword_t partition_table_size;
    DAP read_context;
    size_t li, ri;
    GPT_partition_entry *lp, *rp;

    /* Allocate struct */
    if ((partition_array = malloc(sizeof(GPT_partition_array))) == NULL) {
        return NULL;
    }
    partition_array->entry_size = gpt_hdr->entry_size;

    /* Prepare DAP */
    memset(&read_context, 0, sizeof(DAP));
    read_context.size = sizeof(DAP);
    read_context.segment = get_ds();

    /* Calculate size of array */
    partition_table_size = gpt_hdr->entries_count * gpt_hdr->entry_size;

    /* Calculate count of sectors to read */
    read_context.sectors = (word_t)(partition_table_size / SECTOR_SIZE +
                                    (partition_table_size % SECTOR_SIZE == 0 ? 0 : 1));

    /* Allocate enough space for reading partition array */
    if ((partition_array->array = malloc(read_context.sectors * 512)) == NULL) {
        free(partition_array);
        return NULL;
    }

    /* Read partition array */
    read_context.offset = (word_t)((uintptr_t)partition_array->array);
    read_context.lba = gpt_hdr->partition_array;
    if (!bios_read_drive(&read_context)) {
        free(partition_array->array);
        free(partition_array);
        return NULL;
    }

    /* Remove empty entries */
    li = 0;
    lp = partition_array->array;

    while (true) {
        /* Move left pointer to empty entry */
        while (li < gpt_hdr->entries_count && memcmp(lp->type, null_partition_type, 16) != 0) {
            ++li;
            lp = (GPT_partition_entry*)(((byte_t*)lp) + partition_array->entry_size);
        }

        /* Move right pointer to the first valid entry after left pointer */
        ri = li;
        rp = lp;
        while (ri < gpt_hdr->entries_count) {
            if (memcmp(rp->type, null_partition_type, 16) != 0) {
                break;
            }
            ++ri;
            rp = (GPT_partition_entry*)(((byte_t*)rp) + partition_array->entry_size);
        }

        /* Move entry from right pointer to left pointer */
        if (li < gpt_hdr->entries_count && ri < gpt_hdr->entries_count) {
            memcpy(lp, rp, partition_array->entry_size);
            memset(rp, 0, partition_array->entry_size);
        } else {
            break;
        }
    }
    partition_array->count = li;

    /* Reallocate array with less size */
    if ((partition_array->array = realloc(
           partition_array->array, partition_array->entry_size * partition_array->count)) == NULL) {
        free(partition_array->array);
        free(partition_array);
        return NULL;
    }

    return partition_array;
}

GPT_partition_entry*
find_partition(const GPT_partition_array* partition_array, const byte_t* GUID) {
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
