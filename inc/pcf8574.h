//
//    FILE: PCF8574.H
//  AUTHOR: Rob Tillaart
//    DATE: 02-febr-2013
// VERSION: 0.1.04
// PURPOSE: I2C PCF8574 library for Arduino
//     URL:
//
// HISTORY:
// see PCF8574.cpp file
//

#if !defined(_PCF8574_H)// && defined(HAVE_I2C) && defined(USE_I2C)
#define _PCF8574_H
#include <stdint.h>
#include <stm32_hal.h>
#include <i2cmaster.h>

class Pcf8574
{
public:
	typedef HAL_StatusTypeDef Status;

	Pcf8574(I2cMaster &i2c, uint8_t i2cAddress, uint8_t initialData = 0xff);

	Status	Read(uint8_t &value);
	Status	Read(uint8_t pin, bool &value);

	operator		uint8_t() const { return m_data; }
	inline uint8_t	GetData() const { return m_data; }

	Status	Write(uint8_t value);
	Status	Write(uint8_t pin, bool value);

	Status	Toggle(uint8_t pin);
	Status	ShiftRight(uint8_t n = 1);
	Status	ShiftLeft(uint8_t n = 1);

protected:
	I2cMaster			&m_i2c;
	uint16_t			m_i2cAddress;
	uint8_t				m_data;
	I2cMaster::Mode		m_mode = I2cMaster::Interrupt;
};

#endif
//
// END OF FILE
//
