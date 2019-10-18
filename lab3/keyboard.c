#include <lcom/lcf.h>

#include "keyboard.h"
#include "i8042.h"
#include "utils.h"

int hook_id = KBD_IRQ;
extern int scancode;

int (kbd_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  *bit_no = hook_id;

  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }

  return 0;
}

int (kbd_enable_int)() {
  if (sys_irqenable(&hook_id) != OK) {
    printf("Error when calling sys_irqenable.\n");
    return 1;
  }

  return 0;
}

int (kbd_disable_int)() {
  if (sys_irqdisable(&hook_id) != OK) {
    printf("Error when calling sys_irqdisable.\n");
    return 1;
  }

  return 0;
}

int (kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != OK) {
    printf("Error when calling sys_irqrmpolicy.\n");
    return 1;
  }

  return 0;
}

void (kbc_ih)(void) {
  uint8_t status, output;
  int attempts = 0;

  while (attempts < KBD_TIMEOUT_MAX_ATTEMPTS) {
    if (util_sys_inb(KBD_STATUS_REG, &status)) {
      printf("Error when reading from status register.\n");
    }

    if (status & KBD_OUT_BUF_FULL) {
      if (util_sys_inb(KBD_OUTPUT_BUF, &output)) {
        printf("Error when reading from output buffer.\n");
      }
      
      if ((status & (KBD_PARITY_ERROR | KBD_TIMEOUT)) == 0) { // Output contains no errors
        scancode = output;
        break;
      }
      else if ((status & KBD_TIMEOUT) != 0) {
        ++attempts;
      }
    }
  }
}


int kbd_retrieve_status(uint8_t *status) {
  int attempts = 0;

  while (attempts < KBD_TIMEOUT_MAX_ATTEMPTS) {
    if (util_sys_inb(KBD_STATUS_REG, status)) {
      printf("Error when reading from status register.\n");
      return 1;
    }

    if ((*status & KBD_TIMEOUT) != 0) {
      ++attempts;
    }
    else {
      return 0;
    }
  }

  printf("Couldn't retrieve status. Maximum no. of attempts reached.\n");
  return 1;
}

int kbd_retrieve_output(uint8_t *output) {
  if (util_sys_inb(KBD_OUTPUT_BUF, output)) {
    printf("Error when reading from output buffer.\n");
    return 1;
  }

  return 0;
}
