#include "button.h"

void initGPIOA_BUTTON() {
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef gpio_init_s;
	GPIO_StructInit(&gpio_init_s);
	
	//Push-button
	gpio_init_s.GPIO_Pin = GPIO_Pin_0;		  		// we want to configure PA0, a push-button
	gpio_init_s.GPIO_Mode = GPIO_Mode_IN; 	  	    // we want it to be an input
	gpio_init_s.GPIO_Speed = GPIO_Speed_50MHz;	    // this sets the GPIO modules clock speed
	gpio_init_s.GPIO_OType = GPIO_OType_PP;   	    // this sets the pin type to push / pull (as opposed to open drain)
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_DOWN;   	    // this enables the pulldown resistor --> we want to detect a high level
	
	GPIO_Init(GPIOA, &gpio_init_s);			  		// this passes the configuration to the Init function which takes care of the low level stuff
	
	}
