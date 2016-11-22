#ifndef _I2CMASTER_H_
#define _I2CMASTER_H_
#include <stm32_hal.h>
#include <inttypes.h>

extern "C" {
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
class I2cCallbackDispatcher
{
public:
	static I2cCallbackDispatcher& Instance() { return m_instance; }

	class II2cCallback
	{
	public:
		enum CallbackType
		{
			None,
			MasterTxCpltCallback,
			MasterRxCpltCallback,
			SlaveTxCpltCallback,
			SlaveRxCpltCallback,
			MemTxCpltCallback,
			MemRxCpltCallback,
			ErrorCallback
		};

	private:
		virtual void I2cCallback(I2C_HandleTypeDef *hi2c, CallbackType type) = 0;
		friend class I2cCallbackDispatcher;
	};

	void Register(II2cCallback *handler);
	void Callback(I2C_HandleTypeDef *hi2c, II2cCallback::CallbackType type);

private:
	I2cCallbackDispatcher() {};
	I2cCallbackDispatcher(const I2cCallbackDispatcher &rhs) = delete;

	static I2cCallbackDispatcher	m_instance;

	II2cCallback 					*m_handlers[2] {0, 0};
};

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
class I2cMaster : public I2cCallbackDispatcher::II2cCallback
{
public:
	typedef HAL_StatusTypeDef Status;

	I2cMaster(I2C_HandleTypeDef *hi2c);
	inline Status Write(uint8_t data, const uint16_t address);
	inline Status Read(uint8_t &data, const uint16_t address);

protected:
	virtual void I2cCallback(I2C_HandleTypeDef *hi2c, CallbackType type);
	//virtual ~I2cMaster();	//will never be destructed

	I2C_HandleTypeDef			*m_hi2c;
	volatile CallbackType		m_expectedCallback = None;

	enum Mode { Poll, Interrupt, Dma }	m_mode;
	inline void WaitCallback();
};

//////////////////////////////////////////////////////////////////////////////
inline I2cMaster::Status I2cMaster::Write(uint8_t data, const uint16_t address)
{
	return HAL_I2C_Master_Transmit(m_hi2c, address, &data, 1, HAL_MAX_DELAY);
}

//////////////////////////////////////////////////////////////////////////////
inline I2cMaster::Status I2cMaster::Read(uint8_t &data, const uint16_t address)
{
	return HAL_I2C_Master_Receive(m_hi2c, address, &data, 1, HAL_MAX_DELAY);
}

#endif	//	_I2CMASTER_H_
