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

#endif /* __GESTURE_H */
