/*
(C)2015 NPLink

Description: LoRa MAC layer user application interface.

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Robxr

history: V1.0 Robxr create file and add maintainer functions
         		V1.1 zhangjh Modify functions parameters type, include 4 functions as flow:
              LoRaMac_setAppLayerParameter
				LoRaMac_getAppLayerParameter
				LoRaMac_setMacLayerParameter
				LoRaMac_getMacLayerParameter
			V1.2 :
			  1) XLH/Robxr Add mac mode set function and low power mode set function:
				LoRaMac_setMode
				LoRaMac_setlowPowerMode
			  2) add three MAC Layer parameters:
			   PARAMETER_PHY_FREQUENCY
			   PARAMETER_PHY_SPREADING_FACTOR
			   PARAMETER_PHY_MODULATION_MODE
*/

#ifndef __LORAMACUSR_H__
#define __LORAMACUSR_H__

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "utilities.h"

#if defined(__CC_ARM) || defined(__GNUC__)
#define PACKED                                      __attribute__( ( __packed__ ) )
#elif defined( __ICCARM__ )
#define PACKED                                      __packed
#else
    #warning Not supported compiler type
#endif

/*Macros--------------------------------------------------------------------*/
//define work mode of mote (LoRaMac or PhyMac)
//����MAC�Ĺ���ģʽ��������LoRaMac(��Э��)��phyMac(����Э��)
#define MODE_LORAMAC									0 
#define MODE_PHY											1

//define modulation mode of mote (LoRa or FSK)
//���������оƬ�ĵ��Ʒ�ʽ��������LoRa���ƻ�FSK����
#define MODULATION_FSK								0 
#define MODULATION_LORA								1

//define return status
//����MAC�㴦��״̬
#define LORAMAC_USR_SUCCESS							0//�ɹ�
#define LORAMAC_USR_INVALID_PARAMETER		1//��Ч����
#define LORAMAC_USR_FAILURE							0xFF//ʧ��

//define parameter IDs of LoRaMac application layer
//����Ӧ�ò����ID��
#define PARAMETER_DEV_ADDR 								(u32)(1 << 0)//�豸��ַ(4B)
#define PARAMETER_DEV_EUI									(u32)(1 << 1)
#define PARAMETER_APP_EUI									(u32)(1 << 2)//LoRaWAN AppEUIֵ
#define PARAMETER_APP_KEY									(u32)(1 << 3)//LoRaWAN AppKey����ʹ��over-the-air activationʱʹ��
#define PARAMETER_NWK_SKEY								(u32)(1 << 4)//LoRaWAN NwkSkey����activation by personalizationʱʹ��
#define PARAMETER_APP_SKEY								(u32)(1 << 5)//LoRaWAN AppSkey����activation by personalizationʱʹ��
#define PARAMETER_DEV_TXPOWER 				    (u32)(1 << 6)//���ݷ��͹���

//define parameter IDs of LoRaMac mac layer
//����MAC�����ID��
#define PARAMETER_BANDS 									(u32)(1 << 0)//LoRaWAN MACģʽ����ʱʹ�õ�Ƶ�㣬��ǰ֧��1��Ƶ��
#define PARAMETER_CHANNELS								(u32)(1 << 1)//LoRaWAN MACģʽ����ʱ����Ƶ����ʹ�õ��ŵ�����ǰ֧�����16���ŵ�
#define PARAMETER_DATARATE								(u32)(1 << 2)//LoRaWAN MACģʽ����ʱ����������
#define PARAMETER_ADR_SWITCH							(u32)(1 << 3)//LoRaWAN MACģʽ����ʱ��ADRʹ�ܻ�ȥʹ��
#define PARAMETER_PHY_FREQUENCY			      (u32)(1 << 4)//phy MACģʽ����ʱ������Ƶ��
#define PARAMETER_PHY_SPREADING_FACTOR		(u32)(1 << 5)//phy MACģʽ����ʱ��LORA���Ʒ�ʽ�µ���Ƶ���ӣ���ЧֵΪ7-12
#define PARAMETER_PHY_MODULATION_MODE			(u32)(1 << 6)//phy MACģʽ����ʱ�����Ʒ�ʽ��ѡ��
#define PARAMETER_FSK_FDEV								(u32)(1 << 7)//FSK���Ʒ�ʽ�µ�Ƶƫ
#define PARAMETER_FSK_DATARATE						(u32)(1 << 8)//FSK���Ʒ�ʽ�µ�����
#define PARAMETER_FSK_BANDEIDTH						(u32)(1 << 9)//FSK���Ʒ�ʽ�µĴ���
#define PARAMETER_FSK_AFC_BANDWIDTH				(u32)(1 << 10)//FSK���Ʒ�ʽ�µ�afcbandwidth

//define length of parameters
//�����������
#define APP_EUI_LEN									8
#define MOTE_DEV_ADDR_LEN						4
#define APP_KEY_LEN									16
#define NWK_SKEY_LEN								16
#define APP_SKEY_LEN								16

#define LORA_MAX_NB_BANDS 					1//only one band is supported currentlly.
#define LORA_MAX_NB_CHANNELS 				16//only 16 channels are supported currentlly.

//define TxPower 
//���巢�͹���
#define TX_POWER_MAX_INDEX					5

#define TX_POWER_20_DBM             0
#define TX_POWER_14_DBM             1
#define TX_POWER_11_DBM             2
#define TX_POWER_08_DBM            	3
#define TX_POWER_05_DBM             4
#define TX_POWER_02_DBM             5

/*!
 * LoRaMac datarates definition
 */
#define DR_0                                        0  // SF12 - BW125
#define DR_1                                        1  // SF11 - BW125
#define DR_2                                        2  // SF10 - BW125
#define DR_3                                        3  // SF9  - BW125
#define DR_4                                        4  // SF8  - BW125
#define DR_5                                        5  // SF7  - BW125
#define DR_6                                        6  // SF7  - BW250
#define DR_7                                        7  // FSK

/* typedef -----------------------------------------------------------*/
typedef struct
{
    uint16_t DCycle;
    int8_t TxMaxPower;
    uint64_t LastTxDoneTime;
    uint64_t TimeOff;
} PACKED Band_t;

/*!
 * LoRaMAC channels parameters definition
 */
typedef union
{
    int8_t Value;
    struct
    {
        int8_t Min : 4;
        int8_t Max : 4;
    }PACKED Fields;
}PACKED DrRange_t;


typedef struct
{
    uint32_t Frequency; // Hz
    DrRange_t DrRange;  // Max datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
                        // Min datarate [0: SF12, 1: SF11, 2: SF10, 3: SF9, 4: SF8, 5: SF7, 6: SF7, 7: FSK]
    uint8_t Band;       // Band index
}PACKED ChannelParams_t;

/* typedef -----------------------------------------------------------*/
typedef struct LoRaMacMacPara
{
	Band_t bands[LORA_MAX_NB_BANDS];//LORA MAC��Ƶ�㶨��
	ChannelParams_t channels[LORA_MAX_NB_CHANNELS];//LORA MAC���ŵ�����
	u8 datarate;//LORA MAC�ķ�������
	bool lora_mac_adr_switch ;//LORA MAC��ADRʹ�����
	u32 phyFrequency;//phy MAC��Ƶ�㶨��
	u8  phySF;//phy MAC��LORA���Ʒ�ʽʱ����Ƶ���� 
	u8  phyModulation;//phy MAC�ĵ��Ʒ�ʽ����
	u32 fskFdev;
	u32 fskDatarate;
	u32 fskBandwidth;
	u32 fskAfcBandwidth;
}LoRaMacMacPara_t;

typedef struct LoRaMacAppPara
{
	u32 devAddr;//�豸��ַ
	u8  txPower;//���͹���
	u8  appEUI[APP_EUI_LEN];//LoRaWAN AppEUIֵ
	u8 	appKey[APP_KEY_LEN];//LoRaWAN AppKey
	u8 	nwkSKey[NWK_SKEY_LEN];//LoRaWAN NwkSkey
	u8 	appSKey[APP_SKEY_LEN];//LoRaWAN AppSkey
}LoRaMacAppPara_t;

/* function prototypes -----------------------------------------------*/
/*!
 * LoRaMac_setAppLayerParameter -- ����APP �����
 *
 * \param [IN]  pdata_in -- ָ�������������ָ�룬���ݿռ��ɵ���������
 *                   parameterIDs -- ����ID�ţ���ʹ��λ��ʽͬʱ���ö��������֧�ֵ�ID������:
 							PARAMETER_DEV_ADDR
 							PARAMETER_APP_EUI
 							PARAMETER_APP_KEY
 							PARAMETER_NWK_SKEY
 							PARAMETER_APP_SKEY
 */
u8 LoRaMac_setAppLayerParameter( void* pdata_in, u32 parameterIDs);

/*!
 * LoRaMac_getAppLayerParameter -- ��ȡAPP �㵱ǰ���ò���ֵ
 *
 * \param [IN]  pdata_out -- ָ�������������ָ�룬���ݿռ��ɵ���������
 *                   parameterIDs -- ����ID�ţ���ʹ��λ��ʽͬʱ���ö��������֧�ֵ�ID������:
 							PARAMETER_DEV_ADDR
 							PARAMETER_APP_EUI
 							PARAMETER_APP_KEY
 							PARAMETER_NWK_SKEY
 							PARAMETER_APP_SKEY
 */
u8 LoRaMac_getAppLayerParameter( void* pdata_out, u32 parameterIDs);

/*!
 * LoRaMac_setMacLayerParameter -- ����MAC �����
 *
 * \param [IN]  pdata_in -- ָ�������������ָ�룬���ݿռ��ɵ���������
 *                   parameterIDs -- ����ID�ţ���ʹ��λ��ʽͬʱ���ö��������֧�ֵ�ID������:
 							PARAMETER_BANDS
 							PARAMETER_CHANNELS
 							PARAMETER_ADR_SWITCH
 							PARAMETER_PHY_FREQUENCY
 							PARAMETER_PHY_SPREADING_FACTOR
 							PARAMETER_PHY_MODULATION_MODE
 */
u8 LoRaMac_setMacLayerParameter( void* pdata_in, u32 parameterIDs);

/*!
 * LoRaMac_getMacLayerParameter -- ��ȡMAC �㵱ǰ���ò���ֵ
 *
 * \param [IN]  pdata_out -- ָ�������������ָ�룬���ݿռ��ɵ���������
 *                   parameterIDs -- ����ID�ţ���ʹ��λ��ʽͬʱ���ö��������֧�ֵ�ID������:
 							PARAMETER_BANDS
 							PARAMETER_CHANNELS
 							PARAMETER_ADR_SWITCH
 							PARAMETER_PHY_FREQUENCY
 							PARAMETER_PHY_SPREADING_FACTOR
 							PARAMETER_PHY_MODULATION_MODE
 */
u8 LoRaMac_getMacLayerParameter( void* pdata_out, u32 parameterIDs);

/*!
 * LoRaMac_setMode -- ����MAC�㹤��ģʽ
 *
 * \param [IN]  mode -- ����ģʽ��ȡֵΪ:
 											MODE_LORAMAC -- LORA MAC��ʽ����
 											MODE_PHY -- phy MAC��ʽ����
 *                   
 */
u8 LoRaMac_setMode(u8 mode);

/*!
 * LoRaMac_setlowPowerMode -- ����radio���ֵ͹���,ʹ�ܺ�radio���ֽ��رվ��񣬲�����sleep״̬
 *
 * \param [IN]  enable -- ʹ�����ȡֵΪ:
 							TRUE -- ʹ�ܵ͹���
 							FALSE -- ȥʹ�ܵ͹���			
 *                   
 */
void LoRaMac_setlowPowerMode(u8 enable);


#endif // __LORAMAC_H__
