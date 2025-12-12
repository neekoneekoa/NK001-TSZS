#include "led.h"
#include <stdint.h>

/**************************************************************************
函 数 名:Led_Init
功能描述:初始化LED使用的IO口。
输入参数:None
输出参数:None
返 回 值:None。
其他说明:
        使用LED前，必须调用该函数初始化。
**************************************************************************/
void Led_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	  /* 初始化IO口的时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOB, ENABLE);
	
	  /* 初始化PC13 - 原有LED */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   //设置IO口推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //设置IO口速度
    GPIO_Init(GPIOC, &GPIO_InitStructure);              //初始化GPIO
    GPIO_SetBits(GPIOC, GPIO_Pin_13);                    //设置IO口输出高
    
    /* 初始化PB6用于示波器观察波形 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   //设置IO口推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //设置IO口速度
    GPIO_Init(GPIOB, &GPIO_InitStructure);              //初始化GPIO
    GPIO_ResetBits(GPIOB, GPIO_Pin_6);                  //初始化为低电平
    
    /* 配置PB15为推挽输出并置高 - 点亮LED */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   //设置IO口推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //设置IO口速度
    GPIO_Init(GPIOB, &GPIO_InitStructure);              //初始化GPIO
    GPIO_SetBits(GPIOB, GPIO_Pin_15);                    //设置IO口输出高电平
    
    /* 配置PB14为推挽输出并置低 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;   //设置IO口推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   //设置IO口速度
    GPIO_Init(GPIOB, &GPIO_InitStructure);              //初始化GPIO
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);                  //设置IO口输出低电平
    
    /* 配置PB13为输入模式 */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;      //设置IO口浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);              //初始化GPIO
}
/**************************************************************************
函 数 名:Led1_On
功能描述:使能LED1（PB1职高、PB14置低、PB13浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led1_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
    

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led2_On
功能描述:使能LED2（PB15置低、PB14置高、PB13浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led2_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led3_On
功能描述:使能LED3（PB13置高、PB15置低、PB14浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led3_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led4_On
功能描述:使能LED4（PB13置低、PB15置高、PB14浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led4_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led5_On
功能描述:使能LED5（PB13置高、PB14置低、PB15浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led6_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led6_On
功能描述:使能LED6（PB14置高、PB13置低、PB15浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led5_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led_Off
功能描述:关闭所有LED（PB13、PB14、PB15全部置低）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led_Off(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

typedef enum { LED_MODE_ON, LED_MODE_OFF, LED_MODE_LONG_OFF, LED_MODE_BREATH, LED_MODE_BLINK } led_mode_t;
static led_mode_t g_mode = LED_MODE_BREATH;               /**< 当前LED工作模式 */
static uint8_t  g_brightness = 50;                          /**< 目标亮度（0~100%） */
static uint8_t  g_brightness_cur = 0;                       /**< 当前实际亮度（0~100%） */
static uint16_t g_breath_time_ms = 2000;                    /**< 呼吸灯单次周期时长，单位ms */
static uint16_t g_blink_count = 0;                          /**< 闪烁次数（0表示按时间或无限） */
static uint16_t g_single_blink_time_ms = 300;               /**< 单次闪烁开/关时长，单位ms */
static uint32_t g_sustain_blink_time_ms = 0;                /**< 闪烁总持续时间，单位ms（0表示无限） */
static uint32_t g_acc_ms = 0;                               /**< 当前模式已运行累计时间，单位ms */
static uint16_t g_blink_done = 0;                         /**< 已完成的闪烁次数计数 */
static uint8_t  g_breath_up = 1;                            /**< 呼吸方向标志：1上升，0下降 */
static uint8_t  g_target_led = 1;                           /**< 目标LED编号（1~6） */
static uint8_t  g_pwm_cycle_ms = 1;                         /**< PWM周期时长，单位ms */
static uint8_t  g_pwm_phase_ms = 0;                       /**< 当前PWM周期内相位，单位ms */
static uint8_t  g_pwm_on_ms = 0;                            /**< 当前周期内PWM高电平持续时间，单位ms */
static uint16_t g_pwm_err_acc = 0;                          /**< PWM误差累积器，单位1/100 ms */

static void led_on_target(void);
static void led_off_target(void);

/**************************************************************************
函 数 名:Led_Mode_On
功能描述:设置LED为常亮模式，并按指定亮度输出。
输入参数:
@brightness:亮度，占空比0~100%
输出参数:None
返 回 值:None
其他说明:使用pwm_set_duty控制亮度，通道为LED1。
**************************************************************************/
void Led_Mode_On(uint8_t brightness)
{
    g_mode = LED_MODE_ON;
    g_brightness = brightness;
    g_brightness_cur = g_brightness;
}

/**************************************************************************
函 数 名:Led_Mode_Off
功能描述:设置LED为熄灭模式，关闭所有LED输出。
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led_Mode_Off(void)
{
    g_mode = LED_MODE_OFF;
}

/**************************************************************************
函 数 名:Led_Mode_LongOff
功能描述:设置LED为长灭模式，维持指定时间后自动恢复到呼吸模式。
输入参数:
@duration_ms:长灭持续时长，单位ms
输出参数:None
返 回 值:None
其他说明:长灭结束后恢复为当前亮度与呼吸周期的呼吸模式。
**************************************************************************/
void Led_Mode_LongOff(uint32_t duration_ms)
{
    g_mode = LED_MODE_LONG_OFF;
    g_sustain_blink_time_ms = duration_ms;
    g_acc_ms = 0;
    g_brightness_cur = 0;
}

/**************************************************************************
函 数 名:Led_Mode_Breath
功能描述:设置LED为呼吸模式，按指定亮度与周期上/下渐变。
输入参数:
@brightness:最大亮度，占空比0~100%
@breath_time_ms:单次呼吸周期时长，单位ms
输出参数:None
返 回 值:None
其他说明:内部以LED1加PWM实现亮度渐变。
**************************************************************************/
void Led_Mode_Breath(uint8_t brightness, uint16_t breath_time_ms)
{
    g_mode = LED_MODE_BREATH;
    g_brightness = brightness ? brightness : 1;                     // 确保亮度非0
    g_breath_time_ms = breath_time_ms ? breath_time_ms : 1000;      // 无值则为1000
    g_acc_ms = 0;
    g_breath_up = 1;
    g_brightness_cur = 0;
}

/**************************************************************************
函 数 名:Led_Mode_Blink
功能描述:设置LED为闪烁模式。支持按次数闪烁或按持续时间闪烁。
输入参数:
@brightness:亮度，占空比0~100%
@single_blink_time_ms:单次闪烁的开/关时长，单位ms
@blink_count:闪烁次数，>0按次数；=0按持续时间或无限
@sustain_blink_time_ms:持续闪烁总时长，单位ms；0表示无限
输出参数:None
返 回 值:None
其他说明:次数优先于持续时间；完成后自动恢复呼吸模式。
**************************************************************************/
void Led_Mode_Blink(uint8_t brightness, uint16_t single_blink_time_ms, uint16_t blink_count, uint32_t sustain_blink_time_ms)
{
    g_mode = LED_MODE_BLINK;
    g_brightness = brightness;
    g_single_blink_time_ms = single_blink_time_ms ? single_blink_time_ms : 200;
    g_blink_count = blink_count;
    g_sustain_blink_time_ms = sustain_blink_time_ms;
    g_acc_ms = 0;
    g_blink_done = 0;
    g_brightness_cur = 0;
}

/**************************************************************************
函 数 名:Led_Update_10ms
功能描述:LED状态机更新函数，需每10ms调用一次以推进灯效。
输入参数:None
输出参数:None
返 回 值:None
其他说明:在系统任务中以vTaskDelayUntil(10ms)周期调用。
**************************************************************************/
void Led_Update_10ms(void)
{
}

void Led_Update_1ms(void)
{
    switch(g_mode)
    {
        case LED_MODE_ON:
            g_brightness_cur = g_brightness;
            break;
        case LED_MODE_OFF:
            g_brightness_cur = 0;
            break;
        case LED_MODE_LONG_OFF:
            g_acc_ms += 1;
            if (g_sustain_blink_time_ms && g_acc_ms >= g_sustain_blink_time_ms) {
                Led_Mode_Breath(g_brightness, g_breath_time_ms);
            }
            g_brightness_cur = 0;
            break;
        case LED_MODE_BREATH:
        {
            static uint32_t phase_ms = 0;
            if (++phase_ms >= g_breath_time_ms) phase_ms = 0;
            // 使用正弦波生成更平滑的呼吸曲线，避免三角形波在边界的突变
            // 将phase_ms映射到0-65535的角度值
            uint16_t angle = (uint32_t)phase_ms * 65535 / (g_breath_time_ms ? g_breath_time_ms : 1);
            // 使用正弦函数计算亮度值，范围从0到255
            int16_t sin_val = 0;
            // 使用查表法计算正弦值，避免浮点数运算
            if (angle < 16384) {
                // 0°-90°: sin(angle) = angle * 255 / 16384
                sin_val = (int16_t)((uint32_t)angle * 255 / 16384);
            } else if (angle < 32768) {
                // 90°-180°: sin(angle) = 255 - (angle-16384) * 255 / 16384
                sin_val = 255 - (int16_t)((uint32_t)(angle - 16384) * 255 / 16384);
            } else if (angle < 49152) {
                // 180°-270°: sin(angle) = 0 - (angle-32768) * 255 / 16384
                sin_val = 0 - (int16_t)((uint32_t)(angle - 32768) * 255 / 16384);
            } else {
                // 270°-360°: sin(angle) = (angle-49152) * 255 / 16384 - 255
                sin_val = (int16_t)((uint32_t)(angle - 49152) * 255 / 16384) - 255;
            }
            // 取绝对值并转换为0-255范围
            uint16_t sin_abs = (sin_val < 0) ? (-sin_val) : sin_val;
            // 应用gamma校正，使亮度变化更符合人眼感知
            uint32_t gamma2 = (uint32_t)sin_abs * (uint32_t)sin_abs;
            gamma2 = (gamma2 + 255) / 255;
            // 计算最终亮度
            g_brightness_cur = (uint16_t)((gamma2 * g_brightness + 255) / 255);
            
            // 添加最小亮度阈值，跳过1%的极低亮度区域，避免闪烁
            // 计算最小亮度阈值（总亮度的5%）
            uint16_t min_brightness = (uint16_t)(g_brightness * 0.05);
            if (min_brightness < 1) min_brightness = 1; // 确保至少为1
            
            // 当亮度低于最小阈值时，直接设置为0，避免极低亮度闪烁
            if (g_brightness_cur > 0 && g_brightness_cur < min_brightness) {
                g_brightness_cur = 0;
            }
            //printf("sin_abs=%d\r\n",sin_abs);  //  
            break;
        }
        case LED_MODE_BLINK:
        {
            static uint8_t on_state = 0;
            g_acc_ms += 1;
            uint16_t t = g_single_blink_time_ms ? g_single_blink_time_ms : 200;
            if (g_blink_count) {
                if (g_acc_ms >= t) {
                    g_acc_ms = 0;
                    on_state = !on_state;
                    if (on_state) {
                        g_brightness_cur = g_brightness;
                    } else {
                        g_brightness_cur = 0;
                        g_blink_done++;
                        if (g_blink_done >= g_blink_count) {
                            g_blink_done = 0;
                            Led_Mode_Breath(g_brightness, g_breath_time_ms);
                        }
                    }
                }
            } else {
                uint32_t limit = g_sustain_blink_time_ms;
                if (g_acc_ms >= t) {
                    g_acc_ms = 0;
                    on_state = !on_state;
                    if (on_state) {
                        g_brightness_cur = g_brightness;
                    } else {
                        g_brightness_cur = 0;
                    }
                }
                static uint32_t run_ms = 0;
                run_ms += 1;
                if (limit && run_ms >= limit) {
                    run_ms = 0;
                    Led_Mode_Breath(g_brightness, g_breath_time_ms);
                }
            }
            break;
        }
    }

    //  亮度实现
    if (++g_pwm_phase_ms >= g_pwm_cycle_ms) {
        g_pwm_phase_ms = 0;
        uint16_t desired = (uint16_t)g_brightness_cur * g_pwm_cycle_ms; // in 1/100 ms
        desired += g_pwm_err_acc;
        g_pwm_on_ms = desired / 100;
        if (g_pwm_on_ms > g_pwm_cycle_ms) g_pwm_on_ms = g_pwm_cycle_ms;
        g_pwm_err_acc = desired % 100;
    }

    if (g_pwm_phase_ms < g_pwm_on_ms) {
        led_on_target();
    } else {
        led_off_target();
    }
}

static void led_on_target(void)
{
    /* 控制PB6输出高电平 */
    //GPIO_SetBits(GPIOB, GPIO_Pin_6);
    
    switch(g_target_led){
        case 1: Led1_On(); break;
        case 2: Led2_On(); break;
        case 3: Led3_On(); break;
        case 4: Led4_On(); break;
        case 5: Led5_On(); break;
        case 6: Led6_On(); break;
        default: Led1_On(); break;
    }
}

static void led_off_target(void)
{
    Led_Off();
}

void Led_Select(uint8_t led_id)
{
    g_target_led = led_id;
}
