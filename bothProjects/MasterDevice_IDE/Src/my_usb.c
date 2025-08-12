#include <answers_usb.h>
#include <my_usb.h>
#include <usbd_cdc_if.h>
#include <my_lib_.h>
#include "e220_900t30d.h"
#include "crc_.h"
#include "queue_m.h"
#include "commands.h"
#include "statistics.h"


extern const struct answer_ usb_Answers[];
extern const ___COMMAND Commands_[];
extern const void (*Commands_Handlers[]) ( uint8_t * , uint8_t );
//extern struct E220_900T30D e220_900t30d;
extern struct QUEUE_MANAGER queueManager;
extern struct __VarialbeAnswer VarialbeAnswer;
extern struct STATICTICS_i stat_info;

static void Myusb_receive_task (void);
//static void Myusb_transmission_task (void);
static void SystickHandler(void);
static uint8_t CheckData( uint8_t nReceived );
static ___COMMAND parseData( uint8_t nReceived);

struct MY_USB_ myUsb_ = {

	.rx_index = 0,

	.Myusb_receive_task = &Myusb_receive_task,
	//.Myusb_transmission_task = &Myusb_transmission_task,
	.SystickHandler = &SystickHandler,
	.CheckData = &CheckData,
	.parseData = &parseData,

};


static void SystickHandler(void){

	if ( myUsb_.delayReceive ){
		myUsb_.delayReceive--;
	}

	if ( myUsb_.delayTransmit ){
		myUsb_.delayTransmit--;
	}

}


static void Myusb_receive_task (void){

	//static uint8_t len;
	//static union paramLL CRC_VAL;
	//static uint16_t yy;
	//static uint16_t ii;

	if ( myUsb_.dataIsReceived && !myUsb_.delayReceive){
		myUsb_.dataIsReceived = 0;




		if ( !myUsb_.dataOverflow )  {

			myUsb_.rx_index_static = myUsb_.rx_index;
			myUsb_.rx_index = 0;

			if ( myUsb_.rx_index_static > 6 ){	// the first byte - 'Address' ; the second - 'index' ; plus four bytes of CRC
				myUsb_.rx_buf_static[myUsb_.rx_index_static] = 0;	//?
				my_memcpy(myUsb_.rx_buf_static, myUsb_.rx_buf, myUsb_.rx_index_static);


				if ( myUsb_.CheckData(myUsb_.rx_index_static) ) {
					myUsb_.parseDataResult = myUsb_.parseData(myUsb_.rx_index_static);
				} else {
					myUsb_.parseDataResult = __CMD_BADCRC;
				}


				switch( myUsb_.parseDataResult ){

				case __CMD_RETRANSMIT:

					if ( queueManager.add(myUsb_.rx_buf_static, myUsb_.rx_index_static, QUEUE_ANSWER_REQUIRED) == Q_result_ER ){
						queueManager.queue_overflows++;
					} else {
						// I will confirm only in positive case ( when there is no queue overflow )
						queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_HOST;
						myUsb_.delayTransmit = MY_USB_DELAY_TX_SKIP;	// reset
					}



					//e220_900t30d.Transmit((char*)myUsb_.rx_buf_static, myUsb_.rx_index_static, TR_TYPE_TRANSPARENT);

					break;

				case __CMD_CONFIRMATION:

					if ( queueManager.fromHost.waiting ){
						queueManager.remove(queueManager.fromHost.last_index);
						queueManager.fromHost.waiting = 0;
						myUsb_.delayTransmit = MY_USB_DELAY_TX_SKIP;	// reset
					}

					break;

				case __CMD_GET_STATISTICS:

					queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_HOST;
					myUsb_.delayTransmit = MY_USB_DELAY_TX_SKIP;	// reset


					USB_Answer_Fill(&VarialbeAnswer, &(usb_Answers[USB_ANSW_GET_STATISTICS_INDEX]), (struct cmd_result*)0);

					if ( queueManager.add(VarialbeAnswer.answer, VarialbeAnswer.length, QUEUE_ANSWER_REQUIRED ) == Q_result_ER ){
						queueManager.queue_overflows++;
					}

					break;

				case __CMD_PING:

					queueManager.ImustSendConfirmation |= CONFIRMATION_FLAG_TO_HOST;
					myUsb_.delayTransmit = MY_USB_DELAY_TX_SKIP;	// reset

					break;
				case __CMD_BADCRC:

					stat_info.Events_USB[STAT_EVENTS_USB_BADCRC_INDEX]++;

					break;
				case __CMD_SET_E220_900T30D:

					//Start Setting

					// START_SETTING( command_Params );

					//  * * * * * add code * * * * *       add constant answers to ANSWERS array
					// ( asynchronous block )

					break;

				case __CMD_UNKNOWN:

					stat_info.Events_USB[STAT_EVENTS_USB_UNKNOWN_INDEX]++;

					break;

				}


			} else {

				stat_info.Events_USB[STAT_EVENTS_USB_BADSIZE_INDEX]++;


			}
		} else {
			myUsb_.dataOverflow = 0;			//  ???? here

			USB_Answer_Fill( &VarialbeAnswer, &(usb_Answers[USB_ANSW_USB_RX_BUFFER_OWERFLOW_INDEX]), (struct cmd_result *)0 );

			if ( queueManager.add(VarialbeAnswer.answer, VarialbeAnswer.length, QUEUE_ANSWER_NOT_REQUIRED ) == Q_result_ER ){
				queueManager.queue_overflows++;
			}


		}

		//myUsb_.rx_index_static = 0;

	}

}

/*static void Myusb_transmission_task (void){

	// handle QUEUE


}*/

static uint8_t CheckData( uint8_t nReceived ){
		static union paramLL CRC_VAL;
		static uint16_t yy;
		static uint16_t ii;

		uint8_t res = 0;

		yy = nReceived - CRC_LEN;
		CalculateCRC_32bit(myUsb_.rx_buf_static, yy, &CRC_VAL);

		ii = 0;
		for ( ii = 0 ; ii < CRC_LEN ; ii++ ){
			if ( !(myUsb_.rx_buf_static[yy+ii] == CRC_VAL.par_bytes[ii] ) )
				break;
		}

		if ( ii == 4 ){
			res = 1;
		}

		return res;
}


static ___COMMAND parseData( uint8_t nReceived){

	___COMMAND res = (___COMMAND)__CMD_UNKNOWN;

	if( GET_TARGET_ADDRESS(myUsb_.rx_buf_static[ADDRESS_INDEX]) != THIS_NODE_ADDRESS ){

		res = (___COMMAND)__CMD_RETRANSMIT;
	} else {

		nReceived -= CRC_LEN;

		for ( uint8_t ii = 0; ii < COMMANDS_LEN; ii++ ){
			if ( myUsb_.rx_buf_static[CMD_INDEX] == Commands_[ii] ){

				res = Commands_[ii];
				if ( (uint32_t)Commands_Handlers[ii] != (uint32_t)0 ){
					Commands_Handlers[ii](myUsb_.rx_buf_static, nReceived );
				}
				break;
			}
		}


	}


	return res;
}

