#include "delay.h"

/*****1ms�Ĵ������ʱ����*****/
void delay_ms(unsigned int num)
{
	unsigned int i, j;
	for(i=0;i<num;++i)
		for(j=0;j<mscount;++j);
}

/*****10us�Ĵ������ʱ����*****/
/*
void delay_10us(unsigned int num)
{
	unsigned int i, j;
	for(i=0;i<num;++i)
		for(j=0;j<uscount;++j);
}
*/

/*****2M������Ƶ�ʵ�10us�������ʱ����******/
void delay_10us(unsigned int num)
{
	unsigned int i;
	for(i=0;i<num;++i)
		;
}
void delay_s(unsigned int i)
{
    unsigned char k;
    for(;i>0;i--)
      for(k=0;k<100;k++);
}
