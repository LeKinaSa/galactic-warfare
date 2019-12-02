#include <math.h>

#include "dispatcher.h"
#include "i8042.h"
#include "vbe_constants.h"
#include "game_constants.h"
#include "mouse.h"
#include "video.h"

void process_kbd_scancode(uint8_t bytes[], keyboard_status* status) {
  switch (bytes[0]) {
    case KBD_W_MAKECODE:
      status->w_pressed = true;
      break;
    case KBD_W_BREAKCODE:
      status->w_pressed = false;
      break;
    case KBD_A_MAKECODE:
      status->a_pressed = true;
      break;
    case KBD_A_BREAKCODE:
      status->a_pressed = false;
      break;
    case KBD_S_MAKECODE:
      status->s_pressed = true;
      break;
    case KBD_S_BREAKCODE:
      status->s_pressed = false;
      break;
    case KBD_D_MAKECODE:
      status->d_pressed = true;
      break;
    case KBD_D_BREAKCODE:
      status->d_pressed = false;
      break;   
    default:
      break;
  }
}

void process_kbd_status(const keyboard_status* status, Player* player) {
  Vector2 velocity = (Vector2) {0.0, 0.0};

  if (status->w_pressed) {
    velocity = Vector2_add(velocity, UP);
  }
  if (status->a_pressed) {
    velocity = Vector2_add(velocity, LEFT);
  }
  if (status->s_pressed) {
    velocity = Vector2_add(velocity, DOWN);
  }
  if (status->d_pressed) {
    velocity = Vector2_add(velocity, RIGHT);
  }

  velocity = Vector2_scalar_mult(PLAYER_BASE_SPEED * FRAME_DELTA, Vector2_normalized(velocity));
  player->entity->velocity = velocity;
}


void process_mouse_packet(uint8_t packet_bytes[], mouse_status* status) {
  struct packet p;
  mouse_parse_packet(packet_bytes, &p);
  status->lb_pressed = p.lb;
  status->mb_pressed = p.mb;
  status->rb_pressed = p.rb;
  if (!p.x_ov) {
    if ((status->x + p.delta_x) > vg_get_x_resolution()) {
      status->x = vg_get_x_resolution();
    }
    else {
      status->x = status->x + p.delta_x;
    }
  }
  if (!p.y_ov) {
    if ((status->y + p.delta_y) > vg_get_y_resolution()) {
      status->y = vg_get_y_resolution();
    }
    else {
      status->y = status->y + p.delta_y;
    }
  }
}

void process_mouse_status(mouse_status* status, Player* player) {
  /* Find the angle between mouse and player */
  Vector2 mouse_pos = (Vector2) {status->x, status->y};
  double angle = Vector2_angle_to(player->entity->position, mouse_pos);
  
  /* Aim */
  // TODO: finish aiming (change xpm to be drawn)
  if ((angle > -0.875 * M_PI) && (angle <= -0.625 * M_PI)) {
    /* SW */
  }
  else if ((angle > -0.625 * M_PI) && (angle <= -0.375 * M_PI)) {
    /* S */
  }
  else if ((angle > -0.375 * M_PI) && (angle <= -0.125 * M_PI)) {
    /* SE */
  }
  else if ((angle > -0.125 * M_PI) && (angle <= 0.125 * M_PI)) {
    /* E */
  }
  else if ((angle > 0.125 * M_PI) && (angle <= 0.375 * M_PI)) {
    /* NE */
  }
  else if ((angle > 0.375 * M_PI) && (angle <= 0.625 * M_PI)) {
    /* N */
  }
  else if ((angle > 0.625 * M_PI) && (angle <= 0.875 * M_PI)) {
    /* NW */
  }
  else {
    /* W */
  }

  if (status->lb_pressed) {
    /* Shoot */
    // TODO : use the angle and set the intention to fire
    // TODO : shoot
  }
}
