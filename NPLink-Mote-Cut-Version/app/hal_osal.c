/*
(C)2015 NPLink

Description: hal task implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/
/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx.h"
#include <string.h>
#include <stdio.h>
#include "osal_memory.h"
#include "osal.h"
#include "hal_osal.h"
#include "WaterMeter.h"
#include "app_osal.h"

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
u8 HardWare_taskID;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  HAL task hardware Initiate.
  * @param  u8 task_id -- task ID allocated by OSAL
  * @retval None
  */
void HardWare_Init(u8 task_id)
{
	HardWare_taskID = task_id;
	HAL_Init();
}

/**
  * @brief  HAL task event process routine
  * @param  u8 task_id -- task ID allocated by OSAL
  					  u16 events -- happened events of this task
  * @retval u16 -- events that haven't been processed
  */
u16 HardWare_ProcessEvent( u8 task_id, u16 events )
{
//		if (events & MSG_REED1) 
//    {
//      Reed1_Proc();
//      return events ^ MSG_REED1;
//    }		
//		
//		if (events & MSG_REED2) 
//    {
//      MSG_REED2();
//      return events ^ MSG_REED2;
//    }
		
		if (events & MSG_TEST) 
    {
      Test_Proc();
      return events ^ MSG_TEST;
    }
		
		if (events & MSG_TEST_SEND) 
    {
      Test_Send_Result_Proc();
      return events ^ MSG_TEST_SEND;
    }	
		

    return 0 ;
}


/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2015 STMicroelectronics *****END OF FILE****/

