#ifndef __GPIO_H_
#define __GPIO_H_

// Includes
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

// Global variable declarations
static uint16_t BOARD_LEDS = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;    // LD6, LD5, LD4, LD3

// Function prototypes
void initGPIO(void);    // Setup GPIOD for LEDs
void toggleLED(void);   // Toggles the LEDs using GPIOD

#endif	// __GPIO_H_
