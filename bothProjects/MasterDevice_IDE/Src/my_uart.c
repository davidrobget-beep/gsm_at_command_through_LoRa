#include <my_uart.h>

#include <main.h>

// ALIGN this
uint8_t volatile TransmitBuffer[TRANSMIT_BUFFER_LEN];
uint8_t volatile ReceiveBuffer[RECEIVE_RING_BUFFER_LEN];


static void MyUart_SystickHandler (void);

struct MY_UART my_uart = {

	.TransmitCurState = TX_STOPPED,
	.IdleLineDetected = { .lastPosition = RECEIVE_RING_BUFFER_LEN, },

	.SystickHandler = &MyUart_SystickHandler,
};

//enum TransmitStates TransmitCurState = TX_STOPPED;


// pause - after that transmission ( but the pause can be reset with PAUSE_AFTER_TRANSMIT_SKIP )
void MY_UART_Transmit(uint32_t len, uint16_t pause){

			//   clear the TXE/ RXNE flags in the USART_ISR register.
			//temp32 = USART2->RDR;
			//USART2->TDR = 0x00;
	
			
			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, len);
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
			my_uart.TransmitCurState = TX_ONGOING;

			my_uart.pauseAfterTransmit = pause;

}

// DMA in Circular mode
void MY_UART_Receive(uint32_t len){
	
			
			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, len);
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
			

}

void MY_UART_SetTransmissionBuffer( const char * message, uint16_t length ){
	
	if ( length > TRANSMIT_BUFFER_LEN )
		length = TRANSMIT_BUFFER_LEN;								// FIX. return an error
	
	for ( uint16_t ii = 0; ii < length; ii++ ){
	
			TransmitBuffer[ii] = (uint8_t)(message[ii]);
	
	}

}

void MY_UART_AddToTransmissionBuffer( const char * message, uint16_t length, uint16_t offset ){
	
	if ( length + offset > TRANSMIT_BUFFER_LEN )
		length = TRANSMIT_BUFFER_LEN - offset;						// FIX. return an error
	
	for ( uint16_t ii = 0; ii < length; ii++ ){
	
			TransmitBuffer[offset + ii] = (uint8_t)(message[ii]);
	
	}

}


void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};


  HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 0, 2);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX
  */
  GPIO_InitStruct.Pin = E220_900T30D_TX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(E220_900T30D_TX_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = E220_900T30D_RX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(E220_900T30D_RX_GPIO_Port, &GPIO_InitStruct);


  /* USART2_RX Init */
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MODE_CIRCULAR);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MDATAALIGN_BYTE);

  /* USART2_TX Init */
  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MDATAALIGN_BYTE);

  /* USART2 interrupt Init */
  NVIC_SetPriority(USART2_IRQn, 0);
  NVIC_EnableIRQ(USART2_IRQn);


  USART_InitStruct.BaudRate = 9600;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_DisableIT_CTS(USART2);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_Enable(USART2);

	/* Polling USART2 initialisation */
while((!(LL_USART_IsActiveFlag_TEACK(USART2))) || (!(LL_USART_IsActiveFlag_REACK(USART2))))
{
}

	// USART + DMA. Transmit
	LL_USART_EnableDMAReq_TX(USART2);
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, (uint32_t)TransmitBuffer, (uint32_t)(&(USART2->TDR)), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);
	LL_USART_ClearFlag_TC(USART2);	// check it before clearing
	LL_USART_EnableIT_TC(USART2);

	// USART + DMA. Receive
	LL_USART_EnableDMAReq_RX(USART2);
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, (uint32_t)(&(USART2->RDR)), (uint32_t)ReceiveBuffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
	//LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_5);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5);
	LL_USART_EnableIT_ERROR(USART2);
	LL_USART_EnableIT_IDLE(USART2);

}

static void MyUart_SystickHandler (void){

	if ( my_uart.IdleLineDetected.pauseAfterReceiving )
		my_uart.IdleLineDetected.pauseAfterReceiving--;

	//if ( my_uart.TransmitCurState == TX_COMPLETE && my_uart.pauseAfterTransmit ){
	if ( my_uart.pauseAfterTransmit ){
			my_uart.pauseAfterTransmit--;
	}

}

