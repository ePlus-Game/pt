//#include <objbase.h>
//#include <crtdbg.h>
#include "KCore.h"
#include "KPlayer.h"
#include "KNpc.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "GameDataDef.h"
#include "KProtocolProcess.h"
#ifdef _SERVER
#include "KSubWorldSet.h"
#include "CoreServerShell.h"
#endif
#include "KPlayerSet.h"
#include "Text.h"
#include "CoreUseNameDef.h"

#ifdef _SERVER
#include "ChatCenter_S.h"
#include "ServerSocialUnitMgr.h"
#endif

#define		PLAYER_FIRST_LUCKY					0
#define		PLAYER_NORML_PK_DEFAULT_TIME_LONG	3240

KPlayerSet PlayerSet;

KPlayerSet::KPlayerSet()
{
#ifdef _SERVER
	m_pWelcomeMsg = NULL;
	m_ulNextSaveTime = 0;
	m_ulDelayTimePerSave = 0;
	m_ulMaxSaveTimePerPlayer = 0;
	m_SocialSaveSwitch = 1;
#endif
}

BOOL	KPlayerSet::Init()
{
	int i;
	
#ifdef _SERVER
	m_nNumPlayer = 0;
	m_ulNextSaveTime = 0;
	m_ulMaxSaveTimePerPlayer = 60 * 20 * 15;
	m_ulDelayTimePerSave = m_ulMaxSaveTimePerPlayer / MAX_PLAYER;

	InitAutoSave();

	InitMoneyStatistic();
#endif

	// 优化查找表
	m_FreeIdx.Init(MAX_PLAYER);
	m_UseIdx.Init(MAX_PLAYER);

	// 开始时所有的数组元素都为空
	for (i = MAX_PLAYER - 1; i > 0; i--)
	{
		m_FreeIdx.Insert(i);
	}

	
	for (i = 0; i < MAX_PLAYER; i++)
	{
		Player[i].Release();
		Player[i].SetPlayerIndex(i);
#ifdef _SERVER
		Player[i].GetQuestionState().Init(i);
#endif
		Player[i].m_ItemList.Init(i);
		Player[i].m_Node.m_nIndex = i;
	}

#ifdef _SERVER

	KTabFile	cPKParam;

	memset(m_sPKPunishParam, 0, sizeof(m_sPKPunishParam));
	m_nNormalPKTimeLong = PLAYER_NORML_PK_DEFAULT_TIME_LONG;
	if (!cPKParam.Load(defPK_PUNISH_FILE))
		return FALSE;
	for (i = 0; i < MAX_DEATH_PUNISH_PK_VALUE + 1; i++)
	{
		cPKParam.GetInteger(i + 2, 1, 1, &m_sPKPunishParam[i].m_nPKValueScale);
		cPKParam.GetInteger(i + 2, 2, 1, &m_sPKPunishParam[i].m_dwExp);
		cPKParam.GetInteger(i + 2, 3, 1, &m_sPKPunishParam[i].m_nMoney);
		cPKParam.GetInteger(i + 2, 4, 1, &m_sPKPunishParam[i].m_nItem);
		cPKParam.GetInteger(i + 2, 5, 1, &m_sPKPunishParam[i].m_nEquip);
		//--> Rocker 2005/06/07
		cPKParam.GetInteger(i + 2, 6, 0, &m_sPKPunishParam[i].m_nBagLoseCount);
		cPKParam.GetInteger(i + 2, 7, 0, &m_sPKPunishParam[i].m_nBagLoseRate);
		cPKParam.GetInteger(i + 2, 8, 0, &m_sPKPunishParam[i].m_nEquipLoseCount);
		cPKParam.GetInteger(i + 2, 9, 0, &m_sPKPunishParam[i].m_nEquipLoseRate);
		//<-- End
	}
	cPKParam.GetInteger(2, 7, PLAYER_NORML_PK_DEFAULT_TIME_LONG, &m_nNormalPKTimeLong);

	//--> Rocker 2005/06/07
	m_DeathWeakParam.m_nConstantValue1 = 600;
	m_DeathWeakParam.m_nConstantValue2 = 30;
	m_DeathWeakParam.m_nWeakStartLevel = 30;
	m_DeathWeakParam.m_nScalePercent = 50;
	KIniFile ini;
	if (!ini.Load("\\settings\\npc\\player\\deathweak.ini"))
		return FALSE;
	ini.GetInteger("DeathWeak", "ConstantValue1", 600, &m_DeathWeakParam.m_nConstantValue1);
	ini.GetInteger("DeathWeak", "ConstantValue2", 30, &m_DeathWeakParam.m_nConstantValue2);
	ini.GetInteger("DeathWeak", "WeakStartLevel", 30, &m_DeathWeakParam.m_nWeakStartLevel);
	ini.GetInteger("DeathWeak", "WeakScalePercent", 50, &m_DeathWeakParam.m_nScalePercent);
	//<-- End
#endif

	// 帮会参数
	KIniFile	cTongFile;
	if (cTongFile.Load(defPLAYER_TONG_PARAM_FILE))
	{
		cTongFile.GetInteger("TongCreate", "Level", 60, &m_sTongParam.m_nLevel);
		cTongFile.GetInteger("TongCreate", "LeadLevel", 10, &m_sTongParam.m_nLeadLevel);
	}
	else
	{
		m_sTongParam.m_nLevel		= 50;
		m_sTongParam.m_nLeadLevel	= 20;
	}

	return TRUE;
}

int	KPlayerSet::FindFree()
{
	return m_FreeIdx.GetNext(0);
}

int KPlayerSet::FindSame(DWORD dwID)
{
	int nUseIdx = 0;

	nUseIdx = m_UseIdx.GetNext(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_dwID == dwID)
			return nUseIdx;
		nUseIdx = m_UseIdx.GetNext(nUseIdx);
	}
	return 0;
}


  /*__(@_    功能：根据NPC ID查找与这个NPC对应的Player
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
int     KPlayerSet::FindPlayerByNpcID(DWORD dwID)
{
	int nUseIdx = 0;

	nUseIdx = m_UseIdx.GetNext(0);
	while(nUseIdx)
	{
		if (Npc[Player[nUseIdx].m_nIndex].m_dwID == dwID)
			return nUseIdx;
		nUseIdx = m_UseIdx.GetNext(nUseIdx);
	}
	return 0;
}

int		KPlayerSet::GetFirstPlayer()
{
	m_nListCurIdx = m_UseIdx.GetNext(0);
	return m_nListCurIdx;
}

int		KPlayerSet::GetNextPlayer()
{
	if ( !m_nListCurIdx )
		return 0;
	m_nListCurIdx = m_UseIdx.GetNext(m_nListCurIdx);
	return m_nListCurIdx;
}

#ifdef _SERVER
int KPlayerSet::Add(LPSTR szPlayerID, void* pGuid)
{
	if (!pGuid || !szPlayerID || !szPlayerID[0])
		return 0;

	int i;
/*
	DWORD dwID = g_FileName2Id(szPlayerID);
	i = FindSame(dwID);
	if (i)
		return 0;
*/
	DWORD dwID = g_FileName2Id(szPlayerID);

	i = FindFree();

	if (i)
	{
		// Add by Cooler -->
		// 2005-3-24
		int nUseIdx = m_UseIdx.GetPrev(0);
		BOOL bCanAdd = TRUE;
		while(nUseIdx)
		{
			if (0 == strcmp(Player[nUseIdx].m_PlayerName, szPlayerID))
			{
				bCanAdd = FALSE;
				break;
			}

			nUseIdx = m_UseIdx.GetPrev(nUseIdx);
		}

		if(!bCanAdd)
		{
			return 0;
		}
		// End add by Cooler <--
		
		Player[i].m_dwID = dwID;
		Player[i].m_nNetConnectIdx = -1;
		Player[i].m_dwLoginTime = g_SubWorldSet.GetGameTime();
		memcpy(&Player[i].m_Guid, pGuid, sizeof(GUID));
		Player[i].SetPlayerIndex(i);
		Player[i].GetTeamInfo().Init(i);
		m_FreeIdx.Remove(i);
		m_UseIdx.Insert(i);
		m_nNumPlayer ++;
		return i;
	}
	return 0;
}

int	KPlayerSet::Broadcasting(char* pMessage, int nLen)
{
	if ( !pMessage || nLen <= 0 || nLen >= MAX_SENTENCE_LENGTH)
		return 0;
	if (!g_pServer)
		return 0;
	g_pServer->PreparePackSink();
	//KPlayerChat::SendSystemInfo(0, 0, MESSAGE_SYSTEM_ANNOUCE_HEAD, pMessage, nLen);
	g_pServer->SendPackToClient(-1);
	return 1;
}

void KPlayerSet::ReloadWelcomeMsg()
{
	return;
	/*    欢迎信息文本文件格式要求：
	每行为一条信息。  Msg:信息内容	
	控制信息内容的长度，使每条信息经过编码後长度不大于MAX_SENTENCE_LENGTH
	*/

	if (m_pWelcomeMsg)
	{
		free (m_pWelcomeMsg);
		m_pWelcomeMsg = NULL;
	}

	KFile		File;
	if (File.Open("\\Msg\\WelcomeMsg.txt") == FALSE)
		return;

	int nLen = File.Size();
	m_pWelcomeMsg = malloc(nLen + sizeof(int));
	if (m_pWelcomeMsg == NULL)
		return;

	*(int*)m_pWelcomeMsg = 0;
	char* pSearchBegin = (char*)m_pWelcomeMsg + sizeof(int);
	int* pLineHeader = (int*)pSearchBegin;
	File.Read(pLineHeader, nLen);
	File.Close();

	char* pMsgHeader;
	while(pMsgHeader = (char*)memchr(pSearchBegin, ':', nLen))
	{
		pMsgHeader++;	//跳过那个冒号
		nLen -= pMsgHeader -pSearchBegin; //冒号后的剩余长度
		if (nLen <= 0)
			break;
		int	nSkipLen = pMsgHeader - (char*)pLineHeader - sizeof(int);
		pSearchBegin = pMsgHeader;
		if (nSkipLen < 0)
			continue;
		if (nSkipLen > 0)
			memset(&pLineHeader[1], 0, nSkipLen);
		char* pMsgTail = (char*)memchr(pSearchBegin, 0x0d, nLen);	//查找行结尾
		int	nMsgLen;
		if (pMsgTail)
		{
			*pMsgTail = 0;
			pMsgTail++;
			if ((pMsgTail < pSearchBegin + nLen) && *pMsgTail == 0x0a)
			{
				*pMsgTail = 0;
				pMsgTail++;
			}
			nMsgLen = pMsgTail - pSearchBegin;
			pSearchBegin = pMsgTail;
		}
		else
		{
			nMsgLen = nLen;
		}
		nLen -= nMsgLen;
		nMsgLen = TEncodeText((char*)&pLineHeader[1], nMsgLen + nSkipLen);		
		if (nMsgLen > 0)
		{
			*pLineHeader = nMsgLen;
			pLineHeader = (int*)(((char*)pLineHeader) + sizeof(int) + (*pLineHeader));
			(*(int*)m_pWelcomeMsg) ++;
		}
		if (pMsgTail == NULL || nLen <= 0)
			break;
	}
	if ((int*)m_pWelcomeMsg == 0)
	{
		free(m_pWelcomeMsg);
		m_pWelcomeMsg = NULL;
	}
}

void KPlayerSet::PrepareRemove(int nIndex)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	// 通知聊天好友自己下线了
	// 如果组队，离开队伍
	Player[nIndex].GetTeamInfo().LeaveTeam();

	int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
	int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;

	//------------------------------------
	if (Player[nIndex].m_nDeathDecExp > 0)
	{
		Player[nIndex].m_dwLastDeathTime = UNIX_TMIE_STAMP;
	}

	//------------------------------------
	
	Player[nIndex].m_ItemList.RemoveAll();

	//------------------------------------
	
	Player[nIndex].WaitForRemove();
}

// 玩家离开服务器处理
void KPlayerSet::RemoveQuiting(int nIndex)
{
	if (Player[nIndex].m_nIndex > 0)	// have npc
	{
		int nRegion = Npc[Player[nIndex].m_nIndex].m_RegionIndex;
		int nSubWorld = Npc[Player[nIndex].m_nIndex].m_SubWorldIndex;
		
		if (nSubWorld >= 0 && nRegion >= 0)
		{
			SubWorld[nSubWorld].RemovePlayer(nRegion, nIndex);
			SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(Player[nIndex].m_nIndex);
		}	
		NpcSet.Remove(Player[nIndex].m_nIndex, TRUE);
	}
	
	Player[nIndex].m_dwID = 0;
	Player[nIndex].m_nIndex = 0;
	Player[nIndex].m_nNetConnectIdx = -1;
	Player[nIndex].Release();
	Player[nIndex].GetQuestionState().Init(nIndex);
	
	m_FreeIdx.Insert(nIndex);
	m_UseIdx.Remove(nIndex);
	m_nNumPlayer --;
}	
#endif

#ifdef _SERVER
void KPlayerSet::ProcessClientMessage(int nIndex, const char* pChar, int nSize)
{
	if (nIndex <= 0 || nIndex >= MAX_PLAYER)
		return;

	int i = Player[nIndex].m_nNetConnectIdx;

	if (i >= 0)
	{
		if ( Player[nIndex].m_dwID )
			g_ProtocolProcess.ProcessNetMsg(nIndex, (BYTE*)pChar, nSize);
	}
}

void KPlayerSet::ProcessPaysysMessage(int nIndex, const char* pChar, int nSize)
{
	if (nIndex <= 0 && nIndex >= MAX_PLAYER)
		return;
	
	int i = Player[nIndex].m_nNetConnectIdx;
	
	if (i >= 0)
	{
		if ( Player[nIndex].m_dwID )
			Player[nIndex].ProcessPaysys( pChar, nSize);
	}
}

#endif

#ifdef _SERVER
BOOL	KPlayerSet::GetPlayerName(int nIndex, char* szName)
{
	int i = nIndex;

	if (!szName)
		return FALSE;

	if (i <= 0 || i >= MAX_PLAYER)
	{
		szName[0] = 0;
		return FALSE;
	}
	strcpy(szName, Player[i].m_PlayerName);
	return TRUE;
}
#endif

#ifdef _SERVER
int		KPlayerSet::AttachPlayer(const unsigned long lnID, GUID* pGuid)
{
	if (lnID >= MAX_PLAYER || NULL == pGuid)
		return 0;

	int nUseIdx = m_UseIdx.GetPrev(0);
	while(nUseIdx)
	{
		if (Player[nUseIdx].m_nNetConnectIdx == -1)
		{
			if (0 == memcmp(&Player[nUseIdx].m_Guid, pGuid, sizeof(GUID)))
			{
				if( !Player[nUseIdx].m_bPermitAttach )
					return 0;

				Player[nUseIdx].m_nNetConnectIdx = lnID;
				Player[nUseIdx].m_ulLastSaveTime = g_SubWorldSet.m_nLoopRate;
				return nUseIdx;
			}
		}
		nUseIdx = m_UseIdx.GetPrev(nUseIdx);
	}
	return 0;
}
#endif

#ifdef _SERVER
int		KPlayerSet::GetPlayerIndexByGuid(GUID* pGuid)
{
	int nUseIdx = m_UseIdx.GetNext(0);
	while(nUseIdx)
	{
		if (0 == memcmp(&Player[nUseIdx].m_Guid, pGuid, sizeof(GUID)))
		{
			if (Player[nUseIdx].m_nNetConnectIdx != -1)
			{
				return nUseIdx;
			}
			else
			{
				g_DebugLog("[error]Find Guid to a disconnect player");
				return 0;
			}
		}
		nUseIdx = m_UseIdx.GetNext(nUseIdx);
	}
	return 0;
}

int		KPlayerSet::GetPlayerIndexByGuidOL(GUID* pGuid)
{
	int nUseIdx = m_UseIdx.GetNext(0);
	while(nUseIdx)
	{
		if (0 == memcmp(&Player[nUseIdx].m_Guid, pGuid, sizeof(GUID)))
			return nUseIdx;
		nUseIdx = m_UseIdx.GetNext(nUseIdx);
	}
	return 0;
}

// Add by Cooler 2004-5-12
// Begin -->
int KPlayerSet::GetPlayerIndexByName(const char *pAccName)
{
	int nIndex = 0;
	
	while(TRUE)
	{
		nIndex = m_UseIdx.GetNext(nIndex);
		if(nIndex == 0)
		{
			break;
		}

		if(0 == strcmp(Player[nIndex].m_AccoutName, pAccName))
		{
			return nIndex;
		}
	}

	return 0;
}
// End <--

// Add by Rocker 2004-7-28
// Begin -->
int KPlayerSet::GetPlayerIndexByPlayerName(const char *pPlayerName)
{
	int nIndex = 0;
	
	while(TRUE)
	{
		nIndex = m_UseIdx.GetNext(nIndex);
		if(nIndex == 0)
		{
			break;
		}
		
		if(0 == strcmp(Player[nIndex].m_PlayerName,	pPlayerName))
		{
			return nIndex;
		}
	}
	
	return 0;
}
// End <--

#endif

#ifdef _SERVER

#define AUTO_SAVE_STEP_INTERVAL_MIN 0
#define AUTO_SAVE_STEP_INTERVAL_MAX (GAME_FPS * 60)
#define AUTO_SAVE_STEP_COUNT_MIN 1
#define AUTO_SAVE_STEP_COUNT_MAX 100
#define AUTO_SAVE_INTERVAL_SECONDS_MIN 60
#define AUTO_SAVE_INTERVAL_SECONDS_MAX 3600
#define AUTO_SAVE_FIRST_INTERVAL_SECONDS_MIN 300
#define AUTO_SAVE_FIRST_INTERVAL_SECONDS_MAX 900

void KPlayerSet::InitAutoSave()
{
	int	nMinInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_playersave_min_interval);
	int	nMaxInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_playersave_max_interval);	
	int	firstAutoSaveIntervalSeconds = nMinInterval + g_Random( abs(nMaxInterval - nMinInterval) );
	if(firstAutoSaveIntervalSeconds <= 0)
	{	
		firstAutoSaveIntervalSeconds = AUTO_SAVE_FIRST_INTERVAL_SECONDS_MIN + 
			g_Random(AUTO_SAVE_FIRST_INTERVAL_SECONDS_MAX - AUTO_SAVE_FIRST_INTERVAL_SECONDS_MIN);
	}

	m_AutoSaveCurrentPlayerIndex = 1;
	m_AutoSaveNextTime = UNIX_TMIE_STAMP + firstAutoSaveIntervalSeconds;//设置第一次存盘时间
	m_AutoSaveProcessing = false;
	m_AutoSaveStepInterval = 0;
}

void KPlayerSet::ProcessAutoSave()
{
	if (m_AutoSaveProcessing)
	{
		if (m_AutoSaveStepInterval <= 0)
		{
			//重置步间隔
			m_AutoSaveStepInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_playersave_setp_interval);
			if (m_AutoSaveStepInterval < AUTO_SAVE_STEP_INTERVAL_MIN)
			{
				m_AutoSaveStepInterval = AUTO_SAVE_STEP_INTERVAL_MIN;
			}
			else if (m_AutoSaveStepInterval > AUTO_SAVE_STEP_INTERVAL_MAX)
			{
				m_AutoSaveStepInterval = AUTO_SAVE_STEP_INTERVAL_MAX;
			}

			//每一步存盘数量
			int count = ConfigManager::Singleton().GetGlobalVariable(global_var_playersave_setp_count);
			if (count < AUTO_SAVE_STEP_COUNT_MIN)
			{
				count = AUTO_SAVE_STEP_COUNT_MIN;
			}
			else if (count > AUTO_SAVE_STEP_COUNT_MAX)
			{
				count = AUTO_SAVE_STEP_COUNT_MAX;
			}

			//保存当前步长的玩家
			while (count > 0 && 
				m_AutoSaveCurrentPlayerIndex > 0 && 
				m_AutoSaveCurrentPlayerIndex < MAX_PLAYER)
			{
				if (IsValidPlayer(m_AutoSaveCurrentPlayerIndex))
				{
					Player[m_AutoSaveCurrentPlayerIndex].Save(NULL);
					count--;
				}
				
				m_AutoSaveCurrentPlayerIndex++;
			}
			
			//所有角色是否存完了
			if (m_AutoSaveCurrentPlayerIndex >= MAX_PLAYER)
			{
				//存盘间隔
				int autoSaveIntervalSeconds = ConfigManager::Singleton().GetGlobalVariable(global_var_playersave_interval_seconds);
				if (autoSaveIntervalSeconds < AUTO_SAVE_INTERVAL_SECONDS_MIN)
				{
					autoSaveIntervalSeconds = AUTO_SAVE_INTERVAL_SECONDS_MIN;
				}
				else if (autoSaveIntervalSeconds > AUTO_SAVE_INTERVAL_SECONDS_MAX)
				{
					autoSaveIntervalSeconds = AUTO_SAVE_INTERVAL_SECONDS_MAX;
				}

				m_AutoSaveCurrentPlayerIndex = 1;
				m_AutoSaveNextTime = UNIX_TMIE_STAMP + autoSaveIntervalSeconds;//设置下次存盘时间
				m_AutoSaveProcessing = false;
				m_AutoSaveStepInterval = 0;
			}
		}
		else
		{
			m_AutoSaveStepInterval--;
		}
	}
	else
	{
		if (m_AutoSaveNextTime < UNIX_TMIE_STAMP)
		{
			m_AutoSaveProcessing = true;
		}
	}
}
#endif

#ifdef _SERVER
void KPlayerSet::AddMoney(DWORD money)
{
	m_TotalRecentAddMoney += money;

	if (IsNeedSaveMoneyStatistic())
		SaveMoneyStatistic();
}

void KPlayerSet::DelMoney(DWORD money)
{
	m_TotalRecentDelMoney += money;

	if (IsNeedSaveMoneyStatistic())
		SaveMoneyStatistic();
}

void KPlayerSet::SaveMoneyStatistic()
{
	if (g_pLogSystem)
	{
		LogEventParam addMoneyLogEvent;
		addMoneyLogEvent.event = log_event_total_add_money_statistic;
		addMoneyLogEvent.param4 = m_TotalRecentAddMoney;
		g_pLogSystem->Log(addMoneyLogEvent);

		LogEventParam delMoneyLogEvent;
		delMoneyLogEvent.event = log_event_total_del_money_statistic;
		delMoneyLogEvent.param4 = m_TotalRecentDelMoney;
		g_pLogSystem->Log(delMoneyLogEvent);
	}

	InitMoneyStatistic();
}

#define NEED_SAVE_MONEY_STATISTIC_VALUE (MAX_INT_VALUE / 2)
#define DEFAULT_TOTAL_MONEY_STATISTIC_AUTO_SAVE_INTERVAL 3600

bool KPlayerSet::IsNeedSaveMoneyStatistic()
{
	return (m_NextSaveMoneyStatisticTime < UNIX_TMIE_STAMP
		|| m_TotalRecentAddMoney > NEED_SAVE_MONEY_STATISTIC_VALUE
		|| m_TotalRecentDelMoney > NEED_SAVE_MONEY_STATISTIC_VALUE);
}

void KPlayerSet::InitMoneyStatistic()
{
	m_TotalRecentAddMoney = 0;
	m_TotalRecentDelMoney = 0;

	int interval = ConfigManager::Singleton().GetGlobalVariable(global_var_log_total_money_statistic_auto_save_interval);
	if (interval <= 0)
	{
		interval = DEFAULT_TOTAL_MONEY_STATISTIC_AUTO_SAVE_INTERVAL;
	}
	m_NextSaveMoneyStatisticTime = UNIX_TMIE_STAMP + interval;
}
#endif

