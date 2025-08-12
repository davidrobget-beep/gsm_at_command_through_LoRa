#ifndef ANSWERS_E220_900T30D_H_
#define ANSWERS_E220_900T30D_H_

// constant answers

#include <stdint.h>
#include "queue_m.h"
#include "answer_common.h"

#define E220_900T30D_ANSW_GOTIT_INDEX 	0			// is sended if CRC is good
//#define ANSW__RX_BUFFER_OWERFLOW_INDEX 1

#define E220_900T30D_ANSWERS_NUMBER 1


void e220_900t30d_Answer_Fill( struct __VarialbeAnswer * Answer, const struct answer_ * answFrom, const struct cmd_result * params );


#endif
