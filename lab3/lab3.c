#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"

uint8_t scancode;
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
  bool useful_output = false;

  uint8_t bytes[2];
  int size = 1;
  bool is_makecode, two_byte_scancode = false;

  if (kbd_subscribe_int(&bit_no)) {
    printf("Error when calling kbd_subscribe_int.\n");
    return 1;
  }
  
  if (kbd_check(&scancode, &useful_output)) {
    printf("Error when calling kbd_check.\n");
    return 1;
  }
  
  int ipc_status;
  message msg;

  if (useful_output) {
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
  bool useful_output = false;

  uint8_t bytes[2];
  int size = 1;
  bool is_makecode, two_byte_scancode = false;
  uint8_t status;
  
  if (kbd_check_poll(&scancode, &useful_output)) {
    printf("Error when calling kbd_test_poll.\n");
  }
  
  if (useful_output) {
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
  
  while (scancode != KBD_ESC_BREAKCODE) {
    if (kbd_retrieve_status(&status)) {
      printf("Error when calling kbd_retrieve_status.\n");
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
  
  // TODO: maybe improve this section

  uint8_t command_byte = 0;

  // Issue first KBC command
  if (kbd_write_kbc_command(KBC_READ_COMMAND_BYTE)) {
    printf("Error when calling kbd_write_kbc_command.\n");
    return 1;
  }

  // Get return value from KBC command
  if (kbd_retrieve_output(&command_byte)) {
    printf("Error when calling kbd_retrieve_output.\n");
    return 1;
  }

  // Enable keyboard interrupts, preserve rest of command byte
  command_byte |= CMD_BYTE_ENABLE_KBD_INT;

  // Issue second KBC command
  if (kbd_write_kbc_command(KBC_WRITE_COMMAND_BYTE)) {
    printf("Error when calling kbd_write_kbc_command.\n");
    return 1;
  }

  // Write new command byte
  if (kbd_write_kbc_arg(command_byte)) {
    printf("Error when calling kbd_write_kbc_arg.\n");
    return 1;
  }

  kbd_print_no_sysinb(sys_inb_cnt);
  return 0;
}

int(kbd_test_timed_scan)(uint8_t n) {
  uint8_t timer_no = 0, kbd_no = KBD_IRQ;

  if (timer_subscribe_int(&timer_no)) {
    printf("Error when calling timer_subscribe_int.\n");
    return 1;
  }
  
  if (kbd_subscribe_int(&kbd_no)) {
    printf("Error when calling kbd_subscribe_int.\n");
    return 1;
  }

  int ipc_status;
  message msg;
  uint8_t idle_time = 0, bytes[2];
  int size = 1;
  bool is_makecode, two_byte_scancode = false;

  while ((scancode != KBD_ESC_BREAKCODE) || (idle_time > n)) {
    if (driver_receive(ANY, &msg, &ipc_status)) { 
      printf("Error when calling driver_receive.\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(kbd_no)) {
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
        if (msg.m_notify.interrupts & BIT(timer_no)) {
          //timer_int_handler();
        }
        break;
      default:
        break;
      }
    }
  }
/*
  if (kbd_unsubscribe_int()) {
    printf("Error when calling kbd_unsubscribe_int.\n");
    return 1;
  }

  if (timer_unsubscribe_int()) {
    printf("Error when calling timer_unsubscribe_int.\n");
    return 1;
  }
*/
  return 0;
}
