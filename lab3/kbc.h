#ifndef __KBC_H
#define __KBC_H

#include <lcom/lcf.h>

int kbc_read_status(uint8_t *status);     /* Reads status byte from 0x64 port */
int kbc_write_command(uint8_t command);   /* Writes a KBC command to the 0x64 port */
int kbc_write_arg(uint8_t arg);           /* Writes an argument for a KBC command to the 0x60 port */

#endif /* __KBC_H */
