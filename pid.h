 #ifndef _PID_H
#define _PID_H

#include <stc12c5a60s2.h>

#ifndef uchar				 //
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

extern uint time_out;	 //定义发送序列时间
extern char time_out_flagleft;	   //定义正反转标志
extern char time_out_flagright;	   //定义正反转标志
void temperature_control(float tar_temp, float act_temp);	 //温度pid函数

#endif