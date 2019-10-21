#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <lcom/timer.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"
#include "i8254.h"
#include "kbc.h"

uint8_t scancode;
int counter = 0;    /* Counter for timer 0 interrupt handler */
uint32_t sys_inb_cnt = 0;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  sys_inb_cnt = 0;
  uint8_t bit_no = 0;

  if (kbd_subscribe_int(&bit_no)) {
    printf("Error when calling kbd_subscribe_int.\n");
    return 1;
  }

  uint8_t bytes[2];
  int size = 1;
  bool is_makecode, two_byte_scancode = false;

  int ipc_status;
  message msg;

  while (scancode != KBD_ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status)) { 
      printf("Error when calling driver_receive.\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(bit_no)) {
          kbc_ih();
          
          if (scancode == KBD_TWO_BYTE_CODE) {
            bytes[0] = scancode;
            two_byte_scancode = true;
          }
          else {
            if (two_byte_scancode) {
              bytes[1] = scancode;
              size = 2;
            }
            else {
              bytes[0] = scancode;
              size = 1;
            }

            /* If scancode is breakcode, MSB is set to 1 */
            is_makecode = (scancode & KBD_BREAKCODE) == 0;

            if (kbd_print_scancode(is_makecode, size, bytes)) {
              printf("Error when calling kbd_print_scancode.\n");
            }
          }
        }
        break;
      default:
        break;
      }
    }
  }
  
  if (kbd_unsubscribe_int()) {
    printf("Error when calling kbd_unsubscribe_int.\n");
    return 1;
  }
  
  if (kbd_print_no_sysinb(sys_inb_cnt)) {
    printf("Error when calling kbd_print_no_sysinb.\n");
    return 1;
  }

  return 0;
}

int(kbd_test_poll)() {
  sys_inb_cnt = 0;

  uint8_t bytes[2];
  int size = 1;
  bool is_makecode, two_byte_scancode = false;
  uint8_t status;
  
  while (scancode != KBD_ESC_BREAKCODE) {
    if (kbc_read_status(&status)) {
      printf("Error when calling kbc_read_status.\n");
    }
    
    if ((status & (KBD_TIMEOUT | KBD_PARITY_ERROR)) != 0) {
      printf("Status register indicates keyboard timeout or parity error.\n");
    }
    else if ((status & KBD_OUT_BUF_FULL) != 0) {
      if (kbd_retrieve_output(&scancode)) {
        printf("Error when retrieving keyboard output.\n");
      }
    
      if (scancode == KBD_TWO_BYTE_CODE) {
        bytes[0] = scancode;
        two_byte_scancode = true;
      }
      else {
        if (two_byte_scancode) {
        bytes[1] = scancode;
        size = 2;
        }
        else {
          bytes[0] = scancode;
          size = 1;
        }

        /* If scancode is breakcode, MSB is set to 1 */
        is_makecode = (scancode & KBD_BREAKCODE) == 0;

        if (kbd_print_scancode(is_makecode, size, bytes)) {
          printf("Error when calling kbd_print_scancode.\n");
        }
      }
    }

    tickdelay(micros_to_ticks(KBD_POLLING_INTERVAL));
  }

  if (kbd_reenable_int()) {
    printf("Error when calling kbd_reenable_int.\n");
    return 1;
  }

  if (kbd_print_no_sysinb(sys_inb_cnt)) {
    printf("Error when calling kbd_print_no_sysinb.\n");
    return 1;
  }
  
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t timer_no = TIMER0_IRQ, kbd_no = KBD_IRQ;

  if (timer_subscribe_int(&timer_no)) {
    printf("Error when calling timer_subscribe_int.\n");
    return 1;
  }
  
  if (kbd_subscribe_int(&kbd_no)) {
    printf("Error when calling kbd_subscribe_int. Attempting to unsubscribe timer interrupts.\n");

    if (timer_unsubscribe_int()) {   /* Tries to unsubscribe timer interrupts before returning */
      printf("Error when trying to unsubscribe timer interrupts.\n");
    }

    return 1;
  }

  int ipc_status;
  message msg;

  uint8_t idle_time = 0, bytes[2];
  int size = 1;
  bool is_makecode, two_byte_scancode = false;
  counter = 0;

  while ((scancode != KBD_ESC_BREAKCODE) && (idle_time < n)) {
    if (driver_receive(ANY, &msg, &ipc_status)) { 
      printf("Error when calling driver_receive.\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(kbd_no)) {
          kbc_ih();
          idle_time = 0;
          counter = 0;

          if (scancode == KBD_TWO_BYTE_CODE) {
            bytes[0] = scancode;
            two_byte_scancode = true;
          }
          else {
            if (two_byte_scancode) {
              bytes[1] = scancode;
              size = 2;
            }
            else {
              bytes[0] = scancode;
              size = 1;
            }

            /* If scancode is breakcode, MSB is set to 1 */
            is_makecode = (scancode & KBD_BREAKCODE) == 0;

            if (kbd_print_scancode(is_makecode, size, bytes)) {
              printf("Error when calling kbd_print_scancode.\n");
            }
          }
        }
        if (msg.m_notify.interrupts & BIT(timer_no)) {
          timer_int_handler();
          if (counter == TIMER0_INTERRUPTS_PER_SECOND) {
            counter = 0;
            idle_time++;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (kbd_unsubscribe_int()) {
    printf("Error when calling kbd_unsubscribe_int. Attempting to unsubscribe timer interrupts.\n");

    if (timer_unsubscribe_int()) {   /* Tries to unsubscribe timer interrupts before returning */
      printf("Error when trying to unsubscribe timer interrupts.\n");
    }

    return 1;
  }

  if (timer_unsubscribe_int()) {
    printf("Error when calling timer_unsubscribe_int.\n");
    return 1;
  }

  return 0;
}
