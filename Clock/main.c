#include "iostm8s105c6.h"
#include "LCD5110.h"
#include "RW.h"
#include "RTC.h"

extern uchar ir_code[4];                               //存放解码后的数据
extern uchar receive_over;                           //判断数据是否接受完成
extern uchar time_dec[8];
//char time[16] = {0};
char temp[2] = {0} ;
uchar map_i[4]= {30,36,48,54};
uchar EEP_Save_buffer[4] = {0,0,0,0}; // @0x4000;   //需要保存闹钟1 的数据的数组，地址为EEPROM的地址
uchar EEP_Save_2[4]  @0x4009;   //需要保存闹钟2 的数据的数组，地址为EEPROM的地址  A 类
uchar EEP_Save_3[7]  @0x4013;   //需要保存闹钟3 的数据的数组，地址为EEPROM的地址
//uchar mode_status    @0x4000;//存eep
uchar *EEP_arry[2];

uchar mode_status = 0; //存eep
uchar alarm_index = 0;


uchar Set_alarm();
char* Int2String(int num,char *str);
uchar LCD_Display_menu();
void  LCD_show_time(uchar mode_status);
void  Alarm_LCD_show_time(uchar flag);
void  Save_to_EEP(uchar* arry,uchar index);
void show_now_alarm(void);

uchar getBit(int num, int i){
    return ((num & (1 << i)) != 0);
}


main()
{
  RW_GPIO_Init();			
  time2_init(256);              
  EXTI_Init();
  asm("rim");                                   //打开总中断
  
  
  LCD5110_Init();
  LCD5110_Clear();

  ir_code[0]=0x00;
  ir_code[1]=0xff;
  ir_code[2]=0xff;
  
  EEP_arry[0] = EEP_Save_2;  //时间存储
  EEP_arry[1] = EEP_Save_3;  // 时间存储
  

  //mode_status = 1;
  
  PD_ODR_ODR4 = 1;
 // EEP_Save_buffer[7]={0,0,0,0}
  while(1)
 {
    uchar menu_ = LCD_Display_menu(mode_status);
    switch (menu_)
    {
      case 1:  show_now_alarm(); break;
      case 2:  break;
      case 4:  Set_alarm(); break;
   
    }
  }
}
void show_now_alarm(void)
{
     LCD5110_Clear(); 
     LCD5110_Put_String_6_8(0,0,0,"ACTIVE TIME");
     LCD5110_Put_String_6_8(0,1,0,"One:  :  ");
     LCD5110_Put_String_6_8(0,2,0,"Two:  :  ");
     LCD5110_Put_String_6_8(0,3,0,"ALarm:  ");
    
    for(int a = alarm_index ;a>=0;a--)
    {
    LCD5110_Put_String_6_8(24,1+a,0,Int2String(EEP_arry[a][0],temp));
    LCD5110_Put_String_6_8(30,1+a,0,Int2String(EEP_arry[a][1],temp));
    LCD5110_Put_String_6_8(42,1+a,0,Int2String(EEP_arry[a][2],temp));
    LCD5110_Put_String_6_8(48,1+a,0,Int2String(EEP_arry[a][3],temp));
    }
   if(mode_status == 1) LCD5110_Put_String_6_8(36,3,0,"on ");
   else LCD5110_Put_String_6_8(36,3,0,"off");
    while(1)
   {
     if(receive_over==1)	                        //数据接收完成后再执行解码及显示函数
    {		               
      receive_over=0;                       
      decode();                                  //进行解码
      if(ir_code[2] == 0x43) 
      {
        return; //return 
      }
    }
   }
}

void  Save_to_EEP(uchar* arry,uchar index)
{
    FLASH_CR1 |=0X00; //查询方式检测EEPROM编程结束标志，系统自动安排编程时间
    if(!(FLASH_IAPSR & 0x08))   //EEPROM还没有被解锁
    {
      FLASH_DUKR =0xAE;
      FLASH_DUKR =0x56;  //连续写两个密钥对其进行解锁
     }
    while(!(FLASH_IAPSR & 0x08));//判断是否已经解锁
    for(int b=0;b<4;b++)
   {
     EEP_arry[index][b] = arry[b];
    // EEP_Save_2[b]= arry[b];
    }
    while(!(FLASH_IAPSR & 0X04));   //判断编程是否结束
    FLASH_IAPSR &= 0X00;            //清零DUL，恢复写保护
}


uchar Set_alarm()
{
     LCD5110_Clear(); 
     LCD5110_Put_String_6_8(0,0,0," GDUT TIME");
     LCD5110_Put_String_6_8(0,1,0,"When:00:00");
     LCD5110_Put_String_6_8(0,2,0,"ALarm:off");
     LCD5110_Put_String_6_8(0,3,0,"Comfrim:");
     LCD5110_Put_String_6_8(54,3,0,"OK");
     
     uchar alarm_flag = 1;
     uchar alarm_number = 0;
     while(1)
    {
      Alarm_LCD_show_time(alarm_flag);
     if(receive_over==1)	                        //数据接收完成后再执行解码及显示函数
    {		               
      receive_over=0;                       
      decode();                                  //进行解码
      if(ir_code[2] == 0x43)
      {
        mode_status = 1;
        Save_to_EEP(EEP_Save_buffer,alarm_index);
        LCD5110_Clear(); 
         return 0; //return 
      }
      else if(ir_code[2] == 0x15)
      {
          Save_to_EEP(EEP_Save_buffer,alarm_index);
          alarm_index++;
      }
      else{
                if(ir_code[2] == 0x44 || ir_code[2] == 0x40)
              {
                if(ir_code[2] == 0x44 && alarm_flag == 1)  alarm_flag = 8;
                else if(ir_code[2] == 0x40 && alarm_flag == 8) alarm_flag = 1;
                else if(ir_code[2] == 0x44) alarm_flag = alarm_flag >> 1;
                else alarm_flag = alarm_flag << 1;
              }
                else{
                   alarm_number = get_key(ir_code[2]);
                   for(int i = 0; i < 4;i++)
                   {
                     if(getBit(alarm_flag,i))
                     {
                       EEP_Save_buffer[i] = alarm_number;
                     }
                   }
                }
                
      }
      
   }
     
  }
}
void  Alarm_LCD_show_time(uchar flag)
{
    LCD5110_Put_String_6_8(30,1,getBit(flag,0),Int2String(EEP_Save_buffer[0],temp));
    LCD5110_Put_String_6_8(36,1,getBit(flag,1),Int2String(EEP_Save_buffer[1],temp));
    LCD5110_Put_String_6_8(48,1,getBit(flag,2),Int2String(EEP_Save_buffer[2],temp));
    LCD5110_Put_String_6_8(54,1,getBit(flag,3),Int2String(EEP_Save_buffer[3],temp));
}




void  LCD_show_time(uchar mode_status)
{
  get_now_time();
  
  LCD5110_Put_String_6_8(36,1,0,Int2String(time_dec[0],temp));
  LCD5110_Put_String_6_8(42,1,0,Int2String(time_dec[1],temp));
 
  
  LCD5110_Put_String_6_8(54,1,0,Int2String(time_dec[3],temp));
  LCD5110_Put_String_6_8(60,1,0,Int2String(time_dec[4],temp));
  
  LCD5110_Put_String_6_8(72,1,0,Int2String(time_dec[5],temp));
  LCD5110_Put_String_6_8(78,1,0,Int2String(time_dec[6],temp));
  
  
  LCD5110_Put_String_6_8(30,2,0,Int2String(time_dec[7],temp));
  LCD5110_Put_String_6_8(36,2,0,Int2String(time_dec[8],temp));
  
  LCD5110_Put_String_6_8(48,2,0,Int2String(time_dec[9],temp));
  LCD5110_Put_String_6_8(54,2,0,Int2String(time_dec[10],temp));
  
  LCD5110_Put_String_6_8(66,2,0,Int2String(time_dec[11],temp));
  LCD5110_Put_String_6_8(72,2,0,Int2String(time_dec[12],temp));
  
  if(mode_status == 1) 
  {
    LCD5110_Put_String_6_8(36,3,0,"on ");
    for(int i=alarm_index;i>=0;i--)
      for(int b = 0 ; b <4; b++)
      {
        if(EEP_arry[i][b] != time_dec[7+b]) break;
        else if(b==3) PD_ODR_ODR4 = 0;  //
      }
  }
  else LCD5110_Put_String_6_8(36,3,0,"off");
}


uchar LCD_Display_menu()
{
  uchar menu_flag = 1;
    
  LCD5110_Put_String_6_8(0,0,0," GDUT TIME");
  LCD5110_Put_String_6_8(0,1,0,"Day:20  /  /  ");
  LCD5110_Put_String_6_8(0,2,0,"Time:  :  :  ");
  LCD_show_time(mode_status);
  LCD5110_Put_String_6_8(0,3,0,"ALarm:   ");
  LCD5110_Put_String_6_8(0,4,0,"      SET");
  LCD5110_Put_String_6_8(0,5,1,"Act");
  LCD5110_Put_String_6_8(24,5,0,"Time");
  LCD5110_Put_String_6_8(54,5,0,"ALarm");
    while(1)
  {
    if(receive_over==1)	                        //数据接收完成后再执行解码及显示函数
    {		               
      receive_over=0;                       
      decode();                                  //进行解码
      if(ir_code[2] == 0x44 || ir_code[2] == 0x40)
      {
        if(ir_code[2] == 0x44 && menu_flag == 1)  menu_flag = 4;
        else if(ir_code[2] == 0x40 && menu_flag == 4) menu_flag = 1;
        else if(ir_code[2] == 0x44) menu_flag = menu_flag >> 1;
        else menu_flag = menu_flag << 1;
        
        LCD5110_Put_String_6_8(0,5,getBit(menu_flag,0),"Act");
        LCD5110_Put_String_6_8(24,5,getBit(menu_flag,1),"Time");
        LCD5110_Put_String_6_8(54,5,getBit(menu_flag,2),"ALarm");
      }
      else if(ir_code[2] == 0x46)
      {
         mode_status = ~mode_status;
         PD_ODR_ODR4 = 1;
      }
      else if(ir_code[2] == 0x43)
      {
         return menu_flag;
      }  
   }
   LCD_show_time(mode_status);
  }
}

char* Int2String(int num,char *str)//10进制 
{
    int i = 0;//指示填充str 
    if(num<0)//如果num为负数，将num变正 
    {
        num = -num;
        str[i++] = '-';
    } 
    //转换 
    do
    {
        str[i++] = num%10+48;//取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0' 
        num /= 10;//去掉最低位    
    }while(num);//num不为0继续循环
    
    str[i] = '\0';
    
    //确定开始调整的位置 
    int j = 0;
    if(str[0]=='-')//如果有负号，负号不用调整 
    {
        j = 1;//从第二位开始调整 
        ++i;//由于有负号，所以交换的对称轴也要后移1位 
    }
    //对称交换 
    for(;j<i/2;j++)
    {
        //对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b; 
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    } 
    
    return str;//返回转换后的值 
}



