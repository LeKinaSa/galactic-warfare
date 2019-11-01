#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc.h"


/* subscribe / unsubscribe interruptions */
int mouse_subscribe_int(uint8_t *bit_no);
int mouse_unsubscribe_int();

/* interrupt handler */
void (mouse_ih)(); // Not used the kbc_ih but the code its pretty similar

// If we use the kbc_ih we would be getting a scancode. If we use the keyboard and the mouse at the same time, i dont know if this is usefull or not

/* write */
int mouse_write(uint8_t command);

/* enable / disable data */
int mouse_enable_data_report(); // it's taking too many I/Os operations
int mouse_disable_data_report();

/* modes */
int mouse_set_stream_mode();
int mouse_set_remote_mode();

/* parsers */
void mouse_parse_packet(uint8_t bytes[], struct packet *p);       // Implemented not tested
void mouse_get_buttons_pressed(struct packet *p);                   // Implemented not tested
void mouse_get_x_displacement(struct packet *p);                    // Implemented not tested
void mouse_get_y_displacement(struct packet *p);                    // Implemented not tested
void mouse_get_overflow(struct packet *p);                          // Implemented not tested
void mouse_put_bytes_on_packet(uint8_t bytes[], struct packet *p); // Implemented not tested

#endif /* __MOUSE_H */
