//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-11
//      File_base        : DbLogDevice
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 数据库日志设备（记录日志到游戏数据库）
//
//////////////////////////////////////////////////////////////////////

#ifndef _DB_LOG_DEVICE_H_
#define _DB_LOG_DEVICE_H_

#include "ILogDevice.h"

//数据库日志设备
class DbLogDevice : public ILogDevice
{
public:
	DbLogDevice();
	~DbLogDevice();

	//写日志（继承自ILogDevice）
	void WriteLog(const LogEventParam& logEventParam);

	//写系统调试日志（继承自ILogDevice）
	void SysDbgLog(const char* pLogData, unsigned int size, SysDbgLogEvent logEvent);

private:
};

#endif// _DB_LOG_DEVICE_H_