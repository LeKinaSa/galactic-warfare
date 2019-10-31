// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "mouse.h"
#include "utils.h"

uint8_t packet_byte;
int packet_byte_counter = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    uint8_t bit_no = 0;
    if (mouse_enable_data_reporting()) {
      printf("Error when calling mouse_enable_data_reporting.\n");
      return 1;
    }
    if (mouse_subscribe_int(&bit_no)) {
      printf("Error when calling mouse_subscribe_int.\n");
      return 1;
    }
    //driver_receive_loop
    if (mouse_unsubscribe_int()) {
      printf("Error when calling mouse_unsubscribe_int.\n");
      return 1;
    }
    if (mouse_disable_data_report()) {
      printf("Error when calling mouse_disable_data_report.\n");
      return 1;
    }
    return 0;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* To be completed */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}

int (mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s(%u): under construction\n", __func__, idle_time);
    return 1;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    // To be completed
    printf("%s: under construction\n", __func__);
    return 1;
}
