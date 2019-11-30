// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you
#include "game_logic.h"
#include "game_constants.h"
#include "vbe_constants.h"
#include "i8042.h"
#include "video.h"
#include "keyboard.h"
#include "dispatcher.h"
#include "res/Ship.xpm"
#include "res/Background.xpm"

void* frame_buffer; // Needed for video related functions
int scancode;
uint8_t packet_byte;
int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int (proj_main_loop)(int argc, char *argv[]) {
  uint8_t timer_bit_no = 0, kbd_bit_no = 0;

  if (timer_subscribe_int(&timer_bit_no)) {
    printf("Error when calling timer_subscribe_int.\n");
    return 1;
  }

  if (kbd_subscribe_int(&kbd_bit_no)) {
    timer_unsubscribe_int();
    printf("Error when calling kbd_subscribe_int.\n");
    return 1;
  }

  frame_buffer = vg_init(0x117);

  if (frame_buffer == MAP_FAILED) {
    return 1;
  }

  xpm_map_t ship_xpm = (xpm_map_t)(Ship_xpm);
  xpm_image_t ship_img;
  ship_img.type = XPM_5_6_5;

  if (xpm_load(ship_xpm, ship_img.type, &ship_img) == NULL) {
    timer_unsubscribe_int();
    kbd_unsubscribe_int();
    vg_exit();
    printf("Error when loading xpm.\n");
    return 1;
  }

  Vector2* pos = Vector2_new(20, 20);

  if (vg_draw_xpm(ship_img, pos->x, pos->y, &frame_buffer)) {
    timer_unsubscribe_int();
    kbd_unsubscribe_int();
    vg_exit();
    printf("Error when drawing xpm.\n");
    return 1;
  }

  int ipc_status;
  message msg;

  /* Keyboard-related variables */
  keyboard_status status = { false, false, false, false };
  uint8_t bytes[2];
  bool two_byte_scancode;

  while (scancode != KBD_ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("Error when calling driver_receive.\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(kbd_bit_no)) {
          kbc_ih();

          if (scancode == KBD_TWO_BYTE_CODE) {
            bytes[0] = scancode;
            two_byte_scancode = true;
          }
          else {
            if (two_byte_scancode) {
              bytes[1] = scancode;
            }
            else {
              bytes[0] = scancode;
            }
            process_kbd_scancode(bytes, &status);
          }
        }
        if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
          timer_int_handler();

          if (counter == INTERRUPTS_PER_FRAME) {
            /* 
            Update values according to internal game logic.
            Render a new frame.
            */
            counter = 0;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (kbd_unsubscribe_int()) {
    timer_unsubscribe_int();
    vg_exit();
    printf("Error when calling kbd_unsubscribe_int.\n");
    return 1;
  }

  if (timer_unsubscribe_int()) {
    vg_exit();
    printf("Error when calling timer_unsubscribe_int.\n");
    return 1;
  }

  if (vg_exit()) {
    printf("Error when calling vg_exit.\n");
    return 1;
  }

  return 0;
}
