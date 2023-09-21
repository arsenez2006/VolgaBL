/**
 * @file utils.c
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Utility functions used for Third Stage Loader
 *
 */
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

bool
check_cpuid(void) {
    bool ret;
    __asm__ volatile("pushfl\n"
                     "popl %%eax\n" /* Move EFLAGS to EAX */

                     "movl %%eax, %%ebx\n" /* Save original EFLAGS to EBX*/

                     "xorl $0x200000, %%eax\n" /* Change ID bit */

                     "pushl %%eax\n"
                     "popfl\n" /* Load Updated EFLAGS */

                     "pushfl\n"
                     "popl %%eax\n" /* Move Updated EFLAGS to EAX */

                     "xorl %%eax, %%ebx" /* Check ID bit */
                     : "=@ccz"(ret)
                     :
                     : "eax", "ebx");
    return !ret;
}

static void
_cpuid(dword_t* eax, dword_t* ebx, dword_t* ecx, dword_t* edx) {
    __asm__("cpuid"
            : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
            : "a"(*eax));
}

/* CPUID EAX = 1: EDX */
#define CPUID_PSE (1 << 3)
#define CPUID_MSR (1 << 5)
#define CPUID_PAE (1 << 6)
#define CPUID_APIC (1 << 9)
#define CPUID_PGE (1 << 13)
#define CPUID_PAT (1 << 16)
#define CPUID_ACPI (1 << 22)

/* CPUID EAX = 1: ECX */
#define CPUID_SSE3 (1 << 0)
#define CPUID_SSE41 (1 << 19)
#define CPUID_SSE42 (1 << 20)
#define CPUID_x2APIC (1 << 21)
#define CPUID_AVX (1 << 28)

/* CPUID EAX = 0x80000001: EDX */
#define CPUID_SYSCALL (1 << 11)
#define CPUID_NX (1 << 20)
#define CPUID_PG1G (1 << 26)
#define CPUID_LM (1 << 29)

bool
check_cpu_compat(void) {
    dword_t eax, ebx, ecx, edx;
    dword_t cpuid_max, cpuid_ext_max;

    eax = 0;
    _cpuid(&eax, &ebx, &ecx, &edx);
    cpuid_max = eax;

    eax = 0x80000000;
    _cpuid(&eax, &ebx, &ecx, &edx);
    cpuid_ext_max = eax;

    if (cpuid_max < 1 && cpuid_ext_max < 0x80000001) {
        return false;
    }

    eax = 1;
    _cpuid(&eax, &ebx, &ecx, &edx);
    if (!(edx & CPUID_PAE)) {
        return false;
    }

    eax = 0x80000001;
    _cpuid(&eax, &ebx, &ecx, &edx);
    if (!(edx & CPUID_LM) || !(edx & CPUID_PG1G)) {
        return false;
    }

    return true;
}

void
enable_PAE(void) {
    __asm__ volatile("movl %%cr4, %%eax\n"
                     "orl $0x20, %%eax\n"
                     "movl %%eax, %%cr4\n"
                     :
                     :
                     : "eax");
}

void
load_page_table(void* address) {
    __asm__ volatile("movl %[table], %%cr3\n"
                     :
                     : [table] "a"((dword_t)address));
}

void
enable_long_mode(void) {
    __asm__ volatile("mov $0xC0000080, %%ecx\n"
                     "rdmsr\n"
                     "orl $0x100, %%eax\n"
                     "wrmsr\n"
                     :
                     :
                     : "eax", "ecx", "edx");
}

void
enable_paging(void) {
    __asm__ volatile("movl %%cr0, %%eax\n"
                     "orl $0x80000000, %%eax\n"
                     "movl %%eax, %%cr0\n"
                     :
                     :
                     : "eax");
}

static dword_t
_pci_read_reg(byte_t bus, byte_t device, byte_t func, byte_t reg) {
    dword_t address = ((dword_t)1 << 31) | ((dword_t)bus << 16) |
                      ((dword_t)device << 11) | ((dword_t)func << 8) |
                      ((dword_t)reg * 0x4);

    outl(0xCF8, address);
    return inl(0xCFC);
}

static void
_pci_write_reg(byte_t bus,
               byte_t device,
               byte_t func,
               byte_t reg,
               dword_t value) {
    dword_t address = ((dword_t)1 << 31) | ((dword_t)bus << 16) |
                      ((dword_t)device << 11) | ((dword_t)func << 8) |
                      ((dword_t)reg * 0x4);

    outl(0xCF8, address);
    outl(0xCFC, value);
}

static word_t
_pci_get_vendor(byte_t bus, byte_t device, byte_t func) {
    return (word_t)(_pci_read_reg(bus, device, func, 0x0) & 0xFFFF);
}

static byte_t
_pci_get_type(byte_t bus, byte_t device, byte_t func) {
    return (byte_t)((_pci_read_reg(bus, device, func, 0x3) >> 16) & 0xFF);
}

void
disable_pci(void) {
    size_t bus, device, func;
    dword_t reg;

    for (bus = 0; bus < 256; ++bus) {
        for (device = 0; device < 32; ++device) {
            if (_pci_get_vendor(bus, device, 0) != 0xFFFF) {
                reg = _pci_read_reg(bus, device, 0, 0x1);
                reg &= 0xFFFFFFFC;
                _pci_write_reg(bus, device, 0, 0x1, reg);

                if (_pci_get_type(bus, device, 0) & 0x80) {
                    for (func = 1; func < 8; ++func) {
                        reg = _pci_read_reg(bus, device, func, 0x1);
                        reg &= 0xFFFFFFFC;
                        _pci_write_reg(bus, device, func, 0x1, reg);
                    }
                }
            }
        }
    }
}
