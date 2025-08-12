#include "commands.h"

static void __Cmd_Set_E220_900T30D_Handler( uint8_t * byteArray, uint8_t length );

const ___COMMAND Commands_[COMMANDS_LEN] = {__CMD_RETRANSMIT, __CMD_CONFIRMATION, __CMD_BADCRC, __CMD_SET_E220_900T30D,
											__CMD_PING, __CMD_PING_ANSWER, __CMD_UNKNOWN, __CMD_BADSIZE, __CMD_RX_BUFFER_OWERFLOW, __CMD_PHY_TURN_MODEM_ON,
											__CMD_PHY_TURN_MODEM_OFF, __CMD_PHY_MODEM_SWITCH_RESULT, __CMD_MODEM_POWER_SWITCH, __CMD_MODEM_STATUS_STATE,
											__CMD_ADC_GET_DATA, __CMD_GET_STATISTICS };

const void (*Commands_Handlers[COMMANDS_LEN]) ( uint8_t * , uint8_t ) = {0, 0, 0, &__Cmd_Set_E220_900T30D_Handler, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


static void __Cmd_Set_E220_900T30D_Handler( uint8_t * byteArray, uint8_t length ){


			//     A  D  D

}

