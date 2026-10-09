//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-11
//      File_base        : DebugLogDevice
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 调试用日志设备（直接输出到控制台）
//
//////////////////////////////////////////////////////////////////////

#ifndef _DEBUG_LOG_DEVICE_H_
#define _DEBUG_LOG_DEVICE_H_

#include "ILogDevice.h"

//调试用日志设备
class DebugLogDevice : public ILogDevice
{
public:
	DebugLogDevice();
	~DebugLogDevice();

	//写日志（继承自ILogDevice）
	void WriteLog(const LogEventParam& logEventParam);

	//写系统调试日志（继承自ILogDevice）
	void SysDbgLog(const char* pLogData, unsigned int size, SysDbgLogEvent logEvent);
};

#endif// _DEBUG_LOG_DEVICE_H_