/******************************
�⺯�����ƣ�E2PROM.h
����������	E2PROM����д����������
Ӳ�����ӣ�  �ޣ���Ƭ���ڲ�E2PROM���ã�
ά����¼��  2017-4-7
******************************/
#include<reg52.h>
#include<intrins.h>
#include"E2PROM.h"

/******************************
���¶��峣�ñ���������
******************************/
#define uchar unsigned char
#define uint unsigned int
/*********************
����ISP��������
*********************/
#define RdCommand 0x01 //���� ISP �Ĳ�������
#define PrgCommand 0x02
#define EraseCommand 0x03
#define WaitTime 0x01   //����cpu�ȴ�ʱ��
/*********************
����Ĵ�������
*********************/
sfr ISP_DATA=0xe2; //�Ĵ�������
sfr ISP_ADDRH=0xe3;
sfr ISP_ADDRL=0xe4;
sfr ISP_CMD=0xe5;
sfr ISP_TRIG=0xe6;
sfr ISP_CONTR=0xe7;

/***************************************************************
=================��ISP,IAP����=============================
***************************************************************/
void ISP_IAP_enable(void)
{
	EA = 0;   /* ���ж�*/
	ISP_CONTR =ISP_CONTR & 0x18;  /* 0001,1000 ��λ*/
	ISP_CONTR=ISP_CONTR | WaitTime;	   /* д��Ӳ����ʱ */
	ISP_CONTR=ISP_CONTR | 0x80;     	  /* ISPEN=1*/	   
}
/***************************************************************
=================�ر� ISP,IAP ����=============================
***************************************************************/
void ISP_IAP_disable(void)
{
	ISP_CONTR = ISP_CONTR & 0x7f;   /* ISPEN = 0 */
	ISP_TRIG=0x00;
	EA=1;    /* ���ж� */
}
/***************************************************************
=================���õĴ������� =============================
***************************************************************/
void ISPgoon(void)
{
	ISP_IAP_enable();	   /* �� ISP,IAP ���� */
 	ISP_TRIG=0x46; /* ���� ISP_IAP �����ֽ� 1*/
	ISP_TRIG=0xb9;	/* ���� ISP_IAP �����ֽ� 2 */
	nop_();
}

/***************************************************************
========�ֽڶ�������ַ��������������رգ�����ֵ�� ===========
***************************************************************/
unsigned char byte_read(unsigned int byte_addr)
{
	ISP_ADDRH = (unsigned char)(byte_addr >> 8);/* ��ַ��ֵ */
    ISP_ADDRL = (unsigned char)(byte_addr & 0x00ff);
	ISP_CMD = ISP_CMD & 0xf8;				  /* ����� 3 λ*/
	ISP_CMD = ISP_CMD | RdCommand;			 /* д������� */
	ISPgoon();						   		  /* ����ִ��*/
	ISP_IAP_disable();						 /* �ر� ISP,IAP ���� */
	return (ISP_DATA);						 /* ���ض���������*/
}
/***************************************************************
==========�ֽڲ���������ַ��������������رգ� ================
***************************************************************/
void SectorErase(unsigned int sector_addr)
{
	unsigned int iSectorAddr;
	iSectorAddr = (sector_addr & 0xfe00);   /* ȡ������ַ */
	ISP_ADDRH = (unsigned char)(iSectorAddr >> 8);
	ISP_ADDRL = 0x00;
	ISP_CMD	 = ISP_CMD & 0xf8;				  /* ��յ� 3 λ*/
    ISP_CMD	= ISP_CMD | EraseCommand;		/* �������� 3*/
    ISPgoon();								 	/* ����ִ��*/
	ISP_IAP_disable();						  /* �ر� ISP,IAP ���� */
}
/***************************************************************
========�ֽ�д ������ַ������������ݣ��������رգ�===========
***************************************************************/
void byte_write(unsigned int byte_addr, unsigned char original_data)
{
	ISP_ADDRH =	(unsigned char)(byte_addr >> 8); /* ȡ��ַ*/
	ISP_ADDRL =	(unsigned char)(byte_addr & 0x00ff);
	ISP_CMD	 = ISP_CMD & 0xf8;						  /* ��� 3 λ */
	ISP_CMD = ISP_CMD | PrgCommand;					 /* д���� 2 */
	ISP_DATA = original_data;						/* д������׼�� */
	ISPgoon();									  /* ����ִ��*/
	ISP_IAP_disable();						   /* �ر� IAP ���� */
}
/***************************************************************
========�ֶ� ��˫�ֽڣ�===========
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
========��д ��˫�ֽڣ�===========
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


