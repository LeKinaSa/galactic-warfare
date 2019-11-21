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
int counter = 0;
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
    vg_exit();
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
    kbd_unsubscribe_int();
    printf("Error occurred: couldn't map video memory.\n");
    return 1;
  }

  if (vg_draw_pattern(no_rectangles, first, step)) {
    kbd_unsubscribe_int();
    vg_exit();
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

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  uint8_t kbd_bit_no = 0;

  if (kbd_subscribe_int(&kbd_bit_no)) {
    printf("Error when calling kbd_subscribe_int.\n");
    return 1;
  }

  frame_buffer = vg_init(MODE_GRAPHIC_INDEXED);

  if (frame_buffer == MAP_FAILED) {
    kbd_unsubscribe_int();
    vg_exit();
    printf("Error occurred: couldn't map video memory.\n");
    return 1;
  }

  xpm_image_t img;
  uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);

  if (pixmap == NULL) {
    kbd_unsubscribe_int();
    vg_exit();
    printf("Error occurred: couldn't load pixmap.\n");
    return 1;
  }

  if (vg_draw_xpm(pixmap, img, x, y, false)) {
    kbd_unsubscribe_int();
    vg_exit();
    printf("Error when calling vg_draw_xpm.\n");
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
  
  /* Convert xpm to pixmap */
  xpm_image_t img;
  uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);

  if (pixmap == NULL) {
    printf("Error occurred: couldn't load pixmap.\n");

    timer_unsubscribe_int();
    kbd_unsubscribe_int();
    vg_exit();

    return 1;
  }

  /* Get movement information from arguments */
  movement_info info;
  if (vg_get_movement_info(xi, yi, xf, yf, speed, fr_rate, &info)) {
    timer_unsubscribe_int();
    kbd_unsubscribe_int();
    vg_exit();

    printf("Error occurred: one or more invalid args.\n");
    return 1;
  }

  /* Draw the xpm in its initial position */
  if (vg_draw_xpm(pixmap, img, xi, yi, false)) {
    printf("Error when drawing xpm.\n");
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
          if (counter == info.int_count) {
            counter = 0;
            switch(info.type) {
              case POSITIVE_X:
                xi += info.step;
                if (xi > xf) xi = xf;
                break;
              case NEGATIVE_X:
                xi += info.step;
                if (xi < xf) xi = xf;
                break;
              case POSITIVE_Y:
                yi += info.step;
                if (yi > yf) yi = yf;
                break;
              case NEGATIVE_Y:
                yi += info.step;
                if (yi < yf) yi = yf;
                break;
              default:
                break;
            }
            vg_draw_xpm(pixmap, img, xi, yi, true);
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
  vg_vbe_contr_info_t info_p;
  memset(&info_p, 0, sizeof(info_p));

  if (vbe_return_controller_info(&info_p)) {
    printf("Error when calling vbe_return_controller_info.\n");
    return 1;
  }

  if (vg_display_vbe_contr_info(&info_p)) {
    printf("Error when calling vg_display_vbe_contr_info.\n");
    return 1;
  }

  return 0;
}
