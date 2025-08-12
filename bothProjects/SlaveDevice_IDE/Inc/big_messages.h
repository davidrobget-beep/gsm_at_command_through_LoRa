#ifndef BIG_MESSAGES_H_
#define BIG_MESSAGES_H_

#include <stdint.h>

// host can send to modem
#define BUFFER_TO_MODEM_LEN 1024


#define TIMEOUT_TO_RESET_VALUE 2000			// milliseconds

struct Big_Message_Process {

	uint8_t * buffer_toModem;	// directory
	uint8_t * buffer_toHost;	// directory

	uint16_t length_toModem;	// relates to buffer_toModem
	uint16_t length_toHost;		// relates to buffer_toHost

	uint8_t Started;

	uint16_t timeoutToReset;
	uint8_t  Stop_timer;
	uint32_t Statistics;		// now only one parameter

	uint32_t flags;				// every bit relates to a message status by index.  are 16 indices from index 1.
	void (*set_flag)(uint32_t n);
	void (*reset_flag)(uint32_t n);
	uint32_t (*is_flag_set)(uint32_t n);

	void (*addPacket)(uint8_t * packet, uint8_t len, uint8_t index);

	void (*Systick_Handler)(void);
};

#endif

