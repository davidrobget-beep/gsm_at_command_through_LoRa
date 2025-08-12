#ifndef __MY_USB_H__
#define __MY_USB_H__

#include <stdint.h>
#include "commands.h"

#define MY_USB_DELAY_RX 		50
#define MY_USB_DELAY_TX 		1000
#define MY_USB_DELAY_TX_SKIP 	5	// reset when confirmation is received

#define BUF_MAX_LEN 192				//
/*
 * #define APP_RX_DATA_SIZE  512   see usbd_cdc_if.h
 * #define APP_TX_DATA_SIZE  512
 * */

//enum USB_COMMANDS { USBCMD_RETRANSMIT, USBCMD_TEST, USBCMD_BADCRC, USBCMD_SET_E220_900T30D, USBCMD_UNKNOWN };



struct MY_USB_ {

	uint8_t rx_buf[BUF_MAX_LEN];	// this buffer is dynamically changed in CDC_Receive_FS() function
	uint8_t rx_index;	// current index and the length simultaneously
	uint8_t rx_buf_static[BUF_MAX_LEN+1];		// is used to find the cmd substring.	// ?? does it need ??
	uint8_t rx_index_static;

	uint16_t delayReceive;	// ms    			//after this delay is out I start to handle a message
	uint16_t delayTransmit;	// ms				// a gap between two messages that I send.

	uint8_t dataIsReceived;
	uint8_t dataOverflow;

	//***
	___COMMAND parseDataResult;

	void (*Myusb_receive_task)(void);
	//void (*Myusb_transmission_task)(void);
	void (*SystickHandler)(void);
	uint8_t (*CheckData)(  uint8_t nReceived );
	___COMMAND (*parseData)( uint8_t nReceived );


};


void myusb_task (void);


#endif /* __MY_USB_H__ */

