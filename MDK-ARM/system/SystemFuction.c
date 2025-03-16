#include <string.h>
#include "SystemFuction.h"
#include "usart.h"

/*��ƽ̨���ݶ���*/
static char lockConditionMessageBuffer[6] = {0};
static char deadLockMessgeBuffer[6] = {0};

#define SYSTEM_POST_MESSAGE(_H_, X) 																											\
	do{																 																											\
		memset(lockConditionMessageBuffer, '\0', sizeof(lockConditionMessageBuffer)); 				\
		if(X == 0) strcpy(lockConditionMessageBuffer, "true");																\
		else strcpy(lockConditionMessageBuffer, "false");																			\
		memset(deadLockMessgeBuffer, '\0', sizeof(deadLockMessgeBuffer));											\
		if((_H_)->errorLock) strcpy(deadLockMessgeBuffer, "true");														\
		else strcpy(deadLockMessgeBuffer, "false");																						\
		OneNet_PostMQTTMessage((_H_)->onenet);																								\
	}while(0)

/**
 * ��¼�������
 */
#define SYSTEM_RECORD_ERROR_TIME(_H_, X)																								\
	do{																																										\
		if(X != 0)																																					\
		{																																										\
			(_H_)->errorLockStartMillis = HAL_GetTick();																			\
			(_H_)->errorUnlockMillis = HAL_GetTick() + SYSTEM_ERRORLOCK_MILLISECONDS;					\
			(_H_)->errorTimes ++;																															\
			if((_H_)->errorTimes >= MAX_ERROR_TIME)																						\
			{																																									\
				(_H_)->errorLock = 1;																														\
			}																																									\
		}																																										\
	} while(0)

/**
 * ��������
 */
#define SYSTEM_MOTOR_UNLOCK(_H_, X) 																							\
	do																																							\
	{																																								\
		if((X) == 0)																																	\
		{																																							\
			(_H_)->lockStartMillis = HAL_GetTick();																			\
			(_H_)->lockEndMillis = (_H_)->lockStartMillis + SYSTEM_UNLOCK_MILLISECONDS;	\
			(_H_)->lockStatus = 1;																											\
		}																																							\
		else																																					\
		{																																							\
			if((_H_)->lockStatus == 1)																									\
			{																																						\
				(_H_)->lockStatus = 0;																										\
			}																																						\
		}																																							\
	} while(0)

void System_AddFunction(system_handle_t *system, const char *str, func_t functioal)
{
	if(system->content >= MAX_SYSTEM_FUNCTIONAL_MEMBERS)
	{
		return;
	}
	strcpy(system->funcName[system->content], str);
	system->funcArry[system->content] = functioal;
	system->content ++;
}

void System_ShowCursor(system_handle_t *system)
{
	uint8_t index = system->index, pageContent = system->pageContent;
	uint16_t temp16 = 0;

	temp16 = system->lcd->backColor;
	system->lcd->backColor = system->lcd->pen;
	system->lcd->pen = temp16;

	Display_Show(system->lcd, 16*0, 16*(index % pageContent + 1), system->funcName[index]);

	temp16 = system->lcd->backColor;
	system->lcd->backColor = system->lcd->pen;
	system->lcd->pen = temp16;
}

void System_ShowFunction(system_handle_t *system)
{
	uint8_t i;
	uint8_t pageContent = system->pageContent, index = system->index, content = system->content;
	uint8_t temp = 0, restAmount = 0;
	uint16_t temp16[2] = {0};
	static uint8_t previousPage = 0, currentPage = 0;

	previousPage = currentPage;
	currentPage = index / pageContent;
	if(currentPage != previousPage)
	{
		Display_Clear(system->lcd);
	}
	restAmount = content - currentPage * pageContent;
	temp = restAmount > pageContent ? pageContent : restAmount;

	temp16[0] = system->lcd->backColor;
	temp16[1] = system->lcd->pen;
	system->lcd->backColor = GREEN;
	system->lcd->pen = RED;
	Display_Show(system->lcd, 16*0, 16*0, "  ��������ϵͳ  ");
	system->lcd->backColor = temp16[0];
	system->lcd->pen = temp16[1];
	
	for(i = 0; i < temp; i ++)
	{
		if(system->funcName[currentPage * pageContent + i] != NULL)
		{
			Display_Show(system->lcd, 16*0, 16*(i + 1), system->funcName[currentPage * pageContent + i]);
		}
	}
	System_ShowCursor(system);
}

void System_DoFunction(system_handle_t *system)
{
	uint8_t index = system->index;
	system->funcArry[index](system);
}

int System_UnlockByPassword(system_handle_t *system)
{
	int retval = SYSTEM_CANCELED;
	uint8_t keynum = 0, fingerFlag = 0;
	password_structure_t password = {0};

__goto_input_password_again:
	//�����������������ʾ������
	Display_Clear(system->lcd); //����
	Display_Show(system->lcd, 16*0, 16*0, "����������:");
	Display_Show(system->lcd, 16 * 4, 16 * 7, "��13ȡ��");
	Display_Show(system->lcd, 16 * 4, 16 * 8, "��15ɾ��");
	Display_Show(system->lcd, 16 * 4, 16 * 9, "��16ȷ��");

	//��ʼ��������
	do{
		keynum = Key_GetNum(system->key); //��ȡ��ֵ
		switch(keynum)
		{
			case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:case 10:
				Password_Input(&password, keynum);
				Display_Show(system->lcd, 8*(password.len - 1), 16*1, "%01u_", password.num[password.len - 1]);
				break;
			case 15:
				Password_Back(&password);
				Display_Show(system->lcd, 8*password.len, 16*1, "_ ");
				break;
		}
	}while(keynum != 13 && keynum != 16); //ȡ����ť��ȷ�ϰ�ť

	Display_Clear(system->lcd);
	//ȷ�ϰ����ǰ���ȡ����ť����ȷ�ϰ�ť
	if(keynum == 13)
	{
		retval = SYSTEM_CANCELED;
		return retval;
	}
	else if(keynum == 16)
	{
		retval = Password_Compare(system->password, &password);
		Password_Clear(&password);
		
		//�ж������Ƿ���ȷ,����������Ӧ�Ķ���
		if(retval == PASSWORD_MATCH)
		{
			Display_Show(system->lcd, 16*2, 16*4, "������ȷ\r\n  �����������");
			do{
				keynum = Key_GetNum(system->key);
				fingerFlag = AS608_GetWAK(system->as608);
			}while(keynum == 0 && fingerFlag == 0);
			Display_Clear(system->lcd);
		}
		else
		{
			Display_Show(system->lcd, 16*2, 16*4, "�������\r\n  ��16��������\r\n  ������������");
			do{
				keynum = Key_GetNum(system->key);
				fingerFlag = AS608_GetWAK(system->as608);
			}while(keynum == 0 && fingerFlag == 0);
			if(keynum == 16)
			{
				goto __goto_input_password_again;
			}
		}
	}

#if(USE_ERROR_LOCK == 1)
	SYSTEM_RECORD_ERROR_TIME(system, retval);
#endif

	SYSTEM_POST_MESSAGE(system, retval);
	SYSTEM_MOTOR_UNLOCK(system, retval);


	return retval;
}

static int System_CheckPassword(system_handle_t *system)
{
	int retval = SYSTEM_CANCELED;
	uint8_t keynum = 0, fingerFlag = 0;
	password_structure_t password = {0};

__goto_input_password_again:
	//�����������������ʾ������
	Display_Clear(system->lcd); //����
	Display_Show(system->lcd, 16*0, 16*0, "����������:");
	Display_Show(system->lcd, 16 * 4, 16 * 7, "��13ȡ��");
	Display_Show(system->lcd, 16 * 4, 16 * 8, "��15ɾ��");
	Display_Show(system->lcd, 16 * 4, 16 * 9, "��16ȷ��");

	//��ʼ��������
	do{
		keynum = Key_GetNum(system->key); //��ȡ��ֵ
		switch(keynum)
		{
			case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:case 10:
				Password_Input(&password, keynum);
				Display_Show(system->lcd, 8*(password.len - 1), 16*1, "%01u_", password.num[password.len - 1]);
				break;
			case 15:
				Password_Back(&password);
				Display_Show(system->lcd, 8*password.len, 16*1, "_ ");
				break;
		}
	}while(keynum != 13 && keynum != 16); //ȡ����ť��ȷ�ϰ�ť

	Display_Clear(system->lcd);
	//ȷ�ϰ����ǰ���ȡ����ť����ȷ�ϰ�ť
	if(keynum == 13)
	{
		retval = SYSTEM_CANCELED;
		return retval;
	}
	else if(keynum == 16)
	{
		retval = Password_Compare(system->password, &password);
		Password_Clear(&password);
		
		//�ж������Ƿ���ȷ,����������Ӧ�Ķ���
		if(retval != PASSWORD_MATCH)
		{
			Display_Show(system->lcd, 16*2, 16*4, "�������\r\n  ��16��������\r\n  ������������");
			do{
				keynum = Key_GetNum(system->key);
				fingerFlag = AS608_GetWAK(system->as608);
			}while(keynum == 0 && fingerFlag == 0);
			if(keynum == 16)
			{
				goto __goto_input_password_again;
			}
		}
	}

	return retval;
}

int System_ChangePassword(system_handle_t *system)
{
	int retval = SYSTEM_CANCELED;
	uint8_t keynum = 0, fingerFlag = 0;
	password_structure_t password = {0};
	uint8_t as608FlashContent[32] = {0};

__goto_input_password_again:
	//�����������������ʾ������
	Display_Clear(system->lcd); //����
	Display_Show(system->lcd, 16*0, 16*0, "������ԭ����:");
	Display_Show(system->lcd, 16 * 4, 16 * 7, "��13ȡ��");
	Display_Show(system->lcd, 16 * 4, 16 * 8, "��15ɾ��");
	Display_Show(system->lcd, 16 * 4, 16 * 9, "��16ȷ��");

	//��ʼ��������
	do{
		keynum = Key_GetNum(system->key); //��ȡ��ֵ
		switch(keynum)
		{
			case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:case 10:
				Password_Input(&password, keynum);
				Display_Show(system->lcd, 8*(password.len - 1), 16*1, "%01u_", password.num[password.len - 1]);
				break;
			case 15:
				Password_Back(&password);
				Display_Show(system->lcd, 8*password.len, 16*1, "_ ");
				break;
		}
	}while(keynum != 13 && keynum != 16); //ȡ����ť��ȷ�ϰ�ť

	Display_Clear(system->lcd);
	//ȷ�ϰ����ǰ���ȡ����ť����ȷ�ϰ�ť
	if(keynum == 13)
	{
		retval = SYSTEM_CANCELED;
	}
	else if(keynum == 16)
	{
		retval = Password_Compare(system->password, &password);
		Password_Clear(&password);
		
		//�ж������Ƿ���ȷ,����������Ӧ�Ķ���
		if(retval == PASSWORD_MATCH)
		{
			Display_Show(system->lcd, 16*2, 16*4, "������ȷ\r\n���������������");
			do{
				keynum = Key_GetNum(system->key);
				fingerFlag = AS608_GetWAK(system->as608);
			}while(keynum == 0 && fingerFlag == 0);

			//��ʼ��������
			Display_Clear(system->lcd);
			Display_Show(system->lcd, 16*0, 16*0, "����������:");
			do{
				keynum = Key_GetNum(system->key); //��ȡ��ֵ
				switch(keynum)
				{
					case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:case 10:
						Password_Input(&password, keynum);
						Display_Show(system->lcd, 8*(password.len - 1), 16*1, "%u_", password.num[password.len - 1]);
						break;
					case 15:
						Password_Back(&password);
						Display_Show(system->lcd, 8*password.len, 16*1, "_ ");
						break;
				}
			}while(keynum != 13 && keynum != 16); //ȡ����ť��ȷ�ϰ�ť
			Display_Clear(system->lcd);
			
			if(keynum == 13)
			{
				retval = SYSTEM_CANCELED;
			}
			else if(keynum == 16)
			{
				system->password->len = password.len; //����ϵͳ���볤��
				as608FlashContent[16] = password.len; //д��AS608Flash��,���籣��
				for(uint8_t i = 0; i < password.len; i ++)
				{
					system->password->num[i] = password.num[i];
					as608FlashContent[i] = password.num[i];
				}
				AS608_WriteNotepad(system->as608, 1, as608FlashContent);
				Display_Show(system->lcd, 16*2, 16*4, "���óɹ�\r\n  �����������");
				do{
					keynum = Key_GetNum(system->key);
					fingerFlag = AS608_GetWAK(system->as608);
				}while(keynum == 0 && fingerFlag == 0);
				Display_Clear(system->lcd);
				retval = SYSTEM_PASSWORD_SET_SUCCESS;
			}
		}
		else
		{
			Display_Show(system->lcd, 16*2, 16*4, "�������\r\n  ��16��������\r\n  ������������");
			do{
				keynum = Key_GetNum(system->key);
				fingerFlag = AS608_GetWAK(system->as608);
			}while(keynum == 0 && fingerFlag == 0);
			if(keynum == 16)
			{
				goto __goto_input_password_again;
			}
		}
	}
	return retval;
}

static void System_ReportFingerprintResult(system_handle_t *system, uint16_t x, uint16_t y, as608_retval_t resultCode)
{
	uint8_t keynum = 0, fingerFlag = 0;
	Display_Clear(system->lcd);
	switch(resultCode)
	{
		case AS608_SUCCESS: Display_Show(system->lcd, x, y, "ִ�гɹ�"); break;
		case AS608_ERROR_01H: Display_Show(system->lcd, x, y, "���ݰ����մ���"); break;
		case AS608_ERROR_02H: Display_Show(system->lcd, x, y, "��������û����ָ"); break;
		case AS608_ERROR_03H: Display_Show(system->lcd, x, y, "¼��ָ��ͼ��ʧ��"); break;
		case AS608_ERROR_04H: Display_Show(system->lcd, x, y, "ָ��ͼ��̫��,̫��"); break;
		case AS608_ERROR_05H: Display_Show(system->lcd, x, y, "ָ��ͼ��̫ʪ,̫��"); break;
		case AS608_ERROR_06H: Display_Show(system->lcd, x, y, "ָ��ͼ��̫��"); break;
		case AS608_ERROR_07H: Display_Show(system->lcd, x, y, "���������̫��"); break;
		case AS608_ERROR_08H: Display_Show(system->lcd, x, y, "ָ�Ʋ�ƥ��"); break;
		case AS608_ERROR_09H: Display_Show(system->lcd, x, y, "û������ָ��"); break;
		case AS608_ERROR_0AH: Display_Show(system->lcd, x, y, "�����ϳ�ʧ��"); break;
		case AS608_ERROR_0BH: Display_Show(system->lcd, x, y, "����ָ�ƿ��ַ��ų�����Χ"); break;
		case AS608_ERROR_0CH: Display_Show(system->lcd, x, y, "��ָ�ƿ��ģ�����"); break;
		case AS608_ERROR_0DH: Display_Show(system->lcd, x, y, "�ϴ�����ʧ��"); break;
		case AS608_ERROR_0EH: Display_Show(system->lcd, x, y, "ģ�鲻�ܽ��պ������ݰ�"); break;
		case AS608_ERROR_0FH: Display_Show(system->lcd, x, y, "�ϴ�ͼ��ʧ��"); break;
		case AS608_ERROR_10H: Display_Show(system->lcd, x, y, "ɾ��ģ��ʧ��"); break;
		case AS608_ERROR_11H: Display_Show(system->lcd, x, y, "���ָ�ƿ�ʧ��"); break;
		case AS608_ERROR_13H: Display_Show(system->lcd, x, y, "�����ȷ"); break;
		case AS608_ERROR_15H: Display_Show(system->lcd, x, y, "��������û��ԭʼͼ�����������"); break;
		case AS608_ERROR_18H: Display_Show(system->lcd, x, y, "��дFLASH����"); break;
		case AS608_ERROR_19H: Display_Show(system->lcd, x, y, "δ�������"); break;
		case AS608_ERROR_1AH: Display_Show(system->lcd, x, y, "��Ч�Ĵ������"); break;
		case AS608_ERROR_1BH: Display_Show(system->lcd, x, y, "�Ĵ����趨���ݴ����"); break;
		case AS608_ERROR_1CH: Display_Show(system->lcd, x, y, "���±�ҳ��ָ������"); break;
		case AS608_ERROR_1DH: Display_Show(system->lcd, x, y, "�˿ڲ���ʧ��"); break;
		case AS608_ERROR_1EH: Display_Show(system->lcd, x, y, "�Զ�ע��ʧ��"); break;
		case AS608_ERROR_1FH: Display_Show(system->lcd, x, y, "ָ�ƿ���"); break;
	}
}

int System_UnlockByFingerprint(system_handle_t *system)
{
	int retval = 0;
	uint8_t keynum = 0, fingerFlag = 0;
	
	Display_Clear(system->lcd);
	Display_Show(system->lcd, 16*0, 16*0, "�밴����ָ");
	retval = (int)AS608_BrushFingerprint(system->as608);
	Display_Clear(system->lcd);
	if(retval == (int)AS608_SUCCESS)
	{
		Display_Show(system->lcd, 16*2, 16*4, "�����ɹ�\r\n  �ſ���ָ����");
		do{
			fingerFlag = AS608_GetWAK(system->as608);
		}while(fingerFlag == 1);
	}
	else
	{
		System_ReportFingerprintResult(system, 16*0, 16*0, retval);
		Display_Print(system->lcd, "\r\n  �ſ���ָ����");
		do{
			fingerFlag = AS608_GetWAK(system->as608);
		}while(fingerFlag == 1);
	}

#if(USE_ERROR_LOCK == 1)
	SYSTEM_RECORD_ERROR_TIME(system, retval);
#endif

	SYSTEM_POST_MESSAGE(system, retval);
	SYSTEM_MOTOR_UNLOCK(system, retval);


	return retval;
}

int System_AddFingerprint(system_handle_t *system)
{
	int retval = 0;
	uint8_t index = 0, bitLocation = 0, as608FlashContent[32] = {0}, pageID = 0;
	uint8_t fingerFlag = 0, keynum = 0;

	if(System_CheckPassword(system) != PASSWORD_MATCH)
	{
		return PASSWORD_NO_MATCH;
	}
	Display_Clear(system->lcd);
	//�ȶ�ȡAS608���±�����,�涨0ҳ������¼¼���ָ������
	retval = (int)AS608_ReadNotepad(system->as608, 0, as608FlashContent);
	//�жϺ������صĽ���Ƿ���ȷ
	if(retval == (int)AS608_SUCCESS)
	{
		//�ж��ĸ�λ�û��п�ȱ, �������ָ�� 
		for(index = 0; index < 32; index ++)
		{
			if(as608FlashContent[index] != 0xFF)
			{
				for(bitLocation = 0; bitLocation < 8; bitLocation ++)
				{
					if((as608FlashContent[index] & (0x01 << bitLocation)) == 0)
					{
						break;
					}
				}
				break;
			}
		}
		//ѭ��δ�����ͱ����, ˵���ҵ���ȱλ��
		if(index != 32)
		{
			pageID = index * 8 + bitLocation;
			Display_Show(system->lcd, 16*0, 16*0, "   �밴����ָ");
			retval = (int)AS608_BrushFingerprint(system->as608);
			if(retval != 0)
			{
				retval = (int)AS608_InputFingerprint(system->as608, pageID);
			}
			if(retval == 0)
			{
				as608FlashContent[index] |= (0x01 << bitLocation);
				retval = (int)AS608_WriteNotepad(system->as608, 0, as608FlashContent);
				System_ReportFingerprintResult(system, 16*0, 16*0, retval);
			}
			Display_Print(system->lcd, "\r\n �ſ���ָ����");
			do{
				fingerFlag = AS608_GetWAK(system->as608);
			}while(fingerFlag == 1);
		}
		else
		{
			Display_Show(system->lcd, 16*0, 16*4, "   ָ�ƿ�����\r\n  �����������");
			do{
				keynum = Key_GetNum(system->key);
				fingerFlag = AS608_GetWAK(system->as608);
			}while(fingerFlag == 0 && keynum == 0);
		}
	}
	//��AS608���±�����ʱִ������
	else
	{
		System_ReportFingerprintResult(system, 16*0, 16*0, retval);
		Display_Print(system->lcd, "\r\n  �����������");
		do{
			keynum = Key_GetNum(system->key);
			fingerFlag = AS608_GetWAK(system->as608);
		}while(fingerFlag == 0 && keynum == 0);
	}
	Display_Clear(system->lcd);
	return retval;
}

int System_SearchFingerprintNum(system_handle_t *system)
{
	uint16_t fingerprintNum = 0;
	int retval = 0;
	uint8_t keynum = 0, fingerFlag = 0;

	Display_Clear(system->lcd);
	retval = (int)AS608_ValidTempleteNum(system->as608, &fingerprintNum);
	if(retval == (int)AS608_SUCCESS)
	{
		Display_Show(system->lcd, 16*0, 16*0, "��¼��ָ������:\r\n%03u", fingerprintNum);
	}
	else
	{
		System_ReportFingerprintResult(system, 16*0, 16*0, retval);
	}

	Display_Show(system->lcd, 16*1, 16*9, "�����������");
	do
	{
		keynum = Key_GetNum(system->key);
		fingerFlag = AS608_GetWAK(system->as608);
	} while (keynum == 0 && fingerFlag == 0);
	Display_Clear(system->lcd);

	return retval;
}

int System_DeleteFingerprint(system_handle_t *system)
{
	int retval = 0;
	uint16_t Page_ID = 0;
	uint8_t index = 0, bitLocation = 0;
	uint8_t as608FlashContent[32];
	uint8_t fingerFlag = 0;

	if(System_CheckPassword(system) != PASSWORD_MATCH)
	{
		return PASSWORD_NO_MATCH;
	}

	Display_Clear(system->lcd);
	Display_Show(system->lcd, 16*0, 16*0, "�밴��Ҫɾ������ָ");

	retval = (int)AS608_BrushFingerprint(system->as608); //��ˢָ��, �õ�ָ����Ϣ

	if (retval == (int)AS608_SUCCESS)
	{
		Page_ID = (system->as608->data_packet.param[0] << 8) + system->as608->data_packet.param[1];
		index = Page_ID / 8;
		bitLocation = Page_ID % 8;
		retval = (int)AS608_ReadNotepad(system->as608, 0, as608FlashContent);
		if(retval == (int)AS608_SUCCESS)
		{
			as608FlashContent[index] &= ~((uint8_t)0x01 << bitLocation);
			retval = (int)AS608_WriteNotepad(system->as608, 0, as608FlashContent);
			if(retval == 0)
			{
				retval = (int)AS608_DeleteChar(system->as608, Page_ID, 1);
			}
		}
	}
	
	System_ReportFingerprintResult(system, 16*0, 16*0, retval);
	Display_Print(system->lcd, "\r\n  �ſ���ָ����");
	do{
		fingerFlag = AS608_GetWAK(system->as608);
	}while(fingerFlag == 1);
	return retval;
}

int System_DeInit(system_handle_t *system)
{
	int retval = 0;
	uint8_t keynum = 0, fingerFlag = 0, as608FlashContent[32] = {0};
	Display_Clear(system->lcd);
	retval = System_CheckPassword(system);
	if(retval != PASSWORD_MATCH)
	{
		return retval;
	}
	Display_Show(system->lcd, 16*0, 16*0, "ȷ�����ϵͳ?");
	Display_Show(system->lcd, 16*0, 16*9, "13ȷ��    16ȡ��");
	do{
		keynum = Key_GetNum(system->key);
	}while(keynum != 13 && keynum != 16);
	if(keynum == 13)
	{
		//�������
		for(uint8_t i = 0; i < 16; i ++)
		{
			system->password->num[i] = 0;
			as608FlashContent[i] = 0;
		}
		system->password->len = 6;
		as608FlashContent[16] = 6;
		retval = (int)AS608_WriteNotepad(system->as608, 1, as608FlashContent);
		if(retval == (int)AS608_SUCCESS)
		{
			//���ָ��
			retval = (int)AS608_DeleteChar(system->as608, 0, 255);
			if(retval == (int)AS608_SUCCESS)
			{
				memset(as608FlashContent, 0, 32);
				retval = (int)AS608_WriteNotepad(system->as608, 0, as608FlashContent);
			}
			//���ID��
			memset(as608FlashContent, 0, 32);
			for(uint8_t page = 2; page < 16; page ++)
			{
				retval = (int)AS608_WriteNotepad(system->as608, page, as608FlashContent);
			}
		}
		System_ReportFingerprintResult(system, 16*0, 16*0, retval);
		Display_Print(system->lcd, "\r\n  �����������");
		do{
			keynum = Key_GetNum(system->key);
			fingerFlag = AS608_GetWAK(system->as608);
		}while(keynum == 0 && fingerFlag == 0);
	}
	else
	{
		retval = SYSTEM_CANCELED;
	}
	Display_Clear(system->lcd);
	return retval;
}

int System_WatchMonitor(system_handle_t *system)
{
	int retval = 0;
	uint8_t keynum = 0, fingerFlag = 0;
	uint16_t pixcnt = 0; // ����ͳ��
	uint32_t pix = 0;
	uint16_t linepix = 0;
	uint16_t linecnt = 0; // ����ͳ��
	memset(tx3Buffer, '\0', 256);
	LCD_WriteCommand(system->lcd, 0x36);
	LCD_WriteData8(system->lcd, 0xC0);//��ʾ����
	do{
		keynum = Key_GetNum(system->key);
		fingerFlag = AS608_GetWAK(system->as608);

		pix = 0;
		pixcnt = 0;  // ���ؼ���������
    linecnt = 0; // ��ͳ������
		while (OV2640_READ_VSYNC(system->ov2640) != 0); // �ȴ�֡�ź�
		LCD_AddressSet(	system->lcd, 
										system->lcd->width / 2 - 64, 
										system->lcd->width / 2 + 64 - 1, 
										system->lcd->height / 2 - 80, 
										system->lcd->height / 2 + 80 - 1);
		LCD_WriteCommand(system->lcd, 0x2c);
		LCD_WriteDataStart(system->lcd);
		while (linecnt < 160)
		{
			while (OV2640_READ_HREF(system->ov2640) != 0)
			{
				while (OV2640_READ_PCLK(system->ov2640) != 0);
				while (OV2640_READ_PCLK(system->ov2640) == 0);
				tx3Buffer[linepix ++] = OV2640_READ_DATA(system->ov2640);
				while (OV2640_READ_PCLK(system->ov2640) != 0);
				while (OV2640_READ_PCLK(system->ov2640) == 0);
				tx3Buffer[linepix ++] = OV2640_READ_DATA(system->ov2640);
				pix++;
			}
			if(pixcnt != pix)
			{
				for(uint16_t i = 0; i < 256; i += 2)
				{
					LCD_WriteData16Continue(system->lcd, ((uint16_t)tx3Buffer[i + 1] << 8) + tx3Buffer[i]);
				}
				pixcnt = pix;
				linepix = 0;
				linecnt ++ ;
			}
		}
		LCD_WriteDataStop(system->lcd);
	}while(keynum == 0 && fingerFlag == 0);
	LCD_WriteCommand(system->lcd, 0x36);
	LCD_WriteData8(system->lcd, 0x00);//��ʾ����
	Display_Clear(system->lcd);

	return 0;
}

int System_UnlockByIDCard(system_handle_t *system)
{
	int retval = 0;
	uint8_t keynum = 0, fingerFlag = 0;
	uint8_t tagType[2][2] = {0}, uid[2][4] = {0}, group[2][6] = {0};
	uint8_t as608PageContent[32] = {0};
	uint8_t page = 0, dest = 0;

	Display_Clear(system->lcd);
	Display_Show(system->lcd, 0, 0, "�����Ҫ�����Ŀ�Ƭ");
	Display_Show(system->lcd, 16*4, 16*9, "��13�˳�");

	do{
		retval = -1;
		keynum = Key_GetNum(system->key);
		if(RC522_PcdRequest(system->rc522, PICC_REQALL, tagType[0]) == MI_OK)
		{
			if(RC522_PcdAnticollsion(system->rc522, uid[0]) == MI_OK)
			{
				memmove(group[0], tagType[0], sizeof(tagType[0]));
				memmove(group[0] + 2, uid[0], sizeof(uid[0]));
			}
		}
		if(RC522_PcdRequest(system->rc522, PICC_REQALL, tagType[1]) == MI_OK)
		{
			if(RC522_PcdAnticollsion(system->rc522, uid[1]) == MI_OK)
			{
				memmove(group[1], tagType[1], sizeof(tagType[1]));
				memmove(group[1] + 2, uid[1], sizeof(uid[1]));
			}
		}
		if(group[0][0] != 0)
		{
			retval = memcmp(group[0], group[1], sizeof(group[0]));
		}
	} while(retval != 0 && keynum != 13);
	Display_Clear(system->lcd);
	if(keynum == 13)
	{
		return -2;
	}

	for(page = 2; page < 16; page ++)
	{
		AS608_ReadNotepad(system->as608, page, as608PageContent);
		for(dest = 0; dest < 30; dest += 6)
		{
			retval = memcmp(group[0], as608PageContent + dest, sizeof(group[0]));
			if(retval == 0)
			{
				break;
			}
		}
		if(dest < 30)
		{
			break;
		}
	}

	if(page < 16)
	{
		Display_Show(system->lcd, 16*0, 16*4, "�����ɹ�!\r\n  �����������");
	}
	else
	{
		Display_Show(system->lcd, 16*0, 16*4, "����ʧ��!\r\n  �����������");
	}
	do{
		keynum = Key_GetNum(system->key);
		fingerFlag = AS608_GetWAK(system->as608);
	} while(keynum == 0 && fingerFlag == 0);
	Display_Clear(system->lcd);

#if(USE_ERROR_LOCK == 1)
	SYSTEM_RECORD_ERROR_TIME(system, retval);
#endif

	SYSTEM_POST_MESSAGE(system, retval);
	SYSTEM_MOTOR_UNLOCK(system, retval);


	return retval;
}

int System_AddIDCard(system_handle_t *system)
{
	int retval = 0;
	uint8_t tagType[2][2] = {0}, uid[2][4] = {0}, group[2][6] = {0};
	uint8_t as608PageContent[32] = {0};
	uint8_t page = 0, dest = 0;
	uint8_t keynum = 0, fingerFlag = 0;
	if(System_CheckPassword(system) != PASSWORD_MATCH)
	{
		return PASSWORD_NO_MATCH;
	}
	for(page = 2; page < 16; page ++)
	{
		AS608_ReadNotepad(system->as608, page, as608PageContent);
		for(dest = 0; dest < 30; dest += 6)
		{
			if(as608PageContent[dest] == 0)
			{
				break;
			}
		}
		if(dest < 30)
		{
			break;
		}
	}
	if(page == 16)
	{
		return -1;
	}

	Display_Clear(system->lcd);
	Display_Show(system->lcd, 0, 0, "�����Ҫ¼��Ŀ�Ƭ");
	Display_Show(system->lcd, 16*4, 16*9, "��13�˳�");

	do{
		retval = -1;
		keynum = Key_GetNum(system->key);
		if(RC522_PcdRequest(system->rc522, PICC_REQALL, tagType[0]) == MI_OK)
		{
			if(RC522_PcdAnticollsion(system->rc522, uid[0]) == MI_OK)
			{
				memmove(group[0], tagType[0], sizeof(tagType[0]));
				memmove(group[0] + 2, uid[0], sizeof(uid[0]));
			}
		}
		if(RC522_PcdRequest(system->rc522, PICC_REQALL, tagType[1]) == MI_OK)
		{
			if(RC522_PcdAnticollsion(system->rc522, uid[1]) == MI_OK)
			{
				memmove(group[1], tagType[1], sizeof(tagType[1]));
				memmove(group[1] + 2, uid[1], sizeof(uid[1]));
			}
		}
		if(group[0][0] != 0)
		{
			retval = memcmp(group[0], group[1], sizeof(group[0]));
		}
	} while(retval != 0 && keynum != 13);
	Display_Clear(system->lcd);
	if(keynum == 13)
	{
		return -2;
	}
	
	memmove(as608PageContent + dest, group[0], sizeof(group[0]));
	AS608_WriteNotepad(system->as608, page, as608PageContent);
	if(retval == 0)
	{
		Display_Show(system->lcd, 16*0, 16*4, "¼��ɹ�!\r\n  �����������");
	}
	else
	{
		Display_Show(system->lcd, 16*0, 16*4, "¼��ʧ��!\r\n  �����������");
	}
	do{
		keynum = Key_GetNum(system->key);
		fingerFlag = AS608_GetWAK(system->as608);
	} while(keynum == 0 && fingerFlag == 0);
	Display_Clear(system->lcd);

	return retval;
}

int System_ShowRestErrorMillis(system_handle_t *system)
{
	int retval = 0;
	uint8_t keynum = 0, fingerFlag = 0;
	static uint32_t previousMillis = 0;
	uint32_t currentMillis = 0;
	int64_t restMillis = 0;
	keynum = Key_GetNum(system->key);
	fingerFlag = AS608_GetWAK(system->as608);
	while(AS608_GetWAK(system->as608) == 1);
	Display_Fill(
		system->lcd, 
		system->lcd->width / 2 - 39, 
		system->lcd->width / 2 + 41, 
		system->lcd->height / 2 - 59, 
		system->lcd->height / 2 + 61,
		BLACK
	);
	Display_DrawRectangle(
		system->lcd, 
		system->lcd->width / 2 - 39, 
		system->lcd->width / 2 + 41, 
		system->lcd->height / 2 - 59, 
		system->lcd->height / 2 + 61
	);
	Display_Show(
		system->lcd,
		system->lcd->width / 2 - 8*3, 
		system->lcd->height / 2 - 60, 
		"������"
	);
	Display_Show(
		system->lcd,
		system->lcd->width / 2 - 4*9, 
		system->lcd->height / 2 - 60 + 16, 
		"ʣ��ʱ��:"
	);
	previousMillis = HAL_GetTick();
	do
	{
		OneNet_Run(system->onenet);
		currentMillis = HAL_GetTick();
		if(system->errorUnlockMillis > system->errorLockStartMillis)
		{
			restMillis = system->errorUnlockMillis - currentMillis;
		}
		else
		{
			if(currentMillis > system->errorLockStartMillis)
			{
				restMillis = (uint32_t)(~0) - currentMillis + system->errorUnlockMillis;
			}
			else
			{
				restMillis = system->errorUnlockMillis - currentMillis;
			}
		}
		Display_Show(
			system->lcd,
			system->lcd->width / 2 - 4*4, 
			system->lcd->height / 2, 
			"%03lds",
			restMillis / 1000
		);
		if(restMillis <= 0)
		{
			system->errorLock = 0;
			system->errorTimes = 0;
			break;
		}
	} while(system->errorLock == 1);
	System_ShowFunction(system);
	return retval;
}

int System_UpdateLockStatus(system_handle_t *system)
{
	int retval = 0;
	int64_t restMillis = 0;
	char restSeconds = 0;

	if(system->lockStatus == 0)
	{
		Display_Fill(
			system->lcd, 
			system->lcd->width - system->lcd->fontWidth,
			system->lcd->width,
			system->lcd->height - system->lcd->fontHeight,
			system->lcd->height,
			system->lcd->backColor
		);
		return retval;
	}
	/*Խ���ж�*/
	if(system->lockStartMillis < system->lockEndMillis) //û��Խ��
	{
		restMillis = (int64_t)system->lockEndMillis - HAL_GetTick();
	}
	else
	{
		if(system->lockStartMillis <= HAL_GetTick())
		{
			restMillis = 0xffffffff - HAL_GetTick() + system->lockEndMillis;
		}
		else
		{
			restMillis = (int64_t)system->lockEndMillis - HAL_GetTick();
		}
	}
	if(restMillis <= 0)
	{
		system->lockStatus = 0;
		return retval;
	}
	restSeconds = restMillis / 1000 + '0';
	Display_ShowChar(
		system->lcd,
		system->lcd->width - system->lcd->fontWidth,
		system->lcd->height - system->lcd->fontHeight,
		restSeconds
	);
	return retval;
}

int System_UpdataMotor(system_handle_t *system)
{
	/**
	 * ���ת������ 
	 * ��ǰ״̬Ϊ0, ����״̬Ϊ1, �������.
	 * ��ǰ״̬Ϊ1, ����״̬Ϊ0, ��������.
	 * */
	int retval = 0;
	static uint32_t previousMillis = 0;
	static uint8_t previousLockStatus = 0, j = 0;
	static uint16_t i = 0;
	uint32_t currentMillis = 0;

	if((system->lockStatus != 1 && previousLockStatus != 0) && (previousLockStatus != 1 && system->lockStatus != 0))
	{
		retval = -1;
		previousLockStatus = system->lockStatus;
		return retval;
	}

	/*ִ�п�������*/
	if(system->lockStatus == 1 && previousLockStatus == 0)
	{
		currentMillis = HAL_GetTick();
		if(currentMillis - previousMillis >= 5)
		{
			previousMillis = currentMillis;
			Motor_Set(system->motor, j, MOTOR_DIRECTION_FORWARD);
			j ++;
		}
	}
	/*ִ����������*/
	else if(system->lockStatus == 0 && previousLockStatus == 1)
	{
		currentMillis = HAL_GetTick();
		if(currentMillis - previousMillis >= 5)
		{
			previousMillis = currentMillis;
			Motor_Set(system->motor, j, MOTOR_DIRECTION_REVERSE);
			j ++;
		}
	}


	if(j == 4)
	{
		i ++;
		j = 0;
	}
	if(i == 16 * 8)
	{
		i = 0;
		j = 0;
		previousLockStatus = system->lockStatus;
	}

	return retval;
}

int System_OneNetExternTask(void *p)
{
	onenet_handle_t *onenet = p;
	system_handle_t *system = onenet->parent;
	uint8_t i = 0;
	char *str = NULL, *valueBegin = NULL, *valueEnd = NULL;
	uint8_t require = 0;

	if(onenet->esp->rxBufferAddr[0] == 0)
	{
		return -1;
	}
	if(strstr((char *)onenet->esp->rxBufferAddr, "set") == NULL)
	{
		return -2;
	}

	for(i = 0; i < onenet->iMember; i ++)
	{
		str = strstr((char *)onenet->esp->rxBufferAddr, onenet->data[i].tag);
		if(str != NULL)
		{
			require = 1;
			valueBegin = strstr(str, ":");
			valueEnd = strstr(str, "}");
			switch(onenet->data[i].type)
			{
				case ONENET_DATATYPE_INT32: break;
				case ONENET_DATATYPE_BOOL:
					if(strstr(valueBegin, "true") != NULL)
					{
						system->errorLock = 1;

						strcpy(deadLockMessgeBuffer, "true");
					}
					else if(strstr(valueBegin, "false") != NULL)
					{
						system->errorLock = 0;
						system->errorTimes = 0;
						strcpy(deadLockMessgeBuffer, "false");
					}
					break;
				default: break;
			}
		}
	}
	if(0 == require)
	{
		return -3;
	}
	OneNet_PostMQTTMessage(onenet);
	return 0;

}

int System_Init(system_handle_t *system, uint8_t pageContent)
{
  int retval = 0;
	uint8_t Content[32] = {0}, deviceStatus = 0;

  //��װ�豸
  lcd_init_t lcdInitStructure = {
		.IO = {
			.SCL = {GPIOB, GPIO_PIN_7},
			.SDA = {GPIOB, GPIO_PIN_6},
			.RES = {GPIOB, GPIO_PIN_5},
			.DC = {GPIOB, GPIO_PIN_4},
			.CS = {GPIOB, GPIO_PIN_3},
		},
		.width = 128,
		.height = 160,
		.fontSize = 16 * 8,
		.fontWidth = 8,
		.fontHeight = 16,
		.pen = WHITE,
		.backColor = BLACK,
	};
  as608_init_t as608InitStructure = {
		.huartx = &huart1,
		.IO = {
			.WAK = {GPIOA, GPIO_PIN_11},
			.RX = {GPIOA, GPIO_PIN_9},
			.TX = {GPIOA, GPIO_PIN_10}
		},
		.chip_addr = 0xffffffff,
		.bond = 57600,
	};

  retval = LCD_Init(system->lcd, &lcdInitStructure);
  retval = AS608_Init(system->as608, &as608InitStructure);
	deviceStatus = retval != 0 ? deviceStatus | SYSTEM_DEVICE_ERROR_AS608 : deviceStatus;
  retval = OV2640_Init(system->ov2640);
	deviceStatus = retval != 0 ? deviceStatus | SYSTEM_DEVICE_ERROR_OV2640 : deviceStatus;
	if((deviceStatus & SYSTEM_DEVICE_ERROR_OV2640) == 0)
	{
		OV2640_RGB565_Mode(system->ov2640);
		OV2640_OutSize_Set(system->ov2640, 128, 160);
		SCCB_WriteData(system->ov2640->sccb_handle, 0xFF, 0X00);
		SCCB_WriteData(system->ov2640->sccb_handle, 0XD3, 28); // ����PCLK��Ƶ
		SCCB_WriteData(system->ov2640->sccb_handle, 0XFF, 0X01);
		SCCB_WriteData(system->ov2640->sccb_handle, 0X11, 1); // ����CLK��Ƶ
	}
	retval = RC522_Init(system->rc522);
	deviceStatus = retval != 0 ? deviceStatus | SYSTEM_DEVICE_ERROR_RC522 : deviceStatus;

	ESP_Init(system->esp, ESP_USER_WIFI_SSID, ESP_USER_WIFI_PASSWORD);
	OneNet_Init(system->onenet, ONENET_MQTT_USER_DEVICE_ID, ONENET_MQTT_USER_PRODUCT_ID, ONENET_MQTT_USER_KEY);

	OneNet_AddData(system->onenet, "DeadLock", ONENET_DATATYPE_BOOL, deadLockMessgeBuffer);
	OneNet_AddData(system->onenet, "ErrorTimes", ONENET_DATATYPE_INT32, &system->errorTimes);
	OneNet_AddData(system->onenet, "LockCondition", ONENET_DATATYPE_BOOL, lockConditionMessageBuffer);
	system->onenet->externTask = System_OneNetExternTask;
	system->onenet->parent = system;

	system->deviceStatus = deviceStatus;
	system->index = 0;
	system->pageContent = system->lcd->height / system->lcd->fontHeight - 1;
	system->errorLock = 0;
	system->errorTimes = 0;

	AS608_ReadNotepad(system->as608, 1, Content); //��ȡAS608���±�����
	if (Content[16] == 0)
	{
		system->password->len = 6;
		memset((uint8_t *)system->password->num, 0, 16);
	}
	else
	{
		system->password->len = Content[16];
		memcpy((uint8_t *)system->password->num, (uint8_t *)Content, system->password->len);
	}

	System_AddFunction(system, "    �������    ", System_UnlockByPassword);//��ӹ���
	System_AddFunction(system, "    ��������    ", System_ChangePassword);
	System_AddFunction(system, "    ָ�ƽ���    ", System_UnlockByFingerprint);
	System_AddFunction(system, "    ¼��ָ��    ", System_AddFingerprint);
	System_AddFunction(system, "    ɾ��ָ��    ", System_DeleteFingerprint);
	System_AddFunction(system, "    ָ������    ", System_SearchFingerprintNum);
	System_AddFunction(system, "    ID������    ", System_UnlockByIDCard);
	System_AddFunction(system, "    ¼��ID��    ", System_AddIDCard);
	System_AddFunction(system, "    �鿴���    ", System_WatchMonitor);
	System_AddFunction(system, "    ���ϵͳ    ", System_DeInit);

	System_ShowFunction(system);

	/*test function*/
	/*end*/
	
	return retval;
}
