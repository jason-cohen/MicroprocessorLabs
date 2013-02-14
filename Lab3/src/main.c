#include "stdio.h"
#include "ACC.h"
#include "GPIO.h"
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"
#include "movingAverage.h"
#include "timerInterrupt.h"

#define CALIB_ACC 0
#define RUN_GPIO 0
#define RUN_ACC 1
#define RUN_TIM 1
#define RUN_ACC_EXTI 0
#define RUN_MOV_AVG 0

#define NUM_AXES 3

void calibrateData(void);


float calibratedData[NUM_AXES] = {0.0, 0.0, 0.0};
int32_t accData[NUM_AXES] = {0, 0, 0};


// Acquired during the calibration testing
const float calibrationACC[3][4] = {{1.0384, 0.0096, -0.0001, 0.0683},
                                    {0.0095, 1.0288, -0.0093, -0.0240},
                                    {0.0186, 0.0092, 0.9916, -0.0020}};

int main() {
    
    int i;
    

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
    movAvgState movAvgStateStruct[NUM_AXES];
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
                    
        #if RUN_GPIO
//                 toggleLED();            
        #endif
        
        #if RUN_ACC
        LIS302DL_ReadACC(accData);
        #endif
        
        #if RUN_MOV_AVG 
        for (i = 0; i < NUM_AXES; i++) {
            movingAverage(&movAvgStateStruct[i], accData[i]);
        }
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



// 100Hz interrupt
void TIM3_IRQHandler() {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    #if RUN_GPIO
    toggleLED();            
    #endif
    
    #if RUN_ACC || CALIB_ACC
    calibrateData();
    printf("RAW:\tX:%f\tY:%f\tZ:%f\n", accData[0]/1000.0, accData[1]/1000.0, accData[2]/1000.0);
    printf("CAL:\tX:%f\tY:%f\tZ:%f\n", calibratedData[0], calibratedData[1], calibratedData[2]);
    #endif
  }
}
