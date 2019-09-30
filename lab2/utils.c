#include <lcom/lcf.h>

#include <stdint.h>

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int (util_sys_inb)(int port, uint8_t *value) {
  uint32_t word;
  int retv;

  retv = sys_inb(port, &word);

  if (retv == EINVAL) {
    printf("Error when calling sys_inb.\n");
    return retv;
  }

  *value = (uint8_t)word;
  return retv;
}
