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
    Delay_us(50); // �����Ǳ���Ϊ50us����Ҫ̫�̼���
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
    for (i = 0; i < 8; i++) // ѭ������bit7-bit0
    {
        SCCB_SDA(handle, data & 0x80);
        data <<= 1;
        Delay_us(50);
        SCCB_SCL(handle, 1);
        Delay_us(50);
        SCCB_SCL(handle, 0);
    }
    Delay_us(50);
    SCCB_SCL(handle, 1); // ��SCL��1����ʱ��������ѱ��ӻ����գ��ӻ�����SDA��0
    Delay_us(50);
    if (SCCB_READ_SDA(handle))
        res = 1; // SDA��1��˵���ӻ�û�гɹ���������
    else
        res = 0; // ���ͳɹ�
    SCCB_SCL(handle, 0);
    return res;
}

static unsigned char sccb_read(sccb_handle_t *handle)
{
    uint8_t temp = 0, i;
    sccb_set_sda(handle, GPIO_MODE_INPUT);
    for (i = 8; i > 0; i--) // ѭ����ȡbit7-bit0
    {
        Delay_us(50);
        SCCB_SCL(handle, 1);
        temp = temp << 1;
        if (SCCB_READ_SDA(handle))
            temp++; // SCCB_READ_SDA�Ǵ�IO�ڶ���������
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
    sccb_start(handle); // ��������ı�־
    if (sccb_write(handle, handle->id_addr))
        res = 1; // д��OV2640������ID
    Delay_us(100);
    if (sccb_write(handle, sub_addr))
        res = 1; // д�Ĵ�����ַ
    Delay_us(100);
    if (sccb_write(handle, data))
        res = 1; // дҪ��Ĵ���д�������
    sccb_stop(handle); // ��������ı�־
    return res;
}

int SCCB_ReadData(sccb_handle_t *handle, unsigned char sub_addr)
{
    // ��Ӧ����д����
    handle->sub_addr = sub_addr;
    sccb_start(handle);          // ��������
    sccb_write(handle, handle->id_addr); // ��1
    Delay_us(100);
    sccb_write(handle, sub_addr); // ��2
    Delay_us(100);
    sccb_stop(handle); // ��������
    Delay_us(100);
    // ��Ӧ���������
    sccb_start(handle);                 // ��������
    sccb_write(handle, handle->id_addr | 0X01); // ��1
    Delay_us(100);
    handle->rdata = sccb_read(handle); // ��2
    sccb_nack(handle);        // ��ȡ��8bit���ݺ��Ӧ��
    sccb_stop(handle);          // ��������
    return handle->rdata;
}
