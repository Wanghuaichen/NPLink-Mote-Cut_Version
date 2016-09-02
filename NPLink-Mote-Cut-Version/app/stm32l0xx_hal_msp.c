/**
  ******************************************************************************
  * @file    TIM/TIM_TimeBase/Src/stm32l0xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    18-June-2014
  * @brief   HAL MSP module.
  *
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]
    This file is generated automatically by STM32CubeMX and eventually modified
    by the user

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "spi_board.h"
#include "timer_board.h"
#include "sx1276/sx1276.h"
#include "sx1276_board.h"
#include "board.h"
#include "stm32l0xx_hal_rtc.h"
#include "main.h"
#include "rtc_board.h"
#include "uart_board.h"
#include "stm32l0xx_hal_uart.h"
#include "WaterMeter.h"

void SystemClock_Config(void);
//void SystemPower_Config(void);
void UartSetConfig(void);

/** @addtogroup STM32L0xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
//RTC_HandleTypeDef RTCHandle;
extern UART_HandleTypeDef UartHandle;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief TIM MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param htim: TIM handle pointer
  * @retval None
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* TIMx Peripheral clock enable */
  TIMx_CLK_ENABLE();

  /* The used GPIO (LED2 port) will be configured in the main program through
  LED2 initialization method */

  /*##-2- Configure the NVIC for TIMx ########################################*/
  /* Set the TIMx priority */
  HAL_NVIC_SetPriority(TIMx_IRQn, 0, 0);

  /* Enable the TIMx global Interrupt */
  HAL_NVIC_EnableIRQ(TIMx_IRQn);
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim)
{
	TIMx_CLK_DISABLE();
	HAL_NVIC_DisableIRQ(TIMx_IRQn);
}



/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually
            modified by the user
   */
	//HAL_DeInit();
	
	SystemClock_Config();

	#ifndef USE_LOW_POWER_MODE

	//initiate 3-wire UART
  UART_Init();
	#endif

	//initiate SPI (for sx1276/1279)
	SPI1_Init();

	SX1276IoInit( );

	//sx1279 active crystal initiate and power on
	SX1276Q1CtrlInit();

//	#ifdef USE_LOW_POWER_MODE
//  RtcInit( );
//	#endif

  TimerHwInit( );
	
	RTC_Init();
	RTC_AlarmAConfig(0x29,0x18,0x36,0x05);
	RTC_AlarmBConfig(0x29,0x18,0x36,0x10);
	RTC_CalendarConfig(0x16,0x08,0x29,0x01,0x18,0x36,0x00);
	
	GPIO_WaterMeter_Init();
	
	Reed_Init();
	
	  /* Enable Ultra low power mode */
  HAL_PWREx_EnableUltraLowPower();
  
  /* Enable the fast wake up from Ultra low power mode */
  HAL_PWREx_EnableFastWakeUp();

  /* Select HSI as system clock source after Wake Up from Stop mode */
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_StopWakeUpClock_HSI);
	
	
}

void HAL_MspSleepInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually
            modified by the user
   */

	SystemClock_Config();

	SPI1_Init();

	SX1276IoInit( );

	TimerHwInit( );
	

}

/**
  * @brief  DeInitializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by STM32CubeMX and eventually
            modified by the user
   */
	HAL_UART_MspDeInit(&UartHandle);
	
	SPI1_DeInit();

	SX1276IoDeInit();
	
	TimerHwDeInit();
	
	GPIO_WaterMeter_DeInit();
	
	__GPIOB_CLK_DISABLE();  
	__GPIOA_CLK_DISABLE();   
}

void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is
     clocked below the maximum system frequency, to update the voltage scaling value
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSI Oscillator and activate PLL with HSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLLMUL_4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLLDIV_2;
  RCC_OscInitStruct.HSICalibrationValue = 0x10;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}

//void SystemPower_Config(void)
//{

//  /* Enable Ultra low power mode */
//  HAL_PWREx_EnableUltraLowPower();

//  /* Enable the fast wake up from Ultra low power mode */
//  HAL_PWREx_EnableFastWakeUp();

//  /* Select HSI as system clock source after Wake Up from Stop mode */
//  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_StopWakeUpClock_HSI);

//  /* Configure RTC */
//  RTCHandle.Instance = RTC;
//  /* Configure RTC prescaler and RTC data registers as follow:
//  - Hour Format = Format 24
//  - Asynch Prediv = Value according to source clock
//  - Synch Prediv = Value according to source clock
//  - OutPut = Output Disable
//  - OutPutPolarity = High Polarity
//  - OutPutType = Open Drain */
//  RTCHandle.Init.HourFormat = RTC_HOURFORMAT_24;
//  RTCHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
//  RTCHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
//  RTCHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
//  RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
//  RTCHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
//  if(HAL_RTC_Init(&RTCHandle) != HAL_OK)
//  {
//    /* Initialization Error */
//  }
//}

//void test_alarm(void)
//{
//  RTC_AlarmTypeDef salarmstructure;

//  /*##-2- Configure the RTC Alarm peripheral #################################*/
//  /* Set Alarm to 02:20:30
//     RTC Alarm Generation: Alarm on Hours, Minutes and Seconds */
//  salarmstructure.Alarm = RTC_ALARM_A;
//  salarmstructure.AlarmDateWeekDay = RTC_WEEKDAY_MONDAY;
//  salarmstructure.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
//  salarmstructure.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
//  salarmstructure.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_None;
//  salarmstructure.AlarmTime.TimeFormat = RTC_HOURFORMAT12_AM;
//  salarmstructure.AlarmTime.Hours = 0x00;
//  salarmstructure.AlarmTime.Minutes = 0x00;
//  salarmstructure.AlarmTime.Seconds = 0x05;
//  salarmstructure.AlarmTime.SubSeconds = 0x00;

//  if(HAL_RTC_SetAlarm_IT(&RTCHandle,&salarmstructure,FORMAT_BIN) != HAL_OK)
//  {
//    /* Initialization Error */
//    //Error_Handler();
//  }
//}


/**
  * @brief RTC MSP Initialization 
  *        This function configures the hardware resources used in this example
  * @param hrtc: RTC handle pointer
  * 
  * @note  Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select 
  *        the RTC clock source; in this case the Backup domain will be reset in  
  *        order to modify the RTC Clock source, as consequence RTC registers (including 
  *        the backup registers) and RCC_BDCR register are set to their reset values.
  *             
  * @retval None
  */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTc clock source */
  //__HAL_RCC_PWR_CLK_ENABLE();
	__PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  
//  /*##-2- Configue LSI as RTC clock soucre ###################################*/
//  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI;
//  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
//  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
//  { 
//    while(1);
//  }
//  
//  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
//  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
//  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
//  { 
//    while(1);
//  }
  
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
      while(1);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
      while(1);
  }
	
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
	
	  /*##-3- Configure the NVIC for RTC Alarm ###################################*/
  HAL_NVIC_SetPriority(RTC_IRQn, 0x2, 0);
  HAL_NVIC_EnableIRQ(RTC_IRQn);
}

/**
  * @brief RTC MSP De-Initialization 
  *        This function freeze the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hrtc: RTC handle pointer
  * @retval None
  */
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
  /*##-1- Reset peripherals ##################################################*/
   __HAL_RCC_RTC_DISABLE();     
}

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef                 GPIO_InitStruct;
  
  __GPIOA_CLK_ENABLE();
  
  __ADC1_CLK_ENABLE();
  
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  __ADC1_CLK_DISABLE();
}



/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
