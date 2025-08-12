#ifndef _MY_LIB_H
#define _MY_LIB_H

#include <stddef.h>
#include <stdint.h>

//void *	 my_memcpy (void *__restrict to, const void *__restrict from, size_t len);
//void my_memcpy (void *__restrict to, const void *__restrict from, size_t len);
void my_memcpy (uint8_t * to, const uint8_t * from, size_t len);
int my_strstr(uint8_t *Buffer, uint16_t buf_len, const uint8_t *needle, uint16_t needle_len);
#endif
