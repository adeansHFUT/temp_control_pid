#include <STC12C5A60S2.h>
#include <intrins.h>

#include "sys.h"

void delayms(int count)  // /* X1ms */
{
  while(--count)
  {
	unsigned char i, j;
	_nop_();
	i = 11;
	j = 100;
	do
	{
		while (--j);
	} while (--i);	
  }
}