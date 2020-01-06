#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <stdbool.h>
#include <stdint.h>

#include "game_logic.h"

/** @defgroup dispatcher dispatcher
 * @{
 * @brief Functions related to input device (mouse or keyboard) event handling and state
 * machine implementations.
 */

/** @brief Structure that holds the state of the keyboard. Similar to a state machine */
typedef struct {
  bool w_pressed;   /**< @brief True if the W key is currently pressed */
  bool a_pressed;   /**< @brief True if the A key is currently pressed */
  bool s_pressed;   /**< @brief True if the S key is currently pressed */
  bool d_pressed;   /**< @brief True if the D key is currently pressed */
} keyboard_status;

/** @brief Structure that holds the state of the mouse */
typedef struct {
  bool lb_pressed;  /**< @brief True if the left mouse button is currently pressed */
  bool mb_pressed;  /**< @brief True if the middle mouse button is currently pressed */
  bool rb_pressed;  /**< @brief True if the right mouse button is currently pressed */
  uint16_t x;       /**< @brief Horizontal mouse cursor position */
  uint16_t y;       /**< @brief Vertical mouse cursor position */
} mouse_status;

/** @brief Structure for a xpm that has components for the main cardinal directions */
typedef struct {
  xpm_image_t n, s, w, e, nw, ne, sw, se;
} xpm_animated;

/**
 * @brief Processes a keyboard scancode and updates the keyboard status accordingly
 * @param bytes   scancode to process (one or two-byte scancode)
 * @param status  pointer to keyboard status structure
 */
void process_kbd_scancode(uint8_t bytes[], keyboard_status* status);
/**
 * @brief Updates the player's position according to the keyboard status. 
 * Called every frame.
 * @param status  const pointer to keyboard status structure
 * @param player  pointer to player
 */
void process_kbd_status(const keyboard_status* status, Player* player);

/**
 * @brief Processes a mouse packet and updates the mouse status accordingly
 * @param bytes   packet to process (3 bytes)
 * @param status  pointer to mouse status structure
 */
void process_mouse_packet(uint8_t packet_bytes[], mouse_status* status);
/**
 * @brief Updates the cursors's position and player orientation according to the mouse status. 
 * Called every frame.
 * @param status  const pointer to mouse status structure
 * @param cursor  pointer to mouse cursor
 * @param player  pointer to player
 */
void process_mouse_status(const mouse_status* status, MouseCursor* cursor, Player* player);

/**
 * @brief Updates the player orientation according to the player angle.
 * @param player  pointer to player
 */
void rotate_player(Player* player);

/**@}*/

#endif /* DISPATCHER_H */
