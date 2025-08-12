#include "crc_.h"

void CalculateCRC(uint8_t * data, uint8_t len, union paramLL * oneParam ){


  CRC->CR  |= CRC_CR_RESET;
  for ( uint8_t ii = 0; ii < len; ii++ ){
      CRC->DR = data[ii];
  }

  oneParam->par_u32 = CRC->DR;

}


__attribute__((optimize("O0"))) void CalculateCRC_32bit(uint8_t * data, uint8_t len, union paramLL * oneParam ){

 	uint8_t portion;
 	union paramLL ParLoc;
 	uint8_t ii;


   CRC->CR  |= CRC_CR_RESET;

 	while ( len ){

 	  portion = len < 4 ? len : 4;

 		if ( portion == 4 )
 			//ParLoc.par_u32 = *((uint32_t*)data);	// Hard Fault  ( align )
			for( ii = 0; ii < portion; ii++ ){
				ParLoc.par_bytes[ii] = data[ii];
			}
 		else{
 			for( ii = 0; ii < portion; ii++ ){
 				ParLoc.par_bytes[ii] = data[ii];
 			}
 			for( ii = portion; ii < 4; ii++ ){
 				ParLoc.par_bytes[ii] = 0xFF;
 			}
 		}

 		CRC->DR = ParLoc.par_u32;

 		data += portion;
     len -= portion;
 	}

   oneParam->par_u32 = CRC->DR;

 }
