//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-11
//      File_base        : LogSystem
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 日志系统
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "LogSystem.h"
#include "DbLogDevice.h"
#include "DebugLogDevice.h"
#include "ConfigManager.h"

int CreateLogSystem(ILogSystem* & pLogSystem)
{
	LogSystem* pLog = new LogSystem;
	ConfigManager& cm = ConfigManager::Singleton();

	if (TRUE == cm.GetGlobalVariable(global_var_log_device_db))
	{
		DbLogDevice* pDbLogDevice = new DbLogDevice;
		if (!pLog->AddLogDevice(pDbLogDevice))
		{
			_ASSERT(false);
			delete pDbLogDevice;
		}
	}	

	if (TRUE == cm.GetGlobalVariable(global_var_log_device_debug))
	{
		DebugLogDevice* pDebugLogDevice = new DebugLogDevice;
		if (!pLog->AddLogDevice(pDebugLogDevice))
		{
			_ASSERT(false);
			delete pDebugLogDevice;
		}
	}
	
	pLogSystem = pLog;
	return TRUE;
}

void ReleaseLogSystem(ILogSystem* & pLogSystem)
{
	if (pLogSystem != NULL)
	{
		delete pLogSystem;
		pLogSystem = NULL;
	}
}

LogSystem::LogSystem()
{
	m_LogDeviceList.clear();
}

LogSystem::~LogSystem()
{
	RemoveAllLogDevice();
}

void LogSystem::Log(const LogEventParam& logEventParam)
{
	//参数检查
	if (logEventParam.event <= log_event_invalid
		|| logEventParam.event >= log_event_count
		|| logEventParam.param1.data[sizeof(logEventParam.param1.data) - 1] != 0
		|| logEventParam.param2.data[sizeof(logEventParam.param2.data) - 1] != 0
		|| logEventParam.param3.data[sizeof(logEventParam.param3.data) - 1] != 0
		|| logEventParam.comment[MAX_LOG_COMMENT_LENGTH - 1] != 0)
	{
		_ASSERT(false);
		return;
	}

	LogDeviceList::iterator currentDevice = m_LogDeviceList.begin();
	LogDeviceList::iterator endDevice = m_LogDeviceList.end();
	while (currentDevice != endDevice)
	{	
		(*currentDevice)->WriteLog(logEventParam);
		++currentDevice;
	}
}

bool LogSystem::AddLogDevice(ILogDevice* pLogDevice)
{
	if (pLogDevice != NULL)
	{
		m_LogDeviceList.insert(m_LogDeviceList.end(), pLogDevice);
		return true;
	}
	
	return false;
}

void LogSystem::RemoveAllLogDevice()
{
	LogDeviceList::iterator currentDevice = m_LogDeviceList.begin();
	LogDeviceList::iterator endDevice = m_LogDeviceList.end();
	while (currentDevice != endDevice)
	{
		delete (*currentDevice);
		++currentDevice;
	}

	m_LogDeviceList.clear();
}

void LogSystem::SysDbgLog(const char* pLogData, unsigned int size, SysDbgLogEvent logEvent)
{
	LogDeviceList::iterator currentDevice = m_LogDeviceList.begin();
	LogDeviceList::iterator endDevice = m_LogDeviceList.end();
	while (currentDevice != endDevice)
	{	
		(*currentDevice)->SysDbgLog(pLogData, size, logEvent);
		++currentDevice;
	}
}
