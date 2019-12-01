#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>

#include "game_logic.h"

/* Returns size of frame buffer in bytes */
uint32_t vg_get_frame_buffer_size();

/* Our implementation of vbe_get_mode_info */
int vbe_return_mode_info(uint16_t mode, vbe_mode_info_t *info_ptr);

/* Sets the operating mode of the video card */
int video_set_mode(uint16_t mode);

/* Draws a pixel of the given color in the specified position to a buffer */
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color, void** buffer);

/* Draws a xpm image in the specified position to a buffer. */
int vg_draw_xpm(xpm_image_t img, uint16_t x, uint16_t y, void** buffer);

/* Draws an array of entities to a buffer according to their z-layer. Entities with a higher z-layer are drawn last (on top of the other entities). Returns 0 on success, non-zero otherwise. */
int vg_render_entities(Entity* entities[], uint8_t num_entities, void** buffer);

#endif /* __VIDEO_H */
