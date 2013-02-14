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
    LIS302DL_InterruptConfigStruct.Latch_Request = LIS302DL_INTERRUPTREQUEST_LATCHED;
    LIS302DL_InterruptConfigStruct.SingleClick_Axes = LIS302DL_CLICKINTERRUPT_XYZ_ENABLE;
    LIS302DL_InterruptConfigStruct.DoubleClick_Axes = LIS302DL_DOUBLECLICKINTERRUPT_XYZ_DISABLE;
    
    // Setup interrupt using specications
    LIS302DL_InterruptConfig(&LIS302DL_InterruptConfigStruct);
    
    
}
