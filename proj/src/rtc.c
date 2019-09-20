#include <lcom/lcf.h>

#include "rtc.h"

int hook_id = 8;

static Date current_Date = {0,0,0,0,0,0,0};

int rtc_subscribe_int(uint8_t *bitno) {

*bitno = hook_id;

  if (sys_irqsetpolicy(RTC_IRQ,IRQ_REENABLE,&hook_id) != OK){
    printf("Error: Unable to subscribe interruption.");
    return 1;
  }

  if(rtc_enable_update_ints() != OK) {
    printf("Error: Failed to enable update interruptions.\n");
    return 1;
  }

  return 0;
}


int rtc_unsubscribe_int() {
    
    if(rtc_disable_update_ints() != OK) {
      printf("Error: Failed to disable update interruptions.\n");
      return 1;
    }

    if (sys_irqrmpolicy(&hook_id) != OK){
      printf("Error: Unable to unsubscribe interruptions.\n ");
      return 1;
    }
    
    return 0;
}

int rtc_read_register(uint32_t addr,uint32_t * data)  {
  
  if(sys_outb(ADDR_REG, addr) != OK)
    {
      printf("Error: couldn't access RTC register.\n");
      return 1;
    }

  if(sys_inb(DATA_REG, data) != OK) {
    printf("Error: couldn't retrieve data from RTC register.\n");
    return 1;
  }
  
  return 0;
}

int rtc_write_register(uint32_t addr, uint32_t data) {
  
  if(sys_outb(ADDR_REG,addr) != OK)
    {
      printf("Error: couldn't access RTC register.\n");
      return 1;
    }
  
  if(sys_outb(DATA_REG,data) != OK) {
    printf("Error: couldn't write data to RTC register.1\n");
    return 1;
  }

  return 0;
}

int rtc_enable_update_ints() {
  
  uint32_t data;
  //retrieve register B data
  if(rtc_read_register(REG_B, &data) != 0) {
    return 1;
  }
  //enable periodic interrupts bit
  data |= UIE;
  data &= 0x7F;

  if(rtc_write_register(REG_B, data) != 0) {
    return 1;
  }

  return 0;
}

int rtc_disable_update_ints() {
  
  uint32_t data;
  //retrieve register B data
  if(rtc_read_register(REG_B, &data) != 0) {
    return 1;
  }
  //disable periodic interrupts bit
  data &= 0xEF;

  if(rtc_write_register(REG_B, data) != 0) {
    return 1;
  }

  return 0;
}

uint32_t rtc_read_seconds() {
  uint32_t secs;

  //retrieve data
  if(rtc_read_register(SECONDS, &secs) != 0) {
    printf("Bad Data reading seconds.\n");
  }

  return secs;
}

uint32_t rtc_read_minutes(){
  uint32_t mins;
  
  //retrieve data
  if(rtc_read_register(MINUTES, &mins) != 0) {
    printf("Bad Data reading minutes.\n");
  }
  
  return mins;
}

uint32_t rtc_read_hours(){
  uint32_t hrs;
  //retrieve data
  if(rtc_read_register(HOURS, &hrs) != 0) {
    printf("Bad Data reading hours.\n");
  }
  
  return hrs;
}

uint32_t rtc_read_day_week(){
  uint32_t day_w;
  
  //retrieve data
  if(rtc_read_register(DAY_WEEK, &day_w) != 0) {
    printf("Bad Data reading day of the week.\n");
  }

  return day_w;
}

uint32_t rtc_read_day_month(){
  uint32_t day_m;

  //retrieve data
  if(rtc_read_register(DAY_MONTH, &day_m) != 0) {
    printf("Bad Data reading day of the month.\n");
  }
  
  return day_m;
}

uint32_t rtc_read_month(){
  uint32_t month;
  
  //retrieve data
  if(rtc_read_register(MONTH, &month) != 0) {
    printf("Bad Data reading the month.\n");
  }

  return month;
}

uint32_t rtc_read_year(){
  uint32_t year;
  
  //retrieve data
  if(rtc_read_register(YEAR, &year) != 0) {
    printf("Bad Data reading the year.\n");
  }

  return year;
}


void rtc_ih() {
  uint32_t regC;
  
  if(rtc_read_register(REG_C, &regC)!= 0)
    return;
  
  if(regC & PF)
    printf("Periodic Interrupt Pending.\n");

  if(regC & AF)
    printf("Alarm Interrupt Pending.\n");

  if(regC & UF) {
    if(rtc_update_date() != OK)
      printf("Failed to update date.\n");

    //clear the IRQF bit
    regC &= 0x7F;

    if(rtc_write_register(REG_C,regC)!= OK)
      return;
      
  }
}


Date get_date() {
  return current_Date;
}

int rtc_update_date() {
  uint32_t regB;

  if(rtc_read_register(REG_B, &regB)!= 0)
    return 1;

  if(regB & BINARY_DM) {
    current_Date.year = rtc_read_year();
    current_Date.month = rtc_read_month();
    current_Date.day = rtc_read_day_month();
    current_Date.dayW =rtc_read_day_week();
    current_Date.hour = rtc_read_hours();
    current_Date.minutes = rtc_read_minutes();
    current_Date.seconds = rtc_read_seconds();  
  }
  else {
    current_Date.year = rtc_read_year();
    current_Date.month = rtc_read_month();
    current_Date.day = rtc_read_day_month();
    current_Date.dayW = rtc_read_day_week();
    uint32_t hour = rtc_read_hours();
    if(!((hour & BIT(7)) == 0)) {
      hour &= 0x07F;
      if(hour == 0x12) {
        hour = 0;
      }
      else {
        hour += 0x12;
      }
    }
    current_Date.hour = hour;
    current_Date.minutes = rtc_read_minutes();
    current_Date.seconds = rtc_read_seconds();
  }

  return 0;
}
