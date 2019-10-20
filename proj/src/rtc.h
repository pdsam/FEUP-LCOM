#pragma once
#include "stdint.h"

/** @defgroup rtc rtc
 * @{
 *
 * Real time clock functions 
 */

#define BIT(n) (0x01 << (n))

#define RTC_IRQ 8

#define RTC_ADDRESS_REG 0x70
#define RTC_DATA_REG 0x71

//data registers
#define RTC_YEAR 9
#define RTC_MONTH 8
#define RTC_DAY_MONTH 7
#define RTC_DAY_WEEK 6
#define RTC_HOURS 4
#define RTC_MINUTES 2
#define RTC_SECONDS 0

//registers
#define REGISTER_A 0x0A
#define REGISTER_B 0x0B
#define REGISTER_C 0x0C
#define REGISTER_D 0x0D

//register A:
#define UIP_FLAG BIT(7)
#define RATE_SELECTOR (BIT(3) | BIT(2) | BIT(1) | BIT(0))
//register B:
#define SET_INHIBIT BIT(7)/** @} end of protocol */
#define PERIODIC_INTERRUPT BIT(6)
#define ALARM_INTERRUPT BIT(5)
#define UPDATE_INTERRUPT BIT(4)


//register C:
#define IRQ_LINE_ACTIVE BIT(7)
#define PERIODIC_INTERRUPT_PENDING BIT(6)
#define ALARM_INTERRUPT_PENDING BIT(5)
#define UPDATE_INTERRUPT_PENDING BIT(4)


/**
 * @brief Subscribes interrupts from the rtc
 * 
 * @param irq_bitmask - id of the bitmask
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_subscribe(uint8_t *irq_bitmask);
/**
 * @brief Unsbscribes the rtc interrupts
 * 
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_unsubscribe();
/**
 * @brief Reads the register passed as a argument
 * 
 * @param reg - Register to read
 * @param data - Where the content read is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_read_reg(uint8_t reg, uint8_t *data);
/**
 * @brief 
 * 
 * @param reg - Register to write to
 * @param data - The data to be written
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_write_reg(uint8_t reg, uint8_t data);
/**
 * @brief Read the year
 * 
 * @param year - Where the year value is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int read_year(uint8_t *year);
/**
 * @brief Reads the Month
 * 
 * @param month - Where the month value is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int read_month(uint8_t *month);
/**
 * @brief Reads the day
 * 
 * @param day - Where the day value is returned 
 * @return int - Returns 0 if sucessful and 1 if not
 */
int read_day(uint8_t *day);
/**
 * @brief Reads the hour
 * 
 * @param hours - Where the hour value is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int read_hours(uint8_t *hours);
/**
 * @brief Reads the minutes
 * 
 * @param minutes - Where the minutes value is returned 
 * @return int - Returns 0 if sucessful and 1 if not
 */
int read_minutes(uint8_t *minutes);
/**
 * @brief Reads the seconds
 * 
 * @param seconds - Where the seconds value is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int read_seconds(uint8_t *seconds);
/**
 * @brief Reads the day of the week
 * 
 * @param day - Where the day of the week is returned
 * @return int - Returns 0 if sucessful and 1 if not
 */
int read_day_of_the_week(uint8_t *day);

/**
 * @brief Enables Periodic Interrupts
 * 
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_enable_periodic();
/**
 * @brief Disables Periodic Interrupts
 * 
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_disable_periodic();
/**
 * @brief Sets the rate of the periodic interruptions
 * 
 * @param rate - Rate to witch to Interrupt
 * @return int - Returns 0 if sucessful and 1 if not
 */
int rtc_set_periodic_rate(uint8_t rate);
/**
 * @brief Real time clock interrupt Handler
 * 
 */
void rtc_ih();

/** @} end of rtc */
