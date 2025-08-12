#ifndef MY_UART_H
#define MY_UART_H

#include <stdint.h>

#define TRANSMIT_BUFFER_LEN 128
#define RECEIVE_RING_BUFFER_LEN 128		//

#define PAUSE_AFTER_RECEIVING 		50
#define PAUSE_AFTER_TRANSMIT   		4000	//10000	//2000
#define PAUSE_AFTER_TRANSMIT_SKIP   	5
#define PAUSE_AFTER_TRANSMIT_MINIMUM 400


// To disconnect a connected device DISC pin must be kept in ACTIVE state minimum 10 ms.
// let 12 ms
//#define DISC_HOLD_MIN 12

enum TransmitStates { TX_STOPPED, TX_ONGOING, TX_COMPLETE };


struct IdleLineDetection {
	uint8_t received;				// some new data was received
	uint8_t pauseAfterReceiving;
	uint32_t lastPosition;	// in dma ring buffer
	uint32_t currPosition;	// in dma ring buffer
	uint16_t dmaTC;
	uint16_t End;						// the first  part of a message. Number of bytes from currPosition
	uint16_t Start;					// the second part of a message. Number of bytes from the begin of the Buffer
};

struct usart_or_dma_errors {

	uint8_t DMA_TransferError_uartRX;
	uint8_t DMA_TransferError_uartTX;
	uint8_t USART_Error_RX;

};

struct MY_UART {

	enum TransmitStates TransmitCurState;
	struct usart_or_dma_errors USART_or_DMA_error;
	struct IdleLineDetection IdleLineDetected;
	uint16_t pauseAfterTransmit;

	void (*SystickHandler) (void);

};

void MX_USART2_UART_Init(void);

void MY_UART_Transmit(uint32_t len, uint16_t pause);
void MY_UART_Receive(uint32_t len);
void MY_UART_SetTransmissionBuffer( const char * message, uint16_t length );
void MY_UART_AddToTransmissionBuffer( const char * message, uint16_t length, uint16_t offset );

#endif

