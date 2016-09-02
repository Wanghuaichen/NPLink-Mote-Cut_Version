/*
(C)2015 NPLink

Description: MAC layer osal interface.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr
*/
#ifndef LORAMAC_OSAL_H
#define  LORAMAC_OSAL_H

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>

/* Macros -------------------------------------------------------------*/
//loraMAC system msg types
#define TXDONE 					1//�������
#define RXDONE 					2//�������
#define TXREQUEST				3//��������
#define TXERR_STATUS    4//���ʹ���


/* Typedefs -----------------------------------------------------------*/
typedef struct loraMAC_msg
{
	uint8  msgID;//��ϢID��
	uint8  msgLen;//��Ϣ����
  int8_t msgRxRssi;//�����ź�ǿ��
	int8_t msgRxSnr;//���������
  uint16 frame_no ;//���к�
  int8_t tx_packet_status;//�������ݰ�״̬
  uint8  reserve;//�����ֽ�
	uint8 msgData[70];//��Ϣ����payload,���70�ֽ�
}loraMAC_msg_t;

/* Variables -----------------------------------------------------------*/
extern uint8_t NwkSKey[16];
extern uint8_t AppSKey[16];
extern uint32_t MoteIDAddr;
extern u8 	LoraMAC_taskID;

extern bool g_lora_mac_adr_switch ;
extern u8	mode ;

/* function prototypes -----------------------------------------------*/
//MAC�����ʼ������
void LoRaMAC_Init(u8 task_id);
//MAC������Ϣ������
u16  LoRaMAC_ProcessEvent( u8 task_id, u16 events );

#endif
