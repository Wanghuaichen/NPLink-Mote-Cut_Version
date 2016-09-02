#include "stm32l0xx.h"
#include "WaterMeter.h"
#include "osal.h"
#include "delay.h"
#include "app_osal.h"
#include "hal_osal.h"

//extern RTC_HandleTypeDef RtcHandle;
//void SystemClock_Config(void);


uint32_t g_Water_Init;
uint32_t g_Water_Count;
uint8_t  g_Reed_Error;  
uint8_t  g_Reed_State;
uint8_t  g_Reed_Force_flag;
uint8_t  g_Reed_Steel_flag;

uint8_t Current_GateStatus;

uint16_t Battery_Value;
uint8_t  Battery_Warning;

ADC_HandleTypeDef AdcHandle;

extern void APP_Send(void);
extern uint8_t Send_Success;

//uint8_t wakeup_flag;

void Dataeeprom_Write(uint32_t address,uint32_t data)
{
	FLASH->SR|=FLASH_SR_WRPERR;
	HAL_FLASHEx_DATAEEPROM_Unlock();
	HAL_FLASHEx_DATAEEPROM_Program(TYPEPROGRAM_WORD,address,data);
	HAL_FLASHEx_DATAEEPROM_Lock();
}

uint32_t Dataeeprom_Read(uint32_t address)
{ 
	uint32_t data;
	uint32_t *p=(uint32_t *)address;
	
	data=*p;
	
	return data;
}

void Dataeeprom_EraseAll()
{
	uint32_t i;
	FLASH->SR|=FLASH_SR_WRPERR;
	HAL_FLASHEx_DATAEEPROM_Unlock();
	for(i=0x08080000;i<0x08080200;i=i+4)
	{
		HAL_FLASHEx_DATAEEPROM_Erase(i);
	}
	HAL_FLASHEx_DATAEEPROM_Lock();
}

void GPIO_WaterMeter_Init()
{
  GPIO_InitTypeDef   GPIO_InitStructure;

  /* Enable GPIOA clock */
  __GPIOB_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
  
  /* TEST PB5*/ 
  GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStructure.Pull = GPIO_PULLUP;//GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_5;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH  ;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//Reed3 Force PA12 //PA11 wrong dont know
	//GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_12;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

	//Reed2  PB15
	//GPIO_InitStructure.Pull = GPIO_NOPULL;
	GPIO_InitStructure.Pin = GPIO_PIN_15;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);	

	//Reed1 PB14
	GPIO_InitStructure.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);		

  /* Enable and set EXTI4_15 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);	

//  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 2, 0);
//  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);	
	
	//wakeup_flag = 0;
}

void GPIO_WaterMeter_DeInit()
{
//	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5);
//	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12);
//	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_9);
//	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8);
}

void Reed_Init()
{
	g_Water_Count = 0;
	g_Reed_Force_flag = 0;
	g_Reed_Steel_flag = 0;
	g_Reed_Error = 0; 
	g_Reed_State = 0;
	if(0x1234 == Dataeeprom_Read(FLAG_ADDRESS))
	{
		g_Water_Init=Dataeeprom_Read(WATER_ADDRESS);
		Current_GateStatus = Dataeeprom_Read(MOTOR_ADDRESS);
		//g_Reed_State=(uint8_t)Dataeeprom_Read(STATE_ADDRESS);
	}
	else
	{
		Dataeeprom_EraseAll();
		g_Water_Init = 1578;
		Dataeeprom_Write(WATER_ADDRESS,1578);
		Current_GateStatus = 1;
		Dataeeprom_Write(MOTOR_ADDRESS,1);
		
		Dataeeprom_Write(FLAG_ADDRESS, 0x1234);
	}
}	

void Water_Count_Normal()
{
	g_Water_Count++;
	//Dataeeprom_Write(STATE_ADDRESS,(uint32_t)g_Reed_State); 
	if(0 == (g_Water_Count % 20))
	{
		Dataeeprom_Write(WATER_ADDRESS,(g_Water_Init+(uint32_t)(g_Water_Count/2)));
	}
}

uint8_t Detect_LOW_1s(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
	uint8_t count,i;
	count = 0;
	for(i=0;i<100;i++)
	{
		if(RESET == HAL_GPIO_ReadPin(GPIOx,GPIO_Pin))
		{
			count++;
		}
		DelayMs(10);
	}
	return count;
}

void Reed1_Proc()
{
	if(Detect_LOW_1s(GPIOB,GPIO_PIN_14)<95)
	{
		return;
	}
	
	if(0 == g_Reed_State) //first
	{
		g_Reed_State = 1;
		Water_Count_Normal();
		return;
	}
	
	if(1 == g_Reed_State) //repeat reed1
	{
		g_Reed_Error++;
		Dataeeprom_Write(ERR_ADDRESS,g_Reed_Error);
		//Dataeeprom_Write(STATE_ADDRESS,g_Reed_State);
		return;
	}
	
	if(2 == g_Reed_State) //normal
	{
		g_Reed_State = 1;
		Water_Count_Normal();
		return;
	}	

}

void Reed2_Proc()
{
	if(Detect_LOW_1s(GPIOB,GPIO_PIN_15)<95)
	{
		return;
	}
	
	if(0 == g_Reed_State) //first
	{
		g_Reed_State = 2;
		Water_Count_Normal();
		return;
	}
	
	if(2 == g_Reed_State) //repeat reed2
	{
		g_Reed_Error++;
		Dataeeprom_Write(ERR_ADDRESS,g_Reed_Error);
		//Dataeeprom_Write(STATE_ADDRESS,g_Reed_State);
		return;
	}
	
	if(1 == g_Reed_State) //normal
	{
		g_Reed_State = 2;
		Water_Count_Normal();
		return;
	}	
}

//force Transmit
//void Reed3_Proc()
//{
//	uint8_t i,count;
//	count = 0;
//	
//	/* Disable Wakeup Counter */
//  HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle);
//	
//	    /*## Setting the Wake up time ############################################*/
//    /*  RTC Wakeup Interrupt Generation:
//        Wakeup Time Base = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI))
//        Wakeup Time = Wakeup Time Base * WakeUpCounter 
//                    = (RTC_WAKEUPCLOCK_RTCCLK_DIV /(LSE or LSI)) * WakeUpCounter
//        ==> WakeUpCounter = Wakeup Time / Wakeup Time Base

//        To configure the wake up timer to 4s the WakeUpCounter is set to 0x1FFF:
//          RTC_WAKEUPCLOCK_RTCCLK_DIV = RTCCLK_Div16 = 16 
//          Wakeup Time Base = 16 /(~39.000KHz) = ~0,410 ms
//          Wakeup Time = ~4s = 0,410ms  * WakeUpCounter
//          ==> WakeUpCounter = ~4s/0,410ms = 9750 = 0x2616 */
//    //HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, 0x2616, RTC_WAKEUPCLOCK_RTCCLK_DIV16);
//	
//	
//	for(i=0;i<100;i++)
//	{
//		HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle);
//		HAL_RTCEx_SetWakeUpTimer_IT(&RtcHandle, 0x64, RTC_WAKEUPCLOCK_RTCCLK_DIV16); //41ms
//		HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
//		SystemClock_Config();
//		while(wakeup_flag == 0);
//		if(RESET == HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14))
//		{
//			count++;
//		}
//		wakeup_flag = 0;
//	}
//	
//	HAL_RTCEx_DeactivateWakeUpTimer(&RtcHandle);

//	//SystemClock_Config();
//		
//  if(count < 95)
//	{
//		return;
//	}
//	
//	osal_set_event(HardWare_taskID,0x0010);//delay test
//}


void ADC_Convert(void)
{
  uint16_t Ref_Value[5],Bat_val,i;
  uint32_t VRefCal_address, VRefCal_data, VSum;   // calibration at 3V
	
	Battery_Warning = 0;
	
  ADC_ChannelConfTypeDef   sConfig;
	
  AdcHandle.Instance = ADC1;
  AdcHandle.Init.OversamplingMode      = DISABLE;
  AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV1;
  AdcHandle.Init.LowPowerAutoOff       = DISABLE;
  AdcHandle.Init.LowPowerFrequencyMode = ENABLE;
  AdcHandle.Init.LowPowerAutoWait      = ENABLE;
    
  AdcHandle.Init.Resolution            = ADC_RESOLUTION12b;
  AdcHandle.Init.SamplingTime          = ADC_SAMPLETIME_7CYCLES_5;
  AdcHandle.Init.ScanDirection         = ADC_SCAN_DIRECTION_UPWARD;
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.ContinuousConvMode    = DISABLE;
  AdcHandle.Init.DiscontinuousConvMode = ENABLE;
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIG_EDGE_NONE;
  AdcHandle.Init.EOCSelection          = EOC_SINGLE_CONV;
  AdcHandle.Init.DMAContinuousRequests = DISABLE;
 
  if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    while(1);
  }
	
  if (HAL_ADCEx_Calibration_Start(&AdcHandle, ADC_SINGLE_ENDED) != HAL_OK)
  {
    while(1);
  }
	
  //==========================================================================//	
  sConfig.Channel = ADC_CHANNEL_17;    
	
  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    while(1);
  }
  VSum = 0;
	
  if(HAL_ADC_Start(&AdcHandle) != HAL_OK)
  {
     while(1);
  }
  
  for(i=0;i<5;i++)
  {    	
      HAL_ADC_PollForConversion(&AdcHandle, 100);
      
      if(HAL_ADC_GetState(&AdcHandle) == HAL_ADC_STATE_EOC)
      {
    		
       Ref_Value[i] = (HAL_ADC_GetValue(&AdcHandle));
       VSum += Ref_Value[i];
	   
      }	
  }
  
  VSum = VSum/5;
  
   // use internal 1.2v reference voltage
   //Battery_Value=((120*4096)/Ref_Value[0])+30;
   
   VRefCal_address = 0x1ff80078;

   VRefCal_data = *((uint16_t *)VRefCal_address);
	
   if (HAL_ADC_DeInit(&AdcHandle) != HAL_OK)
   {
      while(1);
   }
	
	 
   // REFINT_CAL value is stored at 0x1ff80078 - 0x1ff80079
   // VDDA = 3V*VREFINT_CAL / ADC_VALUE
   // VDDA + Diode = battery voltage
   Bat_val = 300*VRefCal_data/VSum + 21;
   if(Bat_val > 370 || Bat_val < 340) {Battery_Warning = 1;} //return;
   Battery_Value = Bat_val;

}

void Moter_Init(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	__GPIOB_CLK_ENABLE();
	
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  GPIO_InitStructure.Pin = GPIO_PIN_12;
  GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);//MOTER_IN1

  GPIO_InitStructure.Pin = GPIO_PIN_13;

  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);//MOTER_IN2*/
}

void Moter_DeInit(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_12;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);//MOTER_IN1
	
  GPIO_InitStructure.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);//MOTER_IN2
	
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
}

void Moter_Open(void)
{
	Moter_Init();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); 
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	DelayMs(3000);
	Moter_DeInit();
}

void Moter_Close(void)
{
	Moter_Init();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); 
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	DelayMs(3000);
	Moter_DeInit();
}

void Led_Init()
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	__GPIOA_CLK_ENABLE();
	
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_9;
  GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Led_DeInit()
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
}

void Buzz_Init()
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	
	__GPIOA_CLK_ENABLE();
	

  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Pin = GPIO_PIN_15;
  GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);		
}

void Buzz_DeInit()
{
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);
}

void Buzz_On()  //500Hz
{
	uint8_t i;
	for(i=0;i<200;i++)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_15);
		DelayMs(1);
	}
}

void Buzz_Finish()
{
	uint32_t i;
	for(i=0;i<100000;i++)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_15);
		DelayMs(1);
	}
}

void led_error_ind()
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	DelayMs(300);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	DelayMs(300);	
}


void Test_Proc()
{
	uint8_t i;
	//32768 test
	if(0 == __HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY))
	{
		Buzz_On();
		while(1)
		{
			led_error_ind();			
			DelayMs(1000);
		}
	}
	//power test
	ADC_Convert();
	if(1 == Battery_Warning)
	{
		Buzz_On();
		while(1)
		{
			for(i=0;i<2;i++)
			{
				led_error_ind();
			}
			DelayMs(1000);
		}
	}
	
	//Lora SendRcv test
	APP_Send();
	osal_start_timerEx(HardWare_taskID,MSG_TEST_SEND,1000);
}

void Test_Send_Result_Proc()
{
	uint8_t i;
	if(0 == Send_Success)
	{
		Buzz_On();
		while(1)
		{
			for(i=0;i<3;i++)
			{
				led_error_ind();
			}
			DelayMs(1000);
		}
	}
	Buzz_Finish();
}


//void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
//{
//	wakeup_flag=1;
//}

