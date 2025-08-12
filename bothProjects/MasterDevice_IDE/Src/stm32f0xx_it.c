#include <stdint.h>
#include "main.h"
#include "stm32f0xx_it.h"
#include "my_uart.h"
#include "buzzer.h"
#include "my_usb.h"

extern PCD_HandleTypeDef hpcd_USB_FS;
extern struct Buzzers buzzer;

extern struct MY_UART my_uart;
extern struct MY_USB_ myUsb_;


//extern uint16_t debug_send;				// delete

void SysTick_Handler(void)
{

	my_uart.SystickHandler();

	buzzer.systick();

	myUsb_.SystickHandler();


/*	if ( debug_send )
		debug_send--;
*/

	HAL_IncTick();			// does it need for USB ?
}

void USB_IRQHandler(void)
{

  HAL_PCD_IRQHandler(&hpcd_USB_FS);

}


void NMI_Handler (void){

	while(1);
}

void HardFault_Handler(void)
{

	while(1);
}


void MemManage_Handler(void)
{

	while(1);
}

void BusFault_Handler(void)
{

	while(1);
}

void SVCall_Handler(void){


}

void PendSV_Handler(void){

}

void __attribute__((optimize("O0"))) DMA_CH4_5_IRQHandler(void){

		// RECEIVING
		if ( LL_DMA_IsActiveFlag_TC5(DMA1) ) {

			LL_DMA_ClearFlag_TC5(DMA1);
			my_uart.IdleLineDetected.dmaTC++;

		} else if ( LL_DMA_IsActiveFlag_TE5(DMA1) ){

			LL_DMA_ClearFlag_TE5(DMA1);
			my_uart.USART_or_DMA_error.DMA_TransferError_uartRX = 1;
		}

		// TRANSMISSION
		if ( LL_DMA_IsActiveFlag_TC4(DMA1) ) {

			LL_DMA_ClearFlag_TC4(DMA1);
			LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_4);

		} else if ( LL_DMA_IsActiveFlag_TE4(DMA1) ){

			LL_DMA_ClearFlag_TE4(DMA1);
			my_uart.USART_or_DMA_error.DMA_TransferError_uartTX = 1;
		}

}

void USART2_IRQHandler(void)
{

	// T R A N S M I S S I O N
	if ( LL_USART_IsActiveFlag_TC(USART2)){

		LL_USART_ClearFlag_TC(USART2);
		my_uart.TransmitCurState = TX_COMPLETE;

	}

	// R E C E P T I O N
	if ( LL_USART_IsActiveFlag_IDLE(USART2) ) {

		LL_USART_ClearFlag_IDLE(USART2);


		//*******************************************************
				my_uart.IdleLineDetected.currPosition = ((DMA_Channel_TypeDef *)(__LL_DMA_GET_CHANNEL_INSTANCE(DMA1, LL_DMA_CHANNEL_5)))->CNDTR;

				if ( my_uart.IdleLineDetected.lastPosition > my_uart.IdleLineDetected.currPosition ) {

					if ( my_uart.IdleLineDetected.dmaTC == 0 ){
						my_uart.IdleLineDetected.End = my_uart.IdleLineDetected.lastPosition - my_uart.IdleLineDetected.currPosition;
						my_uart.IdleLineDetected.Start = 0;
					} else {	// We have buffer overrun/overflow
						my_uart.IdleLineDetected.End = 0;
						my_uart.IdleLineDetected.Start = 0;
					}

				} else {	// if ( IdleLineDetected.lastPosition <= IdleLineDetected.currPosition )

					if ( my_uart.IdleLineDetected.dmaTC == 1 ){
						my_uart.IdleLineDetected.End = my_uart.IdleLineDetected.lastPosition;
						my_uart.IdleLineDetected.Start = RECEIVE_RING_BUFFER_LEN - my_uart.IdleLineDetected.currPosition;
					} else {
						// if ( IdleLineDetected.dmaTC == 0 ) { it is imposible }
						// if ( IdleLineDetected.dmaTC > 1  ) { overflow        }
						my_uart.IdleLineDetected.End = 0;
						my_uart.IdleLineDetected.Start = 0;
					}

				}


		if ( my_uart.IdleLineDetected.End + my_uart.IdleLineDetected.Start > 0 ){
			my_uart.IdleLineDetected.received = 1;


			my_uart.IdleLineDetected.pauseAfterReceiving = PAUSE_AFTER_RECEIVING;

		} else {		// for example, I received too many of bytes ( erasing occures)

			my_uart.IdleLineDetected.dmaTC = 0;
			my_uart.IdleLineDetected.lastPosition = my_uart.IdleLineDetected.currPosition;
			if ( my_uart.IdleLineDetected.received )
				my_uart.IdleLineDetected.received = 0;
		}




		//*******************************************************

	} else if ( LL_USART_IsActiveFlag_FE(USART2) ){

		LL_USART_ClearFlag_FE(USART2);
		my_uart.USART_or_DMA_error.USART_Error_RX++;			// = 1;

	} else if ( LL_USART_IsActiveFlag_NE(USART2) ){

		LL_USART_ClearFlag_NE(USART2);
		my_uart.USART_or_DMA_error.USART_Error_RX++;			// = 1;

	} else if ( LL_USART_IsActiveFlag_ORE(USART2) ){

		LL_USART_ClearFlag_ORE(USART2);
		my_uart.USART_or_DMA_error.USART_Error_RX++;			// = 1;

	}


}


