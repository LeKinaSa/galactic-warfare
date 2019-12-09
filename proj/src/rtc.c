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

int rtc_read_register(uint8_t reg, uint8_t *data) {
  if (sys_outb(RTC_ADDR_REG, reg) != OK) {
    printf("Error when writing address to RTC.\n");
    return 1;
  }
  if (util_sys_inb(RTC_DATA_REG, data)) {
    printf("Error when reading data from RTC.\n");
    return 1;
  }
  return 0;
}

int rtc_write_register(uint8_t reg, uint8_t data) {
  if (sys_outb(RTC_ADDR_REG, reg)) {
    printf("Error when writing address to RTC.\n");
    return 1;
  }
  if (sys_outb(RTC_DATA_REG, data)) {
    printf("Error when reading data from RTC.\n");
    return 1;
  }
  return 0;
}

int rtc_init() {
  /* Clear old interrupts */
  uint8_t reg_c;
  if (rtc_read_register(RTC_REG_C, &reg_c)) {
    printf("Error when reading register C from RTC.\n");
    return 1;
  }

  /* Get the current seconds to set up the alarm */
  uint8_t seconds;
  if (rtc_read_register(RTC_SECONDS, &seconds)) {
    printf("Error when reading seconds from RTC.\n");
    return 1;
  }

  /* Set up the Alarm */
    /* Enable Alarm Interrupts */
  uint8_t reg_b;
  if (rtc_read_register(RTC_REG_B, &reg_b)) {
    printf("Error when reading register B from RTC.\n");
    return 1;
  }

  reg_b |= REG_B_AIE;

  if (rtc_write_register(RTC_REG_B, reg_b)) {
    printf("Error when writing register B to RTC.\n");
    return 1;
  }
    /* Alarm Seconds */
  seconds = (seconds + 59) % 60;
  if (rtc_write_register(RTC_ALARM_SECONDS, seconds)) {
    printf("Error when writing alarm seconds to RTC.\n");
    return 1;
  }
    /* Alarm Minutes */
  if (rtc_write_register(RTC_ALARM_MINUTES, RTC_ALARM_DC)) {
    printf("Error when writing alarm minutes to RTC.\n");
    return 1;
  }
    /* Alarm Hour */
  if (rtc_write_register(RTC_ALARM_HOURS, RTC_ALARM_DC)) {
    printf("Error when writing alarm hours to RTC.\n");
    return 1;
  }

  return 0;
}

void rtc_ih(void) {
  uint8_t reg_c;

  sys_outb(RTC_ADDR_REG, RTC_REG_C);
  util_sys_inb(RTC_DATA_REG, &reg_c);

  if (reg_c & REG_C_ALARM_INT) {
    /* Handle alarm interrupt */
  }
}
