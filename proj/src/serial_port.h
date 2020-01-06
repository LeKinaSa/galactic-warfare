#ifndef __SERIAL_PORT_H
#define __SERIAL_PORT_H

#include <lcom/lcf.h>

#include "uart.h"
#include "game_logic.h"

/** @defgroup serial_port serial_port
 * @{
 * @brief Functions and data structures related to the serial port
 */

/**
 * @brief Identify where we are in the received vector treatment.
 */
enum last_read {
  COMPLETE,             /**< @brief Last packet is complete */
  NEW_SEQUENCE,         /**< @brief New Sequence */
  SEND_SEQUENCE,        /**< @brief Send Sequence Again */

  RTC_SIZE_0,           /**< @brief RTC coordinates and type (0 bytes read) */
  RTC_SIZE_1,           /**< @brief RTC coordinates and type (1 byte  read) */
  RTC_SIZE_2,           /**< @brief RTC coordinates and type (2 bytes read) */
  RTC_SIZE_3,           /**< @brief RTC coordinates and type (3 bytes read) */
  RTC_SIZE_4,           /**< @brief RTC coordinates and type (4 bytes read) */
  
  PLAYER_SIZE_0,        /**< @brief Player coordinates and angle (0 bytes read) */
  PLAYER_SIZE_1,        /**< @brief Player coordinates and angle (1 byte  read) */
  PLAYER_SIZE_2,        /**< @brief Player coordinates and angle (2 bytes read) */
  PLAYER_SIZE_3,        /**< @brief Player coordinates and angle (3 bytes read) */
  PLAYER_SIZE_4,        /**< @brief Player coordinates and angle (4 bytes read) */
  PLAYER_SIZE_5,        /**< @brief Player coordinates and angle (5 bytes read) */
  PLAYER_SIZE_6,        /**< @brief Player coordinates and angle (6 bytes read) */
  PLAYER_SIZE_7         /**< @brief Player coordinates and angle (7 bytes read) */
};

/**
 * @brief Help to transform the angle float into bytes to transmit and the bytes to float when receiving.
 */
union angle_to_transmit {
  float angle;
  uint32_t transmit;
};

/**
 * @brief Subscribes serial port interrupts using sys_irqsetpolicy.
 * @param bit_no    pointer to variable where the interrupt bit number will be stored
 * @return          zero if no errors occurred, non-zero otherwise
 */
int sp_subscribe_int(uint8_t *bit_no);

/**
 * @brief Unsubscribes serial port interrupts using sys_irqrmpolicy.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int sp_unsubscribe_int();

/**
 * @brief Initializes serial port and configures it.
 * @param bit_rate  Bit rate used in the serial port configuration and communication.
 * @return          zero if no errors occurred, non-zero otherwise
 */
int sp_config(uint32_t bit_rate);

/**
 * @brief Serial port interrupt handler.
 * When an interrupt occurs, identify the type of interrupt and process it accordingly.
 */
void sp_int_handler();

/**
 * @brief Serial port communication error detector.
 * If there an error on the other machine, this will detect 
 */
int sp_send_again();

/**
 * @brief Treat information received in the serial port.
 * @param player              Enemy player received from the serial port
 * @param rtc_x               Coordinate X for the generated powerup
 * @param rtc_y               Coordinate Y for the generated powerup
 * @param rtc_type            Type of the powerup generated on the other virtual machine
 * @param generate_powerup    true if the other machine generated a powerup, false if it doesn't
 * @param spawn_bullet        true if the other player fired, false if it doesn't
 */
void sp_treat_information_received(Player *player, uint16_t *rtc_x, uint16_t *rtc_y, enum powerup_type *rtc_type, bool *generate_powerup, bool *spawn_bullet);

/**
 * @brief Update the transmission with new information.
 * @param player              Player to be transmitted via serial port
 * @param powerup             Powerup generated in this virtual machine
 * @param generate_powerup    true if the this machine generated a powerup, false if it doesn't
 * @param spawn_bullet        true if the this player fired, false if it doesn't
 */
void sp_new_transmission(Player *player, Powerup *powerup, bool generate_powerup, bool spawn_bullet);

/**
 * @brief Update the transmission with a new sequence but the same information.
 * @param player              Player to be transmitted via serial port
 * @param powerup             Powerup generated in this virtual machine
 * @param generate_powerup    true if the this machine generated a powerup, false if it doesn't
 * @param spawn_bullet        true if the this player fired, false if it doesn't
 */
void sp_retransmit_sequence(Player *player, Powerup *powerup, bool generate_powerup, bool spawn_bullet);

/**
 * @brief Add a byte to the transmission.
 * @param     byte    Byte to be added
 */
void sp_add_to_transmission_queue(uint8_t byte);

/**
 * @brief Transform received vector in information for player, rtc and bullet.
 * @param player_queue      Array with space to put all player information
 * @param player_size       Size of the previous array
 * @param rtc_queue         Array with space to put all player information
 * @param rtc_size          Size of the previous array
 * @param spawn_bullet      Detects bullet generation
 */
void sp_treat_received_queue(uint8_t player_queue[], int *player_size,
                             uint8_t rtc_queue[], int *rtc_size, bool *spawn_bullet);

/**
 * @brief Transform player, powerup and bullet information into bytes to be transmitted.
 * @param player              Player to be transmitted via serial port
 * @param powerup             Powerup generated in this virtual machine
 * @param generate_powerup    true if the this machine generated a powerup, false if it doesn't
 * @param spawn_bullet        true if the this player fired, false if it doesn't
 */
void sp_add_sequence_to_transmission(Player *player, Powerup *powerup, bool generate_powerup, bool spawn_bullet);

/**
 * @brief Initial check to understand if the serial port is ready to transmit in the beginning of the programm.
 */
void sp_check_ready_to_transmit();

/**
 * @brief Transmit the vector to the Transmitter Holding Register
 */
void sp_transmit();

/**
 * @brief Transmit the vector to the Transmitter Holding Register By Polling
 */
void sp_transmit_polled();

/**
 * @brief Receive the Receiver Holding Register to a vector
 */
void sp_receive();

void sp_transmit_polled_2();

#endif /* __SERIAL_PORT_H */
