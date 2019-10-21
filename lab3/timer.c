#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"


int timer0_hook_id = TIMER0_IRQ;
extern int counter;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  uint8_t status;

  if (timer_get_conf(timer, &status)) {
    printf("Error when reading timer configuration.\n");
    return 1;
  }

  /* Certain frequencies are not supported by the timer 
  since they would exceed the limit for a 16-bit unsigned integer */
  if (freq < MIN_FREQ || freq > MAX_FREQ) {
    printf("Invalid frequency for 16-bit timer.");
    return 1;
  }

  uint16_t initial_value = TIMER_FREQ / freq;

  int timer_port;
  uint32_t ctrl_word = TIMER_LSB_MSB | (TIMER_MASK_MODE_BCD & status);

  switch (timer) {
  case 0:
    timer_port = TIMER_0;
    ctrl_word |= TIMER_SEL0;
    break;
  case 1:
    timer_port = TIMER_1;
    ctrl_word |= TIMER_SEL1;
    break;
  case 2:
    timer_port = TIMER_2;
    ctrl_word |= TIMER_SEL2;
    break;
  default:
    printf("Invalid arg: timer.\n");
    return 1;
  }

  if (sys_outb(TIMER_CTRL, ctrl_word) == EINVAL) {
    printf("Error when writing to control register.\n");
    return 1;
  }

  uint8_t lsb, msb;

  if (util_get_LSB(initial_value, &lsb)) {
    return 1;
  }

  if (util_get_MSB(initial_value, &msb)) {
    return 1;
  }

  if (sys_outb(timer_port, lsb) == EINVAL) {
    printf("Error when writng LSB to timer.\n");
    return 1;
  }

  if (sys_outb(timer_port, msb) == EINVAL) {
    printf("Error when writng MSB to timer.\n");
    return 1;
  }

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = timer0_hook_id;

  if (sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &timer0_hook_id) != OK) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }
  
  return 0;
}

int (timer_unsubscribe_int)() {
  if (sys_irqrmpolicy(&timer0_hook_id) != OK) {
    printf("Error when calling sys_irqrmpolicy.\n");
    return 1;
  }

  return 0;
}

void (timer_int_handler)() {
  ++counter;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  uint32_t ctrl_word = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL, ctrl_word) == EINVAL) {
    printf("Error when calling sys_outb.\n");
    return 1;
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
    return 1;
  }

  if (util_sys_inb(timer_port, st) == EINVAL) {
    printf("Error when calling util_sys_inb.\n");
    return 1;
  }

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
  
  union timer_status_field_val val;
  uint8_t mode_bits = st & TIMER_MASK_MODE;

  switch (field) {
  case tsf_all:
    val.byte = st;
    break;
  case tsf_initial:
    st = st & TIMER_LSB_MSB;
    val.in_mode = st >> TIMER_LSB_MSB_SHIFT;
    break;
  case tsf_mode:
    if (TIMER_HW_STROBE == mode_bits) {
      val.count_mode = 5;
    }
    else if (TIMER_SW_STROBE == mode_bits) {
      val.count_mode = 4;
    }
    else if ((TIMER_SQR_WAVE == mode_bits) || (TIMER_SQR_WAVE_ALT == mode_bits)) {
      val.count_mode = 3;
    }
    else if ((TIMER_RATE_GEN == mode_bits) || (TIMER_RATE_GEN_ALT == mode_bits)) {
      val.count_mode = 2;
    }
    else if (TIMER_HW_ONESHOT == mode_bits) {
      val.count_mode = 1;
    }
    else if (TIMER_INTERRUPT == mode_bits) {
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
