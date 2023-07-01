#ifndef BL_UTILS_H
#define BL_UTILS_H

#include "defines.h"
#include "types.h"

/**
 * @brief Read byte from the port
 *
 * @param port Port number
 * @return The byte
 */
byte_t
inb(word_t port);

/**
 * @brief Read word from the port
 *
 * @param port Port number
 * @return The word
 */
word_t
inw(word_t port);

/**
 * @brief Read dword from the port
 *
 * @param port Port number
 * @return The dword
 */
dword_t
inl(word_t port);

/**
 * @brief Send byte to the port
 *
 * @param port Port number
 * @param val Byte
 */
void
outb(word_t port, byte_t val);

/**
 * @brief Send word to the port
 *
 * @param port Port number
 * @param val Word
 */
void
outw(word_t port, word_t val);

/**
 * @brief Send dword to the port
 *
 * @param port Port number
 * @param val Dword
 */
void
outl(word_t port, dword_t val);

#endif /* BL_UTILS_H */
