#include "gesture.h"

static struct packet last_packet = { .rb = false, .mb = false, .lb = false };

/* MOUSE EVENT DETECTION */

struct mouse_ev mouse_get_event(struct packet *p) {
  struct mouse_ev event;

  event.type = MOUSE_MOV;   // Default event type is mouse movement, may be overridden by event types with greater priority, like button presses

  if (!p->x_ov) {
    event.delta_x = p->delta_x;
  }

  if (!p->y_ov) {
    event.delta_y = p->delta_y;
  }

  if (!last_packet.rb && !last_packet.mb && !last_packet.lb) {   
    // No buttons were pressed in the previous packet
    if (p->rb && !p->mb && !p->lb) {
      // Only right button was just pressed
      event.type = RB_PRESSED;
    }
    else if (p->lb && !p->mb && !p->rb) {
      // Only left button was just pressed
      event.type = LB_PRESSED;
    }
  }
  else if (!p->rb && !p->mb && !p->lb) {
    // No buttons are pressed in the current packet */
    if (last_packet.rb && !last_packet.mb && !last_packet.lb) {
      // Only right button was pressed in the previous packet
      event.type = RB_RELEASED;
    }
    if (last_packet.lb && !last_packet.mb && !last_packet.rb) {
      // Only left button was pressed in the previous packet
      event.type = LB_RELEASED;
    }
  }
  else if (p->mb || last_packet.mb || (p->rb && p->lb) || (last_packet.rb && last_packet.lb)) {
    // Some other button-related event
    event.type = BUTTON_EV;
  }

  last_packet = *p;
  return event;
}

/* HIGHER-LEVEL STATE MACHINE IMPLEMENTATION */

void update_state_machine(enum state *current_state, struct mouse_ev event) {
  switch (*current_state) {
  case START:
    if (event.type == LB_PRESSED) {
      *current_state = GOING_UP;
    }
    break;
  case GOING_UP:
    if (event.type == LB_RELEASED) {
      // check x_len
    }
    else if (event.type == MOUSE_MOV) {
      // check slope
      // add to x_len
    }
    else if (event.type == LB_PRESSED) {
      // reset x_len
    }
    else {
      *current_state = START;
    }
    break;
  case ON_VERTEX:
    if (event.type == RB_PRESSED) {
      *current_state = GOING_DOWN;
    }
    break;
  case GOING_DOWN:
    break;
  case FINISHED:
    break;
  default:
    *current_state = START;
    break;
  }
}
