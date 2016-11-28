/*
 * tostr.h
 *
 *  Created on: Nov 28, 2016
 *      Author: compi
 */

#ifndef INC_STRUTIL_H_
#define INC_STRUTIL_H_

#include <stddef.h>

//////////////////////////////////////////////////////////////////////////////
size_t uitodec( char* buffer, unsigned int data);
size_t uitohex( char* buffer, unsigned int data);

//////////////////////////////////////////////////////////////////////////////
inline void strrev(char *first, char *last)
{
	char tmp;
	while(last > first) {
		tmp = *first;
		*first++ = *last;
		*last-- = tmp;
	}
}

#endif /* INC_STRUTIL_H_ */
