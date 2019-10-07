#include <lcom/lcf.h>
#include <lcom/lab2.h>

#include <stdbool.h>
#include <stdint.h>

#include "i8254.h"

int counter = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab2/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab2/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(timer_test_read_config)(uint8_t timer, enum timer_status_field field) {
  uint8_t status;
  
  if (timer_get_conf(timer, &status)) {
    printf("Error when calling timer_get_conf.\n");
    return 1;
  }

  if (timer_display_conf(timer, status, field)) {
    printf("Error when calling timer_display_conf.\n");
    return 1;
  }

  return 0;
}

int(timer_test_time_base)(uint8_t timer, uint32_t freq) {
  if (timer_set_frequency(timer, freq)) {
    printf("Error when calling timer_set_freq.\n");
    return 1;
  }

  return 0;
}

int(timer_test_int)(uint8_t time) {
  uint8_t num_seconds = 0, bit_no = 0;

  counter = 0;

  if (timer_subscribe_int(&bit_no)) {
    printf("Error calling timer_subscribe_int.\n");
    return 1;
  }

  int ipc_status;
  message msg;

  while (num_seconds < time) {
    if (driver_receive(ANY, &msg, &ipc_status)) { 
      printf("Error when calling driver_receive.\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(bit_no)) {
          timer_int_handler();
          if ((counter % INTERRUPTS_PER_SECOND) == 0) {
            timer_print_elapsed_time();
            ++num_seconds;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (timer_unsubscribe_int()) {
    printf("Error calling timer_unsubscribe_int.\n");
    return 1;
  }
  
  return 0;
}
