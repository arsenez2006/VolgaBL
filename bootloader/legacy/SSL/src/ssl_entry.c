#include <bl/types.h>
#include <bl/bios.h>
#include <bl/string.h>
#include <bl/io.h>
#include <bl/utils.h>

static int print_error(const char* error_str) {
    return printf("VLGBL Error: %s.", error_str);
}

/* GDT for Protected mode */
static GDTR48 gdtr_pm;
static GDT_entry gdt_pm[3];

word_t ssl_entry() {
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

    return 0;
}
