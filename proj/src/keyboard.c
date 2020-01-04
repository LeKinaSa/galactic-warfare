#include <lcom/lcf.h>

#include "keyboard.h"
#include "i8042.h"
#include "utils.h"
#include "kbc.h"

static int kbd_hook_id = KBD_IRQ;
extern uint8_t scancode;

int kbd_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  *bit_no = kbd_hook_id;

  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id) != OK) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }

  return 0;
}

int kbd_unsubscribe_int() {
  if (sys_irqrmpolicy(&kbd_hook_id) != OK) {
    printf("Error when calling sys_irqrmpolicy.\n");
    return 1;
  }

  return 0;
}

void (kbc_ih)(void) {
  uint8_t status, output;
  int attempts = 0;

  while (attempts < KBC_TIMEOUT_MAX_ATTEMPTS) {
    if (util_sys_inb(KBC_STATUS_REG, &status)) {
      printf("Error when reading from status register.\n");
    }

    if (status & KBC_OUT_BUF_FULL) {
      if (util_sys_inb(KBC_OUTPUT_BUF, &output)) {
        printf("Error when reading from output buffer.\n");
      }
      
      if ((status & (KBC_PARITY_ERROR | KBC_TIMEOUT)) == 0) { // Output contains no errors
        scancode = output;
        break;
      }
      else if ((status & KBC_TIMEOUT) != 0) {
        ++attempts;
      }
    }
  }
}

int kbd_retrieve_output(uint8_t *output) {
  if (util_sys_inb(KBC_OUTPUT_BUF, output)) {
    printf("Error when reading from output buffer.\n");
    return 1;
  }

  return 0;
}
