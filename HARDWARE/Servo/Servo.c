#include "Servo.h"

/**
 * @brief TIM1_PWM��ʼ�� 20ms 50Hz
 *
 */
void PWM_TIM1_Init(void)
{
    /*��ʼ���ṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /*����ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    /*�������� PA8&PA11*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*������ӳ��*/
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_TIM1);

    /*��ʱ��TIM1��ʼ��*/
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStructure);
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_Period = 20000 - 1;  // arr
    TIM_TimeBaseInitStructure.TIM_Prescaler = 168 - 1; // psc
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);
    TIM_CtrlPWMOutputs(TIM1, ENABLE);

    /*���ö�ʱ��TIM1ΪOC_PWM (oc1&oc4)*/
    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;

    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC4Init(TIM1, &TIM_OCInitStructure);

    // TIM_ARRPreloadConfig(TIM1, ENABLE); // ARRӰ�ӼĴ�������

    // TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable); // ����Ƚ�Ӱ�ӼĴ�������
    // TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);

    /*���PWM_CCR*/
    TIM_SetCompare1(TIM1, 0);
    TIM_SetCompare4(TIM1, 0);

    /*������ʱ��TIM1*/
    TIM_Cmd(TIM1, ENABLE);
}

/**
 * @brief ֱ�����ö��1��PWM_CCR 0.5ms-2ms��500-2000
 *
 * @param Compare
 */
void PWM_SetServo1(uint16_t Compare)
{
    TIM_SetCompare1(TIM1, Compare);
}

/**
 * @brief ֱ�����ö��2��PWM_CCR 0.5ms-2ms��500-2000
 *
 * @param Compare
 */
void PWM_SetServo2(uint16_t Compare)
{
    TIM_SetCompare4(TIM1, Compare);
}

/**
 * @brief ֱ�����ö��1�ĽǶ� 0-180��
 *
 * @param Angle
 */
void Servo1_SetAngle(float Angle)
{
    uint16_t Compare;
    if (Angle > 0)
        Compare = (uint16_t)(Angle / 180 * 2000 + 500);
    else
        Compare = (uint16_t)(-(Angle / 180 * 2000 + 500));
    TIM_SetCompare1(TIM1, Compare);
}

/**
 * @brief ֱ�����ö��1�ĽǶ� 0-180��
 *
 * @param Angle
 */
void Servo2_SetAngle(float Angle)
{
    uint16_t Compare;
    if (Angle > 0)
        Compare = (uint16_t)(Angle / 180 * 2000 + 500);
    else
        Compare = (uint16_t)(-(Angle / 180 * 2000 + 500));
    TIM_SetCompare4(TIM1, Compare);
}
