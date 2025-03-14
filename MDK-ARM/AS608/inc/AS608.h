#ifndef __AS608_H
#define __AS608_H

#include "main.h"
#include "AS608_ERRCode.h"
#include "gpio.h"

#if defined (__CC_ARM)
#pragma anon_unions
#endif

#define CharBuffer1 0x01
#define CharBuffer2 0x02

typedef struct as608_io_t as608_io_t;
typedef struct as608_data_packet_t as608_data_packet_t;
typedef struct as608_init_t as608_init_t;
typedef struct as608_handle_t as608_handle_t;

struct as608_io_t
{
	host_io_t TX;
	host_io_t RX;
	host_io_t WAK;
};


struct as608_data_packet_t
{
	union
	{
		uint16_t data;
		struct
		{
			uint16_t byte1 : 8;
			uint16_t byte2 : 8;
		};
	} p_head;
	union
	{
		uint32_t data;
		struct
		{
			uint32_t byte1 : 8;
			uint32_t byte2 : 8;
			uint32_t byte3 : 8;
			uint32_t byte4 : 8;
		};
	} chip_addr;
	uint8_t packet_tag;
	union
	{
		uint16_t data;
		struct
		{
			uint16_t byte1 : 8;
			uint16_t byte2 : 8;
		};
	} p_len;
	as608_function_result_t confirmation_code;
	uint8_t param[256];
	union
	{
		uint16_t data;
		struct
		{
			uint16_t byte1 : 8;
			uint16_t byte2 : 8;
		};
	} check_sum;
};

struct as608_init_t
{
	UART_HandleTypeDef *huartx;
	as608_io_t IO;
	uint32_t chip_addr;
	uint32_t bond;
};

struct as608_handle_t
{
	UART_HandleTypeDef *huartx;
	as608_data_packet_t data_packet;
	as608_io_t IO;
	uint32_t chip_addr;
	uint32_t bond;
};

uint8_t AS608_GetWAK(as608_handle_t *handle);

int AS608_SendPacket(as608_handle_t *handle, uint8_t Packet_Type, uint16_t P_Len, const uint8_t *DataPacket);
as608_function_result_t AS608_GetImage(as608_handle_t *handle);
as608_function_result_t AS608_GenChar(as608_handle_t *handle, uint8_t Buffer_ID);
as608_function_result_t AS608_Match(as608_handle_t *handle);
as608_function_result_t AS608_Search(as608_handle_t *handle, uint8_t Buffer_ID, uint16_t StartPage, uint16_t PageNum);
as608_function_result_t AS608_RegModel(as608_handle_t *handle);
as608_function_result_t AS608_StoreChar(as608_handle_t *handle, uint8_t Buffer_ID, uint16_t Page_ID);
as608_function_result_t AS608_DeleteChar(as608_handle_t *handle, uint16_t Page_ID, uint16_t DeleteNum);
as608_function_result_t AS608_Empty(as608_handle_t *handle);
as608_function_result_t AS608_WriteReg(as608_handle_t *handle, uint8_t Reg_ID);
as608_function_result_t AS608_ReadSysPara(as608_handle_t *handle);
as608_function_result_t AS608_HighSpeedSearch(as608_handle_t *handle, uint8_t Buffer_ID, uint16_t StartPage, uint16_t PageNum);
as608_function_result_t AS608_ValidTempleteNum(as608_handle_t *handle, uint16_t *ValidN);
as608_function_result_t AS608_WriteNotepad(as608_handle_t *handle, uint8_t Page, uint8_t Content[32]);
as608_function_result_t AS608_ReadNotepad(as608_handle_t *handle, uint8_t Page, uint8_t *Content);

as608_function_result_t AS608_InputFingerprint(as608_handle_t *handle, uint16_t Page_ID);
as608_function_result_t AS608_BrushFingerprint(as608_handle_t *handle);


int AS608_Init(as608_handle_t *handle, as608_init_t *init_structure);

#define AS608_COMMAND_PACKET 0x01
#define AS608_DATA_PACKET 0x02
#define AS608_ACK_PACKET 0x07
#define AS608_END_PACKET 0x08

#endif
