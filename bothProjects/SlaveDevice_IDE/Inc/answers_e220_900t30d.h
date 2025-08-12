#ifndef ANSWERS_E220_900T30D_H_
#define ANSWERS_E220_900T30D_H_

// constant answers

#include <stdint.h>
#include "queue_m.h"
#include "modem_state.h"

struct answer_ {

	const uint8_t * data;
	int16_t length;
	void (*Handler)(struct __VarialbeAnswer * Answer, const struct modem_cmd_result * params);
};



#define E220_900T30D_ANSW_GOTIT_INDEX 					0			// is sent if CRC is good
#define E220_900T30D_ANSW_MODEM_POWER_SWITCHING_INDEX 	1
#define E220_900T30D_ANSW_MODEM_STATUS_STATE_INDEX		2			// is sent when GSM_STATUS_Pin is changed
#define E220_900T30D_ANSW_PING_INDEX					3
#define E220_900T30D_ANSW_PING_ANSWER_INDEX				4
#define E220_900T30D_ANSW_ADC_GET_DATA_INDEX			5
#define E220_900T30D_ANSW_GET_STATISTICS_INDEX			6
//#define ANSW__RX_BUFFER_OWERFLOW_INDEX 1

#define E220_900T30D_ANSWERS_NUMBER 7


void e220_900t30d_Answer_Fill( struct __VarialbeAnswer * Answer, const struct answer_ * answFrom, const struct modem_cmd_result * params );


#endif
