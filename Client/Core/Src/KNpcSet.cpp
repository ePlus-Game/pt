#include "KCore.h"
#include <math.h>
#include "KNpc.h"
#include "KSubWorld.h"
#include "KRegion.h"
#include "GameDataDef.h"
#include "KNpcSet.h"
#include "KPlayer.h"
#ifndef _SERVER
#include "CoreShell.h"
#include "Scene\KScenePlaceC.h"
#include "iRepresentshell.h"
#include "KOption.h"
#include "networkinterface.h"
#include "ChatCenter_C.h"
#else
#include "ServerSocialUnitMgr.h"
#endif
#include "Scene/SceneDataDef.h"
#include "KMath.h"
#include "KDirtyNpcSet.h"
#include "ConfigManager.h"
#include "KItemChangeRes.h"

//#define	ENCHANT_SETTING_PATH	"settings\\npc"
//#define	ENCHANT_NORMAL_FILE		"normalunique.txt"
//#define	ENCHANT_SPECIAL_FILE	"speicalunique.txt"

#define		FILE_PKRELATION_TABLE	"settings/pkrelation.txt"

KNpcSet	NpcSet;

KNpcSet::KNpcSet()
{
	m_dwIDCreator = 1000;
}

// 黄金怪物设定文件的初始化没有放在这里，直接放在 core 的初始化里面
void KNpcSet::Init()
{
    if( !GenPKRelationTbl() )
		_ASSERT(0);

	m_FreeIdx.Init(MAX_NPC-MAX_PLAYER);
	m_UseIdx.Init(MAX_NPC-MAX_PLAYER);

	// Add by Cooler -->
	// 2006-5-26 15:50
	m_FreePlayerIdx.Init(MAX_PLAYER);
	m_UsePlayerIdx.Init(MAX_PLAYER);
	// End add by Cooler <--

	// 开始时所有的数组元素都为空
	int i = 0;
	for (i = MAX_NPC - 1; i > MAX_PLAYER; i--)
	{
		m_FreeIdx.Insert(i);
		Npc[i].m_Node.m_nIndex = i;
	}

	for (i = MAX_PLAYER - 1; i > 0; i--)
	{
		m_FreePlayerIdx.Insert(i);
		Npc[i].m_Node.m_nIndex = i;
	}

	LoadPlayerBaseValue(PLAYER_BASE_VALUE);

#ifdef _SERVER
	KIniFile	cPKIni;
//	g_SetFilePath("\\");
	if (cPKIni.Load(PLAYER_PK_RATE_FILE))
	{
		cPKIni.GetInteger("PK", "rate", 20, &m_nPKDamageRate);
		cPKIni.GetInteger("PK", "FactionPKFaction", 1, &m_nFactionPKFactionAddPKValue);
		cPKIni.GetInteger("PK", "KillerPKFaction", 1, &m_nKillerPKFactionAddPKValue);
		cPKIni.GetInteger("PK", "EnmityPK", 2, &m_nEnmityAddPKValue);
		cPKIni.GetInteger("PK", "BeKilled", -1, &m_nBeKilledAddPKValue);
		cPKIni.GetInteger("PK", "LevelDistance", 25, &m_nLevelDistance);
	}
	else
	{
		m_nPKDamageRate = 20;
		m_nFactionPKFactionAddPKValue = 1;
		m_nKillerPKFactionAddPKValue = 1;
		m_nEnmityAddPKValue = 2;
		m_nBeKilledAddPKValue = -1;
		m_nLevelDistance = 25;
	}
#endif

#ifndef _SERVER
	m_nShowPateFlag = PATE_CHAT;
	m_nShowPateFlag |= PATE_NAME;
	m_nShowPateFlag |= PATE_LIFE;

	m_nNpcPaitFlag = SHOW_NPC;
	m_nNpcPaitFlag |= SHOW_PLAYER;
	
	ZeroMemory(m_RequestNpc, sizeof(m_RequestNpc));
	m_RequestFreeIdx.Init(MAX_NPC_REQUEST);
	m_RequestUseIdx.Init(MAX_NPC_REQUEST);

	for (i = MAX_NPC_REQUEST - 1; i > 0; i--)
	{
		m_RequestFreeIdx.Insert(i);
	}

	RemoveAllSyncToWorldNpcs();
#endif

#ifdef _SERVER
	m_NpcSlotNotFoundCount = 0;
#endif
}

void KNpcSet::LoadPlayerBaseValue(LPSTR szFile)
{
	KIniFile	File;

	File.Load(szFile);

	File.GetInteger("Common", "HurtFrame", 12, &m_cPlayerBaseValue.nHurtFrame);
	File.GetInteger("Common", "RunSpeed", 10, &m_cPlayerBaseValue.nRunSpeed);
	File.GetInteger("Common", "WalkSpeed", 5, &m_cPlayerBaseValue.nWalkSpeed);
	File.GetInteger("Common", "AttackFrame", GAME_FPS, &m_cPlayerBaseValue.nAttackFrame);
	File.GetInteger("Common", "CastFrame", GAME_FPS, &m_cPlayerBaseValue.nCastFrame);
	File.GetInteger("Male", "RunFrame", 15, &m_cPlayerBaseValue.nRunFrame[0]);
	File.GetInteger("Female", "RunFrame", 15, &m_cPlayerBaseValue.nRunFrame[1]);
#ifndef _SERVER
	File.GetInteger("Male", "WalkFrame", 15, &m_cPlayerBaseValue.nWalkFrame[0]);
	File.GetInteger("Female", "WalkFrame", 15, &m_cPlayerBaseValue.nWalkFrame[1]);
	File.GetInteger("Male", "StandFrame", 15, &m_cPlayerBaseValue.nStandFrame[0]);
	File.GetInteger("Female", "StandFrame", 15, &m_cPlayerBaseValue.nStandFrame[1]);
#endif
	File.Clear();
}

BOOL KNpcSet::IsNpcExist(int nIdx, DWORD dwId)
{
	if (Npc[nIdx].m_dwID == dwId)
		return TRUE;
	else
		return FALSE;
}

int	KNpcSet::SearchID(DWORD dwID)
{
	int nIdx = 0;

#ifdef _SERVER
	INDEXIDMAP::iterator it =  m_NpcUseIndex.find(dwID);
	if ( it != m_NpcUseIndex.end( ) )
		return it->second;
#else
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_dwID == dwID)
			return nIdx;
	}
#endif

	// Add by Cooler -->
	// 2006-5-26 16:33
	nIdx = 0;
	while( TRUE )
	{
		nIdx = m_UsePlayerIdx.GetNext(nIdx);
		if(nIdx == 0)
			break;

		if(Npc[nIdx].m_dwID == dwID)
			return nIdx;
	}
	// End add by Cooler <--

	return 0;
}

#ifndef _SERVER
//---------------------------------------------------------------------------
//	功能：查找某个ClientID的npc是否存在
//---------------------------------------------------------------------------
int		KNpcSet::SearchClientID(KClientNpcID sClientID)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_sClientNpcID.m_dwRegionID == sClientID.m_dwRegionID &&
			Npc[nIdx].m_sClientNpcID.m_nNo == sClientID.m_nNo)
			return nIdx;
	}

	// Add by Cooler -->
	// 2006-5-26 16:34
	nIdx = 0;
	while( TRUE )
	{
		nIdx = m_UsePlayerIdx.GetNext(nIdx);
		if(nIdx == 0)
			break;

		if(Npc[nIdx].m_sClientNpcID.m_dwRegionID == sClientID.m_dwRegionID && 
			Npc[nIdx].m_sClientNpcID.m_nNo == sClientID.m_nNo)
		{
			return nIdx;
		}
	}
	// End add by Cooler <--

	return 0;
}
#endif

int KNpcSet::SearchName(LPSTR szName)
{
	int nIdx = 0;
#ifdef _SERVER
	while( TRUE )
	{
		nIdx = m_UsePlayerIdx.GetNext(nIdx);
		if(nIdx == 0)
			break;

		if(g_StrCmp(Npc[nIdx].Name, szName))
			return nIdx;
	}
#else
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (g_StrCmp(Npc[nIdx].Name, szName))
			return nIdx;
	}

	// Add by Cooler -->
	// 2006-5-26 16:36
	nIdx = 0;
	while( TRUE )
	{
		nIdx = m_UsePlayerIdx.GetNext(nIdx);
		if(nIdx == 0)
			break;

		if(g_StrCmp(Npc[nIdx].Name, szName))
			return nIdx;
	}
	// End add by Cooler <--
#endif
	return 0;
}

int KNpcSet::SearchNameID(DWORD dwID)
{
	// Modify by Cooler -->
	// 2006-5-26 16:39
/*	int		nIdx = 0;
	DWORD	dwNameID = 0;
	while(1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		dwNameID = g_FileName2Id(Npc[nIdx].Name);
		if (dwID == dwNameID)
			return nIdx;
	}*/
	int		nIdx = 0;
	DWORD	dwNameID = 0;
	while( TRUE )
	{
		nIdx = m_UsePlayerIdx.GetNext(nIdx);
		if(nIdx == 0)
			break;

		dwNameID = g_FileName2Id(Npc[nIdx].Name);
		if(dwID == dwNameID)
			return nIdx;
	}
	// End modify by Cooler <--

	return 0;
}

int KNpcSet::FindFree(bool bPlayer)
{
	if(bPlayer)
	{
		return m_FreePlayerIdx.GetNext(0);
	}
	else
	{
		return m_FreeIdx.GetNext(0);
	}
}

#ifndef _SERVER
//---------------------------------------------------------------------------
//	功能：添加一个客户端npc（需要设定ClientNpcID）
//---------------------------------------------------------------------------
int		KNpcSet::AddClientNpc(int nTemplateID, int nRegionX, int nRegionY, int nMpsX, int nMpsY, int nNo)
{
	int		nNpcNo, nNpcSettingIdxInfo, nMapX, nMapY, nOffX, nOffY, nRegion;

	nNpcSettingIdxInfo = MAKELONG(1, nTemplateID);
	SubWorld[0].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;
	nNpcNo = this->Add(nNpcSettingIdxInfo, 0, nRegion, nMapX, nMapY, false, nOffX, nOffY);
	if (nNpcNo > 0)
	{
		Npc[nNpcNo].m_sClientNpcID.m_dwRegionID = MAKELONG(nRegionX, nRegionY);
		Npc[nNpcNo].m_sClientNpcID.m_nNo = nNo;
		Npc[nNpcNo].m_RegionIndex = nRegion;
		Npc[nNpcNo].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
		Npc[nNpcNo].m_bClientOnly = TRUE;
		Npc[nNpcNo].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
		//lixuewu 2006.05.23 取消NPC阻挡
		//SubWorld[0].m_Region[nRegion].DecNpcRef(Npc[nNpcNo].m_MapX, Npc[nNpcNo].m_MapY, nNpcNo);
	}

	return nNpcNo;
}

void	KNpcSet::SetUiLoginDisplayerNpc( UI_DISPLAYER_NPC_SYNC* NpcSync )
{
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].Load(NpcSync->NpcSettingIdx, NpcSync->nLevel );
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_Kind	= NpcSync->m_btKind;
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_Height	= 0;			
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_Index	= -1;
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_DataRes.SetPart(BODY_PART_WEAPON, NpcSync->nWeapon, Default_PalIndex, true);
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_DataRes.SetPart(BODY_PART_HELM, NpcSync->nHelm, Default_PalIndex, true);
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_DataRes.SetPart(BODY_PART_ARMOR, NpcSync->nArmor, Default_PalIndex, true);				
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_DataRes.SetPart(BODY_PART_SHOULDER, NpcSync->nShoulder, Default_PalIndex, true);
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_DataRes.SetPart(BODY_PART_CUFF, NpcSync->nCuff, Default_PalIndex, true);
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_DataRes.SetPart(BODY_PART_BOOT, NpcSync->nBoot, Default_PalIndex, true);
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_DataRes.SetPart(BODY_PART_HORSE, NpcSync->nHorse, Default_PalIndex, true);
	Npc[CLIENT_UI_LOGIN_DISPLAY_NPC_INDEX].m_DataRes.SetRideHorse(NpcSync->bRideHorse, true);
}

int		KNpcSet::SearchPet(int nClientID)
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind == kind_pet &&
			Npc[nIdx].m_sClientNpcID.m_nNo == nClientID)
			return nIdx;
	}

	return 0;
}
#endif

int KNpcSet::Add(int nSubWorld, void* pNpcInfo, bool bPlayer)
{
	KSPNpc*	pKSNpcInfo = (KSPNpc *)pNpcInfo;

	int nMpsX = pKSNpcInfo->nPositionX;
	int nMpsY = pKSNpcInfo->nPositionY;
	int	nNpcSettingIdxInfo = MAKELONG(pKSNpcInfo->nLevel, pKSNpcInfo->nTemplateID);

	int nRet = Add(nNpcSettingIdxInfo, nSubWorld, nMpsX, nMpsY, bPlayer);
	
	if (nRet)
	{
		Npc[nRet].m_TrapScriptID = 0;
// 		g_StrCpyLen(Npc[nRet].Name, pKSNpcInfo->szName, sizeof(Npc[nRet].Name));
// 		Npc[nRet].m_Kind = pKSNpcInfo->shKind;
// 		Npc[nRet].m_Camp = pKSNpcInfo->cCamp;
// 		Npc[nRet].m_CurrentCamp = pKSNpcInfo->cCamp;
// 		Npc[nRet].m_Series = pKSNpcInfo->cSeries;

// 		if (pKSNpcInfo->szScript[0])
// 		{
// 			if (pKSNpcInfo->szScript[0] == '.')
// 				g_StrCpyLen(Npc[nRet].ActionScript, &pKSNpcInfo->szScript[1], sizeof(Npc[nRet].ActionScript));
// 			else
// 				g_StrCpyLen(Npc[nRet].ActionScript, pKSNpcInfo->szScript, sizeof(Npc[nRet].ActionScript));
// 			// 保持小写，保证脚本对应关系
// 			g_StrLower(Npc[nRet].ActionScript);
// 			Npc[nRet].m_ActionScriptID = g_FileName2Id(Npc[nRet].ActionScript);
// 			g_DebugLog("[Script]Npc %s,%d", Npc[nRet].ActionScript, Npc[nRet].m_ActionScriptID);
// 		}
// 		else
// 		{
// 			Npc[nRet].m_ActionScriptID = 0;
// 		}
	}
	return nRet;
}

int KNpcSet::Add(int nNpcSettingIdxInfo, int nSubWorld, int nMpsX, int nMpsY, bool bPlayer)
{
	int nRegion, nMapX, nMapY, nOffX, nOffY;
	if (nSubWorld < 0 || nSubWorld >= MAX_SUBWORLD)
		return 0;
	SubWorld[nSubWorld].Mps2Map(nMpsX, nMpsY, &nRegion, &nMapX, &nMapY, &nOffX, &nOffY);
	if (nRegion < 0)
		return 0;
	return Add(nNpcSettingIdxInfo, nSubWorld, nRegion, nMapX, nMapY, bPlayer, nOffX, nOffY);
}

#include "buff_man.h"
int KNpcSet::Add(int nNpcSettingIdxInfo, int nSubWorld, 
				 int nRegion, int nMapX, int nMapY, bool bPlayer /* = FALSE */, int nOffX /* = 0 */, int nOffY /* = 0 */)
{
	int i = FindFree(bPlayer);

	if (i == 0)
	{
#ifdef _SERVER
		m_NpcSlotNotFoundCount++;
#endif
		return 0;
	}

#ifndef _SERVER
	Npc[i].m_sClientNpcID.m_dwRegionID = 0;
	Npc[i].m_sClientNpcID.m_nNo = -1;
	Npc[i].Remove();
#else
//	Npc[i].Remove();
	//Npc[i].Init( );
	Npc[i].m_Command.CmdKind = do_none;
	BuffMgr &mgr = BuffMgr::Singleton();
	mgr.ClearAllBuff( i, TRUE );
#endif

	int nNpcSettingIdx = (short)HIWORD(nNpcSettingIdxInfo);// >> 7; //除于128
	int nLevel = LOWORD(nNpcSettingIdxInfo);// & 0x7f; 

	Npc[i].m_Index = i;	
	Npc[i].m_SkillList.SetNpcIdx(i);
#ifdef _SERVER
	Npc[i].GetController().Init(i);	
#endif
	Npc[i].Load(nNpcSettingIdx, nLevel);
	Npc[i].m_SubWorldIndex = nSubWorld;
	Npc[i].m_RegionIndex = nRegion;
#ifndef _SERVER	
	Npc[i].GetTalismanNpcController().Init(i);
	Npc[i].GetCombatInfoShower().Init(i);
	if (nRegion >= 0 && nRegion < 9)
		Npc[i].m_dwRegionID = SubWorld[nSubWorld].m_Region[nRegion].m_RegionID;
#endif
	Npc[i].m_MapX = nMapX;
	Npc[i].m_MapY = nMapY;
	Npc[i].m_OffX = nOffX;
	Npc[i].m_OffY = nOffY;
	
	SubWorld[nSubWorld].Map2Mps(nRegion, nMapX, nMapY, nOffX, nOffY, &Npc[i].m_OriginX, &Npc[i].m_OriginY);

#ifdef _SERVER
	SetID(i);
#endif
	// 修改可用与使用表
	if(bPlayer)
	{
		m_FreePlayerIdx.Remove(i);
		m_UsePlayerIdx.Insert(i);
	}
	else
	{
		m_FreeIdx.Remove(i);
		m_UseIdx.Insert(i);
#ifdef _SERVER
		m_NpcUseIndex[Npc[i].m_dwID] = i;
#endif
	}
	SubWorld[nSubWorld].m_Region[nRegion].AddNpc(i);//m_WorldMessage.Send(GWM_NPC_ADD, nRegion, i);
	//lixuewu 2006.05.23 取消NPC阻挡
	//SubWorld[nSubWorld].m_Region[nRegion].AddNpcRef(nMapX, nMapY , i);

#ifdef _SERVER	
	Npc[i].SetupEventBuff(NpcEvent_Revive);
#else
	Npc[i].m_dwRegionID = SubWorld[nSubWorld].m_Region[nRegion].m_RegionID;	
#endif

	return i;
}

#ifdef _SERVER

void KNpcSet::Remove(int nIdx, BOOL bPlayer)
{
	if(bPlayer && (nIdx <= 0 || nIdx >= MAX_PLAYER))
	{
		return;
	}
	else if(nIdx <= 0 || nIdx >= MAX_NPC)
	{
		return;
	}

	NPC_REMOVE_SYNC	NetCommand;

	NetCommand.ProtocolType = (BYTE)s2c_npcremove;
	NetCommand.ID = Npc[nIdx].m_dwID;

	int nSubWorld = Npc[nIdx].m_SubWorldIndex;
	int nRegion = Npc[nIdx].m_RegionIndex;
	
	if (nSubWorld >= 0 && nSubWorld <= MAX_SUBWORLD && nRegion >= 0 && nRegion <= SubWorld[nSubWorld].m_nTotalRegion)
	{	
		int nMaxCount = MAX_BROADCAST_COUNT_OPTIMIZED;
		SubWorld[nSubWorld].BroadCastRegion(&NetCommand, sizeof(NetCommand), nMaxCount, nRegion, Npc[nIdx].GetMapX(), Npc[nIdx].GetMapY());
	}

	if (!bPlayer)
	{
		m_NpcUseIndex.erase(Npc[nIdx].m_dwID);
	}

	Npc[nIdx].m_SkillList.Clear();
	Npc[nIdx].Remove();

	if(bPlayer)
	{
		m_FreePlayerIdx.Insert(nIdx);
		m_UsePlayerIdx.Remove(nIdx);
	}
	else
	{
		m_FreeIdx.Insert(nIdx);
		m_UseIdx.Remove(nIdx);
	}
}

#else

void KNpcSet::Remove(int nIdx, bool bServerRemove, BOOL bPlayer)
{
	if(bPlayer && (nIdx <= 0 || nIdx >= MAX_PLAYER))
	{
		return;
	}
	else if(nIdx <= MAX_PLAYER || nIdx >= MAX_NPC)
	{
		return;
	}

	if (Npc[nIdx].m_Kind == kind_player)
		g_TeamViewer.RemoveNpc(nIdx);

	//Enymy go away notify
	if (Npc[nIdx].m_Kind == kind_player && Npc[nIdx].Name[0]!=0)
	{
		bool bIsEnymy=g_ChatCenterC.IsObjectInGroup(Npc[nIdx].Name,GROUPID_ENEMY);
		bool bIsEnymyOnline = g_ChatCenterC.IsObjectOnline(Npc[nIdx].Name);

		if  (bIsEnymy && bIsEnymyOnline)
		{
			char szName[256];
			szName[0]=0;
			sprintf(szName,ENYMY_IS_AWAY,Npc[nIdx].Name);
            CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)szName, 0);     
		}//endif
		
	}//endif

	Npc[nIdx].m_SkillList.Clear();
	
// 	if ((Npc[nIdx].m_Kind == kind_building)&&(bServerRemove))
// 		g_DirtyNpcSet.RemoveItem(Npc[nIdx].m_dwID);

	Npc[nIdx].Remove();
	if ( nIdx == Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetTargetNpc() )
	{
		CoreDataChanged( GDCNI_SEL_TARGET, false, NULL );
		Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].SetTarget(type_npc, 0);
	}

	m_FreeIdx.Insert(nIdx);
	m_UseIdx.Remove(nIdx);
}

#endif
// <-- Rocker End

void KNpcSet::RemoveAll()
{
#ifdef _SERVER
	m_NpcUseIndex.clear();
#endif

	int nIdx = m_UseIdx.GetNext(0);
	int nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UseIdx.GetNext(nIdx);
		Npc[nIdx].m_SkillList.Clear();
		Npc[nIdx].Remove();
		m_FreeIdx.Insert(nIdx);
		m_UseIdx.Remove(nIdx);
		nIdx = nIdx1;
	}

	// Add by Cooler -->
	// 2006-5-26 15:58
	nIdx = m_UsePlayerIdx.GetNext(0);
	nIdx1 = 0;
	while(nIdx)
	{
		nIdx1 = m_UsePlayerIdx.GetNext(nIdx);
		Npc[nIdx].m_SkillList.Clear();
		Npc[nIdx].Remove();
		m_FreePlayerIdx.Insert(nIdx);
		m_UsePlayerIdx.Remove(nIdx);
		nIdx = nIdx1;
	}
	// End add by Cooler <--
}

#ifndef _SERVER
//---------------------------------------------------------------------------
//	功能：从npc数组中寻找属于某个region的 client npc ，添加进去
//---------------------------------------------------------------------------
void	KNpcSet::InsertNpcToRegion(int nRegionIdx)
{
	if (nRegionIdx < 0 || nRegionIdx >= MAX_REGION)
		return;
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;

		if (Npc[nIdx].m_sClientNpcID.m_dwRegionID > 0 && Npc[nIdx].m_dwRegionID == (DWORD)SubWorld[0].m_Region[nRegionIdx].m_RegionID)
		{
			SubWorld[0].m_Region[nRegionIdx].AddNpc(nIdx);
			Npc[nIdx].m_RegionIndex = nRegionIdx;
			Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegionIdx].m_RegionID;
			Npc[nIdx].m_SyncSignal = SubWorld[0].m_dwCurrentTime;
			Npc[nIdx].SendCommand(do_stand);
		}
	}
}
#endif


void KNpcSet::SetID(int m_nIndex)
{
	if (m_nIndex <= 0 || m_nIndex >= MAX_NPC)
		return;

	Npc[m_nIndex].m_dwID = m_dwIDCreator;
	m_dwIDCreator++;
}

int KNpcSet::GetDistance(int nIdx1, int nIdx2)
{
	int nRet = GetDistanceSquare(nIdx1, nIdx2);

	if(-1 == nRet)
		return 0x7fffffff;

	// Fixed By Rocker 2004.03.22 优化平方根
	INTORFLOAT tmp;
	tmp.f = qsqrt((float)(nRet));
	tmp.f += bias.f;
	tmp.i -= bias.i;
	nRet = tmp.i;
	//nRet = (int)sqrt((float)(XOff * XOff + YOff * YOff));
	return nRet;	
	// Fixed By Rocker 2004.03.22 优化平方根
}

int	KNpcSet::GetDistance(int nSrcX, int nSrcY, int nTargetIdx)
{
	int nRet = GetDistanceSquare(nSrcX, nSrcY, nTargetIdx);

	if(-1 == nRet)
		return 0x7fffffff;

	INTORFLOAT tmp;
	tmp.f = qsqrt((float)(nRet));
	tmp.f += bias.f;
	tmp.i -= bias.i;
	nRet = tmp.i;
	return nRet;	
}

int		KNpcSet::GetDistanceSquare(int nSrcX, int nSrcY, int nTargetIdx)
{
	if(nTargetIdx < 0)
		return -1;

	int nDesX, nDesY;

	SubWorld[Npc[nTargetIdx].m_SubWorldIndex].Map2Mps(Npc[nTargetIdx].m_RegionIndex, 
		Npc[nTargetIdx].GetMapX(),
		Npc[nTargetIdx].GetMapY(),
		Npc[nTargetIdx].GetOffX(),
		Npc[nTargetIdx].GetOffY(),
		&nDesX,
		&nDesY);

	int	nOffX = nDesX - nSrcX;
	int nOffY = nDesY - nSrcY;

	return nOffX * nOffX + nOffY * nOffY;
}
#ifndef _SERVER
int	KNpcSet::GetDistanceByMousePt(int nMouseX, int nMouseY, int nTargetIdx)
{
	int	nRet = 0;
	
	int XOff = 0;
	int YOff = 0;

	int X1 = nMouseX;
	int Y1 = nMouseY;
	int Z1 = 0;
	g_ScenePlace.ViewPortCoordToSpaceCoord(X1, Y1, Z1);

	int X2, Y2;
	SubWorld[Npc[nTargetIdx].m_SubWorldIndex].Map2Mps(Npc[nTargetIdx].m_RegionIndex, 
		Npc[nTargetIdx].GetMapX(),
		Npc[nTargetIdx].GetMapY(),
		Npc[nTargetIdx].GetOffX(),
		Npc[nTargetIdx].GetOffY(),
		&X2,
		&Y2);
		XOff = (X2 - X1);
		YOff = (Y2 - Y1);

	nRet = (int)(XOff * XOff + YOff * YOff);

	if(-1 == nRet)
		return 0x7fffffff;
	INTORFLOAT tmp;
	tmp.f = qsqrt((float)(nRet));
	tmp.f += bias.f;
	tmp.i -= bias.i;
	nRet = tmp.i;
	return nRet;	
}
#endif

int		KNpcSet::GetDistanceSquare(int nIdx1, int nIdx2)
{
	int	nRet = 0;
	if (Npc[nIdx1].m_SubWorldIndex != Npc[nIdx2].m_SubWorldIndex)
		return -1;
	
	int XOff = 0;
	int YOff = 0;

	if (Npc[nIdx1].m_RegionIndex == Npc[nIdx2].m_RegionIndex)
	{
		XOff = (Npc[nIdx1].GetMapX() - Npc[nIdx2].GetMapX()) * REGION_CELL_SIZE_X;
		XOff += (Npc[nIdx1].GetOffX() - Npc[nIdx2].GetOffX()) >> 10;
		
		YOff = (Npc[nIdx1].GetMapY() - Npc[nIdx2].GetMapY()) * REGION_CELL_SIZE_Y;
		YOff += (Npc[nIdx1].GetOffY() - Npc[nIdx2].GetOffY()) >> 10;
	}
	else
	{
		int X1, Y1;
		SubWorld[Npc[nIdx1].m_SubWorldIndex].Map2Mps(Npc[nIdx1].m_RegionIndex, 
			Npc[nIdx1].GetMapX(),
			Npc[nIdx1].GetMapY(),
			Npc[nIdx1].GetOffX(),
			Npc[nIdx1].GetOffY(),
			&X1,
			&Y1);
		int X2, Y2;
		SubWorld[Npc[nIdx2].m_SubWorldIndex].Map2Mps(Npc[nIdx2].m_RegionIndex, 
			Npc[nIdx2].GetMapX(),
			Npc[nIdx2].GetMapY(),
			Npc[nIdx2].GetOffX(),
			Npc[nIdx2].GetOffY(),
			&X2,
			&Y2);
		XOff = (X2 - X1);
		YOff = (Y2 - Y1);
	}

	nRet = (int)(XOff * XOff + YOff * YOff);

	if (Npc[nIdx1].m_nBarrierWidth > 1 || Npc[nIdx2].m_nBarrierWidth > 1 || Npc[nIdx1].m_nBarrierHeight > 1 || Npc[nIdx2].m_nBarrierHeight > 1)
	{
		const int nW = Npc[nIdx1].m_nBarrierWidth + Npc[nIdx2].m_nBarrierWidth;
		const int nH = Npc[nIdx1].m_nBarrierHeight + Npc[nIdx2].m_nBarrierHeight;	
		const int nSize = (nW + nH ) * REGION_CELL_SIZE_X/2 + REGION_CELL_SIZE_X + REGION_CELL_SIZE_X;
		const int nDR = nSize * nSize;
		if (nRet > nDR)
		{
			nRet -= nDR;
		}
		else
		{
			nRet = 0;
		}
	}
	
	return nRet;	
}

int		KNpcSet::GetNextIdx(int nIdx)
{
	if (nIdx < 0 || nIdx >= MAX_NPC)
		return 0;
	return m_UseIdx.GetNext(nIdx);
}

#ifdef _SERVER
BOOL KNpcSet::SyncNpc(DWORD dwID, int nPlayerIdx)
{
	int		nFindIndex;

	nFindIndex = SearchID(dwID);
	BOOL	bRet = TRUE;
	if (nFindIndex <= 0)
	{
		bRet = FALSE;
	}

// 	if (Npc[nFindIndex].m_Doing == do_death || Npc[nFindIndex].m_Doing == do_revive)
// 	{
// 		bRet = FALSE;
// 	}

	if (bRet)
	{
		bRet = Npc[nFindIndex].SendSyncData(Player[nPlayerIdx].m_nNetConnectIdx, Player[nPlayerIdx].m_nIndex );
	}

	if (FALSE == bRet)
	{
		NPC_REQUEST_FAIL	RequestFail;
		
		RequestFail.ProtocolType = s2c_requestnpcfail;
		RequestFail.ID = dwID;
		
		if (g_pServer)
			g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &RequestFail, sizeof(NPC_REQUEST_FAIL));
		
		return FALSE;
	}
	
	return TRUE;
}
#endif

#ifndef _SERVER
int KNpcSet::GetNpcByName(const char* name)
{
	if(name == NULL)
		return -1;
	KPlayer& selfPlayer = GetClientPlayer();
	KNpc& selfNpc = Npc[selfPlayer.GetNpcIndex()];
	int regionWidthSquare = REGION_PIXEL_WIDTH * REGION_PIXEL_WIDTH;
	
	int nIdx;
	nIdx = m_UseIdx.GetNext(0);
	while(nIdx)
	{
		int nTmpIdx = m_UseIdx.GetNext(nIdx);
		KNpc& npc = Npc[nIdx];
		if(strcmp(npc.Name,name) == 0)
		{
			if(npc.m_Kind != kind_player)
				return nIdx;
		}
		nIdx = nTmpIdx;
	}
	return -1;
}
void KNpcSet::ForceRemoveAllNpcHeadInfo()
{
	KPlayer& selfPlayer = GetClientPlayer();
	KNpc& selfNpc = Npc[selfPlayer.GetNpcIndex()];
	int regionWidthSquare = REGION_PIXEL_WIDTH * REGION_PIXEL_WIDTH;

	int nIdx;
	nIdx = m_UseIdx.GetNext(0);
	while(nIdx)
	{
		int nTmpIdx = m_UseIdx.GetNext(nIdx);
		KNpc& npc = Npc[nIdx];
		if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex &&
					nIdx != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPetIndex )
		{
			RoleHeadInfo roleHeadInfo;
			ZeroMemory( &roleHeadInfo, sizeof(RoleHeadInfo) );
			char npcShowName[33] = {0};
			char* show = 0;
			if(Npc[nIdx].GetKind() == kind_creature)
			{
				strcpy(npcShowName,Npc[nIdx].Name);
				strcat(npcShowName,PLAYER_PET_NAME_EXTRA);
				show = npcShowName;

			}
			else
			{
		//		strcpy(npcShowName,Name);
				show = Npc[nIdx].Name;
			}
			roleHeadInfo.dwID = Npc[nIdx].GetId();
			CoreDataChanged(GDCNI_ROLEHEADINFO_DEL,(unsigned int)&roleHeadInfo,0);
		}
		nIdx = nTmpIdx;
	}
}
void KNpcSet::CheckBalance()
{
	KPlayer& selfPlayer = GetClientPlayer();
	KNpc& selfNpc = Npc[selfPlayer.GetNpcIndex()];
	int regionWidthSquare = REGION_PIXEL_WIDTH * REGION_PIXEL_WIDTH;

	int nIdx;
	nIdx = m_UseIdx.GetNext(0);
	while(nIdx)
	{
		int nTmpIdx = m_UseIdx.GetNext(nIdx);
		KNpc& npc = Npc[nIdx];
		int nRegionIdx   = Npc[nIdx].m_RegionIndex;

		int syncSignalTimeout = 6 * GAME_FPS;//6sec
		if (nRegionIdx != -1)
		{
			int nRegionNpcCount = SubWorld[0].m_Region[nRegionIdx].m_NpcList.GetNodeCount();
			int nSyncDelay      = nRegionNpcCount / 6 + 1;	//同步该Region上所有Npc所要的时间,3FPS一个（秒数）
			
#define NET_DELAY 6
#define BASE_INTERVAL 10
			
			syncSignalTimeout = ( BASE_INTERVAL + NET_DELAY + nSyncDelay ) * GAME_FPS;
		}

		if (SubWorld[0].m_dwCurrentTime - npc.m_SyncSignal > syncSignalTimeout)
		{
			//if (Npc[nIdx].m_Kind != kind_building) // lixuewu 建筑物超时也不移除
			{
				if (nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex &&
					nIdx != Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_nPetIndex )
				{
					if (Npc[nIdx].m_RegionIndex >= 0)
					{
						//lixuewu 2006.05.23 取消NPC阻挡
						//SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecNpcRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, nIdx);
					Npc[nIdx].DelInfo();
		
					SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
					Npc[nIdx].m_RegionIndex = -1;
					}
					// --> Rocker Edit Start 2005/11/01
					Remove(nIdx, false);	
					// <-- Rocker End
				}
				
			}
		}
		nIdx = nTmpIdx;
	}
	nIdx = m_RequestUseIdx.GetNext(0);
	while(nIdx)
	{
		int nTmpIdx = m_RequestUseIdx.GetNext(nIdx);
		if (SubWorld[0].m_dwCurrentTime - m_RequestNpc[nIdx].dwRequestTime > 100)
		{
			DWORD	dwID = m_RequestNpc[nIdx].dwRequestId;
			m_RequestNpc[nIdx].dwRequestId = 0;	
			m_RequestNpc[nIdx].dwRequestTime = 0;
			
			m_RequestUseIdx.Remove(nIdx);
			m_RequestFreeIdx.Insert(nIdx);
			g_DebugLog("[Request]Remove %d from %d on %d timeout", dwID, nIdx, SubWorld[0].m_dwCurrentTime);
		}
		nIdx = nTmpIdx;
	}
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：获得周围玩家列表，用于界面，队伍邀请列表
//-------------------------------------------------------------------------
int		KNpcSet::GetAroundPlayerForTeamInvite(KUiPlayerItem *pList, int nCount)
{
	int nCamp = Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].m_Camp;
	int nNum = 0, i;

	if (nCount == 0)
	{
		int nIdx = 0;
		while (1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player)
				continue;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				continue;
			if (Npc[nIdx].m_Camp != camp_begin && nCamp == camp_begin)
				continue;
			if (Npc[nIdx].m_RegionIndex < 0)
				continue;
			KTeam& team = GetClientTeam();
			int maxTeammemberCount = team.GetMaxMemberCount();
			for (i = 0; i < maxTeammemberCount; i++)
			{
				ClientTeamMemberInfo* pMemberInfo = team.GetMemberInfo(i);
				if (pMemberInfo != NULL && pMemberInfo->NpcId == Npc[nIdx].GetId())
					break;
			}
			if (i < maxTeammemberCount)
				continue;
			if ((DWORD)team.GetCaptain() == Npc[nIdx].m_dwID)
				continue;
			nNum++;
		}
		
		return nNum;
	}

	if (!pList)
		return 0;

	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;
		if (Npc[nIdx].m_Camp != camp_begin && nCamp == camp_begin)
			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;

		KTeam& team = GetClientTeam();
		int maxTeammemberCount = team.GetMaxMemberCount();
		for (i = 0; i < maxTeammemberCount; i++)
		{
			ClientTeamMemberInfo* pMemberInfo = team.GetMemberInfo(i);
			if (pMemberInfo != NULL && pMemberInfo->NpcId == Npc[nIdx].GetId())
				break;
		}
		if (i < maxTeammemberCount)
			continue;
		if ((DWORD)team.GetCaptain() == Npc[nIdx].m_dwID)
			continue;
		pList[nNum].nIndex = nIdx;
		pList[nNum].uId = Npc[nIdx].m_dwID;
		strcpy(pList[nNum].Name, Npc[nIdx].Name);
		nNum++;
	}
	
	return nNum;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：获得周围玩家列表(用于列表)
//-------------------------------------------------------------------------
int		KNpcSet::GetAroundPlayer(KUiPlayerItem *pList, int nCount)
{
	int nNum = 0;

	if (nCount <= 0)
	{
		int nIdx = 0;
		while (1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player ||
				nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex ||
				Npc[nIdx].m_RegionIndex < 0)
			{
				continue;
			}
//			if (Player[CLIENT_PLAYER_INDEX].m_cChat.CheckExist(Npc[nIdx].Name))
//				continue;
			nNum++;
		}
		
		return nNum;
	}

	if (!pList)
		return 0;

	int nIdx = 0;
	while (nNum < nCount)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player ||
			nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex ||
			Npc[nIdx].m_RegionIndex < 0)
		{
			continue;
		}
//		if (Player[CLIENT_PLAYER_INDEX].m_cChat.CheckExist(Npc[nIdx].Name))
//			continue;
		pList[nNum].nIndex = nIdx;
		pList[nNum].uId = Npc[nIdx].m_dwID;
		strcpy(pList[nNum].Name, Npc[nIdx].Name);
		pList[nNum].nData = Npc[nIdx].GetMenuState();
		nNum++;
	}
	
	return nNum;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的名字
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowNameFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_NAME;
	else
		m_nShowPateFlag &= ~PATE_NAME;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的名字  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowName()
{
	return m_nShowPateFlag & PATE_NAME;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的血
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowLifeFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_LIFE;
	else
		m_nShowPateFlag &= ~PATE_LIFE;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的血  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowLife()
{
	return m_nShowPateFlag & PATE_LIFE;
}

void	KNpcSet::setShowPlayer(BOOL bFlag)
{
	if(bFlag)
	{
		m_nNpcPaitFlag |= SHOW_PLAYER;
	}
	else
	{
		m_nNpcPaitFlag &= ~SHOW_PLAYER;
	}
}

BOOL	KNpcSet::isShowPlayer()
{
	return m_nNpcPaitFlag & SHOW_PLAYER;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的聊天
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowChatFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_CHAT;
	else
		m_nShowPateFlag &= ~PATE_CHAT;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的聊天  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowChat()
{
	return m_nShowPateFlag & PATE_CHAT;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：设定是否全部显示玩家的内力
//			bFlag ==	TRUE 显示，bFlag == FALSE 不显示 zroc add
//-------------------------------------------------------------------------
void	KNpcSet::SetShowManaFlag(BOOL bFlag)
{
	if (bFlag)
		m_nShowPateFlag |= PATE_MANA;
	else
		m_nShowPateFlag &= ~PATE_MANA;
}
#endif

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：判断是否全部显示玩家的内力  返回值 TRUE 显示，FALSE 不显示
//-------------------------------------------------------------------------
BOOL	KNpcSet::CheckShowMana()
{
	return m_nShowPateFlag & PATE_MANA;
}
#endif

//-------------------------------------------------------------------------
//	功能：把所有npc的 bActivateFlag 设为 FALSE
//		(每次游戏循环处理所有npc的activate之前做这个处理)
//-------------------------------------------------------------------------
void	KNpcSet::ClearActivateFlagOfAllNpc()
{
	int nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		Npc[nIdx].m_bActivateFlag = FALSE;
	}

	// Add by Cooler -->
	// 2006-5-26 16:44
	nIdx = 0;
	while( TRUE )
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if(nIdx == 0)
			break;

		Npc[nIdx].m_bActivateFlag = FALSE;
	}
	// End add by Cooler <--
}

#ifndef _SERVER
//-------------------------------------------------------------------------
//	功能：获得周围同阵营的已开放队伍队长列表 不同阵营现在可以组队
//-------------------------------------------------------------------------
void	KNpcSet::GetAroundOpenCaptain(int nCamp)
{
	int		nIdx, nNum, nNo;

	nIdx = 0;
	nNum = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		if (Npc[nIdx].m_Kind != kind_player)
			continue;
		if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
			continue;
		// 不同阵营现在可以组队，老手不能加入新人队伍，新人可以加入老手队伍
		if (Npc[nIdx].m_Camp == camp_begin && nCamp != camp_begin)
			continue;
//		if (Npc[nIdx].m_Camp != nCamp)
//			continue;
		if (Npc[nIdx].m_RegionIndex < 0)
			continue;
	}
	
	if (nNum > 0)
	{
		KUiTeamItem* const pTeamList = new KUiTeamItem[nNum];
		nIdx = 0;
		nNo = 0;
		while (1)
		{
			nIdx = m_UseIdx.GetNext(nIdx);
			if (nIdx == 0)
				break;
			if (Npc[nIdx].m_Kind != kind_player)
				continue;
			if (nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex)
				continue;
			// 不同阵营现在可以组队，老手不能加入新人队伍，新人可以加入老手队伍
			if (Npc[nIdx].m_Camp == camp_begin && nCamp != camp_begin)
				continue;
//			if (Npc[nIdx].m_Camp != nCamp)
//				continue;
			if (Npc[nIdx].m_RegionIndex < 0)
				continue;
		}
//		CoreDataChanged(GDCNI_TEAM_NEARBY_LIST, (unsigned int)pTeamList, nNo);
		delete []pTeamList;
	}
}
#endif

#ifndef _SERVER	// 用于客户端
int	KNpcSet::SearchNpcAt(int nX, int nY, int nRelation, int nRange, bool bSearchSelf /* = false */ , bool bNoPlayer /* = false */ )
{
	int nIdx;
	int	nMin = 800;
	int nMinIdx = 0;
	int	nLength = 0;
	int nSrcX[2];
	int	nSrcY[2];

	nSrcX[0] = nX;
	nSrcY[0] = nY;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nSrcX[0], nSrcY[0], 0);

	nSrcX[1] = nX;
	nSrcY[1] = nY;
	g_ScenePlace.ViewPortCoordToSpaceCoord(nSrcX[1], nSrcY[1], 260);

	int nDx = nSrcX[0] - nSrcX[1];
	int nDy = nSrcY[0] - nSrcY[1];

	nIdx = 0;
	while (1)
	{
		nIdx = m_UseIdx.GetNext(nIdx);

		if (nIdx == 0)
			break;

		if (Npc[nIdx].m_RegionIndex < 0)
			continue;

		if ( nIdx == Player[CLIENT_PLAYER_INDEX].m_nIndex && !bSearchSelf )
			continue;

		if (Npc[nIdx].IsEmployee())
			continue;

		if ( Npc[nIdx].GetCurrentLifePercentage() == 0 && 
			Npc[nIdx].m_Kind == kind_normal )
		{
			continue;
		}

		if (bNoPlayer && Npc[nIdx].m_Kind == kind_player)
		{
			continue;
		}//endif

		//Modified by [Ray]  2005-7-7
		//不屏蔽就会跳过宠物的
		if ( Npc[nIdx].m_bClientOnly )
		{
			if ( Npc[nIdx].m_Kind != kind_pet )
			{
				continue;
			}
		}

		if (!(GetRelation(Player[CLIENT_PLAYER_INDEX].m_nIndex, nIdx) & nRelation))
			continue;
	
		// 没有阻挡的不能选
		if (!Npc[nIdx].m_bHaveBarrier) 
			continue;
		// 大型物体拾取不用那么大的范围
		int nSelWidth = Npc[nIdx].m_nBarrierWidth;
		int nSelHeight = Npc[nIdx].m_nBarrierHeight;
		if ( nSelWidth > 5) nSelWidth = 5;
		if (nSelHeight > 5) nSelHeight = 5;

		int x, y;
		SubWorld[0].Map2Mps(Npc[nIdx].m_RegionIndex, Npc[nIdx].GetMapX(), Npc[nIdx].GetMapY(),
			Npc[nIdx].GetOffX(), Npc[nIdx].GetOffY(), &x, &y);

		

		if (nSrcY[0] > y + nSelHeight * REGION_CELL_SIZE_Y )
			continue;

		if (nSrcY[0] < y - (Npc[nIdx].GetNpcPate() << 1))
			continue;

		nLength = abs(nSrcX[0]-x);//abs(nDx * (nSrcY[0] - y) / nDy + nSrcX[0] - x);
		int nSelLen = nSelWidth * REGION_CELL_SIZE_X;
		if (nSelLen <=0 )
		{
			nSelLen = REGION_CELL_SIZE_X;
		}
		if ((nLength < nMin) && (nLength < nSelLen))
		{
			nMin = nLength;
			nMinIdx = nIdx;
		}
	}

	return nMinIdx;
}
#endif

#ifndef _SERVER
BOOL KNpcSet::IsNpcRequestExist(DWORD dwID)
{
	return (GetRequestIndex(dwID) > 0);
}

void KNpcSet::InsertNpcRequest(DWORD dwID)
{
	if (IsNpcRequestExist(dwID))
	{
		return;
	}

	int nIndex = m_RequestFreeIdx.GetNext(0);
	if (!nIndex)
		return;

	m_RequestNpc[nIndex].dwRequestId = dwID;
	m_RequestNpc[nIndex].dwRequestTime = SubWorld[0].m_dwCurrentTime;
	//--> Rocker 2005/07/14
	m_RequestNpc[nIndex].Addon.btSayType = 0;
	m_RequestNpc[nIndex].Addon.szSayMessage[0] = 0;
	//<-- End
	m_RequestFreeIdx.Remove(nIndex);
	m_RequestUseIdx.Insert(nIndex);
	g_DebugLog("[Request]Insert %d at %d on %d", dwID, nIndex, SubWorld[0].m_dwCurrentTime);
}

void KNpcSet::RemoveNpcRequest(DWORD dwID)
{
	if(!IsNpcRequestExist(dwID))
	{
		return;
	}
	int nIndex = GetRequestIndex(dwID);

	// because _ASSERT(IsNpcRequestExist()); so nIndex > 0;
	m_RequestNpc[nIndex].dwRequestId = 0;	
	m_RequestNpc[nIndex].dwRequestTime = 0;

	m_RequestUseIdx.Remove(nIndex);
	m_RequestFreeIdx.Insert(nIndex);
	g_DebugLog("[Request]Remove %d from %d on %d", dwID, nIndex, SubWorld[0].m_dwCurrentTime);
}

int KNpcSet::GetRequestIndex(DWORD dwID)
{
	int nIndex = m_RequestUseIdx.GetNext(0);

	while(nIndex)
	{
		if (m_RequestNpc[nIndex].dwRequestId == dwID)
		{
			return nIndex;
		}
		nIndex = m_RequestUseIdx.GetNext(nIndex);
	}
	return 0;
}

RequestNpcAddon* KNpcSet::GetRequestAddon(DWORD dwID)
{
	int nIndex = m_RequestUseIdx.GetNext(0);

	while(nIndex)
	{
		if (m_RequestNpc[nIndex].dwRequestId == dwID)
		{
			return &m_RequestNpc[nIndex].Addon;
		}
		nIndex = m_RequestUseIdx.GetNext(nIndex);
	}
	return NULL;
}

#endif

NPC_RELATION KNpcSet::GetRelation(int nIdx1, int nIdx2)
{
	if(Npc[nIdx1].m_Kind == kind_player && Npc[nIdx2].m_Kind == kind_producesrc)
		return relation_produce;
	
	if(Npc[nIdx1].m_Kind == kind_dialoger || Npc[nIdx2].m_Kind == kind_dialoger)
		return relation_dialog;
	
	if(nIdx1 == nIdx2)
		return relation_self;
	
	if( Npc[nIdx1].IsInSafeArea( ) || Npc[nIdx2].IsInSafeArea( ) )
		return relation_ally;
	
#ifndef _SERVER
	if (Npc[nIdx1].m_bClientOnly || Npc[nIdx2].m_bClientOnly)
		return relation_none;
#endif
	
#ifdef _SERVER

	if(Npc[nIdx1].m_Kind == kind_creature) 
		nIdx1 = Npc[nIdx1].GetSummonerIdx();
	else if(Npc[nIdx1].m_Kind == kind_employee)
		nIdx1 = Npc[nIdx1].GetEmployerIdx();
	
	if(Npc[nIdx2].m_Kind == kind_creature)
		nIdx2 = Npc[nIdx2].GetSummonerIdx();
	else if(Npc[nIdx2].m_Kind == kind_employee)
		nIdx2 = Npc[nIdx2].GetEmployerIdx();

	if ((Npc[nIdx1].m_Kind == kind_player && IsValidPlayer(Npc[nIdx1].GetPlayerIdx()) && Player[Npc[nIdx1].GetPlayerIdx()].IsGM())
		|| (Npc[nIdx2].m_Kind == kind_player && IsValidPlayer(Npc[nIdx2].GetPlayerIdx()) && Player[Npc[nIdx2].GetPlayerIdx()].IsGM()))
		return relation_ally;
	
#else
	
	if(Npc[nIdx1].m_Kind == kind_creature) 
	{
		const int nID = Npc[nIdx1].GetSummonerIdx();
		nIdx1 = NpcSet.SearchID(nID);
	}
	else if (Npc[nIdx1].m_Kind == kind_employee)
	{
		const int nID = Npc[nIdx1].m_nPlayerIdx;
		nIdx1 = NpcSet.SearchID(nID);
	}
	
	if (Npc[nIdx2].m_Kind == kind_creature)
	{
		const int nID = Npc[nIdx2].GetSummonerIdx();
		nIdx2 = NpcSet.SearchID(nID);
	}	
	else if (Npc[nIdx2].m_Kind == kind_employee)
	{
		const int nID = Npc[nIdx2].m_nPlayerIdx;
		nIdx2 = NpcSet.SearchID(nID);
	}
	
#endif

	if( Npc[nIdx1].IsInSafeArea( ) || Npc[nIdx2].IsInSafeArea( ) )
		return relation_ally;
	
 	if (Npc[nIdx1].m_Doing == do_death || Npc[nIdx1].m_Doing == do_revive
 		|| Npc[nIdx2].m_Doing == do_death || Npc[nIdx2].m_Doing == do_revive)
 		return relation_none;

	if (Npc[nIdx1].m_SubWorldIndex == Npc[nIdx2].m_SubWorldIndex && Npc[nIdx1].IsInWorldCombatInstance() && Npc[nIdx2].IsInWorldCombatInstance())
	{
		if (Npc[nIdx1].m_WorldCombatOrg == Npc[nIdx2].m_WorldCombatOrg)
		{
			return relation_ally;
		}//endif
		else
		{
			return relation_enemy;
		}//end else
		
	}//endif

// 	// 唯一允许对死亡者攻击的条件是:
// 	// 攻击者是玩家，处于正常状态; 被攻击者是玩家，处于do_revive状态
// 	bool bDeathCanPass = false;
// 
// 	if(kind_player == Npc[nIdx1].m_Kind && kind_player == Npc[nIdx2].m_Kind)
// 	{
// 		if(do_death != Npc[nIdx1].m_Doing && do_revive != Npc[nIdx1].m_Doing && do_revive == Npc[nIdx2].m_Doing)
// 		{
// 			bDeathCanPass = true;
// 		}
// 	}
// 
// 	if(!bDeathCanPass)
// 	{
// 		// 被攻击者不允许处于死亡状态，攻击者如果是玩家也不允许处于死亡状态
// 		if(Npc[nIdx2].m_Doing == do_death || Npc[nIdx2].m_Doing == do_revive)
// 			return relation_none;
// 
// 		if(kind_player == Npc[nIdx1].m_Kind && (Npc[nIdx1].m_Doing == do_death || Npc[nIdx1].m_Doing == do_revive) )
// 			return relation_none;
// 	}
	
	// 所有的特例关系请加在前面，正常关系根据PK关系表来确定
	// 默认条件下返回第一条对应的结果
	int	nPKMode = Npc[nIdx1].m_UnaryAttrMgr[nuai_pkmode];
	int	nRet = m_PKRelationTbl[nPKMode][0].Result;
	PK_TARGET_COND pkCond = GetPKCond(nIdx1, nIdx2);
	
	for(int i = 0; i < MAX_PK_RELATION_ENTRY; ++i)
	{
		PKRelationEntry& entry = m_PKRelationTbl[nPKMode][i];			
		
		if (entry.Condition >= 0)
		{
			bool containCondition = ((entry.Condition & pkCond) == entry.Condition) ? true : false;
			if( ( entry.Match && containCondition ) 
				|| ( !entry.Match && !containCondition )
				)
			{
				nRet = entry.Result;
				break;
			}
		}
		else
		{
			break;
		}
	}
	
	return (NPC_RELATION)nRet;
}

#include "SocialUnit.h"

#ifndef _SERVER
void KNpcSet::SceneProcessNpc()
{
	bool bSetFocus = false;

	const int nPlayer = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	int nIdx = 0;
	for (;;)
	{
		nIdx = m_UseIdx.GetNext(nIdx);
		if (nIdx == 0)
			break;
		Npc[nIdx].m_DataRes.UpDateScene(nIdx, false);
	}
	
	nIdx = 0;
	for (;;)
	{
		nIdx = m_UsePlayerIdx.GetNext(nIdx);
		if(nIdx == 0)
			break;
		Npc[nIdx].m_DataRes.UpDateScene(nIdx, false);
	}

	if ( !bSetFocus )
	{
		const KNpc& rPlayer = Npc[nPlayer];
		g_ScenePlace.SetFocusPosition(rPlayer.m_DataRes.m_nXpos, rPlayer.m_DataRes.m_nYpos, rPlayer.m_DataRes.m_nZpos );
		CoreDataChanged(GDCNI_CHAT_MINI_MAP_UPDATA,0,0);
		bSetFocus = true;
	}
}
#endif

PK_TARGET_COND	KNpcSet::GetSUCond(int nLauncher, int nTarget)
{
	int pkCond = ptc_cond_none;

	if(nLauncher == nTarget)
		pkCond |= ptc_cond_self;
	
	bool bIsLauncherPlayer = (Npc[nLauncher].m_Kind == kind_player);
	bool bIsTargetPlayer = (Npc[nTarget].m_Kind == kind_player);

#ifdef _SERVER
	//是否为同一个社会关系
	SocialUnit* pLUnit = NULL;
	SocialUnit* pTUnit = NULL;

	//取得Launcher对应的SocialUnit
	if (bIsLauncherPlayer)
	{
		int nLPlayerIndex = Npc[nLauncher].GetPlayerIdx( );
		RelationSet& LRS = Player[nLPlayerIndex].GetRelationSet( );
		RelationRecord* pLRec = LRS.GetRelationByTemplate( enSUTplId_Tong );
		if (pLRec)
		{
			pLUnit = pLRec->pLeafUnit;
		}
	}
	else
	{
		pLUnit = ServerSocialUnitMgr::Singleton().GetUnit( Npc[nLauncher].GetLord(), enSUTplId_Tong );
	}

	//取得Target对应的SocialUnit
	if (bIsTargetPlayer)
	{
		int nTPlayerIndex = Npc[nTarget].GetPlayerIdx( );
		RelationSet& TRS = Player[nTarget].GetRelationSet( );
		RelationRecord* pTRec = TRS.GetRelationByTemplate( enSUTplId_Tong );
		if (pTRec)
		{
			pTUnit = pTRec->pLeafUnit;
		}
	}
	else
	{
		pTUnit = ServerSocialUnitMgr::Singleton().GetUnit( Npc[nTarget].GetLord(), enSUTplId_Tong );
	}

	//都不是玩家，并且都没有Lord，则既是诸侯关系又是氏族关系
	if (!bIsLauncherPlayer && !bIsTargetPlayer && !pLUnit && !pTUnit)
	{
		pkCond |= ptc_cond_gens;
		pkCond |= ptc_cond_tong;
		pkCond |= ptc_cond_league;
	}
	else
	{
		SocialUnit* pTOriginalUnit = pTUnit;//记住Taget对应的SocialUnit
		//查找是否有相同的父层节点
		while (pLUnit && pTOriginalUnit)
		{
			int layer = pLUnit->GetLayer();
			pTUnit = pTOriginalUnit;
			
			while(pTUnit)
			{		
				const FSGUID& LGUID = pLUnit->GetUnitGuid( );
				const FSGUID& TGUID = pTUnit->GetUnitGuid( );
				if (LGUID != 0 && TGUID != 0 && LGUID == TGUID)//找到相同的父层节点了
				{
					//根据层数决定关系
					switch(layer)
					{
					case enSULayer_Player:
						pkCond |= ptc_cond_owner;
						break;
					case enSULayer_Gens:
						pkCond |= ptc_cond_gens;
						break;
					case enSULayer_Tong:
						pkCond |= ptc_cond_tong;
						break;
					case enSULayer_League:
						pkCond |= ptc_cond_league;
						break;
					}
				}
				
				pTUnit = pTUnit->GetParent();
			}
			
			pLUnit = pLUnit->GetParent( );
		}
	}
#endif
	return (PK_TARGET_COND)pkCond;
}

PK_TARGET_COND KNpcSet::GetPKCond(int nLauncher, int nTarget)
{
	int pkCond = ptc_cond_none;
	
	if(nLauncher == nTarget)
		pkCond |= ptc_cond_self;
	
	bool bIsLauncherPlayer = (Npc[nLauncher].m_Kind == kind_player);
	bool bIsTargetPlayer = (Npc[nTarget].m_Kind == kind_player);
	
#ifdef _SERVER
	if(bIsLauncherPlayer && bIsTargetPlayer)
	{
		if ( Player[Npc[nLauncher].m_nPlayerIdx].GetTeamInfo().IsInTeam() && Player[Npc[nTarget].m_nPlayerIdx].GetTeamInfo().IsInTeam() )
		{
			if (Player[Npc[nLauncher].m_nPlayerIdx].GetTeamInfo().GetTeamId() == Player[Npc[nTarget].m_nPlayerIdx].GetTeamInfo().GetTeamId())
				pkCond |= ptc_cond_team;
		}
	}
#else
	KPlayerTeam& teamInfo = GetClientPlayer().GetTeamInfo();
	if (teamInfo.IsInTeam())
	{
		KTeam* pTeam = teamInfo.GetTeam();
		if (pTeam != NULL)
		{
			int maxMemberCount = pTeam->GetMaxMemberCount();
			for (int i = 0; i < maxMemberCount; ++i)
			{
				ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(i);
				if (pMemberInfo != NULL)
				{
					if (pMemberInfo->NpcId == Npc[nTarget].GetId())
					{
						pkCond |= ptc_cond_team;
						break;
					}
				}
			}
		}
	}	
#endif
	
	// 是否为同社会关系
	
#ifdef _SERVER
		// 是否为同一个社会关系
	pkCond |= GetSUCond( nLauncher, nTarget );
#else
	if( Player[CLIENT_PLAYER_INDEX].m_nIndex == nLauncher )
		pkCond |= Npc[nTarget].m_UnaryAttrMgr[nuai_servercont];
#endif

	ConfigManager& cm = ConfigManager::Singleton();
	unsigned int titleColor = Npc[nTarget].m_UnaryAttrMgr[nuai_titlecolor];
	
	if (titleColor == cm.GetConfigurableColor(color_pk_punish) ||//是否为PK惩罚（红名）或者恶魔（深红）玩家或者是蓝名
		titleColor == cm.GetConfigurableColor(color_pk_demon) ||
		titleColor == cm.GetConfigurableColor(color_pk_diablo))
	{
		pkCond |= ptc_cond_redname;
	}
	else if (titleColor == cm.GetConfigurableColor(color_killer))// 是否为杀手（灰名）玩家
	{
		pkCond |= ptc_cond_grayname;
	}
	
	if(Npc[nTarget].m_Kind == kind_normal)
		pkCond |= ptc_cond_monster;

	return (PK_TARGET_COND)pkCond;
}

bool KNpcSet::GenPKRelationTbl()
{
	memset(m_PKRelationTbl, 0, sizeof(m_PKRelationTbl));
	{
		for(int i = 0; i < pk_mode_num; ++i)
		{
			for(int j = 0; j < MAX_PK_RELATION_ENTRY; ++j)
				m_PKRelationTbl[i][j].Condition = -1;
		}
	}

    KTabFile	pkFile;

	if( !pkFile.Load(FILE_PKRELATION_TABLE) )
		return false;

	int nDataRowNum = pkFile.GetHeight() - 1;

	int lastPkMode = -1;
	int lastEntryIndex = -1;
	for(int i = 0; i < nDataRowNum; ++i)
	{
		int nMode, nCond, nTrueOrNot, nRst, nAccesser;

		pkFile.GetInteger(i + 2, "PKMode", 0, &nMode);
		pkFile.GetInteger(i + 2, "Cond", 0, &nCond);
		pkFile.GetInteger(i + 2, "True", 0, &nTrueOrNot);
		pkFile.GetInteger(i + 2, "Result", relation_ally, &nRst);
		pkFile.GetInteger(i + 2, "Accesser", pkm_accesser_none, &nAccesser);

		if( (nMode >= 0 && nMode < pk_mode_num)
			&& (nCond >= 0 && nCond < ptc_cond_num)
			&& (nTrueOrNot == 0 || nTrueOrNot == 1)
			&& (nRst == relation_ally || nRst == relation_enemy)
		  )
		{
			if (lastPkMode != nMode)
			{
				lastPkMode = nMode;
				lastEntryIndex = 0;				
			}

			if (lastEntryIndex >= MAX_PK_RELATION_ENTRY)
				return false;

			PKRelationEntry& entry = m_PKRelationTbl[nMode][lastEntryIndex];
			entry.Condition = 1 << nCond;
			entry.Condition >>= 1;
			entry.Match = nTrueOrNot;
			entry.Result = nRst;
			m_PKModeAccesser[nMode] = (PKMODE_ACCESSER)nAccesser;

			lastEntryIndex++;
		}
		else
		{
			return false;
		}
	}

	return true;	
}

//------------------------ class KInstantSpecial start -------------------------
#ifndef _SERVER
// lixuewu 2004.03.18 总体改进
KCacheNode* KInstantSpecial::s_pSndNode = NULL;

KInstantSpecial::KInstantSpecial()
{
	m_nSprNameCount = INVALIDE_COUNT;
	m_nSndNameCount = INVALIDE_COUNT;
	m_SprNames = NULL;
	m_SndNames = NULL;
}

KInstantSpecial::~KInstantSpecial()
{
	if (m_SprNames != NULL)
	{
		delete[] m_SprNames;
	}
	if (m_SndNames != NULL)
	{
		delete[] m_SndNames;
	}
}

void KInstantSpecial::LoadSprName()
{
	KTabFile aSprNames;
	if (aSprNames.Load(PLAYER_INSTANT_SPECIAL_FILE))
	{
		const  int nCount = aSprNames.GetHeight() -1;
		if (nCount > 0)
		{
			m_SprNames = new SPRFILENAME[nCount];
			if(m_SprNames != NULL)
			{
				for(unsigned int i = 0 ; i < nCount ; i++)
				{
					aSprNames.GetString(i + 2, 3 , "",m_SprNames[i],sizeof(m_SprNames[i]));
				}
				m_nSprNameCount = nCount;
				return;
			}
		}
	}
	m_nSprNameCount = 0; // 表明已经Load过但失败了
	return;
}

void	KInstantSpecial::LoadSoundName()
{
	// todo 等待最终定稿
	m_SndNames = new SNDFILENAME[MAX_INSTANT_SOUND];
	if (NULL != m_SndNames)
	{
		KIniFile	cSoundName;
		char		szTemp[32];
		if (!cSoundName.Load(defINSTANT_SOUND_FILE))
			return;
		for (unsigned int i = 0; i < MAX_INSTANT_SOUND; i++)
		{
			sprintf(szTemp, "%d", i);
			cSoundName.GetString("Game", szTemp, "", m_SndNames[i], sizeof(m_SndNames[i]));
		}

	}
}

void KInstantSpecial::GetSprName(unsigned int nNo, char *lpszName,unsigned int nLength)
{
	if (NULL != lpszName)
	{
		if (INVALIDE_COUNT == m_nSprNameCount)
		{
			LoadSprName();
		}
		if ( nNo < m_nSprNameCount)
		{

			if (strlen(m_SprNames[nNo]) < nLength)
			{
				strcpy(lpszName, m_SprNames[nNo]);
				return;
			}
		}
		lpszName[0] = 0;
	}
	return ;
}

void KInstantSpecial::PlaySound(unsigned int nNo )
{
	if (INVALIDE_COUNT == m_nSndNameCount)
	{
		LoadSoundName();
	}
	else if (nNo < m_nSndNameCount)
	{
		if (0 != m_SndNames[nNo][0]) // 简单判定
		{
			s_pSndNode = g_SoundCache.GetNode(m_SndNames[nNo],(KCacheNode*)s_pSndNode);
			KWavSound* pSound = (KWavSound*)s_pSndNode->m_lpData;
			if (NULL != pSound) 
			{
				if (!pSound->IsPlaying()) 
				{
					//pSound->Play(0, -10000 + Option.GetSndVolume() * 100, 0);
					pSound->Play(0, Option.GetSndVolume(), 0);
				}
			}

		}
	}
}
// lixuewu 2004.03.18 总体改进

void KNpcSet::RemoveAllNpcExceptClient()
{
	int nIdx = m_UseIdx.GetNext(0);
	
	while(nIdx)
	{
		int nTmpIdx      = m_UseIdx.GetNext(nIdx);
		
		if ( nIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex )		
		{		
			if (Npc[nIdx].m_RegionIndex >= 0)	//这里是为了保证Npc从NpcSet上删掉前先从Region上删掉，因为从Region上删掉的时候可能会用到Npc的一些属性，如果先从NpcSet上删掉，这些属性肯能会被Init
			{
				Npc[nIdx].DelInfo();
				
				SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].RemoveNpc(nIdx);
				Npc[nIdx].m_RegionIndex = -1;
			}
			
			Remove(nIdx, false);	
		}
		nIdx = nTmpIdx;
	}
}
#endif
//------------------------- class KInstantSpecial end --------------------------
