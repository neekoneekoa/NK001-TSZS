#include "oled_menu.h"
#include "led.h"
#include "buzzer.h"
#include "../BSP/pwm.h"
#include "queue.h"

// 声明按键队列
extern QueueHandle_t xKeyQueue;

// 当前菜单变量
static MENU_TYPE current_menu = MENU_MAIN;

// 菜单回调函数数组
static MenuInitFunc menu_init_funcs[MENU_COUNT];
static MenuLoopFunc menu_loop_funcs[MENU_COUNT];
static MenuExitFunc menu_exit_funcs[MENU_COUNT];

// ---------------------------- 主界面 ----------------------------
static void main_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "Main Menu", 16);
    OLED_ShowString(0, 16, "1:PWM", 16);
    OLED_ShowString(0, 32, "2:ADC", 16);
    OLED_ShowString(0, 48, "3:GPIO", 16);
}

static void main_menu_loop(void) {
    // 主界面不需要持续更新
}

static void main_menu_exit(void) {
    // 主界面退出不需要特殊处理
}

// ---------------------------- PWM界面 ----------------------------
static void pwm_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "PWM Menu", 16);
    OLED_ShowString(0, 16, "Duty:", 16);
    pwm_set_duty(pwm_get_duty()); // 开启PWM
}

static void pwm_menu_loop(void) {
    OLED_ShowNum(48, 16, pwm_get_duty(), 3, 16);
    OLED_ShowString(78, 16, "%", 16);
}

static void pwm_menu_exit(void) {
    pwm_set_duty(0); // 关闭PWM
}

// ---------------------------- ADC界面 ----------------------------
static void adc_menu_init(void) {
    OLED_Clear();
    OLED_ShowString(0, 0, "ADC Menu", 16);
    OLED_ShowString(0, 16, "PB0:", 16);
    OLED_ShowString(0, 32, "PB1:", 16);
}

static void adc_menu_loop(void) {
    uint16_t pb0_mv, pb1_mv;
    key_get_adc_mv(&pb0_mv, &pb1_mv);
    OLED_ShowNum(48, 16, pb0_mv, 4, 16);
    OLED_ShowString(88, 16, "mV", 16);
    OLED_ShowNum(48, 32, pb1_mv, 4, 16);
    OLED_ShowString(88, 32, "mV", 16);
}

static void adc_menu_exit(void) {
    // ADC界面退出不需要特殊处理
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
    Led_Off();
}

static void gpio_output_menu_loop(void) {
    OLED_ShowString(48, 16, gpio_output_state ? "ON " : "OFF", 16);
}

static void gpio_output_menu_exit(void) {
    Led_Off(); // 关闭LED
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
    pwm_set_duty(pwm_get_duty());
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
                    menu_switch(MENU_PWM);
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
                    menu_switch(MENU_MAIN);
                    break;
                case KEY_K5:
                    {   
                        uint8_t duty = pwm_get_duty();
                        if (duty < 100) {
                            pwm_set_duty(duty + 5);
                        }
                    }
                    break;
                case KEY_K6:
                    {   
                        uint8_t duty = pwm_get_duty();
                        if (duty > 0) {
                            pwm_set_duty(duty - 5);
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
            
        case MENU_ADC:
            // ADC界面按键处理
            if (key == KEY_K1) {
                menu_switch(MENU_MAIN);
            }
            break;
            
        case MENU_GPIO_INPUT:
            // GPIO输入界面按键处理
            if (key == KEY_K1) {
                menu_switch(MENU_MAIN);
            }
            break;
            
        case MENU_GPIO_OUTPUT:
            // GPIO输出界面按键处理
            switch (key) {
                case KEY_K1:
                    menu_switch(MENU_MAIN);
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
                menu_switch(MENU_MAIN);
            }
            break;
            
        case MENU_FREE_DEBUG:
            // 自由调试界面按键处理
            if (key == KEY_K1) {
                menu_switch(MENU_MAIN);
            }
            break;
            
        default:
            break;
    }
}