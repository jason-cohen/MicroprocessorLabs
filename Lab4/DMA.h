#ifndef __DMA_H
#define __DMA_H

/* Library includes */
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"
#include "interrupts.h"

/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE              7
#define Rx_DMA DMA2_Stream0
#define Tx_DMA DMA2_Stream3
#define DUMMY_BYTE ((uint8_t)0x00)

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Global variables ---------------------------------------------------------*/
extern int8_t txBuffer[BUFFER_SIZE];
extern int8_t rxBuffer[BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
void DMA_init(void);
#endif // __DMA_H
