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

void update_state_machine(enum state *s, struct mouse_ev event, uint8_t x_len, uint8_t tolerance) {
  /* TODO: remove print statements */
  
  static int16_t delta_x = 0, delta_y = 0;
  
  switch (*s) {
  case START:
    if (event.type == LB_PRESSED) {
      delta_x = 0;
      *s = GOING_UP;
      //printf("GOING UP\n");
    }
    break;
  case GOING_UP:
    if (event.type == MOUSE_MOV) {
      if (event.delta_x >= 0 && event.delta_y >= 0) {
        if (event.delta_y > event.delta_x) {
          // Absolute slope is greater than 1
          delta_x += event.delta_x;
        }
        else if (abs(delta_x) > tolerance || abs(delta_y) > tolerance) {
          *s = START;
          //printf("START\n");
        }
      }
      else if ((event.delta_x < 0 && abs(event.delta_x) < tolerance) || (event.delta_y < 0 && abs(event.delta_y) < tolerance)) {
        delta_x += event.delta_x;
      }
      else {
        *s = START;
        //printf("START\n");
      }
    }
    else if (event.type == LB_RELEASED && delta_x >= x_len) {
      //printf("delta_x: %d | x_len: %d\n", delta_x, x_len);
      delta_x = 0;
      delta_y = 0;
      *s = ON_VERTEX;
      //printf("ON VERTEX\n");
    }
    else {
      *s = START;
      //printf("START\n");
    }
    break;
  case ON_VERTEX:
    if (event.type == MOUSE_MOV) {
      delta_x += event.delta_x;
      delta_y += event.delta_y;
      
      if (abs(delta_x) > tolerance || abs(delta_y) > tolerance) {
        *s = START;
        //printf("START\n");
      }
    }
    else if (event.type == RB_PRESSED) {
      delta_x = 0;
      *s = GOING_DOWN;
      //printf("GOING DOWN\n");
    }
    else if (event.type == LB_PRESSED) {
      delta_x = 0;
      *s = GOING_UP;
    }
    else {
      *s = START;
      //printf("START\n");
    }
    break;
  case GOING_DOWN:
    if (event.type == MOUSE_MOV) {
      if (event.delta_x >= 0 && event.delta_y <= 0) {
        if (-event.delta_y > event.delta_x) {
          // Absolute slope is greater than 1
          delta_x += event.delta_x;
        }
        else if (abs(delta_x) > tolerance || abs(delta_y) > tolerance) {
          *s = START;
          //printf("START\n");
        }
      }
      else if ((event.delta_x < 0 && abs(event.delta_x) < tolerance) || (event.delta_y > 0 && abs(event.delta_y) < tolerance)) {
        delta_x += event.delta_x;
      }
      else {
        *s = START;
        //printf("START\n");
      }
    }
    else if (event.type == RB_RELEASED && delta_x >= x_len) {
      *s = FINISHED;
      //printf("FINISHED\n");
    }
    else {
      *s = START;
      //printf("START\n");
    }
    break;
  case FINISHED:
    break;
  default:
    *s = START;
    break;
  }
}

