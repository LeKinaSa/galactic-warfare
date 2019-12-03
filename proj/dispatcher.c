#include <math.h>

#include "dispatcher.h"
#include "i8042.h"
#include "vbe_constants.h"
#include "game_constants.h"
#include "mouse.h"
#include "video.h"
#include "utils.h"

extern xpm_animated ship;

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
    status->x = min(max(status->x + p.delta_x, 0), vg_get_x_resolution());
  }
  if (!p.y_ov) {
    status->y = min(max(status->y - p.delta_y, 0), vg_get_y_resolution());
  }
}

void process_mouse_status(mouse_status* status, MouseCursor* cursor, Player* player) {
  /* Find the angle between mouse and player */
  Vector2 mouse_pos = (Vector2) {status->x, status->y};
  Vector2 player_center = Vector2_subtract(player->entity->position, player->entity->offset);
  double angle = Vector2_angle_to(player_center, mouse_pos);

  update_cursor_position(cursor, mouse_pos);
  
  /* Aim */
  if ((angle > -0.875 * M_PI) && (angle <= -0.625 * M_PI)) {
    /* SW */
    player->entity->sprite.img = ship.sw;
  }
  else if ((angle > -0.625 * M_PI) && (angle <= -0.375 * M_PI)) {
    /* S */
    player->entity->sprite.img = ship.s;
  }
  else if ((angle > -0.375 * M_PI) && (angle <= -0.125 * M_PI)) {
    /* SE */
    player->entity->sprite.img = ship.se;
  }
  else if ((angle > -0.125 * M_PI) && (angle <= 0.125 * M_PI)) {
    /* E */
    player->entity->sprite.img = ship.e;
  }
  else if ((angle > 0.125 * M_PI) && (angle <= 0.375 * M_PI)) {
    /* NE */
    player->entity->sprite.img = ship.ne;
  }
  else if ((angle > 0.375 * M_PI) && (angle <= 0.625 * M_PI)) {
    /* N */
    player->entity->sprite.img = ship.n;
  }
  else if ((angle > 0.625 * M_PI) && (angle <= 0.875 * M_PI)) {
    /* NW */
    player->entity->sprite.img = ship.nw;
  }
  else {
    /* W */
    player->entity->sprite.img = ship.w;
  }

  if (status->lb_pressed) {
    /* Shoot */
    // TODO : use the angle and set the intention to fire
    // TODO : shoot
  }
}
