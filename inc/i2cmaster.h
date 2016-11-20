#ifndef _I2CMASTER_H_
#define _I2CMASTER_H_
#include <stm32_hal.h>
#include <inttypes.h>

class I2cMaster
{
public:
	typedef HAL_StatusTypeDef Status;

	I2cMaster(I2C_HandleTypeDef *hi2c) : m_hi2c(hi2c) {}
	inline Status Write(uint8_t data, const uint16_t address);
	inline Status Read(uint8_t &data, const uint16_t address);

protected:
	I2C_HandleTypeDef	*m_hi2c;
};

inline I2cMaster::Status I2cMaster::Write(uint8_t data, const uint16_t address)
{
	return HAL_I2C_Master_Transmit(m_hi2c, address, &data, 1, HAL_MAX_DELAY);
}

inline I2cMaster::Status I2cMaster::Read(uint8_t &data, const uint16_t address)
{
	return HAL_I2C_Master_Receive(m_hi2c, address, &data, 1, HAL_MAX_DELAY);
}

#endif	//	_I2CMASTER_H_
