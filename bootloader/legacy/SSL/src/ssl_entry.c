#include <bl/defines.h>
#include <bl/types.h>
#include <bl/bios.h>
#include <bl/io.h>
#include <bl/utils.h>

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
    /* Print loading message */
    printf("Loading VLGBL...\n");

    /* Initialize COM port */
    (void)bios_serial_init();

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

    while(1) {
        __asm__ volatile("hlt");
    }
}
