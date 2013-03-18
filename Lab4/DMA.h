#ifndef __DMA_H
#define __DMA_H

/* Library includes */
#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4_discovery_lis302dl.h"
//#include "stm32f4_discovery_lis302dl.h"

/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE              6
#define TIMEOUT_MAX              10000 /* Maximum timeout value */
#define Rx_DMA DMA2_Stream0
#define Tx_DMA DMA2_Stream1
#define ADC3_DR_ADDRESS ((uint32_t)0x4001224C)
#define SPI1_DR_ADDRESS ((uint32_t)0x4001300C)
#define DUMMY_BYTE ((uint8_t)0x00)

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Global variables ---------------------------------------------------------*/
extern __IO TestStatus  TransferStatus;
extern const uint32_t SRC_Const_Buffer[BUFFER_SIZE];
extern uint32_t DST_Buffer[BUFFER_SIZE];
extern uint32_t spi1TxBuffer[BUFFER_SIZE + 1];
extern uint32_t spiRxBuffer[BUFFER_SIZE];

/* Private function prototypes -----------------------------------------------*/
void DMA_init(void);
void DMA_Config(void);
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength);

#endif // __DMA_H
