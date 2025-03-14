#include "sccb.h"
#include "Delay.h"

static void sccb_set_sda(sccb_handle_t *handle, uint32_t gpio_mode)
{
    GPIO_InitTypeDef gpio_initstructure = {0};
    gpio_initstructure.Pin = handle->io.sda.pin;
    gpio_initstructure.Speed = GPIO_SPEED_FREQ_HIGH;
    switch(gpio_mode)
    {
        case GPIO_MODE_OUTPUT_PP :
            gpio_initstructure.Mode = gpio_mode;
            gpio_initstructure.Pull = GPIO_NOPULL;
            break;
        case GPIO_MODE_INPUT:
            gpio_initstructure.Mode = gpio_mode;
            gpio_initstructure.Pull = GPIO_PULLUP;
            break;
        default :
            gpio_initstructure.Mode = GPIO_MODE_OUTPUT_PP;
            gpio_initstructure.Pull = GPIO_NOPULL;
            break;
    }
    HAL_GPIO_Init(handle->io.sda.GPIOx, &gpio_initstructure);
}

static inline void sccb_start(sccb_handle_t *handle)
{
    SCCB_SDA(handle, 1);
    SCCB_SCL(handle, 1);
    Delay_us(50);
    SCCB_SDA(handle, 0);
    Delay_us(50); // 并不是必须为50us，不要太短即可
    SCCB_SCL(handle, 0);
}

static inline void sccb_stop(sccb_handle_t *handle)
{
    SCCB_SDA(handle, 0);
    Delay_us(50);
    SCCB_SCL(handle, 1);
    Delay_us(50);
    SCCB_SDA(handle, 1);
    Delay_us(50);
}

static int sccb_write(sccb_handle_t *handle, unsigned char data)
{
    uint8_t i, res;
    for (i = 0; i < 8; i++) // 循环发送bit7-bit0
    {
        SCCB_SDA(handle, data & 0x80);
        data <<= 1;
        Delay_us(50);
        SCCB_SCL(handle, 1);
        Delay_us(50);
        SCCB_SCL(handle, 0);
    }
    Delay_us(50);
    SCCB_SCL(handle, 1); // 将SCL置1，此时如果数据已被从机接收，从机将把SDA置0
    Delay_us(50);
    if (SCCB_READ_SDA(handle))
        res = 1; // SDA置1，说明从机没有成功接收数据
    else
        res = 0; // 发送成功
    SCCB_SCL(handle, 0);
    return res;
}

static unsigned char sccb_read(sccb_handle_t *handle)
{
    uint8_t temp = 0, i;
    sccb_set_sda(handle, GPIO_MODE_INPUT);
    for (i = 8; i > 0; i--) // 循环读取bit7-bit0
    {
        Delay_us(50);
        SCCB_SCL(handle, 1);
        temp = temp << 1;
        if (SCCB_READ_SDA(handle))
            temp++; // SCCB_READ_SDA是从IO口读到的数据
        Delay_us(50);
        SCCB_SCL(handle, 0);
    }
    sccb_set_sda(handle, GPIO_MODE_OUTPUT_PP);
    return temp;
}

static void sccb_nack(sccb_handle_t *handle)
{
    Delay_us(50);
    SCCB_SDA(handle, 1);
    SCCB_SCL(handle, 1);
    Delay_us(50);
    SCCB_SCL(handle, 0);
    Delay_us(50);
    SCCB_SDA(handle, 0);
    Delay_us(50);
}

int SCCB_WriteData(sccb_handle_t *handle, unsigned char sub_addr, unsigned char data)
{
    uint8_t res = 0;
    handle->sub_addr = sub_addr;
    handle->wdata = data;
    sccb_start(handle); // 启动传输的标志
    if (sccb_write(handle, handle->id_addr))
        res = 1; // 写入OV2640传感器ID
    Delay_us(100);
    if (sccb_write(handle, sub_addr))
        res = 1; // 写寄存器地址
    Delay_us(100);
    if (sccb_write(handle, data))
        res = 1; // 写要向寄存器写入的数据
    sccb_stop(handle); // 结束传输的标志
    return res;
}

int SCCB_ReadData(sccb_handle_t *handle, unsigned char sub_addr)
{
    // 对应两相写操作
    handle->sub_addr = sub_addr;
    sccb_start(handle);          // 启动传输
    sccb_write(handle, handle->id_addr); // 相1
    Delay_us(100);
    sccb_write(handle, sub_addr); // 相2
    Delay_us(100);
    sccb_stop(handle); // 结束传输
    Delay_us(100);
    // 对应两相读操作
    sccb_start(handle);                 // 启动传输
    sccb_write(handle, handle->id_addr | 0X01); // 相1
    Delay_us(100);
    handle->rdata = sccb_read(handle); // 相2
    sccb_nack(handle);        // 读取完8bit数据后的应答
    sccb_stop(handle);          // 结束传输
    return handle->rdata;
}
