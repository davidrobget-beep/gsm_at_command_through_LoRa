#include "big_messages.h"
#include "commands.h"
#include "my_lib_.h"
#include "queue_m.h"


uint8_t buf_to_modem[BUFFER_TO_MODEM_LEN];	//


static void BM_set_flag  (uint32_t n);
static void BM_reset_flag(uint32_t n);
static uint32_t BM_is_flag_set(uint32_t n);
static void BM_addPacket(uint8_t * packet, uint8_t len, uint8_t index);
static void BM_Systick_Handler(void);

struct Big_Message_Process assembling_big_message = {

		.buffer_toModem = buf_to_modem,

		//.Started =

		//.flags = 0
		.set_flag    		= &BM_set_flag,
		.reset_flag  		= &BM_reset_flag,
		.is_flag_set 		= &BM_is_flag_set,

		.addPacket 			= &BM_addPacket,
		.Systick_Handler 	= &BM_Systick_Handler,

};


static void BM_set_flag(uint32_t n) {
	assembling_big_message.flags |= (1 << n);
}

static void BM_reset_flag(uint32_t n) {
	assembling_big_message.flags &= ~(1 << n);
}

static uint32_t BM_is_flag_set(uint32_t n) {
    return (assembling_big_message.flags >> n) & 1;
}


// len:   whole message with header and CRC
// index: each index has its own place( from 0 up to 15 ) within buffer. indices are from 1 to 16(max)
// last: it is the last item
//
static void BM_addPacket(uint8_t * packet, uint8_t len, uint8_t index){

	uint32_t offset;

	offset = (index-1)*MESSAGE_LEN;
	len = len-TECH_INFO;

	my_memcpy( assembling_big_message.buffer_toModem + offset, packet+FIRST_BYTE_INDEX, len );
	assembling_big_message.length_toModem += len;

}

static void BM_Systick_Handler(void){

	if ( assembling_big_message.timeoutToReset ){

		if ( !assembling_big_message.Stop_timer ){
			assembling_big_message.timeoutToReset--;
			if ( assembling_big_message.timeoutToReset == 0 ){
				assembling_big_message.Statistics++;
				assembling_big_message.Started = 0;
			}
		} else {
			assembling_big_message.Stop_timer = 0;
			assembling_big_message.timeoutToReset = 0;
			assembling_big_message.Started = 0;			// it is already reset in queue_m.c
		}
	}


}

