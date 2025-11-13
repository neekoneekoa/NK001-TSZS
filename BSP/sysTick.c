/***************************************************************************************
*Copyright (c) 2019 - ,
*文 件 名:systick.c
*作    者:赵光鑫
*联系方式:zhao.guangxin@qq.com
*创建日期:2019-9-7
*说    明:				
*修    改:

***************************************************************************************/
#include "sysTick.h"


/**************************************************************************
函 数 名:SysTick_Init
功能描述:初始化systick。
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void SysTick_Init(void)
{
	uint32_t prioritygroup = 0x00U;
	
    SysTick_Config(SystemCoreClock/1000UL);    
    prioritygroup = NVIC_GetPriorityGrouping();
	
	NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(prioritygroup, 15, 0));	//初始化sysTick中断，优先级15
}

/**************************************************************************
函 数 名:Clock_Init
功能描述:初始化时钟。
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Clock_Init(void)
{
#if 1
	RCC_DeInit();													//将外设 RCC寄存器重设为缺省值
	RCC_HSICmd(ENABLE);												//使能HSI
	while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);				//等待HSI使能成功
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12);			//设置 PLL 时钟源及倍频系数
	RCC_PLLCmd(ENABLE);												//如果PLL被用于系统时钟,那么它不能被失能											
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);				//等待指定的 RCC 标志位设置成功 等待PLL初始化成功
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);						//选择想要的系统时钟
	//等待PLL成功用作于系统时钟的时钟源
	// 0x00：HSI 作为系统时钟
	// 0x04：HSE作为系统时钟
	// 0x08：PLL作为系统时钟
	while(RCC_GetSYSCLKSource() != 0x08);							//需与被选择的系统时钟对应起来，RCC_SYSCLKSource_PLL
#else
	RCC_DeInit();    												//先复位RCC寄存器
	RCC_HSEConfig( RCC_HSE_ON );    								//使能HSE
    if ( SUCCESS == RCC_WaitForHSEStartUp() )
    {
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);		//使能预取指，这是FLASH固件中的函数
        FLASH_SetLatency(FLASH_Latency_2);  						//设置FLASH等待周期。  因为倍频成72M 所以等待两个周期。
        //配置三个总线的倍频因子
        RCC_HCLKConfig(RCC_SYSCLK_Div1);							//HCLK --> AHB 最大为72M，所以只需要1分频
		RCC_PCLK1Config(RCC_HCLK_Div2);								//PCLK1 --> APB1 最大为36M，所以要2分频
		RCC_PCLK2Config(RCC_HCLK_Div1);								//PCLK2 --> APB2 最大为72M，所以只需要1分频
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);		//先配置锁相环 PLLCLK = HSE * 倍频因子
        RCC_PLLCmd(ENABLE);        									//使能PLL
        while ( RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY) );  	//等待PLL稳定
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);					//选择系统时钟（选择锁相环输出）
        while ( 0x08 != RCC_GetSYSCLKSource() );					//等待选择稳定
    }
#endif
}


