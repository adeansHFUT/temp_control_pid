 #ifndef _PID_H
#define _PID_H

#include <stc12c5a60s2.h>

#ifndef uchar				 //
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

extern uint time_out;	 //���巢������ʱ��
extern char time_out_flagleft;	   //��������ת��־
extern char time_out_flagright;	   //��������ת��־
void temperature_control(float tar_temp, float act_temp);	 //�¶�pid����

#endif