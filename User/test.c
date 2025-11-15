#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "buzzer.h"
#include "test.h"
#include "oled.h"
#include "key.h"
#include "queue.h"
#include "usart1.h"
static uint8_t key1;
extern QueueHandle_t xKeyQueue;   /* 告诉编译器实体在别处 */

// 定义LED延时常量
#define LED_DELAY_MS 200

void test_init(void){
    
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

// 串口数据接收缓冲区
#define UART_DATA_BUFFER_SIZE 64
static uint8_t uart_data_buffer[UART_DATA_BUFFER_SIZE];
static uint8_t buffer_index = 0;
static uint8_t is_recording = 0;

/**************************************************************************
函 数 名:test_UART1_Receive
功能描述:测试串口1接收数据，当遇到0X55时开始记录并显示在OLED上
输入参数:None
输出参数:None
返 回 值:uint8_t
其他说明:每100ms检查一次串口数据
**************************************************************************/
uint8_t test_UART1_Receive(void){
    static uint32_t last_check_time = 0;
    uint8_t *rx_data = NULL;
    uint32_t rx_len = 0;
    uint32_t current_time = xTaskGetTickCount();
    
    // 每100ms检查一次串口数据
    if (current_time - last_check_time < 100)
        return 1;
    
    last_check_time = current_time;
    
    // 从串口1读取数据
    if (USART1_Read(&rx_data, &rx_len, 10) == 0 && rx_data != NULL && rx_len > 0) {
        // 处理接收到的数据
        for (uint32_t i = 0; i < rx_len; i++) {
            uint8_t data = rx_data[i];
            
            // 检查是否遇到0X55
            if (data == 0x55) {
                is_recording = 1;
                buffer_index = 0;
                // 清空OLED屏幕
                OLED_Clear();
                // 显示开始记录的提示
                OLED_ShowString(0, 0, "Rec Start: 55", 16);
            }
            
            // 如果正在记录状态
            if (is_recording) {
                // 保存数据到缓冲区
                if (buffer_index < UART_DATA_BUFFER_SIZE - 1) {
                    uart_data_buffer[buffer_index++] = data;
                    uart_data_buffer[buffer_index] = '\0'; // 添加结束符
                }
                
                // 显示当前接收的字节（以十六进制形式）
                static char hex_str[4];
                hex_str[0] = '0';
                hex_str[1] = 'x';
                hex_str[2] = (data >> 4) < 10 ? (data >> 4) + '0' : (data >> 4) - 10 + 'A';
                hex_str[3] = (data & 0x0F) < 10 ? (data & 0x0F) + '0' : (data & 0x0F) - 10 + 'A';
                
                // 在OLED上显示接收的数据（限制显示行数，避免刷屏过快）
                if (buffer_index <= 8) {
                    OLED_ShowString(0, buffer_index * 16, hex_str, 16);
                } else if (buffer_index == 9) {
                    OLED_ShowString(0, 0, "Data Overflow", 16);
                }
            }
        }
        
        // 释放内存
        if (rx_data) {
            vPortFree(rx_data);
        }
    }
    
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
    //OLED_ShowNum(0, 16, 0, 1, 16);
    return 1;
}

uint8_t test_KEY(void){
        uint16_t pb0_mv, pb1_mv;
        // 按键按下处理
        if (xQueueReceive(xKeyQueue, &key1, 0) == pdTRUE)
        {
            OLED_ShowNum(0, 48, 1, 1, 16);
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
        static uint16_t last_pb0_mv = 0xFFFF, last_pb1_mv = 0xFFFF;
        key_get_adc_mv(&pb0_mv, &pb1_mv);
        // 当PB0 ADC值变化时才刷新显示
        if (pb0_mv != last_pb0_mv)
        {
            OLED_ShowString(0, 32, "PB0:", 16);
            OLED_ShowNum(32, 32, pb0_mv, 4, 16);
            last_pb0_mv = pb0_mv;
        }
        // 当PB1 ADC值变化时才刷新显示
        if (pb1_mv != last_pb1_mv)
        {
            OLED_ShowString(0, 48, "PB1:", 16);
            OLED_ShowNum(32, 48, pb1_mv, 4, 16);
            last_pb1_mv = pb1_mv;
        }
        // key_get_adc_mv(&pb0_mv, &pb1_mv);
        // // 在不同位置显示PB0和PB1的ADC值，避免重叠
        // OLED_ShowString(0, 32, "PB0:", 16);
        // OLED_ShowNum(32, 32, pb0_mv, 4, 16);
        // OLED_ShowString(0, 48, "PB1:", 16);
        // OLED_ShowNum(32, 48, pb1_mv, 4, 16);
        return 1;
}