/**
 * @file string.h
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Functions for manipulating c-strings and buffers
 *
 */
#ifndef BL_STRING_H
#define BL_STRING_H

#include "types.h"

/**
 * @brief Returns the length of a given string
 *
 * @param [in] str Pointer to the null-terminated byte string to be examined
 * @return The length of the null-terminated byte string str.
 */
size_t __check_ret
strlen(const char* str);

/**
 * @brief Compares two buffers
 *
 * @param [in] lhs Pointer to the first buffer
 * @param [in] rhs Pointer to the second buffer
 * @param [in] count Number of bytes to examine
 * @return Zero if lhs and rhs compare equal, or if count is zero.
 */
int __check_ret
memcmp(const void* lhs, const void* rhs, size_t count);

/**
 * @brief Copies one buffer to another
 *
 * @param [out] dest Pointer to the object to copy to
 * @param [in] src Pointer to the object to copy from
 * @param [in] count Number of bytes to copy
 * @return dest
 */
void*
memcpy(void* dest, const void* src, size_t count);

/**
 * @brief Fills a buffer with a character
 *
 * @param [out] ptr Pointer to the object to fill
 * @param [in] val Fill byte
 * @param [in] count Number of bytes to fill
 * @return ptr
 */
void*
memset(void* ptr, int val, size_t count);

#endif /* BL_STRING_H */
