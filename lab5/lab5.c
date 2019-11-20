// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

// Any header files included below this line should have been created by you
#include "video.h"
#include "vbe_constants.h"
#include "i8042.h"
#include "keyboard.h"
#include "i8254.h"

uint8_t scancode;
int counter; // FIXME: not needed
void* frame_buffer;   // FIXME: Variable might not be needed

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  if (video_set_mode(mode)) {
    printf("Error when calling video_set_mode.\n");
    return 1;
  }

  sleep(delay);

  if (vg_exit()) {
    printf("Error when calling vg_exit.\n");
    return 1;
  }

  return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {  
  uint8_t kbd_bit_no = 0;
  
  if (kbd_subscribe_int(&kbd_bit_no)) {
    printf("Error when calling kbd_subscribe_int.\n");
    return 1;
  }
  
  frame_buffer = vg_init(mode);

  if (frame_buffer == MAP_FAILED) {
    kbd_unsubscribe_int();
    printf("Error occurred: couldn't map video memory.\n");
    return 1;
  }

  if (vg_draw_rectangle(x, y, width, height, color)) {
    kbd_unsubscribe_int();
    printf("Error when calling vg_draw_rectangle.\n");
    return 1;
  }


  int ipc_status;
  message msg;
  
  while (scancode != KBD_ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("Error when calling driver_receive.\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(kbd_bit_no)) {
          kbc_ih();
        }
        break;
      default:
        break;
      }
    }
  }


  if (vg_exit()) {
    kbd_unsubscribe_int();
    printf("Error when calling vg_exit.\n");
    return 1;
  }

  if (kbd_unsubscribe_int()) {
    printf("Error when calling kbd_unsubscribe_int.\n");
    return 1;
  }

  return 0;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  uint8_t kbd_bit_no = 0;

  if (kbd_subscribe_int(&kbd_bit_no)) {
    printf("Error when calling kbd_subscribe_int.\n");
    return 1;
  }

  frame_buffer = vg_init(mode);

  if (frame_buffer == MAP_FAILED) {
    printf("Error occurred: couldn't map video memory.\n");
    return 1;
  }

  if (vg_draw_pattern(no_rectangles, first, step)) {
    printf("Error when calling vg_draw_pattern.\n");
    return 1;
  }

  int ipc_status;
  message msg;

  while (scancode != KBD_ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("Error when calling driver_receive.\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(kbd_bit_no)) {
          kbc_ih();
        }
        break;
      default:
        break;
      }
    }
  }

  if (vg_exit()) {
    printf("Error when calling vg_exit.\n");
    return 1;
  }

  if (kbd_unsubscribe_int()) {
    printf("Error when calling kbd_unsubscribe_int.\n");
    return 1;
  }

  return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  uint8_t kbd_bit_no = 0;

  if (kbd_subscribe_int(&kbd_bit_no)) {
    printf("Error when calling kbd_subscribe_int.\n");
    return 1;
  }

  frame_buffer = vg_init(MODE_GRAPHIC_INDEXED);

  if (frame_buffer == MAP_FAILED) {
    printf("Error occurred: couldn't map video memory.\n");
    return 1;
  }

  if (vg_draw_xpm(xpm, x, y)) {
    printf("Error when calling vg_draw_pixmap.\n");
    return 1;
  }

  int ipc_status;
  message msg;

  while (scancode != KBD_ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("Error when calling driver_receive.\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(kbd_bit_no)) {
          kbc_ih();
        }
        break;
      default:
        break;
      }
    }
  }


  if (vg_exit()) {
    printf("Error when calling vg_exit.\n");
    return 1;
  }

  if (kbd_unsubscribe_int()) {
    printf("Error when calling kbd_unsubscribe_int.\n");
    return 1;
  }
  
  return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  uint8_t kbd_bit_no = 0;
  uint8_t timer0_bit_no = 0;

  if (timer_subscribe_int(&timer0_bit_no)) {
    printf("Error when calling timer_subscribe_int.\n");
    return 1;
  }

  if (kbd_subscribe_int(&kbd_bit_no)) {
    printf("Error when calling kbd_subscribe_int.\n");
    if (timer_unsubscribe_int()) {
      printf("Error when calling timer_unsubscribe_int.\n");
    }
    return 1;
  }

  frame_buffer = vg_init(MODE_GRAPHIC_INDEXED);

  if (frame_buffer == MAP_FAILED) {
    printf("Error occurred: couldn't map video memory.\n");
    if (timer_unsubscribe_int()) {
      printf("Error when calling timer_unsubscribe_int.\n");
    }
    if (kbd_unsubscribe_int()) {
      printf("Error when calling kbd_unsubscribe_int.\n");
    }
    vg_exit();
    return 1;
  }

  int ipc_status;
  message msg;

  uint8_t step_x, step_y;
  uint8_t int_count;
  bool positive_movement;
  
  if (speed > 0) {
    int_count = TIMER0_INTERRUPTS_PER_SECOND / fr_rate;
    if (xi == xf) {
      step_x = 0;
      if (yi < yf) {
        step_y = speed;
        positive_movement = true;
      }
      else {
        step_y = -speed;
        positive_movement = false;
      }
    }
    if (yi == yf) {
      step_y = 0;
      if (xi < xf) {
        step_x = speed;
        positive_movement = true;
      }
      else {
        step_x = -speed;
        positive_movement = false;
      }
    }
  }
  if (speed < 0) {
    step_x = 1;
    step_y = 1;
    int_count = TIMER0_INTERRUPTS_PER_SECOND / fr_rate * (-speed);
    if (xi == xf) {
      step_x = 0;
    }
    if (yi == yf) {
      step_y = 0;
    }
  }

  while (scancode != KBD_ESC_BREAKCODE) {
    if (driver_receive(ANY, &msg, &ipc_status)) {
      printf("Error when calling driver_receive.\n");
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
      case HARDWARE:
        if (msg.m_notify.interrupts & BIT(kbd_bit_no)) {
          kbc_ih();
        }
        if (msg.m_notify.interrupts & BIT(timer0_bit_no)) {
          timer_int_handler();
          if (counter == int_count) {
            counter = 0;
            xi += step_x;
            yi += step_y;
            if (positive_movement) {
              if (xi > xf) {
                xi = xf;
              }
              if (yi > yf) {
                yi = yf;
              }
            }
            else {
              if (xi < xf) {
                xi = xf;
              }
              if (yi < yf) {
                yi = yf;
              }
            }
            vg_draw_xpm(xpm, xi, yi);
          }
        }
        break;
      default:
        break;
      }
    }
  }


  if (vg_exit()) {
    printf("Error when calling vg_exit.\n");
    if (timer_unsubscribe_int()) {
      printf("Error when calling timer_unsubscribe_int.\n");
    }
    if (kbd_unsubscribe_int()) {
      printf("Error when calling kbd_unsubscribe_int.\n");
    }
    return 1;
  }

  if (kbd_unsubscribe_int()) {
    printf("Error when calling kbd_unsubscribe_int.\n");
    if (timer_unsubscribe_int()) {
      printf("Error when calling timer_unsubscribe_int.\n");
    }
    return 1;
  }

  if (timer_unsubscribe_int()) {
    printf("Error when calling timer_unsubscribe_int.\n");
    return 1;
  }
  
  return 0;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
