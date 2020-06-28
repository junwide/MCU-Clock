#include "iostm8s105c6.h"
#include "LCD5110.h"
#include "LCD_ZIKU.h"

/***********************************************
                  LCD5110  管脚配置
LCD5110_rst	->PD0
LCD5110_ce	->PG1
LCD5110_dc	->PC2
LCD5110_din	->PB0
LCD5110_clk	->PB1
************************************************/
#define LCD5110_rst PD_ODR_ODR0
#define LCD5110_ce  PG_ODR_ODR1
#define LCD5110_dc  PC_ODR_ODR2
#define LCD5110_din PB_ODR_ODR0
#define LCD5110_clk PB_ODR_ODR1

/*********************************************************** 
函数名称：LCD5110_GPIO_Init
函数功能：配置IO口为推挽输出
***********************************************************/
void LCD5110_GPIO_Init(void)
{	
  
        PD_DDR |= 0X9d;       
        PD_CR1 |= 0X9d;       
        PD_CR2  = 0X00;   //包括RTC
	//PD_DDR |= 0x01;
	//PD_CR1 |= 0x01;

	PG_DDR |= 0x02;
	PG_CR1 |= 0x02;

	PC_DDR |= 0x04;
	PC_CR1 |= 0x04;

	PB_DDR |= 0x03;
	PB_CR1 |= 0x03;
}

/*********************************************************** 
函数名称：LCD5110_Send_Data
函数功能：模拟SPI接口时序写数据LCD
入口参数：Byte ：写入的数据
***********************************************************/
void LCD5110_Send_Data(unsigned char Byte)
{
    unsigned char i;
	
    LCD5110_ce = 0;		//5110片选有效，允许输入数据
    LCD5110_dc = 1;	        //写数据
	
    for(i=0;i<8;i++)		//传送8bit数据
    { 
        if(Byte&0x80)
     	   LCD5110_din = 1;
		else
       		LCD5110_din = 0;

        LCD5110_clk = 0;
        Byte = Byte<<1;
        LCD5110_clk = 1;
    }
	
    LCD5110_ce = 1;		//禁止5110
}

/*********************************************************** 
函数名称：LCD5110_Send_Commond
函数功能：模拟SPI接口时序写命令LCD
入口参数：Byte    ：写入的命令
***********************************************************/
void LCD5110_Send_Commond(unsigned char Byte)
{
    unsigned char i;

    LCD5110_ce = 0;	        //5110片选有效，允许输入数据
    LCD5110_dc = 0;	        //写命令
	
    for(i=0;i<8;i++)		//传送8bit数据
    { 
        if(Byte&0x80)
        	LCD5110_din = 1;
		else
			LCD5110_din = 0;
	
		LCD5110_clk = 0;
		Byte = Byte << 1;
		LCD5110_clk = 1;
    }
	
    LCD5110_ce = 1;			//禁止5110
}

/*********************************************************** 
函数名称：LCD5110_Set_XY
函数功能：设置LCD坐标函数
入口参数：行 X：0－83
          列 Y：0－5
***********************************************************/
void LCD5110_Set_XY(unsigned char x, unsigned char y)
{
    LCD5110_Send_Commond(y | 0x40);
    LCD5110_Send_Commond(x | 0x80);
}

/*********************************************************
函数名称：void LCD5110_clear 
函数功能：LCD5110清屏
*********************************************************/
void LCD5110_Clear(void)
{
    unsigned char t, k;
	
    for(t=0;t<6;t++)
    {
        for(k=0;k<84;k++)
        {
            LCD5110_Send_Data( 0x00 );
        }
    }
}

/*********************************************************** 
函数名称：LCD5110_Init
函数功能：LCD5110初始化，包括对IO的初始化
***********************************************************/
void LCD5110_Init(void)
{
    LCD5110_GPIO_Init();
	
    LCD5110_rst = 0;
    LCD5110_rst = 1;
    LCD5110_ce = 1;
    LCD5110_ce = 0;
	
    LCD5110_Send_Commond( 0x21 );  	// 使用扩展命令设置LCD模式
    LCD5110_Send_Commond( 0xC8 ); 	// 设置液晶偏置电压
    LCD5110_Send_Commond( 0x06 ); 	// 温度校正
    LCD5110_Send_Commond( 0x13 );	//  1:48
    LCD5110_Send_Commond( 0x20 );	// 使用基本命令，V=0，水平寻址
	
	/*设置显示模式*/
//	LCD5110_Send_Commond( 0x08 );	// 显示空白	
//	LCD5110_Send_Commond( 0x09 );	// 显示所有像素
	LCD5110_Send_Commond( 0x0C );	// 普通模式	
//	LCD5110_Send_Commond( 0x0D );	// 反显模式	
   
    LCD5110_Clear();   				//清屏幕
    LCD5110_Set_XY(0,0);   			//指针回归最上角
}

/*********************************************************** 
函数名称：LCD5110_Put_Char
函数功能：输出ASCII代表的字符
入口参数：x       ：0-83
          y       ：0-5
          ASCII   ：字符的ASCII码（等于ziku.h中各字符的值加32）
***********************************************************/
void LCD5110_Put_Char_6_8(unsigned char x, unsigned char y, unsigned char z, unsigned char ASCII)
{ 
    unsigned char i; 
    unsigned int index; 
  
    index = ASCII - 32;			//字模数据是由空格' '开始,空格的ASCII的值就是32
    LCD5110_Set_XY(x,y);
	
    for(i=0;i<6;i++)  
    {
		if(z==1)
		{
        	LCD5110_Send_Data( ~ Char_6_8[index][i] ); 
		}
		else
		{
			LCD5110_Send_Data( Char_6_8[index][i] );
		}
    }
	LCD5110_Send_Data(0);     
}

/*********************************************************** 
函数名称：LCD5110_PUT_String
函数功能：显示字符串
入口参数：x       ：0-83
          y       ：0-5
          *string ：要显示的字符串
***********************************************************/
void LCD5110_Put_String_6_8(unsigned char x, unsigned char y, unsigned char z, unsigned char *string)
{
    unsigned i=0;
    while(*string)
    {
        LCD5110_Put_Char_6_8(x+i*6,y,z,*string++);
        i++;
    }
}

/*********************************************************** 
函数名称：LCD5110_Put_Char
函数功能：输出ASCII代表的字符
入口参数：x       ：0-83
          y       ：0-5
          ASCII   ：字符的ASCII码（等于ziku.h中各字符的值加32）
***********************************************************/
void LCD5110_Put_Number_6_8(unsigned char x, unsigned char y, unsigned char z, unsigned char number)
{
    unsigned char i; 
    unsigned int index; 
  
    index = number+48 -32;	//字模数据是由空格' '开始,空格的ASCII的值就是32
							//数字的数值和ASCII值相差48
    LCD5110_Set_XY(x,y);
    for(i=0;i<6;i++)
    {   
		if(z==1)
		{
        	LCD5110_Send_Data( ~ Char_6_8[index][i] ); 
		}
		else
		{
			LCD5110_Send_Data( Char_6_8[index][i] );
		}
    }
	
    LCD5110_Send_Data(0);     
}

/*********************************************
* 以 6*12点阵的形式显示字符
*********************************************/
void LCD5110_Put_Char_6_12(unsigned char x, unsigned char y, unsigned char z, unsigned char Index)
{ 
    unsigned char i, j; 
	
	Index *= 2;				//每个字符有两行，在数组中的行数实际还要 x2
	for(j=0;j<2;j++)		//每个字符有两行，所以要循环显示两行
	{
		LCD5110_Set_XY(x, y+j);
		Index += j;
		for(i=0;i<6;i++) 
		{       
			if(z==1)
			{
				LCD5110_Send_Data( ~ Char_6_12[Index][i] );
			}
			else
			{
				LCD5110_Send_Data( Char_6_12[Index][i] );
			}
		} 
	}
}

/*********************************************
* 以 8*16点阵的形式显示字符
*********************************************/
void LCD5110_Put_Char_8_16(unsigned char x, unsigned char y, unsigned char z, unsigned char Index)
{ 
    unsigned char i, j; 
	
	Index *= 2;				//每个字符有两行，在数组中的行数实际还要 x2
	for(j=0;j<2;j++)		//每个字符有两行，所以要循环显示两行
	{
		LCD5110_Set_XY(x, y+j);
		Index += j;
		for(i=0;i<8;i++) 
		{       
			if(z==1)
			{
				LCD5110_Send_Data( ~ Char_8_16[Index][i] );
			}
			else
			{
				LCD5110_Send_Data( Char_8_16[Index][i] );
			}
		} 
	}
}

/***********************************************************
以 12*12点阵形式显示汉字
***********************************************************/
void LCD5110_Put_Chinese_12_12(unsigned char x, unsigned char y, unsigned char z, unsigned char Chinese_Index)
{ 
    unsigned char i, j; 
	
	Chinese_Index *= 2;		//每个汉字有两行字符，在数组中的行数实际还要 x2
	for(j=0;j<2;j++)		//每个汉字有两行字符，所以要循环显示两行
	{
		LCD5110_Set_XY(x, y+j);
		Chinese_Index += j;
		for(i=0;i<12;i++) 
		{       
			if(z==1)
			{
				LCD5110_Send_Data( ~ Chinese_12_12[Chinese_Index][i] );
			}
			else
			{
				LCD5110_Send_Data( Chinese_12_12[Chinese_Index][i] );
			}
		} 
	}
}

/*********************************************************** 
以 16*16点阵形式显示汉字
***********************************************************/
void LCD5110_Put_Chinese_16_16(unsigned char x, unsigned char y, unsigned char z, unsigned char Chinese_Index)
{ 
    unsigned char i, j; 
	
	Chinese_Index *= 2;		//每个汉字有两行字符，在数组中的行数实际还要 x2
	for(j=0;j<2;j++)		//每个汉字有两行字符，所以要循环显示两行
	{
		LCD5110_Set_XY(x, y+j);
		Chinese_Index += j;
		for(i=0;i<16;i++)  
		{
			if(z==1)
			{
				LCD5110_Send_Data( ~ Chinese_16_16[Chinese_Index][i] );
			}
			else
			{
				LCD5110_Send_Data( Chinese_16_16[Chinese_Index][i] );
			}
		} 
	}
}



