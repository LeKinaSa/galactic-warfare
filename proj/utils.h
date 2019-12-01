#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>

int (util_sys_inb)(int port, uint8_t *value);

int (util_get_LSB)(uint16_t val, uint8_t *lsb);

int (util_get_MSB)(uint16_t val, uint8_t *msb);

/* Returns the value of x clamped by a lower and upper bound */
double clamp(double x, double lower, double upper);

#endif /* __UTILS_H */
