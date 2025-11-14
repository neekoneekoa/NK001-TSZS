#ifndef __BUZZER_H
#define __BUZZER_H

#include <stdint.h>

typedef enum {
    BUZZER_STATE_OFF = 0,
    BUZZER_STATE_ON
} BUZZER_State_t;

// 推荐频率定义
#define BUZZER_DEFAULT_FREQ 1300 
#define BUZZER_MIN_FREQ 1000      // 最小频率限制
#define BUZZER_MAX_FREQ 2000      // 最大频率限制

// 初始化蜂鸣器（使用 PA8，输出推挽）
void buzzer_pwm_init(uint16_t frequency, uint16_t duty_cycle);
void buzzer_set_pwm(uint16_t duty_cycle);
// 打开蜂鸣器（PA8 = High）
void buzzer_on(void);
// 关闭蜂鸣器（PA8 = Low）
void buzzer_off(void);
void buzzer_set_frequency(uint16_t frequency);


#endif /* __BUZZER_H */
