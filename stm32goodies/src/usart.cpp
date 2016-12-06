/*
 * dbgserial.cpp
 *
 *  Created on: Dec 6, 2016
 *      Author: compi
 */

#include <usart.h>

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
size_t DbgUsart::FillTxBuffer(char *buffer, size_t count)
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

////////////////////////////////////////////////////////////////////
bool DbgUsart::UsartCallback(UART_HandleTypeDef *huart, CallbackType type)
{
	if(huart != m_huart)
		return false;
	if(m_txPending && type == CallbackType::TxCpltCallback) {
		//TODO: check if we can send another chunk of data
		m_txPending = false;
	}
	return true;
}

////////////////////////////////////////////////////////////////////
size_t  DbgUsart::Send(void *buffer, size_t count)
{
	size_t  sent = 0, copied;

	while(count) {
		while(m_txCount == sizeof(m_buffer))
			if(!m_block)
				return sent;

		__HAL_UART_DISABLE_IT(m_huart, UART_IT_TXE);
		copied = FillTxBuffer((char *)buffer, count);
//TODO: start transmitting if it's not in progress already
//			if(!(_UCSRB & _BV(_UDRIE)))
//				_UCSRB |= _BV(_UDRIE);
		__HAL_UART_ENABLE_IT(m_huart, UART_IT_TXE);
		buffer = (unsigned char *)buffer + copied;
		count -= copied;
		sent += copied;
	}

	return sent;
}



