#include "pwm.h"

static uint8_t pwm_duty = 50; // 默认占空比50%

void pwm_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;

	/* 使能GPIOA时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	/* 使能TIM1时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

	/* GPIOA_Pin_11配置为复用推挽输出 */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* 初始化TIM1
	 * 假设APB2时钟为8MHz (内部HSI时钟)
	 * 预分频值8-1，得到1MHz计数频率
	 * 自动重载值100-1，得到10kHz频率
	 */
	TIM_TimeBaseInitStruct.TIM_Period = 100 - 1;              /* 自动重载值 */
	TIM_TimeBaseInitStruct.TIM_Prescaler = 8 - 1;             /* 预分频值 */
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;  /* 时钟分割 */
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; /* 向上计数模式 */
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStruct);

	/* 配置TIM1的PWM模式 */
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;            /* PWM模式1 */
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable; /* 输出使能 */
	TIM_OCInitStruct.TIM_OutputNState = TIM_OutputNState_Disable; /* 互补输出禁用 */
	TIM_OCInitStruct.TIM_Pulse = pwm_duty;                    /* 占空比 */
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;    /* 输出极性高 */
	TIM_OCInitStruct.TIM_OCNPolarity = TIM_OCNPolarity_High;  /* 互补输出极性高 */
	TIM_OCInitStruct.TIM_OCIdleState = TIM_OCIdleState_Reset; /* 空闲状态重置 */
	TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset; /* 互补输出空闲状态重置 */
	TIM_OC4Init(TIM1, &TIM_OCInitStruct);

	/* 启动TIM1 */
	TIM_Cmd(TIM1, ENABLE);
	/* 使能PWM输出 */
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	/* 使能TIM1主输出 */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void pwm_set_duty(uint8_t duty)
{
	/* 限制占空比范围在0-100%之间 */
	if (duty > 100) {
		duty = 100;
	}
	pwm_duty = duty;
	/* 设置PWM占空比 */
	TIM_SetCompare4(TIM1, duty);
}

uint8_t pwm_get_duty(void)
{
	return pwm_duty;
}