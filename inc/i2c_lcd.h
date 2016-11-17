/*
 * i2c_lcd.h
 *
 *  Created on: Jun 20, 2016
 *      Author: compi
 */
#if !defined(I2C_LCD_H_)// && defined(HAVE_I2C) && defined(USE_I2C)
#define I2C_LCD_H_

class I2cLcd
{
public:
	I2cLcd(I2C_HandleTypeDef *hi2c, uint8_t i2cAddress);
	HAL_StatusTypeDef	Init();
	HAL_StatusTypeDef	Clear();
	HAL_StatusTypeDef	Home();
	HAL_StatusTypeDef	SetCursor(uint8_t x, uint8_t y);
	HAL_StatusTypeDef	Print(const char* str);
	size_t	Print(int i, bool hex = false);

private:
	inline HAL_StatusTypeDef SendData();
	inline HAL_StatusTypeDef Epulse();
	inline HAL_StatusTypeDef SendByte(uint8_t b, bool isCmd);
	inline HAL_StatusTypeDef SendNibble(uint8_t nibble);

	uint8_t				m_data;
	uint8_t				m_i2cAddress;
	I2C_HandleTypeDef	*m_hi2c;

	static const uint8_t m_rowOffsets[4];
	static const uint8_t m_init[5];
};

#endif /* I2C_LCD_H_ */
