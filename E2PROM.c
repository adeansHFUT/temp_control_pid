/******************************
库函数名称：E2PROM.h
功能描述：	E2PROM读，写，擦除数据
硬件连接：  无（单片机内部E2PROM调用）
维护记录：  2017-4-7
******************************/
#include<reg52.h>
#include<intrins.h>
#include"E2PROM.h"

/******************************
重新定义常用变量类型名
******************************/
#define uchar unsigned char
#define uint unsigned int
/*********************
定义ISP操作命令
*********************/
#define RdCommand 0x01 //定义 ISP 的操作命令
#define PrgCommand 0x02
#define EraseCommand 0x03
#define WaitTime 0x01   //定义cpu等待时间
/*********************
特殊寄存器申明
*********************/
sfr ISP_DATA=0xe2; //寄存器申明
sfr ISP_ADDRH=0xe3;
sfr ISP_ADDRL=0xe4;
sfr ISP_CMD=0xe5;
sfr ISP_TRIG=0xe6;
sfr ISP_CONTR=0xe7;

/***************************************************************
=================打开ISP,IAP功能=============================
***************************************************************/
void ISP_IAP_enable(void)
{
	EA = 0;   /* 关中断*/
	ISP_CONTR =ISP_CONTR & 0x18;  /* 0001,1000 复位*/
	ISP_CONTR=ISP_CONTR | WaitTime;	   /* 写入硬件延时 */
	ISP_CONTR=ISP_CONTR | 0x80;     	  /* ISPEN=1*/	   
}
/***************************************************************
=================关闭 ISP,IAP 功能=============================
***************************************************************/
void ISP_IAP_disable(void)
{
	ISP_CONTR = ISP_CONTR & 0x7f;   /* ISPEN = 0 */
	ISP_TRIG=0x00;
	EA=1;    /* 开中断 */
}
/***************************************************************
=================公用的触发代码 =============================
***************************************************************/
void ISPgoon(void)
{
	ISP_IAP_enable();	   /* 打开 ISP,IAP 功能 */
 	ISP_TRIG=0x46; /* 触发 ISP_IAP 命令字节 1*/
	ISP_TRIG=0xb9;	/* 触发 ISP_IAP 命令字节 2 */
	nop_();
}

/***************************************************************
========字节读（给地址，给命令，触发，关闭，返回值） ===========
***************************************************************/
unsigned char byte_read(unsigned int byte_addr)
{
	ISP_ADDRH = (unsigned char)(byte_addr >> 8);/* 地址赋值 */
    ISP_ADDRL = (unsigned char)(byte_addr & 0x00ff);
	ISP_CMD = ISP_CMD & 0xf8;				  /* 清除低 3 位*/
	ISP_CMD = ISP_CMD | RdCommand;			 /* 写入读命令 */
	ISPgoon();						   		  /* 触发执行*/
	ISP_IAP_disable();						 /* 关闭 ISP,IAP 功能 */
	return (ISP_DATA);						 /* 返回读到的数据*/
}
/***************************************************************
==========字节擦除（给地址，给命令，触发，关闭） ================
***************************************************************/
void SectorErase(unsigned int sector_addr)
{
	unsigned int iSectorAddr;
	iSectorAddr = (sector_addr & 0xfe00);   /* 取扇区地址 */
	ISP_ADDRH = (unsigned char)(iSectorAddr >> 8);
	ISP_ADDRL = 0x00;
	ISP_CMD	 = ISP_CMD & 0xf8;				  /* 清空低 3 位*/
    ISP_CMD	= ISP_CMD | EraseCommand;		/* 擦除命令 3*/
    ISPgoon();								 	/* 触发执行*/
	ISP_IAP_disable();						  /* 关闭 ISP,IAP 功能 */
}
/***************************************************************
========字节写 （给地址，给命令，给数据，触发，关闭）===========
***************************************************************/
void byte_write(unsigned int byte_addr, unsigned char original_data)
{
	ISP_ADDRH =	(unsigned char)(byte_addr >> 8); /* 取地址*/
	ISP_ADDRL =	(unsigned char)(byte_addr & 0x00ff);
	ISP_CMD	 = ISP_CMD & 0xf8;						  /* 清低 3 位 */
	ISP_CMD = ISP_CMD | PrgCommand;					 /* 写命令 2 */
	ISP_DATA = original_data;						/* 写入数据准备 */
	ISPgoon();									  /* 触发执行*/
	ISP_IAP_disable();						   /* 关闭 IAP 功能 */
}
/***************************************************************
========字读 （双字节）===========
***************************************************************/
unsigned int word_read(unsigned int byte_addr)
{
	uint word;
	uchar lbyte,hbyte;
	lbyte=byte_read(byte_addr);
	hbyte=byte_read(byte_addr+1);
	word=hbyte;
	word<<=8;
	word|=lbyte;
	return word;
}
/***************************************************************
========字写 （双字节）===========
***************************************************************/
void word_write(unsigned int byte_addr, unsigned int original_data)
{
	uchar lbyte,hbyte;
	lbyte=original_data&0x00ff;
	original_data>>=8;
	hbyte=original_data;
	SectorErase(byte_addr);
	byte_write(byte_addr,lbyte);
//	SectorErase(byte_addr+1);
	byte_write(byte_addr+1,hbyte);
}


