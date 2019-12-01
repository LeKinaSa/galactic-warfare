#include <stdbool.h>
#include <stdint.h>

#include "game_logic.h"

typedef struct {
  bool w_pressed;
  bool a_pressed;
  bool s_pressed;
  bool d_pressed;
} keyboard_status;

typedef struct {
  bool lb_pressed;
  bool mb_pressed;
  bool rb_pressed;
  uint16_t x;
  uint16_t y;
} mouse_status;


void process_kbd_scancode(const uint8_t* bytes, keyboard_status* status);
void process_kbd_status(const keyboard_status* status, Player* player); // TODO

void process_mouse_packet(uint8_t packet_bytes[], mouse_status* status);
void process_mouse_status(mouse_status* status, Player* player);
