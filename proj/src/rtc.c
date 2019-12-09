#include "rtc.h"
#include "rtc_constants.h"
#include "utils.h"

static int rtc_hook_id = RTC_IRQ;

int rtc_subscribe_int(uint8_t *bit_no) {
  if (bit_no == NULL) {
    printf("Error occurred: null pointer.\n");
    return 1;
  }

  *bit_no = rtc_hook_id;

  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hook_id) != OK) {
    printf("Error when calling sys_irqsetpolicy.\n");
    return 1;
  }

  return 0;
}

int rtc_unsubscribe_int() {
  if (sys_irqrmpolicy(&rtc_hook_id)) {
    printf("Error when calling sys_irqrmpolicy.\n");
    return 1;
  }
  
  return 0;
}

void rtc_ih(void) {
  uint8_t reg_c;

  sys_outb(RTC_ADDR_REG, RTC_REG_C);
  util_sys_inb(RTC_DATA_REG, &reg_c);

  if (reg_c & REG_C_PERIODIC_INT) {
    /* Handle periodic interrupt */
  }
}
