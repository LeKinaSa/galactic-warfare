#ifndef __KEYBOARD_H 
#define __KEYBOARD_H

#include <lcom/lcf.h>

/** @defgroup keyboard keyboard
 * @{
 * @brief Functions specific to the keyboard
 */

/**
 * @brief Subscribes keyboard interrupts using sys_irqsetpolicy.
 * @param bit_no    pointer to variable where the interrupt bit number will be stored
 * @return          zero if no errors occurred, non-zero otherwise
 */
int kbd_subscribe_int(uint8_t *bit_no);
/**
 * @brief Unsubscribes keyboard interrupts using sys_irqrmpolicy.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int kbd_unsubscribe_int();

/**
 * @brief Retrieves output (scancodes or KBC return values) from 0x60 port.
 * @param output    pointer to where the information from 0x60 will be stored
 * @return          zero if no errors occurred, non-zero otherwise
 */
int kbd_retrieve_output(uint8_t *output);

/**@}*/

#endif /* __KEYBOARD_H */
