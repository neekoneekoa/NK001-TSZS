#include <string.h>
#include "usart1.h"
#include "stm32f10x_dma.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#define USAER1_RX_MAX        	128    //串口1最大接收数据量
#define USAER1_TX_MAX         	128 
#define USART1_RX_FIFO_MAX		(1024)

static uint8_t USART1_TX_Buff[USAER1_TX_MAX];
static uint8_t USART1_RX_Buff[USAER1_RX_MAX];

/* 传递读取信息 */
typedef struct{
    uint32_t sr_p;      //开始读取地址
    uint32_t er_p;      //结束读取地址
}USART_Read_t;

typedef struct{
	uint8_t buff[USART1_RX_FIFO_MAX];   //空间
    uint32_t w_p;                       //写指针
    uint32_t r_p;                       //读指针
    uint8_t coverFlag:1;                //写覆盖未读数据标记
}USART1_RX_FIFO_t;
static USART1_RX_FIFO_t usart1Fifo;
QueueHandle_t  uartRecQueue;

/**************************************************************************
函 数 名:Usart1_Send_DMA_Init
功能描述:串口1的DMA通道初始化
输入参数:None
输出参数:None
返 回 值:None。
其他说明:
**************************************************************************/
static void Usart1_Send_DMA_Init(void)
{
    DMA_InitTypeDef  DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel4);
   
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	
    DMA_InitStructure.DMA_BufferSize = USAER1_TX_MAX;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART1_TX_Buff;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	
	/* 发送一次数据,产生DMA1_FLAG_TC4标记 */
	DMA_SetCurrDataCounter(DMA1_Channel4, 1);
	DMA_ClearFlag(DMA1_FLAG_TC4);
	DMA_Cmd(DMA1_Channel4, ENABLE);
}

/**************************************************************************
函 数 名:Data_TO_FIFO
功能描述:
输入参数:
@srcData:数据源
@srcLen:数据源长度
输出参数:
@destFIFO:目标FIFO
返 回 值:None。
其他说明:
**************************************************************************/
static void Data_TO_FIFO(const uint8_t *srcData, const uint32_t srcLen, USART1_RX_FIFO_t *destFIFO)
{
	if( srcData == NULL || destFIFO == NULL )
		return;
	
	if( (destFIFO->w_p + srcLen) < USART1_RX_FIFO_MAX ){
		memcpy(destFIFO->buff + destFIFO->w_p, srcData, srcLen );
		destFIFO->w_p += srcLen;
	}else{
		uint32_t temp = USART1_RX_FIFO_MAX - destFIFO->w_p;
		memcpy(destFIFO->buff + destFIFO->w_p, srcData, temp );
		memcpy(destFIFO->buff, srcData + temp, srcLen - temp );
		destFIFO->w_p = srcLen - temp;
		if( destFIFO->w_p >= destFIFO->r_p )
			destFIFO->coverFlag = 1;	//标记覆写了未读取的数据
	}
}

/**************************************************************************
函 数 名:DMA1_Channel5_IRQHandler
功能描述:
输入参数:None
输出参数:None
返 回 值:None。
其他说明:
**************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
	if( DMA_GetITStatus(DMA1_IT_TC5) == SET){			//转移结束
		Data_TO_FIFO(&USART1_RX_Buff[USAER1_RX_MAX/2], USAER1_RX_MAX/2, &usart1Fifo);
//		printf("DMA1_IT_TC5\r\n");
		DMA_ClearITPendingBit(DMA1_IT_TC5);
	}else if( DMA_GetITStatus(DMA1_IT_HT5) == SET){		
		Data_TO_FIFO(USART1_RX_Buff, USAER1_RX_MAX/2, &usart1Fifo);
//		printf("DMA1_IT_HT5\r\n");
		DMA_ClearITPendingBit(DMA1_IT_HT5);
	}
	else if( DMA_GetITStatus(DMA1_IT_TE5) == SET){		//错误
		DMA_ClearITPendingBit(DMA1_IT_TE5);
	}
}

/**************************************************************************
函 数 名:Usart1_Receive_DMA_Init
功能描述:串口1的DMA通道初始化
输入参数:None
输出参数:None
返 回 值:None。
其他说明:
**************************************************************************/
static void Usart1_Receive_DMA_Init(void)
{
    DMA_InitTypeDef  DMA_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel5);
   
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	
    DMA_InitStructure.DMA_BufferSize = USAER1_RX_MAX;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART1_RX_Buff;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	
	NVIC_InitTypeDef NVIC_InitStructure;         //中断配置
	/* 配置中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel                   = DMA1_Channel5_IRQn;             //设置串口1的优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7 ;                      //主优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0 ;		                  //次优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;			            //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	          //根据指定的参数初始化VIC寄存器	
	
	
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC | DMA_IT_HT | DMA_IT_TE, ENABLE);
	
	DMA_Cmd(DMA1_Channel5,ENABLE);
}

/**************************************************************************
函 数 名:Usart1_Init
功能描述:串口1初始化。
输入参数:
    bound:波特率
输出参数:None
返 回 值:None。
其他说明:
        使用串口1前，必须调用该函数初始化。
**************************************************************************/
void Usart1_Init(const uint32_t bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;         //GPIO配置
    USART_InitTypeDef USART_InitStructure;       //串口配置
    NVIC_InitTypeDef NVIC_InitStructure;         //中断配置

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	    //使能USART1，GPIOA时钟
	
   /* 配置串口使用的GPIO */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;                                     //PA9(TX)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	                              //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                          //初始化GPIOA9
	
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;                                    //PA10(RX)
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;                                  //上拉输入(RX引脚可以设置为非模拟功能任何模式，甚至不设置)
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                          //初始化GPIOA10  
	
   /* 配置串口 */
    USART_InitStructure.USART_BaudRate   = bound;                                   //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;                        //一个停止位
    USART_InitStructure.USART_Parity     = USART_Parity_No;                         //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	          //收发模式
		USART_Init(USART1, &USART_InitStructure); //初始化串口1
		
   /* 配置中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;             //设置串口1的优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 8 ;                      //主优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0 ;		                  //次优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;			            //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	          //根据指定的参数初始化VIC寄存器	

    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);                                  //开启串口接收中断
    
	Usart1_Send_DMA_Init();
	Usart1_Receive_DMA_Init();
	USART_DMACmd(USART1, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
	
	uartRecQueue = xQueueCreate(10, sizeof(USART_Read_t));     //申请串口接收队列,用于存放分帧后的数据指针
    
	USART_Cmd(USART1, ENABLE);                                                      //使能串口1 
}

/**************************************************************************
函 数 名:Usart1_SendData
功能描述:串口1发送数据。
输入参数:
    pData:发送的数据缓存
    dataLen:发送的数据长度
输出参数:None
返 回 值:None
其他说明:
支持发送超过发送缓存的数据,但是为了执行效率,尽量不要发送超过发送缓存的数据,或者把发送缓存开大一点。
**************************************************************************/
void Usart1_SendData(const uint8_t *pData, const uint16_t dataLen)
{
#if 0
    uint16_t i =0; 
	
    for( i = 0; i < dataLen; i++ )
    {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);    //等待发送寄存器空       
        USART_SendData(USART1, pData[i]);   			
    }
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);    //等待发送寄存器空   
#endif
	uint32_t circ = dataLen / USAER1_TX_MAX;
	uint32_t res = dataLen % USAER1_TX_MAX;
	uint32_t i = 0;

	while(DMA_GetFlagStatus(DMA1_FLAG_TC4) != SET);	//等待发送完成
	
	for( i = 0; i < circ; i++ ){
		DMA_Cmd(DMA1_Channel4, DISABLE);
		memcpy(USART1_TX_Buff, pData + i * USAER1_TX_MAX, USAER1_TX_MAX);
		DMA_SetCurrDataCounter(DMA1_Channel4, USAER1_TX_MAX);
		DMA_ClearFlag(DMA1_FLAG_TC4);
		DMA_Cmd(DMA1_Channel4, ENABLE);
		while(DMA_GetFlagStatus(DMA1_FLAG_TC4) != SET);	//等待发送完成
	}
	
	if( res ){
		DMA_Cmd(DMA1_Channel4, DISABLE);
		memcpy(USART1_TX_Buff, pData + i * USAER1_TX_MAX , res);
		DMA_SetCurrDataCounter(DMA1_Channel4, res);
		DMA_ClearFlag(DMA1_FLAG_TC4);
		DMA_Cmd(DMA1_Channel4, ENABLE);
	}
	 
}

/**************************************************************************
函 数 名:USART1_IRQHandler
功能描述:串口1中断服务函数。
输入参数:None
输出参数:None
返 回 值:None
其他说明:
        接收到\r\n或者接收数据达到最大值，即接收数据结束。
**************************************************************************/
void USART1_IRQHandler(void)
{
	BaseType_t xHigherPrioritTaskWoken;
	USART_Read_t usartRead;
	
    if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET){    //中断类型是接收数据
		DMA_Cmd(DMA1_Channel5, DISABLE);
		uint32_t recvLen = USAER1_RX_MAX - DMA_GetCurrDataCounter(DMA1_Channel5);
		
		if( recvLen < USAER1_RX_MAX/2 )
			Data_TO_FIFO(USART1_RX_Buff, recvLen, &usart1Fifo);
		else
			Data_TO_FIFO(&USART1_RX_Buff[USAER1_RX_MAX/2], recvLen - USAER1_RX_MAX/2, &usart1Fifo);
		
		usartRead.sr_p = usart1Fifo.r_p;
		usartRead.er_p = (usart1Fifo.w_p == 0) ? (USART1_RX_FIFO_MAX - 1) : (usart1Fifo.w_p - 1); 

		usart1Fifo.r_p = usartRead.er_p + 1;
		if( uartRecQueue )
			xQueueSendFromISR( uartRecQueue, &usartRead, &xHigherPrioritTaskWoken );
		
		DMA_SetCurrDataCounter(DMA1_Channel5,USAER1_RX_MAX);
		DMA_Cmd(DMA1_Channel5, ENABLE);
		/* 清理空闲中断 */
		USART1->SR;//先读SR
        USART1->DR;//再读DR
	}			
}

/**
name:USART1_Read
function:串口1读数据
input:None
output:
@rData:读取的数据指针,实际地址空间在该函数内部申请,所以在外部使用后必须要释放
@rDataLen:读取的数据长度指针
@timeout:等待超时时间
return:错误
other:None
*/
int USART1_Read(uint8_t **rData, uint32_t *rDataLen, uint32_t timeout)
{
    USART_Read_t usartRead;
    uint8_t *p = NULL;
    uint32_t tempLen = 0;

    if( rData == NULL || rDataLen == NULL)
        return -1;

    if(xQueueReceive(uartRecQueue, &usartRead, timeout) != pdTRUE)  //读取串口队列消息,没有数据就阻塞等待
        return -1;

    if( usart1Fifo.coverFlag ){
        usart1Fifo.coverFlag = 0;
		printf("usart1Receive.fifo.coverFlag\r\n");
        return -1;
    }

    if( usartRead.er_p >= usartRead.sr_p ){ //结束地址大于开始地址
		*rDataLen = usartRead.er_p - usartRead.sr_p + 1;
		p = pvPortMalloc(*rDataLen + 1);
		if( p != NULL ){
			memset(p, 0, *rDataLen + 1);
			memcpy(p, &usart1Fifo.buff[usartRead.sr_p], *rDataLen);
		}
	}else{    //结束地址小于开始地址
		tempLen = USART1_RX_FIFO_MAX - usartRead.sr_p;
		*rDataLen = tempLen + usartRead.er_p +1;

    p = pvPortMalloc(*rDataLen + 1);
    if( p != NULL ){
      memset(p, 0, *rDataLen + 1);
      memcpy(p, &usart1Fifo.buff[usartRead.sr_p], tempLen);
      memcpy(p + tempLen, usart1Fifo.buff, usartRead.er_p + 1);
    }
  }

  *rData = p;

  return 0;
}

/******************************************************************************************************
                      以下部分是是实现标准库函数printf
******************************************************************************************************/
 
/**************************************************************************
函 数 名:fputc
功能描述:重定义fputc。
输入参数:
    ch:发送的数据
    f:文件(不用管,在有文件系统的操作系统中使用)
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
int fputc(int ch, FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);    //等待发送寄存器空	
    USART_SendData(USART1, ch);
   
    return ch;
}
