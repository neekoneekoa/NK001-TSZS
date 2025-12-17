#include "oled_menu.h"
#include "led.h"
#include "buzzer.h"
#include "../BSP/pwm.h"
#include "queue.h"
#include <stdio.h>
extern volatile uint32_t g_uptime_seconds;

// 声明按键队列
extern QueueHandle_t xKeyQueue;
extern void LedRequestBreath(uint8_t brightness, uint16_t breath_time_ms);

// 当前菜单变量
static MENU_TYPE current_menu = MENU_MAIN;

// 菜单回调函数数组
static MenuInitFunc menu_init_funcs[MENU_COUNT];
static MenuLoopFunc menu_loop_funcs[MENU_COUNT];
static MenuExitFunc menu_exit_funcs[MENU_COUNT];

// ---------------------------- 主界面 ----------------------------
static void main_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Main", 16);
    LedRequestBreath(50, 2000);
}

static void main_menu_loop(void) {
    static uint32_t last = 0;
    uint32_t sec = g_uptime_seconds;
    // 更新时间显示
    if (sec != last) {
        last = sec;
        uint32_t h = sec / 3600U;
        uint32_t m = (sec % 3600U) / 60U;
        uint32_t s = sec % 60U;
        char buf[12];
        sprintf(buf, "%02lu:%02lu:%02lu", (unsigned long)(h % 100U), (unsigned long)m, (unsigned long)s);
        OLED_ShowString(60, 0, buf, 16);
    }
}

static void main_menu_exit(void) {
    // 主界面退出不需要特殊处理
}

// ---------------------------- PWM界面 ----------------------------
static uint8_t current_pwm_channel = 1; // 当前选中的PWM通道

static void pwm_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "PWM", 16);
    OLED_ShowString(0, 16, "PA11:", 16);
    OLED_ShowString(0, 32, "PA6:", 16);
    OLED_ShowString(96, 16, ">", 16); // 显示当前选中通道
    // 开启两个PWM通道
    pwm_set_duty(1, pwm_get_duty(1));
    pwm_set_duty(2, pwm_get_duty(2));
}

static void pwm_menu_loop(void) {
    // 显示两个通道的占空比
    OLED_ShowNum(48, 16, pwm_get_duty(1), 3, 16);
    OLED_ShowString(78, 16, "%", 16);
    OLED_ShowNum(48, 32, pwm_get_duty(2), 3, 16);
    OLED_ShowString(78, 32, "%", 16);
}

static void pwm_menu_exit(void) {
    // 关闭两个PWM通道
    pwm_set_duty(1, 0);
    pwm_set_duty(2, 0);
}

// ---------------------------- ADC界面 ----------------------------
// ADC显示模式枚举
typedef enum {
    ADC_DISPLAY_RAW,  // 显示原始ADC值
    ADC_DISPLAY_VOLT  // 显示电压值
} ADC_DISPLAY_MODE;

static ADC_DISPLAY_MODE adc_display_mode = ADC_DISPLAY_RAW;  // 当前显示模式

static void adc_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "ADC Menu", 16);
    OLED_ShowString(0, 16, "PA0:", 16);
    OLED_ShowString(0, 32, "PA1:", 16);
    
    // 打开ADC电源
    adc_power_on();
    
    // 默认显示原始ADC值
    adc_display_mode = ADC_DISPLAY_RAW;
}

static void adc_menu_loop(void) {
    uint16_t pa0_raw, pa1_raw;
    uint16_t pa0_mv, pa1_mv;
    
    if (adc_display_mode == ADC_DISPLAY_RAW) {
        // 显示原始ADC值
        adc_get_pa_raw(&pa0_raw, &pa1_raw);
        OLED_ShowNum(48, 16, pa0_raw, 4, 16);
        OLED_ShowString(88, 16, "ADC", 16);
        OLED_ShowNum(48, 32, pa1_raw, 4, 16);
        OLED_ShowString(88, 32, "ADC", 16);
    } else {
        // 显示电压值（mV）
        adc_get_pa_mv(&pa0_mv, &pa1_mv);
        OLED_ShowNum(48, 16, pa0_mv, 4, 16);
        OLED_ShowString(88, 16, "mV", 16);
        OLED_ShowNum(48, 32, pa1_mv, 4, 16);
        OLED_ShowString(88, 32, "mV", 16);
    }
}

static void adc_menu_exit(void) {
    // 关闭ADC电源以节省功耗
    adc_power_off();
}

// ---------------------------- GPIO输入界面 ----------------------------
static void gpio_input_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "GPIO Input", 16);
    OLED_ShowString(0, 16, "Keys:", 16);
}

static void gpio_input_menu_loop(void) {
    // 显示当前按键状态
    KEY_ID key = key_scan();
    if (key != KEY_NONE) {
        OLED_ShowString(48, 16, "    ", 16); // 清除旧值
        OLED_ShowNum(48, 16, key, 1, 16);
    }
}

static void gpio_input_menu_exit(void) {
    // GPIO输入界面退出不需要特殊处理
}

// ---------------------------- GPIO输出界面 ----------------------------
static uint8_t gpio_output_state = 0;

static void gpio_output_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "GPIO Output", 16);
    OLED_ShowString(0, 16, "LED:", 16);
    gpio_output_state = 0;
    // 直接控制LED关闭，不重新初始化GPIO
    GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

static void gpio_output_menu_loop(void) {
    OLED_ShowString(48, 16, gpio_output_state ? "ON " : "OFF", 16);
}

static void gpio_output_menu_exit(void) {
    // 直接控制LED关闭，不重新初始化GPIO
    GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}

// ---------------------------- 串口接收界面 ----------------------------
static void uart_receive_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "UART Receive", 16);
    OLED_ShowString(0, 16, "Data:", 16);
    // 这里可以初始化串口接收缓冲区
}

static void uart_receive_menu_loop(void) {
    // 这里可以显示串口接收的数据
    // 示例：显示"Wait"表示等待数据
    OLED_ShowString(48, 16, "Wait", 16);
}

static void uart_receive_menu_exit(void) {
    // 串口接收界面退出不需要特殊处理
}

// ---------------------------- 自由调试界面 ----------------------------
static void free_debug_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Free Debug", 16);
    OLED_ShowString(0, 16, "Mode:", 16);
    // 自由调试模式下保持PWM开启
    //pwm_set_duty(pwm_get_duty();
}

static void free_debug_menu_loop(void) {
    OLED_ShowString(48, 16, "Active", 16);
}

static void free_debug_menu_exit(void) {
    // 自由调试界面退出时保持PWM状态
    // 如果需要关闭可以调用pwm_set_duty(0);
}

// ---------------------------- 菜单系统初始化 ----------------------------
void menu_init(void) {
    // 注册各界面的回调函数
    menu_init_funcs[MENU_MAIN] = main_menu_init;
    menu_loop_funcs[MENU_MAIN] = main_menu_loop;
    menu_exit_funcs[MENU_MAIN] = main_menu_exit;
    
    menu_init_funcs[MENU_PWM] = pwm_menu_init;
    menu_loop_funcs[MENU_PWM] = pwm_menu_loop;
    menu_exit_funcs[MENU_PWM] = pwm_menu_exit;
    
    menu_init_funcs[MENU_ADC] = adc_menu_init;
    menu_loop_funcs[MENU_ADC] = adc_menu_loop;
    menu_exit_funcs[MENU_ADC] = adc_menu_exit;
    
    menu_init_funcs[MENU_GPIO_INPUT] = gpio_input_menu_init;
    menu_loop_funcs[MENU_GPIO_INPUT] = gpio_input_menu_loop;
    menu_exit_funcs[MENU_GPIO_INPUT] = gpio_input_menu_exit;
    
    menu_init_funcs[MENU_GPIO_OUTPUT] = gpio_output_menu_init;
    menu_loop_funcs[MENU_GPIO_OUTPUT] = gpio_output_menu_loop;
    menu_exit_funcs[MENU_GPIO_OUTPUT] = gpio_output_menu_exit;
    
    menu_init_funcs[MENU_UART_RECEIVE] = uart_receive_menu_init;
    menu_loop_funcs[MENU_UART_RECEIVE] = uart_receive_menu_loop;
    menu_exit_funcs[MENU_UART_RECEIVE] = uart_receive_menu_exit;
    
    menu_init_funcs[MENU_FREE_DEBUG] = free_debug_menu_init;
    menu_loop_funcs[MENU_FREE_DEBUG] = free_debug_menu_loop;
    menu_exit_funcs[MENU_FREE_DEBUG] = free_debug_menu_exit;
    
    // 初始化主界面
    main_menu_init();
}

// ---------------------------- 菜单切换函数 ----------------------------
void menu_switch(MENU_TYPE menu) {
    if (menu >= MENU_COUNT) {
        return; // 无效的菜单类型
    }
    
    // 调用当前菜单的退出函数
    if (menu_exit_funcs[current_menu] != NULL) {
        menu_exit_funcs[current_menu]();
    }
    
    // 切换菜单
    current_menu = menu;
    
    // 调用新菜单的初始化函数
    if (menu_init_funcs[current_menu] != NULL) {
        menu_init_funcs[current_menu]();
    }
}

// ---------------------------- 菜单更新函数 ----------------------------
void menu_update(void) {
    // 调用当前菜单的循环函数
    if (menu_loop_funcs[current_menu] != NULL) {
        menu_loop_funcs[current_menu]();
    }
}

// ---------------------------- 按键处理函数 ----------------------------
void menu_handle_key(KEY_ID key) {
    switch (current_menu) {
        case MENU_MAIN:
            // 主界面按键处理
            switch (key) {
                case KEY_K1:
                    menu_switch(MENU_PWM); // 主菜单 -> PWM菜单
                    break;
                case KEY_K2:
                    menu_switch(MENU_ADC);
                    break;
                case KEY_K3:
                    menu_switch(MENU_GPIO_INPUT);
                    break;
                case KEY_K4:
                    menu_switch(MENU_GPIO_OUTPUT);
                    break;
                case KEY_K5:
                    menu_switch(MENU_UART_RECEIVE);
                    break;
                case KEY_K6:
                    menu_switch(MENU_FREE_DEBUG);
                    break;
                default:
                    break;
            }
            break;
            
        case MENU_PWM:
            // PWM界面按键处理
            switch (key) {
                case KEY_K1:
                    menu_switch(MENU_ADC); // PWM菜单 -> ADC菜单
                    break;
                case KEY_K2:
                    // 切换到PA11通道
                    current_pwm_channel = 1;
                    OLED_ShowString(96, 16, ">", 16); // 显示选中标记
                    OLED_ShowString(96, 32, " ", 16); // 清除另一个通道的选中标记
                    break;
                case KEY_K3:
                    // 切换到PA6通道
                    current_pwm_channel = 2;
                    OLED_ShowString(96, 32, ">", 16); // 显示选中标记
                    OLED_ShowString(96, 16, " ", 16); // 清除另一个通道的选中标记
                    break;
                case KEY_K5:
                    {   
                        uint8_t duty = pwm_get_duty(current_pwm_channel);
                        if (duty < 100) {
                            pwm_set_duty(current_pwm_channel, duty + 5);
                        }
                    }
                    break;
                case KEY_K6:
                    {   
                        uint8_t duty = pwm_get_duty(current_pwm_channel);
                        if (duty > 0) {
                            pwm_set_duty(current_pwm_channel, duty - 5);
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
            
        case MENU_ADC:
            // ADC界面按键处理
            switch (key) {
                case KEY_K1:
                    menu_switch(MENU_GPIO_INPUT); // ADC菜单 -> GPIO输入菜单
                    break;
                case KEY_K2:
                    // 切换显示模式
                    adc_display_mode = (adc_display_mode == ADC_DISPLAY_RAW) ? ADC_DISPLAY_VOLT : ADC_DISPLAY_RAW;
                    // 清屏并重新初始化显示
                    OLED_Clear();
                    OLED_ShowString(0, 0, "ADC Menu", 16);
                    OLED_ShowString(0, 16, "PA0:", 16);
                    OLED_ShowString(0, 32, "PA1:", 16);
                    break;
                default:
                    break;
            }
            break;
            
        case MENU_GPIO_INPUT:
            // GPIO输入界面按键处理
            if (key == KEY_K1) {
                menu_switch(MENU_GPIO_OUTPUT); // GPIO输入菜单 -> GPIO输出菜单
            }
            break;
            
        case MENU_GPIO_OUTPUT:
            // GPIO输出界面按键处理
            switch (key) {
                case KEY_K1:
                    menu_switch(MENU_UART_RECEIVE); // GPIO输出菜单 -> 串口接收菜单
                    break;
                case KEY_K2:
                    Led1_On();
                    gpio_output_state = 1;
                    break;
                case KEY_K3:
                    Led_Off();
                    gpio_output_state = 0;
                    break;
                default:
                    break;
            }
            break;
            
        case MENU_UART_RECEIVE:
            // 串口接收界面按键处理
            if (key == KEY_K1) {
                menu_switch(MENU_FREE_DEBUG); // 串口接收菜单 -> 自由调试菜单
            }
            break;
            
        case MENU_FREE_DEBUG:
            // 自由调试界面按键处理
            if (key == KEY_K1) {
                menu_switch(MENU_MAIN); // 自由调试菜单 -> 主菜单
            }
            break;
            
        default:
            break;
    }
}
