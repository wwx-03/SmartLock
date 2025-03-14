#include <stdio.h>
#include <string.h>
#include "AS608.h"
#include "usart.h"

#define AS608_READ_WAK(_HANDLE_) (((as608_handle_t *)_HANDLE_)->IO.WAK.GPIOx->IDR & ((as608_handle_t *)_HANDLE_)->IO.WAK.pin)

uint32_t chip_addr = 0xffffffff;
uint32_t AS608_Bond = 57600;

uint8_t AS608_GetWAK(as608_handle_t *handle)
{
	return AS608_READ_WAK(handle) != 0;
}

static inline HAL_StatusTypeDef AS608_UART_Send(as608_handle_t *handle, const uint8_t *pData, uint16_t size, uint32_t time_out)
{
	return HAL_UART_Transmit(handle->huartx, pData, size, time_out);
}

static void Delete_RecDataPacket(as608_handle_t *handle)
{
	handle->data_packet.p_head.data = 0;
	handle->data_packet.chip_addr.data = 0;
	handle->data_packet.packet_tag = 0;
	handle->data_packet.p_len.data = 0;
	handle->data_packet.confirmation_code = AS608_SUCCESS;
	for (uint16_t i = 0; i < 256; i++)
	{
		handle->data_packet.param[i] = 0;
	}
	handle->data_packet.check_sum.data = 0;
}

static void encoding_packet(as608_handle_t *handle)
{
	uint16_t index = 0;
	uint16_t i;
	if(handle->huartx == &huart1)
	{
		handle->data_packet.p_head.byte2 = huart1ReceiveBuffer[index ++];
		handle->data_packet.p_head.byte1 = huart1ReceiveBuffer[index ++];
		handle->data_packet.chip_addr.byte4 = huart1ReceiveBuffer[index ++];
		handle->data_packet.chip_addr.byte3 = huart1ReceiveBuffer[index ++];
		handle->data_packet.chip_addr.byte2 = huart1ReceiveBuffer[index ++];
		handle->data_packet.chip_addr.byte1 = huart1ReceiveBuffer[index ++];
		handle->data_packet.packet_tag = huart1ReceiveBuffer[index ++];
		handle->data_packet.p_len.byte2 = huart1ReceiveBuffer[index ++];
		handle->data_packet.p_len.byte1 = huart1ReceiveBuffer[index ++];
		handle->data_packet.confirmation_code = huart1ReceiveBuffer[index ++];
		if(handle->data_packet.p_len.data > 3)
		{
			for(i = 0; i < handle->data_packet.p_len.data - 3; i ++)
			{
				handle->data_packet.param[i] = huart1ReceiveBuffer[index ++];
			}
		}
		handle->data_packet.check_sum.byte2 = huart1ReceiveBuffer[index ++];
		handle->data_packet.check_sum.byte1 = huart1ReceiveBuffer[index];
	}
}

int AS608_SendPacket(as608_handle_t *handle, uint8_t Packet_Type, uint16_t P_Len, const uint8_t *DataPacket)
{
	/*���󷵻�*/
	if(handle->huartx == NULL)
	{
		return -1;
	}

	uint8_t HeadPacket[] = {
		0xEF, 0x01,													 /* ��ͷ */
		chip_addr >> 24, chip_addr >> 16, chip_addr >> 8, chip_addr, /* оƬ��ַ */
		Packet_Type,												 /* ����ʶ */
		P_Len >> 8, P_Len,											 /* ������ */
	};
	Delete_RecDataPacket(handle);
	// ɾ��֮ǰ���ܵ������ݰ�
	huart1IdleFlag = 0;
	// ��ʼ����Ӧ���?,��������ɱ�־λ��ǰ��?0

	AS608_UART_Send(handle, HeadPacket, 9, HAL_MAX_DELAY);
	AS608_UART_Send(handle, DataPacket, P_Len, HAL_MAX_DELAY);


	return 0;
}

as608_function_result_t AS608_GetImage(as608_handle_t *handle)
{
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x01;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1);
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_GenChar(as608_handle_t *handle, uint8_t Buffer_ID)
{
	uint16_t P_Len = 0x04;
	uint8_t Command_Code = 0x02;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Buffer_ID;
	uint8_t DataPacket[] = {Command_Code, Buffer_ID, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1);
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_Match(as608_handle_t *handle)
{
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x03;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1);
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_Search(as608_handle_t *handle, uint8_t Buffer_ID, uint16_t StartPage, uint16_t PageNum)
{
	uint16_t P_Len = 0x08;
	uint8_t Command_Code = 0x04;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Buffer_ID + StartPage + PageNum;
	uint8_t DataPacket[] = {Command_Code, Buffer_ID, StartPage >> 8, StartPage, PageNum >> 8, PageNum, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_RegModel(as608_handle_t *handle)
{
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x05;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_StoreChar(as608_handle_t *handle, uint8_t Buffer_ID, uint16_t Page_ID)
{
	uint16_t P_Len = 0x06;
	uint8_t Command_Code = 0x06;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Buffer_ID + Page_ID;
	uint8_t DataPacket[] = {Command_Code, Buffer_ID, Page_ID >> 8, Page_ID, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_DeleteChar(as608_handle_t *handle, uint16_t Page_ID, uint16_t DeleteNum)
{
	uint16_t P_Len = 0x07;
	uint8_t Command_Code = 0x0c;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Page_ID + DeleteNum;
	uint8_t DataPacket[] = {Command_Code, Page_ID >> 8, Page_ID, DeleteNum >> 8,
							DeleteNum, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_Empty(as608_handle_t *handle)
{
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x0d;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_WriteReg(as608_handle_t *handle, uint8_t Reg_ID)
{
	uint16_t P_Len = 0x05;
	uint8_t Command_Code = 0x0e;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Reg_ID;
	uint8_t DataPacket[] = {Command_Code, Reg_ID, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_ReadSysPara(as608_handle_t *handle)
{
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x0f;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_HighSpeedSearch(as608_handle_t *handle, uint8_t Buffer_ID, uint16_t StartPage, uint16_t PageNum)
{
	uint16_t P_Len = 0x08;
	uint8_t Command_Code = 0x1b;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code + Buffer_ID + StartPage + PageNum;
	uint8_t DataPacket[] = {Command_Code, Buffer_ID, StartPage >> 8, StartPage, PageNum >> 8, PageNum,
							CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_ValidTempleteNum(as608_handle_t *handle, uint16_t *ValidN)
{
	uint16_t P_Len = 0x03;
	uint8_t Command_Code = 0x1d;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Command_Code;
	uint8_t DataPacket[] = {Command_Code, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	*ValidN = (handle->data_packet.param[0] << 8) + handle->data_packet.param[1];
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_WriteNotepad(as608_handle_t *handle, uint8_t Page, uint8_t Content[32])
{
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
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_ReadNotepad(as608_handle_t *handle, uint8_t Page, uint8_t *Content)
{
	uint16_t P_Len = 4;
	uint8_t Command_Code = 0x19;
	uint16_t CheckNum = AS608_COMMAND_PACKET + P_Len + Page + Command_Code;
	uint8_t DataPacket[] = {Command_Code, Page, CheckNum >> 8, CheckNum};
	AS608_SendPacket(handle, AS608_COMMAND_PACKET, P_Len, DataPacket);
	while (huart1IdleFlag != 1)
		;
	encoding_packet(handle);
	for (uint8_t i = 0; i < 32; i++)
	{
		Content[i] = handle->data_packet.param[i];
	}
	return handle->data_packet.confirmation_code;
}

as608_function_result_t AS608_InputFingerprint(as608_handle_t *handle, uint16_t Page_ID)
{
	as608_function_result_t err_code = AS608_SUCCESS;
	while (AS608_GetWAK(handle) == 0)
		;
	err_code = AS608_GetImage(handle);
	if(err_code != AS608_SUCCESS)
	{
		return err_code;
	}

	err_code = AS608_GenChar(handle, CharBuffer1);
	if(err_code != AS608_SUCCESS)
	{
		return err_code;
	}

	if (AS608_Search(handle, CharBuffer1, 0, 255) == AS608_SUCCESS)
	{
		return AS608_SUCCESS;
	}

	while (AS608_GetWAK(handle) == 0)
		;

	err_code = AS608_GetImage(handle);
	if(err_code != AS608_SUCCESS)
	{
		return err_code;
	}
	err_code = AS608_GenChar(handle, CharBuffer2);
	if(err_code != AS608_SUCCESS)
	{
		return err_code;
	}
	err_code = AS608_RegModel(handle);
	if(err_code != AS608_SUCCESS)
	{
		return err_code;
	}
	err_code = AS608_StoreChar(handle, CharBuffer2, Page_ID);
	if(err_code != AS608_SUCCESS)
	{
		return err_code;
	}

	return AS608_SUCCESS;
}

as608_function_result_t AS608_BrushFingerprint(as608_handle_t *handle)
{
	as608_function_result_t err_code = AS608_SUCCESS;
	while (AS608_GetWAK(handle) == 0)
		;
	
	err_code = AS608_GetImage(handle);
	if(err_code != AS608_SUCCESS)
	{
		return err_code;
	}

	err_code = AS608_GenChar(handle, CharBuffer1);
	if(err_code != AS608_SUCCESS)
	{
		return err_code;
	}

	err_code = AS608_Search(handle, CharBuffer1, 0, 255);

	return err_code;
}

int AS608_Init(as608_handle_t *handle, as608_init_t *init_structure)
{
	
	if(init_structure->chip_addr == 0)
	{
		handle->chip_addr = 0xffffffff;
	}
	else
	{
		handle->chip_addr = init_structure->chip_addr;
	}
	if(init_structure->bond == 0)
	{
		handle->bond = 57600;
	}
	else
	{
		handle->bond = init_structure->bond;
	}

	handle->huartx = init_structure->huartx;
	if(handle->huartx == NULL)
	{
		return -1;
	}
	handle->IO = init_structure->IO;

	HAL_Delay(100);

	return 0;

}
