#ifndef UART_COMMON_H
#define UART_COMMON_H

#include <stdint.h>
#include "commands.h"
#include "queue_m.h"


enum TransmitStates { TX_STOPPED, TX_ONGOING, TX_COMPLETE };


struct USART_or_DMA_errors {
	uint8_t DMA_TransferError_uartTX;
	uint8_t DMA_TransferError_uartRX;
	uint8_t USART_Error_RX;							// Framing or Noise or Overrun error
};

struct IdleLineDetection {
	uint8_t received;				// some new data was received
	uint8_t pauseAfterReceiving;
	uint32_t lastPosition;	// in dma ring buffer
	uint32_t currPosition;	// in dma ring buffer
	uint16_t dmaTC;
	uint16_t End;						// the first  part of a message. Number of bytes from currPosition
	uint16_t Start;					// the second part of a message. Number of bytes from the begin of the Buffer
};

// when a user presses Ping button App waits for this time
//  and turns the buzzer on when getting the confirmation message.
#define PING_ANSWER_TIME_TO_BUZZER_ON	PAUSE_AFTER_TRANSMIT

struct UART_INSTANCE {

	uint8_t * TransmitBuffer;
	uint8_t * ReceiveBuffer;
	uint8_t * Linear_Buffer;

	enum TransmitStates TransmitCurState;

	___COMMAND parseDataResult;

	struct USART_or_DMA_errors USART_or_DMA_error;
	struct IdleLineDetection IdleLineDetected;

	volatile uint16_t pauseAfterTransmit;

	uint16_t ping_answer_time_to_buzzer_on;

	void (*Init)(void);
	void (*Transmit)(uint32_t len, uint16_t pause);
	void (*ReceiveEnable)(uint32_t len);
	void (*SetTransmissionBuffer)( const char * message, uint16_t length );
	void (*AddToTransmissionBuffer)( const char * message, uint16_t length, uint16_t offset );

	void (*Receive) (void);

	uint8_t (*CheckData)(  uint8_t nReceived );
	___COMMAND (*parseData)( uint8_t nReceived );

	void (*DMA_Handler)(void);			// IRQ
	void (*UART_IRQHandler)(void);		// IRQ
	void (*SysTickHandler)(void);		// IRQ

};

#endif
