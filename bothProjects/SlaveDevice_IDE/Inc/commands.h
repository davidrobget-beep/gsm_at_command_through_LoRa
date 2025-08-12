#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <stdint.h>

// common Commands for all Nodes ( #0 - HOST; #1 - NODE1; #2 - NODE2 )
// if a command is not used on a NODE , it can be commented
//   and COMMANDS_LEN will be decremented

// third byte(index 2) within every message
// index where a command is
#define CMD_INDEX 2
#define ADDRESS_INDEX 0
// 0 - if it is a single message;  [Current_packet_Number + Total_packet_number], example 0x25 means it is second packet of five.
#define MESSAGE_INDEX_INDEX 1

// first byte of the usefull message following HEADER.
#define FIRST_BYTE_INDEX 3

#define __CMD_RETRANSMIT 				11
#define __CMD_CONFIRMATION				12
#define __CMD_BADCRC					13
#define __CMD_SET_E220_900T30D 			14
#define __CMD_PING 						15
#define __CMD_PING_ANSWER				16
#define __CMD_UNKNOWN 					17
#define __CMD_BADSIZE 					18
#define __CMD_RX_BUFFER_OWERFLOW		19
#define __CMD_PHY_TURN_MODEM_ON			20
#define __CMD_PHY_TURN_MODEM_OFF		21
#define __CMD_PHY_MODEM_SWITCH_RESULT	22
#define __CMD_MODEM_POWER_SWITCH		23
#define __CMD_MODEM_STATUS_STATE		24
#define __CMD_ADC_GET_DATA				25
#define __CMD_GET_STATISTICS			26

// array length
#define COMMANDS_LEN 16

typedef uint8_t ___COMMAND;

#endif
