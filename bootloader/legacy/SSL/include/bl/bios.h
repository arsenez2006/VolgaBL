#ifndef BL_BIOS_H
#define BL_BIOS_H
#include "types.h"

typedef struct _DAP {
    byte_t size;
    byte_t rsv;
    word_t sectors;
    word_t offset;
    word_t segment;
    qword_t lba;
} DAP;

typedef struct _drive_parameteres {
    word_t size;
    word_t flags;
    dword_t cylinders;
    dword_t heads;
    dword_t sectors;
    qword_t count_of_sectors;
    word_t sector_size;
    dword_t EDD;
} drive_parameteres;

extern void bios_putch(byte_t);
extern bool bios_read_drive(const DAP*);
extern bool bios_get_drive_parameteres(drive_parameteres* buffer);

#endif
