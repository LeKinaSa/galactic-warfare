#include <stdbool.h>
#include <stdint.h>

#include "game_logic.h"

typedef struct {
  bool w_pressed;
  bool a_pressed;
  bool s_pressed;
  bool d_pressed;
} keyboard_status;

void process_kbd_scancode(const uint8_t* bytes, keyboard_status* status);
void process_kbd_status(const keyboard_status* status, Player* player); // TODO
