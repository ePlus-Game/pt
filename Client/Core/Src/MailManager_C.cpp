//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 14:28
//      File_base        : MailManager_C
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
#include "MailManager_C.h"

using namespace CHAT;

int MailManager_C::SendMail(const MAIL_PARAM *pMailParam)
{
	if (!CheckPreTime())
		return chat_err_opetoofast;

	if( 0 == strcmp(pMailParam->strReceiver, GetPlayerName(CLIENT_PLAYER_INDEX)) )
		return chat_err_sendmailfailed;

	int nTax = 0;
	if (pMailParam->nItemCount == 0)
		nTax = m_SendTextTax;
	else
		nTax = m_SendItemTax;


	int nRet = CheckMail(CLIENT_PLAYER_INDEX, 
						 pMailParam->szTitle, 
						 pMailParam->postMoney,
						 pMailParam->nContentLen, 
						 pMailParam->szContent, 
						 pMailParam->nItemCount, 
						 (const DWORD*)pMailParam->pPlusData,
						 nTax);

	if(chat_err_none != nRet)
	{
		return nRet;
	}

	char buf[MAXSIZE_MAIL];
	memset(buf, 0, sizeof(buf));

	PCHAT_STRUCTURED_DATABLOCK	pSendMail = (PCHAT_STRUCTURED_DATABLOCK)buf;
	PDBTASK_SENDMAILS_REQ		pMail = (PDBTASK_SENDMAILS_REQ)pSendMail->data;

	pSendMail->protocol.protocol = c2s_chat_family;
	pSendMail->protocol.subProtocol = chat_sendmail;

	strncpy(pMail->tagMailInfo.szSenderName, GetPlayerName(CLIENT_PLAYER_INDEX), 
				sizeof(pMail->tagMailInfo.szSenderName));
	strncpy(pMail->tagMailInfo.szReceiverName, pMailParam->strReceiver, 
				sizeof(pMail->tagMailInfo.szReceiverName));
	strncpy(pMail->tagMailInfo.szTile, pMailParam->szTitle, 
				sizeof(pMail->tagMailInfo.szTile));
	memcpy(pMail->pMailData, pMailParam->szContent, pMailParam->nContentLen);
	pMail->tagMailInfo.nPostMoney = pMailParam->postMoney;
	pMail->tagMailInfo.nMailCost = pMailParam->costMoney;
	pMail->tagMailInfo.enSenderType = enMailSenderType_Player;
	pMail->tagMailInfo.enMailType = pMailParam->nItemCount > 0 ? enMailType_Plugin : enMailType_Text;
	pMail->nMailTextSize = pMailParam->nContentLen;
	pMail->nMailPlusSize = pMailParam->nItemCount * sizeof(DWORD);
	
	// On client, accessory is the item id
	char *pAccessory = pMail->pMailData + pMailParam->nContentLen;
	memcpy(pAccessory, pMailParam->pPlusData, pMail->nMailPlusSize);

	int nMailSize = sizeof(DBTASK_SENDMAILS_REQ) - 1 + pMail->nMailTextSize + pMail->nMailPlusSize;
	pSendMail->protocol.len = nMailSize + sizeof(CHAT_STRUCTURED_DATABLOCK) - 1 - PROTOCOL_SIZE;

	SendDataToServer(buf, pSendMail->protocol.len + PROTOCOL_SIZE);
	
	return chat_err_none;
}

int MailManager_C::LoadMailListReq(int nPage)
{
	Sleep(300);
	if (!CheckPreTime())
		return chat_err_opetoofast;
	//Init();
	m_MailCont.clear();

	CHAT_LOADMAILLIST_REQ	data;
	data.protocol.protocol    = c2s_chat_family;
	data.protocol.subProtocol = chat_loadmaillist;
	data.nPage                = nPage;
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);	

	return chat_err_none;
}

int	MailManager_C::LoadMailReq(DWORD dwMailId)
{
	if (!CheckPreTime())
		return chat_err_opetoofast;
	
	if( m_MailCont.find(dwMailId) == m_MailCont.end() )
	{
		return chat_err_mailnotfound;
	}

	CHAT_LOADMAIL_REQ	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_loadmail;
	data.mailId = dwMailId;
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

void MailManager_C::LoadMailListRet(BYTE *pData)
{
	m_MailCont.clear();

	PDBTASK_GETROLEMAILLIST_RET	pMailList = (PDBTASK_GETROLEMAILLIST_RET)pData;
	PDBTASK_MAILLISTITEM	pHeader = (PDBTASK_MAILLISTITEM)pMailList->pMailList;

	m_TotalMailCont                 = pMailList->nMailTotalCount;

	for(int i = 0; i < pMailList->nMailListCount; ++i)
	{
		CHAT_MAILDATA_CLIENT	data;
		memset(&data, 0, sizeof(data));
		data.bHasMailData = false;
		data.header = *pHeader;

		m_MailCont.insert( MAILCONT::value_type(pHeader->unID, data) );
		++pHeader;
	}
}

int	MailManager_C::GetOutMoneyReq(DWORD dwMailId)
{
	if (!CheckPreTime())
		return chat_err_opetoofast;
		
	if( m_MailCont.end() == m_MailCont.find(dwMailId) )
		return chat_err_mailnotfound;

	PCHAT_MAILDATA_CLIENT	pMailData = (PCHAT_MAILDATA_CLIENT)GetMailData(dwMailId);

	if(NULL == pMailData)
		return chat_err_mailnotfound;

	if(pMailData->header.tagMailInfo.nPostMoney <= 0)
		return chat_err_none;

	VARLEN_PROTOCOL_HEADER	c2sReq;
	c2sReq.protocol = c2s_chat_family;
	c2sReq.subProtocol = chat_getoutmoney;
	c2sReq.len = sizeof(c2sReq) - PROTOCOL_SIZE;

	SendDataToServer(&c2sReq, c2sReq.len + PROTOCOL_SIZE);

	return chat_err_none;
}

int	MailManager_C::GetOutItemReq(DWORD dwMailId, int nIndex)
{	
	if (!CheckPreTime())
		return chat_err_opetoofast;

	if(nIndex < 0 || nIndex >= MAXCOUNT_MAILPLUS)
	{
		return chat_err_none;
	}

	if( m_MailCont.find(dwMailId) == m_MailCont.end() )
	{
		return chat_err_mailnotfound;
	}

	PCHAT_MAILDATA_CLIENT	pMailData = (PCHAT_MAILDATA_CLIENT)GetMailData(dwMailId);
	
	if(NULL == pMailData)
	{
		return chat_err_mailnotfound;
	}

	ItemPos checkPos;
	if( !Player[CLIENT_PLAYER_INDEX].m_ItemList.SearchPosition(&checkPos) )
		return char_err_packagefull;

	CHAT_GETOUT_ITEM	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_getoutitem;
	data.protocol.len = sizeof(data) -	PROTOCOL_SIZE;
	data.mailId = dwMailId;
	data.index = (BYTE)nIndex;

	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

int	MailManager_C::CloseMailReq(DWORD dwMailId)
{
	if( m_MailCont.find(dwMailId) == m_MailCont.end() )
	{
		return chat_err_mailnotfound;
	}

	CHAT_CLOSEMAIL_REQ	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_closemail;
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
	data.mailId = dwMailId;

	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

int	MailManager_C::DelMailReq(DWORD dwMailId)
{
	if (!CheckPreTime())
		return chat_err_opetoofast;

	if(NULL == GetMailData(dwMailId))
		return chat_err_delmailfailed;

	CHAT_DELMAIL_REQ	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_delmail;
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
	data.mailId = dwMailId;

	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

void MailManager_C::LoadMailRet(BYTE *pData)
{
	PDBTASK_GETMAILDATA_RET	pDBMailData = (PDBTASK_GETMAILDATA_RET)pData;

	MAILCONT::iterator itMail = m_MailCont.find(pDBMailData->unMailID);

	if( itMail == m_MailCont.end() )
	{
		return;
	}

	_ASSERT( !(pDBMailData->nMailPlusSize % sizeof(TItemtransfersDataBase)) );
	if( pDBMailData->nMailPlusSize % sizeof(TItemtransfersDataBase) )
		return;

	itMail->second.header.enState = enMailState_Read;
	itMail->second.bHasMailData = true;
	itMail->second.plusCount = pDBMailData->nMailPlusSize / sizeof(TItemtransfersDataBase);
	TItemtransfersDataBase* pPlusData = (TItemtransfersDataBase*)(pDBMailData->pMailData + pDBMailData->nMailTextSize);

	if (itMail->second.plusCount > MAXCOUNT_MAILPLUS)
		itMail->second.plusCount  = MAXCOUNT_MAILPLUS;

	memcpy(itMail->second.szContent, pDBMailData->pMailData, pDBMailData->nMailTextSize);
	
	/*
	** 存储、读取用TItemtransfersData
	** 传输用TItemtransfersDataBase
	*/
	ZeroMemory(itMail->second.plusData, itMail->second.plusCount*sizeof(TItemtransfersData));
	for (int i = 0; i < itMail->second.plusCount; ++i)
	{
		memcpy(&(itMail->second.plusData[i]), pPlusData, sizeof(TItemtransfersDataBase));
		pPlusData++;
	}
	//memcpy(itMail->second.plusData, pPlusData, pDBMailData->nMailPlusSize);
}

BYTE* MailManager_C::GetMailData(DWORD dwMailId)
{
	MAILCONT::const_iterator iterMail = m_MailCont.find(dwMailId);
	
	if( iterMail != m_MailCont.end() && iterMail->second.bHasMailData)
	{
		return (BYTE*)&(iterMail->second);	
	}
	else
	{
		return NULL;
	}
}

void MailManager_C::GetOutPlusRet(BYTE *pData)
{
	PCHAT_OPE_NOTIFY		ps2cNotify = (PCHAT_OPE_NOTIFY)pData;
	PCHAT_MAILDATA_CLIENT	pMailData = (PCHAT_MAILDATA_CLIENT)GetMailData(ps2cNotify->param1);
	
	_ASSERT(pMailData);

	if(pMailData)
	{
		_ASSERT(ps2cNotify->param2 >= 0 && ps2cNotify->param2 < MAXCOUNT_MAILPLUS);

		--pMailData->plusCount;
		memset(&pMailData->plusData[ps2cNotify->param2], 
			   0, 
			   sizeof(pMailData->plusData[ps2cNotify->param2])
			  );
	}
}

void MailManager_C::GetOutMoneyRet(BYTE *pData)
{
	PCHAT_OPE_NOTIFY		ps2cNotify = (PCHAT_OPE_NOTIFY)pData;
	PCHAT_MAILDATA_CLIENT	pMailData = (PCHAT_MAILDATA_CLIENT)GetMailData(ps2cNotify->param1);
	
	_ASSERT(pMailData);

	if(pMailData)
	{
		pMailData->header.tagMailInfo.nPostMoney = 0;
	}
}

int MailManager_C::ReturnMailReq(DWORD dwMailId)
{
	if (!CheckPreTime())
		return chat_err_opetoofast;

	if(NULL == GetMailData(dwMailId))
		return chat_err_mailnotallowreturn;

	CHAT_RETURNMAIL_REQ	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_returnmail;
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
	data.mailId = dwMailId;

	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);	

	return chat_err_none;
}

int MailManager_C::GetNewMailCount()
{
	int nNewMailCount = 0;

	MAILCONT::const_iterator itMail = m_MailCont.begin();
	MAILCONT::const_iterator itMailEnd = m_MailCont.end();

	for(; itMail != itMailEnd; ++itMail)
	{
		if(itMail->second.header.enState == enMailState_New)
			++nNewMailCount;
	}

	return nNewMailCount;
}
