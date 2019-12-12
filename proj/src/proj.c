// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

// Any header files included below this line should have been created by you
#include "game_logic.h"
#include "game_constants.h"
#include "vbe_constants.h"
#include "i8042.h"
#include "video.h"
#include "keyboard.h"
#include "mouse.h"
#include "dispatcher.h"
#include "rtc.h"

#include "../res/Background.xpm"
#include "../res/Cursor.xpm"
#include "../res/ShipN.xpm"
#include "../res/ShipS.xpm"
#include "../res/ShipE.xpm"
#include "../res/ShipW.xpm"
#include "../res/ShipNE.xpm"
#include "../res/ShipNW.xpm"
#include "../res/ShipSE.xpm"
#include "../res/ShipSW.xpm"

typedef struct {
  bool timer_int_subscribed;
  bool rtc_int_subscribed;
  bool kbd_int_subscribed;
  bool mouse_int_subscribed;
  bool vg_initialized;
} program_status_t;

/* Video Related Variables */
void* frame_buffer;
/* Timer Related Variables */
int counter = 0;
/* Keyboard Related Variables */
uint8_t scancode = 0;
/* Mouse Related Variables */
uint8_t packet_byte;
/* XPM Related Variables (for animation) */
xpm_animated ship;

static void exit_program(program_status_t* status);

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // Log function invocations that are being "wrapped" by LCF
  //lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // Save the output of printf function calls on a file
  //lcf_log_output("/home/lcom/labs/proj/output.txt");

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
  uint8_t timer_bit_no = 0, rtc_bit_no = 0, kbd_bit_no = 0, mouse_bit_no = 0;
  
  /* Creates a program_status_t structure with all boolean values set to false */
  program_status_t* program_status = (program_status_t*) malloc(sizeof(program_status_t));
  memset((void *) program_status, (int) false, sizeof(program_status_t));

  if (timer_subscribe_int(&timer_bit_no)) {
    printf("Error when calling timer_subscribe_int.\n");
    return 1;
  }
  program_status->timer_int_subscribed = true;

  if (rtc_subscribe_int(&rtc_bit_no)) {
    exit_program(program_status);
    printf("Error when calling rtc_subscribe_int.\n");
    return 1;
  }
  program_status->rtc_int_subscribed = true;

  if (kbd_subscribe_int(&kbd_bit_no)) {
    exit_program(program_status);
    printf("Error when calling kbd_subscribe_int.\n");
    return 1;
  }
  program_status->kbd_int_subscribed = true;

  if (mouse_subscribe_int(&mouse_bit_no)) {
    exit_program(program_status);
    printf("Error when calling mouse_subscribe_int.\n");
    return 1;
  }
  program_status->mouse_int_subscribed = true;

  if (mouse_disable_int()) {
    exit_program(program_status);
    printf("Error when calling mouse_disable_int.\n");
    return 1;
  }

  if (mouse_enable_data_report()) {
    exit_program(program_status);
    printf("Error when calling mouse_enable_data_report.\n");
    return 1;
  }

  if (mouse_enable_int()) {
    exit_program(program_status);
    printf("Error when calling mouse_enable_int.\n");
    return 1;
  }
  
  frame_buffer = vg_init(MODE_PROJECT);

  if (frame_buffer == MAP_FAILED) {
    exit_program(program_status);
    return 1;
  }
  program_status->vg_initialized = true;

  xpm_map_t bg_xpm = (xpm_map_t) Background_xpm;
  xpm_map_t cursor_xpm = (xpm_map_t) Cursor_xpm;

  xpm_map_t ship_N_xpm = (xpm_map_t) ShipN_xpm;
  xpm_map_t ship_S_xpm = (xpm_map_t) ShipS_xpm;
  xpm_map_t ship_E_xpm = (xpm_map_t) ShipE_xpm;
  xpm_map_t ship_W_xpm = (xpm_map_t) ShipW_xpm;
  xpm_map_t ship_NE_xpm = (xpm_map_t) ShipNE_xpm;
  xpm_map_t ship_NW_xpm = (xpm_map_t) ShipNW_xpm;
  xpm_map_t ship_SE_xpm = (xpm_map_t) ShipSE_xpm;
  xpm_map_t ship_SW_xpm = (xpm_map_t) ShipSW_xpm;

  xpm_image_t bg_img, cursor_img;
  xpm_image_t ship_n_img, ship_s_img, ship_e_img, ship_w_img;
  xpm_image_t ship_ne_img, ship_nw_img, ship_se_img, ship_sw_img;

  if (
  xpm_load(bg_xpm     , XPM_5_6_5, &bg_img     ) == NULL ||
  xpm_load(cursor_xpm , XPM_5_6_5, &cursor_img ) == NULL ||
  xpm_load(ship_N_xpm , XPM_5_6_5, &ship_n_img ) == NULL ||
  xpm_load(ship_S_xpm , XPM_5_6_5, &ship_s_img ) == NULL ||
  xpm_load(ship_E_xpm , XPM_5_6_5, &ship_e_img ) == NULL ||
  xpm_load(ship_W_xpm , XPM_5_6_5, &ship_w_img ) == NULL ||
  xpm_load(ship_NE_xpm, XPM_5_6_5, &ship_ne_img) == NULL ||
  xpm_load(ship_NW_xpm, XPM_5_6_5, &ship_nw_img) == NULL ||
  xpm_load(ship_SE_xpm, XPM_5_6_5, &ship_se_img) == NULL ||
  xpm_load(ship_SW_xpm, XPM_5_6_5, &ship_sw_img) == NULL
  ) {
    exit_program(program_status);
    printf("Error when loading xpm.\n");
    return 1;
  }
  
  ship.n = ship_n_img;
  ship.s = ship_s_img;
  ship.e = ship_e_img;
  ship.w = ship_w_img;
  ship.ne = ship_ne_img;
  ship.nw = ship_nw_img;
  ship.se = ship_se_img;
  ship.sw = ship_sw_img;

  Sprite ship_sprite = { ship.n, PLAYER };
  Entity ship_entity = { ship_sprite, {500.0, 500.0}, {0.0, 0.0},
                         { - ship.n.width / 2, - ship.n.height / 2} };

  uint8_t num_entities = 1;
  Entity* entities[1] = { &ship_entity };

  Player player = { &ship_entity, PLAYER_MAX_HEALTH, 0, false };

  int ipc_status;
  message msg;

  /* Keyboard-related variables */
  keyboard_status kbd_status = { false, false, false, false };
  uint8_t bytes[2];
  bool two_byte_scancode = false;

  /* Mouse-related variables */
  int packet_byte_counter = 0;
  uint8_t packet_bytes[MOUSE_PCK_NUM_BYTES];
  mouse_status m_status = { false, false, false, vg_get_x_resolution() / 2, vg_get_y_resolution() / 2 };

  Sprite cursor_sprite = { cursor_img, MOUSE_CURSOR };
  MouseCursor mouse_cursor = { cursor_sprite, {0.0, 0.0}, {-cursor_img.width / 2, -cursor_img.height / 2}};

  void* aux_buffer = malloc(vg_get_frame_buffer_size());
  
  bool can_fire = true;
  uint8_t frames = 0;

  if (rtc_init()) {
    exit_program(program_status);
    printf("Error when calling rtc_init.\n");
    return 1;
  }
  while (scancode != KBD_ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("Error when calling driver_receive.\n");
      continue;
    }

    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(mouse_bit_no)) {
          mouse_ih();
          
          if ((packet_byte_counter != MOUSE_INDEX_FIRST_BYTE) || ((packet_byte & MOUSE_FIRST_BYTE_CHECK) != 0)) {
            packet_bytes[packet_byte_counter] = packet_byte;

            if (packet_byte_counter == MOUSE_INDEX_THIRD_BYTE) {
              packet_byte_counter = 0;
              process_mouse_packet(packet_bytes, &m_status);
            }
            else {
              ++packet_byte_counter;
            }
          }
        }
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
            
            process_kbd_scancode(bytes, &kbd_status);
            two_byte_scancode = false;
          }
        }
        if (msg.m_notify.interrupts & BIT(rtc_bit_no)) {
          rtc_ih();
          // Generate random position on screen.
          // Create new powerup entity at position.
        }
        if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
          timer_int_handler();

          if (counter == INTERRUPTS_PER_FRAME) {
            counter = 0;

            // Update values according to internal game logic.
            // Render a new frame.
            process_kbd_status(&kbd_status, &player);
            process_mouse_status(&m_status, &mouse_cursor, &player);

            if (!can_fire) {
              frames++;
              if (frames == FRAMES_PER_SHOT) {
                frames = 0;
                can_fire = true;
              }
            }
            else if (player.fire) {
              can_fire = false;
              // Shoot bullet
            }
            update_entity_positions(entities, num_entities);
            vg_draw_xpm(bg_img, 0, 0, &aux_buffer);
            vg_render_entities(entities, num_entities, &aux_buffer);
            vg_draw_xpm(cursor_img, round(mouse_cursor.position.x), round(mouse_cursor.position.y), &aux_buffer);
            memcpy(frame_buffer, aux_buffer, vg_get_frame_buffer_size());
          }
        }
        break;
      default:
        break;
      }
    }
  }

  free(aux_buffer);

  if (mouse_disable_int()) {
    exit_program(program_status);
    printf("Error when calling mouse_disable_int.\n");
    return 1;
  }

  if (mouse_disable_data_report()) {
    exit_program(program_status);
    printf("Error when calling mouse_disable_data_report.\n");
    return 1;
  }

  if (mouse_enable_int()) {
    exit_program(program_status);
    printf("Error when calling mouse_enable_int.\n");
    return 1;
  }

  if (mouse_unsubscribe_int()) {
    exit_program(program_status);
    printf("Error when calling mouse_unsubscribe_int.\n");
    return 1;
  }
  program_status->mouse_int_subscribed = false;

  if (kbc_reset_cmd_byte()) {
    exit_program(program_status);
    printf("Error when calling kbc_reset_cmd_byte.\n");
    return 1;
  }

  if (kbd_unsubscribe_int()) {
    exit_program(program_status);
    printf("Error when calling kbd_unsubscribe_int.\n");
    return 1;
  }
  program_status->kbd_int_subscribed = false;

  if (rtc_unsubscribe_int()) {
    exit_program(program_status);
    printf("Error when calling rtc_unsubscribe_int.\n");
    return 1;
  }
  program_status->rtc_int_subscribed = false;

  if (timer_unsubscribe_int()) {
    exit_program(program_status);
    printf("Error when calling timer_unsubscribe_int.\n");
    return 1;
  }
  program_status->timer_int_subscribed = false;

  if (vg_exit()) {
    printf("Error when calling vg_exit.\n");
    return 1;
  }
  program_status->vg_initialized = false;

  return 0;
}


void exit_program(program_status_t* status) {
  if (status->timer_int_subscribed) {
    timer_unsubscribe_int();
  }
  if (status->rtc_int_subscribed) {
    rtc_unsubscribe_int();
  }
  if (status->kbd_int_subscribed) {
    kbd_unsubscribe_int();
  }
  if (status->mouse_int_subscribed) {
    mouse_disable_int();
    mouse_disable_data_report();
    mouse_enable_int();
    mouse_unsubscribe_int();
    kbc_reset_cmd_byte();
  }
  if (status->vg_initialized) {
    vg_exit();
  }
}
