#include <answers_usb.h>
//#include "queue_m.h"
#include "my_lib_.h"
#include "crc_.h"
#include "commands.h"
#include "statistics.h"


extern struct QUEUE_MANAGER queueManager;
//extern struct __VarialbeAnswer VarialbeAnswer;
extern struct STATICTICS_i stat_info;


#define USB_ANSW_USB_RX_BUFFER_OWERFLOW_LENGTH 3
const uint8_t USB_ANSW_USB_RX_BUFFER_OWERFLOW[USB_ANSW_USB_RX_BUFFER_OWERFLOW_LENGTH] = {GET_FULL_ADDRESS(THIS_NODE_ADDRESS, HOST_ADDRESS), 0, __CMD_RX_BUFFER_OWERFLOW };
static void USB_ANSW_USB_RX_BUFFER_OWERFLOW_Handler(struct __VarialbeAnswer * Answer, const struct cmd_result * params);

#define USB_ANSW_GOTIT_LENGTH 3
const uint8_t USB_ANSW_GOTIT[USB_ANSW_GOTIT_LENGTH] = {GET_FULL_ADDRESS(THIS_NODE_ADDRESS, HOST_ADDRESS), 0, __CMD_CONFIRMATION };			// change name to CONFIRMATION
static void USB_ANSW_GOTIT_Handler(struct __VarialbeAnswer * Answer, const struct cmd_result * params);

#define USB_ANSW_GET_STATISTICS_LENGTH 3
const uint8_t USB_ANSW_GET_STATISTICS[USB_ANSW_GET_STATISTICS_LENGTH] = {GET_FULL_ADDRESS(THIS_NODE_ADDRESS, HOST_ADDRESS), 0, __CMD_GET_STATISTICS };
static void USB_ANSW_GET_STATISTICS_Handler(struct __VarialbeAnswer * Answer, const struct cmd_result * params);

const struct answer_  usb_Answers[USB_ANSWERS_NUMBER] = {			// headers

	[USB_ANSW_USB_RX_BUFFER_OWERFLOW_INDEX] = 	{ .data = USB_ANSW_USB_RX_BUFFER_OWERFLOW, .length = USB_ANSW_USB_RX_BUFFER_OWERFLOW_LENGTH, .Handler = &USB_ANSW_USB_RX_BUFFER_OWERFLOW_Handler },
	[USB_ANSW_GOTIT_INDEX] 					=	{ .data = USB_ANSW_GOTIT, .length = USB_ANSW_GOTIT_LENGTH , 	.Handler = USB_ANSW_GOTIT_Handler},
	[USB_ANSW_GET_STATISTICS_INDEX]			=	{ .data = USB_ANSW_GET_STATISTICS, .length = USB_ANSW_GET_STATISTICS_LENGTH , 	.Handler = USB_ANSW_GET_STATISTICS_Handler},

};



// common
void USB_Answer_Fill( struct __VarialbeAnswer * Answer, const struct answer_ * answFrom, const struct cmd_result * params ){

	static union paramLL CRC_VAL;

	my_memcpy(Answer->answer, answFrom->data, answFrom->length);
	Answer->length = answFrom->length;

	answFrom->Handler(Answer, params);				// add more data

	CalculateCRC_32bit(Answer->answer, Answer->length, &CRC_VAL);
	for ( uint8_t ii = 0; ii < CRC_LEN; ii++ ){
		Answer->answer[Answer->length + ii] = CRC_VAL.par_bytes[ii];
	}
	Answer->length += 4;

}

// * * * * * * * * * * * * *

static void USB_ANSW_USB_RX_BUFFER_OWERFLOW_Handler(struct __VarialbeAnswer * Answer, const struct cmd_result * params){


}


static void USB_ANSW_GOTIT_Handler(struct __VarialbeAnswer * Answer, const struct cmd_result * params){			// change name to CONFIRMATION

	 Answer->answer[Answer->length] = queueManager.available;			// fix: put available item into params
	 Answer->length++;

}

static void USB_ANSW_GET_STATISTICS_Handler(struct __VarialbeAnswer * Answer, const struct cmd_result * params){

	union paramLL Par1;

	for ( uint8_t ii = 0; ii < STAT_EVENTS_E220_NUMBER; ii++ ){
		Par1.par_u32 = stat_info.Events_E220[ii];

		for ( uint8_t yy = 0; yy < 4; yy++){
			Answer->answer[Answer->length++] = Par1.par_bytes[yy];
		}
	}

	for ( uint8_t ii = 0; ii < STAT_EVENTS_USB_NUMBER; ii++ ){
		Par1.par_u32 = stat_info.Events_USB[ii];

		for ( uint8_t yy = 0; yy < 4; yy++){
			Answer->answer[Answer->length++] = Par1.par_bytes[yy];
		}
	}
}
