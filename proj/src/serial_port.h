#ifndef __SERIAL_PORT_H
#define __SERIAL_PORT_H

#include <lcom/lcf.h>

#include "uart.h"

int sp_subscribe_int(uint8_t *bit_no);

int sp_unsubscribe_int();

int sp_config(uint32_t bit_rate);

void sp_int_handler(); // TODO

#endif /* __SERIAL_PORT_H */
