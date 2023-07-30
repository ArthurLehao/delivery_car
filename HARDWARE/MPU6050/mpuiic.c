#include "mpuiic.h"
#include "delay.h"

extern u8 mpu_sel;
/**************************************************************************
 ��  �� ���������
 �Ա���ַ��https://shop119207236.taobao.com
 ΢�Ź��ںš���������ˡ�
 ��̨�ظ���ƽ��С��������ȡƽ��С��ȫ��DIY����
 ��̨�ظ������ӿ������ߡ�����ȡ���ӹ���ʦ�ر���������
 ��̨�ظ�������������ϡ�����ȡ����������ϰ�
 ֪����������
**************************************************************************/
// MPU IIC ��ʱ����
void MPU_IIC_Delay(void)
{
	delay_us(2);
}

// ��ʼ��IIC
void MPU_IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);										  // ʹ��PB�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7; // �˿�����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;											  // �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;											  // 50M
	GPIO_Init(GPIOB, &GPIO_InitStructure);														  // �����趨������ʼ��GPIOB
}
// ����IIC��ʼ�ź�
void MPU_IIC_Start(void)
{
	if (mpu_sel)
	{
		MPU_SDA_OUT(); // sda�����
		MPU_IIC_SDA = 1;
		MPU_IIC_SCL = 1;
		MPU_IIC_Delay();
		MPU_IIC_SDA = 0; // START:when CLK is high,DATA change form high to low
		MPU_IIC_Delay();
		MPU_IIC_SCL = 0; // ǯסI2C���ߣ�׼�����ͻ��������
	}
	else
	{
		MPU_SDA_OUT_2(); // sda�����
		MPU_IIC_SDA_2 = 1;
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SDA_2 = 0; // START:when CLK is high,DATA change form high to low
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 0; // ǯסI2C���ߣ�׼�����ͻ��������
	}
}
// ����IICֹͣ�ź�
void MPU_IIC_Stop(void)
{
	if (mpu_sel)
	{
		MPU_SDA_OUT(); // sda�����
		MPU_IIC_SCL = 0;
		MPU_IIC_SDA = 0; // STOP:when CLK is high DATA change form low to high
		MPU_IIC_Delay();
		MPU_IIC_SCL = 1;
		MPU_IIC_SDA = 1; // ����I2C���߽����ź�
		MPU_IIC_Delay();
	}
	else
	{
		MPU_SDA_OUT_2(); // sda�����
		MPU_IIC_SCL_2 = 0;
		MPU_IIC_SDA_2 = 0; // STOP:when CLK is high DATA change form low to high
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_SDA_2 = 1; // ����I2C���߽����ź�
		MPU_IIC_Delay();
	}
}
// �ȴ�Ӧ���źŵ���
// ����ֵ��1������Ӧ��ʧ��
//         0������Ӧ��ɹ�
u8 MPU_IIC_Wait_Ack(void)
{
	if (mpu_sel)
	{
		u8 ucErrTime = 0;
		MPU_SDA_IN(); // SDA����Ϊ����
		MPU_IIC_SDA = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 1;
		MPU_IIC_Delay();
		while (MPU_READ_SDA)
		{
			ucErrTime++;
			if (ucErrTime > 250)
			{
				MPU_IIC_Stop();
				return 1;
			}
		}
		MPU_IIC_SCL = 0; // ʱ�����0
		return 0;
	}
	else
	{
		u8 ucErrTime = 0;
		MPU_SDA_IN_2(); // SDA����Ϊ����
		MPU_IIC_SDA_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_Delay();
		while (MPU_READ_SDA_2)
		{
			ucErrTime++;
			if (ucErrTime > 250)
			{
				MPU_IIC_Stop();
				return 1;
			}
		}
		MPU_IIC_SCL_2 = 0; // ʱ�����0
		return 0;
	}
}
// ����ACKӦ��
void MPU_IIC_Ack(void)
{
	if (mpu_sel)
	{
		MPU_IIC_SCL = 0;
		MPU_SDA_OUT();
		MPU_IIC_SDA = 0;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 0;
	}
	else
	{
		MPU_IIC_SCL_2 = 0;
		MPU_SDA_OUT_2();
		MPU_IIC_SDA_2 = 0;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 0;
	}
}
// ������ACKӦ��
void MPU_IIC_NAck(void)
{
	if (mpu_sel)
	{
		MPU_IIC_SCL = 0;
		MPU_SDA_OUT();
		MPU_IIC_SDA = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL = 0;
	}
	else
	{
		MPU_IIC_SCL_2 = 0;
		MPU_SDA_OUT_2();
		MPU_IIC_SDA_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 1;
		MPU_IIC_Delay();
		MPU_IIC_SCL_2 = 0;
	}
}
// IIC����һ���ֽ�
// ���شӻ�����Ӧ��
// 1����Ӧ��
// 0����Ӧ��
void MPU_IIC_Send_Byte(u8 txd)
{
	if (mpu_sel)
	{
		u8 t;
		MPU_SDA_OUT();
		MPU_IIC_SCL = 0; // ����ʱ�ӿ�ʼ���ݴ���
		for (t = 0; t < 8; t++)
		{
			MPU_IIC_SDA = (txd & 0x80) >> 7;
			txd <<= 1;
			MPU_IIC_SCL = 1;
			MPU_IIC_Delay();
			MPU_IIC_SCL = 0;
			MPU_IIC_Delay();
		}
	}
	else
	{
		u8 t;
		MPU_SDA_OUT_2();
		MPU_IIC_SCL_2 = 0; // ����ʱ�ӿ�ʼ���ݴ���
		for (t = 0; t < 8; t++)
		{
			MPU_IIC_SDA_2 = (txd & 0x80) >> 7;
			txd <<= 1;
			MPU_IIC_SCL_2 = 1;
			MPU_IIC_Delay();
			MPU_IIC_SCL_2 = 0;
			MPU_IIC_Delay();
		}
	}
}
// ��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 MPU_IIC_Read_Byte(unsigned char ack)
{
	if (mpu_sel)
	{
		unsigned char i, receive = 0;
		MPU_SDA_IN(); // SDA����Ϊ����
		for (i = 0; i < 8; i++)
		{
			MPU_IIC_SCL = 0;
			MPU_IIC_Delay();
			MPU_IIC_SCL = 1;
			receive <<= 1;
			if (MPU_READ_SDA)
				receive++;
			MPU_IIC_Delay();
		}
		if (!ack)
			MPU_IIC_NAck(); // ����nACK
		else
			MPU_IIC_Ack(); // ����ACK
		return receive;
	}
	else
	{
		unsigned char i, receive = 0;
		MPU_SDA_IN_2(); // SDA����Ϊ����
		for (i = 0; i < 8; i++)
		{
			MPU_IIC_SCL_2 = 0;
			MPU_IIC_Delay();
			MPU_IIC_SCL_2 = 1;
			receive <<= 1;
			if (MPU_READ_SDA_2)
				receive++;
			MPU_IIC_Delay();
		}
		if (!ack)
			MPU_IIC_NAck(); // ����nACK
		else
			MPU_IIC_Ack(); // ����ACK
		return receive;
	}
}
