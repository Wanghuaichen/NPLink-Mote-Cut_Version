#ifndef __RTC_BOARD_H__
#define __RTC_BOARD_H__

#include "stm32l0xx.h"

extern RTC_HandleTypeDef RtcHandle;
void RTC_CalendarConfig(uint8_t rtc_year, uint8_t rtc_month, uint8_t rtc_date, uint8_t rtc_weekday, uint8_t rtc_hour, uint8_t rtc_minute, uint8_t rtc_second);
void RTC_AlarmAConfig(uint8_t alarm_date, uint8_t alarm_hour, uint8_t alarm_minute, uint8_t alarm_second);
void RTC_AlarmBConfig(uint8_t alarm_date, uint8_t alarm_hour, uint8_t alarm_minute, uint8_t alarm_second);
void RTC_Init(void);


#endif
