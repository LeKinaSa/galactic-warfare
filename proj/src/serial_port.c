#include "serial_port.h"
#include "utils.h"

static int serial_port_hook_id = SP1_IRQ;

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

  if (sys_irqsetpolicy(SP1_IRQ, IRQ_REENABLE, &serial_port_hook_id) != OK) {
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
  uint8_t byte = 0;
  uint8_t lsr;
  
  util_sys_inb(SP1_BASE_ADDR + SP_IIR, &iir);
  
  switch ( iir & SP_IIR_INT ) {
      case SP_IIR_RDR:
        /* Read next char */
        util_sys_inb(SP1_BASE_ADDR + SP_LSR, &lsr);
        while (lsr & SP_LSR_RD) {
          sp_sys_inb(SP1_BASE_ADDR + SP_RBR, &byte);
          received[received_size] = byte;
          ++ received_size;
          util_sys_inb(SP1_BASE_ADDR + SP_LSR, &lsr);
        }
        break;
      case SP_IIR_THR:
        /* Transmit next char */
        for (int index = 0; index < SP_FIFO_SIZE; ++ index) {
          byte = to_transmit[index];
          sys_outb(SP1_BASE_ADDR + SP_THR, byte);
        }
        util_erase(to_transmit, &to_transmit_size, SP_FIFO_SIZE);
        break;
      case SP_IIR_CTO:
        /* Character Timeout */
        util_sys_inb(SP1_BASE_ADDR + SP_LSR, &lsr);
        while (lsr & SP_LSR_RD) {
          sp_sys_inb(SP1_BASE_ADDR + SP_RBR, &byte);
          received[received_size] = byte;
          ++ received_size;
          util_sys_inb(SP1_BASE_ADDR + SP_LSR, &lsr);
        }
        break;
      case SP_IIR_RLS:
        /* Receiver Line Status - Signals Error in LSR */
        util_sys_inb(SP1_BASE_ADDR + SP_LSR, &error);
        to_transmit[to_transmit_size] = SP_SEND_SEQUENCE;
        ++ to_transmit_size;
        break;
      default:
        break;
  }
}

int sp_send_again() {
  if (received[received_size] == SP_SEND_SEQUENCE) {
    to_transmit[to_transmit_size] = SP_NEW_SEQUENCE;
    ++ to_transmit_size;
    return 1;
  }
  return 0;
}

void sp_treat_received_queue(uint8_t rtc_queue[], int *rtc_size,
                             uint8_t player_queue[], int *player_size, bool *spawn_bullet) {
  static enum last_read last;
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
          case SP_ENEMY_PLAYER:
            last = PLAYER_SIZE_0;
            break;
          case SP_RTC_INTERRUPT:
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

void sp_treat_rtc_packets(uint8_t rtc_queue[], int *rtc_size) {
  int rtc_packet_size = SP_RTC_SIZE - 1;
  uint8_t msb_x, lsb_x, msb_y, lsb_y, type;

  for (int index = 0; index < (*rtc_size / rtc_packet_size); index += rtc_packet_size) {
    msb_x = rtc_queue[index + 0];
    lsb_x = rtc_queue[index + 1];
    msb_y = rtc_queue[index + 2];
    lsb_y = rtc_queue[index + 3];
    type  = rtc_queue[index + 4];
  }

  *rtc_size = 0;
}

void sp_treat_player_packets(uint8_t player_queue[], int *player_size) {
  int player_packet_size = SP_PLAYER_SIZE - 1;
  uint8_t msb_x , lsb_x , msb_y , lsb_y ;
  uint8_t angle1, angle2, angle3, angle4;

  for (int index = 0; index < (*player_size / player_packet_size); index+= player_packet_size) {
    msb_x  = player_queue[index + 0];
    lsb_x  = player_queue[index + 1];
    msb_y  = player_queue[index + 2];
    lsb_y  = player_queue[index + 3];
    angle1 = player_queue[index + 4];
    angle2 = player_queue[index + 5];
    angle3 = player_queue[index + 6];
    angle4 = player_queue[index + 7];
  }
  
  *player_size = 0;
}

int sp_sys_inb(int port, uint8_t *value) {
  if (value == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  uint32_t word;

  if (sys_inb(port, &word) == EINVAL) {
    printf("Error when calling sys_inb.\n");
    return 1;
  }

  /* Place the char in the less significant byte (shift the number of stop bits + the parity bit) */
  word = word >> (SP_STOP_BITS + 1);

  *value = (uint8_t)word;
  return 0;
}

void add_sequence_to_transmission(Player* player, Powerup* powerup, bool spawn_bullet);
