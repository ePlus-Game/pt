//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:11   9:52
//      File_base        : ClientSocialUnitMgr
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
#include "ClientSocialUnitMgr.h"
#include "KSubWorldSet.h"
#include "buff_tab.h"
#include "QueryInfo.h"
#include "keconomysys.h"

ClientSocialUnitMgr::DETAILID2INFO	ClientSocialUnitMgr::m_detailId2Info;
ClientSocialUnitMgr::POPEFUNC		ClientSocialUnitMgr::m_opeFuncs[enSUO_Num];
ClientSocialUnitMgr::PNETRETPROC	ClientSocialUnitMgr::m_netRetProc[enSRProtocol_Num];

bool ClientSocialUnitMgr::Init()
{
	IUIMDL	*pMDLInterface = NULL;

	if( success_errorcode != GetMDLPtr(&pMDLInterface) )
		return false;

	IUIMDLDataset	*pDataSet = NULL;

	// create dataset for ui
	if( success_errorcode != pMDLInterface->queryDataSet(tong_dataset, &pDataSet) )
	{
		if( success_errorcode != pMDLInterface->createDataSet(tong_dataset) )
			return false;
	}

	// create operation for ui
	if( success_errorcode != pMDLInterface->queryDataSet(tong_operation, &pDataSet) )
	{
		if( success_errorcode != pMDLInterface->createDataSet(tong_operation) )
			return false;
	}

	// register my event handle to ui dataset
	if( success_errorcode != pMDLInterface->queryDataSet(tong_operation, &pDataSet) )	
		return false;
	pDataSet->setEventHandle(this);

		// create dataset for ui
	if( success_errorcode != pMDLInterface->queryDataSet(city_dataset, &pDataSet) )
	{
		if( success_errorcode != pMDLInterface->createDataSet(city_dataset) )
			return false;
	}

	// create operation for ui
	if( success_errorcode != pMDLInterface->queryDataSet(city_operation, &pDataSet) )
	{
		if( success_errorcode != pMDLInterface->createDataSet(city_operation) )
			return false;
	}

	// register my event handle to ui dataset
	if( success_errorcode != pMDLInterface->queryDataSet(city_operation, &pDataSet) )	
		return false;

	pDataSet->setEventHandle(this);

	memset(&m_UnConfirmReq, 0, sizeof(m_UnConfirmReq));

	BindOpe2Proc();
	BindNetRetProc();

	//Init the temp anoucement
	for (int i=0;i<MAX_ANOUNCEMENT_LAYER_NUM;i++)
	{
    	m_TempAnoucementRecord [i][0] = 0;
		m_TempAnoucementVersion[i]   = INVALID_ANOUNCEMENT_VERSION;
	}//end for i
		
	return true;	
}

void ClientSocialUnitMgr::BindOpe2Proc()
{
	memset(m_opeFuncs, 0, sizeof(m_opeFuncs));
	
	m_opeFuncs[enSUO_CreateUnit]	= &ClientSocialUnitMgr::CreateUnitReq;
	m_opeFuncs[enSUO_AddSubUnit]	= &ClientSocialUnitMgr::AddSubUnitReq;
	m_opeFuncs[enSUO_RemoveSubUnit] = &ClientSocialUnitMgr::RemoveSubUnitReq;
	m_opeFuncs[enSUO_BreakUnit]		= &ClientSocialUnitMgr::BreakUnitReq;
	m_opeFuncs[enSUO_LeaveUnit]		= &ClientSocialUnitMgr::LeaveUnitReq;	
	m_opeFuncs[enSUO_ForbidChat]	= &ClientSocialUnitMgr::ForbidChatReq;
	m_opeFuncs[enSUO_UnForbidChat]	= &ClientSocialUnitMgr::UnForbidChatReq;
	m_opeFuncs[enSUO_PubAnnouncement] = &ClientSocialUnitMgr::PubAnnouncementReq;
	m_opeFuncs[enSUO_GetSubList]	= &ClientSocialUnitMgr::GetSubListReq;
	m_opeFuncs[enSUO_GetPrePageSubList] = &ClientSocialUnitMgr::GetSubListReq;
	m_opeFuncs[enSUO_GetNextPageSubList] = &ClientSocialUnitMgr::GetSubListReq;
	m_opeFuncs[enSUO_GetAnnouncement]	= &ClientSocialUnitMgr::GetAnnouncementReq;
	m_opeFuncs[enSUO_ChangeOwner]       = &ClientSocialUnitMgr::ChangeUnitOwner;
	m_opeFuncs[enSUO_ReqJoinHigherLevel]= &ClientSocialUnitMgr::ReqJoinUnit;
	m_opeFuncs[enSUO_GetRecruitInfo]    = &ClientSocialUnitMgr::ReqRecruitInfo;
	m_opeFuncs[enSUO_AddRecruitInfo]    = &ClientSocialUnitMgr::ReqAddRecruit;
	m_opeFuncs[enSUO_DelRecruitInfo]    = &ClientSocialUnitMgr::ReqDelRecruit;
	m_opeFuncs[enSUO_ContributeCityRes] = &ClientSocialUnitMgr::SetCityRes;
	m_opeFuncs[enSUO_DistillCityRes] = &ClientSocialUnitMgr::GetCityRes;
	m_opeFuncs[enSUO_ReqCityInfo]    = &ClientSocialUnitMgr::ReqCityInfo;
	m_opeFuncs[enSUO_SetCityTaxRate] = &ClientSocialUnitMgr::SetCityTaxRate;
	
}

void ClientSocialUnitMgr::BindNetRetProc()
{
	memset(m_netRetProc, 0, sizeof(m_netRetProc));
	
	m_netRetProc[enSRProtocol_UnitOperation]  = &ClientSocialUnitMgr::s2cUnitOperation;
	m_netRetProc[enSRProtocol_ReqToConfirm]   = &ClientSocialUnitMgr::s2cReqToConfirm;
	m_netRetProc[enSRProtocol_MsgCode2Client] = &ClientSocialUnitMgr::s2cMsgCode;
	m_netRetProc[enSRProtocol_Msg2Client]     = &ClientSocialUnitMgr::s2cMsg;
	m_netRetProc[enSRProtocol_ReqCityInfo]    = &ClientSocialUnitMgr::GetCityInfoRet;
	m_netRetProc[enSRProtocol_GetUnitInfo]    = &ClientSocialUnitMgr::GetUnitInfoRet;
	m_netRetProc[enSRProtocol_GetRecruitInfo] = &ClientSocialUnitMgr::GetRecruitInfoRet;
	m_netRetProc[enSRProtocol_InfoBroadCast]  = &ClientSocialUnitMgr::BroadCastFromServer;
}

void ClientSocialUnitMgr::BroadCastFromServer(const void * ps2cBroadCast)
{
	S2C_SOCIAL_INFO_BROAD_CAST * pS2C = (S2C_SOCIAL_INFO_BROAD_CAST *)ps2cBroadCast;
	int                     nNpcIndex = NpcSet.SearchID(pS2C->nNpcID);

	if (IsValidNpc(nNpcIndex) && Npc[nNpcIndex].GetPlayerIdx() != CLIENT_PLAYER_INDEX)
	{
		Npc[nNpcIndex].SetShizuName(pS2C->szGensName);
		Npc[nNpcIndex].SetZhuhouName(pS2C->szTongName);
		Npc[nNpcIndex].SetGensMgr((bool)pS2C->isGensOwner);
		Npc[nNpcIndex].SetKing((bool)pS2C->isTongOwner);
		
		ConfigManager & cm = ConfigManager::Singleton();
		//HeadInfo Added
		if (Npc[nNpcIndex].IsKing())
		{
			const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_image_info, 0 );
			if (pTbuff)
				Npc[nNpcIndex].AddLayoutToHeadInfo(pTbuff,HEAD_INFO_TONG,KNpc::HIP_Important);
			
		}//endif
		else if (Npc[nNpcIndex].IsGensMgr())
		{
			const char* pTbuff = cm.GetConfigurableDisplayStyle( style_role_head_image_info, 1 );
			if (pTbuff)
				Npc[nNpcIndex].AddLayoutToHeadInfo(pTbuff,HEAD_INFO_GENS,KNpc::HIP_Important);
		}//end for if
		
	}//endif
}

void ClientSocialUnitMgr::GetUnitInfoRet(const void *ps2cRet)
{
	S2C_UNIT_INFO_RET * pInfo = ( S2C_UNIT_INFO_RET * ) ps2cRet;
    
	TongInfoData              data;
	ZeroMemory(&data,sizeof(data));

	strcpy(data.szUnitName,pInfo->szUnitName);
	strcpy(data.szOwnerName,pInfo->szOwnerName);
    
	data.nLayer               = pInfo->nLayer;
	data.nSubUnitNum          = pInfo->nSubUnitNum;
	data.nPlayerAvgLevel      = pInfo->nPlayerAvgLevel;

	data.flag                 = pInfo->code;

	if (pInfo->nLayer  >= enSULayer_Tong)
	{
		sprintf(data.nCityMapName,"----");
		sprintf(data.nPoolMapName,"----");
		
		CreateQueryManager(NULL);
		MapsInfo & mapinfo = MapsInfo::getSingleton();
		
		if ((int)pInfo->nCityMapID != INVALID_WORLD_ID)
		{
			MapsTab mapTab;
			mapinfo.GetMapInfo(pInfo->nCityMapID,mapTab);	
			if (mapTab.name.length())
				strcpy(data.nCityMapName,mapTab.name.c_str());      
		}//endif
		
		if ((int)pInfo->nPoolMapID != INVALID_WORLD_ID)
		{
			MapsTab mapTab;
			mapinfo.GetMapInfo(pInfo->nPoolMapID,mapTab);
			if (mapTab.name.length())
				strcpy(data.nPoolMapName,mapTab.name.c_str());        
		}//endif
	}

	CoreDataChanged(GDCNI_UPDATA_SOCIAL_INFO, (int)&data, 0);
}

void ClientSocialUnitMgr::GetRecruitInfoRet(const void * ps2cRet)
{
    S2C_GETRECRUIT_RET  * ret = (S2C_GETRECRUIT_RET  *)ps2cRet;
	//Decompression.......................................................
	int nOldSize = ret->comHeader.proHeader.len - sizeof(S2C_GETRECRUIT_RET) + 1 + PROTOCOL_SIZE;
	
	static const int         BUF_SIZE =  8192;
	unsigned char szCompressionBuff[BUF_SIZE];
    unsigned int nLen                 = BUF_SIZE;
	lzo1x_decompress(
		(const unsigned char *)ret->data,
		nOldSize,
		(unsigned char *)szCompressionBuff,
		&nLen,
		NULL);
	//Decompression end...................................................

    
	TongRecruitData       data;
	ZeroMemory(&data,sizeof(data));

	data.nCurPageNum          = ret->nInfoCount;
	data.nPageNo              = ret->nPageNo;
	
	S2C_SOCIAL_RECRUIT_INFO      *  pInfos = (S2C_SOCIAL_RECRUIT_INFO      *)szCompressionBuff;

	for (int i = 0 ;i<data.nCurPageNum;i++)
	{
		data.nLayer                  = pInfos[i].comInfo.nLayer;
		
		strncpy(data.nPageData[i].szUnitName,pInfos[i].comInfo.szUnitName,sizeof(data.nPageData[i].szUnitName));
		strncpy(data.nPageData[i].szOwnerName,pInfos[i].comInfo.szOwnerName,sizeof(data.nPageData[i].szOwnerName));
		
		int nDay = pInfos[i].dueTime;
		
		if (nDay<=0)
			nDay= 1;

		data.nPageData[i].nDueTimeDay   = nDay;

		data.nPageData[i].bOwnerOnline  = pInfos[i].comInfo.bOwnerOnline?true:false;
		data.nPageData[i].nSubUnitCount = pInfos[i].comInfo.nSubUnitCount;
		
		sprintf(data.nPageData[i].szCityMapName,"----");
		sprintf(data.nPageData[i].szPoolMapName,"----");

		CreateQueryManager(NULL);
		MapsInfo & mapinfo = MapsInfo::getSingleton();

		if (pInfos[i].comInfo.nLayer == enSULayer_Tong)
		{
            if ((int)pInfos[i].tongInfo.nCityMapID != INVALID_WORLD_ID)
			{
				MapsTab mapTab;
				mapinfo.GetMapInfo(pInfos[i].tongInfo.nCityMapID,mapTab);	
				if (mapTab.name.length())
					strcpy(data.nPageData[i].szCityMapName,mapTab.name.c_str());      
			}//endif
			
			if ((int)pInfos[i].tongInfo.nPoolMapID != INVALID_WORLD_ID)
			{
				MapsTab mapTab;
				mapinfo.GetMapInfo(pInfos[i].tongInfo.nPoolMapID,mapTab);
				if (mapTab.name.length())
					strcpy(data.nPageData[i].szPoolMapName,mapTab.name.c_str());        
			}//endif

		}//endif

	}//endif

    CoreDataChanged(GDCNI_UPDATA_TONG_RECRUIT, (int)&data, 0);
}

void ClientSocialUnitMgr::ReqUnitInfo(const char * szOwnerName,int nLayer,const TongUIReqeustCode & code)
{
	C2S_REQ_UNIT_INFO	        c2sReq;
	
	memset(&c2sReq, 0, sizeof(c2sReq));
	c2sReq.comHeader.proHeader.protocol    = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_GetUnitInfo;
	c2sReq.comHeader.proHeader.len         = sizeof(c2sReq) - PROTOCOL_SIZE;
	strncpy(c2sReq.szOwnerName,szOwnerName,sizeof(c2sReq.szOwnerName));
	c2sReq.szOwnerName[MAXSIZE_ROLENAME -1 ] = 0;
	c2sReq.nLayer                            = nLayer;
	c2sReq.code                              = code;
	
	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void ClientSocialUnitMgr::onChange(UIMDLEvent& rEvent)
{
	_ASSERT(rEvent.pDataSet);

	if(update_umdl != rEvent.nOperation)
		return;
	
	UIMDLDatasetRecord &record =  rEvent.pDataSet->getDataRecord(rEvent.nRecordIndex);
	TongOperParam *pParam = (TongOperParam*)record.pRecordData;

	if(pParam->nOperationID > enSUO_None && pParam->nOperationID < enSUO_Num)
	{
		if(m_opeFuncs[pParam->nOperationID])
		{
			UI2CORE_REQ_PARAM	ui2coreParam;

			ui2coreParam.comParam.opeId = pParam->nOperationID;
			ui2coreParam.comParam.unitLayer = pParam->nLayerID;
			ui2coreParam.comParam.unitTplId = pParam->nTemplateID;
			ui2coreParam.comParam.confirmCode = enSUReqConfirm_None;
			ui2coreParam.pParam = pParam;

			(this->*m_opeFuncs[pParam->nOperationID])(ui2coreParam);		
		}
	}
}

void ClientSocialUnitMgr::CreateUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_CREATEUNIT_REQ	c2sReq;

	memset(&c2sReq, 0, sizeof(c2sReq));
	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.comHeader.comParam = ui2coreParam.comParam;
	c2sReq.tag = 0;

	TongOperParam	*pTongParam = (TongOperParam*)ui2coreParam.pParam;

	if( !g_IsTextPass(pTongParam->szName) )
	{
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)g_szSocialMsg[enSocialErr_LawlessText], 0);
	}//endif
	else
	{
		strncpy(c2sReq.unitName, pTongParam->szName, sizeof(c2sReq.unitName));
		SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
	}//end else
}

void    ClientSocialUnitMgr::ReqJoinUnit(const UI2CORE_REQ_PARAM &ui2coreParam)
{
    C2S_HIGH_LEVEL_JOIN_REQ  c2sReq;

	memset(&c2sReq, 0, sizeof(c2sReq));
	c2sReq.nh.ch.proHeader.protocol = c2s_social_family;
	c2sReq.nh.ch.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.nh.ch.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.nh.ch.comParam.confirmCode = enSUReqConfirm_None;
	c2sReq.nh.ch.comParam = ui2coreParam.comParam;
	
	strncpy(c2sReq.nh.szLauncherName, 
		GetPlayerName(CLIENT_PLAYER_INDEX),
		sizeof(c2sReq.nh.szLauncherName)
		);
	
	TongOperParam	*pTongParam = (TongOperParam*)ui2coreParam.pParam;
	strncpy(c2sReq.receiverName, pTongParam->szReceiveName, sizeof(c2sReq.receiverName));
	
	SendDataToServer(&c2sReq, c2sReq.nh.ch.proHeader.len + PROTOCOL_SIZE);
}

void    ClientSocialUnitMgr::ReqRecruitInfo(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_RECRUIT_INFO_REQ     c2sReq;
	memset(&c2sReq, 0, sizeof(c2sReq));

	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
    c2sReq.comHeader.comParam = ui2coreParam.comParam;
	c2sReq.comHeader.comParam.unitLayer = enSULayer_Player;

    TongOperParam   *pUIParam = (TongOperParam*)ui2coreParam.pParam;
	c2sReq.curPageNo          = pUIParam->nPage;
	c2sReq.reqLayer           = pUIParam->nLayerID;

	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void   ClientSocialUnitMgr::ReqAddRecruit(const UI2CORE_REQ_PARAM &ui2coreParam)
{
    C2S_ADD_RECRUIT_INFO     c2sReq;
	memset(&c2sReq, 0, sizeof(c2sReq));
	
	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
    c2sReq.comHeader.comParam = ui2coreParam.comParam;
	
	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void    ClientSocialUnitMgr::ReqDelRecruit (const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_DEL_RECRUIT_INFO     c2sReq;
	memset(&c2sReq, 0, sizeof(c2sReq));
	
	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
    c2sReq.comHeader.comParam = ui2coreParam.comParam;
	
	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}


void ClientSocialUnitMgr::AddSubUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_ADDSUBUNIT_REQ	c2sReq;

	memset(&c2sReq, 0, sizeof(c2sReq));
	c2sReq.nh.ch.proHeader.protocol = c2s_social_family;
	c2sReq.nh.ch.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.nh.ch.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.nh.ch.comParam.confirmCode = enSUReqConfirm_None;
	c2sReq.nh.ch.comParam = ui2coreParam.comParam;

	strncpy(c2sReq.nh.szLauncherName, 
			GetPlayerName(CLIENT_PLAYER_INDEX),
			sizeof(c2sReq.nh.szLauncherName)
			);

	TongOperParam	*pTongParam = (TongOperParam*)ui2coreParam.pParam;
	strncpy(c2sReq.receiverName, pTongParam->szName, sizeof(c2sReq.receiverName));

	SendDataToServer(&c2sReq, c2sReq.nh.ch.proHeader.len + PROTOCOL_SIZE);

}

void ClientSocialUnitMgr::ChangeUnitOwner(const UI2CORE_REQ_PARAM &ui2coreParam)
{
    C2S_CHANGE_UNIT_OWNER_REQ	c2sReq;

	memset(&c2sReq, 0, sizeof(c2sReq));
	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.comHeader.comParam = ui2coreParam.comParam;

	TongOperParam	*pTongParam = (TongOperParam*)ui2coreParam.pParam;
	c2sReq.unitGuid = pTongParam->id;

	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);	
}

void ClientSocialUnitMgr::RemoveSubUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_REMOVESUBUNIT_REQ	c2sReq;

	memset(&c2sReq, 0, sizeof(c2sReq));
	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.comHeader.comParam = ui2coreParam.comParam;

	TongOperParam	*pTongParam = (TongOperParam*)ui2coreParam.pParam;
	c2sReq.unitGuid = pTongParam->id;

	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);	

}

void ClientSocialUnitMgr::GetSubListReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	const	int BUF_SIZE = 1024;
	char	buf[BUF_SIZE] = { 0 };

	C2S_GETSUBLIST_REQ	*pc2sReq = (C2S_GETSUBLIST_REQ*)buf;
	TongOperParam		*pUIParam = (TongOperParam*)ui2coreParam.pParam;

	if(pUIParam->nPage < 0)
		return;

	pc2sReq->comHeader.proHeader.protocol = c2s_social_family;
	pc2sReq->comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	pc2sReq->comHeader.comParam = ui2coreParam.comParam;
	pc2sReq->curPageNo = pUIParam->nPage;

	FSGUID	defGuid;

	if( !memcmp(&defGuid, &pUIParam->id, sizeof(defGuid)) )
	{
		pc2sReq->listType = enSUGetSubList_Type_Belong;
		pc2sReq->comHeader.proHeader.len = sizeof(C2S_GETSUBLIST_REQ) - 1 - PROTOCOL_SIZE;
	}
	else
	{
		pc2sReq->listType = enSUGetSubList_Type_Other;
		memcpy(pc2sReq->data, &pUIParam->id, sizeof(pUIParam->id));
		pc2sReq->comHeader.proHeader.len = sizeof(C2S_GETSUBLIST_REQ) - 1 + sizeof(pUIParam->id) - PROTOCOL_SIZE;
	}
	
	SendDataToServer(buf, pc2sReq->comHeader.proHeader.len + PROTOCOL_SIZE);

}

void ClientSocialUnitMgr::BreakUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_BREAKUNIT_REQ	c2sReq;

	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.comHeader.comParam = ui2coreParam.comParam;

	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void ClientSocialUnitMgr::LeaveUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_LEAVEUNIT_REQ	c2sReq;
	
	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.comHeader.comParam = ui2coreParam.comParam;

	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void ClientSocialUnitMgr::ForbidChatReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_FORBIDCHAT_REQ	c2sReq;

	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.comHeader.comParam = ui2coreParam.comParam;
	
	TongOperParam	*pParam = (TongOperParam*)ui2coreParam.pParam;
	memcpy(&c2sReq.unitGuid, &pParam->id, sizeof(c2sReq.unitGuid));

	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);

}

void ClientSocialUnitMgr::UnForbidChatReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_UNFORBIDCHAT_REQ	c2sReq;

	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.comHeader.comParam = ui2coreParam.comParam;

	TongOperParam	*pParam = (TongOperParam*)ui2coreParam.pParam;
	memcpy(&c2sReq.unitGuid, &pParam->id, sizeof(c2sReq.unitGuid));

	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);

}

void ClientSocialUnitMgr::PubAnnouncementReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	const int	BUF_SIZE = 2048;
	char		buf[BUF_SIZE];

	C2S_PUBANNOUNCEMENT_REQ	*pc2sReq = (C2S_PUBANNOUNCEMENT_REQ*)buf;
	pc2sReq->comHeader.proHeader.protocol = c2s_social_family;
	pc2sReq->comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	pc2sReq->comHeader.comParam = ui2coreParam.comParam;

	TongOperParam	*pParam = (TongOperParam*)ui2coreParam.pParam;
	pc2sReq->msgSize = strlen(pParam->szTip) + 1;

	if(pc2sReq->msgSize > MAXSIZE_ANNOUNCEMENT)
	{
		char szError[64] =PUB_ANUCMENT_TOO_LONG;
		
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)szError, 0);
		return;
	}

	_ASSERT(pc2sReq->msgSize + sizeof(C2S_PUBANNOUNCEMENT_REQ) <= BUF_SIZE);
	if(pc2sReq->msgSize + sizeof(C2S_PUBANNOUNCEMENT_REQ) > BUF_SIZE)	
		return;

	memcpy(pc2sReq->msg, pParam->szTip, pc2sReq->msgSize);
	pc2sReq->comHeader.proHeader.len = sizeof(C2S_PUBANNOUNCEMENT_REQ) + 
					pc2sReq->msgSize - 1 - PROTOCOL_SIZE;

	SendDataToServer(buf, pc2sReq->comHeader.proHeader.len + PROTOCOL_SIZE);

}

void ClientSocialUnitMgr::GetAnnouncementReq(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_GETANNOUNCEMENT_REQ		c2sReq;
	
	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_UnitOperation;
	c2sReq.comHeader.comParam = ui2coreParam.comParam;
    int nAnunceIndex = ui2coreParam.comParam.unitLayer - enSULayer_Gens;
    
	if (nAnunceIndex < MAX_ANOUNCEMENT_LAYER_NUM)
	{
        c2sReq.anaucementversion   = m_TempAnoucementVersion[nAnunceIndex];
	}//endif
	else c2sReq.anaucementversion  = INVALID_ANOUNCEMENT_VERSION;

	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void ClientSocialUnitMgr::ProcessProtocol(void *pData)
{
	S2C_COMOPE_HEADER	*pHeader = (S2C_COMOPE_HEADER*)pData;
	int					nSubProtocol = pHeader->proHeader.subProtocol;
	
	_ASSERT(nSubProtocol > enSRProtocol_None && nSubProtocol < enSRProtocol_Num);

	if(nSubProtocol > enSRProtocol_None && nSubProtocol < enSRProtocol_Num)
		(this->*m_netRetProc[nSubProtocol])(pData);
	
}

void ClientSocialUnitMgr::s2cMsgCode(const void *ps2cRet)
{
	S2C_SOCIAL_MSGCODE	*ps2cMsgCode = (S2C_SOCIAL_MSGCODE*)ps2cRet;
	int					nMsgCode = ps2cMsgCode->msgCode;

	if(nMsgCode > enSocialErr_None && nMsgCode < enSocialErr_Num)
		CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)g_szSocialMsg[nMsgCode], 0);
}

void ClientSocialUnitMgr::s2cReqToConfirm(const void *ps2cRet)
{
	S2C_REQ_COMFIRM	*ps2cReqConfirm = (S2C_REQ_COMFIRM*)ps2cRet;
	
	m_UnConfirmReq.nSize = ps2cReqConfirm->reqSize;
	memcpy(m_UnConfirmReq.data, ps2cReqConfirm->data, m_UnConfirmReq.nSize);
	S2C_REQ_COMFIRM	*pNewReq = (S2C_REQ_COMFIRM*)m_UnConfirmReq.data;
	pNewReq->comHeader.comParam.confirmCode = enSUReqConfirm_Ok;
	pNewReq->comHeader.proHeader.subProtocol = enSRProtocol_ReqConfirmRet;

	if(m_UnConfirmReq.isInUse)
	{
		//如果直接返回、发送方会得不到响应! Add by brianyao 2007
        pNewReq->comHeader.comParam.confirmCode = enSUReqConfirm_Ignore;
		SendDataToServer(m_UnConfirmReq.data, m_UnConfirmReq.nSize);
        m_UnConfirmReq.isInUse = false;
		return;
	}//endif

	m_UnConfirmReq.isInUse = 1;

	const char *szHintMsg = ps2cReqConfirm->data + ps2cReqConfirm->reqSize;
	// Debug
	// GDCNI_OPEN_INVOTE_SOCIETY_REPLY_SHIZU 不支持直接显示字符串
	CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)szHintMsg, 0);

	KUiPlayerItem	uiItem;
	memset(&uiItem, 0, sizeof(uiItem));

	uiItem.nParam         = ( int ) szHintMsg;
	uiItem.uId            =  ps2cReqConfirm->comHeader.comParam.unitLayer;

    CoreDataChanged(GDCNI_OPEN_INVOTE_SOCIETY_REPLY_COMFIRM, (int)&uiItem, 0);
}

void ClientSocialUnitMgr::s2cUnitOperation(const void *ps2cRet)
{
	S2C_COMOPE_HEADER *ps2cComHeader = (S2C_COMOPE_HEADER*)ps2cRet;
	
	switch(ps2cComHeader->comParam.opeId)
	{
	case enSUO_GetSubList:
	case enSUO_GetPrePageSubList:
	case enSUO_GetNextPageSubList:
		GetSubListRet(ps2cRet);
		break;

	case enSUO_GetAnnouncement:
		GetAnnouncementRet(ps2cRet);
		break;

	case enSUO_GetRecruitInfo:
		GetRecruitInfoRet(ps2cRet);
		break;

	default:
		break;
	}
}

void ClientSocialUnitMgr::GetAnnouncementRet(const void *ps2cRet)
{
	IUIMDLDataset	*pDataSet = GetDataSet(tong_dataset);

	if(NULL == pDataSet)
		return;

	const S2C_GETANNOUNCEMENT_RET *pAnnoucne = (const S2C_GETANNOUNCEMENT_RET*)ps2cRet;	

	TongData	uiRecData;
	uiRecData.nOperationServerID = pAnnoucne->comHeader.comParam.opeId;
	uiRecData.eOperationClientID = get_society_baseinfo_info;
	uiRecData.nLayerID = pAnnoucne->comHeader.comParam.unitLayer;
	uiRecData.nTemplateID = pAnnoucne->comHeader.comParam.confirmCode;

	int         nLayer    = pAnnoucne->comHeader.comParam.unitLayer; 

	strncpy(uiRecData.szTip, pAnnoucne->data, pAnnoucne->announcementLen);

	if (nLayer - enSULayer_Gens < MAX_ANOUNCEMENT_LAYER_NUM)
	{
		strncpy(m_TempAnoucementRecord[nLayer - enSULayer_Gens],pAnnoucne->data,sizeof(m_TempAnoucementRecord[nLayer - enSULayer_Gens]));
        m_TempAnoucementVersion[nLayer - enSULayer_Gens]  = pAnnoucne->announceversion;
	}//endif

	pDataSet->addDataRecord(&uiRecData, sizeof(uiRecData));

}

void ClientSocialUnitMgr::GetCityInfoRet(const void *ps2cRet)
{
	IUIMDLDataset	*pDataSet = GetDataSet(city_dataset);

	if(NULL == pDataSet)
		return;

	BuffTable& buffTable = BuffTable::Singleton();
	const S2C_SOCIAL_CITY_INFO *pCityInfo = (const S2C_SOCIAL_CITY_INFO*)ps2cRet;	
	if ( pCityInfo )
	{
		if ( pCityInfo->nInfoType == base_info_city)
		{
			CityInfoParam tagCityInfo;
			ZeroMemory( &tagCityInfo, sizeof(CityInfoParam) );
		
			PBAT pBuffPro = buffTable.GetBuff( pCityInfo->nCityProduceBuffTemplateID );
			if ( pBuffPro != NULL )
			{
				strncpy( tagCityInfo.tagBaseInfo.szMaintenanceInfo, pBuffPro->szDesc, COMMON_CLIENT_MSG_LEN_128 );
			}		
			PBAT pBuffCon = buffTable.GetBuff( pCityInfo->nCityConsumeBuffTemplateID );
			if ( pBuffCon != NULL )
			{
				strncat( tagCityInfo.tagBaseInfo.szMaintenanceInfo, pBuffCon->szDesc, COMMON_CLIENT_MSG_LEN_128 );
			}		

			memcpy( tagCityInfo.tagBaseInfo.szKingName, pCityInfo->szOwnerName, CLIENT_NAME_AND_TITLE_MAX );
			tagCityInfo.tagBaseInfo.szKingName[CLIENT_NAME_AND_TITLE_MAX-1] = 0;
	    	memcpy( tagCityInfo.tagBaseInfo.szZhuhouName,Npc[GetClientPlayer().GetNpcIndex()].GetLeagueName(),COMMON_CLIENT_MSG_LEN_16+1);
            tagCityInfo.tagBaseInfo.szZhuhouName[COMMON_CLIENT_MSG_LEN_16] = 0;

			tagCityInfo.tagBaseInfo.nMoney = pCityInfo->arrayCityRes[0];
			tagCityInfo.tagBaseInfo.nCopperCount = pCityInfo->arrayCityRes[1];
			tagCityInfo.tagBaseInfo.nFlixCount = pCityInfo->arrayCityRes[2];
			tagCityInfo.tagBaseInfo.nWoodCount = pCityInfo->arrayCityRes[3];

			tagCityInfo.tagBaseInfo.nTaxRate   = pCityInfo->nTaxRate;
			tagCityInfo.tagBaseInfo.nProsonCount = pCityInfo->nTongMemberCount;
			tagCityInfo.tagBaseInfo.nShizuCount = pCityInfo->nShizuCount;

			tagCityInfo.tagBaseInfo.nDevelopment = pCityInfo->nDevelopment;
			tagCityInfo.tagBaseInfo.nTiredness   = pCityInfo->nTiredness;

			pDataSet->addDataRecord( &tagCityInfo, sizeof(CityInfoParam) );
		}
	}
}

void ClientSocialUnitMgr::GetSubListRet(const void *ps2cRet)
{
	IUIMDLDataset	*pDataSet = GetDataSet(tong_dataset);

	if(NULL == pDataSet)
		return;

	S2C_GETSUBLIST_RET		*pSubListRet = (S2C_GETSUBLIST_RET*)ps2cRet;
	SU_UNITTRANSFER_INFO	*pUnitInfo = (SU_UNITTRANSFER_INFO*)pSubListRet->data;
	int                      nSizeCompression = pSubListRet->comHeader.proHeader.len -  sizeof(S2C_GETSUBLIST_RET) + 1 + PROTOCOL_SIZE;

	const	int              BUF_SIZE = 8192;
	char				buf[BUF_SIZE] = { 0 };

	if (nSizeCompression > 0)
	{
		unsigned int                      nLen     = BUF_SIZE;
		lzo1x_decompress(
			(const unsigned char *)pSubListRet->data,
			nSizeCompression,
			(unsigned char *)buf,
			&nLen,
			NULL);
		
		pUnitInfo           = (SU_UNITTRANSFER_INFO*)buf;
	}//endif

    static TongData	uiRecData;
	ZeroMemory( &uiRecData, sizeof(TongData) );
	int			nCount = pSubListRet->unitCount;
	
	uiRecData.nLayerID = pSubListRet->comHeader.comParam.unitLayer;
	CreateQueryManager(NULL);
    MapsInfo & mapinfo = MapsInfo::getSingleton();
	
	if (pSubListRet->listLayer == enSULayer_Tong || pSubListRet->listLayer == enSULayer_Gens )
	{
		int nClientNpc = GetClientPlayer().GetNpcIndex();
		if (IsValidNpc(nClientNpc) && Npc[nClientNpc].GetCityId() != pSubListRet->cityWorldId)
		{
			Npc[nClientNpc].SetCityId(pSubListRet->cityWorldId);
			char szCityName[MAXSIZE_CITYNAME] = { 0 };
			g_SubWorldSet.GetWorldNameFromID(pSubListRet->cityWorldId, szCityName, sizeof(szCityName));
			Npc[nClientNpc].SetCityName(szCityName);
		}//endif

	}//endif

	if (pSubListRet->cityWorldId != INVALID_WORLD_ID)
	{
		MapsTab mapTab;
        mapinfo.GetMapInfo(pSubListRet->cityWorldId,mapTab);

        strcpy(uiRecData.szCityName,mapTab.name.c_str());      
	}
	else
		sprintf(uiRecData.szCityName,"--");

	if (pSubListRet->poolWorldId != INVALID_WORLD_ID)
	{
		MapsTab mapTab;
        mapinfo.GetMapInfo(pSubListRet->poolWorldId,mapTab);
		
        strcpy(uiRecData.szPoolName,mapTab.name.c_str());      
	}
	else
		sprintf(uiRecData.szPoolName,"--");

	if(enSUGetSubList_Type_Belong == pSubListRet->listType)
	{
		uiRecData.nOperationServerID = pSubListRet->comHeader.comParam.opeId;
		uiRecData.eOperationClientID = get_society_baseinfo_name;
		uiRecData.tagCount.nCount = 1;
		uiRecData.tagCount.nMaxPlayerCount = pSubListRet->maxPlayerCount;
		uiRecData.tagCount.nOnlinePlayerCount = pSubListRet->onlinePlayerCount;
		strncpy(uiRecData.szName, pUnitInfo->comInfo.unitName, sizeof(uiRecData.szName));
		strncpy(uiRecData.szOwnerName,pUnitInfo->comInfo.ownerName,sizeof(uiRecData.szOwnerName));
		uiRecData.szOwnerName[sizeof(uiRecData.szOwnerName) - 1] = 0;
		pDataSet->addDataRecord(&uiRecData, sizeof(TongData));
		--nCount;
		++pUnitInfo;
	}

	switch(pSubListRet->listLayer)
	{
	case enSULayer_Player:
		uiRecData.nOperationServerID = pSubListRet->comHeader.comParam.opeId;
		uiRecData.eOperationClientID = get_society_memberlist_operation;
		break;
		
	case enSULayer_Gens:
		uiRecData.nOperationServerID = pSubListRet->comHeader.comParam.opeId;
		uiRecData.eOperationClientID = get_society_shizulist_operation;
		break;

	case enSULayer_Tong:
		uiRecData.nOperationServerID = pSubListRet->comHeader.comParam.opeId;
		uiRecData.eOperationClientID = get_society_zhuhoulist_operation;
		break;
	default:
		return;
	}

	for(int nRec = 0; nRec < nCount && nRec < MAXCOUNT_SUBLIST_ONETIMEGET; ++nRec)
	{
		uiRecData.memberList[nRec].nLevel = pUnitInfo[nRec].playerInfo.level;
		uiRecData.memberList[nRec].bOnline = pUnitInfo[nRec].playerInfo.isOnline ? true : false;
		uiRecData.memberList[nRec].nMetier = pUnitInfo[nRec].playerInfo.profession;
		
		for (int n=0;n<MAX_PREVENT_SATE_NUM;n++)
			uiRecData.memberList[nRec].bPreventChatState[n] = pUnitInfo[nRec].playerInfoEx.ForbidChatState[n] ? true : false;

		uiRecData.memberList[nRec].nTopOwnerLayer = pUnitInfo[nRec].playerInfoEx.topOwnerLayer;

		const char *szName;

		if(enSULayer_Player == pSubListRet->listLayer)
			szName = pUnitInfo[nRec].comInfo.ownerName;
		else
			szName = pUnitInfo[nRec].comInfo.unitName;

		strncpy(uiRecData.memberList[nRec].szName, 
				szName,
				sizeof(uiRecData.memberList[nRec].szName)
			   );
		memcpy(uiRecData.memberList[nRec].guid,
			   &pUnitInfo[nRec].comInfo.unitGuid,
			   sizeof(uiRecData.memberList[nRec].guid)
			   );
		if (enSULayer_Tong == pSubListRet->listLayer)
		{
			if (pUnitInfo[nRec].tongInfoEx.nCityMapId != ((WORD)INVALID_WORLD_ID))
			{
				MapsTab mapTab;
				mapinfo.GetMapInfo(pUnitInfo[nRec].tongInfoEx.nCityMapId,mapTab);
				
				strcpy(uiRecData.memberList[nRec].szCityName,mapTab.name.c_str());      
			}//endif
			else
				sprintf(uiRecData.memberList[nRec].szCityName,"--");
			
			if (pUnitInfo[nRec].tongInfoEx.nPoolMapId != ((WORD)INVALID_WORLD_ID))
			{
				MapsTab mapTab;
				mapinfo.GetMapInfo(pUnitInfo[nRec].tongInfoEx.nPoolMapId,mapTab);
				strcpy(uiRecData.memberList[nRec].szPoolName,mapTab.name.c_str()); 
			}//endif
			else
				sprintf(uiRecData.memberList[nRec].szPoolName,"--");
			
			uiRecData.memberList[nRec].nSubUnitNum = pUnitInfo[nRec].tongInfoEx.nSubUnitNum;
				
		}//endif
	}

	pDataSet->addDataRecord(&uiRecData, sizeof(TongData));

}

IUIMDLDataset*	ClientSocialUnitMgr::GetDataSet(const char *szName)
{
	IUIMDL			*pMDLInterface = NULL;
	IUIMDLDataset	*pDataSet = NULL;

	if( success_errorcode != GetMDLPtr(&pMDLInterface) )
		return NULL;

	if(NULL == pMDLInterface)
		return NULL;

	if( success_errorcode != pMDLInterface->queryDataSet(szName, &pDataSet) )
		return NULL;

	return pDataSet;
}

void ClientSocialUnitMgr::ReqConfirmRetFromUI(int nParam)
{
	S2C_REQ_COMFIRM	*pNewReq = (S2C_REQ_COMFIRM*)m_UnConfirmReq.data;
	pNewReq->comHeader.proHeader.subProtocol = enSRProtocol_ReqConfirmRet;

	switch (nParam)
	{	
	 case 1:
		 {
			 pNewReq->comHeader.comParam.confirmCode = enSUReqConfirm_Ok;
			 SendDataToServer(m_UnConfirmReq.data, m_UnConfirmReq.nSize);
		 }//end for case
		 break;
     case 2:
		 {
			 pNewReq->comHeader.comParam.confirmCode = enSUReqConfirm_Ignore;
			 SendDataToServer(m_UnConfirmReq.data, m_UnConfirmReq.nSize);
		 }//end for case
		 break;
	 case 0:
		 {
			 pNewReq->comHeader.comParam.confirmCode = enSUReqConfirm_Refuse;
			 SendDataToServer(m_UnConfirmReq.data, m_UnConfirmReq.nSize);
		 }//end for case
         break;
	 default:break;
	}

	m_UnConfirmReq.isInUse = false;
}

void ClientSocialUnitMgr::s2cMsg(const void *ps2cRet)
{
	S2C_SOCIAL_MSG	*ps2cMsg = (S2C_SOCIAL_MSG*)ps2cRet;

	CoreDataChanged(GDCNI_ERROR_MESSAGE, (int)ps2cMsg->msg, 0);
}

void ClientSocialUnitMgr::SetCityRes(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_CITY_RES_REQ tagCityResReq;
	ZeroMemory( &tagCityResReq, sizeof( C2S_CITY_RES_REQ ) );

	tagCityResReq.comHeader.proHeader.protocol = c2s_social_family;
	tagCityResReq.comHeader.proHeader.len = sizeof(tagCityResReq) - PROTOCOL_SIZE;
	tagCityResReq.comHeader.proHeader.subProtocol = enSRProtocol_ContributeCityRes;

	CityOperParam *pCityOperParam = (CityOperParam *) (((TongOperParam *)ui2coreParam.pParam)->szTip);

	tagCityResReq.arrayCityRes[0] = pCityOperParam->tagCityRes.nMoney;
	tagCityResReq.arrayCityRes[1] = pCityOperParam->tagCityRes.nCopperCount;
	tagCityResReq.arrayCityRes[2] = pCityOperParam->tagCityRes.nFlixCount;
	tagCityResReq.arrayCityRes[3] = pCityOperParam->tagCityRes.nWoodCount;

	SendDataToServer(&tagCityResReq, tagCityResReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void ClientSocialUnitMgr::GetCityRes(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_CITY_RES_REQ tagCityResReq;
	ZeroMemory( &tagCityResReq, sizeof( C2S_CITY_RES_REQ ) );

	tagCityResReq.comHeader.proHeader.protocol = c2s_social_family;
	tagCityResReq.comHeader.proHeader.len = sizeof(tagCityResReq) - PROTOCOL_SIZE;
	tagCityResReq.comHeader.proHeader.subProtocol = enSRProtocol_DistillCityRes;

	CityOperParam *pCityOperParam =*((CityOperParam ** ) (((TongOperParam *)ui2coreParam.pParam)->szTip));

	tagCityResReq.arrayCityRes[0] = pCityOperParam->tagCityRes.nMoney;
	tagCityResReq.arrayCityRes[1] = pCityOperParam->tagCityRes.nCopperCount;
	tagCityResReq.arrayCityRes[2] = pCityOperParam->tagCityRes.nFlixCount;
	tagCityResReq.arrayCityRes[3] = pCityOperParam->tagCityRes.nWoodCount;

	SendDataToServer(&tagCityResReq, tagCityResReq.comHeader.proHeader.len + PROTOCOL_SIZE);

}

void ClientSocialUnitMgr::ReqCityInfo(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_CITY_INFO_REQ tagCityInfoReq;
	ZeroMemory( &tagCityInfoReq, sizeof( C2S_CITY_INFO_REQ ) );

	tagCityInfoReq.comHeader.proHeader.protocol = c2s_social_family;
	tagCityInfoReq.comHeader.proHeader.len = sizeof(tagCityInfoReq) - PROTOCOL_SIZE;
	tagCityInfoReq.comHeader.proHeader.subProtocol = enSRProtocol_ReqCityInfo;

	int* pType = (int*)ui2coreParam.pParam;
	tagCityInfoReq.nType = *pType;

	SendDataToServer(&tagCityInfoReq, tagCityInfoReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void ClientSocialUnitMgr::SetCityTaxRateNormal(int nNewTax)
{
	C2S_SET_CITYTAXRATE_REQ c2sReq;
	ZeroMemory(&c2sReq, sizeof(c2sReq));
	
	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_SetCityTaxRate;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	
	c2sReq.nTaxRate = nNewTax;
	
	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}

void ClientSocialUnitMgr::NotifyChangeRelation()
{
	for (int i=0;i<MAX_ANOUNCEMENT_LAYER_NUM;i++)
	{
		m_TempAnoucementRecord [i][0] = 0;
		m_TempAnoucementVersion[i]   = INVALID_ANOUNCEMENT_VERSION;
	}//end for i
}

void ClientSocialUnitMgr::SetCityTaxRate(const UI2CORE_REQ_PARAM &ui2coreParam)
{
	C2S_SET_CITYTAXRATE_REQ c2sReq;
	ZeroMemory(&c2sReq, sizeof(c2sReq));

	c2sReq.comHeader.proHeader.protocol = c2s_social_family;
	c2sReq.comHeader.proHeader.subProtocol = enSRProtocol_SetCityTaxRate;
	c2sReq.comHeader.proHeader.len = sizeof(c2sReq) - PROTOCOL_SIZE;

	c2sReq.nTaxRate = *( (int*)ui2coreParam.pParam );

	SendDataToServer(&c2sReq, c2sReq.comHeader.proHeader.len + PROTOCOL_SIZE);
}