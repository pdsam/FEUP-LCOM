#include "rtc.h"
#include <lcom/lcf.h>

static int rtc_hookID = 3;
uint16_t time_counter = 0;

int rtc_subscribe(uint8_t *irq_bitmask) {

  *irq_bitmask = BIT(rtc_hookID);

  if (sys_irqsetpolicy(RTC_IRQ, IRQ_REENABLE, &rtc_hookID) != OK) {
    printf("Error rtc subscribe\n");
    return 1;
  }

  return 0;
}

int rtc_unsubscribe() {
  if (sys_irqrmpolicy(&rtc_hookID) != OK) {
    printf("error rtc unsubcribing");
    return 1;
  }
  return 0;
}

int rtc_read_reg(uint8_t reg, uint8_t *data) {
  uint32_t temp;

  if (sys_outb(RTC_ADDRESS_REG, reg) != OK) {
    printf("Error setting the adress register\n");
    return 1;
  }

  if (sys_inb(RTC_DATA_REG, &temp) != OK) {
    printf("Error reading the register\n");
    return 1;
  }
  *data = (uint8_t) temp;
  return 0;
}

int rtc_write_reg(uint8_t reg, uint8_t data) {
  uint32_t temp = (uint32_t) data;

  if (sys_outb(RTC_ADDRESS_REG, reg) != OK) {
    printf("Error setting the adress register\n");
    return 1;
  }

  if (sys_outb(RTC_DATA_REG, temp) != OK) {
    printf("Error writing to the register\n");
    return 1;
  }
  return 0;
}

int read_year(uint8_t *year) {
  if (rtc_read_reg(RTC_YEAR, year) != OK) {
    return 1;
  }
  return 0;
}
int read_month(uint8_t *month) {
  if (rtc_read_reg(RTC_MONTH, month) != OK) {
    return 1;
  }
  return 0;
}
int read_day(uint8_t *day) {
  if (rtc_read_reg(RTC_DAY_MONTH, day) != OK) {
    return 1;
  }
  return 0;
}
int read_hours(uint8_t *hours) {
  if (rtc_read_reg(RTC_HOURS, hours) != OK) {
    return 1;
  }
  return 0;
}
int read_minutes(uint8_t *minutes) {
  if (rtc_read_reg(RTC_MINUTES, minutes) != OK) {
    return 1;
  }
  return 0;
}
int read_seconds(uint8_t *seconds) {
  if (rtc_read_reg(RTC_SECONDS, seconds) != OK) {
    return 1;
  }
  return 0;
}
int read_day_of_the_week(uint8_t *day) {
  if (rtc_read_reg(RTC_DAY_WEEK, day) != OK) {
    return 1;
  }
  return 0;
}

int rtc_enable_periodic() {

  uint8_t data;

  if (rtc_read_reg(REGISTER_B, &data)) {
    printf("\n");
    return 1;
  }
  data = data | PERIODIC_INTERRUPT;

  if (rtc_write_reg(REGISTER_B, data)) {
    printf("\n");
    return 1;
  }

  return 0;
}

int rtc_disable_periodic() {
  uint8_t data;

  if (rtc_read_reg(REGISTER_B, &data)) {
    printf("\n");
    return 1;
  }
  data = data ^ PERIODIC_INTERRUPT;

  if (rtc_write_reg(REGISTER_B, data)) {
    printf("\n");
    return 1;
  }

  return 0;
}

int rtc_set_periodic_rate(uint8_t rate) {
  uint8_t data;

  if (rtc_read_reg(REGISTER_A, &data)) {
    printf("Error reading register A\n");
    return 1;
  }

  data = data & 0xF0;
  data = data | rate;

  if (rtc_write_reg(REGISTER_A, data)) {
    printf("Error writing register A\n");
    return 1;
  }

  return 0;
}

void rtc_ih() {
  uint8_t data;
  
  rtc_read_reg(REGISTER_C, &data);

  /*if (data & UPDATE_INTERRUPT_PENDING)
    
  if (data & ALARM_INTERRUPT_PENDING)*/
    
  if (data & PERIODIC_INTERRUPT_PENDING)
    time_counter++;
}
