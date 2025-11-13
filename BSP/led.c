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
函 数 名:Led1_On
功能描述:使能LED1（PB1职高、PB14置低、PB13浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led1_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
    

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led2_On
功能描述:使能LED2（PB15置低、PB14置高、PB13浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led2_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led3_On
功能描述:使能LED3（PB13置高、PB15置低、PB14浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led3_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led4_On
功能描述:使能LED4（PB13置低、PB15置高、PB14浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led4_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_15);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led5_On
功能描述:使能LED5（PB13置高、PB14置低、PB15浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led6_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led6_On
功能描述:使能LED6（PB14置高、PB13置低、PB15浮空输入）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led5_On(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**************************************************************************
函 数 名:Led_Off
功能描述:关闭所有LED（PB13、PB14、PB15全部置低）
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void Led_Off(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOB, GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
}
