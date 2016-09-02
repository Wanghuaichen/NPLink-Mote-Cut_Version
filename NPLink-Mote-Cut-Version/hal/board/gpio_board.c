/*
(C)2015 NPLink

Description: GPIO interrupt handle (for 1276/1279)

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

#include "board.h"
#include "gpio_board.h"
#include "WaterMeter.h"
#include "hal_osal.h"
#include "osal.h"
#include "app_osal.h"
//#include "uart_board.h"


GpioIrqHandler *GpioIrq[6] ;

void EXTI0_1_IRQHandler( void )
{
  //PB0 -- DIO3
	/* EXTI line interrupt detected */
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0) != RESET)
	{
	  if( GpioIrq[3] != NULL )
	  {
		  GpioIrq[3]( );
	  }
	  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	}

  //PB1 -- DIO2
	/* EXTI line interrupt detected */
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_1) != RESET)
	{
	  if( GpioIrq[2] != NULL )
	  {
		  GpioIrq[2]( );
	  }
	  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	}
	
}

void EXTI2_3_IRQHandler( void )
{
  //PB2 -- DIO1
	/* EXTI line interrupt detected */
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2) != RESET)
	{
	  if( GpioIrq[1] != NULL )
	  {
		  GpioIrq[1]( );
	  }
	  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
	}
	
}

void EXTI4_15_IRQHandler( void )
{
	//PB14 -- Reed1
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET) 
  { 
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
		//osal_set_event(HardWare_taskID,MSG_REED1);
		Reed1_Proc();
  }
	
	//PB15 -- Reed2
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET) 
  { 
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_15);
    //osal_set_event(HardWare_taskID,MSG_REED2);
		//HAL_UART_SendBytes("abc...\n", osal_strlen("abc...\n"));
		Reed2_Proc();
  }
	
	//PB5 -- TEST
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) 
  { 
    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_5);
    osal_set_event(HardWare_taskID,MSG_TEST);
  }

	//PA12 -- Reed3
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_12);
		//osal_set_event(APP_taskID,APP_REED3);
		osal_start_timerEx(APP_taskID,APP_REED3,50);
	}
	

  //PB10 -- DIO0
	/* EXTI line interrupt detected */
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_10) != RESET)
	{
	  if( GpioIrq[0] != NULL )
	  {
		  GpioIrq[0]( );
	  }
	  __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_10);
	}
}



