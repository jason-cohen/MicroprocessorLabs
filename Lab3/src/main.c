#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"
#include "stdio.h"
#include "movingAverage.h"
#include "interrupts.h"
#include "ACC.h"
#include "LEDs.h"

/* Program flow macros */
#define CALIB_ACC 0
#define RUN_TIM 1
#define RUN_ACC_CLICK 1

/* Defines */
#define PI 3.14159

/**
 * Converts the accelerometer data into Pitch and Roll
 * alphaAngle get the Roll 
 * betaAngle gets the Pitch
 */
void calculateAngles(void);

/* Variable declarations */

float alphaAngle;					// Roll angle, x axis and horizontal plane
float betaAngle;					// Pitch angle, y axis and horizontal plane

//Moving average struct
movAvgState movAvgStateStruct[NUM_AXES];

uint8_t runACC = 0;				// Program control variable to perform an accelerometer measurment

int main() {
    
	uint8_t i;
	
	// Run setups based on debugging settings

	initGPIOD_LED();

// Initializes the timer and interrupt handler for the accelerometer polling
#if RUN_TIM
  initNVIC_TIM3();					//Interrupt Handler
	initTIM3();								//Timer
#endif
  
	initAcc();
	
// Configuration for setting up the tapping interrupt.
#if RUN_ACC_CLICK
	initEXTI0();
	initNVIC_EXTI0();
	initAccClickInterrupt();
#endif    

	// Initialize the moving average structures (x, y, z), so 0 to NUM_AXES (3)
	for (i = 0; i < NUM_AXES; i++) {
		movingAverageInit(&movAvgStateStruct[i]);
	}
 
// If it's the calibration mode, it prints the raw accelerometer X Y Z data in loop
#if CALIB_ACC
	while(1) {
		printf("X:%d\tY:%d\tZ:%d\n", accData[0], accData[1], accData[2]);
	}
	
// Otherwise, main functionality occurs.
#else
	while (1) {
		
		//Toggled every 100hz
		if (runACC) {
			
			//We read from the accelerometer to obtain the raw data.
			LIS302DL_ReadACC(accData);
			
			//We calibrate the data (filters the values)
			calibrateData();
			
			//Calculate the angles using trig
			calculateAngles();
			
			//Toggle runACC
			runACC = 1 - runACC;
		}
	}
#endif
}

//Doxygen comments in def.
void calculateAngles() {
	uint8_t i;
	// Calcuate angles based on filtered data
	for (i = 0; i < NUM_AXES; i++) {
		
		//Add new data to moving average filter
		movingAverage(&movAvgStateStruct[i], calibratedData[i]);
	}
	
	alphaAngle = atan2(movAvgStateStruct[0].avg, sqrt(movAvgStateStruct[1].avg*movAvgStateStruct[1].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg)) * 180.0 / PI;	// atan(x / sqrt(y^2 + z^2))
	betaAngle = atan2(movAvgStateStruct[1].avg, sqrt(movAvgStateStruct[0].avg*movAvgStateStruct[0].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg)) * 180.0 / PI;	// atan(y / sqrt(x^2 + z^2))
	printf("%g\n", alphaAngle);
}


// 100Hz interrupt
void TIM3_IRQHandler() {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if (!runACC) {
			runACC = 1 - runACC;
		}
  }
}

void EXTI0_IRQHandler() {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line0);
 		toggleLED();
	}
}
