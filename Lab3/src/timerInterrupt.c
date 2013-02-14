
#include "timerInterrupt.h"


// Must be called before the timer initialization
void initNVIC() {
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	
	NVIC_Init(&NVIC_InitStruct);
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
	TIM_TimeBaseStruct.TIM_Prescaler = (uint16_t) (((SystemCoreClock/2) / 10000) - 1);     // Adjusts timer frequency from 84 MHz to 10 KHz
	TIM_TimeBaseStruct.TIM_Period = (uint32_t) (10000 - 1);                               // Timer period of 1 Hz (down from 10 KHz)
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Down;
	
    // Configure TIM3 with the timer configurations
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
    
    // Enable the TIM3 interrups using TIM_IT_UPDATE as the interrupt source
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    // Enable the TIM3 peripheral
    TIM_Cmd(TIM3, ENABLE);
    
}
