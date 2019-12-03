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

typedef struct {
  xpm_image_t n;
  xpm_image_t ne;
  xpm_image_t e;
  xpm_image_t se;
  xpm_image_t s;
  xpm_image_t sw;
  xpm_image_t w;
  xpm_image_t nw;
} xpm_animated;

void process_kbd_scancode(uint8_t bytes[], keyboard_status* status);
void process_kbd_status(const keyboard_status* status, Player* player);

void process_mouse_packet(uint8_t packet_bytes[], mouse_status* status);
void process_mouse_status(mouse_status* status, MouseCursor* cursor, Player* player);
