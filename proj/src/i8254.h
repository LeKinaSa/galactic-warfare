#ifndef _LCOM_I8254_H_
#define _LCOM_I8254_H_

#include <lcom/lcf.h>

/** @defgroup i8254 i8254
 * @{
 * @brief Constants for programming the i8254 Timer.
 */

#define TIMER_FREQ 1193182              /**< @brief Clock frequency for timer in PC and AT */

#define TIMER0_IRQ 0                    /**< @brief Timer 0 IRQ line */
#define TIMER0_INTERRUPTS_PER_SECOND 60 /**< @brief Default number of interrupts per second by timer 0 */

/* Valid frequencies for a timer with a 16-bit register and a clock speed of 1193182 Hz */

#define MIN_FREQ 19             /**< @brief Lower frequencies would require an initial value higher than 65535 (max value for unsigned 16-bit integer) */
#define MAX_FREQ TIMER_FREQ     /**< @brief Highest frequency is equal to the clock speed (initial value is 1) */

/* I/O port addresses */

#define TIMER_0    0x40 /**< @brief Timer 0 count register */
#define TIMER_1    0x41 /**< @brief Timer 1 count register */
#define TIMER_2    0x42 /**< @brief Timer 2 count register */
#define TIMER_CTRL 0x43 /**< @brief Control register */

#define SPEAKER_CTRL 0x61 /**< @brief Register for speaker control  */

/* Timer control */

/* Timer selection: bits 7 and 6 */

#define TIMER_SEL0    0x00              /**< @brief Control Word for Timer 0 */
#define TIMER_SEL1    BIT(6)            /**< @brief Control Word for Timer 1 */
#define TIMER_SEL2    BIT(7)            /**< @brief Control Word for Timer 2 */
#define TIMER_RB_CMD (BIT(7) | BIT(6))  /**< @brief Read Back Command */

/* Register selection: bits 5 and 4 */

#define TIMER_LSB            BIT(4)                  /**< @brief Initialize Counter LSB only */
#define TIMER_MSB            BIT(5)                  /**< @brief Initialize Counter MSB only */
#define TIMER_LSB_MSB       (TIMER_LSB | TIMER_MSB)  /**< @brief Initialize LSB first and MSB afterwards */
#define TIMER_LSB_MSB_SHIFT  0x04                    /**< @brief Number of bits between bit 0 and LSB and MSB bits */

/* Operating mode: bits 3, 2 and 1 */

#define TIMER_INTERRUPT  0x00              /**< @brief Mode 0: interrupt on terminal */
#define TIMER_HW_ONESHOT BIT(1)            /**< @brief Mode 1: hardware retriggerable one-shot */
#define TIMER_RATE_GEN   BIT(2)            /**< @brief Mode 2: rate generator */
#define TIMER_SQR_WAVE  (BIT(2) | BIT(1))  /**< @brief Mode 3: square wave generator */
#define TIMER_SW_STROBE  BIT(3)            /**< @brief Mode 4: software strobe */
#define TIMER_HW_STROBE (BIT(3) | BIT(1))  /**< @brief Mode 5: hardware strobe */

#define TIMER_MODE_SHIFT        1       /**< @brief Number of bits between bit 0 and mode bits */
#define TIMER_MAX_MODE          5       /**< @brief Maximum mode value when following compatibility conventions */
#define TIMER_MASK_ALT_MODES    0x03    /**< @brief Bitmask to conventionalize modes that don't follow compatibility conventions */

/* Counting mode: bit 0 */

#define TIMER_BCD 0x01 /**< @brief Count in BCD */
#define TIMER_BIN 0x00 /**< @brief Count in binary */

/* READ-BACK COMMAND FORMAT */

#define TIMER_RB_COUNT_  BIT(5)
#define TIMER_RB_STATUS_ BIT(4)
#define TIMER_RB_SEL(n)  BIT((n) + 1)

/* Bitmasks */

#define TIMER_MASK_INIT     0x30    /**< @brief Bitmask to obtain initialization mode bits */
#define TIMER_MASK_MODE     0x0E    /**< @brief Bitmask to obtain the mode bits */
#define TIMER_MASK_MODE_BCD 0x0F    /**< @brief Bitmask to obtain the operating mode and counting base bits */

/**@}*/

#endif /* _LCOM_I8254_H */
