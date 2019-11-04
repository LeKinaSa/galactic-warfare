#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc.h"

/* Functions specific to the PS/2 mouse */


int mouse_subscribe_int(uint8_t *bit_no);   /* Subscribes mouse interrupts */
int mouse_unsubscribe_int();                /* Unsubscribes mouse interrupts */

int mouse_write_command(uint8_t command);   /* Writes a command to the PS/2 mouse. Interrupts must be disabled or else the ACK bytes may be automatically processed by Minix. */

int mouse_enable_data_report();             /* Enables data reporting in stream mode */
int mouse_disable_data_report();            /* Disables data reporting */

int mouse_set_stream_mode();                /* Sets the mouse to stream mode */
int mouse_set_remote_mode();                /* Sets the mouse to remote mode */

/* Functions for parsing mouse packets */
void mouse_parse_packet(uint8_t bytes[], struct packet *p);
void mouse_get_buttons_pressed(struct packet *p);
void mouse_get_x_displacement(struct packet *p);
void mouse_get_y_displacement(struct packet *p);
void mouse_get_overflow(struct packet *p);
void mouse_put_bytes_on_packet(uint8_t bytes[], struct packet *p);


#endif /* __MOUSE_H */
