/*
 * strutil.cpp
 *
 *  Created on: Nov 28, 2016
 *      Author: compi
 */
#include "strutil.h"
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////
inline char tochr(uint8_t in, bool upper)
{
	return in + (in < 10) ? '0' : (upper ? 'A' : 'a') - 10;
}

//////////////////////////////////////////////////////////////////////////////
size_t uitodec( char* buffer, unsigned int data)
{
	char *b2 = buffer;
	if(!data) {
		*b2++ = '0';
		*b2 = '\0';
		return 1;
	}

	while(data) {
		*b2++ = (data % 10) + '0';
		data /= 10;
	}
	size_t ret = b2 - buffer;

	*b2-- = 0;

	strrev(buffer, b2);
    return ret;
}

//////////////////////////////////////////////////////////////////////////////
size_t uitohex( char* buffer, unsigned int data)
{
	char *b2 = buffer;
	if(!data) {
		*b2++ = '0';
		*b2 = '\0';
		return 1;
	}

	while(data) {
		uint8_t curval = data & 0x0f;
		*b2++ = tochr(curval, false);
		data /= 16;
	}
	size_t ret = b2 - buffer;

	*b2-- = 0;

	strrev(buffer, b2);
    return ret;
}




