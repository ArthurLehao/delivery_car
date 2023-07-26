#ifndef __PID_H__
#define __PID_H__
#include "sys.h"
//(�F����)///////////////////////////////////// PID���� ////////////////////////////////////////////
#define SPEED_P -9000.0f
#define SPEED_I -2100.0f
#define SPEED_D -1.0f

#define DIS_P 0.01f
// #define DIS_I DIS_P / 200.0f
#define DIS_I 0.003f
#define DIS_D 0.00001f

#define ANGLE_P 0.035f
#define ANGLE_I 0.0f
#define ANGLE_D 0.04f

#define ANGSPE_P -0.00007f
#define ANGSPE_I -0.0f
#define ANGSPE_D -0.000007f

// #define GRAY_P 300.0f
// #define GRAY_I -0.0f
// #define GRAY_D 100.0f
#define GRAY_P 0.2f
#define GRAY_I -0.0f
#define GRAY_D 0.05f
//(�F����)///////////////////////////////////// �Զ���ṹ�� ////////////////////////////////////////////
typedef enum
{
    PI,
    PD,
    P,
    PIDALL
} PIDTYPE;

typedef struct // ����ʽPID
{
    float kp, ki, kd;                   // ����ϵ��
    float error, lastError, lastError2; // ���ϴ����
    float integral, maxIntegral;        // ���֡������޷�
    float output, maxOutput;            // ���������޷�
    PIDTYPE type;
} PID;

typedef struct // ���ִ���PID
{
    PID speed_r, speed_l; // �ǶȻ�
    PID angle;            // �ٶȻ�
    PID angSpe;           // �Ǽ��ٶȻ�
    PID dis;              // λ�ƻ�
    PID gray;             // �ҶȻ�
    int output;           // �������������inner.output
} CAR_PID;

//(�F����)///////////////////////////////////// Function ////////////////////////////////////////////
void PID_set(void);
float PID_Incremental(PID *pid, float target, float feedback);
float PID_Positional(PID *pid, float target, float feedback);

#endif