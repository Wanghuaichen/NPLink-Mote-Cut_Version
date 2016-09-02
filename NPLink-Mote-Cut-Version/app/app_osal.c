/*
(C)2015 NPLink

Description: app task implementation，本APP例程可实现LORAMAC、PHYMAC、低功耗3个应用之间的切换
						用户应该根据需要进行逻辑修改
License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx.h"
#include "stm32l0xx_hal_rtc.h"
#include <string.h>
#include <stdio.h>

#include "osal_memory.h"
#include "osal.h"
#include "app_osal.h"
#include "loraMac_osal.h"
#include "LoRaMacUsr.h"

#include "radio.h"
#include "timer.h"
#include "uart_board.h"
#include "rtc_board.h"
#include "delay.h"
#include "WaterMeter.h"
/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define HEAD_ID 0x7e;
#define MESSAGE_ID_HIGH 0x00
#define MESSAGE_ID_LOW 0x10
#define WATER_SCALE 0x0A;
#define VERSION_HIGH  0x01;
#define VERSION_LOW  0x01;


/* Private variables ---------------------------------------------------------*/
u8 APP_taskID;
char Rx_buf[60]; //buffer for oled display
__IO ITStatus UartReady = RESET;
u8 g_number = 0;

u16 g_msg_count;
u8 message_original[70];
u8 message_translated[70];
u8 message_length;

LoRaMacAppPara_t g_appData;//定义APP 参数结构体
LoRaMacMacPara_t g_macData;//定义MAC参数结构体

uint8_t txuartdataflag ;
u8 uucount = 5;
u8 debugEnable = FALSE;

uint8_t Send_Success;
uint8_t Send_Retry_Times;
uint8_t Send_Retry_Count;

/* variables -----------------------------------------------------------*/
extern UART_HandleTypeDef UartHandle;

void message_pack(void);
void message_translate(void);

loraMAC_msg_t* pMsgSend = NULL;
loraMAC_msg_t* pMsgRecieve = NULL;


/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
void APP_Init(u8 task_id)
{
	APP_taskID = task_id;

	//开启或关闭MAC层的串口打印信息
	#ifdef USE_DEBUG
	debugEnable = TRUE;
	#endif
	
  //设置发射功率
  g_appData.txPower = TX_POWER_20_DBM;
 	LoRaMac_setAppLayerParameter(&g_appData, PARAMETER_DEV_TXPOWER );
 	
	//显示Mote ID
	LoRaMac_getAppLayerParameter(&g_appData, PARAMETER_DEV_ADDR);
//	APP_ShowMoteID(g_appData.devAddr);

	memset( &g_macData , 0 , sizeof(g_macData) );
	
#if 1
  //设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 779500000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 779700000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 779900000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	//发送速率
	g_macData.datarate = DR_5;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
#else

#if defined( USE_BAND_433 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 433175000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 433375000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 433575000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;
	//设置信道4
	g_macData.channels[3].Frequency = 433775000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 434175000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
	//设置信道6
	g_macData.channels[5].Frequency = 434375000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 434575000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 434775000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_470 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 470300000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 470500000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 470700000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;
	//设置信道4
	g_macData.channels[3].Frequency = 470900000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 471100000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
	//设置信道6
	g_macData.channels[5].Frequency = 471300000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;	
	//设置信道7
	g_macData.channels[6].Frequency = 471500000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 471700000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_490 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 490300000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 490500000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 490700000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;	
	//设置信道4
	g_macData.channels[3].Frequency = 490900000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 491100000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
	//设置信道6
	g_macData.channels[5].Frequency = 491300000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;	
	//设置信道7
	g_macData.channels[6].Frequency = 491500000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 491700000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_780 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 779500000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 779700000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 779900000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;	
	//设置信道4
	g_macData.channels[3].Frequency = 780100000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 786500000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;	
	//设置信道6
	g_macData.channels[5].Frequency = 786700000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 786900000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 787100000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_868 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 868100000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 868300000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 868500000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;	
	//设置信道4
	g_macData.channels[3].Frequency = 868700000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 869100000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
  //设置信道6
	g_macData.channels[5].Frequency = 869300000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 869500000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 869700000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_US915 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 902300000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 902500000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 902700000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;
	//设置信道4
	g_macData.channels[3].Frequency = 902900000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 907300000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
  //设置信道6
	g_macData.channels[5].Frequency = 907500000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 907700000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 907900000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#elif defined( USE_BAND_AU915 )

	//设置LORAMAC工作模式的参数(LoRa调制)
	//设置信道1
	g_macData.channels[0].Frequency = 915200000;//频点
	g_macData.channels[0].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 ); //速率范围:((最高速率<<4) | (最低速率))
	g_macData.channels[0].Band = 0;
	//设置信道2
	g_macData.channels[1].Frequency = 915400000;
	g_macData.channels[1].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[1].Band = 0;
	//设置信道3
	g_macData.channels[2].Frequency = 915600000;
	g_macData.channels[2].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[2].Band = 0;
	//设置信道4
	g_macData.channels[3].Frequency = 915800000;
	g_macData.channels[3].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[3].Band = 0;
	//设置信道5
	g_macData.channels[4].Frequency = 916000000;
	g_macData.channels[4].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[4].Band = 0;
	//设置信道6
	g_macData.channels[5].Frequency = 916200000;
	g_macData.channels[5].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[5].Band = 0;
	//设置信道7
	g_macData.channels[6].Frequency = 916400000;
	g_macData.channels[6].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[6].Band = 0;
	//设置信道8
	g_macData.channels[7].Frequency = 916600000;
	g_macData.channels[7].DrRange.Value = ( ( DR_5 << 4 ) | DR_0 );
	g_macData.channels[7].Band = 0;
	//发送速率
	g_macData.datarate = DR_5;
	//ADR开启或关闭
	g_macData.lora_mac_adr_switch = TRUE;
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH | PARAMETER_DATARATE);
	//设置使用LoRaMAC
	LoRaMac_setMode(MODE_LORAMAC);
	
#endif	
	
#endif

#if 0
		//设置LORAMAC工作模式的参数(FSK调制)
		//设置信道1
		g_macData.channels[0].Frequency = 779700000;//频点
		g_macData.channels[0].DrRange.Value = ( ( DR_7 << 4 ) | DR_7 ); //速率范围:((最高速率<<4) | (最低速率))
		g_macData.channels[0].Band = 0;
		//设置信道2
		g_macData.channels[1].Frequency = 779700000;
		g_macData.channels[1].DrRange.Value = ( ( DR_7 << 4 ) | DR_7 );
		g_macData.channels[1].Band = 0;
		//设置信道3
		g_macData.channels[2].Frequency = 779700000;
		g_macData.channels[2].DrRange.Value = ( ( DR_7 << 4 ) | DR_7 );
		g_macData.channels[2].Band = 0;
		//发送速率
		g_macData.datarate = DR_7;
		//ADR开启或关闭
		g_macData.lora_mac_adr_switch = FALSE;
		//设置FSK参数
		g_macData.fskDatarate= 200000;//200000;//FSK调制下的发送速率
		g_macData.fskFdev = 100000;//100000;//FSK调制下的频偏
		g_macData.fskBandwidth = 200000;//200000;//FSK调制下的接收带宽
		g_macData.fskAfcBandwidth = 249999;//249999;//FSK调制下的AFC带宽
		LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_CHANNELS | PARAMETER_ADR_SWITCH \
																	| PARAMETER_DATARATE | PARAMETER_FSK_FDEV | PARAMETER_FSK_DATARATE \
																	| PARAMETER_FSK_BANDEIDTH | PARAMETER_FSK_AFC_BANDWIDTH );
		//设置使用LoRaMAC
		LoRaMac_setMode(MODE_LORAMAC);
#endif


#if 0
	//设置PHYMAC工作模式的参数(LoRa调制方式)
	g_macData.phyFrequency = 779700000;//频率(Hz)
	g_macData.phySF = 7; //扩频因子(7-12)
	g_macData.phyModulation = MODULATION_LORA;//调制方式(FSK or LORA)
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_PHY_FREQUENCY | PARAMETER_PHY_SPREADING_FACTOR \
															| PARAMETER_PHY_MODULATION_MODE );	//设置使用PhyMAC
	LoRaMac_setMode(MODE_PHY);
#endif

#if 0
	//设置PHYMAC工作模式的参数(FSK调制方式)
	g_macData.phyFrequency = 779700000;//频率(Hz)
	g_macData.phyModulation = MODULATION_FSK;//调制方式(FSK or LORA)	
	g_macData.fskDatarate= 100000;//FSK调制下的发送速率
	g_macData.fskFdev = 50000;//FSK调制下的频偏
	g_macData.fskBandwidth = 50000;//FSK调制下的带宽
	g_macData.fskAfcBandwidth = 83333;//FSK调制下的AFC带宽
	LoRaMac_setMacLayerParameter(&g_macData, PARAMETER_PHY_FREQUENCY | PARAMETER_PHY_MODULATION_MODE \
															| PARAMETER_FSK_FDEV | PARAMETER_FSK_DATARATE | PARAMETER_FSK_BANDEIDTH \
															| PARAMETER_FSK_AFC_BANDWIDTH );
	//设置使用PhyMAC
	LoRaMac_setMode(MODE_PHY);
#endif

 	//osal_set_event(APP_taskID,APP_PERIOD_SEND);//启动发包
	
	g_msg_count = 0;
	Send_Success = 0;
	Send_Retry_Times =5;
}

void APP_Send()
{
	u8 dataCount;
	message_pack();
	message_translate();
	pMsgSend = (loraMAC_msg_t*)osal_msg_allocate(sizeof(loraMAC_msg_t));
	if(pMsgSend != NULL)
	{
		#ifdef USE_DEBUG
		HAL_UART_SendBytes("app send start...\n", osal_strlen("app send start...\n"));
		#endif
		osal_memset(pMsgSend,0,sizeof(loraMAC_msg_t));
		pMsgSend->msgID = TXREQUEST;
		pMsgSend->msgLen = 70;
		pMsgSend->msgData[0] = 0x7e;
		for(dataCount = 1; dataCount < message_length; dataCount++)
		{
			pMsgSend->msgData[dataCount] = message_translated[dataCount];
		}
		pMsgSend->msgData[dataCount] = 0x7e;
		osal_msg_send(LoraMAC_taskID,(u8*)pMsgSend);
	}
	g_msg_count++;
}

u16 APP_ProcessEvent( u8 task_id, u16 events )
{
	u8 len;
	u16 rx_id;
	RTC_DateTypeDef sdatestructureget;
	
  //system event
  if(events & SYS_EVENT_MSG)
  {
		//receive msg loop
		while(NULL != (pMsgRecieve = (loraMAC_msg_t*)osal_msg_receive(APP_taskID)))
		{
		//pMsgRecieve[0] is system event type
		switch(pMsgRecieve->msgID)
		{
		//tx done
		case TXDONE :
			HAL_UART_SendBytes("TXDONE...\n", osal_strlen("TXDONE...\n"));
			Send_Success = 1;
		  break; 
		//tx error
		case TXERR_STATUS: 
			HAL_UART_SendBytes("TXERROR...\n", osal_strlen("TXERROR...\n"));
		  if(Send_Retry_Count>0)
			{
				Send_Retry_Count--;
				APP_Send();
			}
		  break;
		//rx done
		case RXDONE:
				len = 0 ;
				g_number++ ;
				memset(Rx_buf , 0 ,sizeof(Rx_buf));                               
				osal_memcpy(Rx_buf,pMsgRecieve->msgData,pMsgRecieve->msgLen);
				len = pMsgRecieve->msgLen;
				Rx_buf[len] = 0;
				#ifdef USE_DEBUG
				HAL_UART_SendBytes("\n",1);
				HAL_UART_SendBytes((uint8_t *)Rx_buf,strlen(Rx_buf));
				#endif
		    rx_id = ((Rx_buf[0] << 8 ) | Rx_buf[1]);
				switch(rx_id)
				{
					case 0x8010:
						HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, FORMAT_BCD);
						RTC_CalendarConfig(Rx_buf[4], Rx_buf[5], Rx_buf[6], sdatestructureget.WeekDay, Rx_buf[7], Rx_buf[8], Rx_buf[9]);
						if(0x00 != Rx_buf[2])
						{
							break;
						}
						else
						{
							if(0x01==Rx_buf[3])
							{
								Moter_Close();
								Current_GateStatus = 0;
								Dataeeprom_Write(MOTOR_ADDRESS,0);
							}
							else if(0x02 == Rx_buf[3])
							{
								Moter_Open();
								Current_GateStatus = 1;
								Dataeeprom_Write(MOTOR_ADDRESS,1);
							}
							else
							{
								;
							}
						}
						break;
					case 0x00f0:
						break;
					case 0x00f1:
						break;
					case 0x00f8:
						break;
					default: break;
				}
				break;
		default: break;
		}
		osal_msg_deallocate((u8*)pMsgRecieve);
		}
		return (events ^ SYS_EVENT_MSG);
	}

	//send a packet event
	if(events & APP_PERIOD_SEND)
	{
		Send_Retry_Count = Send_Retry_Times;
		APP_Send();
	  //osal_start_timerEx(APP_taskID, APP_PERIOD_SEND,1000);//延时继续发送
		return (events ^ APP_PERIOD_SEND);
	}
	
	if(events & APP_REED3)
	{
		//HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
		//DelayMs(1000);
		
		if(RESET == HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12))
		{
			osal_start_timerEx(APP_taskID,APP_REED3_AFTER_3S,1000);
			osal_set_event(APP_taskID,APP_REED3_DETECT_3S);
		}
		return (events ^ APP_REED3);
	}
	
	if(events & APP_REED3_DETECT_3S)
	{
		if(SET == HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12))
		{
			events ^= APP_REED3_AFTER_3S;
			return (events ^= APP_REED3_DETECT_3S);
		}
	}
	
	if(events & APP_REED3_AFTER_3S)
	{
		events ^= APP_REED3_DETECT_3S;
		osal_start_timerEx(APP_taskID,APP_REED3_AFTER_10S,3000);
		return (events ^ APP_REED3_AFTER_3S);
	}
	
	if(events & APP_REED3_AFTER_10S)
	{		
		if(SET == HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_12))
		{
			g_Reed_Force_flag = 1;
			Send_Retry_Count = Send_Retry_Times;
			APP_Send();
			g_Reed_Force_flag = 0;
		}
		else
		{
			g_Reed_Steel_flag = 1;
			Send_Retry_Count = Send_Retry_Times;
			APP_Send();
			g_Reed_Steel_flag = 0;
		}
		return (events ^ APP_REED3_AFTER_10S);
	}
	return 0 ;
	
}


//display NPLink mote ID on the OLED
void APP_ShowMoteID( u32 moteID )
{

}

u16 Onboard_rand(void)
{
	return 0; //return TIM_GetCounter(TIM5);
}

void message_pack()
{
	u32 water_current;
	RTC_DateTypeDef sdatestructureget;
	RTC_TimeTypeDef stimestructureget;
	ADC_Convert();
	HAL_RTC_GetTime(&RtcHandle, &stimestructureget, FORMAT_BCD);
	HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, FORMAT_BCD);
	
	
	message_original[0] = HEAD_ID;
	
	message_original[1] = MESSAGE_ID_HIGH;
	message_original[2] = MESSAGE_ID_LOW;
	
	message_original[3] = 0x33;
	message_original[4] = 0x44;
	
	message_original[5] = 0x00;
	message_original[6] = 0x00;
	message_original[7] = (g_appData.devAddr >> 24) & 0xff;
	message_original[8] = (g_appData.devAddr >> 16) & 0xff;
	message_original[9] = (g_appData.devAddr >> 8) & 0xff;
	message_original[10] = g_appData.devAddr & 0xff;
	
	message_original[11] = (g_msg_count >> 8) & 0xff;
	message_original[12] = g_msg_count & 0xff;	
	
	water_current = g_Water_Init + (uint32_t)(g_Water_Count/2);
	
	message_original[13] = (water_current >> 24) & 0xff;
	message_original[14] = (water_current >> 16) & 0xff;
	message_original[15] = (water_current >> 8) & 0xff;
	message_original[16] = water_current & 0xff;	
  message_original[17] = WATER_SCALE;
	
	message_original[18] = (uint8_t)((g_Reed_Steel_flag <<6) | (g_Reed_Error<<1) | Battery_Warning);//exception

	message_original[19] = sdatestructureget.Year;
	message_original[20] = sdatestructureget.Month;
	message_original[21] = sdatestructureget.Date;
	message_original[22] = stimestructureget.Hours;
	message_original[23] = stimestructureget.Minutes;
	message_original[24] = stimestructureget.Seconds;
	
	message_original[25] = (Battery_Value >> 8) & 0xff; //power
	message_original[26] = Battery_Value & 0xff;

  message_original[27] = Current_GateStatus; //motor state
	
	message_original[28] = 0x78; //RSSI x
	
	message_original[29] = g_Reed_Force_flag;
	 
	message_original[30] = VERSION_HIGH;
	message_original[31] = VERSION_LOW;
}

u8 mess_crc_cal(u8* buf, u8 length)
{
	u8 result;
	u8 i;
	result = 0x00;
	for(i=0;i<length;i++)
	{
		result ^= buf[i];
	}
	return result;
}

void message_translate()
{
	u8 i;
	u8 crc_result;
	message_length = 1;
	for(i=1;i<32;i++)
	{
		if(message_original[i] == 0x7e)
		{
			message_translated[message_length++] = 0x7d;
			message_translated[message_length++] = 0x02;
		}
		else if(message_original[i] == 0x7d)
		{
			message_translated[message_length++] = 0x7d;
			message_translated[message_length++] = 0x01;
		}
		else
		{
			message_translated[message_length++] = message_original[i];
		}
	}
	crc_result = mess_crc_cal(message_original, 32);
	
	if(crc_result == 0x7e)
	{
		message_translated[message_length++] = 0x7d;
		message_translated[message_length++] = 0x02;
	}
	else if(crc_result == 0x7d)
	{
		message_translated[message_length++] = 0x7d;
		message_translated[message_length++] = 0x01;
	}
	else
	{
		message_translated[message_length++] = crc_result;
	}
	
}
/******************* (C) COPYRIGHT 2015 NPLink *****END OF FILE****/

