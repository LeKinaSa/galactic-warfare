#include "mouse.h"

int mouse_hook_id = MOUSE_IRQ;
extern int packet_byte;
extern int packet_byte_counter;

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
  if (p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_LEFT_BUTTON_PRESSED) {
    p->lb = true;
  }
  if (p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_MEDIUM_BUTTON_PRESSED) {
    p->mb = true;
  }
  if (p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_RIGHT_BUTTON_PRESSED) {
    p->rb = true;
  }
}

void mouse_get_x_displacement(struct packet *p) {
  if (p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_MSB_X) { // X is a negative value
    p->delta_x = (NEGATIVE_NUMBER) & (p->bytes[MOUSE_BYTE_X]);
  }
  else { // X is a positive value
    p->delta_x = (POSITIVE_NUMBER) & (p->bytes[MOUSE_BYTE_X]);
  }
}

void mouse_get_y_displacement(struct packet *p) {
  if (p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_MSB_Y) { // Y is a negative value
    p->delta_y = (NEGATIVE_NUMBER) & (p->bytes[MOUSE_BYTE_Y]);
  }
  else { // Y is a positive value
    p->delta_y = (POSITIVE_NUMBER) & (p->bytes[MOUSE_BYTE_Y]);
  }
}

void mouse_get_overflow(struct packet *p) {
  p->x_ov = false;
  p->y_ov = false;
  if (p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_OVERFLOW_X) {
    p->x_ov = true;
  }
  if (p->bytes[MOUSE_BYTE_TO_TREAT] & MOUSE_OVERFLOW_Y) {
    p->y_ov = true;
  }
}
