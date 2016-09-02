#ifndef APP_OSAL_H
#define APP_OSAL_H

#include "LoRaMacUsr.h"

extern u8 APP_taskID;

//extern LoRaMacAppPara_t g_appData;

/* Private macro -------------------------------------------------------------*/
//事件定义
#define APP_PERIOD_SEND     0x0001

#define APP_REED3                     0x0002
#define APP_REED3_DETECT_3S           0x0004
#define APP_REED3_AFTER_3S            0x0008
#define APP_REED3_AFTER_10S           0x0010

void APP_Init(u8 task_id);
u16  APP_ProcessEvent( u8 task_id, u16 events );
//void APP_ShowMoteID( u32 moteID );

#endif
