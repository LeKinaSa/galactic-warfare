#ifndef __KEYBOARD_H 
#define __KEYBOARD_H

#include <lcom/lcf.h>

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_enable_int)();
int (kbd_disable_int)();
int (kbd_unsubscribe_int)();

int kbd_retrieve_status(uint8_t *status);     /* Retrieves status byte from 0x64 port */
int kbd_retrieve_output(uint8_t *output);     /* Retrieves output (scancodes or KBC return values) from 0x60 port */
int kbd_write_kbc_command(uint8_t command);   /* Writes a KBC command to the 0x64 port */
int kbd_write_kbc_arg(uint8_t arg);           /* Writes an argument for a KBC command to the 0x60 port */

int kbd_check(uint8_t *output, bool *useful_output);       /* Self check on KBC */
int kbd_check_poll(uint8_t *output, bool *useful_output);  /* Self check on KBC without interruptions */

#endif /* __KEYBOARD_H */
