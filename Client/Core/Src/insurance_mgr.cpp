#include "KCore.h"
#include "KPlayer.h"
#include "insurance_common.h"
#include "insurance_mgr.h"
#include "CoreRelated.h"
#include "ConfigManager.h"
#include "IBShopProtocol.h"
#include "IBShopUtil.h"
#include "IBShopComDef.h"
#include "KItemGenerator.h"
#include "OnceIBItemMgr.h"
#include "IBCenter_S.h"

InsuranceMgr::InsuranceMgr()
:m_nCurrentInsuranceValue(0),m_nTotalMoneyGot(0),m_nMoneyLeftToGet(0),m_nPlayerIndex(INVALID_PLAYER_INDEX)

#ifndef _SERVER
,m_nInfoReadyFlag(false)
#endif

{}

InsuranceMgr::~InsuranceMgr()
{}

void InsuranceMgr::Init(int nPlayerIndex)
{
	m_nCurrentInsuranceValue = 0;
	m_nTotalMoneyGot         = 0;
	m_nMoneyLeftToGet        = 0;
	m_nPlayerIndex           = nPlayerIndex;
#ifndef _SERVER
	m_nInfoReadyFlag         = false;
#endif
}

int InsuranceMgr::GetCurrentInsuranceValue()const
{
	return m_nCurrentInsuranceValue;
}

int InsuranceMgr::GetMoneyLeftToGet()const
{
	return m_nMoneyLeftToGet;
}

int InsuranceMgr::GetTotalMoneyGot()const
{
	return m_nTotalMoneyGot;
}

#ifdef _SERVER 

void InsuranceMgr::ProcessProcotol( void *pNetMsg, int nMsgSize)
{
	if (!InsuranceSettingMgr::Singleton().IsEnabled())
		return;

	if (IsValidPlayer(m_nPlayerIndex))
	{
		if ( nMsgSize < sizeof(C2S_INSURANCE_PROTOCOL))
			return;

		C2S_INSURANCE_PROTOCOL * pClientOp = (C2S_INSURANCE_PROTOCOL *) pNetMsg;
		if (pClientOp->nProtocol != c2s_insurance)
			return ; 

		int nRetCode = insurance_error_none;

	    switch (pClientOp->nSubProtocol)
		{
		case c2s_add_insurance_value:
			nRetCode = AddInsuranceValue(pClientOp->nParam);
			break;

		case c2s_get_insurance_money:
			nRetCode = GetRemainReward();
			break;

		}//end for switch

		if (nRetCode != insurance_error_none )
			SyncOpCode(nRetCode);

	}//endif

}

int InsuranceMgr::SaveDBReq()
{
	if (IsValidPlayer(m_nPlayerIndex))
	{
		Player[m_nPlayerIndex].SaveInsurance();
	}//endif

	return TRUE;
}

int  InsuranceMgr::LoadDBRet(const int nCurrentInsuranceValue,const int nTotalMoneyGot,const int nMoneyLeftToGet)
{
	if (nCurrentInsuranceValue >= 0 && nTotalMoneyGot >= 0 && nMoneyLeftToGet >= 0 )
	{
		m_nCurrentInsuranceValue = nCurrentInsuranceValue;
		m_nTotalMoneyGot         = nTotalMoneyGot;
		m_nMoneyLeftToGet        = nMoneyLeftToGet;
		
		if (InsuranceSettingMgr::Singleton().IsEnabled())
			SyncInsuranceInfo();

	}//endif
	else
	{
		_ASSERT(false);
	}//end else
	
	return TRUE;
}


bool InsuranceMgr::IBBuyReq(const int nInsuranceValue)
{
	ClientBuyGoods goods;
	memset(&goods,0,sizeof(goods));
	KOnceIBItemMgr::GetSingleten().GetOnceItemParam(insrance,goods);

	C2S_BUYGOODS c2sReq;
	c2sReq.header.protocol    = c2s_IB_family;
	c2sReq.header.subProtocol = enIB_CSProt_BuyGoods;
	c2sReq.ShopIdx            = goods.shopIdx;
	c2sReq.ShopVersion        = 0;
	c2sReq.Goods              = goods.goods;
	c2sReq.ShelfIdx           = (BYTE)goods.shelfIdx;
	c2sReq.ShelfVersion       = 0;
	c2sReq.Price              = nInsuranceValue * 100; /*Í¨±¦*/
	c2sReq.ReqNum             = 1;
	c2sReq.useTicket          = false;
	c2sReq.header.len         = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.bOnecItem          = true;
	c2sReq.Goods.ID           = TRADE_ID_INSURANCE;

	IBCenter_S::Singleton().ProcClientProtocol(m_nPlayerIndex,(unsigned char *)&c2sReq,sizeof(c2sReq));
	//TestCode here
    //ProcessPaysysProtocol(nInsuranceValue);
	return true;
}

void InsuranceMgr::ProcessPaysysProtocol(int nInsuranceValue)
{
	if (!InsuranceSettingMgr::Singleton().IsEnabled())
		return;

	if (IsValidPlayer(m_nPlayerIndex) && nInsuranceValue > 0)
	{		
		ImediatelyReward(nInsuranceValue);
	
		if (m_nCurrentInsuranceValue == 0)    //The first time reward.
			PreviouseReward(nInsuranceValue);

		m_nCurrentInsuranceValue += nInsuranceValue;

		SyncInsuranceInfo();
		SaveDBReq();
	}//endif
}

void InsuranceMgr::ImediatelyReward(const int nInsuraceValue)
{
	int nRate = InsuranceSettingMgr::Singleton().GetImmediateRewardRate();
	
	if (nRate > 0)
	{
		int nRewardInsurance = nInsuraceValue * nRate / 100;
		int nRewardMoney     = InsuranceSettingMgr::Singleton().InsuranceToJinShanBi(nRewardInsurance);

		if ( nRewardMoney > 0 )
			AddRemainMoneyToGet(nRewardMoney,log_event_insurance_get_one_time_reward);
	}//endif
	
}

void InsuranceMgr::PreviouseReward(const int nInsuraceValue)
{
	int   nTotalInsuranceAdd = 0;
	
	int   nPlayerLevel   = GetPlayerLevel(m_nPlayerIndex);
	for  (int nLevel     = 1;nLevel < nPlayerLevel; nLevel ++ )
	{
		int  nRate       = InsuranceSettingMgr::Singleton().GetInsuranceRateByLevel(nLevel);
		
		if ( nRate > 0 )
		{
			nTotalInsuranceAdd += nInsuraceValue * nRate / 100;
		}//endif
		
	}//end for nLevel
	
	int   nTotalMoneyAdded = InsuranceSettingMgr::Singleton().InsuranceToJinShanBi(nTotalInsuranceAdd);
	
	if (nTotalMoneyAdded > 0)
		AddRemainMoneyToGet(nTotalMoneyAdded,log_event_insurance_get_one_time_reward);
}

int  InsuranceMgr::NotifyPlayerLevelAddTo(const int nLevel)
{
	if (!InsuranceSettingMgr::Singleton().IsEnabled())
		return 0 ;

	if (!IsValidPlayer(m_nPlayerIndex))
		return 0;

	if (m_nCurrentInsuranceValue == 0)
		return 0;

	int nRate = InsuranceSettingMgr::Singleton().GetInsuranceRateByLevel(nLevel);
	if (nRate > 0)
	{
		int nInsuranceAdded   = m_nCurrentInsuranceValue * nRate / 100;
		int nMoneyAdded       = InsuranceSettingMgr::Singleton().InsuranceToJinShanBi(nInsuranceAdded);
		
		if (AddRemainMoneyToGet(nMoneyAdded,log_event_insurance_get_level_up))
		{
			if (nMoneyAdded)
				SyncInsuranceInfo();
			
			return nMoneyAdded;	
		}//endif
		else
			return 0;

	}//endif
	else
		return 0;
}

void  InsuranceMgr::SyncInsuranceInfo()
{
	S2C_INSURANCE_BASE_INFO info;
	info.nProtocol     = s2c_insurance;
	info.nSubProtocol  = s2c_insurance_info_sync;
	info.nLen          = sizeof(info) - PROTOCOL_SIZE;

	info.nCurrentInsuranceValue = m_nCurrentInsuranceValue;
	info.nTotalMoneyGot         = m_nTotalMoneyGot;
	info.nMoneyLeftToGet        = m_nMoneyLeftToGet;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].GetNetConnectIdx(), (BYTE*)&info, sizeof(info));
}

void  InsuranceMgr::SyncOpCode(int nOpCode)
{
	S2C_INSURANCE_OP_CODE code;
	code.nProtocol     = s2c_insurance;
	code.nSubProtocol  = s2c_insurance_op_code;
	code.nLen          = sizeof(code) - PROTOCOL_SIZE;
	code.nOpCode       = nOpCode;

	if (g_pServer != NULL)
		g_pServer->PackDataToClient(Player[m_nPlayerIndex].GetNetConnectIdx(), (BYTE*)&code, sizeof(code));
}

bool  InsuranceMgr::AddRemainMoneyToGet(const int nMoneyAdd,LogEvent logEvent)
{
	if (m_nMoneyLeftToGet + nMoneyAdd >= m_nMoneyLeftToGet) //Forbid overflow
	{
		m_nMoneyLeftToGet += nMoneyAdd;

	    //Log Needed here.............................................................
		if (nMoneyAdd >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_insurance_remain_money_add_amount) && g_pLogSystem)
		{
			LogEventParam addRemainMoney;
			addRemainMoney.event  = logEvent;
			addRemainMoney.param1 = Player[m_nPlayerIndex].GetGUID();
			addRemainMoney.param4 = nMoneyAdd;
			g_pLogSystem->Log(addRemainMoney);
		}//endif
	    //Log End ....................................................................
		return true;
	}//endif
	else
	{
		//int overflow !! log here
		_ASSERT(false);
		return false;
	}//end for else
}

int  InsuranceMgr::GetRemainReward(const int nMoney /* = GET_MONEY_ALL_PARAM */)
{
	if (!InsuranceSettingMgr::Singleton().IsEnabled())
		return insurance_error_fetch_failed ;
	
	if (!IsValidPlayer(m_nPlayerIndex))
		return insurance_error_fetch_failed;
	
	int nMoneyReq = nMoney;

	if (nMoneyReq == GET_MONEY_ALL_PARAM)
		nMoneyReq = m_nMoneyLeftToGet;

	if (nMoneyReq <= 0)
		return insurance_error_fetch_failed;

	if (nMoneyReq > m_nMoneyLeftToGet)
		return insurance_error_fetch_not_enough;

	if (m_nTotalMoneyGot + nMoneyReq < m_nTotalMoneyGot) // int overflow
		return insurance_error_fetch_too_much;

	if (!Player[m_nPlayerIndex].Earn(nMoneyReq))
		return insurance_error_fetch_too_much;
	else
	{
		//Log Needed here..............................................
		if (nMoneyReq >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_insurance_fetch_money) && g_pLogSystem)
		{
			LogEventParam addRemainMoney;
			addRemainMoney.event  = log_event_insurance_fetch_money;
			addRemainMoney.param1 = Player[m_nPlayerIndex].GetGUID();
			addRemainMoney.param4 = nMoneyReq;
			g_pLogSystem->Log(addRemainMoney);
		}//endif
		//Log End .....................................................	
	}

	m_nMoneyLeftToGet -= nMoneyReq;
	m_nTotalMoneyGot  += nMoneyReq;

	SaveDBReq();
	SyncInsuranceInfo();

	return insurance_error_none;
}

int  InsuranceMgr::AddInsuranceValue(const int nRequire )
{
	if (!InsuranceSettingMgr::Singleton().IsEnabled())
		return insurance_error_add_failed;
	
	if (!IsValidPlayer(m_nPlayerIndex))
		return insurance_error_add_failed;
	
	int nPlayerLevel = GetPlayerLevel(m_nPlayerIndex) ;
	
	if (   nPlayerLevel <= InsuranceSettingMgr::Singleton().GetMinBuyInsuranceLevel() 
		|| nPlayerLevel >= InsuranceSettingMgr::Singleton().GetMaxBuyInsuranceLevel() 
		)
	{
		return insurance_error_add_wrong_level;
	}//endif
	
	
	if (nRequire <= 0 || nRequire < InsuranceSettingMgr::Singleton().GetMinInsuranceValue() )
		return insurance_error_add_min_lower;
	
	if ( m_nCurrentInsuranceValue >= InsuranceSettingMgr::Singleton().GetMaxInsuranceValue()
		|| m_nCurrentInsuranceValue + nRequire <  m_nCurrentInsuranceValue 
		|| m_nCurrentInsuranceValue + nRequire > InsuranceSettingMgr::Singleton().GetMaxInsuranceValue()
		)
		return insurance_error_add_max_exeed;
	
	if (!IBBuyReq(nRequire))
		return insurance_error_add_pay_failed;
	
	return insurance_error_none;
}



#else

void  InsuranceMgr::ProcessProcotol(void * pNetMsg)
{
	S2C_INSURANCE_COMMON_HEADER * pHeader = (S2C_INSURANCE_COMMON_HEADER *)pNetMsg;
	switch (pHeader->nSubProtocol)
	{
	case s2c_insurance_info_sync:
		{
			NotifyInsuranceInfo(pNetMsg);
		}//end for case
		break;

	case s2c_insurance_op_code:
		{
			NotifyOperatorCode(pNetMsg);
		}//end for case
		break;
	}//end for switch

}

void  InsuranceMgr::GenNotifyMsg(void * pNetMsg)
{
	S2C_INSURANCE_BASE_INFO * pInfo = (S2C_INSURANCE_BASE_INFO *) pNetMsg;
	
	//MsgNotify here......................................................
	
	if (pInfo->nCurrentInsuranceValue > m_nCurrentInsuranceValue)
	{
		const char * szMoneyAdded     = InsuranceSettingMgr::Singleton().GetNotifyCodeString(2);
		if (szMoneyAdded)
		{
			char szNotifyMsg[256]="";
			sprintf(szNotifyMsg,szMoneyAdded,pInfo->nCurrentInsuranceValue -  m_nCurrentInsuranceValue);
			CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)szNotifyMsg, 0);
		}//endif
	}
	
	if (pInfo->nMoneyLeftToGet > m_nMoneyLeftToGet)
	{
		int nAdded     =  pInfo->nMoneyLeftToGet  - m_nMoneyLeftToGet;
		if (nAdded)
		{
			int nAddedJin  =  nAdded / 10000;
			int nAddedYin  =  nAdded / 100 - nAddedJin * 100;
			int nAddedTong =  nAdded % 100;
			
			char         szNotifyMsg[512] = "";
		    const char * szMoneyAdded     = InsuranceSettingMgr::Singleton().GetNotifyCodeString(3);
			
			if (szMoneyAdded)
			{
				char szMoneyString[256] = "";
				if (nAddedJin)
				{
					char szJin[64] = "";
					sprintf(szJin,InsuranceSettingMgr::Singleton().GetNotifyCodeString(10),nAddedJin);
					strcat(szMoneyString,szJin);
				}//endif

				if (nAddedYin)
				{
					char szYin[64] = "";
					sprintf(szYin,InsuranceSettingMgr::Singleton().GetNotifyCodeString(11),nAddedYin);
					strcat(szMoneyString,szYin);
				}//endif
				
				if (nAddedTong)
				{
					char szTong[64] = "";
					sprintf(szTong,InsuranceSettingMgr::Singleton().GetNotifyCodeString(12),nAddedTong);
					strcat(szMoneyString,szTong);
				}//endif

				sprintf(szNotifyMsg,szMoneyAdded,szMoneyString);
				CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)szNotifyMsg, 0);

			}//endif

		}//endif
	
	}//endif

	if (pInfo->nTotalMoneyGot > m_nTotalMoneyGot)
	{
		int nAdded     =  pInfo->nTotalMoneyGot  - m_nTotalMoneyGot;
		if (nAdded)
		{
			int nAddedJin  =  nAdded / 10000;
			int nAddedYin  =  nAdded / 100 - nAddedJin * 100;
			int nAddedTong =  nAdded % 100;
			
			char         szNotifyMsg[512] = "";
			const char * szMoneyAdded     = InsuranceSettingMgr::Singleton().GetNotifyCodeString(1);
			
			if (szMoneyAdded)
			{
				char szMoneyString[256] = "";
				if (nAddedJin)
				{
					char szJin[64] = "";
					sprintf(szJin,InsuranceSettingMgr::Singleton().GetNotifyCodeString(10),nAddedJin);
					strcat(szMoneyString,szJin);
				}//endif
				
				if (nAddedYin)
				{
					char szYin[64] = "";
					sprintf(szYin,InsuranceSettingMgr::Singleton().GetNotifyCodeString(11),nAddedYin);
					strcat(szMoneyString,szYin);
				}//endif
				
				if (nAddedTong)
				{
					char szTong[64] = "";
					sprintf(szTong,InsuranceSettingMgr::Singleton().GetNotifyCodeString(12),nAddedTong);
					strcat(szMoneyString,szTong);
				}//endif
				
				sprintf(szNotifyMsg,szMoneyAdded,szMoneyString);
				CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)szNotifyMsg, 0);
				
			}//endif
			
		}//endif
	}
}

void  InsuranceMgr::NotifyInsuranceInfo(void * pNetMsg)
{
	S2C_INSURANCE_BASE_INFO * pInfo = (S2C_INSURANCE_BASE_INFO *) pNetMsg;
	
	if (!m_nInfoReadyFlag)
		m_nInfoReadyFlag = true;
	else
		GenNotifyMsg(pNetMsg);

	m_nCurrentInsuranceValue        = pInfo->nCurrentInsuranceValue;
	m_nMoneyLeftToGet               = pInfo->nMoneyLeftToGet;
	m_nTotalMoneyGot                = pInfo->nTotalMoneyGot;


	UIInsuranceInfo                 info;
	info.m_nCurrentInsuranceValue   = m_nCurrentInsuranceValue;
	info.m_nMoneyLeftToGet          = m_nMoneyLeftToGet;
	info.m_nTotalMoneyGot           = m_nTotalMoneyGot;

	CoreDataChanged(GDCNI_INSURANCE_INFO,(unsigned int)&info,0);
}

void  InsuranceMgr::NotifyOperatorCode(void * pNetMsg)
{
	S2C_INSURANCE_OP_CODE  *  pCode = (S2C_INSURANCE_OP_CODE   *) pNetMsg;
	const char             *  pErrorCode = InsuranceSettingMgr::Singleton().GetErrorCodeString((InsuranceOpCode)pCode->nOpCode);
	if (pErrorCode)
	{
		CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)pErrorCode, 0);
	}//endif
}

void InsuranceMgr::ShowErorrMsg(InsuranceOpCode code)
{
	const char             *  pErrorCode = InsuranceSettingMgr::Singleton().GetErrorCodeString(code);
	if (pErrorCode)
	{
		CoreDataChanged( GDCNI_ERROR_MESSAGE, (unsigned int)pErrorCode, 0);
	}//endif
}

void InsuranceMgr::AddInsuranceValueReq(const int nRequire)
{
	int nPlayerLevel = GetClientPlayer().GetLevel() ;
	
	if (   nPlayerLevel <= InsuranceSettingMgr::Singleton().GetMinBuyInsuranceLevel() 
		|| nPlayerLevel >= InsuranceSettingMgr::Singleton().GetMaxBuyInsuranceLevel() 
		)
	{
		ShowErorrMsg(insurance_error_add_wrong_level);
		return ;
	}//endif
	
	
	if (nRequire <= 0 || nRequire < InsuranceSettingMgr::Singleton().GetMinInsuranceValue() )
	{
		ShowErorrMsg(insurance_error_add_min_lower);
		return;
	}//endif

	if ( m_nCurrentInsuranceValue >= InsuranceSettingMgr::Singleton().GetMaxInsuranceValue()
		|| m_nCurrentInsuranceValue + nRequire <  m_nCurrentInsuranceValue 
		|| m_nCurrentInsuranceValue + nRequire > InsuranceSettingMgr::Singleton().GetMaxInsuranceValue()
		)
	{
		ShowErorrMsg(insurance_error_add_max_exeed);
		return ;
	}//endif

	C2S_INSURANCE_PROTOCOL operation;
	operation.nProtocol    = c2s_insurance;
	operation.nSubProtocol = c2s_add_insurance_value;
	operation.nParam       = nRequire;
	operation.nLen         = sizeof(operation) - PROTOCOL_SIZE;
	
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&operation,sizeof(operation));
}

void InsuranceMgr::CalcReward(const int nDeltaValue,UIInsuranceCalc * pCalc)
{
	if (pCalc == NULL)
		return;

	ZeroMemory(pCalc,sizeof(UIInsuranceCalc));

	if ( nDeltaValue <= 0)
		return;

	//Immediately reward calc .................................................................................
	int nImaedateRate = InsuranceSettingMgr::Singleton().GetImmediateRewardRate();
	
	if (nImaedateRate > 0)
	{
		int nRewardInsurance         = nDeltaValue * nImaedateRate / 100;
		int nRewardMoney             = InsuranceSettingMgr::Singleton().InsuranceToJinShanBi(nRewardInsurance);

		pCalc->m_nImediatelyReward   = nRewardMoney;
	}//endif
	
	//Previeos reward calc .....................................................................................
	int   nPrevieosAdd = 0;
	
	if ( m_nCurrentInsuranceValue == 0 ) //The first time reward
	{
		int   nTotalInsuranceAdd = 0;	
		int   nPlayerLevel       = GetClientPlayer().GetLevel();
		for  (int nLevel     = 1; nLevel < nPlayerLevel ; nLevel ++ )
		{
			int  nRate       = InsuranceSettingMgr::Singleton().GetInsuranceRateByLevel(nLevel);
			
			if ( nRate > 0 )
			{
				nTotalInsuranceAdd += nDeltaValue * nRate / 100;
			}//endif
			
		}//end for nLevel
		
		int   nTotalMoneyAdded = InsuranceSettingMgr::Singleton().InsuranceToJinShanBi(nTotalInsuranceAdd);
		nPrevieosAdd           = nTotalMoneyAdded;
	}//endif

	pCalc->m_nPreviesReward = nPrevieosAdd;
	CalcFutherReward(pCalc,m_nCurrentInsuranceValue + nDeltaValue);
}

void InsuranceMgr::CalcFutherReward(UIInsuranceCalc * pCalc,int nNewInsuracneValue /*= -1*/)
{
	if (pCalc == NULL)
		return ;

	ZeroMemory(pCalc->m_nLevelRewards,sizeof(pCalc->m_nLevelRewards));

	if (nNewInsuracneValue == -1)
	{
		nNewInsuracneValue = m_nCurrentInsuranceValue;
	}//endif

	if (nNewInsuracneValue <= 0)
		return ;

	int nUsedIndex     = 0;
	int nPlayerLevel   = GetClientPlayer().GetLevel();

	for (int  nLevel = nPlayerLevel + 1 ; nLevel < MAX_UI_INSRUANCE_LEVEL_REC_NUM && nUsedIndex < MAX_UI_INSRUANCE_LEVEL_REC_NUM ; nLevel ++)
	{
		int  nRate       = InsuranceSettingMgr::Singleton().GetInsuranceRateByLevel(nLevel);
		
		if ( nRate > 0 )
		{
			int nRewardInsurance = (nNewInsuracneValue * nRate / 100);
			pCalc->m_nLevelRewards[nUsedIndex].m_nLevel    = nLevel;
			pCalc->m_nLevelRewards[nUsedIndex].m_nMoneyGet = InsuranceSettingMgr::Singleton().InsuranceToJinShanBi(nRewardInsurance);
			nUsedIndex ++ ;
		}//endif
		
	}//end for nLevel

}

void InsuranceMgr::GetRemainRewardReq()
{
	int nMoneyReq = m_nMoneyLeftToGet;

	if (nMoneyReq <= 0)
	{
		ShowErorrMsg(insurance_error_fetch_failed);
		return;
	}//endif

	if (nMoneyReq > m_nMoneyLeftToGet)
	{
		ShowErorrMsg(insurance_error_fetch_not_enough);
		return;
	}//endif

	if (m_nTotalMoneyGot + nMoneyReq < m_nTotalMoneyGot) // int overflow
	{
		ShowErorrMsg(insurance_error_fetch_too_much);
		return;
	}//en dif

	C2S_INSURANCE_PROTOCOL operation;
	operation.nProtocol    = c2s_insurance;
	operation.nSubProtocol = c2s_get_insurance_money;
	operation.nParam       = nMoneyReq;
	operation.nLen         = sizeof(operation) - PROTOCOL_SIZE;
	
	if (g_pClient)
		g_pClient->SendPackToServer(g_ConnectID,&operation,sizeof(operation));
}

#endif