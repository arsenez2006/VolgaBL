#include <bl/utils.h>

byte_t
inb(word_t port) {
    byte_t ret;
    __asm__ volatile("inb %[port], %[ret]"
                     : [ret] "=a"(ret)
                     : [port] "Nd"(port));
    return ret;
}

word_t
inw(word_t port) {
    word_t ret;
    __asm__ volatile("inw %[port], %[ret]"
                     : [ret] "=a"(ret)
                     : [port] "Nd"(port));
    return ret;
}

dword_t
inl(word_t port) {
    dword_t ret;
    __asm__ volatile("inl %[port], %[ret]"
                     : [ret] "=a"(ret)
                     : [port] "Nd"(port));
    return ret;
}

void
outb(word_t port, byte_t val) {
    __asm__ volatile("outb %[val], %[port]"
                     :
                     : [val] "a"(val), [port] "Nd"(port));
}

void
outw(word_t port, word_t val) {
    __asm__ volatile("outw %[val], %[port]"
                     :
                     : [val] "a"(val), [port] "Nd"(port));
}

void
outl(word_t port, dword_t val) {
    __asm__ volatile("outl %[val], %[port]"
                     :
                     : [val] "a"(val), [port] "Nd"(port));
}

void
serial_putch(byte_t ch) {
    while ((inb(0x3F8 + 5) & 0x20) == 0)
        continue;
    outb(0x3F8, ch);
}
