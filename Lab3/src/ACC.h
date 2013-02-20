#ifndef __ACC_H
#define __ACC_H

/* Library includes */
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stdio.h"
#include "stm32f4_discovery_lis302dl.h"
#include "math.h"

/* Defines */
#define NUM_AXES 3

/* Variable declarations */
extern int32_t accData[NUM_AXES];
extern float calibratedData[NUM_AXES];

// Acquired during the calibration testing
extern const float calibrationACC[3][4];

/* Function prototypes */
void initAcc(void);
void initAccClickInterrupt(void);
void calibrateData(void);
	
#endif // __ACC_H
