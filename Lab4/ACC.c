#include "ACC.h"

/* Variable declarations */
int32_t accData[NUM_AXES] = {0, 0, 0};
float calibratedData[NUM_AXES] = {0.0, 0.0, 0.0};

// Acquired during the calibration testing
const float calibrationACC[3][4] = {{1.0384, 0.0096, -0.0001, 0.0683},
                                    {0.0095, 1.0288, -0.0093, -0.0240},
                                    {0.0186, 0.0092, 0.9916, -0.0020}};

void initAcc() {

	LIS302DL_InitTypeDef LIS302DL_InitStruct; // Declare structure
	
	/* Set configuration of LIS302DL */
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;  // Use low-power mode
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;    // Sample at 100Hz
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;    // Enable all 3 sensors (x, y, z)
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;				// Scale on 2G's
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	
	LIS302DL_Init(&LIS302DL_InitStruct); // Actually initialize the accelerometer
}

void initAccClickInterrupt() {
	uint8_t ctrl = 0;
	
	// Declare structure
	LIS302DL_InterruptConfigTypeDef LIS302DL_InterruptConfigStruct;
	
	/* Configure Interrupt control register: enable Click interrupt1 */
	ctrl = 0x07;
	LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);
	
	// Interrupt specifications
	LIS302DL_InterruptConfigStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_NOTLATCHED;
	LIS302DL_InterruptConfigStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_Z_ENABLE;
	LIS302DL_InterruptConfigStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_XYZ_DISABLE;
	
	// Setup interrupt using specifications
	LIS302DL_InterruptConfig(&LIS302DL_InterruptConfigStruct);
	
	/* Configure Click Threshold on X/Y axis (15 x 0.5g) */	
	ctrl = 0xFF;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSY_X_REG_ADDR, 1);

	/* Configure Click Threshold on Z axis (15 x 0.5g) */
	ctrl = 0x0F;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSZ_REG_ADDR, 1);

	/* Configure Time Limit of 1.5 ms*/
	ctrl = 0x03;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_TIMELIMIT_REG_ADDR, 1);
	 
	/* Configure Latency of 250 ms*/
	ctrl = 0xFA;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_LATENCY_REG_ADDR, 1);

	/* Configure Click Window of 255 ms*/
	ctrl = 0xFF;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_WINDOW_REG_ADDR, 1);
}

void calibrateData() {
	uint8_t r, c;
  int32_t tmp[NUM_AXES] = {accData[0], accData[1], accData[2]};  
	
	calibratedData[0] = 0;
	calibratedData[1] = 0;
	calibratedData[2] = 0;
	
	
	for (r = 0; r < 3; r++) {
		for (c = 0; c < 3; c++) {
			calibratedData[r] += calibrationACC[r][c] * tmp[c] / 1000.0;
		}
		calibratedData[r] += calibrationACC[r][c];
	}
}
