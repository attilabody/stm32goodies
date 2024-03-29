/*
 * strutil.cpp
 *
 *  Created on: Nov 28, 2016
 *      Author: compi
 */
#include <sg/Strutil.h>
#include <cstdint>

using namespace sg;

//////////////////////////////////////////////////////////////////////////////
char sg::FromChar( char c, bool decimal)
{
	if(c >= '0' && c <= '9') return c - '0';
	if(decimal) return -1;
	if( c >= 'a' && c <= 'f' ) return c - 'a' + 10;
	if( c >= 'A' && c <= 'F' ) return c - 'A' + 10;
	return -1;
}

//////////////////////////////////////////////////////////////////////////////
int32_t sg::GetIntParam( const char* &input, bool decimal, bool trimstart, bool acceptneg )
{
	int32_t	retval(0);
	char	converted;
	bool	found(false);
	bool	negative(false);

	if( trimstart )
		while( *input && FromChar( *input, decimal ) == (char)-1 && *input != '-' )
			++input;

	while( *input ) {
		if(( converted = FromChar( *input, decimal )) == (char)-1) {
			if( ! retval ) {
				if( *input == 'x' || *input == 'X' ) {
					decimal = false;
					converted = 0;
				} else if( *input == '-' ) {
					negative = true;
					converted = 0;
				} else break;
			} else break;
		}
		retval *=  decimal ? 10 : 16;
		retval += converted;
		found = true;
		++input;
	}
	return found ? (negative ? 0 - retval : retval ) : (acceptneg ? INT32_MIN : -1);
}





