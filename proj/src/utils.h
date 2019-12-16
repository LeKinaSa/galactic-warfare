#ifndef __UTILS_H
#define __UTILS_H

#include <stdint.h>
#include "game_logic.h"

int (util_sys_inb)(int port, uint8_t *value);

int (util_get_LSB)(uint16_t val, uint8_t *lsb);

int (util_get_MSB)(uint16_t val, uint8_t *msb);

Vector2 generate_random_pos(uint16_t max_x, uint16_t max_y);

/* Returns the value of x clamped by a lower and upper bound */
double clamp(double x, double lower, double upper);

int min(int a, int b);
int max(int a, int b);

#endif /* __UTILS_H */
