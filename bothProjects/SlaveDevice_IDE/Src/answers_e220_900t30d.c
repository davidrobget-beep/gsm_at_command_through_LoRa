#include <answers_e220_900t30d.h>
#include "my_lib_.h"
#include "crc_.h"
#include "commands.h"
#include <kty_81_110.h>
#include "adc_measurement.h"
#include "queue_m.h"
#include "statistics.h"

// ! check Total lenght of Answer and compare it with VARIABLE_ANSWER_LENGTH


#define E220_900T30D_ANSW_GOTIT_LENGTH 3
const uint8_t E220_900T30D_ANSW_GOTIT[E220_900T30D_ANSW_GOTIT_LENGTH] = {GET_FULL_ADDRESS(THIS_DEVICE_ADDRESS, NODE1_ADDRESS), 0, __CMD_CONFIRMATION };
static void E220_900T30D_ANSW_GOTIT_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params);

#define E220_900T30D_ANSW_MODEM_POWER_SWITCHING_LENGTH 3
const uint8_t E220_900T30D_ANSW_MODEM_POWER_SWITCHING[E220_900T30D_ANSW_MODEM_POWER_SWITCHING_LENGTH] = {GET_FULL_ADDRESS(THIS_DEVICE_ADDRESS, HOST_ADDRESS), 0, __CMD_PHY_MODEM_SWITCH_RESULT };
static void E220_900T30D_ANSW_MODEM_POWER_SWITCHING_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params);

#define E220_900T30D_ANSW_MODEM_STATUS_STATE_LENGTH 3
const uint8_t E220_900T30D_ANSW_MODEM_STATUS_STATE[E220_900T30D_ANSW_MODEM_STATUS_STATE_LENGTH] = {GET_FULL_ADDRESS(THIS_DEVICE_ADDRESS, HOST_ADDRESS), 0, __CMD_MODEM_STATUS_STATE };
static void E220_900T30D_ANSW_MODEM_STATUS_STATE_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params);

#define E220_900T30D_ANSW_PING_LENGTH 3
const uint8_t E220_900T30D_ANSW_PING[E220_900T30D_ANSW_PING_LENGTH] = {GET_FULL_ADDRESS(THIS_DEVICE_ADDRESS, NODE1_ADDRESS), 0, __CMD_PING };
static void E220_900T30D_ANSW_PING_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params);

#define E220_900T30D_ANSW_PING_ANSWER_LENGTH 3
const uint8_t E220_900T30D_ANSW_PING_ANSWER[E220_900T30D_ANSW_PING_ANSWER_LENGTH] = {GET_FULL_ADDRESS(THIS_DEVICE_ADDRESS, HOST_ADDRESS), 0, __CMD_PING_ANSWER };
static void E220_900T30D_ANSW_PING_ANSWER_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params);

#define E220_900T30D_ANSW_ADC_GET_DATA_LENGTH 3
const uint8_t E220_900T30D_ANSW_ADC_GET_DATA[E220_900T30D_ANSW_ADC_GET_DATA_LENGTH] = {GET_FULL_ADDRESS(THIS_DEVICE_ADDRESS, HOST_ADDRESS), 0, __CMD_ADC_GET_DATA };
static void E220_900T30D_ANSW_ADC_GET_DATA_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params);

#define E220_900T30D_ANSW_GET_STATISTICS_LENGTH 3
const uint8_t E220_900T30D_ANSW_GET_STATISTICS[E220_900T30D_ANSW_GET_STATISTICS_LENGTH] = {GET_FULL_ADDRESS(THIS_DEVICE_ADDRESS, HOST_ADDRESS), 0, __CMD_GET_STATISTICS };
static void E220_900T30D_ANSW_GET_STATISTICS_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params);


const struct answer_  e220_900t30d_Answers[E220_900T30D_ANSWERS_NUMBER] = {

	[E220_900T30D_ANSW_GOTIT_INDEX] = 	{ .data = E220_900T30D_ANSW_GOTIT, 		.length = E220_900T30D_ANSW_GOTIT_LENGTH,	.Handler = &E220_900T30D_ANSW_GOTIT_Handler },
	[E220_900T30D_ANSW_MODEM_POWER_SWITCHING_INDEX] = 	{ .data = E220_900T30D_ANSW_MODEM_POWER_SWITCHING, 		.length = E220_900T30D_ANSW_MODEM_POWER_SWITCHING_LENGTH,	.Handler = &E220_900T30D_ANSW_MODEM_POWER_SWITCHING_Handler },
	[E220_900T30D_ANSW_MODEM_STATUS_STATE_INDEX]	=   { .data = E220_900T30D_ANSW_MODEM_STATUS_STATE, 		.length = E220_900T30D_ANSW_MODEM_STATUS_STATE_LENGTH,		.Handler = &E220_900T30D_ANSW_MODEM_STATUS_STATE_Handler },
	[E220_900T30D_ANSW_PING_INDEX]					=	{.data = E220_900T30D_ANSW_PING,					 	.length = E220_900T30D_ANSW_PING_LENGTH,	.Handler = &E220_900T30D_ANSW_PING_Handler},
	[E220_900T30D_ANSW_PING_ANSWER_INDEX]			=	{.data = E220_900T30D_ANSW_PING_ANSWER, 	.length = E220_900T30D_ANSW_PING_ANSWER_LENGTH,	.Handler = &E220_900T30D_ANSW_PING_ANSWER_Handler},
	[E220_900T30D_ANSW_ADC_GET_DATA_INDEX]			=   {.data = E220_900T30D_ANSW_ADC_GET_DATA, 	.length = E220_900T30D_ANSW_ADC_GET_DATA_LENGTH, .Handler = &E220_900T30D_ANSW_ADC_GET_DATA_Handler},
	[E220_900T30D_ANSW_GET_STATISTICS_INDEX]		=   {.data = E220_900T30D_ANSW_GET_STATISTICS, 	.length = E220_900T30D_ANSW_GET_STATISTICS_LENGTH, .Handler = &E220_900T30D_ANSW_GET_STATISTICS_Handler},
};


// common
void e220_900t30d_Answer_Fill( struct __VarialbeAnswer * Answer, const struct answer_ * answFrom, const struct modem_cmd_result * params ){

	static union paramLL CRC_VAL;

	my_memcpy(Answer->answer, answFrom->data, answFrom->length);
	Answer->length = answFrom->length;

	answFrom->Handler(Answer, params);				// adding more data

	CalculateCRC_32bit(Answer->answer, Answer->length, &CRC_VAL);
	for ( uint8_t ii = 0; ii < CRC_LEN; ii++ ){
		Answer->answer[Answer->length + ii] = CRC_VAL.par_bytes[ii];
	}
	Answer->length += 4;

}

// * * * * * * * * * * * * *


static void E220_900T30D_ANSW_GOTIT_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params){


}

static void E220_900T30D_ANSW_MODEM_POWER_SWITCHING_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params){


	Answer->answer[Answer->length] = (uint8_t)params->value_1;
	Answer->length++;

}

static void E220_900T30D_ANSW_MODEM_STATUS_STATE_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params){

	Answer->answer[Answer->length] = (uint8_t)params->value_1;
	Answer->length++;

}

static void E220_900T30D_ANSW_PING_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params){


}

static void E220_900T30D_ANSW_PING_ANSWER_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params){


}


extern struct KTY_81_110 kty81_110;
extern struct ADC_CALCS_ ADC_data;

static void E220_900T30D_ANSW_ADC_GET_DATA_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params){

	union paramLL Par1;

	Answer->answer[Answer->length++] = (uint8_t)kty81_110.CurrentState;

	for ( uint8_t ii = 0; ii < ADC_BUFFER_LEN; ii++ ){

		Par1.par_fl = ADC_data.values[ii];

		for ( uint8_t yy = 0; yy < 4; yy++){
			Answer->answer[Answer->length++] = Par1.par_bytes[yy];
		}

	}

	// result array:
	// [1byte-KTY81-CurrentState][4bytes-KTY81-AdcData][4bytes-LiPo2S-AdcData][4bytes-IntTemperature-AdcData][4bytes-VREFint-AdcData]

}


extern struct STATICTICS_i stat_info;

static void E220_900T30D_ANSW_GET_STATISTICS_Handler(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params){

	union paramLL Par1;

	for ( uint8_t ii = 0; ii < STAT_EVENTS_E220_NUMBER; ii++ ){
		Par1.par_u32 = stat_info.Events_E220[ii];

		for ( uint8_t yy = 0; yy < 4; yy++){
			Answer->answer[Answer->length++] = Par1.par_bytes[yy];
		}
	}
}

