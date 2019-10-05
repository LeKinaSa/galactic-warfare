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
  // bit_no is the address to be initialized with the bit number to be set in the mask returned upon an interrupt
  int *hook_id = 0;                                                   // <----
  // Prepare hook_id to be input
  int retv = sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, hook_id);
  if (retv == EINVAL) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }
  // Treat hook_id as output
  
  // Not yet done !
  
  return 0;
}

int (timer_unsubscribe_int)() {
  int *hook_id = 0;                                                   // <---
  if (hook_id == NULL) {
    printf("Error when calling timer_unsubscribe_int.\n");
    return 1;
  }
  int res = sys_irqrmpolicy(hook_id);
  if (res == EINVAL) {
    printf("Error when calling sys_irqrmpolicy.\n");
  }
  return 0;
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
  
  union timer_status_field_val val;
  
  switch (field) {
    case tsf_all:
      val.byte = st;
      break;
    case tsf_initial:
      st = st & TIMER_LSB_MSB;
      val.in_mode = st >> TIMER_LSB_MSB_SHIFT;
      break;
    case tsf_mode:
      if (TIMER_HW_STROBE == (st & TIMER_HW_STROBE)) {
        val.count_mode = 5;
      }
      else if (TIMER_SW_STROBE == (st & TIMER_SW_STROBE)) {
        val.count_mode = 4;
      }
      else if (TIMER_SQR_WAVE == (st & TIMER_SQR_WAVE)) {
        val.count_mode = 3;
      }
      else if (TIMER_RATE_GEN == (st & TIMER_RATE_GEN)) {
        val.count_mode= 2;
      }
      else if (TIMER_HW_ONESHOT == (st & TIMER_HW_ONESHOT)) {
        val.count_mode = 1;
      }
      else if (TIMER_INTERRUPT == (st & TIMER_INTERRUPT)) {
        val.count_mode = 0;
      }
      break;
    case tsf_base:
      val.bcd = st & TIMER_BCD;
      break;
    default:
      return 1;
  }
  
  if (timer_print_config(timer, field, val)) {
    printf("Error when calling timer_print_config.\n");
    return 1;
  }

  return 0;
}
