#include "ACC.h"

void initAcc() {

	LIS302DL_InitTypeDef LIS302DL_InitStruct; // Declare structure
	
	/* Set configuration of LIS302DL */
	LIS302DL_InitStruct.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE;  // Use low-power mode
	LIS302DL_InitStruct.Output_DataRate = LIS302DL_DATARATE_100;    // Sample at 100Hz
	LIS302DL_InitStruct.Axes_Enable = LIS302DL_X_ENABLE | LIS302DL_Y_ENABLE | LIS302DL_Z_ENABLE;    // Enable all 3 sensors (x, y, z)
	LIS302DL_InitStruct.Full_Scale = LIS302DL_FULLSCALE_2_3;
	LIS302DL_InitStruct.Self_Test = LIS302DL_SELFTEST_NORMAL;
	
	LIS302DL_Init(&LIS302DL_InitStruct); // Actually initialize the accelerometer
}

void initAccClickInterrupt() {
	
	// Declare structure
	LIS302DL_InterruptConfigTypeDef LIS302DL_InterruptConfigStruct;
	
	// Interrupt specifications
	LIS302DL_InterruptConfigStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_NOTLATCHED;
	LIS302DL_InterruptConfigStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_XYZ_ENABLE;
	LIS302DL_InterruptConfigStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_XYZ_DISABLE;
	
	// Setup interrupt using specications
	LIS302DL_InterruptConfig(&LIS302DL_InterruptConfigStruct);
}
void clickConfig(void) {
		
	uint8_t ctrl = 0;
	
	/* Configure Interrupt control register: enable Click interrupt1 */
	ctrl = 0x07;
	LIS302DL_Write(&ctrl, LIS302DL_CTRL_REG3_ADDR, 1);

	/* Enable Interrupt generation on single click on all axis
	!!!!!	 FOR SINGLE CLICK
	*/
	ctrl = 0x3F;
	
	/* Enable Interrupt generation on double click on all axis
	!!!!!	 FOR DOUBLE CLICK
	*/	
	// ctrl = 0x7F;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_CFG_REG_ADDR, 1);

	/* Configure Click Threshold on X/Y axis (10 x 0.5g) */
	ctrl = 0xFF;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSY_X_REG_ADDR, 1);

	/* Configure Click Threshold on Z axis (10 x 0.5g) */
	ctrl = 0x0F;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_THSZ_REG_ADDR, 1);

	/* Configure Time Limit */
	ctrl = 0x03;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_TIMELIMIT_REG_ADDR, 1);
	 
	/* Configure Latency */
	ctrl = 0xFA;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_LATENCY_REG_ADDR, 1);

	/* Configure Click Window */
	ctrl = 0xFF;
	LIS302DL_Write(&ctrl, LIS302DL_CLICK_WINDOW_REG_ADDR, 1);
}
