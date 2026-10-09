//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:11   9:46
//      File_base        : ClientSocialUnitMgr
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

#ifndef _ClientSocialUnitMgr_h
#define _ClientSocialUnitMgr_h

#include "SocialComDef.h"
#include "GameDataDef.h"
#include "KSimulation.h"

#define   MAX_ANOUNCEMENT_LAYER_NUM   3
#define   INVALID_ANOUNCEMENT_VERSION 0xffffffff

class ClientSocialUnitMgr : public IProtocolSimulation
{
public:
	ClientSocialUnitMgr();

	// Inherit from base interface
	virtual void Breathe() {};
	virtual void onCreate(UIMDLEvent& rEvent) {};
	virtual void onRelease(UIMDLEvent& rEvent) {};
	virtual void onChange(UIMDLEvent& rEvent);

public:
	bool	Init();
	void	ProcessProtocol(void *pData);
	void	ReqConfirmRetFromUI(int nParam);
	void    SetCityTaxRateNormal(int nNewTax);
	void    NotifyChangeRelation(void);

	void    ReqUnitInfo(const char * szOwnerName,int nLayer,const TongUIReqeustCode & code);
	
private:
	void	CreateUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	AddSubUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	RemoveSubUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	BreakUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	LeaveUnitReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	ForbidChatReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	UnForbidChatReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	PubAnnouncementReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	GetSubListReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	GetAnnouncementReq(const UI2CORE_REQ_PARAM &ui2coreParam);
	void    ChangeUnitOwner(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	SetCityRes(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	GetCityRes(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	ReqCityInfo(const UI2CORE_REQ_PARAM &ui2coreParam);
	void	SetCityTaxRate(const UI2CORE_REQ_PARAM &ui2coreParam);
	void    ReqJoinUnit(const UI2CORE_REQ_PARAM &ui2coreParam);
	void    ReqRecruitInfo(const UI2CORE_REQ_PARAM &ui2coreParam);
	void    ReqAddRecruit (const UI2CORE_REQ_PARAM &ui2coreParam);
	void    ReqDelRecruit (const UI2CORE_REQ_PARAM &ui2coreParam);

	void	s2cMsgCode(const void *ps2cRet);
	void	s2cMsg(const void *ps2cRet);
	void	s2cReqToConfirm(const void *ps2cRet);
	void	s2cUnitOperation(const void *ps2cRet);
	void	GetSubListRet(const void *ps2cRet);
	void	GetAnnouncementRet(const void *ps2cRet);
	void	GetCityInfoRet(const void *ps2cRet);
	void    GetUnitInfoRet(const void *ps2cRet);
	void    GetRecruitInfoRet(const void * ps2cRet);
	
	void    BroadCastFromServer(const void * ps2cBroadCast);
	
private:
	void	BindOpe2Proc();
	void	BindNetRetProc();
	IUIMDLDataset*	GetDataSet(const char *szName);

private:
	ClientSocialUnitMgr(const ClientSocialUnitMgr &rhs);
	ClientSocialUnitMgr& operator= (const ClientSocialUnitMgr &rhs);

	typedef map<BYTE, SU_COMOPE_PARAM>		DETAILID2INFO;

	typedef	void (ClientSocialUnitMgr::*POPEFUNC)(const UI2CORE_REQ_PARAM	&ui2coreParam);
	typedef void (ClientSocialUnitMgr::*PNETRETPROC)(const void *ps2cMsg);

private:
	static	DETAILID2INFO	m_detailId2Info;
	static	POPEFUNC		m_opeFuncs[enSUO_Num];
	static	PNETRETPROC		m_netRetProc[enSRProtocol_Num];

	typedef struct _UnConfirm_Req
	{
		BYTE	isInUse;
		WORD	nSize;
		char	data[MAXSIZE_UNCONFIRMREQ];

	} UNCONFIRM_REQ;

	UNCONFIRM_REQ			m_UnConfirmReq;	
    char                    m_TempAnoucementRecord[MAX_ANOUNCEMENT_LAYER_NUM][MAXSIZE_ANNOUNCEMENT];
	int                     m_TempAnoucementVersion[MAX_ANOUNCEMENT_LAYER_NUM];
};

inline ClientSocialUnitMgr::ClientSocialUnitMgr()
{
	m_UnConfirmReq.isInUse = 0;
}

#endif