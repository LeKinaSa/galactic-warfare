#ifndef __KEYBOARD_H 
#define __KEYBOARD_H

#include <lcom/lcf.h>

/* Functions specific to the keyboard (not reusable in Lab 4) */

int kbd_subscribe_int(uint8_t *bit_no);
int kbd_enable_int();
int kbd_disable_int();
int kbd_unsubscribe_int();

int kbd_reenable_int();   /* Reenables keyboard interrupts after kbd_test_poll */

int kbd_retrieve_output(uint8_t *output);     /* Retrieves output (scancodes or KBC return values) from 0x60 port */

#endif /* __KEYBOARD_H */
