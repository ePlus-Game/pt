#ifndef INSRUANCE_MGR_H
#define INSRUANCE_MGR_H

//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 04/18/2008 10:48
//      File_base        : insurance_mgr
//      File_ext         : h
//      Author           : Brianyao (Yaojie)
//      Description      : 保险管理 (For Every Player)
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#define GET_MONEY_ALL_PARAM -1
#include "ILogSystem.h"
#include "insurance_common.h"

#ifndef _SERVER
#include "GameDataDef.h"
#endif

class InsuranceMgr
{
	int  m_nCurrentInsuranceValue;   //目前的保险金额(单位：通宝)
	int  m_nTotalMoneyGot;           //已经领取的金钱(单位：铜) 
	int  m_nMoneyLeftToGet;          //尚未领取的金钱(单位：铜)
	
	int  m_nPlayerIndex;

    #ifndef _SERVER
	bool m_nInfoReadyFlag;
    #endif

public:
	InsuranceMgr(void);
	~InsuranceMgr(void);

	void Init(int nPlayerIndex);

#ifdef _SERVER 
	//Server Code ...........................................................................
public:
	void		ProcessProcotol(void *pNetMsg, int nMsgSize);
	void        ProcessPaysysProtocol(int nInsuranceValue);

	int         LoadDBRet(const int nCurrentInsuranceValue,const int nTotalMoneyGot,const int nMoneyLeftToGet);

public:
	int         AddInsuranceValue( const int nRequire );
	int         GetRemainReward( const int nMoney = GET_MONEY_ALL_PARAM);
	int         NotifyPlayerLevelAddTo( const int nLevel );
private:
	void        ImediatelyReward(const int nInsuraceValue);
	void        PreviouseReward(const int nInsuraceValue);
	bool        AddRemainMoneyToGet(const int nMoneyAdd,LogEvent logEvent);
private:
	void        SyncInsuranceInfo(void);
	void        SyncOpCode(int nOpCode);
private:
	int         SaveDBReq(void);
	bool        IBBuyReq(const int nInsuranceValue);

#else 
	//Client Code ............................................................................
public:
	void        ProcessProcotol(void * pNetMsg);
	void        AddInsuranceValueReq(const int nReqValue);
	void        GetRemainRewardReq(void);
	void        CalcReward(const int nDeltaValue,UIInsuranceCalc * pCalc);
	void        CalcFutherReward(UIInsuranceCalc * pCalc,int nNewInsuranceValue = -1);
private:
	void        NotifyInsuranceInfo(void * pNetMsg);
	void        NotifyOperatorCode(void * pNetMsg);
	void        ShowErorrMsg(InsuranceOpCode code);
private:
	void        GenNotifyMsg(void * pNetMsg);

#endif
	//Common Code.............................................................................

public:
	int         GetCurrentInsuranceValue(void)const;
	int         GetTotalMoneyGot(void)const;
	int         GetMoneyLeftToGet(void)const;
private:
	InsuranceMgr(const InsuranceMgr &);
	const InsuranceMgr & operator = (const InsuranceMgr &);
};

#endif