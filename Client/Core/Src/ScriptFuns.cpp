/*******************************************************************************
// FileName			:	ScriptFuns.cpp
// FileAuthor		:	RomanDou
// FileCreateDate	:	2002-11-19 15:58:20
// FileDescription	:	脚本指令集
// Revision Count	:	
*******************************************************************************/
#ifndef WIN32
#include <string>
#endif

#include "KWin32.h"
#include "KEngine.h"
#include "KDebug.h"
#include "KStepLuaScript.h"
#include "LuaLib.h"
#include "KScriptList.h"
#include <string.h>
#include "LuaFuns.h"
#include "KCore.h"
#include "KNpc.h"
#include "KSubWorld.h"
#include "KObjSet.h"
#include "KItemSet.h"
#include "KScriptValueSet.h"
#include "KNpcSet.h"
#include "KPlayerSet.h"
#include "KPlayer.h"
#include "KSubWorldSet.h"
#include "KProtocolProcess.h"
#include "KBuySell.h"
#include "KSmithShop.h"
#include "KPlayerDef.h"
#include "KSortScript.h"
#include "KNpcTemplate.h"
#ifndef __linux
#include "Shlwapi.h"
#include "windows.h"
#include "winbase.h"
#include <direct.h>
#else
#include "unistd.h"
#endif

#ifdef _STANDALONE
#include "KSG_StringProcess.h"
#else
#include "KSG_StringProcess.h"
#endif
#include "time.h"
#include "KSkills.h"

#include "insurance_common.h"
#include "KWarInfoManager.h"

#ifdef _SERVER
#include "AntiEnthrall.h"
#include "IBLog.h"
#include "ChatCenter_S.h"
#include "SocialComDef.h"
#include "ServerSocialUnitMgr.h"
#include "CoreRelated.h"
#include "ConfigManager.h"
#include "ArmorSet_Table.h"
#include "pool_combat_mgr.h"
#include "OnceIBItemMgr.h"
#include "keconomysys.h"
#include "tong_war_manager.h"
#endif

#include "exp_insruance.h"

#ifndef _SERVER
#include "coreshell.h"
#endif

#ifdef _SERVER
#include "DBAucDataCenter.h"
#include "IBCenter_S.h"
#endif

#ifndef _SERVER
#include "ImgRef.h"
#endif

#ifdef _AUTO_ROBOT
#ifndef _SERVER
#include "AutoRobotMgr.h"
#endif
#endif

#ifdef _SERVER
#include "exp_manager.h"
#include "BannerMgr.h"
#endif

#ifdef _SERVER
#include "ITaisuiWheelTianXiangMgr.h"
#include "KItemGenerator.h"
#include "fseye_protocol.h"
#include "employ.h"
#include "question.h"
#endif

#include "ScriptFuns.h"
#include "specialskill_tab.h"

#ifndef WIN32
typedef struct  _SYSTEMTIME
{
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
}	SYSTEMTIME;
typedef struct  _FILETIME
{
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
}	FILETIME;
#endif

inline const char* _ip2a(DWORD ip) { in_addr ia; ia.s_addr = ip; return inet_ntoa(ia); }
inline DWORD _a2ip(const char* cp) { return inet_addr(cp); }

KScriptList		g_StoryScriptList;
KStepLuaScript * LuaGetScript(Lua_State * L);

static int g_GlobalScriptParam[SCRIPT_PARAM_MAX] = {0};

void ScriptSetPlayerIndex(int nIdx)
{
	g_GlobalScriptParam[SCRIPT_PARAM_PLAYER_INDEX] = 0;

	if (IsValidPlayer(nIdx))
	{
		if (IsValidNpc(Player[nIdx].m_nIndex))
		{
			g_GlobalScriptParam[SCRIPT_PARAM_PLAYER_INDEX] = nIdx;
		}
	}
}

int ScriptGetPlayerIndex()
{
	int nIndex = g_GlobalScriptParam[SCRIPT_PARAM_PLAYER_INDEX];
    if (nIndex >= MAX_PLAYER || nIndex < 0) 
 	{
 		_ASSERT(0);
 		return -1;
 	}
	else if (Player[nIndex].m_nIndex >= MAX_NPC || Player[nIndex].m_nIndex < 0)
 	{
 		_ASSERT(0);
 		return -1;
	}
    return nIndex;
}

void ScriptSetObjIndex(int nIndex)
{
    if (nIndex < MAX_OBJECT || nIndex > 0) 
	{
		if (Object[nIndex].m_nIndex == nIndex)
		{
			int nIndex = g_GlobalScriptParam[SCRIPT_PARAM_OBJ_INDEX];
		}
	}
}

int ScriptGetObjIndex()
{
	int nIndex = g_GlobalScriptParam[SCRIPT_PARAM_OBJ_INDEX];
    if (nIndex >= MAX_OBJECT || nIndex <= 0) 
	{
		_ASSERT(0);
		return -1;
	}
	if (Object[nIndex].m_nIndex != nIndex)
	{
		_ASSERT(0);
		return -1;
	}
    return nIndex;
}


void ScriptSetSubWorldIndex(int nIndex)
{
    if (nIndex < MAX_SUBWORLD && nIndex >= 0) 
	{
		if (SubWorld[nIndex].m_nIndex < MAX_SUBWORLD && SubWorld[nIndex].m_nIndex >= 0)
		{
			g_GlobalScriptParam[SCRIPT_PARAM_SUBWORLD_INDEX] = nIndex;
		}
	}
}

int ScriptGetSubWorldIndex()
{
	int nIndex = g_GlobalScriptParam[SCRIPT_PARAM_SUBWORLD_INDEX];
    if (nIndex >= MAX_SUBWORLD || nIndex < 0) 
	{
		_ASSERT(0);
		return -1;
	}
	if (SubWorld[nIndex].m_nIndex >= MAX_SUBWORLD || SubWorld[nIndex].m_nIndex < 0)
	{
		_ASSERT(0);
		return -1;
	}
    return nIndex;
}

void ScriptSetItemIndex(int nIdx)
{
	g_GlobalScriptParam[SCRIPT_PARAM_ITEM_INDEX] = 0;

	if ( nIdx > 0 && nIdx < MAX_ITEM )
	{
		g_GlobalScriptParam[SCRIPT_PARAM_ITEM_INDEX] = nIdx;
	}
}

int ScriptGetItemIndex()
{
	int nIndex = g_GlobalScriptParam[SCRIPT_PARAM_ITEM_INDEX];
    if (nIndex >= MAX_ITEM || nIndex < 0) 
 	{
 		return -1;
 	}
	else if (Item[nIndex].GetItemIndex() != nIndex )
 	{
 		return -1;
	}
    return nIndex;
}

#ifdef _SERVER
int ExecuteScript(DWORD scriptId, char* functionName, int nParam, int worldIndex)
{
	try
	{
		bool bExecuteScriptMistake = true;
		KLuaScript * pScript = (KLuaScript* )g_GetScript(scriptId);
		if (pScript)
		{
			ScriptSetPlayerIndex(0);
			ScriptSetSubWorldIndex(worldIndex);

			int nTopIndex = 0;
			
			pScript->SafeCallBegin(&nTopIndex);

			unsigned int nResultCount = 0;
			if (pScript->CallFunction(functionName, nResultCount, "d", nParam)) 
			{
				if (nResultCount)
				{
					if (Lua_IsNumber(pScript->m_LuaState, Lua_GetTopIndex(pScript->m_LuaState)) == 1)
					{
					}
				}
				bExecuteScriptMistake = false;
			}
			pScript->SafeCallEnd(nTopIndex);
		}
		
		if (bExecuteScriptMistake)
		{
			return FALSE;
		}
		
		return TRUE;
	}
	catch(...)
	{
		return FALSE;
	}
	return TRUE;
}
#endif

#ifdef _SERVER
int ExecuteScript(char* scriptFileName, char* functionName, int nParam, int worldIndex)
{
	DWORD dwScriptId = g_FileName2Id(scriptFileName);
	return ExecuteScript(dwScriptId, functionName, nParam, worldIndex);
}

bool ScriptMarriage(int nPlayerIdx1, int nPlayerIdx2)
{
	bool ret = false;
	if (IsValidPlayer(nPlayerIdx1) && IsValidPlayer(nPlayerIdx2))
	{
		if (Player[nPlayerIdx1].DoMarry(Player[nPlayerIdx2]))
		{
			ret = true;
		}
	}
	return ret;
}
#endif

#ifdef _SERVER

int LuaGetMaxPlayer(Lua_State *L)
{
    Lua_PushNumber(L, g_nMaxPlayer);
    return 1;
}

int LuaSetSkillSeries(Lua_State *L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();

	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	int nSkillSeries = (int)Lua_ValueToNumber(L, 1);

	if(nSkillSeries <= role_skillseries_invalid || nSkillSeries >= role_skillseries_count)
		return 0;

	if ( Player[nPlayerIndex].m_SkillSeries != (RoleSkillSeries)nSkillSeries )
	{
		Player[nPlayerIndex].m_SkillSeries = (RoleSkillSeries)nSkillSeries;

		PLAYER_SKILLSERIES_SYNC	sync;
		sync.ProtocolType = s2c_sync_skillseries;
		sync.m_Series = (BYTE)nSkillSeries;

		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &sync, sizeof(sync));

		int nWeaponIdx = Player[nPlayerIndex].m_ItemList.GetEquipment(itempart_weapon);
		if ( nWeaponIdx > 0 )
		{
			if ( Item[nWeaponIdx].IsInitWeapon() )
			{
				Player[nPlayerIndex].m_ItemList.Remove( nWeaponIdx );
				ItemSet.Remove(nWeaponIdx);
			}			
		}
	}

	return 0;
}

int LuaAddInitSkills(Lua_State *L)
{	
	int nPlayerIdx = ScriptGetPlayerIndex();
	if(!IsValidPlayer(nPlayerIdx))
		return 0;

	int nSkillSeries = (int)Lua_ValueToNumber(L, 1);

	if(nSkillSeries <= role_skillseries_invalid || nSkillSeries >= role_skillseries_count)
		return 0;

	if ( Player[nPlayerIdx].m_SkillSeries != (RoleSkillSeries)nSkillSeries )
	{
		Player[nPlayerIdx].m_SkillSeries = (RoleSkillSeries)nSkillSeries;

		PLAYER_SKILLSERIES_SYNC	sync;
		sync.ProtocolType = s2c_sync_skillseries;
		sync.m_Series = (BYTE)nSkillSeries;

		if (g_pServer != NULL)
			g_pServer->PackDataToClient(Player[nPlayerIdx].m_nNetConnectIdx, &sync, sizeof(sync));

		int nWeaponIdx = Player[nPlayerIdx].m_ItemList.GetEquipment(itempart_weapon);
		if ( nWeaponIdx > 0 )
		{
			if ( Item[nWeaponIdx].IsInitWeapon() )
			{
				Player[nPlayerIdx].m_ItemList.Remove( nWeaponIdx );
				ItemSet.Remove(nWeaponIdx);
			}			
		}
	}

	int nNpcIdx = Player[nPlayerIdx].m_nIndex;

	Npc[nNpcIdx].AddInitSkills();

	int nLevel = Player[nPlayerIdx].GetLevel();
	if ( nLevel > 0 && nLevel <= 120 )
	{
		int nIdx = 1;
		while ( nIdx <= nLevel  )
		{
			int nCurSpecialSkill = SpecialSkillTab::Singleton().GetSpecialSkill( 
				Player[nPlayerIdx].GetSeries(), 
				Player[nPlayerIdx].GetSkillSeries(), 
				nIdx );
			if ( nCurSpecialSkill > 0 )
			{
				NpcSkillList& skilllist = Npc[Player[nPlayerIdx].GetNpcIndex()].GetSkillList();
				int nSkillIdx = skilllist.FindSkill( nCurSpecialSkill );
				if ( nSkillIdx == INVALID_SKILL_INDEX )
				{
					skilllist.AddSkillEx( nCurSpecialSkill, 1, skill_status_usable );
					skilllist.NotifyAddSkill( nCurSpecialSkill, 1, skill_status_usable );
					//g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_STR, (BYTE*)(SKILLEXP_SKILL), strlen(SKILLEXP_SKILL));
				}
				else
				{
					int nCurLevel = skilllist.GetLevelByIdx( nSkillIdx );
					skilllist.LevelUpTo( nCurSpecialSkill, nCurLevel + 1 );//*/
				}				
			}
			++nIdx;
		}

		int nFirstBuffID = SpecialSkillTab::Singleton().GetNearlyFirstupBuffID( 
			Player[nPlayerIdx].GetSeries(), 
			Player[nPlayerIdx].GetSkillSeries(), 
			Player[nPlayerIdx].GetLevel() );
		if ( nFirstBuffID > 0 )
		{
			BuffMgr::Singleton().AddNpcBuff(nNpcIdx, nNpcIdx, nFirstBuffID );			
		}
	}

	return 0;
}

int LuaGiveMeSkill(Lua_State *L)
{
	LuaSetSkillSeries(L);
	
	int nPlayerIdx = ScriptGetPlayerIndex();

	if (!IsValidPlayer(nPlayerIdx))
		return 0;

	int nNpcIdx = Player[nPlayerIdx].m_nIndex;

	Npc[nNpcIdx].AddInitSkills();

	int	nSkillIdx;
	NpcSkillList::Iterator iter;

	while( (nSkillIdx = Npc[nNpcIdx].m_SkillList.NextSkillIdx(iter)) != INVALID_SKILL_INDEX )
	{
		Npc[nNpcIdx].m_SkillList.LevelUpTo(Npc[nNpcIdx].m_SkillList.GetIdByIdx(nSkillIdx), 1);
	}

	int nCurSpecialSkill = SpecialSkillTab::Singleton().GetNearlySpecialSkill( 
		Player[nPlayerIdx].GetSeries(), 
		Player[nPlayerIdx].GetSkillSeries(), 
		Player[nPlayerIdx].GetLevel() );
	if ( nCurSpecialSkill > 0 )
	{
		NpcSkillList& skilllist = Npc[Player[nPlayerIdx].GetNpcIndex()].GetSkillList();
		int nSkillIdx = skilllist.FindSkill( nCurSpecialSkill );
		if ( nSkillIdx == INVALID_SKILL_INDEX )
		{
			skilllist.AddSkillEx( nCurSpecialSkill, 1, skill_status_usable );
			skilllist.NotifyAddSkill( nCurSpecialSkill, 1, skill_status_usable );
		}
	}

	int nFirstBuffID = SpecialSkillTab::Singleton().GetNearlyFirstupBuffID( 
		Player[nPlayerIdx].GetSeries(), 
		Player[nPlayerIdx].GetSkillSeries(), 
		Player[nPlayerIdx].GetLevel() );
	if ( nFirstBuffID > 0 )
	{
		BuffMgr::Singleton().AddNpcBuff(nNpcIdx, nNpcIdx, nFirstBuffID );			
	}

	return 0;
}
 
int LuaSyncCombatResultOrg2( Lua_State *L )
{
	int nParamNum = Lua_GetTopIndex(L);
	if ( nParamNum < 4 )
		return 0;

	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	int nPlayerNpc   = Player[nPlayerIndex].GetNpcIndex();
	if (!IsValidNpc(nPlayerNpc))
		return 0;

	int   nSubwordIndex = Npc[nPlayerNpc].GetSubWorldIndex();
	if ( nSubwordIndex == INVALID_WORLD_INDEX || nSubwordIndex >= MAX_SUBWORLD || nSubwordIndex < 0)
		return 0;

	int   nPersistTime  = Lua_ValueToNumber(L,1);
	int   nScore1       = Lua_ValueToNumber(L,2);
	int   nScore2       = Lua_ValueToNumber(L,3);
	int   nSelfScore    = Lua_ValueToNumber(L,4);

	COMBAT_MAP_RESULT_ORG_2 mapResult;
	mapResult.Protocol = s2c_byte_extend;
	mapResult.ProtocolExtend = s2c_ex_protocol_combat_result_org_2;
	mapResult.wProtocolSize = sizeof(COMBAT_MAP_RESULT_ORG_2) - 1;
	mapResult.wCombatMapTemplateID = SubWorld[nSubwordIndex].GetWorldTemplateId();
	mapResult.dwPersistTime        = nPersistTime;
	
	mapResult.nOrg[0]      = 1;
	mapResult.nScore[0]    = nScore1;
	mapResult.nOrg[1]      = 2;
	mapResult.nScore[1]    = nScore2;
	mapResult.nSelfScoreGet= nSelfScore;
	
	if (g_pServer)
		g_pServer->PackDataToClient(Player[nPlayerIndex].GetNetConnectIdx(),&mapResult,sizeof(mapResult) );
	
	return 0;
}


int LuaAddSkillExp(Lua_State *L)
{
	int nPlayerIdx = ScriptGetPlayerIndex();

	if(IsValidPlayer(nPlayerIdx))
	{
		int nExp = Lua_ValueToNumber(L, 1);
		if (nExp > 0 && nExp <= MAX_ADD_SKILL_EXP)
		{
			//防沉迷
			int nAntiEnthrallState = Player[nPlayerIdx].m_AntiEnthrall.GetCurState();
			
			if(AntiEnthrall::enAntiEnthrall_Weariness == nAntiEnthrallState)
				nExp /= AntiEnthrall::WEARINESS_EXP_SCALE;
			else if(AntiEnthrall::enAntiEnthrall_Insalubrity == nAntiEnthrallState)
				return 0 ;

			if (nExp == 0)
				return 0 ;

			Player[nPlayerIdx].DirectAddSkillExp((DWORD)nExp);
			Player[nPlayerIdx].SyncAttribute(attr_SkillExp);
		}
	}

	return 0;
}

int LuaLevelUpSkill(Lua_State *L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();

	if(!IsValidPlayer(nPlayerIndex))
		return 0;

	int nSkillId = (int)Lua_ValueToNumber(L, 1);
	int nLevel = (int)Lua_ValueToNumber(L, 2);
	int nNpcIdx = Player[nPlayerIndex].m_nIndex;

	if( g_SkillManager.IsSubSkill(nSkillId) )
		nSkillId = Npc[nNpcIdx].m_SkillList.GetCurSameSubSkillId(nSkillId);
	
	Npc[nNpcIdx].m_SkillList.LevelUpTo(nSkillId, nLevel);
	
	return 0;
}

int LuaAddNpcCompAttr(Lua_State *L)
{
	int nNpcIndex = -1;
	int nValIdx   = -1;
	int nType     = -1;
	int nVal	  = -1;

	if (Lua_GetTopIndex(L) == 3)
	{
		int nPlayerIndex = ScriptGetPlayerIndex();

		if (!IsValidPlayer(nPlayerIndex))
			return 0;

		nNpcIndex = Player[nPlayerIndex].m_nIndex;

		nValIdx   = Lua_ValueToNumber(L, 1);
		nType     = Lua_ValueToNumber(L, 2);
		nVal	  = Lua_ValueToNumber(L, 3);
	}
	else if (Lua_GetTopIndex(L) == 4)
	{
		nNpcIndex = Lua_ValueToNumber(L, 1);
		nValIdx   = Lua_ValueToNumber(L, 2);
		nType     = Lua_ValueToNumber(L, 3);
		nVal      = Lua_ValueToNumber(L, 4);
	}
	else
	{
		return 0;
	}

	if(!IsValidNpc(nNpcIndex))
		return 0;

	if(nValIdx < 0 || nValIdx >= ncai_end)
		return 0;

	if(nType < idx_base_value || nType > idx_current_value)	//注意：CompAttr的idx_base_value的这些值会存盘
		return 0;

	Npc[nNpcIndex].AddCompAttr( (enNpc_CompoundAttr_Idx)nValIdx, (enCompoundMemIdx)nType, nVal );
	Npc[nNpcIndex].SyncAttr(npc_attr_comp, nValIdx, 1 << nType);

	return 0;
}

int LuaChatInRoomByStringID(Lua_State *L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if( nNpcIdx < 0 || nNpcIdx >= MAX_NPC )
		return 0;

	int nChannelId = (int)Lua_ValueToNumber(L, 2);
	if(nChannelId < SYSTEM_ROOM_ID || nChannelId > COMBAT_INFO_ROOM_ID)
		return 0;

	int nStringId = (int)Lua_ValueToNumber(L, 3);
	if(nStringId < 1 || nStringId > 60000)
		return 0;


	g_ChatCenterS.ChatInRoomByStringID( nNpcIdx, nChannelId, nStringId);

	return 0;
}

int LuaChatInRoom(Lua_State *L)
{
	int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if( nNpcIdx < 0 || nNpcIdx >= MAX_NPC )
		return 0;

	int nChannelId = (int)Lua_ValueToNumber(L, 2);
	if(nChannelId < SYSTEM_ROOM_ID || nChannelId > COMBAT_INFO_ROOM_ID)
		return 0;

	/*
	char szString[MAXSIZE_CHAT_MSG];
	const char* szBuff = Lua_ValueToString(L, 3);
	strncpy( szString, szBuff, MAXSIZE_CHAT_MSG );
	g_ChatCenterS.ChatInRoomByString( nNpcIdx, nChannelId, szString, MAXSIZE_CHAT_MSG);
	//*/

	return 0;
}


int LuaAddNpcRAttr(Lua_State *L)
{
	int nNpcIndex = -1;
	int nAttrIdx  = -1;
	int nValPart  = -1;
	int nValType  = -1;
	int nVal      = 0;

	if (Lua_GetTopIndex(L) == 4)
	{
		int nPlayerIndex = ScriptGetPlayerIndex();

		if (!IsValidPlayer(nPlayerIndex))
			return 0;

		nNpcIndex = Player[nPlayerIndex].m_nIndex;
		
		nAttrIdx  = Lua_ValueToNumber(L, 1);
		nValPart  = Lua_ValueToNumber(L, 2);
		nValType  = Lua_ValueToNumber(L, 3);
		nVal	  = Lua_ValueToNumber(L, 4);
	}
	else if (Lua_GetTopIndex(L) == 5)
	{
		nNpcIndex = Lua_ValueToNumber(L, 1);
		nAttrIdx  = Lua_ValueToNumber(L, 2);
		nValPart  = Lua_ValueToNumber(L, 3);
		nValType  = Lua_ValueToNumber(L, 4);
		nVal	  = Lua_ValueToNumber(L, 5);
	}
	else
	{
		return 0;
	}

	if (!IsValidNpc(nNpcIndex))
		return 0;

	if(nAttrIdx < 0 || nAttrIdx > nrai_end)
		return 0;

	if(nValPart != idx_value_low && nValPart != idx_value_hight)
		return 0;

	if(nValType < idx_base_value || nValType > idx_current_value)
		return 0;

	Npc[nNpcIndex].AddRangeAttr((enNpc_RangeAttr_Idx)nAttrIdx, (enRangeMemberIdx)nValPart,
			(enCompoundMemIdx)nValType, nVal);

	BYTE valMask = (1 << nValType) << ( (nValPart == idx_value_low) ? 0 : 4 );
	Npc[nNpcIndex].SyncAttr(npc_attr_range, nAttrIdx, valMask);

	return 0;
}

int LuaAddNpcCurAttr(Lua_State *L)
{
	int nNpcIndex = -1;
	int nValIdx   = -1;
	int nVal      = 0;

	if (Lua_GetTopIndex(L) == 2)
	{
		int nPlayerIndex = ScriptGetPlayerIndex();

		if (!IsValidPlayer(nPlayerIndex))
			return 0;

		nNpcIndex = Player[nPlayerIndex].m_nIndex;

		nValIdx = Lua_ValueToNumber(L, 1);
		nVal    = Lua_ValueToNumber(L, 2);
	}
	else if (Lua_GetTopIndex(L) == 3)
	{
		nNpcIndex = Lua_ValueToNumber(L, 1);
		nValIdx   = Lua_ValueToNumber(L, 2);
		nVal	  = Lua_ValueToNumber(L, 3);
	}
	else
	{
		return 0;
	}

	if (!IsValidNpc(nNpcIndex))
		return 0;

	if(nValIdx < 0 || nValIdx >= nuai_end)
		return 0;

	Npc[nNpcIndex].AddUnaryAttr( (enNpc_UnaryAttr_Idx)nValIdx, nVal );

	switch(nValIdx)
	{
	case nuai_curlife:
		if( Npc[nNpcIndex].m_UnaryAttrMgr[nuai_curlife] > Npc[nNpcIndex].m_CompAttrMgr[ncai_lifeuplimit] )
			Npc[nNpcIndex].m_UnaryAttrMgr.Set(nuai_curlife, Npc[nNpcIndex].m_CompAttrMgr[ncai_lifeuplimit]);
		break;

	case nuai_curmana:
		if( Npc[nNpcIndex].m_UnaryAttrMgr[nuai_curmana] > Npc[nNpcIndex].m_CompAttrMgr[ncai_manauplimit] )
			Npc[nNpcIndex].m_UnaryAttrMgr.Set(nuai_curmana, Npc[nNpcIndex].m_CompAttrMgr[ncai_manauplimit]);
		break;
	}

	Npc[nNpcIndex].SyncAttr(npc_attr_unary, nValIdx, true);

	return 0;
}

int LuaGetNpcCompAttr(Lua_State *L)
{
	int nNpcIndex = -1;
	int nValIdx   = -1;
	int nType     = -1;

	if (Lua_GetTopIndex(L) == 2)
	{
		int nPlayerIndex = ScriptGetPlayerIndex();

		if (!IsValidPlayer(nPlayerIndex))
			return 0;

		nNpcIndex = Player[nPlayerIndex].m_nIndex;

		nValIdx   = Lua_ValueToNumber(L, 1);
		nType     = Lua_ValueToNumber(L, 2);
	}
	else if (Lua_GetTopIndex(L) == 3)
	{
		nNpcIndex = Lua_ValueToNumber(L, 1);
		nValIdx   = Lua_ValueToNumber(L, 2);
		nType     = Lua_ValueToNumber(L, 3);
	}
	else
	{
		return 0;
	}

	if (!IsValidNpc(nNpcIndex))
		return 0;

	if(nValIdx < 0 || nValIdx >= ncai_end)
		return 0;

	if(nType < idx_base_value || nType > idx_current_value)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_CompAttrMgr[(enNpc_CompoundAttr_Idx)nValIdx][(enCompoundMemIdx)nType]);
	return 1;
}

int LuaGetNpcRAttr(Lua_State *L)
{

	int nNpcIndex   = -1;
	int nAttrIdx    = -1;
	int nValPart    = -1;
	int nValType	= -1;
	
	if (Lua_GetTopIndex(L) == 3)
	{
		int nPlayerIndex = ScriptGetPlayerIndex();
		
		if (!IsValidPlayer(nPlayerIndex))
			return 0;
		
		nNpcIndex = Player[nPlayerIndex].m_nIndex;

		nAttrIdx  = Lua_ValueToNumber(L, 1);
		nValPart  = Lua_ValueToNumber(L, 2);
		nValType  = Lua_ValueToNumber(L, 3);
	}
	else if (Lua_GetTopIndex(L) == 4)
	{
		nNpcIndex = Lua_ValueToNumber(L, 1);
		nAttrIdx  = Lua_ValueToNumber(L, 2);
		nValPart  = Lua_ValueToNumber(L, 3);
		nValType  = Lua_ValueToNumber(L, 4);
	}
	else
	{
		return 0;
	}

	if (!IsValidNpc(nNpcIndex))
		return 0;

	if(nAttrIdx < 0 || nAttrIdx > nrai_end)
		return 0;

	if(nValPart != idx_value_low && nValPart != idx_value_hight)
		return 0;

	if(nValType < idx_base_value || nValType > idx_current_value)
		return 0;

	Lua_PushNumber(L, Npc[nNpcIndex].m_RangeAttrMgr[(enNpc_RangeAttr_Idx)nAttrIdx][(enRangeMemberIdx)nValPart][(enCompoundMemIdx)nValType]);
	return 1;
}

int LuaGetNpcCurAttr(Lua_State *L)
{
	int nNpcIndex = -1;
	int nValId    = -1;

	if (Lua_GetTopIndex(L) == 1)
	{
		int nPlayerIndex = ScriptGetPlayerIndex();

		if (!IsValidPlayer(nPlayerIndex))
			return 0;

		nNpcIndex = Player[nPlayerIndex].m_nIndex;
		nValId    = Lua_ValueToNumber(L, 1);
	}
	else if (Lua_GetTopIndex(L) == 2)
	{
		nNpcIndex = Lua_ValueToNumber(L, 1);
		nValId    = Lua_ValueToNumber(L, 2);
	}
	else
	{
		return 0;
	}

	if (!IsValidNpc(nNpcIndex))
		return 0;

	if (nValId < 0 || nValId >= nuai_end)
		return 0;

	int nValue = Npc[nNpcIndex].m_UnaryAttrMgr[(enNpc_UnaryAttr_Idx)nValId];
	Lua_PushNumber(L, nValue);

	return 1;
}

#endif

#ifdef _SERVER
int LuaAddBuffToGens(Lua_State * L)
{
	int   nRes = 0;
	const int nPlayerIndex = ScriptGetPlayerIndex();

	if (Lua_GetTopIndex(L) >= 1 && IsValidPlayer(nPlayerIndex))
	{
		int nBuffID         = Lua_ValueToNumber(L,1);

		SocialUnit     * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
		if (pLeafUnit)
		{
			SocialUnit * pGensUnit = GetUpNUnit(pLeafUnit,enSULayer_Gens);
			if (pGensUnit)
			{
				if (pGensUnit->IsOwner(Player[nPlayerIndex].GetPlayerName()))
				{
					SocialUnit::UnitIterator iter;
					SocialUnit	            *pSubUnit = NULL;
					
					while( pSubUnit = pGensUnit->NextSubUnit(iter) )
					{
						if (pLeafUnit != pSubUnit)
						{
							if(INVALID_BUFF_ID != nBuffID)
							{
								int nReciver = g_PlayerInfoToIndex.GetIndexByName( pSubUnit->GetOwnerName() );
								
								if(INVALID_PLAYER_INDEX != nReciver)
								{
									BuffMgr::Singleton().AddNpcBuff(Player[nPlayerIndex].m_nIndex, 
										Player[nReciver].m_nIndex,
										nBuffID
									);
								}
								
							}//endif

						}///endif

					}//end for while
					
					nRes = 1;
				}//endif
				
			}//endif

		}//endif

	}//endif
	
	Lua_PushNumber(L,nRes);

	return 1;
}

int LuaAddBuffToTongMember(Lua_State * L)
{
	int nRes = 0;
	const int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (Lua_GetTopIndex(L) >= 1 && IsValidPlayer(nPlayerIndex))
	{
		int nBuffID         = Lua_ValueToNumber(L,1);
		
		SocialUnit     * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
		if (pLeafUnit)
		{
			SocialUnit * pTongUnit = GetUpNUnit(pLeafUnit,enSULayer_Tong);
			
			if (pTongUnit)
			{
				if (pTongUnit->IsOwner(Player[nPlayerIndex].GetPlayerName()))
				{
					SocialUnit::UnitIterator GensIter;
					SocialUnit	            *pSubGensUnit = NULL;
					
					while( pSubGensUnit = pTongUnit->NextSubUnit(GensIter) )
					{
						SocialUnit::UnitIterator playerIter;
						SocialUnit             * pSubPlayerUnit = NULL;

						while ( pSubPlayerUnit = pSubGensUnit->NextSubUnit(playerIter))
						{
							if( pLeafUnit != pSubPlayerUnit && INVALID_BUFF_ID != nBuffID)
							{
								int nReciver = g_PlayerInfoToIndex.GetIndexByName( pSubPlayerUnit->GetOwnerName() );
								
								if(INVALID_PLAYER_INDEX != nReciver)
								{
									BuffMgr::Singleton().AddNpcBuff(Player[nPlayerIndex].m_nIndex, 
										Player[nReciver].m_nIndex,
										nBuffID
									);
								}
								
							}//endif

						}
						
					}//end for while
					
					nRes = 1;
				}//endif
				
			}//endif
			
		}//endif
		
	}//endif

	Lua_PushNumber(L,nRes);
	return 1;
}

int LuaAddBuffToTong(Lua_State * L)
{
	int   nRes = 0;
	const int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (Lua_GetTopIndex(L) >= 1 && IsValidPlayer(nPlayerIndex))
	{
		int nBuffID         = Lua_ValueToNumber(L,1);
		
		SocialUnit     * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
		if (pLeafUnit)
		{
			SocialUnit * pTongUnit = GetUpNUnit(pLeafUnit,enSULayer_Tong);
			SocialUnit * pGensUnit = GetUpNUnit(pLeafUnit,enSULayer_Gens);

			if (pTongUnit)
			{
				if (pTongUnit->IsOwner(Player[nPlayerIndex].GetPlayerName()))
				{
					SocialUnit::UnitIterator iter;
					SocialUnit	            *pSubUnit = NULL;
					
					while( pSubUnit = pTongUnit->NextSubUnit(iter) )
					{
						if (pGensUnit != pSubUnit)
						{
							if(INVALID_BUFF_ID != nBuffID)
							{
								int nReciver = g_PlayerInfoToIndex.GetIndexByName( pSubUnit->GetOwnerName() );
								
								if(INVALID_PLAYER_INDEX != nReciver)
								{
									BuffMgr::Singleton().AddNpcBuff(Player[nPlayerIndex].m_nIndex, 
										Player[nReciver].m_nIndex,
										nBuffID
										);
								}
								
							}//endif
						}//endif

					}//end for while
					
					nRes = 1;
				}//endif
				
			}//endif
			
		}//endif
		
	}//endif
	
	Lua_PushNumber(L,nRes);
	
	return 1;
}


int LuaIsJoinUnit(Lua_State *L)
{
	const int nPlayerIdx = ScriptGetPlayerIndex();

	if( IsValidPlayer(nPlayerIdx) )
	{
		int nLayer = (int)Lua_ValueToNumber(L, 1);

		if(nLayer >= enSULayer_Player + 1 && nLayer < enSUTong_LayerNum)
		{
			SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
			SocialUnit *pTargetUnit = GetUpNUnit(pLeafUnit, nLayer);

			if(NULL != pTargetUnit)
			{
				Lua_PushNumber(L, 1);
				return 1;
			}
		}
	}
 
	Lua_PushNumber(L, 0);
	return 1;
}

int luaWearWeapon(Lua_State * L)
{

	return 0;
}

int luaGetUnitParentNodeName(Lua_State * L)
{
    const int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		int nLayer = (int)Lua_ValueToNumber(L, 1);
       	if( nLayer >= enSULayer_Player + 1 && nLayer < enSUTong_LayerNum)
		{	
			RelationSet & relation  = Player[nPlayerIndex].GetRelationSet();	
			RelationRecord * pRec   = relation.GetRelationByTemplate(enSUTplId_Tong);
			
			if (pRec && relation.IsOwnTreeLoad(enSUTplId_Tong))
			{
				SocialUnit * pLeafUnit  = pRec->pLeafUnit;
				if (pLeafUnit)
				{
					int          nCurLayer  = enSULayer_Player;
					SocialUnit * pParent    = pLeafUnit;
                    while (pParent && nCurLayer < enSUTong_LayerNum && nCurLayer!= nLayer)
					{
						pParent = pParent->GetParent();
						++nCurLayer;  
					}//end while

					if (nCurLayer == nLayer && pParent)
					{
						SocialUnitAttr & pAttr = pParent->GetUnitAttr();
                        if (pAttr.IsAttrExist(enSUAttr_UnitName))
						{
							char * szUnitName=NULL;
							int    nSize=pAttr.GetAttr(enSUAttr_UnitName,szUnitName);
							if (szUnitName && nSize)
							{
								Lua_PushString(L,szUnitName);
								return 1;
							}//endif

						}//endif

					}//endif

				}//endif
				
			}//endif
		}//endif

	}//endif

	Lua_PushNil(L);
	return 1;
}

int luaScriptDialogNpc(Lua_State *L)
{
    const int nPlayerIndex      = ScriptGetPlayerIndex();
	const int nTargetNpcIdex    = (int)Lua_ValueToNumber(L, 1);

    if (!IsValidPlayer(nPlayerIndex))
		return 0;

	KPlayer & player          = Player[nPlayerIndex];
	if (nTargetNpcIdex <= 0)
		return 0;
	
	KNpc& dialogNpc = Npc[nTargetNpcIdex];	
	
	bool  bCanTalk  = false;
	// 小于对话半径就开始对话
	const int npcKind = dialogNpc.GetKind();
	if ((npcKind == kind_dialoger)
		|| (npcKind== kind_siege_weapon)
		|| (npcKind == kind_building)
		|| (NpcSet.GetRelation(player.GetNpcIndex(), nTargetNpcIdex) == relation_none)
		|| (NpcSet.GetRelation(player.GetNpcIndex(), nTargetNpcIdex) == relation_dialog))
	{
		int distance = NpcSet.GetDistance(nTargetNpcIdex, player.GetNpcIndex());
		if (distance <= dialogNpc.GetDialogRadius() * 2)
		{
			bCanTalk = true;			
		}//endif
	}//endif
	
	if (bCanTalk)
	{
		int dialogTime = dialogNpc.GetActionTime();
		if (dialogTime >= 0)
		{
			DelayedAction dialogNpcAction(nPlayerIndex, delayed_action_dialog_npc, dialogTime, delayed_action_msg_dialog_npc);
			dialogNpcAction.GetDialogNpcParam().m_DialogNpcId = Npc[nTargetNpcIdex].m_dwID;
			player.GetActionDelayer().NewAction(dialogNpcAction);				
		}//endif
		
	}//endif

	return 0;
}

int LuaGetCityTaxRate(Lua_State  *L)
{
	const int nPlayerIndex = ScriptGetPlayerIndex();

	if( IsValidPlayer(nPlayerIndex) )
	{
		int nWorldIdx = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
		int nWorldLordIdx = SubWorld[nWorldIdx].GetLord();

		if( IsValidNpc(nWorldLordIdx) )
		{
			int nTaxRate = Npc[nWorldLordIdx].m_UnaryAttrMgr[nuai_city_taxrate];
			Lua_PushNumber(L, nTaxRate);
			return 1;
		}
	}

	return 0;
}

int LuaGetTongID(Lua_State * L)
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	if(!IsValidPlayer(nPlayerIndex))
		return 0;
	
	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIndex, enSUTplId_Tong);
	SocialUnit *pGensUnit = GetUpNUnit(pLeafUnit, enSULayer_Gens);

	Lua_PushNumber(L, (unsigned int)pGensUnit);

	return 1;		
}

int luaGetSocialScriptData(Lua_State * L)
{
	const int     nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	const int     nLayer    = (int)Lua_ValueToNumber(L, 2);
	const int     nDataIdx  = (int)Lua_ValueToNumber(L, 3);
	unsigned long nRet      = 0;

	if (IsValidNpc(nNpcIndex) && nLayer >= enSULayer_Player && nLayer <= enSULayer_League)
	{
		if (Npc[nNpcIndex].IsPlayer())
		{
			int nPlayerIndex = Npc[nNpcIndex].GetPlayerIdx();
			if (IsValidPlayer(nPlayerIndex))
			{
				SocialUnit * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
				if (pLeafUnit)
				{
					SocialUnit * pNUnit = GetUpNUnit(pLeafUnit,nLayer);
					if (pNUnit)
					{
						pNUnit->GetScriptUseData(nDataIdx,nRet);
					}//endif

				}//endif
					
			}//endif

		}//endif
		else
		{
			FSGUID lordGuid           = Npc[nNpcIndex].GetLord();
			ServerSocialUnitMgr & mgr = ServerSocialUnitMgr::Singleton();
			SocialUnit * pStartUnit   = mgr.GetUnit(lordGuid,enSUTplId_Tong) ;
			if (pStartUnit)
			{
				SocialUnit * pNUnit = GetUpNUnit(pStartUnit,nLayer);
				if (pNUnit)
				{
					pNUnit->GetScriptUseData(nDataIdx,nRet);
				}//endif

			}//endif

		}//end else

	}//endif

	Lua_PushNumber(L, nRet);

	return 1;
}

int luaSetSocialScriptData(Lua_State * L)
{
	const int     nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	const int     nLayer    = (int)Lua_ValueToNumber(L, 2);
	const int     nDataIdx  = (int)Lua_ValueToNumber(L, 3);
	const int     nValue    = (int)Lua_ValueToNumber(L, 4);
	unsigned long nRet      = 0;
	
	if (IsValidNpc(nNpcIndex) && nLayer >= enSULayer_Player && nLayer <= enSULayer_League)
	{
		if (Npc[nNpcIndex].IsPlayer())
		{
			int nPlayerIndex = Npc[nNpcIndex].GetPlayerIdx();
			if (IsValidPlayer(nPlayerIndex))
			{
				SocialUnit * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
				if (pLeafUnit)
				{
					SocialUnit * pNUnit = GetUpNUnit(pLeafUnit,nLayer);
					if (pNUnit)
					{
						pNUnit->SetScriptUseData(nValue,nDataIdx);
					}//endif
					
				}//endif
				
			}//endif
			
		}//endif
		else
		{
			FSGUID lordGuid           = Npc[nNpcIndex].GetLord();
			ServerSocialUnitMgr & mgr = ServerSocialUnitMgr::Singleton();
			SocialUnit * pStartUnit   = mgr.GetUnit(lordGuid,enSUTplId_Tong) ;
			if (pStartUnit)
			{
				SocialUnit * pNUnit = GetUpNUnit(pStartUnit,nLayer);
				if (pNUnit)
				{
					pNUnit->SetScriptUseData(nValue,nDataIdx);
				}//endif
				
			}//endif
			
		}//end else
		
	}//endif
	
	return 0;
}

int luaGetSocialSaveScriptData(Lua_State * L)
{
	const int     nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	const int     nLayer    = (int)Lua_ValueToNumber(L, 2);
	const int     nDataIdx  = (int)Lua_ValueToNumber(L, 3);
	unsigned long nRet      = 0;
	
	if (IsValidNpc(nNpcIndex) && nLayer > enSULayer_Player && nLayer <= enSULayer_League)	//only 氏族及其以上的社会关系节点才有这个存盘脚本变量
	{
		if (Npc[nNpcIndex].IsPlayer())
		{
			int nPlayerIndex = Npc[nNpcIndex].GetPlayerIdx();
			if (IsValidPlayer(nPlayerIndex))
			{
				SocialUnit * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
				if (pLeafUnit)
				{
					SocialUnit * pNUnit = GetUpNUnit(pLeafUnit,nLayer);
					if (pNUnit)
					{
						GetSaveScriptData(pNUnit, nDataIdx, &nRet);
					}//endif
					
				}//endif
				
			}//endif
			
		}//endif
		else
		{
			FSGUID lordGuid           = Npc[nNpcIndex].GetLord();
			ServerSocialUnitMgr & mgr = ServerSocialUnitMgr::Singleton();
			SocialUnit * pStartUnit   = mgr.GetUnit(lordGuid,enSUTplId_Tong) ;
			if (pStartUnit)
			{
				SocialUnit * pNUnit = GetUpNUnit(pStartUnit,nLayer);
				if (pNUnit)
				{
					GetSaveScriptData(pNUnit, nDataIdx, &nRet);
				}//endif
				
			}//endif
			
		}//end else
		
	}//endif
	
	Lua_PushNumber(L, nRet);
	
	return 1;
}

int luaSetSocialSaveScriptData(Lua_State * L)
{
	const int     nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	const int     nLayer    = (int)Lua_ValueToNumber(L, 2);
	const int     nDataIdx  = (int)Lua_ValueToNumber(L, 3);
	const int     nValue    = (int)Lua_ValueToNumber(L, 4);
	unsigned long nRet      = 0;
	
	if (IsValidNpc(nNpcIndex) && nLayer > enSULayer_Player && nLayer <= enSULayer_League) //only 氏族及其以上的社会关系节点才有这个存盘脚本变量
	{
		if (Npc[nNpcIndex].IsPlayer())
		{
			int nPlayerIndex = Npc[nNpcIndex].GetPlayerIdx();
			if (IsValidPlayer(nPlayerIndex))
			{
				SocialUnit * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
				if (pLeafUnit)
				{
					SocialUnit * pNUnit = GetUpNUnit(pLeafUnit,nLayer);
					if (pNUnit)
					{
						SetSaveScriptData(pNUnit, nDataIdx, nValue);
					}//endif
					
				}//endif
				
			}//endif
			
		}//endif
		else
		{
			FSGUID lordGuid           = Npc[nNpcIndex].GetLord();
			ServerSocialUnitMgr & mgr = ServerSocialUnitMgr::Singleton();
			SocialUnit * pStartUnit   = mgr.GetUnit(lordGuid,enSUTplId_Tong) ;
			if (pStartUnit)
			{
				SocialUnit * pNUnit = GetUpNUnit(pStartUnit,nLayer);
				if (pNUnit)
				{
					SetSaveScriptData(pNUnit, nDataIdx, nValue);
				}//endif
				
			}//endif
			
		}//end else
		
	}//endif
	
	return 0;
}

int LuaGetCoupleName(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	unsigned int nameLength = sizeof(Player[nPlayerIndex].m_MarriageInfo.m_CoupleName);
	Player[nPlayerIndex].m_MarriageInfo.m_CoupleName[nameLength - 1] = 0;
	Lua_PushString(L, Player[nPlayerIndex].m_MarriageInfo.m_CoupleName);
	return 1;
}

int LuaIsCoupleOnline(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	bool ret = false;
	unsigned int nameLength = sizeof(Player[nPlayerIndex].m_MarriageInfo.m_CoupleName);
	if (strncmp(Player[nPlayerIndex].m_MarriageInfo.m_CoupleName, "", nameLength) != 0)
	{
		Player[nPlayerIndex].m_MarriageInfo.m_CoupleName[nameLength - 1] = 0;
		int coupleIdx = g_PlayerInfoToIndex.GetIndexByName(Player[nPlayerIndex].m_MarriageInfo.m_CoupleName);
		ret = (INVALID_PLAYER_INDEX != coupleIdx);
	}

	Lua_PushNumber(L, ret);
	return 1;
}

int LuaGetMarriageTime(Lua_State * L)
{
	int ret = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		unsigned long marriageTime = Player[nPlayerIndex].m_MarriageInfo.m_MarriageTime;

		Lua_PushNumber(L, marriageTime);
		ret = 1;
	}
	return ret;
}

int LuaSendInvitation(Lua_State * L)
{
	int ret = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();

	if (Lua_GetTopIndex(L) != 2)
	{
		return 0;
	}

	const char * szTitle = NULL;
	if (Lua_IsString(L, 1))
	{
		szTitle = Lua_ValueToString(L, 1);
	}
	const char * szContent = NULL;
	if (Lua_IsString(L, 2))
	{
		szContent = Lua_ValueToString(L, 2);
	}

	if (IsValidPlayer(nPlayerIndex))
	{
		if (Player[nPlayerIndex].SendInvitation(szTitle, szContent))
		{
			ret = 1;
		}
	}

	Lua_PushNumber(L, ret);
	return ret;
}

int LuaGetCoupleLastOffLineTime(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	DWORD tLastPlayerTime = Player[nPlayerIndex].m_MarriageInfo.m_CoupleLastOffLineTime;

	Lua_PushNumber(L, tLastPlayerTime);
	return 1;
}

int LuaIsMyFriend(Lua_State* L)
{
	int nRet = 0;

	if (Lua_GetTopIndex(L) != 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nTargetPlayerIndex = Lua_ValueToNumber(L, 1);
	if (!IsValidPlayer(nTargetPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	ChatObjectMgr_S	*pObjMgr = g_ChatCenterS.GetChatObjMgr(nPlayerIndex);
	if (!pObjMgr)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	nRet = pObjMgr->IsMyFriend(nTargetPlayerIndex);

	Lua_PushNumber(L, nRet);
	return 1;
}

int LuaGetLastPlayTime(Lua_State* L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	DWORD tLastPlayerTime = Player[nPlayerIndex].m_dwLastOfflineTime;

	Lua_PushNumber(L, tLastPlayerTime);
	return 1;
}

int LuaGetSocialOnlinePlayerNumber(Lua_State* L)
{
	const int nPlayerIdx = Lua_ValueToNumber(L, 1);
	if ( !IsValidPlayer(nPlayerIdx) )
		return 0;

	int nLayer = Lua_ValueToNumber(L, 2);
	if(nLayer <= enSULayer_Player || nLayer >= enSUTong_LayerNum)
		return 0;

	SocialUnit* pLeafUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
	if ( !pLeafUnit )
		return 0;

	SocialUnit* pUnit     = GetUpNUnit(pLeafUnit, nLayer);
	if ( !pUnit )
		return 0;

	int nNumberOfOnlinePlayer = pUnit->GetOnlinePlayerNum();

	Lua_PushNumber(L, nNumberOfOnlinePlayer);
	return 1;
}

int LuaGetSocialUnitName(Lua_State *L)
{
	const int nPlayerIdx = ScriptGetPlayerIndex();
	if( !IsValidPlayer(nPlayerIdx) )
		return 0;

	int nLayer = (int)Lua_ValueToNumber(L, 1);

	if(nLayer <= enSULayer_Player || nLayer >= enSUTong_LayerNum)
		return 0;

	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
	SocialUnit *pUpUnit = GetUpNUnit(pLeafUnit, nLayer);

	if(NULL == pUpUnit)
		return 0;

	const char *szUnitName = GetUnitName( pUpUnit->GetUnitAttr() );

	if(NULL != szUnitName)
		Lua_PushString(L, szUnitName);
	else
		Lua_PushNil(L);

	return 1;
}

int LuaAddCityRes(Lua_State *L)
{
	int nWorldId = (int)Lua_ValueToNumber(L, 1);
	int nWorldIdx = g_SubWorldSet.SearchWorld(nWorldId);

	if(INVALID_WORLD_INDEX != nWorldIdx)
	{
		int nResIdx = (int)Lua_ValueToNumber(L, 2);

		if(nResIdx >= 0 && nResIdx < CITY_RES_TYPE_COUNT)
		{
			double nVal = Lua_ValueToNumber(L, 3);	
			int nLordIdx = SubWorld[nWorldIdx].GetLord();

			if ( nResIdx == 0 )	//对金钱做特例
			{
				nVal = nVal / 10000;
			}

			if(INVALID_WORLDLORDNPC_INDEX != nLordIdx)
			{
				Npc[nLordIdx].AddCityRes(nResIdx, (int )nVal);
			}
		}
	}

	return 0;
}

int LuaGetCityRes(Lua_State *L)
{
	int nWorldId = (int)Lua_ValueToNumber(L, 1);
	int nWorldIdx = g_SubWorldSet.SearchWorld(nWorldId);

	if(INVALID_WORLD_INDEX != nWorldIdx)
	{
		int nResIdx = (int)Lua_ValueToNumber(L, 2);
		
		if(nResIdx >= 0 && nResIdx < CITY_RES_TYPE_COUNT)
		{
			int nLordIdx = SubWorld[nWorldIdx].GetLord();

			if(INVALID_WORLDLORDNPC_INDEX != nLordIdx)
			{
				int nVal = Npc[nLordIdx].m_UnaryAttrMgr[nuai_lord_res0 + nResIdx];
				if (nResIdx == 0)
				{
					double finalResult = (double)nVal * 10000;

					Lua_PushNumber(L, finalResult);
				}//endif
				else
					Lua_PushNumber(L, nVal);
				return 1;
			}				
		}
	}

	return 0;
}

int LuaIsSameLord(Lua_State *L)
{
	const int nPlayerIdx = ScriptGetPlayerIndex();

	if( IsValidPlayer(nPlayerIdx) )
	{
		int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
		int nLayer = (int)Lua_ValueToNumber(L, 2);

		if( (nNpcIdx > 0 && nNpcIdx < MAX_NPC) && 
			(nLayer >= enSULayer_Player && nLayer < enSUTong_LayerNum)
		  )
		{
			if (Npc[nNpcIdx].IsPlayer())
			{
				int targetPlayerIndex = Npc[nNpcIdx].GetPlayerIdx();

				if (!IsValidPlayer(targetPlayerIndex))
				{
					Lua_PushNumber(L, 0);
					return 1;
				}

				SocialUnit * playerNode1 = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
				SocialUnit * playerNode2 = GetLeafUnit(targetPlayerIndex, enSUTplId_Tong);

				if (NULL == playerNode1 || NULL == playerNode2)
				{
					Lua_PushNumber(L, 0);
					return 1;
				}

				SocialUnit * socialNode1 = GetUpNUnit(playerNode1, nLayer);
				SocialUnit * socialNode2 = GetUpNUnit(playerNode2, nLayer);

				if (NULL == socialNode1 || NULL == socialNode2)
				{
					Lua_PushNumber(L, 0);
					return 1;
				}

				const FSGUID & unitGUID1 = socialNode1->GetUnitGuid();
				const FSGUID & unitGUID2 = socialNode2->GetUnitGuid();

				if (unitGUID1 == unitGUID2)
				{
					Lua_PushNumber(L, 1);
					return 1;
				}
			}
			else
			{
				SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
				if (NULL == pLeafUnit)
				{
					Lua_PushNumber(L, 0);
					return 1;
				}
				
				SocialUnit *pTargetUnit = GetUpNUnit(pLeafUnit, nLayer);
				
				if(NULL != pTargetUnit)
				{
					const FSGUID &npcGuid = Npc[nNpcIdx].GetLord();
					const FSGUID &unitGuid = pTargetUnit->GetUnitGuid();
					
					if(npcGuid == unitGuid)
					{
						Lua_PushNumber(L, 1);
						return 1;
					}
				}
			}
		}
	}

	Lua_PushNumber(L, 0);
	return 1;
}

int LuaIsUnitOwner(Lua_State *L)
{
	const int nPlayerIdx = ScriptGetPlayerIndex();

	if( IsValidPlayer(nPlayerIdx) )
	{
		int nLayer = (int)Lua_ValueToNumber(L, 1);

		if(nLayer > enSULayer_Player && nLayer < enSUTong_LayerNum)
		{
			if( IsUnitOwner(nPlayerIdx, enSUTplId_Tong, nLayer) )
			{
				Lua_PushNumber(L, 1);
				return 1;
			}
		}
	}

	Lua_PushNumber(L, 0);
	return 1;
}

int LuaIsNpcLordOwner(Lua_State *L)
{
	const int nPlayerIdx = ScriptGetPlayerIndex();

	if( IsValidPlayer(nPlayerIdx) )
	{
		int nNpcIdx = (int)Lua_ValueToNumber(L, 1);

		if(nNpcIdx > 0 && nNpcIdx < MAX_NPC)
		{
			const FSGUID &npcLord = Npc[nNpcIdx].GetLord();
			SocialUnit *pUnit = ServerSocialUnitMgr::Singleton().GetUnit(npcLord, enSUTplId_Tong);

			if( NULL != pUnit && pUnit->IsOwner(GetPlayerName(nPlayerIdx)) )
			{
				Lua_PushNumber(L, 1);
				return 1;
			}
			
		}
	}

	Lua_PushNumber(L, 0);
	return 1;
}

/*!
\brief
	打开城市对话框脚本接口	
*/
int LuaOpenCityManageDialog(Lua_State * L)
{
	const int nPlayerIndex = ScriptGetPlayerIndex();

	if (!IsValidPlayer(nPlayerIndex))
		return 0 ;

	const int nLordIdx = Lua_ValueToNumber(L, 1);	
	
	if (nLordIdx > 0 && nLordIdx < MAX_NPC)
	{
		
		ConfigManager &CMgr = ConfigManager::Singleton();
		
		S2C_SOCIAL_CITY_INFO	tagCityInfo;
		ZeroMemory( &tagCityInfo, sizeof(S2C_SOCIAL_CITY_INFO) );
		tagCityInfo.comHeader.proHeader.protocol = s2c_social_family;
		tagCityInfo.comHeader.proHeader.subProtocol = enSRProtocol_ReqCityInfo;
		tagCityInfo.comHeader.proHeader.len = sizeof(S2C_SOCIAL_CITY_INFO) - PROTOCOL_SIZE;
		
		SocialUnit* pPlayerLeafSU = ::GetLeafUnit( nPlayerIndex,enSUTplId_Tong );
		
		if ( pPlayerLeafSU )
		{
			SocialUnit* pPlayerTopSU = ::GetTopUnit( pPlayerLeafSU );
			
			if( pPlayerTopSU )
			{
				SocialUnitAttr& Attr = pPlayerTopSU->GetUnitAttr();
				
				if ( Attr.IsAttrHasData( enSUAttr_CityMap ) )
				{
					char* pData = NULL;
					int nSize  = Attr.GetAttr( enSUAttr_CityMap, pData );
					if (nSize == sizeof (int))
					{
						int nMapID = *((int*)pData);
						int nWorldIndex = g_SubWorldSet.SearchWorld( nMapID );
						if( nWorldIndex != -1 )
						{
							int nPlayerLordIndex = SubWorld[nWorldIndex].GetLord( );
							
							if( nPlayerLordIndex == nLordIdx )
							{
								if ( Attr.IsAttrHasData( enSUAttr_UnitName ) )
								{
									if (nPlayerIndex < 0)
										return 0;
									PLAYER_SCRIPTACTION_SYNC UiInfo;
									UiInfo.m_bOptionNum = 0;
									UiInfo.m_nBufferLen = sizeof(int);
									UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
									UiInfo.m_bUIId = UI_CITY_DLG;
									UiInfo.m_bParam2 = 1;
									Player[nPlayerIndex].DoScriptAction(&UiInfo);
									
									char *pData = NULL;
									int nSize = Attr.GetAttr( enSUAttr_UnitName, pData );
									memcpy( tagCityInfo.szOwnerName, pPlayerTopSU->GetOwnerName(), COMMON_CLIENT_MSG_LEN_16 + 1 );
									tagCityInfo.szOwnerName[COMMON_CLIENT_MSG_LEN_16] = 0;
									
									tagCityInfo.nCityMapId					= nMapID;
									tagCityInfo.nCityLordId					= Npc[nLordIdx].GetId();
									tagCityInfo.nCityProduceBuffTemplateID	= CMgr.GetGlobalVariable(global_var_buff_city_produce);
									tagCityInfo.nCityConsumeBuffTemplateID	= CMgr.GetGlobalVariable(global_var_buff_city_consume);
									tagCityInfo.nTaxRate					= Npc[nLordIdx].m_UnaryAttrMgr[nuai_city_taxrate];
									
									for ( int nType = 0; nType < CITY_RES_TYPE_COUNT; ++nType )
									{
										tagCityInfo.arrayCityRes[nType]	= Npc[nLordIdx].m_UnaryAttrMgr[nType + nuai_lord_res0];
									}
									
									tagCityInfo.nTongMemberCount           = pPlayerTopSU->GetTotalPlayerNum();
									tagCityInfo.nShizuCount                = pPlayerTopSU->GetChildCount();
									
									KEconomySysManager& esm = KEconomySysManager::Singleton();
									DWORD nRet;
									bool bSuccess = esm.GetAttrValue(nMapID, 0, 1, nRet);
									if (bSuccess)
									{
										tagCityInfo.nDevelopment = nRet;
									}

									bSuccess = esm.GetAttrValue(nMapID, 0, 2, nRet);
									if (bSuccess)
									{
										tagCityInfo.nTiredness = nRet;
									}
										
									SendDataToClient(nPlayerIndex, &tagCityInfo, tagCityInfo.comHeader.proHeader.len + PROTOCOL_SIZE);
									
								}	
							}
						}
					}
				}
			}
		}
	}

	return 0;
}


int LuaKillBaby(Lua_State *L)
{
	int nPlayerIdx = ScriptGetPlayerIndex();

	if(IsValidPlayer(nPlayerIdx))
	{
		Player[nPlayerIdx].m_Creature.Dismiss();
	}
	
	return 0;
}

#endif

#ifdef _SERVER

int LuaChgPKMode(Lua_State *L)
{
	int nPlayerIdx = ScriptGetPlayerIndex();
	int nMode = (int)Lua_ValueToNumber(L, 1);

	if(IsValidPlayer(nPlayerIdx))
		Npc[Player[nPlayerIdx].m_nIndex].ChangePKMode((PK_MODE)nMode);

	return 0;
}

#endif

//BitValue = GetBit(Value, BitNo)
int LuaGetBit(Lua_State * L)
{
	int nBitValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);
	
	if (nBitNumber >= 32 || nBitNumber <= 0) 
		goto lab_getbit;
	nBitValue = (nIntValue & (1 << (nBitNumber - 1))) != 0;
lab_getbit:
	Lua_PushNumber(L, nBitValue);
	return 1;
}

//NewBit = SetBit(Value, BitNo, BitValue)
int LuaSetBit(Lua_State * L)
{
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nBitNumber = (int)Lua_ValueToNumber(L, 2);
	int nBitValue = (int)Lua_ValueToNumber(L,3);

	if (nBitNumber > 32 || nBitNumber <= 0) 
	{
		return 0;
	}
	
	if(nBitValue >= 1)
	{
		nIntValue = (nIntValue | (1 << (nBitNumber - 1)));
	}
	else
	{
		nIntValue = (nIntValue & (~(1 << (nBitNumber - 1))));
	}
	Lua_PushNumber(L, nIntValue);
	return 1;
}

//ByteValue = GetByte(Value, ByteNo)
int LuaGetByte(Lua_State * L)
{
	int nByteValue = 0;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		goto lab_getByte;
	nByteValue = (nIntValue & (0xff << ((nByteNumber - 1) * 8) )) >> ((nByteNumber - 1) * 8);
	
lab_getByte:
	Lua_PushNumber(L, nByteValue);
	return 1;
}

//NewByte = SetByte(Value, ByteNo, ByteValue)
int LuaSetByte(Lua_State * L)
{
	BYTE * pByte =	NULL;
	int nIntValue = (int)Lua_ValueToNumber(L, 1);
	int nByteNumber = (int)Lua_ValueToNumber(L, 2);
	int nByteValue = (int)Lua_ValueToNumber(L,3);
	nByteValue = (nByteValue & 0xff);
	
	if (nByteNumber > 4 || nByteNumber <= 0) 
		goto lab_setByte;
	
	pByte = (BYTE*)&nIntValue;
	*(pByte + (nByteNumber -1)) = (BYTE)nByteValue;
lab_setByte:
	Lua_PushNumber(L, nIntValue);
	return 1;
}


//Idx = SubWorldID2Idx(dwID)
int LuaSubWorldIDToIndex(Lua_State * L)
{
	int nTargetSubWorld = -1;
	int nSubWorldID = 0;
	if (Lua_GetTopIndex(L) < 1)
		goto lab_subworldid2idx;
	
	nSubWorldID = (int)Lua_ValueToNumber(L, 1);
	nTargetSubWorld = g_SubWorldSet.SearchWorld(nSubWorldID);	
	
lab_subworldid2idx:
	Lua_PushNumber(L, nTargetSubWorld);
	return 1;
}

#define MAX_CONTENT_LEN 1500
#ifdef _SERVER
int LuaTaskNote(Lua_State * L)
{
	const int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;

	int nPlayerIndex = ScriptGetPlayerIndex();;
	if (IsValidPlayer(nPlayerIndex))
	{
		KPlayer& aPlayer = Player[nPlayerIndex];
		PLAYER_SCRIPTACTION_SYNC UiInfo;
		UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
		UiInfo.m_bUIId = UI_NOTEINFO;
		UiInfo.m_nParam =  Lua_ValueToNumber(L, 1); // task id
		UiInfo.m_bParam2 = 1;
		int uStep = Lua_ValueToNumber(L, 2);
		UiInfo.m_bOptionNum = uStep & 0xFF;
		UiInfo.m_bParam1 = uStep >> 8; // task step
		if (nParamNum > 2)
		{
			char * pInfo = UiInfo.m_pContent;
			for(unsigned int i = 3; i < nParamNum + 1; i++)
			{
				if (Lua_IsNumber(L, i))
				{
					if (pInfo - UiInfo.m_pContent + sizeof(int) < MAX_SCIRPTACTION_BUFFERNUM ) 
					{
						unsigned int uVal = Lua_ValueToNumber(L, i); 
						*(int*)pInfo = uVal;
						aPlayer.m_cTask.SetTaskNote(UiInfo.m_nParam, i - 2, uVal);
						pInfo += sizeof(int);
					}
				}
				else
				{
					_ASSERT(0);
				}
			}
			UiInfo.m_nBufferLen = (pInfo - UiInfo.m_pContent);
		}
		else
		{
			//没有参数
			UiInfo.m_pContent[0] = 0;
			UiInfo.m_nBufferLen = 0;
		}
		
		aPlayer.m_cTask.SetTaskNote(UiInfo.m_nParam, 0, uStep);
		aPlayer.DoScriptAction(&UiInfo);
	}		

	return 0;
}
#endif

int LuaSayTask(Lua_State* L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		if ((Lua_GetTopIndex(L) == 2)&&(Lua_IsTable(L,2)))
		{
			KPlayer& aPlayer = Player[nPlayerIndex];
			aPlayer.m_bWaitingPlayerFeedBack = false;
			char szContent[MAX_CONTENT_LEN];
			int nDataType = 0;
			if (Lua_IsNumber(L,1))
			{
				nDataType = 1;
				szContent[0] = 0;
			}
			else if (Lua_IsString(L,1)) 
			{
				nDataType = 0;
				strncpy(szContent, Lua_ValueToString(L, 1), MAX_CONTENT_LEN );
			}
			else
			{
				return 0;
			}


			Lua_PushString(L, "Size");
			Lua_GetTable(L, 2);
			const int count = Lua_ValueToNumber(L, -1);
            char icons[MAX_ANSWERNUM] = {0};
			Lua_Pop(L, 1);
			if (count <= MAX_ANSWERNUM) 
			{
				for (unsigned int i = 0; i < count; i++)
				{
					lua_rawgeti(L, 2, i + 1);
					if (Lua_IsTable(L, -1)) 
					{
    					lua_rawgeti(L, -1, 1);
						const char* str = Lua_ValueToString(L, -1);
						if ( str )
						{
							int strLen = strlen(str);
							int contentLen = strlen(szContent);
							if ( (contentLen + strLen + 1) < MAX_CONTENT_LEN )
							{
								strcat(szContent, "|");
								strcat(szContent, str);
								strcpy(aPlayer.m_szTaskAnswerFun[i], "menucallback");
							}
						}
    					Lua_Pop(L, 1);
       					lua_rawgeti(L, -1, 2);
                        icons[i] = Lua_ValueToNumber(L, -1);                         
    					Lua_Pop(L, 1);

					}
					Lua_Pop(L, 1);
				}
				
				aPlayer.m_nAvailableAnswerNum = count;

				const int nStrLen = strlen(szContent);
				if (nStrLen + count * sizeof(char) < MAX_SCIRPTACTION_BUFFERNUM - 4) 
				{
					PLAYER_SCRIPTACTION_SYNC UiInfo;
					UiInfo.m_bUIId = UI_SELECTDIALOG;
					UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
		#ifndef _SERVER
					UiInfo.m_bParam2 = 0;
		#else
					UiInfo.m_bParam2 = 1;
		#endif
					UiInfo.m_nParam = 1;
					UiInfo.m_bOptionNum = count;
					UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
					if(nDataType == 1)
					{
						*(int*)UiInfo.m_pContent = Lua_ValueToNumber(L, 1);
						memcpy(UiInfo.m_pContent+sizeof(int), szContent, nStrLen);
						UiInfo.m_pContent[nStrLen+sizeof(int)]=0;
						UiInfo.m_nBufferLen = nStrLen+sizeof(int)+1;
					}
					else
					{
						memcpy(UiInfo.m_pContent, szContent, nStrLen);
						UiInfo.m_pContent[nStrLen]=0;
						UiInfo.m_nBufferLen = nStrLen+1;
					}
                    memcpy(UiInfo.m_pContent + UiInfo.m_nBufferLen, icons, sizeof(char) * count);
                    UiInfo.m_nBufferLen += sizeof(char) * count; 
					aPlayer.m_nAvailableAnswerNum = count;
					aPlayer.m_bWaitingPlayerFeedBack = true;				
					aPlayer.DoScriptAction(&UiInfo);			
				}
			}
		}		
	}
	return 0;
}

int LuaMovieScene(Lua_State* L)
{
	//初始检查
	int playerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(playerIndex))
	{
		return 0;
	}

	KPlayer& player = Player[playerIndex];
	player.m_bWaitingPlayerFeedBack = false;

	if(Lua_GetTopIndex(L) != 3)
	{
		return 0;
	}

	if(!Lua_IsString(L, 1)) 
	{
		return 0;
	}

	if(!Lua_IsNumber(L, 2))
	{
		return 0;
	}

	if(!Lua_IsTable(L, 3))
	{
		return 0;
	}

	//初始化数据
	//主消息内容
	char content[1500];
	strcpy(content, Lua_ValueToString(L, 1));
	strcat(content, "|");
	int imageId = Lua_ValueToNumber(L, 2);
	
	//以下是交互按钮
	//得到交互按钮表的大小
	Lua_PushString(L, "Size");
	Lua_GetTable(L, 3);
	const int count = Lua_ValueToNumber(L, -1);
	if(count >= MAX_ANSWERNUM)
	{
		return 0;
	}
	Lua_Pop(L, 1);

	//得到按钮表的具体内容，并把其写入缓存中
	for(int i = 0; i < count; i++)
	{
		lua_rawgeti(L, 3, i + 1);
		if(Lua_IsTable(L, -1)) 
		{
			lua_rawgeti(L, -1, 1);
			const char* str = Lua_ValueToString(L, -1);
			strcat(content, str);
			strcat(content, "|");
			strcpy(player.m_szTaskAnswerFun[i], "menucallback");
			Lua_Pop(L, 1);
		}
		Lua_Pop(L, 1);
	}
	
	player.m_nAvailableAnswerNum = count;
	
	const int bufferLen = strlen(content);

	if(bufferLen >= MAX_SCIRPTACTION_BUFFERNUM - 4)
	{
		return 0;
	}
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_MOVIE_SCENE;
	UiInfo.m_bOptionNum = count;

#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif

	UiInfo.m_nParam = imageId;			//电影场景
	
	memcpy(UiInfo.m_pContent, content, bufferLen);
	UiInfo.m_pContent[bufferLen] = 0;
	UiInfo.m_nBufferLen = bufferLen + 1;

	player.m_nAvailableAnswerNum = count;
	player.m_bWaitingPlayerFeedBack = true;
	player.DoScriptAction(&UiInfo);

	return 0;
}

int LuaOpenWindow(Lua_State* L)
{
	//初始检查
	int playerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(playerIndex))
	{
		return 0;
	}

	if(Lua_GetTopIndex(L) != 1)
	{
		return 0;
	}

	if(!Lua_IsNumber(L, 1)) 
	{
		return 0;
	}
	int windowId = Lua_ValueToNumber(L, 1);

	//初始化数据	
	PLAYER_SCRIPTACTION_SYNC UiInfo = {0};;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_OPEN_ANY_WINDOW;
	UiInfo.m_bParam1 = windowId;
	UiInfo.m_bParam2 = 1;

	KPlayer& player = Player[playerIndex];
	player.DoScriptAction(&UiInfo);

	return 0;
}

int LuaOpenTimer(Lua_State* L)
{
	//初始检查
	int playerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(playerIndex))
	{
		return 0;
	}

	if(Lua_GetTopIndex(L) != 3)
	{
		return 0;
	}

	if(!Lua_IsNumber(L, 1))
	{
		return 0;
	}

	if(!Lua_IsNumber(L, 2))
	{
		return 0;
	}

	if(!Lua_IsString(L, 3))
	{
		return 0;
	}

	int time = Lua_ValueToNumber(L, 1);
	int type = Lua_ValueToNumber(L, 2);

	//初始化数据	
	PLAYER_SCRIPTACTION_SYNC UiInfo = {0};
	memset(UiInfo.m_pContent, 0, MAX_SCIRPTACTION_BUFFERNUM);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_OPEN_TIMER;
	UiInfo.m_bParam2 = 1;
	
	int strLen = strlen(Lua_ValueToString(L, 3));
	if(strLen + 30 >= MAX_SCIRPTACTION_BUFFERNUM)
	{
		return 0;
	}

	sprintf(UiInfo.m_pContent, "%d|%d|%s", time, type, Lua_ValueToString(L, 3));
	UiInfo.m_nBufferLen = strlen(UiInfo.m_pContent);

	KPlayer& player = Player[playerIndex];
	player.DoScriptAction(&UiInfo);

	return 0;
}

int luaOpenTongCentre(Lua_State * L)
{
	//初始检查
	int playerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(playerIndex))
	{
		return 0;
	}//endif
	
	//初始化数据	
	PLAYER_SCRIPTACTION_SYNC UiInfo = {0};;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_OPEN_TONG_RECRUIT;
	UiInfo.m_bParam2 = 1;
	
	KPlayer& player = Player[playerIndex];
	player.DoScriptAction(&UiInfo);

	return 0 ;
}

//////////////////////////////////////////////////////////////////////////
// 以下的函数(get_field_*)并未精心设计已适应广泛的应用，它只是用来降低ShowQuest的复杂度
// int get_field_int(Lua_State* L, const char *key) 
// {
// 	lua_pushstring(L, key);
// 	lua_gettable(L, -2);
// 	int result = (int)lua_tonumber(L, -1);
// 	lua_pop(L, 1);
// 	return result;
// }
// 
// const char* get_field_string(Lua_State* L, const char *key) 
// {
// 	lua_pushstring(L, key);
// 	lua_gettable(L, -2);
// 	const char* result = (const char*)lua_tostring(L, -1);
// 	lua_pop(L, 1);
// 	return result;
// }
// 
// int LuaShowQuest(Lua_State* L)
// {
// 	int nPlayerIndex = ScriptGetPlayerIndex();
// 	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
// 	{
// 		if ((Lua_GetTopIndex(L) == 3)&&(Lua_IsTable(L,3)))
// 		{
// 			KPlayer& aPlayer = Player[nPlayerIndex];
// 			aPlayer.m_bWaitingPlayerFeedBack = false;
// 			char szContent[MAX_CONTENT_LEN];
// 			int nDataType = 0;
// 			if (Lua_IsNumber(L,1))
// 			{
// 				nDataType = 1;
// 				szContent[0] = 0;
// 			}
// 			else if (Lua_IsString(L,1)) 
// 			{
// 				nDataType = 0;
// 				strncpy(szContent, Lua_ValueToString(L, 1), MAX_CONTENT_LEN);
// 			}
// 			else
// 			{
// 				return 0;
// 			}
// 
// 			unsigned int uQuestID = Lua_ValueToNumber(L, 2);
// 			
// 
// 			Lua_PushString(L, "Size");
// 			Lua_GetTable(L, 3);
// 			const int count = Lua_ValueToNumber(L, -1);
// 			Lua_Pop(L, 1);
// 			if (count < MAX_ANSWERNUM) 
// 			{
// 				for (unsigned int i = 0; i < count; i++)
// 				{
// 					lua_rawgeti(L, 3, i + 1);
// 					if (Lua_IsTable(L, -1)) 
// 					{
// 						//const char* str = Lua_ValueToString(L, -1);
// 						//strcat(szContent, "|");
// 						//strcat(szContent, str);
// 						strcpy(aPlayer.m_szTaskAnswerFun[i], "menucallback");
// 					}
// 					Lua_Pop(L, 1);
// 				}
// 				
// 				aPlayer.m_nAvailableAnswerNum = count;
// 
// 				const int nStrLen = strlen(szContent);
// 				if (nStrLen < MAX_SCIRPTACTION_BUFFERNUM - 4) 
// 				{
// 					PLAYER_SCRIPTACTION_SYNC UiInfo;
// 					UiInfo.m_bUIId = UI_SHOW_QUEST;
// 					UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
// 		#ifndef _SERVER
// 					UiInfo.m_bParam2 = 0;
// 		#else
// 					UiInfo.m_bParam2 = 1;
// 		#endif
// 					UiInfo.m_nParam = uQuestID;
// 					UiInfo.m_bOptionNum = count;
// 					UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
// 					if(nDataType == 1)
// 					{
// 						*(int*)UiInfo.m_pContent = Lua_ValueToNumber(L, 1);
// 						memcpy(UiInfo.m_pContent+sizeof(int), szContent, nStrLen);
// 						UiInfo.m_pContent[nStrLen+sizeof(int)+1]=0;
// 						UiInfo.m_nBufferLen = nStrLen+sizeof(int);
// 					}
// 					else
// 					{
// 						memcpy(UiInfo.m_pContent, szContent, nStrLen);
// 						UiInfo.m_pContent[nStrLen+1]=0;
// 						UiInfo.m_nBufferLen = nStrLen;
// 					}
// 					aPlayer.m_nAvailableAnswerNum = count;
// 					aPlayer.m_bWaitingPlayerFeedBack = true;				
// 					aPlayer.DoScriptAction(&UiInfo);			
// 				}
// 			}
// 		}		
// 	}
// 	return 0;
// }
// 
// int LuaAcceptQuest(Lua_State* L)
// {
// 	int nPlayerIndex = ScriptGetPlayerIndex();
// 	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
// 	{
// 		if ((Lua_GetTopIndex(L) == 2)&&(Lua_IsTable(L,2)))
// 		{
// 			KPlayer& aPlayer = Player[nPlayerIndex];
// 			aPlayer.m_bWaitingPlayerFeedBack = false;
// 			char szContent[MAX_CONTENT_LEN];
// 			szContent[0] = 0;
// 			Lua_PushString(L, "Size");
// 			Lua_GetTable(L, 2);
// 			const int count = Lua_ValueToNumber(L, -1);
// 			Lua_Pop(L, 1);
// 			if (count < MAX_ANSWERNUM) 
// 			{
// 				for (unsigned int i = 0; i < count; i++)
// 				{
// 					lua_rawgeti(L, 2, i + 1);
// 					if (Lua_IsTable(L, -1)) 
// 					{
// 						//const char* str = Lua_ValueToString(L, -1);
// 						//strcat(szContent, "|");
// 						//strcat(szContent, str);
// 						strcpy(aPlayer.m_szTaskAnswerFun[i], "menucallback");
// 					}
// 					Lua_Pop(L, 1);
// 				}
// 				
// 				aPlayer.m_nAvailableAnswerNum = count;
// 
// 				const int nStrLen = strlen(szContent);
// 				if (nStrLen < MAX_SCIRPTACTION_BUFFERNUM - 4) 
// 				{
// 					PLAYER_SCRIPTACTION_SYNC UiInfo;
// 					UiInfo.m_bUIId = UI_ACCEPT_QUEST;
// 					UiInfo.m_bParam1 = 1;//主信息的类型，字符串(0)或数字(1)
// 		#ifndef _SERVER
// 					UiInfo.m_bParam2 = 0;
// 		#else
// 					UiInfo.m_bParam2 = 1;
// 		#endif
// 					UiInfo.m_nParam = Lua_ValueToNumber(L, 1);
// 					UiInfo.m_bOptionNum = count;
// 					UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
// //					*(int*)UiInfo.m_pContent = Lua_ValueToNumber(L, 1);
// 					memcpy(UiInfo.m_pContent, szContent, nStrLen);
// 					UiInfo.m_pContent[nStrLen+1]=0;
// 					UiInfo.m_nBufferLen = nStrLen;
// 					aPlayer.m_nAvailableAnswerNum = count;
// 					aPlayer.m_bWaitingPlayerFeedBack = true;				
// 					aPlayer.DoScriptAction(&UiInfo);			
// 				}
// 			}
// 		}		
// 	}
// 	return 0;
// }

// #ifdef _SERVER
// int LuaSyncQuest(Lua_State* L)
// {
// 	int nPlayerIndex = ScriptGetPlayerIndex();
// 	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
// 	{
// 		if ((Lua_GetTopIndex(L) == 2)&&(Lua_IsTable(L, 1)))
// 		{
// 			int option = Lua_ValueToNumber(L, 2);
// 			Lua_Pop(L, 1);
// 			
// 			int nLen;
// 			char szContent[MAX_CONTENT_LEN];
// 			char* pWritePos = &szContent[0];
// 			int nDataType = 0;
// 
// 			if (option == 0) 
// 			{
// 				{
// 					*(int*)pWritePos = get_field_int(L, "ID");
// 					pWritePos += 4;
// 				}
// 				{		
// 					const char* strTitle = get_field_string(L, "Title");
// 					nLen = g_StrLen(strTitle);
// 					*(int*)pWritePos = nLen;
// 					pWritePos += 4;
// 					memcpy(pWritePos, strTitle, nLen);
// 					pWritePos += nLen;
// 				}
// 			}
// 			else
// 			{
// 				
// 				{
// 					*(int*)pWritePos = get_field_int(L, "ID");
// 					pWritePos += 4;
// 				}
// 				{		
// 					const char* strTitle = get_field_string(L, "Title");
// 					nLen = g_StrLen(strTitle);
// 					*(int*)pWritePos = nLen;
// 					pWritePos += 4;
// 					memcpy(pWritePos, strTitle, nLen);
// 					pWritePos += nLen;
// 				}
// 				{		
// 					const char* strTitle = get_field_string(L, "CategoryName");
// 					nLen = g_StrLen(strTitle);
// 					*(int*)pWritePos = nLen;
// 					pWritePos += 4;
// 					memcpy(pWritePos, strTitle, nLen);
// 					pWritePos += nLen;
// 				}
// 				{		
// 					const char* strDetail = get_field_string(L, "Details");
// 					nLen = g_StrLen(strDetail);
// 					*(int*)pWritePos = nLen;
// 					pWritePos += 4;
// 					memcpy(pWritePos, strDetail, nLen);
// 					pWritePos += nLen;
// 				}
// 				{
// 					
// 					const char* strObjective = get_field_string(L, "ObjectiveInfo");
// 					nLen = g_StrLen(strObjective);
// 					*(int*)pWritePos = nLen;
// 					pWritePos += 4;
// 					memcpy(pWritePos, strObjective, nLen);
// 					pWritePos += nLen;
// 				}
// 
// 				{
// 					lua_pushstring(L, "Objectives");
// 					lua_gettable(L, -2);
// 					
// 					for (unsigned int i = speak_to ; i < max_objective_type; i++)
// 					{
// 						lua_pushnumber(L, i + 1);
// 						lua_gettable(L, -2);
// 						int nCount = get_field_int(L, "Count");
// 						*(int*)pWritePos = nCount;
// 						pWritePos += 4;
// 						if (nCount > 0)
// 						{
// 							for (unsigned int c = 1; c <= nCount; c++)
// 							{
// 								lua_pushnumber(L, c);
// 								lua_gettable(L, -2);
// 
// 								lua_pushnumber(L, 1);
// 								lua_gettable(L, -2);
// 								*(int*)pWritePos = lua_tonumber(L, -1);
// 								pWritePos += 4;
// 								lua_pop(L, 1);
// 
// 								lua_pushnumber(L, 2);
// 								lua_gettable(L, -2);
// 								*(int*)pWritePos = lua_tonumber(L, -1);
// 								pWritePos += 4;
// 								lua_pop(L, 1);
// 								
// 								lua_pop(L, 1);
// 							}
// 						}
// 						lua_pop(L, 1);
// 					}
// 					
// 					lua_pop(L, 1);
// 				}
// 
// 				{
// 					lua_pushstring(L, "EndQuests");
// 					lua_gettable(L, -2);
// 					int nCount = get_field_int(L, "Count");
// 					if (nCount == 1) // 只有一个结局的任务才会提示奖励
// 					{
// 						lua_pushnumber(L, 1);
// 						lua_gettable(L, -2);
// 						
// 						*(int*)pWritePos = get_field_int(L, "RewardXP");
// 						pWritePos += 4;
// 						*(int*)pWritePos = get_field_int(L, "RewardMoney");
// 						pWritePos += 4;
// 						
// 						lua_pushstring(L, "RewardItem");
// 						lua_gettable(L, -2);
// 						nCount = get_field_int(L, "Count");
// 						*(int*)pWritePos = nCount;
// 						pWritePos += 4;
// 						for (int i = 1; i <= nCount; i ++)
// 						{
// 							lua_pushnumber(L, i);
// 							lua_gettable(L, -2);
// 							*(int*)pWritePos = task_item;
// 							pWritePos += 4;
// 							*(int*)pWritePos = get_field_int(L, "Count");
// 							pWritePos += 4;
// 							*(int*)pWritePos = get_field_int(L, "ItemID");
// 							pWritePos += 4;
// 							lua_pop(L, 1);
// 						}
// 						lua_pop(L, 1);
// 						
// 						lua_pushstring(L, "RewardChoiceItem");
// 						lua_gettable(L, -2);
// 						nCount = get_field_int(L, "Count");
// 						*(int*)pWritePos = nCount;
// 						pWritePos += 4;
// 						for (int n = 1; n <= nCount; n ++)
// 						{
// 							lua_pushnumber(L, n);
// 							lua_gettable(L, -2);
// 							*(int*)pWritePos = task_item;
// 							pWritePos += 4;
// 							*(int*)pWritePos = get_field_int(L, "Count");
// 							pWritePos += 4;
// 							*(int*)pWritePos = get_field_int(L, "ItemID");
// 							pWritePos += 4;
// 							lua_pop(L, 1);
// 						}
// 						lua_pop(L, 1);
// 						
// 						lua_pop(L, 1);
// 					}
// 					else
// 					{
// 						*(int*)pWritePos = 0;
// 						pWritePos += 4;
// 						*(int*)pWritePos = 0;
// 						pWritePos += 4;
// 						*(int*)pWritePos = 0;
// 						pWritePos += 4;
// 						*(int*)pWritePos = 0;
// 						pWritePos += 4;
// 					}
// 					lua_pop(L, 1);
// 				}
// 			}
// 			
// 			nLen = (pWritePos - szContent);
// 			_SYNC_QUEST_DETAIL SYN;
// 			SYN.Protocol = s2c_quest_family;
// 			SYN.wProtocolSize = sizeof(BYTE_EXTEND_HEADER) - 1 + nLen;
// 			if (option == 0)
// 			{
// 				SYN.ProtocolExtend  = s2c_sync_quest_title;
// 			}
// 			else if(option == 1)
// 			{
// 				SYN.ProtocolExtend  = s2c_sync_quest_detail_log;
// 			}
// 			else if(option == 2)
// 			{
// 				SYN.ProtocolExtend  = s2c_sync_quest_detail_log_and_show;
// 			}
//             else
//             {
// 				SYN.ProtocolExtend  = s2c_sync_quest_detail_cache;
//             }
// 			memcpy(SYN.pContent, szContent, nLen);
// 			if (g_pServer != NULL)
// 				g_pServer->PackDataToClient(Player[nPlayerIndex].GetNetConnectIdx(), &SYN, SYN.wProtocolSize + 1);
// 		}		
// 	}
// 	return 0;
// }
// #endif

// CloseDialog
// lixuewu 2004.10.08
int LuaCloseDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		KPlayer& aPlayer = Player[nPlayerIndex];
		aPlayer.m_bWaitingPlayerFeedBack = false;
		PLAYER_SCRIPTACTION_SYNC UiInfo = {0};
		UiInfo.m_bUIId = UI_CLOSE_DIALOG;
		UiInfo.m_bParam2 = 1;
		UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
		aPlayer.DoScriptAction(&UiInfo);	
	}
	return 0;
}


//**************************************************************************************************************************************************************
//												界面脚本
//**************************************************************************************************************************************************************
//extern "C"

// Rocker 2004.4.28
#ifdef _SERVER
int LuaGetPlayerIndexByName(Lua_State * L)
{
	int playerIndex = INVALID_PLAYER_INDEX;
	if (Lua_GetTopIndex(L) == 1)
	{
		char* name      = (char*)Lua_ValueToString(L,1);
		
		if (name)
		{
			playerIndex = g_PlayerInfoToIndex.GetIndexByName(name);
		}//endif

	}//endif

	Lua_PushNumber(L, playerIndex);
	return 1;
}
#endif
// Rocker 2004.4.28

// int LuaAddGlobalNews(Lua_State * L)
// {
// 	return 0;
// }
// 
// 
// int LuaAddGlobalCountNews(Lua_State * L)
// {
// 	return 0;
// }
// 
// int LuaAddGlobalTimeNews(Lua_State * L)
// {
// 	return 0;
// }


/*!
\brief
	打开合成对话框脚本借口	
*/
int LuaOpenEnchaserItemDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_nBufferLen = sizeof(int);
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	int nCompoundId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nCompoundId = (int)Lua_ValueToNumber(L,1);

		switch( nCompoundId )
		{
		case COMPOUND_LEVELUP:
			UiInfo.m_bUIId = UI_ITEM_UPDATE;
			break;
		case COMPOUND_ADDMAGIC:
			UiInfo.m_bUIId = UI_ITEM_ADDMAGIC;
			break;
		case COMPOUND_ADDYAO:
			UiInfo.m_bUIId = UI_ITEM_SETYAO;
			break;
		case COMPOUND_GETYAO:
			UiInfo.m_bUIId = UI_ITEM_GETYAO;
			break;
		case COMPOUND_MAKE:
			UiInfo.m_bUIId = UI_ITEM_MAKE;
			break;
		}
	}	
	UiInfo.m_bParam2 = 1;
#endif
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*!
\brief
	打开创建对话框脚本借口	
*/
int LuaOpenCreateTongDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_nBufferLen = sizeof(int);
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	int nCreateId = 0;
	
	if (Lua_IsNumber(L,1))
	{
		nCreateId = (int)Lua_ValueToNumber(L,1);

		switch( nCreateId )
		{
		case 0:
			UiInfo.m_bUIId = UI_CREATE_SHIZU;
			break;
		case 1:
			UiInfo.m_bUIId = UI_CREATE_ZHUHOU;
			break;
		case 2:
			UiInfo.m_bUIId = UI_CREATE_LIANMENG;
			break;
		default:return 0 ;
		}//end for switch
	}	
	UiInfo.m_bParam2 = 1;
#endif
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*!
\brief
	打开拍卖框脚本接口
*/
int LuaOpenAuctionDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_AUCTION_DLG;
	UiInfo.m_bParam2 = 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);

#ifdef _SERVER
	Player[nPlayerIndex].GetUIServerState().SetUIState(player_ui_auction,player_ui_state_open);
#endif

	return 0;
}

/*!
\brief
	打开副本奖励脚本接口
*/
int	LuaOpenInstanceRewardDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	
	if(Lua_GetTopIndex(L) != 6)
	{
		return 0;
	}

	if( !Lua_IsNumber(L, 1) || 
		!Lua_IsNumber(L, 2) ||
		!Lua_IsNumber(L, 3) ||
		!Lua_IsNumber(L, 4) ||
		!Lua_IsNumber(L, 5) ||
		!Lua_IsNumber(L, 6) )
	{
		return 0;
	}

	int useTime		= Lua_ValueToNumber(L, 1);
	int killNum		= Lua_ValueToNumber(L, 2);
	int	rewardExp	= Lua_ValueToNumber(L, 3);
	int skillExp	= Lua_ValueToNumber(L, 4);
	int rewardId	= Lua_ValueToNumber(L, 5);
	int	suc			= Lua_ValueToNumber(L, 6);

	//初始化数据	
	PLAYER_SCRIPTACTION_SYNC UiInfo = {0};
	memset(UiInfo.m_pContent, 0, MAX_SCIRPTACTION_BUFFERNUM);
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_OPEN_INSTANCE_REWARD_WND;
	UiInfo.m_bParam2 = 1;

	sprintf(UiInfo.m_pContent, "%d|%d|%d|%d|%d|%d", useTime, killNum, rewardExp, skillExp, rewardId, suc);
	UiInfo.m_nBufferLen = strlen(UiInfo.m_pContent);

	KPlayer& player = Player[nPlayerIndex];
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	
	return 0;
}

/*!
\brief
	打开信用商店脚本接口
*/
int	LuaOpenCreditShop(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_OPEN_CREDIT_SHOP_WND;
	UiInfo.m_bParam2 = 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*!
\brief 
    打开 太岁之轮 界面
*/

int LuaOpenTaisuiDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_TAISUI_DLG;
	UiInfo.m_bParam2 = 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

#ifdef _SERVER

int luaAddInsuranceValue(Lua_State * L) //Just for test!
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	if (Lua_GetTopIndex(L) != 1)
		return 0;
	
	int nValue = Lua_ValueToNumber(L,1);

	C2S_INSURANCE_PROTOCOL procotol;
	procotol.nProtocol    = c2s_insurance;
	procotol.nSubProtocol = c2s_add_insurance_value;
	procotol.nLen         = sizeof(procotol) - PROTOCOL_SIZE;
	procotol.nParam       = nValue;

	Player[nPlayerIndex].m_InsuranceMgr.ProcessProcotol(&procotol,sizeof(procotol));
	
	return 0;
}

int luaFetchInsuranceMoney(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nValue = Lua_ValueToNumber(L,1);
	
	C2S_INSURANCE_PROTOCOL procotol;
	procotol.nProtocol    = c2s_insurance;
	procotol.nSubProtocol = c2s_get_insurance_money;
	procotol.nLen         = sizeof(procotol) - PROTOCOL_SIZE;
	procotol.nParam       = nValue;

	Player[nPlayerIndex].m_InsuranceMgr.ProcessProcotol(&procotol,sizeof(procotol));
	
	return 0;
}

int luaCreatePassword( Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (Lua_GetTopIndex(L) ==2  &&  IsValidPlayer(nPlayerIndex))
	{
		const char * szOld = Lua_ValueToString(L,1);
		const char * szNew = Lua_ValueToString(L,2);

		if (szOld && szNew)
		{
			Player[nPlayerIndex].SetBoxPassword(szNew,szOld);
		}//endif

	}//endif
	return 0;
}

int luaCloseStoreBox( Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if ( IsValidPlayer(nPlayerIndex))
	{
		Player[nPlayerIndex].lockStoreBox();
	}//endif

	return 0;
}

int luaWorldCombatDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	
	if (Lua_GetTopIndex(L) == 1)
	{
		int nParam = Lua_ValueToNumber(L,1);
		PLAYER_SCRIPTACTION_SYNC UiInfo;
		UiInfo.m_bOptionNum = 0;
		UiInfo.m_nBufferLen = sizeof(int);
		UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
		UiInfo.m_bUIId        = UI_WORLD_COMBAT_SCORE;
		UiInfo.m_bParam2 = 1;
		UiInfo.m_nParam  = nParam;
		
		Player[nPlayerIndex].DoScriptAction(&UiInfo);
		
		int nNpcIndex = Player[nPlayerIndex].m_nIndex;
		
		if (IsValidNpc(nNpcIndex) && Npc[nNpcIndex].IsInWorldCombatInstance() && IsValidCombatID(Npc[nNpcIndex].m_WorldCombatOrg))
		{
			int nSubWorldIndex = Npc[nNpcIndex].GetSubWorldIndex();
			if (nSubWorldIndex != INVALID_WORLD_INDEX && nSubWorldIndex < MAX_SUBWORLD)
			{
				WORLD_COMBAT_INFO         info;
				memset(&info,0,sizeof(info));

				info.Protocol          =  s2c_world_combat_info;
				
				for (int n = 0 ;n < MAX_SCORE_ORG_SYNC && n< MAX_COMBAT_ORG_NUM ; n++)
				{
					const CombatOrgnize * pInfo  =  SubWorld[nSubWorldIndex].GetCombatInstanceOrgInfo(n + 1);
					
					if (pInfo)
					{
						info.nScore[n]            =  pInfo->nScore;
					}//end for n
					
				}//endif
				
				if (g_pServer != NULL)
					g_pServer->PackDataToClient(Player[nPlayerIndex].GetNetConnectIdx(), (BYTE*)&info, sizeof(WORLD_COMBAT_INFO));
				
			}//endif
			
			
		}//endif
		
	}//endif
	
	return 0;
}


int luaSetFuryExp(Lua_State * L)
{
	int nValue       = (int)Lua_ValueToNumber(L, 1);
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		Player[nPlayerIndex].GetFurySys().SetCurFuryExp(nValue);
	}//endif

	return 0;
}

int luaGetFuryExp(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	int nRes         = 0;

	if (IsValidPlayer(nPlayerIndex))
	{
		nRes = Player[nPlayerIndex].GetFurySys().GetFuryExp();
	}//endif
	
	return 1;
}
#endif

/*!
\brief 
    打开 功能面板
*/
int LuaOpenNavigationWnd(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_NAVIGATION_WND;
	UiInfo.m_bParam2 = 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*!
\brief 
    打开 扩展功能面板
*/
int LuaOpenNavigationExWnd(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_NAVIGATIONEX_WND;
	UiInfo.m_bParam2 = 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*!
\brief 
    激活功能面板按键
*/
int LuaActiveNavigationButton(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex ))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_ACTIVE_NAVIGATION_BUTTON;
	UiInfo.m_bParam2 = 1;
	
	UiInfo.m_bParam1 = (unsigned char)Lua_ValueToNumber(L, 1);
/*
	char* pContent = NULL;
	pContent = UiInfo.m_pContent;
	pContent[0] = 0;
	size_t nContentLen = 0;

	const char * pString = NULL;
	pString = Lua_ValueToString(L, 1);
	nContentLen = strlen(pString);
	sprintf(pContent, "%s", pString);
//*/
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*!
\brief 
    打开 快捷栏面板
*/
int LuaOpenShortCutWnd(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_SHORTCUT_WND;
	UiInfo.m_bParam2 = 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*!
\brief 
    打开 扩展快捷栏面板
*/
int LuaOpenShortCutPlusWnd(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_SHORTCUTPLUS_WND;
	UiInfo.m_bParam2 = 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*!
\brief
	打开合成对话框脚本借口	
*/
int LuaOpenSkillManageDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_SKILL_STUDY_DLG;
	UiInfo.m_bParam2 = 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}

/*!
\brief
	打开合成对话框脚本借口	
*/
int LuaOpenMailDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nBufferLen = sizeof(int);
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId = UI_MAIL_CENTRE;
	UiInfo.m_bParam2 = 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
#ifdef _SERVER
	Player[nPlayerIndex].GetUIServerState().SetUIState(player_ui_mail,player_ui_state_open);
#endif
	return 0;
}



//--> Rocker 2005/07/12
int LuaTopMessage(Lua_State* L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex)) 
		return 0;

	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char * pContent = NULL;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 1) 
		return 0;
	
	if  (Lua_IsNumber(L,1))
	{
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 1)) 
	{
		nDataType = 0 ;
	}
	else
		return 0;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_TOP_INFO;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
	UiInfo.m_bOptionNum = 0;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	pContent = UiInfo.m_pContent;
	pContent[0] = 0;
	size_t nContentLen = 0;

	const char * pString = NULL;
	if (!nDataType)
	{
		pString = Lua_ValueToString(L, 1);
		nContentLen = strlen(pString);
		sprintf(pContent, "%s", pString);
	}
	else
	{
		int j = (int)Lua_ValueToNumber(L, 1);
		sprintf(pContent, "%d", j);
	}
	UiInfo.m_nBufferLen  = strlen(pContent);
	UiInfo.m_nParam = 0;
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif

	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}
//<-- End


//**************************************************************************************************************************************************************
//												任务脚本
//**************************************************************************************************************************************************************

#ifdef _SERVER
int LuaGetTaskValue(Lua_State * L)
{
	
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex)) 
	{
		unsigned int nValue =  Player[nPlayerIndex].m_cTask[(unsigned int)Lua_ValueToNumber(L,1)];		
		Lua_PushNumber(L, nValue);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}
#endif

#ifdef _SERVER
int LuaSetTaskValue(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();

	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	unsigned int nValueIndex = (unsigned int)Lua_ValueToNumber(L, 1);
	unsigned int nValue = (unsigned int)Lua_ValueToNumber(L, 2);
	
	if (nPlayerIndex <= 0) return 0;
	Player[nPlayerIndex].m_cTask[nValueIndex] = nValue;
	return 0;
}

int LuaSynQuestBible(Lua_State * L ) 
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex )) return 0;

	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 4) return 0;

	SPECIAL_QUEST_DATA data;
	data.Protocol = s2c_special_quest_data;
	
	data.QuestId = (unsigned int)Lua_ValueToNumber(L, 1);;
	data.Data.count = (unsigned int)Lua_ValueToNumber(L, 2);
	data.Data.doubleExpTag = (unsigned int)Lua_ValueToNumber(L, 3);
	data.Data.color = (unsigned int)Lua_ValueToNumber(L, 4);

	//发送给客户端
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, &data, sizeof(data));
	
	return 0;
}

// int LuaSyncQuestState(Lua_State * L)
// {
// 	int nPlayerIndex = ScriptGetPlayerIndex();
// 	if (nPlayerIndex <= 0) return 0;
// 	unsigned int uQuestID = (unsigned int)Lua_ValueToNumber(L, 1);
// 	unsigned int nValue = (unsigned int)Lua_ValueToNumber(L, 2);
// 	_SYNC_QUEST_STATE SYN;
//     SYN.Protocol = s2c_quest_family;
//     SYN.wProtocolSize = sizeof(SYN) - 1;
//     SYN.ProtocolExtend = s2c_sync_quest_state;
//     SYN.dwQuestID = uQuestID;
//     SYN.dwState = nValue;
//     if (g_pServer != NULL)
//         g_pServer->PackDataToClient(Player[nPlayerIndex].GetNetConnectIdx(), &SYN, sizeof(SYN));
// 	return 0;
// }
// 
// int LuaSetQuestProcess(Lua_State * L)
// {
// 	int nPlayerIndex = ScriptGetPlayerIndex();
// 	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
// 	{
// 		unsigned int uIndex = (unsigned int)Lua_ValueToNumber(L, 1) - 1;
// 		unsigned int uKind = (unsigned int)Lua_ValueToNumber(L, 2) - 1;
// 		unsigned int uObjective = (unsigned int)Lua_ValueToNumber(L, 3) - 1;
// 		unsigned int uQuestID = (unsigned int)Lua_ValueToNumber(L, 4);
// 		unsigned int uValue = (unsigned int)Lua_ValueToNumber(L, 5);
// 		Player[nPlayerIndex].m_cTask.SetTaskProcessVal(uIndex, uKind, uObjective, uQuestID, uValue);
// 	}
// 	return 0;
// }
// 
// int LuaGetQuestProcess(Lua_State * L)
// {
// 	int nPlayerIndex = ScriptGetPlayerIndex();
// 	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
// 	{
// 		unsigned int uIndex = (unsigned int)Lua_ValueToNumber(L, 1) - 1;
// 		unsigned int uKind = (unsigned int)Lua_ValueToNumber(L, 2) - 1;
// 		unsigned int uObjective = (unsigned int)Lua_ValueToNumber(L, 3) - 1;
// 		unsigned int uValue = Player[nPlayerIndex].m_cTask.GetTaskProcessVal(uIndex, uKind, uObjective);
// 		Lua_PushNumber(L, uValue);
// 		return 1;
// 	}
// 	return 0;
// }
// 
// int LuaGetQuestIDByQuestLogIDX(Lua_State * L)
// {
// 	int nPlayerIndex = ScriptGetPlayerIndex();
// 	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
// 	{
// 		unsigned int uIndex = (unsigned int)Lua_ValueToNumber(L, 1) - 1;
// 		unsigned int uValue = Player[nPlayerIndex].m_cTask.GetQuestIDByQuestLogIDX(uIndex);
// 		Lua_PushNumber(L, uValue);
// 		return 1;
// 	}
// 	return 0;
// }
// 
// int LuaFindQuestByQuestID(Lua_State * L)
// {
// 	int nPlayerIndex = ScriptGetPlayerIndex();
// 	if (nPlayerIndex > 0 && nPlayerIndex < MAX_PLAYER)
// 	{
// 		unsigned int uIndex = (unsigned int)Lua_ValueToNumber(L, 1);
// 		unsigned int uValue = Player[nPlayerIndex].m_cTask.FindQuestByQuestID(uIndex);
// 		Lua_PushNumber(L, uValue);
// 		return 1;
// 	}
// 	return 0;
// }
// 
#endif


#ifdef _SERVER
//---------------------------------交易、买卖、打开储藏箱-----------------------
//Sale(id)
//------------------------------------------------------------------------------
int LuaSale(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		int nShopId = (int)Lua_ValueToNumber(L,1);
		//----在以下加入买卖的实际代码!
		BuySell.OpenSale(nPlayerIndex, nShopId - 1, ST_Item);
	}
	return 0;
}

int LuaSmith(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) return 0;
	
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		int nShopId = (int)Lua_ValueToNumber(L,1);
		//----在以下加入买卖的实际代码!
		KSmithShop::getSinglton().beginSmith(nPlayerIndex, nShopId);
	}
	return 0;
}

int LuaHire(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 0) 
	{
		return 0;
	}
	
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		return 0;
	}

	BuySell.OpenHireShop(nPlayerIndex);

	return 0;
}

int LuaOpenBox(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex)) return 0;

	OPENSTOREBOX tagOpenStorage;
	tagOpenStorage.ProtocolType = s2c_openstorebox;
	tagOpenStorage.byNeedPassword = (BYTE)Player[nPlayerIndex].IsExistBoxPassword();

	if(!tagOpenStorage.byNeedPassword)
	{
		//如果没有密码，则自动解锁
		Player[nPlayerIndex].unlockStoreBox();
		tagOpenStorage.byNeedPassword = 0;	//0表示无密码，未锁定（无密码肯定是未锁定）
	}
	else if(Player[nPlayerIndex].m_ItemList.IsLockStorageBox() == false)
	{	
		//如果有密码但是未锁定，界面也不用输入密码
		tagOpenStorage.byNeedPassword = 1;	//1表示有密码、未锁定
	}
	else
	{	
		//如果有密码而且已经锁定，则界面需要输入密码才能操作
		tagOpenStorage.byNeedPassword = 2;	//2表示有密码、已锁定
	}
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, 
		&tagOpenStorage, sizeof(OPENSTOREBOX));

	if (tagOpenStorage.byNeedPassword != 2)
		Player[nPlayerIndex].GetUIServerState().SetUIState(player_ui_repository,player_ui_state_open);
	// End <--

	// Add 2005-2-2
//	Player[nPlayerIndex].m_BuyInfo.m_nBuyIdx = STORAGEBOXGUID;
	
	return 0;
}


//Lucifer~yu[zhangjianyu] [12/30/2005] Add for KUiGeneralInputWnd
//begin------------------------------------------------------------------------
// OpenInputDlg("string",1,1,)
int LuaOpenGeneralInputDlg(Lua_State  * L)
{
//	int nPlayerIdx = ScriptGetPlayerIndex( );
//	int nParamCount = Lua_GetTopIndex( L );
//
//	if ( nParamCount != 3 )
//	{
//		return 0;
//	}
//
//	const char *szMsg	= NULL;
//	int	nType			= 0;	
//	int	nInputType		= 0;
//	if ( !Lua_IsString( L, 1 ) || !Lua_IsNumber( L, 2 ) || !Lua_IsNumber( L, 2 ) )
//	{
//		return 0;
//	}
//	szMsg		= Lua_ValueToString( L, 1 );
//	nType		= Lua_ValueToNumber( L, 2 );	
//	nInputType	= Lua_ValueToNumber( L, 3 );
//
//	if ( nType < 0 || nInputType < 0 )
//	{
//		return 0;
//	}
//
//
//	int nMsgLen = strlen( szMsg );
//	nMsgLen = min( nMsgLen, MAX_SENTENCE_LENGTH - 1 );
//	INPUGDLG_PARAM tagInputParam;
//	g_StrCpyLen( tagInputParam.szBuffer, szMsg, nMsgLen + 1 );
//	tagInputParam.szBuffer[nMsgLen] = 0;
//	
//	tagInputParam.ProtocolType	= s2c_calldlg_input;
//	tagInputParam.nType			= nType;
//	tagInputParam.nInputType	= nInputType;
//	tagInputParam.wLength		= sizeof( tagInputParam ) - sizeof( tagInputParam.szBuffer ) + nMsgLen;
//
//	if (g_pServer != NULL)
//		g_pServer->PackDataToClient( Player[nPlayerIdx].m_nNetConnectIdx, &tagInputParam, tagInputParam.wLength );

	return 0;
}
//end--------------------------------------------------------------------------	

//---------------------------------时间任务-------------------------------------
//SetTimer(Time, TimerTaskId)
// int LuaSetTimer(Lua_State  * L)
// {
// 	int nParamCount = Lua_GetTopIndex(L);
// 	if (nParamCount < 2 ) return 0;
// 	int nPlayerIndex  = ScriptGetPlayerIndex();
// 	if (nPlayerIndex <= 0) return 0;
// 	Player[nPlayerIndex].SetTimer((DWORD) (int)Lua_ValueToNumber(L, 1), (int)Lua_ValueToNumber(L,2));
// 	return 0;
// }
// 
// int LuaStopTimer(Lua_State * L)
// {
// 	int nPlayerIndex  = ScriptGetPlayerIndex();
// 	if (nPlayerIndex <= 0) return 0;
// 	Player[nPlayerIndex].CloseTimer();
// 	return 0;
// }
// 
// int LuaGetCurTimerId(Lua_State * L)
// {
// 	int nPlayerIndex  = ScriptGetPlayerIndex();
// 	if (nPlayerIndex <= 0) 
// 	{
// 		Lua_PushNumber(L,0);
// 		return 1;
// 	}
// 	int nTimerId = Player[nPlayerIndex].m_TimerTask.GetTaskId();
// 	Lua_PushNumber(L, nTimerId);
// 	return 1;
// }
// 
// int LuaGetRestTime(Lua_State * L)
// {
// 	int nPlayerIndex  = ScriptGetPlayerIndex();
// 	if (nPlayerIndex <= 0) 
// 	{
// 		Lua_PushNil(L);
// 		return 1;
// 	}
// 	int nRestTime = Player[nPlayerIndex].m_TimerTask.GetRestTime();//m_dwTimeTaskTime - g_SubWorldSet.GetGameTime();
// 	
// 	if (nRestTime > 0)
// 		Lua_PushNumber(L, nRestTime);
// 	else
// 		Lua_PushNumber(L, 0);
// 	
// 	return 1;
// }

// int LuaGetMissionRestTime(Lua_State * L)
// {
// 	int RestTime = 0;
// 	if (Lua_GetTopIndex(L) >= 2)
// 	{
// 		int nSubWorldIndex = ScriptGetSubWorldIndex();
// 		if (nSubWorldIndex >= 0) 
// 		{
// 			int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 			int nTimerId = (int)Lua_ValueToNumber(L, 2);
// 			
// 			if (nMissionId < 0 || nTimerId < 0 )
// 				goto lab_getmissionresttime;
// 			
// 			KMission Mission;
// 			Mission.SetMissionId(nMissionId);
// 			KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 			if (pMission)
// 			{
// 				RestTime = (int)pMission->GetTimerRestTimer(nTimerId);
// 			}
// 		}
// 	}
// 	
// lab_getmissionresttime:
// 	Lua_PushNumber(L, RestTime);
// 	return 1;
// }
/************************************************************************/
/*						Team Script                                     */
/************************************************************************/
int LuaIsLeader(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex)) 
	{
		if (Player[nPlayerIndex].GetTeamInfo().IsInTeam() && Player[nPlayerIndex].GetTeamInfo().IsCaptain())
			Lua_PushNumber(L,1);
		else 
			Lua_PushNumber(L,0);
		
	}
	else 
		Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetTeamId(Lua_State * L)
{
	int teamId = INVALID_TEAM_ID;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		if (Player[playerIndex].GetTeamInfo().IsInTeam())
			teamId = Player[playerIndex].GetTeamInfo().GetTeamId();
	}

	Lua_PushNumber(L, teamId);
	return 1;
}

int LuaGetTeamMember(Lua_State * L)
{
	int memberPlayerIndex = 0;
	if (Lua_GetTopIndex(L) == 2)
	{
		int teamId = Lua_ValueToNumber(L, 1);
		int memberIndex = Lua_ValueToNumber(L, 2);
		
		if (teamId != INVALID_TEAM_ID && memberIndex > 0)
		{
			KTeam* pTeam = g_TeamSet.GetTeam(teamId);
			if (pTeam)
			{
				int existMemberCount = 0;
				int maxMemberCount = pTeam->GetMaxMemberCount();
				for ( int i = 0; i < maxMemberCount; ++i )
				{
					memberPlayerIndex = pTeam->GetMemberPlayerIndex(i);				
					if (memberPlayerIndex != INVALID_PLAYER_INDEX)
					{
						++existMemberCount;
					}
					if (existMemberCount == memberIndex)
					{
						break;
					}
				}
			}
		}
	}

	Lua_PushNumber(L, memberPlayerIndex);
	return 1;
}

int LuaGetTeamCaptain(Lua_State * L)
{
	int captainPlayerIndex = 0;
	if (Lua_GetTopIndex(L) == 1)
	{
		int teamId = Lua_ValueToNumber(L, 1);//队伍编号
		KTeam* pTeam = g_TeamSet.GetTeam(teamId);
		if (pTeam != NULL)
		{
			captainPlayerIndex = pTeam->GetCaptain();
		}
	}	
	
	Lua_PushNumber(L, captainPlayerIndex);
	return 1;
}

int LuaGetTeamSize(Lua_State * L)
{
	int memberCount = 0;
	if (Lua_GetTopIndex(L) == 1)
	{
		int teamId = Lua_ValueToNumber(L, 1);//队伍编号
		KTeam* pTeam = g_TeamSet.GetTeam(teamId);
		if (pTeam != NULL)
		{
			memberCount = pTeam->GetMemberCount();
		}
	}

	Lua_PushNumber(L, memberCount);
	return 1;
}

int LuaLeaveTeam(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		if (Player[nPlayerIndex].GetTeamInfo().IsInTeam())
		{
			Player[nPlayerIndex].GetTeamInfo().LeaveTeam();
		}
	}
	return 0;
}

//int LuaSetCreateTeamOption(Lua_State * L)
//{
	//TODO 这个已经没有意义了，需要删掉
// 	int nState = (int)Lua_ValueToNumber(L, 1);
// 	
// 	int nPlayerIndex = ScriptGetPlayerIndex();
// 	if (IsValidPlayer(nPlayerIndex))
// 	{
// 		if (nState)
// 			Player[nPlayerIndex].m_cTeam.SetCanTeamFlag(nPlayerIndex, TRUE);
// 		else
// 			Player[nPlayerIndex].m_cTeam.SetCanTeamFlag(nPlayerIndex, FALSE);
// 	}
//	return 0;
//}
//**************************************************************************************************************************************************************
//												聊天消息脚本
//**************************************************************************************************************************************************************

int	LuaMsgToPlayer(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0) 
		return 0;

	int nPlayerIndex = ScriptGetPlayerIndex();

	if (IsValidPlayer(nPlayerIndex)) 
	{
		if( Lua_IsNumber(L, 1) )
		{
			int nStrId = Lua_ValueToNumber(L, 1);
			g_ChatCenterS.SysMsgToSomeone(nPlayerIndex, SYSMSG_TYPE_ID, 
					(const BYTE*)&nStrId, sizeof(nStrId));
		}
		else if( Lua_IsString(L, 1) )
		{
			const char *  szMsg = Lua_ValueToString(L,1);

			if (szMsg)
				g_ChatCenterS.SysMsgToSomeone(nPlayerIndex, SYSMSG_TYPE_STR, 
					(const BYTE*)szMsg, strlen(szMsg));
		}
	}
	
	return 0;
}

int LuaShowMsg(Lua_State *L)
{
	if (Lua_GetTopIndex(L) <= 0) 
		return 0;

	int nPlayerIndex = ScriptGetPlayerIndex();

	if (IsValidPlayer(nPlayerIndex)) 
	{
		const char *  szMsg = Lua_ValueToString(L,1);

		if (szMsg)
			g_ChatCenterS.SysMsgToSomeone(nPlayerIndex, SYSMSG_TYPE_STR, 
				(const BYTE*)szMsg, strlen(szMsg));
	}	

	return 0;
}

int LuaGetLeagueCurrentMaxSubUnitCnt(Lua_State * L)
{
	int nResult   = 0;
	int nNpcIndex = -1;
	SocialUnit * pLeague = NULL ;

	if  ( Lua_GetTopIndex(L) < 1 )
		goto lab_LuaGetLeagueCurrentMaxSubUnitCnt;

	nNpcIndex= Lua_ValueToNumber(L,1);
	if (!IsValidNpc(nNpcIndex))
		goto lab_LuaGetLeagueCurrentMaxSubUnitCnt;

	if (Npc[nNpcIndex].IsPlayer())
	{
		int nPlayerIndex = Npc[nNpcIndex].GetPlayerIdx();
		if (IsValidPlayer(nPlayerIndex))
		{
			SocialUnit * pLeefUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
			pLeague                = GetUpNUnit(pLeefUnit,enSULayer_League);
		}//endif

	}//endif
	else
	{
		FSGUID    lordGUID = Npc[nNpcIndex].GetLord();
		SocialUnit * pUnit = ServerSocialUnitMgr::Singleton().GetUnit(lordGUID);
		if (pUnit && pUnit->GetLayer() == enSULayer_League)
		{
			pLeague = pUnit;
		}//endif
	}

	if (pLeague)
	{
		SocialUnitAttr & attr = pLeague->GetUnitAttr();	
		nResult               = GetForceSubNum(attr); 
	}//endif


lab_LuaGetLeagueCurrentMaxSubUnitCnt:
	Lua_PushNumber(L,nResult);
	return 1;
}

int LuaSetLeagueCurrentMaxSubUnitCnt(Lua_State * L)
{
	int nResult    = 0;
	int nNpcIndex  = -1;
	int nNewSubCnt = 0;
	SocialUnit * pLeague = NULL ;
	
	if  ( Lua_GetTopIndex(L) < 2 )
		return 0;
	
	nNpcIndex  = Lua_ValueToNumber(L,1);
	if (!IsValidNpc(nNpcIndex))
		return 0;
	
	nNewSubCnt = Lua_ValueToNumber(L,2);

	if (Npc[nNpcIndex].IsPlayer())
	{
		int nPlayerIndex = Npc[nNpcIndex].GetPlayerIdx();
		if (IsValidPlayer(nPlayerIndex))
		{
			SocialUnit * pLeefUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
			pLeague                = GetUpNUnit(pLeefUnit,enSULayer_League);
		}//endif
		
	}//endif
	else
	{
		FSGUID    lordGUID = Npc[nNpcIndex].GetLord();
		SocialUnit * pUnit = ServerSocialUnitMgr::Singleton().GetUnit(lordGUID);
		if (pUnit && pUnit->GetLayer() == enSULayer_League)
		{
			pLeague = pUnit;
		}//endif
	}
	
	if (pLeague)
	{
		if (pLeague->GetChildCount() <= nNewSubCnt && nNewSubCnt <= GetMaxSubUnitCnt(enSUTplId_Tong,enSULayer_League))
		{
			SocialUnitAttr & attr = pLeague->GetUnitAttr();		
			bool bResult = attr.ChangeAttr(enSUAttr_ForceSubUnitMaxNum,(const char *)&nNewSubCnt,sizeof(nNewSubCnt));
			_ASSERT(bResult);

			if (bResult)
			{
				if( attr.IsAttrSaveToDb(enSUAttr_ForceSubUnitMaxNum))
					SocialSerializer::Singleton().UpdateAttrReq(-1, pLeague);
				
			}//endif

		}//endif

	}//endif
	
	return 0;
}

int LuaGetBelongCityMapId(Lua_State * L)
{
	int nResult   = 0;
	int nNpcIndex = -1;
	SocialUnit * pLeague = NULL ;
	
	if  ( Lua_GetTopIndex(L) < 1 )
		goto lab_GetBelongCityMapId;
	
	nNpcIndex= Lua_ValueToNumber(L,1);
	if (!IsValidNpc(nNpcIndex))
		goto lab_GetBelongCityMapId;
	
	if (Npc[nNpcIndex].IsPlayer())
	{
		int nPlayerIndex = Npc[nNpcIndex].GetPlayerIdx();
		if (IsValidPlayer(nPlayerIndex))
		{
			SocialUnit * pLeefUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
			pLeague                = GetUpNUnit(pLeefUnit,enSULayer_League);
		}//endif
		
	}//endif
	else
	{
		FSGUID    lordGUID = Npc[nNpcIndex].GetLord();
		SocialUnit * pUnit = ServerSocialUnitMgr::Singleton().GetUnit(lordGUID);
		if (pUnit && pUnit->GetLayer() == enSULayer_League)
		{
			pLeague = pUnit;
		}//endif
	}
	
	if (pLeague)
	{
		SocialUnitAttr & attr = pLeague->GetUnitAttr();	
		nResult               = GetCityMapId(attr); 
	}//endif
	
	
lab_GetBelongCityMapId:
	Lua_PushNumber(L,nResult);
	return 1;
}

int LuaMsgToTeam(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0	) return 0;
	
	int nPlayerIndex = ScriptGetPlayerIndex();

	if(!IsValidPlayer(nPlayerIndex))
		return 0;

	if (Player[nPlayerIndex].GetTeamInfo().IsInTeam())
	{
		if( Lua_IsNumber(L, 1) )
		{
			int nStrId = Lua_ValueToNumber(L, 1);
			g_ChatCenterS.SysMsgToTeam(Player[nPlayerIndex].GetTeamInfo().GetTeamId(), 
					SYSMSG_TYPE_ID, (const BYTE*)&nStrId, sizeof(nStrId));
		}
		else if( Lua_IsString(L, 1) )
		{
			const	char * szMsg = Lua_ValueToString(L,1);
		
			if(szMsg)
				g_ChatCenterS.SysMsgToTeam(Player[nPlayerIndex].GetTeamInfo().GetTeamId(),
					SYSMSG_TYPE_STR, (const BYTE*)szMsg, strlen(szMsg));
		}
	}

	return 0;
}

#define MAX_COMBINE_STRING_BUFF_SIZE 1024

int LuaMsgToTongEx(Lua_State * L)
{
	int nRet      = 0;
	int nParamNum = Lua_GetTopIndex(L);
	
	if (nParamNum >= 4) //NpcIndex , ShowType , StringID
	{
		int    nNpcIndex   = Lua_ValueToNumber(L,1);
		
		if (!IsValidNpc(nNpcIndex))
			goto lab_LuaMsgToTongEx;
		
		int    nLayer      = Lua_ValueToNumber(L,2);
		if ((nLayer < enSULayer_Gens || nLayer > enSULayer_League) && Npc[nNpcIndex].m_Kind == kind_player)
			goto lab_LuaMsgToTongEx;

		SocialUnit * pTargetUnit = NULL;

		if (Npc[nNpcIndex].m_Kind == kind_player)
		{
			int nPlayerIndex = Npc[nNpcIndex].GetPlayerIdx();
			if (!IsValidPlayer(nPlayerIndex))
				goto lab_LuaMsgToTongEx;

			SocialUnit * pLeaf  = GetLeafUnit(nPlayerIndex , enSUTplId_Tong);
		    SocialUnit * pNpUnit= GetUpNUnit (pLeaf,nLayer);

			if (pNpUnit == NULL)
				goto lab_LuaMsgToTongEx;

			pTargetUnit         = pNpUnit;

		}//endif
		else
		{
			FSGUID lordGUID       = Npc[nNpcIndex].GetLord();
			if (lordGUID.data[0] == 0)
				goto lab_LuaMsgToTongEx;
		
			SocialUnit * pLordUnit= ServerSocialUnitMgr::Singleton().GetUnit(lordGUID);
			if (pLordUnit == NULL)
				goto lab_LuaMsgToTongEx;

			pTargetUnit           = pLordUnit;

		}//end else
		
		if (pTargetUnit == NULL)
			goto lab_LuaMsgToTongEx;

		DWORD dwShowType = Lua_ValueToNumber(L,3);
		if ((dwShowType & MSG_SHOWTYPE_TOPSCREEN == 0) && (dwShowType & MSG_SHOWTYPE_MIDDLESCREEN == 0) 
			&& (dwShowType & MSG_SHOWTYPE_ROOM == 0) && (dwShowType & MSG_SHOWTYPE_SPECIAL ) == 0)
			goto lab_LuaMsgToTongEx;
		
		int    nStringID = Lua_ValueToNumber(L,4);
		if  (  nStringID < 0 )
			goto lab_LuaMsgToTongEx;
		
		if   (MAX_COMBINE_STRING_BUFF_SIZE < MAXSIZE_CHAT_MSG)
			goto lab_LuaMsgToTongEx;
		
		char   szCombineString[MAX_COMBINE_STRING_BUFF_SIZE] = "";
		int *  pStringID                                     = (int *)szCombineString;
		*pStringID                                           = nStringID;
		
		int    nTotalSize                                    = sizeof(int) ;
		char*  szDestStringPointer                           = szCombineString + sizeof(int);
		
		if (nParamNum > 4) //Parse  the string parameters
		{
			int nStringParamNum  = nParamNum - 4;
			for (int nParamIndex = 5 ; nParamIndex < 5 + nStringParamNum; nParamIndex ++ )
			{
				if ( !Lua_IsString(L,nParamIndex))
					goto lab_LuaMsgToTongEx;
				
				const char * szString   = Lua_ValueToString(L,nParamIndex);
				if (szString == NULL)
					goto lab_LuaMsgToTongEx;
				
				int          nStringLen  = strlen(szString);
				int          nStringSize = nStringLen + 1;
				
				if ( nTotalSize + nStringSize >= MAXSIZE_CHAT_MSG -1)
					goto lab_LuaMsgToTongEx;
				
				memcpy(szDestStringPointer,szString,nStringSize);
				szDestStringPointer        += nStringSize;
				nTotalSize                 += nStringSize;
			}//end for nParamIndex
			
		}//endif
		
		DWORD dwChatRoomId = GetChatRoomId( pTargetUnit->GetUnitAttr() );
		g_ChatCenterS.SysMsgToTong(pTargetUnit,SYSMSG_TYPE_ID,(unsigned char *)szCombineString,nTotalSize,dwChatRoomId,dwShowType);
			
		nRet = 1;
	}//endif
	
lab_LuaMsgToTongEx:
	Lua_PushNumber(L,nRet);
	return 1;
}

int LuaMsgToMapEx(Lua_State *L)
{
	int nRet      = 0;
	int nParamNum = Lua_GetTopIndex(L);
	
	if (nParamNum >= 3) //NpcIndex , ShowType , StringID
	{
		int    nNpcIndex   = Lua_ValueToNumber(L,1);

		if (!IsValidNpc(nNpcIndex))
			goto lab_msgtomapex;

		int    nWorldIndex = Npc[nNpcIndex].GetSubWorldIndex();
		if (nWorldIndex < 0 || nWorldIndex > MAX_SUBWORLD)
			goto lab_msgtomapex;

		DWORD dwShowType = Lua_ValueToNumber(L,2);
		if ((dwShowType & MSG_SHOWTYPE_TOPSCREEN == 0) && (dwShowType & MSG_SHOWTYPE_MIDDLESCREEN == 0) 
			&& (dwShowType & MSG_SHOWTYPE_ROOM == 0) && (dwShowType & MSG_SHOWTYPE_SPECIAL ) == 0)
			goto lab_msgtomapex;

		int    nStringID = Lua_ValueToNumber(L,3);
		if  (  nStringID < 0 )
			goto lab_msgtomapex;

		if   (MAX_COMBINE_STRING_BUFF_SIZE < MAXSIZE_CHAT_MSG)
			goto lab_msgtomapex;

		char   szCombineString[MAX_COMBINE_STRING_BUFF_SIZE] = "";
		int *  pStringID                                     = (int *)szCombineString;
		*pStringID                                           = nStringID;

		int    nTotalSize                                    = sizeof(int) ;
		char*  szDestStringPointer                           = szCombineString + sizeof(int);

		if (nParamNum > 3) //Parse  the string parameters
		{
			int nStringParamNum  = nParamNum - 3;
			for (int nParamIndex = 4 ; nParamIndex < 4 + nStringParamNum; nParamIndex ++ )
			{
				if ( !Lua_IsString(L,nParamIndex))
					goto lab_msgtomapex;

				const char * szString   = Lua_ValueToString(L,nParamIndex);
				if (szString == NULL)
					goto lab_msgtomapex;
			
				int          nStringLen  = strlen(szString);
				int          nStringSize = nStringLen + 1;

				if ( nTotalSize + nStringSize >= MAXSIZE_CHAT_MSG -1)
					goto lab_msgtomapex;

				memcpy(szDestStringPointer,szString,nStringSize);
				szDestStringPointer        += nStringSize;
				nTotalSize                 += nStringSize;
			}//end for nParamIndex

		}//endif

		g_ChatCenterS.SysMsgToMap(nWorldIndex,SYSMSG_TYPE_ID,(unsigned char *)szCombineString,nTotalSize,SYSTEM_ROOM_ID,dwShowType);
		
		nRet = 1;
	}//endif

lab_msgtomapex:
	Lua_PushNumber(L,nRet);
	return 1;
}

int LuaMsgToTong(Lua_State *L)
{
	int          nLayer = Lua_ValueToNumber(L,1);
	const char * szMsg  = Lua_ValueToString(L,2);

	int          nPlayerIndex = ScriptGetPlayerIndex();

	if (IsValidPlayer(nPlayerIndex) && szMsg)
	{
		SocialUnit * pLeaf  = GetLeafUnit(nPlayerIndex , enSUTplId_Tong);
		SocialUnit * pNpUnit= GetUpNUnit(pLeaf,nLayer);
		
		if (pNpUnit)
		{	
			char szChatMsg[MAXSIZE_CHAT_MSG];
			strncpy(szChatMsg,szMsg,sizeof(szChatMsg));
			szChatMsg[MAXSIZE_CHAT_MSG-1] =0;

			int nMsgLen = strlen(szChatMsg);
			
			if(nMsgLen < 0 || nMsgLen >= sizeof(szChatMsg))
				return 0;
			
			g_ChatCenterS.SysMsgToTong(pNpUnit, 
				SYSMSG_TYPE_STR, 
				(const BYTE*)&szChatMsg, 
				nMsgLen
	     	);

		}//endif

	}//endif

	return 0;
}

int LuaMsgToFaction(Lua_State *L)
{
	if(Lua_GetTopIndex(L) <= 0)
		return 0;

	int nPlayerIdx = ScriptGetPlayerIndex();

	if(!IsValidPlayer(nPlayerIdx))
		return 0;

	int nFaction = (int)Lua_ValueToNumber(L, 1);

	if( Lua_IsNumber(L, 2) )
	{
		int nStrId = (int)Lua_ValueToNumber(L, 2);
		g_ChatCenterS.SysMsgToFaction(nFaction, SYSMSG_TYPE_ID, (const BYTE*)&nStrId, sizeof(nStrId));
	}
	else if( Lua_IsString(L, 2) )
	{
		const char *szMsg = Lua_ValueToString(L, 2);

		if(szMsg)
			g_ChatCenterS.SysMsgToFaction(nFaction, SYSMSG_TYPE_STR, (const BYTE*)szMsg, strlen(szMsg));

	}

	return 0;
}

int LuaMsgToAllPlayer(Lua_State *L)
{
	if(Lua_GetTopIndex(L) <= 0)
		return 0;

	int nPlayerIdx = ScriptGetPlayerIndex();

	if(!IsValidPlayer(nPlayerIdx))
		return 0;

	if( Lua_IsNumber(L, 1) )
	{
		int nStrId = (int)Lua_ValueToNumber(L, 1);
		g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_ID, (const BYTE*)&nStrId, sizeof(nStrId));
	}
	else if( Lua_IsString(L, 1) )
	{
		const char *szMsg = Lua_ValueToString(L, 1);

		if(szMsg)
			g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_STR, (const BYTE*)szMsg, strlen(szMsg));
	}

	return 0;
}

//**************************************************************************************************************************************************************
//												主角脚本
//**************************************************************************************************************************************************************

/*功能：让玩家进入新的一个游戏世界
nPlayerIndex:主角的Index
nSubWorldIndex:游戏世界id
nPosX:
nPosY:
*/

// --> Rocker Edit Start 2005/08/22
int LuaBeginMotion(Lua_State * L)
{
	return 0;
}
// <-- Rocker End

// --> Rocker Edit Start 2005/11/17
int LuaGetNpcID(Lua_State * L)
{
	int nNpcIdx = Lua_ValueToNumber(L ,1);
	int nR = -1;
	if ((nNpcIdx <= 0)||(nNpcIdx >= MAX_NPC))
	{
		Lua_PushNumber(L, nR);
		return 1;
	}

	Lua_PushNumber(L, Npc[nNpcIdx].m_dwID);
	return 1;
}
// <-- Rocker End

int LuaIsEquipItem(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex)) 
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nItemDetail = Lua_ValueToNumber(L, 1);
	int nItemParticular = Lua_ValueToNumber(L, 2);
	int nItemLevel = Lua_ValueToNumber(L, 3);

	if (Player[nPlayerIndex].m_ItemList.HaveItemEquiped(nItemDetail, nItemParticular, nItemLevel))
	{
		Lua_PushNumber(L, 1);
		return 1;
	}

	Lua_PushNumber(L, 0);
	return 1;
}
//<-- End

//--> Rocker 2005/07/13
int LuaNpcSay(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2)
		return 0;

	int nNpcIdx = Lua_ValueToNumber(L, 1);

	if ( (nNpcIdx <= 0) || (nNpcIdx >= MAX_NPC) )
		return 0;

	if( Lua_IsNumber(L, 2) )
	{
		int nStrId = (int)Lua_ValueToNumber(L, 2);
		g_ChatCenterS.SysNpcMsg(nNpcIdx, SYSMSG_TYPE_ID, (const BYTE*)&nStrId, sizeof(nStrId));
	}
	else if( Lua_IsString(L, 2) )
	{
		const char *szMsg = Lua_ValueToString(L, 2);

		if(szMsg)
			g_ChatCenterS.SysNpcMsg(nNpcIdx, SYSMSG_TYPE_STR, (const BYTE*)szMsg, strlen(szMsg));
	}

	return 0;
}
//<-- End


//NewWorld(WorldId, X,Y)
int  LuaEnterNewWorld(Lua_State * L) 
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex)) 
		return 0;
	
	int nResult = 0;
	if (Lua_GetTopIndex(L) == 3)
	{
		DWORD dwWorldId = (DWORD)Lua_ValueToNumber(L, 1);
		int X = (int)Lua_ValueToNumber(L,2) * 32;
		int Y = (int)Lua_ValueToNumber(L,3) * 32;
		nResult = Npc[Player[nPlayerIndex].m_nIndex].ChangeWorld(dwWorldId, X, Y);

	}
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetNpcLevel(Lua_State* L)
{
	int nNpcLevel = 0;
	if (Lua_GetTopIndex(L) >= 1)
	{
		int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
		if ((nNpcIdx > 0)&&(nNpcIdx < MAX_NPC))
		{
			nNpcLevel = Npc[nNpcIdx].m_Level;
		}
	}

	Lua_PushNumber(L, nNpcLevel);
	return 1;
}

//////////////////////////////////////////////////////////////////////////
// 第一个参数是掉在哪个NPC周围, 第二个参数是属于哪个玩家可以传-1
int LuaThrowItem(Lua_State* L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 7)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int nNpcIndex		= (int)Lua_ValueToNumber(L, 1);
	int nBelongPlayer	= (int)Lua_ValueToNumber(L, 2);
	int nItemClass		= (int)Lua_ValueToNumber(L, 3);
	int nDetailType		= (int)Lua_ValueToNumber(L, 4);
	int nParticularType	= (int)Lua_ValueToNumber(L, 5);
	int nLevel			= (int)Lua_ValueToNumber(L, 6);
	int nSeries			= (int)Lua_ValueToNumber(L, 7);
	int nLuck			= (int)Lua_ValueToNumber(L, 8);
	int nItemLevel[6];
	ZeroMemory(nItemLevel, sizeof(nItemLevel));
	nItemLevel[0] = (int)Lua_ValueToNumber(L, 9);
	for (int i = 0; i < 5; i ++)
		nItemLevel[i + 1] = nItemLevel[0];

	if ((nNpcIndex <= 0)||(nNpcIndex >= MAX_NPC))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if ((nBelongPlayer < -1)||(nBelongPlayer >= MAX_PLAYER))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if (Npc[nNpcIndex].m_SubWorldIndex < 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nIndex = ItemSet.Add(
		nItemClass, 
		nDetailType,
		nParticularType,
		nLevel, 
		1);

	if (nIndex <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	KMapPos sMapPos;
	POINT Pos;
	int nX, nY;
	Npc[nNpcIndex].GetMpsPos(&nX, &nY);
	Pos.x = nX;
	Pos.y = nY;
	
	SubWorld[Npc[nNpcIndex].m_SubWorldIndex].GetFreeObjPos(Pos);
	sMapPos.nSubWorld = Npc[nNpcIndex].m_SubWorldIndex;
	SubWorld[Npc[nNpcIndex].m_SubWorldIndex].Mps2Map(
		Pos.x, 
		Pos.y, 
		&sMapPos.nRegion, 
		&sMapPos.nMapX, 
		&sMapPos.nMapY, 
		&sMapPos.nOffX, 
		&sMapPos.nOffY);

	KObjItemInfo	sInfo;
	sInfo.m_nItemID = nIndex;
	sInfo.m_nMoneyNum = (Item[nIndex].GetMaxItemCount()==0)?0:Item[nIndex].GetItemCount();
	strcpy(sInfo.m_szName, Item[nIndex].GetName());
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
			
	int nObj = ObjSet.Add(Item[nIndex].GetObjIdx(), sMapPos, sInfo , nBelongPlayer);

	//ItemDebugLog Begin........................
    #ifdef _SERVER
	int nItemIndex = nIndex;
	if (nItemIndex > 0 && nItemIndex < MAX_ITEM && Item[nItemIndex].GetBelong() != -1)
	{
		int  nBelong         = Item[nItemIndex].GetBelong();
		char szDumpInfo[512] = "";
		snprintf(szDumpInfo,sizeof(szDumpInfo),"ScriptThrow:Index %d,Belong:%d,PlayerIdnex:%d,Name:%s\n",nItemIndex,nBelong,ScriptGetPlayerIndex(),Item[nItemIndex].GetName());
		szDumpInfo[sizeof(szDumpInfo) - 1] = 0;
		DumpInvalidItemOpeStack(false,szDumpInfo,4);
	}//endif
    #endif
    //ItemDebugLog End.........................

/*	if (nObj >= 0)
	{
		Object[nObj].SetItemBelong(nBelongPlayer);
	}
*/
	
	Lua_PushNumber(L, 1);
	return 1;
}

//<-- End


//SetPos(X,Y)
int LuaSetPos(Lua_State * L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount != 2) return 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	int nX = (int) Lua_ValueToNumber(L,1);
	int nY = (int) Lua_ValueToNumber(L,2);
	
	if (IsValidPlayer(nPlayerIndex))
	{
		Npc[Player[nPlayerIndex].m_nIndex].SetPos(nX * 32, nY * 32);
	}
	return 0;
}

int LuaRandomTrans(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();

	if(IsValidPlayer(nPlayerIndex))
	{
		if(Npc[Player[nPlayerIndex].m_nIndex].RandomTrans())
		{
			Lua_PushNumber(L, 1);
			return 1;
		}

	}

	Lua_PushNumber(L, 0);
	return 1;
}

// W,X,Y = GetNpcWorldPos()
int LuaGetWorldPos(Lua_State * L)
{
	int posX = 0;
	int posY = 0;
	int nSubWorldID = 0;

	int nNpcIdx = 0;
	if (Lua_GetTopIndex(L) >= 1)
	{
		nNpcIdx = (int) Lua_ValueToNumber(L,1);
	}
	else
	{
		int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex))
		{
			nNpcIdx = Player[nPlayerIndex].m_nIndex;
		}
	}

	if (nNpcIdx < MAX_NPC && nNpcIdx > 0) // 0号Npc无效
	{
		Npc[nNpcIdx].GetMpsPos(&posX, &posY);
		unsigned int nSubWorldIndex = Npc[nNpcIdx].m_SubWorldIndex;
		if (nSubWorldIndex < MAX_SUBWORLD)
		{
			nSubWorldID = SubWorld[nSubWorldIndex].m_SubWorldID;
		}
	}
	Lua_PushNumber(L, nSubWorldID); 
	Lua_PushNumber(L, ((int)(posX / 32)));
	Lua_PushNumber(L, ((int)(posY / 32)));
	return 3;
}
// Added End

int LuaAddItem(Lua_State * L)
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);

	int nItemClass = 0;		
	int nDetailType = 0;
	int nParticularType = 0;
	int nLevel = 0;
	int nCount = 0;

	if (nParamNum < 5)
	{
//		int nItemID		= (int)Lua_ValueToNumber(L, 1);
//		nLevel			= 0;
//		SpliteHashId(nItemID, nItemClass, nDetailType, nParticularType);
//		nCount			= (int)Lua_ValueToNumber(L, 2);

		//参数错误，不添加
		Lua_PushNumber(L, 0);
		return 1;
	}
	else
	{
		nItemClass		= (int)Lua_ValueToNumber(L, 1);
		nDetailType		= (int)Lua_ValueToNumber(L, 2);
		nParticularType	= (int)Lua_ValueToNumber(L, 3);
		nLevel			= (int)Lua_ValueToNumber(L, 4);
		nCount			= (int)Lua_ValueToNumber(L, 5);
	}
	
	if ( nCount <= 0 )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	const KBASICPROP_ITEM* pItemTemplate = g_ItemGen.GetItemTemplate(nItemClass, nDetailType, nParticularType, nLevel);
	if (!pItemTemplate)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int leftCount = nCount;
	while (leftCount > 0)
	{
		int addCount = 1;
		if (pItemTemplate->nStack > 1)
			addCount = leftCount < pItemTemplate->nStack ? leftCount : pItemTemplate->nStack;
		
		int nIndex = ItemSet.Add(
			nItemClass, 
			nDetailType, 
			nParticularType, 
			nLevel,
			addCount );
		if ( nIndex > 0 && nItemClass == item_ib )
		{
			Item[nIndex].SetIBBuyDate( UNIX_TMIE_STAMP );
		}

		if (nIndex <= 0)
		{
			Lua_PushNumber(L, nCount - leftCount);
			return 1;
		}
		
		KItem& item = Item[nIndex];

		if(nParamNum >= 6)
		{
			int nCreditFlag		= (int)Lua_ValueToNumber(L, 6);		
			if ( nCreditFlag >= 0 )
			{
				item.SetCreditFlag(nCreditFlag);
			}
		}

		if(nParamNum == 8)
		{
			int dur			= (int)Lua_ValueToNumber(L, 7);
			int maxDur		= (int)Lua_ValueToNumber(L, 8);
			item.SetMaxDurability(maxDur);
			item.SetDurability(dur);		
		}
		
		KPlayer& player = Player[nPlayerIndex];
		if (player.GetItemList().Add(nIndex, item_sync_type_gain) == 0)
		{
			ItemSet.Remove( nIndex );

			Lua_PushNumber(L, nCount - leftCount);
			return 1;
		}
		else
		{
			ConfigManager& cm = ConfigManager::Singleton();
			int nTicketGener = 0;
			int nTicketDetail = 0;
			int nTicketParticul = 0;
			int nTicketLevel = 0;
			cm.GetIBTicketId( &nTicketGener, &nTicketDetail, &nTicketParticul, &nTicketLevel );
			if ( nTicketGener == nItemClass && 
				nTicketDetail == nDetailType && 
				nTicketParticul == nParticularType && 
				nTicketLevel == nLevel )
			{
				KIBLog::getSingleton().AddIBMoney( card_add, nPlayerIndex, nCount , &Item[nIndex].GetGUID());
			}

			//统计：系统增加物品（脚本）
			ItemTemplateId templateId;
			item.GetItemTemplateId(templateId);
			player.GetPlayerStatistic().AddItem(templateId, item.GetItemCount(), item_count_type_system_add);

		
			//日志：系统增加物品（脚本）
			bool needLog = (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
			if (needLog)
			{
				LogEventParam systemAddItemEvent;
				systemAddItemEvent.event = log_event_system_add_item;
				systemAddItemEvent.param1 = player.GetGUID();
				systemAddItemEvent.param2 = item.GetGUID();	
				systemAddItemEvent.param4 = item.GetItemCount();

				char itemTemplateId[32] = { 0 };
				item.GetItemTemplateId(itemTemplateId, sizeof(itemTemplateId));
				itemTemplateId[sizeof(itemTemplateId) - 1] = 0;
				snprintf(systemAddItemEvent.param3.data, sizeof(systemAddItemEvent.param3.data), "script %s(%s)", item.GetName(), itemTemplateId);
				systemAddItemEvent.param3.data[sizeof(systemAddItemEvent.param3.data) - 1] = 0;

				g_pLogSystem->Log(systemAddItemEvent);

				//InstantSave
				//if (nCount == 1)
				//{
				//	player.SaveItemData();
				//}
			}
		}

		leftCount -= addCount;
	}

	//InstantSave
	//if (nCount > 1)
	//{
	//	Player[nPlayerIndex].SaveItemData();
	//}

	Lua_PushNumber(L, nCount - leftCount);
	return 1;
}


int LuaAddTimeLimitItem(Lua_State * L)
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}//endif

	int nParamNum = Lua_GetTopIndex(L);

	int nItemClass = 0;		
	int nDetailType = 0;
	int nParticularType = 0;
	int nLevel = 0;
	int nCount = 0;

	if (nParamNum < 5)
	{
		//参数错误，不添加
		Lua_PushNumber(L, 0);
		return 1;
	}
	else
	{
		nItemClass		= (int)Lua_ValueToNumber(L, 1);
		nDetailType		= (int)Lua_ValueToNumber(L, 2);
		nParticularType	= (int)Lua_ValueToNumber(L, 3);
		nLevel			= (int)Lua_ValueToNumber(L, 4);
		nCount			= (int)Lua_ValueToNumber(L, 5);
	}
	
	if ( nCount <= 0 )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	const KBASICPROP_ITEM* pItemTemplate = g_ItemGen.GetItemTemplate(nItemClass, nDetailType, nParticularType, nLevel);
	if (!pItemTemplate)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int leftCount = nCount;
	while (leftCount > 0)
	{
		int addCount = 1;
		if (pItemTemplate->nStack > 1)
			addCount = leftCount < pItemTemplate->nStack ? leftCount : pItemTemplate->nStack;
		
		int nIndex = ItemSet.Add(
			nItemClass, 
			nDetailType, 
			nParticularType, 
			nLevel,
			addCount );

		if ( nIndex > 0)
		{
			Item[nIndex].SetIBBuyDate( UNIX_TMIE_STAMP );
			Item[nIndex].SetTaskGiven(TRUE); //Notice here to mean that the thing is given by task
		}//endif

		if (nIndex <= 0)
		{
			Lua_PushNumber(L, nCount - leftCount);
			return 1;
		}
		
		KItem& item = Item[nIndex];
		if(nParamNum >= 6)
		{
			int nCreditFlag		= (int)Lua_ValueToNumber(L, 6);		
			if ( nCreditFlag >= 0 )
			{
				item.SetCreditFlag(nCreditFlag);
			}
		}

		if(nParamNum == 8)
		{
			int dur			= (int)Lua_ValueToNumber(L, 7);
			int maxDur		= (int)Lua_ValueToNumber(L, 8);
			item.SetMaxDurability(maxDur);
			item.SetDurability(dur);		
		}
		
		KPlayer& player = Player[nPlayerIndex];
		if (player.GetItemList().Add(nIndex, item_sync_type_gain) == 0)
		{
			ItemSet.Remove( nIndex );

			Lua_PushNumber(L, nCount - leftCount);
			return 1;
		}
		else
		{
			ConfigManager& cm = ConfigManager::Singleton();
			int nTicketGener = 0;
			int nTicketDetail = 0;
			int nTicketParticul = 0;
			int nTicketLevel = 0;
			cm.GetIBTicketId( &nTicketGener, &nTicketDetail, &nTicketParticul, &nTicketLevel );
			if ( nTicketGener == nItemClass && 
				nTicketDetail == nDetailType && 
				nTicketParticul == nParticularType && 
				nTicketLevel == nLevel )
			{
				KIBLog::getSingleton().AddIBMoney( card_add, nPlayerIndex, nCount , &Item[nIndex].GetGUID());
			}

			//统计：系统增加物品（脚本）
			ItemTemplateId templateId;
			item.GetItemTemplateId(templateId);
			player.GetPlayerStatistic().AddItem(templateId, item.GetItemCount(), item_count_type_system_add);

		
			//日志：系统增加物品（脚本）
			bool needLog = (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
			if (needLog)
			{
				LogEventParam systemAddItemEvent;
				systemAddItemEvent.event = log_event_system_add_item;
				systemAddItemEvent.param1 = player.GetGUID();
				systemAddItemEvent.param2 = item.GetGUID();	
				strncpy(systemAddItemEvent.param3.data, "script", sizeof(systemAddItemEvent.param3.data));
				g_pLogSystem->Log(systemAddItemEvent);

				//InstantSave
				//if (nCount == 1)
				//{
				//	player.SaveItemData();
				//}
			}
		}

		leftCount -= addCount;
	}

	Lua_PushNumber(L, nCount - leftCount);
	return 1;
}

int LuaAddTaskBindItem(Lua_State * L)
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);

	int nItemClass = 0;		
	int nDetailType = 0;
	int nParticularType = 0;
	int nLevel = 0;
	int nCount = 0;

	if (nParamNum < 5)
	{
		//参数错误，不添加
		Lua_PushNumber(L, 0);
		return 1;
	}
	else
	{
		nItemClass		= (int)Lua_ValueToNumber(L, 1);
		nDetailType		= (int)Lua_ValueToNumber(L, 2);
		nParticularType	= (int)Lua_ValueToNumber(L, 3);
		nLevel			= (int)Lua_ValueToNumber(L, 4);
		nCount			= (int)Lua_ValueToNumber(L, 5);
	}
	
	if ( nCount <= 0 )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	const KBASICPROP_ITEM* pItemTemplate = g_ItemGen.GetItemTemplate(nItemClass, nDetailType, nParticularType, nLevel);
	if (!pItemTemplate)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int leftCount = nCount;
	while (leftCount > 0)
	{
		int addCount = 1;
		if (pItemTemplate->nStack > 1)
			addCount = leftCount < pItemTemplate->nStack ? leftCount : pItemTemplate->nStack;
		
		int nIndex = ItemSet.Add(
			nItemClass, 
			nDetailType, 
			nParticularType, 
			nLevel,
			addCount );
		if ( nIndex > 0 && nItemClass == item_ib )
		{
			Item[nIndex].SetIBBuyDate( UNIX_TMIE_STAMP );
		}

		if (nIndex <= 0)
		{
			Lua_PushNumber(L, nCount - leftCount);
			return 1;
		}
		
		KItem& item = Item[nIndex];
		if( nParamNum >= 6 )
		{
			int nCreditFlag	= static_cast< int >( Lua_ValueToNumber( L, 6 ) );
			if ( nCreditFlag >= 0 )
			{
				item.SetCreditFlag( nCreditFlag );
			}
		}
		
		if(nParamNum == 8)
		{
			int dur			= (int)Lua_ValueToNumber(L, 7);
			int maxDur		= (int)Lua_ValueToNumber(L, 8);
			item.SetMaxDurability(maxDur);
			item.SetDurability(dur);		
		}

		item.SetTaskGiven(TRUE); //Notice here to mean that the thing is given by task
		
		KPlayer& player = Player[nPlayerIndex];
		if (player.GetItemList().Add(nIndex, item_sync_type_gain) == 0)
		{
			ItemSet.Remove( nIndex );
			Lua_PushNumber(L, nCount - leftCount);
			return 1;
		}
		else
		{
			ConfigManager& cm = ConfigManager::Singleton();
			int nTicketGener = 0;
			int nTicketDetail = 0;
			int nTicketParticul = 0;
			int nTicketLevel = 0;
			cm.GetIBTicketId( &nTicketGener, &nTicketDetail, &nTicketParticul, &nTicketLevel );
			if ( nTicketGener == nItemClass && 
				nTicketDetail == nDetailType && 
				nTicketParticul == nParticularType && 
				nTicketLevel == nLevel )
			{
				KIBLog::getSingleton().AddIBMoney( card_add, nPlayerIndex, nCount , &Item[nIndex].GetGUID());
			}

			//统计：系统增加物品（脚本）
			ItemTemplateId templateId;
			item.GetItemTemplateId(templateId);
			player.GetPlayerStatistic().AddItem(templateId, item.GetItemCount(), item_count_type_system_add);

		
			//日志：系统增加物品（脚本）
			bool needLog = (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
			if (needLog)
			{
				LogEventParam systemAddItemEvent;
				systemAddItemEvent.event = log_event_system_reward_add_item;
				systemAddItemEvent.param1 = player.GetGUID();
				systemAddItemEvent.param2 = item.GetGUID();	
				strncpy(systemAddItemEvent.param3.data, "script", sizeof(systemAddItemEvent.param3.data));
				g_pLogSystem->Log(systemAddItemEvent);

				//InstantSave
				//if (nCount == 1)
				//{
				//	player.SaveItemData();
				//}
			}
		}

		leftCount -= addCount;
	}

	Lua_PushNumber(L, nCount - leftCount);
	return 1;
}

int LuaAddWrongItem(Lua_State * L)
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);

	int nItemClass = 0;		
	int nDetailType = 0;
	int nParticularType = 0;
	int nLevel = 0;
	int nCount = 0;

	if (nParamNum < 5)
	{
//		int nItemID		= (int)Lua_ValueToNumber(L, 1);
//		nLevel			= 0;
//		SpliteHashId(nItemID, nItemClass, nDetailType, nParticularType);
//		nCount			= (int)Lua_ValueToNumber(L, 2);

		//参数错误，不添加
		Lua_PushNumber(L, 0);
		return 1;
	}
	else
	{
		nItemClass		= (int)Lua_ValueToNumber(L, 1);
		nDetailType		= (int)Lua_ValueToNumber(L, 2);
		nParticularType	= (int)Lua_ValueToNumber(L, 3);
		nLevel			= (int)Lua_ValueToNumber(L, 4);
		nCount			= (int)Lua_ValueToNumber(L, 5);
	}
	
	if ( nCount <= 0 )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	const KBASICPROP_ITEM* pItemTemplate = g_ItemGen.GetItemTemplate(nItemClass, nDetailType, nParticularType, nLevel);
	if (!pItemTemplate)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int leftCount = nCount;
	while (leftCount > 0)
	{
		int addCount = 1;
		if (pItemTemplate->nStack > 1)
			addCount = leftCount < pItemTemplate->nStack ? leftCount : pItemTemplate->nStack;
		
		int nIndex = ItemSet.Add(
			nItemClass, 
			nDetailType, 
			nParticularType, 
			nLevel,
			addCount );
		if ( nIndex > 0 && nItemClass == item_ib )
		{
			Item[nIndex].SetIBBuyDate( UNIX_TMIE_STAMP );
		}

		if (nIndex <= 0)
		{
			Lua_PushNumber(L, nCount - leftCount);
			return 1;
		}
		
		KItem& item = Item[nIndex];
		item.SetItemCount(0);
		
		if(nParamNum == 8)
		{
			int dur			= (int)Lua_ValueToNumber(L, 7);
			int maxDur		= (int)Lua_ValueToNumber(L, 8);
			item.SetMaxDurability(maxDur);
			item.SetDurability(dur);		
		}
		
		KPlayer& player = Player[nPlayerIndex];
		if (player.GetItemList().Add(nIndex, item_sync_type_gain) == 0)
		{
			ItemSet.Remove( nIndex );

			Lua_PushNumber(L, nCount - leftCount);
			return 1;
		}
		else
		{
			ConfigManager& cm = ConfigManager::Singleton();
			int nTicketGener = 0;
			int nTicketDetail = 0;
			int nTicketParticul = 0;
			int nTicketLevel = 0;
			cm.GetIBTicketId( &nTicketGener, &nTicketDetail, &nTicketParticul, &nTicketLevel );
			if ( nTicketGener == nItemClass && 
				nTicketDetail == nDetailType && 
				nTicketParticul == nParticularType && 
				nTicketLevel == nLevel )
			{
				KIBLog::getSingleton().AddIBMoney( card_add, nPlayerIndex, nCount , &Item[nIndex].GetGUID() );
			}

			//统计：系统增加物品（脚本）
			ItemTemplateId templateId;
			item.GetItemTemplateId(templateId);
			player.GetPlayerStatistic().AddItem(templateId, item.GetItemCount(), item_count_type_system_add);

		
			//日志：系统增加物品（脚本）
			bool needLog = (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
			if (needLog)
			{
				LogEventParam systemAddItemEvent;
				systemAddItemEvent.event = log_event_system_add_item;
				systemAddItemEvent.param1 = player.GetGUID();
				systemAddItemEvent.param2 = item.GetGUID();	
				systemAddItemEvent.param4 = item.GetItemCount();

				char itemTemplateId[32] = { 0 };
				item.GetItemTemplateId(itemTemplateId, sizeof(itemTemplateId));
				itemTemplateId[sizeof(itemTemplateId) - 1] = 0;
				snprintf(systemAddItemEvent.param3.data, sizeof(systemAddItemEvent.param3.data), "script %s(%s)", item.GetName(), itemTemplateId);
				systemAddItemEvent.param3.data[sizeof(systemAddItemEvent.param3.data) - 1] = 0;
				
				g_pLogSystem->Log(systemAddItemEvent);

				//InstantSave
				//if (nCount == 1)
				//{
				//	player.SaveItemData();
				//}
			}
		}

		leftCount -= addCount;
	}

	//InstantSave
	//if (nCount > 1)
	//{
	//	Player[nPlayerIndex].SaveItemData();
	//}

	Lua_PushNumber(L, nCount - leftCount);
	return 1;
}


int LuaAddArmorSet(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();

	int armorsetId = (int)Lua_ValueToNumber(L, 1);
	int armorsetIndex = (int)Lua_ValueToNumber(L, 2);

	ArmorSetTable& asTable = ArmorSetTable::Singleton();
	const ArmorSet* pAS = asTable.GetArmorSet(armorsetId);
	const int playerIndex = ScriptGetPlayerIndex();
	if (pAS != NULL && IsValidPlayer(nPlayerIndex))
	{		
		for (int partLoopCount = 0; partLoopCount < pAS->PartCount; partLoopCount++)
		{
			if (armorsetIndex >= 0 && armorsetIndex < pAS->AlternativePartCount[partLoopCount])
			{
				const EquipmentID& equipId = pAS->PartIDs[partLoopCount][armorsetIndex];
				int itemIndex = ItemSet.Add(
					equipId.IDArray[0],
					equipId.IDArray[1],
					equipId.IDArray[2],
					equipId.IDArray[3],
					1);

				if (itemIndex > 0)
				{
					int	x, y;
					if (Player[playerIndex].m_ItemList.CheckCanPlaceInEquipment(&x, &y))
					{
						if (Player[playerIndex].m_ItemList.Add(itemIndex, pos_equiproom, x, y, NULL, item_sync_type_gain))
						{
							continue;
						}
					}

					ItemSet.Remove(itemIndex);
				}
			}
		}
	}	

	return 0;
}


int LuaIsExistItem(Lua_State *L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (4 == nParamCount)
	{
		const int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex))
		{
			int nItemClass		= (int)Lua_ValueToNumber(L, 1);
			int nDetailType		= (int)Lua_ValueToNumber(L, 2);
			int nParticularType	= (int)Lua_ValueToNumber(L, 3);
			int nLevel			= (int)Lua_ValueToNumber(L, 4);
			int nCount = Player[nPlayerIndex].m_ItemList.IsExistItemIB(nItemClass, nDetailType, nParticularType, nLevel);
			Lua_PushNumber(L, nCount);
			return 1;
		}
	}
	else if (1 == nParamCount)
	{
		const int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex))
		{
			int nItemID		= (int)Lua_ValueToNumber(L, 1);
			int nItemClass;		
			int nDetailType;
			int nParticularType;
			int nLevel		= 1;
			SpliteHashId(nItemID, nItemClass, nDetailType, nParticularType);
			int nCount = Player[nPlayerIndex].m_ItemList.IsExistItemIB(nItemClass, nDetailType, nParticularType, nLevel);
			Lua_PushNumber(L, nCount);
			return 1;
		}
	}

	return 0;
}

int LuaIsHaveItem(Lua_State *L)
{
	int nParamCount = Lua_GetTopIndex(L);
	if (4 == nParamCount)
	{
		const int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex))
		{
			int nItemClass		= (int)Lua_ValueToNumber(L, 1);
			int nDetailType		= (int)Lua_ValueToNumber(L, 2);
			int nParticularType	= (int)Lua_ValueToNumber(L, 3);
			int nLevel			= (int)Lua_ValueToNumber(L, 4);
			int nCount = Player[nPlayerIndex].m_ItemList.m_Room[room_equipment].HaveNormalItemIB(nItemClass, nDetailType, nParticularType, nLevel, FALSE);
			Lua_PushNumber(L, nCount);
			return 1;
		}
	}
	else if (1 == nParamCount)
	{
		const int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex))
		{
			int nItemID		= (int)Lua_ValueToNumber(L, 1);
			int nItemClass;		
			int nDetailType;
			int nParticularType;
			int nLevel		= 1;
			SpliteHashId(nItemID, nItemClass, nDetailType, nParticularType);
			int nCount = Player[nPlayerIndex].m_ItemList.m_Room[room_equipment].HaveNormalItemIB(nItemClass, nDetailType, nParticularType, nLevel, FALSE);
			Lua_PushNumber(L, nCount);
			return 1;
		}
	}

	return 0;
}

int LuaGetItemMaxStackCount(Lua_State *L)
{
	//检验输入参数
	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount != 4)
	{
		return 0;
	}

	if(!Lua_IsNumber(L, 1)) 
	{
		return 0;
	}
	if(!Lua_IsNumber(L, 2)) 
	{
		return 0;
	}
	if(!Lua_IsNumber(L, 3)) 
	{
		return 0;
	}
	if(!Lua_IsNumber(L, 4)) 
	{
		return 0;
	}
	int genre		= (int)Lua_ValueToNumber(L, 1);
	int detail		= (int)Lua_ValueToNumber(L, 2);
	int particular	= (int)Lua_ValueToNumber(L, 3);
	int level		= (int)Lua_ValueToNumber(L, 4);
	
	const KBASICPROP_ITEM* itemTemplate = g_ItemGen.GetItemTemplate(genre, detail, particular, level);

	int pileCount = 0;
	if(itemTemplate)
	{
		pileCount = itemTemplate->nStack;
	}
	Lua_PushNumber(L, pileCount);
	return 1;
}

int LuaIsAlive(Lua_State *L)
{
	const unsigned int nPlayerIndex = ScriptGetPlayerIndex();
    if (IsValidPlayer(nPlayerIndex))
	{
        lua_pushboolean(L,Npc[Player[nPlayerIndex].m_nIndex].IsAlive());
		return 1;
	}
    return 0;
}

int LuaDelNormalItemByID(Lua_State *L)
{
	if (2 == Lua_GetTopIndex(L))
	{
		const int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex))
		{
			int nItemID		= (int)Lua_ValueToNumber(L, 1);
			int nCount		= (int)Lua_ValueToNumber(L, 2);
			int nItemIdx	= Player[nPlayerIndex].m_ItemList.SearchID( nItemID );
			if ( nItemIdx > 0 )
			{
				BOOL nRet = FALSE;
				while ( nCount > 0 )
				{
					nRet = Player[nPlayerIndex].m_ItemList.DelNormalItemByID(nItemID);
					nCount--;
				}
				Lua_PushNumber(L, nRet);
			}
			else
			{
				Lua_PushNumber(L, 0);
			}			
		}
		else
		{
			Lua_PushNumber(L, 0);
		}
	}
	else
	{
		Lua_PushNumber(L, 0);
	}
	return 1;
}

int LuaSetItemCreditFlagByID(Lua_State *L)
{
	if (2 == Lua_GetTopIndex(L))
	{
		const int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex))
		{
			int nItemID		= (int)Lua_ValueToNumber(L, 1);
			int nCreditFlag		= (int)Lua_ValueToNumber(L, 2);
			int nItemIdx	= Player[nPlayerIndex].m_ItemList.SearchID( nItemID );
			if ( nItemIdx > 0 )
			{
				if ( nCreditFlag >= 0 )
				{
					Item[nItemIdx].SetCreditFlag(nCreditFlag);
					Item[nItemIdx].SyncAttribute(item_attr_credit_flag,Player[nPlayerIndex].GetNetConnectIdx());
				}				
				Lua_PushNumber(L, 1);
			}
			else
			{
				Lua_PushNumber(L, 0);
			}			
		}
		else
		{
			Lua_PushNumber(L, 0);
		}
	}
	else
	{
		Lua_PushNumber(L, 0);
	}
	return 1;
}

int LuaDelNormalItem(Lua_State *L)
{
	if (5 <= Lua_GetTopIndex(L))
	{
		const int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex ))
		{
			int nItemClass		= (int)Lua_ValueToNumber(L, 1);
			int nDetailType		= (int)Lua_ValueToNumber(L, 2);
			int nParticularType	= (int)Lua_ValueToNumber(L, 3);
			int nLevel			= (int)Lua_ValueToNumber(L, 4);
			int nCount			= (int)Lua_ValueToNumber(L, 5);
			bool checkLevelupTimes = true;
			if ( Lua_GetTopIndex( L ) >= 6 )
			{
				checkLevelupTimes = (int)Lua_ValueToNumber(L, 6) != 0;
			}
			BOOL nRet = FALSE;
			while ( nCount > 0 )
			{
				nRet = Player[nPlayerIndex].m_ItemList.DelNormalItemOnlyItemBox(nItemClass, nDetailType, nParticularType, nLevel, checkLevelupTimes );
				nCount--;
			}
			Lua_PushNumber(L, nRet);
		}
		else
			Lua_PushNumber(L,0);

	}
	else if (2 == Lua_GetTopIndex(L))
	{
		const int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex))
		{
			int nItemID		= (int)Lua_ValueToNumber(L, 1);
			int nCount		= (int)Lua_ValueToNumber(L, 2);
			int nItemClass;		
			int nDetailType;
			int nParticularType;
			int nLevel		= -1;
			SpliteHashId(nItemID, nItemClass, nDetailType, nParticularType);
			BOOL nRet = FALSE;
			while ( nCount > 0 )
			{
				nRet = Player[nPlayerIndex].m_ItemList.DelNormalItemOnlyItemBox(nItemClass, nDetailType, nParticularType, nLevel);
				nCount--;
			}
			Lua_PushNumber(L, nRet);
		}
		else
			Lua_PushNumber(L,0);

	}
	else
	{
		Lua_PushNumber(L, 0);
	}
	return 1;
}


//**************************************************************************************************************************************************************
//												NPC操作脚本
//**************************************************************************************************************************************************************
/*nNpcTemplateId GetNpcTmpId(sName)
功能从Npc模板中获得名称为sName的Npc在模板中的Id
sName:Npc名称
nNpcTemplateID:模板中Id
*/

int LuaGetNpcTemplateID(Lua_State * L)
{
	if (Lua_GetTopIndex(L) > 0)
	{
		if (Lua_IsNumber(L,1))
		{
			const unsigned int nIdx = Lua_ValueToNumber(L, 1);
			if (nIdx < MAX_NPC )
			{
				Lua_PushNumber(L, Npc[nIdx].m_NpcSettingIdx);			
				return 1;
			}
		}
	}
	return 0;
}

//addNpcIndex = AddNpc(npcTemplateId, npcLevel, setLord)//在玩家所在位置添加
//addNpcIndex = AddNpc(npcTemplateId, npcLevel, setLord, worldIndex, posX, posY)//在指定位置添加
int LuaAddNpc(Lua_State * L)
{
	int playerIndex   = ScriptGetPlayerIndex();
	int npcTemplateId = 0;
	if (Lua_IsNumber(L,1))
	{
		npcTemplateId = (int)Lua_ValueToNumber(L,1);
	}
	else if	(Lua_IsString(L,1))
	{
		char* pName = (char *)lua_tostring(L,1);	
		npcTemplateId = g_NpcSetting.FindRow((char*)pName) - 2;
	}
	else
	{
		return 0;
	}
	
	if (npcTemplateId < 0)
		npcTemplateId = 0;
	
	int npcLevel = (int)Lua_ValueToNumber(L,2);
	if (npcLevel < 0 )
		npcLevel = 1;
	
	int	npcIdxInfo = MAKELONG(npcLevel, npcTemplateId);
	
	int setLord = Lua_ValueToNumber(L, 3);
	
	int nWorldIndex = -1;
	int nX, nY;
	if (Lua_GetTopIndex(L) == 6)
	{
		nWorldIndex = Lua_ValueToNumber(L, 4);
		nX = Lua_ValueToNumber(L, 5);
		nY = Lua_ValueToNumber(L, 6);
	}
	else
	{
		playerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(playerIndex))
		{
			int playerNpcIndex = Player[playerIndex].GetNpcIndex();
			KNpc& playerNpc = Npc[playerNpcIndex];
			nWorldIndex = playerNpc.GetSubWorldIndex();
			playerNpc.GetMpsPos(&nX, &nY);
		}
	}

	if (nWorldIndex < 0)
	{
		//日志 添加失败
		if (g_pLogSystem)
		{	
			char szLogDesc[256] = { 0 };
			snprintf(szLogDesc, sizeof(szLogDesc), "NpcId=%d,Level=%d,SetLord=%d,World=%d,X=%d,Y=%d,Used=%d,Free=%d",
				npcTemplateId, 
				npcLevel,
				setLord,
				nWorldIndex,
				nX,
				nY,
				NpcSet.GetUsedNpcSlotCount(),
				NpcSet.GetFreeNpcSlotCount());
			szLogDesc[sizeof(szLogDesc) - 1] = 0;
			g_pLogSystem->SysDbgLog(szLogDesc, strlen(szLogDesc), sys_dbg_log_event_script_add_npc_failure);
		}

		return 0;
	}

	int addNpcIndex = NpcSet.Add(npcIdxInfo, nWorldIndex, nX, nY);		
	if(addNpcIndex > 0)
	{
		Npc[addNpcIndex].GetController().SetActive(true);
		Npc[addNpcIndex].NormalSync();
		
		int nMode = Npc[addNpcIndex].m_UnaryAttrMgr[nuai_deathmode];
		nMode |= npc_deathmode_autodel;
		Npc[addNpcIndex].m_UnaryAttrMgr.Set(nuai_deathmode, nMode);
		
		if (setLord != FALSE && IsValidPlayer(playerIndex))	
		{
			SocialUnit * pLeafUnit = GetLeafUnit(playerIndex,enSUTplId_Tong);
			SocialUnit * pUPNpUnit = GetUpNUnit(pLeafUnit,setLord);

			if (pUPNpUnit)
				Npc[addNpcIndex].SetLord(pUPNpUnit->GetUnitGuid());
		}//endif

		Lua_PushNumber(L, addNpcIndex);
		return 1;
	}

	//日志 添加失败
	if (g_pLogSystem)
	{	
		char szLogDesc[256] = { 0 };
		snprintf(szLogDesc, sizeof(szLogDesc), "NpcId=%d,Level=%d,SetLord=%d,World=%d,X=%d,Y=%d,Used=%d,Free=%d",
			npcTemplateId, 
			npcLevel,
			setLord,
			nWorldIndex,
			nX,
			nY,
			NpcSet.GetUsedNpcSlotCount(),
			NpcSet.GetFreeNpcSlotCount());
		szLogDesc[sizeof(szLogDesc) - 1] = 0;
		g_pLogSystem->SysDbgLog(szLogDesc, strlen(szLogDesc), sys_dbg_log_event_script_add_npc_failure);
	}

	return 0;
}

int luaSetNpcOwner(Lua_State * L)
{
	int nRet = 0;

	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L,0);
		return 1;
	}//endif

	int nPlayerIndex = ScriptGetPlayerIndex();
	int nNpcIndex    = Lua_ValueToNumber(L,1);
	int nLayer       = Lua_ValueToNumber(L,2);

	if ( ( nLayer >= enSULayer_None && nLayer < enSUTong_LayerNum ) &&
		  IsValidNpc(nNpcIndex) && !Npc[nNpcIndex].IsPlayer()
		  && IsValidPlayer(nPlayerIndex)
		)
	{
		if (nLayer == enSULayer_None)
		{
			FSGUID invalidGuid;
			Npc[nNpcIndex].SetLord(invalidGuid);
			nRet = 1;
		}//endif
		else
		{
			SocialUnit * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
			SocialUnit * pUnNUnit  = GetUpNUnit(pLeafUnit,nLayer);
			if (pUnNUnit)
			{
				Npc[nNpcIndex].SetLord(pUnNUnit->GetUnitGuid());
				nRet = 1;
			}//endif
		
		}//endelse
		
	}//endif

	Lua_PushNumber(L,nRet);
	return 1;
}

//DelNpc(npcIndex)//删除NPC
int LuaDelNpc(Lua_State * L)
{
	if (Lua_GetTopIndex(L) <= 0)
		return 0 ;

	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (IsValidNpc(nNpcIndex))
	{
		if (Npc[nNpcIndex].m_RegionIndex >= 0 && Npc[nNpcIndex].m_Kind != kind_player)
		{
			BuffMgr &mgr = BuffMgr::Singleton();
			mgr.ClearAllBuff( nNpcIndex, TRUE );

			SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_Region[Npc[nNpcIndex].m_RegionIndex].RemoveNpc(nNpcIndex);
			NpcSet.Remove(nNpcIndex);
		}
	}

	return 0;
}

/*
void SetNpcName(npcIdx,name)
功能: 修改Npc的名字
lixuewu 2004.07.15
 */
int LuaSetNpcName(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 2 ) return 0 ;
	const int nNpcIdx = (int)Lua_ValueToNumber(L, 1);
	if (IsValidNpc(nNpcIdx))
	{
		strcpy(Npc[nNpcIdx].Name, Lua_ValueToString(L,2));
	}
	return 0;
}


/*
SetNpcPos (nNpcIndex, x, y)
功能：设置/修改一个NPC的位置
参数：
nNpcIndex:Npc的id 
x：X坐标
y：Y坐标
*/
int LuaSetNpcPos(Lua_State * L)
{
	int nParamCount = 0;
	if ((nParamCount = Lua_GetTopIndex(L)) < 3) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0) return 0;
// 	Npc[nNpcIndex].m_MapX = (int)Lua_ValueToNumber(L, 2);
// 	Npc[nNpcIndex].m_MapY = (int)Lua_ValueToNumber(L, 3);
	
	return 0;
}


/*SetNpcDthSct (nNpcIndex, map, “*.txt” )
功能：设置NPC死亡脚本
参数：
nNpcIndex：NPCIndex
*.txt：脚本文件名
*/
int LuaSetNpcActionScript(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 2 ) return 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <= 0 || nNpcIndex >= MAX_NPC) return 0;
	Npc[nNpcIndex].m_ActionScriptID = g_FileName2Id((char *)Lua_ValueToString(L,2));
	return 0;
}

/*
SetRevivalPos(subworldid = -1, revid )
功能：设置Npc重生点
*/
int LuaSetPlayerRevivalPos(Lua_State * L)
{
	//Question
	int nParamCount = Lua_GetTopIndex(L);
	int nPlayerIndex = 0;
	int nBeginIndex = 2;
	nPlayerIndex = ScriptGetPlayerIndex();
	int nSubWorldId = 0;
	int nRevId = 0;
	if (!IsValidPlayer(nPlayerIndex)) 
	{
		return 0;
	}
	
	if (nParamCount >= 2)
	{
		nSubWorldId = (int) Lua_ValueToNumber(L, 1);
		nRevId = (int) Lua_ValueToNumber(L, 2);
	}
	else if (nParamCount == 1)
	{
		nSubWorldId = -1;
		nRevId = (int) Lua_ValueToNumber(L, 1);
	}
	else 
	{
		return 0;
	}
	
	Player[nPlayerIndex].SetRevivalPos(nSubWorldId, nRevId);
	return 0;
}

//**********************************************************************************************
//							主角属性获得
//**********************************************************************************************


#define MacroFun_GetPlayerInfoInt(L, MemberName) { int nPlayerIndex = ScriptGetPlayerIndex();\
	if (IsValidPlayer(nPlayerIndex)){	int nNpcIndex = Player[nPlayerIndex].m_nIndex;	if (nNpcIndex > 0)Lua_PushNumber(L, Npc[nNpcIndex].MemberName);\
	else Lua_PushNil(L);}\
	else Lua_PushNil(L);\
return 1;}														


#define GetNpcInfoByIdx(L, AttrMgrName, idx) { int nPlayerIndex = ScriptGetPlayerIndex();\
	if (IsValidPlayer(nPlayerIndex)){	int nNpcIndex = Player[nPlayerIndex].m_nIndex;	if (nNpcIndex > 0)Lua_PushNumber(L, Npc[nNpcIndex].AttrMgrName[idx]);\
	else Lua_PushNil(L);}\
	else Lua_PushNil(L);\
return 1;}	

// Add by cooler
// liuyujun@263.net 2004/3/26 -->
int LuaGetPlayerType(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();				
	if (IsValidPlayer(nPlayerIndex))								
	{	
		const int nSeries = Npc[Player[nPlayerIndex].m_nIndex].m_Series + 1;
		Lua_PushNumber(L, nSeries);
	}
	else
	{
		Lua_PushNumber(L, 0);
	}

	return 1;
}

// <-- End cooler add.
int LuaGetSkillSeries(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();				
	if (IsValidPlayer(nPlayerIndex))								
	{	
		const int nSkillSeries = Player[nPlayerIndex].m_SkillSeries;
		Lua_PushNumber(L, nSkillSeries);
	}
	else
	{
		Lua_PushNumber(L, 0);
	}

	return 1;
}


// Add by Cooler 2004-7-2
// Begin -->
int LuaGetNextLevelExp(Lua_State *L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if(IsValidPlayer(nPlayerIndex))
	{
        Lua_PushNumber(L, Player[nPlayerIndex].GetExpMax());
	}
	else
	{
		Lua_PushNil(L);
	}

	return 1;
}
// End <--

//经验值*********************************************************************
int LuaGetPlayerExp(Lua_State *L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex ))
	{
        Lua_PushNumber(L, Player[nPlayerIndex].GetExp());
	}
	else
		Lua_PushNil(L);
	return 1;
}

int LuaAddOwnExp(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	if (paramCount <=0 ) return 0;

	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		int nExp = Lua_ValueToNumber(L,1);
		int nTag = 0;
		if (paramCount == 2)
		{
			nTag = Lua_ValueToNumber(L,2);
		}
		
		if (nExp > 0 && nExp <= MAX_ADD_EXP)
		{
			if (nExp >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_exp_amount) && g_pLogSystem)
			{
				LogEventParam addExpEvent;
				addExpEvent.event = log_event_script_add_exp;
				addExpEvent.param1 = Player[nPlayerIndex].GetGUID();
				snprintf(addExpEvent.param2.data, sizeof(addExpEvent.param2.data), "%d", nTag);
				addExpEvent.param2.data[sizeof(addExpEvent.param2.data) - 1] = 0;
				addExpEvent.param4 = nExp;
				g_pLogSystem->Log(addExpEvent);
			}
			
			Player[nPlayerIndex].QuestAddExp(nExp);
		}
	}
	return 0;
}

//Exp Insurance ...Begin ......................

int luaSetGlobalExpInsuranceState( Lua_State * L )
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum >= 1)
	{
		int nEnable = Lua_ValueToNumber(L,1);
		KExpQuestInsuraceSetting::Singleton().SetExpEnable(nEnable?true:false);
	}//endif

	return 0;
}

int luaSetGlobalQuestInsuranceState( Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum >= 1)
	{
		int nEnable = Lua_ValueToNumber(L,1);
		KExpQuestInsuraceSetting::Singleton().SetQuestEnable(nEnable?true:false);
	}//endif

	return 0;
}

int luaEnterExpInsuranceState( Lua_State * L )
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		Player[nPlayerIndex].m_ExpInsuranceMgr.ReEnterInsuraceState();
	}//endif

	return 0;
}

int luaLeaveExpInsuranceState(Lua_State * L )
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		Player[nPlayerIndex].m_ExpInsuranceMgr.LeaveInsuranceState();
	}//endif

	return 0;
}

int luaIsEnteredExpInsuranceState( Lua_State * L)
{
	int nRet         = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();

	if (IsValidPlayer(nPlayerIndex))
	{
		nRet         =  Player[nPlayerIndex].m_ExpInsuranceMgr.IsEnterInsuraceState() ? 1:0;
	}//endif

	Lua_PushNumber(L,nRet);

	return 1;
}

int luaGetExpInsuranceReward( Lua_State * L )
{
	int nRet         = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		nRet         =  Player[nPlayerIndex].m_ExpInsuranceMgr.GetCurRewardExp();
	}//endif
	
	Lua_PushNumber(L,nRet);

	return 1;
}


int luaEnterQuestInsuranceState( Lua_State * L )
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		Player[nPlayerIndex].m_QuestInsuranceMgr.ReEnterInsuraceState();
	}//endif
	
	return 0;
}

int luaLeaveQuestInsuranceState(Lua_State * L )
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		Player[nPlayerIndex].m_QuestInsuranceMgr.LeaveInsuranceState();
	}//endif
	
	return 0;
}

int luaIsEnteredQuestInsuranceState( Lua_State * L)
{
	int nRet         = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		nRet         =  Player[nPlayerIndex].m_QuestInsuranceMgr.IsEnterInsuraceState() ? 1:0;
	}//endif
	
	Lua_PushNumber(L,nRet);
	
	return 1;
}

int luaGetQuestInsuranceState( Lua_State * L)
{
	int nRet         = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();

	if (IsValidPlayer(nPlayerIndex))
	{
		nRet         = Player[nPlayerIndex].m_QuestInsuranceMgr.GetOfflineTimeCache();
	}

	Lua_PushNumber(L,nRet);

	return 1;
}

int luaAddQuestInsuranceState( Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
    if ( Lua_GetTopIndex(L) != 1 )
		return 0;

	int nAdd         = Lua_ValueToNumber(L,1);
	if (nAdd < 0)
		return 0;

	if (IsValidPlayer(nPlayerIndex))
	{
		Player[nPlayerIndex].m_QuestInsuranceMgr.AddOfflineTimeCache(nAdd);
	}//endif

	return 0;
}

//Exp Insurance .. End ........................

int LuaGetPlayerLevel(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex ))
	{
		Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Level);
	} 
	else
		Lua_PushNil(L);
	return 1;
}

int LuaRestorePlayerLife(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex ))
	{
		int nNpcIdx = Player[nPlayerIndex].m_nIndex;
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curlife, Npc[nNpcIdx].m_CompAttrMgr[ncai_lifeuplimit]);
	}
	return 0;
}

int LuaRestorePlayerMana(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex ))
	{
		int nNpcIdx = Player[nPlayerIndex].m_nIndex;
		Npc[nNpcIdx].m_UnaryAttrMgr.Set(nuai_curmana, Npc[nNpcIdx].m_CompAttrMgr[ncai_manauplimit]);
	}
	return 0;
}

int LuaGetPlayerSex(Lua_State * L)
{
	MacroFun_GetPlayerInfoInt(L , m_nSex);
}


int LuaSaveQuickly(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex ))
	{
		Player[nPlayerIndex].SaveQuickly();
	} 
	return 0;
}

int LuaGetPlayerName(Lua_State * L)
{
    int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
        Lua_PushString(L, Player[nPlayerIndex].m_PlayerName);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetPlayerAccName(Lua_State * L)
{
    int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
        Lua_PushString(L, Player[nPlayerIndex].m_AccoutName);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetNpcNameByTemplateID(Lua_State * L)
{
	int nParamNum = Lua_GetTopIndex(L);

	int nNpcIdx = 0;		

	if (nParamNum < 1)
	{
		Lua_PushNil(L);
		return 1;
	}
	else
	{
		nNpcIdx		= (int)Lua_ValueToNumber(L, 1);
	}

	if (nNpcIdx < 0 || nNpcIdx >= MAX_NPCSTYLE  )
	{
		Lua_PushNil(L);
		return 1;
	}
	if ( g_pNpcTemplate[nNpcIdx][0] )
	{
        Lua_PushString(L, g_pNpcTemplate[nNpcIdx][0]->Name );
	}
	else
	{
		KNpcTemplate nNpcTemp;
		nNpcTemp.InitNpcBaseData( nNpcIdx );
		if ( nNpcTemp.Name[0] != 0 )
		{
			Lua_PushString(L, nNpcTemp.Name );
		}
		else
		{
			Lua_PushNil(L);
		}		
	}
	
	return 1;
}

int LuaGetItemName(Lua_State * L)
{
	int nItemClass = 0;		
	int nDetailType = 0;
	int nParticularType = 0;
	int nLevel = 0;
	int nParamNum = Lua_GetTopIndex(L);

	if (nParamNum < 4 )
	{
		Lua_PushNil(L);
		return 1;
	}
	else
	{
		nItemClass		= (int)Lua_ValueToNumber(L, 1);
		nDetailType		= (int)Lua_ValueToNumber(L, 2);
		nParticularType	= (int)Lua_ValueToNumber(L, 3);
		nLevel			= (int)Lua_ValueToNumber(L, 4);		
	}

	KItem tmpItem;
	if ( g_ItemGen.Gen_Item( nItemClass, nDetailType, nParticularType, nLevel, 1, &tmpItem ) )
	{
        Lua_PushString(L, tmpItem.GetName() );
	}
	else
		Lua_PushNil(L);
	
	return 1;
}

int LuaGetPlayerNameByIdx(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int index = Lua_ValueToNumber(L, 1);
		if (IsValidPlayer(index))
			Lua_PushString(L, Player[index].m_PlayerName);
		else
			Lua_PushNil(L);

	}else
	{
		Lua_PushNil(L);
	}
	return 1;
}

int LuaGetPlayerID(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
        Lua_PushNumber(L, Player[nPlayerIndex].m_dwID);
	}
	else
		Lua_PushNil(L);
	
	return 1;
}


int LuaGetPlayerCashMoney(Lua_State * L)
{
    int nPlayerIndex = ScriptGetPlayerIndex();
	
    if (IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetMoney(room_equipment));
	}
	else Lua_PushNumber(L,0);
	
	return 1;
}

int LuaGetBoxMoney(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	
    if (IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, Player[nPlayerIndex].m_ItemList.GetMoney(room_repository));
	}
	else
	{
		Lua_PushNumber(L,0);
	}
	
	return 1;
}

int LuaAddBoxMoney(Lua_State * L)
{
    int nPlayerIndex = ScriptGetPlayerIndex();
    if (IsValidPlayer(nPlayerIndex))
	{
        int nMoney = (int)Lua_ValueToNumber(L, 1);
        if (nMoney <= 0) return 0;
		
		if (Player[nPlayerIndex].m_ItemList.AddMoney(room_repository, nMoney))
		{
			PlayerSet.AddMoney((DWORD)nMoney);

			if (nMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
			{
				LogEventParam logParam;
				logParam.event = log_event_script_add_box_money;
				logParam.param1 = Player[nPlayerIndex].GetGUID();
				logParam.param4 = nMoney;
				g_pLogSystem->Log(logParam);
			}
		}
	}	
	
	return 0;
}


int LuaPlayerPayMoney(Lua_State * L)
{
    
    int nPlayerIndex = ScriptGetPlayerIndex();
    if (IsValidPlayer(nPlayerIndex))
	{
        int nMoney = (int)Lua_ValueToNumber(L, 1);
        if (nMoney <= 0) return 0;

        if (Player[nPlayerIndex].Pay(nMoney))
		{
			if (nMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
			{
				LogEventParam logParam;
				logParam.event = log_event_script_pay_money;
				logParam.param1 = Player[nPlayerIndex].GetGUID();
				logParam.param4 = -nMoney;
				g_pLogSystem->Log(logParam);
			}

			Lua_PushNumber(L, 1);
		}
		else
			Lua_PushNumber(L, 0);

	}
	else
		Lua_PushNumber(L, 0);
	
	return 1;
}

int luaSetAntiEnthralState(Lua_State  *L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		int  nParamNum = ( int ) Lua_GetTopIndex(L);
		if (nParamNum < 2)
			return 0;

		int  nEnable   = Lua_ValueToNumber(L,1);
		int  nStage    = Lua_ValueToNumber(L,2);

		if (nEnable == 1)
		{
			ConfigManager &cfg      = ConfigManager::Singleton();
			DWORD dwWearinessTime   = cfg.GetGlobalVariable(global_var_antienthrall_wearinesstime);
		    DWORD dwInsalubrityTime = cfg.GetGlobalVariable(global_var_antienthrall_insalubritytime);

			if (nStage == 0)
			{
				//nStaget == 0
				Player[nPlayerIndex].m_AntiEnthrall.PlayerOnline(dwWearinessTime + 1,3);
				Player[nPlayerIndex].m_AntiEnthrall.IncOnlineTime(nPlayerIndex);
			}//endif
			else
			{
				//nStaget == 1
				Player[nPlayerIndex].m_AntiEnthrall.PlayerOnline(dwInsalubrityTime + 1,3);
				Player[nPlayerIndex].m_AntiEnthrall.IncOnlineTime(nPlayerIndex);
			}//end else

		}//endif
		else
		{
			Player[nPlayerIndex].m_AntiEnthrall.PlayerOnline(0,2);
		}//end else


	}//endif

	return 0;
}

int LuaPlayerEarnMoney (Lua_State  *L)
{
    int nPlayerIndex = ScriptGetPlayerIndex();
    if (IsValidPlayer(nPlayerIndex))
	{
        int nMoney = (int)Lua_ValueToNumber(L, 1);
        if (nMoney <= 0) return 0;
		
		//防沉迷
		int nAntiEnthrallState = Player[nPlayerIndex].m_AntiEnthrall.GetCurState();
		
		if(AntiEnthrall::enAntiEnthrall_Weariness == nAntiEnthrallState)
			nMoney /= AntiEnthrall::WEARINESS_EXP_SCALE;
		else if(AntiEnthrall::enAntiEnthrall_Insalubrity == nAntiEnthrallState)
			return 0 ;

		if (nMoney == 0) return 0;

		if (Player[nPlayerIndex].Earn(nMoney))
		{
			if (nMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
			{
				//脚本获得金钱日志
				LogEventParam logParam;
				logParam.event = log_event_script_add_money;
				logParam.param1 = Player[nPlayerIndex].GetGUID();
				logParam.param4 = nMoney;
				g_pLogSystem->Log(logParam);
			}
		}
	}
	return 0;	
}

// Fix By Rocker 2005.6.13
int LuaGetLevel(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L,1);
		return 1;
	}
	
	if ((Player[nPlayerIndex].m_nIndex <= 0)||(Player[nPlayerIndex].m_nIndex >= MAX_NPC))
	{
		Lua_PushNumber(L,1);
		return 1;
	}

	Lua_PushNumber(L, Npc[Player[nPlayerIndex].m_nIndex].m_Level);
	return 1;
}
#endif

#ifdef _SERVER

int LuaSetObjPropState(Lua_State *L)
{
	int  nParamNum = ( int ) Lua_GetTopIndex(L);
	int nState  = 1;
	
	if (nParamNum >= 1)
	{
		nState = (int)Lua_ValueToNumber(L,1);
		nState = (nState == 0)?0 : 1;
	}
	
	int nIndex = 0;
	if ((nIndex = ScriptGetObjIndex()) < 0) 
		return 0;
	
	Object[nIndex].SetState(nState);
	return 0;
}

// SetMissionValue(valueid, value)
// int LuaSetMissionValue(Lua_State * L)
// {
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex < 0) 
// 		return 0;
// 	
// 	int nParamCount = Lua_GetTopIndex(L);
// 	if (nParamCount < 2) 
// 		return 0;
// 	
// 	int nValueId = (int)Lua_ValueToNumber(L, 1);
// 	int nValue = (int)Lua_ValueToNumber(L, 2);
// 	
// 	if (nValueId  < 0)
// 		return 0;
// 	SubWorld[nSubWorldIndex].m_MissionArray.SetMissionValue(nValueId, nValue);
// 	return 0;
// }

// int LuaGetMissionValue(Lua_State * L)
// {
// 	int nResultValue = 0;
// 	int nSubWorldIndex = -1;
// 	int nParamCount = Lua_GetTopIndex(L);
// 	if (nParamCount < 1) 
// 		goto lab_getmissionvalue;
// 	
// 	nSubWorldIndex = ScriptGetSubWorldIndex();
// 	
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		int  nValueId = (int)Lua_ValueToNumber(L, 1);
// 		if (nValueId > 0)
// 			nResultValue = SubWorld[nSubWorldIndex].m_MissionArray.GetMissionValue(nValueId);
// 	}
// 	
// lab_getmissionvalue:
// 	Lua_PushNumber(L, nResultValue);
// 	return 1;
// }

// SetMissionValue(mapid/mapname, valueid, value)
// int LuaSetGlobalMissionValue(Lua_State * L)
// {
// 	int nParamCount = Lua_GetTopIndex(L);
// 	if (nParamCount < 2) 
// 		return 0;
// 	
// 	int nValueId = (int)Lua_ValueToNumber(L, 1);
// 	int nValue = (int)Lua_ValueToNumber(L, 2);
// 	
// 	if (nValueId  < 0)
// 		return 0;
// 	g_GlobalMissionArray.SetMissionValue(nValueId, nValue);
// 	return 0;
// }

// int LuaGetGlobalMissionValue(Lua_State * L)
// {
// 	int nResultValue = 0;
// 	int nValueId = 0;
// 	int nParamCount = Lua_GetTopIndex(L);
// 	if (nParamCount < 1) 
// 		goto lab_getglobalmissionvalue;
// 	nValueId = (int)Lua_ValueToNumber(L, 1);
// 	if (nValueId < 0)
// 		goto lab_getglobalmissionvalue;
// 	
// 	nResultValue = g_GlobalMissionArray.GetMissionValue(nValueId);
// 	
// lab_getglobalmissionvalue:
// 	Lua_PushNumber(L, nResultValue);
// 	return 1;
// }

//StartMission(missionid)
// int LuaInitMission(Lua_State * L)
// {
// 	if (Lua_GetTopIndex(L) < 1) 
// 		return 0;
// 	
// 	int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 	if (nMissionId < 0 )
// 		return 0;
// 	
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex < 0) 
// 		return 0;
// 	//int nPlayerIndex = GetPlayerIndex(L);
// 	
// 	//if (nPlayerIndex <= 0) 
// 		//return 0;
// 	
// 	KMission Mission;
// 	Mission.SetMissionId(nMissionId);
// 	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 	if (pMission)
// 	{
// 		_ASSERT(0);
// 		return 0;
// 	}
// 	
// 	pMission = SubWorld[nSubWorldIndex].m_MissionArray.Add();
// 	if (pMission)
// 	{
// 		pMission->m_MissionPlayer.Clear();
// 		pMission->SetMissionId(nMissionId);
// 		char szScript[MAX_PATH];
// 		g_MissionTabFile.GetString(nMissionId + 1, 2, "", szScript, MAX_PATH);
// 		if (szScript[0])
// 			//Player[nPlayerIndex].ExecuteScript(szScript, "InitMission", "");
// 			pMission->ExecuteScript(szScript, "InitMission", 0);
// 	}
// 	
// 	return 0;
// }

// int LuaRunMission(Lua_State * L)
// {
// 	if (Lua_GetTopIndex(L) < 1) 
// 		return 0;
// 	
// 	int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 	if (nMissionId < 0 )
// 		return 0;
// 	
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex < 0) 
// 		return 0;
// 
// 	KMission Mission;
// 	Mission.SetMissionId(nMissionId);
// 	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 	if (pMission)
// 	{
// 		char szScript[MAX_PATH];
// 		g_MissionTabFile.GetString(nMissionId + 1, 2, "", szScript, MAX_PATH);
// 		if (szScript[0])
// 			pMission->ExecuteScript(szScript, "RunMission", 0);
// 	}
// 	
// 	return 0;
// }
//CloseMission(missionId)
// int LuaCloseMission(Lua_State * L)
// {
// 	if (Lua_GetTopIndex(L) < 1) 
// 		return 0;
// 	
// 	int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 	if (nMissionId < 0 )
// 		return 0;
// 	
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex < 0) 
// 		return 0;
// 	KMission StopMission;
// 	StopMission.SetMissionId(nMissionId);
// 	KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&StopMission);
// 	if (pMission)
// 	{
// 		char szScript[MAX_PATH];
// 		g_MissionTabFile.GetString(nMissionId + 1, 2, "", szScript, MAX_PATH);
// 		if (szScript[0])
// 			pMission->ExecuteScript(szScript, "EndMission",0);
// 		pMission->StopMission();
// 		SubWorld[nSubWorldIndex].m_MissionArray.Remove(pMission);
// 		
// 	}
// 	return 0;
// }
//StopMissionTimer(missionid, timerid)
// int LuaStopMissionTimer(Lua_State * L)
// {
// 	if (Lua_GetTopIndex(L) < 2) 
// 		return 0;
// 	int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 	int nTimerId = (int)Lua_ValueToNumber(L, 2);
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	
// 	if (nMissionId < 0 || nTimerId < 0 ) 
// 		return 0;
// 	
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			KTimerTaskFun StopTimer;
// 			StopTimer.SetTimer(1, nTimerId);
// 			KTimerTaskFun * pTimer = pMission->m_cTimerTaskSet.GetData(&StopTimer);
// 			if (pTimer)
// 			{
// 				pTimer->CloseTimer();
// 				pMission->m_cTimerTaskSet.Remove(pTimer);
// 			}
// 		}
// 		
// 	}
// 	
// 	return 0;
// }

//StartMissionTimer(missionid, timerid, time)
// int LuaStartMissionTimer(Lua_State * L)
// {
// 	if (Lua_GetTopIndex(L) < 3) 
// 		return 0;
// 	int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 	int nTimerId = (int)Lua_ValueToNumber(L, 2);
// 	int nTimeInterval = (int)Lua_ValueToNumber(L, 3);
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	
// 	if (nMissionId < 0 || nTimerId < 0 || nTimeInterval < 0) 
// 		return 0;
// 	
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			KTimerTaskFun * pTimer = pMission->m_cTimerTaskSet.Add();
// 			if (pTimer)
// 			{
// 				pTimer->SetTimer(nTimeInterval, nTimerId);
// 			}
// 		}
// 		
// 	}
// 	return 0;
// }
//SetTempRev(worldid, x, y)
int LuaSetDeathRevivalPos(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex)) 
		return 0;
	int nParamCount = Lua_GetTopIndex(L);
	
	PLAYER_REVIVAL_POS * pTempRev = Player[nPlayerIndex].GetDeathRevivalPos();
	
	if (nParamCount == 3)
	{
		pTempRev->m_nSubWorldID  = (int) Lua_ValueToNumber(L, 1);
		pTempRev->m_nMpsX = (int) Lua_ValueToNumber(L, 2);
		pTempRev->m_nMpsY = (int) Lua_ValueToNumber(L, 3); 
	}
	else if (nParamCount == 1)
	{
		pTempRev->m_nSubWorldID = SubWorld[Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex].m_SubWorldID;
		POINT Pos;
		int nRevId = (int) Lua_ValueToNumber(L, 1);
		if (g_SubWorldSet.GetRevivalPosFromId(pTempRev->m_nSubWorldID, nRevId, &Pos))
		{
			pTempRev->m_ReviveID = nRevId;
			pTempRev->m_nMpsX = Pos.x;
			pTempRev->m_nMpsY = Pos.y;
		}
		else
		{
			pTempRev->m_nSubWorldID = defTRANSFER_PORT_ID;
			pTempRev->m_nMpsX = defTRANSFER_PORT_X;
			pTempRev->m_nMpsY = defTRANSFER_PORT_Y;
		}
	}
	else 
	{
		return 0;
	}
	
	return 0;
}

//AddMSPlayer(MissionId, PlayerIndex, groupid); / AddMSPlayer(MissionId, groupid)
// int LuaAddMissionPlayer(Lua_State * L)
// {
// 	int nParamCount = Lua_GetTopIndex(L);
// 	if (nParamCount < 2) 
// 		return 0;
// 	int nMissionId = 0;
// 	int nPlayerIndex = 0;
// 	int nGroupId = 0;
// 	if (nParamCount >=3)
// 	{
// 		nMissionId = (int)Lua_ValueToNumber(L,1);
// 		nPlayerIndex = (int )Lua_ValueToNumber(L,2);
// 		nGroupId = (int) Lua_ValueToNumber(L,3);
// 	}
// 	else
// 	{
// 		nMissionId = (int)Lua_ValueToNumber(L,1);
// 		nGroupId = (int) Lua_ValueToNumber(L,2);
// 		nPlayerIndex = ScriptGetPlayerIndex();
// 	}
// 	
// 	if (nMissionId < 0 || nPlayerIndex <= 0 || nGroupId <0)
// 		return 0;
// 	
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			pMission->AddPlayer(nPlayerIndex, Player[nPlayerIndex].m_dwID, nGroupId);
// 		}
// 	}
// 	return 0;
// }

//RemoveMSPlayer(MissionId, PlayerIndex, groupid)
// int LuaRemoveMissionPlayer(Lua_State * L)
// {
// 	int nParamCount = Lua_GetTopIndex(L);
// 	if (nParamCount < 2) 
// 		return 0;
// 	int nMissionId = 0;
// 	int nPlayerIndex = 0;
// 	int nGroupId = 0;
// 	if (nParamCount >=3)
// 	{
// 		nMissionId = (int)Lua_ValueToNumber(L,1);
// 		nPlayerIndex = (int )Lua_ValueToNumber(L,2);
// 		nGroupId = (int) Lua_ValueToNumber(L,3);
// 	}
// 	else
// 	{
// 		nMissionId = (int)Lua_ValueToNumber(L,1);
// 		nGroupId = (int) Lua_ValueToNumber(L,2);
// 		nPlayerIndex = ScriptGetPlayerIndex();
// 	}
// 	
// 	if (nMissionId < 0 || nPlayerIndex <= 0 || nGroupId <0)
// 		return 0;
// 	
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			pMission->RemovePlayer(nPlayerIndex);
// 			//pMission->AddPlayer(nPlayerIndex, Player[nPlayerIndex].m_dwID, nGroupId);
// 		}
// 	}
// 	return 0;
// }
//GetNextPlayer(mission, idx,group)
// int LuaGetNextPlayer(Lua_State * L)
// {
// 	unsigned long nPlayerIndex = 0;
// 	
// 	if (Lua_GetTopIndex(L) < 2)
// 	{
// 		Lua_PushNumber(L ,0);
// 		Lua_PushNumber(L, 0);
// 		return 2;
// 	}
// 	
// 	int nMissionId	= (int)Lua_ValueToNumber(L, 1);
// 	int nIdx		= (int)Lua_ValueToNumber(L, 2);
// 	int nGroup		= (int)Lua_ValueToNumber(L, 3);
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	int nResultIdx = 0;
// 	
// 	if (nMissionId < 0 || nIdx < 0 || nGroup < 0) 
// 		goto lab_getnextplayer;
// 	
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			nResultIdx = pMission->GetNextPlayer(nIdx, nGroup, nPlayerIndex);
// 		}
// 	}
// 	
// lab_getnextplayer:	
// 	Lua_PushNumber(L, nResultIdx);
// 	Lua_PushNumber(L, nPlayerIndex);
// 	return 2;
// }

//MSMsg2Group(missionid, string , group)
// int LuaMissionMsg2Group(Lua_State * L)
// {
// 	int nMissionId = (int)Lua_ValueToNumber(L,1);
// 	char * strMsg = (char *)Lua_ValueToString(L, 2);
// 	int	nGroupId = (int) Lua_ValueToNumber(L, 3);
// 	
// 	if (nMissionId < 0 || !strMsg || nGroupId <0)
// 		return 0;
// 	
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			pMission->Msg2Group(strMsg, nGroupId);
// 		}
// 	}
// 	
// 	return 0;
// }

//MSMsg2Group(missionid, string)
// int LuaMissionMsg2All(Lua_State * L)
// {
// 	int nMissionId = (int)Lua_ValueToNumber(L,1);
// 	char * strMsg = (char *)Lua_ValueToString(L, 2);
// 	
// 	if (nMissionId < 0 || !strMsg)
// 		return 0;
// 	
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			pMission->Msg2All(strMsg);
// 		}
// 	}
// 	
// 	return 0;
// }

//MSMsg2Group(missionid, string , group)
// int LuaMissionMsg2Player(Lua_State * L)
// {
// 	int nMissionId = (int)Lua_ValueToNumber(L,1);
// 	char * strMsg = (char *)Lua_ValueToString(L, 2);
// 	int	nPlayerIndex = (int) Lua_ValueToNumber(L, 3);
// 	
// 	if (nMissionId < 0 || !strMsg || nPlayerIndex <0)
// 		return 0;
// 	
// 	int nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			pMission->Msg2Group(strMsg, nPlayerIndex);
// 		}
// 	}
// 	
// 	return 0;
// }

// int LuaMissionPlayerCount(Lua_State * L)
// {
// 	int nParamCount = Lua_GetTopIndex(L);
// 	unsigned long ulCount = 0;
// 	int nMissionId = 0;
// 	int nGroupId = 0;
// 	int nSubWorldIndex = 0;
// 	if (nParamCount < 1) 
// 		goto lab_getmissionplayercount;
// 	
// 	if (nParamCount >= 2)
// 	{
// 		nMissionId = (int)Lua_ValueToNumber(L,1);
// 		nGroupId = (int) Lua_ValueToNumber(L,2);
// 	}
// 	else
// 	{
// 		nMissionId = (int)Lua_ValueToNumber(L,1);
// 	}
// 	
// 	if (nMissionId < 0 || nGroupId <0)
// 		goto lab_getmissionplayercount;
// 	
// 	nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			ulCount = pMission->GetGroupPlayerCount(nGroupId);
// 		}
// 	}
// 	
// lab_getmissionplayercount:
// 	Lua_PushNumber(L, ulCount);
// 	return 1;
// }


int LuaNpcIndexToPlayerIndex(Lua_State * L)
{
	int nResult = 0;
	int nNpcIndex = (int)Lua_ValueToNumber(L, 1);
	if (nNpcIndex <=  0 || nNpcIndex >= MAX_NPC)
		goto lab_npcindextoplayerindex;
	
	if (Npc[nNpcIndex].m_Index > 0 && Npc[nNpcIndex].IsPlayer())
	{
		if (Npc[nNpcIndex].GetPlayerIdx() > 0)
			nResult = Npc[nNpcIndex].GetPlayerIdx();
	}
	
lab_npcindextoplayerindex:
	Lua_PushNumber(L, nResult);
	return 1;
	
}
//  
// int LuaGetMissionPlayer_PlayerIndex(Lua_State * L)
// {
// 	unsigned long nResult = 0;
// 	int nSubWorldIndex = 0;
// 	if (Lua_GetTopIndex(L) < 2) 
// 		goto lab_getmissionplayer_npcindex;
// 	
// 	nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
// 		if (nMissionId < 0 || nDataIndex < 0)
// 			goto lab_getmissionplayer_npcindex;
// 		
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			nResult = pMission->GetMissionPlayer_PlayerIndex(nDataIndex);
// 		}
// 	}
// 	
// lab_getmissionplayer_npcindex:
// 	Lua_PushNumber(L,nResult);
// 	return 1;
// }

// int LuaGetMissionPlayer_DataIndex(Lua_State * L)
// {
// 	unsigned long nResult = 0;
// 	int nSubWorldIndex = 0;
// 	if (Lua_GetTopIndex(L) < 2) 
// 		goto lab_getmissionplayer_dataindex;
// 	
// 	nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 		int nPlayerIndex = (int)Lua_ValueToNumber(L, 2);
// 		if (nMissionId < 0 || nPlayerIndex < 0)
// 			goto lab_getmissionplayer_dataindex;
// 		
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			nResult = pMission->GetMissionPlayer_DataIndex(nPlayerIndex);
// 		}
// 	}
// 	
// lab_getmissionplayer_dataindex:
// 	Lua_PushNumber(L,nResult);
// 	return 1;
// }

//SetMPParam(missionid, nDidx, vid, v)
// int LuaSetMissionPlayerParam(Lua_State * L)
// {
// 	int nSubWorldIndex = 0;
// 	if (Lua_GetTopIndex(L) < 4) 
// 		return 0;
// 	
// 	nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
// 		int nParamId =	 (int)Lua_ValueToNumber(L ,3);
// 		int nValue =	 (int )Lua_ValueToNumber(L, 4);
// 		
// 		if (nMissionId < 0 || nDataIndex < 0 || nParamId > 2)
// 			return 0;
// 		
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			if (nParamId == 1)
// 				pMission->m_MissionPlayer.SetParam1(nDataIndex, nValue);
// 			else
// 				pMission->m_MissionPlayer.SetParam2(nDataIndex, nValue);
// 		}
// 	}
// 	return 0;
// }

// int LuaGetMissionPlayerParam(Lua_State * L)
// {
// 	int nResult = 0;
// 	int nSubWorldIndex = 0;
// 	if (Lua_GetTopIndex(L) < 3) 
// 		goto lab_getmissionplayerparam;
// 	
// 	nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 		int nDataIndex = (int)Lua_ValueToNumber(L, 2);
// 		int nParamId =	 (int)Lua_ValueToNumber(L ,3);
// 		
// 		if (nMissionId < 0 || nDataIndex < 0 || nParamId > 2)
// 			goto lab_getmissionplayerparam;
// 		
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			if (nParamId == 1)
// 				nResult = pMission->m_MissionPlayer.GetParam1(nDataIndex);
// 			else
// 				nResult = pMission->m_MissionPlayer.GetParam2(nDataIndex);
// 		}
// 	}
// lab_getmissionplayerparam:
// 	Lua_PushNumber(L, nResult);
// 	return 1;
// }

// int LuaGetPlayerMissionGroup(Lua_State * L)
// {
// 	int nResult = 0;
// 	int nSubWorldIndex = 0;
// 	if (Lua_GetTopIndex(L) < 2) 
// 		goto lab_getmissionplayergroup;
// 	
// 	nSubWorldIndex = ScriptGetSubWorldIndex();
// 	if (nSubWorldIndex >= 0) 
// 	{
// 		int nMissionId = (int)Lua_ValueToNumber(L, 1);
// 		int nNpcIndex = (int)Lua_ValueToNumber(L, 2);
// 		
// 		if (nMissionId < 0 || nNpcIndex < 0)
// 			goto lab_getmissionplayergroup;
// 		
// 		KMission Mission;
// 		Mission.SetMissionId(nMissionId);
// 		KMission * pMission = SubWorld[nSubWorldIndex].m_MissionArray.GetData(&Mission);
// 		if (pMission)
// 		{
// 			nResult = pMission->GetMissionPlayer_GroupId(nNpcIndex);
// 		}			
// 	}
// lab_getmissionplayergroup:
// 	Lua_PushNumber(L ,nResult);
// 	return 1;
// 	
// }

int LuaSetPlayerRevivalOptionWhenLogout(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex)) return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	int nType = (int)Lua_ValueToNumber(L, 1);
	
	if (nType)
		Player[nPlayerIndex].SetLoginType(1);
	else
		Player[nPlayerIndex].SetLoginType(0);
	
	return 0;
}



int LuaSetPlayerPKValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;	
	int nPKValue = (int)Lua_ValueToNumber(L,1);
	
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		return 0;
	Player[nPlayerIndex].SetPkValue(nPKValue);
	return 0;
}

int LuaGetPlayerPKValue(Lua_State * L)
{
	int nPKValue = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		goto lab_getplayerpkvalue;
	
	if (Player[nPlayerIndex].m_nIndex <= 0) 
		goto lab_getplayerpkvalue;
	nPKValue = Player[nPlayerIndex].GetPkValue();
	
lab_getplayerpkvalue:
	Lua_PushNumber(L, nPKValue);
	return 1;
}

int LuaGetGlobalValue(Lua_State * L)
{
	int nValue = 0;
	int nIndex = (int)Lua_ValueToNumber(L,1);

	if( nIndex < 0 || nIndex > MAX_TASK_VALUE_COUNT )
		goto lab_getplayerpkvalue;

	nValue = GetTaskGlobal( nIndex );
	
lab_getplayerpkvalue:
	Lua_PushNumber(L, nValue);
	return 1;
}

int LuaSetGlobalValue(Lua_State * L)
{
	int nIndex = (int)Lua_ValueToNumber(L,1);
	int nValue = (int)Lua_ValueToNumber(L,2);
	
	if( nIndex < 0 || nIndex > MAX_TASK_VALUE_COUNT )
		return 0;

	SetTaskGlobal( nIndex, nValue );
	
	return 0;
}

int LuaPlayerIndexToNpcIndex(Lua_State * L)
{
	int npcIndex = 0;
	if (Lua_GetTopIndex(L) == 1)
	{
		int playerIndex = Lua_ValueToNumber(L, 1);
		if ( IsValidPlayer(playerIndex))
		{
			npcIndex = Player[playerIndex].m_nIndex;
		}
	}
	Lua_PushNumber(L, npcIndex);
	return 1;
}

int LuaAddMoneyObj(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;

	int nX, nY;
	POINT	ptLocal;
	KMapPos	Pos;

	int nMoney = Lua_ValueToNumber(L, 1);
	if (nMoney <= 0)
		return 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nX, &nY);
	ptLocal.x = nX;
	ptLocal.y = nY;
	int nSubWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex;
	SubWorld[Npc[Player[nPlayerIndex].m_nIndex].m_SubWorldIndex].GetFreeObjPos(ptLocal);
	
	Pos.nSubWorld = nSubWorldIndex;
	SubWorld[nSubWorldIndex].Mps2Map(ptLocal.x, ptLocal.y, 
		&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
		&Pos.nOffX, &Pos.nOffY);
	
	int nObjIdx = ObjSet.AddMoneyObj(Pos, nMoney , -1);
	/*if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		Object[nObjIdx].SetItemBelong(-1);
	}
	*/
	return 0;
}

// Rocker 2004.7.28
int LuaThrowMoneyObj(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 4) 
		return 0;
	
	KMapPos	pos;
	pos.nOffX = 0;
	pos.nOffY = 0;
	pos.nSubWorld = g_SubWorldSet.SearchWorld((int)Lua_ValueToNumber(L, 1));
	if (pos.nSubWorld==-1)
		return 0;
	
	pos.nMapX = (int)Lua_ValueToNumber(L, 2);
	pos.nMapY = (int)Lua_ValueToNumber(L, 3);
	int nMoney = (int)Lua_ValueToNumber(L, 4);
	if (nMoney <= 0)
		return 0;
	
	POINT	ptLocal;
	
	ptLocal.x = pos.nMapX;
	ptLocal.y = pos.nMapY;
	SubWorld[pos.nSubWorld].GetFreeObjPos(ptLocal);
	
	SubWorld[pos.nSubWorld].Mps2Map(ptLocal.x, ptLocal.y, 
		&pos.nRegion, &pos.nMapX, &pos.nMapY, 
		&pos.nOffX, &pos.nOffY);
	
	int nObjIdx = ObjSet.AddMoneyObj(pos, nMoney,-1);
/*	if (nObjIdx > 0 && nObjIdx < MAX_OBJECT)
	{
		Object[nObjIdx].SetItemBelong(-1);
	}
*/
	return 0;
}
// Rocker 2004.7.28


// Add by Cooler -->
// 2005-3-15
#ifdef _SERVER
int LuaDoMarryEx(Lua_State * L)
{
	if(Lua_GetTopIndex(L) != 1)
	{
		return 0;
	}

	int nPlayerIndex = ScriptGetPlayerIndex();
	if(!IsValidPlayer(nPlayerIndex))
	{
		return 0;
	}

	const char* pMarryTarget = Lua_ValueToString(L, 1);
	if(!pMarryTarget)
	{
		return 0;
	}

//	KPlayerChat::STRINGLIST listMarry;
//	listMarry.push_back(Player[nPlayerIndex].m_PlayerName);
//	listMarry.push_back(pMarryTarget);
//	KPlayerChat::MakeMate(listMarry);

	return 0;
}

int LuaUnMarryEx(Lua_State * L)
{
	if(Lua_GetTopIndex(L) != 1)
	{
		return 0;
	}

	int nPlayerIndex = ScriptGetPlayerIndex();
	if(!IsValidPlayer(nPlayerIndex))
	{
		return 0;
	}

	const char* pUnMarryTarget = Lua_ValueToString(L, 1);
	if(!pUnMarryTarget)
	{
		return 0;
	}

//	KPlayerChat::UnMarry(nPlayerIndex, pUnMarryTarget);

	return 0;
}

int LuaIsMarried(Lua_State * L)
{
	return 0;
}

BOOL IsMarryCondition(int nPlayer1, int nPlayer2)
{
	return 0;
}

int LuaCanMarry(Lua_State * L)
{
	return 0;
}

#ifdef _SERVER
int LuaPlayAnimation(Lua_State * L)
{
	if (g_pServer == NULL)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}

	if (Lua_GetTopIndex(L) > 4)
	{
		Lua_PushNumber(L, 0);
		return 0;
	}

	Play_Animation data;
	data.Protocol		= s2c_byte_extend;
	data.ProtocolExtend = s2c_ex_protocol_play_animation;
	data.wProtocolSize	= sizeof(Play_Animation);

	int topIndex = Lua_GetTopIndex(L);
	int numList[4] = {0, 0, 0, 0};
	for (int i = 0; i < topIndex; ++i)
	{
		if (Lua_IsNumber(L, i + 1))
		{
			numList[i] = Lua_ValueToNumber(L, i + 1);
		}
	}

	data.m_AnimationId1	= numList[0];
	data.m_AnimationId2 = numList[1];
	data.m_AnimationId3 = numList[2];
	data.m_AnimationId4 = numList[3];

	for (int j = 1; j < MAX_PLAYER; ++j)
	{
		if (IsValidPlayer(j))
		{
			g_pServer->PackDataToClient(Player[j].GetNetConnectIdx(), &data, sizeof(data));
		}
		else
		{
			break;
		}
	}

	Lua_PushNumber(L, 1);
	return 0;
}
#endif

int LuaDoMarry(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
	{
		return 0;
	}

	int nPlayerIdx1 = INVALID_PLAYER_INDEX;
	if (Lua_IsNumber(L, 1))
	{
		nPlayerIdx1 = Lua_ValueToNumber(L, 1);
	}
	int nPlayerIdx2 = INVALID_PLAYER_INDEX;
	if (Lua_IsNumber(L, 2))
	{
		nPlayerIdx2 = Lua_ValueToNumber(L, 2);
	}

	bool isSucceed = ScriptMarriage(nPlayerIdx1, nPlayerIdx2);

	Lua_PushNumber(L, isSucceed);
	return 0;
}

int LuaUnMarry(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	int ret = 0;
	if (IsValidPlayer(nPlayerIndex))
	{
		if (Player[nPlayerIndex].UnMarry())
		{
			ret = 1;
		}
	}

	Lua_PushNumber(L, ret);
	return ret;
}

int LuaGetMarriedTimes(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	int ret = 0;
	if (IsValidPlayer(nPlayerIndex))
	{
		int marriedTimes = Player[nPlayerIndex].m_MarriageInfo.m_MarriedTimes;
		Lua_PushNumber(L, marriedTimes);
		ret = 1;
	}
	return ret;
}

int LuaGetMateName(Lua_State * L)
{

	return 0;
}

int LuaMarryGetTaskValue(Lua_State * L)
{
	return 0;
}

int LuaMarrySetTaskValue(Lua_State * L)
{
	return 0;
}

int LuaGetNameID(Lua_State * L)
{
   	if(Lua_GetTopIndex(L) != 0)
	{
		Lua_PushNil(L);
		return 1;
	}

	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNil(L);
		return 1;
	}

	Lua_PushNumber(L, (int)Player[nPlayerIndex].m_dwID);
	
	return 1;
}

int LuaDoMasterPREx(Lua_State * L)
{
	if(Lua_GetTopIndex(L) != 1)
	{
		return 0;
	}

	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		return 0;
	}

	const char* pTarget = Lua_ValueToString(L, 1);
	if(!pTarget)
	{
		return 0;
	}

//	KPlayerChat::MakeMasterPR(nPlayerIndex, pTarget);

	return 0;
}

int LuaUnMasterPREx(Lua_State * L)
{
	if(Lua_GetTopIndex(L) != 1)
	{
		return 0;
	}

	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		return 0;
	}

	const char* pTarget = Lua_ValueToString(L, 1);
	if(!pTarget)
	{
		return 0;
	}

//	KPlayerChat::UnMasterPR(nPlayerIndex, pTarget);

	return 0;
}

BOOL IsDoMasterPRCondition(int nPlayer1, int nPlayer2)
{
	if( nPlayer1 > 0 && nPlayer1 < MAX_PLAYER && 
		nPlayer2 > 0 && nPlayer2 < MAX_PLAYER )
	{
		int nMasterPlayer = -1;
		int nPrenticePlayer = -1;

		if(Npc[Player[nPlayer1].m_nIndex].m_Level >= 50)
		{
			nMasterPlayer = nPlayer1;
		}
		else if(Npc[Player[nPlayer1].m_nIndex].m_Level < 30)
		{
			nPrenticePlayer = nPlayer1;
		}

		if(Npc[Player[nPlayer2].m_nIndex].m_Level >= 50)
		{
			nMasterPlayer = nPlayer2;
		}
		else if(Npc[Player[nPlayer2].m_nIndex].m_Level < 30)
		{
			nPrenticePlayer = nPlayer2;
		}

		if(nMasterPlayer != -1 && nPrenticePlayer != -1 && 
			!Player[nPrenticePlayer].IsHaveMaster() && 
			Player[nMasterPlayer].GetPrenticeCount() < 3)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL IsMasterPR(int nPlayer1, int nPlayer2)
{
	if( nPlayer1 > 0 && nPlayer1 < MAX_PLAYER && 
		nPlayer2 > 0 && nPlayer2 < MAX_PLAYER )
	{
		int nMasterPlayer = -1;
		int nPrenticePlayer = -1;

		if(Npc[Player[nPlayer1].m_nIndex].m_Level >= 50)
		{
			nMasterPlayer = nPlayer1;
		}
		else if(Npc[Player[nPlayer1].m_nIndex].m_Level >= 30)
		{
			nPrenticePlayer = nPlayer1;
		}

		if(Npc[Player[nPlayer2].m_nIndex].m_Level >= 50)
		{
			nMasterPlayer = nPlayer2;
		}
		else if(Npc[Player[nPlayer2].m_nIndex].m_Level >= 30)
		{
			nPrenticePlayer = nPlayer2;
		}

		if(nMasterPlayer != -1 && nPrenticePlayer != -1 && 
			strcmp(Player[nPrenticePlayer].GetMasterName(), 
				Player[nMasterPlayer].m_PlayerName) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

int LuaGetWeekDay(Lua_State * L)
{
	if(Lua_GetTopIndex(L) != 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	time_t ltime;
	tm tmStruct;
	
	//time(&ltime);	
	ltime = UNIX_TMIE_STAMP;
	tmStruct = *localtime(&ltime);
	int nWeekDay = tmStruct.tm_wday;
	if(nWeekDay == 0)
	{
		nWeekDay = 7;
	}
	Lua_PushNumber(L, nWeekDay);

	return 1;
}

int LuaReplaceBoxPwd(Lua_State * L)
{
//	if(Lua_GetTopIndex(L) != 0)
//	{
//		return 0;
//	}
//
//	int nPlayerIndex = ScriptGetPlayerIndex();
//	if (!IsValidPlayer(nPlayerIndex))
//	{
//		return 0;
//	}
//
//	BYTE byOpenFlag = s2c_replaceboxpwd;
//
//	if (g_pServer != NULL)
//		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, 
//		&byOpenFlag, sizeof(BYTE));


	return 0;
}

int LuaCDKeyChangeBox(Lua_State * L)
{
//	if(Lua_GetTopIndex(L) != 0)
//	{
//		return 0;
//	}
//
//	int nPlayerIndex = ScriptGetPlayerIndex();
//	if (!IsValidPlayer(nPlayerIndex))
//	{
//		return 0;
//	}
//
//	BYTE byOpenFlag = s2c_cdkeychangebox;
//	if (g_pServer != NULL)
//		g_pServer->PackDataToClient(Player[nPlayerIndex].m_nNetConnectIdx, 
//		&byOpenFlag, sizeof(BYTE));


	return 0;
}


#endif
// End add by Cooler <--


int LuaPrintNetStat(Lua_State * L)
{
	BYTE byOpenFlag = 64;
	if (g_pServer != NULL)
		g_pServer->PackDataToClient(0, 
		&byOpenFlag, sizeof(BYTE));

	return 0;
}

int LuaGMReply(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 0)
	{
		return 0;
	}

	int playerIndex = ScriptGetPlayerIndex();
	if(IsValidPlayer(playerIndex))
	{
		Player[playerIndex].GetGMReply();
	}
	
	return 0;
}

int LuaAddPlusPoint(Lua_State * L)
{
	int nPlayerIndex = 0;
	int plusPoint = 0;
	int plusPointType = 0;

	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	plusPointType	= Lua_ValueToNumber(L, 1);
	plusPoint		= Lua_ValueToNumber(L, 2);
	
	if (plusPoint <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	bool nResult = Player[nPlayerIndex].AddPlusPoint(plusPointType, plusPoint);
	
	Lua_PushNumber(L, nResult ? 1 : 0);
	return 1;
}

int LuaPayPlusPoint(Lua_State * L)
{
	int nPlayerIndex = 0;
	int plusPoint = 0;
	int plusPointType = 0;
	
	if (Lua_GetTopIndex(L) < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	plusPointType	= Lua_ValueToNumber(L, 1);
	plusPoint		= Lua_ValueToNumber(L, 2);
	
	if (plusPoint <= 0)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	bool nResult = Player[nPlayerIndex].DecPlusPoint(plusPointType, plusPoint);
	
	Lua_PushNumber(L, nResult ? 1 : 0);
	return 1;
}

int LuaGetPlusPoint(Lua_State * L)
{
	int nPlayerIndex = 0;
	int plusPointType = 0;
	
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	plusPointType	= Lua_ValueToNumber(L, 1);
	
	DWORD nResult = Player[nPlayerIndex].GetPlusPoint(plusPointType);
	
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaGetPlusPointRecord(Lua_State * L)
{
	int nPlayerIndex = 0;
	int plusPointType = 0;
	
	if (Lua_GetTopIndex(L) < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	plusPointType	= Lua_ValueToNumber(L, 1);
	
	DWORD nResult = Player[nPlayerIndex].GetPlusPointRecord(plusPointType);
	
	Lua_PushNumber(L, nResult);
	return 1;
}

//AddExtPoint
//add by zuolizhi
int LuaAddExtPoint(Lua_State * L)
{
	int nResult = 0;
	int nPay = 0;
	int nPlayerIndex = 0;
	int nPayType = 0;
	if (Lua_GetTopIndex(L) < 2)
		goto lab_payextpoint;
	
	nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		goto lab_payextpoint;
	
	nPayType = Lua_ValueToNumber(L, 1);
	nPay = Lua_ValueToNumber(L, 2);
	
	if (nPay < 0)
		goto lab_payextpoint;
	nResult = Player[nPlayerIndex].AddExtPoint(nPayType, nPay);
	
lab_payextpoint:
	Lua_PushNumber(L, nResult);
	return 1;
}

//PayExtPoint
int LuaPayExtPoint(Lua_State * L)
{
	int nResult = 0;
	int nPay = 0;
	int nPlayerIndex = 0;
	int nPayType = 0;
	if (Lua_GetTopIndex(L) < 2)
		goto lab_payextpoint;
	
	nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		goto lab_payextpoint;
	
	nPayType = Lua_ValueToNumber(L, 1);
	nPay = Lua_ValueToNumber(L, 2);

	if (nPay < 0)
		goto lab_payextpoint;
	nResult = Player[nPlayerIndex].PayExtPoint(nPayType, nPay);

lab_payextpoint:
	Lua_PushNumber(L, nResult);
	return 1;
}

//GetExtPoint
int LuaGetExtPoint(Lua_State * L)
{
	int nResult = 0;
	int nPlayerIndex = 0;
	int nPayType = 0;
	
	if (Lua_GetTopIndex(L) < 1)
		goto lab_getextpoint;

	nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		goto lab_getextpoint;
	
	nPayType = Lua_ValueToNumber(L, 1);
	nResult = Player[nPlayerIndex].GetExtPoint(nPayType);
	
lab_getextpoint:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaSetExtPoint(Lua_State * L)
{
	int nResult      = 0;
	int nPay         = 0;
	int nPlayerIndex = 0;
	int nPayType     = 0;

	if (Lua_GetTopIndex(L) < 2)
		goto lab_payextpoint;
	
	nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		goto lab_payextpoint;
	
	nPayType  = Lua_ValueToNumber(L, 1);
	nPay      = Lua_ValueToNumber(L, 2);

	nResult = Player[nPlayerIndex].SetExtPointImmediately(nPayType, nPay);
	
lab_payextpoint:
	Lua_PushNumber(L, nResult);
	return 1;
}

int LuaActivatePresent( Lua_State * L )
{
	int nResult       = 0;
	int nPlayerIndex  = 0;
	const char * szPresentCode = NULL;

	if (Lua_GetTopIndex(L)<1)
		goto lab_ActivatePresent;

	nPlayerIndex      = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
		goto lab_ActivatePresent;

	szPresentCode = Lua_ValueToString(L,1);
	nResult = Player[nPlayerIndex].RequireActivatePresent(szPresentCode);

lab_ActivatePresent:
	Lua_PushNumber(L,nResult);
	return 1;
}

int LuaGetAccountName(Lua_State * L)
{	int nPlayerIndex = ScriptGetPlayerIndex();
	char szDesMsg[200];
	szDesMsg[0] = 0;
	KPlayer * pPlayer = NULL;
	if (!IsValidPlayer(nPlayerIndex))
		goto lab_getplayerip;
	pPlayer = &Player[nPlayerIndex];
	strcpy(szDesMsg, pPlayer->m_AccoutName);

lab_getplayerip:
	Lua_PushString(L, szDesMsg);
	return 1;
}

int LuaSetPKFlag(Lua_State * L)
{
// 	if (Lua_GetTopIndex(L) < 1) 
// 		return 0;
// 	
// 	int nPlayerIndex = GetPlayerIndex(L);
// 	if (IsValidPlayer(nPlayerIndex))
// 	{
// 		int nState = Lua_ValueToNumber(L, 1);
// 		Player[nPlayerIndex].m_cPK.SetNormalPKState(nState, TRUE);
// 	}
	return 0;
}


int LuaSetDeathPunish(Lua_State * L)
{
	if (Lua_GetTopIndex(L) < 1) 
		return 0;

//--> Rocker 2005/06/15
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		int nState = Lua_ValueToNumber(L, 1);
		if (nState == 0)
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = 1;
		else
			Npc[Player[nPlayerIndex].m_nIndex].m_nCurPKPunishState = 0;
	}
//<-- End
	return 0;
}

int LuaGetTotalTimeInGame(Lua_State * L)
{
	int nGameTime = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		nGameTime = Player[nPlayerIndex].GetOnlineTime();
	}
	Lua_PushNumber(L, nGameTime);
	return 1;
}


//UseSilver(type, usetype, num)
int LuaUseSilver(Lua_State* L)
{
	if (Lua_GetTopIndex(L) < 3)
	{
		return 0;
	}
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		int nType = Lua_ValueToNumber(L, 1);
		int nUseType = Lua_ValueToNumber(L, 2);
		int nUseNum = Lua_ValueToNumber(L, 3);
		Player[nPlayerIndex].UseSilver(nType, nUseType, nUseNum);
	}
		
	return 0;
}

int Lua_GetIBMoneyMax( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);

	int nMoneyType		= jinshanbi;
	

	if (nParamNum < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	else
	{
		nMoneyType		= (int)Lua_ValueToNumber(L, 1);
	}

	if ( nMoneyType <= jinshanbi && nMoneyType >= money_type_count )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nMin = 0;
	int nMax = 0; 
	Player[nPlayerIndex].GetIBMoneySize( (MoneyType)nMoneyType, nMin, nMax );

	Lua_PushNumber(L, nMax);
	return 1;
}

int Lua_ChangeIBMoneyMax( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);

	int nMoneyType		= jinshanbi;
	int nMoneyMax		= 0;		

	if (nParamNum < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	else
	{
		nMoneyType		= (int)Lua_ValueToNumber(L, 1);
		nMoneyMax			= (int)Lua_ValueToNumber(L, 2);
	}

	if ( nMoneyType <= jinshanbi && nMoneyType >= money_type_count )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	int nMin = 0;
	int nMax = 0; 
	Player[nPlayerIndex].GetIBMoneySize( (MoneyType)nMoneyType, nMin, nMax );
	Player[nPlayerIndex].SetIBMoneySize( (MoneyType)nMoneyType, nMin, nMoneyMax );
	Player[nPlayerIndex].SaveIBData();

	Lua_PushNumber(L, 1);
	return 1;
}

int Lua_GetItemStep( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	unsigned int nItemIndex = ScriptGetItemIndex();
	
	if ( nItemIndex == -1  )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	Lua_PushNumber(L, Item[nItemIndex].GetStep());
	return 1;
}

int Lua_IsItemMarkerPos( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	unsigned int nItemIndex = ScriptGetItemIndex();
	
	if ( nItemIndex == -1  )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}	
	
	int mapID = 0;
	int mapX = 0;
	int mapY = 0;
	
	Item[nItemIndex].GetPosInfo( mapID, mapX, mapY );
	if ( mapID == 0 && mapX == 0 && mapY == 0 )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	if ( mapID == -1 && mapX == -1 && mapY == -1 )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	
	Lua_PushNumber(L, 1);
	return 1;
}

int Lua_ItemMarkerPos( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	unsigned int nItemIndex = ScriptGetItemIndex();
	
	if ( nItemIndex == -1  )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}


	int nMap = 0;
	int nX = 0;
	int nY = 0;

	int nNpcIndex = Player[nPlayerIndex].GetNpcIndex();
	if ( !IsValidNpc( nNpcIndex ) )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	if (Npc[nNpcIndex].m_SubWorldIndex < 0 || Npc[nNpcIndex].m_SubWorldIndex >= MAX_SUBWORLD )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	nMap = SubWorld[Npc[nNpcIndex].m_SubWorldIndex].GetWorldTemplateId();
	Npc[nNpcIndex].GetMpsPos(&nX, &nY);

	Item[nItemIndex].SetPosInfo( nMap, nX, nY );
	Item[nItemIndex].SetStep( Item[nItemIndex].GetStep() + 1 );
	Item[nItemIndex].SyncAttribute(item_attr_mapid, Player[nPlayerIndex].GetNetConnectIdx() );
	Item[nItemIndex].SyncAttribute(item_attr_mapx, Player[nPlayerIndex].GetNetConnectIdx() );		
	Item[nItemIndex].SyncAttribute(item_attr_mapy, Player[nPlayerIndex].GetNetConnectIdx() );
	
	Lua_PushNumber(L, 0);
	return 1;
}

int Lua_ItemGoToPos( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	unsigned int nItemIndex = ScriptGetItemIndex();
	
	if ( nItemIndex == -1  )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int mapID = 0;
	int mapX = 0;
	int mapY = 0;

	Item[nItemIndex].GetPosInfo( mapID, mapX, mapY );

	int nNpcIndex = Player[nPlayerIndex].GetNpcIndex();
	if ( !IsValidNpc( nNpcIndex ) )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	Npc[nNpcIndex].ChangeWorld(mapID, mapX, mapY );
	Item[nItemIndex].SetStep( Item[nItemIndex].GetStep() + 1 );
	Lua_PushNumber(L, 0);
	return 1;

}

int Lua_ItemClearPos( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	unsigned int nItemIndex = ScriptGetItemIndex();
	
	if ( nItemIndex == -1  )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nNpcIndex = Player[nPlayerIndex].GetNpcIndex();
	if ( !IsValidNpc( nNpcIndex ) )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	Item[nItemIndex].SetPosInfo( 0, 0, 0 );
	Item[nItemIndex].SyncAttribute(item_attr_mapid, Player[nPlayerIndex].GetNetConnectIdx() );
	Item[nItemIndex].SyncAttribute(item_attr_mapx, Player[nPlayerIndex].GetNetConnectIdx() );		
	Item[nItemIndex].SyncAttribute(item_attr_mapy, Player[nPlayerIndex].GetNetConnectIdx() );

	Lua_PushNumber(L, 0);
	return 1;

}

int Lua_GetIBItemBuyDate( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	unsigned int nItemIndex = ScriptGetItemIndex();
	
	if ( nItemIndex == -1  )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nNpcIndex = Player[nPlayerIndex].GetNpcIndex();
	if ( !IsValidNpc( nNpcIndex ) )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	DWORD dwBuyData = Item[nItemIndex].GetIBBuyData();
	
	Lua_PushNumber(L, dwBuyData);
	return 1;

}

int Lua_SetIBItemUseCount( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	unsigned int nItemIndex = ScriptGetItemIndex();
	
	if ( nItemIndex == -1  )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);
	int nUseCount = 0;
	if (nParamNum < 1)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	else
	{
		nUseCount		= (int)Lua_ValueToNumber(L, 1);
	}

	Item[nItemIndex].SetIBUseCount( nUseCount );

	Lua_PushNumber(L, 1);
	return 1;
}

int Lua_GetIBItemUseCount( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	unsigned int nItemIndex = ScriptGetItemIndex();
	
	if ( nItemIndex == -1  )
	{
		Lua_PushNumber(L, 0);
		return 1;
	}	

	Lua_PushNumber(L, Item[nItemIndex].GetIBUseCount());
	return 1;
}

int Lua_AddIBMoney( Lua_State* L )
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);

	int nMoneyType		= jinshanbi;
	int nMoney			= 0;		

	if (nParamNum < 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	else
	{
		nMoneyType		= (int)Lua_ValueToNumber(L, 1);
		nMoney			= (int)Lua_ValueToNumber(L, 2);
	}

	Player[nPlayerIndex].SetIBPoint( (MoneyType)nMoneyType, nMoney );
	if ( nMoneyType == creditpoint )
	{
		KIBLog::getSingleton().AddIBMoney( gm_creditpoint_add, nPlayerIndex, nMoney);
	}
	
	if ( nMoneyType == point )
	{
		KIBLog::getSingleton().AddIBMoney( gm_point_add, nPlayerIndex, nMoney);
	}		

	Lua_PushNumber(L, 1);
	return 1;
}

int Lua_CostTongbao(Lua_State* L)
{
	unsigned int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L, 0);
		return 1;
	}

	int nParamNum = Lua_GetTopIndex(L);

	int nMoneyType = jinshanbi;
	int nItemClass = 0;		
	int nDetailType = 0;
	int nParticularType = 0;
	int nLevel = 0;
	int nPrice = 0;

	if (nParamNum != 2)
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
	else
	{
		nPrice		= (int)Lua_ValueToNumber(L, 1);
		nItemClass		= item_ib;
		nDetailType		= (int)Lua_ValueToNumber(L, 2);
		nParticularType	= 0;
		nLevel			= 0;		
	}

	KItem tmpItem;
	if ( g_ItemGen.Gen_Item( nItemClass, nDetailType, nParticularType, nLevel, 1, &tmpItem ) && 
		nPrice > 0 )
	{
		IBBuy_Param ibbuy_param;
		ibbuy_param.eMoneyType		= (MoneyType)nMoneyType;
		ibbuy_param.nPrice			= nPrice;
		ibbuy_param.eIBItemType		= (enumIBItemType)tmpItem.GetIBItemType();
		ibbuy_param.nItemGenre		= tmpItem.GetGenre();
		ibbuy_param.nItemDetail		= tmpItem.GetDetailType();
		ibbuy_param.nItemParticular	= tmpItem.GetParticular();
		ibbuy_param.nItemCount		= 1;
		ibbuy_param.dwOverdueTime	= tmpItem.GetIBAvailabilityTime();
		ibbuy_param.bOnceItem		= 1;

		int nErr = Player[nPlayerIndex].BuyIBItem( ibbuy_param );
		if ( nErr != wait_dec_ret )
		{
			Lua_PushNumber(L, 0);
			return 1;
		}
		else
		{
			Lua_PushNumber(L, 1);
			return 1;
		}
	}
	else
	{
		Lua_PushNumber(L, 0);
		return 1;
	}
}

int LuaIBTest(Lua_State* L)
{
	/*
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		int nType = Lua_ValueToNumber(L, 1);

		if( nType )
//			Player[nPlayerIndex].BuyIBItem( );
		else
//			Player[nPlayerIndex].UseIBItem( );
	}
	//*/
	
	return 0;
}

int LuaIBShopLoadShelf(Lua_State* L)
{
	if(Lua_GetTopIndex(L) != 1)
		return 0;

	if(!Lua_IsNumber(L, 1))
		return 0;

	int nShopIdx = Lua_ValueToNumber(L, 1);
	IBCenter_S::Singleton().LoadIBShopFromDBReq(nShopIdx);
	return 0;
}

int LuaIBShopLoadGoods(Lua_State* L)
{
	if(Lua_GetTopIndex(L) != 2)
		return 0;

	if(!Lua_IsNumber(L, 1) || !Lua_IsNumber(L, 2))
		return 0;

	int nShopIdx = Lua_ValueToNumber(L, 1);
	int nShelfIdx = Lua_ValueToNumber(L, 2);

	IBCenter_S::Singleton().LoadIBShopItemFromDBReq(nShopIdx, nShelfIdx);
	return 0;
}

int LuaIBShopLoadPanel(Lua_State* L)
{
	IBCenter_S::Singleton().LoadPanelFromDBReq();
	return 0;
}

int LuaIBShopLoadStyle(Lua_State* L)
{
	IBCenter_S::Singleton().LoadContentStyleFromDBReq();
	return 0;
}

#endif


// Add by Cooler 2004-7-5
// Begin -->
#ifdef _SERVER
int LuaAddCredit(Lua_State * L)
{
	return 0;
}

int LuaDecCredit(Lua_State * L)
{
	return 0;
}
#endif
// End <--


// Add by Cooler 2004-7-20
// Begin -->
#ifdef _SERVER
int LuaGetCredit(Lua_State * L)
{	
	return 0;
}

int LuaMultiCredit(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if(!IsValidPlayer(nPlayerIndex))
	{
		return 0;
	}

	int nParamCount = Lua_GetTopIndex(L);
	if(nParamCount < 2)
	{
		return 0;
	}

	TEMPADDSTATUSINFO tagTempAdd;
	tagTempAdd.nAddValue = (int)Lua_ValueToNumber(L, 1);
	tagTempAdd.nExistTime = (int)Lua_ValueToNumber(L, 2);

	Player[nPlayerIndex].
		SetTempAddStatus(enTempAddType_Credit, tagTempAdd);

	return 0;
}
#endif
// End <--


// Add by Cooler 2004-5-18
// Begin -->
int LuaGetBoxSize(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();

	if(IsValidPlayer(nPlayerIndex))
	{
		int nBoxIndex = (int)Lua_ValueToNumber(L, 1);
		int nBoxSizeType = (int)Lua_ValueToNumber(L, 2);

		Lua_PushNumber(L, Player[nPlayerIndex].
			m_ItemList.GetBoxSize((INVENTORY_ROOM)nBoxIndex, 
				(enBOXSIZETYPE)nBoxSizeType));
	} 
	else
	{
		Lua_PushNil(L);
	}

	return 1;
}
// End <--


// Added By Rocker 2004.7.16
int luaInputUI(Lua_State * L)
{
	char * strMain  = NULL;
	int nMainInfo = 0;
	int nDataType = 0;
	int nOptionNum = 0;
	char * pContent = NULL;
	
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex )) return 0;
	Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	
	int nParamNum = Lua_GetTopIndex(L);
	if (nParamNum < 2) return 0;
	
	if (Lua_IsNumber(L,2))
	{
		nOptionNum = (int)Lua_ValueToNumber(L,2);
	}
	else 
	{
		_ASSERT(0);
		return 0;
	}
	
	if  (Lua_IsNumber(L,1))
	{
		nMainInfo = (int)Lua_ValueToNumber(L,1);
		nDataType = 1 ;
	}
	else if (Lua_IsString(L, 1)) 	//检查主信息是字符串还是字符串标识号
	{
		strMain = (char *)Lua_ValueToString(L, 1);
		nDataType = 0 ;
	}
	else
		return 0;
	
	BOOL bStringTab = FALSE;//标识传进来的选项数据存放在一个数组中，还是许多字符串里
	
	if (Lua_IsString(L,3))
		bStringTab = FALSE;
	else if (Lua_IsTable(L, 3))
	{
		bStringTab = TRUE;
	}
	else 
	{if (nOptionNum > 0)  return 0;
	}
	
	if (bStringTab == FALSE)
	{
		//获得实际传入的选项的个数
		if (nOptionNum > nParamNum - 2) nOptionNum = nParamNum - 2;
	}
	
	if (nOptionNum > 2)	// 回答的响应参数不能超过2个
		return 0;

	if (nOptionNum > MAX_ANSWERNUM) 
		nOptionNum = MAX_ANSWERNUM;
	
	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bUIId = UI_INPUTDIALOG;
	UiInfo.m_bParam1 = nDataType;//主信息的类型，字符串(0)或数字(1)
	//-------> Ray [Luoliang] 2005-5-25
	UiInfo.m_bParam2 = enMerchant;
	//<------- End [Ray]
	UiInfo.m_bOptionNum = nOptionNum;
	UiInfo.m_nOperateType = SCRIPTACTION_UISHOW;
	char szContent[MAX_CONTENT_LEN];
	
	//主信息为字符串
	if (nDataType == 0)
	{
		if (strMain)
			sprintf(szContent, "%s", strMain);
		pContent = szContent;
	}
	else if (nDataType == 1) //主信息为数字标识
	{
		*(int *)szContent = nMainInfo;
		pContent = szContent + sizeof(int);
		*pContent = 0;
	}
	
	Player[nPlayerIndex].m_nAvailableAnswerNum = nOptionNum;		
	
	for (int i  = 0; i < nOptionNum; i ++)
	{	
		char  pAnswer[100];
		pAnswer[0] = 0;
		
		if (bStringTab)
		{
			Lua_PushNumber(L, i + 1);
			Lua_RawGet(L, 3);
			char * pszString = (char *)Lua_ValueToString(L, Lua_GetTopIndex(L));
			if (pszString)
			{
				g_StrCpyLen(pAnswer, pszString, 100);
			}
		}
		else 
		{
			char * pszString = (char *)Lua_ValueToString(L, i + 3);
			if (pszString)
				g_StrCpyLen(pAnswer, pszString, 100);
		}
		
		char * pFunName = strstr(pAnswer, "/");
		
		if (pFunName)
		{
			g_StrCpyLen(Player[nPlayerIndex].m_szTaskAnswerFun[i], pFunName + 1, sizeof(Player[nPlayerIndex].m_szTaskAnswerFun[0]));
			*pFunName = 0;
			sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
		else 
		{
			strcpy_const(Player[nPlayerIndex].m_szTaskAnswerFun[i], "main");
			sprintf(pContent, "%s|%s", pContent, pAnswer);
		}
	}

	if (strlen(pContent) >= MAX_SCIRPTACTION_BUFFERNUM - 4)
	{
		return 0;
	}

	if (nDataType == 0)
		UiInfo.m_nBufferLen  = strlen(pContent);
	else 
		UiInfo.m_nBufferLen = strlen(pContent) + sizeof(int);
	
	memcpy(UiInfo.m_pContent, szContent, sizeof(UiInfo.m_pContent));
	
#ifndef _SERVER
	UiInfo.m_bParam2 = 0;
#else
	UiInfo.m_bParam2 = 1;
#endif
	
	if (nOptionNum == 0)
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = false;
	}
	else
	{
		Player[nPlayerIndex].m_bWaitingPlayerFeedBack = true;
	}
	
	Player[nPlayerIndex].DoScriptAction(&UiInfo);
	return 0;
}
// Added End


// lixuewu 2004.07.12 变身
#ifdef _SERVER
int LuaPolyMorph(Lua_State * L)
{
	//add by zuolizhi
	int nPolyMorphRet = 0;
	//
	const int ParamCount = Lua_GetTopIndex(L);
	if (ParamCount >= 5)
	{
		const int nPlayerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(nPlayerIndex))
		{
			const int nMorphType = Lua_ValueToNumber(L, 1);
			const unsigned int nCanCast = Lua_ValueToNumber(L, 2);
			const unsigned int nSafeGuard = Lua_ValueToNumber(L, 3);
			const int nMoveSpeed =  Lua_ValueToNumber(L, 4);
			const unsigned int nTime = Lua_ValueToNumber(L, 5);
			nPolyMorphRet =	
			Npc[Player[nPlayerIndex].m_nIndex].PolyMorph(nMorphType, nCanCast, nSafeGuard, nMoveSpeed, nTime);
			Npc[Player[nPlayerIndex].m_nIndex].m_bSaveMorphType = TRUE;
			if (ParamCount > 5) 
			{
				Npc[Player[nPlayerIndex].m_nIndex].m_bSaveMorphType =  Lua_ValueToNumber(L, 6);
				//--> Rocker 2005/07/13 增加变身参数，变色
				if (ParamCount > 6)
				{
					Player[nPlayerIndex].m_btMorphHue = Lua_ValueToNumber(L, 7);
					Player[nPlayerIndex].m_bMorphSendHue = false;
				}
				//<-- End
			}
		}
	}

	if( nPolyMorphRet != TRUE )
	{
		Lua_PushNumber(L, 2);
		return 1;
	}
	else
		return 0;
}
#include "buff_man.h"

int LuaAddBuff( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();

	if (IsValidPlayer(nPlayerIndex))
	{
		int nIndex = Player[nPlayerIndex].m_nIndex;
		BuffMgr& BMgr = BuffMgr::Singleton();	
		const int nBuffTempID = Lua_ValueToNumber(L, 1);

		unsigned long ulID = 
		BMgr.AddNpcBuff( 
			nIndex, 
			nIndex, 
			nBuffTempID );

		Lua_PushNumber(L, ulID);
		return 1;
	}

	Lua_PushNumber(L, -1);
	return 1;
}

int LuaAddBuffByID( Lua_State* L )
{

	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcID = Lua_ValueToNumber(L, 1);
	
	int nNpcIndex = NpcSet.SearchID( nNpcID );

	if ( nNpcIndex > 0 && IsValidPlayer(nPlayerIndex))
	{
		int nIndex = Player[nPlayerIndex].m_nIndex;
		BuffMgr& BMgr = BuffMgr::Singleton();	
		const int nBuffTempID = Lua_ValueToNumber(L, 2);
		
		unsigned long ulID = 
		BMgr.AddNpcBuff( 
			nIndex, 
			nNpcIndex, 
			nBuffTempID );

		Lua_PushNumber(L, ulID);
		return 1;
	}
	
	Lua_PushNumber(L, -1);
	return 1;
}

int LuaIsHaveBuff( Lua_State* L )
{
	const int nNpcIdx = Lua_ValueToNumber(L, 1);	
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton();	
		const int nBuffTempID = Lua_ValueToNumber(L, 2);
		
		int nRet = BMgr.IsHaveBuff( nNpcIdx, nBuffTempID );

		Lua_PushNumber(L, nRet);
		return 1;
		
	}
	
	Lua_PushNumber(L, 0);
	return 1;
}

//<------------[Add by wsh]
int LuaAddLordBuff(Lua_State * L)
{
	int nTopIndex = Lua_GetTopIndex(L);
	
	if (nTopIndex != 2 && nTopIndex != 3)
		return 0;
	
	int nBuffId        = Lua_ValueToNumber(L, 1);
	int nSubWorldIndex = Lua_ValueToNumber(L, 2);
	
	//1.check SubWorldIndex
	if (nSubWorldIndex < 0 || nSubWorldIndex >= MAX_SUBWORLD)
		return 0;
	
	//2.check BuffId
	if (nBuffId <= 0)
		return 0;
	
	BuffMgr& bm = BuffMgr::Singleton();
	
	
	//3.add buff
	if (nTopIndex == 2)	//add buff to lord
	{
		int nLordNpcIndex = SubWorld[nSubWorldIndex].GetLord();
		
		if (!IsValidNpc(nLordNpcIndex))
			return 0;
		
		bm.AddNpcBuff(nLordNpcIndex, nLordNpcIndex, nBuffId);
	}
	else //nTopIndex == 3 add buff to Sublord which SublordNpcID point
	{
		int nSubLordNpcID = Lua_ValueToNumber(L, 3);
		
		int nSubLordNpcIndex = SubWorld[nSubWorldIndex].GetSubLord(nSubLordNpcID);
		
		if (!IsValidNpc(nSubLordNpcIndex))
			return 0;
		
		bm.AddNpcBuff(nSubLordNpcIndex, nSubLordNpcIndex, nBuffId);
	}
	
	return 0;
	
}

int LuaGetLord(Lua_State * L)
{
	int nTopIdx = Lua_GetTopIndex(L);

	if (nTopIdx != 1)
		return 0;

	int nSubWorldIdx = Lua_ValueToNumber(L, 1);

	if (nSubWorldIdx < 0 || nSubWorldIdx >= MAX_SUBWORLD)
		return 0;

	int nLord = -1;
	
	nLord = SubWorld[nSubWorldIdx].GetLord();
	
	if (IsValidNpc(nLord))
	{
		Lua_PushNumber(L, nLord);
		return 1;
	}

	return 0;
}

int LuaGetRobber(Lua_State * L)
{
	int nTopIdx = Lua_GetTopIndex(L);
	
	if (nTopIdx != 1)
		return 0;
	
	int nSubWorldIdx = Lua_ValueToNumber(L, 1);
	
	if (nSubWorldIdx < 0 || nSubWorldIdx >= MAX_SUBWORLD)
		return 0;
	
	int nRobber = -1;
	
	nRobber = SubWorld[nSubWorldIdx].GetRobber();
	
	if (IsValidNpc(nRobber))
	{
		Lua_PushNumber(L, nRobber);
		return 1;
	}
	
	return 0;
}

int LuaGetSubLord(Lua_State * L)
{
	if (Lua_GetTopIndex(L) != 2)
		return 0;

	int nSubWorldIdx = Lua_ValueToNumber(L, 1);

	if (nSubWorldIdx < 0 || nSubWorldIdx >= MAX_SUBWORLD)
		return 0;

	int nSubLordIter = Lua_ValueToNumber(L, 2);

	int nSubLordIdx = SubWorld[nSubWorldIdx].GetSubLord(nSubLordIter);

	if (!IsValidNpc(nSubLordIdx))
		return 0;

	Lua_PushNumber(L, nSubLordIdx);

	return 1;
}

int LuaIsCityHaveOwner(Lua_State * L)
{
	if (!GetGlobalTongWarMgr().IsInitedAll())
		return 0;

	if (Lua_GetTopIndex(L) != 1)
		return 0;

	int nSubWorldIndex = Lua_ValueToNumber(L, 1);
	if (nSubWorldIndex <= 0 && nSubWorldIndex >=MAX_SUBWORLD)
		return 0;

	int nLordNpcIndex = SubWorld[nSubWorldIndex].GetLord();
	if (!IsValidNpc(nLordNpcIndex))
		return 0;

	int HasOwner = 0;
	const FSGUID& guid = Npc[nLordNpcIndex].GetLord();
	if (guid.data[0] != 0)
		HasOwner = 1;

	Lua_PushNumber(L, HasOwner);
	return 1;
}

//<-------------[End]

int LuaIsEqualPile( Lua_State* L )
{
	const int nNpcIdx = Lua_ValueToNumber(L, 1);	
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton();	
		const int nBuffTempID = Lua_ValueToNumber(L, 2);
		const int nPileCount = Lua_ValueToNumber(L, 3);
		
		int nRet = BMgr.IsEqualPile( nNpcIdx, nBuffTempID, nPileCount );
		
		Lua_PushNumber(L, nRet);
		return 1;
		
	}
	
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaGetBuffPile( Lua_State* L )
{
	const int nNpcIdx = Lua_ValueToNumber(L, 1);	
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton();	
		const int nBuffTempID = Lua_ValueToNumber(L, 2);
		int nPileCount = 0;
		
		BMgr.GetBuffPileCount( nNpcIdx, nBuffTempID, nPileCount );
		
		Lua_PushNumber(L, nPileCount);
		return 1;
		
	}
	
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaDecBuffPile( Lua_State* L )
{
	const int nNpcIdx = Lua_ValueToNumber(L, 1);	
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton();	
		const int nBuffTempID = Lua_ValueToNumber(L, 2);
		int nRet = 0;
		
		nRet = BMgr.DecBuffPileByTemp( nNpcIdx, nBuffTempID );
		
		Lua_PushNumber(L, nRet);
		return 1;
		
	}
	
	Lua_PushNumber(L, 0);
	return 1;
}

int LuaAddNpcBuff( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);	
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton();	
		const int nBuffTempID = Lua_ValueToNumber(L, 2);
		const int nAddMode = Lua_ValueToNumber(L, 3);

		int nLauncher = 0;
		int nReceiver = 0;

		if(1 == nAddMode)
		{
			if (IsValidPlayer(nPlayerIndex))
			{
				nLauncher = Player[nPlayerIndex].GetNpcIndex();
			}
			nReceiver = nLauncher;
		}
		else if(2 == nAddMode)
		{
			nLauncher = nNpcIdx;
			nReceiver = nNpcIdx;
		}
		else if(3 == nAddMode)
		{
			if (IsValidPlayer(nPlayerIndex))
			{
				nLauncher = Player[nPlayerIndex].GetNpcIndex();
			}
			nReceiver = nNpcIdx;
		}
		else
		{
			nLauncher = nNpcIdx;
			if (IsValidPlayer(nPlayerIndex))
			{
				nReceiver = Player[nPlayerIndex].GetNpcIndex();
			}
		}
		
		if (IsValidNpc(nLauncher) && IsValidNpc(nReceiver))
		{
			unsigned long ulID =
				BMgr.AddNpcBuff( 
					nLauncher, 
					nReceiver, 
					nBuffTempID );

			Lua_PushNumber(L, ulID);
			return 1;
		}
	}
	
	Lua_PushNumber(L, -1);
	return 1;
}


int LuaGetBuffPersist( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);
	unsigned long ulBuffID = Lua_ValueToNumber(L, 2);
	unsigned long ulPersist = 0;
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton( );
		
		ulPersist =
		BMgr.GetBuffPersist( 
			nNpcIdx, 
			ulBuffID );
	}
	
	Lua_PushNumber( L, ulPersist );
	return 1;
}

int LuaGetBuffPersistByGroup( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);
	int nGroup = Lua_ValueToNumber(L, 2);
	unsigned long ulPersist = 0;
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton( );
		
		ulPersist =
		BMgr.GetBuffPersistByGroup( 
			nNpcIdx, 
			nGroup );
	}
	
	Lua_PushNumber( L, ulPersist );
	return 1;
}

int LuaGetBuffPersistByCate( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);
	int nCate = Lua_ValueToNumber(L, 2);
	int nIndex = Lua_ValueToNumber(L, 3);
	unsigned long ulPersist = 0;
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton( );
		
		ulPersist =
		BMgr.GetBuffPersistByCate( 
			nNpcIdx, 
			nCate,
			nIndex );
	}
	
	Lua_PushNumber( L, ulPersist );
	return 1;
}

int LuaSetBuffPersist( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);
	unsigned long ulBuffID = Lua_ValueToNumber(L, 2);
	unsigned long ulPersist = Lua_ValueToNumber(L, 3);
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton( );

		BMgr.SetBuffPersist( 
			nNpcIdx, 
			ulBuffID,
			ulPersist );
	}

	return 0;
}

int LuaSetBuffPersistByGroup( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);
	int nGroup = Lua_ValueToNumber(L, 2);
	int nPersist = Lua_ValueToNumber(L, 3);
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton( );
		
		BMgr.SetBuffPersistByGroup( 
			nNpcIdx, 
			nGroup,
			nPersist );
	}
	
	return 0;
}

int LuaSetBuffPersistByCate( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);
	int nCate = Lua_ValueToNumber(L, 2);
	int nIndex = Lua_ValueToNumber(L, 3);
	int nPersist = Lua_ValueToNumber(L, 4);
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton( );
		
		BMgr.SetBuffPersistByCate( 
			nNpcIdx, 
			nCate,
			nIndex,
			nPersist );
	}
	
	return 0;
}

int LuaAddBuffPersist( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);
	unsigned long ulBuffID = Lua_ValueToNumber(L, 2);
	int nPersist = Lua_ValueToNumber(L, 3);
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton( );
		
		BMgr.AddBuffPersist( 
			nNpcIdx, 
			ulBuffID,
			nPersist );
	}
	
	return 0;
}

int LuaAddBuffPersistByGroup( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);
	int nGroup = Lua_ValueToNumber(L, 2);
	int nPersist = Lua_ValueToNumber(L, 3);
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton( );
		
		BMgr.AddBuffPersistByGroup( 
			nNpcIdx, 
			nGroup,
			nPersist );
	}
	
	return 0;
}

int LuaAddBuffPersistByCate( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	const int nNpcIdx = Lua_ValueToNumber(L, 1);
	int nCate = Lua_ValueToNumber(L, 2);
	int nIndex = Lua_ValueToNumber(L, 3);
	int nPersist = Lua_ValueToNumber(L, 4);
	
	if (nNpcIdx > 0 && nNpcIdx < MAX_NPC)
	{
		BuffMgr& BMgr = BuffMgr::Singleton( );
		
		BMgr.AddBuffPersistByCate( 
			nNpcIdx, 
			nCate,
			nIndex,
			nPersist );
	}
	
	return 0;
}

int LuaDelBuff( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		int nIndex = Player[nPlayerIndex].m_nIndex;
		unsigned long ulID = Lua_ValueToNumber(L, 1);
		BuffMgr& BMgr = BuffMgr::Singleton( );	
		
		BMgr.ClearBuffByTempID(
			nIndex,
			ulID ) ;
	}

	return 0;
}

int LuaAddLandmine( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		int nIndex = Player[nPlayerIndex].m_nIndex;
		int nTrapID = Lua_ValueToNumber(L, 1);

		BuffMgr& BM = BuffMgr::Singleton( );

		BM.AddTrap( nTrapID, nIndex );
	}
	
	return 0;
}

int LuaAddLandminePoint( Lua_State* L )
{
	const int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		int nIndex = Player[nPlayerIndex].m_nIndex;
		int nTrapID = Lua_ValueToNumber(L, 1);
		
		KMapPos	Pos;
		KObjItemInfo ObjInfo;
		
		Pos.nSubWorld = Npc[nIndex].m_SubWorldIndex;
		Pos.nRegion = Npc[nIndex].m_RegionIndex;
		Pos.nMapX = Npc[nIndex].GetMapX();
		Pos.nMapY = Npc[nIndex].GetMapY();
		Pos.nOffX =	Npc[nIndex].GetOffX();
		Pos.nOffY = Npc[nIndex].GetOffY();
		
		ObjInfo.m_nColorID = 1;
		ObjInfo.m_nItemID = 0;
		ObjInfo.m_nMoneyNum = 0;
		ObjInfo.m_nMovieFlag = 1;
		ObjInfo.m_nSoundFlag = 1;
		ObjInfo.m_nLauncher = nIndex;
		strcpy( ObjInfo.m_szName, TRAP );
		ObjSet.Add( nTrapID, Pos, ObjInfo );
	}
	
	return 0;
}

#endif

#ifdef _SERVER
int LuaSendMail(Lua_State *L)
{
	int paramCount = Lua_GetTopIndex(L);

	if (paramCount >= 5)
	{
		const char* receiver = (const char*)Lua_ValueToString(L, 1);
		const char* title =  (const char*)Lua_ValueToString(L, 2);
		const char* content =  (const char*)Lua_ValueToString(L, 3);
		DWORD postMoney = (DWORD)Lua_ValueToNumber(L, 4);
		DWORD costMoney = (DWORD)Lua_ValueToNumber(L, 5);

		if (receiver != NULL || title != NULL || content != NULL)
		{
			if (paramCount >= 10)
			{
				Item_Identifier itemIdentifier;
				itemIdentifier.nItemClass = (int)Lua_ValueToNumber(L, 6);
				itemIdentifier.nDetailType = (int)Lua_ValueToNumber(L, 7);
				itemIdentifier.nParticularType = (int)Lua_ValueToNumber(L, 8);
				itemIdentifier.nLevel = (int)Lua_ValueToNumber(L, 9);
				itemIdentifier.nCount = (int)Lua_ValueToNumber(L, 10);
				itemIdentifier.dwCreditFlag = money_type_count;
				itemIdentifier.isMailBind = 0;

				if (paramCount >= 11)
				{
					int creditFlag = (int)Lua_ValueToNumber(L, 11);
					if (creditFlag >= 0)
						itemIdentifier.dwCreditFlag = (DWORD)creditFlag;
				}

				if (paramCount >= 12)
				{
					int isMailBind = (int)Lua_ValueToNumber(L, 12);
					itemIdentifier.isMailBind = isMailBind;
				}

				
				if (itemIdentifier.nCount > 0)
					SystemSendMail(receiver, title, content, postMoney, costMoney, 1, &itemIdentifier);
			}
			else
			{
				SystemSendMail(receiver, title, content, postMoney, costMoney, 0, NULL);
			}
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaSendMailToAll(Lua_State *L)
{
	int paramCount = Lua_GetTopIndex(L);

	if (paramCount >= 5)
	{
		const char* title =  (const char*)Lua_ValueToString(L, 1);
		const char* content =  (const char*)Lua_ValueToString(L, 2);
		DWORD postMoney = (DWORD)Lua_ValueToNumber(L, 3);
		DWORD costMoney = (DWORD)Lua_ValueToNumber(L, 4);
		int requireLevel = Lua_ValueToNumber(L, 5);

		if (title != NULL || content != NULL)
		{
			if (paramCount >= 10)
			{
				Item_Identifier itemIdentifier;
				itemIdentifier.nItemClass = (int)Lua_ValueToNumber(L, 6);
				itemIdentifier.nDetailType = (int)Lua_ValueToNumber(L, 7);
				itemIdentifier.nParticularType = (int)Lua_ValueToNumber(L, 8);
				itemIdentifier.nLevel = (int)Lua_ValueToNumber(L, 9);
				itemIdentifier.nCount = (int)Lua_ValueToNumber(L, 10);
				itemIdentifier.dwCreditFlag = money_type_count;
				
				if (paramCount >= 11)
				{
					int creditFlag = (int)Lua_ValueToNumber(L, 11);
					if (creditFlag >= 0)
						itemIdentifier.dwCreditFlag = (DWORD)creditFlag;
				}
				
				if (itemIdentifier.nCount > 0)
					SysSendMailToAll(title, content, postMoney, costMoney, 1, &itemIdentifier, requireLevel);
			}
			else
			{
				SysSendMailToAll(title, content, postMoney, costMoney, 0, NULL, requireLevel);
			}
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaExpPercentage(Lua_State *L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	
	if( IsValidPlayer(playerIndex) && paramCount == 1)
	{	
		KPlayer& player = Player[playerIndex];
		int modifyPercentage = Lua_ValueToNumber(L, 1);
		int newPercentage = player.GetExpPercentage() + modifyPercentage;		
		player.SetExpPercentage(newPercentage);
		Lua_PushNumber(L,1);
	}
	else
		Lua_PushNumber(L,0);

	return 1;
}
#endif

#ifdef _SERVER
int LuaQuestExpPercentage(Lua_State *L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	
	if( IsValidPlayer(playerIndex) && paramCount == 1)
	{	
		KPlayer& player = Player[playerIndex];
		int modifyPercentage = Lua_ValueToNumber(L, 1);
		int newPercentage = player.GetQuestExpPercentage() + modifyPercentage;		
		player.SetQuestExpPercentage(newPercentage);
		Lua_PushNumber(L,1);
	}
	else
		Lua_PushNumber(L,0);

	return 1;
}
#endif

#ifdef _SERVER
int LuaSkillExpPercentage(Lua_State *L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	
	if( IsValidPlayer(playerIndex) && paramCount == 1)
	{	
		KPlayer& player = Player[playerIndex];
		int modifyPercentage = Lua_ValueToNumber(L, 1);
		int newPercentage = player.GetSkillExpPercentage() + modifyPercentage;		
		player.SetSkillExpPercentage(newPercentage);
		Lua_PushNumber(L,1);
	}
	else
		Lua_PushNumber(L,0);

	return 1;
}
#endif

#ifdef _SERVER
int LuaGlobalExpPercentage(Lua_State *L)
{
	int paramCount = Lua_GetTopIndex(L);
	
	if(paramCount == 1)
	{
		int percentage = Lua_ValueToNumber(L, 1);
		ConfigManager::Singleton().SetGlobalVariable(global_var_exp_percentage, percentage);
		Lua_PushNumber(L,1);
	}
	else
		Lua_PushNumber(L,0);

	return 1;
}
#endif

#ifdef _SERVER
int LuaGlobalQuestExpPercentage(Lua_State *L)
{
	int paramCount = Lua_GetTopIndex(L);
	
	if(paramCount == 1)
	{
		int percentage = Lua_ValueToNumber(L, 1);
		ConfigManager::Singleton().SetGlobalVariable(global_var_quest_exp_percentage, percentage);
		Lua_PushNumber(L,1);
	}	
	else
		Lua_PushNumber(L,0);

	return 1;
}
#endif

#ifdef _SERVER
int LuaGlobalSkillExpPercentage(Lua_State *L)
{
	int paramCount = Lua_GetTopIndex(L);
	
	if(paramCount == 1)
	{
		int percentage = Lua_ValueToNumber(L, 1);
		ConfigManager::Singleton().SetGlobalVariable(global_var_skill_exp_percentage, percentage);
		Lua_PushNumber(L,1);
	}
	else
		Lua_PushNumber(L,0);

	return 1;
}
#endif

#ifdef _SERVER
int LuaGetSysCorTime(Lua_State * L)
{
	time_t nTime = UNIX_TMIE_STAMP + TIMEZONE_CORRECT;
	Lua_PushNumber(L, nTime);
	return 1;
}

int LuaGetSysTime(Lua_State * L)
{
	time_t nTime = UNIX_TMIE_STAMP;
	Lua_PushNumber(L, nTime);
	return 1;
}

// --> Rocker Edit Start 2005/08/01
int LuaGetYMD(Lua_State * L)
{
	time_t t;
	//time(&t);
	t = UNIX_TMIE_STAMP;
	tm* tp = localtime(&t);
	Lua_PushNumber(L, tp->tm_year + 1900);
	Lua_PushNumber(L, tp->tm_mon + 1);
	Lua_PushNumber(L, tp->tm_mday);
	return 3;
}

int LuaGetHMS(Lua_State * L)
{
	time_t t;
	//time(&t);
	t = UNIX_TMIE_STAMP;
	tm* tp = localtime(&t);
	Lua_PushNumber(L, tp->tm_hour);
	Lua_PushNumber(L, tp->tm_min);
	Lua_PushNumber(L, tp->tm_sec);
	return 3;
}
// <-- Rocker End

// --> Rocker Edit Start 2005/08/30
int  LuaTime2LocalYMD(Lua_State* L)
{
	time_t t = (time_t)Lua_ValueToNumber(L, 1);
	tm* tp = localtime(&t);
	Lua_PushNumber(L, tp->tm_year + 1900);
	Lua_PushNumber(L, tp->tm_mon + 1);
	Lua_PushNumber(L, tp->tm_mday);
	return 3;
}
// <-- Rocker End

#endif


// Add by [Adt.X], 2004-8-14
#ifdef _SERVER
int LuaAddWeightMax(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex)) {
		return 0;
	}
	
	int nParamCount = Lua_GetTopIndex(L);
	if (nParamCount < 1) {
		return 0;
	}
	
	int nDelta = (int)Lua_ValueToNumber(L, 1);
	
	// 服务器脚本加负重需要与客户端同步
	Player[nPlayerIndex].AddWeightMax(nDelta, true);
	return 0;
}

int LuaGetWeightMax(Lua_State *L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex)) {
		return 0;
	}
	
	int nWeightMax = Player[nPlayerIndex].GetWeightMax();
	Lua_PushNumber(L, nWeightMax);
	return 1;
}

int LuaGetWeightTaken(Lua_State *L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex)) {
		return 0;
	}

	int nWeightTaken = Player[nPlayerIndex].GetWeightTaken();
	Lua_PushNumber(L, nWeightTaken);
	return 1;

}


#endif
// End.

#ifdef _SERVER

int LuaGetPlayerIndex(Lua_State * L)
{
	int nRet = ScriptGetPlayerIndex();
	Lua_PushNumber(L, nRet);
	return 1;
}

int LuaSetPlayerIndex(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int nIdx = Lua_ValueToNumber(L, 1);
		ScriptSetPlayerIndex(nIdx);
	}
	return 0;
}
#endif

int Lua_OpenUseItemDialog(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(nPlayerIndex))
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	if (Lua_GetTopIndex(L) != 1)
	{
		Lua_PushNumber(L,0);
		return 1;
	}

	int type = Lua_ValueToNumber(L, 1);

	PLAYER_SCRIPTACTION_SYNC UiInfo;
	UiInfo.m_bOptionNum		= 0;
	UiInfo.m_nBufferLen		= sizeof(int);
	UiInfo.m_nOperateType	= SCRIPTACTION_UISHOW;
	UiInfo.m_bUIId			= UI_OPEN_USEITEM_DLG;
	UiInfo.m_nParam			= type;
	UiInfo.m_bParam1		= 1;
	UiInfo.m_bParam2		= 1;
	Player[nPlayerIndex].DoScriptAction(&UiInfo);

	Lua_PushNumber(L,1);
	return 1;
}

int LuaDelAllItem(Lua_State * L)
{
	if ( 1 == Lua_GetTopIndex(L) )
	{
		int nPlayerIndex = ScriptGetPlayerIndex();
		if (!IsValidPlayer(nPlayerIndex) ) 
		{
			Lua_PushNumber(L,0);
			return 1;
		}//endif

		int nRoomID = (int)Lua_ValueToNumber(L, 1);
		KItemList& itemList = Player[nPlayerIndex].GetItemList();
		itemList.RemoveAllInOneRoom(nRoomID);

		Lua_PushNumber(L,1);
	}
	else
		Lua_PushNumber(L,0);

	return 1;
}

#ifndef _SERVER

int LuaDrawTrap(Lua_State *L)
{
	KObjItemInfo	sInfo;
	sInfo.m_nItemID = 0;
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 0;
	sInfo.m_nSoundFlag = 0;
	sInfo.m_nMoneyNum = 0;
	strcpy(sInfo.m_szName, "\\spr\\skill\\ca.spr");
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
	
	int nX, nY;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX, &nY);
	
	int nObj = ObjSet.ClientAdd(1, 292, 0, 0, 0, nX, nY, sInfo);
	Object[nObj].SetState( OBJ_TRAP_STATE_ACTIVE );
	return 0;
}

int LuaTrapChange(Lua_State *L)
{
	KObjItemInfo	sInfo;
	sInfo.m_nItemID = 0;
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 0;
	sInfo.m_nSoundFlag = 0;
	sInfo.m_nMoneyNum = 0;
	strcpy(sInfo.m_szName, "\\spr\\skill\\ca.spr");
	sInfo.m_nColorID = 0;
	sInfo.m_nMovieFlag = 1;
	sInfo.m_nSoundFlag = 1;
	
	int nX, nY;
	Npc[Player[CLIENT_PLAYER_INDEX].m_nIndex].GetMpsPos(&nX, &nY);
	
	int nObjIdx = ObjSet.FindID( 1 );

	if ( Object[nObjIdx].m_nState == OBJ_TRAP_STATE_ACTING)
	{
		Object[nObjIdx].SetState( OBJ_TRAP_STATE_ACTIVE );		
	}
	else
	{
		Object[nObjIdx].SetState( OBJ_TRAP_STATE_ACTING );
	}
	
	return 0;
}

int LuaComMsg(Lua_State *L)
{
	const char* message = Lua_ValueToString(L, 1);
	const int type = (int)Lua_ValueToNumber(L, 2);
	
	CoreDataChanged( GDCNI_COMMSG, (UINT)message, type);
	return 0;
}

int LuaShowDuraAlert(Lua_State *L)
{
	EQUIP_DUR_STATE state[itempart_num];
		
	state[itempart_helm]	= (EQUIP_DUR_STATE)(int)Lua_ValueToNumber(L, 1);//头盔
	state[itempart_armor]	= (EQUIP_DUR_STATE)(int)Lua_ValueToNumber(L, 2);//衣服
	state[itempart_shoulder]	= (EQUIP_DUR_STATE)(int)Lua_ValueToNumber(L, 3);//护肩
	state[itempart_boots]	= (EQUIP_DUR_STATE)(int)Lua_ValueToNumber(L, 4);//靴子
	state[itempart_pendant] = (EQUIP_DUR_STATE)(int)Lua_ValueToNumber(L, 5);//坠饰（披风/令牌/绳结）
	state[itempart_weapon]	= (EQUIP_DUR_STATE)(int)Lua_ValueToNumber(L, 6);//武器
	state[itempart_amulet]	= (EQUIP_DUR_STATE)(int)Lua_ValueToNumber(L, 7);//玉佩
	state[itempart_ring]	= (EQUIP_DUR_STATE)(int)Lua_ValueToNumber(L, 8);//戒指
	state[itempart_cuff]	= (EQUIP_DUR_STATE)(int)Lua_ValueToNumber(L, 9);//护腕
	

	CoreDataChanged(GDCNI_ALERT_DURA, (UINT)state, 0);
	return 0;
}

int LuaTrackInject(Lua_State *L)
{
	if(1 == Lua_GetTopIndex(L))
	{
		int isTrack = (int)Lua_ValueToNumber(L, 1);
		CoreDataChanged(GDCNI_DEBUG_TRACK_INJECT, NULL, isTrack);
	}
	return 0;
}

int LuaPrintWindow(Lua_State *L)
{
	if(1 == Lua_GetTopIndex(L))
	{
		const char* windowName = (const char*)Lua_ValueToString(L, 1);
		CoreDataChanged(GDCNI_DEBUG_PRINT_WINDOW, (UINT)windowName, NULL);
	}
	return 0;
}
#endif

#ifdef _AUTO_ROBOT
#ifndef _SERVER
int LuaAutoRunTo(Lua_State *L)
{
	return 0;

	int	nParamCount = Lua_GetTopIndex(L);

	if(3 != nParamCount)
		return 0;

	int	nX = Lua_ValueToNumber(L, 1);
	int nY = Lua_ValueToNumber(L, 2);
	int	nSearchUnit = Lua_ValueToNumber(L, 3);

	AutoRobotMgr &mgr = AutoRobotMgr::Singleton();
	mgr.AutoRunTo(nX, nY * 2, nSearchUnit);

	return 0;
}
#endif
#endif

#ifdef _SERVER
int LuaShowBanner(Lua_State *L)
{
	int	paramCount = Lua_GetTopIndex(L);
	if (paramCount != 5)
		return 0;

	const char* pMsg = Lua_ValueToString(L, 1);
	const char* pFont = Lua_ValueToString(L, 2);
	int color = 0;
	sscanf(Lua_ValueToString(L,3), "%x", &color);
	if (pMsg == NULL || pFont == NULL)
		return 0;
	const int speed = Lua_ValueToNumber(L, 4);
	const int second = Lua_ValueToNumber(L, 5);

	char msgBuff[MAX_SHOW_BANNER_MSG_LENGTH] = { 0 };
	char fontBuff[MAX_SHOW_BANNER_FONT_LENGTH] = { 0 };
	strncpy(msgBuff, pMsg, MAX_SHOW_BANNER_MSG_LENGTH);
	strncpy(fontBuff, pFont, MAX_SHOW_BANNER_FONT_LENGTH);

	char sendBuff[COMMON_SHOW_BANNER_BUFF_LENGTH];
	int sendSize = PrepareShowBannerBuff(sendBuff, sizeof(sendBuff), msgBuff, MAX_SHOW_BANNER_MSG_LENGTH, fontBuff, MAX_SHOW_BANNER_FONT_LENGTH, color, speed, second, 1);

	int playerCount = PlayerSet.GetPlayerNumber();
	int playerIndex = PlayerSet.GetFirstPlayer();
	for (int i = 0; i < playerCount; i++)
	{
		if (IsValidPlayer(playerIndex))
		{
			KPlayer& player = Player[playerIndex];
			
			if (g_pServer != NULL)
				g_pServer->PackDataToClient(player.GetNetConnectIdx(), sendBuff, sendSize);

			playerIndex = PlayerSet.GetNextPlayer();
		}
		else
		{
			break;
		}
	}

	return 0;
}

int LuaShowBannerId(Lua_State *L)
{
	if(Lua_GetTopIndex(L) != 5)
	{
		return 0;
	}

	if(!Lua_IsNumber(L, 1)) 
	{
		return 0;
	}

	if(!Lua_IsNumber(L, 2)) 
	{
		return 0;
	}

	if(!Lua_IsString(L, 3)) 
	{
		return 0;
	}

	if(!Lua_IsNumber(L, 4)) 
	{
		return 0;
	}

	if(!Lua_IsNumber(L, 5)) 
	{
		return 0;
	}

	if(g_pServer == NULL)
	{
		return 0;
	}

	int msgId = Lua_ValueToNumber(L, 1);
	int fontId = Lua_ValueToNumber(L, 2);
	int color = 0;
	sscanf(Lua_ValueToString(L, 3), "%x", &color);
	const int speed = Lua_ValueToNumber(L, 4);
	const int second = Lua_ValueToNumber(L, 5);

	SHOW_BANNER_ID showBanner;
	showBanner.Protocol = s2c_show_banner_id;
	showBanner.msgId = msgId;
	showBanner.fontId = fontId;
	showBanner.color = color;
	showBanner.speed = speed;
	showBanner.second = second;
	showBanner.bannerType = 1;

	int playerCount = PlayerSet.GetPlayerNumber();
	int playerIndex = PlayerSet.GetFirstPlayer();

	for(int i = 0; i < playerCount; i++)
	{
		if(IsValidPlayer(playerIndex))
		{
			KPlayer& player = Player[playerIndex];
			g_pServer->PackDataToClient(player.GetNetConnectIdx(), &showBanner, sizeof(SHOW_BANNER_ID));

			playerIndex = PlayerSet.GetNextPlayer();
		}
		else
		{
			break;
		}
	}

	return 0;
}

//xiehong add 2007-11-29
int LuaShowBanner2(Lua_State *L)
{
	int	paramCount = Lua_GetTopIndex(L);
	if (paramCount != 2)
		return 0;

	const char* pMsg = Lua_ValueToString(L, 1);

	ShizuBannerMgr::Style style;
	int maxTextLen = sizeof(style.text);
	strncpy(style.text, pMsg, maxTextLen);
	style.text[maxTextLen - 1] = 0;

	int bannerIndex = Lua_ValueToNumber(L, 2);

	if(!ShizuBannerMgr::getSingleton().setBanner(bannerIndex, style))
	{
		return 0;
	}

	int playerCount = PlayerSet.GetPlayerNumber();
	int playerIndex = PlayerSet.GetFirstPlayer();
	while (playerIndex > 0)
	{
		ShizuBannerMgr::getSingleton().showBannerToPlayer(playerIndex, bannerIndex);
		playerIndex = PlayerSet.GetNextPlayer();
	}

	return 0;
}
#endif

#ifdef _SERVER

int luaIsPoolCombatCondValid(Lua_State *L)
{
	bool    bValid       = false;
	int     nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!GetGlobalPoolCombatMgr().IsInitedAll())
	{
		lua_pushboolean(L,false);
		return 1;
	}//endif
	
	if (IsValidPlayer(nPlayerIndex))
	{
		if ( IsUnitOwner(nPlayerIndex,enSUTplId_Tong,enSULayer_Tong) )
		{
			int nNpcIndex = Player[nPlayerIndex].GetNpcIndex();	
			
			if (IsValidNpc(nNpcIndex))
			{
				int nSubWorldIndex = Npc[nNpcIndex].GetSubWorldIndex();
				if (nSubWorldIndex != INVALID_WORLD_INDEX  && GetGlobalPoolCombatMgr().IsPoolCombatMap(SubWorld[nSubWorldIndex].m_SubWorldID))
				{
					int nPoolIndex = SubWorld[nSubWorldIndex].GetPool();
					if (IsValidNpc(nPoolIndex))
					{
						ConfigManager &       cm = ConfigManager::Singleton();
						int       nProtectBuffID = cm.GetGlobalVariable(global_var_pool_combat_protect_buff);
						BuffMgr & buffMgr        = BuffMgr::Singleton();
						
						if (!buffMgr.IsHaveBuff(nPoolIndex,nProtectBuffID))
						{
							KItemList     & itemlist = Player[nPlayerIndex].GetItemList();
							
							int             nClass   = cm.GetGlobalVariable(global_var_pool_combat_item_class);
							int             nDetail  = cm.GetGlobalVariable(global_var_pool_combat_item_detail);
							int             nSpecial = cm.GetGlobalVariable(global_var_pool_combat_item_special);
							int             nLevel   = cm.GetGlobalVariable(global_var_pool_combat_item_level);
							
							if (itemlist.IsExistItem(nClass,nDetail,nSpecial,nLevel))
							{
								SocialUnit * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
								SocialUnit * pTongUnit = GetUpNUnit(pLeafUnit,enSULayer_Tong);
								if (pTongUnit && GetPoolMapId(pTongUnit->GetUnitAttr()) == INVALID_WORLD_ID)
								{
									bool bExist      = GetPoolCombatInfoManager().ExistRecord(SubWorld[nSubWorldIndex].m_SubWorldID);
									bool bHaveCombat = GetPoolCombatInfoManager().ExistRecord(pTongUnit->GetUnitGuid());
									
									if (!bExist && !bHaveCombat)
									{
										int nStartTime = cm.GetGlobalVariable(global_var_pool_combat_start_dec_t);
										int nEndTime   = cm.GetGlobalVariable(global_var_pool_combat_end_dec_t);
										
										time_t tCurentTime=UNIX_TMIE_STAMP;
										tm * time=localtime(&tCurentTime);
										if (time && time->tm_hour>=nStartTime && time->tm_hour<=nEndTime && GetGlobalPoolCombatMgr().IsInitedAll() )
										{
											lua_pushboolean(L,true);
											return 1;
										}//endif
										else
										{
											//ChatErrCodeToClient(nPlayerIndex, chat_err_pool_wrong_time);
										}//end else
										
									}//endif	
									else
									{
										//ChatErrCodeToClient(nPlayerIndex, chat_err_pool_in_combat);
									}//end else
									
								}//endif
								else
								{
									//ChatErrCodeToClient(nPlayerIndex, chat_err_pool_no_combat_to_own);
								}
								
							}//endif
							else
							{
								//ChatErrCodeToClient(nPlayerIndex, chat_err_pool_item_needed);
							}//end else
							
						}//endif
						
					}//endif
					else
					{
						//ChatErrCodeToClient(nPlayerIndex, chat_err_pool_mustbeowner);
					}//end else

				}//endif
				else
				{
					_ASSERT(false);	
				}//end else

			}//endif

		}//endif
		else
		{
			//ChatErrCodeToClient(nPlayerIndex, chat_err_pool_mustbeowner);
		}//end else

	}//endif

	lua_pushboolean(L,bValid);
	
	return 1;
}

int luaPoolCombatStart(Lua_State *L)
{
	int     nPlayerIndex = ScriptGetPlayerIndex();

	if (IsValidPlayer(nPlayerIndex))
	{
		if (!GetGlobalPoolCombatMgr().IsInitedAll())
		{
			ChatErrCodeToClient(nPlayerIndex, chat_err_pool_sys_busy);
			return 0;
		}//endif

		if ( IsUnitOwner(nPlayerIndex,enSUTplId_Tong,enSULayer_Tong) )
		{
			int nNpcIndex = Player[nPlayerIndex].GetNpcIndex();	
			
			if (IsValidNpc(nNpcIndex))
			{
				int nSubWorldIndex = Npc[nNpcIndex].GetSubWorldIndex();
				if (nSubWorldIndex != INVALID_WORLD_INDEX &&  GetGlobalPoolCombatMgr().IsPoolCombatMap(SubWorld[nSubWorldIndex].m_SubWorldID))
				{
					int nPoolIndex = SubWorld[nSubWorldIndex].GetPool();
					if (IsValidNpc(nPoolIndex))
					{
						ConfigManager &       cm = ConfigManager::Singleton();
						int       nProtectBuffID = cm.GetGlobalVariable(global_var_pool_combat_protect_buff);
						BuffMgr & buffMgr        = BuffMgr::Singleton();
						
						if (!buffMgr.IsHaveBuff(nPoolIndex,nProtectBuffID))
						{
							KItemList     & itemlist = Player[nPlayerIndex].GetItemList();
							
							int             nClass   = cm.GetGlobalVariable(global_var_pool_combat_item_class);
							int             nDetail  = cm.GetGlobalVariable(global_var_pool_combat_item_detail);
							int             nSpecial = cm.GetGlobalVariable(global_var_pool_combat_item_special);
							int             nLevel   = cm.GetGlobalVariable(global_var_pool_combat_item_level);
							
							if (itemlist.IsExistItem(nClass,nDetail,nSpecial,nLevel))
							{
								SocialUnit * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
								SocialUnit * pTongUnit = GetUpNUnit(pLeafUnit,enSULayer_Tong);
								if (pTongUnit && GetPoolMapId(pTongUnit->GetUnitAttr()) == INVALID_WORLD_ID)
								{
									bool bExist = GetPoolCombatInfoManager().ExistRecord(SubWorld[nSubWorldIndex].m_SubWorldID);
									bool bHaveCombat = GetPoolCombatInfoManager().ExistRecord(pTongUnit->GetUnitGuid());
									
									if (!bExist && !bHaveCombat)
									{
										int nStartTime = cm.GetGlobalVariable(global_var_pool_combat_start_dec_t);
										int nEndTime   = cm.GetGlobalVariable(global_var_pool_combat_end_dec_t);
										
										time_t tCurentTime=UNIX_TMIE_STAMP;
										tm * time=localtime(&tCurentTime);
										if (time && time->tm_hour>=nStartTime && time->tm_hour<=nEndTime)
										{
											itemlist.DelNormalItem(nClass,nDetail,nSpecial,nLevel);
											GetGlobalPoolCombatMgr().EnHanceANewCombat(pTongUnit->GetUnitGuid(),SubWorld[nSubWorldIndex].m_SubWorldID);
										}//endif
										else
										{
											ChatErrCodeToClient(nPlayerIndex, chat_err_pool_wrong_time);
										}//end else
										
									}//endif	
									else
									{
										ChatErrCodeToClient(nPlayerIndex, chat_err_pool_in_combat);
									}//end else
									
								}//endif
								else
								{
									ChatErrCodeToClient(nPlayerIndex, chat_err_pool_no_combat_to_own);
								}
								
							}//endif
							else
							{
								ChatErrCodeToClient(nPlayerIndex, chat_err_pool_item_needed);
							}//end else

						}//endif
						else
						{
							ChatErrCodeToClient(nPlayerIndex, chat_err_pool_wrong_time);
						}

						
					}//endif
					else
					{
						ChatErrCodeToClient(nPlayerIndex, chat_err_pool_mustbeowner);
					}//end else
					
				}//endif
				else
				{
					_ASSERT(false);	
				}//end else
				
			}//endif
			
		}//endif
		else
		{
			ChatErrCodeToClient(nPlayerIndex, chat_err_pool_mustbeowner);
		}//end else
		
	}//endif
	
	return 0;
}

#define POOL_DROP_ERT_NORMAL_FAILD 0
#define POOL_DROP_RET_SUCSESS      1
#define POOL_DROP_RET_WRONG_MAP    2
#define POOL_DROP_RET_IN_COMBAT    3

int luaDropPool(Lua_State * L)
{
	int     nPlayerIndex = ScriptGetPlayerIndex();
	
	if (!IsValidPlayer(nPlayerIndex))
	{
		lua_pushnumber(L,POOL_DROP_ERT_NORMAL_FAILD);
		return 1;
	}//endif

	if (!GetGlobalPoolCombatMgr().IsInitedAll())
	{
		lua_pushnumber(L,POOL_DROP_ERT_NORMAL_FAILD);
		return 1;
	}//endif
	
	if ( !IsUnitOwner(nPlayerIndex,enSUTplId_Tong,enSULayer_Tong) )
	{
		lua_pushnumber(L,POOL_DROP_ERT_NORMAL_FAILD);
		return 1;
	}//endif

	int nNpcIndex = Player[nPlayerIndex].GetNpcIndex();	
	
	if (!IsValidNpc(nNpcIndex))
	{
		lua_pushnumber(L,POOL_DROP_ERT_NORMAL_FAILD);
		return 1;
	}//endif

	int nSubWorldIndex = Npc[nNpcIndex].GetSubWorldIndex();
	if (nSubWorldIndex == INVALID_WORLD_INDEX || !GetGlobalPoolCombatMgr().IsPoolCombatMap(SubWorld[nSubWorldIndex].m_SubWorldID))
	{
		lua_pushnumber(L,POOL_DROP_RET_WRONG_MAP);
		return 1;
	}//endif

	int nPoolIndex = SubWorld[nSubWorldIndex].GetPool();
	if (!IsValidNpc(nPoolIndex))
	{
		lua_pushnumber(L,POOL_DROP_RET_WRONG_MAP);
		return 1;
	}//endif
	
	SocialUnit * pLeafUnit = GetLeafUnit(nPlayerIndex,enSUTplId_Tong);
	SocialUnit * pTongUnit = GetUpNUnit(pLeafUnit,enSULayer_Tong);
	if (pTongUnit == NULL || GetPoolMapId(pTongUnit->GetUnitAttr()) != SubWorld[nSubWorldIndex].m_SubWorldID)
	{
		lua_pushnumber(L,POOL_DROP_RET_WRONG_MAP);
		return 1;
	}//endif
	
	if (GetPoolCombatInfoManager().ExistRecord(SubWorld[nSubWorldIndex].m_SubWorldID))
	{
		lua_pushnumber(L,POOL_DROP_RET_IN_COMBAT);
		return 1;
	}//endif

	//Drop process
	FSGUID          invalid;
	Npc[nPoolIndex].SetLord(invalid);
	Npc[nPoolIndex].Save();
	
	int nSubPoolIndex = SubWorld[nSubWorldIndex].GetSubPool();
	if (IsValidNpc(nSubPoolIndex))
	{
		Npc[nSubPoolIndex].SetLord(invalid);
		Npc[nSubPoolIndex].Save();
	}//endif
	
	SocialUnitAttr	& attr = pTongUnit->GetUnitAttr();
	attr.DelAttr(enSUAttr_PoolMap);
	SocialSerializer::Singleton().UpdateAttrReq(-1, pTongUnit);

	lua_pushnumber(L,POOL_DROP_RET_SUCSESS);

	return 1;
}

int LuaIsHaveWorldFlag(Lua_State *L)
{
	DWORD dwWorldFlag = 0;
	int nPlayerIndex  = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		if (Lua_GetTopIndex(L)>=1)
		{
			int nTargetFlag = Lua_ValueToNumber(L,1);
			int nWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].GetSubWorldIndex();		
			dwWorldFlag     = SubWorld[nWorldIndex].IsHaveMapFlage(nTargetFlag);
		}//endif

	}//endif

	Lua_PushNumber(L,dwWorldFlag);
    return 1;
}


int LuaChangeTaisuiTianXiang(Lua_State *L)
{
	unsigned long        dwDay= Lua_ValueToNumber(L,1);
	unsigned long      dwMonth= Lua_ValueToNumber(L,2);
	ITianXiangMgr * pTianXiang=GetMainTianXiangMgr();
	
	if (pTianXiang && dwDay>0 && dwDay<=60)
	{
		pTianXiang->SetCurrentTianXiangDay(dwDay);
	}//endif

	if (pTianXiang && dwMonth>0 && dwMonth <=60)
	{
		pTianXiang->SetCurrentTianXiangMonth(dwMonth);
	}//endif

	return 0;
}

int luaClientCanWheelTaisui(Lua_State * L)
{
    unsigned long dwIndex = Lua_ValueToNumber(L,1);
	unsigned long res=0;

	if (IsValidPlayer(dwIndex)  && Player[dwIndex].GetTaisuiWheelSys()->IsInited())
	{
        unsigned long dwTimes=Player[dwIndex].GetTaisuiWheelSys()->GetCurrentWheeledTimes();
		res                  = dwTimes;
	}//endif

	Lua_PushNumber(L,res);

	return 1;
}

int LuaSuspendTianXiang(Lua_State *L)
{
	ITianXiangMgr * pTianXiang=GetMainTianXiangMgr();
	
	if (pTianXiang)
	{
		pTianXiang->Suspend();
	}//endif
	
	return 0;
}

int LuaResumeTianXiang(Lua_State *L)
{
	ITianXiangMgr * pTianXiang=GetMainTianXiangMgr();
	
	if (pTianXiang)
	{
		pTianXiang->Resume();
	}//endif
	
	return 0;
}

int LuaAddTailsmanExp(Lua_State *L)
{
	int nExp = Lua_ValueToNumber(L,1);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		if (nExp > 0 && nExp <= MAX_ADD_TALISMAN_POTENTIAL)
		{
			Player[playerIndex].AddTalismanPotential((DWORD)nExp);
		}		
	}

	return 0;
}

#endif

//#ifdef _SERVER
//int LuaLogAcceptQuest(Lua_State *L)
//{
// 	int playerIndex = ScriptGetPlayerIndex();
// 	if (IsValidPlayer(playerIndex))
// 	{
// 		KPlayer& player = Player[playerIndex];
// 		int questId = Lua_ValueToNumber(L, 1);
// 
// 		LogEventParam questEvent;
// 		questEvent.event = log_event_quest_accept;
// 		questEvent.param1 = player.GetGUID();
// 		sprintf((char*)&(questEvent.param2), "%d", questId);
// 		questEvent.param4 = player.GetOnlineTime();
// 		sprintf(questEvent.comment, LOG_EVENT_QUEST_ACCEPT_COMMENT, player.GetPlayerName(), questId, player.GetOnlineTime());
// 		g_pLogSystem->Log(questEvent);
// 	}
//
//	return 0;
//}
//#endif

//#ifdef _SERVER
//int LuaLogAbortQuest(Lua_State *L)
//{
// 	int playerIndex = ScriptGetPlayerIndex();
// 	if (IsValidPlayer(playerIndex))
// 	{
// 		KPlayer& player = Player[playerIndex];
// 		int questId = Lua_ValueToNumber(L, 1);
// 
// 		LogEventParam questEvent;
// 		questEvent.event = log_event_quest_abort;
// 		questEvent.param1 = player.GetGUID();
// 		sprintf((char*)&(questEvent.param2), "%d", questId);
// 		questEvent.param4 = player.GetOnlineTime();
// 		sprintf(questEvent.comment, LOG_EVENT_QUEST_ABORT_COMMENT, player.GetPlayerName(), questId, player.GetOnlineTime());
// 		g_pLogSystem->Log(questEvent);
// 	}
//
//	return 0;
//}
//#endif

//#ifdef _SERVER
//int LuaLogCompleteQuest(Lua_State *L)
//{
// 	int playerIndex = ScriptGetPlayerIndex();
// 	if (IsValidPlayer(playerIndex))
// 	{
// 		KPlayer& player = Player[playerIndex];
// 		int questId = Lua_ValueToNumber(L, 1);
// 
// 		LogEventParam questEvent;
// 		questEvent.event = log_event_quest_complete;
// 		questEvent.param1 = player.GetGUID();
// 		sprintf((char*)&(questEvent.param2), "%d", questId);
// 		questEvent.param4 = player.GetOnlineTime();
// 		sprintf(questEvent.comment, LOG_EVENT_QUEST_COMPLETE_COMMENT, player.GetPlayerName(), questId, player.GetOnlineTime());
// 		g_pLogSystem->Log(questEvent);
// 	}
//
//	return 0;
//}
//#endif

#ifdef _SERVER

int LuaChangeMap(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		SEND_CHANGEMAP	tagChangeMap;
		tagChangeMap.Protocol		=	s2c_byte_extend;
		tagChangeMap.ProtocolExtend	=	s2c_ex_protocol_changemap;
		tagChangeMap.wProtocolSize	=	sizeof( tagChangeMap );
		tagChangeMap.bShowElf		=	Lua_ValueToNumber(L, 1);
		tagChangeMap.nMap			=	Lua_ValueToNumber(L, 2);
		SendDataToClient(playerIndex, &tagChangeMap, sizeof( tagChangeMap ));
	}
	return 0;
}

int LuaCreateInstance(Lua_State *L)
{
	DWORD instanceId = INVALID_INSTANCE_ID;
	int worldIndex = INVALID_WORLD_INDEX;
	if (Lua_GetTopIndex(L) >= 2)
	{
		int worldTemplateId = Lua_ValueToNumber(L, 1);
		int creatorPlayerIndex = Lua_ValueToNumber(L, 2);
		DWORD lifeTime = 0;
		if (Lua_GetTopIndex(L) >= 3)
		{
			lifeTime = (DWORD)Lua_ValueToNumber(L, 3);
		}

		worldIndex = g_SubWorldSet.CreateInstance(worldTemplateId, creatorPlayerIndex, lifeTime);
		if (worldIndex != INVALID_WORLD_INDEX)
		{
			instanceId = SubWorld[worldIndex].GetInstanceId();
		}
	}

	Lua_PushNumber(L, instanceId);
	Lua_PushNumber(L, worldIndex);
	return 2;
}
#endif

#ifdef _SERVER
int LuaEnterInstanceByID(Lua_State *L)
{
	int result = 0;
	if (Lua_GetTopIndex(L) == 3)
	{
		int playerIndex = Lua_ValueToNumber(L, 1);
		DWORD instanceId = Lua_ValueToNumber(L, 2);
		int entryIndex = Lua_ValueToNumber(L, 3) - 1;
		result = g_SubWorldSet.EnterInstanceByID(playerIndex, instanceId, entryIndex);
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaCloseInstance(Lua_State *L)
{
	int result = 0;
	if (Lua_GetTopIndex(L) == 1)
	{
		DWORD instanceId = Lua_ValueToNumber(L, 1);
		result = g_SubWorldSet.CloseInstanceByID(instanceId);
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaEnterInstance(Lua_State *L)
{
	int result = 0;
	if (Lua_GetTopIndex(L) >= 2)
	{
		int playerIndex = ScriptGetPlayerIndex();
		int worldTemplateId = Lua_ValueToNumber(L, 1);
		int entryIndex = Lua_ValueToNumber(L, 2) - 1;
		bool createIfNotExist = false;
		if (Lua_GetTopIndex(L) >= 3)
		{
			createIfNotExist = (Lua_ValueToNumber(L, 3) == TRUE);
		}
		DWORD lifeTime = 0;
		if (Lua_GetTopIndex(L) >= 4)
		{
			lifeTime = (DWORD)Lua_ValueToNumber(L, 4);
		}
		
		if (IsValidPlayer(playerIndex) && worldTemplateId >= 0 && entryIndex >= 0)
		{
			result = g_SubWorldSet.EnterInstance(playerIndex, worldTemplateId, entryIndex, lifeTime, createIfNotExist);
		}
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetPkValue(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();	
	if (IsValidPlayer(playerIndex))
	{
		Lua_PushNumber(L, Player[playerIndex].GetPkValue());
	}
	else
		Lua_PushNumber(L,0);

	return 1;;
}
#endif

#ifdef _SERVER
int LuaGetWorldCustomVariable(Lua_State *L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		int varIndex = Lua_ValueToNumber(L, 2);
		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
		{
			Lua_PushNumber(L, SubWorld[worldIndex].GetCustomVariable(varIndex));
			return 1;
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaSetWorldCustomVariable(Lua_State *L)
{
	if (Lua_GetTopIndex(L) == 3)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		int varIndex = Lua_ValueToNumber(L, 2);
		int varValue = Lua_ValueToNumber(L, 3);
		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
		{
			SubWorld[worldIndex].SetCustomVariable(varIndex, varValue);
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaGetWorldCustomString(Lua_State *L)
{
	char outStrBuff[MAX_WORLD_CUSTOM_STRING_LENGTH] = { 0 };

	if (Lua_GetTopIndex(L) == 2)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		int strIndex = Lua_ValueToNumber(L, 2);
		
		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
		{
			SubWorld[worldIndex].GetCustomString(strIndex, outStrBuff, sizeof(outStrBuff));
		}
	}
	
	outStrBuff[sizeof(outStrBuff) - 1] = 0;
	Lua_PushString(L, outStrBuff);
	return 1;
}
#endif

#ifdef _SERVER
int LuaSetWorldCustomString(Lua_State *L)
{
	if (Lua_GetTopIndex(L) == 3)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		int strIndex = Lua_ValueToNumber(L, 2);
		const char* strValue = Lua_ValueToString(L, 3);

		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
		{
			SubWorld[worldIndex].SetCustomString(strIndex, strValue, MAX_WORLD_CUSTOM_STRING_LENGTH);
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaSendWorldCustomStringToPlayer(Lua_State *L)
{
	if (Lua_GetTopIndex(L) == 3)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		int strIndex = Lua_ValueToNumber(L, 2);
		int playerIndex = Lua_ValueToNumber(L, 3);
		
		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
		{
			SubWorld[worldIndex].SendCustomStringToPlayer(strIndex, playerIndex);
		}
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaSendWorldCustomStringToAllPlayer(Lua_State *L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		int strIndex = Lua_ValueToNumber(L, 2);
		
		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
		{
			SubWorld[worldIndex].SendCustomStringToAllPlayer(strIndex);
		}
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaCreateWorldTeam(Lua_State *L)
{
	int teamId = INVALID_TEAM_ID;
	if (Lua_GetTopIndex(L) == 3)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		int teamIndex = Lua_ValueToNumber(L, 2);
		int captainPlayerIndex = Lua_ValueToNumber(L, 3);
		if (IsValidPlayer(captainPlayerIndex))
		{
			teamId = SubWorld[worldIndex].CreateWorldTeam(teamIndex, captainPlayerIndex);
		}
	}

	Lua_PushNumber(L, teamId);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetWorldTeam(Lua_State *L)
{
	int teamId = INVALID_TEAM_ID;
	if (Lua_GetTopIndex(L) == 2)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		int teamIndex = Lua_ValueToNumber(L, 2);
		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
		{
			teamId = SubWorld[worldIndex].GetWorldTeam(teamIndex);
		}
	}

	Lua_PushNumber(L, teamId);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetWorldTeamCount(Lua_State *L)
{
	int teamCount = 0;
	if (Lua_GetTopIndex(L) == 1)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
		{
			teamCount = SubWorld[worldIndex].GetWorldTeamCount();
		}
	}

	Lua_PushNumber(L, teamCount);
	return 1;
}
#endif

#ifdef _SERVER
int LuaTeamAddMember(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		int teamId = Lua_ValueToNumber(L, 1);
		int memberPlayerIndex = Lua_ValueToNumber(L, 2);
		
		if (IsValidPlayer(memberPlayerIndex))
		{
			KPlayerTeam& teamInfo = Player[memberPlayerIndex].GetTeamInfo();
			if (teamInfo.IsInTeam())
			{
				teamInfo.LeaveTeam();
			}

			KTeam* pTeam = g_TeamSet.GetTeam(teamId);
			if (pTeam)
			{
				pTeam->AddMember(memberPlayerIndex);
			}
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaTeamDeleteMember(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		int teamId = Lua_ValueToNumber(L, 1);
		int memberPlayerIndex = Lua_ValueToNumber(L, 2);

		KTeam* pTeam = g_TeamSet.GetTeam(teamId);
		if (pTeam)
		{
			pTeam->DeleteMember(memberPlayerIndex);
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaOpenBigTeamMode(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int teamId = Lua_ValueToNumber(L, 1);
		
		KTeam* pTeam = g_TeamSet.GetTeam(teamId);
		if (pTeam)
		{
			pTeam->OpenBigTeamMode();
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaSetTeamParam(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 3)
	{
		int teamId = Lua_ValueToNumber(L, 1);
		int paramIndex = Lua_ValueToNumber(L, 2);
		int paramValue = Lua_ValueToNumber(L, 3);

		KTeam* pTeam = g_TeamSet.GetTeam(teamId);
		if (pTeam)
		{
			bool can = (paramValue == TRUE);
			switch(paramIndex)
			{
			case 0:
				pTeam->SetCanKick(can);
				break;
			case 1:
				pTeam->SetCanInvite(can);
				break;
			case 2:
				pTeam->SetCanPromoteAssistant(can);
				break;
			case 3:
				pTeam->SetCanDismissAssistant(can);
				break;
			case 4:
				pTeam->SetCanLeave(can);
				break;
			case 5:
				pTeam->SetCanOpenBigTeam(can);
				break;
			case 6:
				pTeam->SetCanDismiss(can);
				break;
			}
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaWorldFirstPlayer(Lua_State * L)
{
	int playerIndex = 0;
	if (Lua_GetTopIndex(L) == 1)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
			playerIndex = SubWorld[worldIndex].FirstPlayer();
	}

	Lua_PushNumber(L, playerIndex);
	return 1;
}
#endif

#ifdef _SERVER
int LuaWorldNextPlayer(Lua_State * L)
{
	int playerIndex = 0;
	if (Lua_GetTopIndex(L) == 1)
	{
		int worldIndex = Lua_ValueToNumber(L, 1);
		if (worldIndex >= 0 && worldIndex < MAX_SUBWORLD)
			playerIndex = SubWorld[worldIndex].NextPlayer();
	}

	Lua_PushNumber(L, playerIndex);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetWorldIndex(Lua_State * L)
{
	int worldIndex = INVALID_WORLD_INDEX;
	if (Lua_GetTopIndex(L) == 1)
	{
		int playerIndex = Lua_ValueToNumber(L, 1);
		if (IsValidPlayer(playerIndex))
			worldIndex = Npc[Player[playerIndex].GetNpcIndex()].GetSubWorldIndex();
	}

	Lua_PushNumber(L, worldIndex);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetCurrentWorldIndex(Lua_State * L)
{
	int worldIndex = ScriptGetSubWorldIndex();
	Lua_PushNumber(L, worldIndex);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetCurrentWorldId(Lua_State * L)
{
	DWORD instanceId = INVALID_INSTANCE_ID;
	int worldIndex = ScriptGetSubWorldIndex();
	if (worldIndex >= INSTANCE_SUBWORLD_START && worldIndex < INSTANCE_SUBWORLD_END)
	{
		instanceId = SubWorld[worldIndex].GetInstanceId();
	}
	Lua_PushNumber(L, instanceId);
	return 1;
}
#endif

#ifdef _SERVER
int LuaCanEnterWorld(Lua_State * L)
{
	int result = 0;
	int playerIndex = INVALID_PLAYER_INDEX;
	int worldTemplateId = 0;
	if (Lua_GetTopIndex(L) >= 1)
	{
		int worldTemplateId = Lua_ValueToNumber(L, 1);
		if (Lua_GetTopIndex(L) == 2)
		{
			playerIndex = Lua_ValueToNumber(L, 2);
		}
		else
		{
			playerIndex = ScriptGetPlayerIndex();
		}		
		
		WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(worldTemplateId);
		if (pSetting)
		{
			if (pSetting->IsMatchRequirements(playerIndex, INVALID_WORLD_INDEX))
				result = 1;
		}
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER

int LuaGetInstanceCombatPersonNum(Lua_State * L)
{
	int result = 0;
	if (Lua_GetTopIndex(L) == 2)
	{
		DWORD dwInstanceId = Lua_ValueToNumber(L, 1);
		int   nOrgId       = Lua_ValueToNumber(L ,2);

		int nSubWorldIndex = g_SubWorldSet.GetInstance(dwInstanceId);
		if (nSubWorldIndex != INVALID_WORLD_INDEX && nSubWorldIndex < MAX_SUBWORLD)
		{
			const CombatOrgnize * orgInfo = SubWorld[nSubWorldIndex].GetCombatInstanceOrgInfo(nOrgId);
			if (orgInfo)
			{
				result                    = orgInfo->nPersonNum;
			}//endif

		}//endif

	}//endif
	
	Lua_PushNumber(L, result);
	return 1;
}

int luaGetPlayerChangeWorld(Lua_State * L)
{
	int nResult = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex) && Lua_GetTopIndex(L) == 1)
	{
		int nSenderWorldIndex = Npc[Player[nPlayerIndex].m_nIndex].GetSubWorldIndex();
		int nSenderX = 0;
		int nSenderY = 0;
		
		Npc[Player[nPlayerIndex].m_nIndex].GetMpsPos(&nSenderX,&nSenderY);
		
		int nTargetPlayerIndex = Lua_ValueToNumber(L,1);
		
		if (IsValidPlayer(nTargetPlayerIndex) && nSenderWorldIndex != INVALID_WORLD_INDEX && nSenderWorldIndex < MAX_SUBWORLD)
		{
			int nTargetNpcIndex = Player[nTargetPlayerIndex].m_nIndex;
			if (IsValidNpc(nTargetNpcIndex))
			{
				nResult = Npc[nTargetNpcIndex].ChangeWorld(
					      SubWorld[nSenderWorldIndex].m_SubWorldID,
					      nSenderX,
					      nSenderY,
					      false);
			}//endif
			
		}//endif
		
	}//endif

	Lua_PushNumber(L,nResult);
	return 1;
}

int LuaIsMapProcesssWar( Lua_State * L)
{
	if (!GetGlobalWarInfoManager().IsInited())
	{
		Lua_PushNumber(L,1);
		return 1;
	}//endif

	int nRet = 0;

	if (Lua_GetTopIndex(L) == 1)
	{
		int nWorldIndex = Lua_ValueToNumber(L,1);
		
		if (nWorldIndex>= 0 && nWorldIndex < MAX_SUBWORLD)
		{
			FSWarInfo * pWarInfo = GetGlobalWarInfoManager().GetRecordByMapId( SubWorld[nWorldIndex].GetWorldTemplateId() );
			if (pWarInfo && pWarInfo->warState == FS_WAR_STATE_PROCESS)
			{
				nRet = 1;
			}//endif

		}//endif

	}//endif

	Lua_PushNumber(L,nRet);
	return 1;
}

int LuaRegisterPlayerToCombat(Lua_State * L)
{
	int result       = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();

	if (IsValidPlayer(nPlayerIndex))
	{
		if (Lua_GetTopIndex(L) == 2)
		{
			DWORD dwInstanceID = Lua_ValueToNumber(L,1);
			DWORD dwOrg        = Lua_ValueToNumber(L,2);

			int   nWorldIndex  = g_SubWorldSet.GetInstance(dwInstanceID);
			
			if (IsValidCombatID(dwOrg) && nWorldIndex!= INVALID_WORLD_INDEX && nWorldIndex < MAX_SUBWORLD)
			{
				const  PlayerCombatInfo & oldInfo = Player[nPlayerIndex].GetCombatInfo();
				ConfigManager           & mgr     = ConfigManager::Singleton();
				
				Player[nPlayerIndex].SetCombatInfoOrg(dwOrg);
				SubWorld[nWorldIndex].AddCombatInstanceOrgPerson(dwOrg,1);

				result = 1;
				
			}//endif
			
		}//endif

	}//endif

	Lua_PushNumber(L,result);
	return 1;
}

int luaDecPlayerCombatScore(Lua_State * L)
{
	int nRes         = INVALID_COMBAT_ORG_ID;
	int nPlayerIndex = ScriptGetPlayerIndex();

	if (Lua_GetTopIndex(L) != 1 && Lua_GetTopIndex(L) != 2)
	{
		Lua_PushNumber(L, nRes);
		return 1;
	}

	if (IsValidPlayer(nPlayerIndex))
	{
		int nScoreDec = Lua_ValueToNumber(L,1);

		int nEffectOrgScore = FALSE;	
		if (Lua_GetTopIndex(L) == 2)
			nEffectOrgScore = Lua_ValueToNumber(L, 2);

		nRes = Player[nPlayerIndex].DecCombatInfoScore(nScoreDec, log_event_combat_score_dec_script, nEffectOrgScore);
	}//endif

	Lua_PushNumber(L,nRes);
	return 1;	
}

int luaGetPlayerPrivateState(Lua_State * L)
{
	int result = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex))
	{
		result       = Player[nPlayerIndex].GetCamoflag(); 
	}//endif

	Lua_PushNumber(L,result);
	return 1;
}

int luaSetPlayerPrivateState(Lua_State * L)
{
	int result = 0;
	int nPlayerIndex  = ScriptGetPlayerIndex();
	if (IsValidPlayer(nPlayerIndex) && Lua_GetTopIndex(L) == 1)
	{
		int nComoFlag = Lua_ValueToNumber(L,1);

		if (nComoFlag == 1)
			Player[nPlayerIndex].SetComoflag(true);
		else 
			Player[nPlayerIndex].SetComoflag(false);

		result        = 1; 
	}//endif
	
	Lua_PushNumber(L,result);
	return 1;
}

int LuaGetInstanceCombatScore(Lua_State * L)
{
	int result = 0;
	if (Lua_GetTopIndex(L) == 2)
	{
		DWORD dwInstanceId = Lua_ValueToNumber(L, 1);
		int   nOrgId       = Lua_ValueToNumber(L ,2);
		
		int nSubWorldIndex = g_SubWorldSet.GetInstance(dwInstanceId);
		if (nSubWorldIndex != INVALID_WORLD_INDEX && nSubWorldIndex < MAX_SUBWORLD)
		{
			const CombatOrgnize * orgInfo = SubWorld[nSubWorldIndex].GetCombatInstanceOrgInfo(nOrgId);
			if (orgInfo)
			{
				result                    = orgInfo->nScore;
			}//endif
			
		}//endif
		
	}//endif
	
	Lua_PushNumber(L, result);
	return 1;
}

int LuaGetPlayerCombatOrg(Lua_State * L)
{
	int nRes         = INVALID_COMBAT_ORG_ID;
	int nPlayerIndex = ScriptGetPlayerIndex();

	if (IsValidPlayer(nPlayerIndex))
	{
		int nNpcIndex = Player[nPlayerIndex].GetNpcIndex();
		nRes          = Npc[nNpcIndex].m_WorldCombatOrg;
	}//endif

	Lua_PushNumber(L,nRes);
	return 1;	
}

int luaAddCombatScore(Lua_State * L)
{
	int nRes         = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();

	if (Lua_GetTopIndex(L) != 1 && Lua_GetTopIndex(L) != 2)
	{
		Lua_PushNumber(L, nRes);
		return 1;
	}

	int nScoreAdded  = Lua_ValueToNumber(L,1);
	if (nScoreAdded > 0)
	{
		if (IsValidPlayer(nPlayerIndex))	
		{
			int nEffectOrgScore = FALSE;
			if (Lua_GetTopIndex(L) == 2)
				nEffectOrgScore = Lua_ValueToNumber(L, 2);
			
			Player[nPlayerIndex].AddCombatScore(nScoreAdded,log_event_combat_score_add_script, nEffectOrgScore);
			nRes   =  1;
		}//endif
		
	}//endif
	

	Lua_PushNumber(L,nRes);
	return 1;
}

int LuaGetPlayerCombatScore(Lua_State * L)
{
	int nRes         = 0;
	int nPlayerIndex = ScriptGetPlayerIndex();
	
	if (IsValidPlayer(nPlayerIndex))
	{
		nRes  = Player[nPlayerIndex].GetCombatInfo().nScore;
	}//endif
	
	Lua_PushNumber(L,nRes);
	return 1;	
}

int luaSetCombatScorePoint(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 3)
	{
		DWORD   dwInstanceId = Lua_ValueToNumber(L, 1);
		DWORD   dwOrgId      = Lua_ValueToNumber(L, 2);
		int     nScore	     = Lua_ValueToNumber(L, 3);
		
		int nSubWorldIndex = g_SubWorldSet.GetInstance(dwInstanceId);
		
		if (nSubWorldIndex != INVALID_WORLD_INDEX && nSubWorldIndex < MAX_SUBWORLD)
		{
			if (SubWorld[nSubWorldIndex].IsWorldCombatMap() && SubWorld[nSubWorldIndex].GetCombatScoreCalcType() == SCRIPT_CALU)
			{
				SubWorld[nSubWorldIndex].SetOrgScore(dwOrgId, nScore);
			}
		}
	}

	return 0;
}

int luaSetTongWarCommanderSyncSwitch(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int nSwitch = Lua_ValueToNumber(L, 1);
		if (nSwitch == 0 || nSwitch == 1)
		{
			ConfigManager::Singleton().SetGlobalVariable(global_var_tong_war_commander_sync_switch, nSwitch);
		}
	}

	return 0;
}

int luaCanChangeLord(Lua_State * L)
{
	int nPlayerIndex = ScriptGetPlayerIndex();
	StatueInfoMgr& sm = StatueInfoMgr::Singleton();

	int res = sm.CanChangeLord(nPlayerIndex);

	Lua_PushNumber(L, res);
	return 1;
}

int luaGetStatueLordInfo(Lua_State * L)
{
	if (Lua_GetTopIndex(L) >= 3)
	{
		int nNpcIndex    = Lua_ValueToNumber(L, 1);
		if (!IsValidNpc(nNpcIndex))
			return 0;

		DWORD dwtime       = Lua_ValueToNumber(L, 2);
		int   nHasBuff     = Lua_ValueToNumber(L, 3);

		int nPlayerIndex = ScriptGetPlayerIndex();

		StatueInfoMgr& sm = StatueInfoMgr::Singleton();
		sm.SendStatueInfoToClient(nNpcIndex, nPlayerIndex, dwtime, nHasBuff);
	}
	
	return 0;
}

int luaClearInvalidStatue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int nMapId = Lua_ValueToNumber(L, 1);
		StatueInfoMgr& sm = StatueInfoMgr::Singleton();
		sm.ForceClearStatue(nMapId);
	}

	return 0;
}

int luaReStoreStatue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int nMapId = Lua_ValueToNumber(L, 1);
		StatueInfoMgr& sm = StatueInfoMgr::Singleton();
		sm.ReStoreStatue(nMapId);
	}

	return 0;
}

int LuaHasEnterableWorld(Lua_State * L)
{
	int result = 0;
	int playerIndex = INVALID_PLAYER_INDEX;
	int worldTemplateId = 0;
	if (Lua_GetTopIndex(L) >= 1)
	{
		int worldTemplateId = Lua_ValueToNumber(L, 1);
		if (Lua_GetTopIndex(L) == 2)
		{
			playerIndex = Lua_ValueToNumber(L, 2);
		}
		else
		{
			playerIndex = ScriptGetPlayerIndex();
		}
		
		WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(worldTemplateId);
		if (pSetting)
		{
			DWORD instanceId = g_SubWorldSet.GetEnterableInstanceID(playerIndex, worldTemplateId);
			if (instanceId != INVALID_INSTANCE_ID)
			{
				int worldIndex = g_SubWorldSet.GetInstance(instanceId);
				if (worldIndex != INVALID_WORLD_INDEX)
				{
					if (pSetting->IsMatchRequirements(playerIndex, worldIndex))
						result = 1;
				}
			}
		}
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaSetSpyLevel(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int playerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(playerIndex))
		{
			int spyLevel = Lua_ValueToNumber(L, 1);
			Player[playerIndex].SetSpyLevel(spyLevel);
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaIsInBigTeam(Lua_State * L)
{
	int isInBigTeam = FALSE;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		KPlayerTeam& teamInfo = Player[playerIndex].GetTeamInfo();
		if (teamInfo.IsInTeam())
		{
			KTeam* pTeam = teamInfo.GetTeam();
			if (pTeam)
			{
				isInBigTeam = pTeam->IsBigTeam() ? TRUE : FALSE;
			}
		}
	}

	Lua_PushNumber(L, isInBigTeam);
	return 1;
}
#endif

#ifdef _SERVER
int LuaKickPlayer(Lua_State * L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		Player[playerIndex].Kick();
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaSave(Lua_State * L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		Player[playerIndex].Save(NULL);
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaMonitorGlobalChat(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 1)
	{
		int on = Lua_ValueToNumber(L, 1);
		g_ChatCenterS.MonitorGlobalChat(TRUE == on);
	}

	return 0;
}
#endif

#ifdef _SERVER
//success = NpcFollow(npcIndex, followWaitSeconds)//NPC跟随，success:1=成功,0=失败
int LuaNpcFollow(Lua_State * L)
{
	int success = FALSE;
	if (Lua_GetTopIndex(L) == 2)
	{
		int npcIndex = Lua_ValueToNumber(L, 1);
		int followWaitTime = Lua_ValueToNumber(L, 2);
		if (IsValidNpc(npcIndex) && !Npc[npcIndex].IsPlayer() && followWaitTime >= 0)
		{
			int playerIndex = ScriptGetPlayerIndex();
			if (IsValidPlayer(playerIndex))
			{
				Npc[npcIndex].GetController().SetFollowNpc(Player[playerIndex].GetNpcIndex(), followWaitTime);
				success = TRUE;
			}
		}
	}

	Lua_PushNumber(L, success);
	return 1;
}
#endif

#ifdef _SERVER
/*
BOOL result = NewWorldNpc(int npcIndex, int worldId, int x, int y)
将NPC传送到一个普通地图；返回值：0-失败，1-成功

BOOL result = NewWorldNpc(int npcIndex, int worldId, int x, int y, BOOL isInstance)
将NPC传送到一个普通地图（isInstance=0）或者一个副本（isInstance=1,worldId=副本Id）；返回值：0-失败，1-成功
*/
int LuaNewWorldNpc(Lua_State * L)
{
	int result = 0;

	if (Lua_GetTopIndex(L) >= 4)
	{
		int npcIndex = Lua_ValueToNumber(L, 1);
		if (IsValidNpc(npcIndex))
		{
			DWORD dwWorldId = (DWORD)Lua_ValueToNumber(L, 2);
			int X = (int)Lua_ValueToNumber(L, 3) * 32;
			int Y = (int)Lua_ValueToNumber(L, 4) * 32;
			bool isInstance = false;
			if (Lua_GetTopIndex(L) >= 5)
			{
				isInstance = ((int)Lua_ValueToNumber(L, 5) == 1);
			}

			result = Npc[npcIndex].ChangeWorld(dwWorldId, X, Y, isInstance);
		}
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaSyncToWorld(Lua_State * L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		KNpc& npc = Npc[Player[playerIndex].GetNpcIndex()];

		const int paramCount = Lua_GetTopIndex(L);
		if (paramCount >= 1)
		{
			int mode = Lua_ValueToNumber(L, 1);			
			if (paramCount >= 2)
			{
				npc.SetSyncToWorldParam(0, Lua_ValueToNumber(L, 2));
				if (paramCount >= 3)
				{
					npc.SetSyncToWorldParam(1, Lua_ValueToNumber(L, 3));
				}
			}

			npc.SetSyncToWorldMode(mode);
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaGetNpcCustomVariable(Lua_State *L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		int npcIndex = Lua_ValueToNumber(L, 1);
		int varIndex = Lua_ValueToNumber(L, 2);
		if (IsValidNpc(npcIndex))
		{
			Lua_PushNumber(L, Npc[npcIndex].GetCustomVariable(varIndex));
			return 1;
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaSetNpcCustomVariable(Lua_State *L)
{
	if (Lua_GetTopIndex(L) == 3)
	{
		int npcIndex = Lua_ValueToNumber(L, 1);
		int varIndex = Lua_ValueToNumber(L, 2);
		int varValue = Lua_ValueToNumber(L, 3);
		if (IsValidNpc(npcIndex))
		{
			Npc[npcIndex].SetCustomVariable(varIndex, varValue);
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaGetRandomPos(Lua_State *L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		int mapid = Lua_ValueToNumber(L, 1);
		int group = Lua_ValueToNumber(L, 2);
		int worldIndex = g_SubWorldSet.SearchWorld(mapid);
		int randomPosX = 0;
		int randomPosY = 0;
		if (worldIndex >= 0)
		{
			if (SubWorld[worldIndex].GetRandomTransPos(randomPosX, randomPosY, group))
			{
				Lua_PushNumber(L, randomPosX);
				Lua_PushNumber(L, randomPosY);
				return 2;
			}
		}
	}

	return 0;
}
#endif

//金山币测试脚本
#ifdef _SERVER
int LuaTestChangeMoney(Lua_State *L)
{
	if (Lua_GetTopIndex(L) == 2)
	{
		int moneyChange = Lua_ValueToNumber(L, 1);
		int moneyType =  Lua_ValueToNumber(L, 2);
		int playerIndex = ScriptGetPlayerIndex();
		if (IsValidPlayer(playerIndex))
		{
			DWORD money = Player[playerIndex].GetIBMoney((MoneyType)moneyType);
			if (moneyChange < 0 && money < -moneyChange)
			{
				money = 0;
			}
			else
			{
				money += moneyChange;
			}

			Player[playerIndex].SetIBMoney((MoneyType)moneyType,&money);				
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaInstanceSummary(Lua_State *L)
{
	int mode = 0;//发送给Guard（0）还是Script（1）
	if (Lua_GetTopIndex(L) == 1)
	{
		mode = Lua_ValueToNumber(L, 1);
	}

	int activeCount = 0;
	int idleCount = 0;
	int readyForReuse = 0;
	int freeCount = 0;
	
	for (int worldIndex = INSTANCE_SUBWORLD_START; worldIndex < INSTANCE_SUBWORLD_END; worldIndex++)
	{
		switch(SubWorld[worldIndex].GetState())
		{
		case world_state_init:
			freeCount++;
			break;
		case world_state_active:
			activeCount++;
			break;
		case world_state_idle:
			idleCount++;
			break;
		case world_state_ready_for_reuse:
			readyForReuse++;
			break;
		}
	}

	char instanceSummary[1024] = { 0 };
	snprintf(instanceSummary, sizeof(instanceSummary), "ActiveCount=%d\nIdleCount=%d\nReadyForReuseCount=%d\nFreeCount=%d", activeCount, idleCount, readyForReuse, freeCount);
	instanceSummary[sizeof(instanceSummary) - 1] = 0;

	if (mode == 0)
	{
		l2e_info info = { 0 };
		info.Header.Protocol = l2e_header_def;
		info.Protocol = l2e_info_def;
		snprintf(info.Info, sizeof(info.Info), "content=InstanceSummary\n%s", instanceSummary);
		info.Info[sizeof(info.Info) - 1] = 0;

		if (g_pController != NULL)
			g_pController->PushData(protocol_type_guard, NULL, &info, sizeof(info));
		return 0;
	}
	else
	{
		int maxMsgSize = sizeof(instanceSummary) < MAXSIZE_CHAT_MSG ? sizeof(instanceSummary) : MAXSIZE_CHAT_MSG;
		instanceSummary[maxMsgSize - 1] = 0;
		Lua_PushString(L, instanceSummary);
		return 1;
	}
}

//for FSEye
int LuaGetCityResEx(Lua_State* L)
{
	int nMode  = 0;//发送给Guard（0）还是Script（1）
	int nMapID = INVALID_WORLD_ID;

	if ( Lua_GetTopIndex(L) == 2 )
	{
		nMode  = Lua_ValueToNumber(L, 1);
		nMapID = Lua_ValueToNumber(L, 2);
	}
	else
	{
		return 0;
	}

	char ResDataStr[1024] = { 0 };
	int nLordNpcIndex = GetLordByMapID(nMapID);
	if ( IsValidNpc(nLordNpcIndex) )
	{	
		int nMoney  = Npc[nLordNpcIndex].m_UnaryAttrMgr[nuai_lord_res0];	//金钱
		int nCopper = Npc[nLordNpcIndex].m_UnaryAttrMgr[nuai_lord_res1];	//青铜
		int nRed    = Npc[nLordNpcIndex].m_UnaryAttrMgr[nuai_lord_res2];	//灵丹
		int nJade	= Npc[nLordNpcIndex].m_UnaryAttrMgr[nuai_lord_res3];	//昆玉
		
		snprintf(ResDataStr, sizeof(ResDataStr), "MapID=%d\nMoney=%d\nCopper=%d\nRed=%d\nJade=%d", nMapID, nMoney, nCopper, nRed, nJade);
		ResDataStr[sizeof(ResDataStr) - 1] = 0;
	}
	else
	{
		snprintf(ResDataStr, sizeof(ResDataStr), "N/A");
		ResDataStr[sizeof(ResDataStr) - 1] = 0;
	}

	if ( nMode == 0 )
	{
		l2e_info info = { 0 };
		info.Header.Protocol = l2e_header_def;
		info.Protocol = l2e_info_def;
		snprintf(info.Info, sizeof(info.Info), "content=CityResSummary\n%s", ResDataStr);
		info.Info[sizeof(info.Info) - 1] = 0;
		
		if (g_pController != NULL)
			g_pController->PushData(protocol_type_guard, NULL, &info, sizeof(info));
		return 0;
	}
	else
	{
		return 0;
	}
}

int LuaGetCaptureCityTime(Lua_State* L)
{
	int nMode  = 0;
	int nMapID = INVALID_WORLD_ID;

	if ( Lua_GetTopIndex(L) == 2 )
	{
		nMode  = Lua_ValueToNumber(L, 1);
		nMapID = Lua_ValueToNumber(L, 2);
	}
	else
	{
		return 0;
	}

	DWORD dwTime = 0;
	char CaptureCityStr[1024] = { 0 };
	SocialUnit* pUnit = GetLordSocialUnitByMapID(nMapID);
	if ( pUnit )	//这里是用来判断城市是否是被占领的
	{
		KEconomySysManager::Singleton().GetAttrValue(nMapID, attrtype_scriptvariable, CAPTURE_CITY_TIME_ITER, dwTime);
	
		snprintf(CaptureCityStr, sizeof(CaptureCityStr), "MapID=%d\nCaptureCitySecond=%u", nMapID, UNIX_TMIE_STAMP - dwTime);
		CaptureCityStr[sizeof(CaptureCityStr) - 1] = 0;
	}
	else
	{
		snprintf(CaptureCityStr, sizeof(CaptureCityStr), "N/A");
		CaptureCityStr[sizeof(CaptureCityStr) - 1] = 0;
	}

	if ( nMode == 0 )
	{
		l2e_info info = { 0 };
		info.Header.Protocol = l2e_header_def;
		info.Protocol = l2e_info_def;
		snprintf(info.Info, sizeof(info.Info), "content=CaptureCityTimeSummary\n%s", CaptureCityStr);
		info.Info[sizeof(info.Info) - 1] = 0;
		
		if (g_pController != NULL)
			g_pController->PushData(protocol_type_guard, NULL, &info, sizeof(info));
		return 0;
	}
	else
	{
		Lua_PushNumber(L, UNIX_TMIE_STAMP - dwTime);
		return 1;
	}
}

int LuaGetCityPopulation(Lua_State* L)
{
	int  nMode = 0;
	int nMapID = INVALID_WORLD_ID;

	if ( Lua_GetTopIndex(L) == 2 )
	{
		nMode  = Lua_ValueToNumber(L, 1);
		nMapID = Lua_ValueToNumber(L, 2);
	}
	else
	{
		return 0;	
	}

	char PopulationStr[1024] = { 0 };
	SocialUnit* pUnit = GetLordSocialUnitByMapID(nMapID);
	if ( pUnit )
	{
		int nNumbers = pUnit->GetTotalPlayerNum();
		DWORD dwActivePopulation = 0;
		KEconomySysManager::Singleton().GetAttrValue(nMapID, attrtype_needsave, needsave_economy_population, dwActivePopulation);
			
		snprintf(PopulationStr, sizeof(PopulationStr), "MapID=%d\nCityPopulation=%d\nCityActivePopulation=%u", nMapID, nNumbers, dwActivePopulation);
		PopulationStr[sizeof(PopulationStr) - 1] = 0;
	}
	else
	{
		snprintf(PopulationStr, sizeof(PopulationStr), "N/A");
		PopulationStr[sizeof(PopulationStr) - 1] = 0;
	}
	
	if ( nMode == 0 )
	{
		l2e_info info = { 0 };
		info.Header.Protocol = l2e_header_def;
		info.Protocol = l2e_info_def;
		snprintf(info.Info, sizeof(info.Info), "content=CityPopulationSummary\n%s", PopulationStr);
		info.Info[sizeof(info.Info) - 1] = 0;
		
		if (g_pController != NULL)
			g_pController->PushData(protocol_type_guard, NULL, &info, sizeof(info));
		return 0;
	}
	else
	{
		return 0;
	}
}
//end
#endif

#ifdef _SERVER
int LuaInstanceDetail(Lua_State *L)
{
	int mode = 0;//发送给Guard（0）还是Script（1）
	int startId = 1;//起始地图编号
	if (Lua_GetTopIndex(L) >= 1)
	{
		mode = Lua_ValueToNumber(L, 1);

		if (Lua_GetTopIndex(L) >= 2)
		{
			startId = Lua_ValueToNumber(L, 2);
		}
	}

	char allInstanceDetail[1024] = { 0 };
	snprintf(allInstanceDetail, sizeof(allInstanceDetail), "StartMap=%d\n", startId);
	char instanceDetail[128] = { 0 };
	for (int worldTeamplateId = startId; worldTeamplateId < INSTANCE_SUBWORLD_START; worldTeamplateId++)
	{
		WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(worldTeamplateId);
		if (pSetting && pSetting->IsInstance)
		{
			snprintf(instanceDetail, sizeof(instanceDetail), "Map_%d:C=%d,L=%d,R=%d\n", pSetting->MapId, pSetting->RuntimeInfo.CreateInstanceCount, pSetting->RuntimeInfo.LivingInstanceCount, pSetting->RuntimeInfo.ReuseableInstanceCount);
			instanceDetail[sizeof(instanceDetail) - 1] = 0;
			
			if (strlen(allInstanceDetail) + strlen(instanceDetail) >= sizeof(allInstanceDetail))
			{
				break;
			}
			else
			{
				strcat(allInstanceDetail, instanceDetail);
			}
		}
	}	
	allInstanceDetail[sizeof(allInstanceDetail) - 1] = 0;
 
	if (mode == 0)
	{
		l2e_info info = { 0 };
		info.Header.Protocol = l2e_header_def;
		info.Protocol = l2e_info_def;
		int copySize = sizeof(info.Info) < sizeof(allInstanceDetail) ? sizeof(info.Info) : sizeof(allInstanceDetail);
		snprintf(info.Info, sizeof(info.Info), "content=InstanceDetail\n%s", allInstanceDetail);
		info.Info[sizeof(info.Info) - 1] = 0;

		if (g_pController != NULL)
			g_pController->PushData(protocol_type_guard, NULL, &info, sizeof(info));
		return 0;
	}
	else
	{
		int maxMsgSize = sizeof(allInstanceDetail) < MAXSIZE_CHAT_MSG ? sizeof(allInstanceDetail) : MAXSIZE_CHAT_MSG;
		allInstanceDetail[maxMsgSize - 1] = 0;
		Lua_PushString(L, allInstanceDetail);
		return 1;
	}
}
#endif

#ifdef _SERVER
int LuaItemAndObjSummary(Lua_State *L)
{
	int mode = 0;//发送给Guard（0）还是Script（1）
	if (Lua_GetTopIndex(L) == 1)
	{
		mode = Lua_ValueToNumber(L, 1);
	}

	int usedItemCount = ItemSet.GetItemCount(-1);
	int usedObjCount = ObjSet.m_UsedObjCount;

	char instanceSummary[1024] = { 0 };
	snprintf(instanceSummary, sizeof(instanceSummary), "UsedItemCount=%d\nUsedObjCount=%d", usedItemCount, usedObjCount);
	instanceSummary[sizeof(instanceSummary) - 1] = 0;

	if (mode == 0)
	{
		l2e_info info = { 0 };
		info.Header.Protocol = l2e_header_def;
		info.Protocol = l2e_info_def;
		snprintf(info.Info, sizeof(info.Info), "content=ItemAndObjSummary\n%s", instanceSummary);
		info.Info[sizeof(info.Info) - 1] = 0;

		if (g_pController != NULL)
			g_pController->PushData(protocol_type_guard, NULL, &info, sizeof(info));
		return 0;
	}
	else
	{
		int maxMsgSize = sizeof(instanceSummary) < MAXSIZE_CHAT_MSG ? sizeof(instanceSummary) : MAXSIZE_CHAT_MSG;
		instanceSummary[maxMsgSize - 1] = 0;
		Lua_PushString(L, instanceSummary);
		return 1;
	}
}
#endif

#ifdef _SERVER
int LuaRecordPlayerReward(Lua_State *L)
{
	unsigned int rewardTag = Lua_ValueToNumber(L, 1);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		Player[playerIndex].RecordPlayerReward(rewardTag);
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaRecordPlayerContact(Lua_State *L)
{
	unsigned int rewardTag = Lua_ValueToNumber(L, 1);
	const char* realName = Lua_ValueToString(L, 2);
	unsigned int sex = Lua_ValueToNumber(L, 3);
	const char* tel = Lua_ValueToString(L, 4);
	const char* email = Lua_ValueToString(L, 5);
	const char* address = Lua_ValueToString(L, 6);
	const char* code = Lua_ValueToString(L, 7);

	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		Player[playerIndex].RecordPlayerContact(rewardTag, realName, sex, tel, email, address, code);
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaLeaveInstance(Lua_State *L)
{
	int result = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		result = Player[playerIndex].TransferToRememberPos();
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaNotifyGMReply(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		Player[playerIndex].GetGMReply();
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaPost(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		if (Lua_GetTopIndex(L) >= 3)
		{
			int type = Lua_ValueToNumber(L, 1);
			int salary = Lua_ValueToNumber(L, 2);
			int rateIndex = Lua_ValueToNumber(L, 3);

			EmployCenter::Singleton().Post(playerIndex, type, salary, rateIndex);
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaCancel(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		EmployCenter::Singleton().Cancel(playerIndex);
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaEmploy(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		if (Lua_GetTopIndex(L) >= 1)
		{
			const char* employeeName = Lua_ValueToString(L, 1);
			Employee& employee = Player[playerIndex].GetEmployee();

			int employTime = 0;
			if (Lua_GetTopIndex(L) >= 2)
			{
				employTime = Lua_ValueToNumber(L, 2);
			}

			if (employee.IsExist())
			{
				employee.Fire();
			}
			if (!employee.IsExist())
			{
				EmployCenter::Singleton().Employ(playerIndex, employeeName, employTime);
			}
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaFire(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		Employee& employee = Player[playerIndex].GetEmployee();
		if (employee.IsExist())
		{
			employee.Fire();
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaSetEmployTime(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		if (Lua_GetTopIndex(L) >= 1)
		{
			int employTime = (int)Lua_ValueToNumber(L, 1);
			if (employTime >= 0 && employTime < MAX_EMPLOY_TIME_LIMIT)
			{
				Player[playerIndex].SetEmployTime((DWORD)employTime);
			}
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaAddEmployTime(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		if (Lua_GetTopIndex(L) >= 1)
		{
			int employTime = (int)Lua_ValueToNumber(L, 1);
			DWORD originalTime = Player[playerIndex].GetEmployTime();
			if (employTime > 0 && employTime < MAX_ADD_EMPLOY_TIME && originalTime < MAX_EMPLOY_TIME_LIMIT && ((DWORD)employTime + originalTime) < MAX_EMPLOY_TIME_LIMIT)
			{
				Player[playerIndex].SetEmployTime((DWORD)employTime + originalTime);
			}
		}
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaGetEmployTime(Lua_State *L)
{
	DWORD employTime = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		employTime = Player[playerIndex].GetEmployTime();
	}

	Lua_PushNumber(L, employTime);
	return 1;
}
#endif

#ifdef _SERVER
int LuaHasEmployee(Lua_State *L)
{
	int hasEmployee = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		hasEmployee = Player[playerIndex].GetEmployee().IsExist() ? 1 : 0;
	}

	Lua_PushNumber(L, hasEmployee);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetInteractiveScriptStep(Lua_State *L)
{
	int step = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		step = Player[playerIndex].GetInteractiveScriptStep();
	}

	Lua_PushNumber(L, step);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetInteractiveScriptIntParam(Lua_State *L)
{
	int intParam = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		int paramIndex = Lua_ValueToNumber(L, 1);
		intParam = Player[playerIndex].GetInteractiveScriptIntParam(paramIndex);
	}

	Lua_PushNumber(L, intParam);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetInteractiveScriptStrParam(Lua_State *L)
{
	char strParam[INTERACTIVE_SCRIPT_STR_PARAM_LENGTH] = { 0 };
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		int paramIndex = Lua_ValueToNumber(L, 1);
		Player[playerIndex].GetInteractiveScriptStrParam(paramIndex, strParam, sizeof(strParam));
	}

	Lua_PushString(L, strParam);
	return 1;
}
#endif

#ifdef _SERVER
int LuaInteractiveScriptDone(Lua_State *L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		Player[playerIndex].InteractiveScriptDone();
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaDoRevive(Lua_State *L)
{	
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		//重生
		Player[playerIndex].Revive( FALSE );

		//补满血蓝
		KNpc& npc = Npc[Player[playerIndex].GetNpcIndex()];
		npc.m_UnaryAttrMgr.Set(nuai_curlife, npc.m_CompAttrMgr[ncai_lifeuplimit][idx_current_value]);
		npc.m_UnaryAttrMgr.Set(nuai_curmana, npc.m_CompAttrMgr[ncai_manauplimit][idx_current_value]);		
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaNoChat(Lua_State * L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		if (Lua_GetTopIndex(L) == 1)
		{
			DWORD noChatTime = (DWORD)Lua_ValueToNumber(L, 1);
			Player[playerIndex].NoChat(noChatTime);
		}
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaWithdraw(Lua_State * L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		if (Lua_GetTopIndex(L) == 1)
		{
			const char* sn = Lua_ValueToString(L, 1);
			Player[playerIndex].Withdraw(sn);
		}
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaStartInteractiveScript(Lua_State * L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		int paramCount = Lua_GetTopIndex(L);
		if (paramCount >= 2)
		{
			const char* scriptPath = Lua_ValueToString(L, 1);
			const char* funcName = Lua_ValueToString(L, 2);
			char scriptPathBuff[64] = { 0 };
			strncpy(scriptPathBuff, scriptPath, sizeof(scriptPathBuff));
			scriptPathBuff[sizeof(scriptPathBuff) - 1] = 0;

			if (scriptPath && funcName)
			{
				DWORD scriptId = g_FileName2Id(scriptPathBuff);				
				int param1 = 0;
				int param2 = 0;
				int param3 = 0;
				
				if (paramCount >= 3)
				{
					param1 = Lua_ValueToNumber(L, 3);
					
					if (paramCount >= 4)
					{
						param2 = Lua_ValueToNumber(L, 4);
						
						if (paramCount >= 5)
						{
							param3 = Lua_ValueToNumber(L, 5);
						}
					}
				}			
				
				Player[playerIndex].ExecuteInteractiveScript(scriptId, funcName, param1, param2, param3);
			}
		}
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaIsInInstance(Lua_State * L)
{
	int isInstance = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		int npcIndex = Player[playerIndex].GetNpcIndex();
		if(IsValidNpc(npcIndex))
		{
			KNpc& npc = Npc[npcIndex];
			WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(SubWorld[npc.GetSubWorldIndex()].GetWorldTemplateId());
			if (pSetting && pSetting->IsInstance)
				isInstance = 1;
		}
	}

	Lua_PushNumber(L, isInstance);
	return 1;
}
#endif

#ifdef _SERVER
int LuaAddStudent(Lua_State * L)
{
	int result = 0;
	int playerIndex = ScriptGetPlayerIndex();
	int paramCount = Lua_GetTopIndex(L);
	if (IsValidPlayer(playerIndex) && paramCount == 1)
	{
		const char* scriptRecommenderName = Lua_ValueToString(L, 1);
		result = Player[playerIndex].AddStudent(scriptRecommenderName);
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaUpdateStudent(Lua_State * L)
{
	int result = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		result = Player[playerIndex].UpdateStudent();
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaListStudent(Lua_State * L)
{
	int result = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		result = Player[playerIndex].ListStudent();
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaListMaster(Lua_State * L)
{
	int result = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		result = Player[playerIndex].ListMaster();
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetRememberPos(Lua_State * L)
{
	int subworldId = 0;
	int posX = 0;
	int posY = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		Player[playerIndex].GetRememberPos(subworldId, posX, posY);
	}

	Lua_PushNumber(L, subworldId);
	Lua_PushNumber(L, posX);
	Lua_PushNumber(L, posY);
	return 3;
}
#endif

#ifdef _SERVER
int LuaGetCreateTime(Lua_State * L)
{
	DWORD createUnixTimestamp = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		createUnixTimestamp = Player[playerIndex].GetCreateTime();
	}

	Lua_PushNumber(L, createUnixTimestamp);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetPlayerAttribute(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(playerIndex) || paramCount < 3)
		return 0;
	int npcIndex = Player[playerIndex].GetNpcIndex();
	if (!IsValidNpc(npcIndex))
		return 0;

	KNpc& npc = Npc[npcIndex];

	int mode = Lua_ValueToNumber(L, 1);//发送给Guard（0）还是Script（1）
	int attrType = Lua_ValueToNumber(L, 2);//属性类型：m_UnaryAttrMgr（0），m_CompAttrMgr（1），m_RangeAttrMgr（2）
	int attrIndex = Lua_ValueToNumber(L, 3);//属性序号

	char runtimeInfo[1024] = { 0 };
	snprintf(runtimeInfo, sizeof(runtimeInfo), "Name=\"%s\"\n", npc.Name);

	char attributeDesc[1024] = { 0 };
	switch (attrType)
	{
	case 0:
		{
			if (attrIndex >= 0 && attrIndex < nuai_end)
			{
				snprintf(attributeDesc, sizeof(attributeDesc),
					"Type=%d,Index=%d,Value=%d\n",
					attrType,
					attrIndex,
					npc.m_UnaryAttrMgr[attrIndex]);
			}
			else
			{
				snprintf(attributeDesc, sizeof(attributeDesc), "Invalid AttributeIndex Type=%d,Index=%d\n", attrType, attrIndex);
			}
		}
		break;
	case 1:
		{
			if (attrIndex >= 0 && attrIndex < ncai_end)
			{
				snprintf(attributeDesc, sizeof(attributeDesc),
					"Type=%d,Index=%d,Base=%d,Append=%d,AppendPercent=%d,Current=%d\n", 
					attrType,
					attrIndex,
					npc.m_CompAttrMgr[attrIndex][idx_base_value],
					npc.m_CompAttrMgr[attrIndex][idx_append_value],
					npc.m_CompAttrMgr[attrIndex][idx_append_percent],
					npc.m_CompAttrMgr[attrIndex][idx_current_value]);
			}
			else
			{
				snprintf(attributeDesc, sizeof(attributeDesc), "Invalid AttributeIndex Type=%d,Index=%d\n", attrType, attrIndex);
			}
		}
		break;
	case 2:
		{
			if (attrIndex >= 0 && attrIndex < nrai_end)
			{
				snprintf(attributeDesc, sizeof(attributeDesc),
					"Type=%d,Index=%d,LowBase=%d,LowAppend=%d,LowAppendPercent=%d,LowCurrent=%d,HighBase=%d,HighAppend=%d,HighAppendPercent=%d,HighCurrent=%d\n", 
					attrType,
					attrIndex,
					npc.m_RangeAttrMgr[attrIndex][idx_value_low][idx_base_value],
					npc.m_RangeAttrMgr[attrIndex][idx_value_low][idx_append_value],
					npc.m_RangeAttrMgr[attrIndex][idx_value_low][idx_append_percent],
					npc.m_RangeAttrMgr[attrIndex][idx_value_low][idx_current_value],
					npc.m_RangeAttrMgr[attrIndex][idx_value_hight][idx_base_value],
					npc.m_RangeAttrMgr[attrIndex][idx_value_hight][idx_append_value],
					npc.m_RangeAttrMgr[attrIndex][idx_value_hight][idx_append_percent],
					npc.m_RangeAttrMgr[attrIndex][idx_value_hight][idx_current_value]);
			}
			else
			{
				snprintf(attributeDesc, sizeof(attributeDesc), "Invalid AttributeIndex Type=%d,Index=%d\n", attrType, attrIndex);
			}
		}
		break;
	default:
		{
			snprintf(attributeDesc, sizeof(attributeDesc), "Invalid AttributeType\n");
		}
		break;
	}

	attributeDesc[sizeof(attributeDesc) -1] = 0;
	strncat(runtimeInfo, attributeDesc, sizeof(runtimeInfo));
	runtimeInfo[sizeof(runtimeInfo) - 1] = 0;
 
	if (mode == 0)
	{
		l2e_info info = { 0 };
		info.Header.Protocol = l2e_header_def;
		info.Protocol = l2e_info_def;
		int copySize = sizeof(info.Info) < sizeof(runtimeInfo) ? sizeof(info.Info) : sizeof(runtimeInfo);
		snprintf(info.Info, sizeof(info.Info), "content=PlayerRuntimeInfo\n%s", runtimeInfo);
		info.Info[sizeof(info.Info) - 1] = 0;

		if (g_pController != NULL)
			g_pController->PushData(protocol_type_guard, NULL, &info, sizeof(info));
		return 0;
	}
	else
	{
		int maxMsgSize = sizeof(runtimeInfo) < MAXSIZE_CHAT_MSG ? sizeof(runtimeInfo) : MAXSIZE_CHAT_MSG;
		runtimeInfo[maxMsgSize - 1] = 0;
		Lua_PushString(L, runtimeInfo);
		return 1;
	}

	return 0;
}
#endif

#ifdef _SERVER
int LuaCostLockedItemVolume(Lua_State * L)
{
	int result = 0;

	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (!IsValidPlayer(playerIndex) || paramCount < 5)
	{
		result = 0;
		Lua_PushNumber(L, result);
		return 1;
	}

	int itemGenre = Lua_ValueToNumber(L, 1);
	int itemDetail = Lua_ValueToNumber(L, 2);
	int itemParticular = Lua_ValueToNumber(L, 3);
	int itemLevel = Lua_ValueToNumber(L, 4);
	int costVolume = Lua_ValueToNumber(L, 5);

	if (costVolume <= 0)
	{
		result = 0;
		Lua_PushNumber(L, result);
		return 1;
	}

	result = -1;
	int findItemIndex = 0;
	int itemPosX = 0;
	int itemPosY = 0;
	if (TRUE == Player[playerIndex].GetItemList().FindLockedParticularItem(itemGenre, itemDetail, itemParticular, &findItemIndex, &itemPosX, &itemPosY))
	{
		if (findItemIndex > 0 && findItemIndex < MAX_ITEM)
		{
			result = 0;
			KItem& findItem = Item[findItemIndex];

			int nCurDur = findItem.GetDurability();
			if (nCurDur > 0)
			{
				int realCost = 0;
				if (nCurDur < costVolume)
				{
					realCost = nCurDur;
				}
				else
				{
					realCost = costVolume;
				}

				findItem.SetDurability(nCurDur - realCost);				
				findItem.SyncAttribute(item_attr_durability, Player[playerIndex].GetNetConnectIdx());

				result = realCost;
			}
		}
	}

	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetNpcName(Lua_State * L)
{
	char npcName[32] = { 0 };
	int paramCount = Lua_GetTopIndex(L);
	if (paramCount >= 1)
	{
		int npcIndex = Lua_ValueToNumber(L, 1);
		if (IsValidNpc(npcIndex))
		{
			strncpy(npcName, Npc[npcIndex].Name, sizeof(npcName));
		}
	}

	npcName[sizeof(npcName) - 1] = 0;
	Lua_PushString(L, npcName);
	return 1;
}
#endif

#ifdef _SERVER
int LuaIsInstanceExist(Lua_State * L)
{
	int isExist = FALSE;
	int paramCount = Lua_GetTopIndex(L);
	if (1 == paramCount)
	{
		DWORD instanceId = Lua_ValueToNumber(L, 1);
		int worldIndex = g_SubWorldSet.GetInstance(instanceId);
		if (worldIndex != INVALID_WORLD_INDEX)
		{
			isExist = TRUE;
		}
	}
	
	Lua_PushNumber(L, isExist);
	return 1;
}
#endif

#ifdef _SERVER
int LuaReloadQuestionSettings(Lua_State * L)
{
	QuestionManager::Singleton().ReloadAllSettings();

	return 0;
}
#endif

#ifdef _SERVER
int LuaNewQuestion(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	int result = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		int keepCount = 1;
		if (paramCount >= 1)
		{
			keepCount = Lua_ValueToNumber(L, 1);
			if (keepCount <= 0)
			{
				keepCount = 1;
			}
		}
		
		int forbidTime = QuestionManager::Singleton().GetForbidQuestionTime();
		if (paramCount >= 2)
		{
			forbidTime = Lua_ValueToNumber(L, 2);
			if (forbidTime < 0)
			{
				forbidTime = QuestionManager::Singleton().GetForbidQuestionTime();
			}
		}
		
		int timeout = QuestionManager::Singleton().GetTimeout();
		if (paramCount >= 3)
		{
			timeout = Lua_ValueToNumber(L, 3);
			if (timeout < 0)
			{
				timeout = QuestionManager::Singleton().GetTimeout();
			}
		}
		
		if (Player[playerIndex].GetQuestionState().NewQuestion(keepCount, forbidTime, timeout, NULL, NULL))
		{
			result = 1;
		}
	}
	
	Lua_PushNumber(L, result);
	return 1;
}
#endif

#ifdef _SERVER
int LuaAntiRobot(Lua_State * L)
{
	int isHuman = 0;
	
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		if (Player[playerIndex].GetQuestionState().IsHuman())
		{
			Player[playerIndex].GetQuestionState().UseKeepCount();
			isHuman = 1;
		}
		else
		{
			CallbackScriptParam callbackParam;
			Player[playerIndex].GetQuestionState().GetTempParam(callbackParam);

			int keepCount = 1;
			if (paramCount >= 1)
			{
				keepCount = Lua_ValueToNumber(L, 1);
				if (keepCount <= 0)
				{
					keepCount = 1;
				}
			}

			int forbidTime = QuestionManager::Singleton().GetForbidQuestionTime();
			if (paramCount >= 2)
			{
				forbidTime = Lua_ValueToNumber(L, 2);
				if (forbidTime < 0)
				{
					forbidTime = QuestionManager::Singleton().GetForbidQuestionTime();
				}
			}
			
			int timeout = QuestionManager::Singleton().GetTimeout();
			if (paramCount >= 3)
			{
				timeout = Lua_ValueToNumber(L, 3);
				if (timeout < 0)
				{
					timeout = QuestionManager::Singleton().GetTimeout();
				}
			}
			
			Player[playerIndex].GetQuestionState().NewQuestion(keepCount, forbidTime, timeout, &callbackParam, NULL);
		}
	}
	
	Lua_PushNumber(L, isHuman);
	return 1;
}
#endif

#ifdef _SERVER
int LuaResetBadAnswerState(Lua_State * L)
{
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
		Player[playerIndex].GetQuestionState().ResetBadAnswerState();
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaGetSubWorldIdxFromInstanceID(Lua_State * L)
{
	int subworldIndex = INVALID_WORLD_INDEX;

	int paramCount = Lua_GetTopIndex(L);
	if (paramCount > 0)
	{
		DWORD instanceId = Lua_ValueToNumber(L, 1);
		subworldIndex = g_SubWorldSet.GetInstance(instanceId);
	}

	Lua_PushNumber(L, subworldIndex);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetInstanceIDFromSubWorldIdx(Lua_State * L)
{
	DWORD instanceId = INVALID_INSTANCE_ID;

	int paramCount = Lua_GetTopIndex(L);
	if (paramCount > 0)
	{
		int subworldIndex = Lua_ValueToNumber(L, 1);
		if (subworldIndex >= 0 && subworldIndex < MAX_SUBWORLD)
		{
			instanceId = SubWorld[subworldIndex].GetInstanceId();
		}
	}
	
	Lua_PushNumber(L, instanceId);
	return 1;
}
/****************EconomySys**********************/
int LuaGetEconomyValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 3)
	{
		int nMapId    = Lua_ValueToNumber(L, 1);
		int nAttrType = Lua_ValueToNumber(L, 2);
		int nAttrId   = Lua_ValueToNumber(L, 3);

		DWORD nRet;
		KEconomySysManager& esm = KEconomySysManager::Singleton();
		bool bSuccess = esm.GetAttrValue(nMapId, nAttrType, nAttrId, nRet);
		
		if (bSuccess)
		{
			Lua_PushNumber(L, 1);
			Lua_PushNumber(L, nRet);
			return 2;
		}
	}

	Lua_PushNumber(L, 0);
	Lua_PushNumber(L, -1);

	return 2;
}

int LuaSetEconomyValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 4)
	{
		int nMapId    = Lua_ValueToNumber(L, 1);
		int nAttrType = Lua_ValueToNumber(L, 2);
		int nAttrId   = Lua_ValueToNumber(L, 3);
		DWORD nNewV     = Lua_ValueToNumber(L, 4);
		
		KEconomySysManager& esm = KEconomySysManager::Singleton();
		bool bSuccess = esm.SetAttrValue(nMapId, nAttrType, nAttrId, nNewV);

		if (bSuccess)
		{
			Lua_PushNumber(L, 1);
			return 1;
		}
	}

	Lua_PushNumber(L, 0);
	return 1;
}

int LuaAddEconomyValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 4)
	{
		int nMapId    = Lua_ValueToNumber(L, 1);
		int nAttrType = Lua_ValueToNumber(L, 2);
		int nAttrId   = Lua_ValueToNumber(L, 3);
		DWORD nInc      = Lua_ValueToNumber(L, 4);
		
		KEconomySysManager& esm = KEconomySysManager::Singleton();
		bool bSuccess = esm.AddAttrValue(nMapId, nAttrType, nAttrId, nInc);

		if (bSuccess)
		{
			Lua_PushNumber(L, 1);
			return 1;
		}
	}
	
	Lua_PushNumber(L, 0);
	return 0;
}

int LuaDecEconomyValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 4)
	{
		int nMapId    = Lua_ValueToNumber(L, 1);
		int nAttrType = Lua_ValueToNumber(L, 2);
		int nAttrId   = Lua_ValueToNumber(L, 3);
		DWORD nDec    = Lua_ValueToNumber(L, 4);
		
		KEconomySysManager& esm = KEconomySysManager::Singleton();
		bool bSuccess = esm.DecAttrValue(nMapId, nAttrType, nAttrId, nDec);

		if (bSuccess)
		{
			Lua_PushNumber(L, 1);
			return 1;
		}
	}
	
	Lua_PushNumber(L, 0);
	return 1;
}

#define STR0 "Population"
#define STR1 "BaseDevelopment"
#define STR2 "Tiredness"
#define STR3 "PopulationProportion"
#define STR4 "Increment of Develop"
	
#define STR "%d Map %s Value is %u\n"
#define STRERROR "Sreach error!\n"

int LuaShowEconomyValue(Lua_State * L)
{
	if (Lua_GetTopIndex(L) == 3)
	{
		int nPlayerIndex = ScriptGetPlayerIndex();
		int nMapId    = Lua_ValueToNumber(L, 1);
		int nAttrType = Lua_ValueToNumber(L, 2);
		int nAttrId   = Lua_ValueToNumber(L, 3);

		KEconomySysManager& esm = KEconomySysManager::Singleton();
		DWORD nRet;
		bool bSuccess = esm.GetAttrValue(nMapId, nAttrType, nAttrId, nRet);

		char szString[150];
		if (bSuccess && (nAttrType == attrtype_needsave || nAttrType == attrtype_notneedsave))
		{
			if (nAttrType == attrtype_needsave)
			{
				switch (nAttrId)
				{
				case 0:
					snprintf(szString, sizeof(szString), STR, nMapId, STR0, nRet);
					break;
				case 1:
					snprintf(szString, sizeof(szString), STR, nMapId, STR1, nRet);
					break;
				case 2:
					snprintf(szString, sizeof(szString), STR, nMapId, STR2, nRet);
					break;
				}
			}
			
			if (nAttrType == attrtype_notneedsave)
			{
				switch (nAttrId)
				{
				case 0:
					snprintf(szString, sizeof(szString), STR, nMapId, STR3, nRet);
					break;
				case 1:
					snprintf(szString, sizeof(szString), STR, nMapId, STR4, nRet);
					break;
				}
			}
			
			szString[sizeof(szString) - 1] = 0;
			if (szString)
				g_ChatCenterS.SysMsgToSomeone(nPlayerIndex, SYSMSG_TYPE_STR, (const BYTE*)szString, strlen(szString));
			return 0;
		}

		snprintf(szString, sizeof(szString), STRERROR);
		szString[sizeof(szString) - 1] = 0;
		if (szString)
				g_ChatCenterS.SysMsgToSomeone(nPlayerIndex, SYSMSG_TYPE_STR, (const BYTE*)szString, strlen(szString));	
	}

	return 0;
}


/**************************************************/
#endif

#ifdef _SERVER
int LuaGrantTitle(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex) && paramCount > 0)
	{
		TitleManager& titleManager = Player[playerIndex].GetTitleManager();

		int titleIndex = Lua_ValueToNumber(L, 1);
		switch(paramCount)
		{
		case 1:
			{
				titleManager.GrantTitle(titleIndex);
			}
			break;
		case 2:
			{
				DWORD expireTime = (DWORD)Lua_ValueToNumber(L, 2);
				titleManager.GrantTitle(titleIndex, expireTime);
			}
			break;
		case 3:
			{
				WORD startLevel = (WORD)Lua_ValueToNumber(L, 2);
				WORD startValue = (WORD)Lua_ValueToNumber(L, 3);
				
				titleManager.GrantTitle(titleIndex, startLevel, startValue);
			}
			break;
		}
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaRevokeTitle(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex) && paramCount > 0)
	{
		TitleManager& titleManager = Player[playerIndex].GetTitleManager();
		
		int titleIndex = Lua_ValueToNumber(L, 1);
		titleManager.RevokeTitle(titleIndex);
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaSelectTitle(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex) && paramCount > 0)
	{
		TitleManager& titleManager = Player[playerIndex].GetTitleManager();
		
		int titleIndex = Lua_ValueToNumber(L, 1);
		
		if (titleIndex == RANDOM_SELECT_TITLE_FALG)
		{
			titleManager.SetRandomSelectTitle(true);
			titleManager.SyncSelectTitleResult();
		}
		else if (titleIndex >= 0 && titleIndex <= MAX_TITLE_COUNT)
		{
			titleManager.SetRandomSelectTitle(false);
			titleManager.SelectTitle(titleIndex);
			titleManager.SyncSelectTitleResult();
		}
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaSetTitleExpireTime(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex) && paramCount >= 2)
	{
		TitleManager& titleManager = Player[playerIndex].GetTitleManager();
		
		int titleIndex = Lua_ValueToNumber(L, 1);
		DWORD expireTime = Lua_ValueToNumber(L, 2);
		
		titleManager.SetTitleExpireTime(titleIndex, expireTime);
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaModifyTitleExpireTime(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex) && paramCount >= 2)
	{
		TitleManager& titleManager = Player[playerIndex].GetTitleManager();
		
		int titleIndex = Lua_ValueToNumber(L, 1);
		int changeExpireTime = Lua_ValueToNumber(L, 2);
		
		titleManager.ModifyTitleExpireTime(titleIndex, changeExpireTime);
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaModifyTitleValue(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex) && paramCount >= 2)
	{
		TitleManager& titleManager = Player[playerIndex].GetTitleManager();
		
		int titleIndex = Lua_ValueToNumber(L, 1);
		int changeValue = Lua_ValueToNumber(L, 2);
		
		titleManager.ModifyTitleLevelInfo(titleIndex, changeValue);
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaHasTitle(Lua_State * L)
{
	int ret = FALSE;
	int paramCount = Lua_GetTopIndex(L);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex) && paramCount > 0)
	{
		TitleManager& titleManager = Player[playerIndex].GetTitleManager();
		
		int titleIndex = Lua_ValueToNumber(L, 1);
		ret = titleManager.HasTitle(titleIndex) ? TRUE : FALSE;
	}

	Lua_PushNumber(L, ret);
	return 1;
}
#endif

#ifdef _SERVER
int LuaWriteLog(Lua_State * L)
{
	int paramCount = Lua_GetTopIndex(L);
	if (paramCount <= 0)
		return 0;

	const char* pLogMsg = Lua_ValueToString(L, 1);
	if (g_pLogSystem && pLogMsg)
	{
		char logMsgBuff[512] = { 0 };
		strncpy(logMsgBuff, pLogMsg, sizeof(logMsgBuff));
		logMsgBuff[sizeof(logMsgBuff) - 1] = 0;
		g_pLogSystem->SysDbgLog(logMsgBuff, strlen(logMsgBuff), sys_dbg_log_event_script_custom);
	}
	
	return 0;
}
#endif

#ifdef _SERVER
int LuaGetNewConsumePoint(Lua_State * L)
{
	DWORD newConsumePoint = 0;
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		int newConsumePointIndex = ConfigManager::Singleton().GetGlobalVariable(globar_var_new_consume_point_index);
		if (newConsumePointIndex <= 0 || newConsumePointIndex > MAX_PLUS_POINT_COUNT)
		{
			newConsumePointIndex = DEFAULT_NEW_CONSUME_POINT_INDEX;
		}
		newConsumePoint = Player[playerIndex].GetPlusPoint(newConsumePointIndex);
	}
	
	Lua_PushNumber(L, newConsumePoint);
	return 1;
}
#endif

#ifdef _SERVER
int LuaGetItemExp(Lua_State * L)
{
	int expAdd = 0;
	int expItemId = Lua_ValueToNumber(L, 1);
	int getExpMax = Lua_ValueToNumber(L, 2);
	int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		KPlayer& player = Player[playerIndex];
		int expItemIndex = player.GetItemList().SearchID(expItemId);
		if (expItemIndex > 0 && expItemIndex < MAX_ITEM && Item[expItemIndex].IsExpItem())
		{
			KItem& item = Item[expItemIndex];
			int currentExp = item.GetDurability();
			if (currentExp > 0 && getExpMax > 0)
			{
				expAdd = (currentExp < getExpMax) ? currentExp : getExpMax;

				if (expAdd >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_exp_amount) && g_pLogSystem)
				{
					LogEventParam addExpEvent;
					addExpEvent.event = log_event_get_item_exp;
					addExpEvent.param1 = player.GetGUID();
					addExpEvent.param2 = item.GetGUID();

					char itemTemplateId[32] = { 0 };
					item.GetItemTemplateId(itemTemplateId, sizeof(itemTemplateId));
					itemTemplateId[sizeof(itemTemplateId) - 1] = 0;
					snprintf(addExpEvent.param3.data, sizeof(addExpEvent.param3.data), "%s(%s)", item.GetName(), itemTemplateId);
					addExpEvent.param3.data[sizeof(addExpEvent.param3.data) - 1] = 0;

					addExpEvent.param4 = expAdd;
					g_pLogSystem->Log(addExpEvent);
				}

				player.DirectAddExp(expAdd);
				item.SetDurability(currentExp - expAdd);
				item.SyncAttribute(item_attr_durability, player.GetNetConnectIdx());
			}
		}
	}
	
	Lua_PushNumber(L, expAdd);
	return 1;
}
#endif

#ifdef _SERVER
int LuaItemAddBuff( Lua_State* L )
{
	const int itemID = Lua_ValueToNumber(L, 1);
	const int buffTempID = Lua_ValueToNumber(L, 2);	
	const int playerIndex = ScriptGetPlayerIndex();
	if (IsValidPlayer(playerIndex))
	{
		int itemIndex = Player[playerIndex].GetItemList().SearchID(itemID);
		if (itemIndex > 0 && itemIndex < MAX_ITEM)
		{
			int npcIndex = Player[playerIndex].GetNpcIndex();
			BuffMgr& BMgr = BuffMgr::Singleton();
			
			BUFF_PARAM Param;
			Param = itemIndex;
			unsigned long ulID = 
				BMgr.AddNpcBuff( 
				npcIndex, 
				npcIndex, 
				buffTempID,
				&Param);
			
			Lua_PushNumber(L, ulID);
			return 1;
		}
	}
	
	Lua_PushNumber(L, -1);
	return 1;
}
#endif

#ifdef _SERVER

int LuaSocialSaveSwitch(Lua_State* L)
{
	if (Lua_GetTopIndex(L) != 1)
		return 0;
	
	int nValue = Lua_ValueToNumber(L, 1);

	PlayerSet.SetSocialSaveSwitch(nValue != 0);

	return 0;
}

#endif
TLua_Funcs GameScriptFuns[] = 
{
#ifdef _SERVER
	{"GetCityResEx",		LuaGetCityResEx			},
	{"GetCaptureCityTime",	LuaGetCaptureCityTime	},
	{"GetCityPopulation",	LuaGetCityPopulation	},
	/*************EconomySys**********************/
	{"ShowEconomyValue", LuaShowEconomyValue},
	{"GetEconomyValue", LuaGetEconomyValue},
	{"SetEconomyValue", LuaSetEconomyValue},
	{"AddEconomyValue", LuaAddEconomyValue},
	{"DecEconomyValue", LuaDecEconomyValue},
	/**************************************************/
	{"SocialAutoSaveSwitch", LuaSocialSaveSwitch},
	{"AddArmorSet",					LuaAddArmorSet },
	{"AddBuff",						LuaAddBuff },
	{"AddBuffByID",					LuaAddBuffByID },
	{"DelBuff",						LuaDelBuff },
	{"AddNpcBuff",					LuaAddNpcBuff },
    {"IsHaveBuff",					LuaIsHaveBuff },
	{"AddLordBuff",					LuaAddLordBuff},
	{"IsEqualPile",					LuaIsEqualPile },
	{"GetBuffPile",					LuaGetBuffPile },
	{"DecBuffPile",					LuaDecBuffPile },
	{"GetBuffPersist",				LuaGetBuffPersist },
	{"GetBuffPersistByGroup",		LuaGetBuffPersistByGroup },
	{"GetBuffPersistByCate",		LuaGetBuffPersistByCate },
	{"SetBuffPersist",				LuaSetBuffPersist },
	{"SetBuffPersistByGroup",		LuaSetBuffPersistByGroup },
	{"SetBuffPersistByCate",		LuaSetBuffPersistByCate },
	{"AddBuffPersist",				LuaAddBuffPersist },
	{"AddBuffPersistByGroup",		LuaAddBuffPersistByGroup },
	{"AddBuffPersistByCate",		LuaAddBuffPersistByCate },
	{"AddLandmine",					LuaAddLandmine },
	{"AddLandmineP",				LuaAddLandminePoint },
	{"SetSkillSeries",		LuaSetSkillSeries},
	{"AddInitSkills",		LuaAddInitSkills},
	{"GiveMeSkill",			LuaGiveMeSkill},
	{"LevelUpSkill",		LuaLevelUpSkill},
	{"AddSkillExp",			LuaAddSkillExp},
    {"GetMaxPlayer",        LuaGetMaxPlayer},
	//通用指令
	{"BeginMotion", LuaBeginMotion},
	{"GetNpcID", LuaGetNpcID},
	{"SayTask", LuaSayTask},
	{"TaskNote", LuaTaskNote},
	{"MovieScene", LuaMovieScene},
	{"OpenWindow", LuaOpenWindow},
	{"OpenTimer", LuaOpenTimer},
// 	{"AcceptQuest", LuaAcceptQuest},
// 	{"ShowQuest", LuaShowQuest},
// 	{"SyncQuest", LuaSyncQuest},
	{"CloseDialog", LuaCloseDialog},
	{"InputDialog", luaInputUI},
	{"TopMessage", LuaTopMessage},
	
	{"GetBit",	LuaGetBit},
	{"GetByte",	LuaGetByte},
	{"SetBit",	LuaSetBit},
	{"SetByte",	LuaSetByte},
	{"PrintNetStat",LuaPrintNetStat},
	{"GMReply",LuaGMReply},
// 	{"AddGlobalNews",LuaAddGlobalNews},
// 	{"AddGlobalTimeNews",LuaAddGlobalTimeNews},
// 	{"AddGlobalCountNews",LuaAddGlobalCountNews	},
	{"GetLord", LuaGetLord},
	{"GetRobber", LuaGetRobber},
	{"GetSubLord", LuaGetSubLord},
	{"IsCityHaveOwner", LuaIsCityHaveOwner},
#endif

	//服务器端脚本指令
#ifdef _SERVER
	{"ChatInRoomByString", LuaChatInRoom},
	{"ChatInRoomByStringID",	LuaChatInRoomByStringID},//EnchaseItem()宝石相嵌
	{"OpenEnchaserItemDialog",	LuaOpenEnchaserItemDialog},//EnchaseItem()宝石相嵌
	{"OpenSkillManageDialog",	LuaOpenSkillManageDialog},//技能学习
	{"CreateTong",	LuaOpenCreateTongDialog},//创建
	{"CityMgr",LuaOpenCityManageDialog},//城市管理
	{"OpenMailDialog",	LuaOpenMailDialog},//邮件
	{"OpenAuctionDialog", LuaOpenAuctionDialog},//拍卖
	{"OpenInstanceRewardDialog", LuaOpenInstanceRewardDialog},//副本奖励
	{"OpenCreditShop", LuaOpenCreditShop},//信用商店
	{"ActiveNavigationButton", LuaActiveNavigationButton},//激活功能按键, 参数0-7代表8个功能按键
	{"WorldCombatDialog",luaWorldCombatDialog},
	{"AddPlayerCombatScore",luaAddCombatScore},
	{"GetTongID",	LuaGetTongID},
	{"GetSocialUnitName", LuaGetSocialUnitName},
	{"AddCityRes", LuaAddCityRes},
	{"GetCityRes", LuaGetCityRes},
	{"IsSameLord", LuaIsSameLord},
	{"IsUnitOwner", LuaIsUnitOwner},
	{"IsJoinUnit", LuaIsJoinUnit},
	{"GetParentUnitName",luaGetUnitParentNodeName},
	{"ScriptDialogNpc",luaScriptDialogNpc},
	{"IsNpcLordOwner", LuaIsNpcLordOwner},
	{"GetCityTaxRate", LuaGetCityTaxRate},
	//-----------------基本功能
	//--> Rocker 2005/07/14
	{"NpcSay", LuaNpcSay},
	//<-- End
	//--> Rocker 2005/07/15
	{"IsEquipItem", LuaIsEquipItem},
	//<-- End
	
	// Rocker 2004.7.28
	{"PlayerIndexToNpcIndex",	LuaPlayerIndexToNpcIndex},
	// Rocker 2004.7.28
	
//	{"SetTimer",		LuaSetTimer},		//SetTimer(时间量, 时间TaskId):给玩家打开计时器,时间到时将自动调用OnTimer函数
//	{"StopTimer",		LuaStopTimer},		//StopTimer()：关闭当前玩家的计时器
//	{"GetRestTime",		LuaGetRestTime},	//GetRestTime:获得计时器将触发的剩于时间	
//	{"GetTimerId",		LuaGetCurTimerId},	//CurTimerId = GetTimerId():获得当前执行的计时器的id,如果没有则返回0
	{"GetQuestState",	LuaGetTaskValue},	//GetTask(任务号):获得当前玩家该任务号的值
	{"SetQuestState",	LuaSetTaskValue},	//SetTask(任务号,值):设置任务值
	{"SyncQuestBible",  LuaSynQuestBible},
// 	{"GetQuestProcess",	LuaGetQuestProcess},
// 	{"SetQuestProcess",	LuaSetQuestProcess},
// 	{"SyncQuestState",	LuaSyncQuestState},
// 	{"GetQuestIDByQuestLogIDX",	LuaGetQuestIDByQuestLogIDX},
// 	{"FindQuestByID",	LuaFindQuestByQuestID},
	{"IsCaptain",		LuaIsLeader},		//IsCaptain()是否为队长
	{"GetTeam",			LuaGetTeamId},		//GetTeam()返回玩家队伍ID
	{"GetTeamSize",		LuaGetTeamSize},	//得到队员成员（包括队长）的数量
	{"LeaveTeam",		LuaLeaveTeam},		//LeaveTeam()让玩家离开自身队伍
	{"GetTeamMember",	LuaGetTeamMember},//得到队伍成员（包括队长）的PlayerIndex
	{"GetTeamCaptain",	LuaGetTeamCaptain},//得到队长的PlayerIndex

	{"Msg2Player",		LuaMsgToPlayer	},	//Msg2Player(消息)
	{"Msg2Team",		LuaMsgToTeam},		//Msg2Team(消息)通知玩家的组
	{"Msg2Tong",		LuaMsgToTong},
	{"Msg2Faction",		LuaMsgToFaction},
	{"Msg2AllPlayer",	LuaMsgToAllPlayer},
	{"ShowMsg",			LuaShowMsg},		// 用于测试显示获取一些数据显示在聊天频道

	{"GetAccount",		LuaGetAccountName},
	{"ActivePresent",   LuaActivatePresent},
	{"SetPos",			LuaSetPos},			//SetPos(x,y)进入某点	
	{"RandomTrans",		LuaRandomTrans},
	{"GetWorldPos",		LuaGetWorldPos},
	{"NewWorld",		LuaEnterNewWorld},
	

	{"GetNpcLevel", LuaGetNpcLevel},
	{"ThrowItem", LuaThrowItem},
	{"AddItem",	LuaAddItem},		//AddItem(nItemClass, nDetailType, nParticualrType, nLevel, nSeries, nLuck, nItemLevel..6)
	{"AddRewardBindItem",LuaAddTaskBindItem},
	{"AddTimeLimitedItem",LuaAddTimeLimitItem},
	{"AddWrongItem",LuaAddWrongItem},//Add a Wrong Item With ItemCount == 0
	{"IsExistItem",		LuaIsExistItem},
	{"IsHaveItem",		LuaIsHaveItem},
	{"GetItemMaxStackCount",	LuaGetItemMaxStackCount},
	{"DelNormalItem",	LuaDelNormalItem},
	{"DelNormalItemByItemID",	LuaDelNormalItemByID},
	{"SetItemCreditFlagByID", LuaSetItemCreditFlagByID},
	
	{"AddMoneyObj",		LuaAddMoneyObj},//LuaAddMoneyObj(money)
	// Rocker 2004.7.28
	{"ThrowMoneyObj",	LuaThrowMoneyObj},
	{"GetPlayerIndexByName",	LuaGetPlayerIndexByName},	// 得到玩家的Index
	// Rocker 2004.7.28

	
	{"SubWorldID2Idx",	LuaSubWorldIDToIndex}, //SubWorldID2Idx
	
    {"IsAlive", LuaIsAlive},
	{"AddNpc",			LuaAddNpc},			//AddNpc(人物模板id或人物模板名,所处世界id，点坐标x,点坐标y),返回npcid值
	{"DelNpc",			LuaDelNpc},
	{"SetNpcOwner",luaSetNpcOwner},
	{"SetNpcName",		LuaSetNpcName},
	{"SetNpcScript",	LuaSetNpcActionScript},	//SetNpcScript(npcid, 脚本文件名)设置npc当前的脚本
	{"SetRevPos",		LuaSetPlayerRevivalPos},//SetRevPos(点位置X，点位置Y)设置玩家的当前世界的等入点位置
	{"SetTempRevPos",	LuaSetDeathRevivalPos}, //SetTempRevPos(subworldid, x, y ) or SetTempRevPos(id);
	// Add by cooler
	// liuyujun@263.net 2004/3/26 -->
	{"GetPlayerType",		LuaGetPlayerType},	// GetPlayerType, 0 = Knight, 1 = Enchanter, 2 = Monstrous
	{"GetSkillSeries",		LuaGetSkillSeries},	// GetPlayerType, 0 = Knight, 1 = Enchanter, 2 = Monstrous
	// <-- End cooler add.

	
	// Add by Cooler 2004-7-2
	// Begin -->
	{"GetNextExp",		LuaGetNextLevelExp},		// Get player next level up exp
	// End <--
//	{"GetExp",			LuaGetPlayerExp	},			//GetExp():获得玩家的当前经验值
	{"AddExp",			LuaAddOwnExp},				//AddExp，给玩家直接加经验（实现和下面这个函数一样）
	{"AddOwnExp",		LuaAddOwnExp},				//AddOwnExp(Exp)，给玩家直接加经验
	{"RestoreLife",		LuaRestorePlayerLife},		//RestoreLife()恢复玩家的生命
	{"RestoreMana",		LuaRestorePlayerMana},		//RestoreMana()恢复玩家的Mana
	{"GetSex",			LuaGetPlayerSex},			//GetSex()获得玩家的性别
	{"GetName",			LuaGetPlayerName},			//GetName()获得玩家的姓名
	{"GetNameByIdx",	LuaGetPlayerNameByIdx},		//GetName()获得玩家的姓名
	{"GetAccName",		LuaGetPlayerAccName},		//GetAccName()获得玩家的帐号名
	{"GetNpcNameByTemplateID",	LuaGetNpcNameByTemplateID},			//GetName()获得玩家的姓名
	{"GetItemName",				LuaGetItemName},		//GetName()获得玩家的姓名
	{"GetUUID",			LuaGetPlayerID},			//GetUUID()获得玩家的唯一ID
	{"GetLevel",		LuaGetLevel},				//GetLevel()GetPlayers Level
	{"GetCash",			LuaGetPlayerCashMoney},		//GetCash()获得玩家的现金
	{"Pay",				LuaPlayerPayMoney},			//Pay(金额数)扣除玩家金钱成功返回1，失败返回0
	{"Earn",			LuaPlayerEarnMoney},		//Earn(金额数)增加玩家金钱
	{"Sale",			LuaSale},					//Sale(SaleId)买卖，SaleId为便卖的物品信息列表id
	{"Smith",			LuaSmith},					//打造
	{"Hire",			LuaHire},					//雇佣商店
	{"SetAntiEnthral",luaSetAntiEnthralState},
	{"GetLeagueMaxSubUnitCnt",LuaGetLeagueCurrentMaxSubUnitCnt},
	{"SetLeagueMaxSubUnitCnt",LuaSetLeagueCurrentMaxSubUnitCnt},
	{"GetBelongCityMapId",LuaGetBelongCityMapId},
	
	//Lucifer~yu[zhangjianyu] [12/30/2005] Add for 
	//begin------------------------------------------------------------------------
	{"OpenInputDlg",		LuaOpenGeneralInputDlg},
	//end--------------------------------------------------------------------------	
	{"OpenBox",			LuaOpenBox},
	
	{"SetPropState",	LuaSetObjPropState},//SetPropState( hide = 1) hide obj

	{"IsMyFriend",		LuaIsMyFriend},
	{"GetLastPlayTime",	LuaGetLastPlayTime},
	{"GetSocialOnlinePlayerNumber", LuaGetSocialOnlinePlayerNumber},
	
	//-----------------Mission Script-----------------
//	{"GetMissionV", LuaGetMissionValue},//GetMissionV(Vid)
//	{"SetMissionV", LuaSetMissionValue},//SetMissionV(Vid, Value)
//	{"GetGlbMissionV", LuaGetGlobalMissionValue	},
//	{"SetGlbMissionV", LuaSetGlobalMissionValue	},
//	{"OpenMission", LuaInitMission},//OpenMission(missionid)
//	{"RunMission", LuaRunMission},
//	{"CloseMission", LuaCloseMission},//CloseMission(missionid)
//	{"StartMissionTimer", LuaStartMissionTimer},////StartMissionTimer(missionid, timerid, time)
//	{"StopMissionTimer", LuaStopMissionTimer},
//	{"GetMSRestTime", LuaGetMissionRestTime}, //GetMSRestTime(missionid, timerid)
//	{"GetMSIdxGroup",LuaGetPlayerMissionGroup},//GetPlayerGroup(missionid, playerid);
	
//	{"AddMSPlayer", LuaAddMissionPlayer},
//	{"DelMSPlayer", LuaRemoveMissionPlayer},
//	{"GetNextPlayer", LuaGetNextPlayer},
//	{"PIdx2MSDIdx", LuaGetMissionPlayer_DataIndex},//(missionid, pidx)
//	{"MSDIdx2PIdx", LuaGetMissionPlayer_PlayerIndex},//(missionid, dataidx)
	{"NpcIdx2PIdx", LuaNpcIndexToPlayerIndex},
//	{"GetMSPlayerCount", LuaMissionPlayerCount},//GetMSPlayerCount(missionid, group = 0)

//	{"SetPMParam", LuaSetMissionPlayerParam },
//	{"GetPMParam", LuaGetMissionPlayerParam},
//	{"Msg2MSGroup", LuaMissionMsg2Group},
//	{"Msg2MSAll", LuaMissionMsg2All},
//	{"Msg2MSPlayer", LuaMissionMsg2Player},

	{"SetLogoutRV", LuaSetPlayerRevivalOptionWhenLogout},
//	{"SetCreateTeam",LuaSetCreateTeamOption},
	{"GetPK", LuaGetPlayerPKValue},  //pkValue = GetPK() 
	{"SetPK", LuaSetPlayerPKValue}, //SetPK(pkValue)
	{"GetGameTime",LuaGetTotalTimeInGame},//获得当前玩家从第一次进入游戏开始共在线的时间

	{"SetPKFlag",LuaSetPKFlag},//SetPKFlag(1/0)
	//------------------------------------------------
	{"GetNpcTemplateID", LuaGetNpcTemplateID},
	{"GetPlayerIndex", LuaGetPlayerIndex},
	{"SetPlayerIndex", LuaSetPlayerIndex},
	{"SetPunish",	LuaSetDeathPunish},// SetPunish(0/1) 0表示不受任何惩罚

	{"GetGlobalValue", LuaGetGlobalValue},
	{"SetGlobalValue", LuaSetGlobalValue},

#ifdef _SERVER
	{"DoMarryEx",		LuaDoMarryEx},
	{"UnMarryEx",		LuaUnMarryEx},
	{"IsMarried",		LuaIsMarried},
	{"CanMarry",		LuaCanMarry},
	{"DoMarry",			LuaDoMarry},
	{"UnMarry",			LuaUnMarry},
	{"GetNameID",		LuaGetNameID},
	{"GetWeekDay",	LuaGetWeekDay},
	{"ReplaceBoxPwd",	LuaReplaceBoxPwd},
	{"CDKeyChangeBox",	LuaCDKeyChangeBox},
#endif
	{"GetExtPoint",	LuaGetExtPoint},
	{"AddExtPoint", LuaAddExtPoint},
	{"PayExtPoint",	LuaPayExtPoint},
	{"SetExtPoint", LuaSetExtPoint},

	{"GetPlusPoint",	LuaGetPlusPoint},
	{"AddPlusPoint",	LuaAddPlusPoint},
	{"PayPlusPoint",	LuaPayPlusPoint},
	{"GetPlusPointRecord",	LuaGetPlusPointRecord},
	
	{"UseSilver",	LuaUseSilver},
	{"IBTest",		LuaIBTest},
	// IB相关
	{"IBShopLoadShelf",	LuaIBShopLoadShelf},	// 重新加载IBSHOP的所有页面
	{"IBShopLoadGoods",	LuaIBShopLoadGoods},	// 重新加载IBSHOP的某个页面
	{"IBShopLoadPanel",	LuaIBShopLoadPanel},	// 重新加载所有的PANEL
	{"IBShopLoadStyle",	LuaIBShopLoadStyle},	// 重新加载所有排版的STYLE

	{"GetBoxMoney",	   LuaGetBoxMoney},
	{"AddBoxMoney",		LuaAddBoxMoney},
	

	{"SaveQuickly", LuaSaveQuickly},

	{"OpenTaisuiDialog",LuaOpenTaisuiDialog},
	{"SetTianXiang",LuaChangeTaisuiTianXiang},
	{"SuspendTianXiang",LuaSuspendTianXiang},
	{"ResumeTianXiang",LuaResumeTianXiang},
	{"IfTaisuiAvailable",luaClientCanWheelTaisui},
	{"MsgToMapEx",LuaMsgToMapEx},
	{"MsgToTongEx",LuaMsgToTongEx},
	{"CreateBoxPassword",luaCreatePassword},
	{"CloseStorageBox",luaCloseStoreBox},
	
	{"IsHaveWorldFlag",LuaIsHaveWorldFlag},
	{"GetCombatPersonNum",LuaGetInstanceCombatPersonNum},
	{"GetCombatScore",LuaGetInstanceCombatScore},
	{"LetPlayerChangeToSelf",luaGetPlayerChangeWorld},

	{"GetPlayerCombatScore",LuaGetPlayerCombatScore},
	{"GetPlayerCombatOrg",LuaGetPlayerCombatOrg},
	{"RegisterPlayerToCombat",LuaRegisterPlayerToCombat},
	{"DecPlayerCombatScore",luaDecPlayerCombatScore},
	{"SetCombatScorePoint", luaSetCombatScorePoint},
	{"SetTongWarCommanderSyncSwitch", luaSetTongWarCommanderSyncSwitch},

	{"GetPlayerPrivateState",luaGetPlayerPrivateState},
	{"SetPlayerPrivateState",luaSetPlayerPrivateState},


	{"AddTailsmanExp",LuaAddTailsmanExp},
	{"OpenTongCentre",luaOpenTongCentre},
	{"GetSocialData",luaGetSocialScriptData},
	{"SetSocialData",luaSetSocialScriptData},
	{"GetSaveSocialData",	luaGetSocialSaveScriptData},
	{"SetSaveSocialData",	luaSetSocialSaveScriptData},
	{"SetFuryExp",luaSetFuryExp},
	{"GetFuryExp",luaGetFuryExp	},
	{"AddBuffToGens",LuaAddBuffToGens},
	{"AddBuffToTong",LuaAddBuffToTong},
	{"AddBuffToTongPlayer",LuaAddBuffToTongMember},
	{"AddInsurance",luaAddInsuranceValue},
	{"FetchInsuranceMoney",luaFetchInsuranceMoney},

	{"SetGlobalExpInsuranceState",luaSetGlobalExpInsuranceState},
	{"SetGlobalQuestInsuranceState",luaSetGlobalQuestInsuranceState},
	
	{"EnterExpInsuranceState",luaEnterExpInsuranceState},
	{"LeaveExpInsuranceState",luaLeaveExpInsuranceState},
	{"IsEnteredExpInsuranceState",luaIsEnteredExpInsuranceState},
	{"GetExpInsuraceReward",luaGetExpInsuranceReward},
	
	{"EnterQuestInsuranceState",luaEnterQuestInsuranceState},
	{"LeaveQuestInsuranceState",luaLeaveQuestInsuranceState},
	{"IsEnteredQuestInsuranceState",luaIsEnteredQuestInsuranceState},
	{"AddQuestInsuranceReward",luaAddQuestInsuranceState},
	{"GetQuestInsuraceReward",luaGetQuestInsuranceState},

#endif
	// Add by Cooler 2004-5-18
	// Begin -->
	{"GetBoxSize", LuaGetBoxSize}, // Get inventory size.
	// End <--


	// Add by Cooler 2004-7-5
	// Begin -->
#ifdef _SERVER
	{"AddCredit", LuaAddCredit},				// Add credit
	{"DecCredit", LuaDecCredit},				// Dec credit
#endif
	// End <--

	// Add by Cooler 2004-7-20
	// Begin -->
#ifdef _SERVER
	{"GetCredit", LuaGetCredit},				// Get credit
	{"MultiCredit", LuaMultiCredit},			// Multiple credit
#endif
	// End <--

	
#ifdef _SERVER 
	{"PolyMorph", LuaPolyMorph}, // 变身函数lixuewu 2004.07.12 13
	{"SystemTime", LuaGetSysTime},
	{"SystemCorTime", LuaGetSysCorTime},
	// --> Rocker Edit Start 2005/08/01
	{"GetYMD", LuaGetYMD},
	{"GetHMS", LuaGetHMS},
	// <-- Rocker End
	{"Time2LocalYMD", LuaTime2LocalYMD}, 
#endif
	
// Add by [Adt.X], 2004-8-14
#ifdef _SERVER
    {"AddWeightMax",    LuaAddWeightMax},
	{"GetWeightMax",    LuaGetWeightMax},
	{"GetWeightTaken",  LuaGetWeightTaken},
#endif
// End.
#ifdef _SERVER
	{"CanChangeLord",       luaCanChangeLord},
	{"GetStatueLordInfo",  luaGetStatueLordInfo},
	{"ClearInvalidStatue",	luaClearInvalidStatue},
	{"ReStoreStatue",		luaReStoreStatue},
#endif
#ifndef _SERVER
	// 除此之外，请不要使用
	{"DrawTrap", LuaDrawTrap},
	{"TrapChange", LuaTrapChange},
	//调试通用对话框
	{"PopMessage", LuaComMsg},
	{"ShowDuraAlert", LuaShowDuraAlert},
	{"TI", LuaTrackInject},	
	{"PW", LuaPrintWindow},	
#ifdef _AUTO_ROBOT
	{"AutoRunTo", LuaAutoRunTo},
#endif

#endif

#ifdef _SERVER
	{"DelAllItem", LuaDelAllItem},
	{"AddNpcRAttr", LuaAddNpcRAttr},
	{"AddNpcCompAttr", LuaAddNpcCompAttr},
	{"AddNpcCurAttr", LuaAddNpcCurAttr},
	{"GetNpcRAttr", LuaGetNpcRAttr},
	{"GetNpcCompAttr", LuaGetNpcCompAttr},
	{"GetNpcCurAttr", LuaGetNpcCurAttr},
	{"KillBaby", LuaKillBaby},
	{"ChgPKMode", LuaChgPKMode},
	{"ShowBanner", LuaShowBanner},//显示滚动标题
	{"ShowBannerById", LuaShowBannerId},//显示滚动标题
	{"ShowBanner2", LuaShowBanner2},//显示滚动标题
//	{"LogAcceptQuest", LuaLogAcceptQuest},//日志：接受任务
//	{"LogAbortQuest", LuaLogAbortQuest},//日志：放弃任务
//	{"LogCompleteQuest", LuaLogCompleteQuest},//日志：完成任务
	{"SendMail", LuaSendMail},//给指定玩家（不一定在线）发送邮件
	{"SendMailToAll", LuaSendMailToAll},//给所有玩家（包括不在线的）发送邮件
	{"ExpPercentage", LuaExpPercentage},//修改打怪经验获得百分比
	{"QuestExpPercentage", LuaQuestExpPercentage},//修改任务经验获得百分比
	{"SkillExpPercentage", LuaSkillExpPercentage},//修改蕴魂获得百分比
	{"GlobalExpPercentage", LuaGlobalExpPercentage},//修改全局打怪经验获得加成百分比
	{"GlobalQuestExpPercentage", LuaGlobalQuestExpPercentage},//修改全局任务经验获得加成百分比
	{"GlobalSkillExpPercentage", LuaGlobalSkillExpPercentage},//修改全局蕴魂获得加成百分比
	{"EnterInstance", LuaEnterInstance},//当前上下文中的玩家进入副本，第三个参数可选，省略时表示不创建
	{"GetPkValue", LuaGetPkValue},//得到PK值
	{"GetWorldCustomVariable", LuaGetWorldCustomVariable},//得到世界（地图）自定义变量
	{"SetWorldCustomVariable", LuaSetWorldCustomVariable},//设置世界（地图）自定义变量
	{"GetWorldCustomString", LuaGetWorldCustomString},//得到世界（地图）自定义字符串
	{"SetWorldCustomString", LuaSetWorldCustomString},//设置世界（地图）自定义字符串
	{"SendWorldCustomStringToPlayer", LuaSendWorldCustomStringToPlayer},//发送地图自定义字符串到指定玩家
	{"SendWorldCustomStringToAllPlayer", LuaSendWorldCustomStringToAllPlayer},//发送地图自定义字符串到本地图所有玩家
	{"TeamAddMember", LuaTeamAddMember},//添加队伍成员
	{"TeamDeleteMember", LuaTeamDeleteMember},//移除队伍成员
	{"CreateWorldTeam", LuaCreateWorldTeam},//创建地图绑定队伍
	{"GetWorldTeam", LuaGetWorldTeam},//得到地图绑定队伍
	{"GetWorldTeamCount", LuaGetWorldTeamCount},//得到地图绑定队伍数量
	{"SetTeamParam", LuaSetTeamParam},//设置队伍参数
	{"CreateInstance", LuaCreateInstance},//创建副本
	{"EnterInstanceByID", LuaEnterInstanceByID},//传送到指定ID的副本
	{"GetSubWorldIdxFromInstanceID", LuaGetSubWorldIdxFromInstanceID},//通过副本ID得到SubworldIndex
	{"GetInstanceIDFromSubWorldIdx", LuaGetInstanceIDFromSubWorldIdx},//通过SubworldIndex得到副本ID
	{"CloseInstance", LuaCloseInstance},//关闭副本
	{"WorldFirstPlayer", LuaWorldFirstPlayer},//地图第一个玩家
	{"WorldNextPlayer", LuaWorldNextPlayer},//地图下一个玩家
	{"GetWorldIndex", LuaGetWorldIndex},//得到地图索引
	{"OpenBigTeamMode", LuaOpenBigTeamMode},//打开大队伍模式	
	{"GetCurrentWorldIndex", LuaGetCurrentWorldIndex},//得到当前上下文中的地图索引
	{"GetCurrentWorldId", LuaGetCurrentWorldId},//得到当前上下文中的地图ID
	{"CanEnterWorld", LuaCanEnterWorld},//判断是否可以进入某个地图，第二个参数可选，省略时表示取当前上下文中的PlayerIndex
	{"HasEnterableWorld", LuaHasEnterableWorld},//判断是否有可进入的地图存在，第二个参数可选，省略时表示取当前上下文中的PlayerIndex
	{"SetSpyLevel", LuaSetSpyLevel},//设置监视等级。参数一：角色名；参数二：监视等级
	{"IsInBigTeam", LuaIsInBigTeam},//是否在团队中
	{"KickPlayer", LuaKickPlayer},//踢出玩家
	{"NoChat", LuaNoChat},//禁言
	{"Save", LuaSave},//保存
	{"MonitorGlobalChat", LuaMonitorGlobalChat},//开启/关闭公聊监督
	{"ChangeMap", LuaChangeMap},
	{"NpcFollow", LuaNpcFollow},//NPC跟随
	{"NewWorldNpc", LuaNewWorldNpc},//传送NPC
	{"SyncToWorld", LuaSyncToWorld},//设置世界同步
	{"GetNpcCustomVariable", LuaGetNpcCustomVariable},//得到NPC自定义变量
	{"SetNpcCustomVariable", LuaSetNpcCustomVariable},//设置NPC自定义变量
	{"GetRandomPos", LuaGetRandomPos},//得到随机位置
	{"IsPoolCondValid",luaIsPoolCombatCondValid},//分星池条件是否满足
	{"StartPoolCombat",luaPoolCombatStart},
	{"DropPool",luaDropPool},               //放弃分星池
	{"TestChangeMoney",LuaTestChangeMoney},//测试金山币，不要误用
	{"InstanceSummary", LuaInstanceSummary},//得到副本概况
	{"InstanceDetail", LuaInstanceDetail},//得到副本详述
	{"ItemAndObjSummary", LuaItemAndObjSummary},
	{"RecordPlayerReward", LuaRecordPlayerReward},//记录玩家中奖
	{"RecordPlayerContact", LuaRecordPlayerContact},//记录玩家信息
	{"LeaveInstance", LuaLeaveInstance},//离开副本
	{"NotifyGMReply", LuaNotifyGMReply},//通知检查GM回复
	{"Post", LuaPost},//发布
	{"Employ", LuaEmploy},//雇用
	{"Fire", LuaFire},//解雇
	{"SetEmployTime", LuaSetEmployTime},//设置雇用时间
	{"NotifyGMReply", LuaNotifyGMReply},//离开副本
	{"SetIBPoint", Lua_AddIBMoney},
	{"CostTongbao", Lua_CostTongbao},
	{"ChangeIBMoneyMax", Lua_ChangeIBMoneyMax},
	{"AddEmployTime", LuaAddEmployTime},//添加雇用时间
	{"GetEmployTime", LuaGetEmployTime},//得到雇用时间
	{"HasEmployee", LuaHasEmployee},//是否有佣兵
	{"OpenUseItemDialog", Lua_OpenUseItemDialog},
	{"ItemGoToPos", Lua_ItemGoToPos},
	{"ItemClearPos", Lua_ItemClearPos},
	{"ItemMarkerPos", Lua_ItemMarkerPos},
	{"IsItemMarkerPos", Lua_IsItemMarkerPos},
	{"GetItemStep", Lua_GetItemStep},
	{"GetIBItemBuyDate", Lua_GetIBItemBuyDate},
	{"SyncWorldCombatResult2",LuaSyncCombatResultOrg2},
	{"IsMapProcessWar",LuaIsMapProcesssWar},
	
	//交互脚本相关
	{"StartInteractiveScript", LuaStartInteractiveScript},//开始执行交互脚本
	{"GetInteractiveScriptStep", LuaGetInteractiveScriptStep},//得到交互脚本步骤数
	{"GetInteractiveScriptIntParam", LuaGetInteractiveScriptIntParam},//得到交互脚本数字参数
	{"GetInteractiveScriptStrParam", LuaGetInteractiveScriptStrParam},//得到交互脚本字符串参数
	{"InteractiveScriptDone", LuaInteractiveScriptDone},//交互脚本结束

	{"DoRevive", LuaDoRevive},//重生并补满血蓝
	{"SetIBItemUseCount", Lua_SetIBItemUseCount},
	{"GetIBItemUseCount", Lua_GetIBItemUseCount},
	{"GetIBMoneyMax", Lua_GetIBMoneyMax},

	{"Withdraw", LuaWithdraw},//领取
	{"IsInInstance", LuaIsInInstance},//是否在副本
	
	//推荐人系统策划接口
	{"AddRecommender", LuaAddStudent},//添加推荐人
	{"OpenRecommenderRewardDialog", LuaListStudent},//打开推荐人奖金查询和领取界面
	{"OpenReportToRecommenderDialog", LuaListMaster},//打开被推荐人向推荐人汇报情况界面

	//推荐人系统调试接口
	{"UpdateStudent", LuaUpdateStudent},//被推荐人汇报情况
	{"GetRememberPos", LuaGetRememberPos},//得到记录的位置

	{"GetCreateTime", LuaGetCreateTime},//得到角色创建时间
	{"CostLockedItemVolume", LuaCostLockedItemVolume},//消耗指定锁定物品的使用量
	{"GetNpcName", LuaGetNpcName},//得到Npc名字
	{"IsInstanceExist", LuaIsInstanceExist},//副本是否存在
	//游戏信息获取相关
	{"GetPlayerAttribute", LuaGetPlayerAttribute},//得到玩家属性

	//问答相关
	{"AntiRobot", LuaAntiRobot},
	{"NewQuestion", LuaNewQuestion},
	{"ReloadQuestionSettings", LuaReloadQuestionSettings},
	{"ResetBadAnswerState", LuaResetBadAnswerState},

	//称号相关
	{"GrantTitle", LuaGrantTitle},//授予称号
	{"RevokeTitle", LuaRevokeTitle},//取消称号
	{"SelectTitle", LuaSelectTitle},//选择称号
	{"SetTitleExpireTime", LuaSetTitleExpireTime},//设置称号到期时间
	{"ModifyTitleExpireTime", LuaModifyTitleExpireTime},//修改称号到期时间
	{"ModifyTitleValue", LuaModifyTitleValue},//修改称号活跃度
	{"HasTitle", LuaHasTitle},//判断是否有称号
	
	{"WriteLog", LuaWriteLog},//记录日志

	{"GetCoupleLastOffLineTime", LuaGetCoupleLastOffLineTime},	//取得配偶最后一次下线时间
	{"GetCoupleName", LuaGetCoupleName},		//取得配偶名字
	{"IsCoupleOnline", LuaIsCoupleOnline},		//配偶是否在线
	{"GetMarriageTime", LuaGetMarriageTime},	//取得结婚时间
	{"SendInvitation", LuaSendInvitation},		//发送邀请邮件
	{"GetMarriedTimes", LuaGetMarriedTimes},	//取得结婚次数
	{"PlayAnimation", LuaPlayAnimation},		//播放客户端动画

	{"GetNewConsumePoint", LuaGetNewConsumePoint},//得到新消费积分
	{"GetItemExp", LuaGetItemExp},//取出经验物品中存储的经验值
	{"ItemAddBuff", LuaItemAddBuff},//物品添加BUFF
	
#endif
};

TLua_Funcs WorldScriptFuns[] =// 非指对玩家的脚本指令集
{
	//通用指令
	//服务器端脚本指令
#ifdef _SERVER
	{"SubWorldID2Idx",	LuaSubWorldIDToIndex}, //SubWorldID2Idx
#else	
	{"PopMessage", LuaComMsg},
#endif
}; 

int g_GetGameScriptFunNum()
{
	return sizeof(GameScriptFuns)  / sizeof(GameScriptFuns[0]);
}

int g_GetWorldScriptFunNum()
{
	return sizeof(WorldScriptFuns)  / sizeof(WorldScriptFuns[0]);
}

 