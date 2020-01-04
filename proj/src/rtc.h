#ifndef RTC_H
#define RTC_H

#include <lcom/lcf.h>

/** @defgroup rtc rtc
 * @{
 * @brief Functions related to the real-time clock (RTC).
 * Mainly focused on the alarm functionality.
 */ 

/**
 * @brief Subscribes RTC interrupts using sys_irqsetpolicy.
 * @param bit_no    pointer to variable where the interrupt bit number will be stored
 * @return          zero if no errors occurred, non-zero otherwise
 */
int rtc_subscribe_int(uint8_t *bit_no);
/**
 * @brief Unsubscribes RTC interrupts using sys_irqrmpolicy.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int rtc_unsubscribe_int();

/**
 * @brief Sets up alarm interrupts for the RTC. Interval specified in rtc_constants.h
 * @return  zero if no errors occurred, non-zero otherwise
 */
int rtc_setup_alarms();

/**
 * @brief Reads data from the specified RTC register.
 * @param reg   register to read from
 * @param data  pointer to where the data will be stored
 * @return      zero if no errors occurred, non-zero otherwise
 */
int rtc_read_register(uint8_t reg, uint8_t* data);
/**
 * @brief Writes data to the specified RTC register.
 * @param reg   register to write to
 * @param data  data to write to the register
 * @return      zero if no errors occurred, non-zero otherwise
 */
int rtc_write_register(uint8_t reg, uint8_t data);

/**
 * @brief Initializes the RTC's registers so that the RTC creates an alarm interrupt
 * every minute.
 * @return  zero if no errors occurred, non-zero otherwise
 */
int rtc_init();

/**
 * @brief Real time clock interrupt handler. 
 * When an alarm interrupt occurs, increments the minute_counter variable.
 */
void rtc_int_handler(void);

/**@}*/

#endif /* RTC_H */
