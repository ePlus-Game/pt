#include "KCore.h"
#include "social_recruit_svr.h"
#include "ServerSocialUnitMgr.h"
#include "ConfigManager.h"
#include "KSubWorldSet.h"
#define MAX_RECRUIT_LOG_LEN 512

bool tagSocialRecruitInfo::operator < (const tagSocialRecruitInfo & info)
{
	 if ( (comInfo.bOwnerOnline && !info.comInfo.bOwnerOnline) ||
		 (comInfo.bOwnerOnline && info.comInfo.bOwnerOnline && baseInfo.dueTime > info.baseInfo.dueTime) ||
		 ( !comInfo.bOwnerOnline && !info.comInfo.bOwnerOnline && baseInfo.dueTime > info.baseInfo.dueTime)		 
		 )
		 return true;
	 else
		 return false;
}

KSocialRecruitMgr & KSocialRecruitMgr::Singlton()
{
	static KSocialRecruitMgr mgr;
	return mgr;
}


KSocialRecruitMgr::KSocialRecruitMgr():mInited(false),mLastCheckTime(UNIX_TMIE_STAMP)
{}

KSocialRecruitMgr::~KSocialRecruitMgr()
{
}

void KSocialRecruitMgr::Init()
{
	ReqDBInfoList();
}

void KSocialRecruitMgr::Release()
{
	mSortedGensInfo.clear();
	mSortedTongInfo.clear();
	mLoadingUnitInfos.clear();
	mRecruitInfos.clear();
	mInited = false;
}

void KSocialRecruitMgr::ProcessGlobalDBRet(int nDbOpeRst, IProcRet* pRet )
{
	if (pRet == 0)
		return ;

    int   nPassBySize = 0;
	char* pPassBy     = pRet->GetPassBy( nPassBySize );
	
	if( nPassBySize != sizeof(_SOCIAL_RECRUIT_DB) || 
		pPassBy == NULL )
	{
		return;
	}//endif

	_SOCIAL_RECRUIT_DB* pHeader = (_SOCIAL_RECRUIT_DB*)pPassBy;
	
	int uDBOpeType = pHeader->ope;
	
	_ASSERT(uDBOpeType >= SR_DBOpe_Add && uDBOpeType < SR_DBOpe_end);

	switch (uDBOpeType)
	{
	case SR_DBOpe_Add:
		{
            if (!nDbOpeRst)
			{
				_ASSERT(false);
			}//end else

		}//end for case 
		break;

	case SR_DBOpe_Del:
		{
			if (!nDbOpeRst)
			{
				_ASSERT(false);
			}//endif
			
		}//end for case 	
		break;

	case SR_DBOpe_lst:
		{
           _ASSERT(nDbOpeRst);
		   if (nDbOpeRst)
		   {
			   int	nRow = pRet->GetRowCount( );
			   
			   for (int row = 0; row < nRow; row++)
			   {
				    ProcessListData(row,pRet);
			   }//end for row

               mInited        = true;
			   mLastCheckTime = UNIX_TMIE_STAMP; 

		   }//endif

		}//end for case 
		break;
	}//end for switch
}

#define SOCIAL_RECRUIT_LOADING_INTERVAL (GAME_FPS * 5)

void KSocialRecruitMgr::Breathe()
{
	if (mInited)
	{
		if ( (g_SubWorldSet.GetGameTime() % SOCIAL_RECRUIT_LOADING_INTERVAL == 0) &&  mLoadingUnitInfos.size() )
		{
            CheckLoadingUnit();
		}//endif

		if (UNIX_TMIE_STAMP - mLastCheckTime >= SR_INFO_CHECK_INTERVAL)
		{
			CheckInvalid();

			if (mSortedGensInfo.size())
				CheckSortedList(mSortedGensInfo);
			
			if (mSortedTongInfo.size())
				CheckSortedList(mSortedTongInfo);
			
			mLastCheckTime = UNIX_TMIE_STAMP;
		}//endif

	}//endif
}

bool KSocialRecruitMgr::IsLoading(const FSGUID & guid)
{
	std::list<FSGUID>::iterator it = mLoadingUnitInfos.begin();
	while (it != mLoadingUnitInfos.end())
	{
		if (*it == guid)
		{
			return true;
		}//endif
		
		++it;
	}//end while

	return false;
}

void KSocialRecruitMgr::CheckInvalid()
{
	bool           bChange = false;
	
	//1.Check for TimeOut........................................................
	if (mRecruitInfos.size())
	{
		std::map<FSGUID,SocialRecruitBaseInfo>::iterator recruitIT= mRecruitInfos.begin();
		
		while (recruitIT!=mRecruitInfos.end())
		{
			FSGUID                 guid  = recruitIT->first;
			bool                   bDel  = false;

			if ( UNIX_TMIE_STAMP >= (unsigned long)(recruitIT->second.dueTime))
				bDel = true;
			else
			{
				ServerSocialUnitMgr &  mgr   = ServerSocialUnitMgr::Singleton();	
			    SocialUnit          *  pUnit = mgr.GetUnit(guid);
			
				if (pUnit)
				{
					//Unit Full...
					if (pUnit->GetChildCount() >= GetMaxSubUnitCnt(enSUTplId_Tong, pUnit->GetLayer()))
						bDel = true;	
				}//endif
				else
				{
					//Unit Break...
					if (!IsLoading(guid))
						bDel = true;

				}//end else

			}//end else

			if (  bDel )
			{
				if (!ReqDBDelInfoRecord(recruitIT->second,-1))
				{
					_ASSERT(false);	
				}//end
				
				DelInfoDirectly(recruitIT++);
				
			}//end if 
			else
				++recruitIT;
		}//end for while
		
	}//endif

}


int KSocialRecruitMgr::AddInfo(const FSGUID & guid,const int nPlayerIndex)
{
	if (!mInited)
		return enSocialErr_RetryLater;
	
    _ASSERT(ServerSocialUnitMgr::Singleton().GetUnit(guid));
	std::map<FSGUID,SocialRecruitBaseInfo>::iterator it= mRecruitInfos.find(guid);
	
	if (it != mRecruitInfos.end())
		return enSocialErr_RecruitExist;
    
    ConfigManager & mgr = ConfigManager::Singleton();
	
	SocialRecruitBaseInfo info;
	info.dueTime          = mgr.GetGlobalVariable(global_var_social_recruit_exist_time) + UNIX_TMIE_STAMP;
    info.guid             = guid;
	
	if (AddInfoDirectly(info))
	{
		SocialUnit *         pUnit = ServerSocialUnitMgr::Singleton().GetUnit(guid);
		if (pUnit && IsValidPlayer(nPlayerIndex))
		{
			//Send Msg to client ....
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
			ps2cRet->comHeader.comParam.confirmCode  = 0;
			ps2cRet->comHeader.comParam.opeId        = enSUO_GetRecruitInfo;
			ps2cRet->comHeader.comParam.unitLayer    = pUnit->GetLayer();
			ps2cRet->comHeader.comParam.unitTplId    = enSUTplId_Tong;
			
			int nRet = GetInfoList(0,MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE,pUnit->GetLayer(),BUF_SIZE,ps2cRet);
			_ASSERT(nRet == enSocialErr_None || nRet == enSocialErr_NoInfo);
			
			if (nRet == enSocialErr_None)
			{	
				SendDataToClient(nPlayerIndex, buf, ps2cRet->comHeader.proHeader.len + PROTOCOL_SIZE);
			}//endif
			
		}//endif
		
		ReqDBAddInfoRecord(info,nPlayerIndex);
		return enSocialErr_RecruitOpSuc;
	}//endif	
	else
		return enSocialErr_RecruitFailed;
}


int KSocialRecruitMgr::DelInfo(const FSGUID & guid,const int nPlayerIndex)
{
	if (!mInited)
		return enSocialErr_RetryLater;
	
    _ASSERT(ServerSocialUnitMgr::Singleton().GetUnit(guid));
	
	if (!DelInfoDirectly(guid))
		return enSocialErr_RecruitNotExist;
	
    SocialRecruitBaseInfo info;
	info.guid           = guid;
	info.dueTime        = 0;
	
	ReqDBDelInfoRecord(info,nPlayerIndex);
	
	SocialUnit *  pUnit = ServerSocialUnitMgr::Singleton().GetUnit(guid);
	if (pUnit && IsValidPlayer(nPlayerIndex))
	{
		//Send Msg to client ....
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
		ps2cRet->comHeader.comParam.confirmCode  = 0;
		ps2cRet->comHeader.comParam.opeId        = enSUO_GetRecruitInfo;
		ps2cRet->comHeader.comParam.unitLayer    = pUnit->GetLayer();
		ps2cRet->comHeader.comParam.unitTplId    = enSUTplId_Tong;
		
		int nRet = GetInfoList(0,MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE,pUnit->GetLayer(),BUF_SIZE,ps2cRet);
		_ASSERT(nRet == enSocialErr_None || nRet == enSocialErr_NoInfo);
		
		if (nRet == enSocialErr_None)
		{	
			SendDataToClient(nPlayerIndex, buf, ps2cRet->comHeader.proHeader.len + PROTOCOL_SIZE);
		}//endif
		
	}//endif
	
	return enSocialErr_RecruitOpSuc;
}

int KSocialRecruitMgr::ReqDBDelInfoRecord(const SocialRecruitBaseInfo & info,const int nPlayerIndex)
{
	if (g_pController == NULL)
		return false;

	_SOCIAL_RECRUIT_DB DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	  = -1;
	DBHeader.ProcType	  = Proc_SocialRecruit;
	DBHeader.ope	   	  = SR_DBOpe_Del;
    DBHeader.info         = info;
	DBHeader.nLauncherIdx = nPlayerIndex; 
	
	if (g_pController == NULL)
		return false;

	IProcParam* pParam = g_pController->GetProcParam( );

	if (!pParam)
		return false;
	
	//begin
	pParam->BeginPush( PN_DEL_RECRUIT );
	pParam->Push(info.guid.data);
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
	_ASSERT(nRet);

	return nRet;
}

int KSocialRecruitMgr::ReqDBInfoList(void)
{
    _SOCIAL_RECRUIT_DB DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	  = -1;
	DBHeader.ProcType	  = Proc_SocialRecruit;
	DBHeader.ope	   	  = SR_DBOpe_lst;
	
	if (g_pController == NULL)
		return false;

	IProcParam* pParam = g_pController->GetProcParam( );
	
	if (pParam == NULL)
		return false;

	//begin
	pParam->BeginPush( PN_LIST_RECRUIT );
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
	_ASSERT(nRet);
	
	return nRet;
}

int KSocialRecruitMgr::ReqDBAddInfoRecord(const SocialRecruitBaseInfo & info,const int nPlayerIndex)
{
	_SOCIAL_RECRUIT_DB DBHeader;
	memset( &DBHeader, 0, sizeof(DBHeader) );
	
	DBHeader.ulNetID	  = -1;
	DBHeader.ProcType	  = Proc_SocialRecruit;
	DBHeader.ope	   	  = SR_DBOpe_Add;
    DBHeader.info         = info;
	DBHeader.nLauncherIdx = nPlayerIndex; 
	
	if (g_pController == NULL )
		return false;

	IProcParam* pParam = g_pController->GetProcParam( );
	
	if (pParam == NULL)
		return false;

	//begin
	pParam->BeginPush( PN_ADD_RECRUIT );
	pParam->Push(info.guid.data);
	pParam->Push(info.dueTime - UNIX_TMIE_STAMP);
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	int nRet = g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );
	_ASSERT(nRet);
	
	return nRet;
}

int KSocialRecruitMgr::AddInfoDirectly(const SocialRecruitBaseInfo & srcInfo )
{
    std::map<FSGUID,SocialRecruitBaseInfo>::iterator it= mRecruitInfos.find(srcInfo.guid);
	
	if (it == mRecruitInfos.end())
	{
        mRecruitInfos[srcInfo.guid] = srcInfo;
		//Add to sorted list

		ServerSocialUnitMgr & mgr   = ServerSocialUnitMgr::Singleton();
		SocialUnit          * pUnit = mgr.GetUnit(srcInfo.guid);
		if (pUnit)
		{
            SocialRecruitInfo          infoDetail;
			RefreshInfo(infoDetail,srcInfo);

			if (pUnit->GetLayer() == enSULayer_Tong)
			{
               AddToSortedList(infoDetail,mSortedTongInfo);
			}//end for if
			else
			{
               AddToSortedList(infoDetail,mSortedGensInfo);
			}//end for else

		}
		else
		{
			//Loaded later...
			mLoadingUnitInfos.push_back(srcInfo.guid);
		}
	
		return TRUE;
	}//endif
	else
	{
		return FALSE;
	}//end else
}

void KSocialRecruitMgr::CheckSortedList(std::list<SocialRecruitInfo> & destList)
{	
    std::list<SocialRecruitInfo>::iterator it = destList.begin();
	while (it != destList.end())
	{	
		SocialRecruitInfo &   info     = *it;
		SocialRecruitBaseInfo baseInfo = info.baseInfo;
		RefreshInfo(info,baseInfo);
		++ it;
	}//end for while
	
	destList.sort();
}

int  KSocialRecruitMgr::DelInfoDirectly(const FSGUID & guid)
{
	std::map<FSGUID,SocialRecruitBaseInfo>::iterator it= mRecruitInfos.find(guid);
	
	if (it != mRecruitInfos.end())
	{
		mRecruitInfos.erase(it);
		std::list<FSGUID>::iterator loadIter = mLoadingUnitInfos.begin();
		
		while (loadIter != mLoadingUnitInfos.end() )
		{
		   if (*loadIter == guid)
		   {
			   mLoadingUnitInfos.erase(loadIter);
			   return TRUE;
		   }//endif

           ++loadIter; 
		}//end while

		SocialUnit * pUnit = ServerSocialUnitMgr::Singleton().GetUnit(guid);
		if (pUnit)
		{
			if (pUnit->GetLayer() == enSULayer_Gens)
			{
                bool bRet = DelFromSortedList(guid,mSortedGensInfo);
				_ASSERT(bRet);
			}//endif
			else
			{
                bool bRet = DelFromSortedList(guid,mSortedTongInfo);
				_ASSERT(bRet);
			}//end else
		}//endif

		return TRUE;
	}//endif
	else
		return FALSE;

}

int  KSocialRecruitMgr::DelInfoDirectly(std::map<FSGUID,SocialRecruitBaseInfo>::iterator it)
{
	_ASSERT(it != mRecruitInfos.end());
	if (it == mRecruitInfos.end())
		return FALSE;

	FSGUID guid = it->second.guid;

	mRecruitInfos.erase(it);
	std::list<FSGUID>::iterator loadIter = mLoadingUnitInfos.begin();
	
	while (loadIter != mLoadingUnitInfos.end() )
	{
		if (*loadIter == guid)
		{
			mLoadingUnitInfos.erase(loadIter);
			return TRUE;
		}//endif
		
		++loadIter; 
	}//end while
	
	SocialUnit * pUnit = ServerSocialUnitMgr::Singleton().GetUnit(guid);
	if (pUnit)
	{
		if (pUnit->GetLayer() == enSULayer_Gens)
		{
			bool bRet = DelFromSortedList(guid,mSortedGensInfo);
			_ASSERT(bRet);
		}//endif
		else
		{
			bool bRet = DelFromSortedList(guid,mSortedTongInfo);
			_ASSERT(bRet);
		}//end else
	}//endif
	else
	{
		//The Unit is Break!
		DelFromSortedList(guid,mSortedGensInfo);
		DelFromSortedList(guid,mSortedTongInfo);
	}
	
	return TRUE;
}


int  KSocialRecruitMgr::RefreshInfo(SocialRecruitInfo & infoDetail,const SocialRecruitBaseInfo & baseInfo)
{
	ServerSocialUnitMgr & mgr = ServerSocialUnitMgr::Singleton();
    SocialUnit * pUnit  = mgr.GetUnit(baseInfo.guid);
	if (pUnit)
	{
		infoDetail.baseInfo      = baseInfo;
		const char * szUnitName  = GetUnitName(pUnit->GetUnitAttr());
		
		if (szUnitName)
			strncpy(infoDetail.comInfo.szUnitName,szUnitName,sizeof(infoDetail.comInfo.szUnitName));
		
		infoDetail.comInfo.szUnitName[MAXSIZE_ORGNAME -1 ] = 0;
		
		const char * szOwnerName = pUnit->GetOwnerName();
		if (szOwnerName)
			strncpy(infoDetail.comInfo.szOwnerName,szOwnerName,sizeof(infoDetail.comInfo.szOwnerName));
		
		infoDetail.comInfo.szOwnerName[MAXSIZE_ROLENAME -1] = 0;
		
		int nPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(szOwnerName);
		if (IsValidPlayer(nPlayerIndex))
		{
			infoDetail.comInfo.bOwnerOnline = true;
		}//endif
		else
			infoDetail.comInfo.bOwnerOnline = false;	
		
		infoDetail.comInfo.nLayer         = pUnit->GetLayer();
		infoDetail.comInfo.nSubUnitCount  = pUnit->GetChildCount();
		infoDetail.tongInfo.nCityMapID    = -1;
		infoDetail.tongInfo.nPoolMapID    = -1;
		
		if (pUnit->GetLayer() == enSULayer_Tong)
		{
			infoDetail.tongInfo.nCityMapID   = INVALID_WORLD_ID;	
			SocialUnit     * pLeague         = pUnit->GetParent();

			if (pLeague)
			{
				SocialUnitAttr & leagueAttr      = pLeague->GetUnitAttr();
				infoDetail.tongInfo.nCityMapID = GetCityMapId(leagueAttr);
			}//endif
			//Reserve for pool here...
			infoDetail.tongInfo.nPoolMapID = GetPoolMapId(pUnit->GetUnitAttr());
		}//endif 
		
		return TRUE;
	}//end for if
	else
	{
		return FALSE;
	}//end else
}

void KSocialRecruitMgr::ProcessListData(const int nRow,IProcRet* pRet)
{
	_ASSERT(pRet && enSDBRecruit_Col_Num == pRet->GetColCount() );
	
	if( pRet == NULL || enSDBRecruit_Col_Num != pRet->GetColCount() )
		return;
	
	SocialRecruitBaseInfo info;
	
	pRet->GetData(nRow, enSDBRecruit_Col_ID, info.guid.data,sizeof(info.guid.data));
	pRet->GetData(nRow, enSDBRecruit_Col_Time, info.dueTime);

	if (info.dueTime > UNIX_TMIE_STAMP)
	{ 
        if(!AddInfoDirectly(info))
		{
            _ASSERT(false);
		}//endif

	}//endif
	else
	{
        ReqDBDelInfoRecord(info,-1);
	}//endelse

	if (mSortedGensInfo.size())
		CheckSortedList(mSortedGensInfo);
	
	if (mSortedTongInfo.size())
		CheckSortedList(mSortedTongInfo);
	
}

void KSocialRecruitMgr::CheckLoadingUnit()
{
	ServerSocialUnitMgr & mgr = ServerSocialUnitMgr::Singleton();
	
	std::list<FSGUID>::iterator it          = mLoadingUnitInfos.begin();
	bool                        bGensChange = false;
	bool                        bTongChange = false;
	
	while (it!= mLoadingUnitInfos.end())
	{
        SocialUnit * pUnit  = mgr.GetUnit(*it);

		if (pUnit)
		{
			std::map<FSGUID,SocialRecruitBaseInfo>::iterator recit= mRecruitInfos.find(*it);
			
			if (recit != mRecruitInfos.end())
			{
				SocialRecruitInfo          infoDetail;
				RefreshInfo(infoDetail,recit->second);
				
				if (pUnit->GetLayer() == enSULayer_Tong)
				{
					AddToSortedList(infoDetail,mSortedTongInfo);
					bTongChange = true;
				}//end for if
				else
				{
					AddToSortedList(infoDetail,mSortedGensInfo);
					bGensChange = true;
				}//end for else

			}//end for if

			mLoadingUnitInfos.erase(it++);
		}//endif
		else
			++it;
		
	}//end for whil

	if (bGensChange && mSortedGensInfo.size())
		mSortedGensInfo.sort();

	if (bTongChange && mSortedTongInfo.size())
		mSortedTongInfo.sort();
	
}

void KSocialRecruitMgr::AddToSortedList(const SocialRecruitInfo & info,std::list<SocialRecruitInfo> & destList)
{
	destList.push_front(info);
}

bool KSocialRecruitMgr::DelFromSortedList(const FSGUID & guid , std::list<SocialRecruitInfo> & destList)
{
    std::list<SocialRecruitInfo>::iterator it = destList.begin();
	
	while (it!=destList.end())
	{
		if ((*it).baseInfo.guid == guid)
		{
			destList.erase(it);
			return true;
		}//endif

		++ it;
	}//end for while

	return false;
}

int KSocialRecruitMgr::GetInfoList(const int iStartPage, const int nPageSize, const int nLayer , const int nBuffSize , S2C_GETRECRUIT_RET * pBuff)
{
	if (!mInited)
	{
		return enSocialErr_RetryLater;
	}//endif

	std::list<SocialRecruitInfo> * pDestList = NULL;

	switch (nLayer)
	{
	case enSULayer_Gens:
		{
			pDestList = &mSortedGensInfo;
		}//end for case 
		break;
	case enSULayer_Tong:	
		{
			pDestList = &mSortedTongInfo;
		}//end for case 
		break;

	default:_ASSERT(false);	
	}//end for switch 

	if (pDestList)
	{
		int nNum = pDestList->size();
		if (nNum > iStartPage * nPageSize)
		{
			pBuff->nPageNo         = iStartPage;
			int nInfoCurCount      = nNum - iStartPage * nPageSize;

			if (nInfoCurCount > nPageSize)
			{
				pBuff->nInfoCount  = nPageSize;	
			}
			else
				pBuff->nInfoCount  = nInfoCurCount;

			int nTotalSizeNeeded   = sizeof(S2C_GETRECRUIT_RET) - 1 + pBuff->nInfoCount * sizeof(S2C_SOCIAL_RECRUIT_INFO);
			pBuff->comHeader.proHeader.len = nTotalSizeNeeded - PROTOCOL_SIZE;
			
			if (nBuffSize < nTotalSizeNeeded)
			{
				_ASSERT(false);
				return enSocialErr_NoInfo;
			}//endif

			//Search the dest pos
			int iVisit      = 0;
			int iStartIndex = iStartPage * nPageSize;

			if  (iStartIndex >= pDestList->size() )
			{
				return enSocialErr_NoInfo;
			}//endif

			std::list<SocialRecruitInfo>::iterator iPos = pDestList->begin();
			while (iVisit != iStartIndex && iPos != pDestList->end())
			{
				iVisit ++ ;
				++iPos;
			}//end for while

			if (iVisit != iStartIndex)
			{
				_ASSERT(false);
				return enSocialErr_NoInfo;
			}//endif
			
			int                       nParsNum = 0;
			S2C_SOCIAL_RECRUIT_INFO * pInfo    = (S2C_SOCIAL_RECRUIT_INFO *)pBuff->data;

			while (nParsNum < pBuff->nInfoCount && iPos!=pDestList->end())
			{
				SocialRecruitInfo & pTemp = *iPos;
				pInfo->dueTime  = ( (int) ( pTemp.baseInfo.dueTime - UNIX_TMIE_STAMP) )/86400;
				pInfo->comInfo  = pTemp.comInfo;
				pInfo->tongInfo = pTemp.tongInfo;

				++iPos; 
				pInfo    ++ ;
				nParsNum ++ ;
			}//end for while

			if (nParsNum != pBuff->nInfoCount)
			{
				_ASSERT(false);
				return enSocialErr_NoInfo; 
			}//endif

			//Compression begin .......................................................................
			int                nOldSize = pBuff->nInfoCount * sizeof(S2C_SOCIAL_RECRUIT_INFO);
			static const int   BUF_SIZE =  8192;
			unsigned char szCompressionBuff[BUF_SIZE];
			unsigned int nLen           = BUF_SIZE;
			lzo1x_1_compress( 
				(const unsigned char *)pBuff->data,
				nOldSize,
				szCompressionBuff,
				&nLen,
				wrkmem);
			
			if (nLen >= BUF_SIZE - sizeof(S2C_GETRECRUIT_RET) )
				return enSocialErr_NoInfo;
			
			memcpy(pBuff->data,szCompressionBuff,nLen);          
	        pBuff->comHeader.proHeader.len = nLen + sizeof(S2C_GETRECRUIT_RET) - 1 - PROTOCOL_SIZE;
			//Compression end   .......................................................................

			return enSocialErr_None;
		}//endif

	}//endif

	return enSocialErr_NoInfo;
}

