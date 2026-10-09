//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-26 12:13
//      File_base        : CoreRelated
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
#include "KSubWorld.h"
#include "KRegion.h"
#include "KItemGenerator.h"

#ifdef _SERVER
#include "MailManager_S.h"
#endif

#ifdef _SERVER

unsigned int g_GuidPadding;//GuidPadding

PlayerInfoToIndex	g_PlayerInfoToIndex;

//---------------------------------------------------------------------------------------------------------

void PlayerInfoToIndex::ForEach(CHATCALLBACK pCallBack,  const void *pCallbackParam,
					BROADCASTFILTER pFilter /* = NULL */, unsigned int uFilterPassby /* = 0 */)
{
	NAMETOINDEXCONT::iterator	itIdx;
	NAMETOINDEXCONT::iterator	endIdx = m_NameToIndexCont.end();

	for(itIdx = m_NameToIndexCont.begin(); itIdx != endIdx; ++itIdx)
	{
		if( NULL == pFilter || pFilter(itIdx->second, uFilterPassby) )
		{
			pCallBack(pCallbackParam, itIdx->second);
		}
	}
}

int	PlayerInfoToIndex::NextPlayerIdx(Iterator &iter)
{
	if( m_NameToIndexCont.end() == iter.iter )
		return INVALID_PLAYER_INDEX;
	else
	{
		int nIdx = iter.iter->second;
		++(iter.iter);
		return nIdx;
	}
}

#endif

#ifdef _SERVER
void ForEachRegionPlayer(KRegion &CurRegion,
						 CHATCALLBACK pCallBack,
						 const void *pCallbackParam,
						 BROADCASTFILTER pFilter,
						 unsigned int uFilterPassby
						 )
{
	KIndexNode *pNode = (KIndexNode *)CurRegion.m_NpcList.GetHead();

	while(pNode)
	{
		int nNpcIdx = pNode->m_nIndex;
		pNode = (KIndexNode*)pNode->GetNext();

		if( Npc[nNpcIdx].IsPlayer() )
		{
			int	nPlayerIdx = Npc[nNpcIdx].GetPlayerIdx();
			if( NULL == pFilter || pFilter(nPlayerIdx, uFilterPassby) )
				pCallBack(pCallbackParam, nPlayerIdx);
		}
	}
}

void ForEachLocalPlayer(int nSayerNpcIdx,
						CHATCALLBACK pCallBack,  
						const void *pCallbackParam, 
						BROADCASTFILTER pFilter /* = NULL */, 
						unsigned int uFilterPassby /* = 0 */
						)
{
	int	nSubWorldIdx = Npc[nSayerNpcIdx].GetSubWorldIndex();
	int	nCurRegionIdx = Npc[nSayerNpcIdx].m_RegionIndex;
	KRegion &CurRegion = SubWorld[nSubWorldIdx].m_Region[nCurRegionIdx];

	ForEachRegionPlayer(CurRegion, pCallBack, pCallbackParam, pFilter, uFilterPassby);

	for(int nLoop = 0; nLoop < 8; ++nLoop)
	{
		int nRegionIdx = CurRegion.m_nConnectRegion[nLoop];

		if(-1 != nRegionIdx)
		{
			ForEachRegionPlayer(SubWorld[nSubWorldIdx].m_Region[nRegionIdx],
								pCallBack,
								pCallbackParam,
								pFilter,
								uFilterPassby
								);	
		}
	}		
}
#endif

#ifdef _SERVER

#define MAX_LENGTH_DUMP_STACK_STRING 1024 * 4
#define MAX_DUMP_LAYER               10
#define MAX_LENGTH_PER_STACK_LAYER   512

void DumpInvalidItemOpeStack(const bool bDumpStack,const char * szExtraComment,const int nLayer)
{
	static char szDumpString[MAX_LENGTH_DUMP_STACK_STRING];

	if (szExtraComment == NULL || nLayer >= MAX_DUMP_LAYER || nLayer <= 0)
		return;
	
	int nTotalSize  = 0;
	szDumpString[0] = 0;

	strncpy(szDumpString,szExtraComment,sizeof(szDumpString));
	szDumpString[MAX_LENGTH_DUMP_STACK_STRING - 1] = 0;
	nTotalSize = strlen(szDumpString) + 1;

#ifndef WIN32

	if (bDumpStack)
	{
		char          szStackInfo[MAX_LENGTH_PER_STACK_LAYER] = "";
		unsigned long ulStack[MAX_DUMP_LAYER];
		getcallstack( ulStack, nLayer );
	
		for (int nLoopCount = 0 ; nLoopCount < nLayer ; nLoopCount ++ )
		{
			snprintf(szStackInfo,sizeof(szStackInfo),"%X\n",ulStack[nLoopCount]);
			szStackInfo[MAX_LENGTH_PER_STACK_LAYER - 1] = 0;
			
			int nLayerSize = strlen(szStackInfo) + 1;
			
			if (nTotalSize + nLayerSize >= MAX_LENGTH_DUMP_STACK_STRING )
				break;
			
			strcat(szDumpString,szStackInfo);
			nTotalSize += nLayerSize;

		}//end for while
		
	}//endif

#endif

	szDumpString[MAX_LENGTH_DUMP_STACK_STRING - 1] = 0;

	if (g_pLogSystem)
	{	
		g_pLogSystem->SysDbgLog(szDumpString, strlen(szDumpString), sys_dbg_log_event_item_ope);
	}//endif
	
}

void ChatErrCodeToClient(int nPlayerIdx, int nErrCode)
{
	CHAT_ERR_CODE	data;

	data.protocol.protocol = s2c_chat_family;
	data.protocol.subProtocol = chat_errcode;
	data.errorCode = (short int)nErrCode;
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

	SendDataToClient(nPlayerIdx, &data, data.protocol.len + PROTOCOL_SIZE);
}
#endif

#ifdef _SERVER
void NewMailNotify(int nPlayerIdx, int nMailCnt, const char *szSenderName)
{
	if(nMailCnt <= 0)
		return;

	CHAT_NEWMAIL_NOTIFY	s2cReq;
	s2cReq.protocol.protocol = s2c_chat_family;
	s2cReq.protocol.subProtocol = chat_newmailnotify;
	s2cReq.newMailCnt = nMailCnt;
	s2cReq.protocol.len = sizeof(s2cReq) - PROTOCOL_SIZE;

	if(1 == nMailCnt)
	{
		if(NULL == szSenderName)
			s2cReq.senderName[0] = '\0';
		else
			strncpy(s2cReq.senderName, szSenderName, MAXSIZE_ROLENAME);
	}

	SendDataToClient(nPlayerIdx, &s2cReq, s2cReq.protocol.len + PROTOCOL_SIZE);
}
#endif

#ifdef _SERVER
void ChatOpeNotify(int nPlayerIdx, BYTE subProtocol, DWORD dwParam1, DWORD dwParam2, DWORD dwParam3)
{
	_ASSERT(subProtocol > chat_subprotocol_begin && subProtocol < chat_subprotocol_end);

	if(subProtocol > chat_subprotocol_begin && subProtocol < chat_subprotocol_end)
	{
		CHAT_OPE_NOTIFY		s2cNotify;
		s2cNotify.protocol.protocol = s2c_chat_family;
		s2cNotify.protocol.subProtocol = subProtocol;
		s2cNotify.param1 = dwParam1;
		s2cNotify.param2 = dwParam2;
		s2cNotify.param3 = dwParam3;
		s2cNotify.protocol.len = sizeof(s2cNotify) - PROTOCOL_SIZE;

		SendDataToClient(nPlayerIdx, &s2cNotify, s2cNotify.protocol.len + PROTOCOL_SIZE);	
	}
}
#endif

#ifdef _SERVER
int	 GetTeamMember(int nTeamId, int *pOutRst, int nCapacity)
{
	if(INVALID_TEAM_ID == nTeamId)
		return 0;

	int	nCount = 0;

	KTeam& team = g_TeamS[nTeamId];
	for(int i = 0; i < MAX_BIG_TEAM_MEMBER && i < nCapacity; ++i)
	{
		int memberPlayerIndex = team.GetMemberPlayerIndex(i);
		if(memberPlayerIndex > 0)
		{
			*pOutRst++ = memberPlayerIndex;
			++nCount;
		}
	}

	return nCount;
}
#endif

// Copy and Changed from FS1's code
bool LoadTextFilterExp(const char *szFile)
{
#ifndef _SERVER
	KPakFile	FileChatFlt;
#else
	KFile		FileChatFlt;
#endif

	if( !FileChatFlt.Open((char*)szFile) )
		return false;

	char*	pBuffer = NULL;
	int		nSize = FileChatFlt.Size();

	if (nSize)
		pBuffer = (char*)malloc(nSize + 1);

	if(NULL == pBuffer)
	{
		FileChatFlt.Close();
		return false;
	}
	
	int nFinalSize = FileChatFlt.Read(pBuffer, nSize);
	if (nFinalSize >= nSize)
		pBuffer[nSize] = 0;
	else
		memset(pBuffer + nFinalSize, 0, nSize - nFinalSize + 1);
	
	char* pLineHeader = pBuffer;
	
	do
	{
		char* pLineEnd = strchr(pLineHeader, 0x0a);
		int nLineLen;
		if (pLineEnd)
		{
			*pLineEnd = 0;
			nLineLen = pLineEnd - pLineHeader;
		}
		else
		{
			nLineLen = nFinalSize - (pLineHeader - pBuffer);
		}
		
		if (pLineHeader[0] && nLineLen > 0)
		{
			if (pLineHeader[nLineLen - 1] == 0x0d)
				pLineHeader[--nLineLen] = 0;
			if (nLineLen)
			{						
				g_TextFilter.AddExpression(pLineHeader);
			}
		}				
		pLineHeader = pLineEnd ? pLineEnd + 1: NULL;

	}while(pLineHeader);

	free(pBuffer);
	FileChatFlt.Close();

	return true;
}

bool LoadChatTxtFilterExp(const char * szFile)
{
#ifndef _SERVER
	KPakFile	FileChatFlt;
#else
	KFile		FileChatFlt;
#endif
	
	if( !FileChatFlt.Open((char*)szFile) )
		return false;
	
	char*	pBuffer = NULL;
	int		nSize = FileChatFlt.Size();
	
	if (nSize)
		pBuffer = (char*)malloc(nSize + 1);
	
	if(NULL == pBuffer)
	{
		FileChatFlt.Close();
		return false;
	}
	
	int nFinalSize = FileChatFlt.Read(pBuffer, nSize);
	if (nFinalSize >= nSize)
		pBuffer[nSize] = 0;
	else
		memset(pBuffer + nFinalSize, 0, nSize - nFinalSize + 1);
	
	char* pLineHeader = pBuffer;
	
	do
	{
		char* pLineEnd = strchr(pLineHeader, 0x0a);
		int nLineLen;
		if (pLineEnd)
		{
			*pLineEnd = 0;
			nLineLen = pLineEnd - pLineHeader;
		}
		else
		{
			nLineLen = nFinalSize - (pLineHeader - pBuffer);
		}
		
		if (pLineHeader[0] && nLineLen > 0)
		{
			if (pLineHeader[nLineLen - 1] == 0x0d)
				pLineHeader[--nLineLen] = 0;
			if (nLineLen)
			{						
				g_ChatTxtFilter.AddExpression(pLineHeader);
			}
		}				
		pLineHeader = pLineEnd ? pLineEnd + 1: NULL;
		
	}while(pLineHeader);
	
	free(pBuffer);
	FileChatFlt.Close();
	
	return true;
}

#ifndef _SERVER
bool LoadUserChatFilterExp(const char * szFile)
{
	KPakFile	FileChatFlt;
	
	if( !FileChatFlt.Open((char*)szFile) )
		return false;
	
	char*	pBuffer = NULL;
	int		nSize = FileChatFlt.Size();
	
	if (nSize)
		pBuffer = (char*)malloc(nSize + 1);
	
	if(NULL == pBuffer)
	{
		FileChatFlt.Close();
		return false;
	}
	
	int nFinalSize = FileChatFlt.Read(pBuffer, nSize);
	if (nFinalSize >= nSize)
		pBuffer[nSize] = 0;
	else
		memset(pBuffer + nFinalSize, 0, nSize - nFinalSize + 1);
	
	char* pLineHeader = pBuffer;
	
	do
	{
		char* pLineEnd = strchr(pLineHeader, 0x0a);
		int nLineLen;
		if (pLineEnd)
		{
			*pLineEnd = 0;
			nLineLen = pLineEnd - pLineHeader;
		}
		else
		{
			nLineLen = nFinalSize - (pLineHeader - pBuffer);
		}
		
		if (pLineHeader[0] && nLineLen > 0)
		{
			if (pLineHeader[nLineLen - 1] == 0x0d)
				pLineHeader[--nLineLen] = 0;
			if (nLineLen)
			{						
				g_UserChatFilter.AddExpression(pLineHeader);
			}
		}				
		pLineHeader = pLineEnd ? pLineEnd + 1: NULL;
		
	}while(pLineHeader);
	
	free(pBuffer);
	FileChatFlt.Close();
	
	return true;
}

bool LoadChatRecvFilterExp(const char * szFile)
{
	KPakFile	FileChatFlt;
	
	if( !FileChatFlt.Open((char*)szFile) )
		return false;
	
	char*	pBuffer = NULL;
	int		nSize = FileChatFlt.Size();
	
	if (nSize)
		pBuffer = (char*)malloc(nSize + 1);
	
	if(NULL == pBuffer)
	{
		FileChatFlt.Close();
		return false;
	}
	
	int nFinalSize = FileChatFlt.Read(pBuffer, nSize);
	if (nFinalSize >= nSize)
		pBuffer[nSize] = 0;
	else
		memset(pBuffer + nFinalSize, 0, nSize - nFinalSize + 1);
	
	char* pLineHeader = pBuffer;
	
	do
	{
		char* pLineEnd = strchr(pLineHeader, 0x0a);
		int nLineLen;
		if (pLineEnd)
		{
			*pLineEnd = 0;
			nLineLen = pLineEnd - pLineHeader;
		}
		else
		{
			nLineLen = nFinalSize - (pLineHeader - pBuffer);
		}
		
		if (pLineHeader[0] && nLineLen > 0)
		{
			if (pLineHeader[nLineLen - 1] == 0x0d)
				pLineHeader[--nLineLen] = 0;
			if (nLineLen)
			{						
				g_ChatRecvFilter.AddExpression(pLineHeader);
			}
		}				
		pLineHeader = pLineEnd ? pLineEnd + 1: NULL;
		
	}while(pLineHeader);
	
	free(pBuffer);
	FileChatFlt.Close();
	
	return true;
}
#endif

#ifndef _SERVER
const char* GetItemName(int nItemClass,int nDetailType,int nParticualrType,int nLevel)
{
	const KBASICPROP_ITEM *pItemBaseProp = g_ItemGen.GetItemTemplate(nItemClass,
		nDetailType,
		nParticualrType,
		nLevel
		);

	if(NULL != pItemBaseProp)
		return pItemBaseProp->szName;
	else
		return NULL;
}
#endif

#ifndef _SERVER
void ShowMsgInSysRoom(const char *szMsg)
{
	char	buf[sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG];
	PCHATROOMMSG_TO_SOMEONE	pRoomMsg = (PCHATROOMMSG_TO_SOMEONE)buf;

	strncpy(pRoomMsg->senderName, CHAT_SENDER_SYSTEMNAME, sizeof(pRoomMsg->senderName));
	pRoomMsg->msgLen = strlen(szMsg);

	memcpy(pRoomMsg->msg, szMsg, pRoomMsg->msgLen);
	pRoomMsg->msg[pRoomMsg->msgLen] = '\0';
	
	CoreDataChanged( GDCNI_RECV_CHAT_DATE_R2P, SYSTEM_ROOM_ID, (int)pRoomMsg );	
}
#endif

#ifdef _SERVER
bool SystemSendCustomMail(const char * szReceiver,
						  const char * szTitle,
						  const char * szContent,
						  const char * szSender)
{
	if(NULL == szReceiver)
		return false;

	if(NULL == szTitle)
		return false;

	if(NULL == szContent)
		return false;

	if (NULL == szSender)
	{
		return false;
	}

	int	nContSize = strlen(szContent) + 1;
	int nTotalMailSize = sizeof(DBTASK_SENDMAILS_REQ) + nContSize + 0 * sizeof(CHAT_MAILPLUS_ITEM);

	if(nTotalMailSize > MAXSIZE_MAIL)
		return false;

	BYTE buf[MAXSIZE_MAIL];

	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_Mail;

	IProcParam* pParam = g_pController->GetProcParam( );

	pParam->BeginPush( PN_SENDMAIL );

	pParam->Push( szSender );
	pParam->Push( szReceiver );
	pParam->Push( szTitle );
	pParam->Push( 0 );
	pParam->Push( enMailSenderType_Player );
	pParam->Push( enMailType_Text );
	pParam->Push( szContent );
	pParam->Push( BinPair( buf, 0 * sizeof(CHAT_MAILPLUS_ITEM) ) );
	pParam->Push( 0 );
	pParam->Push( 0 );

	pParam->Push( MAIL_LIVINGDATA );		//邮件最大生存周期
	pParam->Push( 0 );						//最大发送邮件数
	pParam->Push( 0 );						//最大接受邮件数

	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );

	return g_pController->CallProc( cfs_db_cnn_mail_auction, pParam ) ? true : false;
}

bool SystemSendMail(const char *szReceiver, 
	const char *szTitle,
	const char *szContent,
	DWORD dwPostMoney,
	DWORD dwMailCost,
	DWORD dwPlusCount,
	Item_Identifier *pPlusData )
{
	if(NULL == szReceiver)
		return false;

	if(NULL == szTitle)
		return false;

	if(NULL == szContent)
		return false;

	if(dwPlusCount > 0 && NULL == pPlusData)
		return false;

	int	nContSize = strlen(szContent) + 1;
	int nTotalMailSize = sizeof(DBTASK_SENDMAILS_REQ) + nContSize + dwPlusCount * sizeof(CHAT_MAILPLUS_ITEM);

	if(nTotalMailSize > MAXSIZE_MAIL)
		return false;


	BYTE buf[MAXSIZE_MAIL];
	PCHAT_MAILPLUS_ITEM  pItemData = (PCHAT_MAILPLUS_ITEM)buf;

	for(int nPlus = 0; nPlus < dwPlusCount; ++nPlus)
	{
		int nIndex = ItemSet.Add(pPlusData[nPlus].nItemClass, 
			pPlusData[nPlus].nDetailType, 
			pPlusData[nPlus].nParticularType, 
			pPlusData[nPlus].nLevel, 
			pPlusData[nPlus].nCount
			);
		
		if (nIndex <= 0)
			return false;

		if (pPlusData[nPlus].nItemClass == item_ib)
		{
			Item[nIndex].SetIBBuyDate(UNIX_TMIE_STAMP);
		}

		if(pPlusData[nPlus].dwCreditFlag != money_type_count)
		{
			Item[nIndex].SetCreditFlag(pPlusData[nPlus].dwCreditFlag);
		}

		Item[ nIndex ].SetBind( pPlusData[nPlus].isMailBind != 0 );
		
		Item[nIndex].GetItemtransfersData(pItemData->data);
		++pItemData;
		
		//日志：邮件发送物品
		KItem& item = Item[nIndex];
		
		if (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level))
		{
			LogEventParam sendItemEvent;
			sendItemEvent.event = log_event_send_mail_item;
			sendItemEvent.param2 = item.GetGUID();
			char itemIdBuff[32] = { 0 };
			item.GetItemTemplateId(itemIdBuff, sizeof(itemIdBuff));
			itemIdBuff[sizeof(itemIdBuff) - 1] = 0;
			snprintf(sendItemEvent.param3.data, sizeof(sendItemEvent.param3.data), "%s %s", itemIdBuff, szReceiver);
			sendItemEvent.param3.data[sizeof(sendItemEvent.param3.data) - 1] = 0;
			g_pLogSystem->Log(sendItemEvent);
		}
		
		ItemSet.Remove(nIndex);
	}
	//-------------------------------------------------
	
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_Mail;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SENDMAIL );
	
	pParam->Push( CHAT_CHANNEL_NAME_SYSTEM );
	pParam->Push( szReceiver );
	pParam->Push( szTitle );
	pParam->Push( 0 );
	pParam->Push( enMailSenderType_GM );
	pParam->Push( dwPlusCount > 0 ? enMailType_Plugin : enMailType_Text );
	pParam->Push( szContent );
	pParam->Push( BinPair( buf, dwPlusCount * sizeof(CHAT_MAILPLUS_ITEM) ) );
	pParam->Push( dwPostMoney );
	pParam->Push( dwMailCost );

	pParam->Push( MAIL_LIVINGDATA );		//邮件最大生存周期
	pParam->Push( 0 );						//最大发送邮件数
	pParam->Push( 0 );						//最大接受邮件数
	
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_mail_auction, pParam ) ? true : false;

	//-------------------------------------------------
}
#endif

#ifdef _SERVER
bool SysSendMailToAll(const char *szTitle,
	const char *szContent,
	DWORD dwPostMoney,
	DWORD dwMailCost,
	DWORD dwPlusCount,
	Item_Identifier *pPlusData,
	int requireLevel)
{	
	if(NULL == szTitle)
		return false;
	
	if(NULL == szContent)
		return false;
	
	if(dwPlusCount > 0 && NULL == pPlusData)
		return false;
	
	int	nContSize = strlen(szContent) + 1;
	int nTotalMailSize = sizeof(DBTASK_SENDMAILS_REQ) + nContSize + dwPlusCount * sizeof(CHAT_MAILPLUS_ITEM);
	
	if(nTotalMailSize > MAXSIZE_MAIL)
		return false;
	
	
	BYTE buf[MAXSIZE_MAIL];
	PCHAT_MAILPLUS_ITEM  pItemData = (PCHAT_MAILPLUS_ITEM)buf;
	
	for(int nPlus = 0; nPlus < dwPlusCount; ++nPlus)
	{
		int nIndex = ItemSet.Add(pPlusData[nPlus].nItemClass, 
			pPlusData[nPlus].nDetailType, 
			pPlusData[nPlus].nParticularType, 
			pPlusData[nPlus].nLevel, 
			pPlusData[nPlus].nCount
			);
		
		if (nIndex <= 0)
			return false;

		if (pPlusData[nPlus].nItemClass == item_ib)
		{
			Item[nIndex].SetIBBuyDate(UNIX_TMIE_STAMP);
		}

		if(pPlusData[nPlus].dwCreditFlag != money_type_count)
		{
			Item[nIndex].SetCreditFlag(pPlusData[nPlus].dwCreditFlag);
		}
		
		Item[nIndex].GetItemtransfersData(pItemData->data);
		++pItemData;

		ItemSet.Remove(nIndex);
	}
	//-------------------------------------------------
	
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = -1;
	DBHeader.ProcType = Proc_Mail;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SENDMAILTOALL );
	
	pParam->Push( CHAT_CHANNEL_NAME_SYSTEM );
	pParam->Push( szTitle );
	pParam->Push( 0 );
	pParam->Push( enMailSenderType_GM );
	pParam->Push( dwPlusCount > 0 ? enMailType_Plugin : enMailType_Text );
	pParam->Push( szContent );
	pParam->Push( BinPair( buf, dwPlusCount * sizeof(CHAT_MAILPLUS_ITEM) ) );
	pParam->Push( dwPostMoney );
	pParam->Push( dwMailCost );

	pParam->Push( MAIL_LIVINGDATA );		//邮件最大生存周期
	pParam->Push( 0 );						//最大发送邮件数
	pParam->Push( 0 );						//最大接受邮件数
	pParam->Push( requireLevel );			//需求等级
	
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	return g_pController->CallProc( cfs_db_cnn_mail_auction, pParam ) ? true : false;
	
	//-------------------------------------------------
}
#endif

#ifndef _SERVER
KTabFile g_StringResourseTabFile;

char* g_GetStringRes(int nStringID, char * szString, int nMaxLen)
{
	char szStringId[10];
	sprintf(szStringId, "%d", nStringID);
	g_StringResourseTabFile.GetString(szStringId, "STRING", "", szString, nMaxLen);
	return szString;
}
#endif

#ifdef _SERVER
int PrepareShowBannerBuff(char* pBuff,
						  int buffSize,
						  const char* pMsg,
						  int msgSize,
						  const char* pFont,
						  int fontSize,
						  int color,
						  int param1,
						  int param2,
						  int type)
{
	if (pBuff == NULL)
		return 0;

	if (buffSize < COMMON_SHOW_BANNER_BUFF_LENGTH)
		return 0;

	memset(pBuff, 0, buffSize);

	PSHOW_BANNER pShowBanner = (PSHOW_BANNER)pBuff;
	pShowBanner->Protocol = s2c_show_banner;
	pShowBanner->Colour = color;
	pShowBanner->speed = param1;
	pShowBanner->second = param2;
	pShowBanner->bannerType = type;

	char* pData = pShowBanner->data;

	int copyMsgSize = MAX_SHOW_BANNER_MSG_LENGTH < msgSize ? MAX_SHOW_BANNER_MSG_LENGTH : msgSize;
	if (pMsg && copyMsgSize > 0)
	{
		int msgLength = strlen(strncpy(pData, pMsg, copyMsgSize));
		pData += msgLength + 1;
	}
	else
	{
		pData++;
	}

	int copyFontSize = MAX_SHOW_BANNER_FONT_LENGTH < fontSize ? MAX_SHOW_BANNER_FONT_LENGTH : fontSize;
	if (pFont && copyFontSize > 0)
	{
		int fontLength = strlen(strncpy(pData, pFont, copyFontSize));
		pData += fontLength + 1;
	}
	else
	{
		pData++;
	}

	int usedBuffSize = pData - pBuff;
	pShowBanner->Length = usedBuffSize - 1;
	return usedBuffSize;
}
#endif

#define SQL_KEY_WORD_ARRAY_LENGTH 45
static const char* SqlKeyWordArray[SQL_KEY_WORD_ARRAY_LENGTH] =
{
	",",
	";",
	"'",
	"\"",
	"(",
	")",
	"[",
	"]",
	"<",
	">",
	"=",
	" and ",
	" or ",
	" not ",
	" create ",
	" database ",
	" drop ",
	" alter ",
	" optimize ",
	" delete ",
	" from ",
	" select ",
	" table ",
	" join ",
	" insert ",
	" replace ",
	" load ",
	" update ",
	" use ",
	" flush ",
	" kill ",
	" show ",
	" explain ",
	" describe ",
	" lock ",
	" unlock ",
	" set ",
	" grant ",
	" index ",
	" function ",
	" where ",
	" group ",
	" order ",
	" call ",
	" union "
};

// bool HasSqlKeyWord( const char* szStr )
// {
// 	if (NULL == szStr)
// 		return false;
// 
// 	for (int index = 0; index < SQL_KEY_WORD_ARRAY_LENGTH; index++)
// 	{
// 		if (NULL != strstr(szStr, SqlKeyWordArray[index]))
// 			return true;
// 	}
// 
// 	return false;
// }

#define MAX_SQL_KEY_WORD_CHECK_BUFF_LENGTH 128

bool HasSqlKeyWord( const char* szStr, int strBuffLen )
{
	if (NULL == szStr || strBuffLen <= 0)
		return false;

	char checkBuff[MAX_SQL_KEY_WORD_CHECK_BUFF_LENGTH];
	memset(checkBuff, 0, sizeof(checkBuff));
	int copyLen = strBuffLen < MAX_SQL_KEY_WORD_CHECK_BUFF_LENGTH ? strBuffLen : MAX_SQL_KEY_WORD_CHECK_BUFF_LENGTH;
	strncpy(checkBuff, szStr, copyLen);
	checkBuff[sizeof(checkBuff) - 1] = 0;
	strtolower(checkBuff);

	for (int index = 0; index < SQL_KEY_WORD_ARRAY_LENGTH; index++)
	{
		if (NULL != strstr(checkBuff, SqlKeyWordArray[index]))
			return true;
	}

	return false;
}

#define MAX_COMPRESS_BUFF (1024*4)

bool CompressProtocol(BYTE* protocolBuff, unsigned int protocolLength, unsigned int protocolBuffLength, unsigned int headerLength)
{
	char compressBuff[MAX_COMPRESS_BUFF];
	memset(compressBuff, 0, sizeof(compressBuff));
	
	BYTE* pCompressBuff = (BYTE*)compressBuff;
	unsigned int compressBuffLength = MAX_COMPRESS_BUFF;
	BYTE* pCompressSrc = protocolBuff + headerLength;
	unsigned int compressSrcLength = protocolLength - headerLength;	
	lzo1x_1_compress(
		pCompressSrc,
		compressSrcLength,
		pCompressBuff,
		&compressBuffLength,
		wrkmem);

	if (compressBuffLength < protocolBuffLength - headerLength)
	{
		memcpy(pCompressSrc, pCompressBuff, compressBuffLength);
		PCOMPRESSED_PROTOCOL_HEADER pProtocolHeader = (PCOMPRESSED_PROTOCOL_HEADER)protocolBuff;
		pProtocolHeader->Length = compressBuffLength + headerLength - 1;

		return true;
	}
	else
	{
		return false;
	}
}

bool DecompressProtocol(const BYTE* compressedProtocolBuff, BYTE* decompressBuff, unsigned int decompressBuffLength, unsigned int headerLength)
{
	memset(decompressBuff, 0, decompressBuffLength);
	memcpy(decompressBuff, compressedProtocolBuff, headerLength);
	PCOMPRESSED_PROTOCOL_HEADER pProtocolHeader = (PCOMPRESSED_PROTOCOL_HEADER)decompressBuff;
		
	const BYTE* pCompressedBuff = compressedProtocolBuff + headerLength;
	unsigned int compressedBuffLength = pProtocolHeader->Length + 1 - headerLength;
	BYTE* pDecompressBuff = decompressBuff + headerLength;
	unsigned int realDecompressBuffLength = decompressBuffLength - headerLength;
	lzo1x_decompress(
		pCompressedBuff,
		compressedBuffLength,
		pDecompressBuff,
		&realDecompressBuffLength,
		NULL);

	pProtocolHeader->Length = realDecompressBuffLength + headerLength - 1;

	return (realDecompressBuffLength < decompressBuffLength - headerLength);
}

#ifdef _SERVER
int GlobalAddItemToPlayer(
	int playerIndex,
	int itemGenre,
	int itemDetail,
	int itemParticular,
	int itemLevel,
	int itemCount,
	int& actualAddCount,
	ItemCountType statisticType, 
	enumLogEvent logEvent)
{
	if (!IsValidPlayer(playerIndex))
		return FALSE;

	KPlayer& player = Player[playerIndex];

	const KBASICPROP_ITEM* pItemTemplate = g_ItemGen.GetItemTemplate(itemGenre, itemDetail, itemParticular, itemLevel);
	if (NULL == pItemTemplate)
		return FALSE;
	
	int leftCount = itemCount;
	while (leftCount > 0)
	{
		int addCount = 1;
		if (pItemTemplate->nStack > 1)
			addCount = leftCount < pItemTemplate->nStack ? leftCount : pItemTemplate->nStack;
		
		int nIndex = ItemSet.Add(
			itemGenre, 
			itemDetail, 
			itemParticular, 
			itemLevel,
			addCount );
		if ( nIndex > 0 && itemGenre == item_ib )
		{
			Item[nIndex].SetIBBuyDate( UNIX_TMIE_STAMP );
		}
		
		if (nIndex <= 0)
		{
			break;
		}
		
		KItem& item = Item[nIndex];
		
		if (player.GetItemList().Add(nIndex, item_sync_type_gain) == 0)
		{
			ItemSet.Remove( nIndex );
			break;
		}
		else
		{			
			ItemTemplateId templateId;
			item.GetItemTemplateId(templateId);
			player.GetPlayerStatistic().AddItem(templateId, item.GetItemCount(), statisticType);
			
			bool needLog = (item.GetLogLevel() >= ConfigManager::Singleton().GetGlobalVariable(global_var_log_item_log_level));
			if (needLog)
			{
				LogEventParam systemAddItemEvent;
				systemAddItemEvent.event = logEvent;
				systemAddItemEvent.param1 = player.GetGUID();
				systemAddItemEvent.param2 = item.GetGUID();
				g_pLogSystem->Log(systemAddItemEvent);
			}
		}
		
		leftCount -= addCount;
	}

	actualAddCount = itemCount - leftCount;

	return TRUE;
}
#endif

#ifdef _SERVER
static const char* SystemVariableNames[system_var_count] =
{
	"QuestionEnabled",
	"QuestionPoolSize",
	"QuestionRefreshInterval",
	"QuestionCompress",
	"QuestionTemplate",
	"QuestionKeepTime",
	"QuestionTimeout",
	"QuestionForbidTime",
	"QuestionCharCountMin",
	"QuestionCharCountMax",
	"QuestionFontSizeMin",
	"QuestionFontSizeMax",
	"QuestionOverlapMin",
	"QuestionOverlapMax",
	"QuestionPlusPercent",
	"QuestionAngleMin",
	"QuestionAngleMax",
	"QuestionXTransMin",
	"QuestionXTransMax",
	"QuestionYTransMin",
	"QuestionYTransMax",
	"QuestionXScaleMin",
	"QuestionXScaleMax",
	"QuestionYScaleMin",
	"QuestionYScaleMax",
	"QuestionImageHeight",
	"QuestionNoiseScaleMin",
	"QuestionNoiseScaleMax",
	"QuestionNoiseCharMin",
	"QuestionNoiseCharMax",
	"QuestionBadAnswerClearInterval",
	"QuestionBadAnswerMaxCount",
	"QuestionLongTermBadAnswerClearInterval",
	"QuestionLongTermBadAnswerMaxCount",
	"QuestionBadAnswerStage1KeepTime",
	"QuestionBadAnswerStage2KeepTime",
};

void GetSystemVar(enumSystemVar systemVar, enumSystemVarValueType valueType)
{
	if (systemVar < 0 || systemVar >= system_var_count)
		return;

	if (valueType < 0 || valueType >= system_var_value_type_count)
		return;

	if (NULL == g_pController)
		return;
	
	_SystemVarHeader DBHeader;
	memset(&DBHeader, 0, sizeof(DBHeader));
	DBHeader.ulNetID	= -1;
	DBHeader.ProcType	= Proc_GetSystemVar;
	DBHeader.SystemVar	= systemVar;
	DBHeader.ValueType	= valueType;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	if (pParam)
	{
		switch (valueType)
		{
		case system_var_value_type_int:
			pParam->BeginPush( PN_GET_SYS_VAR_AS_INT );
			break;
		case system_var_value_type_string:
			pParam->BeginPush( PN_GET_SYS_VAR_AS_STRING );
			break;
		case system_var_value_type_blob:
			pParam->BeginPush( PN_GET_SYS_VAR_AS_BLOB );
			break;
		}
		
		pParam->Push( SystemVariableNames[systemVar] );

		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_global_npcsave, pParam );	
	}
}

void ProcessGetSystemVar(IProcRet* pRet, _SystemVarHeader* pHeader)
{
	if (NULL == pRet || NULL == pHeader)
		return;

	switch (pHeader->SystemVar)
	{
	case system_var_question_enabled:
	case system_var_question_pool_size:
	case system_var_question_refresh_interval:
	case system_var_question_compress:
	case system_var_question_template:
	case system_var_question_keep_time:
	case system_var_question_timeout:
	case system_var_question_forbid_time:
	case system_var_question_char_count_min:
	case system_var_question_char_count_max:
	case system_var_question_font_size_min:
	case system_var_question_font_size_max:
	case system_var_question_overlap_min:
	case system_var_question_overlap_max:
	case system_var_question_plus_percent:
	case system_var_question_angle_min:
	case system_var_question_angle_max:
	case system_var_question_xtrans_min:
	case system_var_question_xtrans_max:	
	case system_var_question_ytrans_min:
	case system_var_question_ytrans_max:
	case system_var_question_xscale_min:
	case system_var_question_xscale_max:
	case system_var_question_yscale_min:
	case system_var_question_yscale_max:
	case system_var_question_image_height:
	case system_var_question_noise_scale_min:
	case system_var_question_noise_scale_max:
	case system_var_question_noise_char_min:
	case system_var_question_noise_char_max:
	case system_var_question_bad_answer_clear_interval:
	case system_var_question_bad_answer_max_count:
	case system_var_question_long_term_bad_answer_clear_interval:
	case system_var_question_long_term_bad_answer_max_count:
	case system_var_question_bad_answer_stage1_keep_time:
	case system_var_question_bad_answer_stage2_keep_time:
		QuestionManager::Singleton().ProcessLoadQuestionSettings(pRet, pHeader->SystemVar);
		break;
	}
}
#endif

#ifndef _SERVER
bool ParseQuestionProtocol( BYTE* pMsg, UIQuestionData& uiQuestionData )
{
	if (NULL == pMsg)
		return false;
	
	QUESTION* pQuestion = (QUESTION*)pMsg;
	
	char realQuestion[COMMON_QUESTION_BUFF_SIZE];
	memset(realQuestion, 0, sizeof(realQuestion));
	
	int questionDataLength = pQuestion->wProtocolSize - (sizeof(QUESTION) - sizeof(pQuestion->QuestionData) - 1);
	
	if (TRUE == pQuestion->IsCompressed)
	{
		const BYTE* pCompressedBuff = (BYTE*)pQuestion->QuestionData;
		unsigned int compressedBuffLength = questionDataLength;
		BYTE* pDecompressBuff = (BYTE*)realQuestion;
		unsigned int decompressBuffLength = sizeof(realQuestion);
		lzo1x_decompress(
			pCompressedBuff,
			compressedBuffLength,
			pDecompressBuff,
			&decompressBuffLength,
			NULL);
		
		questionDataLength = decompressBuffLength;
	}
	else
	{
		memcpy(realQuestion, pQuestion->QuestionData, questionDataLength);
	}
	
	memset(&uiQuestionData, 0, sizeof(uiQuestionData));
	
	char* pQuestionData = realQuestion;
	int leftQuestionBuffSize = questionDataLength;
	
	if (leftQuestionBuffSize < sizeof(WORD))
		return false;
	WORD* pImgLength = (WORD*)(pQuestionData);
	pQuestionData += sizeof(WORD);
	leftQuestionBuffSize -= sizeof(WORD);
	
	if (leftQuestionBuffSize < *pImgLength || sizeof(uiQuestionData.ImgData) < *pImgLength)
		return false;
	memcpy(uiQuestionData.ImgData, pQuestionData, *pImgLength);
	uiQuestionData.ImgDataLength = *pImgLength;
	pQuestionData += *pImgLength;
	leftQuestionBuffSize -= *pImgLength;
	
	if (leftQuestionBuffSize < sizeof(WORD))
		return false;
	WORD* pQuestionTextLength = (WORD*)pQuestionData;
	pQuestionData += sizeof(WORD);
	leftQuestionBuffSize -= sizeof(WORD);
	
	if (leftQuestionBuffSize < *pQuestionTextLength || sizeof(uiQuestionData.TextData) < *pQuestionTextLength)
		return false;
	memcpy(uiQuestionData.TextData, pQuestionData, *pQuestionTextLength);
	uiQuestionData.TextDataLength = *pQuestionTextLength;
	pQuestionData += *pQuestionTextLength;
	leftQuestionBuffSize -= *pQuestionTextLength;
	
	uiQuestionData.Timeout = pQuestion->Timeout;
	g_GetStringRes( pQuestion->AppendDescStrId, uiQuestionData.AppendDescStrId, sizeof(uiQuestionData.AppendDescStrId) );
	
	return true;
}
#endif

#ifndef _SERVER
bool PrintReadableNumber(char* outputBuff, size_t outputBuffSize, DWORD number, int stepSize)
{
	if (outputBuff == NULL)
		return false;

	if (stepSize < 1)
		return false;

	char tempBuff[64] = { 0 };
	char tempBuff2[64] = { 0 };

	snprintf(tempBuff, sizeof(tempBuff), "%u", number);
	tempBuff[sizeof(tempBuff) - 1] = 0;

	int length = strlen(tempBuff);
	int insertCount = 0;
	if (length <= stepSize)
		insertCount = 0;
	else
		insertCount = (length - 1) / stepSize;

	char* pTempPos = tempBuff;
	char* pTemp2Pos = tempBuff2;
	for (int i = insertCount; i >= 0; i--)
	{
		int copyLength = stepSize;
		if (i == insertCount)
		{
			copyLength = length - insertCount * stepSize;
		}
		memcpy(pTemp2Pos, pTempPos, copyLength);

		pTempPos += copyLength;
		pTemp2Pos += copyLength;

		if (i > 0)
		{
			*pTemp2Pos = ',';
			pTemp2Pos++;
		}
	}

	strncpy(outputBuff, tempBuff2, outputBuffSize);
	outputBuff[outputBuffSize - 1] = 0;

	return true;
}
#endif
