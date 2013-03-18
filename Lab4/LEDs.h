#ifndef __GPIO_H_
#define __GPIO_H_

// Includes
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

// Global variable declarations
static uint16_t LD4 = GPIO_Pin_12;
static uint16_t LD3 = GPIO_Pin_13;
static uint16_t LD5 = GPIO_Pin_14;
static uint16_t LD6 = GPIO_Pin_15;
static uint16_t BOARD_LEDS = GPIO_Pin_15 | GPIO_Pin_14 | GPIO_Pin_13 | GPIO_Pin_12;    // LD6, LD5, LD4, LD3

/**
 * Initializes the GPIO for the 4 LEDs
*/
void initGPIOD_LED(void);    // Setup GPIOD for LEDs

/**
 * Toggles the LEDs
*/
void toggleLED(void);   // Toggles the LEDs using GPIOD

/**
 * Toggles the LEDs
*/
void toggleLEDs(uint16_t);   // Toggles the LEDs using GPIOD

#endif	// __GPIO_H_
