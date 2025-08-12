#ifndef QUEUE_MANAGER_H_
#define QUEUE_MANAGER_H_

#include <stdint.h>

#define Q_QUEUE_LEN    8
#define Q_MESSAGE_LEN  96

enum Q_result { Q_result_OK, Q_result_ER };

#define GET_TARGET_ADDRESS(data_1B) ( (uint8_t)(data_1B & 0x0F) )
#define GET_SOURCE_ADDRESS(data_1B) ( (uint8_t)((data_1B & 0xF0) >> 4) )
#define GET_FULL_ADDRESS( Source, Target ) (  (uint8_t)((Source << 4) | ( Target & 0x0F )) )

#define HOST_ADDRESS 0
#define THIS_NODE_ADDRESS 1
#define NODE2_ADDRESS 2
#define MODEM_ADDRESS 3

#define CONFIRMATION_FLAG_TO_HOST  0x01
#define CONFIRMATION_FLAG_TO_NODE2 0x02

struct waitForConfirmation {
	uint8_t waiting;		// 0 - no ; 1 - yes
	uint8_t last_index;
};

struct Q_Node {
	uint8_t array[Q_MESSAGE_LEN];		// [4bits-Source_address & 4bits-Target_address ][1B-Index][message][4B-CRC]

	uint8_t len;			// uint16_t					// empty node is with len = 0
	uint8_t pri;			// priority: high(1) or low(0)
	uint8_t toHost_toModem;	// 0 - to Host; 1 - to Modem

	uint8_t is_it_Request;	// 0 - no, this message is answer ; 1 - yes, a message is request.
};


#define QUEUE_ANSWER_REQUIRED		1			// it is request
#define QUEUE_ANSWER_NOT_REQUIRED	0			// it is answer

struct QUEUE_MANAGER {

	struct Q_Node *queue;
	uint8_t available;			// number of empty nodes ready to use
	uint8_t high_pri;			// number of nodes with high priority
	uint8_t  low_pri;			// number of nodes with low  priority

	uint8_t toHostDirectory;	// number of messages which need to be sent to Host side
	uint8_t toModemDirectory;	// number of messages which need to be sent to Devices with 2 or 3 address

	uint8_t ImustSendConfirmation;

	uint32_t queue_overflows;	// statistics

	struct waitForConfirmation fromHost;
	struct waitForConfirmation fromNode2;

	//void (*init)(void);
	void (*task)(void);
	enum Q_result (*add)( const uint8_t * data, uint16_t length, uint8_t is_it_Request );
	void (*remove)( uint8_t index );

};


// * * * * * * * * * * * * * * * *

#define VARIABLE_ANSWER_LENGTH 32							// FIX. CHANGE !!!!!!!!!!!!!!!!!!!!!!!

struct __VarialbeAnswer {

	uint8_t answer[VARIABLE_ANSWER_LENGTH];
	uint8_t length;
};

#endif

