#pragma once

/** @defgroup rtc rtc
 * @{
 *
 * Functions for using the real-time clock
 */

#ifndef BIT
#define BIT(x) 1<<x
#endif

//RTC IRQ LINE
#define RTC_IRQ 8

//RTC DATA PORTS
#define ADDR_REG 0x70
#define DATA_REG 0x71

//RTC REGISTERS
#define SECONDS 0x00
#define MINUTES 0x02
#define HOURS 0x04
#define DAY_WEEK 0x06
#define DAY_MONTH 0x07
#define MONTH 0x08
#define YEAR 0x09
#define REG_A 0x0A
#define REG_B 0x0B
#define REG_C 0x0C
#define REG_D 0X0D

//RTC FLAGS
//REGISTER A
#define UIP BIT(7)

//REGISTER B
#define SET_RTC BIT(7)
#define PIE BIT(6)
#define AIE BIT(5)
#define UIE BIT(4)
#define BINARY_DM BIT(2)
#define TIME_FORMAT_24H BIT(1)

//REGISTER C
#define IRQF BIT(7)
#define PF BIT(6)
#define AF BIT(5)
#define UF BIT(4)

//Date class
typedef struct {
  
  uint32_t year;
  uint32_t month;
  uint32_t day;
  uint32_t dayW;
  uint32_t hour;
  uint32_t minutes;
  uint32_t seconds;

} Date;

/**
 * @brief Subscribes RTC interruptions
 * 
 * @param bitno - address of memory to be initialized with the
 *         bit number to be set in the mask returned upon an interrupt
 * @return int - Return 0 upon success and non-zero otherwise
 */
int rtc_subscribe_int(uint8_t *bitno);

/**
 * @brief Unsubscribes RTC interruptions
 * 
 * @return int - Return 0 upon success and non-zero otherwise
 */
int rtc_unsubscribe_int();

/**
 * @brief Reads RTC address passed in the arguments and stores it
 * 
 * @param addr - RTC address to be accessed and read
 * @param data - pointer to the location where data will be stored
 * @return int - Return 0 upon success and non-zero otherwise
 */
int rtc_read_register(uint32_t addr, uint32_t * data);

/**
 * @brief Writes to the RTC address passed in the arguments
 * 
 * @param addr - RTC address to be accesed and altered
 * @param data - Data to be written to the register
 * @return int - Return 0 upon success and non-zero otherwise
 */
int rtc_write_register(uint32_t addr, uint32_t data);

/**
 * @brief Enables update-ended interruptions
 * 
 * @return int - Return 0 upon success and non-zero otherwise
 */
int rtc_enable_update_ints();

/**
 * @brief Disables update-ended interruptions
 * 
 * @return int - Return 0 upon success and non-zero otherwise
 */
int rtc_disable_update_ints();

/**
 * @brief Reads seconds from the appropriate register
 * 
 * @return uint32_t  - Returns the seconds in the BCD format
 */
uint32_t rtc_read_seconds();

/**
 * @brief Reads minutes from the appropriate register
 * 
 * @return uint32_t  - Returns the minutes in the BCD format
 */
uint32_t rtc_read_minutes();

/**
 * @brief Reads hour from the appropriate register
 * 
 * @return uint32_t  - Returns the hour in the BCD format
 */
uint32_t rtc_read_hours();

/**
 * @brief Reads day of the week from the appropriate register
 * 
 * @return uint32_t  - Returns the day of the week in the BCD format
 */
uint32_t rtc_read_day_week();

/**
 * @brief Reads day of the month from the appropriate register
 * 
 * @return uint32_t  - Returns the day of the month in the BCD format
 */
uint32_t rtc_read_day_month();

/**
 * @brief Reads month from the appropriate register
 * 
 * @return uint32_t  - Returns the month in the BCD format
 */
uint32_t rtc_read_month();

/**
 * @brief Reads year from the appropriate register
 * 
 * @return uint32_t  - Returns the year in the BCD format
 */
uint32_t rtc_read_year();

/**
 * @brief RTC interrupt handler, reads register C to verify what is the source of the interruptions and handles the information accordingly
 * 
 */
void rtc_ih();

/**
 * @brief Get the Date object
 * 
 * @return Date - returns the current date
 */
Date get_date();

/**
 * @brief Update the Date object with new information
 * 
 * @return int - Return 0 upon success and non-zero otherwise
 */
int rtc_update_date();
