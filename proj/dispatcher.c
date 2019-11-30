#include "dispatcher.h"
#include "i8042.h"

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
