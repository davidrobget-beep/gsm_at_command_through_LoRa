#include "queue_m.h"
#include "my_lib_.h"
#include "modem_uart1.h"
#include "answers_e220_900t30d.h"
#include "big_messages.h"
#include "modem_state.h"

extern struct UART_INSTANCE MODEM_UART1_;
extern struct UART_INSTANCE E220_UART2_;
extern const struct answer_  e220_900t30d_Answers[];
extern struct Big_Message_Process assembling_big_message;


static void Q_task(void);
static enum Q_result Q_add( const uint8_t * data, uint16_t length, uint8_t is_it_Request );
static void Q_remove( uint8_t index );


static struct Q_Node queue[Q_QUEUE_LEN];

struct __VarialbeAnswer VarialbeAnswer;

struct QUEUE_MANAGER queueManager = {

	//.ImustSendConfirmation = 0,

	.queue = queue,
	.available = Q_QUEUE_LEN,

	//.fromHost  = {.waiting = 0,},
	//.fromNode2 = {.waiting = 0,},

	.task = &Q_task,				// place to main loop
	.add = &Q_add,
	.remove = &Q_remove,

};





//struct debug01 debug_01;


static void Q_task(void){

	static uint8_t targetPri;
	uint8_t ii;
	static uint8_t Total_packet_number;
	static uint8_t Current_packet_Number;

	if ( queueManager.available == Q_QUEUE_LEN && !queueManager.ImustSendConfirmation )
		return;

	if ( ( queueManager.toHostDirectory || (queueManager.ImustSendConfirmation & CONFIRMATION_FLAG_TO_NODE1) ) && ( !E220_UART2_.pauseAfterTransmit )  ){
		// FIX



		if ( queueManager.ImustSendConfirmation & CONFIRMATION_FLAG_TO_NODE1 ){



			e220_900t30d_Answer_Fill( &VarialbeAnswer, &(e220_900t30d_Answers[E220_900T30D_ANSW_GOTIT_INDEX]), (struct modem_cmd_result*)0 );
			E220_UART2_.SetTransmissionBuffer( (char *)VarialbeAnswer.answer, VarialbeAnswer.length );
			E220_UART2_.Transmit(VarialbeAnswer.length, PAUSE_AFTER_TRANSMIT_MINIMUM);

			queueManager.ImustSendConfirmation &= !CONFIRMATION_FLAG_TO_NODE1;


		} else {

			if ( queueManager.high_pri ){
				targetPri = 1;
			} else {
				targetPri = 0;
			}

			for ( ii = 0; ii < Q_QUEUE_LEN; ii++ ){
				if ( queueManager.queue[ii].len && (queueManager.queue[ii].pri == targetPri) ){

					switch ( GET_TARGET_ADDRESS(queueManager.queue[ii].array[ADDRESS_INDEX]) ) {

					case 0:			// host, PC; Node1	<<<==== thisDevice
					case 1:

						E220_UART2_.SetTransmissionBuffer( (char *)queueManager.queue[ii].array, queueManager.queue[ii].len );

						if ( !(queueManager.queue[ii].is_it_Request) ){
							E220_UART2_.Transmit(queueManager.queue[ii].len, PAUSE_AFTER_TRANSMIT_MINIMUM);
							queueManager.remove(ii);
						} else {
							E220_UART2_.Transmit(queueManager.queue[ii].len, PAUSE_AFTER_TRANSMIT);
							queueManager.fromNode1.last_index = ii;
							queueManager.fromNode1.waiting    = 1;
						}
						return;

					  break;

					//case 2:			this node is with address 2.

					case 3:			//      				// thisDevice ====>>> address 3

						continue;

					  break;

					}

				}
			}
		}
	} else if ( queueManager.toModemDirectory && !MODEM_UART1_.pauseAfterTransmit ){



		for ( ii = 0; ii < Q_QUEUE_LEN; ii++ ){
			if ( queueManager.queue[ii].len && (queueManager.queue[ii].pri == 0) ){

				switch ( GET_TARGET_ADDRESS(queueManager.queue[ii].array[ADDRESS_INDEX]) ) {

				case 0:			// host, PC; Node1	<<<==== thisDevice
				case 1:

					continue;

				  break;

				//case 2:			this node is with address 2.

				case 3:			//      				// thisDevice ====>>> address 3


					Current_packet_Number = queueManager.queue[ii].array[MESSAGE_INDEX_INDEX];		// temp   Current_packet_Number + Total_packet_number

					if ( Current_packet_Number == 0 ){	// single message
						uint8_t len_1;
						len_1 = queueManager.queue[ii].len - TECH_INFO;
						MODEM_UART1_.SetTransmissionBuffer( (char *)(queueManager.queue[ii].array + FIRST_BYTE_INDEX), len_1 );
						MODEM_UART1_.Transmit( len_1, 0);
						queueManager.remove(ii);

						return;
					} else {

						Total_packet_number   =  0x0F & Current_packet_Number;
						Current_packet_Number = (0xF0 & Current_packet_Number) >> 4;		// index

						if ( !assembling_big_message.Started ){

							assembling_big_message.flags = 0;
							assembling_big_message.length_toModem = 0;

							for ( uint8_t yy = 1; yy <= Total_packet_number; yy++ ){

								assembling_big_message.set_flag(yy);
							}

							assembling_big_message.Started = 1;

						}

						assembling_big_message.addPacket( queueManager.queue[ii].array, queueManager.queue[ii].len, Current_packet_Number );
						queueManager.remove(ii);

						assembling_big_message.timeoutToReset = TIMEOUT_TO_RESET_VALUE;
						assembling_big_message.reset_flag(Current_packet_Number);


						if ( assembling_big_message.flags == 0 ){	// all flags were reset/(all messages were added to buf_to_modem)
							MODEM_UART1_.SetTransmissionBuffer( (char *)assembling_big_message.buffer_toModem, assembling_big_message.length_toModem );
							MODEM_UART1_.Transmit(assembling_big_message.length_toModem, 0);
							assembling_big_message.Started = 0;
							assembling_big_message.Stop_timer = 1;
						}


						return;
					}

				  break;

				}

			}
		}

	}

}

// is_it_Request:   0 - when I send the next packet from queue I remove it right now ( foget it )
//					1 - when I send the packet, then I wait for Confirmation, then I remove.
//
static enum Q_result Q_add( const uint8_t * data, uint16_t length, uint8_t is_it_Request ){

	enum Q_result res = Q_result_OK;
	uint8_t ii;

	if ( queueManager.available ){

		for ( ii = 0; ii < Q_QUEUE_LEN; ii++ ){
			if ( queueManager.queue[ii].len == 0 ){			// means that Node is empty
				queueManager.queue[ii].len = length;
				my_memcpy(queueManager.queue[ii].array, data, length);
				if ( GET_SOURCE_ADDRESS(data[ADDRESS_INDEX]) == MODEM_ADDRESS ){
					queueManager.queue[ii].pri = 1;
					queueManager.high_pri++;
				} else {
					queueManager.queue[ii].pri = 0;
					queueManager.low_pri++;
				}

				if ( !(GET_TARGET_ADDRESS(data[ADDRESS_INDEX]) == MODEM_ADDRESS) ){//==HOST_ADDRESS
					queueManager.queue[ii].toHost_toModem = 0;
					queueManager.toHostDirectory++;
				} else{
					queueManager.queue[ii].toHost_toModem = 1;
					queueManager.toModemDirectory++;
				}
				queueManager.available--;

				queueManager.queue[ii].is_it_Request = is_it_Request;

				break;
			}
		}


	} else {
		res = Q_result_ER;
	}

	return res;
}

static void Q_remove( uint8_t index ){

	if ( queueManager.available == Q_QUEUE_LEN || index >= Q_QUEUE_LEN ){
		return;
	}


	if ( queueManager.queue[index].pri ){
		queueManager.high_pri--;
	} else {
		queueManager.low_pri--;
	}

	if ( queueManager.queue[index].toHost_toModem == 0 ){	// host
		queueManager.toHostDirectory--;
	} else{
		queueManager.toModemDirectory--;
	}

	queueManager.queue[index].len = 0;
	queueManager.available++;

}


//*********************************
//


