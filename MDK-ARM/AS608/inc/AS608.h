#ifndef __AS608_H
#define __AS608_H

#include "stm32f1xx_hal.h"
#include "AS608_ERRCode.h"
#include "gpio.h"

#if defined (__CC_ARM)
#pragma anon_unions
#endif

#define AS608_MAX_WAIT_MILLIS 1000 //阻塞动作最多等待时间宏定义

typedef struct as608_host_io_t
{
	GPIO_TypeDef *GPIOx;
	uint16_t pin;
} as608_host_io_t;

typedef struct as608_io_t
{
	as608_host_io_t TX;
	as608_host_io_t RX;
	as608_host_io_t WAK;
} as608_io_t;

typedef struct as608_init_t
{
	UART_HandleTypeDef *huartx;
	as608_io_t IO;
	uint32_t chip_addr;
	uint32_t bond;
} as608_init_t;

typedef struct as608_handle_t
{
	UART_HandleTypeDef *huartx;
	as608_io_t IO;
	uint32_t chip_addr;
	uint32_t bond;
	uint8_t rxdata[256];
} as608_handle_t;

uint8_t AS608_GetWAK(as608_handle_t *hdev);

int AS608_SendPacket(as608_handle_t *hdev, uint8_t Packet_Type, uint16_t P_Len, const uint8_t *DataPacket);
as608_retval_t AS608_GetImage(as608_handle_t *hdev);
as608_retval_t AS608_GenChar(as608_handle_t *hdev, uint8_t Buffer_ID);
as608_retval_t AS608_Match(as608_handle_t *hdev);
as608_retval_t AS608_Search(as608_handle_t *hdev, uint8_t Buffer_ID, uint16_t StartPage, uint16_t PageNum);
as608_retval_t AS608_RegModel(as608_handle_t *hdev);
as608_retval_t AS608_StoreChar(as608_handle_t *hdev, uint8_t Buffer_ID, uint16_t Page_ID);
as608_retval_t AS608_DeleteChar(as608_handle_t *hdev, uint16_t Page_ID, uint16_t DeleteNum);
as608_retval_t AS608_Empty(as608_handle_t *hdev);
as608_retval_t AS608_WriteReg(as608_handle_t *hdev, uint8_t Reg_ID);
as608_retval_t AS608_ReadSysPara(as608_handle_t *hdev);
as608_retval_t AS608_HighSpeedSearch(as608_handle_t *hdev, uint8_t Buffer_ID, uint16_t StartPage, uint16_t PageNum);
as608_retval_t AS608_ValidTempleteNum(as608_handle_t *hdev, uint16_t *ValidN);
as608_retval_t AS608_WriteNotepad(as608_handle_t *hdev, uint8_t Page, uint8_t Content[32]);
as608_retval_t AS608_ReadNotepad(as608_handle_t *hdev, uint8_t Page, uint8_t *Content);

as608_retval_t AS608_InputFingerprint(as608_handle_t *hdev, uint16_t Page_ID);
as608_retval_t AS608_BrushFingerprint(as608_handle_t *hdev);


int AS608_Init(as608_handle_t *hdev, as608_init_t *init_structure);

#define CharBuffer1 0x01
#define CharBuffer2 0x02

#define AS608_COMMAND_PACKET 0x01
#define AS608_DATA_PACKET 0x02
#define AS608_ACK_PACKET 0x07
#define AS608_END_PACKET 0x08

#define AS608_PACKET_HEAD_MSK 0 //包头掩码位
#define AS608_PACKET_ADDR_MSK 2 //包地址掩码位
#define AS608_PACKET_TAG_MSK 6 //包标识掩码位
#define AS608_PACKET_LEN_MSK 7 //包长度掩码位
#define AS608_PACKET_CODE_MSK 8 //包验证码掩码位
#define AS608_PACKET_PARAM_MSK 8 //包参数掩码位

#endif
