/**
 * @brief rc522�����������
 */

#include "RC522.h"

#define MAXRLEN 18

/*rc522��������*/
#if defined USE_RC522PROTOCOL_SPI

#define RC522_SET_SDA(_H_, _BIT_) ((rc522_handle_t *)_H_)->IO.SDA.GPIOx->BSRR \
= ((rc522_handle_t *)_H_)->IO.SDA.pin << 16u * ((_BIT_) == 0)

#define RC522_SET_SCK(_H_, _BIT_) ((rc522_handle_t *)_H_)->IO.SCK.GPIOx->BSRR \
= ((rc522_handle_t *)_H_)->IO.SCK.pin << 16u * ((_BIT_) == 0)

#define RC522_SET_MOSI(_H_, _BIT_) ((rc522_handle_t *)_H_)->IO.MOSI.GPIOx->BSRR \
= ((rc522_handle_t *)_H_)->IO.MOSI.pin << 16u * ((_BIT_) == 0)

#define RC522_READ_MISO(_H_) ((((rc522_handle_t *)_H_)->IO.MISO.GPIOx->IDR \
& ((rc522_handle_t *)_H_)->IO.MISO.pin) != 0)

#define RC522_SET_RST(_H_, _BIT_) ((rc522_handle_t *)_H_)->IO.RST.GPIOx->BSRR \
= ((rc522_handle_t *)_H_)->IO.RST.pin << 16u * ((_BIT_) == 0)

static void SPI_Init(void *self)
{
    RC522_SET_SCK(self, 0);
    RC522_SET_SDA(self, 1);
}

static void SPI_Start(void *self)
{
    RC522_SET_SDA(self, 0);
}

static void SPI_Stop(void *self)
{
    RC522_SET_SDA(self, 1);
}

static void SPI_Swap(void *self, unsigned char *data)
{
    for(uint8_t i = 0; i < 8; i ++)
	{
		RC522_SET_MOSI(self, *data & 0x80);
		*data <<= 1;
		RC522_SET_SCK(self, 1);
		*data |= RC522_READ_MISO(self);
		RC522_SET_SCK(self, 0);
	}
}

static void SPI_SwapN(void *self, unsigned char *data, unsigned int n)
{
    for(uint32_t i = 0; i < 8 * n; i ++)
    {
        RC522_SET_MOSI(self, *(data  + i / 8) & 0x80);
        *(data  + i / 8) <<= 1;
        RC522_SET_SCK(self, 1);
        *(data  + i / 8) |= RC522_READ_MISO(self);
        RC522_SET_SCK(self, 0);
    }
}

#endif

/*---------------------------------rc522������뿪ʼ---------------------------------*/

/**
 * @brief RC522�Ĵ���ͨ��
 * @param handle:�ò�����Ҫ����rc522�ľ��
 */
uint8_t RC522_WriteRegister(rc522_handle_t *handle, uint8_t reg_addr, uint8_t command)
{
    uint8_t buffer[2] = {reg_addr, command};
    handle->reg.address = reg_addr;
    handle->reg.value = command;
    reg_addr = (reg_addr << 1) & 0x7e;
    SPI_Start(handle);
    SPI_Swap(handle, &reg_addr);
    SPI_Swap(handle, &command);
    SPI_Stop(handle);
    return handle->reg.value;
}

uint8_t RC522_ReadRegister(rc522_handle_t *handle, uint8_t reg_addr)
{
    handle->reg.address = reg_addr;
    reg_addr = (reg_addr << 1) & 0x7e;
    reg_addr |= 0x80;
    SPI_Start(handle);
    SPI_Swap(handle, &reg_addr);
    SPI_Swap(handle, &handle->reg.value);
    SPI_Stop(handle);
    return handle->reg.value;
}

/**
 * @brief  rc522�Ĵ��������ø߲�����
 * @param  handle: rc522�����
 * @param  reg_addr: ����Ҫ�����ļĴ�����ַ���Ĵ�����ַ��Ϣ��ѯRC522.hͷ�ļ��еļĴ�����ַ�궨�塣
 * @param  bitmask: �������룬�����øߡ�
 * @retval ������ļĴ������ݡ�
 */
uint8_t RC522_SetRegisterBitMask(rc522_handle_t *handle, uint8_t reg_addr, uint8_t bitmask)
{
    RC522_ReadRegister(handle, reg_addr);
    handle->reg.value |= bitmask;
    RC522_WriteRegister(handle, reg_addr, handle->reg.value);

    return handle->reg.value;
}

/**
 * @brief  ���rc522ָ�������롣
 * @param  handle: rc522�����
 * @param  reg_addr: ����Ҫ�����ļĴ�����ַ��
 * @param  bitmask: �������룬������ա�
 * @retval ������ļĴ������ݡ�
 */
uint8_t RC522_ClearRegisterBitMask(rc522_handle_t *handle, uint8_t reg_addr, uint8_t bitmask)
{
    RC522_ReadRegister(handle, reg_addr);
    handle->reg.value &= ~bitmask;
    RC522_WriteRegister(handle, reg_addr, handle->reg.value);

    return handle->reg.value;
}

/**
 * @brief  rc522дfifo��������
 * @param  handle: �����rc522�����
 * @param  data: ��������ݵ�ַ��
 * @param  data_length: ��������ݳ��ȡ�
 * @retval FIFO�������д��еĽ�����
 */
int RC522_WriteFIFOBuffer(rc522_handle_t *handle, uint8_t *data, uint8_t data_length)
{
    SPI_Start(handle);
    handle->reg.address = (FIFODataReg << 1) & 0x7e;
    SPI_Swap(handle, &handle->reg.address);
    for (uint8_t i = 0; i < data_length; i++)
    {
        SPI_Swap(handle, &data[i]); /* ��ʼ��FIFO������д������ */
    }
    SPI_Stop(handle);

    return RC522_ReadRegister(handle, FIFOLevelReg); /* ��ȡFIFO�������д��ж����ֽ��� */
}

/**
 * @brief  rc522��ȡFIFO������������
 * @param  handle: ����rc522�����
 * @param  store_arry: ��ȡ�����ݴ洢�ڴ����顣
 * @param  length: ��ȡ�ĳ��ȡ�
 * @retval FIFO�������д��еĽ�����
 */
int RC522_ReadFIFOBuffer(rc522_handle_t *handle, uint8_t *store_arry, uint8_t length)
{
    SPI_Start(handle);
    handle->reg.address = ((FIFODataReg << 1) & 0x7e) | 0x80;
    SPI_Swap(handle, &handle->reg.address);
    for (uint8_t i = 0; i < length; i++)
    {
        store_arry[i] = ((FIFODataReg << 1) & 0x7e) | 0x80;
        SPI_Swap(handle, &store_arry[i]);
    }
    SPI_Stop(handle);

    return RC522_ReadRegister(handle, FIFOLevelReg); /* ��ȡFIFO�������д��ж����ֽ��� */
}

/**
 * @brief  ���rc522FIFO���������ݡ�
 * @param  handle: ����rc522�����
 * @retval None.
 */
void RC522_ClearFIFOBuffer(rc522_handle_t *handle)
{
    RC522_ClearRegisterBitMask(handle, FIFOLevelReg, 0x80);
}

/**
 * @brief  ����rc522��ʱ��
 * @param  handle: ����rc522�����
 * @param  TAuto: ����Ϊ�߼�1��������ͨ��ģʽ�£��������ٶ��£���ʱ���ڴ������ʱ�Զ�������
 *                ����Ĵ���RxModeReg�е�λRxMultipleδ���ã���ʱ���ڽ��յ���һ������λ������ֹͣ��
 *                ���RxMultiple����Ϊ�߼�1����ʱ����ֹͣ��
 *                ����������£���ʱ������ͨ�����Ĵ���ControlReg�е�λTStopNow����Ϊ�߼�1��ֹͣ��
 *                �������Ϊ�߼�0�����ʾ�ö�ʱ������Э��Ӱ�졣
 * @param  TGated: ȡֵ��Χ[0,3]��
 *                 0�����ڲ���ʱ�������ſ�ģʽ���С�
 *                 1����ͨ��MFIN������ʱ����
 *                 2����ͨ��AUX1������ʱ����
 *                 3����ͨ��A3������ʱ����
 * @param  TAutoRestart: ����Ϊ�߼�1����ʱ���Զ��� TReloadValue���¿�ʼ����ʱ�������ǵ������㡣
 *                       �������Ϊ�߼�0����ʱ����Ϊ0��TimerIRqλ����Ϊ�߼�1��
 * @retval TGated������Χ�򷵻�-1��
 *         �ɹ����ؼĴ�����4λ��ֵ��
 */
uint8_t RC522_SetTMode(rc522_handle_t *handle, uint8_t TAuto, uint8_t TGated, uint8_t TAutoRestart)
{
    uint8_t bitmask = 0;
    bitmask = ((TAuto != 0) << 7) + ((TGated * (TGated < 4)) << 5) + ((TAutoRestart != 0) << 4);

    return RC522_WriteRegister(handle, TModeReg, bitmask);
}

/**
 * @brief  ����rc522��ʱ��Ԥ��Ƶֵ
 * @param  handle: ����rc522�����
 * @param  psc: ȡֵ��Χ[0,4095].
 * @retval ftimer ��ʱ��Ƶ�ʣ���λhz��
 */
int RC522_SetTPrescaler(rc522_handle_t *handle, uint16_t psc)
{
    psc &= 0x0fff;
    RC522_SetRegisterBitMask(handle, TModeReg, psc >> 8);
    RC522_WriteRegister(handle, TPrescalerReg, psc);

    return 6780000u / psc;
}

/**
 * @brief  ����rc522��ʱ���ļ�����װֵ��
 * @param  handle������rc522�����
 * @param  reload�����õļ�����װֵ��
 * @retval int���ͣ�����0�������塣
 */
int RC522_SetTReload(rc522_handle_t *handle, uint16_t reload)
{
    RC522_WriteRegister(handle, TReloadRegH, reload >> 8);
    RC522_WriteRegister(handle, TReloadRegL, reload);

    return 0;
}

uint16_t RC522_ReadTCounter(rc522_handle_t *handle)
{
    uint16_t counter_value = 0;
    counter_value = RC522_ReadRegister(handle, TCounterValueRegH) << 8;
    counter_value |= RC522_ReadRegister(handle, TCounterValueRegL);

    return counter_value;
}

void RC522_SwitchTimer(rc522_handle_t *handle, uint8_t status)
{
    status = 1 << (6 + (status == 0));
    RC522_SetRegisterBitMask(handle, ControlReg, status);
}

void RC522_SetTimerIRq(rc522_handle_t *handle, uint8_t status)
{
    status = status != 0;
    RC522_SetRegisterBitMask(handle, CommIEnReg, status);
}

/*-----rc522�ж�ʹ�ܴ��벿�ֿ�ʼ-----*/

uint8_t RC522_SetCommIEnReg(rc522_handle_t *handle, commienreg_bitmask_t bitmask)
{
    return RC522_SetRegisterBitMask(handle, CommIEnReg, bitmask);
}

uint8_t RC522_ClearCommIEnReg(rc522_handle_t *handle, commienreg_bitmask_t bitmask)
{
    return RC522_ClearRegisterBitMask(handle, CommIEnReg, bitmask);
}

uint8_t RC522_SetDivIEnReg(rc522_handle_t *handle, divienreg_bitmask_t bitmask)
{
    return RC522_SetRegisterBitMask(handle, DivlEnReg, bitmask);
}

uint8_t RC522_ClearDivIEnReg(rc522_handle_t *handle, divienreg_bitmask_t bitmask)
{
    return RC522_ClearRegisterBitMask(handle, DivlEnReg, bitmask);
}

/*-----rc522�ж�ʹ�ܴ��벿�ֽ���-----*/

/*-----rc522�ǽӴ�ʽuart�ӿں������忪ʼ-----*/

/**
 * @brief  ����rc522���������ݴ������ʡ�
 * @param  handle������rc522�����
 * @param  TxCRCEn������Ϊ�߼�1��ʹ�����ݴ���Ĺ���������CRC��
 * @param  TxSpeed���������ݴ����λ�������ʣ����ߴ�848kbit/s.
 *                  ȡֵ��Χ[0,3]����0��ʼ�����ֱ����λ����106kbit/s��212kbit/s��424kbit/s��848kbit/s��
 * @param  InvMod������Ϊ�߼�1�����ƵĴ������ݽ����á�
 * @retval uint8_t���ͣ���������֮��TxModReg��ֵ��
 */
uint8_t RC522_SetTxModReg(rc522_handle_t *handle, uint8_t TxCRCEn, uint8_t TxSpeed, uint8_t InvMod)
{
    uint8_t bitmask = 0;
    bitmask = ((TxCRCEn != 0) << 7) + (TxSpeed * (TxSpeed < 4) << 4) + ((InvMod != 0) << 3);
    return RC522_WriteRegister(handle, TxModeReg, bitmask);
}

/**
 * @brief  rc522���ط�����
 * @param  handle��rc522�����
 * @param  status��0����أ�1������
 * @retval uint8_t���ͣ�����TxControlReg�Ĵ����е�ֵ��
 */
uint8_t RC522_SwitchTransmitter(rc522_handle_t *handle, uint8_t status)
{
    if (status)
        return RC522_SetRegisterBitMask(handle, TxControlReg, 0x03); /*bit0->Tx1RFEn, bit1->Tx2RFEn*/
    else
        return RC522_ClearRegisterBitMask(handle, TxControlReg, 0x03);
}

uint8_t RC522_SetTxASKReg(rc522_handle_t *handle, uint8_t status)
{
    return RC522_WriteRegister(handle, TxASKReg, (status != 0) << 6);
}

/*-----rc522�ǽӴ�ʽuart�ӿں����������-----*/

/**
 * @brief  rc522��λ��
 * @param  handle������rc522�����
 * @retval ��
 */
void RC522_Reset(rc522_handle_t *handle)
{
    RC522_SET_RST(handle, 0);
    HAL_Delay(1);
    RC522_SET_RST(handle, 1);
    /*Ӳ����λ*/
    RC522_SetRegisterBitMask(handle, CommandReg, 0x0f);
    /*�����λ*/

    while (RC522_ReadRegister(handle, CommandReg) != 0x20)
        ;
    /*�ȴ������λ���*/
}

/**
 * @brief  rc522��ʼ��
 * @param  handle��rc522�����
 * @retval int���ͣ�����0�������塣
 */
int RC522_Init(rc522_handle_t *handle)
{
    SPI_Init(handle);

    RC522_Reset(handle); /*rc522��λ*/

    RC522_ClearFIFOBuffer(handle); /*���FIFO����������*/

    RC522_SetTMode(handle, 1, 0, 0); /*���ö�ʱ�����յ���һ�����ݾ�ֹͣ, ���ſ�ģʽ, ���Զ���װ*/

    RC522_SetTPrescaler(handle, 3380);
    RC522_SetTReload(handle, 30);
    /*��ʱ15ms*/

    RC522_SetTxASKReg(handle, 1); /*�����ǽӴ�ʽuartASK100%����*/

    return 0;
}

int RC522_PcdCommand(rc522_handle_t *handle,
                     uint8_t rc522_command,
                     uint8_t *data_to_fifo,
                     size_t data_len,
                     uint8_t *fifo_to_data,
                     size_t *fifo_bit_len)
{
    char func_status = MI_ERR;
    unsigned char irqEn = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n; /*��ʱ����*/
    unsigned int i;
    switch (rc522_command)
    {
    case PCD_AUTHENT:
        irqEn = 0x12;
        waitFor = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn = 0x77; /*ʹ�������ж�����,����FIFO����ˮƽ���ж�����*/
        waitFor = 0x30;
        break;
    default:
        break;
    }

    RC522_WriteRegister(handle, CommIEnReg, irqEn | 0x80); /*ʹ���ж�����,���ҽ�IRQ�����������*/
    RC522_ClearRegisterBitMask(handle, ComIrqReg, 0x80);   /*7bit->Set1, ��ʾ�������ж���������*/
    RC522_WriteRegister(handle, CommandReg, PCD_IDLE);     /*ֹͣ��������*/
    RC522_SetRegisterBitMask(handle, FIFOLevelReg, 0x80);  /*��FIFO����������*/

    for (i = 0; i < data_len; i++)
    {
        RC522_WriteRegister(handle, FIFODataReg, data_to_fifo[i]); /*������д��FIFO������*/
    }
    RC522_WriteRegister(handle, CommandReg, rc522_command);

    if (rc522_command == PCD_TRANSCEIVE)
    {
        RC522_SetRegisterBitMask(handle, BitFramingReg, 0x80); /*��ʼ���ݴ���*/
    }

    i = 600; // ����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    do
    {
        n = RC522_ReadRegister(handle, ComIrqReg);
        i--;
    } while ((i != 0) /*iΪ0*/ && !(n & 0x01) /*��ʱ�ж�*/ && !(n & waitFor) /*���ݴ������*/);
    RC522_ClearRegisterBitMask(handle, BitFramingReg, 0x80); /*ֹͣ���ݴ���*/

    if (i != 0)
    {
        if (!(RC522_ReadRegister(handle, ErrorReg) & 0x1B)) /*���FIFO�������Ƿ����,�Ƿ�λ��ͻ,��żУ���Ƿ����*/
        {
            func_status = MI_OK;
            if (n & irqEn & 0x01)
            {
                func_status = MI_NOTAGERR;
            }
            if (rc522_command == PCD_TRANSCEIVE)
            {
                n = RC522_ReadRegister(handle, FIFOLevelReg);
                lastBits = RC522_ReadRegister(handle, ControlReg) & 0x07;
                /*bit2~bit0λΪRxLastBits��ָʾ���һ���ֽڽ��յ���λ��������ñ�־Ϊ0�����ʾ���һ���ֽ�8λȫΪ��Чλ��*/
                if (lastBits)
                {
                    *fifo_bit_len = (n - 1) * 8 + lastBits;
                }
                else
                {
                    *fifo_bit_len = n * 8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for (i = 0; i < n; i++)
                {
                    fifo_to_data[i] = RC522_ReadRegister(handle, FIFODataReg);
                }
            }
        }
        else
        {
            func_status = MI_ERR;
        }
    }

    RC522_SetRegisterBitMask(handle, ControlReg, 0x80); // stop timer now
    RC522_WriteRegister(handle, CommandReg, PCD_IDLE);
    return func_status;
}

int RC522_PcdRequest(rc522_handle_t *handle, uint8_t req_code, uint8_t *tag_type)
{
    char status;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    RC522_ClearRegisterBitMask(handle, Status2Reg, 0x08); /*���MFCypto1Onλ����λֻ��ͨ��������*/
    RC522_WriteRegister(handle, BitFramingReg, 0x07);     /*���崫������һ���ֽڷ���7λ*/
    RC522_SetRegisterBitMask(handle, TxControlReg, 0x03); /*�򿪷ǽӴ�ʽuart������*/

    ucComMF522Buf[0] = req_code;

    status = RC522_PcdCommand(handle, PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &unLen);

    if ((status == MI_OK) && (unLen == 0x10))
    {
        *tag_type = ucComMF522Buf[0];
        *(tag_type + 1) = ucComMF522Buf[1];
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

int RC522_PcdAnticollsion(rc522_handle_t *handle, uint8_t *uid)
{
    char status;
    unsigned char i, snr_check = 0;
    unsigned int unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    RC522_ClearRegisterBitMask(handle, Status2Reg, 0x08);
    RC522_WriteRegister(handle, BitFramingReg, 0x00);
    RC522_ClearRegisterBitMask(handle, CollReg, 0x80);

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = RC522_PcdCommand(handle, PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &unLen);

    if (status == MI_OK)
    {
        for (i = 0; i < 4; i++)
        {
            *(uid + i) = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
        }
        if (snr_check != ucComMF522Buf[i])
        {
            status = MI_ERR;
        }
    }

    RC522_SetRegisterBitMask(handle, CollReg, 0x80);
    return status;
}
