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

  printf("Couldn't retrieve status. Maximum num. of attempts reached.\n");
  return 1;
}

int kbd_retrieve_output(uint8_t *output) {
  if (util_sys_inb(KBD_OUTPUT_BUF, output)) {
    printf("Error when reading from output buffer.\n");
    return 1;
  }

  return 0;
}

int kbd_write_kbc_command(uint8_t command) {
  uint8_t status;
  int wait_time = 0; // Current wait time in microseconds */

  while (wait_time < KBC_COMMAND_WAIT_TIME) {
    if (kbd_retrieve_status(&status)) {
      printf("Error when calling kbd_retrieve_status.\n");
      return 1;
    }

    // First we must check if there are any errors and if the input buffer is full
    if ((status & (KBD_TIMEOUT | KBD_PARITY_ERROR)) != 0) {
      printf("Status register indicates keyboard timeout or parity error.\n");
      return 1;
    }

    if ((status & KBD_IN_BUF_FULL) == 0) {
      // Input buffer is not full, we can write the KBC command
      if (sys_outb(KBD_INPUT_COMMANDS_BUF, command)) {
        printf("Error when calling sys_outb: couldn't write KBC command.\n");
        return 1;
      }
      return 0;
    }

    wait_time += KBD_POLLING_INTERVAL;
    micro_delay(KBD_POLLING_INTERVAL);
  }

  printf("Timeout occurred: took too long to write command.\n");
  return 1;
}

int kbd_write_kbc_arg(uint8_t arg) {
  uint8_t status;
  int wait_time = 0; // Current wait time in microseconds */

  while (wait_time < KBC_COMMAND_WAIT_TIME) {
    if (kbd_retrieve_status(&status)) {
      printf("Error when calling kbd_retrieve_status.\n");
      return 1;
    }

    // First we must check if there are any errors and if the input buffer is full
    if ((status & (KBD_TIMEOUT | KBD_PARITY_ERROR)) != 0) {
      printf("Status register indicates keyboard timeout or parity error.\n");
      return 1;
    }

    if ((status & KBD_IN_BUF_FULL) == 0) {
      // Input buffer is not full, we can write the argument
      if (sys_outb(KBD_INPUT_ARGS_BUF, arg)) {
        printf("Error when calling sys_outb: couldn't write argument.\n");
        return 1;
      }
      return 0;
    }

    wait_time += KBD_POLLING_INTERVAL;
    micro_delay(KBD_POLLING_INTERVAL);
  }

  printf("Timeout occurred: took too long to write argument.\n");
  return 1;
}


int kbd_check() {
  uint8_t response = 0x00;
  bool error = false, unable_to_write_command = false;

  // Disable Keyboard Interruptions
  if (kbd_disable_int()) {
    error = true;
  }

  // Disable Keyboard Output
  if (kbd_write_kbc_command(KBC_DISABLE_INTERFACE)) {
    error = true;
  }

  // Write the command to the KBC
  if (kbd_write_kbc_command(KBC_CHECK_SELF)) {
    error = true;
  }
  // Read the answer from the KBC
  if (!unable_to_write_command) {
    //kbd_read_kbc_response(response);
  }

  // Enable Keyboard Output
  if (kbd_write_kbc_command(KBC_ENABLE_INTERFACE)) {
    error = true;
  }

  // Enable Keyboard Interruptions
  if (kbd_enable_int()) {
    error = true;
  }

  if (error) {
    return 1;
  }
  
  if (response != KBC_CHECK_OK) {
    return 1;
  }
  return 0;
}

int kbd_check_poll() {
  return 0;
}
