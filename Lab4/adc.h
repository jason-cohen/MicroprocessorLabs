#ifndef __ADC_H
#define __ADC_H

/** @brief Initializes the ADC
 *
 * This initializes the ADC and its temperature sensor, so that we may read
 * digital values from it, which we then convert to temperatures.
 */
void temperatureSensorInit(void);

float convertBinaryToTemp(uint16_t);
	
#endif // __ADC_H
