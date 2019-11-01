#include "mouse.h"

static int mouse_hook_id = MOUSE_IRQ;
extern uint8_t packet_byte;

/* SUBSCRIBE / UNSUBSCRIBE INTERRUPTIONS */

int mouse_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  *bit_no = mouse_hook_id;

  if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &mouse_hook_id) != OK) {
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

/* INTERRUPT HANDLER */

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

/* WRITE FUNCTION */

int mouse_write(uint8_t command) {
  uint8_t ack = 0;
  
  if (kbc_write_command(MOUSE_WRITE_BYTE)) {
    printf("Error when calling kbc_write_command.\n");
    return 1;
  }

  if (kbc_write_arg(command)) {
    printf("Error when calling kbc_write_arg.\n");
    return 1;
  }

  /*
  if (kbc_read_output_buf(&ack)) {
    printf("Error when calling kbc_read_ouput_buf.\n");
    return 1;
  } 
  */

  return ack == MOUSE_ACK_OK;
}

/* ENABLE / DISABLE DATA */

int mouse_enable_data_report() {
  if (mouse_set_stream_mode()) {
    printf("Error when calling mouse_set_stream_mode.\n");
    return 1;
  }

  if(mouse_write(MOUSE_ENABLE_DATA)) {
    printf("Error when calling mouse_write.\n");
    return 1;
  }

  return 0;
}

int mouse_disable_data_report() {
  if(mouse_write(MOUSE_RESET)) {
    printf("Error when calling mouse_write.\n");
    return 1;
  }

  return 0;
}

/* MODES */

int mouse_set_stream_mode() {
  if (mouse_write(MOUSE_STREAM_MODE)) {
    printf("Error when setting mouse to stream mode.\n");
    return 1;
  }

  return 0;
}

int mouse_set_remote_mode() {
  if (mouse_write(MOUSE_REMOTE_MODE)) {
    printf("Error when setting mouse to remote mode.\n");
    return 1;
  }

  return 0;
}

/* PARSERS */

void mouse_parse_packet(uint8_t bytes[], struct packet *p) {
  mouse_put_bytes_on_packet(bytes, p);
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
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_MIDDLE_BUTTON_PRESSED) != 0) {
    p->mb = true;
  }
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_RIGHT_BUTTON_PRESSED) != 0) {
    p->rb = true;
  }
}

void mouse_get_x_displacement(struct packet *p) {
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_MSB_X) != 0) { // X is a negative value
    p->delta_x = (NEGATIVE_NUMBER) | (p->bytes[MOUSE_BYTE_X]);
  }
  else { // X is a positive value
    p->delta_x = (POSITIVE_NUMBER) | (p->bytes[MOUSE_BYTE_X]);
  }
}

void mouse_get_y_displacement(struct packet *p) {
  if ((p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_MSB_Y) != 0){ // Y is a negative value
    p->delta_y = (NEGATIVE_NUMBER) | (p->bytes[MOUSE_BYTE_Y]);
  }
  else { // Y is a positive value
    p->delta_y = (POSITIVE_NUMBER) | (p->bytes[MOUSE_BYTE_Y]);
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

void mouse_put_bytes_on_packet(uint8_t bytes[], struct packet *p) {
  for (int index = 0; index < MOUSE_PCK_NUM_BYTES; index++) {
    p->bytes[index] = bytes[index];
  }
}
