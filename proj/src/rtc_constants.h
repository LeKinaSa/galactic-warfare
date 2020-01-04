#ifndef RTC_CONSTANTS_H
#define RTC_CONSTANTS_H

/** @defgroup rtc_constants rtc_constants
 * @{
 * @brief File containing various RTC-related constants
 */

#define RTC_IRQ     8  /**< @brief IRQ line for RTC interrupts */

#define RTC_ADDR_REG    0x70  /**< @brief RTC address register port */
#define RTC_DATA_REG    0x71  /**< @brief RTC data register port */

/* CONTROL REGISTERS */
#define RTC_REG_A   10  /**< @brief Control register A address */
#define RTC_REG_B   11  /**< @brief Control register B address */
#define RTC_REG_C   12  /**< @brief Control register C address */
#define RTC_REG_D   13  /**< @brief Control register D address */

#define RTC_ALARM_INTERVAL  5                 /**< @brief Minutes between event generation */
#define RTC_ALARM_DC        BIT(7) | BIT(6)   /**< @brief Don't care bits for an alarm interrupt */

/* RTC REGISTER A */
#define REG_A_UIP   BIT(7)  /**< @brief Bit set to 1 if an update is in progress */

/* RTC REGISTER B */
#define REG_B_INHIBIT   BIT(7)  /**< @brief Inhibits updates of date/time registers when set to 1 */
#define REG_B_PIE       BIT(6)  /**< @brief Set to 1 to enable periodic interrupts */
#define REG_B_AIE       BIT(5)  /**< @brief Set to 1 to enable alarm interrupts */
#define REG_B_UIE       BIT(4)  /**< @brief Set to 1 to enable update interrupts */ 
#define REG_B_SQR_WAVE  BIT(3)  /**< @brief Set to 1 to enable square wave generation */

/* RTC REGISTER C */
#define REG_C_IRQF          BIT(7)  /**< @brief Bit set to 1 if IRQ line is active */
#define REG_C_PERIODIC_INT  BIT(6)  /**< @brief Bit set to 1 if a periodic interrupt is pending */
#define REG_C_ALARM_INT     BIT(5)  /**< @brief Bit set to 1 if an alarm interrupt is pending */
#define REG_C_UPDATE_INT    BIT(4)  /**< @brief Bit set to 1 if an update interrupt is pending */

/* RTC REGISTER D */
#define REG_D_ERROR         BIT(7)  /**< @brief Bit set to 0 if battery runs out of energy */

/* RTC ADDRESSES */
#define RTC_SECONDS         0   /**< @brief Seconds register address */
#define RTC_ALARM_SECONDS   1   /**< @brief Alarm Seconds register address */
#define RTC_MINUTES         2   /**< @brief Minutes register address */
#define RTC_ALARM_MINUTES   3   /**< @brief Alarm Minutes register address */
#define RTC_HOURS           4   /**< @brief Hours register address */
#define RTC_ALARM_HOURS     5   /**< @brief Alarm Hours register address */
#define RTC_WEEK_DAY        6   /**< @brief Week Day register address */
#define RTC_MONTH_DAY       7   /**< @brief Month Day register address */
#define RTC_MONTH           8   /**< @brief Month register address */
#define RTC_YEAR            9   /**< @brief Year register address */

/**@}*/

#endif /* RTC_CONSTANTS_H */
