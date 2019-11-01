#include "i8042.h"
#include "kbc.h"


int kbc_read_status(uint8_t *status) {
  if (status == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  if (util_sys_inb(KBC_STATUS_REG, status)) {
    printf("Error when callign util_sys_inb.\n");
    return 1;
  }

  return 0;
}


int kbc_read_output_buf(uint8_t *output) {
  if (output == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  int hook_id = 0;
  
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id)) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }

  if (sys_irqdisable(&hook_id)) {
    printf("Error when calling sys_irqdisable.\n");
    return 1;
  }

  int wait_time = 0; /* Current wait time in microseconds */
  uint8_t status = 0;

  while (wait_time < KBC_RETURN_VAL_WAIT_TIME) {
    if (kbc_read_status(&status)) {
      printf("Error when reading from status register.\n");
      return 1;
    }

    if ((status & KBC_OUT_BUF_FULL) != 0) {
      if (util_sys_inb(KBC_OUTPUT_BUF, output)) {
        printf("Error when calling util_sys_inb.\n");
        sys_irqenable(&hook_id);
        sys_irqrmpolicy(&hook_id);
        return 1;
      }
      
      if ((status & (KBC_TIMEOUT | KBC_PARITY_ERROR)) != 0) {
        printf("Status register indicates timeout or parity error.\n");
        sys_irqenable(&hook_id);
        sys_irqrmpolicy(&hook_id);
        return 1;
      }

      sys_irqenable(&hook_id);
      sys_irqrmpolicy(&hook_id);

      return 0;
    }

    wait_time += KBD_POLLING_INTERVAL;
    tickdelay(micros_to_ticks(KBD_POLLING_INTERVAL));
  }

  printf("Took too long to read from output buffer.\n");
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
      printf("Status register indicates timeout or parity error.\n");
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
