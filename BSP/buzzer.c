
#include "buzzer.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include <stdint.h>

#define BUZZER_PIN    GPIO_Pin_8
#define BUZZER_PORT   GPIOA
#define BUZZER_RCC    RCC_APB2Periph_GPIOA

// 初始化 PA8 为推挽输出，初始为低电平（蜂鸣器关闭）
void buzzer_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BUZZER_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);

	buzzer_off();
}

// PA8 输出高，蜂鸣器响
void buzzer_on(void)
{
	GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
}

// PA8 输出低，蜂鸣器停止
void buzzer_off(void)
{
	GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
}

// 切换蜂鸣器状态
void buzzer_toggle(void)
{
	if (GPIO_ReadOutputDataBit(BUZZER_PORT, BUZZER_PIN))
		buzzer_off();
	else
		buzzer_on();
}

