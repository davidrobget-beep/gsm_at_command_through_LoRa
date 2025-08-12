
import commands as CMD

def reorder4(src, length):
    dst = [0] * 4
    appendlen = 4 - (length % 4) if (length % 4) else 0
    idx = 0

    # Handle the padding of 0xFF
    while appendlen > 0:
        dst[idx] = 0xFF
        idx += 1
        appendlen -= 1

    # Copy the reversed src into dst
    while length > 0:
        dst[idx] = src[3 - idx]
        idx += 1
        length -= 1

    return dst


POLY = 0x04C11DB7


def crc32_formula_normal_STM32(length, data):
    crc = 0xFFFFFFFF  # Initialize CRC to all 1s
    buffer = bytearray(data)  # Convert input data into a bytearray

    while length:
        portion = min(length, 4)
        reordered = reorder4(buffer[:portion], portion)

        for i in range(4):
            crc ^= (reordered[i] << 24)
            for bit in range(8):
                if crc & (1 << 31):
                    crc = (crc << 1) ^ POLY
                else:
                    crc = (crc << 1)

        buffer = buffer[portion:]  # Move the buffer forward
        length -= portion

    return crc & 0xFFFFFFFF  # Ensure the result is within 32 bits


def CheckData( data, length ):

    yy = length - CMD.CRC_LEN
    crc32 = crc32_formula_normal_STM32(yy, data)
    byte_array_little = crc32.to_bytes(CMD.CRC_LEN, 'little') # 4 bytes, little-endian

    ii = 0
    while ( ii < CMD.CRC_LEN ) :
        if ( byte_array_little[ii] != data[yy + ii] ) :
            break
        ii += 1

    if ( ii == 4 ) :
        return 1            # GOOD
    else:
        return 0


def add_tech_info_into_message(PacketsList):
    length = 0

    Total_Packet_number = len(PacketsList)

    if (Total_Packet_number > 1):
        packet_index = 1    # start index
        index_byte = (packet_index << 4) | (Total_Packet_number)
    else:
        packet_index = 0
        index_byte = 0

    for b_array in PacketsList:
        header = bytearray(3)
        header[0] = 0x03
        header[1] = index_byte
        header[2] = CMD.__CMD_RETRANSMIT
        packet_index += 1       # next index
        index_byte = (packet_index << 4) | (Total_Packet_number)    # next
        # add the header to the start
        b_array[0:0] = header
        # add CRC to the end
        length = len(b_array)
        CRC32 = crc32_formula_normal_STM32(length, b_array)
        byte_array_little = CRC32.to_bytes(CMD.CRC_LEN, 'little')  # 4 bytes, little-endian
        b_array[length:length] = byte_array_little