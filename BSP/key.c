#include "key.h"
#include "stm32f10x_adc.h"
#include "stm32f10x.h"  // 根据你的硬件平台选择合适的头文件
#include "stm32f10x_gpio.h"
#include "stm32f10x_dma.h"
#include <stdint.h>

#define KEY_ADC_CHANNEL_K1_K2_K3    ADC_Channel_8    // PB0
#define KEY_ADC_CHANNEL_K4_K5_K6    ADC_Channel_9    // PB1
#define ADC_CHANNEL_PA0              ADC_Channel_0    // PA0
#define ADC_CHANNEL_PA1              ADC_Channel_1    // PA1

#define KEY_NONE_PRESS_MV           5000    // 无按键时接近5V
#define KEY3_PRESS_MV               1650    // 3个二极管压降≈0.7V*3
#define KEY2_PRESS_MV               1100    // 2个二极管压降≈0.7V*2
#define KEY1_PRESS_MV               550    // 1个二极管压降≈0.7V*1

#define ADC_MV_MAX                  5000
#define ADC_MV_MIN                  0


// 定义DMA传输缓冲区
#define ADC_CHANNEL_COUNT 4  // 4个ADC通道：PB0, PB1, PA0, PA1
static uint16_t adc_dma_buffer[ADC_CHANNEL_COUNT];  // DMA传输目标缓冲区

// 初始化 ADC 和 GPIO
void adc_init(void) {
    // 使能 ADC 时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    // 使能 GPIO 时钟（PB用于按键，PA用于额外ADC）
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
    // 使能DMA1时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
    // 配置 PB0 和 PB1 为模拟输入模式（按键）
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 配置 PA0 和 PA1 为模拟输入模式（额外ADC）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置 ADC
    ADC_InitTypeDef ADC_InitStructure;
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = ENABLE;  // 启用扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;  // 启用连续转换模式
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;  // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 右对齐
    ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_COUNT;  // 4个通道
    ADC_Init(ADC1, &ADC_InitStructure);

    // 配置ADC通道转换顺序和采样时间
    ADC_RegularChannelConfig(ADC1, KEY_ADC_CHANNEL_K1_K2_K3, 1, ADC_SampleTime_239Cycles5);  // PB0 (K1/K2/K3)
    ADC_RegularChannelConfig(ADC1, KEY_ADC_CHANNEL_K4_K5_K6, 2, ADC_SampleTime_239Cycles5);  // PB1 (K4/K5/K6)
    ADC_RegularChannelConfig(ADC1, ADC_CHANNEL_PA0, 3, ADC_SampleTime_239Cycles5);  // PA0
    ADC_RegularChannelConfig(ADC1, ADC_CHANNEL_PA1, 4, ADC_SampleTime_239Cycles5);  // PA1

    // 配置DMA
    DMA_InitTypeDef DMA_InitStructure;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;  // ADC1数据寄存器地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adc_dma_buffer;  // 内存缓冲区地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  // 外设到内存
    DMA_InitStructure.DMA_BufferSize = ADC_CHANNEL_COUNT;  // 传输数量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // 外设数据大小：半字(16位)
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  // 内存数据大小：半字(16位)
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  // 高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  // 禁用内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  // ADC1对应DMA1通道1

    // 使能DMA
    DMA_Cmd(DMA1_Channel1, ENABLE);

    // 使能ADC的DMA请求
    ADC_DMACmd(ADC1, ENABLE);

    // 使能 ADC
    ADC_Cmd(ADC1, ENABLE);

    // 启动 ADC 校准
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));

    // 开始ADC连续转换
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

// 获取指定通道的 ADC 值（从DMA缓冲区读取）
uint32_t adc_get_value(uint32_t channel) {
    // 根据通道号返回DMA缓冲区中对应的值
    if (channel == KEY_ADC_CHANNEL_K1_K2_K3) {
        return adc_dma_buffer[0];  // PB0
    } else if (channel == KEY_ADC_CHANNEL_K4_K5_K6) {
        return adc_dma_buffer[1];  // PB1
    } else if (channel == ADC_CHANNEL_PA0) {
        return adc_dma_buffer[2];  // PA0
    } else if (channel == ADC_CHANNEL_PA1) {
        return adc_dma_buffer[3];  // PA1
    } else {
        return 0;
    }
}

static uint16_t key_adc_mv[2];   // 0:PB0  1:PB1
static uint16_t pa_adc_mv[2];    // 0:PA0  1:PA1

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

// 获取PA0和PA1的ADC电压值
void adc_get_pa_mv(uint16_t *pa0_mv, uint16_t *pa1_mv)
{
    // 从DMA缓冲区读取PA0的原始值
    pa_adc_mv[0] = adc_to_mv(adc_dma_buffer[2]);
    
    // 从DMA缓冲区读取PA1的原始值
    pa_adc_mv[1] = adc_to_mv(adc_dma_buffer[3]);
    
    *pa0_mv = pa_adc_mv[0];
    *pa1_mv = pa_adc_mv[1];
}

// 获取PA0和PA1的ADC原始值
void adc_get_pa_raw(uint16_t *pa0_raw, uint16_t *pa1_raw)
{
    // 从DMA缓冲区直接读取PA0的ADC原始值
    *pa0_raw = adc_dma_buffer[2];
    
    // 从DMA缓冲区直接读取PA1的ADC原始值
    *pa1_raw = adc_dma_buffer[3];
}

// PA0和PA1 ADC采集控制（不需要关闭整个ADC，因为按键也需要使用ADC1）
// 打开PA0和PA1的ADC采集（实际上只需要在需要时读取即可）
void adc_power_on(void)
{
    // 由于ADC1已经在按键初始化时打开，这里不需要额外操作
    // 只需要确保PA0和PA1的GPIO已经配置为模拟输入（在adc_init中已完成）
}

// 关闭PA0和PA1的ADC采集（实际上只需要停止读取即可）
void adc_power_off(void)
{
    // 由于ADC1需要用于按键扫描，不能关闭
    // 只需要在非ADC界面时停止读取PA0和PA1即可
}
