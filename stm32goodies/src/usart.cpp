/*
 * dbgserial.cpp
 *
 *  Created on: Dec 6, 2016
 *      Author: compi
 */

#include <usart.h>
#include <stdlib.h>

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UsartCallbackDispatcher::Instance().Callback(huart, UsartCallbackDispatcher::IUsartCallback::TxCpltCallback);
}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart)
{

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

}
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{

}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{

}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
bool UsartCallbackDispatcher::Register(IUsartCallback *handler)
{
	decltype(handler)*	hp = nullptr;

	for( auto& h : m_handlers) {
		if( h == handler )
			return true;
		if(!hp && !h)
			hp = &h;
	}
	if(hp) {
		*hp = handler;
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
void UsartCallbackDispatcher::Callback(UART_HandleTypeDef *huart, IUsartCallback::CallbackType type)
{
	for(auto& handler : m_handlers) {
		if(handler && handler->UsartCallback(huart, type))
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
UsartCallbackDispatcher UsartCallbackDispatcher::m_instance;


////////////////////////////////////////////////////////////////////
bool DbgUsart::Init(UsartCallbackDispatcher &disp, UART_HandleTypeDef *huart, uint8_t *buffer, uint16_t size, bool block)
{
	if(m_buffer) {
		return false;
	}
	m_huart = huart;
	m_buffer = buffer;
	m_size = size;
	m_block = block;

	disp.Register(this);

	return true;
}

////////////////////////////////////////////////////////////////////
uint16_t DbgUsart::FillTxBuffer(uint8_t *buffer, uint16_t count)
{
	uint32_t			pm;
	HAL_StatusTypeDef	st;
	uint16_t   			tocopy, copied = 0;

	pm = __get_PRIMASK();
	__disable_irq();
	uint16_t freestart = m_txStart + m_txCount;
	//if(freestart >= m_size) freestart -= m_size;
	freestart -= (freestart >= m_size) ? m_size : 0;
	uint16_t free = m_size - m_txCount;
	if(count > free) count = free;
	tocopy = freestart + count > m_size ? m_size - freestart : count;
	if(!pm) __enable_irq();

	memcpy(m_buffer + freestart, buffer, tocopy);

	__disable_irq();
	if(!m_txCount) {
		m_chunkSize = tocopy;
		st = HAL_UART_Transmit_IT(m_huart, m_buffer + freestart, tocopy);
	}
	copied = tocopy;
	count -= tocopy;
	m_txCount += tocopy;
	if(!pm) __enable_irq();

	if(!count)
		return copied;

	buffer += tocopy;
	memcpy(m_buffer, buffer, count);

	__disable_irq();
	m_txCount += count;
	if(!pm) __enable_irq();

	return copied + count;
}

////////////////////////////////////////////////////////////////////
bool DbgUsart::UsartCallback(UART_HandleTypeDef *huart, CallbackType type)
{
	HAL_StatusTypeDef	st;

	if(huart != m_huart)
		return false;
	if(m_txCount && type == CallbackType::TxCpltCallback)
	{
		m_txCount -= m_chunkSize;
		m_txStart += m_chunkSize;
		if(m_txStart >= m_size)
			m_txStart -= m_size;
		m_chunkSize = m_txStart + m_txCount > m_size ? m_size - m_txStart : m_txCount;
		st = HAL_UART_Transmit_IT(m_huart, m_buffer + m_txStart, m_chunkSize);
	}
	return true;
}

////////////////////////////////////////////////////////////////////
uint16_t  DbgUsart::Send(void *buffer, uint16_t count)
{
	uint16_t  sent = 0, copied;

	while(count) {
		while(m_txCount == sizeof(m_buffer))
			if(!m_block)
				return sent;

		//__HAL_UART_DISABLE_IT(m_huart, UART_IT_TXE);
		copied = FillTxBuffer((uint8_t*)buffer, count);
		//__HAL_UART_ENABLE_IT(m_huart, UART_IT_TXE);
		buffer = (unsigned char *)buffer + copied;
		count -= copied;
		sent += copied;
	}

	return sent;
}



