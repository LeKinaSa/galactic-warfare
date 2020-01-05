#include "serial_port.h"
#include "utils.h"

static int serial_port_hook_id = SP1_IRQ;
extern bool ready_to_transmit;

static uint8_t to_transmit[SP_FIFO_SIZE * SP_INT_PER_TIMER_INT];
static int to_transmit_size = 0;
static uint8_t received[SP_FIFO_SIZE * SP_INT_PER_TIMER_INT];
static int received_size = 0;

int sp_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  *bit_no = serial_port_hook_id;

  if (sys_irqsetpolicy(SP1_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &serial_port_hook_id) != OK) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }
  
  return 0;
}

int sp_unsubscribe_int() {
  if (sys_irqrmpolicy(&serial_port_hook_id) != OK) {
    printf("Error when calling sys_irqrmpolicy.\n");
    return 1;
  }

  return 0;
}

int sp_config(uint32_t bit_rate) {
  /* Configure IER */
  if (sys_outb(SP1_BASE_ADDR + SP_IER, SP_IER_CONFIG)) {
    printf("Error when configuring Interrupt Enable Register.\n");
    return 1;
  }

  /* Configure LCR (to configure DL)*/
  if (sys_outb(SP1_BASE_ADDR + SP_LCR, SP_LCR_CONFIG | SP_LCR_DLAB)) {
    printf("Error when configuring Line Control Register.\n");
    return 1;
  }

  /* Configure DL Value */
  uint16_t dl_value = SP_DL_VALUE(bit_rate);
  uint8_t lsb = 0, msb = 0;
  util_get_LSB(dl_value, &lsb);
  util_get_MSB(dl_value, &msb);

  if (sys_outb(SP1_BASE_ADDR + SP_DLAB_DLL, lsb)) {
    printf("Error when writing LSB to DLL.\n");
    return 1;
  }
  if (sys_outb(SP1_BASE_ADDR + SP_DLAB_DLM, msb)) {
    printf("Error when writing MSB to DLL.\n");
    return 1;
  }

  /* Reconfigure LCR */
  if (sys_outb(SP1_BASE_ADDR + SP_LCR, SP_LCR_CONFIG)) {
    printf("Error when configuring Line Control Register.\n");
    return 1;
  }

  /* Configure FIFO */
  if (sys_outb(SP1_BASE_ADDR + SP_FCR, SP_FCR_CONFIG)) {
    printf("Error when configuring FIFO Control Register.\n");
    return 1;
  }

  return 0;
}

void sp_int_handler() {
  uint8_t iir = 0, error = 0;
  util_sys_inb(SP1_BASE_ADDR + SP_IIR, &iir);
  switch ( iir & SP_IIR_INT ) {
      case SP_IIR_RDR:
        printf("Read  "); // RETIRAR
        /* Read next char */
        sp_receive();
        break;
      case SP_IIR_THR:
        printf("Transmit  "); // RETIRAR
        /* Transmit next char */
        if (to_transmit_size == 0) {
          ready_to_transmit = true;
        }
        else {
          sp_transmit();
          ready_to_transmit = false;
        }
        break;
      case SP_IIR_CTO:
        printf("Timeout  "); // RETIRAR
        /* Character Timeout */
        sp_receive();
        break;
      case SP_IIR_RLS:
        printf("Error  "); // RETIRAR
        /* Receiver Line Status - Signals Error in LSR */
        util_sys_inb(SP1_BASE_ADDR + SP_LSR, &error);
        sp_add_to_transmission_queue(SP_SEND_SEQUENCE);
        if (ready_to_transmit) {
          sp_transmit();
        }
        break;
      default:
        printf("Modem  "); // RETIRAR
        break;
  }
  util_sys_inb(SP1_BASE_ADDR + SP_IIR, &iir);
  if (( iir & SP_IIR_INT ) == SP_IIR_THR) {
    ready_to_transmit = true;
  }
}

int sp_send_again() {
  if ((received_size != 0) && (received[received_size - 1] == SP_SEND_SEQUENCE)) {
    return 1;
  }
  return 0;
}

void sp_treat_information_received(Player *player, Powerup *powerup, bool *generate_powerup, bool *spawn_bullet) {
  *generate_powerup = false;
  *spawn_bullet = false;
  printf("%d\n", received_size); // RETIRAR
  if (received_size == 0) {
    return;
  }
  uint8_t rtc_queue[SP_RTC_SIZE - 1], player_queue[SP_PLAYER_SIZE - 1];
  int rtc_size, player_size;

  uint8_t msb_x, lsb_x, msb_y, lsb_y;
  uint8_t angle1 = 0, angle2 = 0, angle3 = 0, angle4 = 0;
  //uint8_t type;
  uint16_t x = 0, y = 0;
  union angle_to_transmit angle;

  /* Get the Bullet and the Arrays for Player and RTC Info */
  sp_treat_received_queue(player_queue, &player_size, rtc_queue, &rtc_size, spawn_bullet);
  
  /* Get PowerUp */
  if (rtc_size != SP_RTC_SIZE - 1) {
    *generate_powerup = false;
  }
  else {
    /*
    *generate_powerup = true;
    // Coordinates and Type
    msb_x = rtc_queue[0];
    lsb_x = rtc_queue[1];
    msb_y = rtc_queue[2];
    lsb_y = rtc_queue[3];
    type  = rtc_queue[4];
    util_get_val(&x, msb_x, lsb_x);
    util_get_val(&y, msb_y, lsb_y);
    // Put the Coordinates and the Type on the PowerUp 
    powerup->entity->position.x = x;
    powerup->entity->position.y = y;
    powerup->type = (enum powerup_type) type;*/
  }

  /* Get Player */
  printf("PLAYER\n"); // RETIRAR
  if (player_size != SP_PLAYER_SIZE - 1) {
    return;
  }
  printf("Still Player\n"); // RETIRAR
    /* Coordinates */
  msb_x  = player_queue[0];
  lsb_x  = player_queue[1];
  msb_y  = player_queue[2];
  lsb_y  = player_queue[3];
  util_get_val(&x, msb_x, lsb_x);
  util_get_val(&y, msb_y, lsb_y);
    /* Angle */
  angle1 = player_queue[4];
  angle2 = player_queue[5];
  angle3 = player_queue[6];
  angle4 = player_queue[7];
  util_join_parts(&(angle.transmit), angle1, angle2, angle3, angle4);
    /* Put the Coordinates and the Angle on the Player */
  player->entity->position.x = x;
  player->entity->position.y = y;
  player->angle = angle.angle;
  printf("%x %x\n", x, y);  // RETIRAR
  printf("        %x\n", angle.transmit);  // RETIRAR
}

void sp_new_transmission(Player* player, Powerup* powerup, bool generate_powerup, bool spawn_bullet) {
  to_transmit_size = 0;
  sp_add_sequence_to_transmission(player, powerup, generate_powerup, spawn_bullet);
}

void sp_retransmit_sequence(Player* player, Powerup* powerup, bool generate_powerup, bool spawn_bullet) {
  sp_add_to_transmission_queue(SP_NEW_SEQUENCE);
  sp_add_sequence_to_transmission(player, powerup, generate_powerup, spawn_bullet);
}

void sp_add_to_transmission_queue(uint8_t byte) {
  to_transmit[to_transmit_size] = byte;
  ++ to_transmit_size;
}

void sp_treat_received_queue(uint8_t player_queue[], int *player_size,
                             uint8_t rtc_queue[], int *rtc_size, bool *spawn_bullet) {
  static enum last_read last = COMPLETE;
  uint8_t next_char;

  for (int index = 0; index < received_size; ++ index) {
    next_char = received[index];
    switch (last) {
      case COMPLETE:
        switch (next_char) {
          case NEW_SEQUENCE:
            *player_size = 0;
            *rtc_size = 0;
            *spawn_bullet = false;
            last = COMPLETE;
            break;
          case SP_ENEMY_PLAYER:
            *player_size = 0;
            last = PLAYER_SIZE_0;
            break;
          case SP_RTC_INTERRUPT:
            *rtc_size = 0;
            last = RTC_SIZE_0;
            break;
          case SP_GENERATE_BULLET:
            *spawn_bullet = true;
            break;
          default:
            break;
        }
        break;
      
      case PLAYER_SIZE_0:
        player_queue[*player_size] = next_char;
        ++ (*player_size);
        last = PLAYER_SIZE_1;
        break;
      case PLAYER_SIZE_1:
        player_queue[*player_size] = next_char;
        ++ (*player_size);
        last = PLAYER_SIZE_2;
        break;
      case PLAYER_SIZE_2:
        player_queue[*player_size] = next_char;
        ++ (*player_size);
        last = PLAYER_SIZE_3;
        break;
      case PLAYER_SIZE_3:
        player_queue[*player_size] = next_char;
        ++ (*player_size);
        last = PLAYER_SIZE_4;
        break;
      case PLAYER_SIZE_4:
        player_queue[*player_size] = next_char;
        ++ (*player_size);
        last = PLAYER_SIZE_5;
        break;
      case PLAYER_SIZE_5:
        player_queue[*player_size] = next_char;
        ++ (*player_size);
        last = PLAYER_SIZE_6;
        break;
      case PLAYER_SIZE_6:
        player_queue[*player_size] = next_char;
        ++ (*player_size);
        last = PLAYER_SIZE_7;
        break;
      case PLAYER_SIZE_7:
        player_queue[*player_size] = next_char;
        ++ (*player_size);
        last = COMPLETE;
        break;

      case RTC_SIZE_0:
        rtc_queue[*rtc_size] = next_char;
        ++ (*rtc_size);
        last = RTC_SIZE_1;
        break;
      case RTC_SIZE_1:
        rtc_queue[*rtc_size] = next_char;
        ++ (*rtc_size);
        last = RTC_SIZE_2;
        break;
      case RTC_SIZE_2:
        rtc_queue[*rtc_size] = next_char;
        ++ (*rtc_size);
        last = RTC_SIZE_3;
        break;
      case RTC_SIZE_3:
        rtc_queue[*rtc_size] = next_char;
        ++ (*rtc_size);
        last = RTC_SIZE_4;
        break;
      case RTC_SIZE_4:
        rtc_queue[*rtc_size] = next_char;
        ++ (*rtc_size);
        last = COMPLETE;
        break;
      
      default:
        break;
    }
  }
  received_size = 0;
}

void sp_add_sequence_to_transmission(Player* player, Powerup* powerup, bool generate_powerup, bool spawn_bullet) {
  uint8_t msb_x, lsb_x, msb_y, lsb_y;
  uint8_t angle1 = 0, angle2 = 0, angle3 = 0, angle4 = 0;
  uint8_t type;
  union angle_to_transmit angle;

  /* Player */
  sp_add_to_transmission_queue(SP_ENEMY_PLAYER);
    /* Coordinates */
  util_get_MSB(player->entity->position.x, &msb_x);
  util_get_LSB(player->entity->position.x, &lsb_x);
  util_get_MSB(player->entity->position.y, &msb_y);
  util_get_LSB(player->entity->position.y, &lsb_y);
  sp_add_to_transmission_queue(msb_x);
  sp_add_to_transmission_queue(lsb_x);
  sp_add_to_transmission_queue(msb_y);
  sp_add_to_transmission_queue(lsb_y);
    /* Angle */
  angle.angle = player->angle;
  util_get_parts(angle.transmit, &angle1, &angle2, &angle3, &angle4);
  sp_add_to_transmission_queue(angle1);
  sp_add_to_transmission_queue(angle2);
  sp_add_to_transmission_queue(angle3);
  sp_add_to_transmission_queue(angle4);

  /* RTC */
  if (generate_powerup) {
    sp_add_to_transmission_queue(SP_RTC_INTERRUPT);
    util_get_MSB(powerup->entity->position.x, &msb_x);
    util_get_LSB(powerup->entity->position.x, &lsb_x);
    util_get_MSB(powerup->entity->position.y, &msb_y);
    util_get_LSB(powerup->entity->position.y, &lsb_y);
    type = powerup->type;
    sp_add_to_transmission_queue(msb_x);
    sp_add_to_transmission_queue(lsb_x);
    sp_add_to_transmission_queue(msb_y);
    sp_add_to_transmission_queue(lsb_y);
    sp_add_to_transmission_queue(type);
  }

  /* Bullet */
  if (spawn_bullet) {
    sp_add_to_transmission_queue(SP_GENERATE_BULLET);
  }

  /* End of Sequence */
  sp_add_to_transmission_queue(SP_END_SEQUENCE);
}

void sp_check_ready_to_transmit() {
  uint8_t lsr = 0;
  util_sys_inb(SP1_BASE_ADDR + SP_LSR, &lsr);
  if (( lsr & SP_LSR_THR_EMPTY ) != 0) {
    ready_to_transmit = true;
  }
}

void sp_transmit() {
  uint8_t byte = 0;
  int minimum = min(SP_FIFO_SIZE, to_transmit_size);
  for (int index = 0; index < minimum; ++ index) {
    byte = to_transmit[index];
    sys_outb(SP1_BASE_ADDR + SP_THR, byte);
  }
  util_erase(to_transmit, &to_transmit_size, minimum);
}

void sp_receive() {
  uint8_t lsr = 0, byte = 0;

  int packets = 0; // RETIRAR

  util_sys_inb(SP1_BASE_ADDR + SP_LSR, &lsr);
  while ((lsr & SP_LSR_RD) != 0) {
    util_sys_inb(SP1_BASE_ADDR + SP_RBR, &byte);
    received[received_size] = byte;
    ++ received_size;
    util_sys_inb(SP1_BASE_ADDR + SP_LSR, &lsr);
    if (received_size >= (SP_FIFO_SIZE * SP_INT_PER_TIMER_INT) / 2) {
      received_size = 0;
    }
    ++ packets; // RETIRAR
  }
  printf("Received : %d out of %d packets\n", received_size, packets); // RETIRAR
}

void sp_clear_error() {
  printf("Caught a Bad Error\n"); // RETIRAR
  uint8_t iir = 0;
  util_sys_inb(SP1_BASE_ADDR + SP_IIR, &iir);
  if (( iir & SP_IIR_INT ) == SP_IIR_ERROR) {
    uint8_t lsr = 0, byte = 0;
    util_sys_inb(SP1_BASE_ADDR + SP_LSR, &lsr);
    while ((lsr & SP_LSR_RD) != 0) {
      util_sys_inb(SP1_BASE_ADDR + SP_RBR, &byte);
      util_sys_inb(SP1_BASE_ADDR + SP_LSR, &lsr);
    }
  sp_add_to_transmission_queue(SP_SEND_SEQUENCE);
  }
}
