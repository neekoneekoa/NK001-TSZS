#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "buzzer.h"
#include "test.h"
#include "oled.h"
#include "key.h"
#include "queue.h"
#include "../BSP/pwm.h"
static uint8_t key1;
extern QueueHandle_t xKeyQueue;   /* 告诉编译器实体在别处 */

// 定义界面类型
typedef enum {
    INTERFACE_PWM,
    INTERFACE_LED
} INTERFACE_TYPE;

// 当前界面变量
static INTERFACE_TYPE current_interface = INTERFACE_PWM;

// 定义LED延时常量
#define LED_DELAY_MS 200

void test_init(void){
    // 根据初始界面设置PWM状态
    if (current_interface != INTERFACE_PWM) {
        pwm_set_duty(0); // 初始界面不是PWM，关闭PWM
    }
}

// 使用状态机实现流水灯，避免长时间阻塞任务
static int8_t led_state = 0;

// 使用绝对时间戳
static uint32_t next_update_tick = 0;

typedef enum {
    LED_OFF_ALL,
    LED_ON_1,
    LED_ON_2,
    LED_ON_3,
    LED_ON_4,
    LED_ON_5,
    LED_ON_6
} LED_STATE;

typedef struct {
    LED_STATE state;
    void (*action)(void);
    LED_STATE next_state;
} LED_StateMachine;



uint8_t test_LED(void){
    // uint32_t current_tick = xTaskGetTickCount();    // 获取当前时间戳
    
    // // 初始化下一次更新时间（如果是第一次运行）
    // if (next_update_tick == 0) {
    //     next_update_tick = current_tick + LED_DELAY_MS;
    // }
    
    // // 使用绝对时间戳判断是否需要更新状态
    // if (current_tick >= next_update_tick) {
    //     // 计算下一次更新时间（确保连续性）
    //     next_update_tick += LED_DELAY_MS;
        
    //     // 处理可能的时间溢出
    //     if (next_update_tick < current_tick) {
    //         next_update_tick = current_tick + LED_DELAY_MS;
    //     }
        
        
        // 根据当前状态打开对应的LED
        switch(led_state) {
            case 0:
                Led1_On();
                break;
            case 1:
                Led2_On();
                break;
            case 2:
                Led3_On();
                break;
            case 3:
                Led4_On();
                break;
            case 4:
                Led5_On();
                break;
            case 5:
                Led6_On();
                break;
            default:
                led_state = 0; // 重置状态
                return 1;
        }
        vTaskDelay(200);  
        led_state = (led_state + 1) % 6; // 循环6个状态
    // }
    
    return 1;
}
//  1000-1800 测试蜂鸣器频率
uint8_t buzzer_test(void){
    // 对于9042一体无源蜂鸣器，先确保使用推荐的频率
    buzzer_on();    
    vTaskDelay(200);  // 1秒，比原来的300ms更长，更容易听到
    // 关闭蜂鸣器
    buzzer_off();
    return 1;
}

uint8_t oled_test(void){
    // 根据当前界面显示不同内容
    switch(current_interface) {
        case INTERFACE_PWM:
            // PWM界面
            OLED_ShowString(0, 0, "PWM", 16);  // 界面名称
            OLED_ShowString(0, 16, "PWM Duty:", 16);
            OLED_ShowNum(80, 16, pwm_get_duty(), 3, 16);
            OLED_ShowString(110, 16, "%", 16);
            break;
        case INTERFACE_LED:
            // LED界面
            OLED_ShowString(0, 0, "LED", 16);   // 界面名称
            OLED_ShowString(0, 16, "LED State:", 16);
            OLED_ShowNum(80, 16, led_state, 1, 16);
            break;
    }
    return 1;
}

uint8_t test_KEY(void){
        uint16_t pb0_mv, pb1_mv;
        uint8_t current_duty;
        // 按键按下处理
        if (xQueueReceive(xKeyQueue, &key1, 0) == pdTRUE)
        {
            OLED_ShowNum(0, 48, key1, 1, 16);
            
            // 处理界面切换
            if (key1 == KEY_K1) {
                // 切换界面
                INTERFACE_TYPE previous_interface = current_interface;
                current_interface = (current_interface + 1) % 2;
                // 清屏
                OLED_Clear();
                
                // 根据界面切换控制PWM
                if (current_interface == INTERFACE_PWM) {
                    // 进入PWM界面，开启PWM
                    pwm_set_duty(pwm_get_duty()); // 设置为当前占空比
                } else {
                    // 离开PWM界面，关闭PWM
                    pwm_set_duty(0);
                }
            } else {
                // 根据当前界面处理其他按键
                switch(current_interface) {
                    case INTERFACE_PWM:
                        // PWM界面：调节占空比
                        current_duty = pwm_get_duty();
                        switch(key1)
                        {
                            case KEY_K5: // 增加占空比
                                if (current_duty < 100)
                                {
                                    current_duty += 5;
                                    pwm_set_duty(current_duty);
                                }
                                break;
                            case KEY_K6: // 减少占空比
                                if (current_duty > 0)
                                {
                                    current_duty -= 5;
                                    pwm_set_duty(current_duty);
                                }
                                break;
                            default:
                                break;
                        }
                        break;
                    case INTERFACE_LED:
                        // LED界面：控制LED
                        switch(key1)
                        {
                            case KEY_K2: // 点亮LED
                                Led1_On();
                                break;
                            case KEY_K3: // 熄灭LED
                                Led_Off();
                                break;
                            default:
                                break;
                        }
                        break;
                }
            }
            
            // // 按键按下时蜂鸣器响一下
            // buzzer_on();
            // vTaskDelay(100 / portTICK_PERIOD_MS);
            // buzzer_off();
        }
            // switch(key_current)
            // {
            //     case KEY_K1:
            //         OLED_ShowNum(0, 48, 1, 6, 16);
            //         break;
            //     case KEY_K2:
            //         OLED_ShowNum(0, 48, 2, 6, 16);   
            //         break;
            //     case KEY_K3:
            //         OLED_ShowNum(0, 48, 3, 6, 16);
            //         break;
            //     case KEY_K4:
            //         OLED_ShowNum(0, 48, 4, 6, 16);
            //         break;
            //     case KEY_K5:
            //         OLED_ShowNum(0, 48, 5, 6, 16);
            //         break;
            //     case KEY_K6:
            //         OLED_ShowNum(0, 48, 6, 6, 16);
            //         break;
            //     default:
            //         break;
            // }
        // static uint16_t last_pb0_mv = 0xFFFF, last_pb1_mv = 0xFFFF;
        // key_get_adc_mv(&pb0_mv, &pb1_mv);
        // // 当PB0 ADC值变化时才刷新显示
        // if (pb0_mv != last_pb0_mv)
        // {
        //     OLED_ShowString(0, 32, "PB0:", 16);
        //     OLED_ShowNum(32, 32, pb0_mv, 4, 16);
        //     last_pb0_mv = pb0_mv;
        // }
        // // 当PB1 ADC值变化时才刷新显示
        // if (pb1_mv != last_pb1_mv)
        // {
        //     OLED_ShowString(0, 48, "PB1:", 16);
        //     OLED_ShowNum(32, 48, pb1_mv, 4, 16);
        //     last_pb1_mv = pb1_mv;
        // }
        // key_get_adc_mv(&pb0_mv, &pb1_mv);
        // // 在不同位置显示PB0和PB1的ADC值，避免重叠
        // OLED_ShowString(0, 32, "PB0:", 16);
        // OLED_ShowNum(32, 32, pb0_mv, 4, 16);
        // OLED_ShowString(0, 48, "PB1:", 16);
        // OLED_ShowNum(32, 48, pb1_mv, 4, 16);
        return 1;
}