#include"pid.h"
#include"18b20.h"
#include<stdio.h>  
#include<stdlib.h> 

#define	MAXdegree 150

uint time_out=MAXdegree*100;  //�����������ʱ����,��ʼֵ������
char time_out_flagleft=0; 	//0��ʾ���£�2��ʾ����
char time_out_flagright=0;
float  Kp=20;//����ϵ��
float  Ki=0.3;   //����ϵ��
float  Kd=10;  //΢��ϵ��

void temperature_control(float tar_temp, float act_temp)	  //����Ԥ���¶���ʵ���¶�
{  
    static float Last_Err=0,Sum=0;
	float err=(tar_temp-act_temp)*4;   //�¶Ȳ�
    int	degree_out;

    					          
    degree_out=err+Kp*err+Kd*(err-Last_Err)+Ki*Sum;  
	
		   
      
    Last_Err=err;
    Sum+=err;
	if(Sum>100)
          Sum=100;
      else if(Sum<-100)
          Sum=-100;
   
  
    if(degree_out>MAXdegree)
        degree_out=MAXdegree;      //����������
      
    else if(degree_out<-MAXdegree)
		 degree_out=-MAXdegree;
  
    if(degree_out>0)
	{
		time_out=MAXdegree+1-degree_out;
		time_out_flagleft=0;		      //����
		time_out_flagright=0;
	}
	if(degree_out<0)
	{
		 degree_out=-degree_out;
		 time_out=MAXdegree+1-degree_out;
		 time_out_flagleft=2;		//����
		 time_out_flagright=2;
	}
      
	

}  
 