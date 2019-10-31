
#include "i8042.h"
#include "kbc.h"

int kbc_read_status(uint8_t *status) {
  int attempts = 0;

  while (attempts < KBD_TIMEOUT_MAX_ATTEMPTS) {
    if (util_sys_inb(KBC_STATUS_REG, status)) {
      printf("Error when reading from status register.\n");
      return 1;
    }

    if ((*status & KBC_TIMEOUT) != 0) {
      ++attempts;
    }
    else {
      return 0;
    }
  }

  printf("Couldn't retrieve status. Maximum num. of attempts reached.\n");
  return 1;
}

int kbc_write_command(uint8_t command) {
  uint8_t status;
  int wait_time = 0; /* Current wait time in microseconds */

  while (wait_time < KBC_COMMAND_WAIT_TIME) {
    if (kbc_read_status(&status)) {
      printf("Error when calling kbd_retrieve_status.\n");
      return 1;
    }

    // First we must check if there are any errors and if the input buffer is full
    if ((status & (KBC_TIMEOUT | KBC_PARITY_ERROR)) != 0) {
      printf("Status register indicates keyboard timeout or parity error.\n");
      return 1;
    }

    if ((status & KBC_IN_BUF_FULL) == 0) {
      // Input buffer is not full, we can write the KBC command
      if (sys_outb(KBC_INPUT_COMMANDS_BUF, command)) {
        printf("Error when calling sys_outb: couldn't write KBC command.\n");
        return 1;
      }
      return 0;
    }

    wait_time += KBD_POLLING_INTERVAL;
    tickdelay(micros_to_ticks(KBD_POLLING_INTERVAL));
  }

  printf("Timeout occurred: took too long to write command.\n");
  return 1;
}

int kbc_write_arg(uint8_t arg) {
  uint8_t status;
  int wait_time = 0; /* Current wait time in microseconds */

  while (wait_time < KBC_COMMAND_WAIT_TIME) {
    if (kbc_read_status(&status)) {
      printf("Error when calling kbd_retrieve_status.\n");
      return 1;
    }

    // First we must check if there are any errors and if the input buffer is full
    if ((status & (KBC_TIMEOUT | KBC_PARITY_ERROR)) != 0) {
      printf("Status register indicates keyboard timeout or parity error.\n");
      return 1;
    }

    if ((status & KBC_IN_BUF_FULL) == 0) {
      // Input buffer is not full, we can write the argument
      if (sys_outb(KBC_INPUT_ARGS_BUF, arg)) {
        printf("Error when calling sys_outb: couldn't write argument.\n");
        return 1;
      }
      return 0;
    }

    wait_time += KBD_POLLING_INTERVAL;
    tickdelay(micros_to_ticks(KBD_POLLING_INTERVAL));
  }

  printf("Timeout occurred: took too long to write argument.\n");
  return 1;
}
