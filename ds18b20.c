#include<reg52.h>
#include<intrins.h>
#include"ds18b20.h"

/****************************
重新定义常用数据类型
****************************/
#define uchar unsigned char
#define uint  unsigned int


/********************************************
函数名         ds_delayus
函数功能       用于单总线通信微秒级延时
函数参数       无符号整型x
返回值         无
函数说明   
*********************************************/
void ds_delayus(uchar x)
{
	/*uchar a=9;
	while(a--)
	{
		while(--x);
	}*/
	uchar i;
	for(i=0;i<x;i++);
	
}

/********************************************
函数名         ds_delayms
函数功能       用于单总线通信毫秒级延时
函数参数       无符号整型x
返回值         无
函数说明   
*********************************************/
void ds_delayms(uint x)
{
	/*uint i,j;
	uchar a=9;
	while(a--)
	{
		for(i=0;i<110;i++)
			for(j=0;j<x;j++);
	}*/
	uint i,j,k;
	for(k=0;k<x;k++)
		for(j=0;j<36;j++)
			for(i=0;i<26;i++);
}

/**********************************************
函 数 名         : ds_init
函数功能		     : 用于初始化单总线
输    入         : 无
输    出         : 无
函数说明         ：无
***********************************************/
uint ds_init()
{
	uchar i;
	DQ=0;
	ds_delayus(250);    //总线拉低500us
	
	DQ=1;
	i=50;
	                    //总线拉高后等待传感器拉低总线
	while(DQ)           //如果DS18B20做出反应会将在15us~60us后总线拉低
	{
		i--;
		if(i==0)
		{
			return 0;        //初始化失败
		}
	}
	return 1;            //初始化成功
}

/**********************************************
函 数 名         : ds_wtdat
函数功能		     : 用于通过单总线发送信号
输    入         : 无符号字符型dat
输    出         : 无
函数说明         ：无
***********************************************/
void ds_wtdat(uchar dat)
{
	uint i;
	
	DQ=1;                //拉高总线，并持续1us
	_nop_();
	
	for(i=0;i<8;i++)
	{
		DQ=0;              //每写入一位数据之前先把总线拉低1us
		_nop_();
		
		DQ=dat&0x01;       //写入一个数据，从最低位开始，并持续65us
		ds_delayus(30); 
		
		DQ=1;              //释放总线，至少1us给总线恢复时间才能接着写入第二个数值
		_nop_();
		dat=dat>>1;
	}
}
/**********************************************
函 数 名         : ds_rddat
函数功能		     : 用于通过单总线接收信号
输    入         : 无
输    出         : 无符号字符型dat
函数说明         ：无
***********************************************/
uchar ds_rddat()
{
	uint i;
	uchar dat=0,temp;
	
	for(i=0;i<8;i++)
	{
		DQ=0;                       //将总线拉低1us
		_nop_();
		
		DQ=1;                       //释放总线,并维持6us
		ds_delayus(1);
		
		temp=DQ;                    //读取数据并等待55us
		dat=(temp<<7) | (dat>>1);
		ds_delayus(25);
	}
	return dat;
}

/**********************************************
函 数 名         : ds_gettmp
函数功能		 : 用于获取ds18b20传感器温度数据
输    入         : 无
输    出         : 无
函数说明         ：无
***********************************************/
int ds_gettmp()
{
	int temp=0;
	static int temp_last=0;
	uchar tmph,tmpl;
	
	ds_init();
	ds_delayms(1);
	ds_wtdat(0xcc);
	ds_wtdat(0x44);
	
	ds_init();
	ds_delayms(1);
	ds_wtdat(0xcc);
	ds_wtdat(0xbe);
	
	tmpl=ds_rddat();
	tmph=ds_rddat();
	temp = tmph;
	temp <<= 8;
	temp |= tmpl;
	temp*=0.625;
	if(temp<700&&temp>0)   //防止异常数据
	{
		temp_last=temp;
		return temp;
	}
	else
		return temp_last;
}