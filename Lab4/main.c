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
#include "DMA.h"

#define ACC_SAMPLE_FLAG 0x01
#define TEMP_SAMPLE_FLAG 0x01	

#define USE_DMA 1

/* Variable declarations */

float alphaAngle = 0.0;					// Roll angle, x axis and horizontal plane
float betaAngle = 0.0;					// Pitch angle, y axis and horizontal plane

float alphaRad = 0.0;					// Roll angle, x axis and horizontal plane
float betaRad = 0.0;					// Pitch angle, y axis and horizontal plane

float xOffset = 0.0;						// Gravity offset for calculating horizontal movement
float yOffset = 0.0;						// Gravity offset for calculating horizontal movement

float xAccel = 0.0;							// Summation of all x direction accelerations
float yAccel = 0.0;							// Summation of all y direction accelerations

// int8_t txBuffer[BUFFER_SIZE] = {LIS302DL_OUT_X_ADDR|0x40|0x80, 
// 															DUMMY_BYTE, DUMMY_BYTE, 
// 															DUMMY_BYTE, DUMMY_BYTE, 
// 															DUMMY_BYTE, DUMMY_BYTE};
// int8_t rxBuffer[BUFFER_SIZE] = {DUMMY_BYTE,
// 															DUMMY_BYTE, DUMMY_BYTE, 
// 															DUMMY_BYTE, DUMMY_BYTE, 
// 															DUMMY_BYTE, DUMMY_BYTE};

// Temperature measurement modulo 8 for use with the board LEDs
uint8_t tempModulo = 0;

//Moving average struct
movAvgState movAvgStateStruct[NUM_AXES];
movAvgState movAvgTemperature;

// Select tap mode
uint8_t tapSelection = 0;
// Select button mode
uint8_t buttonSelection = 0;
// Used to prevent ACC tap detection during button press
uint8_t buttonPressed = 0;
// Scale factor so that ADC polls at 20Hz instead of 100Hz
uint8_t ADCIntCount = 5;
// Flag to be set when DMA has complete transfer
uint8_t dmaDataReady = 0;
															
// Semaphore definitions
osSemaphoreDef(temp_s)
osSemaphoreDef(acc_s)
osSemaphoreId temp_sid;
osSemaphoreId acc_sid;

/*!
 @brief Thread to perform temperature measurements
 @param argument Unused
 */
void threadTempSense(void const * argument);
/*!
 @brief Thread to perform accelerometer measurements
 @param argument Unused
 */
void threadACC(void const *argument);

//! Thread structure for above thread
osThreadDef(threadTempSense, osPriorityNormal, 1, 0);
osThreadDef(threadACC, osPriorityNormal, 1, 0);
// ID for threads
osThreadId tid_threadTempSense;
osThreadId tid_threadACC;

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
 @brief Program entry point
 */
int main (void) {
	uint8_t i;
	
	// Semaphore initialization
	temp_sid = osSemaphoreCreate(osSemaphore(temp_s), 1);
	acc_sid = osSemaphoreCreate(osSemaphore(acc_s), 1);
	
	initGPIOD_LED();
	initGPIOA_BUTTON();

// Initializes the timer and interrupt handler for the accelerometer polling
  initNVIC_TIM3();					//Interrupt Handler
	initTIM3();								//Timer
  
	initAcc();
	temperatureSensorInit();
	#if USE_DMA
	DMA_init();
	#endif
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
			if (xAccel > 0.005) {
				GPIO_SetBits(GPIOD, LD6);
				GPIO_ResetBits(GPIOD, LD3);
			} else if (xAccel < -0.005) {
				GPIO_SetBits(GPIOD, LD3);
				GPIO_ResetBits(GPIOD, LD6);
			} else {
				GPIO_ResetBits(GPIOD, LD3);
				GPIO_ResetBits(GPIOD, LD6);
			}
			
			if (yAccel > 0.005) {
				GPIO_SetBits(GPIOD, LD5);
				GPIO_ResetBits(GPIOD, LD4);
			} else if (yAccel < -0.005) {
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
		osSignalWait(TEMP_SAMPLE_FLAG,osWaitForever);
		ADC_SoftwareStartConv(ADC1);

		while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
		ADC_ClearFlag(ADC1, ADC_FLAG_EOC);  		// Read from ADC
		
		// Calculate the temperature using conversions
		calculateTemperature();
		//osSignalClear(tid_threadTempSense, TEMP_SAMPLE_FLAG); //Clear the sample flag
	}
}

void threadACC(void const *argument) {
	#if USE_DMA
	uint32_t i = 0;
	
	LIS302DL_CS_LOW(); //Lower CS line

	//DMA2_Stream0->M0AR = (uint32_t)rxptr;
	Rx_DMA->NDTR = BUFFER_SIZE;
	Rx_DMA->M0AR = (uint32_t)rxBuffer;

	//DMA2_Stream3->M0AR = (uint32_t)txptr;
	Tx_DMA->NDTR = BUFFER_SIZE;
	Tx_DMA->M0AR = (uint32_t)txBuffer;

  DMA_Cmd(Rx_DMA, ENABLE);
	DMA_Cmd(Tx_DMA, ENABLE);
	
	#endif
	while (1) {
		//Toggled at a rate of 100hz
		osSignalWait(ACC_SAMPLE_FLAG,osWaitForever);
		#if USE_DMA
		if (dmaDataReady) {
			//osSemaphoreWait(acc_sid, osWaitForever); //Have exclusive access to temperature
			
			int32_t* out = &accData[0];
			// Scale the values from DMA to the actual values
			// This was done in LIS302DL_ReadACC() when using the LIS302 driver
			for(i=0; i<0x03; i++) {
					*out =(int32_t)(18 *  (int8_t)rxBuffer[2*i +1]);
					out++;
			}
		#endif
		#if !USE_DMA
			LIS302DL_ReadACC(accData);
		#endif
			//We calibrate the data (filters the values)
			calibrateData();
			
			//Calculate the angles using trig
			calculateAngles();
			
			determineMovement();
			
			//osSemaphoreRelease(acc_sid);
			#if USE_DMA
			// Setup DMA to run again
			LIS302DL_CS_LOW(); //Lower CS line
			
// 			DMA_Cmd(Rx_DMA, DISABLE);
// 			DMA_Cmd(Tx_DMA, DISABLE);

			//DMA2_Stream0->M0AR = (uint32_t)rxptr;
			Rx_DMA->M0AR = (uint32_t)rxBuffer;
			Tx_DMA->M0AR = (uint32_t)txBuffer;
			
			Rx_DMA->NDTR = BUFFER_SIZE;
			Tx_DMA->NDTR = BUFFER_SIZE;
			
			DMA_Cmd(Rx_DMA, ENABLE);
			DMA_Cmd(Tx_DMA, ENABLE);
			
			
			
			dmaDataReady = 0; //Clear DMA flag0
			//osSignalClear(tid_threadACC, ACC_SAMPLE_FLAG); //Clear the sample flag
		}
		#endif
		osSignalClear(tid_threadACC, ACC_SAMPLE_FLAG);
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
	
	alphaRad = atan2(movAvgStateStruct[0].avg, sqrt(movAvgStateStruct[1].avg*movAvgStateStruct[1].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg));
	betaRad = atan2(movAvgStateStruct[1].avg, sqrt(movAvgStateStruct[0].avg*movAvgStateStruct[0].avg + movAvgStateStruct[2].avg*movAvgStateStruct[2].avg));
	alphaAngle = alphaRad * 180.0 / PI;	// atan(x / sqrt(y^2 + z^2))
	betaAngle = betaRad * 180.0 / PI;	// atan(y / sqrt(x^2 + z^2))
	
	xOffset = arm_cos_f32(PI/2 - alphaRad);
	yOffset = arm_cos_f32(PI/2 - betaRad);
}

//Doxygen comments in def.
void determineMovement() {
	float tmp;
	tmp = movAvgStateStruct[0].avg - xOffset;
	tmp *= arm_cos_f32(alphaRad);
	//if (tmp > 0.05)
		xAccel = tmp;
	tmp = movAvgStateStruct[1].avg - yOffset;
	tmp *= arm_cos_f32(betaRad);
	
	//if (tmp > 0.05)
		yAccel = tmp;
}

// 100Hz interrupt
void TIM3_IRQHandler() {
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		osSignalSet(tid_threadACC, ACC_SAMPLE_FLAG);
		if (--ADCIntCount <= 0) {
			osSignalSet(tid_threadTempSense,TEMP_SAMPLE_FLAG);
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

/**
*@brief An interrupt handler for DMA2_Stream0
*@retval None
*/
void DMA2_Stream0_IRQHandler(void)
{
	if (DMA_GetITStatus(Rx_DMA, DMA_IT_TCIF0) != RESET) {
		DMA_ClearITPendingBit(Rx_DMA,DMA_IT_TCIF0);
		DMA_ClearFlag(Rx_DMA, DMA_FLAG_TCIF0); //Clear the flag for transfer complete
		DMA_ClearFlag(Tx_DMA, DMA_FLAG_TCIF3);
		
		dmaDataReady = 1;				//Set flag for accelerometer sampling

		LIS302DL_CS_HIGH();  //Raise CS Line
		
		DMA_Cmd(Rx_DMA, DISABLE); /// RX
		DMA_Cmd(Tx_DMA, DISABLE); /// TX
	}
	
}
