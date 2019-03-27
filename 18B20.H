 #ifndef _18b20_H
#define _18b20_H

#include <STC12C5A60S2.h>
#include <intrins.h>



#ifndef uchar				 //条件编译
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

sbit DSPORT=P3^5;	   //温度传感器数据拐脚

int Ds18b20ReadTemp();	    //读取温度
float datapros(int temp);	//解析温度
#endif
