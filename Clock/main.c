#include "iostm8s105c6.h"
#include "LCD5110.h"
#include "RW.h"
#include "RTC.h"

extern uchar ir_code[4];                               //��Ž���������
extern uchar receive_over;                           //�ж������Ƿ�������
extern uchar time_dec[8];
//char time[16] = {0};
char temp[2] = {0} ;
uchar map_i[4]= {30,36,48,54};
uchar EEP_Save_buffer[4] = {0,0,0,0}; // @0x4000;   //��Ҫ��������1 �����ݵ����飬��ַΪEEPROM�ĵ�ַ
uchar EEP_Save_2[4]  @0x4009;   //��Ҫ��������2 �����ݵ����飬��ַΪEEPROM�ĵ�ַ  A ��
uchar EEP_Save_3[7]  @0x4013;   //��Ҫ��������3 �����ݵ����飬��ַΪEEPROM�ĵ�ַ
//uchar mode_status    @0x4000;//��eep
uchar *EEP_arry[2];

uchar mode_status = 0; //��eep
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
  asm("rim");                                   //�����ж�
  
  
  LCD5110_Init();
  LCD5110_Clear();

  ir_code[0]=0x00;
  ir_code[1]=0xff;
  ir_code[2]=0xff;
  
  EEP_arry[0] = EEP_Save_2;  //ʱ��洢
  EEP_arry[1] = EEP_Save_3;  // ʱ��洢
  

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
     if(receive_over==1)	                        //���ݽ�����ɺ���ִ�н��뼰��ʾ����
    {		               
      receive_over=0;                       
      decode();                                  //���н���
      if(ir_code[2] == 0x43) 
      {
        return; //return 
      }
    }
   }
}

void  Save_to_EEP(uchar* arry,uchar index)
{
    FLASH_CR1 |=0X00; //��ѯ��ʽ���EEPROM��̽�����־��ϵͳ�Զ����ű��ʱ��
    if(!(FLASH_IAPSR & 0x08))   //EEPROM��û�б�����
    {
      FLASH_DUKR =0xAE;
      FLASH_DUKR =0x56;  //����д������Կ������н���
     }
    while(!(FLASH_IAPSR & 0x08));//�ж��Ƿ��Ѿ�����
    for(int b=0;b<4;b++)
   {
     EEP_arry[index][b] = arry[b];
    // EEP_Save_2[b]= arry[b];
    }
    while(!(FLASH_IAPSR & 0X04));   //�жϱ���Ƿ����
    FLASH_IAPSR &= 0X00;            //����DUL���ָ�д����
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
     if(receive_over==1)	                        //���ݽ�����ɺ���ִ�н��뼰��ʾ����
    {		               
      receive_over=0;                       
      decode();                                  //���н���
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
    if(receive_over==1)	                        //���ݽ�����ɺ���ִ�н��뼰��ʾ����
    {		               
      receive_over=0;                       
      decode();                                  //���н���
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

char* Int2String(int num,char *str)//10���� 
{
    int i = 0;//ָʾ���str 
    if(num<0)//���numΪ��������num���� 
    {
        num = -num;
        str[i++] = '-';
    } 
    //ת�� 
    do
    {
        str[i++] = num%10+48;//ȡnum���λ �ַ�0~9��ASCII����48~57������˵����0+48=48��ASCII���Ӧ�ַ�'0' 
        num /= 10;//ȥ�����λ    
    }while(num);//num��Ϊ0����ѭ��
    
    str[i] = '\0';
    
    //ȷ����ʼ������λ�� 
    int j = 0;
    if(str[0]=='-')//����и��ţ����Ų��õ��� 
    {
        j = 1;//�ӵڶ�λ��ʼ���� 
        ++i;//�����и��ţ����Խ����ĶԳ���ҲҪ����1λ 
    }
    //�Գƽ��� 
    for(;j<i/2;j++)
    {
        //�Գƽ������˵�ֵ ��ʵ����ʡ���м��������a+b��ֵ��a=a+b;b=a-b;a=a-b; 
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    } 
    
    return str;//����ת�����ֵ 
}



