 #ifndef _18b20_H
#define _18b20_H

#include <STC12C5A60S2.h>
#include <intrins.h>



#ifndef uchar				 //��������
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

sbit DSPORT=P3^5;	   //�¶ȴ��������ݹս�

int Ds18b20ReadTemp();	    //��ȡ�¶�
float datapros(int temp);	//�����¶�
#endif