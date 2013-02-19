#include "stdio.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"
#include "math.h"
#include "movingAverage.h"
#include "timerInterrupt.h"
#include "ACC.h"
#include "GPIO.h"

#define CALIB_ACC 0
#define RUN_GPIO 1
#define RUN_ACC 1
#define RUN_TIM 1
#define RUN_ACC_EXTI 0
#define RUN_MOV_AVG 1

#define NUM_AXES 3
#define PI 3.1415

void calibrateData(void);
void calculateAngles(void);
void Delay(uint32_t);

int32_t accData[NUM_AXES] = {0, 0, 0};
float calibratedData[NUM_AXES] = {0.0, 0.0, 0.0};
float alphaAngle, betaAngle;
movAvgState movAvgStateStruct[NUM_AXES];
uint8_t count = 0;

static uint8_t runACC = 0;




// Acquired during the calibration testing
const float calibrationACC[3][4] = {{1.0384, 0.0096, -0.0001, 0.0683},
                                    {0.0095, 1.0288, -0.0093, -0.0240},
                                    {0.0186, 0.0092, 0.9916, -0.0020}};

int main() {
    
	int i;
// 	RCC_ClocksTypeDef RCC_ClocksTypeStruct;
// 	RCC_GetClocksFreq(&RCC_ClocksTypeStruct);
// 	printf("%d\n%d\n%d\n%d\n",RCC_ClocksTypeStruct.SYSCLK_Frequency, RCC_ClocksTypeStruct.HCLK_Frequency, RCC_ClocksTypeStruct.PCLK1_Frequency, RCC_ClocksTypeStruct.PCLK2_Frequency);
// 	return 0;
	// Run setups based on debugging settings
#if RUN_GPIO 
    initGPIO();
#endif
    
#if RUN_TIM
    initNVIC();
    initTIM3();
#endif
    
#if RUN_ACC || CALIB_ACC
    initAcc();
#endif
    
#if RUN_ACC_EXTI 
    initAccClickInterrupt();
#endif
    
#if RUN_MOV_AVG
    for (i = 0; i < NUM_AXES; i++) {
        movingAverageInit(&movAvgStateStruct[i]);
    }
#endif
    
    
    
#if CALIB_ACC
    while(1) {
        printf("X:%d\tY:%d\tZ:%d\n", accData[0], accData[1], accData[2]);
    }
#else
    while (1) {
			Delay(0x7FFFF);	// wait for approx. 10 ms.
                    
#if RUN_GPIO
//                 toggleLED();            
#endif
        
#if RUN_ACC
			LIS302DL_ReadACC(accData);
			calibrateData();
			calculateAngles();
			
#endif
    }
#endif
}

void calibrateData() {
    uint8_t r, c;
    
    calibratedData[0] = 0;
    calibratedData[1] = 0;
    calibratedData[2] = 0;
    int32_t tmp[NUM_AXES] = {accData[0], accData[1], accData[2]};
    
    for (r = 0; r < 3; r++) {
        for (c = 0; c < 3; c++) {
            calibratedData[r] += calibrationACC[r][c] * tmp[c] / 1000.0;
        }
        calibratedData[r] += calibrationACC[r][c];
    }
}

void calculateAngles() {
	int i;
#if RUN_MOV_AVG
		// Calcuate angles based on filtered data
		for (i = 0; i < NUM_AXES; i++) {
				movingAverage(&movAvgStateStruct[i], calibratedData[i]);
		}
		alphaAngle = atan2(movAvgStateStruct[0].avg, sqrt(movAvgStateStruct[1].avg*movAvgStateStruct[1].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg)) * 180.0 / PI;	// atan(x / sqrt(y^2 + z^2))
		betaAngle = atan2(movAvgStateStruct[1].avg, sqrt(movAvgStateStruct[0].avg*movAvgStateStruct[0].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg)) * 180.0 / PI;	// atan(y / sqrt(x^2 + z^2))

#else
		// Calculate angles based on calibrated data only
		alphaAngle = atan2(calibratedData[0], sqrt(calibratedData[1]*calibratedData[1] + calibratedData[2]*calibratedData[2])) * 180.0 / PI;	// atan(x / sqrt(y^2 + z^2))
		betaAngle = atan2(calibratedData[1], sqrt(calibratedData[0]*calibratedData[0] + calibratedData[2]*calibratedData[2])) * 180.0 / PI;	// atan(y / sqrt(x^2 + z^2))
#endif
}

void Delay(uint32_t nTime) {
		while(nTime--);
}

// 100Hz interrupt
void TIM3_IRQHandler() {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		runACC = 1 - runACC;
		
#if RUN_GPIO
		if (++count == 100) {
			count = 0;
			toggleLED();            
		}
#endif
    
#if RUN_ACC || CALIB_ACC
//     calibrateData();
// 		calculateAngles();
// 		
//     printf("RAW:\tX:%f\tY:%f\tZ:%f\n", accData[0]/1000.0, accData[1]/1000.0, accData[2]/1000.0);
//     printf("CAL:\tX:%f\tY:%f\tZ:%f\n", calibratedData[0], calibratedData[1], calibratedData[2]);
// 		printf("ANGLE:\ta:%f\tb:%f\n", alphaAngle, betaAngle);
// 		printf("--------------------------------------------------\n");
#endif
  }
}
