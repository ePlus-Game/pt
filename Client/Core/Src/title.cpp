//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 2008-9-8
//      File_base        : title
//      File_ext         : .cpp
//      Author           : 徐晓刚
//      Description      : 称号系统
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KPlayer.h"
#include "KRegion.h"
#include "title.h"

#ifdef _SERVER
#include "buff_man.h"
#else
#include "CoreShell.h"
#include "cfs_fs2_savedef.h"
#endif

#define TITLE_CONFIG_FILE "/settings/titleconfig.txt"
#define DEFAULT_RANDOM_SELECT_TITLE_INTERVAL 900
#define MIN_RANDOM_SELECT_TITLE_INTERVAL 60
#define DEFAULT_CHECK_EXPIRE_TITLE_INTERVAL 60
#define MAX_TITLE_EXPIRE_TIME 2000000000
#define CURRENT_DB_TITLE_INFO_VERSION 1

//Title Config File Column Names
#define TITLE_CONFIG_FILE_COLUMN_INDEX			"Index"
#define TITLE_CONFIG_FILE_COLUMN_DEPRECATED		"Deprecated"
#define TITLE_CONFIG_FILE_COLUMN_TYPE			"Type"
#define TITLE_CONFIG_FILE_COLUMN_APPENDVALUE	"AppendValue"
#define TITLE_CONFIG_FILE_COLUMN_NAME			"Name"
#define TITLE_CONFIG_FILE_COLUMN_SHOWNAME		"ShowName"
#define TITLE_CONFIG_FILE_COLUMN_TIP			"Tip"
#define TITLE_CONFIG_FILE_COLUMN_BUFF			"Buff"

TitleInfo TitleManager::m_TitleInfo[MAX_TITLE_COUNT + 1];

TitleManager::TitleManager()
{
	Init(0);
}

TitleManager::~TitleManager()
{
}

void TitleManager::Init(int playerIndex)
{
	m_SelectedTitle = 0;
	memset(m_State, 0, sizeof(m_State));
	m_PlayerIndex = playerIndex;

#ifdef _SERVER
	m_RandomSelectTitle = false;
	m_NextRandomChangeTitleTime = 0;
	m_NextCheckExpireTitleTime = 0;
	m_CurrenTitleBuffTemplateId = 0;
#else
	m_CurrentSelectTitleResult = 0;
#endif
}

#ifdef _SERVER

void TitleManager::Active()
{
	if (m_RandomSelectTitle)
	{
		if (m_NextRandomChangeTitleTime <= UNIX_TMIE_STAMP)
		{
			RandomSelectTitle();
			
			int interval = ConfigManager::Singleton().GetGlobalVariable(global_var_random_select_title_interval);
			if (interval <= 0)
				interval = DEFAULT_RANDOM_SELECT_TITLE_INTERVAL;
			else if (interval < MIN_RANDOM_SELECT_TITLE_INTERVAL)
				interval = MIN_RANDOM_SELECT_TITLE_INTERVAL;
			m_NextRandomChangeTitleTime = UNIX_TMIE_STAMP + interval;
		}
	}

	if (m_NextCheckExpireTitleTime <= UNIX_TMIE_STAMP)
	{
		CheckExpireTitle();
		m_NextCheckExpireTitleTime = UNIX_TMIE_STAMP + DEFAULT_CHECK_EXPIRE_TITLE_INTERVAL;
	}
}

void TitleManager::GrantTitle(int titleIndex)
{
	if (!IsValidTitleIndex(titleIndex))
		return;

	m_State[titleIndex].IsActive = TRUE;

	UpdateSelfTitle(titleIndex);
}

void TitleManager::GrantTitle(int titleIndex, DWORD expireTime)
{
	if (!IsValidTitleIndex(titleIndex))
		return;

	m_State[titleIndex].IsActive = TRUE;
	m_State[titleIndex].ExpireTime = expireTime;

	UpdateSelfTitle(titleIndex);
}

void TitleManager::GrantTitle(int titleIndex, WORD startLevel, WORD startValue)
{
	if (!IsValidTitleIndex(titleIndex))
		return;

	m_State[titleIndex].IsActive = TRUE;
	m_State[titleIndex].LevelInfo.Level = startLevel;
	m_State[titleIndex].LevelInfo.Value = startValue;

	UpdateSelfTitle(titleIndex);
}

void TitleManager::RevokeTitle(int titleIndex)
{
	if (!IsValidTitleIndex(titleIndex))
		return;

	m_State[titleIndex].IsActive = FALSE;
	m_State[titleIndex].ExpireTime = 0;

	if (m_SelectedTitle == titleIndex)
	{
		SelectTitle(0);
	}

	UpdateSelfTitle(titleIndex);
}

void TitleManager::SetTitleExpireTime(int titleIndex, DWORD newExpireTime)
{
	if (!HasTitle(titleIndex))
		return;

	TitleInfo* pInfo = GetTitleInfo(titleIndex);
	if (NULL == pInfo)
		return;

	if (pInfo->Type != title_type_time_limited)
		return;

	TitleState& state = m_State[titleIndex];

	state.ExpireTime = newExpireTime;

	CheckExpireTitle();
	UpdateSelfTitle(titleIndex);
}

void TitleManager::ModifyTitleExpireTime(int titleIndex, int changeExpireTime)
{
	if (!HasTitle(titleIndex))
		return;

	if (changeExpireTime == 0)
		return;

	TitleInfo* pInfo = GetTitleInfo(titleIndex);
	if (NULL == pInfo)
		return;

	if (pInfo->Type != title_type_time_limited)
		return;

	TitleState& state = m_State[titleIndex];

	if (changeExpireTime > 0)
	{
		if (MAX_TITLE_EXPIRE_TIME - state.ExpireTime < changeExpireTime)
		{
			state.ExpireTime = MAX_TITLE_EXPIRE_TIME;
		}
		else
		{
			state.ExpireTime += changeExpireTime;
		}
	}
	else
	{
		if (state.ExpireTime < -changeExpireTime)
		{
			state.ExpireTime = 0;
		}
		else
		{
			state.ExpireTime += changeExpireTime;
		}
	}

	CheckExpireTitle();
	UpdateSelfTitle(titleIndex);
}

void TitleManager::ModifyTitleLevelInfo(int titleIndex, int changeValue)
{
	if (!HasTitle(titleIndex))
		return;

	if (changeValue == 0)
		return;

	TitleInfo* pInfo = GetTitleInfo(titleIndex);
	if (NULL == pInfo)
		return;

	if (pInfo->Type != title_type_upgradable)
		return;

	TitleState& state = m_State[titleIndex];

	if (changeValue > 0)
	{
		for (int level = state.LevelInfo.Level; level < pInfo->LevelCount; level++)
		{
			int maxAddValue = pInfo->UpgradInfo[level].MaxValue - state.LevelInfo.Value;
			if (changeValue <= maxAddValue)
			{
				state.LevelInfo.Value += changeValue;
				break;
			}
			else
			{
				if (state.LevelInfo.Level + 1 < pInfo->LevelCount)
				{
					changeValue -= maxAddValue;
					state.LevelInfo.Value = 0;
					state.LevelInfo.Level++;
				}
				else
				{
					state.LevelInfo.Value = pInfo->UpgradInfo[level].MaxValue;
					break;
				}
			}
		}
	}
	else
	{
		while (state.LevelInfo.Level >= 0)
		{
			int maxDelValue = state.LevelInfo.Value;
			if (-changeValue < maxDelValue)
			{
				state.LevelInfo.Value += changeValue;
				break;
			}
			else
			{
				if (state.LevelInfo.Level > 0)
				{
					changeValue += maxDelValue;
					state.LevelInfo.Level--;
					state.LevelInfo.Value = pInfo->UpgradInfo[state.LevelInfo.Level].MaxValue;
				}
				else
				{
					RevokeTitle(titleIndex);
					return;
				}
			}
		}
	}

	CheckTitleBuff();
	UpdateSelfTitle(titleIndex);
}

void TitleManager::SyncSelfTitle() const
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	char titleSyncDataBuff[MAX_TITLE_COUNT * sizeof(TransferTitleInfo) + sizeof(SYNC_SELF_TITLE)];
	PSYNC_SELF_TITLE pSyncSelfTitle = (PSYNC_SELF_TITLE)titleSyncDataBuff;

	pSyncSelfTitle->Protocol = s2c_sync_self_title;
	pSyncSelfTitle->Length = sizeof(SYNC_SELF_TITLE) - 1;
	pSyncSelfTitle->SelectedTitle = m_RandomSelectTitle ? RANDOM_SELECT_TITLE_FALG : m_SelectedTitle;
	pSyncSelfTitle->TitleCount = 0;
	for (int i = 1; i <= MAX_TITLE_COUNT; i++)
	{
		if (m_State[i].IsActive)
		{
			pSyncSelfTitle->InfoList[pSyncSelfTitle->TitleCount].TitleIndex = i;
			pSyncSelfTitle->InfoList[pSyncSelfTitle->TitleCount].ExpireTime = m_State[i].ExpireTime;
			pSyncSelfTitle->TitleCount++;
			pSyncSelfTitle->Length += sizeof(TransferTitleInfo);
		}
	}

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(), (BYTE*)titleSyncDataBuff, pSyncSelfTitle->Length + 1);
}

void TitleManager::UpdateSelfTitle(int titleIndex) const
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	if (!IsValidTitleIndex(titleIndex))
		return;

	UPDATE_SELF_TITLE updateTitle;
	updateTitle.Protocol = s2c_update_self_title;
	updateTitle.IsActive = m_State[titleIndex].IsActive;
	updateTitle.Info.TitleIndex = titleIndex;
	updateTitle.Info.ExpireTime = m_State[titleIndex].ExpireTime;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(), (BYTE*)&updateTitle, sizeof(updateTitle));

	if (m_SelectedTitle == titleIndex)
	{
		UpdateSelectedTitle();
	}
}

void TitleManager::SelectTitle(int titleIndex)
{
	if (!IsValidTitleIndex(titleIndex))
	{
		if (m_SelectedTitle != 0)
		{
			m_SelectedTitle = 0;
			UpdateSelectedTitle();
		}
		
		CheckTitleBuff();
		return;
	}
	
	if (HasTitle(titleIndex) && m_SelectedTitle != titleIndex)
	{
		m_SelectedTitle = titleIndex;
		UpdateSelectedTitle();
	}

	CheckTitleBuff();
}

void TitleManager::UpdateSelectedTitle() const
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;
	
	int npcIndex = Player[m_PlayerIndex].GetNpcIndex();
	if (!IsValidNpc(npcIndex))
		return;
	
	CHANGE_TITLE changeTitle;
	changeTitle.Protocol = s2c_change_title;
	changeTitle.NpcID = Npc[npcIndex].GetId();
	GetSelectedTitle(changeTitle.TitleIndex, changeTitle.TitleLevel);
	
	int broadcastCount = MAX_BROADCAST_COUNT_MIN;
	Npc[npcIndex].BroadCastRegion(&changeTitle, sizeof(changeTitle), broadcastCount);
}

void TitleManager::SyncSelectTitleResult() const
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	SELECT_TITLE selectTitleResult;
	selectTitleResult.Protocol = s2c_select_title_result;
	selectTitleResult.TitleIndex = m_RandomSelectTitle ? RANDOM_SELECT_TITLE_FALG : m_SelectedTitle;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_PlayerIndex].GetNetConnectIdx(), (BYTE*)&selectTitleResult, sizeof(selectTitleResult));
}

void TitleManager::RandomSelectTitle()
{
	int titleCount = 0;
	for (int i = 1; i <= MAX_TITLE_COUNT; i++)
	{
		if (m_State[i].IsActive)
			titleCount++;
	}

	if (titleCount > 0)
	{
		int random = g_Random(titleCount);
		titleCount = 0;
		for (int i = 1; i <= MAX_TITLE_COUNT; i++)
		{
			if (m_State[i].IsActive)
			{
				if (titleCount == random)
				{
					SelectTitle(i);
				}

				titleCount++;
			}
		}
	}
}

void TitleManager::CheckExpireTitle()
{
	for (int i = 1; i <= MAX_TITLE_COUNT; i++)
	{
		if (m_State[i].IsActive)
		{
			TitleInfo* pInfo = GetTitleInfo(i);
			if (pInfo && title_type_time_limited == pInfo->Type && m_State[i].ExpireTime <= UNIX_TMIE_STAMP)
			{
				RevokeTitle(i);
			}
		}
	}
}

void TitleManager::CheckTitleBuff()
{
	if (!IsValidPlayer(m_PlayerIndex))
		return;

	int npcIndex = Player[m_PlayerIndex].GetNpcIndex();
	if (!IsValidNpc(npcIndex))
		return;

	//检查目前应该是哪个BUFF
	int newTitleBuffTemplateId = 0;
	if (m_SelectedTitle > 0 && HasTitle(m_SelectedTitle))
	{
		TitleInfo* pTitleInfo = GetTitleInfo(m_SelectedTitle);
		if (pTitleInfo)
		{
			if (title_type_upgradable == pTitleInfo->Type)
			{
				int level = m_State[m_SelectedTitle].LevelInfo.Level;
				if (level >= 0 && level < MAX_UPGRADABLE_TITLE_LEVEL_COUNT)
				{
					newTitleBuffTemplateId = pTitleInfo->UpgradInfo[level].BuffTemplateId;
				}
			}
			else
			{
				newTitleBuffTemplateId = pTitleInfo->BuffTemplateId;
			}
		}
	}

	//BUFF需要发生变化
	if (m_CurrenTitleBuffTemplateId != newTitleBuffTemplateId)
	{
		//清除旧BUFF
		if (m_CurrenTitleBuffTemplateId > 0)
		{
			BuffMgr::Singleton().ClearBuffByTempID(npcIndex, m_CurrenTitleBuffTemplateId);
		}
		
		//添加新BUFF
		if (newTitleBuffTemplateId > 0)
		{
			BuffMgr::Singleton().AddNpcBuff(npcIndex, npcIndex, newTitleBuffTemplateId);
		}

		m_CurrenTitleBuffTemplateId = newTitleBuffTemplateId;
	}
}

bool TitleManager::LoadState(const BYTE* pData, int dataSize)
{
	if (NULL == pData)
		return false;
	
	PFS2DB_TITLE_INFO_COL pDBTitleInfoCol = (PFS2DB_TITLE_INFO_COL)pData;
	
	if (dataSize < (sizeof(FS2DB_TITLE_INFO_COL) - sizeof(FS2DB_TITLE_INFO)))
		return false;

	if (dataSize != sizeof(FS2DB_TITLE_INFO_COL) - sizeof(FS2DB_TITLE_INFO) + pDBTitleInfoCol->TitleCount * sizeof(FS2DB_TITLE_INFO))
		return false;

	for (int i = 0; i < pDBTitleInfoCol->TitleCount; i++)
	{
		int titleIndex = pDBTitleInfoCol->InfoList[i].TitleIndex;
		if (IsValidTitleIndex(titleIndex))
		{
			TitleInfo* pInfo = GetTitleInfo(titleIndex);
			if (pInfo && !pInfo->Deprecated)
			{
				m_State[titleIndex].IsActive = true;
				m_State[titleIndex].ExpireTime = pDBTitleInfoCol->InfoList[i].TitleData;
			}
		}
	}

	if (pDBTitleInfoCol->SelectedTitle == RANDOM_SELECT_TITLE_FALG)
	{
		m_RandomSelectTitle = true;
	}
	else
	{
		if (HasTitle(pDBTitleInfoCol->SelectedTitle))
		{
			m_SelectedTitle = pDBTitleInfoCol->SelectedTitle;
		}
	}
	
	return true;
}

bool TitleManager::SaveState(BYTE* pSaveBuff, int& buffSize)
{
	if (NULL == pSaveBuff)
		return false;

	if (buffSize < sizeof(FS2DB_TITLE_INFO_COL) - sizeof(FS2DB_TITLE_INFO) + MAX_TITLE_COUNT * sizeof(FS2DB_TITLE_INFO))
		return false;
	
	PFS2DB_TITLE_INFO_COL pDBTitleInfoCol = (PFS2DB_TITLE_INFO_COL)pSaveBuff;

	pDBTitleInfoCol->Version = CURRENT_DB_TITLE_INFO_VERSION;
	pDBTitleInfoCol->SelectedTitle = (short)(m_RandomSelectTitle ? RANDOM_SELECT_TITLE_FALG : m_SelectedTitle);
	pDBTitleInfoCol->TitleCount = 0;

	for (int i = 1; i <= MAX_TITLE_COUNT; i++)
	{
		if (m_State[i].IsActive)
		{
			pDBTitleInfoCol->InfoList[pDBTitleInfoCol->TitleCount].TitleIndex = i;
			pDBTitleInfoCol->InfoList[pDBTitleInfoCol->TitleCount].TitleData = m_State[i].ExpireTime;
			pDBTitleInfoCol->TitleCount++;
		}
	}

	buffSize = sizeof(FS2DB_TITLE_INFO_COL) - sizeof(FS2DB_TITLE_INFO) + pDBTitleInfoCol->TitleCount * sizeof(FS2DB_TITLE_INFO);

	return true;
}

void TitleManager::OnLaunchPlayer()
{
	CheckTitleBuff();
}

#else

int TitleManager::GetSelfTitleInfo(UiTitleInfo* uiTitleInfoArray, int maxCount) const
{
	if (NULL == uiTitleInfoArray || maxCount <= 0)
		return 0;

	int count = 0;
	for (int i = 0; i < MAX_TITLE_COUNT; i++)
	{
		const TitleState& state = m_State[i];
		if (state.IsActive)
		{
			TitleInfo* pInfo = GetTitleInfo(i);
			if (NULL == pInfo)
				continue;

			UiTitleInfo& uiInfo = uiTitleInfoArray[count];

			uiInfo.ID = i;
			uiInfo.Type = pInfo->Type;
			
			switch(pInfo->Type)
			{
			case title_type_permanent:
				{
					strncpy(uiInfo.Name, pInfo->Name, sizeof(uiInfo.Name));
					strncpy(uiInfo.Tip, pInfo->Tip, sizeof(uiInfo.Tip));
					uiInfo.State = 0;
				}
				break;
			case title_type_time_limited:
				{
					strncpy(uiInfo.Name, pInfo->Name, sizeof(uiInfo.Name));
					strncpy(uiInfo.Tip, pInfo->Tip, sizeof(uiInfo.Tip));
					uiInfo.State = state.ExpireTime;
				}
				break;
			case title_type_upgradable:
				{
					if (state.LevelInfo.Level >= 0 && state.LevelInfo.Level < MAX_UPGRADABLE_TITLE_LEVEL_COUNT)
					{
						strncpy(uiInfo.Name, pInfo->UpgradInfo[state.LevelInfo.Level].Name, sizeof(uiInfo.Name));
						strncpy(uiInfo.Tip, pInfo->UpgradInfo[state.LevelInfo.Level].Tip, sizeof(uiInfo.Tip));
					}
					
					uiInfo.State = state.LevelInfo.Value;
				}
				break;
			}

			count++;
			if (count >= maxCount)
				break;
		}
	}

	return count;
}

int TitleManager::GetDetailSelfTitleInfo(UiTitleInfo* uiTitleInfoArray, int maxCount) const
{
	if (NULL == uiTitleInfoArray || maxCount <= 0)
		return 0;
	
	int count = 0;
	for (int i = 0; i < MAX_TITLE_COUNT; i++)
	{
		const TitleState& state = m_State[i];
		
		TitleInfo* pInfo = GetTitleInfo(i);
		if (NULL == pInfo)
			continue;
		
		switch(pInfo->Type)
		{
		case title_type_permanent:
			{
				if (state.IsActive)
				{
					UiTitleInfo& uiInfo = uiTitleInfoArray[count];
					
					uiInfo.ActiveState = title_active_state_yes;
					uiInfo.ID = i;
					uiInfo.Type = pInfo->Type;
					strncpy(uiInfo.Name, pInfo->Name, sizeof(uiInfo.Name));
					strncpy(uiInfo.Tip, pInfo->Tip, sizeof(uiInfo.Tip));
					uiInfo.State = 0;
					
					count++;
					if (count >= maxCount)
						break;
				}
			}
			break;
		case title_type_time_limited:
			{
				if (state.IsActive)
				{
					UiTitleInfo& uiInfo = uiTitleInfoArray[count];
					
					uiInfo.ActiveState = title_active_state_yes;
					uiInfo.ID = i;
					uiInfo.Type = pInfo->Type;
					strncpy(uiInfo.Name, pInfo->Name, sizeof(uiInfo.Name));
					strncpy(uiInfo.Tip, pInfo->Tip, sizeof(uiInfo.Tip));
					uiInfo.State = state.ExpireTime;
					
					count++;
					if (count >= maxCount)
						break;
				}
			}
			break;
		case title_type_upgradable:
			{
				for (int level = 0; level < MAX_UPGRADABLE_TITLE_LEVEL_COUNT; level++)
				{	
					if (level >= pInfo->LevelCount)
						break;
					
					UiTitleInfo& uiInfo = uiTitleInfoArray[count];

					if (TRUE == state.IsActive)
					{
						if (level <= state.LevelInfo.Level)
						{
							uiInfo.ActiveState = title_active_state_yes;
							uiInfo.State = 0;
						}
						else if (level == state.LevelInfo.Level + 1)
						{
							uiInfo.ActiveState = title_active_state_next;
							uiInfo.State = state.LevelInfo.Value;
						}
						else
						{
							uiInfo.ActiveState = title_active_state_no;
							uiInfo.State = 0;
						}
					}
					else
					{
						uiInfo.ActiveState = (level == 0) ? title_active_state_first : title_active_state_no;
						uiInfo.State = 0;
					}
										
					uiInfo.ID = i;
					uiInfo.Type = pInfo->Type;
					strncpy(uiInfo.Name, pInfo->UpgradInfo[level].Name, sizeof(uiInfo.Name));
					strncpy(uiInfo.Tip, pInfo->UpgradInfo[level].Tip, sizeof(uiInfo.Tip));
					
					count++;
					if (count >= maxCount)
						break;
				}
			}
			break;
		}
	}
	
	return count;
}

int TitleManager::GetCurrentSelectedTitle() const
{
	return m_CurrentSelectTitleResult;
}

void TitleManager::ReceiveSyncData(BYTE* pMsg)
{
	PSYNC_SELF_TITLE pSyncSelfTitle = (PSYNC_SELF_TITLE)pMsg;

	int dataLength = pSyncSelfTitle->Length + 1 - sizeof(SYNC_SELF_TITLE);
	if (dataLength != (pSyncSelfTitle->TitleCount * sizeof(TransferTitleInfo)))
		return;

	Init(0);

	m_CurrentSelectTitleResult = pSyncSelfTitle->SelectedTitle;

	for (int i = 0; i < pSyncSelfTitle->TitleCount; i++)
	{
		int titleIndex = pSyncSelfTitle->InfoList[i].TitleIndex;
		if (IsValidTitleIndex(titleIndex))
		{
			m_State[titleIndex].IsActive = true;
			m_State[titleIndex].ExpireTime = pSyncSelfTitle->InfoList[i].ExpireTime;
		}
	}

	CoreDataChanged(GDCNI_TITLEINFO_UPDATA, 0, 0);
}

void TitleManager::ReceiveUpdateTitle(BYTE* pMsg)
{
	PUPDATE_SELF_TITLE pUpdateSelfTitle = (PUPDATE_SELF_TITLE)pMsg;

	if (IsValidTitleIndex(pUpdateSelfTitle->Info.TitleIndex))
	{
		m_State[pUpdateSelfTitle->Info.TitleIndex].IsActive = pUpdateSelfTitle->IsActive;
		m_State[pUpdateSelfTitle->Info.TitleIndex].ExpireTime = pUpdateSelfTitle->Info.ExpireTime;

		CoreDataChanged(GDCNI_TITLEINFO_UPDATA, 0, 0);
	}
}

void TitleManager::ReceiveSelectTitleResult(BYTE* pMsg)
{
	PSELECT_TITLE pSelectTitle = (PSELECT_TITLE)pMsg;

	m_CurrentSelectTitleResult = pSelectTitle->TitleIndex;

	CoreDataChanged(GDCNI_TITLEINFO_UPDATA, 0, 0);
}

void TitleManager::SelectTitle(int titleIndex)
{
	if (titleIndex == RANDOM_SELECT_TITLE_FALG || (titleIndex >= 0 && titleIndex <= MAX_TITLE_COUNT))
	{
		if (titleIndex != m_CurrentSelectTitleResult)
		{
			SELECT_TITLE selectTitle;
			selectTitle.Protocol = c2s_select_title;
			selectTitle.TitleIndex = (char)titleIndex;

			if (g_pClient)
				g_pClient->SendPackToServer(g_ConnectID, &selectTitle, sizeof(selectTitle));
		}
	}
}

#endif

bool TitleManager::LoadSettings()
{
	memset(m_TitleInfo, 0, sizeof(m_TitleInfo));

	KTabFile titleConfigFile;
	if (TRUE == titleConfigFile.Load(TITLE_CONFIG_FILE))
	{
		int currentTitleIndex = 0;
		int currentTitleLevel = 0;
		int rowCount = titleConfigFile.GetHeight();
		int row = 2;
		while (row <= rowCount)
		{
			int field = 1;

			int titleIndex = 0;
			if (FALSE == titleConfigFile.GetInteger(row, TITLE_CONFIG_FILE_COLUMN_INDEX, 0, &titleIndex))
				return false;

			if (titleIndex > MAX_TITLE_COUNT)
				return false;

			if (titleIndex == currentTitleIndex)
			{
				currentTitleLevel++;
				if (currentTitleLevel >= MAX_UPGRADABLE_TITLE_LEVEL_COUNT)
					return false;
			}
			else if (titleIndex > currentTitleIndex)
			{
				currentTitleLevel = 0;
				currentTitleIndex = titleIndex;
			}
			else
			{
				return false;
			}

			TitleInfo& titleInfo = m_TitleInfo[currentTitleIndex];

			int deprecated = FALSE;
			if (FALSE == titleConfigFile.GetInteger(row, TITLE_CONFIG_FILE_COLUMN_DEPRECATED, 0, (int*)&(deprecated)))
				return false;
			titleInfo.Deprecated = (deprecated == TRUE);

			if (FALSE == titleConfigFile.GetInteger(row, TITLE_CONFIG_FILE_COLUMN_TYPE, 0, (int*)&(titleInfo.Type)))
				return false;

			if (title_type_upgradable == titleInfo.Type)
			{
				if (FALSE == titleConfigFile.GetInteger(row, TITLE_CONFIG_FILE_COLUMN_APPENDVALUE, 0, (int*)&(titleInfo.UpgradInfo[currentTitleLevel].MaxValue)))
					return false;

#ifdef _SERVER
				titleConfigFile.GetInteger(row, TITLE_CONFIG_FILE_COLUMN_BUFF, 0, &(titleInfo.UpgradInfo[currentTitleLevel].BuffTemplateId));
#else
				if (FALSE == titleConfigFile.GetString(row, TITLE_CONFIG_FILE_COLUMN_NAME, "DefaultName", titleInfo.UpgradInfo[currentTitleLevel].Name, sizeof(titleInfo.UpgradInfo[currentTitleLevel].Name)))
					return false;

				titleConfigFile.GetString(row, TITLE_CONFIG_FILE_COLUMN_SHOWNAME, "", titleInfo.UpgradInfo[currentTitleLevel].ShowName, sizeof(titleInfo.UpgradInfo[currentTitleLevel].ShowName));
				titleConfigFile.GetString(row, TITLE_CONFIG_FILE_COLUMN_TIP, "", titleInfo.UpgradInfo[currentTitleLevel].Tip, sizeof(titleInfo.UpgradInfo[currentTitleLevel].Tip));				

				if (0 == currentTitleLevel)
				{
					strncpy(titleInfo.Name, titleInfo.UpgradInfo[currentTitleLevel].Name, sizeof(titleInfo.Name));
					titleInfo.Name[sizeof(titleInfo.Name) - 1] = 0;
					strncpy(titleInfo.ShowName, titleInfo.UpgradInfo[currentTitleLevel].ShowName, sizeof(titleInfo.ShowName));
					titleInfo.ShowName[sizeof(titleInfo.ShowName) - 1] = 0;
					strncpy(titleInfo.Tip, titleInfo.UpgradInfo[currentTitleLevel].Tip, sizeof(titleInfo.Tip));
					titleInfo.Tip[sizeof(titleInfo.Tip) - 1] = 0;
				}
#endif

				titleInfo.LevelCount++;
			}
			else
			{
#ifdef _SERVER
				titleConfigFile.GetInteger(row, TITLE_CONFIG_FILE_COLUMN_BUFF, 0, &(titleInfo.BuffTemplateId));
#else
				if (FALSE == titleConfigFile.GetString(row, TITLE_CONFIG_FILE_COLUMN_NAME, "DefaultName", titleInfo.Name, sizeof(titleInfo.Name)))
					return false;

				titleConfigFile.GetString(row, TITLE_CONFIG_FILE_COLUMN_SHOWNAME, "", titleInfo.ShowName, sizeof(titleInfo.ShowName));
				titleConfigFile.GetString(row, TITLE_CONFIG_FILE_COLUMN_TIP, "", titleInfo.Tip, sizeof(titleInfo.Tip));
#endif			
			}

			titleInfo.Valid = true;
			row++;
		}
	}

	return true;
}

void TitleManager::GetSelectedTitle(BYTE& selectedTitle, BYTE& titleLevel) const
{
	selectedTitle = (BYTE)m_SelectedTitle;
	titleLevel = 0;
	if (IsValidTitleIndex(m_SelectedTitle))
	{
		if (title_type_upgradable == m_TitleInfo[m_SelectedTitle].Type)
			titleLevel = (BYTE)m_State[m_SelectedTitle].LevelInfo.Level;
	}
}