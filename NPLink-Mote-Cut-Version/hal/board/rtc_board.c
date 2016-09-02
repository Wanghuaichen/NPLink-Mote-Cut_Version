#include "stm32l0xx.h"
#include "osal.h"
#include "hal_osal.h"
#include "app_osal.h"

extern uint8_t Send_Retry_Times;
extern uint8_t Send_Retry_Count;

RTC_HandleTypeDef RtcHandle;

/**
  * @brief  Configure the current time and date.
  * @param  None
  * @retval None
  */
void RTC_CalendarConfig(uint8_t rtc_year, uint8_t rtc_month, uint8_t rtc_date, uint8_t rtc_weekday, uint8_t rtc_hour, uint8_t rtc_minute, uint8_t rtc_second)
{
  RTC_DateTypeDef sdatestructure;
  RTC_TimeTypeDef stimestructure;

  /*##-1- Configure the Date #################################################*/
  /* Set Date: Tuesday February 18th 2014 */
  sdatestructure.Year = rtc_year;
  sdatestructure.Month = rtc_month;
  sdatestructure.Date = rtc_date;
  sdatestructure.WeekDay = rtc_weekday;
  
  if(HAL_RTC_SetDate(&RtcHandle,&sdatestructure,FORMAT_BCD) != HAL_OK)
  {
		while(1);
  }

  /*##-2- Configure the Time #################################################*/
  /* Set Time: 02:00:00 */
  stimestructure.Hours = rtc_hour;
  stimestructure.Minutes = rtc_minute;
  stimestructure.Seconds = rtc_second;
  stimestructure.TimeFormat = RTC_HOURFORMAT12_PM;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;

  if (HAL_RTC_SetTime(&RtcHandle, &stimestructure, FORMAT_BCD) != HAL_OK)
  {
		while(1);
  }
}


void RTC_AlarmAConfig(uint8_t alarm_date, uint8_t alarm_hour, uint8_t alarm_minute, uint8_t alarm_second)
{
	RTC_AlarmTypeDef salarmstructure;	
	
	salarmstructure.Alarm = RTC_ALARM_A;
  salarmstructure.AlarmDateWeekDay = alarm_date;
  salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  //salarmstructure.AlarmMask = (RTC_ALARMMASK_DATEWEEKDAY | \
                                                     RTC_ALARMMASK_HOURS       | \
                                                     RTC_ALARMMASK_MINUTES )    ;
	salarmstructure.AlarmMask = RTC_ALARMMASK_NONE;
  salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_PM;
  salarmstructure.AlarmTime.Hours = alarm_hour;
  salarmstructure.AlarmTime.Minutes = alarm_minute;
  salarmstructure.AlarmTime.Seconds = alarm_second;
  salarmstructure.AlarmTime.SubSeconds = 0x00;
	
  if(HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,FORMAT_BCD) != HAL_OK)
  {
  }
}

void RTC_AlarmBConfig(uint8_t alarm_date, uint8_t alarm_hour, uint8_t alarm_minute, uint8_t alarm_second)
{
	RTC_AlarmTypeDef salarmstructure;	
	
	salarmstructure.Alarm = RTC_ALARM_B;
  salarmstructure.AlarmDateWeekDay = alarm_date;
  salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  salarmstructure.AlarmMask = RTC_ALARMMASK_NONE;
  salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_PM;
  salarmstructure.AlarmTime.Hours = alarm_hour;
  salarmstructure.AlarmTime.Minutes = alarm_minute;
  salarmstructure.AlarmTime.Seconds = alarm_second;
  salarmstructure.AlarmTime.SubSeconds = 0x00;
	
  if(HAL_RTC_SetAlarm_IT(&RtcHandle,&salarmstructure,FORMAT_BCD) != HAL_OK)
  {
  }
}

/*##-1- Configure the RTC peripheral #######################################*/
/* Configure RTC prescaler and RTC data registers */
/* RTC configured as follows:
	- Hour Format    = Format 24
	- Asynch Prediv  = Value according to source clock
	- Synch Prediv   = Value according to source clock
	- OutPut         = Output Disable
	- OutPutPolarity = High Polarity
	- OutPutType     = Open Drain */ 

void RTC_Init(void)
{
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = 0x7F;
  RtcHandle.Init.SynchPrediv = 0x00ff;//0x00ff;//0x0130;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

  if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
  {
  }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	Send_Retry_Count = Send_Retry_Times;
	osal_set_event(APP_taskID,APP_PERIOD_SEND);//启动发包
}

void HAL_RTCEx_AlarmBEventCallback(RTC_HandleTypeDef *hrtc)
{
	Send_Retry_Count = Send_Retry_Times;
	osal_set_event(APP_taskID,APP_PERIOD_SEND);//启动发包
}
