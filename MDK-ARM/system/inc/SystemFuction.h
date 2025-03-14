/**
 * file name "SystemFunction.h"
 * file encoding "GB2312"
 */
#ifndef __SYSTEMFUNCTION_H
#define __SYSTEMFUNCTION_H

#define USE_ERROR_LOCK 1	//ʹ�ô����������

#if(USE_ERROR_LOCK == 1)

#define MAX_ERROR_TIME 5	//�������������

#endif

#include "AS608.h"
#include "lcd.h"
#include "display.h"
#include "password.h"
#include "Key.h"
#include "RC522.h"
#include "OV2640.h"
#include "onenet.h"
#include "motor.h"

#define MAX_SYSTEM_FUNCTIONAL_MEMBERS 32
#define SYSTEM_UNLOCK_MILLISECONDS 8000
#define SYSTEM_ERRORLOCK_MILLISECONDS 180000

typedef struct system_handle_t system_handle_t;

typedef int (*func_t)(system_handle_t *);

struct system_handle_t
{
	func_t funcArry[MAX_SYSTEM_FUNCTIONAL_MEMBERS]; //����ָ��
  char funcName[MAX_SYSTEM_FUNCTIONAL_MEMBERS][32]; //������

	//ϵͳ�豸
	lcd_handle_t *lcd;
	as608_handle_t *as608;
	rc522_handle_t *rc522;
	esp_handle_t *esp;
	ov2640_handle_t *ov2640;
	key_handle_t *key;
	password_structure_t *password;
	onenet_handle_t *onenet;
#if(USE_ERROR_LOCK == 1)
	TIM_HandleTypeDef *htim;
#endif
	motor_handle_t *motor;
	
	uint8_t index; //��������
	uint8_t pageContent; //ÿҳ�ж��ٸ�����
	uint8_t content; //�ܹ��ж��ٸ�����
	uint8_t deviceStatus; //�豸״̬	0��ʾ����, 1��ʾ�쳣
	uint8_t lockStatus; // ����״̬
	uint32_t lockStartMillis;	//������ʼʱ��
	uint32_t lockEndMillis; //��������ʱ��

#if(USE_ERROR_LOCK == 1) 
	int errorTimes; //����������
	uint32_t errorLockStartMillis; //����ʱ��
	uint32_t errorUnlockMillis; //����ʱ��
	uint8_t errorLock;	//��������
#endif
};


void System_ShowCursor(system_handle_t *system);//��ʾ���
void System_ShowFunction(system_handle_t *system);//��ʾ����
void System_DoFunction(system_handle_t *system);//ִ�й���

int System_UnlockByPassword(system_handle_t *system);//ͨ���������
int System_ChangePassword(system_handle_t *system);//��������

int System_UnlockByFingerprint(system_handle_t *system);//ͨ��ָ�ƽ���
int System_AddFingerprint(system_handle_t *system);//���ָ��
int System_DeleteFingerprint(system_handle_t *system);//ɾ��ָ��
int System_SearchFingerprintNum(system_handle_t *system);//�鿴ָ������

int System_WatchMonitor(system_handle_t *system);//�鿴���
int System_UnlockByIDCard(system_handle_t *system);
int System_AddIDCard(system_handle_t *system);
int System_Init(system_handle_t *sys, uint8_t pageContent);//ϵͳ��ʼ��
int System_DeInit(system_handle_t *system);//ϵͳ����ʼ��

int System_ShowRestErrorMillis(system_handle_t *system);
int System_UpdateLockStatus(system_handle_t *system);
int System_UpdataMotor(system_handle_t *system);

#define SYSTEM_CANCELED -1
#define SYSTEM_PASSWORD_SET_SUCCESS 1

#define SYSTEM_INIT_OK 0
#define SYSTEM_INIT_ERROR_LCD -1
#define SYSTEM_INIT_ERROR_AS608 -2
#define SYSTEM_INIT_ERRPR_OV2640 -3
#define SYSTEM_INIT_ERROR_RC522 -4

#define SYSTEM_DEVICE_ERROR_AS608 1
#define SYSTEM_DEVICE_ERROR_OV2640 2
#define SYSTEM_DEVICE_ERROR_RC522 4
#define SYSTEM_DEVICE_ERROR_ESP01 8

#endif
