/***************************************************************************************
*Copyright (c) 2019 - ,
*文 件 名:usart1.c
*作    者:赵光鑫
*联系方式:zhao.guangxin@qq.com
*创建日期:2019-9-7
*说    明:
        (1)usart1使用的IO口
				USART1_RX<------------->PA10(43)
				USART1_TX<------------->PA9(42)
		(2)该文件usart1接收和发送，并向外提供接口。
        (3当串口接收数据完成后将串口接收一帧数据完成标记置位。等待外部处理
如果不及时处理，将不会接收新的数据.	
*修    改:

***************************************************************************************/
#include <string.h>
#include "usart2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define USAER2_REC_MAX          256    //串口1最大接收数据量
#define UART_QUEUE_NUM          (16)              //队列长度
static QueueHandle_t                   uartRecv_Queue;	  //串口接收数据队列

/* 串口接收FIFO有效数据索引 */
typedef struct {
    uint32_t ori;   //读取指针
    uint32_t end;   //数据结束指针
}ST_RECV_INDEX;

/* uart fifo指针 */
static struct ST_UART_RECV{
  uint32_t writeIndex;
  uint32_t readIndex;
  uint8_t overflow;                               //溢出标记
  uint8_t *buff;
}uartRecv = {
  .writeIndex = 0,
  .readIndex = 0,
  .overflow = 0,
  .buff = NULL,
};

/**************************************************************************
函 数 名:Usart2_Init
功能描述:串口2初始化。
输入参数:
    bound:波特率
输出参数:None
返 回 值:None。
其他说明:
        使用串口1前，必须调用该函数初始化。
**************************************************************************/
void Usart2_Init(const uint32_t bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;         //GPIO配置
    USART_InitTypeDef USART_InitStructure;       //串口配置
    NVIC_InitTypeDef NVIC_InitStructure;         //中断配置

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	    //使能USART2，GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    //使能USART2，GPIOA时钟
	
   /* 配置串口使用的GPIO */
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;                                     //PA3(TX)
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;	                              //复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                          
	
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;                                    //PA2(RX)
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;                                  //上拉输入(RX引脚可以设置为非模拟功能任何模式，甚至不设置)
    GPIO_Init(GPIOA, &GPIO_InitStructure);                                          
	
   /* 配置串口 */
    USART_InitStructure.USART_BaudRate   = bound;                                   //串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     //字长为8位数据格式
    USART_InitStructure.USART_StopBits   = USART_StopBits_1;                        //一个停止位
    USART_InitStructure.USART_Parity     = USART_Parity_No;                         //无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //无硬件数据流控制
    USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;	          //收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口1
		
   /* 配置中断优先级 */
    NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn;             //设置串口1的优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6 ;                      //主优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0 ;		                  //次优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;			            //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	          //根据指定的参数初始化VIC寄存器	

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                  //开启串口接收中断
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);                  //开启串口空闲中断
    USART_Cmd(USART2, ENABLE);                                                      //使能串口2 

    uartRecv_Queue = xQueueCreate(UART_QUEUE_NUM, sizeof(ST_RECV_INDEX));     //申请串口接收队列
    configASSERT( uartRecv_Queue );

      /* 动态申请FIFO空间 */
    uartRecv.buff = (uint8_t *)pvPortMalloc(USAER2_REC_MAX);   //此处申请的内存需要长期使用，所以不用释放
    configASSERT( uartRecv.buff );	
}

/**************************************************************************
函 数 名:USART2_IRQHandler
功能描述:串口2中断服务函数。
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void USART2_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)    //中断类型是接收数据
	{
        uartRecv.buff[uartRecv.writeIndex++] = (uint8_t)USART_ReceiveData(USART2);    //接收数据到缓存   
        /* 写FIFO溢出，再从头写 */
        if( uartRecv.writeIndex >= USAER2_REC_MAX )   
            uartRecv.writeIndex = 0;

        /* 如果写FIFO的指针等于读FIFO的指针，表示数据没有读取 */
        if( uartRecv.writeIndex == uartRecv.readIndex )     
            uartRecv.overflow = 1;      //标记溢出  

	}else if( USART_GetITStatus(USART2, USART_IT_IDLE) != RESET ){    //空闲中断
        ST_RECV_INDEX recIndex;
        recIndex.ori = uartRecv.readIndex;
        recIndex.end = (uartRecv.writeIndex == 0) ? (USAER2_REC_MAX - 1) : (uartRecv.writeIndex - 1); 
        uartRecv.readIndex = recIndex.end + 1;
        
        xQueueSendToFrontFromISR( uartRecv_Queue, &recIndex, &xHigherPriorityTaskWoken );
        USART_ReceiveData(USART2);                                  //清理空闲中断
    }		
}

/**************************************************************************
函 数 名:USART2_GetData
功能描述:串口2获取数据。
输入参数:
@timeout:超时
输出参数:
@getBuff:输出获取的数据 需要外部释放
@len:获取的数据长度
返 回 值:0:成功  其他失败
其他说明:
**************************************************************************/
int USART2_GetData(uint8_t **getBuff, uint32_t *len, uint32_t timeout)
{
  uint32_t tempLen = 0;
  ST_RECV_INDEX recIndex;
  uint8_t *p = NULL;

  if( getBuff == NULL || len == NULL){
    return -1;
  }

  if(xQueueReceive(uartRecv_Queue, &recIndex, timeout) != pdTRUE){  //读取串口队列消息,没有数据就阻塞等待
    return -1;
  }


  if( uartRecv.overflow != 0 ){
    uartRecv.overflow = 0;
  }
  
  if( recIndex.end >= recIndex.ori ){
    *len = recIndex.end - recIndex.ori + 1;
    p = pvPortMalloc(*len + 1);
    if( p != NULL ){
      memset(p, 0, *len + 1);
      memcpy(p, &uartRecv.buff[recIndex.ori], *len);
    }
  }else{
    tempLen = USAER2_REC_MAX - recIndex.ori;
    *len = tempLen + recIndex.end +1;

    p = pvPortMalloc(*len + 1);
    if( p != NULL ){
      memset(p, 0, *len + 1);
      memcpy(p, &uartRecv.buff[recIndex.ori], tempLen);
      memcpy(p + tempLen, uartRecv.buff,recIndex.end + 1);
    }
  }
  *getBuff = p;

  return 0;
}

