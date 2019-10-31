#include "mouse.h"

int mouse_hook_id = MOUSE_IRQ;
extern int packet_byte;
extern int packet_byte_counter;

int mouse_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  *bit_no = mouse_hook_id;

  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }

  return 0;
}

int mouse_unsubscribe_int() {
  if (sys_irqrmpolicy(&mouse_hook_id) != OK) {
    printf("Error when calling sys_irqrmpolicy.\n");
    return 1;
  }

  return 0;
}

void (mouse_ih)() { // kbc_ih but with packet_byte instead of scancode
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
        packet_byte = output;
        break;
      }
      else if ((status & KBC_TIMEOUT) != 0) {
        ++attempts;
      }
    }
  }
}

int (mouse_enable_data_report) () {
  int attempts = 0;

  while (attempts < KBC_TIMEOUT_MAX_ATTEMPTS) {
    if (kbc_write_command(MOUSE_DISABLE_DATA)) {
      printf("Error when calling kbc_write_command.\n");
    }
    if (kbc_write_arg(MOUSE_DISABLE_DATA)) {
      printf("Error when calling kbc_write_arg.\n");
    }
    if (kbc_read_output_buf(&answer)) {
      printf("Error when calling kbc_read_ouput_buff.\n");
    }
    if (answer != MOUSE_ACK_OK) {
      attempts ++;
    }
    else {
      break;
    }
  }
  if (attempts == KBC_TIMEOUT_MAX_ATTEMPTS) {
    printf("Error occurred when trying to disable the mouse inside mouse_disable_data_report.\n");
    return 1;
  }

  default_config = minix_get_dflt_kbc_cmd_byte();
  attempts = 0;

  while (attempts < KBC_TIMEOUT_MAX_ATTEMPTS) {
    if (kbc_write_command(KBC_WRITE_COMMAND_BYTE)) {
      printf("Error when calling kbc_write_command.\n");
      attempts ++;
      continue;
    }
    if (kbc_write_arg(default_config)) {
      printf("Error when calling kbc_write_arg.\n");
      attempts ++;
      continue;
    }
    break;
  }
  return 0;
}

int mouse_disable_data_report() {
  uint8_t default_config;
  uint8_t answer = 0;
  int attempts = 0;

  while (attempts < KBC_TIMEOUT_MAX_ATTEMPTS) {
    if (kbc_write_command(MOUSE_WRITE_BYTE)) {
      printf("Error when calling kbc_write_command.\n");
    }
    if (kbc_write_arg(MOUSE_DISABLE_DATA)) {
      printf("Error when calling kbc_write_arg.\n");
    }
    if (kbc_read_output_buf(&answer)) {
      printf("Error when calling kbc_read_ouput_buff.\n");
    }
    if (answer != MOUSE_ACK_OK) {
      attempts ++;
    }
    else {
      break;
    }
  }
  if (attempts == KBC_TIMEOUT_MAX_ATTEMPTS) {
    printf("Error occurred when trying to disable the mouse inside mouse_disable_data_report.\n");
    return 1;
  }

  default_config = minix_get_dflt_kbc_cmd_byte();
  attempts = 0;

  while (attempts < KBC_TIMEOUT_MAX_ATTEMPTS) {
    if (kbc_write_command(KBC_WRITE_COMMAND_BYTE)) {
      printf("Error when calling kbc_write_command.\n");
      attempts ++;
      continue;
    }
    if (kbc_write_arg(default_config)) {
      printf("Error when calling kbc_write_arg.\n");
      attempts ++;
      continue;
    }
    break;
  }
  return 0;
}

void mouse_packet_parser(struct packet *p) {
  mouse_get_buttons_pressed(p);
  mouse_get_x_displacement(p);
  mouse_get_y_displacement(p);
  mouse_get_overflow(p);
}

void mouse_get_buttons_pressed(struct packet *p) {
  p->lb = false;
  p->mb = false;
  p->rb = false;
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_LEFT_BUTTON_PRESSED) != 0) {
    p->lb = true;
  }
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_MEDIUM_BUTTON_PRESSED) != 0) {
    p->mb = true;
  }
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_RIGHT_BUTTON_PRESSED) != 0) {
    p->rb = true;
  }
}

void mouse_get_x_displacement(struct packet *p) {
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_MSB_X) != 0) { // X is a negative value
    p->delta_x = (NEGATIVE_NUMBER) & (p->bytes[MOUSE_BYTE_X]);
  }
  else { // X is a positive value
    p->delta_x = (POSITIVE_NUMBER) & (p->bytes[MOUSE_BYTE_X]);
  }
}

void mouse_get_y_displacement(struct packet *p) {
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_MSB_Y) != 0){ // Y is a negative value
    p->delta_y = (NEGATIVE_NUMBER) & (p->bytes[MOUSE_BYTE_Y]);
  }
  else { // Y is a positive value
    p->delta_y = (POSITIVE_NUMBER) & (p->bytes[MOUSE_BYTE_Y]);
  }
}

void mouse_get_overflow(struct packet *p) {
  p->x_ov = false;
  p->y_ov = false;
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_OVERFLOW_X) != 0) {
    p->x_ov = true;
  }
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_OVERFLOW_Y) != 0) {
    p->y_ov = true;
  }
}
