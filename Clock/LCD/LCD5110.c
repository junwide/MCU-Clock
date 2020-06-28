#include "iostm8s105c6.h"
#include "LCD5110.h"
#include "LCD_ZIKU.h"

/***********************************************
                  LCD5110  �ܽ�����
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
�������ƣ�LCD5110_GPIO_Init
�������ܣ�����IO��Ϊ�������
***********************************************************/
void LCD5110_GPIO_Init(void)
{	
  
        PD_DDR |= 0X9d;       
        PD_CR1 |= 0X9d;       
        PD_CR2  = 0X00;   //����RTC
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
�������ƣ�LCD5110_Send_Data
�������ܣ�ģ��SPI�ӿ�ʱ��д����LCD
��ڲ�����Byte ��д�������
***********************************************************/
void LCD5110_Send_Data(unsigned char Byte)
{
    unsigned char i;
	
    LCD5110_ce = 0;		//5110Ƭѡ��Ч��������������
    LCD5110_dc = 1;	        //д����
	
    for(i=0;i<8;i++)		//����8bit����
    { 
        if(Byte&0x80)
     	   LCD5110_din = 1;
		else
       		LCD5110_din = 0;

        LCD5110_clk = 0;
        Byte = Byte<<1;
        LCD5110_clk = 1;
    }
	
    LCD5110_ce = 1;		//��ֹ5110
}

/*********************************************************** 
�������ƣ�LCD5110_Send_Commond
�������ܣ�ģ��SPI�ӿ�ʱ��д����LCD
��ڲ�����Byte    ��д�������
***********************************************************/
void LCD5110_Send_Commond(unsigned char Byte)
{
    unsigned char i;

    LCD5110_ce = 0;	        //5110Ƭѡ��Ч��������������
    LCD5110_dc = 0;	        //д����
	
    for(i=0;i<8;i++)		//����8bit����
    { 
        if(Byte&0x80)
        	LCD5110_din = 1;
		else
			LCD5110_din = 0;
	
		LCD5110_clk = 0;
		Byte = Byte << 1;
		LCD5110_clk = 1;
    }
	
    LCD5110_ce = 1;			//��ֹ5110
}

/*********************************************************** 
�������ƣ�LCD5110_Set_XY
�������ܣ�����LCD���꺯��
��ڲ������� X��0��83
          �� Y��0��5
***********************************************************/
void LCD5110_Set_XY(unsigned char x, unsigned char y)
{
    LCD5110_Send_Commond(y | 0x40);
    LCD5110_Send_Commond(x | 0x80);
}

/*********************************************************
�������ƣ�void LCD5110_clear 
�������ܣ�LCD5110����
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
�������ƣ�LCD5110_Init
�������ܣ�LCD5110��ʼ����������IO�ĳ�ʼ��
***********************************************************/
void LCD5110_Init(void)
{
    LCD5110_GPIO_Init();
	
    LCD5110_rst = 0;
    LCD5110_rst = 1;
    LCD5110_ce = 1;
    LCD5110_ce = 0;
	
    LCD5110_Send_Commond( 0x21 );  	// ʹ����չ��������LCDģʽ
    LCD5110_Send_Commond( 0xC8 ); 	// ����Һ��ƫ�õ�ѹ
    LCD5110_Send_Commond( 0x06 ); 	// �¶�У��
    LCD5110_Send_Commond( 0x13 );	//  1:48
    LCD5110_Send_Commond( 0x20 );	// ʹ�û������V=0��ˮƽѰַ
	
	/*������ʾģʽ*/
//	LCD5110_Send_Commond( 0x08 );	// ��ʾ�հ�	
//	LCD5110_Send_Commond( 0x09 );	// ��ʾ��������
	LCD5110_Send_Commond( 0x0C );	// ��ͨģʽ	
//	LCD5110_Send_Commond( 0x0D );	// ����ģʽ	
   
    LCD5110_Clear();   				//����Ļ
    LCD5110_Set_XY(0,0);   			//ָ��ع����Ͻ�
}

/*********************************************************** 
�������ƣ�LCD5110_Put_Char
�������ܣ����ASCII������ַ�
��ڲ�����x       ��0-83
          y       ��0-5
          ASCII   ���ַ���ASCII�루����ziku.h�и��ַ���ֵ��32��
***********************************************************/
void LCD5110_Put_Char_6_8(unsigned char x, unsigned char y, unsigned char z, unsigned char ASCII)
{ 
    unsigned char i; 
    unsigned int index; 
  
    index = ASCII - 32;			//��ģ�������ɿո�' '��ʼ,�ո��ASCII��ֵ����32
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
�������ƣ�LCD5110_PUT_String
�������ܣ���ʾ�ַ���
��ڲ�����x       ��0-83
          y       ��0-5
          *string ��Ҫ��ʾ���ַ���
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
�������ƣ�LCD5110_Put_Char
�������ܣ����ASCII������ַ�
��ڲ�����x       ��0-83
          y       ��0-5
          ASCII   ���ַ���ASCII�루����ziku.h�и��ַ���ֵ��32��
***********************************************************/
void LCD5110_Put_Number_6_8(unsigned char x, unsigned char y, unsigned char z, unsigned char number)
{
    unsigned char i; 
    unsigned int index; 
  
    index = number+48 -32;	//��ģ�������ɿո�' '��ʼ,�ո��ASCII��ֵ����32
							//���ֵ���ֵ��ASCIIֵ���48
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
* �� 6*12�������ʽ��ʾ�ַ�
*********************************************/
void LCD5110_Put_Char_6_12(unsigned char x, unsigned char y, unsigned char z, unsigned char Index)
{ 
    unsigned char i, j; 
	
	Index *= 2;				//ÿ���ַ������У��������е�����ʵ�ʻ�Ҫ x2
	for(j=0;j<2;j++)		//ÿ���ַ������У�����Ҫѭ����ʾ����
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
* �� 8*16�������ʽ��ʾ�ַ�
*********************************************/
void LCD5110_Put_Char_8_16(unsigned char x, unsigned char y, unsigned char z, unsigned char Index)
{ 
    unsigned char i, j; 
	
	Index *= 2;				//ÿ���ַ������У��������е�����ʵ�ʻ�Ҫ x2
	for(j=0;j<2;j++)		//ÿ���ַ������У�����Ҫѭ����ʾ����
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
�� 12*12������ʽ��ʾ����
***********************************************************/
void LCD5110_Put_Chinese_12_12(unsigned char x, unsigned char y, unsigned char z, unsigned char Chinese_Index)
{ 
    unsigned char i, j; 
	
	Chinese_Index *= 2;		//ÿ�������������ַ����������е�����ʵ�ʻ�Ҫ x2
	for(j=0;j<2;j++)		//ÿ�������������ַ�������Ҫѭ����ʾ����
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
�� 16*16������ʽ��ʾ����
***********************************************************/
void LCD5110_Put_Chinese_16_16(unsigned char x, unsigned char y, unsigned char z, unsigned char Chinese_Index)
{ 
    unsigned char i, j; 
	
	Chinese_Index *= 2;		//ÿ�������������ַ����������е�����ʵ�ʻ�Ҫ x2
	for(j=0;j<2;j++)		//ÿ�������������ַ�������Ҫѭ����ʾ����
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



