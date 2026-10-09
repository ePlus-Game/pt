//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-12-11
//      File_base        : DbLogDevice
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 数据库日志设备（记录日志到游戏数据库）
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "DbLogDevice.h"
#include "cfs_common_def.h"
#include "cfs_fs2_savedef.h"
#include "cfs_db_interface.h"

DbLogDevice::DbLogDevice()
{
}

DbLogDevice::~DbLogDevice()
{
}

void DbLogDevice::WriteLog(const LogEventParam& logEventParam)
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_WriteLog;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_WRITELOG );
	
	pParam->Push( logEventParam.event );
	//pParam->Push( logEventParam.comment );
	pParam->Push( logEventParam.param1.data );
	pParam->Push( logEventParam.param2.data );
	pParam->Push( logEventParam.param3.data );
	pParam->Push( logEventParam.param4 );

	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_log, pParam );

}

void DbLogDevice::SysDbgLog(const char* pLogData, unsigned int size, SysDbgLogEvent logEvent)
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_WriteSysDbgLog;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_SYSTEMLOG );
	
	pParam->Push( logEvent );

	if (pLogData == NULL || size == 0)
		pParam->Push( NullPair() );
	else
		pParam->Push( BinPair( (void*)pLogData, size ) );

	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_log, pParam );
}