 #ifndef _LANYA_H
#define _LANYA_H

#include <stc12c5a60s2.h>

#ifndef uchar				 
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

void uart_send(char *a);	  //�����ַ���
void lanya_init(void);	 //������ʼ��

#endif