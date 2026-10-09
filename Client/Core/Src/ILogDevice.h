//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-11
//      File_base        : ILogDevice
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 日志设备接口
//
//////////////////////////////////////////////////////////////////////

#ifndef _I_LOG_DEVICE_H_
#define _I_LOG_DEVICE_H_

#include "ILogSystem.h"

//日志设备接口
interface ILogDevice
{
	//写日志
	virtual void WriteLog(const LogEventParam& logEventParam) = 0;

	//写系统调试日志
	virtual void SysDbgLog(const char* pLogData, unsigned int size, SysDbgLogEvent logEvent) = 0;
};

#endif// _I_LOG_DEVICE_H_