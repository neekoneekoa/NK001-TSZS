/***************************************************************************************
*Copyright (c) 2019 - ,
*文 件 名:led.c
*作    者:赵光鑫
*联系方式:zhao.guangxin@qq.com
*创建日期:2019-9-6
*说    明:
        (1)LED灯使用的IO口
		  LED0<------------------>PA8(41)
			LED1<------------------>PD2(54)    
		(2)LED等低电平点亮，高电平熄灭
        (3)该文件实现LED的驱动，并向外提供接口
		(4)增加了PB13(输入)、PB14(低电平)、PB15(高电平)的GPIO配置					
*修    改:

***************************************************************************************/
#include "led.h"

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
函 数 名:GetPB13State
功能描述:获取PB13引脚的状态
输入参数:None
输出参数:None
返 回 值:uint8_t - 返回PB13的电平状态
其他说明:
        返回1表示高电平，0表示低电平
**************************************************************************/
uint8_t GetPB13State(void)
{
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13);
}

