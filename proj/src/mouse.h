#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc.h"

/** @defgroup mouse mouse
 * @{
 * @brief Functions specific to the PS/2 mouse 
 */

/**
 * @brief Subscribes mouse interrupts using sys_irqsetpolicy.
 * @param bit_no    pointer to variable where the interrupt bit number will be stored
 * @return          zero if no errors occurred, non-zero otherwise
 */
int mouse_subscribe_int(uint8_t *bit_no);
/**
 * @brief Unsubscribes mouse interrupts using sys_irqrmpolicy.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int mouse_unsubscribe_int();
/**
 * @brief Enables mouse interrupts using sys_irqenable.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int mouse_enable_int();
/**
 * @brief Disables mouse interrupts using sys_irqdisable.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int mouse_disable_int();

/**
 * @brief Writes a command to the PS/2 mouse. 
 * Interrupts must be disabled or else the ACK bytes may be automatically processed by Minix.
 * @param command   command to write to the mouse
 * @return  zero if no errors occurred, non-zero otherwise
 */
int mouse_write_command(uint8_t command);

/**
 * @brief Enables data reporting in stream mode.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int mouse_enable_data_report();
/**
 * @brief Disables data reporting.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int mouse_disable_data_report();

/**
 * @brief Sets the mouse to stream mode.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int mouse_set_stream_mode();
/**
 * @brief Sets the mouse to remote mode.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int mouse_set_remote_mode();

/* Functions for parsing mouse packets */
void mouse_parse_packet(uint8_t bytes[], struct packet *p);
void mouse_get_buttons_pressed(struct packet *p);
void mouse_get_x_displacement(struct packet *p);
void mouse_get_y_displacement(struct packet *p);
void mouse_get_overflow(struct packet *p);
void mouse_put_bytes_on_packet(uint8_t bytes[], struct packet *p);

/**@}*/

#endif /* __MOUSE_H */
