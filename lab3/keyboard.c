#include <lcom/lcf.h>

#include "keyboard.h"
#include "i8042.h"

int hook_id = KBD_IRQ;

int (kbd_subscribe_int)(uint8_t *bit_no) {
  if (bit_no == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  *bit_no = hook_id;

  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }

  return 0;
}

int (kbd_enable_int)() {
  if (sys_irqenable(&hook_id) != OK) {
    printf("Error when calling sys_irqenable.\n");
    return 1;
  }

  return 0;
}

int (kbd_disable_int)() {
  if (sys_irqdisable(&hook_id) != OK) {
    printf("Error when calling sys_irqdisable.\n");
    return 1;
  }

  return 0;
}

int (kbd_unsubscribe_int)() {
  if (sys_irqrmpolicy(&hook_id) != OK) {
    printf("Error when calling sys_irqrmpolicy.\n");
    return 1;
  }

  return 0;
}
