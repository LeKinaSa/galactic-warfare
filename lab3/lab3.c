#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

#include "keyboard.h"
#include "i8042.h"

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
  uint8_t bit_no = KBD_IRQ, scancode = 0;
  uint8_t bytes[2];
  int size = 1;
  bool two_byte_scancode = false;

  // check if keyboard is OK

  if (kbd_subscribe_int(&bit_no)) {
    printf("Error calling kbd_subscribe_int.\n");
    return 1;
  }
  
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
          // -- IH --
          // read status
          // read output - 1 byte
          // check error (parity | timeout) , timeout -> read 3-5 times -> discard
          if (scancode == KBD_2_BYTES_CODE) {
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
            kbd_print_scancode(~(scancode & KBD_BREAKCODE), size, bytes);
          }
        }
        break;
      default:
        break;
      }
    }
  }
  
  if (kbd_unsubscribe_int()) {
    printf("Error calling kbd_unsubscribe_int.\n");
    return 1;
  }
  //printf(cnt);
  return 0;
  
}

int(kbd_test_poll)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
