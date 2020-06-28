
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
//音乐数据，三个数据为一组，其中第一个数据表示音调，第二个数据表示音调高低，其中1为低8度，2为正常，3为高8度
//第三个数据表示节拍，即每个音调持续的时间长度

uint FREQH[]={
                                3823,3404,3034,2865,2551,2273,2024, 
                                1912,1702,1517,1433,1276,1137,1012, //1,2,3,4,5,6,7,8,i
                                956,851,759,717,638,569,506                           
                               } ;
//音阶频率表，根据每个音调的频率（网上可查），以及定时器的频率计算出来的

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
    TIM2_PSCR=1;            //定时器频率为1MHZ
    TIM2_CCMR1_OC1M=0X07;   //PWM2模式
    TIM2_CCER1_CC1E=1;      //使能TIM2通道1输出   
    TIM2_CR1_CEN=1;        //使能定时器
}

void  Time3_Init(void)
{
    TIM3_PSCR_PSC=12;         //2/2^11=1/2KHz,计数器为1时定时时间约2ms
    TIM3_IER_UIE=1;           //允许更新中断
    TIM3_ARRH=1000/256;
    TIM3_ARRL=1000%256;       //启动1S后开始播放音乐
    TIM3_CR1_CEN=1;           //使能定时器3
    TIM3_EGR_UG=1;             //允许产生更新中断
}


#pragma vector=17
__interrupt void Time3_IR_Handle(void)
{
    TIM3_SR1_UIF=0;                   //清除中断标志
   
    TIM2_CR1_CEN=0;
    Music_Freq=Music[Music_Num]+7*Music[Music_Num+1]-1; //计算音符频率
    TIM2_ARRH=FREQH[Music_Freq]/256;
    TIM2_ARRL=FREQH[Music_Freq]%256;
     
    PWM=FREQH[Music_Freq]/8;
    TIM2_CCR1H=PWM/256;                   //PWM占空比为%20
    TIM2_CCR1L=PWM%256;
    TIM2_CR1_CEN=1;
   
    TIM3_CR1_CEN=0;  
    Music_time=Music[Music_Num+2]*128;         //计算音符之间间隔时间
    TIM3_ARRH=Music_time/256;
    TIM3_ARRL=Music_time%256;
    TIM3_CR1_CEN=1;  
    Music_Num=Music_Num+3;
    if(Music_Num>74)
      Music_Num=0;
}