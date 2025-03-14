#ifndef __ONENET_H
#define __ONENET_H

//file encoding "GB2312"

#include "ESP01.h"

/*��ƽ̨MQTTЭ���û����ú궨����*/
#define ONENET_MQTT_USER_DEVICE_ID "STM32" //"stm32"
#define ONENET_MQTT_USER_PRODUCT_ID "t30VuiGbtd"  //"J2942sVwNP"
#define ONENET_MQTT_USER_KEY "version=2018-10-31&res=products%2Ft30VuiGbtd%2Fdevices%2FSTM32&et=2715639643&method=md5&sign=i%2Fr3T2bUzgsFM8Vqj0phfw%3D%3D"
//"version=2018-10-31&res=products%2FJ2942sVwNP%2Fdevices%2Fstm32&et=2715639643&method=md5&sign=YngzbVZQwNQUoIyx4H0%2FiQ%3D%3D"

typedef int (*onenet_extern_task_t)(void *);

typedef enum onenet_data_type_t
{
  ONENET_DATATYPE_INT32, /*32λ����*/
  ONENET_DATATYPE_INT64, /*64λ����*/
  ONENET_DATATYPE_FLOAT, /*�����ȸ�����*/
  ONENET_DATATYPE_DOUBLE, /*˫���ȸ�����*/
  ONENET_DATATYPE_ENUM, /*ö����*/
  ONENET_DATATYPE_BOOL, /*��������*/
  ONENET_DATATYPE_STRING, /*�ַ�������*/
  ONENET_DATATYPE_BITMAP, /*λͼ*/
  ONENET_DATATYPE_STRUCT, /*�ṹ��*/
  ONENET_DATATYPE_ARRY, /*����*/
  ONENET_DATATYPE_DATE, /*ʱ��*/
  ONENET_DATATYPE_BUFFER, /*������*/
} onenet_data_type_t;

typedef struct onenet_data_structure_t
{
  void *p; /*����ָ��*/

  onenet_data_type_t type; /*��������*/

  char tag[32]; /*���ݱ�ʶ*/
} onenet_data_structure_t;

typedef struct onenet_handle_t 
{
  esp_handle_t *esp;
  void *parent;

  char deviceID[32];
  char productID[32];
  char key[256];
  char subscribeTopic[2][64];
  char postTopic[64];
  char message[256];

  onenet_data_structure_t data[32];
  uint8_t iMember;

  uint8_t status;

  onenet_extern_task_t externTask;
} onenet_handle_t;


void OneNet_Init(onenet_handle_t *p, const char *deviceID, const char *productID, const char *key);
int OneNet_Run(onenet_handle_t *p);
int OneNet_GetStatus(onenet_handle_t *p);
int OneNet_AddData(onenet_handle_t *p, const char *tag, onenet_data_type_t dataType, void *pData);
int OneNet_ChangeStringFormat(onenet_handle_t *p);
int OneNet_PostMQTTMessage(onenet_handle_t *p);

#define ONENET_MQTT_URL "mqtts.heclouds.com"
#define ONENET_MQTT_PORT 1883

#define ONENET_ERROR_MQTTUSERCFG 1
#define ONENET_ERROR_MQTTCONN 2
#define ONENET_ERROR_MQTTSUB 4
#define ONENET_ERROR_MQTTPUB 8

#endif
