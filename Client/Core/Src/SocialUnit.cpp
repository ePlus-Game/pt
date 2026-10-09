//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:11   14:46
//      File_base        : SocialUnit
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
#include "relation_template.h"
#include "ChatCenter_S.h"
#include "ServerSocialUnitMgr.h"
#include "SocialUnit.h"
#include "SocialSerializer.h"
#include "buff_man.h"
#include "KSubWorldSet.h"
#include "CoreRelated.h"
#include "social_recruit_svr.h"
#include "tong_war_manager.h"


#define   SOCIAL_ANNOUNCEMENT_VERSON_INVALID    0xffffffff

SocialUnit::POPEFUNC	SocialUnit::m_opeProcs[enSUO_Num];

SocialUnit::SocialUnit(int nTplId, int nLayer)
{
	memset(m_IsProcExist, 0, sizeof(m_IsProcExist));
	m_subUnits.clear();
	m_pParent = NULL;
	
	m_tplId = (BYTE)nTplId;
	m_layer = (BYTE)nLayer;
	m_maxJoinedLayer = nLayer;
	m_subUnitCnt = 0;

	// 氏族和其子节点是当作一条记录存储的，所以数据库中不存在子节点
	if(m_layer > enSULayer_Player)
		m_isAllSubUnitLoad = false;
	else
		m_isAllSubUnitLoad = true;

	BindOpe2Proc();	
	InitProcAccess(nTplId, nLayer);
	m_attrs.InitAttrs(nTplId, nLayer);

    m_AnuncmentVersion = 0;

	memset(m_ScriptDatas,0,sizeof(m_ScriptDatas));
}


void SocialUnit::BindOpe2Proc()
{
	static	bool bIsBind = false;
	
	if( !bIsBind )
	{
		memset(m_opeProcs, 0, sizeof(m_opeProcs));

		m_opeProcs[enSUO_CreateUnit]	= &SocialUnit::CreateUnit;
		m_opeProcs[enSUO_AddSubUnit]	= &SocialUnit::AddSubUnit;
		m_opeProcs[enSUO_RemoveSubUnit] = &SocialUnit::RemoveSubUnit;
		m_opeProcs[enSUO_BreakUnit]		= &SocialUnit::BreakUnit;
		m_opeProcs[enSUO_LeaveUnit]		= &SocialUnit::LeaveUnit;
		m_opeProcs[enSUO_ForbidChat]	= &SocialUnit::ForbidChatInUnit;
		m_opeProcs[enSUO_UnForbidChat]	= &SocialUnit::UnForbidChatInUnit;
		m_opeProcs[enSUO_PubAnnouncement] = &SocialUnit::PubAnnouncement;
		m_opeProcs[enSUO_GetSubList]	= &SocialUnit::GetSubList;
		m_opeProcs[enSUO_GetAnnouncement] = &SocialUnit::GetAnnouncement;
		m_opeProcs[enSUO_GetPrePageSubList] = &SocialUnit::GetSubList;
		m_opeProcs[enSUO_GetNextPageSubList] = &SocialUnit::GetSubList;
		m_opeProcs[enSUO_ChangeOwner]        = &SocialUnit::ChangeUnitOwner;
		m_opeProcs[enSUO_ReqJoinHigherLevel] = &SocialUnit::ReqJoinHighLevel;
		m_opeProcs[enSUO_GetRecruitInfo] = &SocialUnit::ReqRecruitInfo;
		m_opeProcs[enSUO_AddRecruitInfo] = &SocialUnit::AddRecruitInfo;
		m_opeProcs[enSUO_DelRecruitInfo] = &SocialUnit::DelRecruitInfo;
		
		bIsBind = true;
	}
}

void SocialUnit::InitProcAccess(int nTplId, int nLayer)
{
	const PRelationLayer pLayer = GetRelationLayer(nTplId, nLayer);

	_ASSERT(pLayer);
	if(NULL == pLayer)
		return;

	_ASSERT(pLayer->OperationCount <= enSUO_Num);

	for(int nOpeLoop = 0; nOpeLoop < enSUO_Num && nOpeLoop < pLayer->OperationCount; ++nOpeLoop)
	{
		int	nOpeId = pLayer->Operations[nOpeLoop].Id;

		_ASSERT(nOpeId > enSUO_None && nOpeId < enSUO_Num);

		if(nOpeId > enSUO_None && nOpeId < enSUO_Num)
			m_IsProcExist[nOpeId] = true;
	}
}

int	SocialUnit::ProcessOperation(int nLauncherIdx, void *pParam, int nMsgSize)
{
	C2S_COMOPE_HEADER	*pc2sHeader = (C2S_COMOPE_HEADER*)pParam;
	int					nOpeId = pc2sHeader->comParam.opeId;

	_ASSERT(nOpeId > enSUO_None && nOpeId < enSUO_Num);
	
	if(nOpeId <= enSUO_None && nOpeId >= enSUO_Num)
		return enSocialErr_OperationNotExist;

	if( !m_IsProcExist[nOpeId] || (NULL == m_opeProcs[nOpeId]) )
		return enSocialErr_OperationNotExist;

	return (this->*m_opeProcs[nOpeId])(nLauncherIdx, pParam, nMsgSize);
}

bool SocialUnit::CreateChannel(int nPlayerIdx)
{
	if( m_attrs.IsAttrExist(enSUAttr_ChatChannel) && !m_attrs.IsAttrHasData(enSUAttr_ChatChannel) )
	{
		ChatRoomMgr_S	&roomMgr = g_ChatCenterS.GetRoomMgr();

		DWORD	dwRoomId;
		int nRet = roomMgr.CreateRoom(m_ownerName, NULL, dwRoomId);

		if(chat_err_none == nRet)
			return m_attrs.AddAttr(enSUAttr_ChatChannel, (const char*)&dwRoomId, sizeof(dwRoomId));
		else if (nPlayerIdx!=-1)
			ChatErrCodeToClient(nPlayerIdx, nRet);
	}	

	return false;
}

bool SocialUnit::SendInvitationToSubPlayer(const char * szTitle, const char * szConstant, const char * szSender, const char * coupleName)
{
	bool ret = false;
	if (szTitle && szConstant && szSender != NULL && coupleName != NULL)
	{
		ret = true;
        if (IsLeaf())
		{
			const char * ownerName = GetOwnerName();
			if (strcmp(ownerName, szSender) != 0)
			{
				if (strcmp(ownerName, coupleName) != 0)
				{
					ret = SystemSendCustomMail(ownerName, szTitle, szConstant, szSender);
				}
			}
		}//endif
		else
		{
			int nMaxCount = (int)m_subUnits.size();

			for(int i = 0; i < nMaxCount; ++i)
			{
				if(m_subUnits[i])
				{
					ret = m_subUnits[i]->SendInvitationToSubPlayer(szTitle, szConstant, szSender, coupleName);
				}

				if (!ret)
				{
					break;
				}
			}//end for i
		}//end else
	}//endif
	return ret;
}

void SocialUnit::SendMailToSubPlayer(const char * szTitle,const char * szConstant)
{
	if (szTitle && szConstant)
	{
        if (IsLeaf())
		{
            SystemSendMail(GetOwnerName(),szTitle,szConstant,0,0,0,NULL);
		}//endif
		else
		{
			int nMaxCount = (int)m_subUnits.size();
			
			for(int i = 0; i < nMaxCount; ++i)
			{
				if(m_subUnits[i])
					m_subUnits[i]->SendMailToSubPlayer(szTitle,szConstant);

			}//end for i

		}//end else

	}//endif

}

int	SocialUnit::CreateUnit(int nLauncherIdx, void *pParam, int nMsgSize)
{
	if (g_pController == NULL)
		return enSocialErr_RetryLater;

	int	nRetCode = CheckCreateCond(nLauncherIdx, m_tplId, m_layer + 1);

	if(enSocialErr_None != nRetCode)
		return nRetCode;

	C2S_CREATEUNIT_REQ	*pc2sParam = (C2S_CREATEUNIT_REQ*)pParam;
	pc2sParam->unitName[sizeof(pc2sParam->unitName) - 1] = '\0';

	if( !g_IsNamePass(pc2sParam->unitName) )
		return enSocialErr_LawlessText;

	// 因为需要先查询数据库名字是否存在，数据返回后，如果名字不存在，
	// 会重新调用这个函数，进行真正的创建过程，这样，就需要一个标记
	// 来标识是否已经查询过数据库了，但参数是从客户端传递过来的
	// 如果单纯用一个bool值来标识是否已经查询过了，很容易被外挂搞掉，
	// 所以这里用当前节点的地址来作为标记，外挂不可能知道这个地址
	if(pc2sParam->tag != (DWORD)this)
	{
		pc2sParam->tag = (DWORD)this;
		SocialSerializer::Singleton().CheckUnitNameReq(GetNetConnectIdx(nLauncherIdx), *pc2sParam);
		return enSocialErr_CheckUnitName;
	}

	// 创建节点并初始化相关属性
	SocialUnit	*pNewUnit = SocialAllocator::AllocUnit(m_tplId, m_layer + 1);
	
	if(NULL == pNewUnit)
		return enSocialErr_CreateFailed;

	FSGUID	guid;
	g_pController->GenGUID(guid.data, g_GuidPadding);
	pNewUnit->SetUnitGuid(guid);
	pNewUnit->SetOwnerName( GetPlayerName(nLauncherIdx) );

	SocialUnitAttr		&newUnitAttr = pNewUnit->GetUnitAttr();

	if( newUnitAttr.IsAttrExist(enSUAttr_ChatChannel) )
	{
		bool bSucceed = pNewUnit->CreateChannel(nLauncherIdx);
		_ASSERT(bSucceed);
	}

	if( newUnitAttr.IsAttrExist(enSUAttr_UnitName) )
		newUnitAttr.AddAttr(enSUAttr_UnitName, pc2sParam->unitName, strlen(pc2sParam->unitName) + 1);

	char	szNotifyMsg[MAXSIZE_HINT_MSG];
	int		nNotifyMsgSize;
	
	nNotifyMsgSize = FormatCreateUnitMsg(szNotifyMsg,
		sizeof(szNotifyMsg),
		pNewUnit,
		MSG_SOCIAL_CREATE_UNIT
		);

	SocialMsgToClient(nLauncherIdx, szNotifyMsg, nNotifyMsgSize);

	nNotifyMsgSize = FormatJoinLeaveUnitMsg(szNotifyMsg, 
		sizeof(szNotifyMsg),
		pNewUnit,
		MSG_SOCIAL_JOIN_UNIT);

	if(GetMinSubUnitCnt(m_tplId, m_layer + 1) > 1)
	{
		int		teamMembers[MAX_BIG_TEAM_MEMBER + 1];
		int		nTeamId = GetTeamId(nLauncherIdx);
	    int		nCount	= GetTeamMember(nTeamId, teamMembers, MAX_BIG_TEAM_MEMBER + 1);

		for(int nMember = 0; nMember < nCount; ++nMember)
		{
			SocialUnit *pLeafUnit = GetLeafUnit(teamMembers[nMember], m_tplId);
			SocialUnit *pTopUnit = GetTopUnit(pLeafUnit);
			
			_ASSERT(pTopUnit);
			
			if(pTopUnit)
			{
				pTopUnit->OnJoinParentUnit(pNewUnit);
				pNewUnit->OnSubUnitJoin(pTopUnit);
				
				if(teamMembers[nMember] != nLauncherIdx)
					SocialMsgToClient(teamMembers[nMember], szNotifyMsg, nNotifyMsgSize);
			}//endif

		}//end for nMember	
		
	}//endif
	else
	{
		_ASSERT(GetMinSubUnitCnt(m_tplId, m_layer + 1) == 1);
		
		SocialUnit *pLeafUnit = GetLeafUnit(nLauncherIdx, m_tplId);
		SocialUnit *pTopUnit  = GetTopUnit(pLeafUnit);
		
		_ASSERT(pTopUnit);
		
		if(pTopUnit)
		{
			pTopUnit->OnJoinParentUnit(pNewUnit);
			pNewUnit->OnSubUnitJoin(pTopUnit);
		}//endif

	}//end else

	//Sepcial case for League Unit 
	if (pNewUnit->GetLayer() == enSULayer_League)
	{
		SocialUnitAttr & newAttr = pNewUnit->GetUnitAttr();
		if (newAttr.IsAttrExist(enSUAttr_ForceSubUnitMaxNum) && !newAttr.IsAttrHasData(enSUAttr_ForceSubUnitMaxNum))
		{
			_ASSERT( LEAGUE_LAYER_DEFAULT_SUBUNIT_MAX <= GetMaxSubUnitCnt(m_tplId,m_layer + 1));
			
			if (LEAGUE_LAYER_DEFAULT_SUBUNIT_MAX <= GetMaxSubUnitCnt(m_tplId,m_layer + 1))
			{
				int         nForceSubUnitMaxNum = LEAGUE_LAYER_DEFAULT_SUBUNIT_MAX;
				newAttr.AddAttr(enSUAttr_ForceSubUnitMaxNum,(const char *)&nForceSubUnitMaxNum,sizeof(nForceSubUnitMaxNum));
			}
			
		}//endif

	}//endif

	pNewUnit->SetAllSubUnitLoadFlag(true);
	ServerSocialUnitMgr::Singleton().AddUnit(pNewUnit->GetUnitGuid(), pNewUnit);
	DeductCreateCost(nLauncherIdx, m_tplId, m_layer + 1);

	if( IsSocialTmplSave(m_tplId) )
		UpdateDBOnCreateUnit(GetNetConnectIdx(nLauncherIdx), pNewUnit);

	KPlayer& player = Player[nLauncherIdx];

	//记录日志
	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_social_unit_create))
	{
		LogEventParam socialUnitCreateEvent;
		socialUnitCreateEvent.event = log_event_social_unit_create;
		socialUnitCreateEvent.param1 = player.GetGUID();
		socialUnitCreateEvent.param2 = pNewUnit->GetUnitGuid();
		if (g_pLogSystem)
			g_pLogSystem->Log(socialUnitCreateEvent);
	}

#ifdef _DEBUG
	CFS_FILELOGS::WriteDebugLog("%s Create Unit: %s %d %d",
								GetPlayerName(nLauncherIdx),
								pc2sParam->unitName,
								m_tplId,
								m_layer + 1
								);
#endif

	return enSocialErr_None;
}

#define  MAX_MAIL_BREAK_CONSTANT_LEN  256

int	SocialUnit::BreakUnit(int nLauncherIdx, void *pParam, int nMsgSize)
{
	if (!m_isAllSubUnitLoad)
	{
		SocialSerializer::Singleton().LoadAllSubUnitReq(GetNetConnectIdx(nLauncherIdx), 
			GetTplId(),
			GetLayer(),
			GetUnitGuid()
			);
		
		return enSocialErr_RetryLater;
	}//endif

	if( !CheckPrivilege(nLauncherIdx, m_tplId, m_layer, enSUO_BreakUnit) )
		return enSocialErr_NoPrivilige;

	if (NULL != m_pParent && IsUnitOwner(nLauncherIdx,enSUTplId_Tong,GetLayer() + 1))
		return enSocialErr_OperationFailed;

	//Send Mail to sub player unit
/*	char szBreakMailConstant[MAX_MAIL_BREAK_CONSTANT_LEN]="";
	switch (m_layer)
	{
	case enSULayer_Gens:
		{
			snprintf(szBreakMailConstant,
				sizeof(szBreakMailConstant),
				MAIL_SOCIAL_BREAK_CONSTANT,
				MSG_SOCIAL_SHIZUZHANG,
				Player[nLauncherIdx].GetPlayerName(),
				GetUnitName(m_attrs)
				);
		}
		break;

	case enSULayer_Tong:
		{
			snprintf(szBreakMailConstant,
				sizeof(szBreakMailConstant),
				MAIL_SOCIAL_BREAK_CONSTANT,
				MSG_SOCIAL_ZHUHOUZHANG,
				Player[nLauncherIdx].GetPlayerName(),
				GetUnitName(m_attrs)
				);
		}
		break;

	}//end for switch

	szBreakMailConstant[MAX_MAIL_BREAK_CONSTANT_LEN - 1];
	SendMailToSubPlayer(MAIL_SOCIAL_TITLE_BREAK,szBreakMailConstant);
*/	
	BreakUnitPassive(nLauncherIdx);

	return enSocialErr_None;
}

void SocialUnit::BreakUnitPassive(int nPlayerIdx)
{
	if(m_pParent)
	{
		int	nSubUnitCnt = m_pParent->GetChildCount();
		
		if( GetMinSubUnitCnt(m_tplId, m_layer + 1) == nSubUnitCnt )
			m_pParent->BreakUnitPassive(nPlayerIdx);
	}

#ifdef _DEBUG
	CFS_FILELOGS::WriteDebugLog("Unit Break Begin: %s %d %d\n",
								GetUnitName(m_attrs),
								m_tplId,
								m_layer
								);
#endif

	SocialSerializer	&serializer = SocialSerializer::Singleton();
	int	nSubUnitCnt = (int)m_subUnits.size();

	for(int nSubUnit = 0; nSubUnit < nSubUnitCnt; ++nSubUnit)
	{
		if(m_subUnits[nSubUnit])
		{
			m_subUnits[nSubUnit]->OnLeaveParentUnit(this);

			int	nMemberIdx = g_PlayerInfoToIndex.GetIndexByName( m_subUnits[nSubUnit]->GetOwnerName() );
			if(IsValidPlayer(nMemberIdx))
				NotifyUnitBreak(nMemberIdx, m_tplId, m_layer);

			if(m_subUnits[nSubUnit]->GetLayer() > enSULayer_Player)
			{
				if( IsSocialTmplSave(m_tplId) )
				{
					serializer.UpdatePGuidReq(GetNetConnectIdx(nPlayerIdx), m_subUnits[nSubUnit]);
					UpdateLeafUnitPriv(GetNetConnectIdx(nPlayerIdx), m_subUnits[nSubUnit]);
				}
			}

			// 这个函数会将m_subUnits[nSubUnit]设为NULL, 所以要放在这块
			// 代码段的最后
			OnSubUnitLeave(m_subUnits[nSubUnit]);
		}
	}

	if(m_pParent)
	{
		m_pParent->OnSubUnitLeave(this);

		if( IsSocialTmplSave(m_tplId) )
			serializer.UpdateSubUnitCntReq(GetNetConnectIdx(nPlayerIdx), m_pParent);

		int	nParentIdx = g_PlayerInfoToIndex.GetIndexByName( m_pParent->GetOwnerName() );
		if(IsValidPlayer(nParentIdx))
			NotifySubUnitLeave(nParentIdx, this);
		OnLeaveParentUnit(m_pParent);
	}

	DWORD	dwRoomId = GetChatRoomId(m_attrs);

	if(INVALID_ROOM_ID != dwRoomId)
	{
		ChatRoomMgr_S	&roomMgr = g_ChatCenterS.GetRoomMgr();
		roomMgr.DeleteRoom(dwRoomId);
	}

	
	//清空城市的所有权
	if (m_attrs.IsAttrHasData(enSUAttr_CityMap))
	{
		int	   nCityMapId = GetCityMapId( GetUnitAttr() );
		FSGUID invalid;

		if(INVALID_WORLD_ID != nCityMapId)
		{
           int iSubwordIndex = g_SubWorldSet.SearchWorld(nCityMapId);
        
		   if (iSubwordIndex!= INVALID_WORLD_INDEX)
		   {
               int iLordIndex = SubWorld[iSubwordIndex].GetLord();
			   if (IsValidNpc(iLordIndex))
			   {
				   //能到这里一定是联盟，如果该联盟的city lord上有保护buff，就摘掉
				   BuffMgr& bm = BuffMgr::Singleton();
				   int nProtecteBuffId = ConfigManager::Singleton().GetGlobalVariable(global_var_tong_war_protect_buff);
				   if (bm.IsHaveBuff(iLordIndex, nProtecteBuffId))
					   bm.ClearBuffByTempID(iLordIndex, nProtecteBuffId);

				   Npc[iLordIndex].SetLord(invalid);
				   Npc[iLordIndex].Save();
			   }//endif

			   for (int iSubLordIndex = 0;iSubLordIndex<SUBLORD_COUNT ;iSubLordIndex++ )
			   {
				   int iSubLordNpcIndex = SubWorld[iSubwordIndex].GetSubLord(iSubLordIndex);
				   if (IsValidNpc(iSubLordNpcIndex))
				   {
					   Npc[iSubLordNpcIndex].SetLord(invalid);
					   Npc[iSubLordNpcIndex].Save();
				   }//endif
			   }//end for iSublordIndex	   
		   }//endif

		   
		   NotifyCityLeagueBreak(GetUnitName(m_attrs), nCityMapId);
		   
		   StatueInfoMgr::Singleton().ForceClearStatue(nCityMapId);

		}//endif
		
	}//endif

	//清空分星池所有权利
	if (m_attrs.IsAttrHasData(enSUAttr_PoolMap))
	{
		int	   nPoolMapID = GetPoolMapId( GetUnitAttr() );
		FSGUID invalid;
		
		if(INVALID_WORLD_ID != nPoolMapID)
		{
			int iSubwordIndex = g_SubWorldSet.SearchWorld(nPoolMapID);
			
			if (iSubwordIndex!= INVALID_WORLD_INDEX)
			{
				int iPoolIndex = SubWorld[iSubwordIndex].GetPool();
				if (IsValidNpc(iPoolIndex))
				{
					Npc[iPoolIndex].SetLord(invalid);
					Npc[iPoolIndex].Save();
				}//endif

				int iSubPoolIndex = SubWorld[iSubwordIndex].GetSubPool();
				if (IsValidNpc(iSubPoolIndex))
				{
				    Npc[iSubPoolIndex].SetLord(invalid);
					Npc[iSubPoolIndex].Save();
				}//endif
				
			}//endif
			
		}//endif
		
	}//endif
	
	if( IsSocialTmplSave(m_tplId) )
		serializer.RemoveRecordReq(GetNetConnectIdx(nPlayerIdx), this);
	
	//记录日志
	if (TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_social_unit_delete))
	{
		KPlayer& player = Player[nPlayerIdx];

		LogEventParam socialUnitCreateEvent;
		socialUnitCreateEvent.event = log_event_social_unit_delete;
		socialUnitCreateEvent.param1 = player.GetGUID();
		socialUnitCreateEvent.param2 = GetUnitGuid();
		
		if (g_pLogSystem)
			g_pLogSystem->Log(socialUnitCreateEvent);
	}

#ifdef _DEBUG
	CFS_FILELOGS::WriteDebugLog("Unit Break End\n",
								GetUnitName(m_attrs),
								m_tplId,
								m_layer
								);
#endif

	// delete 操作必须放在最后
	if( IsSocialTmplSave(m_tplId) )
		ServerSocialUnitMgr::Singleton().RemoveUnit(m_unitGuid, m_tplId);
}

void SocialUnit::OnLeaveParentUnit(SocialUnit *pParent)
{
	if (pParent == NULL)  //Notice here
		return ;

	SetParent(NULL);

	if(enSULayer_Player + 1 == pParent->GetLayer())
		SetRecParentGuid(m_ownerName, m_tplId, NULL);

	int nLeaveBuff = GetLeaveBuff(m_tplId, pParent->GetLayer());
	bool bAdded    = AddBuffToUnit(this, nLeaveBuff);

	SocialUnit	*pCurUnit = pParent;

	while(pCurUnit)
	{
		ClearLeafUnitPrivilege( pCurUnit->GetLayer() );
		ChatChannelOpe(pCurUnit, false);
		pCurUnit = pCurUnit->GetParent();
	}

	ChgLeafUnitMaxJoinedLayer(pParent->GetLayer() - 1);
	
	SyncLeafUnitPrivilege();
	
	if (!bAdded)
		CheckDelayAddLeaveBuff(GetOwnerName(),pParent->GetLayer());
}

void SocialUnit::CheckDelayAddLeaveBuff( const char * szOwnerName,const int nLeaveLayer )
{
	if (!szOwnerName)
		return ;

	if ( IsLeaf() )
	{
		if ( IsOwner(szOwnerName) )
		{

			int nPlayerIdx = g_PlayerInfoToIndex.GetIndexByName( szOwnerName );

			if (nPlayerIdx == INVALID_PLAYER_INDEX)
			{
				m_maxJoinedLayer = nLeaveLayer;
			}//endif

		}//endif

	}//endif
	else
	{
		int nMaxCount = (int)m_subUnits.size();
		
		for(int i = 0; i < nMaxCount; ++i)
		{
			if(m_subUnits[i])
				m_subUnits[i]->CheckDelayAddLeaveBuff( szOwnerName,nLeaveLayer );
		}	

	}//end else
}

void SocialUnit::OnJoinParentUnit(SocialUnit *pParent)
{
	SetParent(pParent);

	if(enSULayer_Player + 1 == pParent->GetLayer())
		SetRecParentGuid( m_ownerName, m_tplId, &(pParent->GetUnitGuid()) );

	int nJoinBuff = GetJoinBuff(m_tplId, pParent->GetLayer());
	AddBuffToUnit(this, nJoinBuff);

	int	nMaxLayer = m_layer;
	SocialUnit	*pCurUnit = pParent;

	while(pCurUnit)
	{
		AddLeafUnitPrivilege( pCurUnit->GetLayer() );
		ChatChannelOpe(pCurUnit, true);
		nMaxLayer = pCurUnit->GetLayer();
		pCurUnit = pCurUnit->GetParent();
	}

	ChgLeafUnitMaxJoinedLayer(nMaxLayer);

	SyncLeafUnitPrivilege();
}

void SocialUnit::ChgLeafUnitMaxJoinedLayer(int nMaxLayer)
{
	_ASSERT( IsLayerValid(m_tplId, nMaxLayer) );

	if( IsLeaf() )	
	{
		m_maxJoinedLayer = (BYTE)nMaxLayer;
		return;
	}

	SocialUnit		*pSubUnit;
	UnitIterator	iter;

	while( pSubUnit = NextSubUnit(iter) )
		pSubUnit->ChgLeafUnitMaxJoinedLayer(nMaxLayer);
}


int	SocialUnit::LeaveUnit(int nLauncherIdx, void *pParam, int nMsgSize)
{
	if (!m_isAllSubUnitLoad)
	{
		SocialSerializer::Singleton().LoadAllSubUnitReq(GetNetConnectIdx(nLauncherIdx), 
				GetTplId(),
				GetLayer(),
				GetUnitGuid()
				);
			
		return enSocialErr_RetryLater;
	}//endif

	if( !CheckPrivilege(nLauncherIdx, m_tplId, m_layer, enSUO_LeaveUnit) )
		return enSocialErr_NoPrivilige;

	SocialUnit *pLeafUnit = GetLeafUnit(nLauncherIdx, m_tplId);

	_ASSERT(pLeafUnit);
	if(NULL == pLeafUnit)
		return enSocialErr_LeaveFailed;

	SocialUnit	*pSubUnit = GetUpNUnit(pLeafUnit, m_layer - 1);

	_ASSERT(pSubUnit);
	if(NULL == pSubUnit)
		return enSocialErr_LeaveFailed;

	// 一个节点的主人禁止离开该节点
	if( IsOwner( GetPlayerName(nLauncherIdx) ) )
		return enSocialErr_LeaveFailed;

	// 只有氏族长才能脱离诸侯，其他类似
	if( !pSubUnit->IsOwner( GetPlayerName(nLauncherIdx) ) )
		return enSocialErr_LeaveFailed;

	if (pSubUnit->GetParent() != this)
		return enSocialErr_LeaveFailed;

	if( GetMinSubUnitCnt(m_tplId, m_layer) == GetChildCount() )
		BreakUnitPassive(nLauncherIdx);
	else
	{
		pSubUnit->OnLeaveParentUnit(this);
		OnSubUnitLeave(pSubUnit);

		if( IsSocialTmplSave(m_tplId) )
			UpdateDBOnUnitLeave(GetNetConnectIdx(nLauncherIdx), this, pSubUnit);

		NotifyLeaveParentUnit(nLauncherIdx, this);

		int	nTargetIdx = g_PlayerInfoToIndex.GetIndexByName(m_ownerName);
		if(IsValidPlayer(nTargetIdx))
			NotifySubUnitLeave(nTargetIdx, pSubUnit);
	}

	return enSocialErr_None;
}

int	SocialUnit::RemoveSubUnit(int nLauncherIdx, void *pParam, int nMsgSize)
{
	//子树都Load后才可以,避免数据不一致
	if (!m_isAllSubUnitLoad)
	{
		SocialSerializer::Singleton().LoadAllSubUnitReq(GetNetConnectIdx(nLauncherIdx), 
			GetTplId(),
			GetLayer(),
			GetUnitGuid()
		);

		return enSocialErr_RetryLater;
	}//endif

	if( !CheckPrivilege(nLauncherIdx, m_tplId, m_layer, enSUO_RemoveSubUnit) )
		return enSocialErr_NoPrivilige;

	C2S_REMOVESUBUNIT_REQ	*pc2sReq = (C2S_REMOVESUBUNIT_REQ*)pParam;
	ServerSocialUnitMgr		&mgr = ServerSocialUnitMgr::Singleton();

	pc2sReq->unitGuid.data[32] = 0;

	SocialUnit				*pTargetUnit = mgr.GetUnit(pc2sReq->unitGuid, m_tplId);

	if(NULL == pTargetUnit)
		return enSocialErr_UnitNotFound;

	if( pTargetUnit->GetParent() != this )
		return enSocialErr_RemoveFailed;

	_ASSERT(pTargetUnit->GetLayer() + 1 == m_layer);
	if(pTargetUnit->GetLayer() + 1 != m_layer)
		return enSocialErr_RemoveFailed;

	if (strcmp(pTargetUnit->GetOwnerName(),m_ownerName)==0)  //不能开除自己的为Owner的子节点
		return enSocialErr_RemoveFailed;

	if( GetMinSubUnitCnt(m_tplId, m_layer) == GetChildCount() )
		BreakUnitPassive(nLauncherIdx);
	else
	{
		pTargetUnit->OnLeaveParentUnit(this);
		OnSubUnitLeave(pTargetUnit);

		if( IsSocialTmplSave(m_tplId) )
			UpdateDBOnUnitLeave(GetNetConnectIdx(nLauncherIdx), this, pTargetUnit);

		int	nTargetIdx = g_PlayerInfoToIndex.GetIndexByName( pTargetUnit->GetOwnerName() );
		if(INVALID_PLAYER_INDEX != nTargetIdx)
			NotifyRemovedFromParent(nTargetIdx, this);

		NotifySubUnitLeave(nLauncherIdx, pTargetUnit);
	}
	
	return enSocialErr_None;
}

int SocialUnit::AddRecruitInfo(int nLuancherIdx,void * pParam,int nMsgSize)
{
	if( !CheckPrivilege(nLuancherIdx, m_tplId, m_layer, enSUO_AddRecruitInfo) )
		return enSocialErr_NoPrivilige;

	if( GetChildCount() >= GetMaxSubUnitCnt(m_tplId, m_layer) )
		return enSocialErr_MemberNumIsFull;

	if ( GetLayer() > enSULayer_Tong )
		return enSocialErr_NoPrivilige;

	KSocialRecruitMgr & mgr = KSocialRecruitMgr::Singlton();


	int nRet = mgr.AddInfo(GetUnitGuid(),nLuancherIdx);

	if (enSocialErr_RecruitOpSuc == nRet)
		DeductComOpeCost(nLuancherIdx,enSUTplId_Tong,GetLayer(),enSUO_AddRecruitInfo);
	
	return nRet;
}

int SocialUnit::DelRecruitInfo(int nLuancherIdx,void * pParam,int nMsgSize)
{
	if( !CheckPrivilege(nLuancherIdx, m_tplId, m_layer, enSUO_DelRecruitInfo) )
		return enSocialErr_NoPrivilige;

	KSocialRecruitMgr & mgr = KSocialRecruitMgr::Singlton();
	return mgr.DelInfo(GetUnitGuid(),nLuancherIdx);

}

int SocialUnit::ReqRecruitInfo(int nLuancherIdx, void *pParam, int nMsgSize)
{
	C2S_RECRUIT_INFO_REQ *    pc2sReq  = (C2S_RECRUIT_INFO_REQ *)pParam;

	if (pc2sReq->reqLayer < enSULayer_Gens || pc2sReq->reqLayer> enSULayer_Tong || pc2sReq->curPageNo<0)
	{
		_ASSERT(false);
		return enSocialErr_None;
	} //endif

	static const int          BUF_SIZE =  8192;
	char				 buf[BUF_SIZE] = { 0 };

	if (BUF_SIZE <= sizeof(S2C_GETRECRUIT_RET) - 1 + (MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE + 1) * sizeof(S2C_SOCIAL_RECRUIT_INFO))
	{
		_ASSERT(BUF_SIZE > sizeof(S2C_GETRECRUIT_RET) - 1 + (MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE + 1) * sizeof(S2C_SOCIAL_RECRUIT_INFO));
		return enSocialErr_None;
	}//endif

	S2C_GETRECRUIT_RET	*ps2cRet = (S2C_GETRECRUIT_RET*)buf;

	ps2cRet->comHeader.proHeader.protocol    = s2c_social_family;
	ps2cRet->comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	ps2cRet->comHeader.comParam = pc2sReq->comHeader.comParam;

	int nRet = KSocialRecruitMgr::Singlton().GetInfoList(pc2sReq->curPageNo,MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE,pc2sReq->reqLayer,BUF_SIZE,ps2cRet);
    _ASSERT(nRet == enSocialErr_None || nRet == enSocialErr_NoInfo);

	if (nRet == enSocialErr_None)
	{	
	    SendDataToClient(nLuancherIdx, buf, ps2cRet->comHeader.proHeader.len + PROTOCOL_SIZE);
	}//endif

	return enSocialErr_None;
	
}

int SocialUnit::ChangeUnitOwner(int nLuancherIdx, void *pParam, int nMsgSize)
{
	//1.Self condition check
	if (!m_isAllSubUnitLoad)
		return enSocialErr_RetryLater;

	if( !CheckPrivilege(nLuancherIdx, m_tplId, m_layer, enSUO_ChangeOwner) )
		return enSocialErr_NoPrivilige;

	SocialUnit                  * pSelfUnit   = GetLeafUnit(nLuancherIdx,m_tplId);
	
	if (!pSelfUnit)
		return enSocialErr_ChangeOwnerFaild;
	
	if (m_layer == enSULayer_Gens  || m_layer == enSULayer_Tong)
	{
		int nBuffMustNotHaveOnJoin = GetBuffMustNotHaveOnJoin(m_tplId, m_layer + 1);
		if( BuffMgr::Singleton().IsHaveBuff(nLuancherIdx, nBuffMustNotHaveOnJoin) )
			return enSocialErr_HaveLeaveBuff;
	}//endif

	SocialUnit                  * pParentUnit = m_pParent;
	while (pParentUnit)
	{
		if (strcmp(pParentUnit->GetOwnerName(),pSelfUnit->GetOwnerName())==0)
			return enSocialErr_ChangeOwnerSubOrderNeeded;   //要按照从上到下的顺序来执行禅让

		pParentUnit   = pParentUnit->GetParent();
	}

	SocialUnit                 * pSelfTopUnit = GetTopUnit(pSelfUnit);
	_ASSERT(pSelfTopUnit);

	//Self invalid condition checked
	for (int nTestLayer = enSULayer_Gens ; nTestLayer <= pSelfTopUnit->GetLayer() ; nTestLayer ++)
	{
		if( !CheckPrivilege(nLuancherIdx, m_tplId, nTestLayer, enSUO_ChatInUnit) )
			return enSocialErr_NoChatOpeWhenChangeOwnerSelf;
	}//end for ntestLayer
	
	//2.Target condition check
	C2S_CHANGE_UNIT_OWNER_REQ	*pc2sReq = (C2S_CHANGE_UNIT_OWNER_REQ*)pParam;
	pc2sReq->unitGuid.data[32]           = 0;

	ServerSocialUnitMgr	    	&mgr = ServerSocialUnitMgr::Singleton();
	SocialUnit				    *pTargetPlayerUnit = mgr.GetUnit(pc2sReq->unitGuid, m_tplId);

	if (NULL == pTargetPlayerUnit)
		return enSocialErr_ChangeOwnerFaild;

	if (pTargetPlayerUnit->GetLayer() == enSULayer_Tong)
	{
		int nPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(pTargetPlayerUnit->GetOwnerName());
		if (!IsValidPlayer(nPlayerIndex))
			return enSocialErr_ChangeOwnerSubOrderOnlineNeeded;

		pTargetPlayerUnit = mgr.GetUnit(Player[nPlayerIndex].GetGUID(), m_tplId);
	}

	if(NULL == pTargetPlayerUnit || pTargetPlayerUnit->GetLayer()!=enSULayer_Player)
		return enSocialErr_UnitNotFound;

	//Online check...
    int nTargetPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(pTargetPlayerUnit->GetOwnerName());
	if (!IsValidPlayer(nTargetPlayerIndex))
		return enSocialErr_ChangeOwnerSubOrderOnlineNeeded;
	
	if (nTargetPlayerIndex == nLuancherIdx)
		return enSocialErr_ChangeOwnerFaild;

	RelationLayer *pCurrLayerRL = GetRelationLayer(m_tplId, m_layer);
	if( pCurrLayerRL == NULL || GetPlayerLevel(nTargetPlayerIndex) < pCurrLayerRL->Operations[enSUO_ChangeOwner].ReqTargetLevel )
		return enSocialErr_LevelInValid;	
	
	if (IsHasRefuseBuff(nTargetPlayerIndex, nLuancherIdx))
		return enSocialErr_ChangeOwnerFaild;

	SocialUnit *pTargetTopUnit = GetUpNUnit(pTargetPlayerUnit,m_layer-1);
	if (NULL == pTargetTopUnit)
		return enSocialErr_ChangeOwnerFaild;
	
	if( pTargetTopUnit->GetParent() != this )
		return enSocialErr_ChangeOwnerSubUnitNeeded;
	
	_ASSERT(pTargetTopUnit->GetLayer() + 1 == m_layer);

	if(pTargetTopUnit->GetLayer() + 1 != m_layer)
		return enSocialErr_ChangeOwnerFaild;

	if (strcmp(pTargetTopUnit->GetOwnerName(),pTargetPlayerUnit->GetOwnerName())!=0)
		return enSocialErr_ChangeOwnerSubUnitOwnerNeeded;

	//Invalide Target player state check
	//Self invalid condition checked
	SocialUnit                 * pTarTopUnit = GetTopUnit(pTargetPlayerUnit);
	_ASSERT(pTarTopUnit);

	for (int nTargetLayer = enSULayer_Gens ; nTargetLayer <= pTarTopUnit->GetLayer() ; nTargetLayer ++)
	{
		if( !CheckPrivilege(nTargetPlayerIndex, m_tplId, nTargetLayer, enSUO_ChatInUnit) )
			return enSocialErr_NoChatOpeWhenChangeOwner;

	}//end for ntestLayer

	//Additional test for city and statue  because statue should be destroy
	if(!GetGlobalTongWarMgr().IsInitedAll())
		return enSocialErr_RetryLater;


	//Operation Process
	//1. change owner name
	strncpy(m_ownerName,pTargetPlayerUnit->GetOwnerName(),MAXSIZE_ROLENAME);
	m_ownerName[MAXSIZE_ROLENAME -1]=0;

	//2.old owner prilage change
	SocialUnit * pCurUnit = this;
	while (pCurUnit)
	{
		pSelfUnit->ClearLeafUnitPrivilege(pCurUnit->GetLayer());
		pSelfUnit->AddLeafUnitPrivilege(pCurUnit->GetLayer());
		pTargetPlayerUnit->ClearLeafUnitPrivilege(pCurUnit->GetLayer());
		pTargetPlayerUnit->AddLeafUnitPrivilege(pCurUnit->GetLayer());

		pCurUnit = pCurUnit->GetParent();
	}//end for while

	//3.DBSave
	if( IsSocialTmplSave(m_tplId) )
	{
		int nNetId = GetNetConnectIdx(nLuancherIdx);

		SocialSerializer &ss = SocialSerializer::Singleton();
		ss.UpdateRecordReq(nNetId, this);

		_ASSERT(pSelfUnit->GetParent() 
			 && pSelfUnit->GetParent()->GetLayer()==enSULayer_Gens 
			 && pSelfUnit->GetParent()->IsAllSubUnitLoad());

        ss.UpdateAppDataReq(nNetId,pSelfUnit->GetParent());
		ss.UpdateAppDataReq(nNetId,pTargetPlayerUnit->GetParent());

	}//endif

	//4.SyncLeaf
	Player[nLuancherIdx].SyncSocialRelation(m_tplId);
	Player[nTargetPlayerIndex].SyncSocialRelation(m_tplId);

	//4.5 Statue
	SocialUnitAttr & attr = GetUnitAttr();
	int nMapID            = GetCityMapId(attr);
	if (nMapID!=INVALID_WORLD_ID)
	{
		StatueInfoMgr::Singleton().ForceClearStatue(nMapID);
	}//endif

	//5.Msg to Client
	char	szMsg[MAXSIZE_HINT_MSG];
	
	int		nSize = FormatChangeOwnerMsg(szMsg,
		    sizeof(szMsg),
		    pSelfUnit->GetOwnerName(),
			pTargetPlayerUnit->GetOwnerName(),
			this,
		    MSG_SOCIAL_CHANGE_UNIT_OWNER
		    );

	SocialMsgToClient(nLuancherIdx, szMsg, nSize);
	SocialMsgToClient(nTargetPlayerIndex,szMsg,nSize);
	
	return enSocialErr_None;
}

int	SocialUnit::AddSubUnit(int nLauncherIdx, void *pParam, int nMsgSize)
{
	if( !CheckPrivilege(nLauncherIdx, m_tplId, m_layer, enSUO_AddSubUnit) )
		return enSocialErr_NoPrivilige;

	//Check force sub unit num
	SocialUnitAttr & selfAttr = GetUnitAttr();
	if (selfAttr.IsAttrExist(enSUAttr_ForceSubUnitMaxNum))
	{
		char*    pForceSubUnitNum = 0;
		int      nSize = 0;

		nSize = selfAttr.GetAttr(enSUAttr_ForceSubUnitMaxNum, pForceSubUnitNum);
		
		if(nSize == sizeof(int))
		{
			int		nForceSub = *( (int*)pForceSubUnitNum );
			if (GetChildCount() >= nForceSub )
				return enSocialErr_MemberNumIsFull;
		}//endif

	}//endif

	if( GetChildCount() >= GetMaxSubUnitCnt(m_tplId, m_layer) )
		return enSocialErr_MemberNumIsFull;

	C2S_ADDSUBUNIT_REQ	*pc2sReq = (C2S_ADDSUBUNIT_REQ*)pParam;

	// 检测目标能否被加入
	pc2sReq->receiverName[sizeof(pc2sReq->receiverName) - 1] = '\0';
	int	nTargetIdx = g_PlayerInfoToIndex.GetIndexByName(pc2sReq->receiverName);

	if(!IsValidPlayer(nTargetIdx))
		return enSocialErr_PlayerNotOnline;

	// 有可能玩家已经加入了社会关系，但当其上线时，Load关系数据失败
	// 在重新load完数据之前，应该不允许玩家加入任何团体
	RelationSet	&targetRelSet = GetRelationSet(nTargetIdx);
	if( !targetRelSet.IsOwnTreeLoad(m_tplId) )
		return enSocialErr_AddChildFailed;

	RelationLayer *pLayer = GetRelationLayer(m_tplId, m_layer);
	if(NULL == pLayer)
	{
		_ASSERT(false);
		return enSocialErr_AddChildFailed;
	}
	else
	{
		if( GetPlayerLevel(nTargetIdx) < pLayer->Operations[enSUO_AddSubUnit].ReqTargetLevel )
			return enSocialErr_LevelInValid;
	}

	SocialUnit *pLeafUnit = GetLeafUnit(nTargetIdx, m_tplId);
	SocialUnit *pTopUnit = GetTopUnit(pLeafUnit);

	_ASSERT(pTopUnit);
	if(NULL == pTopUnit)
		return enSocialErr_AddChildFailed;

	if(pTopUnit->GetLayer() + 1 != m_layer)
		return enSocialErr_AddChildFailed;

	if (!pTopUnit->IsOwner(Player[nTargetIdx].GetPlayerName()))
		return enSocialErr_AddChildFailed;
	
	int nBuffMustNotHaveOnJoin = GetBuffMustNotHaveOnJoin(m_tplId, m_layer);
	if(INVALID_BUFF_ID != nBuffMustNotHaveOnJoin)
	{
		if( BuffMgr::Singleton().IsHaveBuff(Player[nTargetIdx].m_nIndex, nBuffMustNotHaveOnJoin) )
			return enSocialErr_HaveLeaveBuff;
	}

	// 如果目标可以加入，先发送请求给目标，邀请其加入		
	// 目标确认后，将其加入
	if(enSUReqConfirm_None == pc2sReq->nh.ch.comParam.confirmCode)
	{
		_ASSERT(nMsgSize <= MAXSIZE_UNCONFIRMREQ);

		if(nMsgSize > MAXSIZE_UNCONFIRMREQ)
			return enSocialErr_AddChildFailed;

		char	buf[MAXSIZE_UNCONFIRMREQ + MAXSIZE_HINT_MSG + sizeof(S2C_REQ_COMFIRM) - 1];

		S2C_REQ_COMFIRM	*ps2cRet = (S2C_REQ_COMFIRM*)buf;
		ps2cRet->comHeader.proHeader.protocol = s2c_social_family;
		ps2cRet->comHeader.proHeader.subProtocol = enSRProtocol_ReqToConfirm;
		ps2cRet->comHeader.comParam = pc2sReq->nh.ch.comParam;
		ps2cRet->reqSize = nMsgSize;
		memcpy(ps2cRet->data, pParam, nMsgSize);

		ps2cRet->hitMsgLen = FormatAddRemoveUnitMsg(ps2cRet->data + nMsgSize,
						 						    MAXSIZE_HINT_MSG,
													this,
													MSG_SOCIAL_ADD_SUBUNIT
												   );

		ps2cRet->comHeader.proHeader.len = sizeof(S2C_REQ_COMFIRM) - 1 + 
				ps2cRet->hitMsgLen + ps2cRet->reqSize - PROTOCOL_SIZE;

		SendDataToClient(nTargetIdx, buf, ps2cRet->comHeader.proHeader.len + PROTOCOL_SIZE);

		return enSocialErr_WaitToConfirm;
	}
	else if(enSUReqConfirm_Ok == pc2sReq->nh.ch.comParam.confirmCode)
	{
		pTopUnit->OnJoinParentUnit(this);
		OnSubUnitJoin(pTopUnit);

		if( IsSocialTmplSave(m_tplId) )
			UpdateDBOnUnitJoin(GetNetConnectIdx(nLauncherIdx), this, pTopUnit);

		NotifySubUnitJoined(nLauncherIdx, pTopUnit);
		NotifyJoinParentUnit(nTargetIdx, this);

		ConfigManager & cfgmgr = ConfigManager::Singleton();
		int  nSucBuff          = cfgmgr.GetGlobalVariable(global_var_social_recruit_suc_buff);

		//AddBuff when joined sucessful
		if (IsValidPlayer(nTargetIdx) && nSucBuff)
		{
			int nTargetNpcIdx = Player[nTargetIdx].GetNpcIndex();
			if (IsValidNpc(nTargetNpcIdx))
			{
				BuffMgr & buffmgr = BuffMgr::Singleton();

				buffmgr.AddNpcBuff(nTargetNpcIdx,nTargetNpcIdx,nSucBuff);
			}//endif

		}//endif

	}
	else if (enSUReqConfirm_Ignore == pc2sReq->nh.ch.comParam.confirmCode)
	{
        return enSocialErr_RequestIgnored;
	}
	else if (enSUReqConfirm_Refuse == pc2sReq->nh.ch.comParam.confirmCode)
	{
        return enSocialErr_RequestRefused;
	}

	return enSocialErr_None;
}

int SocialUnit::ForbidChatInUnit(int nLauncherIdx, void *pParam, int nMsgSize)
{
	if( !CheckPrivilege(nLauncherIdx, m_tplId, m_layer, enSUO_ForbidChat) )
		return enSocialErr_NoPrivilige;
	
	C2S_FORBIDCHAT_REQ	*pc2sReq = (C2S_FORBIDCHAT_REQ*)pParam;

	pc2sReq->unitGuid.data[32] = 0;

	SocialUnit	*pTargetUnit = ServerSocialUnitMgr::Singleton().GetUnit(pc2sReq->unitGuid, m_tplId);
	SocialUnit  *pSelfUnit   = GetLeafUnit(nLauncherIdx,enSUTplId_Tong);
	if(NULL == pTargetUnit || pSelfUnit == pTargetUnit)
		return enSocialErr_ForbidChatFailed;

	// 关闭禁止一个团体聊天权限的功能，需要的话注掉下面两行即可
	if(pTargetUnit->GetLayer() != enSULayer_Player)
		return enSocialErr_ForbidChatFailed;

	pTargetUnit->ChatPrivilegeOpe(this, true);

	if( IsSocialTmplSave(m_tplId) )
	{
		if(pTargetUnit->GetLayer() == enSULayer_Player)
			UpdateLeafUnitPriv(GetNetConnectIdx(nLauncherIdx), pTargetUnit->GetParent());
		else
			UpdateLeafUnitPriv(GetNetConnectIdx(nLauncherIdx), pTargetUnit);
	}

	int	nTargetIdx = g_PlayerInfoToIndex.GetIndexByName( pTargetUnit->GetOwnerName() );
	if(INVALID_PLAYER_INDEX != nTargetIdx)
		NotifyForbidChatMsg(nTargetIdx, m_ownerName, this);

	return enSocialErr_None;
}

int	SocialUnit::UnForbidChatInUnit(int nLauncherIdx, void *pParam, int nMsgSize)
{
	if( !CheckPrivilege(nLauncherIdx, m_tplId, m_layer, enSUO_UnForbidChat) )	
		return enSocialErr_NoPrivilige;

	C2S_UNFORBIDCHAT_REQ	*pc2sReq = (C2S_UNFORBIDCHAT_REQ*)pParam;
	
	pc2sReq->unitGuid.data[32] = 0;

	SocialUnit	*pTargetUnit = ServerSocialUnitMgr::Singleton().GetUnit(pc2sReq->unitGuid, m_tplId);

	if(NULL == pTargetUnit)
		return enSocialErr_UnForbidChatFailed;

	// 关闭解禁一个团体聊天权限的功能，需要的话注掉下面两行即可
	if(pTargetUnit->GetLayer() != enSULayer_Player)
		return enSocialErr_ForbidChatFailed;

	pTargetUnit->ChatPrivilegeOpe(this, false);

	if( IsSocialTmplSave(m_tplId) )
	{
		if(pTargetUnit->GetLayer() == enSULayer_Player)
			UpdateLeafUnitPriv(GetNetConnectIdx(nLauncherIdx), pTargetUnit->GetParent());
		else
			UpdateLeafUnitPriv(GetNetConnectIdx(nLauncherIdx), pTargetUnit);
	}

	int	nTargetIdx = g_PlayerInfoToIndex.GetIndexByName( pTargetUnit->GetOwnerName() );
	if(IsValidPlayer(nTargetIdx))
		NotifyUnForbidChatMsg(nTargetIdx, m_ownerName, this);

	return enSocialErr_None;
}

void SocialUnit::ChatPrivilegeOpe(SocialUnit *pChannelUnit, bool bForbid)
{
	if (pChannelUnit == NULL )
		return ;

	if( !IsLeaf() )
	{
		int	nMaxCount = (int)m_subUnits.size();

		for(int nSubUnit = 0; nSubUnit < nMaxCount; ++nSubUnit)
		{
			if(m_subUnits[nSubUnit])
				m_subUnits[nSubUnit]->ChatPrivilegeOpe(pChannelUnit, bForbid);
		}	

		return;
	}

	if( pChannelUnit->IsOwner(m_ownerName) )
		return;

	if(bForbid)
		m_privSet.Remove(m_tplId, pChannelUnit->GetLayer(), enSUO_ChatInUnit);
	else
		m_privSet.Add(m_tplId, pChannelUnit->GetLayer(), enSUO_ChatInUnit);

	SocialUnitAttr &chanUnitAttr = pChannelUnit->GetUnitAttr();
	DWORD	dwRoomId = GetChatRoomId(chanUnitAttr);
	if(INVALID_ROOM_ID == dwRoomId)
		return;

	int	nPlayerIdx = g_PlayerInfoToIndex.GetIndexByName(m_ownerName);

	if(!IsValidPlayer(nPlayerIdx))
		return;

	ChatRoomMgr_S	&roomMgr = g_ChatCenterS.GetRoomMgr();
	ChatObjectMgr_S	*pObjMgr = g_ChatCenterS.GetChatObjMgr(nPlayerIdx);

	if(pObjMgr)
		pObjMgr->ChgRoomChatPrivilege(dwRoomId, !bForbid);
}

int	SocialUnit::PubAnnouncement(int nLauncherIdx, void *pParam, int nMsgSize)
{
	if( !CheckPrivilege(nLauncherIdx, m_tplId, m_layer, enSUO_PubAnnouncement) )
		return enSocialErr_NoPrivilige;

	C2S_PUBANNOUNCEMENT_REQ	*pc2sReq = (C2S_PUBANNOUNCEMENT_REQ*)pParam;

	if(pc2sReq->msgSize > MAXSIZE_ANNOUNCEMENT)
		return enSocialErr_AnnounceLenExceed;

	if(sizeof(C2S_PUBANNOUNCEMENT_REQ) + pc2sReq->msgSize - 1 > nMsgSize)
		return enSocialErr_AnnounceLenExceed;

	_ASSERT( m_attrs.IsAttrExist(enSUAttr_Announcement) );
	if( !m_attrs.IsAttrExist(enSUAttr_Announcement) )
		return enSocialErr_PubAnnounceFailed;

	if( m_attrs.IsAttrHasData(enSUAttr_Announcement) )
	{
		m_attrs.ChangeAttr(enSUAttr_Announcement, pc2sReq->msg, pc2sReq->msgSize);
		m_AnuncmentVersion ++ ;
	}//endif
	else
		m_attrs.AddAttr(enSUAttr_Announcement, pc2sReq->msg, pc2sReq->msgSize);

	if( IsSocialTmplSave(m_tplId) )
	{
		if( m_attrs.IsAttrSaveToDb(enSUAttr_Announcement) && m_layer > enSULayer_Player )
			SocialSerializer::Singleton().UpdateAttrReq(GetNetConnectIdx(nLauncherIdx), this);
	}

	return enSocialErr_None;
}

int SocialUnit::GetSubList(int nLauncherIdx, void *pParam, int nMsgSize)
{
	// 应该用叶节点检测权限
	//这里对获取信息列表做了特例，因为这种操作不是IsDefult(只有最高层的Owner 才有的操作)
	//而是所有成员可以拥有的.
	if( !CheckPrivilege(nLauncherIdx, m_tplId, m_layer, enSUO_GetSubList) )	
		return enSocialErr_NoPrivilige;
   
	C2S_GETSUBLIST_REQ	*pc2sReq = (C2S_GETSUBLIST_REQ*)pParam;
	SocialUnit			*pTargetUnit = NULL;

	if(enSUGetSubList_Type_Belong == pc2sReq->listType)
	{
		_ASSERT(sizeof(C2S_GETSUBLIST_REQ) - 1 == nMsgSize);
		if(nMsgSize != sizeof(C2S_GETSUBLIST_REQ) - 1)
			return enSocialErr_GetSubListFailed;
	
		pTargetUnit = this;
	}
	else if(enSUGetSubList_Type_Other == pc2sReq->listType)
	{
		_ASSERT(sizeof(C2S_GETSUBLIST_REQ) + sizeof(FSGUID) - 1 == nMsgSize);
		if(sizeof(C2S_GETSUBLIST_REQ) + sizeof(FSGUID) - 1 != nMsgSize)
			return enSocialErr_GetSubListFailed;

		ServerSocialUnitMgr	&mgr = ServerSocialUnitMgr::Singleton();

		FSGUID	*pGuid = (FSGUID*)pc2sReq->data;
		pGuid->data[32]=  0;

		pTargetUnit = mgr.GetUnit(*pGuid, m_tplId);

		_ASSERT(pTargetUnit);
		if(NULL == pTargetUnit)
			return enSocialErr_GetSubListFailed;

		_ASSERT( GetTopUnit(this) == GetTopUnit(pTargetUnit) );
		if( GetTopUnit(this) != GetTopUnit(pTargetUnit) )
			return enSocialErr_GetSubListFailed;

		// 不做这个限制，客户端只知道节点guid，不知道layer
// 		_ASSERT( pTargetUnit->GetLayer() == m_layer );
// 		if( pTargetUnit->GetLayer() != m_layer )
// 			return enSocialErr_GetSubListFailed;
	}
	else
		return enSocialErr_GetSubListFailed;

	int	nStartOffset = pc2sReq->curPageNo * MAXCOUNT_SUBLIST_ONETIMEGET;
	if( nStartOffset >= pTargetUnit->GetChildCount() )
		return enSocialErr_AlreadyLastPage;

	if( !pTargetUnit->IsAllSubUnitLoad() )
	{
		SocialSerializer::Singleton().LoadAllSubUnitReq(GetNetConnectIdx(nLauncherIdx), 
														pTargetUnit->GetTplId(),
														pTargetUnit->GetLayer(),
														pTargetUnit->GetUnitGuid()
														);
		return enSocialErr_RetryLater;
	}

	const	int BUF_SIZE = 8192;

	_ASSERT( sizeof(S2C_GETSUBLIST_RET) - 1 <= BUF_SIZE );
	if( sizeof(S2C_GETSUBLIST_RET) - 1 > BUF_SIZE )
		return enSocialErr_GetSubListFailed;

	char				buf[BUF_SIZE] = { 0 };
	S2C_GETSUBLIST_RET	*ps2cRet = (S2C_GETSUBLIST_RET*)buf;
	ps2cRet->comHeader.proHeader.protocol = s2c_social_family;
	ps2cRet->comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	ps2cRet->comHeader.comParam = pc2sReq->comHeader.comParam;
	ps2cRet->listType = pc2sReq->listType;
	ps2cRet->listLayer = pTargetUnit->GetLayer() - 1;
	ps2cRet->unitCount = 0;
	ps2cRet->maxPlayerCount = 0;
	ps2cRet->onlinePlayerCount = 0;
	ps2cRet->curPageNo = pc2sReq->curPageNo;
	
	if ( GetLayer() == enSULayer_Tong && GetParent())
	{
		SocialUnit     * pLeague         = GetParent();
		SocialUnitAttr & leagueAttr      = pLeague->GetUnitAttr();
		ps2cRet->cityWorldId             = GetCityMapId( leagueAttr );
	}//endif
	else
		ps2cRet->cityWorldId = GetCityMapId( GetUnitAttr() );	

	ps2cRet->poolWorldId = GetPoolMapId(GetUnitAttr());

	if(pTargetUnit == this)
	{
		int nMaxPlayerCount = 0;
		int nOnlinePlayerCount = 0;
		GetUnitPlayerCount(nMaxPlayerCount, nOnlinePlayerCount);
		ps2cRet->maxPlayerCount = (WORD)nMaxPlayerCount;
		ps2cRet->onlinePlayerCount = (WORD)nOnlinePlayerCount;
	}

	int		nLeftSize                 = BUF_SIZE - (sizeof(S2C_GETSUBLIST_RET) - 1);
	int     nCompressionLeftSizeStart = nLeftSize; 
	char	*pUnitInfo = ps2cRet->data;

	if(enSUGetSubList_Type_Belong == pc2sReq->listType)
	{
		int nUsedSize = pTargetUnit->GetUnitTransferInfo(pUnitInfo, nLeftSize);

		if(nUsedSize > 0)
		{
			nLeftSize -= nUsedSize;
			++ps2cRet->unitCount;
			pUnitInfo += nUsedSize;
		}
	}

	if(nStartOffset < 0)
		nStartOffset = 0;

	ps2cRet->unitCount += pTargetUnit->GetSubListInfo(nStartOffset, pUnitInfo, nLeftSize);
	ps2cRet->comHeader.proHeader.len = BUF_SIZE - nLeftSize;
	
	int    nComressionLeftSizeEnd    = nLeftSize;
	int    nSizeBeforeCompression    = nCompressionLeftSizeStart - nComressionLeftSizeEnd;

	if (nSizeBeforeCompression > 0)
	{
		unsigned char szBuff[BUF_SIZE];
		unsigned int nLen = BUF_SIZE;
		lzo1x_1_compress( 
			(const unsigned char *)ps2cRet->data,
			nSizeBeforeCompression,
			szBuff,
			&nLen,
			wrkmem);
		
		if (nLen >= BUF_SIZE - sizeof(S2C_GETSUBLIST_RET) )
			return enSocialErr_RetryLater;
		
		memcpy(ps2cRet->data,szBuff,nLen);          
		ps2cRet->comHeader.proHeader.len = nLen + sizeof(S2C_GETSUBLIST_RET) - 1 - PROTOCOL_SIZE;
	}//endif

	SendDataToClient(nLauncherIdx, buf, ps2cRet->comHeader.proHeader.len + PROTOCOL_SIZE);

	return enSocialErr_None;
}

int SocialUnit::GetAnnouncement(int nLauncherIdx, void *pParam, int nMsgSize)
{
	// 应该用叶节点来检查权限
	if( !CheckPrivilege(nLauncherIdx, m_tplId, m_layer, enSUO_GetAnnouncement) )
		return enSocialErr_NoPrivilige;

	const int				BUF_SIZE = 2048;
	C2S_GETANNOUNCEMENT_REQ	*pc2sReq = (C2S_GETANNOUNCEMENT_REQ*)pParam;

	if( m_attrs.IsAttrHasData(enSUAttr_Announcement) && pc2sReq->anaucementversion != m_AnuncmentVersion)
	{
		char	*pAttrData;
		int		nAttrSize;
		nAttrSize = m_attrs.GetAttr(enSUAttr_Announcement, pAttrData);

		int	nPackageLen = sizeof(S2C_GETANNOUNCEMENT_RET) - 1 + nAttrSize;
					
		_ASSERT(nPackageLen <= BUF_SIZE);
		if(nPackageLen > BUF_SIZE)
			return enSocialErr_GetAnnounceFailed;

		char					buf[BUF_SIZE] = { 0 };
		S2C_GETANNOUNCEMENT_RET	*ps2cRet = (S2C_GETANNOUNCEMENT_RET*)buf;
		ps2cRet->comHeader.proHeader.protocol = s2c_social_family;
		ps2cRet->comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
		ps2cRet->comHeader.proHeader.len = nPackageLen - PROTOCOL_SIZE;
		ps2cRet->comHeader.comParam = pc2sReq->comHeader.comParam;
		ps2cRet->announcementLen = nAttrSize;
		ps2cRet->announceversion = m_AnuncmentVersion;
		memcpy(ps2cRet->data, pAttrData, nAttrSize);
			   
		SendDataToClient(nLauncherIdx, buf, nPackageLen);
		return enSocialErr_None;
	}
//	else
//		return enSocialErr_AnnounceNotExist;

	return enSocialErr_None;
}

int	SocialUnit::GetSubListInfo(int nStartOffset, char *pOutBuf, int &nLeftSize)
{
	int	nCount = 0;
	int	nPassedCount= 0;
	int	nMaxCount = (int)m_subUnits.size();

	for(int nSubUnit = 0; nSubUnit < nMaxCount && nCount < MAXCOUNT_SUBLIST_ONETIMEGET; ++nSubUnit)
	{
		if(NULL == m_subUnits[nSubUnit])
			continue;

		if(nPassedCount < nStartOffset)
		{
			++nPassedCount;
			continue;
		}

		int	nUsedSize = m_subUnits[nSubUnit]->GetUnitTransferInfo(pOutBuf, nLeftSize);

		if(nUsedSize > 0)
		{
			++nCount;
			pOutBuf += nUsedSize;
			nLeftSize -= nUsedSize;
		}
		else
			break;
	}
	
	return nCount;
}

int SocialUnit::GetUnitTransferInfo(char *pOutBuf, int nBufSize)
{
	int	nUsedSize = 0;

	_ASSERT(nBufSize >= sizeof(SU_UNITTRANSFER_INFO));
	if(nBufSize < sizeof(SU_UNITTRANSFER_INFO))
		return nUsedSize;

	SU_UNITTRANSFER_INFO *pUnitInfo = (SU_UNITTRANSFER_INFO*)pOutBuf;

	strncpy(pUnitInfo->comInfo.ownerName, 
			m_ownerName, 
			sizeof(pUnitInfo->comInfo.ownerName)
			);

	pUnitInfo->comInfo.ownerName[MAXSIZE_ORGNAME - 1] = 0;

	char	*pAttrData;
	int		nAttrSize;

	if( m_attrs.IsAttrHasData(enSUAttr_UnitName) )
	{
		nAttrSize = m_attrs.GetAttr(enSUAttr_UnitName, pAttrData);

		_ASSERT(nAttrSize <= sizeof(pUnitInfo->comInfo.unitName));

		if(nAttrSize <= sizeof(pUnitInfo->comInfo.unitName))
			strncpy(pUnitInfo->comInfo.unitName, pAttrData, nAttrSize);
	}

	if( m_attrs.IsAttrHasData(enSUAttr_PlayerInfo) )
	{
		_ASSERT(m_layer == enSULayer_Player);

		nAttrSize = m_attrs.GetAttr(enSUAttr_PlayerInfo, pAttrData);

		_ASSERT(nAttrSize == sizeof(SU_PLAYER_INFO));

		if(nAttrSize != sizeof(SU_PLAYER_INFO))
			return 0;

		memcpy(&pUnitInfo->playerInfo, pAttrData, nAttrSize);

		int nPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(pUnitInfo->comInfo.ownerName);
		if (IsValidPlayer(nPlayerIndex))
		{
			pUnitInfo->playerInfo.isOnline = 1;
		}//endif
		else
		{
			pUnitInfo->playerInfo.isOnline = 0;
		}
		
		//AdditianlPart SU_PLAYER_INFO_EX :Prevent chat info, top owner layer info
		pUnitInfo->playerInfoEx.topOwnerLayer = enSULayer_Player;
		
		SocialUnit * pOwnParent               = m_pParent;
		while (pOwnParent)
		{
            const char * pParentOwnerName = pOwnParent->GetOwnerName();
			
			if (pParentOwnerName)
			{
                if (strcmp(pParentOwnerName,m_ownerName)==0)
				{
                    pUnitInfo->playerInfoEx.topOwnerLayer = pOwnParent->GetLayer();
				}//endif

			}//endif

            pOwnParent = pOwnParent->GetParent();
		}//end for while

		for (int i=0;i<MAX_CHAT_OPERATION_FORBIDDEN_LAYER;i++)
		{
			pUnitInfo->playerInfoEx.ForbidChatState[i] = 1;
			PrivilegeSet &  priSet = GetPrivilegeSet();

			if (priSet.Check(m_tplId,i+enSULayer_Gens,enSUO_ChatInUnit))
                 pUnitInfo->playerInfoEx.ForbidChatState[i] = 0;

		}//end for i

	}//endif

	pUnitInfo->tongInfoEx.nCityMapId  = GetCityMapId(m_attrs);
	pUnitInfo->tongInfoEx.nPoolMapId  = GetPoolMapId(m_attrs);
	pUnitInfo->tongInfoEx.nSubUnitNum = GetChildCount();
	memcpy(&pUnitInfo->comInfo.unitGuid, &m_unitGuid, sizeof(pUnitInfo->comInfo.unitGuid));

	return sizeof(SU_UNITTRANSFER_INFO);
}

void SocialUnit::ClearLeafUnitPrivilege(int nLayer)
{
	if( IsLeaf() )
		m_privSet.Remove(m_tplId, nLayer);

	int	nMaxCount = (int)m_subUnits.size();

	for(int i = 0; i < nMaxCount; ++i)
	{
		if(m_subUnits[i])
			m_subUnits[i]->ClearLeafUnitPrivilege(nLayer);
	}
}

void SocialUnit::ChatChannelOpe(SocialUnit *pChannelUnit, bool bAddChannel)
{

	if (pChannelUnit == NULL)
		return ;

	SocialUnitAttr	&chanUnitAttr = pChannelUnit->GetUnitAttr();

	if( !chanUnitAttr.IsAttrHasData(enSUAttr_ChatChannel) )
		return;

	if( !IsLeaf() )
	{
		int nMaxCount = (int)m_subUnits.size();

		for(int i = 0; i < nMaxCount; ++i)
		{
			if(m_subUnits[i])
				m_subUnits[i]->ChatChannelOpe(pChannelUnit, bAddChannel);
		}
		
		return;
	}

	DWORD	dwRoomId = GetChatRoomId(chanUnitAttr);
	if(INVALID_ROOM_ID == dwRoomId)
		return;

	int	nPlayerIdx = g_PlayerInfoToIndex.GetIndexByName(m_ownerName);
	if(!IsValidPlayer(nPlayerIdx))
		return; //不在线

	if(bAddChannel)
	{
		const char *szName = GetLayerName(pChannelUnit->GetTplId(), pChannelUnit->GetLayer());
		if (szName == NULL )
			return ;

		g_ChatCenterS.s2cChannelOpe(nPlayerIdx, dwRoomId, chat_addchannel, szName);

		int	nRet = OnJoinRoom(nPlayerIdx, dwRoomId);

		if(chat_err_none != nRet)
			ChatErrCodeToClient(nPlayerIdx, nRet);	
		else
		{
			ChatRoomMgr_S	&roomMgr = g_ChatCenterS.GetRoomMgr();
			ChatObjectMgr_S	*pObjMgr = g_ChatCenterS.GetChatObjMgr(nPlayerIdx);

			if(pObjMgr)
			{
				if( m_privSet.Check(pChannelUnit->GetTplId(), pChannelUnit->GetLayer(), enSUO_ChatInUnit) )
					pObjMgr->ChgRoomChatPrivilege(dwRoomId, true);
				else
					pObjMgr->ChgRoomChatPrivilege(dwRoomId, false);
			}
		}
	}
	else
	{
		OnLeaveRoom(nPlayerIdx, dwRoomId);
		g_ChatCenterS.s2cChannelOpe(nPlayerIdx, dwRoomId, chat_delchannel, NULL);
	}
}

int SocialUnit::OnJoinRoom(int nPlayerIdx, DWORD dwRoomId)
{
	ChatRoomMgr_S	&roomMgr = g_ChatCenterS.GetRoomMgr();
	ChatRoom_S		*pRoom = roomMgr.GetChatRoom(dwRoomId);
	ChatObjectMgr_S	*pObjMgr = g_ChatCenterS.GetChatObjMgr(nPlayerIdx);

	int	nRet = chat_err_none;			

	if(pRoom)
		nRet = pRoom->AddMember(nPlayerIdx);

	if(chat_err_none != nRet)
		return nRet;

	if(pObjMgr)
		nRet = pObjMgr->JoinRoom(dwRoomId);

	return nRet;
}

void SocialUnit::OnLeaveRoom(int nPlayerIdx, DWORD dwRoomId)
{
	ChatRoomMgr_S	&roomMgr = g_ChatCenterS.GetRoomMgr();
	ChatRoom_S		*pRoom = roomMgr.GetChatRoom(dwRoomId);
	ChatObjectMgr_S	*pObjMgr = g_ChatCenterS.GetChatObjMgr(nPlayerIdx);

	if(pRoom)
		pRoom->DelMemberDirect(nPlayerIdx);

	if(pObjMgr)
		pObjMgr->LeaveRoom(dwRoomId);
}

void SocialUnit::AddLeafUnitPrivilege(int nLayer)
{
	if( !IsLeaf() )
	{
		int nMaxCount = (int)m_subUnits.size();

		for(int i = 0; i < nMaxCount; ++i)
		{
			if(m_subUnits[i])
				m_subUnits[i]->AddLeafUnitPrivilege(nLayer);
		}

		// 权限存储在叶节点，非叶节点不存储
		return;
	}

	RelationLayer *pLayer = GetRelationLayer(m_tplId, nLayer);

	_ASSERT(pLayer);
	if(NULL == pLayer)
		return;

	SocialUnit *pTopUnit = GetUpNUnit(this, nLayer);

	_ASSERT(pTopUnit);
	if(NULL == pTopUnit)
		return;

	bool	bIsOwner = pTopUnit->IsOwner(m_ownerName);

	SocialUnit *pSecondParentUnit = GetUpNUnit(this, nLayer-1); 
	
	_ASSERT(pSecondParentUnit);
	if (NULL == pSecondParentUnit)
		return ;

	bool        bIsDefaultAllowed = ((pSecondParentUnit == this) || (pSecondParentUnit->IsOwner(m_ownerName)));

	for(int nOpe = 0; nOpe < pLayer->OperationCount; ++nOpe)
	{
		if((pLayer->Operations[nOpe].IsDefault && bIsDefaultAllowed) || bIsOwner || pLayer->Operations[nOpe].IsToAll )
			m_privSet.Add(m_tplId, nLayer, pLayer->Operations[nOpe].Id);		
	}//end for nOpe
}

void SocialUnit::SyncLeafUnitPrivilege()
{
	if( !IsLeaf() )
	{
		int nMaxCount = (int)m_subUnits.size();

		for(int i = 0; i < nMaxCount; ++i)
		{
			if(m_subUnits[i])
				m_subUnits[i]->SyncLeafUnitPrivilege();
		}	
	}

	int nPlayerIdx = g_PlayerInfoToIndex.GetIndexByName(m_ownerName);

	if(IsValidPlayer( nPlayerIdx))
	{
		Player[nPlayerIdx].SyncSocialRelation(m_tplId);
		Player[nPlayerIdx].BroadCastSocialInfo();
	}//endif

}

void SocialUnit::PlayerOnLine(int nPlayerIdx, bool bOnline)
{
	_ASSERT(enSULayer_Player == m_layer);

	if(enSULayer_Player != m_layer)
		return;

	RefreshPlayerInfoAttr(m_attrs, nPlayerIdx, bOnline);

	if(bOnline)
	{	
		// 如果当前玩家的节点在没有load到内存时，其上层节点
		// 离开或加入更上层的节点，此时玩家的权限需要增加或删除
		// 但此时数据并不在内存中，所以在玩家上线时作判断，然后加上或删掉
		SocialUnit	*pTopUnit = GetTopUnit(this);
		_ASSERT(pTopUnit);
		if(NULL == pTopUnit)
			return;

		int	nMaxLayer = pTopUnit->GetLayer();

		if(m_maxJoinedLayer < (BYTE)nMaxLayer)
		{
			for(int nLayer = m_maxJoinedLayer + 1; nLayer <= nMaxLayer; ++nLayer)
				AddLeafUnitPrivilege(nLayer);

			m_maxJoinedLayer = (BYTE)nMaxLayer;

			_ASSERT(m_pParent && m_pParent->IsAllSubUnitLoad());

			if( IsSocialTmplSave(m_tplId) && m_pParent && m_pParent->IsAllSubUnitLoad())
				SocialSerializer::Singleton().UpdateAppDataReq(GetNetConnectIdx(nPlayerIdx), m_pParent);
		}
		else if(m_maxJoinedLayer > (BYTE)nMaxLayer)
		{
			for(int nLayer = nMaxLayer + 1; nLayer <= m_maxJoinedLayer; ++nLayer)
				ClearLeafUnitPrivilege(nLayer);

			if( pTopUnit->IsOwner( GetPlayerName(nPlayerIdx) ) )
			{
				int nLeaveBuff = GetLeaveBuff(pTopUnit->GetTplId(), pTopUnit->GetLayer() + 1);
				AddBuffToPlayer(nPlayerIdx, nLeaveBuff);
			}

			m_maxJoinedLayer = (BYTE)nMaxLayer;

			_ASSERT(m_pParent && m_pParent->IsAllSubUnitLoad());

			if( IsSocialTmplSave(m_tplId) && m_pParent && m_pParent->IsAllSubUnitLoad())
				SocialSerializer::Singleton().UpdateAppDataReq(GetNetConnectIdx(nPlayerIdx), m_pParent);
		}

		Player[nPlayerIdx].SyncSocialRelation(m_tplId);
		Player[nPlayerIdx].BroadCastSocialInfo();

		NotifyWarInfo(nPlayerIdx);
	}

	// 聊天频道处理放在最后，因为上面有可能改变权限
	SocialUnit	*pParent = m_pParent;
	while(pParent)
	{
		ChatChannelOpe(pParent, bOnline);
		pParent = pParent->GetParent();
	}

	NotifyMemberOnline(m_ownerName, m_pParent, bOnline);
}

void SocialUnit::AddSubUnit(SocialUnit *pUnit)
{
	if(NULL == pUnit)
		return;

	int	nSize = (int)m_subUnits.size();

	for(int i = 0; i < nSize; ++i)
	{
		if(NULL == m_subUnits[i])
		{	
			m_subUnits[i] = pUnit;
			return;
		}
	}

	m_subUnits.push_back(pUnit);
}

void SocialUnit::RemoveSubUnit(SocialUnit *pUnit)
{
	int	nMaxCount = m_subUnits.size();

	for(int nSubUnit = 0; nSubUnit < nMaxCount; ++nSubUnit)
	{
		if(m_subUnits[nSubUnit] == pUnit)
		{
			m_subUnits[nSubUnit] = NULL;
			break;
		}
	}
}

int SocialUnit::SaveUnitToBuf(char *pBuf, int nBufSize)
{
	if(NULL == pBuf || nBufSize <= 0)
		return 0;

	if(nBufSize < sizeof(DB_UNIT_DATA) - 1)
	{
		_ASSERT(false);
		return 0;
	}

	DB_UNIT_DATA	*pUnitData = (DB_UNIT_DATA*)pBuf;
		
	pUnitData->version = CURRENT_SOCIALDATA_DB_VERSIONNO;
	pUnitData->tplId = m_tplId;
	pUnitData->layer = m_layer;
	pUnitData->maxJoinedLayer = m_maxJoinedLayer;
	strncpy(pUnitData->ownerName, m_ownerName, sizeof(pUnitData->ownerName));
	memcpy(&pUnitData->unitGuid, &m_unitGuid, sizeof(pUnitData->unitGuid));

	int	nLeftSize = nBufSize - (pUnitData->data - pBuf);
	
	int	nUsedSize;
	nUsedSize = m_attrs.SaveAttrToBuf(pUnitData->data, nLeftSize);
	pUnitData->attrSize = nUsedSize;
	nLeftSize -= nUsedSize;

#ifdef _DEBUG
	CFS_FILELOGS::WriteDebugLog("Save Privileges: %s\n", m_ownerName);
#endif

	nUsedSize = m_privSet.Save(pUnitData->data + nUsedSize, nLeftSize);

	pUnitData->privSize = nUsedSize;
	nLeftSize -= nUsedSize;

	return pUnitData->attrSize + pUnitData->privSize + sizeof(DB_UNIT_DATA) - 1;
}

void SocialUnit::GetUnitPlayerCount(int &nTotalCount, int &nTotalOnlineCount)
{
	// Note: nTotalCount and nTotalOnlineCount must be zero when first call this function

	if(m_layer > enSULayer_Player)
	{
		int nSubUnitCount = m_subUnits.size();

		for(int nSubUnit = 0; nSubUnit < nSubUnitCount; ++nSubUnit)
		{
			if(NULL != m_subUnits[nSubUnit])
			{
				m_subUnits[nSubUnit]->GetUnitPlayerCount(nTotalCount, nTotalOnlineCount);
			}
		}
	}
	else
	{
		++nTotalCount;

		SocialUnitAttr &attr = GetUnitAttr();
		char *pInfo;
		int nSize = attr.GetAttr(enSUAttr_PlayerInfo, pInfo);

		if(sizeof(SU_PLAYER_INFO) == nSize)
		{
			int nPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(GetOwnerName());
			if( IsValidPlayer(nPlayerIndex))
				++nTotalOnlineCount;
		}//endif
	}
}
void SocialUnit::RecountSubUnitNum()
{
    int nSubUnitCount = m_subUnits.size();
	int nRealCount    = 0;
	
	for(int nSubUnit = 0; nSubUnit < nSubUnitCount; ++nSubUnit)
	{
		if(NULL != m_subUnits[nSubUnit])
		{
			++nRealCount;
		}//endif

	}//end for nSubUnit

	if (m_subUnitCnt != nRealCount)
	{
        m_subUnitCnt  = nRealCount;
        CFS_FILELOGS::WriteDebugLog("Refix DB SubUnit Count!");
	}//endif
}

int SocialUnit::ReqJoinHighLevel(int nLuancherIdx, void *pParam, int nMsgSize)
{
	if( m_layer == enSULayer_Gens && (!CheckPrivilege(nLuancherIdx, m_tplId, m_layer, enSUO_ReqJoinHigherLevel)) )
		return enSocialErr_NoPrivilige;
    
	//1.被申请方检查
	C2S_HIGH_LEVEL_JOIN_REQ * req = (C2S_HIGH_LEVEL_JOIN_REQ *)pParam;
	req->receiverName[MAXSIZE_ROLENAME - 1] = 0;

	//1.0 判断氏族长或侯主是否在线
	const char             * pUnitOwnerName = req->receiverName;  
	int	                     nTargetIdx     = 0;
	
	if (pUnitOwnerName[0])
	{
       	nTargetIdx = g_PlayerInfoToIndex.GetIndexByName(pUnitOwnerName);
		if (!IsValidPlayer(nTargetIdx))
			return enSocialErr_OwnerNotOnline;
	}//endif
	else
	{
		return enSocialErr_OwnerNotOnline;
	}//end else

	//1.1 判断被申请方还没有读取出，或者已经解散
	SocialUnit * pOwnerLeafUnit = GetLeafUnit(nTargetIdx,enSUTplId_Tong);
	if (!pOwnerLeafUnit)
		return enSocialErr_UnitNoExist;

	SocialUnit * pUnit = GetUpNUnit(pOwnerLeafUnit, m_layer + 1);
	if (!pUnit)
		return enSocialErr_UnitNoExist;

    //1.2 判断人数是否已经过了
	if( pUnit->GetChildCount() >= GetMaxSubUnitCnt(m_tplId, pUnit->GetLayer()) )
		return enSocialErr_MemberNumIsFull;

	
	//2. 申请方检查
	//2.1 申请方必须是玩家或者氏族长
	if(!IsOwner(Player[nLuancherIdx].GetPlayerName()))
		return enSocialErr_NoPrivilige;

	//2.2 申请方等级判断
	RelationLayer *pLayer = GetRelationLayer(m_tplId, pUnit->GetLayer());
	if(NULL == pLayer)
	{
		_ASSERT(false);
		return enSocialErr_ReqJoinFaild;
	}
	else
	{
		if( GetPlayerLevel(nLuancherIdx) < pLayer->Operations[enSUO_AddSubUnit].ReqTargetLevel )
			return enSocialErr_LevelInValid;
	}//end else
	
	//2.3 层次判断
	if (m_pParent || GetLayer()+1 != pUnit->GetLayer() )
	{
		return enSocialErr_RelationInvalid;
	}//endif
	
	//2.4 脱离Buff判断 
	int nBuffMustNotHaveOnJoin = GetBuffMustNotHaveOnJoin(m_tplId, m_layer + 1);
	if(INVALID_BUFF_ID != nBuffMustNotHaveOnJoin)
	{
		if( BuffMgr::Singleton().IsHaveBuff(Player[nLuancherIdx].m_nIndex, nBuffMustNotHaveOnJoin) )
			return enSocialErr_HaveLeaveBuff;
	}//endif
    
	if(enSUReqConfirm_None == req->nh.ch.comParam.confirmCode)
	{
	    _ASSERT(nMsgSize <= MAXSIZE_UNCONFIRMREQ);
		
		if(nMsgSize > MAXSIZE_UNCONFIRMREQ)
			return enSocialErr_ReqJoinFaild;
		
		char	buf[MAXSIZE_UNCONFIRMREQ + MAXSIZE_HINT_MSG + sizeof(S2C_REQ_COMFIRM) - 1];
		
		S2C_REQ_COMFIRM	*ps2cRet = (S2C_REQ_COMFIRM*)buf;
		ps2cRet->comHeader.proHeader.protocol = s2c_social_family;
		ps2cRet->comHeader.proHeader.subProtocol = enSRProtocol_ReqToConfirm;
		ps2cRet->comHeader.comParam = req->nh.ch.comParam;
		ps2cRet->reqSize            = nMsgSize;
		memcpy(ps2cRet->data, pParam, nMsgSize);
		
		const char *szLayerName = GetLayerName(enSUTplId_Tong, pUnit->GetLayer());
		if(NULL == szLayerName)
			szLayerName = SOCIAL_EMPTY_STRING;
		
		const char *szUnitName = SOCIAL_EMPTY_STRING;
		if (GetUnitName(pUnit->GetUnitAttr()))
			szUnitName = GetUnitName(pUnit->GetUnitAttr());

		ps2cRet->hitMsgLen = FormatMsg(ps2cRet->data + nMsgSize,
			MAXSIZE_HINT_MSG,
			MSG_SOCIAL_REQ_JOIN,
			Player[nLuancherIdx].GetPlayerName(),
			szLayerName,
			szUnitName
					);

		ps2cRet->comHeader.proHeader.len = sizeof(S2C_REQ_COMFIRM) - 1 + 
			ps2cRet->hitMsgLen + ps2cRet->reqSize - PROTOCOL_SIZE;
		
		SendDataToClient(nTargetIdx, buf, ps2cRet->comHeader.proHeader.len + PROTOCOL_SIZE);

		return enSocialErr_WaitToConfirm;

	}
	else if(enSUReqConfirm_Ok == req->nh.ch.comParam.confirmCode)
	{
		C2S_ADDSUBUNIT_REQ	c2sReq;
		
		memset(&c2sReq, 0, sizeof(c2sReq));
		c2sReq.nh.ch.proHeader.protocol    = c2s_social_family;
		c2sReq.nh.ch.proHeader.subProtocol = enSRProtocol_UnitOperation;
		c2sReq.nh.ch.proHeader.len         = sizeof(c2sReq) - PROTOCOL_SIZE;
		c2sReq.nh.ch.comParam.confirmCode  = enSUReqConfirm_Ok;
		c2sReq.nh.ch.comParam.opeId        = enSUO_AddSubUnit;
		c2sReq.nh.ch.comParam.unitLayer    = pUnit->GetLayer();
		c2sReq.nh.ch.comParam.unitTplId    = enSUTplId_Tong;
		
		strncpy(c2sReq.nh.szLauncherName, 
			Player[nTargetIdx].GetPlayerName(),
			sizeof(c2sReq.nh.szLauncherName)
			);
		
	    strncpy(c2sReq.receiverName, Player[nLuancherIdx].GetPlayerName(), sizeof(c2sReq.receiverName));

		pUnit->AddSubUnit(nTargetIdx,&c2sReq,sizeof(c2sReq));

	}
	else if (enSUReqConfirm_Ignore == req->nh.ch.comParam.confirmCode)
	{
        return enSocialErr_RequestIgnored;
	}
	else if (enSUReqConfirm_Refuse == req->nh.ch.comParam.confirmCode)
	{
        return enSocialErr_RequestRefused;
	}

	return enSocialErr_None;
}

int SocialUnit::GetTotalPlayerNum(void)
{
    int nTotalPlayerNum  = 0 ;
    int nOnlinePlayerNum = 0 ;

	GetUnitPlayerCount(nTotalPlayerNum,nOnlinePlayerNum);
	return nTotalPlayerNum;
}

int SocialUnit::GetOnlinePlayerNum(void)
{
	int nTotalPlayerNum  = 0 ;
    int nOnlinePlayerNum = 0 ;
	
	GetUnitPlayerCount(nTotalPlayerNum,nOnlinePlayerNum);
	return nOnlinePlayerNum;
}

int SocialUnit::GetPlayerAvgLevel()
{
    int nCount = 0;
	int nLevel = 0;
	GetUnitPlayerLevelAndCount(nCount,nLevel);

	return nLevel / nCount;
}

void SocialUnit::GetUnitPlayerLevelAndCount(int & nCount,int & nLevel)
{
	if (!IsLeaf())
	{
		int	nMaxCount = (int)m_subUnits.size();
		
		for(int i = 0; i < nMaxCount; ++i)
		{
			if (m_subUnits[i])
				m_subUnits[i]->GetUnitPlayerLevelAndCount(nCount,nLevel);
		}//end for i
		
	}//endif
	else
	{
        nCount ++ ;
		
		if( m_attrs.IsAttrHasData(enSUAttr_PlayerInfo) )
		{
			char * pAttrData = NULL;
			int          nAttrSize = 0;

			_ASSERT(m_layer == enSULayer_Player);
			
			nAttrSize = m_attrs.GetAttr(enSUAttr_PlayerInfo, pAttrData);
			
			_ASSERT(nAttrSize == sizeof(SU_PLAYER_INFO));
			
			if(nAttrSize != sizeof(SU_PLAYER_INFO))
				return ;
			
			const SU_PLAYER_INFO * pRoleInfo  = (const SU_PLAYER_INFO *)pAttrData;
			nLevel                           += pRoleInfo->level;

		}//endif
		else
		{
			_ASSERT(false);
		}//end for else

	}//end else
}

bool SocialUnit::GetScriptUseData(const int nIndex, unsigned long & nValueRet)
{
	if (nIndex >= 0 && nIndex< MAX_SOCIAL_SCRIPT_DATA_NUM_NONEEDSAVE)
	{
		nValueRet = m_ScriptDatas[nIndex];
		return true;
	}//endif

	return false;
}

bool SocialUnit::SetScriptUseData(const unsigned long dwNum, const int nIndex)
{
	if (nIndex >= 0 && nIndex< MAX_SOCIAL_SCRIPT_DATA_NUM_NONEEDSAVE)
	{
		m_ScriptDatas[nIndex] = dwNum;
		return true;
	}//endif
	
	return false;
}

bool SocialUnit::IsHasRefuseBuff(int nTargetPlayerIndex, int nLuncherIndex)
{
	if (!IsValidPlayer(nTargetPlayerIndex) || !IsValidPlayer(nLuncherIndex))
		return true;
	
	int nTargetNpcIndex   = Player[nTargetPlayerIndex].m_nIndex;
	int nLuancherNpcIndex = Player[nLuncherIndex].m_nIndex;

	if (!IsValidNpc(nTargetNpcIndex) || !IsValidNpc(nLuancherNpcIndex))
		return true;

	BuffMgr& bm = BuffMgr::Singleton();
	RelationLayer *pCurrLayerRL = GetRelationLayer(m_tplId, m_layer);

	if (!pCurrLayerRL)
		return true;
	
	for (int nBuffIter = 0; nBuffIter < CHANGEOWNERBUFF_NUM; ++nBuffIter)
	{
		if (0 == pCurrLayerRL->ChangeOwnerBuff[nBuffIter])
			continue;
	
		int nBuffId = pCurrLayerRL->ChangeOwnerBuff[nBuffIter];		
		if (bm.IsHaveBuff(nTargetNpcIndex, nBuffId) || bm.IsHaveBuff(nLuancherNpcIndex, nBuffId))
			return true;		
	}

	return false;
}