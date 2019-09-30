#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void (timer_int_handler)() {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  int retv;
  uint32_t ctrl_word = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  retv = sys_outb(TIMER_CTRL, ctrl_word);

  if (retv == EINVAL) {
    printf("Error when calling sys_outb.\n");
    return retv;
  }

  int timer_port;

  switch (timer) {
  case 0:
    timer_port = TIMER_0;
    break;
  case 1:
    timer_port = TIMER_1;
    break;
  case 2:
    timer_port = TIMER_2;
    break;
  default:
    printf("Invalid arg: timer.\n");
    return EINVAL;
  }

  retv = util_sys_inb(timer_port, st);

  if (retv == EINVAL) {
    printf("Error when calling util_sys_inb.\n");
    return retv;
  }

  return retv;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
