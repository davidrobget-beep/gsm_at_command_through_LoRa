#include "my_lib_.h"


//void * my_memcpy (void *__restrict to, const void *__restrict from, size_t len){
//void my_memcpy (void *__restrict to, const void *__restrict from, size_t len){
void my_memcpy ( uint8_t * to, const uint8_t * from, size_t len){

	/*uint8_t * TO   = (uint8_t *)to;
	uint8_t * FROM = (uint8_t *)from;


	for ( size_t ii = 0; ii < len ; ii++ ){
		*TO++ = *FROM++;
	}*/

	for ( size_t ii = 0; ii < len ; ii++ ){
			*to++ = *from++;
	}
}
