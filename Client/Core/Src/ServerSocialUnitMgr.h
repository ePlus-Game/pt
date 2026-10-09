//////////////////////////////////////////////////////////////////////
// 
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:11   9:44
//      File_base        : ServerSocialUnitMgr
//      File_ext         : h
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

#ifndef _ServerSocialUnitMgr_h
#define _ServerSocialUnitMgr_h

#include "SocialComDef.h"
#include "CoreRelated.h"
#include "SocialUnit.h"
#include "SocialAllocator.h"
#include "SocialSerializer.h"

class ServerSocialUnitMgr
{
public:
	ServerSocialUnitMgr();
	~ServerSocialUnitMgr();

	static ServerSocialUnitMgr&	Singleton();

	void		ProcessProcotol(int nPlayerIdx, void *pNetMsg, int nMsgSize);
	void		PlayerOnLine(int nPlayerIdx);
	void		PlayerOffLine(int nPlayerIdx);
	void		AddUnit(const FSGUID &guid, SocialUnit *pUnit);
	void		RemoveUnit(const FSGUID &guid, int nTplId = enSUTplId_Tong);
	void		SaveUnitsRelToPlayer(int nPlayerIdx);

	SocialUnit*	GetUnit(const FSGUID &guid, int nTplId = enSUTplId_Tong);
	int			GetCityMapIdx(int nPlayerIdx);

	void		ProcessDBOpeRet(
					int nDbOpeRst, 
					int nPlayerIdx, 
					IProcRet* pRet );

	void        ProcessGlobalDBRet(
		            int nDbOpeRst, 
		 			IProcRet* pRet
		);


	void        CheckPoolAttr(void);
	void        OnMapPoolLoadReady(void);
private:
	
	void		UnitOperationReq(int nLauncherIdx, void *pParam, int nMsgSize);
	void		ReqConfirmRet(int nLauncherIdx, void *pParam, int nMsgSize);
	void		SetCityResReq(int nLauncherIdx, void *pParam, int nMsgSize);
	void		GetCityResReq(int nLauncherIdx, void *pParam, int nMsgSize);
	void		GetCityInfoReq(int nLauncherIdx, void *pParam, int nMsgSize);
	void		SetCityTaxRateReq(int nLauncherIdx, void *pParam, int nMsgSize);
	void        ReqUnitInfo(int nLauncherIdx, void *pParam, int nMsgSize);

	void		CreatePlayerUnit(int nPlayerIdx, int nTplId);

	// Process DB operation return
	void		LoadOwnTreeUnitRet(int nDbOpeRst, int nPlayerIdx, _SocialDBHeader* pHeader, IProcRet* pRet );
	void		LoadAllSubUnitRet(int nDbOpeRst, int nPlayerIdx, _SocialDBHeader* pHeader, IProcRet* pRet );
	void		DBAddUnitRet(int nDbOpeRst, int nPlayerIdx, _SocialDBHeader* pHeader, IProcRet* pRet );
	void		CheckUnitNameRet(int nDbOpeRst, int nPlayerIdx, _SocialDBHeader* pHeader, IProcRet* pRet );

	// Assistant functions
	SocialUnit*	ConstructUnit(int nPlayerIdx, IProcRet* pRet, int nRow, bool &bIsNew);
	void		ParseUnitBloc(int nPlayerIdx, char *pBloc, int nBlocSize, SocialUnit *pParent);
	void		OnLoadOwnTreeFinished(const FSGUID &parentGuid, int nTplId);	
	SocialUnit*	GetCityUnit(int nPlayerIdx);

	void        CheckPoolAttr(SocialUnit * pUnit);
private:
/*	ServerSocialUnitMgr(const ServerSocialUnitMgr &rhs);
	ServerSocialUnitMgr& operator= (const ServerSocialUnitMgr &rhs);
*/	
 	typedef	map<FSGUID, SocialUnit*>	GUID2SOCIALUNIT;
	typedef void (ServerSocialUnitMgr::*PNETPROC)(int nPlayer, void *pParam, int nMsgSize);
	
	typedef void (ServerSocialUnitMgr::*PDBRETPROC)(int nDBOpeRst, 
													int nPlayerIdx, 
													_SocialDBHeader* pHeader,
													IProcRet* pRet );

private:
	GUID2SOCIALUNIT		m_guid2SocialUnit[enSUTplId_Num];
	PNETPROC			m_netProc[enSRProtocol_Num];
	PDBRETPROC			m_dbRetProc[enSoc_DBOpe_Num];

	bool                m_IsMapLordLoadedReady;
	bool                m_IsMapPoolLoadedReady;
};

inline SocialUnit*	ServerSocialUnitMgr::GetUnit(const FSGUID &guid, int nTplId /* = enSUTplId_Tong */)
{
	if( !IsTplIdValid(nTplId) )
		return NULL;

	GUID2SOCIALUNIT::iterator	itUnit = m_guid2SocialUnit[nTplId].find(guid);

	return itUnit == m_guid2SocialUnit[nTplId].end() ? NULL : itUnit->second;
}

inline	void ServerSocialUnitMgr::AddUnit(const FSGUID &guid, SocialUnit *pUnit)
{
	_ASSERT(pUnit);

	if(pUnit)
	{
		pair<GUID2SOCIALUNIT::iterator, bool> ret;
		ret = m_guid2SocialUnit[pUnit->GetTplId()].insert( GUID2SOCIALUNIT::value_type(guid, pUnit) );

		_ASSERT(ret.second);
	}
}

inline void	ServerSocialUnitMgr::RemoveUnit(const FSGUID &guid, int nTplId /* = enSUTplId_Tong */)
{
	if( IsTplIdValid(nTplId) )
	{
		GUID2SOCIALUNIT::iterator	itUnit = m_guid2SocialUnit[nTplId].find(guid);

		if( itUnit != m_guid2SocialUnit[nTplId].end() )
		{
			SocialAllocator::FreeUnit(itUnit->second);
			m_guid2SocialUnit[nTplId].erase(itUnit);
		}
	}
}

inline void ServerSocialUnitMgr::ProcessDBOpeRet(
			int nDbOpeRst, 
			int nPlayerIdx, 
			IProcRet* pRet )
{
	int nPassBySize = 0;
	char* pPassBy = pRet->GetPassBy( nPassBySize );

	if( nPassBySize != sizeof(_SocialDBHeader) || 
		pPassBy == NULL )
		return;

	_SocialDBHeader* Header = (_SocialDBHeader*)pPassBy;

	int uDBOpeType = Header->nOp;

	_ASSERT(uDBOpeType > enSoc_DBOpe_Begin && uDBOpeType < enSoc_DBOpe_LastUsed);

	if(uDBOpeType > enSoc_DBOpe_Begin && uDBOpeType < enSoc_DBOpe_LastUsed)
	{
		if(m_dbRetProc[uDBOpeType - enSoc_DBOpe_Begin - 1])
			(this->*m_dbRetProc[uDBOpeType - enSoc_DBOpe_Begin - 1])(nDbOpeRst, 
																	 nPlayerIdx, 
																	 Header,
																	 pRet );
	}
}

inline SocialUnit*	ServerSocialUnitMgr::GetCityUnit(int nPlayerIdx)
{
	RelationSet	&relationSet = GetRelationSet(nPlayerIdx);
	SocialUnit *pLeafUnit = GetLeafUnit(nPlayerIdx, enSUTplId_Tong);
	SocialUnit *pTongUnit = GetUpNUnit(pLeafUnit, enSULayer_League);
	
	return pTongUnit;
}

#endif