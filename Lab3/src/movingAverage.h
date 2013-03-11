#ifndef __MOVINGAVERAGE_H_
#define __MOVINGAVERAGE_H_

#define d 10

// Struct definition
typedef struct {
	float memory[d];
	float avg;
	float oneOverD;
	float *lastValue;
} movAvgState;

/**
 * Initializes the MovingAverageState
 * @param State represents the movAvgState pointer to be initialized.
*/
void movingAverageInit(movAvgState *);

/**
 * Adds a value to moving average and calculates the new average (storing it back into State)
 * @param State represents the movAvgState pointer that holds current state.
 * @param newValue represents the new value to add into the moving average filter.
*/
void movingAverage(movAvgState *, float);

#endif		// __MOVINGAVERAGE_H_
