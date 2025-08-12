#ifndef ANSWERS_USB_H_
#define ANSWERS_USB_H_

// constant answers

#include <stdint.h>
#include "queue_m.h"
#include "answer_common.h"

#define USB_ANSW_USB_RX_BUFFER_OWERFLOW_INDEX 0
#define USB_ANSW_GOTIT_INDEX 1			// is sended if CRC is good;	CONFIRMATION
#define USB_ANSW_GET_STATISTICS_INDEX 2

#define USB_ANSWERS_NUMBER 3

//************************

void USB_Answer_Fill( struct __VarialbeAnswer * Answer, const struct answer_ * answFrom, const struct cmd_result * params );

#endif
