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


I2cCallbackDispatcher I2cCallbackDispatcher::m_instance;

void I2cCallbackDispatcher::Register(II2cCallback *handler)
{
	if(!m_handlers[0])
		m_handlers[0] = handler;
	else if(!m_handlers[1])
		m_handlers[1] = handler;
}

void I2cCallbackDispatcher::Callback(I2C_HandleTypeDef *hi2c, II2cCallback::CallbackType type)
{
	if(m_handlers[0])
		m_handlers[0]->I2cCallback(hi2c, type);
}

I2cMaster::I2cMaster(I2C_HandleTypeDef *hi2c)
: m_hi2c(hi2c)
{
	I2cCallbackDispatcher::Instance().Register(this);
}

void I2cMaster::I2cCallback(I2C_HandleTypeDef *hi2c, CallbackType type)
{
	if(m_expectedCallback == type || type == ErrorCallback ) {
		m_expectedCallback = None;
	}
}

inline void I2cMaster::WaitCallback()
{
	while(m_expectedCallback != None) {}
}
