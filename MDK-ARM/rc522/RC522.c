/**
 * @brief rc522基础命令代码
 */

#include "RC522.h"

#define MAXRLEN 18

/*rc522驱动代码*/
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

/*---------------------------------rc522命令代码开始---------------------------------*/

/**
 * @brief RC522寄存器通信
 * @param handle:该参数需要传入rc522的句柄
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
 * @brief  rc522寄存器掩码置高操作。
 * @param  handle: rc522句柄。
 * @param  reg_addr: 传入要操作的寄存器地址，寄存器地址信息查询RC522.h头文件中的寄存器地址宏定义。
 * @param  bitmask: 传入掩码，将其置高。
 * @retval 操作后的寄存器内容。
 */
uint8_t RC522_SetRegisterBitMask(rc522_handle_t *handle, uint8_t reg_addr, uint8_t bitmask)
{
    RC522_ReadRegister(handle, reg_addr);
    handle->reg.value |= bitmask;
    RC522_WriteRegister(handle, reg_addr, handle->reg.value);

    return handle->reg.value;
}

/**
 * @brief  清楚rc522指定的掩码。
 * @param  handle: rc522句柄。
 * @param  reg_addr: 传入要操作的寄存器地址。
 * @param  bitmask: 传入掩码，将其清空。
 * @retval 操作后的寄存器内容。
 */
uint8_t RC522_ClearRegisterBitMask(rc522_handle_t *handle, uint8_t reg_addr, uint8_t bitmask)
{
    RC522_ReadRegister(handle, reg_addr);
    handle->reg.value &= ~bitmask;
    RC522_WriteRegister(handle, reg_addr, handle->reg.value);

    return handle->reg.value;
}

/**
 * @brief  rc522写fifo缓冲区。
 * @param  handle: 传入的rc522句柄。
 * @param  data: 传入的数据地址。
 * @param  data_length: 传入的数据长度。
 * @retval FIFO缓冲区中存有的节数。
 */
int RC522_WriteFIFOBuffer(rc522_handle_t *handle, uint8_t *data, uint8_t data_length)
{
    SPI_Start(handle);
    handle->reg.address = (FIFODataReg << 1) & 0x7e;
    SPI_Swap(handle, &handle->reg.address);
    for (uint8_t i = 0; i < data_length; i++)
    {
        SPI_Swap(handle, &data[i]); /* 开始向FIFO缓冲区写入数据 */
    }
    SPI_Stop(handle);

    return RC522_ReadRegister(handle, FIFOLevelReg); /* 读取FIFO缓冲区中存有多少字节数 */
}

/**
 * @brief  rc522读取FIFO缓冲区函数。
 * @param  handle: 传入rc522句柄。
 * @param  store_arry: 读取的数据存储在此数组。
 * @param  length: 读取的长度。
 * @retval FIFO缓冲区中存有的节数。
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

    return RC522_ReadRegister(handle, FIFOLevelReg); /* 读取FIFO缓冲区中存有多少字节数 */
}

/**
 * @brief  清空rc522FIFO缓冲区数据。
 * @param  handle: 传入rc522句柄。
 * @retval None.
 */
void RC522_ClearFIFOBuffer(rc522_handle_t *handle)
{
    RC522_ClearRegisterBitMask(handle, FIFOLevelReg, 0x80);
}

/**
 * @brief  设置rc522定时器
 * @param  handle: 传入rc522句柄。
 * @param  TAuto: 设置为逻辑1，在所有通信模式下，在所有速度下，定时器在传输结束时自动启动。
 *                如果寄存器RxModeReg中的位RxMultiple未设置，则定时器在接收到第一个数据位后立即停止。
 *                如果RxMultiple设置为逻辑1，则定时器不停止。
 *                在这种情况下，定时器可以通过将寄存器ControlReg中的位TStopNow设置为逻辑1来停止。
 *                如果设置为逻辑0，则表示该定时器不受协议影响。
 * @param  TGated: 取值范围[0,3]。
 *                 0代表内部计时器不以门控模式运行。
 *                 1代表通过MFIN触发计时器。
 *                 2代表通过AUX1触发计时器。
 *                 3代表通过A3触发计时器。
 * @param  TAutoRestart: 设置为逻辑1，计时器自动从 TReloadValue重新开始倒计时，而不是倒数到零。
 *                       如果设置为逻辑0，则定时器减为0，TimerIRq位设置为逻辑1。
 * @retval TGated超出范围则返回-1。
 *         成功返回寄存器高4位的值。
 */
uint8_t RC522_SetTMode(rc522_handle_t *handle, uint8_t TAuto, uint8_t TGated, uint8_t TAutoRestart)
{
    uint8_t bitmask = 0;
    bitmask = ((TAuto != 0) << 7) + ((TGated * (TGated < 4)) << 5) + ((TAutoRestart != 0) << 4);

    return RC522_WriteRegister(handle, TModeReg, bitmask);
}

/**
 * @brief  设置rc522定时器预分频值
 * @param  handle: 传入rc522句柄。
 * @param  psc: 取值范围[0,4095].
 * @retval ftimer 定时器频率，单位hz。
 */
int RC522_SetTPrescaler(rc522_handle_t *handle, uint16_t psc)
{
    psc &= 0x0fff;
    RC522_SetRegisterBitMask(handle, TModeReg, psc >> 8);
    RC522_WriteRegister(handle, TPrescalerReg, psc);

    return 6780000u / psc;
}

/**
 * @brief  设置rc522定时器的计数重装值。
 * @param  handle：传入rc522句柄。
 * @param  reload：设置的计数重装值。
 * @retval int类型，返回0，无意义。
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

/*-----rc522中断使能代码部分开始-----*/

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

/*-----rc522中断使能代码部分结束-----*/

/*-----rc522非接触式uart接口函数定义开始-----*/

/**
 * @brief  设置rc522发送器数据传输速率。
 * @param  handle：传入rc522句柄。
 * @param  TxCRCEn：设置为逻辑1，使能数据传输的过程中生成CRC。
 * @param  TxSpeed：定义数据传输的位传输速率，最快高达848kbit/s.
 *                  取值范围[0,3]，从0开始递增分别代表位速率106kbit/s，212kbit/s，424kbit/s，848kbit/s。
 * @param  InvMod：设置为逻辑1，调制的传输数据将倒置。
 * @retval uint8_t类型，返回设置之后TxModReg的值。
 */
uint8_t RC522_SetTxModReg(rc522_handle_t *handle, uint8_t TxCRCEn, uint8_t TxSpeed, uint8_t InvMod)
{
    uint8_t bitmask = 0;
    bitmask = ((TxCRCEn != 0) << 7) + (TxSpeed * (TxSpeed < 4) << 4) + ((InvMod != 0) << 3);
    return RC522_WriteRegister(handle, TxModeReg, bitmask);
}

/**
 * @brief  rc522开关发送器
 * @param  handle：rc522句柄。
 * @param  status：0代表关，1代表开。
 * @retval uint8_t类型，返回TxControlReg寄存器中的值。
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

/*-----rc522非接触式uart接口函数定义结束-----*/

/**
 * @brief  rc522复位。
 * @param  handle：传入rc522句柄。
 * @retval 空
 */
void RC522_Reset(rc522_handle_t *handle)
{
    RC522_SET_RST(handle, 0);
    HAL_Delay(1);
    RC522_SET_RST(handle, 1);
    /*硬件复位*/
    RC522_SetRegisterBitMask(handle, CommandReg, 0x0f);
    /*软件复位*/

    while (RC522_ReadRegister(handle, CommandReg) != 0x20)
        ;
    /*等待软件复位完成*/
}

/**
 * @brief  rc522初始化
 * @param  handle：rc522句柄。
 * @retval int类型，返回0，无意义。
 */
int RC522_Init(rc522_handle_t *handle)
{
    SPI_Init(handle);

    RC522_Reset(handle); /*rc522复位*/

    RC522_ClearFIFOBuffer(handle); /*清除FIFO缓冲区数据*/

    RC522_SetTMode(handle, 1, 0, 0); /*设置定时器接收到第一个数据就停止, 非门控模式, 非自动重装*/

    RC522_SetTPrescaler(handle, 3380);
    RC522_SetTReload(handle, 30);
    /*定时15ms*/

    RC522_SetTxASKReg(handle, 1); /*开启非接触式uartASK100%调制*/

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
    unsigned char n; /*临时变量*/
    unsigned int i;
    switch (rc522_command)
    {
    case PCD_AUTHENT:
        irqEn = 0x12;
        waitFor = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn = 0x77; /*使能所有中断请求,除了FIFO高于水平线中断请求*/
        waitFor = 0x30;
        break;
    default:
        break;
    }

    RC522_WriteRegister(handle, CommIEnReg, irqEn | 0x80); /*使能中断请求,并且将IRQ引脚输出倒置*/
    RC522_ClearRegisterBitMask(handle, ComIrqReg, 0x80);   /*7bit->Set1, 表示将所有中断请求清零*/
    RC522_WriteRegister(handle, CommandReg, PCD_IDLE);     /*停止运行命令*/
    RC522_SetRegisterBitMask(handle, FIFOLevelReg, 0x80);  /*将FIFO缓冲区清零*/

    for (i = 0; i < data_len; i++)
    {
        RC522_WriteRegister(handle, FIFODataReg, data_to_fifo[i]); /*将数据写入FIFO缓冲区*/
    }
    RC522_WriteRegister(handle, CommandReg, rc522_command);

    if (rc522_command == PCD_TRANSCEIVE)
    {
        RC522_SetRegisterBitMask(handle, BitFramingReg, 0x80); /*开始数据传输*/
    }

    i = 600; // 根据时钟频率调整，操作M1卡最大等待时间25ms
    do
    {
        n = RC522_ReadRegister(handle, ComIrqReg);
        i--;
    } while ((i != 0) /*i为0*/ && !(n & 0x01) /*定时中断*/ && !(n & waitFor) /*数据传输完成*/);
    RC522_ClearRegisterBitMask(handle, BitFramingReg, 0x80); /*停止数据传输*/

    if (i != 0)
    {
        if (!(RC522_ReadRegister(handle, ErrorReg) & 0x1B)) /*检查FIFO缓冲区是否溢出,是否位冲突,奇偶校验是否错误*/
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
                /*bit2~bit0位为RxLastBits，指示最后一个字节接收到的位数，如果该标志为0，则表示最后一个字节8位全为有效位。*/
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

    RC522_ClearRegisterBitMask(handle, Status2Reg, 0x08); /*清除MFCypto1On位，该位只能通过软件清除*/
    RC522_WriteRegister(handle, BitFramingReg, 0x07);     /*定义传输的最后一个字节发送7位*/
    RC522_SetRegisterBitMask(handle, TxControlReg, 0x03); /*打开非接触式uart发送器*/

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
