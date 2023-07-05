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
byte_t
inb(word_t port);

/**
 * @brief Read word from the port
 *
 * @param [in] port Port number
 * @return The word
 */
word_t
inw(word_t port);

/**
 * @brief Read dword from the port
 *
 * @param [in] port Port number
 * @return The dword
 */
dword_t
inl(word_t port);

/**
 * @brief Send byte to the port
 *
 * @param [in] port Port number
 * @param [in] val Byte
 */
void
outb(word_t port, byte_t val);

/**
 * @brief Send word to the port
 *
 * @param [in] port Port number
 * @param [in] val Word
 */
void
outw(word_t port, word_t val);

/**
 * @brief Send dword to the port
 *
 * @param [in] port Port number
 * @param [in] val Dword
 */
void
outl(word_t port, dword_t val);

/**
 * @brief Print character to COM port
 *
 * @param [in] ch ASCII character to print
 */
void
serial_putch(byte_t ch);

#endif /* BL_UTILS_H */
