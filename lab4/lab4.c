// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "mouse.h"
#include "utils.h"
#include "i8254.h"
#include "gesture.h"

uint8_t packet_byte;
int counter;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
  uint8_t bit_no = 0;

  if (mouse_subscribe_int(&bit_no)) {
    printf("Error when calling mouse_subscribe_int.\n");
    return 1;
  }

  if (mouse_disable_int()) {
    printf("Error when calling mouse_disable_int.\n");
    return 1;
  }

  if (mouse_enable_data_report()) {
    printf("Error when calling mouse_enable_data_report.\n");
    return 1;
  }

  if (mouse_enable_int()) {
    printf("Error when calling mouse_enable_int.\n");
    return 1;
  }

  int ipc_status;
  message msg;

  uint32_t num_packets = 0;
  uint8_t packet_bytes[MOUSE_PCK_NUM_BYTES];
  int packet_byte_counter = 0;
  struct packet p;
  
  while (num_packets < cnt) {
    if (driver_receive(ANY, &msg, &ipc_status)) { 
      printf("Error when calling driver_receive.\n");
      continue;
    }
    
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(bit_no)) {
          mouse_ih();

          if ((packet_byte_counter == MOUSE_INDEX_FIRST_BYTE) && ((packet_byte & MOUSE_FIRST_BYTE_CHECK) == 0)) {
            continue;
          }
          
          packet_bytes[packet_byte_counter] = packet_byte;

          if (packet_byte_counter == MOUSE_INDEX_THIRD_BYTE) {
            packet_byte_counter = 0;
            ++num_packets;

            mouse_parse_packet(packet_bytes, &p);
            mouse_print_packet(&p);
          }
          else {
            ++packet_byte_counter;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (mouse_disable_int()) {
    printf("Error when calling mouse_disable_int.\n");
    return 1;
  }

  if (mouse_disable_data_report()) {
    printf("Error when calling mouse_disable_data_report.\n");
    return 1;
  }

  if (mouse_enable_int()) {
    printf("Error when calling mouse_enable_int.\n");
    return 1;
  }

  if (mouse_unsubscribe_int()) {
    printf("Error when calling mouse_unsubscribe_int.\n");
    return 1;
  }

  if (kbc_reset_cmd_byte()) {
    printf("Error when calling kbc_reset_cmd_byte.\n");
    return 1;
  }

  return 0;
}


int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
  uint32_t num_packets = 0;
  uint8_t packet_bytes[MOUSE_PCK_NUM_BYTES];
  int packet_byte_counter = 0;
  struct packet p;
  uint8_t status;

  while (num_packets < cnt) {
    if (mouse_write_command(MOUSE_READ_DATA)) {
      printf("Error when calling mouse_write_command.\n");
    }

    packet_byte_counter = 0;

    while (packet_byte_counter <= MOUSE_INDEX_THIRD_BYTE) {
      if (kbc_read_status(&status)) {
        printf("Error when reading from status register.\n");
      }

      if (((status & KBC_OUT_BUF_FULL) != 0) && ((status & KBC_MOUSE_DATA) != 0)) {
        if (util_sys_inb(KBC_OUTPUT_BUF, &packet_byte)) {
          printf("Error when calling util_sys_inb.\n");
        }
        
        if ((status & (KBC_TIMEOUT | KBC_PARITY_ERROR)) != 0) {
          printf("Status register indicates timeout or parity error.\n");
        }
        else {
          if ((packet_byte_counter == MOUSE_INDEX_FIRST_BYTE) && ((packet_byte & MOUSE_FIRST_BYTE_CHECK) == 0)) {
            continue;
          }

          packet_bytes[packet_byte_counter] = packet_byte;

          if (packet_byte_counter == MOUSE_INDEX_THIRD_BYTE) {
            ++num_packets;

            mouse_parse_packet(packet_bytes, &p);
            mouse_print_packet(&p);
          }

          ++packet_byte_counter;
        }
      }
    }

    tickdelay(micros_to_ticks(period * 1000));
  }

  if (mouse_set_stream_mode()) {
    printf("Error when calling mouse_set_stream_mode.\n");
    return 1;
  }

  if (mouse_disable_data_report()) {
    printf("Error when calling mouse_disable_data_report.\n");
    return 1;
  }

  if (kbc_reset_cmd_byte()) {
    printf("Error when calling kbc_reset_cmd_byte.\n");
    return 1;
  }

  return 0;
}


int (mouse_test_async)(uint8_t idle_time) {
  uint8_t timer_bit_no = 0, mouse_bit_no = 0;

  if (mouse_subscribe_int(&mouse_bit_no)) {
    printf("Error when calling mouse_subscribe_int.\n");
    return 1;
  }

  if (mouse_disable_int()) {
    printf("Error when calling mouse_disable_int.\n");
    return 1;
  }

  if (mouse_enable_data_report()) {
    printf("Error when calling mouse_enable_data_report.\n");
    return 1;
  }

  if (mouse_enable_int()) {
    printf("Error when calling mouse_enable_int.\n");
    return 1;
  }

  if (timer_subscribe_int(&timer_bit_no)) {
    printf("Error when calling timer_subscribe_int.\n");
    return 1;
  }

  int ipc_status;
  message msg;

  uint8_t packet_bytes[MOUSE_PCK_NUM_BYTES];
  int packet_byte_counter = 0;
  struct packet p;

  uint8_t num_seconds = 0;
  
  while (num_seconds < idle_time) {
    if (driver_receive(ANY, &msg, &ipc_status)) { 
      printf("Error when calling driver_receive.\n");
      continue;
    }
    
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(mouse_bit_no)) {
          mouse_ih();

          num_seconds = 0;
          counter = 0;

          if ((packet_byte_counter == MOUSE_INDEX_FIRST_BYTE) && ((packet_byte & MOUSE_FIRST_BYTE_CHECK) == 0)) {
            continue;
          }

          packet_bytes[packet_byte_counter] = packet_byte;

          if (packet_byte_counter == 2) {
            packet_byte_counter = 0;

            mouse_parse_packet(packet_bytes, &p);
            mouse_print_packet(&p);
          }
          else {
            ++packet_byte_counter;
          }
        }
        if (msg.m_notify.interrupts & BIT(timer_bit_no)) {
          timer_int_handler();

          if (counter == TIMER0_INTERRUPTS_PER_SECOND) {
            counter = 0;
            ++num_seconds;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (timer_unsubscribe_int()) {
    printf("Error when calling timer_unsubscribe_int.\n");
    return 1;
  }

  if (mouse_disable_int()) {
    printf("Error when calling mouse_disable_int.\n");
    return 1;
  }

  if (mouse_disable_data_report()) {
    printf("Error when calling mouse_disable_data_report.\n");
    return 1;
  }

  if (mouse_enable_int()) {
    printf("Error when calling mouse_enable_int.\n");
    return 1;
  }

  if (mouse_unsubscribe_int()) {
    printf("Error when calling mouse_unsubscribe_int.\n");
    return 1;
  }

  if (kbc_reset_cmd_byte()) {
    printf("Error when calling kbc_reset_cmd_byte.\n");
    return 1;
  }

  return 0;
}


int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
  uint8_t bit_no = 0;

  if (mouse_subscribe_int(&bit_no)) {
    printf("Error when calling mouse_subscribe_int.\n");
    return 1;
  }

  if (mouse_disable_int()) {
    printf("Error when calling mouse_disable_int.\n");
    return 1;
  }

  if (mouse_enable_data_report()) {
    printf("Error when calling mouse_enable_data_report.\n");
    return 1;
  }

  if (mouse_enable_int()) {
    printf("Error when calling mouse_enable_int.\n");
    return 1;
  }

  int ipc_status;
  message msg;

  uint32_t num_packets = 0;
  uint8_t packet_bytes[MOUSE_PCK_NUM_BYTES];
  int packet_byte_counter = 0;
  struct packet p;

  enum state current_state = START;
  
  while (current_state != FINISHED) {
    if (driver_receive(ANY, &msg, &ipc_status)) { 
      printf("Error when calling driver_receive.\n");
      continue;
    }
    
    if (is_ipc_notify(ipc_status)) {
      switch(_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(bit_no)) {
          mouse_ih();

          if ((packet_byte_counter == MOUSE_INDEX_FIRST_BYTE) && ((packet_byte & MOUSE_FIRST_BYTE_CHECK) == 0)) {
            continue;
          }
          
          packet_bytes[packet_byte_counter] = packet_byte;

          if (packet_byte_counter == MOUSE_INDEX_THIRD_BYTE) {
            packet_byte_counter = 0;
            ++num_packets;

            mouse_parse_packet(packet_bytes, &p);
            mouse_print_packet(&p);
            update_state_machine(&current_state, mouse_get_event(&p), x_len, tolerance);
          }
          else {
            ++packet_byte_counter;
          }
        }
        break;
      default:
        break;
      }
    }
  }

  if (mouse_disable_int()) {
    printf("Error when calling mouse_disable_int.\n");
    return 1;
  }

  if (mouse_disable_data_report()) {
    printf("Error when calling mouse_disable_data_report.\n");
    return 1;
  }

  if (mouse_enable_int()) {
    printf("Error when calling mouse_enable_int.\n");
    return 1;
  }

  if (mouse_unsubscribe_int()) {
    printf("Error when calling mouse_unsubscribe_int.\n");
    return 1;
  }

  if (kbc_reset_cmd_byte()) {
    printf("Error when calling kbc_reset_cmd_byte.\n");
    return 1;
  }

  return 0;
}
