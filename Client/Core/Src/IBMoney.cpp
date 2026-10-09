//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2008
//
//      Created_datetime : 03/18/2008 17:53
//      File_base        : IBMoney
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "IBMoney.h"
#include "KPlayer.h"
#include "IBLog.h"
#include "IBCenter_S.h"
#include "ILogSystem.h"

/************************************************************************/
/*							JinshanBi implement                         */
/************************************************************************/

JinshanBi::JinshanBi()
{
	SetMin(DWORD_MIN_LIMIT);
	SetMax(DWORD_MAX_LIMIT);
}

JinshanBi::~JinshanBi()
{

}

DWORD JinshanBi::AddReq( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return error_param;
	}

	return not_allow_oper;
}

DWORD JinshanBi::DecReq( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return error_param;
	}
#ifdef _SERVER

	if (g_pController && !g_pController->PaysysIsValid())
	{
		return not_allow_oper;
	}//endif

	KAccountBuyIBItem* tongbaoParam = (KAccountBuyIBItem*)money;

	ConfigManager& cm = ConfigManager::Singleton();
	int varMax = cm.GetIBGlobalVariable( ib_global_var_jinshanbi_exchange_limit );
	if ( tongbaoParam->nPrice > varMax || 
		tongbaoParam->nPrice <= 0 )
	{
		return error_param;
	}

	if ( !IsEnough( tongbaoParam->nPrice ) )
	{
		return not_enough_money;
	}

//	Dec( tongbaoParam->nPrice );

	KIBLog::getSingleton().AddIBMoney( jinshanbi_pay, d_player->GetPlayerIndex(), tongbaoParam->nPrice );
	
	const size_t uBufferSize = sizeof(KAccountBuyIBItem) + 1;
	
	BYTE Buffer[sizeof(KAccountBuyIBItem) + 1];
	Buffer[0] = l2p_ib_buy_item;
	
	KAccountBuyIBItem *pBuyIB = (KAccountBuyIBItem *)(Buffer + 1);
	
	memcpy( pBuyIB, tongbaoParam, sizeof(KAccountBuyIBItem));			
	g_pController->PushData( protocol_type_paysys, INVALID_VALUE, Buffer, uBufferSize );

	FSGUID fsGuid;
	if ( g_pController )
	{
		g_pController->GenGUID(fsGuid.data, g_GuidPadding);
	}//endif
	
	if (d_player)
	{
		LogEventParam buyEvent;
		buyEvent.event  = log_event_jinshanbi_buy_wait;
		buyEvent.param1 = d_player->GetGUID();
		snprintf(buyEvent.param3.data,sizeof(buyEvent.param3.data),"%d,%d,0,0",tongbaoParam->nItemTypeID,tongbaoParam->nItemLevel);
		buyEvent.param4 = tongbaoParam->nPrice;
		
		if (g_pLogSystem)
			g_pLogSystem->Log(buyEvent);
		
	}//endif

#endif
	return wait_dec_ret;
}

void JinshanBi::AddRet( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return;
	}

	return;
}

void JinshanBi::DecRet( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return;
	}//endif

	KAccountBuyIBItemRet* pBuyRet = (KAccountBuyIBItemRet*) money;	

	if ( !IsEnough( pBuyRet->nPrice ) )
	{
		return;
	}//endif

	Dec( pBuyRet->nPrice );
}

DWORD JinshanBi::SetMoney( DWORD money )
{
	IMoney<DWORD>::SetMoney( money );
#ifdef _SERVER
	if ( d_player == NULL )
	{
		return error_param;
	}
	d_player->SyncAttribute( attr_jinshanbi );
#endif
	return money_ok;
}

void JinshanBi::SetMax( DWORD money )
{
	if ( money <= DWORD_MAX_LIMIT )
	{
		d_max = money;
	}
	else
	{
		d_max = DWORD_MAX_LIMIT;
	}
}

void JinshanBi::SetMin( DWORD money )
{
	if ( money >= DWORD_MIN_LIMIT )
	{
		d_min = money;
	}
	else
	{
		d_min = DWORD_MIN_LIMIT;
	}
}

/************************************************************************/
/*							Credit implement                            */
/************************************************************************/

CreditPoint::CreditPoint()
{
	SetMin(LONG_MIN_LIMIT);
	SetMax(LONG_MAX_LIMIT);
}

CreditPoint::~CreditPoint()
{

}

int CreditPoint::AddReq( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return error_param;
	}


#ifdef _SERVER
	if ( d_player->GetCreditState() == disable )
	{
		return not_allow_oper;
	}

	int* add = (int*)money;

	ConfigManager& cm = ConfigManager::Singleton();
	int varMax = cm.GetIBGlobalVariable( ib_global_var_creditpoint_exchange_limit );
	if ( *add > varMax || 
		*add <= 0 )
	{
		return error_param;
	}

	if ( OverMin(GetMoney() - (*add)) )
	{
		return below_min;
	}

	Dec( *add );

	_IBDSHOPBHeader DBHeader;
	memset(&DBHeader, 0, sizeof(_IBDSHOPBHeader) );
	DBHeader.ulNetID = d_player->GetNetConnectIdx();
	DBHeader.ProcType = Proc_IB_AddCreditPoint;

	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_ROLEDATA );
	pParam->Push( d_player->m_PlayerName );
	pParam->Push( GetMoney() );
	pParam->Push( GetMax() );
	pParam->Push( d_player->GetCreditState() );
	pParam->Push( d_player->GetCreditReturnTime() );
	pParam->Push( d_player->GetIBMoney(point) );
	pParam->Push( d_player->GetIBPlus(point) );
	pParam->Push( d_player->GetTotolJinshanbi() );
	pParam->Push( d_player->GetRecentJinshanbi());
	pParam->Push( d_player->GetRecentTime() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	g_pController->CallProc( cfs_db_cnn_role, pParam );

	KIBLog::getSingleton().AddIBMoney( creditpoint_add, d_player->GetPlayerIndex(), *add );

	return wait_add_ret;

#endif
	return not_allow_oper;
}

int CreditPoint::DecReq( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return error_param;
	}
#ifdef _SERVER

	d_player->CheckCreditState();
	
	if ( d_player->GetCreditState() != good )
	{
		return not_allow_oper;
	}
	
	KAccountBuyIBItem* dec = (KAccountBuyIBItem*)money;
	
	ConfigManager& cm = ConfigManager::Singleton();
	int varMax = cm.GetIBGlobalVariable( ib_global_var_creditpoint_exchange_limit );
	if ( dec->nPrice > varMax || 
		dec->nPrice <= 0 )
	{
		return error_param;
	}
	
	if ( OverMax( (GetMoney() + dec->nPrice) ) )
	{
		return not_enough_money;
	}
	
	Add( dec->nPrice );

	KIBLog::getSingleton().AddIBMoney( creditpoint_pay, d_player->GetPlayerIndex(), dec->nPrice );
	
	_IBDSHOPBHeader DBHeader;
	memset(&DBHeader, 0, sizeof(_IBDSHOPBHeader) );
	DBHeader.ulNetID = d_player->GetNetConnectIdx();
	DBHeader.ProcType = Proc_IB_DecCreditPoint;
	DBHeader.itemHashID	= dec->nItemTypeID;
	DBHeader.itemLevel	= dec->nItemLevel;
	DBHeader.nPrice		= dec->nPrice;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_ROLEDATA );
	pParam->Push( d_player->m_PlayerName );
	pParam->Push( GetMoney() );
	pParam->Push( GetMax() );
	pParam->Push( d_player->GetCreditState() );
	pParam->Push( d_player->GetCreditReturnTime() );
	pParam->Push( d_player->GetIBMoney(point) );
	pParam->Push( d_player->GetIBPlus(point)  );
	pParam->Push( d_player->GetTotolJinshanbi() );
	pParam->Push( d_player->GetRecentJinshanbi());
	pParam->Push( d_player->GetRecentTime() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_role, pParam );//*/
	
#endif
	return wait_dec_ret;
}

void CreditPoint::AddRet( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return;
	}

	d_player->CheckCreditState();
}

void CreditPoint::DecRet( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return;
	}

#ifdef _SERVER
	IProcRet*	pRet	= (IProcRet*)money;
	int			nSize	= 0;
	char*		pPassBy = pRet->GetPassBy( nSize );
	
	if( pPassBy == NULL || 
		nSize == 0 || 
		nSize != sizeof(_IBDSHOPBHeader) )
	{
		return;
	}
	
	_IBDSHOPBHeader* itemData = (_IBDSHOPBHeader*) pPassBy;

	for ( int nIdx = 0; nIdx < itemData->itemLevel; ++nIdx )
	{
		int nItemIdx = ItemSet.Add( itemData->itemHashID, 0, 1 );
		if ( nItemIdx > 0 && nItemIdx < MAX_ITEM )
		{
			Item[nItemIdx].SetIBBuyDate(UNIX_TMIE_STAMP);
			if ( d_player->GetItemList().Add( nItemIdx ) == 0 )
			{
				d_player->GetItemList().Remove( nItemIdx );
				KIBLog::getSingleton().DelIBItem( buy_ok_add_no_delete, Item[nItemIdx].GetGUID(), d_player->GetPlayerIndex() );
				ItemSet.Remove( nItemIdx );				
			}				
			else
			{
				if ( !Item[nItemIdx].IsBind() )
				{
					Item[nItemIdx].SetBind(true);
					Item[nItemIdx].SyncAttribute(item_attr_isbind, d_player->GetNetConnectIdx());
				}
				Item[nItemIdx].SetCreditFlag( creditpoint );
				Item[nItemIdx].SetIBGuid( 0 );
				Item[nItemIdx].SyncAttribute( item_attr_buytime, d_player->GetNetConnectIdx() );
				Item[nItemIdx].SyncAttribute( item_attr_credit_flag, d_player->GetNetConnectIdx() );

				int nHashId = GenerateItemHashId(Item[nItemIdx].GetGenre(), Item[nItemIdx].GetDetailType(), Item[nItemIdx].GetParticular());
				IBCenter_S& ib_s =  IBCenter_S::Singleton();
				ib_s.ErrCodeToClient(d_player->GetPlayerIndex(), enIBShopErr_OnceItemUseOk, nHashId );

				KIBLog::getSingleton().AddIBItem( 
										creditpoint_buy, 
										Item[nItemIdx].GetGUID(),
										d_player->GetPlayerIndex(), 
										itemData->itemHashID,
										itemData->itemLevel, 
										itemData->nPrice / itemData->itemLevel );			
			}
		}
	}

#endif
}

int CreditPoint::SetMoney( int money )
{
	int err = IMoney<int>::SetMoney( money );
#ifdef _SERVER
	if ( d_player == NULL )
	{
		return error_param;
	}
	d_player->SyncAttribute( attr_creditpoint );
#endif
	return err;
}

void CreditPoint::SetMax( int money )
{
	if ( !IsGoodParam( &money ) )
	{
		return;
	}
	if ( money <= LONG_MAX_LIMIT )
	{
		d_max = money;
	}
	else
	{
		d_max = LONG_MAX_LIMIT;
	}
#ifdef _SERVER
	d_player->SyncAttribute( attr_maxcreditpoint );
#endif
}

void CreditPoint::SetMin( int money )
{
	if ( money >= LONG_MIN_LIMIT )
	{
		d_min = money;
	}
	else
	{
		d_min = LONG_MIN_LIMIT;
	}
}

/************************************************************************/
/*							Point implement                             */
/************************************************************************/
//Notice Point 不可以有 负值

Point::Point()
{
	SetMin( 0 );
	SetMax(LONG_MAX_LIMIT);
	d_MoneyPlus = 0; 
}

Point::~Point()
{
	d_MoneyPlus = 0; 	
}

int Point::AddReq( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return error_param;
	}
	int* add = (int*)money;
	int  err = not_allow_oper;

#ifdef _SERVER

	//Check.............................................................................

	int          * ToTalMoney     = (int *) money; //JinShanbi

	if ( *ToTalMoney <= 0)
		return  error_param;

	int            nInteger       = 0;
	int            nPlus          = 0;
	
	JinShanbiAddedToPoint(*ToTalMoney,nInteger,nPlus);

	ConfigManager& cm = ConfigManager::Singleton();
	int varMax = cm.GetIBGlobalVariable( ib_global_var_point_exchange_limit );
	if ( nInteger - d_money > varMax || 
		 nInteger - d_money < 0 || 
		 nPlus < 0 )
	{
		return error_param;
	}

	if ( OverMax( nInteger ) )
	{
		return over_max;
	}

	SetMoney(nInteger);
	SetMoneyPlus(nPlus);
	
	int min = 0;
	int max = 0;
	d_player->GetIBMoneySize(creditpoint, min, max);
	
	_IBDSHOPBHeader DBHeader;
	memset(&DBHeader, 0, sizeof(_IBDSHOPBHeader) );
	DBHeader.ulNetID = d_player->GetNetConnectIdx();
	DBHeader.ProcType = Proc_IB_AddPoint;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_IB_ROLEDATA );
	pParam->Push( d_player->m_PlayerName );
	pParam->Push( d_player->GetIBMoney(creditpoint) );
	pParam->Push( max );
	pParam->Push( d_player->GetCreditState() );
	pParam->Push( d_player->GetCreditReturnTime() );
	pParam->Push( GetMoney() );
	pParam->Push( GetMoneyPlus() );
	pParam->Push( d_player->GetTotolJinshanbi() );
	pParam->Push( d_player->GetRecentJinshanbi());
	pParam->Push( d_player->GetRecentTime() );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_role, pParam );

	KIBLog::getSingleton().AddIBMoney( point_add, d_player->GetPlayerIndex(), nInteger );
	
	return wait_add_ret;

#endif

	return err;
}

int Point::SetMoneyPlus(int moneyPlus)
{
	d_MoneyPlus = moneyPlus;
	return money_ok;
}

int Point::GetMoneyPlus()
{
	return d_MoneyPlus;
}

void Point::JinShanbiAddedToPoint(const int iJinShanBi,int & nInterger,int & nPlus)
{
	nInterger = 0;
	nPlus     = 0;

	ConfigManager & mgr   = ConfigManager::Singleton();
	unsigned int   nRate = mgr.GetIBGlobalVariable(ib_global_var_point_rate);
	if ( nRate !=0 )
	{
		nInterger = (d_MoneyPlus + iJinShanBi) / nRate + d_money;
		nPlus     = (d_MoneyPlus + iJinShanBi) % nRate;
	}//endif
	else
	{
		nInterger = d_money;
		nPlus     = d_MoneyPlus;
	}//end else

}

int Point::DecReq( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return error_param;
	}

	KAccountBuyIBItem* dec = (KAccountBuyIBItem*)money;
	if ( dec == NULL  || d_player == NULL  )
	{
		return error_param;
	}
	int err = not_allow_oper;

#ifdef _SERVER

	ConfigManager& cm = ConfigManager::Singleton();
	int varMax = cm.GetIBGlobalVariable( ib_global_var_point_exchange_limit );
	if ( dec->nPrice > varMax || 
		dec->nPrice <= 0 )
	{
		return error_param;
	}

	if ( !IsEnough( dec->nPrice ) )
	{
		return not_enough_money;
	}
	
	err = Dec( dec->nPrice );

	KIBLog::getSingleton().AddIBMoney( point_pay, d_player->GetPlayerIndex(), dec->nPrice );

	if ( err == money_ok )
	{
		int min = 0;
		int max = 0;
		d_player->GetIBMoneySize(creditpoint, min, max);
		
		_IBDSHOPBHeader DBHeader;
		memset(&DBHeader, 0, sizeof(_IBDSHOPBHeader) );
		DBHeader.ulNetID	= d_player->GetNetConnectIdx();
		DBHeader.ProcType	= Proc_IB_DecPoint;
		DBHeader.itemHashID	= dec->nItemTypeID;
		DBHeader.itemLevel	= dec->nItemLevel;
		DBHeader.nPrice		= dec->nPrice;
		
		IProcParam* pParam = g_pController->GetProcParam( );
		
		pParam->BeginPush( PN_IB_ROLEDATA );
		pParam->Push( d_player->m_PlayerName );
		pParam->Push( d_player->GetIBMoney(creditpoint) );
		pParam->Push( max );
		pParam->Push( d_player->GetCreditState() );
		pParam->Push( d_player->GetCreditReturnTime() );
		pParam->Push( GetMoney()  );
		pParam->Push( GetMoneyPlus() );
		pParam->Push( d_player->GetTotolJinshanbi() );
		pParam->Push( d_player->GetRecentJinshanbi());
		pParam->Push( d_player->GetRecentTime() );
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_role, pParam );
		
		return wait_dec_ret;
	}
#endif
	return err;
}

void Point::AddRet( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return;
	}
	IProcRet* pRet = (IProcRet*)money;
	if ( pRet && d_player )
	{
	}
}

void Point::DecRet( void* money )
{
	if ( !IsGoodParam( money ) )
	{
		return;
	}
#ifdef _SERVER
	IProcRet* pRet = (IProcRet*)money;
	if ( pRet && d_player )
	{
		int nSize = 0;
		char* pPassBy = pRet->GetPassBy( nSize );
		
		if( pPassBy == NULL || 
			nSize == 0 || 
			nSize != sizeof(_IBDSHOPBHeader) )
			return;
		
		_IBDSHOPBHeader* itemData = (_IBDSHOPBHeader*) pPassBy;

		for ( int nIdx = 0; nIdx < itemData->itemLevel; ++nIdx )
		{
			int nItemIdx = ItemSet.Add(  itemData->itemHashID, 0, 1 );
			if ( nItemIdx > 0 && nItemIdx < MAX_ITEM )
			{
				Item[nItemIdx].SetIBBuyDate(UNIX_TMIE_STAMP);
				if ( d_player->GetItemList().Add( nItemIdx ) == 0 )
				{
					d_player->GetItemList().Remove( nItemIdx );
					KIBLog::getSingleton().DelIBItem( buy_ok_add_no_delete, Item[nItemIdx].GetGUID(), d_player->GetPlayerIndex() );
					ItemSet.Remove( nItemIdx );					
				}				
				else
				{

					if ( !Item[nItemIdx].IsBind() )
					{
						Item[nItemIdx].SetBind(true);
						Item[nItemIdx].SyncAttribute(item_attr_isbind, d_player->GetNetConnectIdx());
					}
					Item[nItemIdx].SetCreditFlag( point );
					Item[nItemIdx].SetIBGuid( 0 );
					Item[nItemIdx].SyncAttribute( item_attr_buytime, d_player->GetNetConnectIdx() );
					Item[nItemIdx].SyncAttribute( item_attr_credit_flag, d_player->GetNetConnectIdx() );
					Item[nItemIdx].SetIBGuid( 0 );

					int nHashId = GenerateItemHashId(Item[nItemIdx].GetGenre(), Item[nItemIdx].GetDetailType(), Item[nItemIdx].GetParticular());
					IBCenter_S& ib_s =  IBCenter_S::Singleton();
					ib_s.ErrCodeToClient(d_player->GetPlayerIndex(), enIBShopErr_OnceItemUseOk, nHashId );

					KIBLog::getSingleton().AddIBItem( 
											point_buy, 
											Item[nItemIdx].GetGUID(),
											d_player->GetPlayerIndex(), 
											itemData->itemHashID,
											itemData->itemLevel, 
											itemData->nPrice / itemData->itemLevel );	
				}
			}
		}

	}
#endif
}

int Point::SetMoney( int money )
{
	if ( d_player == NULL )
	{
		error_param;
	}
	int err = IMoney<int>::SetMoney( money );
#ifdef _SERVER
	if ( d_player && err == money_ok )
	{
		d_player->SyncAttribute( attr_point );
	}	
#endif
	return err;
}

void Point::SetMax( int money )
{
	if ( money <= LONG_MAX_LIMIT )
	{
		d_max = money;
	}//endif
	else
	{
		d_max = LONG_MAX_LIMIT;
	}//end else

	if (d_max < 0 )
		d_max = 0;
}

void Point::SetMin( int money )
{
	if ( money >= 0 )
	{
		d_min = money;
	}
	else
	{
		d_min   = 0;
	}
}

/************************************************************************/
/*							MoneyMgr implement                          */
/************************************************************************/
MoneyMgr::MoneyMgr( void )
{
	d_money[jinshanbi]		= (IMoney<int>*)&d_jinshabi;
	d_money[creditpoint]	= &d_creditPoint;
	d_money[point]			= &d_point;
	
}

MoneyMgr::~MoneyMgr( void )
{
	
}

void MoneyMgr::SetBelongPlayer( KPlayer* player )
{
	d_jinshabi.SetBelongPlayer( player );
	d_money[creditpoint]->SetBelongPlayer( player );
	d_money[point]->SetBelongPlayer( player );
}

void MoneyMgr::SetMoneySize( MoneyType type,  int min, int max )
{	
	if ( !IsOkMoneyType( type ) )
	{
		return;
	}
	if ( type == jinshanbi )
	{
		return;
	}
	d_money[type]->SetMin( min );
	d_money[type]->SetMax( max );
}

void MoneyMgr::GetMoneySize( MoneyType type,  int& min, int& max )
{
	if ( !IsOkMoneyType( type ) )
	{
		min = 0;
		max = 0xffffffff;
		return;
	}
	if ( type == jinshanbi )
	{
		return;
	}
	min = d_money[type]->GetMin();
	max = d_money[type]->GetMax();
}

int MoneyMgr::SetMoney( MoneyType type, int money )
{
	if ( !IsOkMoneyType( type ) )
	{
		return not_allow_money_type;
	}
	if ( type == jinshanbi )
	{
		return not_allow_money_type;
	}
	return d_money[type]->SetMoney( money );
}

int MoneyMgr::SetMoneyPlus(MoneyType type, int money )
{
	if ( !IsOkMoneyType( type ) )
	{
		return not_allow_money_type;
	}

	if ( type == jinshanbi )
	{
		return not_allow_money_type;
	}
	return d_money[type]->SetMoneyPlus( money );
}

int MoneyMgr::GetMoneyPlus(MoneyType type)
{
	return d_money[type]->GetMoneyPlus();
}

int MoneyMgr::GetMoney( MoneyType type )
{
	if ( !IsOkMoneyType( type ) )
	{
		return not_allow_money_type;
	}
	if ( type == jinshanbi )
	{
		return 0;
	}
	else
	{
		return d_money[type]->GetMoney( );
	}
	
}

DWORD MoneyMgr::SetJinshanbi( DWORD money )
{
	return d_jinshabi.SetMoney( money );
}

DWORD MoneyMgr::GetJinshanbi(  )
{
	return d_jinshabi.GetMoney( );
}

int MoneyMgr::AddReq( MoneyType type, void* money )
{
	if ( !IsOkMoneyType( type ) )
	{
		return not_allow_money_type;
	}
	if ( type == jinshanbi )
	{
		return d_jinshabi.AddReq( money );
	}
	else
	{
		return d_money[type]->AddReq( money );
	}
}

int MoneyMgr::DecReq(  MoneyType type, void* money )
{
	if ( !IsOkMoneyType( type ) )
	{
		return not_allow_money_type;
	}
	if ( type == jinshanbi )
	{
		return d_jinshabi.DecReq( money );
	}
	else
	{
		return d_money[type]->DecReq( money );
	}
}

void MoneyMgr::AddRet( MoneyType type, void* money )
{
	if ( !IsOkMoneyType( type ) )
	{
		return;
	}
	if ( type == jinshanbi )
	{
		d_jinshabi.AddRet( money );
	}
	else
	{
		d_money[type]->AddRet( money );
	}
}

void MoneyMgr::DecRet(  MoneyType type, void* money )
{
	if ( !IsOkMoneyType( type ) )
	{
		return;
	}
	if ( type == jinshanbi )
	{
		d_jinshabi.DecRet( money );
	}
	else
	{
		d_money[type]->DecRet( money );
	}	
}

bool MoneyMgr::IsOkMoneyType( MoneyType type )
{
	if ( type >= jinshanbi &&
		type < money_type_count &&
		d_money[type] )
	{
		return true;
	}
	else
	{
		return false;
	}
}


