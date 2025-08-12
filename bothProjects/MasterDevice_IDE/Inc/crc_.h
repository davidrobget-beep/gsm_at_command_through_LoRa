#ifndef MY_CRC_H
#define MY_CRC_H

#include "stm32f070x6.h"

#include "stdint.h"

#define CRC_LEN 4

////// for converting uint8_t[4] <--> uint32_t  ////////////////////////////
union paramLL {
  uint32_t par_u32;
  uint8_t par_bytes[4];		//byte0, byte1, byte2, byte3
};


void CalculateCRC(uint8_t * data, uint8_t len, union paramLL * oneParam );
void CalculateCRC_32bit(uint8_t * data, uint8_t len, union paramLL * oneParam );

#endif
