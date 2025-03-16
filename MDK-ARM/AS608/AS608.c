#include <stdio.h>
#include <string.h>
#include "AS608.h"
#include "usart.h"

static uint8_t rxdata[256] = {0};
static uint8_t rxdata_used = 0;
as608_handle_t *user = NULL;

#define AS608_READ_WAK(_HANDLE_) (((as608_handle_t *)_HANDLE_)->IO.WAK.GPIOx->IDR & ((as608_handle_t *)_HANDLE_)->IO.WAK.pin)

#define AS608_CHECK_ERROR(_R_) do {	\
	if((_R_) != AS608_SUCCESS)				\
	{																	\
		return (_R_);										\
	}																	\
} while(0)

#define AS608_CHECK_DEVICE(_H_) do { 					\
	if((_H_) == NULL || (_H_)->huartx == NULL)	\
	{																						\
		return -1;																\
	}																						\
} while(0)

uint8_t AS608_GetWAK(as608_handle_t *hdev)
{
	return AS608_READ_WAK(hdev) != 0;
}

static int as608_clear_rxdata(as608_handle_t *hdev)
{
	AS608_CHECK_DEVICE(hdev);
	memset(hdev->rxdata, 0, sizeof(hdev->rxdata));
	return 0;
}

int AS608_SendPacket(as608_handle_t *hdev, uint8_t Packet_Type, uint16_t P_Len, const uint8_t *DataPacket)
{
	AS608_CHECK_DEVICE(hdev);

	HAL_StatusTypeDef uart_retval = HAL_OK;
	uint32_t ticks_up = 0;
	int retval = 0;

	uint8_t packet_head[] = {
		0xEF, 0x01, /* 包头 */
		hdev->chip_addr >> 24, hdev->chip_addr >> 16, hdev->chip_addr >> 8, hdev->chip_addr, /* 芯片地址 */
		Packet_Type, /* 数据包类型 */
		P_Len >> 8, P_Len, /* 包长度 */
	};
	uint8_t packet[256] = {0};
	memmove(packet, packet_head, sizeof(uint8_t) * 9);
	memmove(packet + 9, DataPacket, sizeof(uint8_t) * P_Len);
	as608_clear_rxdata(hdev); //清除接收缓冲区
	ticks_up = HAL_GetTick + AS608_MAX_WAIT_MILLIS;
	do {
		uart_retval = HAL_UARTEx_ReceiveToIdle_IT(hdev->huartx, hdev->rxdata, sizeof(hdev->rxdata));
	} while(uart_retval != HAL_OK || HAL_GetTick() < ticks_up);
	if(uart_retval != HAL_OK)
	{
		return -1;
	}
	__HAL_DMA_DISABLE_IT(hdev->huartx->hdmarx, DMA_IT_HT);

	HAL_UART_Transmit(hdev, packet, 9 + P_Len, AS608_MAX_WAIT_MILLIS);
	return 0;
}

as608_retval_t AS608_GetImage(as608_handle_t *hdev)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x01;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_GenChar(as608_handle_t *hdev, uint8_t Buffer_ID)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x04;
	uint8_t Command_Code = 0x02;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Buffer_ID;
	uint8_t DataPacket[] = {Command_Code, Buffer_ID, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_Match(as608_handle_t *hdev)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x03;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_Search(as608_handle_t *hdev, uint8_t Buffer_ID, uint16_t StartPage, uint16_t PageNum)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x08;
	uint8_t Command_Code = 0x04;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Buffer_ID + StartPage + PageNum;
	uint8_t DataPacket[] = {Command_Code, Buffer_ID, StartPage >> 8, StartPage, PageNum >> 8, PageNum, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_RegModel(as608_handle_t *hdev)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x05;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_StoreChar(as608_handle_t *hdev, uint8_t Buffer_ID, uint16_t Page_ID)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x06;
	uint8_t Command_Code = 0x06;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Buffer_ID + Page_ID;
	uint8_t DataPacket[] = {Command_Code, Buffer_ID, Page_ID >> 8, Page_ID, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_DeleteChar(as608_handle_t *hdev, uint16_t Page_ID, uint16_t DeleteNum)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x07;
	uint8_t Command_Code = 0x0c;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Page_ID + DeleteNum;
	uint8_t DataPacket[] = {Command_Code, Page_ID >> 8, Page_ID, DeleteNum >> 8,
							DeleteNum, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_Empty(as608_handle_t *hdev)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x0d;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_WriteReg(as608_handle_t *hdev, uint8_t Reg_ID)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x05;
	uint8_t Command_Code = 0x0e;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Reg_ID;
	uint8_t DataPacket[] = {Command_Code, Reg_ID, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_ReadSysPara(as608_handle_t *hdev)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x0f;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_HighSpeedSearch(as608_handle_t *hdev, uint8_t Buffer_ID, uint16_t StartPage, uint16_t PageNum)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x08;
	uint8_t Command_Code = 0x1b;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Buffer_ID + StartPage + PageNum;
	uint8_t DataPacket[] = {Command_Code, Buffer_ID, StartPage >> 8, StartPage, PageNum >> 8, PageNum,
							CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_ValidTempleteNum(as608_handle_t *hdev, uint16_t *ValidN)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x1d;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_WriteNotepad(as608_handle_t *hdev, uint8_t Page, uint8_t Content[32])
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 36;
	uint8_t Command_Code = 0x18;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Page + Command_Code;
	for (uint8_t i = 0; i < 32; i++)
	{
		CheckNum += Content[i];
	}
	uint8_t DataPacket[] = {Command_Code, Page,
							Content[0], Content[1], Content[2], Content[3], Content[4], Content[5], Content[6], Content[7],
							Content[8], Content[9], Content[10], Content[11], Content[12], Content[13], Content[14], Content[15],
							Content[16], Content[17], Content[18], Content[19], Content[20], Content[21], Content[22], Content[23],
							Content[24], Content[25], Content[26], Content[27], Content[28], Content[29], Content[30], Content[31],
							CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
}

as608_retval_t AS608_ReadNotepad(as608_handle_t *hdev, uint8_t Page, uint8_t *Content)
{
	int retval = 0;
	HAL_UART_StateTypeDef uart_state = HAL_UART_STATE_READY;
	uint32_t ticks_up = 0;
	uint16_t P_Len = 4;
	uint8_t Command_Code = 0x19;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Page + Command_Code;
	uint8_t DataPacket[] = {Command_Code, Page, CheckNum >> 8, CheckNum};
	AS608_SendPacket(hdev, AS608_COMMAND_PACKET, P_Len, DataPacket);
	ticks_up = HAL_GetTick() + AS608_MAX_WAIT_MILLIS;
	do {
		uart_state = HAL_UART_GetState(hdev->huartx);
	} while(uart_state != HAL_UART_STATE_READY || HAL_GetTick() < ticks_up);
	retval = uart_state != HAL_UART_STATE_READY ? -1 : (int)hdev->rxdata[AS608_PACKET_CODE_MSK];
	return (as608_retval_t)retval;
	for (uint8_t i = 0; i < 32; i++)
	{
		Content[i] = hdev->rxdata[AS608_PACKET_PARAM_MSK + i];
	}
	return (as608_retval_t)retval;
}

as608_retval_t AS608_InputFingerprint(as608_handle_t *hdev, uint16_t Page_ID)
{
	as608_retval_t retval = AS608_SUCCESS;
	while (AS608_GetWAK(hdev) == 0);
	retval = AS608_GetImage(hdev);
	AS608_CHECK_ERROR(retval);

	retval = AS608_GenChar(hdev, CharBuffer1);
	AS608_CHECK_ERROR(retval);

	if (AS608_Search(hdev, CharBuffer1, 0, 255) == AS608_SUCCESS) //说明存在该指纹了
	{
		return AS608_SUCCESS;
	}

	while (AS608_GetWAK(hdev) == 0);

	retval = AS608_GetImage(hdev);
	AS608_CHECK_ERROR(retval);
	retval = AS608_GenChar(hdev, CharBuffer2);
	AS608_CHECK_ERROR(retval);
	retval = AS608_RegModel(hdev);
	AS608_CHECK_ERROR(retval);
	retval = AS608_StoreChar(hdev, CharBuffer2, Page_ID);

	return AS608_SUCCESS;
}

as608_retval_t AS608_BrushFingerprint(as608_handle_t *hdev)
{
	as608_retval_t retval = AS608_SUCCESS;
	while (AS608_GetWAK(hdev) == 0);
	
	retval = AS608_GetImage(hdev);
	AS608_CHECK_ERROR(retval);

	retval = AS608_GenChar(hdev, CharBuffer1);
	AS608_CHECK_ERROR(retval);

	retval = AS608_Search(hdev, CharBuffer1, 0, 255);

	return retval;
}

int AS608_Init(as608_handle_t *hdev, as608_init_t *init_structure)
{
	/*检测传入的初始化结构体地址是否为空*/
	if(init_structure == NULL || init_structure->huartx == NULL)
	{
		return -1;
	}

	/*不为空执行以下指令*/
	hdev->chip_addr = init_structure->chip_addr == 0 ? 0xffffffff : init_structure->chip_addr;
	hdev->bond = init_structure->bond == 0 ? 57600 : init_structure->bond;
	hdev->huartx = init_structure->huartx;
	hdev->IO = init_structure->IO;

	HAL_Delay(100);

	return 0;
}
