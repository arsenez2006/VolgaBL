#ifndef BL_BIOS_H
#define BL_BIOS_H
#include "types.h"

typedef struct _DAP {
    byte_t size;
    byte_t rsv;
    word_t sectors;
    word_t offset;
    word_t segment;
    word_t lba0;
    word_t lba1;
    word_t lba2;
    word_t lba3;
} DAP;

typedef struct _drive_parameteres {
    word_t size;
    word_t flags;
    word_t cylindersl;
    word_t cylindersh;
    word_t headsl;
    word_t headsh;
    word_t sectorsl;
    word_t sectorsh;
    word_t count_of_sectors0;
    word_t count_of_sectors1;
    word_t count_of_sectors2;
    word_t count_of_sectors3;
    word_t sector_size;
    word_t EDDl;
    word_t EDDh;
} drive_parameteres;

extern void bios_putch(byte_t);
extern bool bios_read_drive(const DAP*);
extern bool bios_get_drive_parameteres(drive_parameteres* buffer);

#endif
