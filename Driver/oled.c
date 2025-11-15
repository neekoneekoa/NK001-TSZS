/***************************************************************************************
*说    明:
        使用IIC的方式驱动OLED
		SCL	(PB8)
		SDA	(PB9)
***************************************************************************************/
#include "oled.h"
#include "FreeRTOS.h"
#include "task.h"
#include "oledfont.h"

// CPU指令延时宏定义，根据实际系统时钟调整
#define CPU_DELAY_NOP()  __NOP()
#define CPU_DELAY_US(us) { \
    uint32_t i; \
    for(i = 0; i < (us * 8); i++) { \
        CPU_DELAY_NOP(); \
    } \
}

// IIC总线引脚定义
#define OLED_SCL_Clr()	GPIO_ResetBits(GPIOB,GPIO_Pin_8)  // SCL - PB8
#define OLED_SCL_Set()	GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define OLED_SDA_Clr()	GPIO_ResetBits(GPIOB,GPIO_Pin_9)  // SDA - PB9
#define OLED_SDA_Set()	GPIO_SetBits(GPIOB,GPIO_Pin_9)

// IIC设备地址（7位地址）
#define OLED_ADDRESS 0x78

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据

// IIC函数声明
static void IIC_Start(void);
static void IIC_Stop(void);
static uint8_t IIC_Wait_Ack(void);
static void IIC_Ack(void);
static void IIC_NAck(void);
static void IIC_Write_Byte(uint8_t dat);

//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
static uint8_t OLED_GRAM[128][8];	//针对于128*64的显示屏	

/**************************************************************************
函 数 名:OLED_WR_Byte
功能描述:IIC方式写入一个字节
输入参数:
@dat:写入的数据
@cmd:数据/命令标志 0,表示命令;1,表示数据
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{	
	taskENTER_CRITICAL();
	IIC_Start();
	IIC_Write_Byte(OLED_ADDRESS);  //写入从机地址
	IIC_Wait_Ack();
	
	if(cmd)  //发送数据/命令标志
		IIC_Write_Byte(0x40);
	else
		IIC_Write_Byte(0x00);
	
	IIC_Wait_Ack();
	IIC_Write_Byte(dat);  //写入数据
	IIC_Wait_Ack();
	IIC_Stop();
	taskEXIT_CRITICAL();
}

/**************************************************************************
函 数 名:OLED_GPIO_Init
功能描述:OLED引脚初始化
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
/**************************************************************************
函 数 名:IIC_Start
功能描述:产生IIC起始信号
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void IIC_Start(void)
{
	OLED_SDA_Set();
	OLED_SCL_Set();
	CPU_DELAY_US(1); // 用CPU延时替代任务延时
	OLED_SDA_Clr();
	CPU_DELAY_US(1);
	OLED_SCL_Clr();
}

/**************************************************************************
函 数 名:IIC_Stop
功能描述:产生IIC停止信号
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void IIC_Stop(void)
{
	OLED_SDA_Clr();
	OLED_SCL_Set();
	CPU_DELAY_US(1); // 用CPU延时替代任务延时
	OLED_SDA_Set();
	CPU_DELAY_US(1);
}

/**************************************************************************
函 数 名:IIC_Wait_Ack
功能描述:等待应答信号到来
输入参数:None
输出参数:None
返 回 值:0：接收应答成功 1：接收应答失败
其他说明:
**************************************************************************/
static uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	
	OLED_SDA_Set();
	CPU_DELAY_US(1); // 用CPU延时替代任务延时
	OLED_SCL_Set();
	CPU_DELAY_US(1);
	
	while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
		CPU_DELAY_US(1); // 添加小延时避免CPU空转过快
	}
	
	OLED_SCL_Clr();
	return 0;
}

/**************************************************************************
函 数 名:IIC_Ack
功能描述:产生ACK应答
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void IIC_Ack(void)
{
	OLED_SDA_Clr();
	CPU_DELAY_US(1); // 用CPU延时替代任务延时
	OLED_SCL_Set();
	CPU_DELAY_US(1);
	OLED_SCL_Clr();
}

/**************************************************************************
函 数 名:IIC_NAck
功能描述:不产生ACK应答
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void IIC_NAck(void)
{
	OLED_SDA_Set();
	CPU_DELAY_US(1); // 用CPU延时替代任务延时
	OLED_SCL_Set();
	CPU_DELAY_US(1);
	OLED_SCL_Clr();
}

/**************************************************************************
函 数 名:IIC_Write_Byte
功能描述:IIC发送一个字节
输入参数:
@dat:发送的字节
输出参数:None
返 回 值:None
其他说明:在调用此函数的上层函数中已经有临界区保护，这里不再添加
**************************************************************************/
static void IIC_Write_Byte(uint8_t dat)
{
	uint8_t t;
	
	for(t=0;t<8;t++)
	{
		if((dat&0x80)>>7)
			OLED_SDA_Set();
		else
			OLED_SDA_Clr();
		
		dat<<=1;
		OLED_SCL_Set();
		CPU_DELAY_US(1); // 用CPU延时替代任务延时
		OLED_SCL_Clr();
		CPU_DELAY_US(1);
	}
}

/**************************************************************************
函 数 名:OLED_GPIO_Init
功能描述:OLED引脚初始化
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void OLED_GPIO_Init( void )
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); 	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; // SCL - PB8, SDA - PB9
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;  	  	//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//速度50MHz
  	GPIO_Init(GPIOB, &GPIO_InitStructure);  
	
	GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);
}

/**************************************************************************
函 数 名:OLED_Refresh_Gram
功能描述:显示屏刷新 - IIC模式下的优化版本
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void OLED_Refresh_Gram(void)
{
	uint8_t i, n;
	
	taskENTER_CRITICAL();
	for(i = 0; i < 8; i++){  
		// 设置页地址和列地址
		OLED_WR_Byte (0xb0 + i, OLED_CMD);  //设置页地址（0~7）
		OLED_WR_Byte (0x00, OLED_CMD);      //设置显示位置—列低地址
		OLED_WR_Byte (0x10, OLED_CMD);      //设置显示位置—列高地址
		
		// 在IIC模式下，优化数据传输
		for( n = 0; n < 128; n++)
		{
			OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
			// 在IIC模式下，每次写入一个字节后需要有足够的延迟
		}
	}
	taskEXIT_CRITICAL();
}

/**************************************************************************
函 数 名:OLED_Display_On
功能描述:开启OLED显示 
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X14, OLED_CMD);  //DCDC ON
	OLED_WR_Byte(0XAF, OLED_CMD);  //DISPLAY ON
}

/**************************************************************************
函 数 名:OLED_Display_Off
功能描述:关闭OLED显示 
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/  
void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);  //SET DCDC命令
	OLED_WR_Byte(0X10, OLED_CMD);  //DCDC OFF
	OLED_WR_Byte(0XAE, OLED_CMD);  //DISPLAY OFF
}	

/**************************************************************************
函 数 名:OLED_Clear
功能描述:清屏函数
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/  	  
void OLED_Clear(void)  
{  
	uint8_t i, n;  
	for( i = 0; i < 8; i++)
		for( n = 0; n < 128; n++)
			OLED_GRAM[n][i] = 0X00;
	
	OLED_Refresh_Gram();//更新显示
}

/**************************************************************************
函 数 名:OLED_DrawPoint
功能描述:画点 
输入参数:
@x:0~127
@y:0~63
@t:1 填充 0,清空
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/			   
static void OLED_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
	uint8_t pos, bx, temp = 0;
	
	if(x > 127 || y > 63)
		return;
	
	pos = 7 - y/8;
	bx = y%8;
	temp = 1<<(7 - bx);
	if(t)
		OLED_GRAM[x][pos] |= temp;
	else 
		OLED_GRAM[x][pos] &= ~temp;	    
}

/**************************************************************************
函 数 名:OLED_ShowChar
功能描述:在指定位置显示一个字符,包括部分字符
输入参数:
@x:0~127
@y:0~63
@mode:0,反白显示;1,正常显示
@size:选择字体 12/16/24
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
static void OLED_ShowChar(uint8_t x ,uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{      			    
	uint8_t temp, t, t1;
	uint8_t y0 = y;
	uint8_t csize = (size/8 + ((size %8 ) ? 1 : 0))*(size/2);		//得到字体一个字符对应点阵集所占的字节数
	chr=chr - ' ';//得到偏移后的值		 
    
	for(t = 0; t < csize; t++){   
		if(size == 12)
			temp=asc2_1206[chr][t]; 	 	//调用1206字体
		else if(size == 16)
			temp=asc2_1608[chr][t];			//调用1608字体
		else if(size == 24)
			temp=asc2_2412[chr][t];			//调用2412字体
		else 
			return;								//没有的字库
		
        for(t1 = 0; t1 < 8; t1++){
			if(temp & 0x80)
				OLED_DrawPoint(x, y, mode);
			else 
				OLED_DrawPoint(x, y, !mode);
			
			temp <<= 1;
			y++;
			if((y - y0) ==size){
				y = y0;
				x++;
				break;
			}
		}  	 
    }          
}

/**************************************************************************
函 数 名:OLED_ShowString
功能描述:显示字符串
输入参数:
@x:0~127
@y:0~63
@p:字符串起始地址
@size:选择字体 12/16/24
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void OLED_ShowString(uint8_t x, uint8_t y, const char *p, uint8_t size)
{	
    while((*p <= '~') && (*p >= ' ')){//判断是不是非法字符!     
        if( x > (128 - (size/2))){
			x = 0;
			y += size;
		}
		
        if(y > (64 - size)){
			y = x = 0;
			OLED_Clear();
		}
        OLED_ShowChar(x, y, *p, size, 1);	 
        x += size/2;
        p++;
    } 
	OLED_Refresh_Gram();
}	

/**************************************************************************
函 数 名:OLED_ShowNum
功能描述:在指定位置显示一个数字（变量）
输入参数:
@x,y:起点坐标 
@num:要显示的数字
@len:数字位数
@size:选择字体 12/16/24
输出参数:None
返 回 值:None
其他说明:支持显示负数，自动转换为字符串后调用OLED_ShowString
**************************************************************************/
void OLED_ShowNum(uint8_t x, uint8_t y, int32_t num, uint8_t len, uint8_t size)
{
    char buf[12];  // 最大支持10位数字 + 符号 + '\0'
    uint8_t i = 0;
    uint8_t neg = 0;

    if(num < 0){
        neg = 1;
        num = -num;
    }

    do{
        buf[i++] = num % 10 + '0';
        num /= 10;
    }while(num > 0);

    if(neg) buf[i++] = '-';

    // 补零到指定长度
    while(i < len) buf[i++] = '0';

    buf[i] = '\0';

    // 反转字符串
    for(uint8_t j = 0; j < i/2; j++){
        char tmp = buf[j];
        buf[j] = buf[i-1-j];
        buf[i-1-j] = tmp;
    }

    OLED_ShowString(x, y, buf, size);
}


/**************************************************************************
函 数 名:OLED_Init
功能描述:OLED初始化
输入参数:None
输出参数:None
返 回 值:None
其他说明:
**************************************************************************/
void OLED_Init( void )
{
	OLED_GPIO_Init();
	
	// 等待OLED电源稳定
	vTaskDelay(200);

	// IIC方式OLED初始化命令序列
	OLED_WR_Byte(0xAE,OLED_CMD); // 关闭显示
	OLED_WR_Byte(0xD5,OLED_CMD); // 设置显示时钟分频率/振荡器频率
	OLED_WR_Byte(0x80,OLED_CMD); // 设置分频因子，建议值
	OLED_WR_Byte(0xA8,OLED_CMD); // 设置多路复用率
	OLED_WR_Byte(0x3F,OLED_CMD); // 1/64占空比
	OLED_WR_Byte(0xD3,OLED_CMD); // 设置显示偏移
	OLED_WR_Byte(0x00,OLED_CMD); // 无偏移
	OLED_WR_Byte(0x40,OLED_CMD); // 设置显示开始行[5:0]
	OLED_WR_Byte(0x8D,OLED_CMD); // 设置充电泵
	OLED_WR_Byte(0x14,OLED_CMD); // 开充电泵
	OLED_WR_Byte(0x20,OLED_CMD); // 设置内存寻址模式
	OLED_WR_Byte(0x00,OLED_CMD); // 水平寻址模式
	OLED_WR_Byte(0xA1,OLED_CMD); // 设置段重定义设置,bit0:0,0->0;1,0->127; 
	OLED_WR_Byte(0xC0,OLED_CMD); // 设置COM扫描方向;bit3:0,普通模式;0,COM0->COM[N-1];N:驱动路数
	OLED_WR_Byte(0xDA,OLED_CMD); // 设置COM硬件引脚配置
	OLED_WR_Byte(0x12,OLED_CMD); // 配置
	OLED_WR_Byte(0x81,OLED_CMD); // 设置对比度控制
	OLED_WR_Byte(0xEF,OLED_CMD); // 对比度设置，可根据实际效果调整
	OLED_WR_Byte(0xD9,OLED_CMD); // 设置预充电周期
	OLED_WR_Byte(0xF1,OLED_CMD); // 设置预充电为15个时钟，放电为1个时钟
	OLED_WR_Byte(0xDB,OLED_CMD); // 设置VCOMH取消选择级别
	OLED_WR_Byte(0x30,OLED_CMD); // 设置VCOMH为0.83V
	OLED_WR_Byte(0xA4,OLED_CMD); // 全局显示开启
	OLED_WR_Byte(0xA6,OLED_CMD); // 设置显示方式为正常显示
	OLED_Clear();
	OLED_WR_Byte(0xAF,OLED_CMD); // 开启OLED显示
}

