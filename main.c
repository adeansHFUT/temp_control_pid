#include <STC12C5A60S2.h>
#include <intrins.h>
#include<stdio.h>
#include<math.h>
#include"lanya.h"
#include"pid.h"
#include"ds18b20.h"
//#include"E2PROM.h"
#include"lcd.h"
#include"font.h"
#include"sys.h"

/***************ע������************************/
/*
1�� �������кŶ�ȡ�󣬲�Ҫ������������������¼�����к�
	�����ڶ���ģʽ����ˮ����ȷ�Ĺرջ��.	������߰�������E2PROM
                        */
/***************io������************************/

//�������(motor_control)
sbit MC_hui=P2^0;	 //��ˮ���� 
sbit MC_right=P2^1;	  //�ұ�
sbit MC_left=P2^2;  //���
//������(motor_port)
#define M_PORT P1 //ֻʹ�õ���λ

//����
sbit k1 =P3^2;
sbit k2 =P3^3;
sbit k3 =P3^4;


/***************ȫ�ֱ�������************************/
uchar code CW[8] ={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};   //��ʱ����ת�����
uint T0_num=0;	       //���ڶ�ʱ��0����
uint T1_num=0;
uint T0_motor_num=0;   //���ڵ������Ƶ�ʼ�ʱ
float tar_temp=0;   //Ĭ��Ŀ���¶�
float cur_temp=0;       //��ǰ�¶�
char lanya_get=0;		 //��������¶�,�ַ�'2'Ϊ50	 '<'Ϊ60,'F'Ϊ70��'P'Ϊ80
uchar yemian=0;			 //�û������־��1Ϊѡ�񼾽�ҳ�棬2Ϊѡ���¶�ҳ��
uchar flag_season=0;	//1Ϊ������2Ϊ�ļ�
char flag_hui=0;      //�����������������־,1Ϊ��ʱ�������2Ϊ˳ʱ�����
static uint motor1_xulie=22000;	  //���˳ʱ��ת����������
static uint motor2_xulie=22000;	  //22000��ʱ������ȫ�أ��ҵ��ȫ��
static uint hui_xulie=2200;	   //����ȫ��
uchar update_flag=0;
uchar flag_exit=0;
/*//����Ϊ����test
char code str2[]="current : ";
char temperature[10];
 
char test1[10];

uint xulie=0;
char test2[10];
*/
char tar_tem[10];
/***************��������************************/
void delay(uint xms);	 //���뼶�����ʱ
void T0_init();          //��ʱ��T0��ʼ�� 1ms
void T1_init();			 //��ʱ��T1��ʼ�� 20ms 
void season();            //ѡ�񼾽�ҳ��
void winter();			   //��������
void summer();			   //�ļ�����
void send_init();        //�������͵�ǰ�¶ȡ���ʾ�û������¶�
void motor_init();       //�����ʼ������ ����mc1 mc2 mc3

void lcd_show_season();	   //ѡ�񼾽�
void lcd_clr_season();

void lcd_show_open();	   //�����û���ˮ
void lcd_clr_open();

void lcd_show_info();	   //��ʾ��ǰ�¶�  
void lcd_clr_info();        
void lcd_update_tmp();    //�¶ȸ���


/***************������************************/
void main()
{	 
  motor_init();
  Lcd_Init();
  LCD_Clear(BLACK);
  //welcome
//	motor1_xulie=0;
//	motor2_xulie=0;
//  motor1_xulie=word_read(0x2000);	 //һ���ȶ����ϴζϵ�ʱд������кţ�Ϊwinter׼���������⻹δ�����ͱ���ʱд���򸲸�

//  lanya_init();		//bluetooth��ʼ��;

  cur_temp=(float)ds_gettmp()/10;	 //��ȡ��ǰ�¶�

//  while(lanya_get==0);	  //�ȴ��û����뿪��ָ��
	 
  T0_init();		//������10ms���һ�Σ������(time_out ms����һ������)
  T1_init();		//tft��Ļ��ʼ��,���ܺ�����һ����
//  sprintf(test2,"%d",motor1_xulie);
//  uart_send(test2);
  season();	   //�ȴ�ѡ�񼾽�winter/summer
  if(flag_season==1)
  	 winter();
  else
   	 summer();
//   while(motor2_xulie<22000) //�����ҵ����ʱ�����(��ˮȫ��)
//	{
//	    update_flag=1;//update temp. flag
//		
//		time_out_flag=2; 
//		time_out=1;
//	}
//	time_out=15000;    //�û����޴�״̬
////	flag_hui=1;  //������ˮ��


//	hui_xulie=0;
//	flag_hui=2;
  while(1);

  
}

void lcd_show_season()
{
  //��ʾ���
  LCD_ShowString(5*8,0*16,"Select");
  LCD_ShowString(6*8,1*16+8,"the");
  LCD_ShowString(5*8,3*16,"current");
  LCD_ShowString(5*8,4*16+8,"season");
  LCD_ShowString(5*8,6*16,"please");
  //�ָ���
  LCD_DrawLine(0+4,9*16-3,127-4,9*16-3);
  LCD_DrawLine(5*8+4,9*16-3,5*8+4,160);
  LCD_DrawLine(10*8+4,9*16-3,10*8+4,160);
  LCD_DrawLine(0+4,9*16-3,0+4,160);
  LCD_DrawLine(127-4,9*16-3,127-4,160);
  //��ť������ʾ
  LCD_ShowString(1*8,9*16,"NULL");
  LCD_ShowString(6*8,9*16,"SUM.");
  LCD_ShowString(11*8,9*16,"WIN.");
}

void lcd_clr_season()
{
  LCD_ShowString(5*8,0*16,"      ");
  LCD_ShowString(6*8,1*16+8,"   ");
  LCD_ShowString(5*8,3*16,"       ");
  LCD_ShowString(5*8,4*16+8,"      ");
  LCD_ShowString(5*8,6*16,"      ");

  LCD_ShowString(1*8,9*16,"    ");
  LCD_ShowString(6*8,9*16,"    ");
  LCD_ShowString(11*8,9*16,"    ");	 	
}

void lcd_show_open()
{
  LCD_ShowString(6*8,0*16,"You");
  LCD_ShowString(6*8,1*16+8,"can");
  LCD_ShowString(6*8,3*16,"open");
  LCD_ShowString(6*8,4*16+8,"the");
  LCD_ShowString(5*8,6*16,"vavle");
  
  //��ť������ʾ
  LCD_ShowString(1*8,9*16,"NULL");
  LCD_ShowString(6*8,9*16,"NULL");
  LCD_ShowString(11*8,9*16,"NULL");  
}
void lcd_clr_open()
{
  LCD_ShowString(6*8,0*16,"   ");
  LCD_ShowString(6*8,1*16+8,"   ");
  LCD_ShowString(6*8,3*16,"    ");
  LCD_ShowString(6*8,4*16+8,"   ");
  LCD_ShowString(5*8,6*16,"     ");
  
  //��ť������ʾ
  LCD_ShowString(1*8,9*16,"NULL");
  LCD_ShowString(6*8,9*16,"NULL");
  LCD_ShowString(11*8,9*16,"NULL");
}
void lcd_show_info()
{
  LCD_ShowString(0,0*16,"target TEMP. :");
  LCD_ShowString(4*8,1*16+8,"    ");LCD_ShowChar(8*8+4,16+8-7,'.',0);LCD_ShowChar(9*8,16+8,'C',0);
 // LCD_ShowString(0,3*16,"Too High");
  LCD_ShowString(0,5*16,"current TEMP. :");
  LCD_ShowString(4*8,6*16+8,"    ");LCD_ShowChar(8*8+4,6*16-7+8,'.',0);LCD_ShowChar(9*8,6*16+8,'C',0);
  
  //��ť������ʾ
  LCD_ShowString(1*8,9*16,"CLS.");
  LCD_ShowString(6*8,9*16," -  ");
  LCD_ShowString(11*8,9*16,"  + ");
}

void lcd_clr_info()
{

  LCD_ShowString(0,0*16,"              ");
  LCD_ShowString(4*8,1*16+8,"    ");LCD_ShowChar(8*8+4,16+8-7,' ',0);LCD_ShowChar(9*8,16+8,' ',0);
  LCD_ShowString(0,3*16,"        ");
  LCD_ShowString(0,5*16,"               ");
  LCD_ShowString(4*8,6*16+8,"    ");LCD_ShowChar(8*8+4,6*16-7+8,' ',0);LCD_ShowChar(9*8,6*16+8,' ',0);
  
  //��ť������ʾ
  LCD_ShowString(1*8,9*16,"    ");
  LCD_ShowString(6*8,9*16,"    ");
  LCD_ShowString(11*8,9*16,"    ");
}



void delay(uint xms)
{
  while(--xms)
  {
	unsigned char i, j;
	_nop_();
	i = 11;
	j = 100;
	do
	{
		while (--j);
	} while (--i);	
  }
}

void T0_init()
{
  AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ

  TMOD &= 0xF0;		//���ö�ʱ��ģʽ
  TMOD |= 0x01;		//���ö�ʱ��ģʽ

  TL0 = 0x66;		//���ö�ʱ��ֵ
  TH0 = 0xFC;		//���ö�ʱ��ֵ

  ET0 = 1; //����T0�ж�
  EA  = 1; //�����ж�
  TR0 = 1;
}

void T1_init()
{
  AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ

  TMOD &= 0x0F;		//���ö�ʱ��ģʽ
  TMOD |= 0x10;		//���ö�ʱ��ģʽ

  TL1 = 0x00;		//���ö�ʱ��ֵ
  TH1 = 0xB8;		//���ö�ʱ��

  ET1 = 1; //����T1�ж�
  EA  = 1; //�����ж�
  TR1 = 1;
}

/***************�жϴ������************************/
void T0_timer() interrupt 1			//1ms
{

  static uchar motor1_index=0;
  static uchar motor2_index=0;
  static uchar motor3_index=0;
  ET0=0;
 
  
  TL0 = 0x66;		//���ö�ʱ��ֵ
  TH0 = 0xFC;		//���ö�ʱ��ֵ
  ++T0_num;

  if(T0_num>=100)	//100ms
   {
	 T0_num=0;
//	 flag=1;
   }
/***************��������**************/
  if(T0_num%10==0) //��ռ��ʽ����10msˢ��һ��
  {
 // 		static unsigned char keylast3;
		if(yemian==1)	 //��ҳ��һʱ��������ѡ�񼾽�
		{
			static unsigned char keylast1,keylast2;	//�����ϴεİ���ֵ
			if(k1==0 && keylast1==1)	//��⵽һ���½���
			{
			    flag_season=1;	  //����
				tar_temp=45;	  //��ʼĿ���¶�Ϊ65
			}
			keylast1=k1;
			if(k2==0 && keylast2==1)	//��⵽һ���½���
			{
			    flag_season=2;			//�ļ�
				tar_temp=40;			//��ʼ�¶�Ϊ
			}
			keylast2=k2;
		}
		if(yemian==2||yemian==3)	 //��ҳ�����ʱ���������Ӽ��¶�
		{
			static unsigned char keylast1,keylast2,keylast3;	//�����ϴεİ���ֵ
			if(k1==0 && keylast1==1)	//��⵽һ���½���
			{
			    tar_temp++;
			}
			keylast1=k1;
			if(k2==0 && keylast2==1)	//��⵽һ���½���
			{
			    tar_temp--;
			}
			keylast2=k2;
			if(k3==0 && keylast3==1)	//��⵽һ���½���
			{
				flag_exit=1;
			}
			keylast3=k3;
				
		}

  }

/***************�����������**************/
  ++T0_motor_num;
  if(T0_motor_num>=time_out)
  {
    T0_motor_num=0;

	MC_left=1; //������
    M_PORT=CW[motor1_index];
	if(time_out_flagleft==2) //����Ϊ�� ���� ����ת
	{
     	 ++motor1_index;
		 motor1_xulie++;
		 if(motor1_xulie>22000)	  //��ֹ������������Ȼ��
		 	motor1_xulie=22000;
	}     
	if(time_out_flagleft==0)
	{
		  --motor1_index;   //
		  motor1_xulie--;
		  if(motor1_xulie>60000)
		  	 motor1_xulie=0;
	}
    MC_left=0;   //������

	MC_right =1; //������� 
	M_PORT=CW[motor2_index];
	if(time_out_flagright==0) //����Ϊ�� ������ ������ת
	{
	  ++motor2_index;
	  motor2_xulie++;
	  if(motor2_xulie>22000)
		 	motor2_xulie=22000;
	}     
	if(time_out_flagright==2)
	{
	  --motor2_index;
	  motor2_xulie--;
	  if(motor2_xulie>60000)
		  	 motor2_xulie=0;
	}    
	MC_right=0;   //���ҵ��	 


	//motor1_index�������
	if(motor1_index==8)
	  motor1_index=0;
	else if(motor1_index==255)
	  motor1_index=7;
    //motor2_index�������
	if(motor2_index==8)
	  motor2_index=0;
	else if(motor2_index==255)
	  motor2_index=7;
  }
  if(T0_num%1==0)
  {
	   if(flag_hui==1)	  //��ʱ��ȫ���������
		{
			MC_hui=1;
			M_PORT=CW[motor3_index];
			--motor3_index;
			hui_xulie--;
			MC_hui=0;
			if(hui_xulie==0||hui_xulie>60000)
				flag_hui=0;			
		}
		if(flag_hui==2)	  //˳ʱ��
		{
			MC_hui=1;
			M_PORT=CW[motor3_index];
			++motor3_index;
			hui_xulie++;
			MC_hui=0;
			if(hui_xulie>2200)
				flag_hui=0;			
		}
		if(motor3_index==8)
		  motor3_index=0;
		else if(motor3_index==255)
		  motor3_index=7;
    }
	ET0=1;

  
 }
void T1_timer() interrupt 3
{
  TL1 = 0x00;		//���ö�ʱ��ֵ
  TH1 = 0xB8;		//���ö�ʱ��

  ++T1_num;
  if(T1_num>=50)	//��������50*20ms
    T1_num=0;   
	
	
	if(T1_num%5==0)
		if(update_flag==1)
		   {
			 EA=0;
			 lcd_update_tmp();
			 EA=1;  
			}
} 

void motor_init()
{
  M_PORT = 0x00;

  MC_left=0;
  _nop_();
  MC_hui=0;
  _nop_();
  MC_right=0;
  _nop_();
}
void uart(void) interrupt 4		 //���ڽ����ж�
{

	 RI=0;  //���ж�����
	 lanya_get=SBUF;  	//ȡֵ
}
void send_init()
{   
    char code str1[]="please enter temperature: ";
//	sprintf(temperature,"%.1f",cur_temp);
//	uart_send(str2);
///	uart_send(temperature);
	uart_send(str1);
}

void season()
{
	lcd_show_season();
	yemian=1;	 //ҳ���־
	while(!flag_season);
}
void summer()	 //flag_season=2
{
//	uchar flag_wen=0;
//	float temp_last;
	yemian=3;
	lcd_clr_season();
	lcd_show_info();	
	while(motor1_xulie>11000&&motor1_xulie<=22000) //��������ת��һ�룬�ҵ����ת��һ��(����ˮ�����м�)
	{
	    update_flag=1;//update temp. flag
		cur_temp=(float)ds_gettmp()/10;
		time_out_flagleft=0;
		time_out_flagright=2; 
		time_out=1;
	}
	time_out=15000;
	flag_hui=1;  //������ˮ��
	while(flag_hui);
	
	while(1)
	{
	    
		update_flag=1;//update temp
		cur_temp=(float)ds_gettmp()/10;		
		temperature_control(tar_temp,cur_temp);
		if(cur_temp>=tar_temp-0.5&&cur_temp<=tar_temp+2)	//����0.5�����
			break;
	}
	flag_hui=2;//�ػ���ˮ��
	while(flag_hui);
	update_flag=0;//�ظ����¶ȱ�־
	lcd_clr_info();
	lcd_show_open();
	delay(1000);
	lcd_clr_open();
	lcd_show_info();
	time_out=15000;
	delay(1000);
    while(1)
	{
		update_flag=1;//update temp flag
		cur_temp=(float)ds_gettmp()/10;
//		flag_wen=1;
//		temp_last=cur_temp;
//		while(flag_wen)
//		{
//			delay(50);
//			cur_temp=(float)ds_gettmp()/10;	
//			if(abs(temp_last-cur_temp)<0.5)
//			{
//				flag_wen=0;
//			}
//			temp_last=cur_temp;
//		}
		temperature_control(tar_temp,cur_temp);		
		if(flag_exit==1)		//����3ֹͣ��ˮ
		{
			time_out_flagleft=2;
			time_out_flagright=0;
			  while(motor1_xulie<22000||motor2_xulie<22000) //����ʱ�����������ת�����ر�
			{
		    update_flag=1;//update temp. flag
			if(motor1_xulie==22000) 
				time_out_flagleft=1;	 //ת���׾Ͳ�ת
			if(motor2_xulie==22000) 
				time_out_flagright=1;	 //ת���׾Ͳ�ת
			time_out=1;
			}
			time_out=15000;
			break;
		}
	}
	while(1);

}
void winter()	//flag_season=1
{
//	uchar flag_wen=0;
//	float temp_last; 
	yemian=2;

	lcd_clr_season();
	lcd_show_info();

	while(motor1_xulie>11000&&motor1_xulie<=22000) 
	{
	    update_flag=1;//update temp. flag
		cur_temp=(float)ds_gettmp()/10;
		time_out_flagleft=0;   //������ת
		time_out_flagright=1; //�ҵ������ 
		time_out=1;
	}
	time_out=15000;    //�û����޴�״̬
	flag_hui=1;  //������ˮ��
	while(flag_hui);
	while(1)	  //�ȴ���ˮ�㹻��
	{
     	update_flag=1;//update flag
		cur_temp=(float)ds_gettmp()/10;
		if(cur_temp>=tar_temp&&cur_temp<tar_temp+2) 
			break;
	} 
	while(motor2_xulie>15000) //�ҵ����ת��Сһ��(�ص�����������λ�ã�Ҳ������pid������λ��)
	{
	    update_flag=1;//update temp. flag
		cur_temp=(float)ds_gettmp()/10;
		time_out_flagleft=1;	//������ת
		time_out_flagright=2; 	// �ҵ����ת
		time_out=1;
	}
	time_out=15000;
	while(1)	   
	{
		cur_temp=(float)ds_gettmp()/10;
		temperature_control(tar_temp,cur_temp);
		if(cur_temp>=tar_temp-0.5&&cur_temp<=tar_temp+0.5)	//����0.5�����
			break;
	}
	flag_hui=2;//�ػ���ˮ��
	while(flag_hui);

	update_flag=0;//�ظ����¶ȱ�־
	lcd_clr_info();
	lcd_show_open();   //�����û���ˮ
	delay(1000);
	lcd_clr_open();
	lcd_show_info();
	time_out=15000;
	delay(1000);
	while(1)
	{
		
	    update_flag=1;//update flag
		cur_temp=(float)ds_gettmp()/10;
//		flag_wen=1;
//		temp_last=cur_temp;
//		while(flag_wen)
//		{
//			delay(50);
//			cur_temp=(float)ds_gettmp()/10;	
//			if(abs(temp_last-cur_temp)<0.5)
//			{
//				flag_wen=0;
//			}
//			temp_last=cur_temp;
//		}
		temperature_control(tar_temp,cur_temp);
		if(flag_exit==1)		//����3ֹͣ��ˮ
		{
			time_out_flagleft=2;
			time_out_flagright=0;
			  while(motor1_xulie<22000||motor2_xulie<22000) //����ʱ�����������ת�����ر�
			{
		    update_flag=1;//update temp. flag
			if(motor1_xulie==22000) 
				time_out_flagleft=1;	 //ת���׾Ͳ�ת
			if(motor2_xulie==22000) 
				time_out_flagright=1;	 //ת���׾Ͳ�ת
			time_out=1;
			}
			time_out=15000;
			break;
		}
	}
	while(1);
}
void lcd_update_tmp()
{
  uchar buf[4];
  //����Ŀ���¶�
  sprintf(buf,"%.1f",tar_temp);
  LCD_ShowString(4*8,1*16+8,buf); 
  //���µ�ǰ�¶� 
  sprintf(buf,"%.1f",cur_temp);
  LCD_ShowString(4*8,6*16+8,buf);
}
