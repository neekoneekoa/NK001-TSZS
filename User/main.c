#include "FreeRTOS.h"
#include "task.h"
#include "usart1.h"
#include "usart2.h"
#include "led.h"
#include "sysTick.h"
#include "oled.h"
#include "test.h"
#include "pms5003st.h"
#include "key.h"
#include "buzzer.h"
#include "../BSP/pwm.h"
#include "oled_menu.h"
#include <string.h>
#include "queue.h"
QueueHandle_t xKeyQueue = NULL;   /* 定义实体，只能有一次 */
const char testStr[] = {"123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop\r\n"};
volatile uint32_t g_uptime_seconds = 0;
typedef enum { LED_MODE_ON, LED_MODE_OFF, LED_MODE_LONG_OFF, LED_MODE_BREATH, LED_MODE_BLINK } LedMode;
typedef struct {
    LedMode mode;
    uint8_t brightness;
    uint16_t breath_time_ms;
    uint16_t blink_count;
    uint16_t single_blink_time_ms;
    uint32_t sustain_blink_time_ms;
} LedCommand;
QueueHandle_t xLedQueue = NULL;
/**************************************************************************
函 数 名:MainTaskFunc
功能描述:主任务。
输入参数:
@pvParameters:系统传参
输出参数:None
返 回 值:None
其他说明:主要任务是保证程序在运行，任务中包括：LED呼吸，进入休眠。
**************************************************************************/
static void MainTaskFunc( void *pvParameters )
{
    uint8_t *tmp = NULL;
    uint32_t len = 0;
    uint8_t key;
    
    // 初始化OLED并显示等待状态
    OLED_Init();
    USART1_ShowWaiting();
    
    // 初始化菜单系统
    menu_init();
    
	while(1)
    {
        // 检查串口1接收队列
        if( 0 == USART1_Read(&tmp, &len, 10) ){
            // 数据已经在USART1_Read内部被解析和处理
            vPortFree(tmp);
        }
        
        // 更新菜单显示
        menu_update();
        
        // 处理按键
        if (xQueueReceive(xKeyQueue, &key, 0) == pdTRUE) {
            menu_handle_key((KEY_ID)key);
        }

	}
}

/**************************************************************************
函 数 名:main
功能描述:工程功能调用接口,应用程序入口。
输入参数:None
输出参数:None
返 回 值:向系统环境返回应用运行结果。
其他说明:
中断优先级分组模式有4种模式:
     @arg NVIC_PriorityGroup_0: 0 bits for pre-emption priority 4 bits for subpriority
     @arg NVIC_PriorityGroup_1: 1 bits for pre-emption priority 3 bits for subpriority
     @arg NVIC_PriorityGroup_2: 2 bits for pre-emption priority 2 bits for subpriority
     @arg NVIC_PriorityGroup_3: 3 bits for pre-emption priority 1 bits for subpriority
     @arg NVIC_PriorityGroup_4: 4 bits for pre-emption priority 0 bits for subpriority
		 主优先级为:pre-emption priority,抢占优先级,可以抢占优先低的中断的CPU使用权,数值越小，优先级越高
     次优先级为:subpriority,在同一抢占优先级下的中断,次优先级数值越小先执行
**************************************************************************/
/**************************************************************************
函 数 名:KeyTaskFunc
功能描述:按键检测任务。
输入参数:
@pvParameters:系统传参
输出参数:None
返 回 值:None
其他说明:定期扫描按键状态，并在检测到按键时进行处理
**************************************************************************/
static void KeyTaskFunc( void *pvParameters )
{
    KEY_ID key_last = KEY_NONE;
    KEY_ID key_current;
    // 初始化按键
    key_init();
    //test_init();
    xKeyQueue = xQueueCreate(10, sizeof(uint8_t));  /* 创建队列 */
    if(xKeyQueue == NULL)
    {
        // 队列创建失败处理
    }
    while(1)
    {
        // 扫描按键
        key_current = key_scan();
        // 按键状态变化处理（只在按键按下时响应，实现按键消抖）
        if (key_current != KEY_NONE && key_current != key_last)
        {
            if (key_current != 0)
                xQueueSend(xKeyQueue, &key_current, 0);  // 抛给业务任务        
        }
        // 保存当前按键状态
        key_last = key_current;
        // 延时20ms进行按键消抖
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}

/**************************************************************************
函 数 名:TimeKeeperTaskFunc
功能描述:系统运行时间守护任务，每秒递增全局运行秒数计数器。
输入参数:
@pvParameters:系统传参，未使用
输出参数:None
返 回 值:None
其他说明:使用vTaskDelayUntil确保周期稳定为1秒，保证g_uptime_seconds精度。
**************************************************************************/
static void TimeKeeperTaskFunc( void *pvParameters )
{
    TickType_t last = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(1000);
    for( ;; )
    {
        vTaskDelayUntil(&last, period);
        g_uptime_seconds++;
    }
}

/**************************************************************************
函 数 名:LedTaskFunc
功能描述:LED控制任务，循环接收队列指令并执行对应LED模式。
输入参数:
@pvParameters:系统传参，未使用
输出参数:None
返 回 值:None
其他说明:初始化默认呼吸模式，周期1ms调用Led_Update_1ms以实现平滑效果。
**************************************************************************/
static void LedTaskFunc( void *pvParameters )
{
    Led_Mode_Breath(50, 4000);
    TickType_t last = xTaskGetTickCount();
    for(;;)
    {
        LedCommand cmd;
        if (xQueueReceive(xLedQueue, &cmd, 0) == pdTRUE) {
            switch(cmd.mode){
                case LED_MODE_ON: Led_Mode_On(cmd.brightness); break;
                case LED_MODE_OFF: Led_Mode_Off(); break;
                case LED_MODE_LONG_OFF: Led_Mode_LongOff(cmd.sustain_blink_time_ms); break;
                case LED_MODE_BREATH: Led_Mode_Breath(cmd.brightness, cmd.breath_time_ms); break;
                case LED_MODE_BLINK: Led_Mode_Blink(cmd.brightness, cmd.single_blink_time_ms, cmd.blink_count, cmd.sustain_blink_time_ms); break;
            }
        }
        vTaskDelayUntil(&last, pdMS_TO_TICKS(1));
        Led_Update_1ms();
        printf("1");  //  
    }
}

/**************************************************************************
函 数 名:LedRequestBreath
功能描述:向LED任务发送呼吸模式请求。
输入参数:
@brightness:呼吸亮度百分比(0~100)
@breath_time_ms:单次呼吸周期时长，单位毫秒
输出参数:None
返 回 值:None
其他说明:非阻塞发送，队列满时直接返回，不等待。
**************************************************************************/
void LedRequestBreath(uint8_t brightness, uint16_t breath_time_ms)
{
    LedCommand c;
    c.mode = LED_MODE_BREATH;
    c.brightness = brightness;
    c.breath_time_ms = breath_time_ms;
    c.blink_count = 0;
    c.single_blink_time_ms = 0;
    c.sustain_blink_time_ms = 0;
    if (xLedQueue) {
        xQueueSend(xLedQueue, &c, 0);
    }
}

int main(void)
{
    /* 配置中断优先级分组为4 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
	Clock_Init();				//使用内部晶振
	SysTick_Init();
    Usart1_Init(115200);                              		//初始化串口1
    Usart2_Init(115200);                              		//初始化串口2
    printf("HELLO NEEKO\r\n");						//测试串口打印
    Led_Init();                                      //初始化LED        
    pwm_init();                                      //初始化PWM        
    xLedQueue = xQueueCreate(10, sizeof(LedCommand));
    xTaskCreate( MainTaskFunc, "main", 128, NULL, 8, NULL ); // 降低主任务优先级
    xTaskCreate( KeyTaskFunc, "key", 128, NULL, 9, NULL ); // 按键任务优先级更高
    xTaskCreate( TimeKeeperTaskFunc, "time", 128, NULL, 10, NULL );
    xTaskCreate( LedTaskFunc, "led", 128, NULL, 7, NULL );
    vTaskStartScheduler();
    return 0;
}
