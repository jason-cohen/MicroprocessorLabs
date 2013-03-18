
#include "DMA.h"

__IO TestStatus  TransferStatus = FAILED;
// const uint32_t SRC_Const_Buffer[BUFFER_SIZE]= {
//                                     0x01020304,0x05060708,0x090A0B0C,0x0D0E0F10,
//                                     0x11121314,0x15161718};
uint32_t DST_Buffer[BUFFER_SIZE];
uint32_t spi1TxBuffer[BUFFER_SIZE + 1] = {LIS302DL_OUT_X_ADDR, 
															DUMMY_BYTE, DUMMY_BYTE, 
															DUMMY_BYTE, DUMMY_BYTE, 
															DUMMY_BYTE, DUMMY_BYTE};

uint32_t spiRxBuffer[BUFFER_SIZE];

void DMA_init() {
	//program initiates SPI1 and confirms that polling works.
	LIS302DL_CS_HIGH(); //GPIO_SetBits(GPIOA, GPIO_Pin_4); //Set CS Off
	SPI_Cmd(SPI1, DISABLE);
	 
	uint16_t SPI_MASTER_Buffer_Rx[BUFFER_SIZE] = {99,99,99,99,99};
	uint16_t SPI_MASTER_Buffer_Tx[BUFFER_SIZE] = {0x8000, 0, 0x7E00,  0x8003,  0};
	 
	RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_DMA2, ENABLE);
	DMA_Cmd(Rx_DMA, DISABLE);
	DMA_Cmd(Tx_DMA, DISABLE);
	 
	/* SPI_MASTER_Rx_DMA_Channel configuration ---------------------------------*/
	DMA_DeInit(Rx_DMA);
	DMA_InitTypeDef DMA_InitStructure;
	DMA_StructInit(&DMA_InitStructure);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(LIS302DL_SPI->DR); //not sure about this
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)spiRxBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = (uint32_t)BUFFER_SIZE;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_Channel = DMA_Channel_3;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	//  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(Rx_DMA, &DMA_InitStructure);
	 
	/* SPI_MASTER_Tx_DMA_Channel configuration ---------------------------------*/
	DMA_DeInit(Tx_DMA);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(LIS302DL_SPI->DR); //not sure about this
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)spi1TxBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(Tx_DMA, &DMA_InitStructure);
	 
	DMA_ITConfig(Rx_DMA, DMA_IT_TC, ENABLE);
	 
	/* Enable SPI_MASTER DMA Tx request */
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx | SPI_I2S_DMAReq_Rx, ENABLE);
	/* Enable SPI_MASTER DMA Rx request */
	//SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx, ENABLE);
	 
	SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_TXE);
	SPI_I2S_ClearFlag(SPI1, SPI_I2S_FLAG_RXNE);
	LIS302DL_CS_LOW(); //GPIO_ResetBits(GPIOA, GPIO_Pin_4); //Set CS On
	/* Enable SPI_MASTER */
	SPI_Cmd(LIS302DL_SPI, ENABLE);
	 
	/* Enable DMA channels */
	DMA_Cmd(DMA2_Stream0, ENABLE);
	DMA_Cmd(DMA2_Stream5, ENABLE);
	 
	//while(!DMA_GetFlagStatus(DMA2_Stream0, DMA_FLAG_TCIF0));
	while(!DMA_GetFlagStatus(DMA2_Stream5, DMA_FLAG_TCIF5));
	LIS302DL_CS_HIGH(); //GPIO_SetBits(GPIOA, GPIO_Pin_4); //Set CS Off
	
	
	
	
	
	
	
//   __IO uint32_t    Timeout = TIMEOUT_MAX;
//   DMA_InitTypeDef       DMA_InitStructure;

//   /* Enable DMA clock */
//   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
// 	
// 	/* Reset DMA Stream registers (for debug purpose) */
//   DMA_DeInit(DMA2_Stream0);
// 	
//   /* Check if the DMA Stream is disabled before enabling it.
//      Note that this step is useful when the same Stream is used multiple times:
//      enabled, then disabled then re-enabled... In this case, the DMA Stream disable
//      will be effective only at the end of the ongoing data transfer and it will 
//      not be possible to re-configure it before making sure that the Enable bit 
//      has been cleared by hardware. If the Stream is used only once, this step might 
//      be bypassed. */
//   while (DMA_GetCmdStatus(DMA2_Stream0) != DISABLE);
//   
//   /* Configure DMA Stream */
// 	DMA_StructInit(&DMA_InitStructure);
// 	/* DMA2 Stream0 channel0 configuration **************************************/
//   DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
//   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(LIS302DL_SPI->DR);
//   DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) DST_Buffer;
//   DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//   DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;
//   DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//   DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//   DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//   DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//   DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//   DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
//   DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//   DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//   DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//   DMA_Init(DMA2_Stream0, &DMA_InitStructure);
//   DMA_Cmd(DMA2_Stream0, ENABLE);

//   
// 	/* Enable DMA Stream Transfer Complete interrupt */
//   DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
// 	
// 	/* Enable SPI1 DMA */
//   SPI_I2S_DMACmd(LIS302DL_SPI, SPI_I2S_DMAReq_Rx, ENABLE);
//   /* DMA Stream enable */
//   DMA_Cmd(DMA2_Stream0, ENABLE);

//   /* Check if the DMA Stream has been effectively enabled.
//      The DMA Stream Enable bit is cleared immediately by hardware if there is an 
//      error in the configuration parameters and the transfer is no started (ie. when
//      wrong FIFO threshold is configured ...) */
//   Timeout = TIMEOUT_MAX;
//   while ((DMA_GetCmdStatus(DMA2_Stream0) != ENABLE) && (Timeout-- > 0));
//    
//   /* Check if a timeout condition occurred */
//   if (Timeout == 0) {
//     /* Manage the error: to simplify the code enter an infinite loop */
//     while (1);
//   }
}


/**
  * @brief  Configure the DMA controller according to the Stream parameters
  *         defined in main.h file 
  * @param  None
  * @retval None
  */
void DMA_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  __IO uint32_t    Timeout = TIMEOUT_MAX;
    
  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
  
  /* Reset DMA Stream registers (for debug purpose) */
  DMA_DeInit(Rx_DMA);

  /* Check if the DMA Stream is disabled before enabling it.
     Note that this step is useful when the same Stream is used multiple times:
     enabled, then disabled then re-enabled... In this case, the DMA Stream disable
     will be effective only at the end of the ongoing data transfer and it will 
     not be possible to re-configure it before making sure that the Enable bit 
     has been cleared by hardware. If the Stream is used only once, this step might 
     be bypassed. */
  while (DMA_GetCmdStatus(Rx_DMA) != DISABLE);
  
  /* Configure DMA Stream */
	DMA_StructInit(&DMA_InitStructure);
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(LIS302DL_SPI->DR);
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DST_Buffer;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);
    
  /* Enable DMA Stream Transfer Complete interrupt */
  DMA_ITConfig(Rx_DMA, DMA_IT_TC, ENABLE);

  /* DMA Stream enable */
  DMA_Cmd(Rx_DMA, ENABLE);

  /* Check if the DMA Stream has been effectively enabled.
     The DMA Stream Enable bit is cleared immediately by hardware if there is an 
     error in the configuration parameters and the transfer is no started (ie. when
     wrong FIFO threshold is configured ...) */
  Timeout = TIMEOUT_MAX;
  while ((DMA_GetCmdStatus(DMA2_Stream0) != ENABLE) && (Timeout-- > 0))
  {
  }
   
  /* Check if a timeout condition occurred */
  if (Timeout == 0)
  {
    /* Manage the error: to simplify the code enter an infinite loop */
    while (1)
    {
    }
  }

  /* Enable the DMA Stream IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);     
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer, pBuffer1: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer identical to pBuffer1
  *         FAILED: pBuffer differs from pBuffer1
  */
TestStatus Buffercmp(const uint32_t* pBuffer, uint32_t* pBuffer1, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer != *pBuffer1)
    {
      return FAILED;
    }
    
    pBuffer++;
    pBuffer1++;
  }

  return PASSED;  
}
