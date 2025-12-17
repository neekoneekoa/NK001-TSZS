#include "oled_menu.h"
#include "led.h"
#include "buzzer.h"
#include "../BSP/pwm.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "usart1.h"
#include "usart2.h"
#include "key.h"
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
    // 关闭灯光
    Led_Off();
    Led_Mode_Off();
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
    
    // 关闭灯光
    Led_Off();
    Led_Mode_Off();
    
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
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    // 配置PB2-PB7为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    OLED_Clear();
    OLED_ShowString(0, 0, "GPIO Input", 16);
    OLED_ShowString(0, 16, "PB2:", 16);
    OLED_ShowString(0, 32, "PB3:", 16);
    OLED_ShowString(0, 48, "PB4:", 16);
    OLED_ShowString(64, 16, "PB5:", 16);
    OLED_ShowString(64, 32, "PB6:", 16);
    OLED_ShowString(64, 48, "PB7:", 16);
    
    // 关闭灯光
    Led_Off();
    Led_Mode_Off();
}

static void gpio_input_menu_loop(void) {
    uint8_t pb2_state, pb3_state, pb4_state, pb5_state, pb6_state, pb7_state;
    
    // 读取GPIO状态
    pb2_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_2);
    pb3_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
    pb4_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
    pb5_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5);
    pb6_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6);
    pb7_state = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
    
    // 显示GPIO状态为1/0
    OLED_ShowString(32, 16, pb2_state ? "1" : "0", 16);
    OLED_ShowString(32, 32, pb3_state ? "1" : "0", 16);
    OLED_ShowString(32, 48, pb4_state ? "1" : "0", 16);
    OLED_ShowString(96, 16, pb5_state ? "1" : "0", 16);
    OLED_ShowString(96, 32, pb6_state ? "1" : "0", 16);
    OLED_ShowString(96, 48, pb7_state ? "1" : "0", 16);
}

static void gpio_input_menu_exit(void) {
    // GPIO输入界面退出不需要特殊处理
}

// ---------------------------- GPIO输出界面 ----------------------------
static uint8_t gpio_output_selected = 0;  // 当前选中的GPIO引脚
static uint8_t gpio_output_states[5] = {0};  // PA5,PB12,PC13,PA12,PA15的状态

static void gpio_output_menu_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    
    // 配置PA5, PA12, PA15为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置PB12为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 配置PC13为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 配置PA15为推挽输出（需要特殊处理，因为PA15默认是JTAG引脚）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 初始化所有GPIO为低电平
    GPIO_ResetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_15);
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    
    OLED_Clear();
    OLED_ShowString(0, 0, "GPIO Output", 16);
    OLED_ShowString(0, 16, "PA5:", 16);
    OLED_ShowString(0, 32, "PB12:", 16);
    OLED_ShowString(0, 48, "PC13:", 16);
    OLED_ShowString(64, 16, "PA12:", 16);
    OLED_ShowString(64, 32, "PA15:", 16);
    
    // 显示选中状态 - 在选中的数据后面添加O
    OLED_ShowString(56, 16, "O", 16);  // 默认选中PA5
    
    // 关闭灯光
    Led_Off();
    Led_Mode_Off();
}

static void gpio_output_menu_loop(void) {
    // 显示所有GPIO状态
    OLED_ShowString(48, 16, gpio_output_states[0] ? "1" : "0", 16);
    OLED_ShowString(48, 32, gpio_output_states[1] ? "1" : "0", 16);
    OLED_ShowString(48, 48, gpio_output_states[2] ? "1" : "0", 16);
    OLED_ShowString(112, 16, gpio_output_states[3] ? "1" : "0", 16);
    OLED_ShowString(112, 32, gpio_output_states[4] ? "1" : "0", 16);
    
    // 重新绘制选中标记
    OLED_ShowString(56, 16, " ", 16);
    OLED_ShowString(56, 32, " ", 16);
    OLED_ShowString(56, 48, " ", 16);
    OLED_ShowString(120, 16, " ", 16);
    OLED_ShowString(120, 32, " ", 16);
    
    switch(gpio_output_selected) {
        case 0: OLED_ShowString(56, 16, "O", 16); break;  // PA5
        case 1: OLED_ShowString(56, 32, "O", 16); break;  // PB12
        case 2: OLED_ShowString(56, 48, "O", 16); break;  // PC13
        case 3: OLED_ShowString(120, 16, "O", 16); break; // PA12
        case 4: OLED_ShowString(120, 32, "O", 16); break; // PA15
    }
}

static void gpio_output_menu_exit(void) {
    // 关闭所有GPIO输出
    GPIO_ResetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_12 | GPIO_Pin_15);
    GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
}

// ---------------------------- 串口接收界面 ----------------------------
static uint8_t *uart1_buffer = NULL;
static uint32_t uart1_len = 0;
static uint8_t *uart2_buffer = NULL;
static uint32_t uart2_len = 0;
static uint8_t uart_display_line = 0;

static void uart_receive_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "UART Receive", 16);
    OLED_ShowString(0, 16, "U1:", 16);
    OLED_ShowString(0, 32, "U2:", 16);
    OLED_ShowString(0, 48, "", 16);
    
    // 初始化缓冲区
    if (uart1_buffer) {
        vPortFree(uart1_buffer);
        uart1_buffer = NULL;
    }
    if (uart2_buffer) {
        vPortFree(uart2_buffer);
        uart2_buffer = NULL;
    }
    uart1_len = 0;
    uart2_len = 0;
    uart_display_line = 0;
    
    // 关闭灯光
    Led_Off();
    Led_Mode_Off();
}

static void uart_receive_menu_loop(void) {
    // 检查串口1数据
    if (0 == USART1_Read(&uart1_buffer, &uart1_len, 10)) {
        // 显示串口1数据（最多显示10个字符）
        uint8_t display_str[11] = {0};
        uint32_t display_len = uart1_len > 10 ? 10 : uart1_len;
        memcpy(display_str, uart1_buffer, display_len);
        OLED_ShowString(16, 16, (char *)display_str, 16);
        vPortFree(uart1_buffer);
        uart1_buffer = NULL;
        uart1_len = 0;
    }
    
    // 检查串口2数据
    if (0 == USART2_GetData(&uart2_buffer, &uart2_len, 10)) {
        // 显示串口2数据（最多显示10个字符）
        uint8_t display_str[11] = {0};
        uint32_t display_len = uart2_len > 10 ? 10 : uart2_len;
        memcpy(display_str, uart2_buffer, display_len);
        OLED_ShowString(16, 32, (char *)display_str, 16);
        vPortFree(uart2_buffer);
        uart2_buffer = NULL;
        uart2_len = 0;
    }
    
    // 按键处理已经移到menu_handle_key函数中统一管理
    // 这里只负责更新显示数据
}

static void uart_receive_menu_exit(void) {
    // 释放缓冲区
    if (uart1_buffer) {
        vPortFree(uart1_buffer);
        uart1_buffer = NULL;
    }
    if (uart2_buffer) {
        vPortFree(uart2_buffer);
        uart2_buffer = NULL;
    }
}

// ---------------------------- 自由调试界面 ----------------------------
static void free_debug_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Free Debug", 16);
    OLED_ShowString(0, 16, "Mode:", 16);
    // 关闭灯光
    Led_Off();
    Led_Mode_Off();
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
                    // 切换选中的GPIO状态
                    gpio_output_states[gpio_output_selected] = !gpio_output_states[gpio_output_selected];
                    
                    // 更新实际GPIO输出
                    switch(gpio_output_selected) {
                        case 0:  // PA5
                            if(gpio_output_states[0]) GPIO_SetBits(GPIOA, GPIO_Pin_5);
                            else GPIO_ResetBits(GPIOA, GPIO_Pin_5);
                            break;
                        case 1:  // PB12
                            if(gpio_output_states[1]) GPIO_SetBits(GPIOB, GPIO_Pin_12);
                            else GPIO_ResetBits(GPIOB, GPIO_Pin_12);
                            break;
                        case 2:  // PC13
                            if(gpio_output_states[2]) GPIO_SetBits(GPIOC, GPIO_Pin_13);
                            else GPIO_ResetBits(GPIOC, GPIO_Pin_13);
                            break;
                        case 3:  // PA12
                            if(gpio_output_states[3]) GPIO_SetBits(GPIOA, GPIO_Pin_12);
                            else GPIO_ResetBits(GPIOA, GPIO_Pin_12);
                            break;
                        case 4:  // PA15
                            if(gpio_output_states[4]) GPIO_SetBits(GPIOA, GPIO_Pin_15);
                            else GPIO_ResetBits(GPIOA, GPIO_Pin_15);
                            break;
                    }
                    break;
                case KEY_K3:
                    // 向下切换引脚
                    gpio_output_selected = (gpio_output_selected + 1) % 5;
                    break;
                case KEY_K4:
                    // 向上切换引脚
                    gpio_output_selected = (gpio_output_selected + 4) % 5;
                    break;
                default:
                    break;
            }
            break;
            
        case MENU_UART_RECEIVE:
            // 串口接收界面按键处理
            switch (key) {
                case KEY_K1:
                    menu_switch(MENU_FREE_DEBUG); // 串口接收菜单 -> 自由调试菜单
                    break;
                case KEY_K3:
                    // 翻页处理
                    uart_display_line = (uart_display_line + 1) % 2;
                    OLED_Clear();
                    OLED_ShowString(0, 0, "UART Receive", 16);
                    
                    if (uart_display_line == 0) {
                        OLED_ShowString(0, 16, "U1:", 16);
                        OLED_ShowString(0, 32, "U2:", 16);
                    } else {
                        OLED_ShowString(0, 16, "U1:", 16);
                        OLED_ShowString(0, 32, "U2:", 16);
                    }
                    break;
                default:
                    break;
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
