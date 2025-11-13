#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>

// 按键ID定义
typedef enum {
    KEY_NONE = 0,
    KEY_K1,
    KEY_K2,
    KEY_K3,
    KEY_K4,
    KEY_K5,
    KEY_K6
} KEY_ID;

// 函数声明
void key_init(void);           // 按键初始化
KEY_ID key_scan(void);         // 扫描按键并返回按键ID
// 获取原始ADC电压值：pb0_mv 对应 PB0（K1/K2/K3），pb1_mv 对应 PB1（K4/K5/K6）
void key_get_adc_mv(uint16_t *pb0_mv, uint16_t *pb1_mv);

#endif /* __KEY_H */
