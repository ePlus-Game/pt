//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 03/22/2008 12:14
//      File_base        : IBLog
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "IBLog.h"
#include "KPlayer.h"
#include "KItemGenerator.h"

KIBLog& KIBLog::getSingleton( void )
{
	static KIBLog s_singleten;
	return s_singleten;
}

//添加（购买）IB物品：
void KIBLog::AddIBItem( int type, const FSGUID& ibItemGuid, int playerIndex, int itemHashId, int itemLevel, int price )
{
	if ( !IsOkIBLogType( type ) )
	{
		return;
	}

	if ( !IsValidPlayer( playerIndex ) )
	{
		return;
	}

	int nGenre		= 0;
	int nDetail		= 0;
	int nParticular	= 0;
	itemLevel = 0;

	SpliteHashId( itemHashId, nGenre, nDetail, nParticular );

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IB_Log_AddIbItem;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if ( pParam && g_pController )
	{
		pParam->BeginPush( PN_IB_ADDIBITEM );
		pParam->Push( (char*)&ibItemGuid );
		pParam->Push( nGenre );
		pParam->Push( nDetail );
		pParam->Push( nParticular );
		pParam->Push( itemLevel );
		pParam->Push( Player[playerIndex].GetPlayerName() );
		pParam->Push( type );
		pParam->Push( price );
		pParam->Push( Player[playerIndex].GetSeries() );
		pParam->Push( Player[playerIndex].GetSkillSeries() );
		pParam->Push( Player[playerIndex].GetLevel() );
		pParam->Push( Player[playerIndex].m_dwLastLoginIP );
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_log, pParam );
	}

	LogIbItem( type, ibItemGuid, playerIndex, NULL );

	int commonLogType = IBLogToCommonLog( type );
	if ( commonLogType != log_event_count )
	{
		LogEventParam logEventParam;
		logEventParam.event = (LogEvent)commonLogType;
		logEventParam.param1 = Player[playerIndex].GetGUID();
		logEventParam.param2 = ibItemGuid;
		logEventParam.param4 = price;
		g_pLogSystem->Log(logEventParam);
	}
}

//转移（交易）IB物品：
void KIBLog::TransferIBItem( int type, const FSGUID& ibItemGuid, int playerIndex, int newOwerIndex )
{
	if ( !IsOkIBLogType( type ) )
	{
		return;
	}

	if ( !IsValidPlayer( playerIndex ) )
	{
		return;
	}

	if ( !IsValidPlayer( newOwerIndex ) )
	{
		return;
	}

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IB_Log_TransferIbItem;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if ( pParam && g_pController )
	{
		pParam->BeginPush( PN_IB_TRANSFERIBITEM );
		pParam->Push( (char*)&ibItemGuid );
		pParam->Push( Player[newOwerIndex].GetPlayerName() );
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_log, pParam );
	}
	LogIbItem( type, ibItemGuid, playerIndex, newOwerIndex );
}

//转移（交易）IB物品：
void KIBLog::TransferIBItem( int type, const FSGUID& ibItemGuid, int newOwerIndex )
{
	if ( !IsOkIBLogType( type ) )
	{
		return;
	}

	if ( !IsValidPlayer( newOwerIndex ) )
	{
		return;
	}

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IB_Log_TransferIbItem;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if ( pParam && g_pController )
	{
		pParam->BeginPush( PN_IB_TRANSFERIBITEM );
		pParam->Push( (char*)&ibItemGuid );
		pParam->Push( Player[newOwerIndex].GetPlayerName() );
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_log, pParam );
	}
}

//删除IB物品：
void KIBLog::DelIBItem( int type, const FSGUID& ibItemGuid, int playerIndex )
{
	if ( !IsOkIBLogType( type ) )
	{
		return;
	}

	if ( !IsValidPlayer( playerIndex ) )
	{
		return;
	}

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IB_Log_DelIbItem;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if ( pParam && g_pController )
	{
		pParam->BeginPush( PN_IB_DELIBITEM );
		pParam->Push( (char*)&ibItemGuid );
		pParam->Push( type );
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_log, pParam );
	}

	LogIbItem( type, ibItemGuid, playerIndex, NULL );
}

	//充值IB货币
void KIBLog::AddIBMoney( int type,int playerIndex, int price ,const FSGUID * pGUID /* = NULL */)
{
	if ( !IsOkIBLogType( type ) )
	{
		return;
	}

	if ( !IsValidPlayer( playerIndex ) )
	{
		return;
	}

	int commonLogType = IBLogToCommonLog( type );
	if ( commonLogType != log_event_count )
	{
		LogEventParam logEventParam;
		logEventParam.event = (LogEvent)commonLogType;
		logEventParam.param1 = Player[playerIndex].GetGUID();
		
		if (pGUID != NULL)
			logEventParam.param2 = *pGUID;
	
		logEventParam.param4 = price;
		g_pLogSystem->Log(logEventParam);
	}
}


//记录IB物品行为：
void KIBLog::LogIbItem( int type, const FSGUID& ibItemGuid, int playerIndex, int newOwerIndex )
{
	//Lucifer~yu(zhangjianyu) 03/23/2008 Modify
	//Begin-------------------------------------------------------------------
	return;	
	//End---------------------------------------------------------------------

	if ( !IsOkIBLogType( type ) )
	{
		return;
	}

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_IB_Log_LogIbItem;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if ( pParam && g_pController )
	{
		pParam->BeginPush( PN_IB_LOGIBITEM );
		pParam->Push( (char*)&ibItemGuid );
		pParam->Push( type );
		//player A
		if ( IsValidPlayer( playerIndex ) )
		{
			pParam->Push( Player[playerIndex].GetPlayerName() );
			pParam->Push( Player[playerIndex].GetSeries() );
			pParam->Push( Player[playerIndex].GetSkillSeries() );
			pParam->Push( Player[playerIndex].GetLevel() );
			pParam->Push( Player[playerIndex].m_dwLastLoginIP );
		}
		//player B
		if ( IsValidPlayer( newOwerIndex ) )
		{
			pParam->Push( Player[playerIndex].GetPlayerName() );
			pParam->Push( Player[playerIndex].GetSeries() );
			pParam->Push( Player[playerIndex].GetSkillSeries() );
			pParam->Push( Player[playerIndex].GetLevel() );
			pParam->Push( Player[playerIndex].m_dwLastLoginIP );
		}
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_log, pParam );
	}
}

bool KIBLog::IsOkIBLogType( int type )
{
	if ( type < 0 || type >= iblogtype_count)
	{
		return false;
	}
	return true;
}

int KIBLog::IBLogToCommonLog( int type )
{
	int rettype = log_event_count;
	switch( type )
	{
	case jinshanbi_onetime_buy:
	case jinshanbi_buy:
		rettype = log_event_jinshanbi_dec;
		break;
	case creditpoint_buy:
		rettype = log_event_creditpoint_add;
		break;
	case creditpoint_add:
		rettype = log_event_creditpoint_dec;
		break;
	case point_add:
		rettype = log_event_point_add;
		break;
	case point_buy:
		rettype = log_event_point_dec;
		break;
	case card_add:
		rettype = log_event_ticket_add;
		break;
	case card_buy:
		rettype = log_event_ticket_dec;
		break;
	case recommender_reward_card:
		rettype = log_event_recommend_reward_add_ticket;
		break;
	default:
		break;
	}
	return rettype;
}