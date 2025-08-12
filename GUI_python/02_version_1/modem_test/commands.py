
BUFFER_LEN = 96 # size of one text node in Queue on both Nodes (firmware)
TECH_INFO = 7   # 1 address byte, 1 index byte, 1 command number byte, ..... 4 CRC bytes
MESSAGE_LEN = BUFFER_LEN - TECH_INFO    # usefull data length
CRC_LEN = 4     # 4 bytes

# COMMANDS
__CMD_RETRANSMIT                =   11
__CMD_CONFIRMATION			    =   12
__CMD_BADCRC				    =   13
__CMD_SET_E220_900T30D 		    =   14
__CMD_PING 					    =   15
__CMD_PING_ANSWER               =   16
__CMD_UNKNOWN 				    =   17
__CMD_BADSIZE 				    =   18
__CMD_RX_BUFFER_OWERFLOW	    =   19
__CMD_PHY_TURN_MODEM_ON		    =	20
__CMD_PHY_TURN_MODEM_OFF	    =	21
__CMD_PHY_MODEM_SWITCH_RESULT   =	22
__CMD_MODEM_POWER_SWITCH		=   23
__CMD_MODEM_STATUS_STATE		=   24
__CMD_ADC_GET_DATA              =   25
__CMD_GET_STATISTICS            =   26

COMMANDS_LEN                    =   16       # # # # # #
Commands_ = [__CMD_RETRANSMIT, __CMD_CONFIRMATION, __CMD_BADCRC,
                __CMD_SET_E220_900T30D, __CMD_PING, __CMD_UNKNOWN, __CMD_BADSIZE, __CMD_RX_BUFFER_OWERFLOW,
                __CMD_PHY_TURN_MODEM_ON, __CMD_PHY_TURN_MODEM_OFF, __CMD_PHY_MODEM_SWITCH_RESULT, __CMD_MODEM_POWER_SWITCH,
                __CMD_MODEM_STATUS_STATE, __CMD_ADC_GET_DATA, __CMD_GET_STATISTICS]


# # # # #

def GET_TARGET_ADDRESS(data_1B):
    return (data_1B & 0x0F)


def GET_SOURCE_ADDRESS(data_1B):
    return (data_1B & 0xF0) >> 4


def GET_FULL_ADDRESS(Source, Target):
    return (((Source & 0x0F) << 4) | (Target & 0x0F))

# message[MESSAGE_INDEX_INDEX] == 0 - when it is a single message;  [Current_packet_Number + Total_packet_number], example 0x25 means it is second packet of five.
class PART_NUMBERS_INFO:
  def __init__(self):
    self.CurrentPacket = 0
    self.TotalPackets   = 0

def GET_PART_NUMBERS(data_1B):
    pn_info = PART_NUMBERS_INFO()
    pn_info.CurrentPacket = (data_1B & 0xF0) >> 4
    pn_info.TotalPackets = (data_1B & 0x0F)
    return pn_info



ADDRESS_INDEX       = 0
MESSAGE_INDEX_INDEX = 1
CMD_INDEX           = 2

#   first byte of the usefull message following HEADER.
FIRST_BYTE_INDEX    = 3


__HOST_ADDRESS = 0  # ITSELF
__NODE1_ADDRESS = 1
__NODE2_ADDRESS = 2
__MODEM_ADDRESS = 3

