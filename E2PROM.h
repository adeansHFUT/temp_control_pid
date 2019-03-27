/**************************
文件名 ：E2PROM.h
作者   :杨俊杰
日期   ：2017-04-07
**************************/

/*************************
文件说明：
1.内部E2PROM的读字节，写字节，字节擦除函数
2.同一地址，字节写前要先擦除！！！ 
3.郭天祥书中例程地址为0x2000

包含的函数
unsigned char byte_read(unsigned int byte_addr)	   //字节读
void SectorErase(unsigned int sector_addr)		   //字节擦除
void byte_write(unsigned int byte_addr, unsigned char original_data)	//字节写
                 
**************************/

#ifndef __E2PROM_H__
#define __E2PROM_H__


/***********************************
函数名：  三个见下 
函数功能：完成E2PROM的读，写，擦除操作 
函数参数：16进制地址，一字节大小数据 
函数返回值：字节读函数返回一字节数据 
函数说明：同一地址，先用擦出函数，再用写函数 
************************************/
extern unsigned char byte_read(unsigned int byte_addr);	   //字节读
extern void SectorErase(unsigned int sector_addr);		   //字节擦除
extern void byte_write(unsigned int byte_addr, unsigned char original_data);	//字节写
unsigned int word_read(unsigned int byte_addr);	 //双字节读
extern void word_write(unsigned int byte_addr, unsigned int original_data);	//双字杰写
#endif
