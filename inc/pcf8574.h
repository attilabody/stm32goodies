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
#ifdef STM32F0
#include "stm32f0xx_nucleo_32.h"
#elif defined(STM32F1)
#include "stm32f1xx_hal.h"
#endif


class Pcf8574
{
public:
	Pcf8574(I2C_HandleTypeDef *hi2c, uint8_t i2cAddress, uint8_t initialData = 0xff);

	HAL_StatusTypeDef	Read(uint8_t &value);
	HAL_StatusTypeDef	Read(uint8_t pin, bool &value);

	operator			uint8_t() const { return m_data; }
	inline uint8_t		GetData() const { return m_data; }

	HAL_StatusTypeDef	Write(uint8_t value);
	HAL_StatusTypeDef	Write(uint8_t pin, bool value);

	HAL_StatusTypeDef	Toggle(uint8_t pin);
	HAL_StatusTypeDef	ShiftRight(uint8_t n = 1);
	HAL_StatusTypeDef	ShiftLeft(uint8_t n = 1);

protected:
	uint8_t				m_i2cAddress;
	uint8_t				m_data;

	inline HAL_StatusTypeDef Read();
	inline HAL_StatusTypeDef Write();

private:
	I2C_HandleTypeDef	*m_hi2c;
};

inline HAL_StatusTypeDef Pcf8574::Write()
{
	return HAL_I2C_Master_Transmit(m_hi2c, m_i2cAddress, &m_data, 1, HAL_MAX_DELAY);
}

inline HAL_StatusTypeDef Pcf8574::Read()
{
	return HAL_I2C_Master_Receive(m_hi2c, m_i2cAddress, &m_data, 1, HAL_MAX_DELAY);
}

#endif
//
// END OF FILE
//
