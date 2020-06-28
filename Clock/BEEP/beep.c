
#include <iostm8s105c6.h>
#define uchar unsigned char
#define uint unsigned int
#define Beep PD_ODR_ODR4 

uchar Music_Num=0,Music_Freq;
uint Music_time,PWM;
uint t=1000;
uchar Music[]={5,1,1, 5,1,1, 6,1,2, 5,1,2, 1,2,2, 7,1,4,
         5,1,1, 5,1,1, 6,1,2, 5,1,2, 2,2,2, 1,2,4,
         5,1,1, 5,1,1, 5,2,2, 3,2,2, 1,2,2, 7,1,2, 6,1,2,
         4,2,1, 4,2,1, 3,2,2, 1,2,2, 2,2,2, 1,2,4};
//�������ݣ���������Ϊһ�飬���е�һ�����ݱ�ʾ�������ڶ������ݱ�ʾ�����ߵͣ�����1Ϊ��8�ȣ�2Ϊ������3Ϊ��8��
//���������ݱ�ʾ���ģ���ÿ������������ʱ�䳤��

uint FREQH[]={
                                3823,3404,3034,2865,2551,2273,2024, 
                                1912,1702,1517,1433,1276,1137,1012, //1,2,3,4,5,6,7,8,i
                                956,851,759,717,638,569,506                           
                               } ;
//����Ƶ�ʱ�����ÿ��������Ƶ�ʣ����Ͽɲ飩���Լ���ʱ����Ƶ�ʼ��������

void  Time2_Init(void);
void  Time3_Init(void);
void main( void )
{
   Time2_Init();
   Time3_Init();
   asm("RIM");
   while(1)
   {
      __interrupt void Time3_IR_Handle();
   }
}

void  Time2_Init(void)
{
    TIM2_PSCR=1;            //��ʱ��Ƶ��Ϊ1MHZ
    TIM2_CCMR1_OC1M=0X07;   //PWM2ģʽ
    TIM2_CCER1_CC1E=1;      //ʹ��TIM2ͨ��1���   
    TIM2_CR1_CEN=1;        //ʹ�ܶ�ʱ��
}

void  Time3_Init(void)
{
    TIM3_PSCR_PSC=12;         //2/2^11=1/2KHz,������Ϊ1ʱ��ʱʱ��Լ2ms
    TIM3_IER_UIE=1;           //��������ж�
    TIM3_ARRH=1000/256;
    TIM3_ARRL=1000%256;       //����1S��ʼ��������
    TIM3_CR1_CEN=1;           //ʹ�ܶ�ʱ��3
    TIM3_EGR_UG=1;             //������������ж�
}


#pragma vector=17
__interrupt void Time3_IR_Handle(void)
{
    TIM3_SR1_UIF=0;                   //����жϱ�־
   
    TIM2_CR1_CEN=0;
    Music_Freq=Music[Music_Num]+7*Music[Music_Num+1]-1; //��������Ƶ��
    TIM2_ARRH=FREQH[Music_Freq]/256;
    TIM2_ARRL=FREQH[Music_Freq]%256;
     
    PWM=FREQH[Music_Freq]/8;
    TIM2_CCR1H=PWM/256;                   //PWMռ�ձ�Ϊ%20
    TIM2_CCR1L=PWM%256;
    TIM2_CR1_CEN=1;
   
    TIM3_CR1_CEN=0;  
    Music_time=Music[Music_Num+2]*128;         //��������֮����ʱ��
    TIM3_ARRH=Music_time/256;
    TIM3_ARRL=Music_time%256;
    TIM3_CR1_CEN=1;  
    Music_Num=Music_Num+3;
    if(Music_Num>74)
      Music_Num=0;
}