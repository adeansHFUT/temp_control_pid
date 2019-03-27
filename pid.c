#include"pid.h"
#include"18b20.h"
#include<stdio.h>  
#include<stdlib.h> 

#define	MAXdegree 150

uint time_out=MAXdegree*100;  //输出发送序列时间间隔,初始值尽量大
char time_out_flagleft=0; 	//0表示升温，2表示降温
char time_out_flagright=0;
float  Kp=20;//比例系数
float  Ki=0.3;   //积分系数
float  Kd=10;  //微分系数

void temperature_control(float tar_temp, float act_temp)	  //输入预期温度与实际温度
{  
    static float Last_Err=0,Sum=0;
	float err=(tar_temp-act_temp)*4;   //温度差
    int	degree_out;

    					          
    degree_out=err+Kp*err+Kd*(err-Last_Err)+Ki*Sum;  
	
		   
      
    Last_Err=err;
    Sum+=err;
	if(Sum>100)
          Sum=100;
      else if(Sum<-100)
          Sum=-100;
   
  
    if(degree_out>MAXdegree)
        degree_out=MAXdegree;      //设置上下限
      
    else if(degree_out<-MAXdegree)
		 degree_out=-MAXdegree;
  
    if(degree_out>0)
	{
		time_out=MAXdegree+1-degree_out;
		time_out_flagleft=0;		      //升温
		time_out_flagright=0;
	}
	if(degree_out<0)
	{
		 degree_out=-degree_out;
		 time_out=MAXdegree+1-degree_out;
		 time_out_flagleft=2;		//降温
		 time_out_flagright=2;
	}
      
	

}  
 