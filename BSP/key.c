#include "key.h"
#include "stm32f10x_adc.h"
#include "stm32f10x.h"  // 根据你的硬件平台选择合适的头文件
#include "stm32f10x_gpio.h"
#include <stdint.h>

#define KEY_ADC_CHANNEL_K1_K2_K3    ADC_Channel_8    // PB0
#define KEY_ADC_CHANNEL_K4_K5_K6    ADC_Channel_9    // PB1

#define KEY_NONE_PRESS_MV           5000    // 无按键时接近5V
#define KEY3_PRESS_MV               1650    // 3个二极管压降≈0.7V*3
#define KEY2_PRESS_MV               1100    // 2个二极管压降≈0.7V*2
#define KEY1_PRESS_MV               550    // 1个二极管压降≈0.7V*1

#define ADC_MV_MAX                  5000
#define ADC_MV_MIN                  0


// 初始化 ADC 和 GPIO
void adc_init(void) {
    // 使能 ADC 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // 使能 GPIO 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 配置 PB0 和 PB1 为模拟输入模式
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置 ADC
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    // 使能 ADC
    ADC_Cmd(ADC1, ENABLE);

    // 启动 ADC 校准
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}

// 获取指定通道的 ADC 值
uint32_t adc_get_value(uint32_t channel) {
    // 配置当前要读取的通道
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_239Cycles5);
    
    // 清除转换完成标志
    ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
    
    // 启动 ADC 软件转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
    // 等待 ADC 转换完成
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    // 读取 ADC 值
    return ADC_GetConversionValue(ADC1);
}

static uint16_t key_adc_mv[2];   // 0:PB0  1:PB1

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

// 扫描PB0对应的K1/K3/K5
static KEY_ID key_scan_pb10(void)
{
    uint16_t adc_raw;
    adc_raw = adc_get_value(KEY_ADC_CHANNEL_K1_K2_K3);
    key_adc_mv[0] = adc_to_mv(adc_raw);
    return key_decode_mv(key_adc_mv[0]);
}

// 扫描PB1对应的K2/K4/K6
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
    // 初始化ADC和GPIO
    adc_init();
    
    // 初始化电压值数组
    key_adc_mv[0] = 0;
    key_adc_mv[1] = 0;
}

// 对外接口：扫描所有按键，返回按下的键ID
KEY_ID key_scan(void)
{
    KEY_ID key;

    key = key_scan_pb10();
    if (key != KEY_NONE)
        return (key*2-1);

    key = key_scan_pb11();
    if (key != KEY_NONE)
        return (key*2);   

    return KEY_NONE;
}

// 获取原始ADC电压值（调试用）
void key_get_adc_mv(uint16_t *pb0_mv, uint16_t *pb1_mv)
{
    *pb0_mv = key_adc_mv[0];
    *pb1_mv = key_adc_mv[1];
}
