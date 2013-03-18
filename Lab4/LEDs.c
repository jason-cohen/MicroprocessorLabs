#include "LEDs.h"



void initGPIOD_LED() {
    
	// GPIO clock setup
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	// GPIO initialization structure
	GPIO_InitTypeDef gpio_init_s;
	GPIO_StructInit(&gpio_init_s);

	// GPIO settings
	gpio_init_s.GPIO_Pin = BOARD_LEDS; 					// Select LED pins
	gpio_init_s.GPIO_Mode = GPIO_Mode_OUT; 		  // Set as output
	gpio_init_s.GPIO_Speed = GPIO_Speed_100MHz; // Don't limit slew rate
	gpio_init_s.GPIO_OType = GPIO_OType_PP; 		// Push-pull
	gpio_init_s.GPIO_PuPd = GPIO_PuPd_NOPULL; 	// Not input, don't pull
	
	// Map settings to GPIOD for usage
	GPIO_Init(GPIOD, &gpio_init_s);							// Configure pin for GPIOD
}

void toggleLED() {
	//Literally flips the register bits...
	GPIO_ToggleBits(GPIOD, BOARD_LEDS);
}

void toggleLEDs(uint16_t LEDs) {
	//Literally flips the register bits...
	GPIO_ToggleBits(GPIOD, LEDs);
}
