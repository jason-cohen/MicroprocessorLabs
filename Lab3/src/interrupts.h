#ifndef __TIMERINTERRUPT_H_
#define __TIMERINTERRUPT_H_

// Includes
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"

// Function prototypes
void initNVIC_TIM3(void);		// Timer interrupt handler
void initNVIC_EXTI0(void);	// Click interrupt handler
void initTIM3(void);				// Timer setup config
void initEXTI0(void);				// External interrupt config

#endif // __TIMERINTERRUPT_H_
