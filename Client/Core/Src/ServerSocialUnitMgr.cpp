//////////////////////////////////////////////////////////////////////
// 
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:11   14:12
//      File_base        : ServerSocialUnitMgr
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
#include "CoreRelated.h"
#include "ServerSocialUnitMgr.h"
#include "SocialSerializer.h"
#include "CoreUtil.h"
#include "KSubWorldSet.h"
#include "tong_war_manager.h"
#include "ILogDevice.h"
#include "social_recruit_svr.h"
#include "pool_combat_mgr.h"
#include "KPlayerSet.h"

#define MAX_INVALID_INFO_LEN 1024

ServerSocialUnitMgr& ServerSocialUnitMgr::Singleton()
{
	static ServerSocialUnitMgr SocialUnitMgr;
	return SocialUnitMgr;
}

ServerSocialUnitMgr::ServerSocialUnitMgr()
{
	memset(m_netProc, 0, sizeof(m_netProc));
	m_netProc[enSRProtocol_UnitOperation] = &ServerSocialUnitMgr::UnitOperationReq;
	m_netProc[enSRProtocol_ReqConfirmRet] = &ServerSocialUnitMgr::ReqConfirmRet;
	m_netProc[enSRProtocol_ContributeCityRes] = &ServerSocialUnitMgr::SetCityResReq;
	m_netProc[enSRProtocol_DistillCityRes] = &ServerSocialUnitMgr::GetCityResReq;
	m_netProc[enSRProtocol_ReqCityInfo] = &ServerSocialUnitMgr::GetCityInfoReq;
	m_netProc[enSRProtocol_SetCityTaxRate] = &ServerSocialUnitMgr::SetCityTaxRateReq;
	m_netProc[enSRProtocol_GetUnitInfo] = &ServerSocialUnitMgr::ReqUnitInfo;

 	memset(m_dbRetProc, 0, sizeof(m_dbRetProc));
	m_dbRetProc[enSoc_DBOpe_LoadOwnTree - enSoc_DBOpe_Begin - 1] 
						= &ServerSocialUnitMgr::LoadOwnTreeUnitRet;
	m_dbRetProc[enSoc_DBOpe_LoadAllSubUnit - enSoc_DBOpe_Begin - 1] 
						= &ServerSocialUnitMgr::LoadAllSubUnitRet;
	m_dbRetProc[enSoc_DBOpe_AddUnit - enSoc_DBOpe_Begin - 1]
						= &ServerSocialUnitMgr::DBAddUnitRet;
	m_dbRetProc[enSoc_DBOpe_CheckUnitName - enSoc_DBOpe_Begin - 1]
						= &ServerSocialUnitMgr::CheckUnitNameRet;

	m_IsMapLordLoadedReady = false;
	m_IsMapPoolLoadedReady = false;
	
}

ServerSocialUnitMgr::~ServerSocialUnitMgr()
{
	for(int nTpl = enSUTplId_None + 1; nTpl < enSUTplId_Num; ++nTpl)
	{
		GUID2SOCIALUNIT::iterator	itUnit = m_guid2SocialUnit[nTpl].begin();
		GUID2SOCIALUNIT::iterator	itUnitEnd = m_guid2SocialUnit[nTpl].end();
		
		for(; itUnit != itUnitEnd; ++itUnit)
		{
			_ASSERT(itUnit->second);
			
			if(itUnit->second)		
				SocialAllocator::FreeUnit(itUnit->second);
		}
		
	}
	
}

void ServerSocialUnitMgr::ProcessProcotol(int nPlayerIdx, void *pNetMsg, int nMsgSize)
{
	C2S_COMOPE_HEADER	*pComHeader = (C2S_COMOPE_HEADER*)pNetMsg;
	int					nSubProtocol = pComHeader->proHeader.subProtocol;
	
	_ASSERT(nSubProtocol > enSRProtocol_None && nSubProtocol < enSRProtocol_Num);
	
	if(nSubProtocol > enSRProtocol_None && nSubProtocol < enSRProtocol_Num)
	{
		if(m_netProc[nSubProtocol])
			(this->*m_netProc[nSubProtocol])(nPlayerIdx, pNetMsg, nMsgSize);
	}
}

void ServerSocialUnitMgr::UnitOperationReq(int nLauncherIdx, void *pParam, int nMsgSize)
{
	C2S_COMOPE_HEADER	*pc2sHeader = (C2S_COMOPE_HEADER*)pParam;
	int					nTplId = pc2sHeader->comParam.unitTplId;
	int					nLayer = pc2sHeader->comParam.unitLayer;
	int					nOpeId = pc2sHeader->comParam.opeId;

	RelationSet			&relationSet = GetRelationSet(nLauncherIdx);

	if( !relationSet.IsOwnTreeLoad(nTplId) )
	{
		SocialErrCode2Client(nLauncherIdx, pc2sHeader->comParam, enSocialErr_RetryLater);
		return;
	}

	RelationRecord		*pRelRecord = relationSet.GetRelationByTemplate(nTplId);

	if(NULL == pRelRecord)
		return;

	SocialUnit *pTargetUnit = GetUpNUnit(pRelRecord->pLeafUnit, nLayer);

	_ASSERT(pTargetUnit);

	if(NULL == pTargetUnit)
		return;

	int	nRet = CheckComOpeCond(nLauncherIdx, nTplId, nLayer, nOpeId);

	if(enSocialErr_None == nRet)
		nRet = pTargetUnit->ProcessOperation(nLauncherIdx, pParam, nMsgSize);

	if(enSocialErr_None == nRet)
		DeductComOpeCost(nLauncherIdx, nTplId, nLayer, nOpeId);
	else if(nRet > enSocialErr_None)
		SocialErrCode2Client(nLauncherIdx, pc2sHeader->comParam, nRet);
}

void ServerSocialUnitMgr::ReqConfirmRet(int nLauncherIdx, void *pParam, int nMsgSize)
{
	// 这条协议是被邀请者确认请求后发送过来的，所以需要
	// 找到真正的邀请者，然后将请求转发过去
	C2S_NEEDCONFIRMOPE_HEADER	*pc2sHeader = (C2S_NEEDCONFIRMOPE_HEADER*)pParam;

	pc2sHeader->szLauncherName[sizeof(pc2sHeader->szLauncherName) - 1] = '\0';
	int	nRealLauncherIdx = g_PlayerInfoToIndex.GetIndexByName(pc2sHeader->szLauncherName);

	if(IsValidPlayer(nRealLauncherIdx))
		UnitOperationReq(nRealLauncherIdx, pParam, nMsgSize);	
}

void ServerSocialUnitMgr::SetCityTaxRateReq(int nLauncherIdx, void *pParam, int nMsgSize)
{
	C2S_SET_CITYTAXRATE_REQ	*pc2sReq = (C2S_SET_CITYTAXRATE_REQ*)pParam;
	RelationSet &relationSet = GetRelationSet(nLauncherIdx);

	if( !relationSet.IsOwnTreeLoad(enSUTplId_Tong) )
	{
		SocialErrCode2Client(nLauncherIdx, pc2sReq->comHeader.comParam, enSocialErr_RetryLater);
		return;
	}

	if(pc2sReq->nTaxRate < MIN_CITY_DISCOUNT || pc2sReq->nTaxRate > MAX_CITY_DISCOUNT)
		return;

	int	nCityMapIdx = GetCityMapIdx(nLauncherIdx);
	if(INVALID_WORLD_INDEX == nCityMapIdx)
		return;

	int	nPlayerNpcIdx = Player[nLauncherIdx].m_nIndex;
	if(!IsValidNpc(nPlayerNpcIdx) || Npc[nPlayerNpcIdx].GetSubWorldIndex() != nCityMapIdx )
		return;

	int	nWorldLordNpcIdx = SubWorld[nCityMapIdx].GetLord();
	if(!IsValidNpc(nWorldLordNpcIdx))
		return;

	BuffMgr &bm = BuffMgr::Singleton();
	int nCityTaxRateBuff = GetCityTaxRateBuff();

	if( IsBuffIdValid(nCityTaxRateBuff) )
	{
		if( bm.IsHaveBuff(Player[nLauncherIdx].m_nIndex, nCityTaxRateBuff) )
		{
			SocialErrCode2Client(nLauncherIdx, 
				pc2sReq->comHeader.comParam, 
				enSocialErr_WaitToSetCityTaxRate
				);
			return;
		}
		else
			bm.AddNpcBuff(Player[nLauncherIdx].m_nIndex, 
				Player[nLauncherIdx].m_nIndex, 
				nCityTaxRateBuff
				);
	}

	Npc[nWorldLordNpcIdx].m_UnaryAttrMgr.Set(nuai_city_taxrate, pc2sReq->nTaxRate);
	Npc[nWorldLordNpcIdx].SetDataChangedFlag(true);
	Npc[nWorldLordNpcIdx].Save();

	NotifyCityDiscount(nWorldLordNpcIdx);	
}

void ServerSocialUnitMgr::ReqUnitInfo(int nLauncherIdx, void *pParam, int nMsgSize)
{
    C2S_REQ_UNIT_INFO   * pReq = (C2S_REQ_UNIT_INFO *)pParam;

	pReq->szOwnerName[MAXSIZE_ROLENAME - 1] = 0;

	int	nTargetIdx = g_PlayerInfoToIndex.GetIndexByName(pReq->szOwnerName);
	
	if(!IsValidPlayer(nTargetIdx) || Player[nTargetIdx].GetCamoflag()) //蒙面状态不可查询
		return ;

	SocialUnit * pLeafUnit = GetLeafUnit(nTargetIdx,enSUTplId_Tong);
	if (pLeafUnit == NULL)
		return ;

	SocialUnit * pUnit = GetUpNUnit(pLeafUnit,pReq->nLayer);
	if (pUnit && pUnit->GetLayer() >= enSULayer_Gens && pUnit->GetLayer() < enSUTong_LayerNum)
	{
	    S2C_UNIT_INFO_RET ret;
		ret.comHeader = pReq->comHeader;
		ret.comHeader.proHeader.protocol = s2c_social_family;
	    ret.comHeader.proHeader.len = sizeof(ret) - PROTOCOL_SIZE;

		const char * szUnitName  = GetUnitName(pUnit->GetUnitAttr());
		if (szUnitName)
			strncpy(ret.szUnitName,szUnitName,sizeof(ret.szUnitName));

		ret.szUnitName[MAXSIZE_ORGNAME - 1] = 0;

		const char * szOwnerName = pUnit->GetOwnerName();
		if (szOwnerName)
			strncpy(ret.szOwnerName,szOwnerName,sizeof(ret.szOwnerName));

		ret.szOwnerName[MAXSIZE_ROLENAME - 1] = 0;

		ret.nLayer          = pUnit->GetLayer();
		ret.nSubUnitNum     = pUnit->GetChildCount();

		ret.nPlayerAvgLevel = pUnit->GetPlayerAvgLevel();

		ret.nCityMapID      = INVALID_WORLD_ID;
		ret.nPoolMapID      = INVALID_WORLD_ID;

		int nCityMapID      = GetCityMapId(pUnit->GetUnitAttr());
        if (nCityMapID     != INVALID_WORLD_ID)
			ret.nCityMapID  = nCityMapID;

		//This reserve for start pool used

		int nPoolMapID      = GetPoolMapId(pUnit->GetUnitAttr());
		if (nPoolMapID     != INVALID_WORLD_ID)
			ret.nPoolMapID  = nPoolMapID;

		ret.code            = pReq->code;
		
		SendDataToClient(nLauncherIdx, &ret, ret.comHeader.proHeader.len + PROTOCOL_SIZE);
	}//endif

}

void ServerSocialUnitMgr::SetCityResReq(int nLauncherIdx, void *pParam, int nMsgSize)
{
	//闭掉
	return ;

	C2S_CITY_RES_REQ	*pCityResReq = (C2S_CITY_RES_REQ*)pParam;
	RelationSet			&relationSet = GetRelationSet(nLauncherIdx);

	if( !relationSet.IsOwnTreeLoad(enSUTplId_Tong) )
	{
		SocialErrCode2Client(nLauncherIdx, pCityResReq->comHeader.comParam, enSocialErr_RetryLater);
		return;
	}

	int	nCityMapIdx = GetCityMapIdx(nLauncherIdx);
	if(INVALID_WORLD_INDEX == nCityMapIdx)
		return;

	int	nPlayerNpcIdx = Player[nLauncherIdx].m_nIndex;
	if(!IsValidNpc(nPlayerNpcIdx) || Npc[nPlayerNpcIdx].GetSubWorldIndex() != nCityMapIdx )
		return;

	int	nWorldLordNpcIdx = SubWorld[nCityMapIdx].GetLord();
	if(!IsValidNpc(nWorldLordNpcIdx))
		return;

	for(int nCityRes = 0; nCityRes < CITY_RES_TYPE_COUNT; ++nCityRes)
	{
		enNpc_UnaryAttr_Idx	enResIdx = (enNpc_UnaryAttr_Idx)(nuai_lord_res0 + nCityRes);
		_ASSERT(enResIdx < nuai_end);
		int	nOldPlayerVal = Npc[nPlayerNpcIdx].m_UnaryAttrMgr[enResIdx];

		if(pCityResReq->arrayCityRes[nCityRes] > 0 && 
			pCityResReq->arrayCityRes[nCityRes] <= nOldPlayerVal)
		{
			Npc[nPlayerNpcIdx].AddUnaryAttr(enResIdx, -pCityResReq->arrayCityRes[nCityRes]);
			Npc[nWorldLordNpcIdx].AddUnaryAttr(enResIdx, pCityResReq->arrayCityRes[nCityRes]);
		}
	}

	Npc[nWorldLordNpcIdx].SetDataChangedFlag(true);
}

void ServerSocialUnitMgr::GetCityResReq(int nLauncherIdx, void *pParam, int nMsgSize)
{
	//闭掉
	return ;

	C2S_CITY_RES_REQ	*pCityResReq = (C2S_CITY_RES_REQ*)pParam;
	RelationSet			&relationSet = GetRelationSet(nLauncherIdx);

	if( !relationSet.IsOwnTreeLoad(enSUTplId_Tong) )
	{
		SocialErrCode2Client(nLauncherIdx, pCityResReq->comHeader.comParam, enSocialErr_RetryLater);
		return;
	}

	SocialUnit	*pTongUnit = GetCityUnit(nLauncherIdx);
	if(NULL == pTongUnit)
		return;

	if( !pTongUnit->IsOwner( GetPlayerName(nLauncherIdx) ) )
		return;

	int	nCityMapId = GetCityMapId( pTongUnit->GetUnitAttr() );
	if(INVALID_WORLD_ID == nCityMapId)
		return;

	int	nCityMapIdx = g_SubWorldSet.SearchWorld(nCityMapId);
	if(INVALID_WORLD_INDEX == nCityMapIdx)
		return;

	// 只允许在城市地图上进行资源操作
	int	nPlayerNpcIdx = Player[nLauncherIdx].m_nIndex;
	if( !IsValidNpc(nPlayerNpcIdx) || Npc[nPlayerNpcIdx].GetSubWorldIndex() != nCityMapIdx )
		return;

	int	nWorldLordNpcIdx = SubWorld[nCityMapIdx].GetLord();
	if(!IsValidNpc(nWorldLordNpcIdx))
		return;

//	for(int nCityRes = 0; nCityRes < CITY_RES_TYPE_COUNT; ++nCityRes)
	int nCityRes = 0;
	enNpc_UnaryAttr_Idx	enResIdx = (enNpc_UnaryAttr_Idx)(nuai_lord_res0 + nCityRes);
	_ASSERT(enResIdx < nuai_end);
	int	nOldLordVal = Npc[nWorldLordNpcIdx].m_UnaryAttrMgr[enResIdx];
	
	if(pCityResReq->arrayCityRes[nCityRes] > 0 
		&& pCityResReq->arrayCityRes[nCityRes] <= nOldLordVal)
	{
		Npc[nWorldLordNpcIdx].AddUnaryAttr(enResIdx, -pCityResReq->arrayCityRes[nCityRes]);

		if (Player[nLauncherIdx].GetItemList().AddMoney(room_equipment, pCityResReq->arrayCityRes[nCityRes]))
		{
			PlayerSet.AddMoney((DWORD)pCityResReq->arrayCityRes[nCityRes]);

			int cityRes = pCityResReq->arrayCityRes[nCityRes];
			if (cityRes >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
			{
				LogEventParam logParam;
				logParam.event = log_event_get_city_res_add_money;
				logParam.param1 = Player[nLauncherIdx].GetGUID();
				logParam.param4 = cityRes;
				g_pLogSystem->Log(logParam);
			}
		}
	}
	Npc[nWorldLordNpcIdx].SetDataChangedFlag(true);
}

void ServerSocialUnitMgr::GetCityInfoReq(int nLauncherIdx, void *pParam, int nMsgSize)
{
	C2S_CITY_INFO_REQ	*pc2sReq = (C2S_CITY_INFO_REQ*)pParam;
	RelationSet			&relationSet = GetRelationSet(nLauncherIdx);

	if( !relationSet.IsOwnTreeLoad(enSUTplId_Tong) )
	{
		SocialErrCode2Client(nLauncherIdx, pc2sReq->comHeader.comParam, enSocialErr_RetryLater);
		return;
	}	

	SocialUnit *pTongUnit = GetCityUnit(nLauncherIdx);
	if(NULL == pTongUnit)
		return;

	int	nCityMapIdx = GetCityMapIdx(nLauncherIdx);
	if(INVALID_WORLD_INDEX == nCityMapIdx)
		return;

	int	nWorldLordNpcIdx = SubWorld[nCityMapIdx].GetLord();
	if(!IsValidNpc(nWorldLordNpcIdx))
		return;

	S2C_SOCIAL_CITY_INFO	s2cRet;
	s2cRet.comHeader = pc2sReq->comHeader;
	s2cRet.comHeader.proHeader.protocol = s2c_social_family;
	s2cRet.comHeader.proHeader.len = sizeof(s2cRet) - PROTOCOL_SIZE;
	
	s2cRet.nInfoType    = pc2sReq->nType;
	s2cRet.nCityLordId  = Npc[nWorldLordNpcIdx].m_dwID;
	s2cRet.nCityMapId   = SubWorld[nCityMapIdx].m_SubWorldID;
	s2cRet.nShizuCount  = pTongUnit->GetChildCount();
	s2cRet.nTongMemberCount = pTongUnit->GetTotalPlayerNum();

	if (pTongUnit->GetOwnerName())
		strncpy(s2cRet.szOwnerName, pTongUnit->GetOwnerName(), sizeof(s2cRet.szOwnerName));
	
	for(int nCityRes = 0; nCityRes < CITY_RES_TYPE_COUNT; ++nCityRes)
	{
		_ASSERT(nuai_lord_res0 + nCityRes < nuai_end);
	
		s2cRet.arrayCityRes[nCityRes] = Npc[nWorldLordNpcIdx].m_UnaryAttrMgr[nuai_lord_res0 + nCityRes];
	}
	
	SendDataToClient(nLauncherIdx, &s2cRet, s2cRet.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void ServerSocialUnitMgr::PlayerOnLine(int nPlayerIdx)
{
	if (!IsValidPlayer(nPlayerIdx))
		return;

	RelationSet	&relationSet = GetRelationSet(nPlayerIdx);

	for(int nTplId = enSUTplId_None + 1; nTplId < enSUTplId_Num; ++nTplId)
	{
		RelationRecord *pRec = relationSet.GetRelationByTemplate(nTplId);

		if(NULL == pRec) //数据库表明没有社会关系
		{
			SocialUnit	*pLeafUnit = GetUnit( GetPlayerGuid(nPlayerIdx), nTplId );

			if (pLeafUnit && nTplId == enSUTplId_Tong)
			{
				//数据库数据有问题
				_ASSERT(false);

				SocialUnitAttr &newUnitAttr = pLeafUnit->GetUnitAttr();
				RefreshPlayerInfoAttr(newUnitAttr, nPlayerIdx, true);
				
				RelationSet &relationSet = GetRelationSet(nPlayerIdx);
				relationSet.Add(pLeafUnit);

			}//endif
			else
			{
				CreatePlayerUnit(nPlayerIdx, nTplId);
				relationSet.SetOwnTreeLoadFlag(nTplId, true);
			}//end else
		}
		else
		{
			//数据库表明有社会关系
			SocialUnit	*pLeafUnit = GetUnit( GetPlayerGuid(nPlayerIdx), nTplId );

			if(pLeafUnit) //已经被读取
			{
				// 把这一行放在最前面
				pRec->pLeafUnit = pLeafUnit;
				relationSet.SetOwnTreeLoadFlag(nTplId, true);
				pLeafUnit->PlayerOnLine(nPlayerIdx, true);

				// 如果玩家所属的氏族(或其他层次节点)已经加载进来，
				// 但是随后解散了，这里给出通知
				if( IsGUIDValid(pRec->ParentGuid) && NULL == pLeafUnit->GetParent() )
				{
					memset(&pRec->ParentGuid.data, 0, sizeof(pRec->ParentGuid.data));
					NotifyUnitBreak(nPlayerIdx, nTplId, pLeafUnit->GetLayer() + 1);

					int nLeaveBuff = GetLeaveBuff(nTplId, pLeafUnit->GetLayer() + 1);
					AddBuffToPlayer(nPlayerIdx, nLeaveBuff);
				}
			}
			else			
			{
				//没有被读取
				_ASSERT( !relationSet.IsOwnTreeLoad(nTplId) );
				SocialSerializer::Singleton().LoadTreeUpReq(GetNetConnectIdx(nPlayerIdx), 
															pRec->TplId, 
															pRec->ParentGuid
															);
			}
		}
	}
}

void ServerSocialUnitMgr::PlayerOffLine(int nPlayerIdx)
{
	if(!IsValidPlayer(nPlayerIdx))
		return ;

	RelationSet	&relationSet = GetRelationSet(nPlayerIdx);

	for(int nTplId = enSUTplId_None + 1; nTplId < enSUTplId_Num; ++nTplId)
	{
		SocialUnit	*pLeafUnit = GetLeafUnit(nPlayerIdx, nTplId);

        //_ASSERT(pLeafUnit);

		if(pLeafUnit)//Notice:存在这样的情况，Player上线时，它的OwnTree还没有Load完，这时pLeafUnit 为空.
		{
			// 对于没有加入关系树的节点，直接删掉
			// 对于加入了关系树的节点，调用正常下线流程
			if( NULL == pLeafUnit->GetParent() )
				RemoveUnit( pLeafUnit->GetUnitGuid(), pLeafUnit->GetTplId() );
			else
			{
				pLeafUnit->PlayerOnLine(nPlayerIdx, false);
			}//end else

		}
	}

	
}

void ServerSocialUnitMgr::CreatePlayerUnit(int nPlayerIdx, int nTplId)
{
	if (!IsValidPlayer(nPlayerIdx))
		return ;

	SocialUnit *pNewUnit =  SocialAllocator::AllocUnit(nTplId, enSULayer_Player);

	_ASSERT(pNewUnit);

	if(NULL == pNewUnit)
		return;

	pNewUnit->SetUnitGuid( GetPlayerGuid(nPlayerIdx) );
	pNewUnit->SetOwnerName( GetPlayerName(nPlayerIdx) );

	SocialUnitAttr &newUnitAttr = pNewUnit->GetUnitAttr();
	RefreshPlayerInfoAttr(newUnitAttr, nPlayerIdx, true);

	RelationSet &relationSet = GetRelationSet(nPlayerIdx);
	relationSet.Add(pNewUnit);

	AddUnit(pNewUnit->GetUnitGuid(), pNewUnit);
}

void ServerSocialUnitMgr::LoadOwnTreeUnitRet(int nDbOpeRst, int nPlayerIdx, _SocialDBHeader* pHeader, IProcRet* pRet )
{
	if(!nDbOpeRst)
		return;

	int		nTplId = pHeader->ntplId;

	SocialSerializer::Singleton().RemoveTask(pHeader->Guid);

	_ASSERT( IsTplIdValid(nTplId) );
	if( !IsTplIdValid(nTplId) )
		return;

	RelationSet		&relationSet = GetRelationSet(nPlayerIdx);
	RelationRecord	*pRecord = relationSet.GetRelationByTemplate(nTplId);

	// 如果pRecord为空，是不可能进入到这个函数中的
	_ASSERT(pRecord);
	if(NULL == pRecord)
	{
		relationSet.SetOwnTreeLoadFlag(nTplId, true);

        #ifdef _DEBUG
		CFS_FILELOGS::WriteDebugLog(LOGMSG_SOCIAL_LOADOWNTREEERR, 
			GetPlayerName(nPlayerIdx), 
			nTplId,
			enSULayer_Player
			);
        #endif

		return;
	}

	int	nRow = pRet->GetRowCount( );

	if(0 == nRow)
	{
		// 没有记录说明氏族解散了
		relationSet.Remove(nTplId);
		relationSet.SetOwnTreeLoadFlag(nTplId, true);
		CreatePlayerUnit(nPlayerIdx, nTplId);
		NotifyUnitBreak(nPlayerIdx, nTplId, enSULayer_Player + 1);

		int nLeaveBuff = GetLeaveBuff(nTplId, enSULayer_Player + 1);
		AddBuffToPlayer(nPlayerIdx, nLeaveBuff);

		return;
	}

	SocialUnit	*pPreConsUnit = NULL;

	for(int nLoop = 0; nLoop < nRow; ++nLoop)
	{
		bool	bIsNew = true;
		SocialUnit *pUnit = ConstructUnit(nPlayerIdx, pRet, nLoop, bIsNew);	

		_ASSERT(pUnit);
		if(NULL == pUnit) //数据库错误
			break;

		// 返回的纪录必须按照层次从小到大排序，第一条记录的层次一定
		// 是enSULayer_Player + 1, 否则会出问题
		if(  ( 0 == nLoop && enSULayer_Player + 1 != pUnit->GetLayer() ) 
			|| ( pPreConsUnit && pPreConsUnit->GetLayer() + 1 != pUnit->GetLayer() )
			)
		{
			_ASSERT(false);
			break;
		}//endif

		if(enSULayer_Player + 1 == pUnit->GetLayer())
		{
			// 加此判断的原因请参考本函数末尾的注释
			if( !pUnit->IsAllSubUnitLoad() )
			{
				char	*pColData;
				int		nColDatasize;

				nColDatasize = pRet->GetData(nLoop, enSocDBRec_Col_AppendData, &pColData );
				_ASSERT(nColDatasize > 0);
				ParseUnitBloc(nPlayerIdx, pColData, nColDatasize, pUnit);
				pUnit->SetAllSubUnitLoadFlag(true);
				pUnit->RecountSubUnitNum();
			}
		}

		if (pUnit->GetLayer() == enSULayer_Tong && m_IsMapPoolLoadedReady && bIsNew)
		{
            CheckPoolAttr(pUnit);
		}//endif

		if(pPreConsUnit)
		{
			pPreConsUnit->SetParent(pUnit);
			pUnit->AddSubUnit(pPreConsUnit);
		}

		pPreConsUnit = pUnit;

		// 如果节点已经载入内存，则其父节点也应该载入内存了
		// 余下的不用再解析了
		if(bIsNew)
			AddUnit(pUnit->GetUnitGuid(), pUnit);
		else
			break;
	}
	
	// 如果开除某个不在线的玩家，从社会关系数据中会将其删除
	// 但player数据库中，玩家的父节点GUID并不能更新，仍然有效，
	// 当这个玩家上线时，会按正常的顺序load自己的那棵树，此时有
	// 两种情况: 1. 此树已经load进来了 2. 此树没有load进来
	// 对于第一种情况，上面的循环执行1次就会break，而且不会
	// parse氏族的子节点(因为已经全部parse了)，对于第二种情况
	// 上面的加载过程会正常进行，玩家以前所属的关系属会完整的加载
	// 进来。如果在这里找不到玩家的叶节点，就表示玩家被踢下线了
	// 这里清除其相关信息，然后重新创建一个节点，如果找到了叶节点
	// 表示是正常玩家，执行后面的正常流程
	if( NULL == GetUnit(GetPlayerGuid(nPlayerIdx), nTplId) )
	{
		relationSet.Remove(nTplId);
		relationSet.SetOwnTreeLoadFlag(nTplId, true);
		CreatePlayerUnit(nPlayerIdx, nTplId);
		NotifyBeRemovedOffLine(nPlayerIdx, nTplId, enSULayer_Player + 1, MSG_SOCIAL_BEREMOVED_OFFLINE);

		int nLeaveBuff = GetLeaveBuff(nTplId, enSULayer_Player + 1);
		AddBuffToPlayer(nPlayerIdx, nLeaveBuff);
	}
	else
	{
		OnLoadOwnTreeFinished(pRecord->ParentGuid, pRecord->TplId);
	}//end else
}


SocialUnit*	ServerSocialUnitMgr::ConstructUnit(
		int nPlayerIdx, 
		IProcRet* pRet,
		int nRow, 
		bool &bIsNew)
{
	_ASSERT( enSocDBRec_Col_Num == pRet->GetColCount() );

	if( enSocDBRec_Col_Num != pRet->GetColCount() )
		return NULL;

	char	*pColData;
	int		nColDataSize;
	bool	bRet = false;

	int		nTplId;
	pRet->GetData(nRow, enSocDBRec_Col_TplId, nTplId );

	int		nLayer;
	pRet->GetData(nRow, enSocDBRec_Col_Layer, nLayer );

	bRet = IsLayerValid(nTplId, nLayer);
	_ASSERT(bRet);
	if( !bRet )
		return NULL;

	FSGUID	unitGuid;
	pRet->GetData(nRow, enSocDBRec_Col_UnitGuid, unitGuid.data, sizeof(unitGuid) );
	
	// 避免数据不完整时重复加载
	SocialUnit	*pOldUnit = GetUnit(unitGuid, nTplId);
	if( pOldUnit )
	{
		bIsNew = false;
		return pOldUnit;
	}

	int	nSubUnitCnt;
	pRet->GetData(nRow, enSocDBRec_Col_SubUnitCnt, nSubUnitCnt );

	if(nSubUnitCnt < GetMinSubUnitCnt(nTplId, nLayer) || nSubUnitCnt > GetMaxSubUnitCnt(nTplId, nLayer) )
	{
		_ASSERT(false);
		return NULL;
	}

	char	szOwnerName[MAXSIZE_ROLENAME];
	pRet->GetData(nRow, enSocDBRec_Col_OwnerName, szOwnerName, sizeof(szOwnerName) );
	szOwnerName[MAXSIZE_ROLENAME - 1] = 0;

	SocialUnit	*pUnit = SocialAllocator::AllocUnit(nTplId, nLayer);

	_ASSERT(pUnit);
	if(NULL == pUnit)
		return NULL;

	pUnit->SetSubUnitCnt(nSubUnitCnt);
	pUnit->SetOwnerName(szOwnerName);
	pUnit->SetUnitGuid(unitGuid);

	SocialUnitAttr	&attr = pUnit->GetUnitAttr();
	nColDataSize = pRet->GetData(nRow, enSocDBRec_Col_UnitAttr, &pColData );

	bRet = attr.LoadAttrFromBuf(pColData, nColDataSize);
	_ASSERT(bRet);

	bRet = attr.RecheckAttrVersion(nTplId,nLayer);
	_ASSERT(bRet);

	// 这个需要放在最后调用
	if(attr.IsAttrExist(enSUAttr_ChatChannel) )
	{
		bRet = pUnit->CreateChannel(nPlayerIdx);
		_ASSERT(bRet);
	}

	return pUnit;
}

void ServerSocialUnitMgr::ParseUnitBloc(int nPlayerIdx, char *pBloc, int nBlocSize, SocialUnit *pParent)
{
	int		nUsedSize = 0;
	char	*pReadPos = pBloc;	

	while(nUsedSize < nBlocSize)
	{
		DB_UNIT_DATA	*pUnitData = (DB_UNIT_DATA*)(pReadPos + nUsedSize);

		nUsedSize += pUnitData->size();

		_ASSERT( IsLayerValid(pUnitData->tplId, pUnitData->layer) );
		if( !IsLayerValid(pUnitData->tplId, pUnitData->layer) )
			continue;

		// 目前来说，这个函数解析的只可能是玩家节点，但流程通用
		if (enSULayer_Player != pUnitData->layer)
			continue;

		if (GetUnit(pUnitData->unitGuid)!=NULL)  //数据库存取有问题
		{
			_ASSERT(false);
            continue;
		}//endif

		SocialUnit	*pUnit = SocialAllocator::AllocUnit(pUnitData->tplId, pUnitData->layer);

		_ASSERT(pUnit);
		if(NULL == pUnit)
			continue;

		pUnit->SetOwnerName(pUnitData->ownerName);
		pUnit->SetUnitGuid(pUnitData->unitGuid);
		pUnit->ChgLeafUnitMaxJoinedLayer(pUnitData->maxJoinedLayer);
		
		bool	bSucceed;

		SocialUnitAttr	&attr = pUnit->GetUnitAttr();
		bSucceed = attr.LoadAttrFromBuf(pUnitData->data, pUnitData->attrSize);
		_ASSERT(bSucceed);

		bSucceed = attr.RecheckAttrVersion(pUnitData->tplId,pUnitData->layer);
		_ASSERT(bSucceed);
		
#ifdef _DEBUG
		CFS_FILELOGS::WriteDebugLog("Load Privilege: %s\n", pUnitData->ownerName);
#endif

		PrivilegeSet	&priv = pUnit->GetPrivilegeSet();
		bSucceed = priv.Load(pUnitData->data + pUnitData->attrSize, pUnitData->privSize);
		_ASSERT(bSucceed);

		pUnit->SetParent(pParent);
		pParent->AddSubUnit(pUnit);

		AddUnit(pUnit->GetUnitGuid(), pUnit);

		bSucceed = CheckLeafUnitPrivVerion(pUnit);
		_ASSERT(bSucceed);

		if( attr.IsAttrExist(enSUAttr_ChatChannel) )
		{
			bSucceed = pUnit->CreateChannel(nPlayerIdx);
			_ASSERT(bSucceed);
		}
	}
}

void ServerSocialUnitMgr::LoadAllSubUnitRet(int nDbOpeRst, int nPlayerIdx, _SocialDBHeader* pHeader, IProcRet* pRet)
{
	if(!nDbOpeRst)
		return;
	
	int			nTplId = pHeader->ntplId;
	SocialSerializer::Singleton().RemoveTask(pHeader->Guid);
	SocialUnit	*pParent = GetUnit(pHeader->Guid, nTplId);

	_ASSERT(pParent);
	if(NULL == pParent)
		return;

	if( pParent->IsAllSubUnitLoad() )
	{
		_ASSERT(false);
		return;
	}

	pParent->SetAllSubUnitLoadFlag(true);

	// 因为氏族节点和玩家节点是当作一条记录存储的，如所load
	// 氏族的子节点，实际上load的是氏族的节点
	if( enSULayer_Player + 1 == pParent->GetLayer() )
	{
		_ASSERT(1 == pRet->GetRowCount());

		if( !pParent->IsAllSubUnitLoad() )
		{
			char	*pColData;
			int		nColDatasize;
			
			nColDatasize = pRet->GetData(0, enSocDBRec_Col_AppendData, &pColData );
			_ASSERT(nColDatasize > 0);
			ParseUnitBloc(nPlayerIdx, pColData, nColDatasize, pParent);	
			//修正子节点个数,数据库信息可能不准确
			pParent->RecountSubUnitNum();
		}//endif

		return;
	}

	int		nRowCnt = pRet->GetRowCount();
	FSGUID	unitGuid;

	for(int nRow = 0; nRow < nRowCnt; ++nRow)
	{
		pRet->GetData(nRow, enSocDBRec_Col_UnitGuid, unitGuid.data, sizeof(unitGuid));

		int		nLayer;
	    pRet->GetData(nRow, enSocDBRec_Col_Layer, nLayer );

		if( GetUnit(unitGuid, nTplId) ) //This mean that bIsNew 
			continue;

		if ( nLayer + 1 != pParent->GetLayer())
			continue;

		bool	bIsNew = true;
		SocialUnit	*pUnit = ConstructUnit(nPlayerIdx, pRet, nRow, bIsNew);

		_ASSERT(pUnit && bIsNew);

		if(pUnit)
		{
			pUnit->SetParent(pParent);
			pParent->AddSubUnit(pUnit);
			AddUnit(pUnit->GetUnitGuid(), pUnit);

			if(enSULayer_Player + 1 == pUnit->GetLayer())
			{
				if( !pUnit->IsAllSubUnitLoad() )
				{
					char	*pColData;
					int		nColDatasize;
					
					nColDatasize = pRet->GetData(nRow, enSocDBRec_Col_AppendData, &pColData );
					_ASSERT(nColDatasize > 0);
					ParseUnitBloc(nPlayerIdx, pColData, nColDatasize, pUnit);
					pUnit->SetAllSubUnitLoadFlag(true);
					pUnit->RecountSubUnitNum();
				}//endif
				
			}//endif

			if (pUnit->GetLayer() == enSULayer_Tong && m_IsMapPoolLoadedReady)
			{
				CheckPoolAttr(pUnit);
			}//endif

		}//endif
	}
	pParent->RecountSubUnitNum();
}

void ServerSocialUnitMgr::CheckPoolAttr()
{
	GUID2SOCIALUNIT::iterator it = m_guid2SocialUnit[enSUTplId_Tong].begin();
    while (it!=m_guid2SocialUnit[enSUTplId_Tong].end())
	{
		if (it->second && it->second->GetLayer() == enSULayer_Tong)
		{
            CheckPoolAttr(it->second);
		}//endif
		
		++it;
	}//end for while

}

void ServerSocialUnitMgr::CheckPoolAttr(SocialUnit * pUnit)
{
	_ASSERT(pUnit && pUnit->GetLayer()==enSULayer_Tong);

	if (pUnit == NULL)
		return;
	
	int iMapID  = GetPoolMapId(pUnit->GetUnitAttr());
	if (iMapID == INVALID_WORLD_ID)
		return;
	
	_ASSERT(GetGlobalPoolCombatMgr().IsPoolCombatMap(iMapID));
	if (GetGlobalPoolCombatMgr().IsPoolCombatMap(iMapID))
	{
		int iWorldIndex  = g_SubWorldSet.SearchWorld(iMapID);
		if (iWorldIndex != INVALID_WORLD_INDEX)
		{
			int nPoolNpc = SubWorld[iWorldIndex].GetPool();
			if (IsValidNpc(nPoolNpc))
			{
				if (Npc[nPoolNpc].GetLord()==pUnit->GetUnitGuid())
				{
					return;
				}//endif
				
			}//endif	
			
		}//endif
		
	}//endif

	//invalid pool id .
	_ASSERT(false);
	SocialUnitAttr   & attr = pUnit->GetUnitAttr();
	attr.DelAttr(enSUAttr_PoolMap);
	
	SocialSerializer & ss   = SocialSerializer::Singleton();
	ss.UpdateAttrReq(-1,pUnit);
	
	//Log to DB
	char szLogString[MAX_INVALID_INFO_LEN];
	char szUnitGUID[34];
	
	memcpy(szUnitGUID,pUnit->GetUnitGuid().data,33);
	szUnitGUID[33]=0;
	
	snprintf(szLogString,sizeof(szLogString),"Invalid Pool Attr,UnitGUID:%s MapId:%d",szUnitGUID,iMapID);
	szLogString[MAX_INVALID_INFO_LEN -1] = 0;
	
	int nSize      =strlen(szLogString);
	
	g_pLogSystem->SysDbgLog(szLogString,nSize,sys_dbg_log_event_pool_combat);
	
}

void ServerSocialUnitMgr::OnMapPoolLoadReady()
{
	m_IsMapPoolLoadedReady = true;
	CheckPoolAttr();
}

void ServerSocialUnitMgr::ProcessGlobalDBRet(int nDbOpeRst, IProcRet* pRet )
{
	if (!pRet)
		return ;

    int   nPassBySize = 0;
	char* pPassBy     = pRet->GetPassBy( nPassBySize );
	
	if( nPassBySize != sizeof(_SocialDBHeader) || 
		pPassBy == NULL )
		return;
	
	_SocialDBHeader* pHeader = (_SocialDBHeader*)pPassBy;
	
	int uDBOpeType = pHeader->nOp;
	
	_ASSERT(uDBOpeType > enSoc_DBOpe_Begin && uDBOpeType < enSoc_DBOpe_LastUsed);
	
	switch (uDBOpeType)
	{
	case enSoc_DBOpe_LoadOwnTree:
		{
            if (nDbOpeRst && pRet->GetRowCount( ))
			{
				int		nTplId = pHeader->ntplId;	
				SocialSerializer::Singleton().RemoveTask(pHeader->Guid);
				
				_ASSERT( IsTplIdValid(nTplId) );
				if( !IsTplIdValid(nTplId) )
					return;
				
				int	nRow = pRet->GetRowCount( );
				
				SocialUnit	*pPreConsUnit = NULL;
				
				for(int nLoop = 0; nLoop < nRow; ++ nLoop)
				{
					bool	bIsNew = true;
					SocialUnit *pUnit = ConstructUnit( -1 , pRet, nLoop, bIsNew);	
					
					_ASSERT(pUnit);
					if(NULL == pUnit) //数据库错误
						break;
					
					// 返回的纪录必须按照层次从小到大排序，第一条记录的层次一定
					// 是enSULayer_Player + 1, 否则会出问题
					if(  pPreConsUnit && pPreConsUnit->GetLayer() + 1 != pUnit->GetLayer() )
					{
						_ASSERT(false);
						break;
					}//endif
					
					if(  enSULayer_Tong > pUnit->GetLayer() )  // Tong And Pool Used Only Notice!!
					{
						_ASSERT(false);
						break;
					}//endif
					
					if(pPreConsUnit)
					{
						pPreConsUnit->SetParent(pUnit);
						pUnit->AddSubUnit(pPreConsUnit);
					}
					
					pPreConsUnit = pUnit;
					
					// 如果节点已经载入内存，则其父节点也应该载入内存了
					// 余下的不用再解析了
					if(bIsNew)
						AddUnit(pUnit->GetUnitGuid(), pUnit);
					else
						break;
				}//end for nLoop
				  
			}//endif

			GetGlobalTongWarMgr().OnTongLoaded(pHeader->Guid); 
			GetGlobalPoolCombatMgr().OnTongLoaded(pHeader->Guid);
			
		}//end for case
		break;
	default:break;	
	}//end switch
}

void ServerSocialUnitMgr::OnLoadOwnTreeFinished(const FSGUID &parentGuid, int nTplId)
{
	SocialUnit*	pParent = GetUnit(parentGuid, nTplId);

	if(NULL == pParent)
	{
		_ASSERT(pParent);

#ifdef _DEBUG
		CFS_FILELOGS::WriteDebugLog(LOGMSG_SOCIAL_LOADUNITERR, 
			parentGuid.data, 
			nTplId
			);
#endif

		return;
	}

	SocialUnit::UnitIterator iter;
	SocialUnit				 *pLeafUnit;

	// 有可能在该玩家请求Load数据的过程中，同一氏族的其他玩家
	// 也上线了，但是其它玩家请求暂时被搁置，这里来通知那些
	// 玩家，数据已经准备好了
	while( pLeafUnit = pParent->NextSubUnit(iter) )			
	{
		int	nPlayer = g_PlayerInfoToIndex.GetIndexByName( pLeafUnit->GetOwnerName() );

		if(!IsValidPlayer(nPlayer))
			continue;

		RelationSet		&rset = GetRelationSet(nPlayer);
		rset.SetOwnTreeLoadFlag(nTplId, true);

		RelationRecord	*pRec = rset.GetRelationByTemplate(nTplId);		

		// 正常情况下，pRec已经在玩家数据加载的时候add进来了
		_ASSERT(pRec);

		if(pRec)
			pRec->pLeafUnit = pLeafUnit;
		else
			rset.Add(pLeafUnit);

		pLeafUnit->PlayerOnLine(nPlayer, true);
	}
}

void ServerSocialUnitMgr::DBAddUnitRet(int nDbOpeRst, int nPlayerIdx, _SocialDBHeader* pHeader, IProcRet* pRet )
{
	if(!nDbOpeRst)
	{
#ifdef _DEBUG
		CFS_FILELOGS::WriteDebugLog("DB Add Unit failed!");
#endif
	}
}

void ServerSocialUnitMgr::CheckUnitNameRet(int nDbOpeRst, int nPlayerIdx, _SocialDBHeader* pHeader, IProcRet* pRet )
{
	const C2S_CREATEUNIT_REQ *pc2sReq = &pHeader->Req;

	if(!nDbOpeRst)
	{
		SocialErrCode2Client(nPlayerIdx, pc2sReq->comHeader.comParam, enSocialErr_CreateFailed);
		return;
	}

	_ASSERT(1 == pRet->GetRowCount() && 1 == pRet->GetColCount());

	int		nRecCount;
	pRet->GetData(0, 0, nRecCount );

	_ASSERT(1 == nRecCount || 0 == nRecCount);

	if(0 != nRecCount)
		SocialErrCode2Client(nPlayerIdx, pc2sReq->comHeader.comParam, enSocialErr_NameAlreadyExist);
	else
		UnitOperationReq(nPlayerIdx, (void*)pc2sReq, sizeof(C2S_CREATEUNIT_REQ));
	
}

int	ServerSocialUnitMgr::GetCityMapIdx(int nPlayerIdx)
{
	SocialUnit *pTongUnit = GetCityUnit(nPlayerIdx);

	if(NULL == pTongUnit)
		return INVALID_WORLD_INDEX;

	int	nCityMapId = GetCityMapId( pTongUnit->GetUnitAttr() );
	if(INVALID_WORLD_ID == nCityMapId)
		return INVALID_WORLD_INDEX;

	int	nCityMapIdx = g_SubWorldSet.SearchWorld(nCityMapId);
	
	return nCityMapIdx;
}

void ServerSocialUnitMgr::SaveUnitsRelToPlayer(int nPlayerIdx)
{
	if( !IsPlayerIdxValid(nPlayerIdx) )
		return;

	for(int nTplId = enSUTplId_None + 1; nTplId < enSUTplId_Num; ++nTplId)
	{
		if( IsSocialTmplSave(nTplId) )
		{
			SocialUnit *pUnit = GetLeafUnit(nPlayerIdx, nTplId);

			if(NULL == pUnit)
				continue;

			// 玩家节点不作为存盘单位，其父节点及以上才作为存盘单位
			SocialSerializer &ss = SocialSerializer::Singleton();
			pUnit = pUnit->GetParent();

			while(NULL != pUnit)
			{
				ss.UpdateRecordReq(GetNetConnectIdx(nPlayerIdx), pUnit);
				pUnit = pUnit->GetParent();
			}
		}
	}
}