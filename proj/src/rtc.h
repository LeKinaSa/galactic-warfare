#ifndef RTC_H
#define RTC_H

#include <lcom/lcf.h>

int rtc_subscribe_int(uint8_t *bit_no);   /* Subscribes RTC interrupts */
int rtc_unsubscribe_int();                /* Unsubscribes RTC interrupts */

int rtc_setup_alarms();   /* Set up alarm interrupts. Interval specified in rtc_constants.h */

int rtc_read_register(uint8_t reg, uint8_t* data);  /* Reads data from the specified RTC register */
int rtc_write_register(uint8_t reg, uint8_t data);  /* Writes data to the specified RTC register */

int rtc_init();

void rtc_ih(void);


#endif /* RTC_H */
