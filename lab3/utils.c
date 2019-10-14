#include <lcom/lcf.h>

#include "utils.h"

extern uint32_t cnt;

int (util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  uint32_t word;

  if (sys_inb(port, &word) == EINVAL) {
    printf("Error when calling sys_inb.\n");
    return 1;
  }

  *value = (uint8_t)word;
  return 0;

  #ifdef LAB3
  cnt++;
  #endif
}
