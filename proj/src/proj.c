// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <lcom/liblm.h>
#include <lcom/proj.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

// Any header files included below this line should have been created by you
#include "game_logic.h"
#include "game_constants.h"
#include "vbe_constants.h"
#include "i8042.h"
#include "video.h"
#include "keyboard.h"
#include "mouse.h"
#include "dispatcher.h"
#include "utils.h"
#include "rtc.h"
#include "serial_port.h"

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
#include "../res/SpeedPowerup.xpm"
#include "../res/DamagePowerup.xpm"
#include "../res/Bullet.xpm"
#include "../res/BulletEnemy.xpm"

/** @brief Structure representing the current state of the program.
 * Useful for knowing what function calls should be made before exiting the program. */
typedef struct {
  bool timer_int_subscribed;  /**< @brief Timer interrupts subscribed */
  bool rtc_int_subscribed;    /**< @brief RTC interrupts subscribed */
  bool kbd_int_subscribed;    /**< @brief Keyboard interrupts subscribed */
  bool mouse_int_subscribed;  /**< @brief Mouse interrupts subscribed */
  bool sp_int_subscribed;     /**< @brief Serial port interrupts subscribed */
  bool vg_initialized;        /**< @brief Video mode and frame buffer initialized */
} program_status_t;

/* Video Related Variables */
void* frame_buffer;
/* Timer Related Variables */
int counter = 0;
/* Keyboard Related Variables */
uint8_t scancode = 0;
/* Mouse Related Variables */
uint8_t packet_byte;
/* RTC Related Variables */
uint8_t minute_counter = 0;
/* Serial Port Related Variables */
bool ready_to_transmit = false;
/* XPM Related Variables (for animation) */
xpm_animated ship;

static const xpm_map_t bg_xpm = (xpm_map_t) Background_xpm;
static const xpm_map_t cursor_xpm = (xpm_map_t) Cursor_xpm;

static const xpm_map_t ship_N_xpm = (xpm_map_t) ShipN_xpm;
static const xpm_map_t ship_S_xpm = (xpm_map_t) ShipS_xpm;
static const xpm_map_t ship_E_xpm = (xpm_map_t) ShipE_xpm;
static const xpm_map_t ship_W_xpm = (xpm_map_t) ShipW_xpm;
static const xpm_map_t ship_NE_xpm = (xpm_map_t) ShipNE_xpm;
static const xpm_map_t ship_NW_xpm = (xpm_map_t) ShipNW_xpm;
static const xpm_map_t ship_SE_xpm = (xpm_map_t) ShipSE_xpm;
static const xpm_map_t ship_SW_xpm = (xpm_map_t) ShipSW_xpm;

static const xpm_map_t speed_powerup_xpm = (xpm_map_t) SpeedPowerup_xpm;
static const xpm_map_t damage_powerup_xpm = (xpm_map_t) DamagePowerup_xpm;

static const xpm_map_t bullet_xpm = (xpm_map_t) Bullet_xpm;
static const xpm_map_t bullet_enemy_xpm = (xpm_map_t) BulletEnemy_xpm;

static void exit_program(program_status_t* status);

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // Log function invocations that are being "wrapped" by LCF
  //lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // Save the output of printf function calls on a file
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
  static const char host_str[] = "host", remote_str[] = "remote";
  
  if (argc != 1) {
    printf("Usage: lcom_run proj <host/remote>\n");
    return 1;
  }
  
  bool host;
  
  if (strcmp(argv[0], host_str) == 0) {
    host = true;
  }
  else if (strcmp(argv[0], remote_str) == 0) {
    host = false;
  }
  else {
    printf("Usage: lcom_run proj <host/remote>\n");
    return 1;
  }

  uint8_t timer_bit_no = 0, rtc_bit_no = 0, kbd_bit_no = 0, mouse_bit_no = 0, sp_bit_no = 0;
  
  /* Creates a program_status_t structure with all boolean values set to false */
  program_status_t* program_status = (program_status_t*) malloc(sizeof(program_status_t));
  memset((void *) program_status, false, sizeof(program_status_t));

  srand(time(NULL));

  if (timer_subscribe_int(&timer_bit_no)) {
    printf("Error when calling timer_subscribe_int.\n");
    return 1;
  }
  program_status->timer_int_subscribed = true;

  if (host) {
    if (rtc_subscribe_int(&rtc_bit_no)) {
      exit_program(program_status);
      printf("Error when calling rtc_subscribe_int.\n");
      return 1;
    }
    program_status->rtc_int_subscribed = true;
  }

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

  if (sp_subscribe_int(&sp_bit_no)) {
    exit_program(program_status);
    printf("Error when calling sp_subscribe_int.\n");
    return 1;
  }
  program_status->sp_int_subscribed = true;

  if (sp_config(SP_BIT_RATE)) {
    exit_program(program_status);
    printf("Error when calling sp_config.\n");
    return 1;
  }

  frame_buffer = vg_init(MODE_PROJECT);

  if (frame_buffer == MAP_FAILED) {
    exit_program(program_status);
    return 1;
  }
  program_status->vg_initialized = true;

  xpm_image_t bg_img, cursor_img;
  xpm_image_t ship_n_img, ship_s_img, ship_e_img, ship_w_img;
  xpm_image_t ship_ne_img, ship_nw_img, ship_se_img, ship_sw_img;
  xpm_image_t speed_powerup_img, damage_powerup_img;
  xpm_image_t bullet_img, bullet_enemy_img;

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
  xpm_load(ship_SW_xpm, XPM_5_6_5, &ship_sw_img) == NULL ||
  xpm_load(speed_powerup_xpm, XPM_5_6_5, &speed_powerup_img) == NULL ||
  xpm_load(damage_powerup_xpm, XPM_5_6_5, &damage_powerup_img) == NULL ||
  xpm_load(bullet_xpm, XPM_5_6_5, &bullet_img) == NULL ||
  xpm_load(bullet_enemy_xpm, XPM_5_6_5, &bullet_enemy_img) == NULL
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

  Triangle* ship_collision_shape = Triangle_new((Vector2) {-10.0, -5.0}, (Vector2) {0.0, 20.0}, (Vector2) {10.0, -5.0});
  Sprite ship_sprite = { ship.s, TRIANGLE, ship_collision_shape };

  LinkedList* bullets = LinkedList_new(sizeof(Bullet));
  double x, y, enemy_x, enemy_y;

  if (host) {
    x = 500.0;
    y = 500.0;
    enemy_x = 200.0;
    enemy_y = 200.0;
  }
  else {
    x = 200.0;
    y = 200.0;
    enemy_x = 500.0;
    enemy_y = 500.0;
  }

  Entity ship1_entity = { ship_sprite, {x, y}, {0.0, 0.0}, { - ship.n.width / 2, - ship.n.height / 2} };
  Entity ship2_entity = { ship_sprite, {enemy_x, enemy_y}, {0.0, 0.0}, { - ship.n.width / 2, - ship.n.height / 2} };
  
  Entity* ship1_entity_ptr = &ship1_entity;
  Entity* ship2_entity_ptr = &ship2_entity;

  Player player = { ship1_entity_ptr, PLAYER_MAX_HEALTH, PLAYER_BASE_SPEED, 0.0, false };
  Player enemy  = { ship2_entity_ptr, PLAYER_MAX_HEALTH, PLAYER_BASE_SPEED, 0.0, false };
  bool can_fire = true;
  uint8_t frames = 0;
  bool spawn_player_bullet = false, spawn_enemy_bullet = false;

  Circle* bullet_collision_shape = Circle_new(8.0);
  Sprite bullet_sprite = { bullet_img, CIRCLE, bullet_collision_shape };
  Sprite bullet_enemy_sprite = { bullet_enemy_img, CIRCLE, bullet_collision_shape };

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

  Sprite cursor_sprite = { cursor_img, NO_SHAPE, NULL };
  MouseCursor mouse_cursor = { cursor_sprite, {0.0, 0.0}, {-cursor_img.width / 2, -cursor_img.height / 2}};

  void* aux_buffer = malloc(vg_get_frame_buffer_size());

  Circle* powerup_collision_shape = Circle_new(16.0);
  const Sprite speed_powerup_sprite = (Sprite) { speed_powerup_img, CIRCLE, powerup_collision_shape };
  const Sprite damage_powerup_sprite = (Sprite) { damage_powerup_img, CIRCLE, powerup_collision_shape };
  const uint16_t powerup_sprite_width = speed_powerup_img.width, powerup_sprite_height = speed_powerup_img.height;
  uint16_t powerup_x = 0, powerup_y = 0;
  enum powerup_type type = SPEED;

  Powerup* current_powerup = NULL;
  Entity powerup_entity;

  powerup_entity.velocity = (Vector2) { 0.0, 0.0 };
  powerup_entity.sprite = speed_powerup_sprite;

  bool generate_powerup = false, generate_enemy_powerup = false;
  bool rtc_interrupted;

  if (rtc_init()) {
    exit_program(program_status);
    printf("Error when calling rtc_init.\n");
    return 1;
  }

  sp_check_ready_to_transmit();
  
  uint8_t iir = 0; // RETIRAR
  
  while (scancode != KBD_ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("Error when calling driver_receive.\n");
      continue;
    }

    rtc_interrupted = false;
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
        if ((msg.m_notify.interrupts & BIT(rtc_bit_no)) && (host)) {
          printf("RTC\n"); // RETIRAR
          rtc_int_handler();
          if (minute_counter == POWERUP_INTERVAL) {
            minute_counter = 0;
            // Generate random position on screen.
            // Create new powerup entity at position.
            Powerup_delete(current_powerup);
            current_powerup = NULL;

            powerup_entity.position = generate_random_pos(vg_get_x_resolution() - powerup_sprite_width, 
            vg_get_y_resolution() - powerup_sprite_height);
            current_powerup = Powerup_new(&powerup_entity, (enum powerup_type) (rand() % 2));
              
            if (current_powerup->type == SPEED) {
              powerup_entity.sprite = speed_powerup_sprite;
            }
            else {
              powerup_entity.sprite = damage_powerup_sprite;
            }

            rtc_interrupted = true;
          }
        }
        if (msg.m_notify.interrupts & BIT(sp_bit_no)) {
          printf("SP INT : "); // RETIRAR
          sp_int_handler(host);
          util_sys_inb(SP1_BASE_ADDR + SP_IIR, &iir); // RETIRAR
          sp_clear_error();
          printf("0x%x\n", iir); // RETIRAR
          if (sp_send_again()) {
            sp_retransmit_sequence(&player, current_powerup, generate_powerup, spawn_player_bullet);
          }
        }
        if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
          printf("TIMER\n"); // RETIRAR
          timer_int_handler();
          
          generate_powerup = false;
          if (rtc_interrupted) {
            generate_powerup = true;
          }

          if (counter == INTERRUPTS_PER_FRAME) {
            counter = 0;

            // Update values according to internal game logic.
            // Render a new frame.
            printf("Info\n"); // RETIRAR
            sp_treat_information_received(&enemy, &powerup_x, &powerup_y, &type, &generate_enemy_powerup, &spawn_enemy_bullet);
            printf("\n\n\n"); // RETIRAR
            if (generate_enemy_powerup) {

              /* Generate the PowerUp Coming from the Serial Port */
              printf("Powerup Generated\n"); // RETIRAR
            }
            if (spawn_enemy_bullet) {
              printf("Spawn Bullet\n"); // RETIRAR
              /* Spawn the Bullet on the Enemy Player */
              Vector2 bullet_position = Vector2_subtract(Vector2_subtract(enemy.entity->position, enemy.entity->offset), 
              (Vector2) {bullet_enemy_sprite.img.width / 2, bullet_enemy_sprite.img.height / 2});
              Vector2 bullet_velocity = Vector2_scalar_mult(BULLET_SPEED, rotate_point((Vector2) {1.0, 0.0}, enemy.angle));

              Bullet bullet = { {bullet_enemy_sprite, bullet_position, bullet_velocity, {0.0, 0.0}}, PLAYER_TWO, PLAYER_BASE_DAMAGE };

              LinkedList_add(bullets, &bullet);
            }
            
            printf("Process\n"); // RETIRAR
            process_kbd_status(&kbd_status, &player);
            process_mouse_status(&m_status, &mouse_cursor, &player);

            spawn_player_bullet = false;
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
              Vector2 bullet_position = Vector2_subtract(Vector2_subtract(player.entity->position, player.entity->offset), 
              (Vector2) {bullet_sprite.img.width / 2, bullet_sprite.img.height / 2});
              Vector2 bullet_velocity = Vector2_scalar_mult(BULLET_SPEED, rotate_point((Vector2) {1.0, 0.0}, player.angle));

              Bullet bullet = { { bullet_sprite, bullet_position, bullet_velocity, {0.0, 0.0} }, 
              PLAYER_ONE, PLAYER_BASE_DAMAGE };

              LinkedList_add(bullets, &bullet);
              spawn_player_bullet = true;
            }

            update_entity_positions(bullets, &player);
            detect_collisions(bullets, &current_powerup, &player);
            vg_draw_xpm(bg_img, 0, 0, &aux_buffer);
            vg_render_entities(bullets, current_powerup, &player, &enemy, &aux_buffer);
            vg_draw_xpm(cursor_img, round(mouse_cursor.position.x), round(mouse_cursor.position.y), &aux_buffer);
            memcpy(frame_buffer, aux_buffer, vg_get_frame_buffer_size());
            printf("End of Process\n"); // RETIRAR
            /* Next Sequence to Be Transmitted By the Serial Port */
            sp_new_transmission(&player, current_powerup, generate_powerup, spawn_player_bullet);
            if (ready_to_transmit) {
              sp_transmit();
              printf("transmission succeed.\n"); // RETIRAR
            }
            printf("New Sequence Sent\n"); // RETIRAR
          }
        }
        break;
      default:
        break;
      }
    }
  }
  
  // Free dynamically allocated memory
  free(aux_buffer);
  LinkedList_delete(bullets);
  free(ship_collision_shape);
  free(bullet_collision_shape);
  free(powerup_collision_shape);

  if (sp_unsubscribe_int()) {
    exit_program(program_status);
    printf("Error when calling sp_unsubscribe_int.\n");
    return 1;
  }
  program_status->sp_int_subscribed = false;
  
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

  if (host) {
    if (rtc_unsubscribe_int()) {
      exit_program(program_status);
      printf("Error when calling rtc_unsubscribe_int.\n");
      return 1;
    }
    program_status->rtc_int_subscribed = false;
  }

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
  if (status->sp_int_subscribed) {
    sp_unsubscribe_int();
  }
  if (status->vg_initialized) {
    vg_exit();
  }
}
