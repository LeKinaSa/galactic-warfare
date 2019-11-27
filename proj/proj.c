// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

// Any header files included below this line should have been created by you
#include "game_logic.h"
#include "vbe_constants.h"
#include "video.h"
#include "keyboard.h"
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
  uint8_t kbd_hook_id = 0;

  if (kbd_subscribe_int(&kbd_hook_id)) {
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

  xpm_load(ship_xpm, ship_img.type, &ship_img);

  if (vg_draw_xpm(ship_img, 20, 20, &frame_buffer)) {
    kbd_unsubscribe_int();
    vg_exit();
    printf("Error when drawing xpm.\n");
    return 1;
  }

  sleep(2);

  if (vg_exit()) {
    kbd_unsubscribe_int();
    printf("Error when calling vg_exit.\n");
    return 1;
  }

  if (kbd_unsubscribe_int()) {
    printf("Error when calling kbd_unsubscribe_int.\n");
    return 1;
  }

  return 0;
}
