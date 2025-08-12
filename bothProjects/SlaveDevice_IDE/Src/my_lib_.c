#include "my_lib_.h"


//void * my_memcpy (void *__restrict to, const void *__restrict from, size_t len){
//void my_memcpy (void *__restrict to, const void *__restrict from, size_t len){
void my_memcpy (uint8_t * to, const uint8_t * from, size_t len){

	/*uint8_t * TO   = (uint8_t *)to;
	uint8_t * FROM = (uint8_t *)from;


	for ( size_t ii = 0; ii < len ; ii++ ){
		*TO++ = *FROM++;
	}*/

	for ( size_t ii = 0; ii < len ; ii++ ){
			*to++ = *from++;
	}
}


/**
 * Searches for a substring in a byte array.
 *
 * @param Buffer The byte array to search in.
 * @param buf_len The length of the Buffer.
 * @param needle The substring to search for (as a byte array).
 * @param needle_len The length of the needle.
 * @return The index of the first occurrence of 'needle' in 'Buffer', or -1 if not found.
 */
int my_strstr(uint8_t *Buffer, uint16_t buf_len, const uint8_t *needle, uint16_t needle_len) {
    if (needle_len == 0) {
        return -2; // Empty needle matches at start.
    }

    if (buf_len < needle_len) {
        return -1; // Needle is longer than the buffer.
    }

    for (uint16_t i = 0; i <= buf_len - needle_len; i++) {
        uint16_t j;
        for (j = 0; j < needle_len; j++) {
            if (Buffer[i + j] != needle[j]) {
                break;
            }
        }
        if (j == needle_len) {
            return i; // Found at position i.
        }
    }

    return -1; // Not found.
}
