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

/* Function prototypes */
void calculateAngles(void);

/* Variable declarations */
float alphaAngle, betaAngle;
movAvgState movAvgStateStruct[NUM_AXES];
uint8_t runACC = 0, count = 0, toggle = 0;



int main() {
    
	uint8_t i;
	
	// Run setups based on debugging settings

	initGPIOD_LED();

#if RUN_TIM
  initNVIC_TIM3();
	initTIM3();
#endif
    
	initAcc();
	
#if RUN_ACC_CLICK
	initEXTI0();
	initNVIC_EXTI0();
	initAccClickInterrupt();
#endif    


	for (i = 0; i < NUM_AXES; i++) {
		movingAverageInit(&movAvgStateStruct[i]);
	}
  
#if CALIB_ACC
	while(1) {
		printf("X:%d\tY:%d\tZ:%d\n", accData[0], accData[1], accData[2]);
	}
#else
	while (1) {
		if (runACC) {
			LIS302DL_ReadACC(accData);
			calibrateData();
			calculateAngles();
			runACC = 1 - runACC;
		}
	}
#endif
}

void calculateAngles() {
	uint8_t i;
	// Calcuate angles based on filtered data
	for (i = 0; i < NUM_AXES; i++) {
		movingAverage(&movAvgStateStruct[i], calibratedData[i]);
	}
	alphaAngle = atan2(movAvgStateStruct[0].avg, sqrt(movAvgStateStruct[1].avg*movAvgStateStruct[1].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg)) * 180.0 / PI;	// atan(x / sqrt(y^2 + z^2))
	betaAngle = atan2(movAvgStateStruct[1].avg, sqrt(movAvgStateStruct[0].avg*movAvgStateStruct[0].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg)) * 180.0 / PI;	// atan(y / sqrt(x^2 + z^2))
}


// 100Hz interrupt
void TIM3_IRQHandler() {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		if (!runACC) {
			runACC = 1 - runACC;
		}
		if (!toggle) {
			if(++count == 100) {
				toggleLED();
				count = 0;
			}
		}
  }
}

void EXTI0_IRQHandler() {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line0);
// 		toggleLED();
		toggle = 1 - toggle;
	}
}
