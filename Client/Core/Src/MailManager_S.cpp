//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 14:29
//      File_base        : MailManager_S
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
#include "MailManager_S.h"
#include "ChatDataDef.h"
#include "CoreUtil.h"
#include "IBLog.h"

using namespace CHAT;

#ifdef WIN32
#define		snprintf	_snprintf
#endif

MailManager_S::PDBRETPROC		MailManager_S::m_DBRetPRoc[enMailDBOpe_Num];
MailManager_S::PCLIENTREQPROC	MailManager_S::m_ClientReqProc[enMailDBOpe_Num];

MailManager_S::MailManager_S()
{
	m_CurDBOpe = enMailDBOpe_None;

	memset(&m_DBRetPRoc, 0, sizeof(m_DBRetPRoc));
	m_DBRetPRoc[enMailDBOpe_LoadMailList] = &MailManager_S::LoadMailListRet;
	m_DBRetPRoc[enMailDBOpe_LoadMail] = &MailManager_S::LoadMailRet;
	m_DBRetPRoc[enMailDBOpe_SendMail] = &MailManager_S::SendMailRet;
	m_DBRetPRoc[enMailDBOpe_GetOutPlus] = &MailManager_S::GetOutPlusRet;
	m_DBRetPRoc[enMailDBOpe_GetOutMoney] = &MailManager_S::GetOutMoneyRet;
	m_DBRetPRoc[enMailDBOpe_DelMail] = &MailManager_S::DelMailRet;
	m_DBRetPRoc[enMailDBOpe_CloseMail] = &MailManager_S::CloseMailRet;
	m_DBRetPRoc[enMailDBOpe_QueryNewMail] = &MailManager_S::QueryNewMailRet;
	m_DBRetPRoc[enMailDBOpe_ReturnMail] = &MailManager_S::ReturnMailRet;

	memset(&m_ClientReqProc, 0, sizeof(m_ClientReqProc));
	m_ClientReqProc[enMailDBOpe_LoadMailList] = &MailManager_S::LoadMailListReq;
	m_ClientReqProc[enMailDBOpe_LoadMail] = &MailManager_S::LoadMailReq;
	m_ClientReqProc[enMailDBOpe_SendMail] = &MailManager_S::SendMailReq;
	m_ClientReqProc[enMailDBOpe_GetOutPlus] = &MailManager_S::GetOutPlusReq;
	m_ClientReqProc[enMailDBOpe_GetOutMoney] = &MailManager_S::GetOutMoneyReq;
	m_ClientReqProc[enMailDBOpe_DelMail] = &MailManager_S::DelMailReq;
	m_ClientReqProc[enMailDBOpe_CloseMail] = &MailManager_S::CloseMailReq;
	m_ClientReqProc[enMailDBOpe_ReturnMail] = &MailManager_S::ReturnMailReq;

}

int	MailManager_S::GetOutMoneyReq(int nPlayerIdx, BYTE *pData, int nDataSize)
{
	if(enMailDBOpe_None != m_CurDBOpe)
		return char_err_dbreqondoing;

	_ASSERT(-1 != m_MailInfo.mailIdx);

	if(-1 == m_MailInfo.mailIdx)
		return chat_err_mailnotfound;

	int nMailMoneyRequire = (int)m_MailListInfo[m_MailInfo.mailIdx].mailCost;
	if (nMailMoneyRequire < 0)
		return char_err_moneynotenough;

	if( GetTotalMoney(nPlayerIdx) < nMailMoneyRequire )
		return char_err_moneynotenough;

	if( nMailMoneyRequire > 0)
	{
		if ( nMailMoneyRequire >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
		{
			LogEventParam logParam;
			logParam.event = log_event_get_out_mail_plus_money_pay_money;
			logParam.param1 = Player[nPlayerIdx].GetGUID();
			strncpy(logParam.param2.data, m_MailListInfo[m_MailInfo.mailIdx].senderName, sizeof(logParam.param2.data));
			logParam.param2.data[sizeof(logParam.param2.data) - 1] = 0;
			logParam.param4 = - nMailMoneyRequire;
			g_pLogSystem->Log(logParam);
		}

		DecMoney(nPlayerIdx, nMailMoneyRequire);
		m_MailListInfo[m_MailInfo.mailIdx].mailCost = 0;
	}//endif

	m_MailInfo.updateInfo.curGetOutPlusIdx = -1;
	m_MailInfo.updateInfo.newCostMoney = 0;
	m_MailInfo.updateInfo.newPostMoney = 0;

	return UpdateMailReq(nPlayerIdx, enMailDBOpe_GetOutMoney);
}

int	MailManager_S::GetOutPlusReq(int nPlayerIdx, BYTE *pData, int nDataSize)
{
	if(enMailDBOpe_None != m_CurDBOpe)
		return char_err_dbreqondoing;

	_ASSERT(-1 != m_MailInfo.mailIdx);

	if(-1 == m_MailInfo.mailIdx)
		return chat_err_mailnotfound;

	int nCheckPosX, nCheckPosY;
	if( !Player[nPlayerIdx].m_ItemList.CheckCanPlaceInEquipment(&nCheckPosX, &nCheckPosY) )
		return char_err_packagefull;

	if (nDataSize < sizeof(CHAT_GETOUT_ITEM))
		return chat_err_itemnotfound;

	PCHAT_GETOUT_ITEM	pc2sReq = (PCHAT_GETOUT_ITEM)pData;
	int					nIndex = pc2sReq->index;

	if(nIndex < 0 || nIndex >= m_MailInfo.mailPlusCount)
		return chat_err_itemnotfound;

	if( GetTotalMoney(nPlayerIdx) < (int) m_MailListInfo[m_MailInfo.mailIdx].mailCost )
		return char_err_moneynotenough;
	
	if( (int)m_MailListInfo[m_MailInfo.mailIdx].mailCost > 0)
	{
		if ( (int) m_MailListInfo[m_MailInfo.mailIdx].mailCost >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
		{
			LogEventParam logParam;
			logParam.event = log_event_get_out_mail_plus_item_pay_money;
			logParam.param1 = Player[nPlayerIdx].GetGUID();
			strncpy(logParam.param2.data, m_MailListInfo[m_MailInfo.mailIdx].senderName, sizeof(logParam.param2.data));
			logParam.param2.data[sizeof(logParam.param2.data) - 1] = 0;
			logParam.param4 = - (int)m_MailListInfo[m_MailInfo.mailIdx].mailCost;
			g_pLogSystem->Log(logParam);
		}

		DecMoney(nPlayerIdx, (int)m_MailListInfo[m_MailInfo.mailIdx].mailCost);
		m_MailListInfo[m_MailInfo.mailIdx].mailCost = 0;
	}

	if( !m_MailInfo.mailPlus[nIndex].bGetOut )
	{
		m_MailInfo.updateInfo.curGetOutPlusIdx = (char)nIndex;
		m_MailInfo.updateInfo.newCostMoney = 0;
		m_MailInfo.updateInfo.newPostMoney = m_MailListInfo[m_MailInfo.mailIdx].postMoney;
	
		return UpdateMailReq(nPlayerIdx, enMailDBOpe_GetOutPlus);
	}

	return chat_err_none;
}

int MailManager_S::LoadMailListReq(int nPlayerIdx, BYTE *pData, int nDataSize)
{
	int nRet = chat_err_none;

	if (nDataSize < sizeof (CHAT_LOADMAILLIST_REQ))
		return chat_err_none;

	if(enMailDBOpe_None != m_CurDBOpe)
	{
		nRet = char_err_dbreqondoing;
	}
	else
	{
		m_CurDBOpe = enMailDBOpe_LoadMailList;
		CHAT_LOADMAILLIST_REQ * pPack = (CHAT_LOADMAILLIST_REQ *)pData;
		
		_DBProcHeader DBHeader = {0};
		DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
		DBHeader.ProcType = Proc_Mail;
		
		IProcParam* pParam = g_pController->GetProcParam( );
		
		pParam->BeginPush( PN_GETMAILLIST );
		pParam->Push( GetPlayerName(nPlayerIdx) );
		pParam->Push( MAX_MAILCOUNT_PERPLAYER );
		pParam->Push( GetPlayerLevel(nPlayerIdx) );
		pParam->Push( pPack->nPage * MAX_MAILCOUNT_PERPLAYER);
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
	}
	
	return nRet;
}

int	MailManager_S::DelMailReq(int nPlayerIdx, BYTE *pData, int nDataSize)
{
	if(enMailDBOpe_None != m_CurDBOpe)
		return char_err_dbreqondoing;

	_ASSERT(-1 != m_MailInfo.mailIdx);

	if(-1 == m_MailInfo.mailIdx)
		return chat_err_mailnotfound;

//	if(m_MailListInfo[m_MailInfo.mailIdx].mailCost > 0 
//		&& enMailSenderType_Player == m_MailListInfo[m_MailInfo.mailIdx].senderType)
	if( (int)m_MailListInfo[m_MailInfo.mailIdx].mailCost > 0 ||
		(int)m_MailListInfo[m_MailInfo.mailIdx].postMoney > 0 ||
		(int)m_MailInfo.mailPlusCount > 0 )
		return chat_err_delmailfailed;
	

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Mail;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_DELETEMAIL );
	pParam->Push( m_MailListInfo[m_MailInfo.mailIdx].mailId );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );	

	m_CurDBOpe = enMailDBOpe_DelMail;

	return chat_err_none;
}

int	MailManager_S::LoadMailReq(int nPlayerIdx, BYTE *pData, int nDataSize)
{
	if(enMailDBOpe_None != m_CurDBOpe)
		return char_err_dbreqondoing;

	if (nDataSize < sizeof(CHAT_LOADMAIL_REQ))
		return chat_err_mailnotfound;

	PCHAT_LOADMAIL_REQ	pc2sReq = (PCHAT_LOADMAIL_REQ)pData;
	DWORD				dwMailId = pc2sReq->mailId;

	if( -1 == MailId2Idx(dwMailId) )
		return chat_err_mailnotfound;

	if(-1 != m_MailInfo.mailIdx)
	{
		if(m_MailListInfo[m_MailInfo.mailIdx].mailId == dwMailId)
			return chat_err_none;
		else
			CloseMailReq(nPlayerIdx, NULL, 0);
	}

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Mail;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_GETMAIL );
	pParam->Push( dwMailId );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );	

	m_CurDBOpe = enMailDBOpe_LoadMail;

	return chat_err_none;
}

int	MailManager_S::QueryNewMailReq(int nPlayerIdx, BYTE *pData, int nDataSize)
{
	if(enMailDBOpe_None != m_CurDBOpe)
		return char_err_dbreqondoing;

	m_CurDBOpe = enMailDBOpe_QueryNewMail;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Mail;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_COUNTNEWMAIL );
	pParam->Push( GetPlayerName(nPlayerIdx) );
	pParam->Push( GetPlayerLevel(nPlayerIdx) );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );

	return chat_err_none;
}

int	MailManager_S::SendMailReq(int nPlayerIdx, BYTE *pData, int nDataSize)
{
	if(enMailDBOpe_None != m_CurDBOpe)
		return	char_err_dbreqondoing;

	PCHAT_STRUCTURED_DATABLOCK	pc2sReq = (PCHAT_STRUCTURED_DATABLOCK)pData;
	PDBTASK_SENDMAILS_REQ		pMail   = (PDBTASK_SENDMAILS_REQ)pc2sReq->data;

	//Check the package size 
	if (nDataSize < sizeof(CHAT_STRUCTURED_DATABLOCK) - 1 + sizeof(DBTASK_SENDMAILS_REQ) -1 )
		return chat_err_sendmailfailed;

	if (nDataSize < sizeof(CHAT_STRUCTURED_DATABLOCK) - 1 + sizeof(DBTASK_SENDMAILS_REQ) -1 + pMail->nMailTextSize + pMail->nMailPlusSize)
		return chat_err_sendmailfailed;

	DWORD	*pItemIds = (DWORD*)(pMail->pMailData + pMail->nMailTextSize);
	int		nPlusCount = pMail->nMailPlusSize / sizeof(DWORD);

	pMail->tagMailInfo.szSenderName[sizeof(pMail->tagMailInfo.szSenderName) - 1] = '\0';
	pMail->tagMailInfo.szReceiverName[sizeof(pMail->tagMailInfo.szReceiverName) - 1] = '\0';
	pMail->tagMailInfo.szTile[sizeof(pMail->tagMailInfo.szTile) - 1] = '\0';

	if( 0 == strcmp(GetPlayerName(nPlayerIdx), pMail->tagMailInfo.szReceiverName) )
		return chat_err_sendmailfailed;

	int nTax = 0;
	if (nPlusCount == 0)
	{
		nTax = ConfigManager::Singleton().GetGlobalVariable(global_var_sendmail_text_tax);
		pMail->tagMailInfo.nMailCost = 0;
	}
	else
		nTax = ConfigManager::Singleton().GetGlobalVariable(global_var_sendmail_item_tax);

	int nPostMoney = (int)pMail->tagMailInfo.nPostMoney;
	int nCostMoney = (int)pMail->tagMailInfo.nMailCost;
	if (nPostMoney >= MAX_MAIL_MONEY || nPostMoney < 0 ||
		nCostMoney >= MAX_MAIL_MONEY || nCostMoney < 0)
		return char_err_moneynotenough;

	int nRet = CheckMail(nPlayerIdx,  
						 pMail->tagMailInfo.szTile, 
						 nPostMoney,
						 pMail->nMailTextSize, 
						 pMail->pMailData, 
						 nPlusCount, 
						 pItemIds,
						 nTax );

	if(chat_err_none == nRet)
	{
		BYTE buf[MAXSIZE_SEND_BUF];

		int decMoney  = nPostMoney + nTax;
		if (decMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
		{
			LogEventParam logParam;
			logParam.event = log_event_send_mail_pay_money;
			logParam.param1 = Player[nPlayerIdx].GetGUID();
			strncpy(logParam.param2.data, pMail->tagMailInfo.szReceiverName, sizeof(logParam.param2.data));
			logParam.param2.data[sizeof(logParam.param2.data) - 1] = 0;
			logParam.param4 = -decMoney;
			g_pLogSystem->Log(logParam);
		}

		DecMoney(nPlayerIdx, nPostMoney);
		DecMoney(nPlayerIdx, nTax, true);

		PCHAT_MAILPLUS_ITEM  pItemData = (PCHAT_MAILPLUS_ITEM)(buf);

		KPlayer& player = Player[nPlayerIdx];

		for(int i = 0; i < nPlusCount; ++i)
		{
			DWORD itemId = pItemIds[i];
			
			int itemIndex = player.GetItemList().SearchID(itemId);
			if (itemIndex > 0)
			{
				KItem& item = Item[itemIndex];
				
				//统计：邮件发送物品
				ItemTemplateId templateId;
				item.GetItemTemplateId(templateId);
				player.GetPlayerStatistic().AddItem(templateId, item.GetItemCount(), item_count_type_mail_out);
				
				//日志：邮件发送物品
				if (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
				{
					LogEventParam sendItemEvent;
					sendItemEvent.event = log_event_send_mail_item;
					sendItemEvent.param1 = player.GetGUID();
					sendItemEvent.param2 = item.GetGUID();
					char itemIdBuff[32] = { 0 };
					item.GetItemTemplateId(itemIdBuff, sizeof(itemIdBuff));
					itemIdBuff[sizeof(itemIdBuff) - 1] = 0;
					snprintf(sendItemEvent.param3.data, sizeof(sendItemEvent.param3.data), "%s %s", itemIdBuff, pMail->tagMailInfo.szReceiverName);
					sendItemEvent.param3.data[sizeof(sendItemEvent.param3.data) - 1] = 0;
					g_pLogSystem->Log(sendItemEvent);
				}

			}
			
			GetItemTransData(nPlayerIdx, pItemData->data, itemId);
			RemoveItem(nPlayerIdx, itemId);
			++pItemData;
		}

		int nMaxMailPerPlayer = ConfigManager::Singleton().GetGlobalVariable(global_var_max_mails_per_player);
		if  (nMaxMailPerPlayer == 0)
		{
			nMaxMailPerPlayer = MAIL_RECEIVERMAXMAIL;
		}//endif

		//-------------------------------------------------
		_DBProcHeader DBHeader = {0};
		DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
		DBHeader.ProcType = Proc_Mail;
		
		IProcParam* pParam = g_pController->GetProcParam( );
		
		pParam->BeginPush( PN_SENDMAIL );
		
		pParam->Push( GetPlayerName(nPlayerIdx) );
		pParam->Push( pMail->tagMailInfo.szReceiverName );
		pParam->Push( pMail->tagMailInfo.szTile );
		pParam->Push( 0 );
		pParam->Push( pMail->tagMailInfo.enSenderType );
		pParam->Push( pMail->tagMailInfo.enMailType );
		pParam->Push( BinPair( pMail->pMailData, pMail->nMailTextSize ) );
		pParam->Push( BinPair( buf, nPlusCount * sizeof(CHAT_MAILPLUS_ITEM) ) );
		pParam->Push( nPostMoney );
		pParam->Push( nCostMoney );
		pParam->Push( nCostMoney > 0 ? 1 : MAIL_LIVINGDATA );		//邮件最大生存周期
		pParam->Push( MAIL_SENDERMAXMAIL );		//最大发送邮件数
		pParam->Push( nMaxMailPerPlayer );	//最大接受邮件数
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );

		//---------------------------------------------------
		strncpy(m_SendMailInfo.receiverName, pMail->tagMailInfo.szReceiverName, 
					sizeof(m_SendMailInfo.receiverName));

		m_CurDBOpe = enMailDBOpe_SendMail;
	}

	return nRet;
}

int	MailManager_S::UpdateMailReq(int nPlayerIdx, enMailDBOpe enDBOpe)
{
	_ASSERT(-1 != m_MailInfo.mailIdx);

	if(-1 == m_MailInfo.mailIdx)
		return chat_err_mailnotfound;

	int							nSize = 0;
	char						buf[MAXSIZE_MAIL];
	PCHAT_MAILPLUS_ITEM			pPlus = (PCHAT_MAILPLUS_ITEM)buf;

	for(int i = 0; i < m_MailInfo.mailPlusCount; ++i)
	{
		if(i != m_MailInfo.updateInfo.curGetOutPlusIdx && !m_MailInfo.mailPlus[i].bGetOut)
		{
			memcpy(pPlus, &m_MailInfo.mailPlus[i].plusItem, sizeof(CHAT_MAILPLUS_ITEM));
			++pPlus;
			nSize += sizeof(CHAT_MAILPLUS_ITEM);
		}
	}

	//-------------------------------------------------
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Mail;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SETMAILDATA );
	
	pParam->Push( m_MailListInfo[m_MailInfo.mailIdx].mailId );
	pParam->Push( BinPair( buf, nSize ) );
	pParam->Push( m_MailInfo.updateInfo.newPostMoney );
	pParam->Push( m_MailInfo.updateInfo.newCostMoney );
	
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
	
	//---------------------------------------------------

	m_CurDBOpe = enDBOpe;

	return chat_err_none;
}

void MailManager_S::LoadMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_CurDBOpe = enMailDBOpe_None;

	if(!nDBOpeRst)
		return;

	unsigned int MailID;
	char* pText = NULL;
	char* pPlus = NULL;
	int nTextSize = pRet->GetData( 0, 1, &pText );
	int nPlusSize = pRet->GetData( 0, 2, &pPlus );
	pRet->GetData( 0, 0, MailID );

	TItemtransfersData transData[MAXCOUNT_MAILPLUS];
	nPlusSize = KItem::GetItemtransfersData( transData, pPlus, nPlusSize );

	if(nPlusSize % sizeof(CHAT_MAILPLUS_ITEM))
		return;

	int nPlusCount = nPlusSize / sizeof(CHAT_MAILPLUS_ITEM);
	
	if(nPlusCount > MAXCOUNT_MAILPLUS)
		return;

	if( nTextSize + nPlusSize + sizeof(VARLEN_PROTOCOL_HEADER) > MAXSIZE_SEND_BUF)
		return;
/*
	PCHAT_MAILPLUS_ITEM	pPlusData = (PCHAT_MAILPLUS_ITEM)(pPlus);
	for(int i = 0; i < nPlusCount; ++i)
	{
		m_MailInfo.mailPlus[i].bGetOut = false;
		m_MailInfo.mailPlus[i].plusItem = *pPlusData;
		++pPlusData;
	}
//*/

	m_MailInfo.hasChanged = false;
	m_MailInfo.mailPlusCount = (BYTE)nPlusCount;
	m_MailInfo.mailIdx = MailId2Idx(MailID);
	_ASSERT(-1 != m_MailInfo.mailIdx);

	if(-1 != m_MailInfo.mailIdx)
		m_MailInfo.mailCost = m_MailListInfo[m_MailInfo.mailIdx].mailCost;
	else
		m_MailInfo.mailCost = 0;

	if (sizeof(CHAT_STRUCTURED_DATABLOCK) -1 + sizeof( DBTASK_GETMAILDATA_RET ) - 1 + \
		nPlusSize + nTextSize > MAXSIZE_SEND_BUF)
		return;

	char buf[MAXSIZE_SEND_BUF];
	PCHAT_STRUCTURED_DATABLOCK	pDataToClient = (PCHAT_STRUCTURED_DATABLOCK)buf;
	pDataToClient->protocol.protocol = s2c_chat_family;
	pDataToClient->protocol.subProtocol = chat_loadmail;

	PDBTASK_GETMAILDATA_RET pMailData = (PDBTASK_GETMAILDATA_RET)pDataToClient->data;

	pMailData->nMailTextSize = nTextSize;
	//pMailData->nMailPlusSize = nPlusSize;
	pMailData->nMailPlusSize = nPlusCount * sizeof(TItemtransfersDataBase);
	pMailData->unMailID	=	MailID;

	memcpy( pMailData->pMailData, pText, nTextSize );

	PCHAT_MAILPLUS_ITEM	pPlusData = (PCHAT_MAILPLUS_ITEM)(transData);
	/*
	** 存储、读取数据使用TItemtransfersData
	** 传输数据用TItemtransfersDataBase
	*/
	TItemtransfersDataBase* pMailPlusData = (TItemtransfersDataBase*)(pMailData->pMailData + nTextSize);
	for(int i = 0; i < nPlusCount; ++i)
	{
		m_MailInfo.mailPlus[i].bGetOut = false;
		m_MailInfo.mailPlus[i].plusItem = *pPlusData;		
		memcpy( pMailPlusData, pPlusData, sizeof(TItemtransfersDataBase) );
		++pMailPlusData;
		++pPlusData;
	}
	//memcpy( pMailData->pMailData + nTextSize , pPlus, nPlusSize );

	pDataToClient->protocol.len =
	sizeof(CHAT_STRUCTURED_DATABLOCK) -1 + sizeof( DBTASK_GETMAILDATA_RET ) - 1 +
	pMailData->nMailPlusSize + nTextSize - PROTOCOL_SIZE;

	//Compression Begin .....................................................................
	int nOldSize = sizeof( DBTASK_GETMAILDATA_RET ) - 1 + pMailData->nMailPlusSize + nTextSize;
	
	unsigned char szCompressionBuff[MAXSIZE_SEND_BUF];
	unsigned int nLen = MAXSIZE_SEND_BUF;
	lzo1x_1_compress( 
		pDataToClient->data,
		nOldSize,
		szCompressionBuff,
		&nLen,
		wrkmem);
	
	if (nLen >= MAXSIZE_SEND_BUF - sizeof(CHAT_STRUCTURED_DATABLOCK) )
		return;
	
	memcpy(pDataToClient->data,szCompressionBuff,nLen);          
	pDataToClient->protocol.len = nLen + sizeof(CHAT_STRUCTURED_DATABLOCK) - 1 - PROTOCOL_SIZE;

	//Compression End .......................................................................

	SendDataToClient(nPlayerIdx, pDataToClient, pDataToClient->protocol.len + PROTOCOL_SIZE);

}

void MailManager_S::GetOutMoneyRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_CurDBOpe = enMailDBOpe_None;

	if(!nDBOpeRst)
		return;

	_ASSERT(-1 != m_MailInfo.mailIdx);

	if(-1 == m_MailInfo.mailIdx)
		return;

	m_MailInfo.hasChanged = true;

	int postMoney = (int)m_MailListInfo[m_MailInfo.mailIdx].postMoney;
	if (postMoney < 0)
		return;

	if (postMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
	{
		LogEventParam logParam;
		logParam.event = log_event_get_out_mail_money;
		logParam.param1 = Player[nPlayerIdx].GetGUID();
		strncpy(logParam.param2.data, m_MailListInfo[m_MailInfo.mailIdx].senderName, sizeof(logParam.param2.data));
		logParam.param2.data[sizeof(logParam.param2.data) - 1] = 0;
		logParam.param4 = postMoney;
		g_pLogSystem->Log(logParam);
	}

	AddMoney(nPlayerIdx, postMoney);
	m_MailListInfo[m_MailInfo.mailIdx].postMoney = 0;

	ChatOpeNotify(nPlayerIdx, 
				  chat_getoutmoney,
				  m_MailListInfo[m_MailInfo.mailIdx].mailId,
				  0,
				  SUCCESS_GETOUT_MAILMONEY
				  );

	if((int)m_MailInfo.mailCost > 0)
		MailMoneyReq(nPlayerIdx, m_MailListInfo[m_MailInfo.mailIdx].senderName, m_MailInfo.mailCost);
}

// The following code make sure the maillist's size not exceed the buffer size
static ASSERT_NOT_GREATER<sizeof(CHAT_STRUCTURED_DATABLOCK) + 
	sizeof(DBTASK_GETROLEMAILLIST_RET) + 
	MAX_MAILCOUNT_PERPLAYER * sizeof(DBTASK_MAILLISTITEM), 
	MAXSIZE_SEND_BUF
	> __AssertMailListSizeValid;

void MailManager_S::LoadMailListRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_CurDBOpe = enMailDBOpe_None;
	
	if(!nDBOpeRst)
		return;
	
	// Clear previous mail list information
	ClearCacheInfo();

	int nRowCount = pRet->GetRowCount( );
	int nColCount = pRet->GetColCount( );
	
	if(nRowCount <= 0)
		return;
	
	char buf[MAXSIZE_SEND_BUF];
	int	nSendMailListSize = 0;//nDataSize;
	PCHAT_STRUCTURED_DATABLOCK	pSendData = (PCHAT_STRUCTURED_DATABLOCK)buf;
	
	PDBTASK_GETROLEMAILLIST_RET	pMailListRet = (PDBTASK_GETROLEMAILLIST_RET)pSendData->data;
	PDBTASK_MAILLISTITEM		pMailList = (PDBTASK_MAILLISTITEM)pMailListRet->pMailList;
	pMailListRet->nMailListCount  = nRowCount;
	pMailListRet->nMailTotalCount = pRet->GetRet();
	
	if(sizeof(CHAT_STRUCTURED_DATABLOCK) - 1 + sizeof(DBTASK_GETROLEMAILLIST_RET) -1 + \
		nRowCount * sizeof(DBTASK_MAILLISTITEM) > MAXSIZE_SEND_BUF)
		return;

	for(int i = 0; i < nRowCount && i < MAX_MAILCOUNT_PERPLAYER; ++i)
	{
		int nCol = 0;
		pRet->GetData( i, nCol++, pMailList[i].unID );
		pRet->GetData( i, nCol++, pMailList[i].tagMailInfo.szSenderName, sizeof(pMailList[i].tagMailInfo.szSenderName) );
		pRet->GetData( i, nCol++, pMailList[i].tagMailInfo.szTile, sizeof(pMailList[i].tagMailInfo.szTile) );
		pRet->GetData( i, nCol++, *((int*)&pMailList[i].enState) );
		pRet->GetData( i, nCol++, *((int*)&pMailList[i].tagMailInfo.enSenderType) );
		pRet->GetData( i, nCol++, *((int*)&pMailList[i].tagMailInfo.enMailType) );
		pRet->GetData( i, nCol++, pMailList[i].tagMailInfo.nPostMoney );
		pRet->GetData( i, nCol++, pMailList[i].tagMailInfo.nMailCost );
		pRet->GetData( i, nCol++, pMailList[i].tagMailInfo.bHasApp);
		
		int nExpireTime = 0; 
		pRet->GetData( i, nCol++, nExpireTime);
		pMailList[i].unDeadSeconds = nExpireTime > 0 ? nExpireTime:0;	
		
		m_MailListInfo[i].mailId = pMailList[i].unID;
		m_MailListInfo[i].state = pMailList[i].enState;
		m_MailListInfo[i].senderType = (BYTE)pMailList[i].tagMailInfo.enSenderType;
		m_MailListInfo[i].mailCost = pMailList[i].tagMailInfo.nMailCost;
		m_MailListInfo[i].postMoney = pMailList[i].tagMailInfo.nPostMoney;
		
		strncpy(m_MailListInfo[i].senderName, 
			pMailList[i].tagMailInfo.szSenderName, sizeof(m_MailListInfo[i].senderName));
		
		nSendMailListSize += sizeof(DBTASK_MAILLISTITEM);
	}
	
	pSendData->protocol.protocol = s2c_chat_family;
	pSendData->protocol.subProtocol = chat_loadmaillist;
	pSendData->protocol.len = sizeof(CHAT_STRUCTURED_DATABLOCK) - 1 \
		+ sizeof(DBTASK_GETROLEMAILLIST_RET) - 1 + nSendMailListSize  - PROTOCOL_SIZE;
	
	//Compression Begin .....................................................................
	int nOldSize = sizeof(DBTASK_GETROLEMAILLIST_RET) - 1 + nSendMailListSize;
	
	unsigned char szCompressionBuff[MAXSIZE_SEND_BUF];
	unsigned int nLen = MAXSIZE_SEND_BUF;
	lzo1x_1_compress( 
		pSendData->data,
		nOldSize,
		szCompressionBuff,
		&nLen,
		wrkmem);
	
	if (nLen >= MAXSIZE_SEND_BUF - sizeof(CHAT_STRUCTURED_DATABLOCK) )
		return;
	
	memcpy(pSendData->data,szCompressionBuff,nLen);          
	pSendData->protocol.len = nLen + sizeof(CHAT_STRUCTURED_DATABLOCK) - 1 - PROTOCOL_SIZE;
	
	//Compression End .......................................................................

	SendDataToClient(nPlayerIdx, buf, pSendData->protocol.len + PROTOCOL_SIZE);
}

void MailManager_S::QueryNewMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_CurDBOpe = enMailDBOpe_None;
	
	if(!nDBOpeRst)
		return;

	int nNewMailCount;

	pRet->GetData( 0, 0, nNewMailCount );

	NewMailNotify(nPlayerIdx, nNewMailCount, NULL);
}

void MailManager_S::GetOutPlusRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_CurDBOpe = enMailDBOpe_None;

	if(!nDBOpeRst)
		return;

	_ASSERT(-1 != m_MailInfo.mailIdx);

	if(-1 == m_MailInfo.mailIdx)
		return;

	int nPlusIdx = m_MailInfo.updateInfo.curGetOutPlusIdx;

	if(nPlusIdx >= 0 && nPlusIdx < MAXCOUNT_MAILPLUS)
	{	
		int addItemIndex = AddItem(nPlayerIdx, &m_MailInfo.mailPlus[nPlusIdx].plusItem.data);
		if( addItemIndex > 0 )
		{
			KPlayer& player = Player[nPlayerIdx];
			KItem& item = Item[addItemIndex];
			const FSGUID &oldGuid = item.GetGUID();
		
			if (g_pController != NULL && '\0' == oldGuid.data[0])
			{
				FSGUID guid;
				g_pController->GenGUID(guid.data, g_GuidPadding);
				item.SetGUID(guid);
			}		

			//统计：邮件获取物品
			ItemTemplateId templateId;
			item.GetItemTemplateId(templateId);
			player.GetPlayerStatistic().AddItem(templateId, item.GetItemCount(), item_count_type_mail_in);
			
			//日志：邮件获取物品
			if (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
			{
				LogEventParam getMailItemEvent;
				getMailItemEvent.event = log_event_get_mail_item;
				getMailItemEvent.param1 = player.GetGUID();
				getMailItemEvent.param2 = item.GetGUID();
				char itemIdBuff[32] = { 0 };
				item.GetItemTemplateId(itemIdBuff, sizeof(itemIdBuff));
				itemIdBuff[sizeof(itemIdBuff) - 1] = 0;
				snprintf(getMailItemEvent.param3.data, sizeof(getMailItemEvent.param3.data), "%s %s", itemIdBuff, m_MailListInfo[m_MailInfo.mailIdx].senderName);
				getMailItemEvent.param3.data[sizeof(getMailItemEvent.param3.data) - 1] = 0;
				g_pLogSystem->Log(getMailItemEvent);

				//InstantSave
				//player.SaveItemData();
			}
			
			if ( item.GetGenre() == item_ib )
			{
				KIBLog::getSingleton().TransferIBItem( mail_auction_exchange, item.GetGUID(), player.GetPlayerIndex() );
			}

			m_MailInfo.hasChanged = true;
			m_MailInfo.mailPlus[nPlusIdx].bGetOut = true;
			m_MailInfo.updateInfo.curGetOutPlusIdx = -1;
			--m_MailInfo.mailPlusCount;

			ChatOpeNotify(
				nPlayerIdx, 
				chat_getoutitem, 
				m_MailListInfo[m_MailInfo.mailIdx].mailId,  
				nPlusIdx,
				SUCCESS_GETOUT_MAILPLUS );

			if((int)m_MailInfo.mailCost > 0)
				MailMoneyReq(nPlayerIdx, 
				m_MailListInfo[m_MailInfo.mailIdx].senderName, 
				m_MailInfo.mailCost);
		}
	}

}

void MailManager_S::SendMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_CurDBOpe = enMailDBOpe_None;

	if(!nDBOpeRst)
	{
		int nPostMoney = 0;
		int nMailPlusSize = 0;
		void* pMailPlus = NULL;
		pRet->GetData( 0, 0, nPostMoney );

		if (nPostMoney >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_add_money_amount))
		{
			LogEventParam logParam;
			logParam.event = log_event_send_mail_failed_return_money;
			logParam.param1 = Player[nPlayerIdx].GetGUID();
			strncpy(logParam.param2.data, m_SendMailInfo.receiverName, sizeof(logParam.param2.data));
			logParam.param2.data[sizeof(logParam.param2.data) - 1] = 0;
			logParam.param4 = nPostMoney;
			g_pLogSystem->Log(logParam);
		}

		AddMoney(nPlayerIdx, nPostMoney);

		nMailPlusSize = pRet->GetData( 0, 1, &pMailPlus );

		int nPlusCount = nMailPlusSize / sizeof(CHAT_MAILPLUS_ITEM);
		PCHAT_MAILPLUS_ITEM pItem = (PCHAT_MAILPLUS_ITEM)pMailPlus;

		for(int i = 0; i < nPlusCount; ++i)
		{
			AddItem(nPlayerIdx, &pItem->data);
			++pItem;
		}

		ChatErrCodeToClient(nPlayerIdx, chat_err_sendmailfailed);	
	}	
	else
	{
		ChatErrCodeToClient(nPlayerIdx, chat_err_sendmailsuccess);

		int nReceiverIdx = g_PlayerInfoToIndex.GetIndexByName(m_SendMailInfo.receiverName);

		if(INVALID_PLAYER_INDEX != nReceiverIdx)
		{
			NewMailNotify(nReceiverIdx, 1, GetPlayerName(nPlayerIdx));
		}

		//日志：发送邮件
		LogEventParam sendMailEvent;
		sendMailEvent.event = log_event_send_mail;
		sendMailEvent.param1 = Player[nPlayerIdx].GetGUID();
		strncpy(sendMailEvent.param2.data, m_SendMailInfo.receiverName, sizeof(sendMailEvent.param2.data));		
		sendMailEvent.param2.data[sizeof(sendMailEvent.param2.data) - 1] = 0;
		g_pLogSystem->Log(sendMailEvent);

	}

}

void MailManager_S::DelMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet)
{
	m_CurDBOpe = enMailDBOpe_None;

	if(!nDBOpeRst)
		return;

	_ASSERT(-1 != m_MailInfo.mailIdx);

	if(-1 == m_MailInfo.mailIdx)
		return;

	DelCurMail(nPlayerIdx);

}

void MailManager_S::CloseMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet )
{
	m_CurDBOpe = enMailDBOpe_None;	
}

void MailManager_S::MailMoneyReq(int nPlayerIdx, const char *szReceiverName, DWORD dwMoney)
{
	const char *szTile = MSG_MAILMONEY_TITLE;
	int nTitleLen = strlen(szTile);

	//-------------------------------------------------

	int nMaxMailPerPlayer = ConfigManager::Singleton().GetGlobalVariable(global_var_max_mails_per_player);
	if  (nMaxMailPerPlayer == 0)
	{
		nMaxMailPerPlayer = MAIL_RECEIVERMAXMAIL;
	}

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Mail;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SENDMAIL );
	
	pParam->Push( GetPlayerName(nPlayerIdx) );
	pParam->Push( szReceiverName );
	pParam->Push( szTile );
	pParam->Push( 0 );
	pParam->Push( enMailSenderType_Player );
	pParam->Push( enMailType_Text );
	pParam->Push( BinPair( szTile, nTitleLen ) );
	pParam->Push( NullPair( ) );
	pParam->Push( dwMoney );
	pParam->Push( 0 );
	pParam->Push( MAIL_LIVINGDATA );		//邮件最大生存周期
	pParam->Push( MAIL_SENDERMAXMAIL * 2 );	//最大发送邮件数
	pParam->Push( nMaxMailPerPlayer * 2 );	//最大接受邮件数
	
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );

	strncpy(m_SendMailInfo.receiverName, 
		szReceiverName, 
		sizeof(m_SendMailInfo.receiverName) );

	m_MailInfo.mailCost = 0;

  m_CurDBOpe = enMailDBOpe_SendMail;

}

int MailManager_S::ReturnMailReq(int nPlayerIdx, BYTE *pData, int nDataSize)
{
	if(enMailDBOpe_None != m_CurDBOpe)
		return char_err_dbreqondoing;

	if(-1 == m_MailInfo.mailIdx)
	{
		_ASSERT(false);
		return chat_err_mailnotfound;	
	}

	if(enMailSenderType_Player != m_MailListInfo[m_MailInfo.mailIdx].senderType)
		return chat_err_mailnotallowreturn;

	if(m_MailInfo.hasChanged)
		return chat_err_mailnotallowreturn;

	m_CurDBOpe = enMailDBOpe_ReturnMail;

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = GetNetConnectIdx(nPlayerIdx);
	DBHeader.ProcType = Proc_Mail;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_RETURNMAIL );
	pParam->Push( m_MailListInfo[m_MailInfo.mailIdx].mailId );
	pParam->Push( MSG_MAIL_RETURN_TITLE );
	pParam->Push( MAIL_LIVINGDATA );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );

	return chat_err_none;
}

void MailManager_S::ReturnMailRet(int nDBOpeRst, int nPlayerIdx, IProcRet* pRet )
{
	m_CurDBOpe = enMailDBOpe_None;

	if( nDBOpeRst )
	{
		char szReceiverName[MAXSIZE_ROLENAME];

		DelCurMail(nPlayerIdx);
		ChatErrCodeToClient(nPlayerIdx, chat_err_returnmailsucceed);
		pRet->GetData( 0, 0, szReceiverName, sizeof(szReceiverName) );
	
		int nReceiverIdx = g_PlayerInfoToIndex.GetIndexByName(szReceiverName);
		
		if(INVALID_PLAYER_INDEX != nReceiverIdx)
		{
			NewMailNotify(nReceiverIdx, 1, GetPlayerName(nPlayerIdx));
		}
	}
	else
		ChatErrCodeToClient(nPlayerIdx, chat_err_returnmailfailed);
}

void MailManager_S::DelCurMail(int nPlayerIdx)
{
	CHAT_DELMAIL_REQ	ret;
	ret.protocol.protocol = s2c_chat_family;
	ret.protocol.subProtocol = chat_delmail;
	ret.protocol.len = sizeof(ret) - PROTOCOL_SIZE;
	ret.mailId = m_MailListInfo[m_MailInfo.mailIdx].mailId;
	SendDataToClient(nPlayerIdx, &ret, ret.protocol.len + PROTOCOL_SIZE);

	m_MailListInfo[m_MailInfo.mailIdx].mailId = INVALID_MAIL_ID;
	m_MailInfo.mailIdx = -1;	
}