#include "queue_m.h"
#include "my_lib_.h"
#include "my_uart.h"
#include "e220_900t30d.h"
#include "my_usb.h"
#include "usbd_cdc_if.h"
#include "answers_usb.h"
#include "answers_e220_900t30d.h"

extern struct MY_UART my_uart;
extern struct E220_900T30D e220_900t30d;
extern struct MY_USB_ myUsb_;
extern const struct answer_ usb_Answers[];
extern const struct answer_  e220_900t30d_Answers[];

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


static void Q_task(void){

	static uint8_t targetPri;
	uint8_t ii;

	if ( queueManager.available == Q_QUEUE_LEN && !queueManager.ImustSendConfirmation )
		return;

	if ( ( queueManager.toHostDirectory || ( queueManager.ImustSendConfirmation & CONFIRMATION_FLAG_TO_HOST )) && !myUsb_.delayTransmit ){
		// FIX
		if ( queueManager.ImustSendConfirmation & CONFIRMATION_FLAG_TO_HOST ){


			USB_Answer_Fill( &VarialbeAnswer, &(usb_Answers[USB_ANSW_GOTIT_INDEX]), (struct cmd_result *)0 );	// CONFIRMATION
			CDC_Transmit_FS( VarialbeAnswer.answer, VarialbeAnswer.length );

			queueManager.ImustSendConfirmation &= !CONFIRMATION_FLAG_TO_HOST;

			// ? set time to Transmit next message



		} else {

			// high_pri is important only when transmitting to Host directory ( from Modem )
			if ( queueManager.high_pri ){
				targetPri = 1;
			} else {
				targetPri = 0;
			}

			for ( ii = 0; ii < Q_QUEUE_LEN; ii++ ){
				if ( queueManager.queue[ii].len && (queueManager.queue[ii].pri == targetPri) ){

					switch ( GET_TARGET_ADDRESS(queueManager.queue[ii].array[ADDRESS_INDEX]) ) {

					case 0:			// host, PC	<<<==== thisDevice

						CDC_Transmit_FS((uint8_t*)queueManager.queue[ii].array, queueManager.queue[ii].len);
						if ( !(queueManager.queue[ii].is_it_Request) )
							queueManager.remove(ii);
						else{
							queueManager.fromHost.last_index = ii;
							queueManager.fromHost.waiting    = 1;
						}
						return;

						break;

					//case 1:    this node is with address 1.

					case 2:			//						// ====>>>
					case 3:			//      				// thisDevice ====>>> address 2,3

						continue;

						break;

					}

				}
			}
		}
	}

	else if ( (queueManager.toModemDirectory || ( queueManager.ImustSendConfirmation & CONFIRMATION_FLAG_TO_NODE2 )) && !my_uart.pauseAfterTransmit ) {

		if ( queueManager.ImustSendConfirmation & CONFIRMATION_FLAG_TO_NODE2 ){

			e220_900t30d_Answer_Fill( &VarialbeAnswer, &(e220_900t30d_Answers[E220_900T30D_ANSW_GOTIT_INDEX]), (struct cmd_result *)0 );
			e220_900t30d.Transmit((char*)VarialbeAnswer.answer, VarialbeAnswer.length, TR_TYPE_TRANSPARENT, PAUSE_AFTER_TRANSMIT_MINIMUM);

			queueManager.ImustSendConfirmation &= !CONFIRMATION_FLAG_TO_NODE2;

			// ? set time to Transmit next message

		} else {

			for ( ii = 0; ii < Q_QUEUE_LEN; ii++ ){
				if ( queueManager.queue[ii].len && (queueManager.queue[ii].pri == 0) ){

					switch ( GET_TARGET_ADDRESS(queueManager.queue[ii].array[ADDRESS_INDEX]) ) {

					case 0:			// host, PC	<<<==== thisDevice

						continue;

						break;

					//case 1:    this node is with address 1.

					case 2:			//						// ====>>>
					case 3:			//      				// thisDevice ====>>> address 2,3

						if ( !(queueManager.queue[ii].is_it_Request) ){
							e220_900t30d.Transmit((char*)queueManager.queue[ii].array, queueManager.queue[ii].len, TR_TYPE_TRANSPARENT, PAUSE_AFTER_TRANSMIT_MINIMUM);
							queueManager.remove(ii);
						} else {
							e220_900t30d.Transmit((char*)queueManager.queue[ii].array, queueManager.queue[ii].len, TR_TYPE_TRANSPARENT, PAUSE_AFTER_TRANSMIT);
							queueManager.fromNode2.last_index = ii;
							queueManager.fromNode2.waiting    = 1;
						}
						return;

						break;

					}

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

				if ( GET_TARGET_ADDRESS(data[ADDRESS_INDEX]) == HOST_ADDRESS ){
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



