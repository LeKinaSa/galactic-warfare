#ifndef __KBC_H
#define __KBC_H

#include <lcom/lcf.h>

/** @defgroup kbc kbc
 * @{
 * @brief Reusable functions related to the i8042 KBC.
 * Can be used to program the keyboard and mouse.
 */

/**
 * @brief Reads status byte from the 0x64 port
 * @param status    pointer to where the status byte will be stored
 * @return          zero if no errors occurred, non-zero otherwise
 */
int kbc_read_status(uint8_t *status);
/**
 * @brief Reads output (data or return value) from the 0x60 port
 * @param output    pointer to where the output will be stored
 * @return          zero if no errors occurred, non-zero otherwise
 */
int kbc_read_output_buf(uint8_t *output);

/**
 * @brief Writes a KBC command to the 0x64 port
 * @param command   KBC command to write  
 * @return          zero if no errors occurred, non-zero otherwise
 */
int kbc_write_command(uint8_t command);
/**
 * @brief Writes an argument for a KBC command to the 0x60 port
 * @param arg   KBC command argument to write
 * @return      zero if no errors occurred, non-zero otherwise
 */
int kbc_write_arg(uint8_t arg);

/**
 * @brief Disables both the mouse and keyboard interrupts by writing a command byte
 * @return zero if no errors occurred, non-zero otherwise
 */
int kbc_disable_int();
/**
 * @brief Resets the KBC command byte to Minix's default
 * @return zero if no errors occurred, non-zero otherwise
 */
int kbc_reset_cmd_byte();

/**@}*/

#endif /* __KBC_H */
