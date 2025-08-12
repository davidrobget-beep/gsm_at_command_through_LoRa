#include "uart.h"
#include "e220_uart2.h"
#include <main.h>
#include <string.h>

#include "my_lib_.h"
#include "crc_.h"
#include "queue_m.h"
#include "answers_e220_900t30d.h"
#include "modem_state.h"
#include "buzzer.h"
#include "statistics.h"

#include "sl_debug.h"

extern struct QUEUE_MANAGER queueManager;
extern const struct answer_  e220_900t30d_Answers[];
extern struct __VarialbeAnswer VarialbeAnswer;
extern const ___COMMAND Commands_[];
extern const void (*Commands_Handlers[]) ( uint8_t * , uint8_t );
extern struct MODEM modem_;
extern struct Buzzers buzzer;
extern struct STATICTICS_i stat_info;

extern uint16_t Current_distance_number_milliseconds;

// ALIGN this
static uint8_t TransmitBuffer[E220_UART2_TRANSMIT_BUFFER_LEN];
static uint8_t ReceiveBuffer[E220_UART2_RECEIVE_RING_BUFFER_LEN];
static uint8_t e220_900t30d_Buffer[E220_UART2_RECEIVE_RING_BUFFER_LEN+1];	// Linear buffer


static void E220_UART2_Init(void);

static void E220_UART2_Transmit(uint32_t len, uint16_t pause);
static void E220_UART2_ReceiveEnable(uint32_t len);
static void E220_UART2_SetTransmissionBuffer( const char * message, uint16_t length );
static void E220_UART2_AddToTransmissionBuffer( const char * message, uint16_t length, uint16_t offset );

static void E220_UART2_Receive (void);

static void E220_UART2_DMA_Handler(void);		// IRQ
static void E220_UART2_IRQHandler(void);		// IRQ
static void E220_UART2_SysTick(void);			// IRQ

static uint8_t CheckData( uint8_t nReceived );
static ___COMMAND parseData( uint8_t nReceived);

struct UART_INSTANCE E220_UART2_ = {

	.TransmitBuffer = TransmitBuffer,
	.ReceiveBuffer  = ReceiveBuffer,
	.Linear_Buffer  = e220_900t30d_Buffer,
	.TransmitCurState = TX_STOPPED,

	//.USART_or_DMA_error
	.IdleLineDetected = {
			.lastPosition = E220_UART2_RECEIVE_RING_BUFFER_LEN,
	},

	.ping_answer_time_to_buzzer_on = 0,

	.Init = &E220_UART2_Init,									// place into Peripheral Init section
	.Transmit = &E220_UART2_Transmit,
	.ReceiveEnable = &E220_UART2_ReceiveEnable,					// place after Peripheral Init section
	.SetTransmissionBuffer = &E220_UART2_SetTransmissionBuffer,
	.AddToTransmissionBuffer = &E220_UART2_AddToTransmissionBuffer,
	.Receive = &E220_UART2_Receive,								// place in main loop
	.CheckData = &CheckData,
	.parseData = &parseData,
	.DMA_Handler = &E220_UART2_DMA_Handler,						// place in DMA_IRQ_Hander
	.UART_IRQHandler = &E220_UART2_IRQHandler,					// place in UART_IRQ_Hander
	.SysTickHandler = &E220_UART2_SysTick,						// place in SysTick

};

static void E220_UART2_Transmit(uint32_t len, uint16_t pause){

			//   clear the TXE/ RXNE flags in the USART_ISR register.
			//temp32 = USART2->RDR;
			//USART2->TDR = 0x00;
	
			
			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_5, len);
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_5);
			E220_UART2_.TransmitCurState = TX_ONGOING;

			E220_UART2_.pauseAfterTransmit = pause;	//PAUSE_AFTER_TRANSMIT

}

// DMA in Circular mode
static void E220_UART2_ReceiveEnable(uint32_t len){
	
			
			LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_4, len);
			LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_4);
			

}

static void E220_UART2_SetTransmissionBuffer( const char * message, uint16_t length ){
	
	if ( length > E220_UART2_TRANSMIT_BUFFER_LEN )
		length = E220_UART2_TRANSMIT_BUFFER_LEN;													// FIX. return an error
	
	for ( uint16_t ii = 0; ii < length; ii++ ){
	
		E220_UART2_.TransmitBuffer[ii] = (uint8_t)(message[ii]);
	
	}

}

static void E220_UART2_AddToTransmissionBuffer( const char * message, uint16_t length, uint16_t offset ){
	
	if ( length + offset > E220_UART2_TRANSMIT_BUFFER_LEN )
		length = E220_UART2_TRANSMIT_BUFFER_LEN - offset;													// FIX. return an error
	
	for ( uint16_t ii = 0; ii < length; ii++ ){
	
		E220_UART2_.TransmitBuffer[offset + ii] = (uint8_t)(message[ii]);
	
	}

}

static void E220_UART2_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX
  */
  GPIO_InitStruct.Pin = E220_900T30D_TX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(E220_900T30D_TX_GPIO_Port, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = E220_900T30D_RX_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(E220_900T30D_RX_GPIO_Port, &GPIO_InitStruct);

  /* USART2 DMA Init */

  /* USART2_RX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_4, LL_DMAMUX_REQ_USART2_RX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_4, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MODE_CIRCULAR);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_4, LL_DMA_MDATAALIGN_BYTE);

  /* USART2_TX Init */
  LL_DMA_SetPeriphRequest(DMA1, LL_DMA_CHANNEL_5, LL_DMAMUX_REQ_USART2_TX);

  LL_DMA_SetDataTransferDirection(DMA1, LL_DMA_CHANNEL_5, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);

  LL_DMA_SetChannelPriorityLevel(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PRIORITY_LOW);

  LL_DMA_SetMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MODE_NORMAL);

  LL_DMA_SetPeriphIncMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PERIPH_NOINCREMENT);

  LL_DMA_SetMemoryIncMode(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MEMORY_INCREMENT);

  LL_DMA_SetPeriphSize(DMA1, LL_DMA_CHANNEL_5, LL_DMA_PDATAALIGN_BYTE);

  LL_DMA_SetMemorySize(DMA1, LL_DMA_CHANNEL_5, LL_DMA_MDATAALIGN_BYTE);

  /* USART2 interrupt Init */
  NVIC_SetPriority(USART2_IRQn, 0);
  NVIC_EnableIRQ(USART2_IRQn);

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
  USART_InitStruct.BaudRate = 9600;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);

  /* USER CODE BEGIN WKUPType USART2 */

  /* USER CODE END WKUPType USART2 */

  LL_USART_Enable(USART2);

  /* Polling USART2 initialisation */
  while((!(LL_USART_IsActiveFlag_TEACK(USART2))) || (!(LL_USART_IsActiveFlag_REACK(USART2))))
  {
  }

	// USART + DMA. Transmit
	LL_USART_EnableDMAReq_TX(USART2);
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_5, (uint32_t)E220_UART2_.TransmitBuffer, (uint32_t)(&(USART2->TDR)), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_5);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_5);
	LL_USART_ClearFlag_TC(USART2);	// check it before clearing
	LL_USART_EnableIT_TC(USART2);

	// USART + DMA. Receive
	LL_USART_EnableDMAReq_RX(USART2);
	LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_4, (uint32_t)(&(USART2->RDR)), (uint32_t)E220_UART2_.ReceiveBuffer, LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
	LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_4);
	//LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_4);
	LL_DMA_EnableIT_TE(DMA1, LL_DMA_CHANNEL_4);
	LL_USART_EnableIT_ERROR(USART2);
	LL_USART_EnableIT_IDLE(USART2);

}


static void __attribute__((optimize("O0"))) E220_UART2_DMA_Handler(void){

		if ( LL_DMA_IsActiveFlag_TC4(DMA1) ) {					// RECEIVING

			LL_DMA_ClearFlag_TC4(DMA1);
			E220_UART2_.IdleLineDetected.dmaTC++;

		} else if ( LL_DMA_IsActiveFlag_TE4(DMA1) ){

			LL_DMA_ClearFlag_TE4(DMA1);
			E220_UART2_.USART_or_DMA_error.DMA_TransferError_uartRX = 1;
		}


		if ( LL_DMA_IsActiveFlag_TC5(DMA1) ) {

			LL_DMA_ClearFlag_TC5(DMA1);						   // TRANSMISSION
			LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_5);

		} else if ( LL_DMA_IsActiveFlag_TE5(DMA1) ){

			LL_DMA_ClearFlag_TE5(DMA1);
			E220_UART2_.USART_or_DMA_error.DMA_TransferError_uartTX = 1;
		}

}

static void E220_UART2_IRQHandler(void){

	//debugUART.usart2++;

		// T R A N S M I S S I O N
		if ( LL_USART_IsActiveFlag_TC(USART2)){

			LL_USART_ClearFlag_TC(USART2);
			E220_UART2_.TransmitCurState = TX_COMPLETE;
		}

		// R E C E P T I O N
		if ( LL_USART_IsActiveFlag_IDLE(USART2) ) {

			LL_USART_ClearFlag_IDLE(USART2);
			//IdleLineDetected.idle++;			// = 1;

			//*******************************************************
			E220_UART2_.IdleLineDetected.currPosition = ((DMA_Channel_TypeDef *)(__LL_DMA_GET_CHANNEL_INSTANCE(DMA1, LL_DMA_CHANNEL_4)))->CNDTR;

					if ( E220_UART2_.IdleLineDetected.lastPosition > E220_UART2_.IdleLineDetected.currPosition ) {

						if ( E220_UART2_.IdleLineDetected.dmaTC == 0 ){
							E220_UART2_.IdleLineDetected.End = E220_UART2_.IdleLineDetected.lastPosition - E220_UART2_.IdleLineDetected.currPosition;
							E220_UART2_.IdleLineDetected.Start = 0;
						} else {	// We have buffer overrun/overflow
							E220_UART2_.IdleLineDetected.End = 0;
							E220_UART2_.IdleLineDetected.Start = 0;
						}

					} else {	// if ( IdleLineDetected.lastPosition <= IdleLineDetected.currPosition )

						if ( E220_UART2_.IdleLineDetected.dmaTC == 1 ){
							E220_UART2_.IdleLineDetected.End = E220_UART2_.IdleLineDetected.lastPosition;
							E220_UART2_.IdleLineDetected.Start = E220_UART2_RECEIVE_RING_BUFFER_LEN - E220_UART2_.IdleLineDetected.currPosition;
						} else {
							// if ( IdleLineDetected.dmaTC == 0 ) { it is imposible }
							// if ( IdleLineDetected.dmaTC > 1  ) { overflow        }
							E220_UART2_.IdleLineDetected.End = 0;
							E220_UART2_.IdleLineDetected.Start = 0;
						}

					}


			//the Length of received bytes =  IdleLineDetected.End + IdleLineDetected.Start
			// if the number of received bytes is more than	E220_UART2_RECEIVE_RING_BUFFER_LEN then
			// 		IdleLineDetected.End = 0 and IdleLineDetected.Start = 0 in that algorithm
			// It does not need to know the exactly number of received bytes if
			//   that quantity is more than E220_UART2_RECEIVE_RING_BUFFER_LEN. The main point is to define that "bigData"
			//   to avoid ALL THAT RECEPRION.

			if ( E220_UART2_.IdleLineDetected.End + E220_UART2_.IdleLineDetected.Start > 0 ){
				E220_UART2_.IdleLineDetected.received = 1;


				E220_UART2_.IdleLineDetected.pauseAfterReceiving = E220_UART2_PAUSE_AFTER_RECEIVING;

			} else {		// for example, I received too many of bytes ( erasing occures)

				E220_UART2_.IdleLineDetected.dmaTC = 0;
				E220_UART2_.IdleLineDetected.lastPosition = E220_UART2_.IdleLineDetected.currPosition;
				if ( E220_UART2_.IdleLineDetected.received )
					E220_UART2_.IdleLineDetected.received = 0;
			}




			//*******************************************************

		} else if ( LL_USART_IsActiveFlag_FE(USART2) ){

			LL_USART_ClearFlag_FE(USART2);
			E220_UART2_.USART_or_DMA_error.USART_Error_RX++;			// = 1;

		} else if ( LL_USART_IsActiveFlag_NE(USART2) ){

			LL_USART_ClearFlag_NE(USART2);
			E220_UART2_.USART_or_DMA_error.USART_Error_RX++;			// = 1;

		} else if ( LL_USART_IsActiveFlag_ORE(USART2) ){

			LL_USART_ClearFlag_ORE(USART2);
			E220_UART2_.USART_or_DMA_error.USART_Error_RX++;			// = 1;

		}


}

// temporary
static void E220_UART2_Receive (void){

	  //static union paramLL CRC_VAL;				// debug
	  //uint8_t ii;								// debug
	  //uint8_t bug = 0;							// debug

	static uint32_t offset;
	static uint16_t length;
	static struct modem_cmd_result res;

		// handling messages: moving to linear array and parsing.
		if ( E220_UART2_.IdleLineDetected.received && !E220_UART2_.IdleLineDetected.pauseAfterReceiving ){
			E220_UART2_.IdleLineDetected.received = 0;

			// begin * * * * copy all received data from cyclic format into linear format

			if ( E220_UART2_.IdleLineDetected.End > 0 ){		// End is alwase more than 0.   delete this if
				if ( E220_UART2_.IdleLineDetected.lastPosition > E220_UART2_.IdleLineDetected.currPosition )
					offset = E220_UART2_RECEIVE_RING_BUFFER_LEN-E220_UART2_.IdleLineDetected.currPosition-E220_UART2_.IdleLineDetected.End;
				else{
					offset = E220_UART2_RECEIVE_RING_BUFFER_LEN-E220_UART2_.IdleLineDetected.End;
				}
					 //memcpy(E220_UART2_.Linear_Buffer, E220_UART2_.ReceiveBuffer + offset, IdleLineDetected.End);
					my_memcpy(E220_UART2_.Linear_Buffer, E220_UART2_.ReceiveBuffer + offset, E220_UART2_.IdleLineDetected.End);
			}

			if ( E220_UART2_.IdleLineDetected.Start > 0 ){
				 //memcpy(E220_UART2_.Linear_Buffer+E220_UART2_.IdleLineDetected.End, E220_UART2_.ReceiveBuffer, E220_UART2_.IdleLineDetected.Start);
				my_memcpy(E220_UART2_.Linear_Buffer+E220_UART2_.IdleLineDetected.End, E220_UART2_.ReceiveBuffer, E220_UART2_.IdleLineDetected.Start);
			}

		E220_UART2_.IdleLineDetected.dmaTC = 0;																			// was coped from USART2_IRQHandler()
		E220_UART2_.IdleLineDetected.lastPosition = E220_UART2_.IdleLineDetected.currPosition;	// was coped from USART2_IRQHandler()

		// end * * * * copy all received data from cyclic format into linear format



			length = E220_UART2_.IdleLineDetected.End + E220_UART2_.IdleLineDetected.Start;	// length of the current message
			E220_UART2_.Linear_Buffer[length] = 0;	// string termination. in ALL modes


			// * * *   P A R S I N G   * * *

				//  this maybe either the Command from PC for this NODE or the data for MODEM.
			//   in both cases I must check CRC.
			// add parseData( uint8_t nReceived) function like in MasterDevice


			if ( length > 6 ){


				if ( E220_UART2_.CheckData(length) ) {
					E220_UART2_.parseDataResult = E220_UART2_.parseData(length);
				} else {
					E220_UART2_.parseDataResult = __CMD_BADCRC;
				}

				switch( E220_UART2_.parseDataResult ){

				case __CMD_RETRANSMIT:


					// Glueing all messages ( in QueueManager module )

					if ( queueManager.add(E220_UART2_.Linear_Buffer, length, QUEUE_ANSWER_NOT_REQUIRED ) == Q_result_ER ){
						queueManager.queue_overflows++;
					} else {
						// I will confirm only in positive case ( when there is no queue overflow )
						queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_NODE1;
						E220_UART2_.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;
					}

					break;

				case __CMD_CONFIRMATION:

					if ( queueManager.fromNode1.waiting ){
						queueManager.remove(queueManager.fromNode1.last_index);
						queueManager.fromNode1.waiting = 0;
						E220_UART2_.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;
					}

#ifndef _DEBUG_SWD_EN
					if ( E220_UART2_.ping_answer_time_to_buzzer_on ){	// I see value 1405
						Current_distance_number_milliseconds = PING_ANSWER_TIME_TO_BUZZER_ON - E220_UART2_.ping_answer_time_to_buzzer_on;
						E220_UART2_.ping_answer_time_to_buzzer_on = 0;

						buzzer.addNoise(BUZZER_NOISE_SINGLE_170_100);
						buzzer.addNoise(BUZZER_NOISE_SINGLE_170_100);
					}
#endif

					break;

				case __CMD_BADCRC:
					// I am here when CRC is bad. Other Nodes could not send a message with __CMD_BADCRC code.

					stat_info.Events_E220[STAT_EVENTS_E220_BADCRC_INDEX]++;

					break;

				case __CMD_ADC_GET_DATA:


					//third param is 0, but I add sensors data within E220_900T30D_ANSW_ADC_GET_DATA_Handler()
					e220_900t30d_Answer_Fill(&VarialbeAnswer, &(e220_900t30d_Answers[E220_900T30D_ANSW_ADC_GET_DATA_INDEX]), (struct modem_cmd_result*)0);//

					if ( queueManager.add(VarialbeAnswer.answer, VarialbeAnswer.length, QUEUE_ANSWER_REQUIRED ) == Q_result_ER ){
						queueManager.queue_overflows++;
					} else {
						queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_NODE1;
						E220_UART2_.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;
					}

					break;

				case __CMD_PHY_TURN_MODEM_ON:


					//switches if modem_.pause_between_switches is equal to 0.
					//if not, no answer will be sent.
					res = modem_.phy_SwitchPower(1);

					e220_900t30d_Answer_Fill(&VarialbeAnswer, &(e220_900t30d_Answers[E220_900T30D_ANSW_MODEM_POWER_SWITCHING_INDEX]), &res);	//

					if ( queueManager.add(VarialbeAnswer.answer, VarialbeAnswer.length, QUEUE_ANSWER_REQUIRED ) == Q_result_ER ){
						queueManager.queue_overflows++;
					} else {
						queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_NODE1;
						E220_UART2_.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;
					}

					break;

				case __CMD_PHY_TURN_MODEM_OFF:


					//switches if modem_.pause_between_switches is equal to 0.
					//if not, no answer will be sent.
					res = modem_.phy_SwitchPower(0);

					e220_900t30d_Answer_Fill(&VarialbeAnswer, &(e220_900t30d_Answers[E220_900T30D_ANSW_MODEM_POWER_SWITCHING_INDEX]), &res);	//

					if ( queueManager.add(VarialbeAnswer.answer, VarialbeAnswer.length, QUEUE_ANSWER_REQUIRED ) == Q_result_ER ){
						queueManager.queue_overflows++;
					} else {
						queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_NODE1;
						E220_UART2_.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;
					}

					break;

				case __CMD_MODEM_POWER_SWITCH:

					queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_NODE1;
					E220_UART2_.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;

					modem_.Press_PWRKEY();		// add the "how much time I must press the PWRKEY" parameter. ?

					// no deals needed
					// further Modem changes the Status pin or/and  Modem sends "RDY" message through UART.
					//  ( "AT" test command can be sent also to check modem's Status )

					break;

				case __CMD_GET_STATISTICS:

					queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_NODE1;
					E220_UART2_.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;

					e220_900t30d_Answer_Fill(&VarialbeAnswer, &(e220_900t30d_Answers[E220_900T30D_ANSW_GET_STATISTICS_INDEX]), (struct modem_cmd_result*)0);

					if ( queueManager.add(VarialbeAnswer.answer, VarialbeAnswer.length, QUEUE_ANSWER_REQUIRED ) == Q_result_ER ){
						queueManager.queue_overflows++;
					}

					break;

				case __CMD_PING:


					e220_900t30d_Answer_Fill(&VarialbeAnswer, &(e220_900t30d_Answers[E220_900T30D_ANSW_PING_ANSWER_INDEX]), (struct modem_cmd_result*)0);			//

					if ( queueManager.add(VarialbeAnswer.answer, VarialbeAnswer.length, QUEUE_ANSWER_REQUIRED ) == Q_result_ER ){
						queueManager.queue_overflows++;
					} else {
						queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_NODE1;
						E220_UART2_.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;
					}

					break;

				case __CMD_UNKNOWN:

					stat_info.Events_E220[STAT_EVENTS_E220_UNKNOWN_INDEX]++;

					break;

				}


			} else {

				stat_info.Events_E220[STAT_EVENTS_E220_BADSIZE_INDEX]++;


			}





		}

}

//extern struct debug01 debug_01;

static void E220_UART2_SysTick(void){

	if (E220_UART2_.IdleLineDetected.pauseAfterReceiving )
		E220_UART2_.IdleLineDetected.pauseAfterReceiving--;


	//if ( E220_UART2_.TransmitCurState == TX_COMPLETE && E220_UART2_.pauseAfterTransmit ){
	if ( E220_UART2_.pauseAfterTransmit ){
		E220_UART2_.pauseAfterTransmit--;
	}

#ifndef _DEBUG_SWD_EN
	if ( E220_UART2_.ping_answer_time_to_buzzer_on ){
		E220_UART2_.ping_answer_time_to_buzzer_on--;
		if ( E220_UART2_.ping_answer_time_to_buzzer_on == 0 )
			buzzer.addNoise(BUZZER_NOISE_SINGLE_300_100);	// bad
	}
#endif

}


static uint8_t CheckData( uint8_t nReceived ){

	static union paramLL CRC_VAL;
	static uint16_t yy;
	static uint16_t ii;

	uint8_t res = 0;

	yy = nReceived - CRC_LEN;
	CalculateCRC_32bit(E220_UART2_.Linear_Buffer, yy, &CRC_VAL);

	ii = 0;
	for ( ii = 0 ; ii < CRC_LEN ; ii++ ){
		if ( !(E220_UART2_.Linear_Buffer[yy+ii] == CRC_VAL.par_bytes[ii] ) )
			break;
	}

	if ( ii == 4 ){
		res = 1;
	}

	return res;

}

static ___COMMAND parseData( uint8_t nReceived){


	___COMMAND res = (___COMMAND)__CMD_UNKNOWN;

	if( GET_TARGET_ADDRESS(E220_UART2_.Linear_Buffer[ADDRESS_INDEX]) != THIS_DEVICE_ADDRESS ){

		res = (___COMMAND)__CMD_RETRANSMIT;
	} else {

		nReceived -= CRC_LEN;

		for ( uint8_t ii = 0; ii < COMMANDS_LEN; ii++ ){
			if ( E220_UART2_.Linear_Buffer[CMD_INDEX] == Commands_[ii] ){

				res = Commands_[ii];
				if ( (uint32_t)Commands_Handlers[ii] != (uint32_t)0 ){
					Commands_Handlers[ii](E220_UART2_.Linear_Buffer, nReceived );
				}
				break;
			}
		}


	}


	return res;
}


// private

////send message
void PING_button_down_handler(void){

	e220_900t30d_Answer_Fill(&VarialbeAnswer, &(e220_900t30d_Answers[E220_900T30D_ANSW_PING_INDEX]), (struct modem_cmd_result*)0 );	//

	if ( queueManager.add(VarialbeAnswer.answer, VarialbeAnswer.length, QUEUE_ANSWER_NOT_REQUIRED ) == Q_result_ER ){
		queueManager.queue_overflows++;
#ifndef _DEBUG_SWD_EN
		buzzer.addNoise(BUZZER_NOISE_SINGLE_300_100);	// bad
#endif
	} else {
#ifndef _DEBUG_SWD_EN
		buzzer.addNoise(BUZZER_NOISE_SINGLE_170_100);	// good
		E220_UART2_.ping_answer_time_to_buzzer_on =  PING_ANSWER_TIME_TO_BUZZER_ON;
#endif
	}



}

