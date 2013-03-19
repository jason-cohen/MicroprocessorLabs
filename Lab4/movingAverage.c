#include "stdint.h"
#include <stdio.h>
#include "movingAverage.h"


void movingAverageInit(movAvgState* newState) {
	newState->oneOverD = (float) 1 / d; // Store 1/d for multiplication after
	newState->lastValue = &newState->memory[0]; // Pointer to last value initialized to the first memory location
	newState->avg = 0; // Avg initialized to 0
	int i;
	for (i = 0; i < d; i++) {
		newState->memory[i] = 0; // Clear all data in the memory locations
	}
}

void movingAverage(movAvgState *currState, float newValue) {
	// Subtract the last value from the average
	currState->avg -= *currState->lastValue;
	// Replace the last value with the new value from input (factored by 1/d)
	*currState->lastValue = newValue * currState->oneOverD;
	// Add the new factored value to the average
	currState->avg += *currState->lastValue;
	
	// Shift the pointer to the last value to the new last value
	// using a circular array
	if (currState->lastValue == &currState->memory[d-1]) {
		currState->lastValue = &currState->memory[0];
	} else {
		currState->lastValue += 1;
	}
}


