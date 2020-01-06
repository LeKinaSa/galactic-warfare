#ifndef __VIDEO_H
#define __VIDEO_H

#include <lcom/lcf.h>

#include "game_logic.h"
#include "utils.h"

/** @defgroup video video
 * @{
 * @brief Functions related to the video card.
 */

/**
 * @brief Returns size of frame buffer in bytes. Returns 0 if frame buffer
 * hasn't been initialized.
 */
uint32_t vg_get_frame_buffer_size();
/** 
 * @brief Returns the screen's horizontal resolution in pixels.
 * Returns 0 if the video mode or frame buffer haven't been properly initialized. 
 */
uint16_t vg_get_x_resolution();
/** 
 * @brief Returns the screen's vertical resolution in pixels.
 * Returns 0 if the video mode or frame buffer haven't been properly initialized.
 */
uint16_t vg_get_y_resolution();

/**
 * @brief Our implementation of vbe_get_mode_info. 
 * Returns information about the specified video card mode.
 * @param mode      video card mode
 * @param info_ptr  pointer to vbe_mode_info_t structure where the info. will be stored
 * @return          zero if no errors occurred, non-zero otherwise
 */
int vbe_return_mode_info(uint16_t mode, vbe_mode_info_t *info_ptr);

/**
 * @brief Sets the operating mode of the video card
 * @param mode  video card mode to set
 * @return      zero if no errors occurred, non-zero otherwise
 */
int video_set_mode(uint16_t mode);

/**
 * @brief Draws a pixel of the given color in a specified position to a given buffer.
 * @param x         horizontal position of the pixel
 * @param y         vertical position of the pixel
 * @param color     pixel's color in the format specified by the video card's mode
 * @param buffer    buffer where the pixel should be drawn (this helps with double buffering)
 * @return          zero if no errors occurred, non-zero otherwise
 */
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color, void** buffer);

/**
 * @brief Draws a rectangle of the given color, width and height in a specified position to a given buffer.
 * @param x         horizontal position of the rectangle
 * @param y         vertical position of the rectangle
 * @param width     width of the rectangle
 * @param height    height of the rectangle
 * @param color     rectangle's color in the format specified by the video card's mode
 * @param buffer    buffer where the rectangle should be drawn
 * @return          zero if no errors occurred, non-zero otherwise
 */
int vg_draw_rectangle_to_buffer(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color, void** buffer);

/**
 * @brief Draws an X Pixmap image in a specified position to a given buffer.
 * @param img       xpm_image_t structure to draw
 * @param x         horizontal position of the first pixel of the xpm
 * @param y         vertical position of the first pixel of the xpm
 * @param buffer    buffer where the xpm should be drawn
 * @return          zero if no errors occurred, non-zero otherwise
 */
int vg_draw_xpm(xpm_image_t img, uint16_t x, uint16_t y, void** buffer);

/**
 * @brief Draws entities to a buffer in a specific order.
 * The entities that are drawn last are drawn on top of the others.
 * @param bullets           linked list containing bullets from the player and the enemy
 * @param current_powerup   pointer to the current powerup (NULL if it doesn't exist or has been picked up)
 * @param player            pointer to player
 * @param enemy             pointer to enemy
 * @param buffer            buffer where the entities should be drawn
 * @return                  zero if no errors occurred, non-zero otherwise
 */
int vg_render_entities(const LinkedList* bullets, const Powerup* current_powerup, const Player* player, const Player* enemy, void** buffer);

/**@}*/

#endif /* __VIDEO_H */
