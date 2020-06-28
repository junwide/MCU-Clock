#ifndef _LCD5110_H__
#define _LCD5110_H__

/*****************************************
		LCD5110  º¯ÊýÉùÃ÷
******************************************/
void LCD5110_GPIO_Init(void);
void LCD5110_Init(void);
void LCD5110_Send_Data(unsigned char );
void LCD5110_Send_Commond(unsigned char );
void LCD5110_Set_XY(unsigned char ,unsigned char );
void LCD5110_Clear(void);

void LCD5110_Put_Char_6_8(unsigned char ,unsigned char ,unsigned char, unsigned char );
void LCD5110_Put_String_6_8(unsigned char ,unsigned char ,unsigned char, unsigned char *);
void LCD5110_Put_Number_6_8(unsigned char ,unsigned char ,unsigned char, unsigned char );

void LCD5110_Put_Char_6_12(unsigned char ,unsigned char ,unsigned char, unsigned char );
void LCD5110_Put_Char_8_16(unsigned char ,unsigned char ,unsigned char, unsigned char );

void LCD5110_Put_Chinese_12_12(unsigned char ,unsigned char ,unsigned char, unsigned char );
void LCD5110_Put_Chinese_16_16(unsigned char ,unsigned char ,unsigned char, unsigned char );


#endif