//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-11
//      File_base        : DebugLogDevice
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 调试用日志设备（直接输出到控制台）
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "DebugLogDevice.h"

DebugLogDevice::DebugLogDevice()
{
}

DebugLogDevice::~DebugLogDevice()
{
}

void DebugLogDevice::WriteLog(const LogEventParam& logEventParam)
{	
	printf("LogEvent: event=%d", logEventParam.event);
	if (strlen(logEventParam.comment) > 0)
	{
		printf(", comment=\"%s\"", logEventParam.comment);
	}

	printf("\n");
}

void DebugLogDevice::SysDbgLog(const char* pLogData, unsigned int size, SysDbgLogEvent logEvent)
{
	printf("SysDbgLogEvent: event=%d", logEvent);
	if (pLogData && size > 0)
	{
		printf(", data=\"%s\"", pLogData);
	}

	printf("\n");
}