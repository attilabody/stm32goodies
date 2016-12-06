/*
 * dbgserial.cpp
 *
 *  Created on: Dec 6, 2016
 *      Author: compi
 */

#include <dbgserial.h>

////////////////////////////////////////////////////////////////////
size_t DbgSerial::FillTxBuffer(char *buffer, size_t count)
{
	size_t  copied = 0;

	while(m_txCount < m_size && count--) {
		uint16_t pos = m_txStart + m_txCount++;

		if(pos >= m_size) {
			pos -= m_size;
		}
		m_buffer[pos] = *buffer++;
		++copied;
	}

	return copied;
}




