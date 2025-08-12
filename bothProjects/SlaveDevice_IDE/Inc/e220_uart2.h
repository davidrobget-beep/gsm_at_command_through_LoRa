#ifndef E220_UART2_H
#define E220_UART2_H

#include <stdint.h>
#include "uart.h"

#define E220_UART2_TRANSMIT_BUFFER_LEN 128
#define E220_UART2_RECEIVE_RING_BUFFER_LEN 128

#define E220_UART2_PAUSE_AFTER_RECEIVING 50

void PING_button_down_handler(void);

#endif

