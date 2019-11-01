#ifndef __MOUSE_H
#define __MOUSE_H

#include <lcom/lcf.h>

#include "i8042.h"
#include "kbc.h"

int mouse_subscribe_int(uint8_t *bit_no);
int mouse_unsubscribe_int();

void (mouse_ih)(); // Not used the kbc_ih but the code its pretty similar

// If we use the kbc_ih we would be getting a scancode. If we use the keyboard and the mouse at the same time, i dont know if this is usefull or not

int mouse_enable_data_report(); // mouse_enable_data_reporting() -> its taking too many I/Os operations
int mouse_disable_data_report();

void mouse_packet_parser(uint8_t bytes[], struct packet *p);       // Implemented not tested
void mouse_get_buttons_pressed(struct packet *p);                   // Implemented not tested
void mouse_get_x_displacement(struct packet *p);                    // Implemented not tested
void mouse_get_y_displacement(struct packet *p);                    // Implemented not tested
void mouse_get_overflow(struct packet *p);                          // Implemented not tested
void mouse_put_bytes_on_packet(uint8_t bytes[], struct packet *p); // Implemented not tested
#endif /* __MOUSE_H */
