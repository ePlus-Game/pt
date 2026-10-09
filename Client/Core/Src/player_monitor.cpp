//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 2007-08-28
//      File_base        : player_monitor
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 玩家监视器
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KPlayer.h"
#include "cfs_fs2_savedef.h"
#include "player_monitor.h"

#define SPY_LEVEL_SETTING_FILE "\\settings\\spy_level.txt"		//监视等级设置文件

PlayerMonitor g_PlayerMonitor;

struct RecordPlayerActionReq
{
	FSGUID PlayerGuid;
	enumPlayerAction Action;
	char Desc[PLAYER_ACTION_DESC_MAX_LENGTH];
};

PlayerMonitor::PlayerMonitor()
{
}

PlayerMonitor::~PlayerMonitor()
{
}

bool PlayerMonitor::Load()
{
	KTabFile spyLevelFile;
	if (TRUE == spyLevelFile.Load(SPY_LEVEL_SETTING_FILE))
	{	
		memset(m_SpyAction, 0, sizeof(m_SpyAction));
		for (int level = 1; level <= MAX_SPY_LEVEL; ++level)
		{
			for (int action = player_action_start + 1; action < player_action_end; action++)
			{
				int row = action + 1;
				int col = level;
				BOOL spy = FALSE;
				spyLevelFile.GetInteger(row, col, 0, &spy);
				m_SpyAction[level][action] = (spy == TRUE);
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool PlayerMonitor::IsNeedRecord(int playerIndex, enumPlayerAction action) const
{
	return (IsValidPlayer(playerIndex)
		&& Player[playerIndex].GetSpyLevel() > 0
		&& action > player_action_start
		&& action < player_action_end
		&& m_SpyAction[Player[playerIndex].GetSpyLevel()][action]);
}

bool PlayerMonitor::IsNeedRecordC2SProtocol(int playerIndex) const
{
	return (IsValidPlayer(playerIndex) && Player[playerIndex].GetSpyLevel() >= MAX_SPY_LEVEL);
}

void PlayerMonitor::RecordPlayerAction(const RecordPlayerActionParam& param)
{
	if (IsValidPlayer(param.PlayerIndex) && g_pController)
	{
		KPlayer& player = Player[param.PlayerIndex];
		
		_DBProcHeader DBHeader;
		memset( &DBHeader, 0, sizeof(DBHeader) );
		
		DBHeader.ulNetID = -1;
		DBHeader.ProcType = Proc_RecPlayerAct;
		
		IProcParam* pParam = g_pController->GetProcParam( );
		if (pParam)
		{
			pParam->BeginPush( PN_RECPLAYERACTION );			
			pParam->Push( player.GetGUID().data );			
			pParam->Push( param.Action );			
			pParam->Push( param.Desc );		
			pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
			
			g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
		}
	}
}

void PlayerMonitor::RecordC2SProtocol(int playerIndex, BYTE protocol, BYTE* data, int size)
{
	if ( IsValidPlayer(playerIndex) && g_pController && data && size>0 )
	{
		KPlayer& player = Player[playerIndex];
		
		_DBProcHeader DBHeader;
		memset( &DBHeader, 0, sizeof(DBHeader) );
		
		DBHeader.ulNetID = -1;
		DBHeader.ProcType = Proc_RecordC2SProtocol;
		
		IProcParam* pParam = g_pController->GetProcParam( );
		if (pParam)
		{
			pParam->BeginPush( PN_RECORDPROTOCOL );
			pParam->Push( player.m_PlayerName );
			pParam->Push( player.m_AccoutName );
			pParam->Push( player.m_dwLastLoginIP );
			pParam->Push( protocol );
			pParam->Push( BinPair( data, size ) );
			pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
			
			g_pController->CallProc( cfs_db_cnn_log, pParam );
		}
	}
}