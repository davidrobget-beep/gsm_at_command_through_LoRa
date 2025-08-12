import struct

import commands as CMD

def parseData(data, length):
    res = CMD.__CMD_UNKNOWN

    if (CMD.GET_TARGET_ADDRESS(data[CMD.ADDRESS_INDEX]) == CMD.__HOST_ADDRESS):  # it always is

        for _cmd in CMD.Commands_:

            if (data[CMD.CMD_INDEX] == _cmd):
                res = _cmd
                break

    return res


KTY81_States = ["OK_T", "ERROR_T", "DISATTACHED_T"]  # //HI_T,  //LOW_T,


# Private parse
# ba - bytearray-AllAnswer
#
def parse_ADC_float_data(data):
    # [3bytes_Header][1byte-KTY81-CurrentState][4bytes-KTY81-AdcData][4bytes-LiPo2S-AdcData][4bytes-IntTemperature-AdcData][4bytes-VREFint-AdcData][4bytes_CRC]

    result = ""

    result += f"KTY State: {KTY81_States[data[3]]}\r\n"

    float_value = struct.unpack('<f', data[4:8])[0]
    result += f"KTY Temperature: {float_value:.1f}\r\n"

    float_value = struct.unpack('<f', data[8:12])[0]
    result += f"LiPo 2S Voltage: {float_value:.1f}\r\n"

    float_value = struct.unpack('<f', data[12:16])[0]
    result += f"Internal mcu temperature: {float_value:.1f}\r\n"

    float_value = struct.unpack('<f', data[16:20])[0]
    result += f"VRefInt: {float_value:.1f}\r\n"

    return result


def parse_Statistics_data(data):

    result = ""

    source_address = CMD.GET_SOURCE_ADDRESS(data[CMD.ADDRESS_INDEX])

    if (source_address == CMD.__NODE1_ADDRESS):
        result += "= = = STATISTICS from NODE 1:\r\n"

        float_value = struct.unpack('<I', data[3:7])[0]  # unsigned int; 4 bytes
        result += f"E220_BADCRC: {float_value}\r\n"

        float_value = struct.unpack('<I', data[7:11])[0]
        result += f"E220_BADSIZE: {float_value}\r\n"

        float_value = struct.unpack('<I', data[11:15])[0]
        result += f"E220_UNKNOWN: {float_value}\r\n"
        # * * * * * * * *
        float_value = struct.unpack('<I', data[15:19])[0]  # unsigned int; 4 bytes
        result += f"USB_BADCRC: {float_value}\r\n"

        float_value = struct.unpack('<I', data[19:23])[0]
        result += f"USB_BADSIZE: {float_value}\r\n"

        float_value = struct.unpack('<I', data[23:27])[0]
        result += f"USB_UNKNOWN: {float_value}\r\n"


    elif (source_address == CMD.__NODE2_ADDRESS):
        result += "= = = STATISTICS from NODE 2:\r\n"

        float_value = struct.unpack('<I', data[3:7])[0]  # unsigned int; 4 bytes
        result += f"E220_BADCRC: {float_value}\r\n"

        float_value = struct.unpack('<I', data[7:11])[0]
        result += f"E220_BADSIZE: {float_value}\r\n"

        float_value = struct.unpack('<I', data[11:15])[0]
        result += f"E220_UNKNOWN: {float_value}\r\n"

    else:
        result += "Known source address.\r\n"

    return result


class ParseModemData_info:
  def __init__(self):
    self.TotalPackets = 0
    self.CurrentPacket = 0
    self.message = ""


# any packet:
#  [3bytes_Header][usefull_message][4bytes_CRC32]
#
def parse_Modem_Data(data, length) :

    res = ParseModemData_info()

    if ( data[CMD.MESSAGE_INDEX_INDEX] == 0 ) :         # single message
        res.TotalPackets  = 1
        res.CurrentPacket = 1
    else:
        numbers = CMD.GET_PART_NUMBERS(data[CMD.MESSAGE_INDEX_INDEX])
        res.TotalPackets = numbers.TotalPackets
        res.CurrentPacket = numbers.CurrentPacket

    res.message = data[CMD.FIRST_BYTE_INDEX:length - CMD.CRC_LEN]

    return res

def parse_Modem_Status(data):

    if data[CMD.FIRST_BYTE_INDEX] == 0:
        return "OFF"
    else:
        return "ON"

