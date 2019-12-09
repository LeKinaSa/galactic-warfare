#ifndef RTC_H
#define RTC_H

#include <lcom/lcf.h>

int rtc_subscribe_int(uint8_t *bit_no);   /* Subscribes RTC interrupts */
int rtc_unsubscribe_int();                /* Unsubscribes RTC interrupts */

int rtc_setup_alarms();   /* Set up alarm interrupts. Interval specified in rtc_constants.h */

void rtc_ih(void);

#endif /* RTC_H */
