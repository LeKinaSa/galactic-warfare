#ifndef __KBC_H
#define __KBC_H

#include <lcom/lcf.h>

/* Reusable functions related to the i8042 KBC */


int kbc_read_status(uint8_t *status);     /* Reads status byte from 0x64 port */
int kbc_read_output_buf(uint8_t *output); /* Reads output (data or return value) from 0x60 port */
int kbc_write_command(uint8_t command);   /* Writes a KBC command to the 0x64 port */
int kbc_write_arg(uint8_t arg);           /* Writes an argument for a KBC command to the 0x60 port */
int kbc_reset_cmd_byte();                 /* Resets the KBC command byte to Minix's default */


#endif /* __KBC_H */
