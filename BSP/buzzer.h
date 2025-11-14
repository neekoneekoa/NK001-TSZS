#ifndef __BUZZER_H
#define __BUZZER_H

#include <stdint.h>

typedef enum {
    BUZZER_STATE_OFF = 0,
    BUZZER_STATE_ON
} BUZZER_State_t;

// 初始化蜂鸣器（使用 PA8，输出推挽）
void buzzer_init(void);
// 打开蜂鸣器（PA8 = High）
void buzzer_on(void);
// 关闭蜂鸣器（PA8 = Low）
void buzzer_off(void);
// 切换蜂鸣器状态
void buzzer_toggle(void);

#endif /* __BUZZER_H */
