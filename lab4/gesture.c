#include "gesture.h"

static struct packet lastPacket = { .rb = false, .mb = false, .lb = false };

struct mouse_ev mouse_get_event(struct packet *p) {
  struct mouse_ev event;

  event.type = MOUSE_MOV;   // Default event type is mouse movement, may be overridden by event types with greater priority, like button presses

  if (!p->x_ov) {
    event.delta_x = p->delta_x;
  }

  if (!p->y_ov) {
    event.delta_y = p->delta_y;
  }

  if (!lastPacket.rb && !lastPacket.mb && !lastPacket.lb) {   
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
    if (lastPacket.rb && !lastPacket.mb && !lastPacket.lb) {
      // Only right button was pressed in the previous packet
      event.type = RB_RELEASED;
    }
    if (lastPacket.lb && !lastPacket.mb && !lastPacket.rb) {
      // Only left button was pressed in the previous packet
      event.type = LB_RELEASED;
    }
  }
  else if (p->mb || (p->rb && p->lb) || (lastPacket.rb && lastPacket.lb)) { 
    // TODO: Might need a bit more work
    event.type = BUTTON_EV;
  }

  lastPacket = *p;
  return event;
}
