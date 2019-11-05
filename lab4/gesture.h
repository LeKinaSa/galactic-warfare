#ifndef __GESTURE_H
#define __GESTURE_H
 
#include <lcom/lcf.h>
#include <lcom/lab4.h>

enum state {
  START,
  GOING_UP,
  ON_VERTEX,
  GOING_DOWN,
  FINISHED
};

struct mouse_ev mouse_get_event(struct packet *p);
void update_state_machine(enum state *current_state, struct mouse_ev event);

#endif /* __GESTURE_H */
