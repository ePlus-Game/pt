//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006:12:20   18:55
//      File_base        : DBAucDataCenter
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
#include "DBAucDataCenter.h"
#include <stdio.h>
#include <time.h>

#define		DEF_CLEARGOODS_TIMEINTERVAL		300		// seconds

DBAucDataCenter& DBAucDataCenter::Singleton()
{
	static DBAucDataCenter center;
	
	return center;
}

void DBAucDataCenter::Active()
{
	DWORD	dwCurTime = UNIX_TMIE_STAMP;

	if(dwCurTime - m_preClearGoodsTime >= DEF_CLEARGOODS_TIMEINTERVAL)
	{
		m_preClearGoodsTime = dwCurTime;
		ClearOverdueGoodsReq();
	}
}

void DBAucDataCenter::SearchReq(int nPlayerIdx, const S2DB_SEARCHGOODS_COND &searchCond)
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Auction;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_AUC_SEARCH );
	
	//goods name
	if( searchCond.filterCond.goodsName[0] == 0 )
		pParam->Push( NullPair( ) );
	else
		pParam->Push( searchCond.filterCond.goodsName );

	//qualitylabel
	if( 0xff == searchCond.filterCond.qualityLabel )
		pParam->Push( NullPair( ) );
	else
		pParam->Push( searchCond.filterCond.qualityLabel );

	//low level
	if( 0xffff == searchCond.filterCond.levelReqLow )
		pParam->Push( NullPair( ) );
	else
		pParam->Push( searchCond.filterCond.levelReqLow );

	//faction
	int nFactionReq = searchCond.filterCond.factionReq;
	if(0xff == searchCond.filterCond.factionReq)
		nFactionReq = 0x3f;		// 0011 1111
	pParam->Push( nFactionReq );

	//seller name
	if( searchCond.filterCond.sellerName[0] == 0 )
		pParam->Push( NullPair( ) );
	else
		pParam->Push( searchCond.filterCond.sellerName );

	//buyer name
	if( searchCond.filterCond.buyerName[0] == 0 )
		pParam->Push( NullPair( ) );
	else
		pParam->Push( searchCond.filterCond.buyerName );
	
	//level hight
	if( 0xffff == searchCond.filterCond.levelReqHight )
		pParam->Push( NullPair( ) );
	else
		pParam->Push( searchCond.filterCond.levelReqHight );
	
	//itemType
	if( searchCond.filterCond.itemType[0] == 0 )
		pParam->Push( NullPair( ) );
	else
		pParam->Push( searchCond.filterCond.itemType );

	pParam->Push( MAXRECORDS_PER_PAGE );
	pParam->Push( searchCond.startRecordOffset );

	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
}

void DBAucDataCenter::SellReq(int nPlayerIdx, const S2DB_SELLGOODS_REQ &sellReq)
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Auction;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_AUC_SELLGOODS );
	
	//seller name
	if( sellReq.sellerName[0] == 0 )
		pParam->Push( NullPair( ) );
	else
		pParam->Push( sellReq.sellerName );
	
	//guid
	pParam->Push( sellReq.playerGuid.data );


	//item name
	if( sellReq.itemName[0] == 0 )
		pParam->Push( NullPair( ) );
	else
		pParam->Push( sellReq.itemName );
	
	//item guid
	pParam->Push( sellReq.itemGuid.data );


	//totalValidTime
	pParam->Push( sellReq.recordComData.totalValidTime );

	//cur price
	pParam->Push( sellReq.recordComData.currentPrice );

	//one price
	pParam->Push( sellReq.recordComData.onePrice );


	//qualityLabel
	pParam->Push( sellReq.qualityLabel );

	//playerLvlReq
	pParam->Push( sellReq.playerLvlReq );

	//itemGenre
	pParam->Push( sellReq.itemGenre );

	//itemDetail
	pParam->Push( sellReq.itemDetail );

	//itemParticular
	pParam->Push( sellReq.itemParticular );

	//itemLevel
	pParam->Push( sellReq.itemLevel );

	//factionReq
	pParam->Push( sellReq.factionReq );
	
	//item data
	pParam->Push( BinPair( (void*)&sellReq.itemData, sizeof(sellReq.itemData) ) );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
}

void DBAucDataCenter::LockGoodsReq(int nPlayerIdx, const S2DB_LOCKGOODS_REQ &lockReq)
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Auction;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_AUC_LOCKGOODS );
	
	//id
	pParam->Push( lockReq.buyReqData.recordId );

	//price
	pParam->Push( lockReq.buyReqData.price );

	//guid
	pParam->Push( lockReq.playerGuid.data );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
}

void DBAucDataCenter::UnLockGoodsReq(int nPlayerIdx, const S2DB_UNLOCKGOODS_REQ &unlockReq)
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Auction;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_AUC_UNLOCKGOODS );
	
	//id
	pParam->Push( unlockReq.auctionId );
	
	//guid
	pParam->Push( unlockReq.playerGuid.data );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
}

void DBAucDataCenter::CancelReq(int nPlayerIdx, const S2DB_CANCELAUCTION_REQ &s2dbReq)
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Auction;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_AUC_CANCELAUCTION );
	
	//id
	pParam->Push( s2dbReq.auctionId );
	
	//guid
	pParam->Push( s2dbReq.playerGuid.data );
	
	pParam->Push( MSG_AUCTION_MAILSENDER );
	pParam->Push( MSG_AUCTION_CANCELMAILTITLE );
	pParam->Push( MSG_AUCTION_CANCELMAILBODY );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
}

void DBAucDataCenter::UpdatePrice(int nPlayerIdx, const S2DB_UPDATEPRICE_REQ &buyReq)
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Auction;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_AUC_UPDATEPRICE );
	
	//id
	pParam->Push( buyReq.buyGoodsData.recordId );

	//name
	pParam->Push( buyReq.buyerName );

	//guid
	pParam->Push( buyReq.playerGuid.data );

	//price
	pParam->Push( buyReq.buyGoodsData.price );
	
	pParam->Push( MSG_AUCTION_MAILSENDER );
	pParam->Push( MSG_AUCTION_RETMONEYMAILTITLE );
	pParam->Push( MSG_AUCTION_RETMONEYMAILBODY );
	pParam->Push( MSG_AUCTION_MAILTITLE );
	pParam->Push( MSG_AUCTION_BUYERMAILBODY );
	pParam->Push( MSG_AUCTION_SELLERMAILBODY );

	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
}

void DBAucDataCenter::ClearOverdueGoodsReq()
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_Auction;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	//begin
	pParam->BeginPush( PN_AUC_CHECKOVERDUE );

	pParam->Push( MSG_AUCTION_MAILSENDER );
	pParam->Push( MSG_AUCTION_MAILTITLE );
	pParam->Push( MSG_AUCTION_BUYERMAILBODY );
	pParam->Push( MSG_AUCTION_SELLERMAILBODY );
	pParam->Push( MSG_AUCTION_RETURNMAILTITLE );
	pParam->Push( MSG_AUCTION_RETURNMAILBODY );
	
	//end push
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
}