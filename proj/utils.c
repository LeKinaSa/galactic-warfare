#include <lcom/lcf.h>

#include "utils.h"

#ifdef LAB3
extern uint32_t sys_inb_cnt;
#endif

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

  #ifdef LAB3
  sys_inb_cnt++;
  #endif

  *value = (uint8_t)word;
  return 0;
}

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if (lsb == NULL) {
    printf("Error when calling util_get_LSB.\n");
    return 1;
  }

  *lsb = (uint8_t)(val);
  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  if (msb == NULL) {
    printf("Error when calling util_get_MSB.\n");
    return 1;
  }

  *msb = (uint8_t)(val >> 8);
  return 0;
}
