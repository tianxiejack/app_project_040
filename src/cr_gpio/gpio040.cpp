

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "osa.h"
#include "gpio_rdwr.h"
#include "gpio040.h"

int hard_flag=0;

void Hard_Init()
{
	//Hard_capturereset();
	//Hard_displayreset();
	Hard_Gpiocreate();
	hard_flag=1;
	
}


void Hard_displayreset()
{
	unsigned int disp_gpio;

	disp_gpio = 426;

	GPIO_create(disp_gpio,GPIO_DIRECTION_OUT);
	
	GPIO_set(disp_gpio,0);
	OSA_waitMsecs(100);
	GPIO_set(disp_gpio,1);

	GPIO_close(disp_gpio);		

}

void Hard_capturereset()
{
	unsigned int cap_gpio;

	cap_gpio = 478;


	GPIO_create(cap_gpio,GPIO_DIRECTION_OUT);
	GPIO_set(cap_gpio,0);
	OSA_waitMsecs(100);
	GPIO_set(cap_gpio,1);
	OSA_waitMsecs(100);
	GPIO_set(cap_gpio,0);
	OSA_waitMsecs(1200);
	GPIO_set(cap_gpio,1);

	GPIO_close(cap_gpio);

}

void Hard_Gpiocreate()
{
	unsigned int getccd_gpio;
	unsigned int getfir_gpio;

	getccd_gpio = 480;
	getfir_gpio = 390;

	//ccd detect
	GPIO_create(getccd_gpio,GPIO_DIRECTION_IN);
	//fir detect
	GPIO_create(getfir_gpio,GPIO_DIRECTION_IN);

}

int Hard_getccdstatus()
{
	unsigned int getccd_gpio;

	getccd_gpio = 480;
	
	//ccd
	if(hard_flag==0)
		return 1;
	
	return GPIO_get(getccd_gpio);

}

int Hard_getfirstatus()
{
	unsigned int getfir_gpio;

	getfir_gpio = 390;
	
	//fir
	if(hard_flag==0)
		return 1;

	return GPIO_get(getfir_gpio);
	
}