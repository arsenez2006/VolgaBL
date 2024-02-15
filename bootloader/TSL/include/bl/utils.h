/**
 * @file utils.h
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Utility functions used for Third Stage Loader
 *
 */
#ifndef BL_UTILS_H
#define BL_UTILS_H

#include "defines.h"
#include "types.h"

/**
 * @brief Read byte from the port
 *
 * @param [in] port Port number
 * @return The byte
 */
byte_t  inb(word_t port);

/**
 * @brief Read word from the port
 *
 * @param [in] port Port number
 * @return The word
 */
word_t  inw(word_t port);

/**
 * @brief Read dword from the port
 *
 * @param [in] port Port number
 * @return The dword
 */
dword_t inl(word_t port);

/**
 * @brief Send byte to the port
 *
 * @param [in] port Port number
 * @param [in] val Byte
 */
void    outb(word_t port, byte_t val);

/**
 * @brief Send word to the port
 *
 * @param [in] port Port number
 * @param [in] val Word
 */
void    outw(word_t port, word_t val);

/**
 * @brief Send dword to the port
 *
 * @param [in] port Port number
 * @param [in] val Dword
 */
void    outl(word_t port, dword_t val);

/**
 * @brief Print character to COM port
 *
 * @param [in] ch ASCII character to print
 */
void    serial_putch(byte_t ch);

/**
 * @brief Checks if CPUID available
 *
 * @return true - CPUID is available
 * @return false - CPUID is not available
 */
bool    check_cpuid(void);

/**
 * @brief Checks CPU extensions
 *
 * @return true - all required extensions are present
 * @return false - some of the extensions are not implemented
 */
bool    check_cpu_compat(void);

/**
 * @brief Enables Physical Address Extension
 *
 */
void    enable_PAE(void);

/**
 * @brief Loads page table in CR3
 *
 * @param address Physical address of the page table
 */
void    load_page_table(void* address);

/**
 * @brief Enables Long Mode
 *
 */
void    enable_long_mode(void);

/**
 * @brief Enables paging
 *
 */
void    enable_paging(void);

/**
 * @brief Disables all PCI devices
 *
 */
void    disable_pci(void);

/**
 * @brief Align address to Page alignment
 *
 * @param addr Address to align
 * @return Aligned address
 */
dword_t align_page(dword_t addr);

#endif /* BL_UTILS_H */
