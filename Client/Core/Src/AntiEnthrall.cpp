//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:5:9   15:15
//      File_base        : AntiEnthrall
//      File_ext         : cpp
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "AntiEnthrall.h"
#include "ConfigManager.h"
#include "CoreRelated.h"
#include "KSubWorldSet.h"

void AntiEnthrall::PlayerOnline(DWORD dwEnthrallOnlineTime, DWORD dwFlag)
{
	m_TotalOnlineTime = dwEnthrallOnlineTime;
	m_PreStatTime = g_SubWorldSet.GetGameTime();
	m_StatEnthrallFlag = dwFlag;
	m_EnforceState     = enAntiEnthrall_InValid;
}

void AntiEnthrall::SetEnforceState(DWORD dwAntiState ,int nPlayerIndex)
{
	if (IsValidPlayer(nPlayerIndex) && dwAntiState >= enAntiEnthrall_InValid && dwAntiState <= enAntiEnthrall_Insalubrity )
	{
		m_EnforceState = dwAntiState;
		NotifyStateChange(nPlayerIndex);
	}//endif
}

int AntiEnthrall::GetCurState()
{
	DWORD dwRet = enAntiEnthrall_Normal;

	if( IsStatEnthrall() )
	{
		static ConfigManager &cfg = ConfigManager::Singleton();
		DWORD dwWearinessTime = cfg.GetGlobalVariable(global_var_antienthrall_wearinesstime);
		DWORD dwInsalubrityTime = cfg.GetGlobalVariable(global_var_antienthrall_insalubritytime);

		if(m_TotalOnlineTime < dwWearinessTime)
			dwRet = enAntiEnthrall_Normal;
		else if(m_TotalOnlineTime >= dwWearinessTime && m_TotalOnlineTime < dwInsalubrityTime)
			dwRet = enAntiEnthrall_Weariness;
		else
			dwRet = enAntiEnthrall_Insalubrity;
	}
	else
		dwRet = enAntiEnthrall_Normal;
	
	if (m_EnforceState > dwRet)
		return m_EnforceState;
	else
		return dwRet;	
}

void AntiEnthrall::NotifyStateChange(int nPlayerIdx)
{
	int nCurState = GetCurState();

	if(nCurState == enAntiEnthrall_Weariness)
		ChatErrCodeToClient(nPlayerIdx, chat_err_antienthralweariness);
	else if(nCurState == enAntiEnthrall_Insalubrity)
		ChatErrCodeToClient(nPlayerIdx, chat_err_antienthralinsalubrity);	
}

void AntiEnthrall::IncOnlineTime(int nPlayerIdx)
{
	if( IsStatEnthrall() )
	{
		DWORD dwCurTime = g_SubWorldSet.GetGameTime();

		m_TotalOnlineTime += (dwCurTime - m_PreStatTime) / GAME_FPS;
		m_PreStatTime = dwCurTime;

		NotifyStateChange(nPlayerIdx);
	}
}


