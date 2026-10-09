#include "KCore.h"
#include "KObjSet.h"
#include "KNpcSet.h"
#include "KSubWorld.h"
#include "KNpc.h"
#include "KIniFile.h"
#include "KSubWorldSet.h"
#include "LuaFuns.h"
#include "KNpcTemplate.h"
#include "KPlayerSet.h"
#include "KPlayer.h"

//Add By Brianyao2007

#ifdef _SERVER
#include "KTaisuiWheelServer.h"
#include "KWarInfoManager.h"
#include "tong_war_manager.h"
#include "pool_combat_info_mgr.h"
#include "social_recruit_svr.h"
#include "pool_combat_mgr.h"
#include "npc_save.h"
#include "keconomysys.h"
#endif

#ifdef _SERVER
#include "ChatCommon.h"
#include "fseye_protocol.h"
#endif

#ifndef _SERVER
unsigned long KSubWorldSet::s_uLastTime = 0L;
float KSubWorldSet::s_fScale = 1.0f;
#endif

extern KNpcTemplate	* g_pNpcTemplate[MAX_NPCSTYLE][MAX_NPC_LEVEL]; //0,0为起点

KSubWorldSet g_SubWorldSet;

#ifdef _SERVER
#include "buff_man.h"
#include "DBAucDataCenter.h"
#include "ChatCenter_S.h"
#include "question.h"
#endif

#define MAXSIZE_INIKEYVALUE				512

#ifdef _SERVER
BOOL KSubWorldSet::s_bNeedBlanceSpawn = FALSE;
#endif

KSubWorldSet::KSubWorldSet()
{
	m_nLoopRate = 0;
#ifndef _SERVER
	m_dwPing = 0;
	m_bMusic = true;
#endif
#ifdef _SERVER
	m_NextSaveSpawnInfoTime = 0;
	m_NextPingTime = 0;
#endif
}

int KSubWorldSet::SearchWorld(DWORD dwID)
{
	for (int i = 0; i < INSTANCE_SUBWORLD_START; i++)
	{
		if ((DWORD)SubWorld[i].m_SubWorldID == dwID)
			return i;
	}
	return INVALID_WORLD_INDEX;
}

int	KSubWorldSet::SearchWorld(char* szWorldName)
{
	if (szWorldName == 0)
	{
		return INVALID_WORLD_INDEX;
	}

	char szBuffer[256];
	::memset(szBuffer, 0, sizeof(szBuffer));

	for (int i=0; i<MAX_SUBWORLD; ++i)
	{
		if (GetWorldNameFromID(SubWorld[i].m_SubWorldID, szBuffer, sizeof(szBuffer)) && 
			::strcmp(szWorldName, szBuffer) == 0)
		{
			return i;
		}		
	}

	return INVALID_WORLD_INDEX;
}

BOOL KSubWorldSet::GetWorldNameFromID(DWORD dwID, char* szWorldName, size_t uBufLen)
{
	char		Index[16];
	char		Buff[128];

	sprintf(Index, "%d", (int)dwID);
	if (m_MapListIni.GetString("List", Index, "", Buff, sizeof(Buff)) == FALSE)
		return FALSE;

	strcat(Index, "_name");
	if (!m_MapListIni.GetString("List", Index, "", szWorldName, uBufLen))
	{
		char* pName = strchr(Buff, '\\');
		if (pName)
		{
			while(strchr(pName, '\\'))
				pName = strchr(pName, '\\') + 1;
			strncpy(szWorldName, pName, uBufLen);
		}
		else
			strncpy(szWorldName, Buff, uBufLen);
	}

	return TRUE;
}

#define KEYLEN	32
#define CONTENTLEN	256

#ifndef _SERVER
BOOL KSubWorldSet::LoadMapList()
{
	if (!m_MapListIni.Load("\\settings\\maplist.ini"))
		return FALSE;

	return TRUE;
}
#endif

BOOL KSubWorldSet::Load(LPSTR szFileName)
{
	KIniFile	IniFile;
	char		szKeyName[KEYLEN];
	int			nWorldID;
	int			nWorldCount = 0;

	IniFile.Load(szFileName);
	IniFile.GetInteger("Init", "Count", 1, &nWorldCount);
	if (nWorldCount > MAX_SUBWORLD)
		return FALSE;

	if (!m_MapListIni.Load("\\settings\\maplist.ini"))
		return FALSE;

#ifdef _SERVER
	m_nLaodedMapCount = 0;

	if(!m_ReviveIniFile.Load("settings/RevivePos.ini"))
		return	FALSE;
#endif

	for (int i = 0; i < nWorldCount; i++)
	{
		sprintf((char*)szKeyName, "World%02d", i);

		if (IniFile.GetInteger("World", szKeyName, 0, &nWorldID))
		{
#ifdef _SERVER
			SubWorld[i].m_nIndex = i;
			if (!SubWorld[i].LoadMap(nWorldID))
			{
				// LogError szWorldFile Not Load
			}
			m_nLaodedMapCount++;
#endif
		}
		else
		{
			CFS_FILELOGS::WriteDebugLog("WorldSet.ini Error!\n");
			return FALSE;
		}
	}

	return TRUE;
}

int nActiveRegionCount;

void KSubWorldSet::MainLoop()
{
	m_nLoopRate++;

	nActiveRegionCount = 0;
	
	for (int worldIndex = 0; worldIndex < MAX_SUBWORLD; worldIndex++)
	{
		SubWorld[worldIndex].Activate();
	}
	
#ifdef _SERVER
	
	GetGlobalWarInfoManager().Breathe();
	GetPoolCombatInfoManager().Breathe();
	GetGlobalTongWarMgr().Breathe();
	GetGlobalPoolCombatMgr().Breathe();	
	StatueInfoMgr::Singleton().Breathe();
	KEconomySysManager::Singleton().Breathe();
	
	KTaisuiWheelServer::Breathe();
	KSocialRecruitMgr::Singlton().Breathe();
	BuffMgr::Singleton().Breathe( );
	
	
	DBAucDataCenter::Singleton().Active();
	
 	if ( s_bNeedBlanceSpawn )
 	{
// 		//记录日志：BalanceSpawn
// 		if (g_pLogSystem)
// 		{
// 			char spawnInfo[256] = { 0 };
// 			snprintf(spawnInfo, sizeof(spawnInfo), "BalanceSpawn");
// 			spawnInfo[sizeof(spawnInfo) - 1] = 0;
// 			g_pLogSystem->SysDbgLog(spawnInfo, strlen(spawnInfo), sys_dbg_log_event_balance_spawn);
// 		}
// 
// 		for(int x =0; x < MAX_SUBWORLD ; x++)
// 		{
// 			KSubWorld& aSubWorld = SubWorld[x];
// 			for(int y=0; y < aSubWorld.m_nTotalRegion ; y++)
// 			{
// 				aSubWorld.m_Region[y].RemoveAllSpawnNpc();
// 			}
// 		}
 		s_bNeedBlanceSpawn = FALSE;
 	}
	
//	g_PlayerStatusChgList.Active();

	RecycleInstance();

	NpcStatisticActive();

	g_ChatCenterS.Active();

	PlayerSet.ProcessAutoSave();

	if (PlayerSet.IsNeedSaveMoneyStatistic())
	{
		PlayerSet.SaveMoneyStatistic();
	}

	QuestionManager::Singleton().Active();

	EmployCenter::Singleton().Active();


	//刷怪信息日志
	if (m_NextSaveSpawnInfoTime < UNIX_TMIE_STAMP)
	{
		m_NextSaveSpawnInfoTime = UNIX_TMIE_STAMP + 600;//10分钟记录一次
		
		char spawnInfo[256] = { 0 };
		snprintf(spawnInfo, sizeof(spawnInfo), "Used=%d,Free=%d,Spawn=%u,Recycle=%u,NoSlot=%u",
			NpcSet.GetUsedNpcSlotCount(),
			NpcSet.GetFreeNpcSlotCount(),
			SpawnRecoder::s_SpawnCount,
			KSubWorld::s_RecycleCount,
			NpcSet.m_NpcSlotNotFoundCount);
		spawnInfo[sizeof(spawnInfo) - 1] = 0;
		g_pLogSystem->SysDbgLog(spawnInfo, strlen(spawnInfo), sys_dbg_log_event_npc_spawn_info);
	}

	//向FSEye发送PlayerCount(FSEye Ping)
	if (m_NextPingTime < UNIX_TMIE_STAMP)
	{
		m_NextPingTime = UNIX_TMIE_STAMP + 5;//5秒发送一次

		l2e_PlayerCount playerCount;
		playerCount.Header.Protocol = l2e_header_def;
		playerCount.Protocol = l2e_PlayerCount_def;
		playerCount.PlayerCount = PlayerSet.GetPlayerNumber();
		if (g_pController != NULL)
			g_pController->PushData(protocol_type_guard, NULL, &playerCount, sizeof(playerCount));
	}
	
#else
	
	NpcSet.CheckBalance();
	
	static	KTimer	s_Timer;
	unsigned long uTimeNow = s_Timer.GetElapse();
	KSubWorldSet::s_fScale = (float)(uTimeNow - KSubWorldSet::s_uLastTime)/(float)(1000.0/GAME_FPS);
	if(KSubWorldSet::s_fScale > 10.0f)
	{
		KSubWorldSet::s_fScale = 10.0f;
	}
	else if(KSubWorldSet::s_fScale < 0.03f)
	{
		KSubWorldSet::s_fScale = 0.03f;
	}
	
	KSubWorldSet::s_uLastTime = uTimeNow;
	
	if ( m_bMusic )
	{
		this->m_cMusic.Start();
		this->m_cMusic.Play(SubWorld[0].m_SubWorldID, SubWorld[0].m_dwCurrentTime, FALSE);
	}
	else
		this->m_cMusic.Stop();
	
#endif
}

void KSubWorldSet::Close()
{
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		SubWorld[i].Close();
	}
	NpcSet.RemoveAll();
#ifndef _SERVER
	Player[CLIENT_PLAYER_INDEX].m_ItemList.RemoveAll();
	Player[CLIENT_PLAYER_INDEX].m_cTeam.Release();
	g_TeamViewer.Init();
	g_TeamC[0].Release();
	m_cMusic.Stop();
#endif
}
#ifndef _SERVER
// lixuewu 2004.04.06
//void KSubWorldSet::Paint()
//{
//	SubWorld[0].Paint();
//}
#endif

#ifdef _SERVER
int	KSubWorldSet::GetRevivalID( DWORD dwSubWorldId )
{
	char	szKeyName[32];
	char	szSection[32];
	
	sprintf(szSection, "%d", dwSubWorldId);
	sprintf(szKeyName, "%s", "region");
	
	int nRevivalID;
	if (m_ReviveIniFile.GetInteger(szSection, szKeyName, 0, &nRevivalID))
		return nRevivalID;
	
	return FALSE;
}

BOOL KSubWorldSet::GetRevivalPosFromId(DWORD dwSubWorldId, int nRevivalId, POINT* pPos)
{
	if (!pPos)
		return FALSE;

//	int nIdx = SearchWorld(dwSubWorldId);
//	if (nIdx >= 0)
//	{
	char	szKeyName[32];
	char	szSection[32];
	
	sprintf(szSection, "%d", dwSubWorldId);
	sprintf(szKeyName, "%d", nRevivalId);
	
	int nX = 51200;
	int nY = 102400;
	if (m_ReviveIniFile.GetInteger2(szSection, szKeyName, &nX, &nY))
	{
		pPos->x = nX;
		pPos->y = nY;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
//	}
}

bool KSubWorldSet::NotifyMapIndex(int nMapId, int nIndexInUniverse)
{
	for (int i = 0; i < MAX_SUBWORLD; i++)
	{
		if (SubWorld[i].m_SubWorldID == nMapId &&
			SubWorld[i].m_nIndexInUniverse == 0)
		{
			SubWorld[i].m_nIndexInUniverse = nIndexInUniverse;
			return true;
		}
	}
	return false;
}

#endif

#ifdef _SERVER
int KSubWorldSet::CreateInstance(int worldTemplateId, int creatorPlayerIndex, DWORD lifeTime)
{
	WorldSetting* pSetting = GetWorldSetting(worldTemplateId);
	if (pSetting != NULL)
	{
		//进入条件不满足的玩家也无法创建
		if (IsValidPlayer(creatorPlayerIndex) && !pSetting->IsMatchRequirements(creatorPlayerIndex))
			return INVALID_WORLD_INDEX;

		DWORD expireTime = 0;
		if (lifeTime > 0)
		{
			expireTime = lifeTime + UNIX_TMIE_STAMP;
		}
		else if (pSetting->LifeTime > 0)
		{
			expireTime = pSetting->LifeTime + UNIX_TMIE_STAMP;
		}		

		while (pSetting->RuntimeInfo.ReuseableInstanceCount > 0)//有可重用的副本
		{
			int reuseableInstanceIndex = FindReuseableInstanceSlot(worldTemplateId);
			if (reuseableInstanceIndex != INVALID_WORLD_INDEX)
			{
				DWORD instanceId = NewInstanceId();
				if (instanceId > 0)
				{
					KSubWorld& subworld = SubWorld[reuseableInstanceIndex];
					if (subworld.Reuse())
					{
						subworld.SetInstanceId(instanceId);
						subworld.SetExpireTime(expireTime);

						//设置属主
						if (enSULayer_None != pSetting->NeedOwner)
						{
							if (IsValidPlayer(creatorPlayerIndex))
							{
								switch (pSetting->NeedOwner)
								{
								case enSULayer_Player:
									{
										subworld.SetOwner(creatorPlayerIndex);
									}
									break;
								case enSULayer_Gens:
									{
										SocialUnit *pUnit = GetLeafUnit(creatorPlayerIndex, enSUTplId_Tong);
										if (pUnit != NULL)
										{
											SocialUnit *pShizu = GetUpNUnit(pUnit, enSULayer_Gens);
											if (pShizu != NULL)
											{
												subworld.SetOwnerSocialGUID(pShizu->GetUnitGuid());
												SetSocialInstanceId(pShizu, worldTemplateId, instanceId);
											}
										}
									}
									break;
								case enSULayer_Tong:
									{
										SocialUnit *pUnit = GetLeafUnit(creatorPlayerIndex, enSUTplId_Tong);
										if (pUnit != NULL)
										{
											SocialUnit *pZhuhou = GetUpNUnit(pUnit, enSULayer_Tong);
											if (pZhuhou != NULL)
											{
												subworld.SetOwnerSocialGUID(pZhuhou->GetUnitGuid());
												SetSocialInstanceId(pZhuhou, worldTemplateId, instanceId);
											}
										}
									}
									break;
								}
							}
							else
							{
								CloseInstance(reuseableInstanceIndex);
								return INVALID_WORLD_INDEX;
							}
						}

						pSetting->RuntimeInfo.LivingInstanceCount++;
						pSetting->RuntimeInfo.ReuseableInstanceCount--;

						//记录日志
						if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_instance_create))
						{	
							LogEventParam instanceCreateEvent;
							instanceCreateEvent.event = log_event_instance_create;
							if (IsValidPlayer(creatorPlayerIndex))
							{
								KPlayer& creator = Player[creatorPlayerIndex];
								instanceCreateEvent.param1 = creator.GetGUID();
							}
							snprintf(instanceCreateEvent.param2.data, sizeof(instanceCreateEvent.param2.data), "%d", pSetting->MapId);
							g_pLogSystem->Log(instanceCreateEvent);
						}

						return reuseableInstanceIndex;
					}
					else
					{
						CloseInstance(reuseableInstanceIndex);
						pSetting->RuntimeInfo.ReuseableInstanceCount--;
					}
				}
			}
		}

		if (pSetting->RuntimeInfo.LivingInstanceCount < pSetting->InstanceCountMax)//还没有到达该副本允许数量的上限
		{
			int freeInstanceIndex = FindFreeInstanceSlot();
			if (freeInstanceIndex != INVALID_WORLD_INDEX)
			{
				KSubWorld& subworld = SubWorld[freeInstanceIndex];
				int templateWorldIndex = SearchWorld(worldTemplateId);
				if (templateWorldIndex != INVALID_WORLD_INDEX)
				{
					DWORD instanceId = NewInstanceId();
					if (instanceId > 0)
					{
						if (SubWorld[templateWorldIndex].CopyInstance(freeInstanceIndex))
						{
							subworld.SetInstanceId(instanceId);
							subworld.SetExpireTime(expireTime);
							
							//设置属主
							if (enSULayer_None != pSetting->NeedOwner)
							{
								if (IsValidPlayer(creatorPlayerIndex))
								{
									switch (pSetting->NeedOwner)
									{
									case enSULayer_Player:
										{
											subworld.SetOwner(creatorPlayerIndex);
										}
										break;
									case enSULayer_Gens:
										{
											SocialUnit *pUnit = GetLeafUnit(creatorPlayerIndex, enSUTplId_Tong);
											if (pUnit != NULL)
											{
												SocialUnit *pShizu = GetUpNUnit(pUnit, enSULayer_Gens);
												if (pShizu != NULL)
												{
													subworld.SetOwnerSocialGUID(pShizu->GetUnitGuid());
													SetSocialInstanceId(pShizu, worldTemplateId, instanceId);
												}
											}
										}
										break;
									case enSULayer_Tong:
										{
											SocialUnit *pUnit = GetLeafUnit(creatorPlayerIndex, enSUTplId_Tong);
											if (pUnit != NULL)
											{
												SocialUnit *pZhuhou = GetUpNUnit(pUnit, enSULayer_Tong);
												if (pZhuhou != NULL)
												{
													subworld.SetOwnerSocialGUID(pZhuhou->GetUnitGuid());
													SetSocialInstanceId(pZhuhou, worldTemplateId, instanceId);
												}
											}
										}
										break;
									}
								}
								else
								{
									subworld.Close(false);
									return INVALID_WORLD_INDEX;
								}
							}

							pSetting->RuntimeInfo.CreateInstanceCount++;
							pSetting->RuntimeInfo.LivingInstanceCount++;

							//记录日志
							if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_instance_create))
							{
								LogEventParam instanceCreateEvent;
								instanceCreateEvent.event = log_event_instance_create;
								if (IsValidPlayer(creatorPlayerIndex))
								{
									KPlayer& creator = Player[creatorPlayerIndex];
									instanceCreateEvent.param1 = creator.GetGUID();
								}
								snprintf(instanceCreateEvent.param2.data, sizeof(instanceCreateEvent.param2.data), "%d", pSetting->MapId);
								g_pLogSystem->Log(instanceCreateEvent);
							}

							return freeInstanceIndex;
						}
						else
						{
							subworld.Close(false);
						}
					}
				}
			}
		}
	}

	return INVALID_WORLD_INDEX;
}
#endif

#ifdef _SERVER
int KSubWorldSet::EnterInstance(int playerIndex, int worldTemplateId, int entryIndex, DWORD lifeTime, bool createIfNotExist)
{
	if (IsValidPlayer(playerIndex) && worldTemplateId > 0 && entryIndex >= 0)
	{
		WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(worldTemplateId);
		if (pSetting && pSetting->IsInstance)
		{
			if (entryIndex < pSetting->EntryCount)
			{
				WorldEntryInfo& entryInfo = pSetting->Entrys[entryIndex];
				KPlayer& player = Player[playerIndex];
				KPlayerTeam& teamInfo = player.GetTeamInfo();
				KNpc& npc = Npc[player.GetNpcIndex()];
				
				DWORD instanceId = GetEnterableInstanceID(playerIndex, worldTemplateId);
				
				if (instanceId == INVALID_INSTANCE_ID || g_SubWorldSet.GetInstance(instanceId) == INVALID_WORLD_INDEX)
				{
					if (createIfNotExist)
					{
						int worldIndex = g_SubWorldSet.CreateInstance(worldTemplateId, playerIndex, lifeTime);
						if (worldIndex != INVALID_WORLD_INDEX)
							instanceId = SubWorld[worldIndex].GetInstanceId();
						else
							instanceId = INVALID_INSTANCE_ID;
					}
				}

				if (instanceId != INVALID_INSTANCE_ID)
				{
					player.RememberCurrentPos();

					int posX = entryInfo.PosX * 32;
					int posY = entryInfo.PosY * 32;
					if (npc.ChangeWorld(instanceId, posX, posY, true))
					{
						if (enSULayer_None == pSetting->NeedOwner)
						{
							player.GetInstanceInfo().SetInstanceId(worldTemplateId, instanceId);
							if (teamInfo.IsInTeam())
							{
								KTeam* pTeam = teamInfo.GetTeam();
								if (pTeam != NULL)
								{
									pTeam->SetInstanceId(worldTemplateId, instanceId);
								}
							}
						}

						return TRUE;
					}
				}
			}
		}
	}

	return FALSE;
}
#endif

#ifdef _SERVER
DWORD KSubWorldSet::GetEnterableInstanceID(int playerIndex, int worldTemplateId)
{
	DWORD instanceId = INVALID_INSTANCE_ID;

	if (IsValidPlayer(playerIndex) && worldTemplateId > 0)
	{
		WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(worldTemplateId);
		if (pSetting && pSetting->IsInstance)
		{
			KPlayer& player = Player[playerIndex];
			KPlayerTeam& teamInfo = player.GetTeamInfo();
			KNpc& npc = Npc[player.GetNpcIndex()];
			
			switch (pSetting->NeedOwner)
			{
			case enSULayer_None:
				{
					if (teamInfo.IsInTeam())
					{
						KTeam* pTeam = teamInfo.GetTeam();
						if (pTeam != NULL)
						{
							if (teamInfo.IsCaptain())
							{
								if (player.GetInstanceInfo().GetInstanceId(worldTemplateId) == INVALID_INSTANCE_ID)
									instanceId = pTeam->GetInstanceId(worldTemplateId);
								else
									instanceId = player.GetInstanceInfo().GetInstanceId(worldTemplateId);
							}
							else
							{
								instanceId = pTeam->GetInstanceId(worldTemplateId);
							}
						}
					}
					else
					{
						instanceId = player.GetInstanceInfo().GetInstanceId(worldTemplateId);
					}
				}
				break;
			case enSULayer_Player:
				{
					if (teamInfo.IsInTeam())
					{
						KTeam* pTeam = teamInfo.GetTeam();
						if (pTeam != NULL)
						{
							instanceId = Player[pTeam->GetCaptain()].GetInstanceInfo().GetInstanceId(worldTemplateId);
						}
					}
					else
					{
						instanceId = player.GetInstanceInfo().GetInstanceId(worldTemplateId);
					}
				}
				break;
			case enSULayer_Gens:
				{
					SocialUnit *pUnit = GetLeafUnit(playerIndex, enSUTplId_Tong);
					if (pUnit != NULL)
					{
						SocialUnit *pShizu = GetUpNUnit(pUnit, enSULayer_Gens);
						if (pShizu != NULL)
						{
							instanceId = GetSocialInstanceId(pShizu, worldTemplateId);
						}
					}
				}
				break;
			case enSULayer_Tong:
				{
					SocialUnit *pUnit = GetLeafUnit(playerIndex, enSUTplId_Tong);
					if (pUnit != NULL)
					{
						SocialUnit *pZhuhou = GetUpNUnit(pUnit, enSULayer_Tong);
						if (pZhuhou != NULL)
						{
							instanceId = GetSocialInstanceId(pZhuhou, worldTemplateId);
						}
					}
				}
				break;
			}
		}
	}

	return instanceId;
}
#endif

#ifdef _SERVER
int KSubWorldSet::EnterInstanceByID(int playerIndex, DWORD instanceId, int entryIndex)
{
	int worldIndex = GetInstance(instanceId);
	if (worldIndex != INVALID_WORLD_INDEX)
	{
		KSubWorld& world = SubWorld[worldIndex];
		WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(world.GetWorldTemplateId());
		if (pSetting && pSetting->IsInstance)
		{
			if (entryIndex >= 0 && entryIndex < pSetting->EntryCount)
			{
				WorldEntryInfo& entryInfo = pSetting->Entrys[entryIndex];
				int posX = entryInfo.PosX * 32;
				int posY = entryInfo.PosY * 32;
				
				if (IsValidPlayer(playerIndex))
				{
					KPlayer& player = Player[playerIndex];
					KNpc& npc = Npc[player.GetNpcIndex()];
					
					player.RememberCurrentPos();

					if (npc.ChangeWorld(instanceId, posX, posY, true))
					{
						if (enSULayer_None == pSetting->NeedOwner)
							player.GetInstanceInfo().SetInstanceId(world.GetWorldTemplateId(), instanceId);
						return TRUE;
					}
				}
			}
		}
	}
	return FALSE;
}
#endif

#ifdef _SERVER
int KSubWorldSet::CloseInstanceByID(DWORD instanceId)
{
	int worldIndex = GetInstance(instanceId);
	if (worldIndex != INVALID_WORLD_INDEX)
	{
		return CloseInstance(worldIndex);
	}

	return FALSE;
}
#endif

#ifdef _SERVER
int KSubWorldSet::CloseInstance(int worldIndex)
{
	if (worldIndex >= INSTANCE_SUBWORLD_START && worldIndex < INSTANCE_SUBWORLD_END)
	{
		KSubWorld& world = SubWorld[worldIndex];
		int worldTemplateId = world.m_SubWorldID;
		world.Close();

		WorldSetting* pSetting = GetWorldSetting(worldTemplateId);
		if (pSetting)
			pSetting->RuntimeInfo.LivingInstanceCount--;

		return TRUE;
	}

	return FALSE;
}
#endif

#ifdef _SERVER
int KSubWorldSet::FindFreeInstanceSlot()
{	
	for (int worldIndex = INSTANCE_SUBWORLD_START; worldIndex < INSTANCE_SUBWORLD_END; worldIndex++)
	{
		if (SubWorld[worldIndex].IsFree())
			return worldIndex;
	}

	return INVALID_WORLD_INDEX;
}
#endif

#ifdef _SERVER
int KSubWorldSet::FindReuseableInstanceSlot(int worldTemplateId)
{
	for (int worldIndex = INSTANCE_SUBWORLD_START; worldIndex < INSTANCE_SUBWORLD_END; worldIndex++)
	{
		if (SubWorld[worldIndex].GetState() == world_state_ready_for_reuse && SubWorld[worldIndex].m_SubWorldID == worldTemplateId)
			return worldIndex;
	}

	return INVALID_WORLD_INDEX;
}
#endif

#ifdef _SERVER
void KSubWorldSet::RecycleInstance()
{
	for (int worldIndex = INSTANCE_SUBWORLD_START; worldIndex < INSTANCE_SUBWORLD_END; worldIndex++)
	{
		KSubWorld& subworld = SubWorld[worldIndex];
		if (subworld.GetState() == world_state_idle)
		{
			WorldSetting* pSetting = GetWorldSetting(subworld.m_SubWorldID);
			if (pSetting != NULL)
			{
				DWORD idleTime = subworld.GetIdleTime();
				if (idleTime > pSetting->IdleTimeBeforeRecycle)
				{
					RecycleInstanceNotify(worldIndex);

					if (pSetting->RuntimeInfo.LivingInstanceCount + pSetting->RuntimeInfo.ReuseableInstanceCount > pSetting->InstanceCountMin)
					{
						CloseInstance(worldIndex);
					}
					else
					{
						if (subworld.PrepareForReuse())
						{
							pSetting->RuntimeInfo.LivingInstanceCount--;
							pSetting->RuntimeInfo.ReuseableInstanceCount++;
						}
						else
						{
							CloseInstance(worldIndex);
						}
					}
				}
				else
				{
					static int recycleNotifyTime[] = {
						3600 * GAME_FPS, 
						1800 * GAME_FPS,
						900 * GAME_FPS,
						300 * GAME_FPS,
						180 * GAME_FPS,
						120 * GAME_FPS,
						60 * GAME_FPS,
						45 * GAME_FPS,
						30 * GAME_FPS,
						15 * GAME_FPS,
						10 * GAME_FPS,
						5 * GAME_FPS
					};
					
					int secondToRecycle = pSetting->IdleTimeBeforeRecycle - idleTime;
					for(int i = 0; i < sizeof(recycleNotifyTime) / sizeof(int); i++)
					{
						if (secondToRecycle == recycleNotifyTime[i])
						{
							RecycleInstanceNotify(worldIndex);
							break;
						}
					}
				}
			}
		}
	}
}
#endif

#ifdef _SERVER
int KSubWorldSet::GetInstance(DWORD instanceId)
{
	if (instanceId != INVALID_INSTANCE_ID)
	{
		for (int worldIndex = INSTANCE_SUBWORLD_START; worldIndex < INSTANCE_SUBWORLD_END; worldIndex++)
		{
			KSubWorld& subworld = SubWorld[worldIndex];
			if (subworld.GetInstanceId() == instanceId)
				return worldIndex;
		}
	}

	return INVALID_WORLD_INDEX;
}
#endif

#ifdef _SERVER
WorldSetting::WorldSetting()
{
	memset(Name, 0, sizeof(Name));
	IsInstance = false;
	MapId = 0;
	InstanceCountMin = 0;
	InstanceCountMax = 1;
	IdleTimeBeforeRecycle = GAME_FPS * 60;
	OfflineMode = 0;
	LifeTime = 0;
	
	EntryCount = 0;
	memset((void*)Entrys, 0, sizeof(Entrys));
	
	PlayerCountMax = MAX_BIG_TEAM_MEMBER;
	RequireLevelMin = 1;
	RequireLevelMax = MAX_LEVEL;
	memset(&RequireItem, 0, sizeof(RequireItem));
	RequireBuff = 0;
	for (int i = 0; i < 6; i++)
	{
		PermitProfession[i] = true;
	}
	PermitMale = true;
	PermitFemale = true;		
	PermitSingle = true;
	PermitTeam = true;
	PermitBigTeam = true;
	NeedOwner = 0;
	CanGainSroce = TRUE;
}
#endif

#ifdef _SERVER
bool WorldSetting::IsMatchRequirements(int playerIndex, int worldIndex) const
{
	if (!IsInstance)
		return true;

	if (!IsValidPlayer(playerIndex))
		return false;

	KPlayer& player = Player[playerIndex];
	
	if (!IsValidNpc(player.GetNpcIndex()))
		return false;
	
	KNpc& npc = Npc[player.GetNpcIndex()];
	
	//等级需求
	if (player.GetLevel() < RequireLevelMin || player.GetLevel() > RequireLevelMax)
		return false;
	
	//性别需求
	if ((PermitMale == FALSE) && (npc.GetSex() == sex_male))
		return false;
	if ((PermitFemale == FALSE) && (npc.GetSex() == sex_female))
		return false;
	
	KPlayerTeam& teamInfo = player.GetTeamInfo();
	
	//组队需求
	if (teamInfo.IsInTeam())
	{
		KTeam* pTeam = teamInfo.GetTeam();
		if (pTeam)
		{
			if (pTeam->IsBigTeam())
			{
				if (PermitBigTeam == FALSE)
					return false;
			}
			else
			{
				if (PermitTeam == FALSE)
					return false;
			}
		}
	}
	else
	{
		if (PermitSingle == FALSE)
			return false;
	}
	
	//职业需求
	int roleSeries = player.GetSeries();
	int skillSeries = player.GetSkillSeries();
	if (skillSeries != role_skillseries_invalid)
	{
		if (PermitProfession[(roleSeries * role_skillseries_count) + skillSeries] == FALSE)
			return false;
	}
	else
	{
		if (PermitProfession[(roleSeries * role_skillseries_count)] == FALSE && PermitProfession[(roleSeries * role_skillseries_count) + 1] == FALSE )
			return false;
	}
	
	//物品需求
	const int* idArray = RequireItem.IDArray;
	if (idArray[0] || idArray[1] || idArray[2] || idArray[3])
	{
		if (player.GetItemList().HaveNormalItem(idArray[0], idArray[1], idArray[2], idArray[3]) == 0)
			return false;
	}		
	
	//BUFF需求
	if (RequireBuff > 0)
	{
		if (BuffMgr::Singleton().IsHaveBuff(player.GetNpcIndex(), RequireBuff) == FALSE)
			return false;
	}

	//氏族（/诸侯）副本只有属于氏族（/诸侯）的玩家才能创建
	switch (NeedOwner)
	{
	case enSULayer_Gens:
		{
			bool match = false;
			SocialUnit *pUnit = GetLeafUnit(playerIndex, enSUTplId_Tong);
			if (pUnit != NULL)
			{
				SocialUnit *pShizu = GetUpNUnit(pUnit, enSULayer_Gens);
				if (pShizu != NULL)
				{
					match = true;
				}
			}
			
			if (!match)
			{
				return false;
			}
		}
		break;
	case enSULayer_Tong:
		{
			bool match = false;
			SocialUnit *pUnit = GetLeafUnit(playerIndex, enSUTplId_Tong);
			if (pUnit != NULL)
			{
				SocialUnit *pZhuhou = GetUpNUnit(pUnit, enSULayer_Tong);
				if (pZhuhou != NULL)
				{
					match = true;
				}
			}
			
			if (!match)
			{
				return false;
			}
		}
		break;
	}

	if (worldIndex >=0 && worldIndex < MAX_SUBWORLD)
	{
		KSubWorld& world = SubWorld[worldIndex];

		//总计人数
		if (world.GetPlayerCount() >= PlayerCountMax)
			return false;
		
		//检测拥有者
		switch (NeedOwner)
		{
		case enSULayer_Player:
			{
				if (player.GetInstanceInfo().GetInstanceId(world.GetWorldTemplateId()) != world.GetInstanceId())
				{
					if (player.GetTeamInfo().IsInTeam())
					{
						KTeam* pTeam = player.GetTeamInfo().GetTeam();
						if (pTeam)
						{
							int captainPlayerIndex = pTeam->GetCaptain();
							if (Player[captainPlayerIndex].GetInstanceInfo().GetInstanceId(world.GetWorldTemplateId()) != world.GetInstanceId())
							{
								return false;
							}
						}
					}
					else
					{
						return false;
					}
				}
			}
			break;
		case enSULayer_Gens:
			{
				bool match = false;
				SocialUnit *pUnit = GetLeafUnit(playerIndex, enSUTplId_Tong);
				if (pUnit != NULL)
				{
					SocialUnit *pShizu = GetUpNUnit(pUnit, enSULayer_Gens);
					if (pShizu != NULL)
					{
						if (world.GetOwnerSocialGUID() == pShizu->GetUnitGuid())
						{
							match = true;
						}
					}
				}

				if (!match)
				{
					return false;
				}
			}
			break;
		case enSULayer_Tong:
			{
				bool match = false;
				SocialUnit *pUnit = GetLeafUnit(playerIndex, enSUTplId_Tong);
				if (pUnit != NULL)
				{
					SocialUnit *pZhuhou = GetUpNUnit(pUnit, enSULayer_Tong);
					if (pZhuhou != NULL)
					{
						if (world.GetOwnerSocialGUID() == pZhuhou->GetUnitGuid())
						{
							match = true;
						}
					}
				}

				if (!match)
				{
					return false;
				}
			}
			break;
		}
	}

	return true;
}
#endif

#ifdef _SERVER
void KSubWorldSet::RecycleInstanceNotify(int worldIndex)
{
	if (worldIndex >= INSTANCE_SUBWORLD_START && worldIndex < INSTANCE_SUBWORLD_END)
	{
		KSubWorld& subworld = SubWorld[worldIndex];
		if (subworld.GetState() == world_state_idle)
		{
			WorldSetting* pSetting = GetWorldSetting(subworld.m_SubWorldID);
			if (pSetting != NULL)
			{
				if (enSULayer_Player == pSetting->NeedOwner)
				{
					int owner = subworld.GetOwner();
					if (IsValidPlayer(owner))
					{
						char message[256] = {0};
						if (pSetting->IdleTimeBeforeRecycle > subworld.GetIdleTime())
						{
							DWORD secondToRecycle = (pSetting->IdleTimeBeforeRecycle - subworld.GetIdleTime()) / GAME_FPS;					
							snprintf(message, sizeof(message), MSG_INSTANCE_RECYCLE_NOTIFY_NEAR, pSetting->Name, secondToRecycle);
						}
						else
						{
							snprintf(message, sizeof(message), MSG_INSTANCE_RECYCLE_NOTIFY_DONE, pSetting->Name);
						}
						
						message[sizeof(message) - 1] = 0;
						g_ChatCenterS.SysMsgToSomeone(owner, SYSMSG_TYPE_STR, (const BYTE*)message, strlen(message));
					}
				}
			}
		}
	}
}
#endif

#ifdef _SERVER
void KSubWorldSet::PlayerOffLine(int playerIndex)
{
	//清除所有副本信息中跟PlayerIndex相关的内容
	for (int worldIndex = INSTANCE_SUBWORLD_START; worldIndex < INSTANCE_SUBWORLD_END; worldIndex++)
	{
		KSubWorld& subworld = SubWorld[worldIndex];
		WorldSetting* pSetting = GetWorldSetting(subworld.GetWorldTemplateId());
		if (pSetting && (enSULayer_Player == pSetting->NeedOwner) && subworld.GetOwner() == playerIndex)
		{
			subworld.SetOwner(INVALID_PLAYER_INDEX);
		}
	}
}
#define MAP_SETTING_FILE_PATH "/settings/tongwar.ini"

StatueInfoMgr::StatueInfoMgr()
{
	m_bGlobalNpcLoaded = false;
}

StatueInfoMgr::~StatueInfoMgr()
{
	//
}

StatueInfoMgr& StatueInfoMgr::Singleton()
{
	static StatueInfoMgr sm;
	return sm;
}

void StatueInfoMgr::InitStatueUsedInfo()
{
	KIniFile inifile;
	
	if (inifile.Load(MAP_SETTING_FILE_PATH))
	{
		for (int i = 0; i < MAX_WAR_MAP_NUM; i++)
		{
			char szKeyName[32];
			snprintf(szKeyName, sizeof(szKeyName), "map%d", i + 1);  //TongWarMapID从1开始的
			szKeyName[31] = 0;
			
			int nMapId = INVALID_WORLD_ID;
			inifile.GetInteger("TongWarMapID", szKeyName, INVALID_WORLD_ID, &nMapId);
			
			if (nMapId != INVALID_WORLD_ID)
			{
				m_StatueUsedInfoArray[i].nMapTemplateId = nMapId;
			}
		}
	}
	else
	{
		_ASSERT(0);
	}
}

bool StatueInfoMgr::FillStatueUsedInfo(int nMapId, int nNpcIndex)
{
	int i = GetIterByMapId(nMapId);
	
	if (i < MAX_WAR_MAP_NUM && m_StatueUsedInfoArray[i].nMapTemplateId != INVALID_WORLD_ID)
	{
		_ASSERT(!IsValidNpc(m_StatueUsedInfoArray[i].nNpcIndex));

		if (IsValidNpc(nNpcIndex) && !IsValidNpc(m_StatueUsedInfoArray[i].nNpcIndex))
		{
			m_StatueUsedInfoArray[i].nNpcIndex = nNpcIndex;
			return true;
		}//endif

	}//endif

	return false;
}


void StatueInfoMgr::DeleteStatue(int nNpcIndex, int iter)
{
	if (!IsValidNpc(nNpcIndex))
		return ;

	if (iter < 0 || iter >= MAX_WAR_MAP_NUM )
		return ;

	Npc[nNpcIndex].m_UnaryAttrMgr.Set(nuai_curlife, 0);
	Npc[nNpcIndex].SendCommand(do_death, 0, 0, 0);
	Npc[nNpcIndex].GetController().SetActive(false);
	Npc[nNpcIndex].SetProcessAI(TRUE);	
	Npc[nNpcIndex].ProcCommand(TRUE);

	int nSubWorld = Npc[nNpcIndex].m_SubWorldIndex;
	int nRegion = Npc[nNpcIndex].m_RegionIndex;
	
	BuffMgr &mgr = BuffMgr::Singleton();
	mgr.ClearAllBuff( nNpcIndex, TRUE );
	
	SubWorld[nSubWorld].m_Region[nRegion].RemoveNpc(nNpcIndex);
	NpcSet.Remove(nNpcIndex);

	m_StatueUsedInfoArray[iter].ClearData();
}

void StatueInfoMgr::SendStatueInfoToClient(int nNpcIndex, int nPlayerIndex, DWORD dwtime, int nHasBuff)
{
	if (IsValidNpc(nNpcIndex) && IsValidPlayer(nPlayerIndex))
	{
		const FSGUID& temp = Npc[nNpcIndex].GetLord(); //AddNpc中SetLord将Lord置为玩家的GUID
		ServerSocialUnitMgr& ssum = ServerSocialUnitMgr::Singleton();
		SocialUnit* pUnit = ssum.GetUnit(temp);
		if (pUnit)
		{
			_StatueInfo TempSI;
			ZeroMemory(&TempSI, sizeof(_StatueInfo));
			TempSI.Protocol       = s2c_byte_extend;
			TempSI.ProtocolExtend = s2c_ex_protocol_statue_info;
			TempSI.wProtocolSize  = sizeof(_StatueInfo) - 1;
			strncpy(TempSI.cName, pUnit->GetOwnerName(), sizeof(TempSI.cName));
			TempSI.cName[sizeof(TempSI.cName) - 1] = '\0';

			const char* pName = GetUnitName(pUnit->GetUnitAttr());
			if (!pName)
				return;

			strncpy(TempSI.cTongName, pName, sizeof(TempSI.cTongName));
			TempSI.cTongName[sizeof(TempSI.cTongName) - 1] = '\0';
			TempSI.nMapId = GetCityMapId(pUnit->GetUnitAttr());

			TempSI.dwtime = dwtime;

			TempSI.HasBuff = (nHasBuff != 0);

			
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, (void*)&TempSI, sizeof(TempSI));
		}
		
	}
}
#define INTERVAL_BREATHE     GAME_FPS
void StatueInfoMgr::Breathe()
{
	if (g_SubWorldSet.GetGameTime() % INTERVAL_BREATHE == 0)
		ProcInvalidStatue();
}

void StatueInfoMgr::ProcInvalidStatue()
{
	if (IsGlobalNpcLoaded())	
	{
		for (int i = 0; i < MAX_WAR_MAP_NUM; i++)
		{
			int nNpcIndex = m_StatueUsedInfoArray[i].nNpcIndex;
			if (!IsValidNpc(nNpcIndex))
				continue;
			
			if (IsStatueNeedClear(i))
			{
				DeleteStatue(nNpcIndex, i);
			}
		}//end for
	}
}

bool StatueInfoMgr::IsStatueNeedClear(int iter) //注意，本函数是在GetGlobalTongWarMgr().IsInitedAll()的情况下调用的，这时前提！否者取不到lord的guid
{
	if(iter < 0 || iter >= MAX_WAR_MAP_NUM )
		return false;

	int nStatueNpcIndex = m_StatueUsedInfoArray[iter].nNpcIndex;
	if (!IsValidNpc(nStatueNpcIndex))
		return false;

	int nMapId			= m_StatueUsedInfoArray[iter].nMapTemplateId;
	int nSubWorldIndex  = g_SubWorldSet.SearchWorld(nMapId);

	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
		return false;

	if (Npc[nStatueNpcIndex].GetLoadSaveState() != npc_load_save_state_loaded) //否则取不到statue的guid
		return false;

	KTongWarManager& km = GetGlobalTongWarMgr();
	if (!km.IsInitedAll())														//否则取不到lord的guid
		return false;
	
	int nLordNpcIndex = SubWorld[nSubWorldIndex].GetLord();
	if (IsValidNpc(nLordNpcIndex))
	{
		const FSGUID& guidlord   = Npc[nLordNpcIndex].GetLord();
		const FSGUID& guidstatue = Npc[nStatueNpcIndex].GetLord();
		
		if (guidlord != guidstatue || (guidlord.data[0] == 0 && guidstatue.data[0] != 0)) //有雕像必然有城市lord=>没有lord肯定没有雕像
		{
			return true;
		}
	}
	
	return false;
}

int StatueInfoMgr::CanChangeLord(int nPlayerIndex)
{
	if (!IsGlobalNpcLoaded())
		return database_busyness;

	if (!IsValidPlayer(nPlayerIndex))
		return not_have_purview;

	int nNpcIndex = Player[nPlayerIndex].GetNpcIndex();

	if (!IsValidNpc(nNpcIndex))
		return not_have_purview;
	
	bool switnch = false;
	int mapid = INVALID_WORLD_ID;

	//1.找到最高层社会关系节点
	SocialUnit* pCityUnit = GetLeafUnit(nPlayerIndex, enSUTplId_Tong);
	
	while (pCityUnit)
	{
		mapid = GetCityMapId(pCityUnit->GetUnitAttr());

		if (mapid != INVALID_WORLD_ID )
		{
			switnch = true;
			break;
		}//endif
		
		pCityUnit = pCityUnit->GetParent();
	}//end while

	//2.开始判断

	//没有占有城市
	if (!switnch)
		return not_have_purview;
	
	//不是盟主
	if (!pCityUnit->IsOwner(Player[nPlayerIndex].GetPlayerName()))
		return not_have_purview;
	
	//已经树立过雕像
	if (HasStatue(mapid))		
		return statue_already_establish;
						
	//占有地图是过占地图 
	if ( GetIterByMapId(mapid)>= MAX_WAR_MAP_NUM )
		return not_have_purview;

	return mapid;
}
	
int StatueInfoMgr::GetIterByMapId(int nMapId)
{
	int i;
	for ( i = 0; i < MAX_WAR_MAP_NUM; i++)
	{
		if (m_StatueUsedInfoArray[i].nMapTemplateId == nMapId)
			return i;
	}
	
	return MAX_WAR_MAP_NUM;
}

bool StatueInfoMgr::HasStatue(int nMapId)
{
	int i = GetIterByMapId(nMapId);

	if (i < MAX_WAR_MAP_NUM)
		return m_StatueUsedInfoArray[i].nNpcIndex != -1;
	else
		return false;
}

void StatueInfoMgr::ForceClearStatue(int nMapId)
{
	if (IsGlobalNpcLoaded())
	{	
		int iter = GetIterByMapId(nMapId);
		if (iter < MAX_WAR_MAP_NUM)
		{
			int nNpcIndex = m_StatueUsedInfoArray[iter].nNpcIndex;
			if (IsValidNpc(nNpcIndex))
			{
				DeleteStatue(nNpcIndex, iter);
			}//endif

		}//endif
	
	}//endif

}

void StatueInfoMgr::SetGlobalNpcLoaded()
{
	m_bGlobalNpcLoaded = true;

	for (int i = 0; i <= MAX_WAR_MAP_NUM; ++i)
	{
		int nNpcIndex = m_StatueUsedInfoArray[i].nNpcIndex;
		if (IsValidNpc(nNpcIndex))
			NpcSave::LoadNpc(nNpcIndex);
	}
}

void StatueInfoMgr::ReStoreStatue(int nMapId)
{
	if (nMapId == INVALID_WORLD_ID)
		return ;

	int iter = GetIterByMapId(nMapId);
	if (iter >= MAX_WAR_MAP_NUM)
		return ;

	int nNpcIndex = m_StatueUsedInfoArray[iter].nNpcIndex;

	if (!IsValidNpc(nNpcIndex))
		return;

	NpcSave::SaveNpc(nNpcIndex);
}
#endif