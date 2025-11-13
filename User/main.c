#include "FreeRTOS.h"
#include "task.h"
#include "usart1.h"
#include "led.h"
#include "sysTick.h"
#include "oled.h"
#include "pms5003st.h"
#include <string.h>

const char testStr[] = {"123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop123456789zxcvbnmasdfghjklqwertyuiop\r\n"};
/**************************************************************************
函 数 名:MainTaskFunc
功能描述:主任务。
输入参数:
@pvParameters:系统传参
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void MainTaskFunc( void *pvParameters )
{
	OLED_Init();
//	PMS5003ST_Init(9600);					//初始化传感器
	OLED_ShowString(0, 0, "OLED Test", 16);
	OLED_ShowString(0, 20, "IIC Mode", 12);
	uint8_t *tmp = NULL;
	uint32_t len = 0;
	
	Usart1_SendData((const uint8_t *)testStr, strlen(testStr));
	while(1)
	{
		if( 0 == USART1_Read(&tmp, &len, portMAX_DELAY) ){
			Usart1_SendData(tmp, len);
			vPortFree(tmp);
		}
		
		// 添加LED闪烁指示系统运行
		LED0(LED_ON);
		vTaskDelay(200);
		LED0(LED_OFF);
		vTaskDelay(200);
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
int main(void)
{
    /* 串口接收将使用中断，所以要配置中断优先级的分组模式,此处配置为模式2,即2bit表示主优先级,2bit表示次优先级 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
	Clock_Init();												//使用内部晶振
	SysTick_Init();
    Usart1_Init(115200);                            			//初始化串口
//	printf("USART1 Init...\r\n");
	Led_Init();
	xTaskCreate( MainTaskFunc, "main", 128, NULL, 2, NULL );
	
	vTaskStartScheduler();
	return 0;
}
