#ifndef __TIMERINTERRUPT_H_
#define __TIMERINTERRUPT_H_

// Includes
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"

/**
 * Initializes the interrupt handler for timer 3
*/
void initNVIC_TIM3(void);		// Timer interrupt handler

/**
 * Initializes the interrupt handler for tap
*/
void initNVIC_EXTI0(void);	// Click interrupt handler

/**
 * Initializes the interrupt handler for DMA completion
*/
void initNVIC_DMA2(void);	// DMA interrupt handler

/**
 * Initializes timer 3 to operate at 100hz
*/
void initTIM3(void);				// Timer setup config

/**
 * Initializes the GPIOE to link to external triggered interrupt.
*/
void initEXTI0(void);				// External interrupt config

#endif // __TIMERINTERRUPT_H_
