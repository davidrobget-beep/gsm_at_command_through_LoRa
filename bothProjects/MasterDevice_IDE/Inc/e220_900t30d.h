#ifndef E220_900T30D_H
#define E220_900T30D_H

#include <stdint.h>
#include "commands.h"

enum TransmitType { TR_TYPE_TRANSPARENT, TR_TYPE_NORMAL };

struct E220_900T30D {

	uint8_t * bufferToParse;

	___COMMAND parseDataResult;

	void (*Init)(void);
	void (*Transmit)(const char * message, uint32_t len, enum TransmitType transType, uint16_t pause );
	void (*Receive)(void);

	uint8_t (*CheckData)(  uint8_t nReceived );
	___COMMAND (*parseData)( uint8_t nReceived );

};

#endif
