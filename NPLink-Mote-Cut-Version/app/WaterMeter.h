#ifndef __WATER_METER_H__
#define __WATER_METER_H__

#include "stm32l0xx.h"

#define WATER_ADDRESS                                 0x8080120
#define FLAG_ADDRESS                                  0x8080110
#define ERR_ADDRESS                                   0x8080124
#define STATE_ADDRESS                                 0x8080128
#define MOTOR_ADDRESS                                 0x808012C

#define MSG_TEST         0x0001
#define MSG_TEST_SEND    0x0002

//#define MSG_REED2        0x0010
//#define MSG_REED1        0x0020

extern uint32_t g_Water_Init;
extern uint32_t g_Water_Count;
extern uint8_t  g_Reed_Error;  
extern uint8_t  g_Reed_State;
extern uint8_t  g_Reed_Force_flag;
extern uint8_t  g_Reed_Steel_flag;
extern uint8_t Current_GateStatus;
extern uint16_t Battery_Value;
extern uint8_t  Battery_Warning;


void Dataeeprom_Write(uint32_t address,uint32_t data);
uint32_t Dataeeprom_Read(uint32_t address);
void GPIO_WaterMeter_Init(void);
void GPIO_WaterMeter_DeInit(void);
void Reed_Init(void);

void Reed1_Proc(void);
void Reed2_Proc(void);

void ADC_Convert(void);

void Test_Proc(void);
void Test_Send_Result_Proc(void);

void Moter_Open(void);

void Moter_Close(void);

#endif
