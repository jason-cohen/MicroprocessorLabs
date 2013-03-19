
#include "interrupts.h"


// Must be called before the timer initialization
void initNVIC_TIM3() {
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;								// Maps interrupt handler to timer 3
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;		// Sets highest priority interrupt (0 highest)
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;					// Sets sub priority
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;								// Enable channel
	
	NVIC_Init(&NVIC_InitStruct);																// Passes in the initialization parameters
}

// Must be called after the click EXTI interrupt initialization
void initNVIC_EXTI0() {
	NVIC_InitTypeDef NVIC_InitStruct;
	
	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	// interrupt handler
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;								// Maps interrupt handler to EXTI0
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;   // Sets priority to number 2 (1 = second highest)
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;  				// Sets sub priority
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;								// Enable channel
	
	NVIC_Init(&NVIC_InitStruct);																// Passes in the initialization parameters
}

// Must be called after the click EXTI interrupt initialization
void initNVIC_EXTI1() {
	NVIC_InitTypeDef NVIC_InitStruct;
	
	/* Enable and set EXTI Line0 Interrupt to the lowest priority */
	// interrupt handler
	NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;								// Maps interrupt handler to EXTI1
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;   // Sets priority to number 2 (1 = second highest)
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;  				// Sets sub priority
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;								// Enable channel
	
	NVIC_Init(&NVIC_InitStruct);																// Passes in the initialization parameters
}


void initTIM3() {
	// Configuration structure declaration
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStruct;									
	
	// Setup TIM3 to operate on the APB1 clock at 84 MHz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	// Initialize the timer configurations
	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
	
	// Custom settings for the timer configuration
	// SystemCoreClock is preset to 168 MHz
	TIM_TimeBaseStruct.TIM_Prescaler = (uint16_t) (((SystemCoreClock/2) / 100000) - 1);     // Adjusts timer frequency from 84 MHz to 100 KHz, *value must be less than 65535
	TIM_TimeBaseStruct.TIM_Period = (uint32_t) (1000 - 1);                               		// Timer period of 100 Hz (down from 100 KHz), *value must be less than 65535
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Down;
	
	// Configure TIM3 with the timer configurations
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
    
	// Enable the TIM3 interrups using TIM_IT_UPDATE as the interrupt source
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
	// Enable the TIM3 peripheral
	TIM_Cmd(TIM3, ENABLE);
    
}

void initEXTI0() {
	
	EXTI_InitTypeDef EXTI_InitStruct;
	
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Connect EXTI Line0 to interrupt pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
	
	/* Configure EXTI Line0 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line0;
	// EXTI mode
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	// operate on rising edge of clock
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);
	
}

void initEXTI1() {
	EXTI_InitTypeDef   EXTI_InitStructure;

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Connect EXTI Line1 to interrupt pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource1);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}
