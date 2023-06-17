#ifndef BL_BIOS_H
#define BL_BIOS_H

#include "types.h"

typedef struct __packed _DAP {
    byte_t size;
    byte_t rsv;
    word_t sectors;
    word_t offset;
    word_t segment;
    qword_t lba;
} DAP;

typedef struct __packed _drive_parameteres {
    word_t size;
    word_t flags;
    dword_t cylinders;
    dword_t heads;
    dword_t sectors;
    qword_t count_of_sectors;
    word_t sector_size;
} drive_parameteres;

void bios_putch(byte_t ch);
bool __check_ret bios_read_drive(const DAP* read_context);
bool __check_ret bios_get_drive_parameteres(drive_parameteres* buffer);
bool __check_ret bios_get_e820(dword_t* offset, word_t buf_size, void* buffer);
bool bios_serial_init(void);
void bios_serial_putch(byte_t ch);

#endif
