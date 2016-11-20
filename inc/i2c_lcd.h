/*
 * i2c_lcd.h
 *
 *  Created on: Jun 20, 2016
 *      Author: compi
 */
#if !defined(I2C_LCD_H_)// && defined(HAVE_I2C) && defined(USE_I2C)
#define I2C_LCD_H_

#include <stm32_hal.h>
#include <i2cmaster.h>

class I2cLcd : public I2cMaster
{
public:
	I2cLcd(I2C_HandleTypeDef *hi2c, uint16_t i2cAddress);
	Status	Init();
	Status	Clear();
	Status	Home();
	Status	SetCursor(uint8_t x, uint8_t y);
	Status	Print(const char* str);
	size_t	Print(unsigned int i, bool hex = false);

private:
	inline Status SendData();
	inline Status Epulse();
	inline Status SendByte(uint8_t b, bool isCmd);
	inline Status SendNibble(uint8_t nibble);

	uint8_t				m_data;
	uint16_t			m_i2cAddress;

	static const uint8_t m_rowOffsets[4];
	static const uint8_t m_init[5];
};

#endif /* I2C_LCD_H_ */
