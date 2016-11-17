/*
 * i2c_lcd.c
 *
 *  Created on: Jun 20, 2016
 *      Author: compi
 */

//#include "config.h"
//#if defined(HAVE_I2C) && defined(USE_I2C)
#include "stm32f0xx_hal.h"
#include <string.h>
//#include <util/delay.h>

#include "i2c_lcd.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0b00000100  // Enable bit
#define Rw 0b00000010  // Read/Write bit
#define Rs 0b00000001  // Register select bit

//////////////////////////////////////////////////////////////////////////////
inline void revstr(char *first, char *last)
{
	char tmp;
	while(last > first) {
		tmp = *first;
		*first++ = *last;
		*last-- = tmp;
	}
}

//////////////////////////////////////////////////////////////////////////////
inline size_t uitodec( char* buffer, unsigned int data)
{
	char *b2 = buffer;
	if(!data) {
		*b2++ = '0';
		*b2 = '\0';
		return 1;
	}

	while(data) {
		*b2++ = (data % 10) + '0';
		data /= 10;
	}
	size_t ret = b2 - buffer;

	*b2-- = 0;

	revstr(buffer, b2);
    return ret;
}

//////////////////////////////////////////////////////////////////////////////
inline size_t uitohex( char* buffer, unsigned int data)
{
	char *b2 = buffer;
	if(!data) {
		*b2++ = '0';
		*b2 = '\0';
		return 1;
	}

	while(data) {
		uint8_t curval = data & 0x0f;
		*b2++ = curval + '0' + (curval < 10) ? 0 : 'A' - '0';
		data /= 10;
	}
	size_t ret = b2 - buffer;

	*b2-- = 0;

	revstr(buffer, b2);
    return ret;
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
const uint8_t I2cLcd::m_rowOffsets[4] =  { 0x00, 0x40, 0x14, 0x54 };

const uint8_t I2cLcd::m_init[5] = {
	0x28,
	0x01,	//clear display
	0x06,	//increment mode, no display shift
	0x0C,	//display on, hide cursor, not blinking
};

//////////////////////////////////////////////////////////////////////////////
I2cLcd::I2cLcd(I2C_HandleTypeDef *hi2c, uint8_t i2cAddress)
: m_data(LCD_BACKLIGHT)
, m_i2cAddress(i2cAddress)
, m_hi2c(hi2c)
{
}

//////////////////////////////////////////////////////////////////////////////
// 200 us @ 100kHz
inline HAL_StatusTypeDef I2cLcd::SendData()
{
	return HAL_I2C_Master_Transmit(m_hi2c, m_i2cAddress, &m_data, 1, HAL_MAX_DELAY);
}

//////////////////////////////////////////////////////////////////////////////
// 400 us @ 100kHz
inline HAL_StatusTypeDef I2cLcd::Epulse()
{
	HAL_StatusTypeDef	ret;
	m_data |= En;
	ret = SendData();
	m_data &= ~En;
	if(ret == HAL_OK)
		return SendData();
	return ret;
}

//////////////////////////////////////////////////////////////////////////////
// 600 us @ 100kHz
inline HAL_StatusTypeDef I2cLcd::SendNibble(uint8_t nibble)
{
	HAL_StatusTypeDef ret;
	m_data = ((m_data & 0x0f) | (nibble & 0xf0));
	ret = SendData();
	if(ret == HAL_OK)
		return Epulse();
	return ret;
}

//////////////////////////////////////////////////////////////////////////////
inline HAL_StatusTypeDef I2cLcd::SendByte(uint8_t b, bool isCmd)
{
	HAL_StatusTypeDef ret;

	m_data = ((m_data & 0x0f & ~Rs) | (b & 0xf0) | (isCmd ? 0 : Rs));
	ret = SendData();
	if(ret == HAL_OK) {
		ret = Epulse();
		if(ret == HAL_OK) {
			m_data = ((m_data & 0x0f & ~Rs) | ((b & 0x0f) << 4) | (isCmd ? 0 : Rs));
			ret = SendData();
			if(ret == HAL_OK)
				return Epulse();
		}
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef I2cLcd::Init()
{
	HAL_StatusTypeDef ret;
	uint8_t count;

	m_data = LCD_BACKLIGHT;
	ret = SendData();
	if(ret == HAL_OK) {
		HAL_Delay(500);
		ret = SendNibble(0x30);
		if(ret == HAL_OK) {
			HAL_Delay(5);
			ret = SendNibble(0x30);
			if(ret == HAL_OK) {
				ret = SendNibble(0x30);
				if(ret == HAL_OK) {
					ret = SendNibble(0x20);
					if(ret == HAL_OK)
						for (count = 0; count < sizeof(m_init); count++) {
							ret = SendByte(m_init[count], true);
							if(ret != HAL_OK)
								return ret;
							HAL_Delay(3);
						}
	}}}}
	return ret;
}

//////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef I2cLcd::Clear()
{
	HAL_StatusTypeDef ret = SendByte(LCD_CLEARDISPLAY, true);	// clear display, set cursor position to zero
	HAL_Delay(3);												// this command takes a long time!
	return ret;
}

//////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef I2cLcd::Home()
{
	HAL_StatusTypeDef ret = SendByte(LCD_RETURNHOME, true);		// set cursor position to zero
	HAL_Delay(2);												// this command takes a long time!
	return ret;
}

//////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef I2cLcd::SetCursor(uint8_t x, uint8_t y)
{
	return SendByte(LCD_SETDDRAMADDR | (x + m_rowOffsets[y & 3]), true);
}

//////////////////////////////////////////////////////////////////////////////
HAL_StatusTypeDef I2cLcd::Print(const char *str)
{
	HAL_StatusTypeDef ret = HAL_OK;
	while(*str) {
		ret = SendByte(*str++, false);
		if(ret != HAL_OK)
			return ret;
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////////
size_t I2cLcd::Print(unsigned int u, bool hex)
{
	char	buffer[11];
	size_t ret = uitodec(buffer, u);
	Print(buffer);
	return ret;
}


//#endif	//	#if defined(HAVE_I2C) && defined(USE_I2C)
