/**************************
文件名 ：ds18b20.h
作者   :物联网16郭义臣
日期   ：2017-03-27
**************************/

/*************************************************************************************************
文件说明：
包含了单总线通信的一些函数和驱动ds18b20温度传感器获取温度的函数

包含的宏定义
DQ                                                   单总线端口

unsigned int ds_init();                              初始化总线函数
void ds_wtdat(unsigned char);                        通过总线发送一个字节
unsigned char ds_rddat();                            通过总线接收一个字节
int ds_gettmp();                                     读取温度传感器读取温度
**************************************************************************************************/


#ifndef __DS18B20_H__
#define __DS18B20_H__


//--定义使用的IO口--//
sbit DQ=P3^5;

extern unsigned int ds_init();
extern void ds_wtdat(unsigned char);
extern unsigned char ds_rddat();
extern int ds_gettmp();
/*		用如下代码转换
		char table[5];
		wendu=ds_gettmp();;
		sprintf(table,"%4.1f",wendu/10);
*/

#endif