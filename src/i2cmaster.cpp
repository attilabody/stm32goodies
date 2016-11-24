/*
 * i2cmaster.cpp
 *
 *  Created on: Nov 20, 2016
 *      Author: compi
 */

#include <i2cmaster.h>

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	I2cCallbackDispatcher::Instance().Callback(hi2c, I2cCallbackDispatcher::II2cCallback::MasterTxCpltCallback);
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	I2cCallbackDispatcher::Instance().Callback(hi2c, I2cCallbackDispatcher::II2cCallback::MasterRxCpltCallback);
}


void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	I2cCallbackDispatcher::Instance().Callback(hi2c, I2cCallbackDispatcher::II2cCallback::SlaveTxCpltCallback);
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	I2cCallbackDispatcher::Instance().Callback(hi2c, I2cCallbackDispatcher::II2cCallback::SlaveRxCpltCallback);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	I2cCallbackDispatcher::Instance().Callback(hi2c, I2cCallbackDispatcher::II2cCallback::MemTxCpltCallback);
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	I2cCallbackDispatcher::Instance().Callback(hi2c, I2cCallbackDispatcher::II2cCallback::MemRxCpltCallback);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	I2cCallbackDispatcher::Instance().Callback(hi2c, I2cCallbackDispatcher::II2cCallback::ErrorCallback);
}


//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
I2cCallbackDispatcher I2cCallbackDispatcher::m_instance;

//////////////////////////////////////////////////////////////////////////////
void I2cCallbackDispatcher::Register(II2cCallback *handler)
{
	if(!m_handlers[0])
		m_handlers[0] = handler;
	else if(!m_handlers[1])
		m_handlers[1] = handler;
}

//////////////////////////////////////////////////////////////////////////////
void I2cCallbackDispatcher::Callback(I2C_HandleTypeDef *hi2c, II2cCallback::CallbackType type)
{
	for(uint8_t u = 0; u < sizeof(m_handlers) / sizeof(m_handlers[0]); u++) {
		if(m_handlers[u] && m_handlers[u]->I2cCallback(hi2c, type))
			break;
	}
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
I2cMaster::I2cMaster(I2C_HandleTypeDef *hi2c, I2cCallbackDispatcher &disp)
: m_hi2c(hi2c)
{
	disp.Register(this);
}

//////////////////////////////////////////////////////////////////////////////
bool I2cMaster::I2cCallback(I2C_HandleTypeDef *hi2c, CallbackType type)
{
	if(hi2c != m_hi2c)
		return false;

	if(m_expectedCallback == type || type == ErrorCallback ) {
		m_expectedCallback = None;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
I2cMaster::Status I2cMaster::Write(const uint16_t i2cAddress, uint8_t *data, uint8_t size, Mode mode)
{
	WaitCallback();
	if(mode == Poll)
		return HAL_I2C_Master_Transmit(m_hi2c, i2cAddress, data, size, HAL_MAX_DELAY);
	else {
		m_expectedCallback = MasterTxCpltCallback;
		if(mode == Interrupt)
			return HAL_I2C_Master_Transmit_IT(m_hi2c, i2cAddress, data, size);
		else
			return HAL_I2C_Master_Transmit_DMA(m_hi2c, i2cAddress, data, size);
	}
}

//////////////////////////////////////////////////////////////////////////////
I2cMaster::Status I2cMaster::Read(const uint16_t i2cAddress, uint8_t *data, uint8_t size, Mode mode)
{
	WaitCallback();
	if(mode == Poll)
		return HAL_I2C_Master_Receive(m_hi2c, i2cAddress, data, size, HAL_MAX_DELAY);
	else {
		m_expectedCallback = MasterRxCpltCallback;
		if(mode == Interrupt)
			return HAL_I2C_Master_Receive_IT(m_hi2c, i2cAddress, data, size);
		else
			return HAL_I2C_Master_Receive_DMA(m_hi2c, i2cAddress, data, size);
	}
}

//////////////////////////////////////////////////////////////////////////////
I2cMaster::Status I2cMaster::WriteMem(const uint16_t i2cAddress, uint16_t memAddr, uint8_t memAddrSize, uint8_t *data, uint16_t size, Mode mode)
{

}

//////////////////////////////////////////////////////////////////////////////
I2cMaster::Status I2cMaster::ReadMem(const uint16_t i2cAddress, uint16_t memAddr, uint8_t memAddrSize, uint8_t *data, uint16_t size, Mode mode)
{
//	WaitCallback();
//	if(mode == Poll)
//		return HAL_I2C_Mem_Read(m_hi2c, address, &data, 1, HAL_MAX_DELAY);
//	else {
//		m_expectedCallback = MasterRxCpltCallback;
//		if(mode == Interrupt)
//			return HAL_I2C_Master_Receive_IT(m_hi2c, address, &data, 1);
//		else
//			return HAL_I2C_Master_Receive_DMA(m_hi2c, address, &data, 1);
//	}
//
}
