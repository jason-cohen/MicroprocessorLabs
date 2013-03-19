
#include "DMA.h"

int8_t txBuffer[BUFFER_SIZE] = {LIS302DL_OUT_X_ADDR|0x40|0x80, 
															DUMMY_BYTE, DUMMY_BYTE, 
															DUMMY_BYTE, DUMMY_BYTE, 
															DUMMY_BYTE, DUMMY_BYTE};

int8_t rxBuffer[BUFFER_SIZE] = {DUMMY_BYTE,
 															DUMMY_BYTE, DUMMY_BYTE, 
 															DUMMY_BYTE, DUMMY_BYTE, 
 															DUMMY_BYTE, DUMMY_BYTE};

void DMA_init() {
	 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_Cmd(Rx_DMA, DISABLE);
	DMA_Cmd(Tx_DMA, DISABLE);
	 
	/* SPI_MASTER_Rx_DMA_Channel configuration ---------------------------------*/
	
	DMA_InitTypeDef DMA_InitStructure;
	DMA_DeInit(Rx_DMA);
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(LIS302DL_SPI->DR); //not sure about this
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;

	DMA_Init(Rx_DMA, &DMA_InitStructure);
	 
	/* SPI_MASTER_Tx_DMA_Channel configuration ---------------------------------*/
	DMA_DeInit(Tx_DMA);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(LIS302DL_SPI->DR); //not sure about this
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(Tx_DMA, &DMA_InitStructure);
	 
	DMA_ITConfig(Rx_DMA, DMA_IT_TC, ENABLE);
	 
	/* Enable the DMA Stream IRQ Channel */
	initNVIC_DMA2();
	 
	/* Enable DMA channels */
// 	DMA_Cmd(Rx_DMA, ENABLE);
// 	DMA_Cmd(Tx_DMA, ENABLE);
	
	
	
	SPI_DMACmd(SPI1, SPI_DMAReq_Rx | SPI_DMAReq_Tx, ENABLE); //Start Rx dma on SPI1
	
}