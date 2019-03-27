/**************************
�ļ��� ��ds18b20.h
����   :������16���峼
����   ��2017-03-27
**************************/

/*************************************************************************************************
�ļ�˵����
�����˵�����ͨ�ŵ�һЩ����������ds18b20�¶ȴ�������ȡ�¶ȵĺ���

�����ĺ궨��
DQ                                                   �����߶˿�

unsigned int ds_init();                              ��ʼ�����ߺ���
void ds_wtdat(unsigned char);                        ͨ�����߷���һ���ֽ�
unsigned char ds_rddat();                            ͨ�����߽���һ���ֽ�
int ds_gettmp();                                     ��ȡ�¶ȴ�������ȡ�¶�
**************************************************************************************************/


#ifndef __DS18B20_H__
#define __DS18B20_H__


//--����ʹ�õ�IO��--//
sbit DQ=P3^5;

extern unsigned int ds_init();
extern void ds_wtdat(unsigned char);
extern unsigned char ds_rddat();
extern int ds_gettmp();
/*		�����´���ת��
		char table[5];
		wendu=ds_gettmp();;
		sprintf(table,"%4.1f",wendu/10);
*/

#endif