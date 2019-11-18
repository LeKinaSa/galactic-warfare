#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>

/* Our implementation of vbe_get_mode_info */
int vbe_return_mode_info(uint16_t mode, vbe_mode_info_t *info_ptr);
int video_set_mode(uint16_t mode);
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int vg_draw_pattern(uint8_t no_rectangles, uint32_t first_color, uint8_t step);
int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y, uint16_t mode);

#endif /* __VIDEO_H */
