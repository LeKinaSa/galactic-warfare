#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>



/* Our implementation of vbe_get_mode_info */
int vbe_return_mode_info(uint16_t mode, vbe_mode_info_t *info_ptr);

/* Sets the operating mode of the video card */
int video_set_mode(uint16_t mode);

/* Draws a pixel of the given color in the specified position to a buffer */
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color, void** buffer);

/* Draws a xpm image to the frame buffer in the specified position. If double_buffered is set to true, uses an auxiliary buffer to prevent flickering */
int vg_draw_xpm(xpm_image_t img, uint16_t x, uint16_t y, void** buffer);

#endif /* __VIDEO_H */
