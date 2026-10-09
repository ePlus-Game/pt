//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-11
//      File_base        : LogSystem
//      File_ext         : .h
//      Author           : 徐晓刚
//      Description      : 日志系统
//
//////////////////////////////////////////////////////////////////////

#ifndef _LOG_SYSTEM_H_
#define _LOG_SYSTEM_H_

#include "ILogSystem.h"
#include "ILogDevice.h"
#include <list>

typedef std::list<ILogDevice*> LogDeviceList;

//日志系统
class LogSystem : public ILogSystem
{
public:
	LogSystem();
	~LogSystem();

	//记录日志（继承自ILogSystem）
	void Log(const LogEventParam& logEventParam);

	//系统调试日志
	void SysDbgLog(const char* pLogData, unsigned int size, SysDbgLogEvent logEvent = sys_dbg_log_event_common);

	//添加日志设备
	bool AddLogDevice(ILogDevice* pLogDevice);

private:
	//删除所有的日志设备
	void RemoveAllLogDevice();

	//日志设备列表
	LogDeviceList m_LogDeviceList;
};

#endif// _LOG_SYSTEM_H_