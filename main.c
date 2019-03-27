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

/***************注意事项************************/
/*
1、 增加序列号读取后，不要再用手掰电机，否则记录的序列号
	不能在冬季模式下让水管正确的关闭或打开.	按最左边按键记忆E2PROM
                        */
/***************io口声明************************/

//电机控制(motor_control)
sbit MC_hui=P2^0;	 //冷水回流 
sbit MC_right=P2^1;	  //右边
sbit MC_left=P2^2;  //左边
//电机输出(motor_port)
#define M_PORT P1 //只使用低四位

//按键
sbit k1 =P3^2;
sbit k2 =P3^3;
sbit k3 =P3^4;


/***************全局变量声明************************/
uchar code CW[8] ={0x09,0x01,0x03,0x02,0x06,0x04,0x0c,0x08};   //正时针旋转相序表
uint T0_num=0;	       //用于定时器0计数
uint T1_num=0;
uint T0_motor_num=0;   //用于电机脉冲频率计时
float tar_temp=0;   //默认目标温度
float cur_temp=0;       //当前温度
char lanya_get=0;		 //蓝牙获得温度,字符'2'为50	 '<'为60,'F'为70，'P'为80
uchar yemian=0;			 //用户界面标志，1为选择季节页面，2为选择温度页面
uchar flag_season=0;	//1为冬季，2为夏季
char flag_hui=0;      //回流电机正反打死标志,1为逆时针打死，2为顺时针打死
static uint motor1_xulie=22000;	  //电机顺时针转动序列增大
static uint motor2_xulie=22000;	  //22000的时候左电机全关，右电机全关
static uint hui_xulie=2200;	   //不用全开
uchar update_flag=0;
uchar flag_exit=0;
/*//以下为蓝牙test
char code str2[]="current : ";
char temperature[10];
 
char test1[10];

uint xulie=0;
char test2[10];
*/
char tar_tem[10];
/***************函数声明************************/
void delay(uint xms);	 //毫秒级软件延时
void T0_init();          //定时器T0初始化 1ms
void T1_init();			 //定时器T1初始化 20ms 
void season();            //选择季节页面
void winter();			   //冬季进程
void summer();			   //夏季进程
void send_init();        //蓝牙发送当前温度、提示用户输入温度
void motor_init();       //电机初始化程序 清零mc1 mc2 mc3

void lcd_show_season();	   //选择季节
void lcd_clr_season();

void lcd_show_open();	   //提醒用户开水
void lcd_clr_open();

void lcd_show_info();	   //显示当前温度  
void lcd_clr_info();        
void lcd_update_tmp();    //温度更新


/***************主程序************************/
void main()
{	 
  motor_init();
  Lcd_Init();
  LCD_Clear(BLACK);
  //welcome
//	motor1_xulie=0;
//	motor2_xulie=0;
//  motor1_xulie=word_read(0x2000);	 //一定先读出上次断电时写入的序列号（为winter准备），避免还未读出就被定时写程序覆盖

//  lanya_init();		//bluetooth初始化;

  cur_temp=(float)ds_gettmp()/10;	 //获取当前温度

//  while(lanya_get==0);	  //等待用户输入开启指令
	 
  T0_init();		//按键（10ms检测一次）、电机(time_out ms发送一次序列)
  T1_init();		//tft屏幕初始化,不能和蓝牙一起用
//  sprintf(test2,"%d",motor1_xulie);
//  uart_send(test2);
  season();	   //等待选择季节winter/summer
  if(flag_season==1)
  	 winter();
  else
   	 summer();
//   while(motor2_xulie<22000) //都左右电机逆时针打死(热水全开)
//	{
//	    update_flag=1;//update temp. flag
//		
//		time_out_flag=2; 
//		time_out=1;
//	}
//	time_out=15000;    //置回无限大状态
////	flag_hui=1;  //开回流水管


//	hui_xulie=0;
//	flag_hui=2;
  while(1);

  
}

void lcd_show_season()
{
  //提示语句
  LCD_ShowString(5*8,0*16,"Select");
  LCD_ShowString(6*8,1*16+8,"the");
  LCD_ShowString(5*8,3*16,"current");
  LCD_ShowString(5*8,4*16+8,"season");
  LCD_ShowString(5*8,6*16,"please");
  //分割线
  LCD_DrawLine(0+4,9*16-3,127-4,9*16-3);
  LCD_DrawLine(5*8+4,9*16-3,5*8+4,160);
  LCD_DrawLine(10*8+4,9*16-3,10*8+4,160);
  LCD_DrawLine(0+4,9*16-3,0+4,160);
  LCD_DrawLine(127-4,9*16-3,127-4,160);
  //按钮功能提示
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
  
  //按钮功能提示
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
  
  //按钮功能提示
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
  
  //按钮功能提示
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
  
  //按钮功能提示
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
  AUXR &= 0x7F;		//定时器时钟12T模式

  TMOD &= 0xF0;		//设置定时器模式
  TMOD |= 0x01;		//设置定时器模式

  TL0 = 0x66;		//设置定时初值
  TH0 = 0xFC;		//设置定时初值

  ET0 = 1; //允许T0中断
  EA  = 1; //开放中断
  TR0 = 1;
}

void T1_init()
{
  AUXR &= 0xBF;		//定时器时钟12T模式

  TMOD &= 0x0F;		//设置定时器模式
  TMOD |= 0x10;		//设置定时器模式

  TL1 = 0x00;		//设置定时初值
  TH1 = 0xB8;		//设置定时初

  ET1 = 1; //允许T1中断
  EA  = 1; //开放中断
  TR1 = 1;
}

/***************中断处理程序************************/
void T0_timer() interrupt 1			//1ms
{

  static uchar motor1_index=0;
  static uchar motor2_index=0;
  static uchar motor3_index=0;
  ET0=0;
 
  
  TL0 = 0x66;		//设置定时初值
  TH0 = 0xFC;		//设置定时初值
  ++T0_num;

  if(T0_num>=100)	//100ms
   {
	 T0_num=0;
//	 flag=1;
   }
/***************按键部分**************/
  if(T0_num%10==0) //非占用式按键10ms刷新一次
  {
 // 		static unsigned char keylast3;
		if(yemian==1)	 //在页面一时按键用作选择季节
		{
			static unsigned char keylast1,keylast2;	//保存上次的按键值
			if(k1==0 && keylast1==1)	//检测到一个下降沿
			{
			    flag_season=1;	  //冬季
				tar_temp=45;	  //初始目标温度为65
			}
			keylast1=k1;
			if(k2==0 && keylast2==1)	//检测到一个下降沿
			{
			    flag_season=2;			//夏季
				tar_temp=40;			//初始温度为
			}
			keylast2=k2;
		}
		if(yemian==2||yemian==3)	 //在页面二三时按键用作加减温度
		{
			static unsigned char keylast1,keylast2,keylast3;	//保存上次的按键值
			if(k1==0 && keylast1==1)	//检测到一个下降沿
			{
			    tar_temp++;
			}
			keylast1=k1;
			if(k2==0 && keylast2==1)	//检测到一个下降沿
			{
			    tar_temp--;
			}
			keylast2=k2;
			if(k3==0 && keylast3==1)	//检测到一个下降沿
			{
				flag_exit=1;
			}
			keylast3=k3;
				
		}

  }

/***************电机驱动部分**************/
  ++T0_motor_num;
  if(T0_motor_num>=time_out)
  {
    T0_motor_num=0;

	MC_left=1; //开左电机
    M_PORT=CW[motor1_index];
	if(time_out_flagleft==2) //条件为真 则降温 即右转
	{
     	 ++motor1_index;
		 motor1_xulie++;
		 if(motor1_xulie>22000)	  //防止打死后序列任然加
		 	motor1_xulie=22000;
	}     
	if(time_out_flagleft==0)
	{
		  --motor1_index;   //
		  motor1_xulie--;
		  if(motor1_xulie>60000)
		  	 motor1_xulie=0;
	}
    MC_left=0;   //关左电机

	MC_right =1; //开电机右 
	M_PORT=CW[motor2_index];
	if(time_out_flagright==0) //条件为真 则升温 即向右转
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
	MC_right=0;   //关右电机	 


	//motor1_index溢出处理
	if(motor1_index==8)
	  motor1_index=0;
	else if(motor1_index==255)
	  motor1_index=7;
    //motor2_index溢出处理
	if(motor2_index==8)
	  motor2_index=0;
	else if(motor2_index==255)
	  motor2_index=7;
  }
  if(T0_num%1==0)
  {
	   if(flag_hui==1)	  //逆时针全开回流电机
		{
			MC_hui=1;
			M_PORT=CW[motor3_index];
			--motor3_index;
			hui_xulie--;
			MC_hui=0;
			if(hui_xulie==0||hui_xulie>60000)
				flag_hui=0;			
		}
		if(flag_hui==2)	  //顺时针
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
  TL1 = 0x00;		//设置定时初值
  TH1 = 0xB8;		//设置定时初

  ++T1_num;
  if(T1_num>=50)	//最大计数到50*20ms
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
void uart(void) interrupt 4		 //串口接收中断
{

	 RI=0;  //清中断请求
	 lanya_get=SBUF;  	//取值
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
	yemian=1;	 //页面标志
	while(!flag_season);
}
void summer()	 //flag_season=2
{
//	uchar flag_wen=0;
//	float temp_last;
	yemian=3;
	lcd_clr_season();
	lcd_show_info();	
	while(motor1_xulie>11000&&motor1_xulie<=22000) //都左电机左转打一半，右电机右转打一半(冷热水都在中间)
	{
	    update_flag=1;//update temp. flag
		cur_temp=(float)ds_gettmp()/10;
		time_out_flagleft=0;
		time_out_flagright=2; 
		time_out=1;
	}
	time_out=15000;
	flag_hui=1;  //开回流水管
	while(flag_hui);
	
	while(1)
	{
	    
		update_flag=1;//update temp
		cur_temp=(float)ds_gettmp()/10;		
		temperature_control(tar_temp,cur_temp);
		if(cur_temp>=tar_temp-0.5&&cur_temp<=tar_temp+2)	//正负0.5度误差
			break;
	}
	flag_hui=2;//关回流水管
	while(flag_hui);
	update_flag=0;//关更新温度标志
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
		if(flag_exit==1)		//按键3停止用水
		{
			time_out_flagleft=2;
			time_out_flagright=0;
			  while(motor1_xulie<22000||motor2_xulie<22000) //结束时两个电机都右转打死关闭
			{
		    update_flag=1;//update temp. flag
			if(motor1_xulie==22000) 
				time_out_flagleft=1;	 //转到底就不转
			if(motor2_xulie==22000) 
				time_out_flagright=1;	 //转到底就不转
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
		time_out_flagleft=0;   //左电机左转
		time_out_flagright=1; //右电机不动 
		time_out=1;
	}
	time_out=15000;    //置回无限大状态
	flag_hui=1;  //开回流水管
	while(flag_hui);
	while(1)	  //等待热水足够热
	{
     	update_flag=1;//update flag
		cur_temp=(float)ds_gettmp()/10;
		if(cur_temp>=tar_temp&&cur_temp<tar_temp+2) 
			break;
	} 
	while(motor2_xulie>15000) //右电机左转打小一半(回到冬天电机正常位置，也给后面pid调温留位置)
	{
	    update_flag=1;//update temp. flag
		cur_temp=(float)ds_gettmp()/10;
		time_out_flagleft=1;	//左电机不转
		time_out_flagright=2; 	// 右电机左转
		time_out=1;
	}
	time_out=15000;
	while(1)	   
	{
		cur_temp=(float)ds_gettmp()/10;
		temperature_control(tar_temp,cur_temp);
		if(cur_temp>=tar_temp-0.5&&cur_temp<=tar_temp+0.5)	//正负0.5度误差
			break;
	}
	flag_hui=2;//关回流水管
	while(flag_hui);

	update_flag=0;//关更新温度标志
	lcd_clr_info();
	lcd_show_open();   //提醒用户用水
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
		if(flag_exit==1)		//按键3停止用水
		{
			time_out_flagleft=2;
			time_out_flagright=0;
			  while(motor1_xulie<22000||motor2_xulie<22000) //结束时两个电机都右转打死关闭
			{
		    update_flag=1;//update temp. flag
			if(motor1_xulie==22000) 
				time_out_flagleft=1;	 //转到底就不转
			if(motor2_xulie==22000) 
				time_out_flagright=1;	 //转到底就不转
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
  //更新目标温度
  sprintf(buf,"%.1f",tar_temp);
  LCD_ShowString(4*8,1*16+8,buf); 
  //更新当前温度 
  sprintf(buf,"%.1f",cur_temp);
  LCD_ShowString(4*8,6*16+8,buf);
}
