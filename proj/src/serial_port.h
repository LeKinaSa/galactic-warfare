#ifndef __SERIAL_PORT_H
#define __SERIAL_PORT_H

#include <lcom/lcf.h>

#include "uart.h"
#include "game_logic.h"

enum last_read {
  COMPLETE,             /* Last packet is complete */
  NEW_SEQUENCE,         /* New Sequence */
  SEND_SEQUENCE,        /* Send Sequence Again */

  RTC_SIZE_0,           /* RTC coordinates and type (0 bytes read) */
  RTC_SIZE_1,           /* RTC coordinates and type (1 byte  read) */
  RTC_SIZE_2,           /* RTC coordinates and type (2 bytes read) */
  RTC_SIZE_3,           /* RTC coordinates and type (3 bytes read) */
  RTC_SIZE_4,           /* RTC coordinates and type (4 bytes read) */
  
  PLAYER_SIZE_0,        /* Player coordinates and angle (0 bytes read) */
  PLAYER_SIZE_1,        /* Player coordinates and angle (1 byte  read) */
  PLAYER_SIZE_2,        /* Player coordinates and angle (2 bytes read) */
  PLAYER_SIZE_3,        /* Player coordinates and angle (3 bytes read) */
  PLAYER_SIZE_4,        /* Player coordinates and angle (4 bytes read) */
  PLAYER_SIZE_5,        /* Player coordinates and angle (5 bytes read) */
  PLAYER_SIZE_6,        /* Player coordinates and angle (6 bytes read) */
  PLAYER_SIZE_7         /* Player coordinates and angle (7 bytes read) */
};

int sp_subscribe_int(uint8_t *bit_no);

int sp_unsubscribe_int();

int sp_config(uint32_t bit_rate);

void sp_int_handler();

int sp_send_again();

void sp_treat_information_received(Player* player, Powerup* powerup, bool* generate_powerup, bool* spawn_bullet);

void sp_new_transmission(Player* player, Powerup* powerup, bool generate_powerup, bool spawn_bullet);

void sp_retransmit_sequence(Player* player, Powerup* powerup, bool generate_powerup, bool spawn_bullet);

int sp_sys_inb(int port, uint8_t *value);

void sp_add_to_transmission_queue(uint8_t byte);

void sp_treat_received_queue(uint8_t rtc_queue[], int *rtc_size,
                             uint8_t player_queue[], int *player_size, bool *spawn_bullet);

void sp_add_sequence_to_transmission(Player* player, Powerup* powerup, bool generate_powerup, bool spawn_bullet);

#endif /* __SERIAL_PORT_H */
