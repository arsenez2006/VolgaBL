/**
 * @file bios.h
 * @author Arseny Lashkevich (arsenez@cybercommunity.space)
 * @brief Useful BIOS call wrappers for C
 *
 */
#ifndef BL_BIOS_H
#define BL_BIOS_H

#include "types.h"

/**
 * @struct DAP
 * @brief Disk Address Packet
 * @details Used by BIOS int 13h for reading/writing drive
 *
 * @typedef DAP
 * @brief DAP type
 *
 */
typedef struct __packed DAP {
  /**
   * @brief Size of the DAP. Must be 16 or sizeof(DAP)
   *
   */
  byte_t  size;
  /**
   * @brief Reserved. Always 0
   *
   */
  byte_t  rsv;
  /**
   * @brief Number of sectors to read/write. Maximum value is 128
   *
   */
  word_t  sectors;
  /**
   * @brief Read/Write buffer offset in the segment
   *
   */
  word_t  offset;
  /**
   * @brief Read/Write buffer segment
   *
   */
  word_t  segment;
  /**
   * @brief LBA of the drive
   *
   */
  qword_t lba;
} DAP;

/**
 * @struct drive_parameteres
 * @brief Parameteres of the drive
 * @details Used by BIOS int 13h to get drive parameteres
 *
 * @typedef drive_parameteres
 * @brief drive_parameteres type
 *
 */
typedef struct __packed drive_parameteres {
  /**
   * @brief Size of the result buffer. Must be 26 or sizeof(drive_parameteres)
   *
   */
  word_t  size;
  /**
   * @brief Information flags
   *
   */
  word_t  flags;
  /**
   * @brief Physical number of cylinders
   *
   */
  dword_t cylinders;
  /**
   * @brief Physical number of heads
   *
   */
  dword_t heads;
  /**
   * @brief Physical number of sectors per track
   *
   */
  dword_t sectors;
  /**
   * @brief Absolute number of sectors
   *
   */
  qword_t count_of_sectors;
  /**
   * @brief Bytes per sector
   *
   */
  word_t  sector_size;
} drive_parameteres;

/**
 * @brief Print char to terminal, using BIOS int 10h
 *
 * @param [in] ch ASCII character to print
 */
void             bios_putch(byte_t ch);

/**
 * @brief Read drive using BIOS int 13h
 * @details Can read 128 sectors on all BIOSes
 *
 * @param [in] read_context Pointer to DAP
 * @return true on success
 * @return false on failure
 */
bool __check_ret bios_read_drive(const DAP* read_context);

/**
 * @brief Get drive parameteres, using BIOS int 13h
 *
 * @param [out] buffer Pointer to resulting drive_parameteres
 * @return true on success
 * @return false on failure
 */
bool __check_ret bios_get_drive_parameteres(drive_parameteres* buffer);

/**
 * @brief Get E820 memory map entry, using BIOS int 15h
 *
 * @param [in out]  offset
 *                  On call: offset from the beggining of the memory card (0 -
 * start over)\n On return: Next offset from the beggining of the memory card (0
 * - the whole map has been transferred)
 * @param [in] buf_size Buffer size. Should be sizeof(memory_map_entry)
 * @param [in] buffer Pointer to buffer
 * @return true on success
 * @return false on failure
 */
bool __check_ret bios_get_e820(dword_t* offset, dword_t buf_size, void* buffer);

/**
 * @brief Initialize COM port, using BIOS int 14h
 *
 * @return true on success
 * @return false on failure
 */
bool             bios_serial_init(void);

/**
 * @brief Print character to COM port, using BIOS int 14h
 *
 * @param [in] ch ASCII character to print
 */
void             bios_serial_putch(byte_t ch);

#endif /* BL_BIOS_H */
