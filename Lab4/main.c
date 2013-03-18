/* Includes ------------------------------------------------------------------*/
#include "arm_math.h"
#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "DMA.h"
#include "LEDs.h"
#include "stdio.h"
#include "interrupts.h"
#include "ACC.h"

/* Defines -------------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/


uint32_t runACC = 0;
uint32_t counti = 0;
uint32_t countb = 0;
/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void thread(void const * argument);
void pollACC(void);

//! Thread structure for above thread
osThreadDef(thread, osPriorityNormal, 1, 0);

/*!
 @brief Program entry point
 */
int main (void) {
	
	initGPIOD_LED();
// Initializes the timer and interrupt handler for the accelerometer polling
  initNVIC_TIM3();					//Interrupt Handler
	initTIM3();								//Timer
	
	initAcc();
	DMA_init();
	initNVIC_DMA2();
	
	while(1) {
		if (runACC) {
				/* DMA Stream enable */
				//SPI_I2S_DMACmd(LIS302DL_SPI, SPI_I2S_DMAReq_Rx, ENABLE);
				pollACC();
				//DMA_Cmd(DMA2_Stream0, ENABLE);
				runACC = 1 - runACC;
		}
	}
	
	
// 	// ID for thread
// 	osThreadId tid_thread1;

// 	// Start thread
// 	tid_thread1 = osThreadCreate(osThread(thread), NULL);

// 	// The below doesn't really need to be in a loop
// 	while(1){
// 		osDelay(osWaitForever);
// 	}
}
void pollACC() {
	LIS302DL_CS_HIGH(); //GPIO_ResetBits(GPIOA, GPIO_Pin_4); //Set CS On
	/* Enable SPI_MASTER */
	SPI_Cmd(LIS302DL_SPI, ENABLE);
	 
	/* Enable DMA channels */
	DMA_Cmd(Rx_DMA, ENABLE);
	DMA_Cmd(Tx_DMA, ENABLE);
	 
	//while(!DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0));
	while(!DMA_GetFlagStatus(DMA2_Stream5, DMA_FLAG_TCIF5));
	LIS302DL_CS_LOW(); //GPIO_SetBits(GPIOA, GPIO_Pin_4); //Set CS Off
}
void thread (void const *argument) {
	while(1){
		osDelay(1000);
		GPIOD->BSRRL = GPIO_Pin_12;
		osDelay(1000);
		GPIOD->BSRRH = GPIO_Pin_12;
	}
}

/**
  * @brief  This function handles DMA Stream interrupt request.
  * @param  None
  * @retval None
  */
void DMA2_Stream0_IRQHandler()
{
  /* Test on DMA Stream Transfer Complete interrupt */
  if(DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
  {
    /* Clear DMA Stream Transfer Complete interrupt pending bit */
    DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);  
		toggleLED();
		countb++;
  }
}

// 1Hz interrupt
void TIM3_IRQHandler() {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		counti++;
		if (!runACC) {
			runACC = 1 - runACC;
		}
  }
}
