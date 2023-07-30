#include "sys.h"
#include "usart.h"
#include "pid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern CAR_PID pid;
extern K210 k210;

u8 USART_RX_BUF[USART_REC_LEN]; // ���ջ���,���USART_REC_LEN���ֽ�.
u16 USART_RX_STA = 0;			// ����״̬���

u8 USART_RX_BUF_U1[10];	 // ���ջ���,���USART_REC_LEN���ֽ�.
u16 USART_RX_STA_U1 = 0; // ����״̬���
//////////////////////////////////////////////////////////////////////////////////
// ���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h" //ucos ʹ��
#endif

static void analysis_data(u8 *buf);
static void change_pid(u8 *buf);
void analysis_k210_data(u8 *buf);

//(�F����)/////////////////////////////////////  ////////////////////////////////////////////

void RX_k210data_deal(void)
{
	analysis_k210_data(USART_RX_BUF_U1);

	// printf("K210Data:%s\r\n", USART_RX_BUF_U1);
	// ��ջ���
	for (int i = 0; i < 10; i++)
	{
		USART_RX_BUF_U1[i] = '\0';
	}
	USART_RX_STA_U1 = 0;
}
/*��������K210������
���ݸ�ʽ�ǣ�
!x��y@
*/
// void analysis_k210_data(u8 *buf)
// {
// 	// Step 1: �ж����ݸ�ʽ�Ƿ���ȷ
// 	if (buf[0] != '!' || buf[2] != ',' || buf[4] != '@')
// 	{
// 		printf("Invalid data format!\r\n");
// 		return;
// 	}

// 	// Step 2: ��ȡx��y��ֵ
// 	k210.x = buf[1];
// 	k210.y = buf[3];
// 	// Step 3: ����x��y��ֵ������������Ը�����Ҫ��������������
// 	printf("x: %c, y: %c\r\n", k210.x, k210.y);

// }
#define K210_FILTER 0.3f

void analysis_k210_data(u8 *buf)
{
	char *start_ptr = strchr((char *)buf, '!'); // �ҵ�'!'��λ��
	char *comma_ptr = strchr((char *)buf, ','); // �ҵ�','��λ��
	char *end_ptr = strchr((char *)buf, '@');	// �ҵ�'@'��λ��

	if (start_ptr != NULL && comma_ptr != NULL && end_ptr != NULL)
	{
		// ������ݰ���ʽ�Ƿ���ȷ
		if (comma_ptr > start_ptr && end_ptr > comma_ptr)
		{
			// ��ȡx��y
			char x_str[10];
			char y_str[10];
			memset(x_str, 0, sizeof(x_str));
			memset(y_str, 0, sizeof(y_str));

			// ����x��y���ַ�������
			strncpy(x_str, start_ptr + 1, comma_ptr - start_ptr - 1);
			strncpy(y_str, comma_ptr + 1, end_ptr - comma_ptr - 1);

			// ����ȡ����x��yת������ֵ
			int8_t x = 112 - (int8_t)(atoi(x_str));
			int8_t y = 112 - (int8_t)(atoi(y_str));
			// ��ͨ�˲�
			k210.x += K210_FILTER * ((float)x - k210.x);
			k210.y += K210_FILTER * ((float)y - k210.y);

			// printf("x = %.2f, y = %.2f\n", k210.x, k210.y);
		}
	}
}

void RX_data_deal(void)
{
	// printf("RX_data_deal\r\n");
	analysis_data(USART_RX_BUF);

	// ��ջ���
	for (int i = 0; i < 32; i++)
	{
		USART_RX_BUF[i] = '\0';
	}
	USART_RX_STA = 0;
}

/*
�趨�ٶȣ�v1.5
��ʾ���Σ�wa
*/
static void analysis_data(u8 *buf)
{

	static float speed = 0.2;
	if (buf[0] != '\0')
	{
		printf("RX data:%s\r\n", USART_RX_BUF);
		if (buf[1] != '\0')
		{
			if (buf[0] == 'v')
			{
				speed = atof((char *)buf + 1);
				car.speed_set = speed;
				printf("//////////////////////////\r\nspeed:%f\r\n\r\n", speed);
			}
			else if (buf[0] == 'w' && buf[1] == 'a') // ��ʾ����
			{
				flag_G.wave ^= 1;
			}
			else if (buf[0] == 'a') // ת��
			{
				car.turn_set = atof((char *)buf + 1);
				printf("//////////////////////////\r\nturn:%f\r\n\r\n", car.turn_set);
			}
			else if (buf[1] == 's' && buf[2] == 'e') // ���
			{
				if (buf[0] == '1')
				{
					car.servo1_set = atoi((char *)buf + 3);
					printf("//////////////////////////\r\nservo1:%d\r\n\r\n", car.servo1_set);
				}
				else if (buf[0] == '2')
				{
					car.servo2_set = atoi((char *)buf + 3);
					printf("//////////////////////////\r\nservo2:%d\r\n\r\n", car.servo2_set);
				}
			}
			else if (buf[0] == '*')
				change_pid(buf);
		}
		else
		{
			u8 speChange_flag = 0;
			switch (buf[0])
			{
			case 'G':
				car.speed_set = speed;
				speChange_flag = 1;
				break;
			case 'S': // ֹͣ
				car.speed_set = 0;
				speChange_flag = 1;

				car.status = stop;
				car.turn_set = 0;
				break;

			case 'Q': // ֹͣ
				// car.dis_set = 4280.0f;
				car.dis_set = -3860.0f * 4;
				car.l_distanc = 0;
				car.r_distanc = 0;

				break;

			case 'E': // ֹͣ
				car.turn_set = 90.0f;
				break;
				// case 'K':
				// 	car.speed_set = -speed;
				// 	break;
				// case 'D':
				// 	car.speed_set += 1;
				// 	break;
				// case 'E':
				// 	car.speed_set -= 1;
				// 	break;
				// case 'F':
				// 	car.status = CAR_STOP;
				// 	break;
				// case 'R':
				// 	// car.turnSet -= 20;
				// 	car.dirOPEN = 2;
				// 	// flag_G.buzzer = 1;
				// 	break;
				// case 'L':
				// 	// car.turnSet += 20;
				// 	car.dirOPEN = 1;
				// 	break;
				// default:
				// 	break;
			}
			if (speChange_flag)
			{
				speChange_flag = 0;
				printf("set spe:%f \r\n", car.speed_set);
			}
		}
	}
}

/*******************************
 * �޸�PID
 *******************************/
static void change_pid(u8 *buf)
{
	static float *val;
	// static float *val_l;

	int8_t pidSet = 20;
	PID *SetPID;

	switch (buf[1])
	{
	case 'v':
		SetPID = &pid.speed_r;
		pidSet = 0;
		switch (buf[2])
		{
		case 'p':
			val = &pid.speed_r.kp;
			// val_l = &pid.speed_l.kp;
			break;
		case 'i':
			val = &pid.speed_r.ki;
			// val_l = &pid.speed_l.ki;
			break;
		case 'd':
			val = &pid.speed_r.kd;
			// val_l = &pid.speed_l.kd;
			break;
		}
		break;

	case 'a':
		SetPID = &pid.angle;
		pidSet = 0;
		switch (buf[2])
		{
		case 'p':
			val = &pid.angle.kp;

			break;
		case 'i':
			val = &pid.angle.ki;

			break;
		case 'd':
			val = &pid.angle.kd;

			break;
		}
		break;
	}

	if (pidSet != 20)
	{
		// buf[0] = '\0';
		// buf[1] = '\0';
		*val = atof((char *)buf + 3);
		// *val_l = atof((char *)buf + 3);
		printf("//////////////////////////\r\nPID set P:%f I:%f D:%f\r\n\r\n", SetPID->kp, SetPID->ki, SetPID->kd);
	}
}

//(�F����)///////////////////////////////////// ��ʼ�� ////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

// �������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
// ��׼����Ҫ��֧�ֺ���
struct __FILE
{
	int handle;
};

FILE __stdout;
// ����_sys_exit()�Ա���ʹ�ð�����ģʽ
void _sys_exit(int x)
{
	x = x;
}
// �ض���fputc����
int fputc(int ch, FILE *f)
{
	// while ((USART1->SR & 0X40) == 0)
	// 	; // ѭ������,ֱ���������
	// USART1->DR = (u8)ch;
	// return ch;

	while ((USART3->SR & 0X40) == 0)
		; // ѭ������,ֱ���������
	USART3->DR = (u8)ch;
	return ch;
}
#endif

#if EN_USART1_RX // ���ʹ���˽���
// ����1�жϷ������
// ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���

// ��ʼ��IO ����1
// bound:������
void uart_init(u32 bound)
{
	// GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  // ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // ʹ��USART1ʱ��

	// ����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  // GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); // GPIOA10����ΪUSART1

	// USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; // GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// �ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			// ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// ��ʼ��PA9��PA10

	// USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;										// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ
	USART_Init(USART1, &USART_InitStructure);										// ��ʼ������1

	USART_Cmd(USART1, ENABLE); // ʹ�ܴ���1

	// USART_ClearFlag(USART1, USART_FLAG_TC);

#if EN_USART1_RX
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // ��������ж�

	// Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		  // ����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ�����

#endif

	//-----(�F����)/---------------------- init UART3 -----------------------------//

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  // ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); // ʹ��USART3ʱ��

	// ����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART2); // GPIOB11����ΪUSART3
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART2); // GPIOB10����ΪUSART3

	// USART1�˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; // GPIOB11��GPIOB10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			 // ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 // �ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			 // ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;			 // ����
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 // ��ʼ��

	// USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;										// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ
	USART_Init(USART3, &USART_InitStructure);										// ��ʼ������1

	USART_Cmd(USART3, ENABLE); // ʹ�ܴ���3

	// USART_ClearFlag(USART1, USART_FLAG_TC);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // ��������ж�

	// Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		  // ����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; // ��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		  // �����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);							  // ����ָ���Ĳ�����ʼ��VIC�Ĵ�����
}

void USART1_IRQHandler(void) // ����1�жϷ������
{
	u8 Res;
#if SYSTEM_SUPPORT_OS // ���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(USART1); //(USART1->DR);	//��ȡ���յ�������
		if (Res == '@')
		{
			flag_G.k210 = 1;
			// printf("2ss\r\n");
		}
		if ((USART_RX_STA_U1 & 0x8000) == 0) // ����δ���
		{
			if (USART_RX_STA_U1 & 0x4000) // ���յ���0x0d
			{
				if (Res != 0x0a)
					USART_RX_STA_U1 = 0; // ���մ���,���¿�ʼ
				else
					USART_RX_STA_U1 |= 0x8000; // ���������
			}
			else // ��û�յ�0X0D
			{
				if (Res == 0x0d)
					USART_RX_STA_U1 |= 0x4000;
				else
				{
					USART_RX_BUF_U1[USART_RX_STA_U1 & 0X3FFF] = Res;
					USART_RX_STA_U1++;
					if (USART_RX_STA_U1 > (USART_REC_LEN - 1))
						USART_RX_STA_U1 = 0; // �������ݴ���,���¿�ʼ����
				}
			}
		}
	}
#if SYSTEM_SUPPORT_OS // ���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();
#endif
}
#endif

void USART3_IRQHandler(void) // ����3�жϷ������
{
	u8 Res;
#if SYSTEM_SUPPORT_OS // ���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();
#endif
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // �����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(USART3); //(USART3->DR);	//��ȡ���յ�������

		if ((USART_RX_STA & 0x8000) == 0) // ����δ���
		{
			if (USART_RX_STA & 0x4000) // ���յ���0x0d
			{
				if (Res != 0x0a)
					USART_RX_STA = 0; // ���մ���,���¿�ʼ
				else
					USART_RX_STA |= 0x8000; // ���������
			}
			else // ��û�յ�0X0D
			{
				if (Res == 0x0d)
					USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = Res;
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1))
						USART_RX_STA = 0; // �������ݴ���,���¿�ʼ����
				}
			}
		}
	}
#if SYSTEM_SUPPORT_OS // ���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();
#endif
}
