#include "dispatcher.h"
#include "i8042.h"
#include "vbe_constants.h"
#include "mouse.h"

void process_kbd_scancode(const uint8_t* bytes, keyboard_status* status) {
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
  
}

void process_mouse_status(uint8_t packet_bytes[], mouse_status* status) {
  struct packet* p = 0;
  mouse_parse_packet(packet_bytes, p);
  status->lb_pressed = p->lb;
  status->mb_pressed = p->mb;
  status->rb_pressed = p->rb;
  if (!p->x_ov) {
    if ((status->x + p->delta_x) > MAX_X) {
      status->x = MAX_X;
    }
    else {
      status->x = status->x + p->delta_x;
    }
  }
  if (!p->y_ov) {
    if ((status->y + p->delta_y) > MAX_Y) {
      status->y = MAX_Y;
    }
    else {
      status->y = status->y + p->delta_y;
    }
  }
}
