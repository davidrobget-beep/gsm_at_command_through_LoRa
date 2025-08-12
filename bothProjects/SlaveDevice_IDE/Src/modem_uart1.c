#include "uart.h"
#include "modem_uart1.h"
#include <main.h>
#include <string.h>

#include "my_lib_.h"
#include "crc_.h"
#include "queue_m.h"

extern struct QUEUE_MANAGER queueManager;

// ALIGN this
static uint8_t TransmitBuffer[MODEM_UART1_TRANSMIT_BUFFER_LEN];
static uint8_t ReceiveBuffer[MODEM_UART1_RECEIVE_RING_BUFFER_LEN];
static uint8_t LinearBuffer[MODEM_UART1_RECEIVE_RING_BUFFER_LEN+1];



static void MODEM_UART1_Init(void);

static void MODEM_UART1_Transmit(uint32_t len, uint16_t unused);
static void MODEM_UART1_ReceiveEnable(uint32_t len);
static void MODEM_UART1_SetTransmissionBuffer( const char * message, uint16_t length );
static void MODEM_UART1_AddToTransmissionBuffer( const char * message, uint16_t length, uint16_t offset );

static void MODEM_UART1_Receive (void);

static void MODEM_UART1_DMA_Handler(void);		// IRQ
static void MODEM_UART1_IRQHandler(void);		// IRQ
static void MODEM_UART1_SysTick(void);			// IRQ


struct UART_INSTANCE MODEM_UART1_ = {

	.TransmitBuffer = TransmitBuffer,
	.ReceiveBuffer  = ReceiveBuffer,
	.Linear_Buffer  = LinearBuffer,
	.TransmitCurState = TX_STOPPED,

	//.USART_or_DMA_error
	.IdleLineDetected = {
			.lastPosition = MODEM_UART1_RECEIVE_RING_BUFFER_LEN,
	},

	.Init = &MODEM_UART1_Init,									// place into Peripheral Init section
	.Transmit = &MODEM_UART1_Transmit,
	.ReceiveEnable = &MODEM_UART1_ReceiveEnable,				// place after Peripheral Init section
	.SetTransmissionBuffer = &MODEM_UART1_SetTransmissionBuffer,
	.AddToTransmissionBuffer = &MODEM_UART1_AddToTransmissionBuffer,
	.Receive = &MODEM_UART1_Receive,							// place in main loop
	.DMA_Handler = &MODEM_UART1_DMA_Handler,					// place in DMA_IRQ_Hander
	.UART_IRQHandler = &MODEM_UART1_IRQHandler,					// place in UART_IRQ_Hander
	.SysTickHandler = &MODEM_UART1_SysTick,						// place in SysTick

};

static void MODEM_UART1_Transmit(uint32_t len, uint16_t unused){

			//   clear the TXE/ RXNE flags in the USART_ISR register.
			//temp32 = USART2->RDR;
			//USART2->TDR = 0x00;


			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, len);
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
			MODEM_UART1_.TransmitCurState = TX_ONGOING;

			MODEM_UART1_.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_TO_MODEM;			// FIX ?

}

// DMA in Circular mode
static void MODEM_UART1_ReceiveEnable(uint32_t len){


			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, len);
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);


}

static void MODEM_UART1_SetTransmissionBuffer( const char * message, uint16_t length ){

	if ( length > MODEM_UART1_TRANSMIT_BUFFER_LEN )
		length = MODEM_UART1_TRANSMIT_BUFFER_LEN;													// FIX. return an error

	for ( uint16_t ii = 0; ii < length; ii++ ){

		MODEM_UART1_.TransmitBuffer[ii] = (uint8_t)(message[ii]);

	}

}

static void MODEM_UART1_AddToTransmissionBuffer( const char * message, uint16_t length, uint16_t offset ){

	if ( length + offset > MODEM_UART1_TRANSMIT_BUFFER_LEN )
		length = MODEM_UART1_TRANSMIT_BUFFER_LEN - offset;													// FIX. return an error

	for ( uint16_t ii = 0; ii < length; ii++ ){

		MODEM_UART1_.TransmitBuffer[offset + ii] = (uint8_t)(message[ii]);

	}

}

static void MODEM_UART1_Init(void)
{

	LL_USART_InitTypeDef USART_InitStruct = {0};

	  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	  LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);

	  /* Peripheral clock enable */
	  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
	  //LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	  /**USART1 GPIO Configuration
	  PB7   ------> USART1_RX
	  PA11 [PA9]   ------> USART1_CTS				not used
	  PA12 [PA10]   ------> USART1_RTS				not used
	  PB6   ------> USART1_TX
	  */
	  GPIO_InitStruct.Pin = GSM_RX_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
	  LL_GPIO_Init(GSM_RX_GPIO_Port, &GPIO_InitStruct);

	/*  GPIO_InitStruct.Pin = GSM_CTS_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
	  LL_GPIO_Init(GSM_CTS_GPIO_Port, &GPIO_InitStruct);

	  GPIO_InitStruct.Pin = GSM_RTS_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
	  LL_GPIO_Init(GSM_RTS_GPIO_Port, &GPIO_InitStruct);
	*/

	  GPIO_InitStruct.Pin = GSM_TX_Pin;
	  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	  GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
	  LL_GPIO_Init(GSM_TX_GPIO_Port, &GPIO_InitStruct);

  /* USART1 DMA Init */

  /* USART1_RX Init */
	LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_2, LL_DMAMUX_REQ_USART1_RX);

	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_2, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

	LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PRIORITY_LOW);

	LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MODE_CIRCULAR);

	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PERIPH_NOINCREMENT);

	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MEMORY_INCREMENT);

	LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_PDATAALIGN_BYTE);

	LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_2, LL_DMA_MDATAALIGN_BYTE);

	/* USART1_TX Init */
	LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_3, LL_DMAMUX_REQ_USART1_TX);

	LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_3, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

	LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PRIORITY_LOW);

	LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MODE_NORMAL);

	LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PERIPH_NOINCREMENT);

	LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MEMORY_INCREMENT);

	LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_PDATAALIGN_BYTE);

	LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_3, LL_DMA_MDATAALIGN_BYTE);

  /* USART2 interrupt Init */
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
  USART_InitStruct.BaudRate = 9600;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;	//LL_USART_HWCONTROL_RTS_CTS;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_DisableFIFO(USART1);
  LL_USART_ConfigAsyncMode(USART1);


  LL_USART_Enable(USART1);

    /* Polling USART1 initialisation */
    while((!(LL_USART_IsActiveFlag_TEACK(USART1))) || (!(LL_USART_IsActiveFlag_REACK(USART1))))
    {
    }

	// USART + DMA. Transmit
	LL_USART_EnableDMAReq_TX(USART1);
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3, (uint32_t)MODEM_UART1_.TransmitBuffer, (uint32_t)(&(USART1->TDR)), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_3);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_3);
	LL_USART_ClearFlag_TC(USART1);	// check it before clearing
	LL_USART_EnableIT_TC(USART1);

	// USART + DMA. Receive
	LL_USART_EnableDMAReq_RX(USART1);
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_2, (uint32_t)(&(USART1->RDR)), (uint32_t)MODEM_UART1_.ReceiveBuffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);
	//LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_2);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_2);
	LL_USART_EnableIT_ERROR(USART1);
	LL_USART_EnableIT_IDLE(USART1);

}


static void __attribute__((optimize("O0"))) MODEM_UART1_DMA_Handler(void){

		if ( LL_DMA_IsActiveFlag_TC2(DMA1) ) {					// RECEIVING

			LL_DMA_ClearFlag_TC2(DMA1);
			MODEM_UART1_.IdleLineDetected.dmaTC++;

		} else if ( LL_DMA_IsActiveFlag_TE2(DMA1) ){

			LL_DMA_ClearFlag_TE2(DMA1);
			MODEM_UART1_.USART_or_DMA_error.DMA_TransferError_uartRX = 1;
		}


		if ( LL_DMA_IsActiveFlag_TC3(DMA1) ) {

			LL_DMA_ClearFlag_TC3(DMA1);						   // TRANSMISSION
			LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);

		} else if ( LL_DMA_IsActiveFlag_TE3(DMA1) ){

			LL_DMA_ClearFlag_TE3(DMA1);
			MODEM_UART1_.USART_or_DMA_error.DMA_TransferError_uartTX = 1;
		}

}

static void MODEM_UART1_IRQHandler(void){

	//debugUART.usart2++;

		// T R A N S M I S S I O N
		if ( LL_USART_IsActiveFlag_TC(USART1)){

			LL_USART_ClearFlag_TC(USART1);
			MODEM_UART1_.TransmitCurState = TX_COMPLETE;
		}

		// R E C E P T I O N
		if ( LL_USART_IsActiveFlag_IDLE(USART1) ) {

			LL_USART_ClearFlag_IDLE(USART1);
			//IdleLineDetected.idle++;			// = 1;

			//*******************************************************
			MODEM_UART1_.IdleLineDetected.currPosition = ((DMA_Channel_TypeDef *)(__LL_DMA_GET_CHANNEL_INSTANCE(DMA1, LL_DMA_CHANNEL_2)))->CNDTR;

					if ( MODEM_UART1_.IdleLineDetected.lastPosition > MODEM_UART1_.IdleLineDetected.currPosition ) {

						if ( MODEM_UART1_.IdleLineDetected.dmaTC == 0 ){
							MODEM_UART1_.IdleLineDetected.End = MODEM_UART1_.IdleLineDetected.lastPosition - MODEM_UART1_.IdleLineDetected.currPosition;
							MODEM_UART1_.IdleLineDetected.Start = 0;
						} else {	// We have buffer overrun/overflow
							MODEM_UART1_.IdleLineDetected.End = 0;
							MODEM_UART1_.IdleLineDetected.Start = 0;
						}

					} else {	// if ( IdleLineDetected.lastPosition <= IdleLineDetected.currPosition )

						if ( MODEM_UART1_.IdleLineDetected.dmaTC == 1 ){
							MODEM_UART1_.IdleLineDetected.End = MODEM_UART1_.IdleLineDetected.lastPosition;
							MODEM_UART1_.IdleLineDetected.Start = MODEM_UART1_RECEIVE_RING_BUFFER_LEN - MODEM_UART1_.IdleLineDetected.currPosition;
						} else {
							// if ( IdleLineDetected.dmaTC == 0 ) { it is imposible }
							// if ( IdleLineDetected.dmaTC > 1  ) { overflow        }
							MODEM_UART1_.IdleLineDetected.End = 0;
							MODEM_UART1_.IdleLineDetected.Start = 0;
						}

					}


			//the Length of received bytes =  IdleLineDetected.End + IdleLineDetected.Start
			// if the number of received bytes is more than	MODEM_UART1_RECEIVE_RING_BUFFER_LEN then
			// 		IdleLineDetected.End = 0 and IdleLineDetected.Start = 0 in that algorithm
			// It does not need to know the exactly number of received bytes if
			//   that quantity is more than MODEM_UART1_RECEIVE_RING_BUFFER_LEN. The main point is to define that "bigData"
			//   to avoid ALL THAT RECEPRION.

			if ( MODEM_UART1_.IdleLineDetected.End + MODEM_UART1_.IdleLineDetected.Start > 0 ){
				MODEM_UART1_.IdleLineDetected.received = 1;


				MODEM_UART1_.IdleLineDetected.pauseAfterReceiving = MODEM_UART1_PAUSE_AFTER_RECEIVING;

			} else {		// for example, I received too many of bytes ( erasing occures)

				MODEM_UART1_.IdleLineDetected.dmaTC = 0;
				MODEM_UART1_.IdleLineDetected.lastPosition = MODEM_UART1_.IdleLineDetected.currPosition;
				if ( MODEM_UART1_.IdleLineDetected.received )
					MODEM_UART1_.IdleLineDetected.received = 0;
			}




			//*******************************************************

		} else if ( LL_USART_IsActiveFlag_FE(USART1) ){

			LL_USART_ClearFlag_FE(USART1);
			MODEM_UART1_.USART_or_DMA_error.USART_Error_RX++;			// = 1;

		} else if ( LL_USART_IsActiveFlag_NE(USART1) ){

			LL_USART_ClearFlag_NE(USART1);
			MODEM_UART1_.USART_or_DMA_error.USART_Error_RX++;			// = 1;

		} else if ( LL_USART_IsActiveFlag_ORE(USART1) ){

			LL_USART_ClearFlag_ORE(USART1);
			MODEM_UART1_.USART_or_DMA_error.USART_Error_RX++;			// = 1;

		}


}


// temporary
static void MODEM_UART1_Receive (void){

	static uint32_t offset;
	static uint16_t length;
	static uint8_t ii;

		// handling messages: moving to linear array and parsing.
		if ( MODEM_UART1_.IdleLineDetected.received && !MODEM_UART1_.IdleLineDetected.pauseAfterReceiving ){
			MODEM_UART1_.IdleLineDetected.received = 0;

			// begin * * * * copy all received data from cyclic format into linear format

			if ( MODEM_UART1_.IdleLineDetected.End > 0 ){		// End is alwase more than 0.   delete this if
				if ( MODEM_UART1_.IdleLineDetected.lastPosition > MODEM_UART1_.IdleLineDetected.currPosition )
					offset = MODEM_UART1_RECEIVE_RING_BUFFER_LEN-MODEM_UART1_.IdleLineDetected.currPosition-MODEM_UART1_.IdleLineDetected.End;
				else{
					offset = MODEM_UART1_RECEIVE_RING_BUFFER_LEN-MODEM_UART1_.IdleLineDetected.End;
				}
					 //memcpy(MODEM_UART1_.Linear_Buffer, MODEM_UART1_.ReceiveBuffer + offset, IdleLineDetected.End);
					my_memcpy(MODEM_UART1_.Linear_Buffer, MODEM_UART1_.ReceiveBuffer + offset, MODEM_UART1_.IdleLineDetected.End);
			}

			if ( MODEM_UART1_.IdleLineDetected.Start > 0 ){
				 //memcpy(MODEM_UART1_.Linear_Buffer+MODEM_UART1_.IdleLineDetected.End, MODEM_UART1_.ReceiveBuffer, MODEM_UART1_.IdleLineDetected.Start);
				my_memcpy(MODEM_UART1_.Linear_Buffer+MODEM_UART1_.IdleLineDetected.End, MODEM_UART1_.ReceiveBuffer, MODEM_UART1_.IdleLineDetected.Start);
			}

		MODEM_UART1_.IdleLineDetected.dmaTC = 0;																			// was coped from USART2_IRQHandler()
		MODEM_UART1_.IdleLineDetected.lastPosition = MODEM_UART1_.IdleLineDetected.currPosition;	// was coped from USART2_IRQHandler()

		// end * * * * copy all received data from cyclic format into linear format



			length = MODEM_UART1_.IdleLineDetected.End + MODEM_UART1_.IdleLineDetected.Start;	// length of the current message
			MODEM_UART1_.Linear_Buffer[length] = 0;	// string termination. in ALL modes



			// * * *   P A R S I N G   * * *

			// . . .

			// I may to check the length if It is more than queueManager can add.

			static uint8_t tempNode[Q_MESSAGE_LEN];
			static uint8_t * pData;
			static uint8_t CompleteMessageNumber;
			static uint8_t UnCompleteMessageNumber;
			static uint8_t TotalMessageNumber;
			static uint8_t index_;
			static union paramLL CRC_VAL;

			CompleteMessageNumber     = length / MESSAGE_LEN;
			UnCompleteMessageNumber   = length % MESSAGE_LEN;			// part of the full packet
			if ( UnCompleteMessageNumber )
				ii = 1;
			else
				ii = 0;

			TotalMessageNumber = CompleteMessageNumber + ii;

			// i can check TotalMessageNumber ( if it is out of Queue size )

			pData = MODEM_UART1_.Linear_Buffer;	// offset 0

			index_ = 0;		// !
			tempNode[ADDRESS_INDEX] = GET_FULL_ADDRESS(MODEM_ADDRESS,HOST_ADDRESS);		// common for all packets
			tempNode[CMD_INDEX] = __CMD_RETRANSMIT;										// common for all packets

			for ( ; index_ < CompleteMessageNumber; index_++ ){

				{
					tempNode[MESSAGE_INDEX_INDEX] = GET_FULL_MESSAGE_INDEX(index_+1, TotalMessageNumber);

					my_memcpy(tempNode+FIRST_BYTE_INDEX, pData, MESSAGE_LEN);

					CalculateCRC_32bit(tempNode, Q_MESSAGE_LEN-CRC_LEN, &CRC_VAL);
					for ( ii = 0; ii < CRC_LEN; ii++ ){
						tempNode[Q_MESSAGE_LEN-CRC_LEN + ii] = CRC_VAL.par_bytes[ii];
					}

					if ( queueManager.add( tempNode, Q_MESSAGE_LEN, QUEUE_ANSWER_REQUIRED ) == Q_result_ER ){
						queueManager.queue_overflows++;
					}
				}

				pData += MESSAGE_LEN;
			}

			if ( UnCompleteMessageNumber ){

				if ( index_ != 0 ){		// if we have only one message ( cycle above was not run )
					index_++;
					tempNode[MESSAGE_INDEX_INDEX] = GET_FULL_MESSAGE_INDEX(index_, TotalMessageNumber);
				} else {
					tempNode[MESSAGE_INDEX_INDEX] = 0;
				}


				my_memcpy(tempNode+FIRST_BYTE_INDEX, pData, UnCompleteMessageNumber);

				CalculateCRC_32bit(tempNode, FIRST_BYTE_INDEX+UnCompleteMessageNumber, &CRC_VAL);
				for ( ii = 0; ii < CRC_LEN; ii++ ){
					tempNode[FIRST_BYTE_INDEX+UnCompleteMessageNumber + ii] = CRC_VAL.par_bytes[ii];
				}

				if ( queueManager.add( tempNode, UnCompleteMessageNumber+TECH_INFO, QUEUE_ANSWER_REQUIRED ) == Q_result_ER ){
					queueManager.queue_overflows++;
				}
			}



/*static uint16_t yy;
static uint16_t ii;

				yy = length - CRC_LEN;
				CalculateCRC_32bit(MODEM_UART1_.Linear_Buffer, yy, &CRC_VAL);

				ii = 0;
				for ( ii = 0 ; ii < CRC_LEN ; ii++ ){
					if ( !(MODEM_UART1_.Linear_Buffer[yy+ii] == CRC_VAL.par_bytes[ii] ) )
						break;
				}

				if ( ii < 4 ){	// bad
static uint8_t test_mes[11] = {0x30, 0x00, 'B', 'A', 'D', '=', 'C', 'R', 'C', '\r', '\n', };
					//MODEM_UART1_SetTransmissionBuffer( (char *)MODEM_UART1_.Linear_Buffer, length );
					MODEM_UART1_SetTransmissionBuffer( (char *)test_mes, 11 );
					MODEM_UART1_Transmit(length);
				} else {
static uint8_t test_mes[11] = {0x30, 0x00, '=', '=', '1', '2', '3', 'C', 'R', '\r', '\n', };
					//MODEM_UART1_SetTransmissionBuffer( (char *)MODEM_UART1_.Linear_Buffer, length );
					MODEM_UART1_SetTransmissionBuffer( (char *)test_mes, 11 );
					MODEM_UART1_Transmit(length);

				}
*/









		}

}

static void MODEM_UART1_SysTick(void){
	if (MODEM_UART1_.IdleLineDetected.pauseAfterReceiving )
		MODEM_UART1_.IdleLineDetected.pauseAfterReceiving--;

	//if ( MODEM_UART1_.TransmitCurState == TX_COMPLETE && MODEM_UART1_.pauseAfterTransmit ){
	if ( MODEM_UART1_.pauseAfterTransmit ){
		MODEM_UART1_.pauseAfterTransmit--;
	}
}

