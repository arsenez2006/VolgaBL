/**
 * @file bios.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Useful BIOS call wrappers for C
 *
 */
#include <bl/bios.h>

/* Leave this undocument */
#ifndef DOX_SKIP

/* From bootstrap.asm */
extern byte_t _drive_number;

#endif /* DOX_SKIP */

bool
bios_serial_init(void) {
    bool ret;
    __asm__ volatile("int $0x14" : "=@ccc"(ret) : "a"((word_t)0x00E3), "d"((word_t)0x0000));
    return !ret;
}

void
bios_serial_putch(byte_t ch) {
    __asm__ volatile("int $0x14" : : "a"((word_t)0x0100 | (word_t)ch), "d"((word_t)0x0000));
}

void
bios_putch(byte_t ch) {
    __asm__ volatile("int $0x10" : : "a"((word_t)0x0E00 | (word_t)ch), "b"((word_t)0x0000));
}

bool
bios_get_e820(dword_t* offset, dword_t buf_size, void* buffer) {
    dword_t SMAP_sig;
    __asm__ volatile("int $0x15"
                     : "=a"(SMAP_sig), "=b"(*offset)
                     : "a"((word_t)0xE820),
                       "d"((dword_t)0x534D4150),
                       "b"(*offset),
                       "c"(buf_size),
                       "D"((word_t)((uintptr_t)buffer & 0xFFFF)));
    if (SMAP_sig != 0x534D4150) {
        return false;
    } else {
        return true;
    }
}
bool
bios_get_drive_parameteres(drive_parameteres* buffer) {
    bool ret;
    __asm__ volatile(
      "int $0x13"
      : "=@ccc"(ret)
      : "a"((word_t)0x4800), "d"(_drive_number), "S"((word_t)((uintptr_t)buffer & 0xFFFF)));
    return !ret;
}

bool
bios_read_drive(const DAP* read_context) {
    bool ret;
    __asm__ volatile(
      "int $0x13"
      : "=@ccc"(ret)
      : "a"((word_t)0x4200), "d"(_drive_number), "S"((word_t)((uintptr_t)read_context & 0xFFFF)));

    /* Some BIOSes can't read more than 127 sectors */
    if (ret && read_context->sectors == 128) {
        DAP tmp_read_context;
        tmp_read_context = *read_context;

        /* Read first 127 sectors */
        tmp_read_context.sectors = 127;
        if (!bios_read_drive(&tmp_read_context)) {
            return false;
        }

        /* Read last sector */
        tmp_read_context.sectors = 1;
        tmp_read_context.offset += 127 * SECTOR_SIZE;
        if (!bios_read_drive(&tmp_read_context)) {
            return false;
        } else {
            return true;
        }
    }
    return !ret;
}
