#include "key.h"
#include "adc.h"
#include <stdint.h>

#define KEY_ADC_CHANNEL_K1_K2_K3    ADC_CHANNEL_10   // PB10
#define KEY_ADC_CHANNEL_K4_K5_K6    ADC_CHANNEL_11   // PB11

#define KEY_NONE_PRESS_MV           5000    // 无按键时接近5V
#define KEY3_PRESS_MV               2600    // 3个二极管压降≈0.7V*3
#define KEY2_PRESS_MV               1900    // 2个二极管压降≈0.7V*2
#define KEY1_PRESS_MV               1200    // 1个二极管压降≈0.7V*1

#define ADC_MV_MAX                  5000
#define ADC_MV_MIN                  0

typedef enum {
    KEY_NONE = 0,
    KEY_K1,
    KEY_K2,
    KEY_K3,
    KEY_K4,
    KEY_K5,
    KEY_K6
} KEY_ID;

static uint16_t key_adc_mv[2];   // 0:PB10  1:PB11

// 将ADC原始值转换为毫伏
static uint16_t adc_to_mv(uint16_t adc_val)
{
    // 假设12位ADC，3.3V参考
    return (uint16_t)((uint32_t)adc_val * 3300 / 4095);
}

// 根据电压值判断哪个按键被按下
static KEY_ID key_decode_mv(uint16_t mv)
{
    if (mv > KEY_NONE_PRESS_MV - 200)
        return KEY_NONE;
    else if (mv > KEY3_PRESS_MV - 200 && mv < KEY3_PRESS_MV + 200)
        return KEY_K3;
    else if (mv > KEY2_PRESS_MV - 200 && mv < KEY2_PRESS_MV + 200)
        return KEY_K2;
    else if (mv > KEY1_PRESS_MV - 200 && mv < KEY1_PRESS_MV + 200)
        return KEY_K1;
    else
        return KEY_NONE;
}

// 扫描PB10对应的K1/K2/K3
static KEY_ID key_scan_pb10(void)
{
    uint16_t adc_raw;
    adc_raw = adc_get_value(KEY_ADC_CHANNEL_K1_K2_K3);
    key_adc_mv[0] = adc_to_mv(adc_raw);
    return key_decode_mv(key_adc_mv[0]);
}

// 扫描PB11对应的K4/K5/K6
static KEY_ID key_scan_pb11(void)
{
    uint16_t adc_raw;
    adc_raw = adc_get_value(KEY_ADC_CHANNEL_K4_K5_K6);
    key_adc_mv[1] = adc_to_mv(adc_raw);
    return key_decode_mv(key_adc_mv[1]);
}

// 按键初始化函数
void key_init(void)
{
    // 初始化ADC配置（假设adc_init函数已经实现）
    // 这里需要初始化PB10和PB11对应的ADC通道
    adc_init(KEY_ADC_CHANNEL_K1_K2_K3);
    adc_init(KEY_ADC_CHANNEL_K4_K5_K6);
}

// 对外接口：扫描所有按键，返回按下的键ID
KEY_ID key_scan(void)
{
    KEY_ID key;

    key = key_scan_pb10();
    if (key != KEY_NONE)
        return key;

    key = key_scan_pb11();
    if (key != KEY_NONE)
        return key + 3;   // PB11的键映射为K4/K5/K6

    return KEY_NONE;
}

// 获取原始ADC电压值（调试用）
void key_get_adc_mv(uint16_t *pb10_mv, uint16_t *pb11_mv)
{
    *pb10_mv = key_adc_mv[0];
    *pb11_mv = key_adc_mv[1];
}
