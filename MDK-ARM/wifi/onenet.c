/**
 * file name "onenet.c"
 * file encoding "GB2312"
 */


#include "onenet.h"

#include <stdio.h>
#include <string.h>

void OneNet_Init(onenet_handle_t *p, const char *deviceID, const char *productID, const char *key)
{
  strcpy(p->deviceID, deviceID);
  strcpy(p->productID, productID);
  strcpy(p->key, key);
  sprintf(p->subscribeTopic[0], "$sys/%s/%s/thing/property/post/reply", productID, deviceID);
  sprintf(p->subscribeTopic[1], "$sys/%s/%s/thing/property/set", productID, deviceID);
  sprintf(p->postTopic, "$sys/%s/%s/thing/property/post", productID, deviceID);
  memset(p->message, '\0', sizeof(p->message));
  memset(p->data, 0, sizeof(p->data));
  p->iMember = 0;
}

int OneNet_Run(onenet_handle_t *p)
{
  static int status = 0;
  static uint32_t previousMillis = 0, time = 0;
  uint32_t currentMillis = 0;

  currentMillis = HAL_GetTick();
  if(currentMillis - previousMillis <= time || p->esp->status == 0)
  {
    return -1;
  }
  previousMillis = currentMillis;

  switch(status)
  {
    case 0:
      if(strstr((char *)p->esp->rxBufferAddr, "AT+MQTTUSERCFG=0,1") != NULL)
      {
        status ++;
      }
      else if(strstr((char *)p->esp->rxBufferAddr, "OK") != NULL)
      {
        status ++;
      }
      ESP_TransmitCommand(p->esp, 1000, "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n", p->deviceID, p->productID, p->key);
      time = 4000;
      break;
    case 1:
      if(strstr((char *)p->esp->rxBufferAddr, "OK") != NULL)
      {
        status ++;
      }
      ESP_TransmitCommand(p->esp, 1000, "AT+MQTTCONN=0,\"%s\",1883,1\r\n", ONENET_MQTT_URL);
      time = 5000;
      break;
    case 2:
      if(strstr((char *)p->esp->rxBufferAddr, "OK") != NULL)
      {
        status ++;
      }
      ESP_TransmitCommand(p->esp, 1000, "AT+MQTTSUB=0,\"%s\",1\r\n", p->subscribeTopic[0]);
      time = 3000;
      break;
    case 3:
      if(strstr((char *)p->esp->rxBufferAddr, "OK") != NULL)
      {
        status ++;
      }
      ESP_TransmitCommand(p->esp, 1000, "AT+MQTTSUB=0,\"%s\",2\r\n", p->subscribeTopic[1]);
      time = 3000;
      break;
    case 4:
      time = 0;
      if(p->esp->huartx->RxState == HAL_UART_STATE_READY)
      {
        HAL_UART_DMAStop(p->esp->huartx);
        memset(p->esp->rxBufferAddr, '\0', p->esp->rxBufferSize);
        HAL_UARTEx_ReceiveToIdle_DMA(p->esp->huartx, p->esp->rxBufferAddr, p->esp->rxBufferSize);
        __HAL_DMA_DISABLE_IT(p->esp->huartx->hdmarx, DMA_IT_HT);
      }
      p->externTask(p);
      break;
    default:
      break;
  }

  return status;
}

int OneNet_GetStatus(onenet_handle_t *p)
{
  return (int)p->status;
}

/**
 * @brief: 添加数据标签, 传入云平台设备句柄以及标签
 * @retval:
 *   0   ->    执行成功
 *  -1   ->    标签字符串长度太长
 *  -2   ->    标签满了
 * 
 */
int OneNet_AddData(onenet_handle_t *p, const char *tag, onenet_data_type_t dataType, void *pData)
{
  size_t strLength = 0;
  uint8_t iMember = p->iMember;

  strLength = strlen(tag);
  if(strLength > 32)
  {
    return -1;
  }
  else if(iMember > 32)
  {
    return -2;
  }
  strcpy(p->data[iMember].tag, tag);
  p->data[iMember].type = dataType;
  p->data[iMember].p = pData;
  iMember ++;
  p->iMember = iMember;
  return 0;
}

int OneNet_ChangeStringFormat(onenet_handle_t *p)
{
  int funcResult = 0;
  uint8_t allMembers = 0; /*总数*/
  char buffer[256] = {0}, destStr[256] = {0};

  allMembers = p->iMember;
  strcpy(destStr, "{\\\"id\\\":\\\"123\\\"\\,\\\"params\\\":{");
  for(uint8_t i = 0; i < allMembers; i ++)
  {
    switch(p->data[i].type)
    {
      case ONENET_DATATYPE_INT32:
        sprintf(buffer, "\\\"%s\\\":{\\\"value\\\":%d}", p->data[i].tag, *(int *)p->data[i].p);
        break;

      case ONENET_DATATYPE_INT64:
        sprintf(buffer, "\\\"%s\\\":{\\\"value\\\":%ld}", p->data[i].tag, *(long int *)p->data[i].p);
        break;

      case ONENET_DATATYPE_FLOAT:
        sprintf(buffer, "\\\"%s\\\":{\\\"value\\\":%f}", p->data[i].tag, *(float *)p->data[i].p);
        break;

      case ONENET_DATATYPE_DOUBLE:
        sprintf(buffer, "\\\"%s\\\":{\\\"value\\\":%f}", p->data[i].tag, *(double *)p->data[i].p);
        break;

      case ONENET_DATATYPE_ENUM:
        sprintf(buffer, "\\\"%s\\\":{\\\"value\\\":%d}", p->data[i].tag, *(int *)p->data[i].p);
        break;

      case ONENET_DATATYPE_BOOL:
        sprintf(buffer, "\\\"%s\\\":{\\\"value\\\":%s}", p->data[i].tag, (char *)p->data[i].p);
        break;

      case ONENET_DATATYPE_STRING:
        sprintf(buffer, "\\\"%s\\\":{\\\"value\\\":%s}", p->data[i].tag, (char *)p->data[i].p);
        break;

      case ONENET_DATATYPE_BITMAP:
        sprintf(buffer, "\\\"%s\\\":{\\\"value\\\":%d}", p->data[i].tag, *(int *)p->data[i].p);
        break;

      case ONENET_DATATYPE_STRUCT:
        break;
      
      case ONENET_DATATYPE_ARRY:
        break;

      case ONENET_DATATYPE_DATE:
        break;

      case ONENET_DATATYPE_BUFFER:
        break;
    }
    strcat(destStr, buffer);
    if(i != allMembers - 1)
    {
      strcat(destStr, "\\,");
    }
    memset((char *)buffer, '\0', sizeof(buffer));
  }

  strcat(destStr, "}}");
  memset((char *)p->message, '\0', sizeof(p->message));
  strcpy(p->message, destStr);
	
	return funcResult;
}

int OneNet_PostMQTTMessage(onenet_handle_t *p)
{
  int funcResult = 0;
  OneNet_ChangeStringFormat(p);
  funcResult = ESP_TransmitCommand(p->esp, 1000, "AT+MQTTPUB=0,\"%s\",\"%s\",0,0\r\n", p->postTopic, p->message);

  /*---测试函数, 不用可以注释掉---*/
  // char buffer[256] = {0};
  // HAL_UART_Transmit(&huart1, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
  /*---测试函数结束---*/

  return funcResult;
}
