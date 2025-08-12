#include "e220_900t30d.h"
#include "my_uart.h"
//#include <string.h>
#include "my_lib_.h"

#include "buzzer.h"
#include "crc_.h"
#include "queue_m.h"
#include "commands.h"
#include "answers_e220_900t30d.h"
#include "statistics.h"

extern struct MY_UART my_uart;
extern uint8_t volatile ReceiveBuffer[];
extern struct QUEUE_MANAGER queueManager;
extern const struct answer_  e220_900t30d_Answers[];
extern const ___COMMAND Commands_[];
extern const void (*Commands_Handlers[]) ( uint8_t * , uint8_t );
//extern struct Buzzers buzzer;				// delete
extern struct __VarialbeAnswer VarialbeAnswer;
extern struct STATICTICS_i stat_info;


// Linear buffer of bytes received
// It is used to parse
static volatile uint8_t e220_900t30d_Buffer[RECEIVE_RING_BUFFER_LEN+1];


static void Init (void);
static void Transmit ( const char * message, uint32_t len, enum TransmitType transType, uint16_t pause );
static void Receive (void);

static uint8_t CheckData( uint8_t nReceived );
static ___COMMAND parseData( uint8_t nReceived);

struct E220_900T30D e220_900t30d = {

	.bufferToParse = (uint8_t *)e220_900t30d_Buffer,
	.Init = &Init,
	.Transmit = &Transmit,
	.Receive = &Receive,

	.CheckData = &CheckData,
	.parseData = &parseData,

};

static void Init (void){

	MY_UART_Receive(RECEIVE_RING_BUFFER_LEN);

}

// to E220_900T30D
//
static void Transmit ( const char * message, uint32_t len, enum TransmitType transType, uint16_t pause ){

	uint32_t totalLen = len;
	union paramLL CRC_VAL;


	// is ADDRESS set into the first byte?


	// put the data into UART buffer
	MY_UART_SetTransmissionBuffer( message, len );
	// add the CRC into UART buffer
	if ( transType == TR_TYPE_NORMAL ){
		CalculateCRC_32bit( (uint8_t *)message, len, &CRC_VAL);
		MY_UART_AddToTransmissionBuffer( (char*)CRC_VAL.par_bytes, CRC_LEN, len );
		totalLen += CRC_LEN;
	}

	MY_UART_Transmit(totalLen, pause);										// test

}

// FROM E220_900T30D TO UART
//
static __attribute__((optimize("O0"))) void Receive (void){

  //static union paramLL CRC_VAL;
  //uint8_t ii;

  static uint32_t offset;
  static uint16_t length;

//  uint8_t bug = 0;

// handling messages: moving to linear array and parsing.
	if ( my_uart.IdleLineDetected.received && !my_uart.IdleLineDetected.pauseAfterReceiving ){
		my_uart.IdleLineDetected.received = 0;

		// begin * * * * copy all received data from cyclic format into linear format

		if ( my_uart.IdleLineDetected.End > 0 ){		// End is always more than 0.   delete this if
			if ( my_uart.IdleLineDetected.lastPosition > my_uart.IdleLineDetected.currPosition )
				offset = RECEIVE_RING_BUFFER_LEN-my_uart.IdleLineDetected.currPosition-my_uart.IdleLineDetected.End;
			else{
				offset = RECEIVE_RING_BUFFER_LEN-my_uart.IdleLineDetected.End;
			}
			//memcpy(e220_900t30d.bufferToParse, ReceiveBuffer + offset, my_uart.IdleLineDetected.End);
		   my_memcpy(e220_900t30d.bufferToParse, (uint8_t * )(ReceiveBuffer + offset), my_uart.IdleLineDetected.End);
		}

		if ( my_uart.IdleLineDetected.Start > 0 ){
			//memcpy(e220_900t30d.bufferToParse+my_uart.IdleLineDetected.End, ReceiveBuffer, my_uart.IdleLineDetected.Start);
		 my_memcpy(e220_900t30d.bufferToParse+my_uart.IdleLineDetected.End, (uint8_t * )ReceiveBuffer, my_uart.IdleLineDetected.Start);
		}

		my_uart.IdleLineDetected.dmaTC = 0;																			// was coped from USART2_IRQHandler()
		my_uart.IdleLineDetected.lastPosition = my_uart.IdleLineDetected.currPosition;	// was coped from USART2_IRQHandler()

	// end * * * * copy all received data from cyclic format into linear format

	// Parse.

		length = my_uart.IdleLineDetected.End + my_uart.IdleLineDetected.Start;	// length of the current message
		e220_900t30d.bufferToParse[length] = 0;	// string termination.


		//  Message is prepared for the parsing. Parse it.

		// * * *   P A R S I N G   * * *

		//buzzer.addNoise(BUZZER_NOISE_SINGLE_170_100);


		if ( length > 6 ){

			if ( e220_900t30d.CheckData(length) ) {
				e220_900t30d.parseDataResult = e220_900t30d.parseData(length);
			} else {
				e220_900t30d.parseDataResult = __CMD_BADCRC;
			}

			switch( e220_900t30d.parseDataResult ){

			case __CMD_RETRANSMIT:

				// QUEUE_ANSWER_REQUIRED means This node will wait the answer(confirmation) from Node0 (PC)
				if ( queueManager.add(e220_900t30d.bufferToParse, length, QUEUE_ANSWER_REQUIRED ) == Q_result_ER ){
					queueManager.queue_overflows++;
				} else {
					// I will confirm only in positive case ( when there is no queue overflow )
					queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_NODE2;
					my_uart.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;	// reset
				}

				break;

			case __CMD_CONFIRMATION:

				if ( queueManager.fromNode2.waiting ){
					queueManager.remove(queueManager.fromNode2.last_index);
					queueManager.fromNode2.waiting = 0;
					my_uart.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;	// reset
				}

				break;

			case __CMD_PING:

				queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_NODE2;
				my_uart.pauseAfterTransmit = PAUSE_AFTER_TRANSMIT_SKIP;	// reset

				break;
			case __CMD_BADCRC:

				stat_info.Events_E220[STAT_EVENTS_E220_BADCRC_INDEX]++;

				break;

			case __CMD_UNKNOWN:

				stat_info.Events_E220[STAT_EVENTS_E220_UNKNOWN_INDEX]++;

				break;

			}


		} else {

			stat_info.Events_E220[STAT_EVENTS_E220_BADSIZE_INDEX]++;

		}







		}

		// * * *   P A R S I N G   * * *

}

static uint8_t CheckData( uint8_t nReceived ){
		static union paramLL CRC_VAL;
		static uint16_t yy;
		static uint16_t ii;

		uint8_t res = 0;

		yy = nReceived - CRC_LEN;
		CalculateCRC_32bit(e220_900t30d.bufferToParse, yy, &CRC_VAL);

		ii = 0;
		for ( ii = 0 ; ii < CRC_LEN ; ii++ ){
			if ( !(e220_900t30d.bufferToParse[yy+ii] == CRC_VAL.par_bytes[ii] ) )
				break;
		}

		if ( ii == 4 ){
			res = 1;
		}

		return res;
}


static ___COMMAND parseData( uint8_t nReceived){

	___COMMAND res = (___COMMAND)__CMD_UNKNOWN;

		if( GET_TARGET_ADDRESS(e220_900t30d.bufferToParse[ADDRESS_INDEX]) != THIS_NODE_ADDRESS ){

			res = (___COMMAND)__CMD_RETRANSMIT;
		} else {

			nReceived -= CRC_LEN;

			for ( uint8_t ii = 0; ii < COMMANDS_LEN; ii++ ){
				if ( e220_900t30d.bufferToParse[CMD_INDEX] == Commands_[ii] ){

					res = Commands_[ii];
					if ( (uint32_t)Commands_Handlers[ii] != (uint32_t)0 ){
						Commands_Handlers[ii](e220_900t30d.bufferToParse, nReceived );
					}
					break;
				}
			}


		}


		return res;

}

