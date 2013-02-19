#ifndef __MOVINGAVERAGE_H_
#define __MOVINGAVERAGE_H_

#define d 5

// Struct definition
typedef struct {
	float memory[d];
	float avg;
	float oneOverD;
	float *lastValue;
} movAvgState;

void movingAverageInit(movAvgState *);
void movingAverage(movAvgState *, float);

#endif		// __MOVINGAVERAGE_H_
