#include <bl/bios.h>

void bios_putch(byte_t ch) {
    __asm__ volatile(
        "int $0x10"
        :
        : "a"((word_t)0x0E00 | (word_t)ch), "b"((word_t)0x0000)
    );
}

bool bios_serial_init(void) {
    bool ret;
    __asm__ volatile(
        "int $0x14"
        : "=@ccc"(ret)
        : "a"((word_t)0x00E3), "d"((word_t)0x0000)
    );
    return !ret;
}

void bios_serial_putch(byte_t ch) {
    __asm__ volatile(
        "int $0x14"
        :
        : "a"((word_t)0x0100 | (word_t)ch), "d"((word_t)0x0000)
    );
}

bool bios_get_e820(dword_t* offset, dword_t buf_size, void* buffer) {
    dword_t SMAP_sig;
    __asm__ volatile(
        "int $0x15"
        : "=a"(SMAP_sig), "=b"(*offset)
        : "a"((word_t)0xE820), "d"((dword_t)0x534D4150),"b"(*offset), "c"(buf_size), "D"((word_t)((dword_t)buffer & 0xFFFF))
    );
    if (SMAP_sig != 0x534D4150) {
        return false;
    } else {
        return true;
    }
}
