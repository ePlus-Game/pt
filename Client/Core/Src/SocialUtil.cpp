//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:18   21:02
//      File_base        : SocialUtil
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
#include "SocialUtil.h"
#include "SocialUnit.h"
#include "ServerSocialUnitMgr.h"
#include "CoreRelated.h"
#include "SocialSerializer.h"
#include "cfs_filelogs.h"
#include "CoreUtil.h"
#include <stdarg.h>
#include "buff_man.h"
#include "KSubWorld.h"
#include "KSubWorldSet.h"
#include "ChatCenter_S.h"
#include "KWarInfoManager.h"
#include "pool_combat_info_mgr.h"

ASSERT_NOT_GREATER<enSoc_DBOpe_LastUsed, enSocial_DBOpe_End>	notUsed;

bool CheckPrivilege(int nPlayerIdx, int nTplId, int nLayer, int nOpeId)
{
	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, nTplId);

	_ASSERT(pLeafUnit);
	if(NULL == pLeafUnit)
		return false;

	PrivilegeSet	&privSet = pLeafUnit->GetPrivilegeSet();

	return privSet.Check(nTplId, nLayer, nOpeId);
}

void SocialErrCode2Client(int nPlayerIdx, const SU_COMOPE_PARAM &param, int nCode)
{
	S2C_SOCIAL_MSGCODE	s2cRet;
	
	s2cRet.comHeader.proHeader.protocol = s2c_social_family;
	s2cRet.comHeader.proHeader.len = sizeof(s2cRet) - PROTOCOL_SIZE;
	s2cRet.comHeader.proHeader.subProtocol = enSRProtocol_MsgCode2Client;
	s2cRet.comHeader.comParam = param;
	s2cRet.msgCode = (BYTE)nCode;

	SendDataToClient(nPlayerIdx, &s2cRet, s2cRet.comHeader.proHeader.len + PROTOCOL_SIZE);
}

int	CheckComOpeCond(int nPlayerIdx, int nTplId, int nLayer, int nOpeId)
{
	_ASSERT(nOpeId > enSUO_None && nOpeId < enSUO_Num);

	if(nOpeId <= enSUO_None || nOpeId >= enSUO_Num)
		return enSocialErr_OperationNotExist;

	RelationLayer	*pLayer = GetRelationLayer(nTplId, nLayer);
	
	_ASSERT(pLayer);
	if(NULL == pLayer || !IsValidPlayer(nPlayerIdx))
		return enSocialErr_OperationFailed;

	if( GetTotalMoney(nPlayerIdx) < pLayer->Operations[nOpeId].RequireMoney )
 		return enSocialErr_MoneyNotEnough;

	if( GetPlayerLevel(nPlayerIdx) < pLayer->Operations[nOpeId].RequireLevel )
		return enSocialErr_LevelInValid;

	return enSocialErr_None;
}

void DeductComOpeCost(int nPlayerIdx, int nTplId, int nLayer, int nOpeId)
{
	_ASSERT(nOpeId > enSUO_None && nOpeId < enSUO_Num);

	if(nOpeId <= enSUO_None || nOpeId >= enSUO_Num)
		return;

	RelationLayer	*pLayer = GetRelationLayer(nTplId, nLayer);

	_ASSERT(pLayer);
	if(NULL == pLayer || !IsValidPlayer(nPlayerIdx))
		return;

	for (int i = 0 ; i< enSUO_Num; i++)
		if (pLayer->Operations[i].Id == nOpeId)
		{
			int requireMoney = pLayer->Operations[i].RequireMoney;
			if (requireMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
			{
				LogEventParam logParam;
				logParam.event = log_event_social_op_pay_money;
				logParam.param1 = Player[nPlayerIdx].GetGUID();
				logParam.param4 = -requireMoney;
				g_pLogSystem->Log(logParam);
			}

			DecMoney(nPlayerIdx, pLayer->Operations[i].RequireMoney);
		}

}

bool IsInRange(int nNpcIdx1, int nNpcIdx2, int nRangeSquare)
{
	int nDistanceSquare = NpcSet.GetDistanceSquare(nNpcIdx1, nNpcIdx2);

	if(-1 != nDistanceSquare && nDistanceSquare <= nRangeSquare)
		return true;
	else
		return false;
}

int CheckCreateCond(int nPlayerIdx, int nTplId, int nLayer)
{
	if( !IsLayerValid(nTplId, nLayer) || !IsValidPlayer(nPlayerIdx))
		return enSocialErr_CreateFailed;
	
	if (GetMinSubUnitCnt(nTplId, nLayer) > 1)
	{
		int	nTeamId     = GetTeamId(nPlayerIdx);
		int	teamMembers[MAX_BIG_TEAM_MEMBER + 1];
		int	nTeamMemNum = GetTeamMember(nTeamId, teamMembers, MAX_BIG_TEAM_MEMBER + 1);
		
		if( nTeamMemNum < GetMinSubUnitCnt(nTplId, nLayer) )
			return enSocialErr_TeamMemTooLess;
		
		if( nTeamMemNum > GetMaxSubUnitCnt(nTplId, nLayer) )
			return enSocialErr_TeamMemTooMore;
		
		if( !IsTeamCaptain(nTeamId, nPlayerIdx) )
			return enSocialErr_CreaterMustBeCaption;
		
		RelationLayer *pCurLayer = GetRelationLayer(nTplId, nLayer - 1);
		RelationLayer *pNextLayer = GetRelationLayer(nTplId, nLayer);
		
		if(NULL == pCurLayer || NULL == pNextLayer)
		{
			_ASSERT(0);
			return enSocialErr_CreateFailed;
		}
		
		for(int nTeamMember = 0; nTeamMember < nTeamMemNum; ++nTeamMember)
		{
			// 有可能玩家已经加入了社会关系，但是上线时其关系数据没有load
			// 进来，此时应该不允许玩家创建节点
			RelationSet	&set = GetRelationSet(teamMembers[nTeamMember]);
			if( !set.IsOwnTreeLoad(nTplId) )
				return enSocialErr_MemberInvalid;
			
			int	nLevel = GetPlayerLevel(teamMembers[nTeamMember]);
			
			if(nLevel < pCurLayer->Operations[enSUO_CreateUnit].RequireLevel)
				return enSocialErr_LevelInValid;
			
			SocialUnit	*pLeafUnit = GetLeafUnit(teamMembers[nTeamMember], nTplId);
			SocialUnit	*pTopUnit = GetTopUnit(pLeafUnit);
			
			if(NULL == pTopUnit || pTopUnit->GetLayer() + 1 != nLayer || pTopUnit->GetTplId() != nTplId)
				return enSocialErr_RelationInvalid;
			
			if( !pTopUnit->IsOwner( GetPlayerName(teamMembers[nTeamMember]) ) )
				return enSocialErr_RelationInvalid;
			
			int	nNpcIdx = Player[teamMembers[nTeamMember]].m_nIndex;
			if( BuffMgr::Singleton().IsHaveBuff(nNpcIdx, pNextLayer->BuffMustNotHaveOnJoin) )
				return enSocialErr_HaveLeaveBuff;
			
			static int __MAXDISTANCESQUARE = 1024 * 1024;	
			
			if( !IsInRange(Player[nPlayerIdx].m_nIndex, nNpcIdx, __MAXDISTANCESQUARE) )
				return enSocialErr_TeamMemberTooFar;
		}//end for nTeamMember
		
	}//endif
	else
	{
		_ASSERT(GetMinSubUnitCnt(nTplId, nLayer)==1);
		//don't need team
		if (!IsValidPlayer(nPlayerIdx) || GetMinSubUnitCnt(nTplId, nLayer)!=1 )
			return enSocialErr_CreateFailed;

		RelationLayer *pCurLayer  = GetRelationLayer(nTplId, nLayer - 1);
		RelationLayer *pNextLayer = GetRelationLayer(nTplId, nLayer);
		
		if(NULL == pCurLayer || NULL == pNextLayer)
		{
			_ASSERT(0);
			return enSocialErr_CreateFailed;
		}//endif

		RelationSet	&set = GetRelationSet(nPlayerIdx);
		if( !set.IsOwnTreeLoad(nTplId) )
			return enSocialErr_MemberInvalid;
		
		int	nLevel = GetPlayerLevel(nPlayerIdx);
		
		if (nLevel < pCurLayer->Operations[enSUO_CreateUnit].RequireLevel)
			return enSocialErr_LevelInValid;
		
		SocialUnit	*pLeafUnit = GetLeafUnit(nPlayerIdx, nTplId);
		SocialUnit	*pTopUnit  = GetTopUnit(pLeafUnit);
		
		if(NULL == pTopUnit || pTopUnit->GetLayer() + 1 != nLayer || pTopUnit->GetTplId() != nTplId)
			return enSocialErr_RelationInvalid;
		
		if( !pTopUnit->IsOwner( GetPlayerName(nPlayerIdx) ) )
			return enSocialErr_RelationInvalid;
		
		int	nNpcIdx = Player[nPlayerIdx].m_nIndex;
		if( BuffMgr::Singleton().IsHaveBuff(nNpcIdx, pNextLayer->BuffMustNotHaveOnJoin) )
			return enSocialErr_HaveLeaveBuff;

	}//end else
	
	return enSocialErr_None;
}

void DeductCreateCost(int nPlayerIdx, int nTplId, int nLayer)
{
	// to do
}

// Don't inline the following three functions:
// GetUpNUnit GetTopUnit GetLeafUnit
// Avoid head files depend cycle depending.
SocialUnit* GetUpNUnit(SocialUnit *pUnit, int nLayer)
{
	for(; pUnit; pUnit = pUnit->GetParent())
		if(pUnit->GetLayer() == nLayer)
			break;

	return pUnit;
}

SocialUnit*	GetTopUnit(SocialUnit *pUnit)
{
	SocialUnit	*pCurUnit = NULL;

	for(; pUnit; pUnit = pUnit->GetParent())
		pCurUnit = pUnit;
	
	return pCurUnit;
}

SocialUnit* GetLeafUnit(int nPlayerIdx, int nTplId)
{
	RelationSet &relationSet = GetRelationSet(nPlayerIdx);
	RelationRecord	*pRelRecord = relationSet.GetRelationByTemplate(nTplId);

	return pRelRecord ? pRelRecord->pLeafUnit : NULL;
}

const char*	GetLayerName(int nTplId, int nLayer)
{
	RelationLayer *pLayer = GetRelationLayer(nTplId, nLayer);

	if(pLayer)
		return	pLayer->Name;
	else
		return	NULL;
}

int	FormatMsg(char *pBuf, int nBufSize, const char *szFormat, ...)
{
	va_list	va;
	va_start(va, szFormat);
	int	nRetSize = vsnprintf(pBuf, nBufSize, szFormat, va);
	va_end(va);

	_ASSERT(nRetSize >= 0 && nRetSize < nBufSize);

	// 返回的长度包含\0
	if(nRetSize < 0 || nRetSize >= nBufSize)
	{
		nRetSize = nBufSize - 1;
		pBuf[nBufSize - 1] = '\0';
	}

	return nRetSize + 1;
}

void SocialMsgToClient(int nPlayerIdx, const char *szMsg, int nMsgSize)
{
	char			buf[MAXSIZE_HINT_MSG + sizeof(S2C_SOCIAL_MSG)];
	S2C_SOCIAL_MSG	*ps2cMsg = (S2C_SOCIAL_MSG*)buf;

	_ASSERT(nMsgSize <= MAXSIZE_HINT_MSG);
	if(nMsgSize > MAXSIZE_HINT_MSG )
		return;

	ps2cMsg->comHeader.proHeader.protocol = s2c_social_family;
	ps2cMsg->comHeader.proHeader.subProtocol = enSRProtocol_Msg2Client;
	ps2cMsg->msgLen = (WORD)nMsgSize;
	memcpy(ps2cMsg->msg, szMsg, nMsgSize);
	ps2cMsg->comHeader.proHeader.len = sizeof(S2C_SOCIAL_MSG) - 1 + nMsgSize - PROTOCOL_SIZE;

	SendDataToClient(nPlayerIdx, buf, ps2cMsg->comHeader.proHeader.len + PROTOCOL_SIZE);
}

void NotifyBeRemovedOffLine(int nPlayerIdx, int nTplId, int nLayer, const char *szFormat)
{
	_ASSERT(szFormat);
	if(NULL == szFormat)
		return;

	const char	 *szLayerName = GetLayerName(nTplId, nLayer);

	if(NULL == szLayerName)
		szLayerName = SOCIAL_EMPTY_STRING;
		
	char	hintMsg[MAXSIZE_HINT_MSG];
	int		nHintMsgSize = FormatMsg(hintMsg, sizeof(hintMsg), szFormat, szLayerName);
	SocialMsgToClient(nPlayerIdx, hintMsg, nHintMsgSize);			
}

int	FormatAddRemoveUnitMsg(char *pBuf, int nBufSize, SocialUnit *pParent, const char *szFormat)
{
	_ASSERT(pBuf && pParent);

	int	nTpl = pParent->GetTplId();
	int	nLayer = pParent->GetLayer();

	const char *szLayerName = GetLayerName(nTpl, nLayer);
	if(NULL == szLayerName)
		szLayerName = SOCIAL_EMPTY_STRING;

	const char *szUnitName = GetUnitName( pParent->GetUnitAttr() );
	if(NULL == szUnitName)
		szUnitName = SOCIAL_EMPTY_STRING;

	const char *szLauncherName = pParent->GetOwnerName();
	if(NULL == szLauncherName)
		szLauncherName = SOCIAL_EMPTY_STRING;

	return FormatMsg(pBuf,
					 nBufSize,
					 szFormat,
					 szLauncherName,
					 szLayerName,
					 szUnitName
					);
}

int	FormatJoinLeaveUnitMsg(char *pBuf, int nBufSize, SocialUnit *pParent, const char *szFormat)
{
	_ASSERT(pBuf && pParent);

	int	nTpl = pParent->GetTplId();
	int	nLayer = pParent->GetLayer();

	const char *szLayerName = GetLayerName(nTpl, nLayer);
	if(NULL == szLayerName)
		szLayerName = SOCIAL_EMPTY_STRING;

	const char *szUnitName = GetUnitName( pParent->GetUnitAttr() );
	if(NULL == szUnitName)
		szUnitName = SOCIAL_EMPTY_STRING;

	return FormatMsg(pBuf,
					 nBufSize,
					 szFormat,
					 szLayerName,
					 szUnitName
					);
}

int	FormatSubUnitJoinLeaveMsg(char *pBuf, int nBufSize, SocialUnit *pSubUnit, const char *szFormat)
{
	_ASSERT(pBuf && pSubUnit && szFormat);

	int	nTpl = pSubUnit->GetTplId();
	int	nLayer = pSubUnit->GetLayer();

	const char *szSubLayerName = GetLayerName(nTpl, nLayer);
	if(NULL == szSubLayerName)
		szSubLayerName = SOCIAL_EMPTY_STRING;

	const char *szLayerName = GetLayerName(nTpl, nLayer + 1);
	if(NULL == szLayerName)
		szLayerName = SOCIAL_EMPTY_STRING;

	const char *szUnitName;

	if(enSULayer_Player == nLayer)
		szUnitName = pSubUnit->GetOwnerName();
	else
		szUnitName = GetUnitName( pSubUnit->GetUnitAttr() );

	if(NULL == szUnitName)
		szUnitName = SOCIAL_EMPTY_STRING;

	return FormatMsg(pBuf,
					 nBufSize,
					 szFormat,
					 szSubLayerName,
					 szUnitName,
					 szLayerName
					);
}

int	FormatChatPrivChgMsg(char *pBuf, 
						 int nBufSize, 
						 const char *szLauncherName,
						 SocialUnit *pChannelUnit, 
						 const char *szFormat
						 )
{
	_ASSERT(pBuf && pChannelUnit && szFormat && szLauncherName);

	int	nTpl = pChannelUnit->GetTplId();
	int	nLayer = pChannelUnit->GetLayer();

	const char *szLayerName = GetLayerName(nTpl, nLayer);
	if(NULL == szLayerName)
		szLayerName = SOCIAL_EMPTY_STRING;

	if(NULL == szLauncherName)
		szLauncherName = SOCIAL_EMPTY_STRING;

	return FormatMsg(pBuf, 
					 nBufSize,
					 szFormat,
					 szLayerName,
					 szLauncherName
					 );
}

int     FormatChangeOwnerMsg(char *pBuf,
							 int   nBuffSize,
							 const char * szOldOwner,
							 const char * szNewOwner,
							 SocialUnit * pUnit,
							 const char * szFormat)
{
	
	_ASSERT(pBuf && szOldOwner && szNewOwner && pUnit && szFormat);
	
	switch (pUnit->GetLayer())
	{
	case enSULayer_Gens:
		return FormatMsg(pBuf, 
			nBuffSize,
			szFormat,
			szOldOwner,
			MSG_SOCIAL_SHIZUZHANG,
			szNewOwner
			);
		break;
		
    case enSULayer_Tong:
		return FormatMsg(pBuf, 
			nBuffSize,
			szFormat,
			szOldOwner,
			MSG_SOCIAL_ZHUHOUZHANG,
			szNewOwner
			);
		
		break;

	case enSULayer_League:
		return FormatMsg(pBuf,
			nBuffSize,
			szFormat,
			szOldOwner,
			MSG_SOCIAL_MENZU,
			szNewOwner
			);

		break;
	
	default: return FormatMsg(pBuf, 
				 nBuffSize,
				 szFormat,
				 szOldOwner,
				 szNewOwner,
				 " "
				 );	
	}
}

int	SaveSubUnitsToBuf(char *pBuf, int nBufSize, SocialUnit *pUnit)
{
	if(NULL == pUnit || NULL == pBuf)
		return 0;

	SocialUnit::UnitIterator iter;
	SocialUnit	*pSubUnit;
	int			nUseSize = 0;

	while( pSubUnit = pUnit->NextSubUnit(iter) )
	{
		if ( pSubUnit->GetParent() != pUnit )  //避免数据库错误引起严重问题
		{
			_ASSERT(false);
			continue;
		}//endif

		nUseSize += pSubUnit->SaveUnitToBuf(pBuf + nUseSize, nBufSize - nUseSize);
	}

	return nUseSize;
}

void SetRecParentGuid(const char *szPlayerName, int nTplId, const FSGUID *pParentGuid)
{
	int	nPlayerIdx = g_PlayerInfoToIndex.GetIndexByName(szPlayerName);

	if(INVALID_PLAYER_INDEX != nPlayerIdx)
	{
		RelationSet	&rset = GetRelationSet(nPlayerIdx);
		RelationRecord	*pRec = rset.GetRelationByTemplate(nTplId);

		_ASSERT(pRec);

		if(pRec)
		{
			if(pParentGuid)
				pRec->ParentGuid = *pParentGuid;
			else
				memset(&pRec->ParentGuid, 0, sizeof(pRec->ParentGuid));

			//InstantSave
			//Player[nPlayerIdx].SaveBaseInfoData();
		}
	}	
}

DWORD	GetChatRoomId(const SocialUnitAttr &attr)
{
	char	*pId;
	int		nSize;

	nSize = attr.GetAttr(enSUAttr_ChatChannel, pId);
	
	if(nSize != sizeof(DWORD))
		return INVALID_ROOM_ID;

	DWORD	dwRoomId = *((DWORD*)pId);

	return dwRoomId;
}

const char*	GetUnitName(const SocialUnitAttr &attr)
{
	char	*pName = NULL;
	int		nSize;

	nSize = attr.GetAttr(enSUAttr_UnitName, pName);

	return	pName;
}

int	GetCityMapId(const SocialUnitAttr &attr)
{
	char	*pMapId;
	int		nSize;

	nSize = attr.GetAttr(enSUAttr_CityMap, pMapId);

	if(nSize != sizeof(int))
		return INVALID_WORLD_ID;

	int		nMapId = *( (int*)pMapId );
	
	return	nMapId;
}

int GetPoolMapId(const SocialUnitAttr & attr)
{
	char	*pMapId;
	int		nSize;
	
	nSize = attr.GetAttr(enSUAttr_PoolMap, pMapId);
	
	if(nSize != sizeof(int))
		return INVALID_WORLD_ID;
	
	int		nMapId = *( (int*)pMapId );
	
	return	nMapId;
}

int GetForceSubNum(const SocialUnitAttr & attr)
{
	char	*pForceSub;
	int		nSize;
	
	nSize = attr.GetAttr(enSUAttr_ForceSubUnitMaxNum, pForceSub);
	
	if(nSize != sizeof(int))
		return 0;
	
	int		nForceSubCont = *( (int*)pForceSub );
	
	return	nForceSubCont;
}

void UpdateLeafUnitPriv(int nNetId, SocialUnit *pUnit)
{
	if(NULL == pUnit)
		return;

	_ASSERT(enSULayer_Player + 1 != pUnit->GetLayer() || 
		(enSULayer_Player + 1 == pUnit->GetLayer() &&  pUnit->IsAllSubUnitLoad() )
		);

	if( enSULayer_Player + 1 == pUnit->GetLayer() && pUnit->IsAllSubUnitLoad())
	{
		SocialSerializer::Singleton().UpdateAppDataReq(nNetId, pUnit);

#ifdef _DEBUG
		CFS_FILELOGS::WriteDebugLog("Update Leaf Units Privilege: %s\n", 
									GetUnitName(pUnit->GetUnitAttr()) 
									);
#endif
	}
	else if( pUnit->GetLayer() > enSULayer_Player + 1 )
	{
		SocialUnit::UnitIterator iter;
		SocialUnit	*pSubUnit = NULL;

		while( pSubUnit = pUnit->NextSubUnit(iter) )
			UpdateLeafUnitPriv(nNetId, pSubUnit);
	}
}

bool CheckLeafUnitPrivVerion(SocialUnit * pUnit)
{
	if (pUnit->GetPrivilegeSet().GetPrivilegeVersion() < CURRENT_SOCIALDATA_PRIV_VERSIONNO)
	{
		//DoPrivelege eidt when privilege version changed

		//This maybe changed next time 
		pUnit->GetPrivilegeSet().SetPrivilegeVersion(CURRENT_SOCIALDATA_PRIV_VERSIONNO);
	}//endif

	return true;
}

void UpdateDBOnUnitJoin(int nNetId, SocialUnit *pParent, SocialUnit *pSubUnit)
{
	_ASSERT(pParent && pSubUnit);
	if(NULL == pParent || NULL == pSubUnit)
		return;

	SocialSerializer &ss = SocialSerializer::Singleton();
	ss.UpdateSubUnitCntReq(nNetId, pParent);

	if(pSubUnit->GetLayer() > enSULayer_Player)
	{
		ss.UpdatePGuidReq(nNetId, pSubUnit);
		UpdateLeafUnitPriv(nNetId, pSubUnit);
	}
	else
	{
		_ASSERT( enSULayer_Player + 1 == pParent->GetLayer() );
		UpdateLeafUnitPriv(nNetId, pParent);
	}
}

void UpdateDBOnCreateUnit(int nNetId, SocialUnit *pNewUnit)
{
	_ASSERT(pNewUnit);
	if(NULL == pNewUnit)
		return;

	if(pNewUnit->GetLayer() <= enSULayer_Player)
	{
		_ASSERT(false);
		return;
	}

	SocialSerializer &serializer = SocialSerializer::Singleton();

	// 如果子节点层次等于enSULayer_Player，
	// 权限会在随后的AddRecordReq中存储
	// 而且不用更新父节点
	if( pNewUnit->GetLayer() > enSULayer_Player + 1 )
	{
		SocialUnit::UnitIterator iter;
		SocialUnit	*pSubUnit;

		while( pSubUnit = pNewUnit->NextSubUnit(iter) )
		{
			serializer.UpdatePGuidReq(nNetId, pSubUnit);
			UpdateLeafUnitPriv(nNetId, pSubUnit);
		}
	}
			
	serializer.AddRecordReq(nNetId, pNewUnit);
}

bool AddBuffToUnit(SocialUnit *pUnit, int nBuffId)
{
	_ASSERT(NULL != pUnit);

	if(INVALID_BUFF_ID != nBuffId)
	{
		int nPlayerIdx = g_PlayerInfoToIndex.GetIndexByName( pUnit->GetOwnerName() );

		if(INVALID_PLAYER_INDEX != nPlayerIdx)
		{
			BuffMgr::Singleton().AddNpcBuff(Player[nPlayerIdx].m_nIndex, 
				Player[nPlayerIdx].m_nIndex,
				nBuffId
				);

			return true;
		}
	}

	return false;
}

void RefreshPlayerInfoAttr(SocialUnitAttr &attr, int nPlayerIdx, bool bOnline)
{
	SU_PLAYER_INFO	playerInfo;
	playerInfo.isOnline = bOnline;

	DWORD                 dwBaseProfession   = GetPlayerSeries(nPlayerIdx);
	DWORD                 dwHiwordProfession = 0x0000000f;  //Invalid means 
	
	if (Player[nPlayerIdx].IsValid() && Player[nPlayerIdx].GetSkillSeries()!=role_skillseries_invalid)
	{
        dwHiwordProfession = Player[nPlayerIdx].GetSkillSeries();
	}//endif

	DWORD                   dwProfessionFinal  = dwBaseProfession + (dwHiwordProfession<<4);
	playerInfo.profession                      = (BYTE)dwProfessionFinal;

	playerInfo.level = (WORD)GetPlayerLevel(nPlayerIdx);

	if( attr.IsAttrHasData(enSUAttr_PlayerInfo) )
		attr.ChangeAttr(enSUAttr_PlayerInfo, (const char*)&playerInfo, sizeof(playerInfo));
	else
		attr.AddAttr(enSUAttr_PlayerInfo, (const char*)&playerInfo, sizeof(playerInfo));	
}

void RefreshPlayerInfoAttr(int nPlayerIdx, bool bOnline)
{
	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);

	if(NULL != pLeafUnit)		
		RefreshPlayerInfoAttr(pLeafUnit->GetUnitAttr(), nPlayerIdx, bOnline);
}

bool IsUnitOwner(int nPlayerIdx, int nTplId, int nLayer)
{
	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, nTplId);
	SocialUnit *pTargetUnit = GetUpNUnit(pLeafUnit, nLayer);

	if(NULL != pTargetUnit)
		return pTargetUnit->IsOwner( GetPlayerName(nPlayerIdx) );
	else
		return false;
}

const FSGUID* GetUnitGuid(int nPlayerIdx, int nTplId, int nLayer)
{
	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, nTplId);
	SocialUnit *pTargetUnit = GetUpNUnit(pLeafUnit, nLayer);

	if(NULL != pTargetUnit)
		return &pTargetUnit->GetUnitGuid();
	else
		return NULL;
}

void NotifyDeclareWarSucceed(int nPlayerIdx)
{
	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
	SocialUnit *pTargetUnit = GetTopUnit(pLeafUnit);

	if(NULL == pTargetUnit)
		return;

	const char *szUnitName = GetUnitName( pTargetUnit->GetUnitAttr() );

	if(NULL == szUnitName)
		return;

	int nWorldIdx = Npc[Player[nPlayerIdx].m_nIndex].GetSubWorldIndex();
	int nWorldId = SubWorld[nWorldIdx].m_SubWorldID;
	char szWorldName[MAXSIZE_WORLD_NAME];
	g_SubWorldSet.GetWorldNameFromID(nWorldId, szWorldName, sizeof(szWorldName));
	
	char szMsg[256];
	int nLen = snprintf(szMsg, sizeof(szMsg), MSG_WAR_DECLAREWAR, szUnitName, szWorldName);
	szWorldName[MAXSIZE_WORLD_NAME - 1] = 0;

	if(nLen < 0 || nLen >= sizeof(szMsg))
	{
		nLen = 0;
		szMsg[nLen] = 0;
	}

	g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_STR, 
		(const BYTE*)szMsg, 
		nLen,
		SYSTEM_ROOM_ID,
		MSG_SHOWTYPE_ROOM | MSG_SHOWTYPE_MIDDLESCREEN | MSG_SHOWTYPE_TOPSCREEN
		);
}

#define MAXSIZE_NOTIFY_STR 150
void NotifyCityLeagueBreak(const char* pUnitName, int nMapId)
{
	if ( !pUnitName || nMapId == INVALID_WORLD_ID )
		return ;
	
	char notifyStrBuff[MAXSIZE_NOTIFY_STR] = { 0 };
	WorldSetting* pSetting = g_SubWorldSet.GetWorldSetting(nMapId);
	
	if ( pSetting && pSetting->Name )
	{
		int  nLen = snprintf(notifyStrBuff, sizeof(notifyStrBuff), MSG_SOCIAL_LEAGUE_BREAK, pUnitName, pSetting->Name);
		notifyStrBuff[sizeof(notifyStrBuff) - 1] = 0;
		
		g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_STR, 
			(const BYTE*)notifyStrBuff, 
			nLen,
			SYSTEM_ROOM_ID,
			MSG_SHOWTYPE_ROOM | MSG_SHOWTYPE_MIDDLESCREEN | MSG_SHOWTYPE_TOPSCREEN
			);
	}
}

void NotifyWarInfo(int nPlayerIdx)
{
	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
	SocialUnit *pTongUnit = GetUpNUnit(pLeafUnit, enSULayer_League);

	if(NULL == pTongUnit)
		return;

	const FSGUID &tongGuid = pTongUnit->GetUnitGuid();

	const FSWarInfo *pWarInfo;
	KWarInfoManager::SelfIterator iter;
	KWarInfoManager& warMgr = GetGlobalWarInfoManager();
	
	while( (pWarInfo = warMgr.NextRecord(iter)) != NULL )
	{
		char	szMapName[MAXSIZE_WORLD_NAME];
		if( !g_SubWorldSet.GetWorldNameFromID(pWarInfo->mapID, szMapName, sizeof(szMapName)) )
			continue;
		
		szMapName[sizeof(szMapName) - 1] = 0;
		
		char	szMsg[MAXSIZE_CHAT_MSG];
		int		nMsgLen=0;
		
		if (pWarInfo->invaderGUID == tongGuid ) //正在攻打它城
		{
			if(FS_WAR_STATE_NOTIFY == pWarInfo->warState)
				nMsgLen = snprintf(szMsg, sizeof(szMsg), MSG_WAR_NOTIFY_DECLARED, szMapName);
			else if(FS_WAR_STATE_PROCESS == pWarInfo->warState)
			{
				bool bProtected = false;
				
				int nWorldIndex          = g_SubWorldSet.SearchWorld(pWarInfo->mapID);
				if (nWorldIndex != INVALID_WORLD_INDEX)
				{
					int       nLifeDeathPercent    = ConfigManager::Singleton().GetGlobalVariable(global_var_tong_war_death_percent);
					int       nLordScapegoatBuffID = ConfigManager::Singleton().GetGlobalVariable(global_var_tong_war_scapegoat_buff);
					
					for (int nSubLord = 0; nSubLord< SUBLORD_COUNT ;nSubLord ++ )
					{
						int iSubLordIndex = SubWorld[nWorldIndex].GetSubLord(nSubLord);
						if (IsValidNpc(iSubLordIndex))
						{
							if (Npc[iSubLordIndex].GetLoadSaveState() == npc_load_save_state_waiting || Npc[iSubLordIndex].GetLoadSaveState() ==  npc_load_save_state_loading )
								continue;
							
							if(BuffMgr::Singleton().IsHaveBuff(iSubLordIndex,nLordScapegoatBuffID) && Npc[iSubLordIndex].GetCurrentLifePercentage() > nLifeDeathPercent)
							{
								bProtected    = true;
								break;
							}//endif
							
						}//endif
						
					}//end for nSublord
					
				}//endif
				
				if (bProtected)
					nMsgLen = snprintf(szMsg, sizeof(szMsg), MSG_WAR_NOTIFY_PROCESS_PROTECTED, szMapName);
				else	
					nMsgLen = snprintf(szMsg, sizeof(szMsg), MSG_WAR_NOTIFY_PROCESS, szMapName);	
			}//end else
			
		}//endif
		
		if (pWarInfo->defenderGUID == tongGuid )//正在被攻击
		{
			SocialUnit * pInvader      = ServerSocialUnitMgr::Singleton().GetUnit( pWarInfo->invaderGUID );
			
			if (pInvader)
			{
				const char * szInvaderName = GetUnitName(pInvader->GetUnitAttr());
				
				if (szInvaderName)
				{
					if(FS_WAR_STATE_NOTIFY == pWarInfo->warState)
						nMsgLen = snprintf(szMsg, sizeof(szMsg), MSG_WAR_NOTIFY_DECLARED_DEFENDER, szInvaderName);
					else if(FS_WAR_STATE_PROCESS == pWarInfo->warState)
						nMsgLen = snprintf(szMsg, sizeof(szMsg), MSG_WAR_NOTIFY_PROCESS_DEFENDER, szInvaderName);
				}//endif
				
			}//endif

		}//endif

		if(nMsgLen <= 0 || nMsgLen >= sizeof(szMsg))
			continue;

		g_ChatCenterS.SysMsgToSomeone(nPlayerIdx, SYSMSG_TYPE_STR, (const BYTE*)&szMsg, nMsgLen);
	}

	const PoolCombatInfo *pPoolInfo;
	KPoolCombatInfoManager::SelfIterator it;
	KPoolCombatInfoManager& poolMgr = GetPoolCombatInfoManager();
	
	while( (pPoolInfo = poolMgr.NextRecord(tongGuid, it)) != NULL )
	{
		char	szMapName[MAXSIZE_WORLD_NAME];
		if( !g_SubWorldSet.GetWorldNameFromID(pPoolInfo->mapID, szMapName, sizeof(szMapName)) )
			continue;
		
		szMapName[sizeof(szMapName) - 1] = 0;
		
		char	szMsg[MAXSIZE_CHAT_MSG];
		int		nMsgLen=0;
		
		if(FS_POOL_COMBAT_STATE_PROCESS == pPoolInfo->warState)
			nMsgLen = snprintf(szMsg, sizeof(szMsg), MSG_POOL_NOTIFY_PROCESS, szMapName);
		
		if(nMsgLen <= 0 || nMsgLen >= sizeof(szMsg))
			continue;
		
		g_ChatCenterS.SysMsgToSomeone(nPlayerIdx, SYSMSG_TYPE_STR, (const BYTE*)&szMsg, nMsgLen);
	}

}

void NotifyRobRes(const char *szInvaderTongName, int nMapId, int nResType, int nValue)
{
	nResType -= nuai_lord_res0;

	static const char *RESNAME[] = 
		{
			NAME_CITYRES_0,
			NAME_CITYRES_1,
			NAME_CITYRES_2,
			NAME_CITYRES_3
		};

	if(NULL == szInvaderTongName)
		return;

	if(nResType < 0 || nResType > 3)
		return;

	char szMapName[MAXSIZE_ORGNAME];
	if( !g_SubWorldSet.GetWorldNameFromID(nMapId, szMapName, sizeof(szMapName)) )
		return;

	szMapName[sizeof(szMapName) - 1] = 0;

	char szNotifyMsg[MAXSIZE_CHAT_MSG];
	int	nMsgLen =0 ;
	nMsgLen = snprintf(szNotifyMsg, 
		sizeof(szNotifyMsg), 
		MSG_WAR_NOTIFY_ROBRES,
		szInvaderTongName,
		szMapName,
		nValue,
		RESNAME[nResType]
		);

	if(nMsgLen > 0 && nMsgLen < sizeof(szNotifyMsg))
		g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_STR, (const BYTE*)&szNotifyMsg, nMsgLen);
}

void NotifyRobCity(const char *szInvaderTongName, int nMapId)
{
	if(NULL == szInvaderTongName)
		return;

	char szMapName[MAXSIZE_ORGNAME];
	if( !g_SubWorldSet.GetWorldNameFromID(nMapId, szMapName, sizeof(szMapName)) )
		return;

	szMapName[sizeof(szMapName) - 1] = 0;

	char szNotifyMsg[MAXSIZE_CHAT_MSG];
	int nMsgLen =0;
	nMsgLen = snprintf(szNotifyMsg, 
		sizeof(szNotifyMsg), 
		MSG_WAR_NOTIFY_ROBCITY, 
		szInvaderTongName, 
		szMapName
		);

	if(nMsgLen > 0 && nMsgLen < sizeof(szNotifyMsg))
		g_ChatCenterS.SysMsgToAll(SYSMSG_TYPE_STR, (const BYTE*)&szNotifyMsg, nMsgLen);
}

void NotifyCityDiscount(int nCityLordIdx)
{
	char szNotifyMsg[MAXSIZE_CHAT_MSG];
	int	 nMsgLen =0;

	nMsgLen = snprintf(szNotifyMsg, 
		sizeof(szNotifyMsg), 
		MSG_CITY_NOTIFY_DISCOUNT, 
		Npc[nCityLordIdx].m_UnaryAttrMgr[nuai_city_taxrate]
		);

	if(nMsgLen > 0 && nMsgLen < sizeof(szNotifyMsg))
	{
		const FSGUID &tongGuid = Npc[nCityLordIdx].GetLord();
		SocialUnit *pTongUnit = ServerSocialUnitMgr::Singleton().GetUnit(tongGuid, enSUTplId_Tong);

		g_ChatCenterS.SysMsgToTong(pTongUnit, SYSMSG_TYPE_STR, (const BYTE*)szNotifyMsg, nMsgLen);
	}
}

int	GetCityTaxRateBuff()
{
	RelationLayer *pLayer = GetRelationLayer(enSUTplId_Tong, enSULayer_League);

	if(NULL != pLayer)
		return pLayer->CityTaxRateBuff;
	else
		return 0;
}

void NotifyMemberOnline(const char *szMemberName, SocialUnit *pParentUnit, bool bOnline)
{
	if(NULL != szMemberName && NULL != pParentUnit)	
	{
		const char *szFormat = bOnline ? MSG_MEMBER_ONLINE_NOTIFY : MSG_MEMBER_OFFLINE_NOTIFY;

		char szNotifyMsg[MAXSIZE_CHAT_MSG];
		int	 nMsgLen =0 ;
		
		nMsgLen = snprintf(szNotifyMsg,
			sizeof(szNotifyMsg),
			szFormat,
			szMemberName
			);

		if(nMsgLen > 0 && nMsgLen < sizeof(szNotifyMsg))
		{
			DWORD dwShowRoomId;
			dwShowRoomId = GetChatRoomId( pParentUnit->GetUnitAttr() );

			if(INVALID_ROOM_ID == dwShowRoomId)
				dwShowRoomId = SYSTEM_ROOM_ID;

			g_ChatCenterS.SysMsgToTong(pParentUnit, 
				SYSMSG_TYPE_STR,
				(const BYTE*)szNotifyMsg,
				nMsgLen + 1,
				dwShowRoomId,
				MSG_SHOWTYPE_ROOM
				);
		}
	}
}

#define MAX_INSTANCE_DATA_BUFF_LENGTH (sizeof(PDBInstanceData) * INSTANCE_SUBWORLD_START + 1)

DWORD GetSocialInstanceId(SocialUnit *pUnit, int worldTemplateId)
{
	if (NULL == pUnit)
		return INVALID_INSTANCE_ID;
	
	char* pInstanceData = NULL;
	int dataSize = pUnit->GetUnitAttr().GetAttr(enSUAttr_InstanceInfo, pInstanceData);
	if (dataSize > 0)
	{
		InstanceInfo instanceInfo;
		instanceInfo.Load((BYTE*)pInstanceData, dataSize);
		return instanceInfo.GetInstanceId(worldTemplateId);
	}

	return INVALID_INSTANCE_ID;
}

bool SetSocialInstanceId(SocialUnit *pUnit, int worldTemplateId, DWORD instanceId)
{										
	if (NULL == pUnit)
		return false;

	InstanceInfo instanceInfo;
	char* pSocialInstanceData = NULL;
	SocialUnitAttr& attr = pUnit->GetUnitAttr();
	int originalDataSize = attr.GetAttr(enSUAttr_InstanceInfo, pSocialInstanceData);
	if (originalDataSize > 0)
	{
		instanceInfo.Load((BYTE*)pSocialInstanceData, originalDataSize);
	}
	
	instanceInfo.SetInstanceId(worldTemplateId, instanceId);												
	char newInstanceData[MAX_INSTANCE_DATA_BUFF_LENGTH];
	memset(newInstanceData, 0, sizeof(newInstanceData));
	int newInstanceDataSize = instanceInfo.Save((BYTE*)newInstanceData, sizeof(newInstanceData));
	
	if (originalDataSize > 0)
	{
		return attr.ChangeAttr(enSUAttr_InstanceInfo, newInstanceData, newInstanceDataSize);
	}
	else
	{
		return attr.AddAttr(enSUAttr_InstanceInfo, newInstanceData, newInstanceDataSize);
	}
}


#define SCRIPT_DATA_BUFF sizeof(SCRIPT_DATA)	//这里是 脚本变量个数*每个元素的长度 + 脚本元素计数 (注意这个Buff是当前server版本的脚本数据的存储Buff大小
												//从数据库中读出的大小肯定是 <= 这个数值的，要以count分量为准)

bool SetSaveScriptData(SocialUnit* pUnit, int nIndex, unsigned long dwData)
{
	if (   pUnit == NULL 
		|| nIndex < 0 
		|| nIndex >= MAX_SOCIAL_SCRIPT_DATA_NUM_NEEDSAVE
		|| pUnit->GetLayer() <= enSULayer_Player)
		return false;
	
	char* pTemp;
	SocialUnitAttr& attr = pUnit->GetUnitAttr();
	int originalDataSize = attr.GetAttr(enSUAttr_ScriptData, pTemp);
	SCRIPT_DATA* pOldData = (SCRIPT_DATA* )pTemp;


	char nNewBuff[SCRIPT_DATA_BUFF];
	memset(nNewBuff, 0, sizeof(nNewBuff));
	SCRIPT_DATA* pNew  = (SCRIPT_DATA* )nNewBuff;

	if (originalDataSize > 0)
	{
		memcpy(pNew, pTemp, sizeof(unsigned long) * (pOldData->nCount + 1));
	}

	pNew->Data[nIndex] = dwData;
	pNew->nCount       = MAX_SOCIAL_SCRIPT_DATA_NUM_NEEDSAVE;
	
	bool bReturn;
	if (originalDataSize > 0)
	{	
		bReturn = attr.ChangeAttr(enSUAttr_ScriptData, nNewBuff, SCRIPT_DATA_BUFF);
		_ASSERT(bReturn);
	}
	else
	{
		bReturn = attr.AddAttr(enSUAttr_ScriptData, nNewBuff, SCRIPT_DATA_BUFF);
		_ASSERT(bReturn);
	}

	
	if (bReturn)
	{
		if( attr.IsAttrSaveToDb(enSUAttr_ScriptData))
			SocialSerializer::Singleton().UpdateAttrReq(-1, pUnit);

		return true;
		
	}//endif

	return false;
}

bool GetSaveScriptData(SocialUnit* pUnit, int nIndex, unsigned long* pData)
{
	//1.test and process the arguments
	if (   pUnit == NULL 
		|| nIndex < 0 
		|| nIndex >= MAX_SOCIAL_SCRIPT_DATA_NUM_NEEDSAVE
		|| pUnit->GetLayer() <= enSULayer_Player)
		return false;

	*pData = 0;

	//2.get the data
	char* pTemp;
	SocialUnitAttr& attr = pUnit->GetUnitAttr();
	int originalDataSize = attr.GetAttr(enSUAttr_ScriptData, pTemp);

	if (originalDataSize > 0)						//有数据
	{
		SCRIPT_DATA* pScriptData = (SCRIPT_DATA* )pTemp;

		if (    pScriptData 
			&&  nIndex < pScriptData->nCount)			//取的时候必需小于Count才有意义
		{
			*pData = pScriptData->Data[nIndex];
			return true;
		}

		return false;
	}

	return false;
}

int GetLordByMapID(int nMapID)
{
	if ( nMapID == INVALID_WORLD_ID )
		return -1;
	
	//1. Get SubWorldIndex
	int nSubWorldIndex = g_SubWorldSet.SearchWorld(nMapID);
	if ( nSubWorldIndex <= INVALID_WORLD_INDEX || nSubWorldIndex > MAX_SUBWORLD )
		return -1;
	
	//2. Get Lord
	return SubWorld[nSubWorldIndex].GetLord();
}

SocialUnit* GetLordSocialUnitByMapID(int nMapID)
{
	int nLordNpcIndex = GetLordByMapID(nMapID);
	
	if ( !IsValidNpc(nLordNpcIndex) )
		return NULL;
	
	const FSGUID& guid = Npc[nLordNpcIndex].GetLord();
	if ( guid.data[0] == 0 )
		return NULL;
	
	return ServerSocialUnitMgr::Singleton().GetUnit(guid, enSUTplId_Tong);
}
