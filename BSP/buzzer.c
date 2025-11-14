
#include "buzzer.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"
#include <stdint.h>

#define BUZZER_PIN    GPIO_Pin_8
#define BUZZER_PORT   GPIOA
#define BUZZER_RCC    RCC_APB2Periph_GPIOA

// 配置 PA8 为 PWM 输出（TIM1 CH1）
// frequency：PWM 的基频，单位为 Hz。函数内用公式 TIM_Period = (72 000 000 / frequency) - 1 计算计数周期（代码假定定时器时钟为 72MHz 且 Prescaler = 0）。要求：
// - frequency > 0（否则除 0 错误）。
// - 由于 TIM1 为 16 位定时器，ARR 最大为 65535，因此最低可设置的 frequency ≈ 72 000 000 / 65536 ≈ 1098 Hz（不允许更低，除非修改 Prescaler）。
// - frequency 不要太大以致 Period 变为 0 或未按预期工作；常见蜂鸣器可用 1–4 kHz 或者按需要调整 Prescaler。

// 建议：在调用处先检查 frequency>0 且使得计算出的 Period ≤ 0xFFFF；并把 duty_cycle 限定在 0..100，或在初始化里使用非零 Prescaler 以支持更低频率。
void buzzer_pwm_init(uint16_t frequency, uint16_t duty_cycle)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	// 使能 TIM1 和 GPIOA 时钟
	// TIM1在APB2总线上，GPIOA也在APB2总线上
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | BUZZER_RCC, ENABLE);
	
	// 清除之前可能的TIM1配置
	TIM_DeInit(TIM1);

	// 配置 PA8 为 TIM1_CH1（复用功能）
	GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出，提高驱动能力
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 高速输出
	GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
	
	// 增加一个小延时确保GPIO配置稳定
	volatile uint32_t delay = 0;
	while(delay++ < 1000);

	// 参数验证 - 确保频率有效
	// 使用合理的默认值和范围检查
	#define TEMP_MIN_FREQ 1000
	#define TEMP_MAX_FREQ 5000
	#define TEMP_DEFAULT_FREQ 2000
	if (frequency < TEMP_MIN_FREQ || frequency > TEMP_MAX_FREQ) {
		// 如果频率不在合理范围内，使用默认频率
		frequency = TEMP_DEFAULT_FREQ;
	}
	
	// 使用8MHz系统时钟计算PWM周期
	// 对于无源蜂鸣器，频率精度很重要
	uint16_t period = (8000000 / frequency) - 1;
	
	// 确保周期值有效（16位定时器限制）
	if (period > 65535) period = 65535; // 限制最大值
	if (period < 1) period = 1; // 确保至少为1
	
	// 确保占空比在有效范围内
	if (duty_cycle > 100) duty_cycle = 100;
	
	// 配置 TIM1 基础参数
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// 对于高级定时器TIM1，需要使能重复计数器（这里设为0）
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	// 配置 PWM 模式
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 高级定时器需要配置互补输出状态
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse = (period + 1) * duty_cycle / 100;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// 互补输出极性
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	// 死区时间配置
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset;
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);

	// 使能TIM1主输出（高级定时器必须配置）
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
	// 使能计数器
	TIM_Cmd(TIM1, ENABLE);
}
// 设置 PWM 占空比（0-100）
void buzzer_set_pwm(uint16_t duty_cycle)
{
	// 确保占空比在有效范围内
	if (duty_cycle > 100) duty_cycle = 100;
	
	// 计算比较值并设置 - 确保计算正确
	uint32_t compare_value = (uint32_t)(TIM1->ARR + 1) * duty_cycle / 100;
	// 确保比较值不超过ARR值
	if (compare_value > TIM1->ARR) compare_value = TIM1->ARR;
	TIM_SetCompare1(TIM1, (uint16_t)compare_value);
}

// PA8 输出PWM，蜂鸣器响
void buzzer_on(void)
{
	// 对于无源蜂鸣器，使用70%占空比以获得更大音量
	buzzer_set_pwm(10);
}

// PA8 输出低，蜂鸣器停止
void buzzer_off(void)
{
	buzzer_set_pwm(0);
}

// 直接设置蜂鸣器频率（Hz）
void buzzer_set_frequency(uint16_t frequency)
{
	// 重新初始化PWM，保持当前占空比
	// 使用直接寄存器访问代替TIM_GetCompare1函数
	uint16_t current_duty = (TIM1->CCR1 * 100) / (TIM1->ARR + 1);
	buzzer_pwm_init(frequency, current_duty);
}




