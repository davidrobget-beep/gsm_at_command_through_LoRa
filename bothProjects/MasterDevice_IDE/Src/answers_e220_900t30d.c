#include <answers_e220_900t30d.h>
#include "my_lib_.h"
#include "crc_.h"
#include "commands.h"


extern struct QUEUE_MANAGER queueManager;


#define E220_900T30D_ANSW_GOTIT_LENGTH 3
const uint8_t E220_900T30D_ANSW_GOTIT[E220_900T30D_ANSW_GOTIT_LENGTH] = {GET_FULL_ADDRESS(THIS_NODE_ADDRESS, NODE2_ADDRESS), 0, __CMD_CONFIRMATION };
static void E220_900T30D_ANSW_GOTIT_Handler(struct __VarialbeAnswer * Answer, const struct cmd_result * params);

const struct answer_  e220_900t30d_Answers[E220_900T30D_ANSWERS_NUMBER] = {

	[E220_900T30D_ANSW_GOTIT_INDEX] = 	{ .data = E220_900T30D_ANSW_GOTIT, 		.length = E220_900T30D_ANSW_GOTIT_LENGTH,	.Handler = &E220_900T30D_ANSW_GOTIT_Handler},

};



// common
void e220_900t30d_Answer_Fill( struct __VarialbeAnswer * Answer, const struct answer_ * answFrom, const struct cmd_result * params ){

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


static void E220_900T30D_ANSW_GOTIT_Handler(struct __VarialbeAnswer * Answer, const struct cmd_result * params){


}
