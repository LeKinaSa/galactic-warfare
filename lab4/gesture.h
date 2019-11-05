#ifndef __GESTURE_H
#define __GESTURE_H
 
#include <lcom/lcf.h>
#include <lcom/lab4.h>

/* Functions specific to the mouse_test_gesture method implementation */

enum state {
  START,
  GOING_UP,
  ON_VERTEX,
  GOING_DOWN,
  FINISHED
};

struct mouse_ev mouse_get_event(struct packet *p);      /* Converts a packet's information into a mouse event, using information from last packet */

void update_state_machine(enum state *s, struct mouse_ev event, uint8_t x_len, uint8_t tolerance);
/* Updates the current state of the state machine */
#endif /* __GESTURE_H */
