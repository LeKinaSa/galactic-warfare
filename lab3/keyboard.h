#ifndef __KEYBOARD_H 
#define __KEYBOARD_H

#include <lcom/lcf.h>

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_enable_int)();
int (kbd_disable_int)();
int (kbd_unsubscribe_int)();

uint8_t retrieve_status();
uint8_t retrieve_output();

#endif /* __KEYBOARD_H */
