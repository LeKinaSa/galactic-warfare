#include <lcom/lcf.h>

#include "keyboard.h"
#include "i8042.h"
#include "utils.h"
#include "kbc.h"

int kbd_hook_id = KBD_IRQ;
extern int scancode;

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

int kbd_enable_int() {
  if (sys_irqenable(&kbd_hook_id) != OK) {
    printf("Error when calling sys_irqenable.\n");
    return 1;
  }

  return 0;
}

int kbd_disable_int() {
  if (sys_irqdisable(&kbd_hook_id) != OK) {
    printf("Error when calling sys_irqdisable.\n");
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

int kbd_reenable_int() {
  uint8_t command_byte = 0;

  // Issue first KBC command
  if (kbc_write_command(KBC_READ_COMMAND_BYTE)) {
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
  if (kbc_write_command(KBC_WRITE_COMMAND_BYTE)) {
    printf("Error when calling kbd_write_kbc_command.\n");
    return 1;
  }

  // Write new command byte
  if (kbc_write_arg(command_byte)) {
    printf("Error when calling kbd_write_kbc_arg.\n");
    return 1;
  }

  return 0;
}

int kbd_retrieve_output(uint8_t *output) {
  if (util_sys_inb(KBD_OUTPUT_BUF, output)) {
    printf("Error when reading from output buffer.\n");
    return 1;
  }

  return 0;
}

int kbc_read_kbc_response(uint8_t *answer) {
  int attempts = 0;
  uint8_t status;

  if (answer == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }
  while (attempts < KBD_TIMEOUT_MAX_ATTEMPTS) {
    if (util_sys_inb(KBD_STATUS_REG, &status)) {
      printf("Error when reading from status register.\n");
      return 1;
    }
    if ((status & KBD_TIMEOUT) != 0) {
      ++attempts;
    }
    else if ((status & KBD_OUT_BUF_FULL) != 0) {
      if ((status & KBD_PARITY_ERROR) != 0) {
        printf("Status register indicates parity error.\n");
        return 1;
      }
      if (kbd_retrieve_output(answer)) {
        printf("Error when calling kbd_retrieve_output.\n");
        return 1;
      }
      return 0;
    }
  }

  printf("Couldn't retrieve status. Maximum num. of attempts reached.\n");
  return 1;
}
