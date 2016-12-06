/*
 * dbgserial.h
 *
 *  Created on: Dec 6, 2016
 *      Author: compi
 */

#ifndef DBGSERIAL_H_
#define DBGSERIAL_H_

#include <stddef.h>
#include <inttypes.h>


class DbgSerial {
public:
	DbgSerial(const DbgSerial &) = delete;
	DbgSerial(DbgSerial &&) = delete;
	DbgSerial &operator=(const DbgSerial &) = delete;

	void Init(uint8_t *buffer, uint16_t size, bool block);
	//DbgSerial& Instance() { return m_instance; }
	void SetBlock(bool block) { m_block = block; }

	size_t Send(void *buffer, size_t count);
	size_t Send(char c);
	size_t Send(bool b);
	size_t Send(uint32_t l, bool hex = false, bool prefix = true, bool zeroes = false);
	size_t Send(uint16_t i, bool hex = false, bool prefix = true, bool zeroes = false);
	size_t Send(uint8_t i, bool hex = false, bool prefix = true);
	size_t Send(const char *str);

private:
	DbgSerial() = default;
	size_t FillTxBuffer(char *buffer, size_t count);

	static DbgSerial	m_instance;

	uint8_t				*m_buffer = nullptr;
	uint16_t			m_size = 0;
	bool				m_block = true;

	volatile uint16_t m_txStart = 0;
	volatile uint16_t m_txCount = 0;
};

#endif /* DBGSERIAL_H_ */
