#include "arm_math.h"

#include "stm32f4xx.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "movingAverage.h"
#include "ACC.h"
#include "ADC.h"
#include "LEDs.h"
#include "button.h"
#include "interrupts.h"

/* Variable declarations */

float alphaAngle = 0.0;					// Roll angle, x axis and horizontal plane
float betaAngle = 0.0;					// Pitch angle, y axis and horizontal plane

float xOffset = 0.0;						// Gravity offset for calculating horizontal movement
float yOffset = 0.0;						// Gravity offset for calculating horizontal movement

float xAccel = 0.0;							// Summation of all x direction accelerations
float yAccel = 0.0;							// Summation of all y direction accelerations

// Temperature measurement modulo 8 for use with the board LEDs
uint8_t tempModulo = 0;

//Moving average struct
movAvgState movAvgStateStruct[NUM_AXES];
movAvgState movAvgTemperature;

// ID for thread
osThreadId tid_threadTempSense, tid_threadACC;
// Select tap mode
uint8_t tapSelection = 0;
// Select button mode
uint8_t buttonSelection = 0;
// Used to prevent ACC tap detection during button press
uint8_t buttonPressed = 0;
// Scale factor so that ADC polls at 20Hz instead of 100Hz
uint8_t ADCIntCount = 5;

/**
 * Converts the ADC data into a temperature measurement
 * tempModulo is the temperature used for the LEDs
 */
void calculateTemperature(void);

/**
 * Converts the accelerometer data into Pitch and Roll
 * alphaAngle get the Roll 
 * betaAngle gets the Pitch
 */
void calculateAngles(void);

/**
 * Converts the measured accelerometer data into a movement vector
 * X measured: movAvgStateStruct[0].avg
 * Y measured: movAvgStateStruct[1].avg
 */
void determineMovement(void);
/*!
 @brief Thread to perform menial tasks such as switching LEDs
 @param argument Unused
 */
void threadTempSense(void const * argument);
void threadACC(void const *argument);

//! Thread structure for above thread
osThreadDef(threadTempSense, osPriorityNormal, 1, 0);
osThreadDef(threadACC, osPriorityNormal, 1, 0);

/*!
 @brief Program entry point
 */
int main (void) {
	uint8_t i;
	
	// Run setups based on debugging settings

	initGPIOD_LED();
	initGPIOA_BUTTON();

// Initializes the timer and interrupt handler for the accelerometer polling
  initNVIC_TIM3();					//Interrupt Handler
	initTIM3();								//Timer
  
	initAcc();
	temperatureSensorInit();
	
// Configuration for setting up the tapping interrupt.
	initEXTI0();
	initNVIC_EXTI0();
	initAccClickInterrupt();

	// Initialize the moving average structures (x, y, z), so 0 to NUM_AXES (3)
	for (i = 0; i < NUM_AXES; i++) {
		movingAverageInit(&movAvgStateStruct[i]);
	}
	
	// Initialize the moving average structure for the temperature sensor
	movingAverageInit(&movAvgTemperature);
	
	// Start thread
	tid_threadTempSense = osThreadCreate(osThread(threadTempSense), NULL);
	tid_threadACC = osThreadCreate(osThread(threadACC), NULL);

	// The below doesn't really need to be in a loop
	while(1){
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)) {
			buttonSelection = 1 - buttonSelection;
			buttonPressed = 1;
			osDelay(100);
			while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0));
			buttonPressed = 0;
			GPIO_ResetBits(GPIOD, BOARD_LEDS);
		}
		//osSemaphoreWait(tap_sid, osWaitForever);
		if (tapSelection && buttonSelection) {
			// display ACC angle LEDs
			if (alphaAngle > 10) {
				GPIO_SetBits(GPIOD, LD6);
				GPIO_ResetBits(GPIOD, LD3);
			} else if (alphaAngle < -10) {
				GPIO_SetBits(GPIOD, LD3);
				GPIO_ResetBits(GPIOD, LD6);
			} else {
				GPIO_ResetBits(GPIOD, LD3);
				GPIO_ResetBits(GPIOD, LD6);
			}
			
			if (betaAngle > 10) {
				GPIO_SetBits(GPIOD, LD5);
				GPIO_ResetBits(GPIOD, LD4);
			} else if (betaAngle < -10) {
				GPIO_SetBits(GPIOD, LD4);
				GPIO_ResetBits(GPIOD, LD5);
			} else {
				GPIO_ResetBits(GPIOD, LD4);
				GPIO_ResetBits(GPIOD, LD5);
			}
		} else if (tapSelection && !buttonSelection) {
			// display ACC movement LEDs
			if (xAccel > 0.01) {
				GPIO_SetBits(GPIOD, LD6);
				GPIO_ResetBits(GPIOD, LD3);
			} else if (xAccel < -0.01) {
				GPIO_SetBits(GPIOD, LD3);
				GPIO_ResetBits(GPIOD, LD6);
			} else {
				GPIO_ResetBits(GPIOD, LD3);
				GPIO_ResetBits(GPIOD, LD6);
			}
			
			if (yAccel > 0.01) {
				GPIO_SetBits(GPIOD, LD5);
				GPIO_ResetBits(GPIOD, LD4);
			} else if (yAccel < -0.01) {
				GPIO_SetBits(GPIOD, LD4);
				GPIO_ResetBits(GPIOD, LD5);
			} else {
				GPIO_ResetBits(GPIOD, LD4);
				GPIO_ResetBits(GPIOD, LD5);
			}
		} else if (!tapSelection && buttonSelection) {
			if(tempModulo >= 6)
			{
				GPIO_ResetBits(GPIOD, BOARD_LEDS);
				GPIO_SetBits(GPIOD, LD3);
			}
			else if(tempModulo >= 4) 
			{
				GPIO_ResetBits(GPIOD, BOARD_LEDS);
				GPIO_SetBits(GPIOD, LD4);
			}
			else if(tempModulo >= 2)
			{
				GPIO_ResetBits(GPIOD, BOARD_LEDS);
				GPIO_SetBits(GPIOD, LD6);
			}
			else
			{
				GPIO_ResetBits(GPIOD, BOARD_LEDS);
				GPIO_SetBits(GPIOD, LD5);
			}
		} else {
			// display flashing lights
			toggleLED();
			
		}
		//osSemaphoreRelease(tap_sid);
		osDelay(50);
	}
}

void threadTempSense (void const *argument) {
	while(1){
		//Toggled at a rate of 20hz
		osSignalWait(2,osWaitForever);
		ADC_SoftwareStartConv(ADC1);

		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);  		// Read from ADC
		
		// Calculate the temperature using conversions
		calculateTemperature();
		
	}
}

void threadACC(void const *argument) {
	while (1) {
		//Toggled at a rate of 100hz
		osSignalWait(1,osWaitForever);
		
		//We read from the accelerometer to obtain the raw data.
		LIS302DL_ReadACC(accData);
		
		//We calibrate the data (filters the values)
		calibrateData();
		
		//Calculate the angles using trig
		calculateAngles();
		
		determineMovement();
	}
}


//Doxygen comments in def.
void calculateTemperature() {
	// Convert reading to temperature and add to the moving average filter
	movingAverage(&movAvgTemperature, convertBinaryToTemp(ADC_GetConversionValue(ADC1)));
	
	tempModulo = (uint32_t)movAvgTemperature.avg % 8;
}

//Doxygen comments in def.
void calculateAngles() {
	uint8_t i;
	// Calcuate angles based on filtered data
	for (i = 0; i < NUM_AXES; i++) {
		
		//Add new data to moving average filter
		movingAverage(&movAvgStateStruct[i], calibratedData[i]);
	}
	
	xOffset = atan2(movAvgStateStruct[0].avg, sqrt(movAvgStateStruct[1].avg*movAvgStateStruct[1].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg));
	yOffset = atan2(movAvgStateStruct[1].avg, sqrt(movAvgStateStruct[0].avg*movAvgStateStruct[0].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg));
	alphaAngle = xOffset * 180.0 / PI;	// atan(x / sqrt(y^2 + z^2))
	betaAngle = yOffset * 180.0 / PI;	// atan(y / sqrt(x^2 + z^2))
	
	xOffset = sin(xOffset);
	yOffset = sin(yOffset);
}

//Doxygen comments in def.
void determineMovement() {
	xAccel += (movAvgStateStruct[0].avg - xOffset);
	yAccel += (movAvgStateStruct[1].avg - yOffset);
}

// 100Hz interrupt
void TIM3_IRQHandler() {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		osSignalSet(tid_threadACC, 1);
		if (--ADCIntCount <= 0) {
			osSignalSet(tid_threadTempSense,2);
			ADCIntCount = 5;
		}
  }
}

void EXTI0_IRQHandler() {
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		EXTI_ClearITPendingBit(EXTI_Line0);
		if (buttonPressed == 0) {
			tapSelection = 1 - tapSelection;
		}
// 		osSignalSet(tid_thread, 1);
		
	}
}
