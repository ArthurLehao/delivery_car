/*
 * @Author: hz 1281989921@qq.com
 * @Date: 2023-07-10 10:31:29
 * @LastEditors: hz 1281989921@qq.com
 * @LastEditTime: 2023-07-10 13:49:25
 * @FilePath: \USERe:\����\Posture _Glove\HARDWARE\MPU6050\mpuiic.h
 * @Description: ����Ĭ������,������`customMade`, ��koroFileHeader�鿴���� ��������: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __MPUIIC_H
#define __MPUIIC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
// ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// ALIENTEK MiniSTM32F103������
// MPU6050 IIC���� ����
// ����ԭ��@ALIENTEK
// ������̳:www.openedv.com
// ��������:2015/4/18
// �汾��V1.0
// ��Ȩ���У�����ؾ���
// Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

// IO��������
#define MPU_SDA_IN()                     \
    {                                    \
        GPIOB->CRL &= 0X0FFFFFFF;        \
        GPIOB->CRL |= (uint32_t)8 << 28; \
    }
#define MPU_SDA_OUT()                    \
    {                                    \
        GPIOB->CRL &= 0X0FFFFFFF;        \
        GPIOB->CRL |= (uint32_t)3 << 28; \
    }
// #define MPU_SDA_IN()                     \
//     {                                    \
//         GPIOB->CRL &= 0XFF0FFFFF;        \
//         GPIOB->CRL |= (uint32_t)8 << 20; \
//     }
// #define MPU_SDA_OUT()                    \
//     {                                    \
//         GPIOB->CRL &= 0XFF0FFFFF;        \
//         GPIOB->CRL |= (uint32_t)3 << 20; \
//     }
// 0��λ��
#define MPU_SDA_IN_2()                   \
    {                                    \
        GPIOB->CRL &= 0XFF0FFFFF;        \
        GPIOB->CRL |= (uint32_t)8 << 20; \
    }
#define MPU_SDA_OUT_2()                  \
    {                                    \
        GPIOB->CRL &= 0XFF0FFFFF;        \
        GPIOB->CRL |= (uint32_t)3 << 20; \
    }

// IO��������

extern u8 scl, sda;

// #define MPU_IIC_SCL PBout(scl) // SCL
// #define MPU_IIC_SDA PBout(sda) // SDA
// #define MPU_READ_SDA PBin(sda) // SDA

// #define MPU_IIC_SCL PBout(6) // SCL
// #define MPU_IIC_SDA PBout(7) // SDA
// #define MPU_READ_SDA PBin(7) // SDA

// PB4 ��������һ�������

// #define MPU_IIC_SCL PBout(6) // SCL
// #define MPU_IIC_SDA PBout(7) // SDA
// #define MPU_READ_SDA PBin(7) // SDA
#define MPU_IIC_SCL PBout(6) // SCL
#define MPU_IIC_SDA PBout(7) // SDA
#define MPU_READ_SDA PBin(7) // SDA

#define MPU_IIC_SCL_2 PBout(4) // SCL
#define MPU_IIC_SDA_2 PBout(5) // SDA
#define MPU_READ_SDA_2 PBin(5) // SDA

// IIC���в�������
void MPU_IIC_Delay(void);                // MPU IIC��ʱ����
void MPU_IIC_Init(void);                 // ��ʼ��IIC��IO��
void MPU_IIC_Start(void);                // ����IIC��ʼ�ź�
void MPU_IIC_Stop(void);                 // ����IICֹͣ�ź�
void MPU_IIC_Send_Byte(u8 txd);          // IIC����һ���ֽ�
u8 MPU_IIC_Read_Byte(unsigned char ack); // IIC��ȡһ���ֽ�
u8 MPU_IIC_Wait_Ack(void);               // IIC�ȴ�ACK�ź�
void MPU_IIC_Ack(void);                  // IIC����ACK�ź�
void MPU_IIC_NAck(void);                 // IIC������ACK�ź�

void IMPU_IC_Write_One_Byte(u8 daddr, u8 addr, u8 data);
u8 MPU_IIC_Read_One_Byte(u8 daddr, u8 addr);

void MPU_IIC_SCL_switch(void);
void MPU_IIC_SDA_switch(void);
void MPU_READ_SDA_switch(void);

// #define MPU_IIC_SCL MPU_IIC_SCL_switch()   // SCL
// #define MPU_IIC_SDA MPU_IIC_SDA_switch()   // SDA
// #define MPU_READ_SDA MPU_READ_SDA_switch() // SDA

#endif
