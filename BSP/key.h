#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>

// ADC通道定义
#define KEY_ADC_CHANNEL_K1_K2_K3 8  // PB0 (ADC1 Channel 8)
#define KEY_ADC_CHANNEL_K4_K5_K6 9  // PB1 (ADC1 Channel 9)
#define ADC_CHANNEL_PA0          0  // PA0 (ADC1 Channel 0)
#define ADC_CHANNEL_PA1          1  // PA1 (ADC1 Channel 1)

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

// 获取PA0和PA1的ADC电压值
void adc_get_pa_mv(uint16_t *pa0_mv, uint16_t *pa1_mv);

// 获取PA0和PA1的ADC原始值
void adc_get_pa_raw(uint16_t *pa0_raw, uint16_t *pa1_raw);

// ADC电源控制
void adc_power_off(void);  // 关闭ADC电源
void adc_power_on(void);   // 打开ADC电源并重新校准

#endif /* __KEY_H */
