/**************************
�ļ��� ��E2PROM.h
����   :���
����   ��2017-04-07
**************************/

/*************************
�ļ�˵����
1.�ڲ�E2PROM�Ķ��ֽڣ�д�ֽڣ��ֽڲ�������
2.ͬһ��ַ���ֽ�дǰҪ�Ȳ��������� 
3.�������������̵�ַΪ0x2000

�����ĺ���
unsigned char byte_read(unsigned int byte_addr)	   //�ֽڶ�
void SectorErase(unsigned int sector_addr)		   //�ֽڲ���
void byte_write(unsigned int byte_addr, unsigned char original_data)	//�ֽ�д
                 
**************************/

#ifndef __E2PROM_H__
#define __E2PROM_H__


/***********************************
��������  �������� 
�������ܣ����E2PROM�Ķ���д���������� 
����������16���Ƶ�ַ��һ�ֽڴ�С���� 
��������ֵ���ֽڶ���������һ�ֽ����� 
����˵����ͬһ��ַ�����ò�������������д���� 
************************************/
extern unsigned char byte_read(unsigned int byte_addr);	   //�ֽڶ�
extern void SectorErase(unsigned int sector_addr);		   //�ֽڲ���
extern void byte_write(unsigned int byte_addr, unsigned char original_data);	//�ֽ�д
unsigned int word_read(unsigned int byte_addr);	 //˫�ֽڶ�
extern void word_write(unsigned int byte_addr, unsigned int original_data);	//˫�ֽ�д
#endif
