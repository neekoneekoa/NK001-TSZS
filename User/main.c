#include "FreeRTOS.h"
#include "task.h"
#include "usart1.h"
#include "led.h"
#include "sysTick.h"
#include "oled.h"
#include "test.h"
#include "pms5003st.h"
#include "key.h"
#include "buzzer.h"
#include <string.h>
#include "queue.h"
QueueHandle_t xKeyQueue = NULL;   /* 定义实体，只能有一次 */
const char testStr[] = {"123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop\r\n"};
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
    
    // 初始化OLED并显示等待状态
    OLED_Init();
    USART1_ShowWaiting();
    
	while(1)
    {
        // 检查串口1接收队列
        if( 0 == USART1_Read(&tmp, &len, 10) ){
            // 数据已经在USART1_Read内部被解析和处理
            vPortFree(tmp);
        }
        
        // 添加LED闪烁指示系统运行
        test_LED();
        //buzzer_test();
        oled_test();
        test_KEY();

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

int main(void)
{
    /* 配置中断优先级分组为4 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
	Clock_Init();						//使用内部晶振
	SysTick_Init();
    Usart1_Init(115200);                             			//初始化串口
	printf("HELLO NEEKO\r\n");						//测试串口打印
	Led_Init();                                      //初始化LED        
	xTaskCreate( MainTaskFunc, "main", 128, NULL, 8, NULL ); // 降低主任务优先级
	xTaskCreate( KeyTaskFunc, "key", 128, NULL, 9, NULL ); // 按键任务优先级更高
	vTaskStartScheduler();
	return 0;
}
