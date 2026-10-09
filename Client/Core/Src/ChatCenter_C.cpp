//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-12 9:27
//      File_base        : ChatCenter
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
#include "CoreUtil.h"
#include "ChatCenter_C.h"
#include <algorithm>

#include "screeneffect_man.h"

//RecnetlyObjRecorder ................................................................................................

RecentlyObjRecorder::RecentlyObjRecorder()
{}

RecentlyObjRecorder::~RecentlyObjRecorder()
{}

void RecentlyObjRecorder::Init()
{
	m_RecentObjs.clear();
}

void RecentlyObjRecorder::NotifyAttach(const char * szName)
{
	if (szName == NULL)
		return ;

	if (std::find(m_RecentObjs.begin(),m_RecentObjs.end(),szName)!= m_RecentObjs.end())
		return ;

	if (m_RecentObjs.size() >= MAX_RECENT_OBJ_RECORD_NUM)
	{
		m_RecentObjs.pop_front();
	}//endif

	m_RecentObjs.push_back(szName);
}

int RecentlyObjRecorder::GetRecordNum()const
{
	return (int)m_RecentObjs.size();
}

int RecentlyObjRecorder::GetNames(void * pBuff,int nSize)
{
	UI_RECENT_OBJ_NAME * pName = ( UI_RECENT_OBJ_NAME * ) pBuff;
	int                  nNum  = 0;
	NAME_LIST::iterator  iter  = m_RecentObjs.begin();
	int                  nLeft = nSize;
	
	while (iter != m_RecentObjs.end() && nLeft >= sizeof (UI_RECENT_OBJ_NAME))
	{
		strcpy(pName->szName,(*iter).c_str());

		nLeft -= sizeof(UI_RECENT_OBJ_NAME);

		++ pName;	
		++ nNum;
		++ iter; 
	}//end for while

	return nNum;
}

//End RecentlyObjRecorder .............................................................................................

bool ChatCenter_C::Init()
{
	m_ChatObjMgr.Init();
	m_ChatRoomMgr.Init();
	m_MailManager.Init();
	m_RencentObjMgr.Init();
	
	m_preChatTime = 0;

	return true;
}

//--------------- Functions process package from server -----------------------------------------------------

void ChatCenter_C::ProcessProtocol(BYTE *pMsg)
{
	if(NULL == pMsg)
	{
		return;
	}

	PVARLEN_PROTOCOL_HEADER	pHeader = (PVARLEN_PROTOCOL_HEADER)pMsg;

	if(pHeader->subProtocol <= chat_subprotocol_begin || pHeader->subProtocol >= chat_subprotocol_end)
	{
		return;
	}

	switch(pHeader->subProtocol)
	{
	case chat_msgtosomeonebyname:
		OnRecvMsgByName(pMsg);
		break;

	case chat_notifyobjectid:
		OnAddObjectNotify(pMsg);
		break;

	case chat_createroom:
		OnCreateChatRoomNotify(pMsg);
		break;
		
	case chat_addmembertoroom:
		OnAddRoomMemberNotify(pMsg);
		break;

	case chat_errcode:
		ProcessError(pMsg);
		break;

	case chat_itemerrcode:
		ProcessItemError(pMsg);
		break;

	case chat_joinroom:
		OnJoinRoomNotify(pMsg);
		break;

	case chat_msgtoroom:
		RecvMsgFromRoom(pMsg);
		break;

	case chat_leaveroom:
		OnMemberLeaveRoomNotify(pMsg);
		break;

	case chat_memberkicknotify:
		OnKickRoomMemberNotify(pMsg);
		break;

	case chat_creategroup:
		OnCreateGroupNotify(pMsg);
		break;

	case chat_loadfriendsdata:
		OnLoadFriendsDataNotify(pMsg);
		break;

	case chat_loadmaillist:
		OnLoadMailListNotify(pMsg);
		break;

	case chat_loadmail:
		OnLoadMailNotify(pMsg);
		break;

	case chat_addchannel:
	case chat_delchannel:
		OnAddChannel(pMsg);
		break;

	case chat_changerelation:
		OnChangeRelationNotify(pMsg);
		break;

	case chat_deletegroup:
		OnDeleteGroupNotify(pMsg);
		break;

	case chat_rmobject:
		OnRemoveObjectNotify(pMsg);
		break;

	case chat_changegroup:
		OnChangeGroupNotify(pMsg);
		break;

	case chat_renamegroup:
		OnRenameGroupNotify(pMsg);
		break;

	case chat_delmail:
		OnDelMailNotify(pMsg);
		break;

	case chat_systemmsg:
		OnRecvSystemMsg(pMsg);
		break;

	case chat_systemnpcmsg:
		OnRecvSysNpcMsg(pMsg);
		break;

	case chat_newmailnotify:
		OnNewMailNotify(pMsg);
		break;

	case chat_getoutitem:
		GetOutPlusRet(pMsg);
		break;

	case chat_getoutmoney:
		GetOutMoneyRet(pMsg);
		break;

	case chat_onlinestatusnotify:
		OnPlayerStatusNotify(pMsg);
		break;

	case chat_changeroomowner:
		OnChangeRoomOwnerNotify(pMsg);
		break;

	case chat_chatroomprivope:
		OnForbidChatInRoomNotify(pMsg);
		break;

	case chat_pkvaluechange:
		OnPKValueChangeNotify(pMsg);
		break;
		
	default:
		break;
	}
}

int  ChatCenter_C::GetRecentlyObjNum()
{
	return m_RencentObjMgr.GetRecordNum();
}

int  ChatCenter_C::GetRecentlyObjs(void * pBuff,const int nSize)
{
	return m_RencentObjMgr.GetNames(pBuff,nSize);
}

#define MAX_STRING_PARAM_NUM 5

void ChatCenter_C::OnRecvSystemMsg(BYTE *pMsg)
{
	CHAT_SYSTEM_MSG	*pData = (CHAT_SYSTEM_MSG*)pMsg;

	// Convert pData to PCHATROOMMSG_TO_SOMEONE due to don't
	// want to change existing interface.
	char	buf[sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG];
	PCHATROOMMSG_TO_SOMEONE	pRoomMsg = (PCHATROOMMSG_TO_SOMEONE)buf;

	pRoomMsg->gm = 1;

	pRoomMsg->senderPlayerId = INVALID_PLAYER_INDEX;

	strncpy(pRoomMsg->senderName, CHAT_SENDER_SYSTEMNAME, sizeof(pRoomMsg->senderName));
	pRoomMsg->msgLen = pData->msgLen;

	if(SYSMSG_TYPE_ID == pData->msgType)
	{
		int  nStrId                    = *((int*)pData->msg);
		char szToken[MAXSIZE_CHAT_MSG] = "";

		g_GetStringRes(nStrId, (char*)szToken, MAXSIZE_CHAT_MSG);

		szToken[MAXSIZE_CHAT_MSG - 1]  = 0;
		if (szToken[0] == 0)
			return;

		int  nStringParamSize          = pData->msgLen - sizeof(int);
		int  nStringParamNum           = 0;

		char   szDefaultStringParam[1] = "";	
		char * szStringParamBuff[MAX_STRING_PARAM_NUM];

		for (int n = 0 ; n < MAX_STRING_PARAM_NUM ; n ++ )
			szStringParamBuff[n] = szDefaultStringParam;

		if   (nStringParamSize > 0)
		{
			//Parse the string param to param buffer
			int    nParsedSize         = 0;
			char * szSrc               = pData->msg + sizeof(int);

			while (nParsedSize < nStringParamSize)
			{
				   int nParamLen  = strlen(szSrc);
				   int nParamSize = nParamLen + 1;
				  
				   if (nStringParamNum >= MAX_STRING_PARAM_NUM )
					   return;

				   szStringParamBuff[nStringParamNum] = szSrc;
				   szSrc                             += nParamSize; 
				   nParsedSize                       += nParamSize;
				   nStringParamNum                   += 1;
			}//end for while
			
		}//endif

		//Check for tocken format 
		int    nParamNeeded = 0;
		int    nFormatLen   = strlen(szToken);
		for   (int nFormatIndex = 0; nFormatIndex < nFormatLen - 1; nFormatIndex ++ )
		{
			if (szToken[nFormatIndex] == '%')
			{
				if (szToken[nFormatIndex + 1] == 's')
					nParamNeeded += 1;
				else
					return;
			}//endif

		}//end for nFormatIndex

		if ( nParamNeeded > MAX_STRING_PARAM_NUM)
			return ;

		char   szTempFormatString[(MAX_STRING_PARAM_NUM + 1) * MAXSIZE_CHAT_MSG ] = "";
		sprintf(szTempFormatString,szToken,szStringParamBuff[0],szStringParamBuff[1],szStringParamBuff[2],szStringParamBuff[3],szStringParamBuff[4]);
		
		int    nFinalLen        = strlen(szTempFormatString);
		if    (nFinalLen       >= MAXSIZE_CHAT_MSG -1)
			return;

		memcpy(pRoomMsg->msg,szTempFormatString,nFinalLen + 1);
		pRoomMsg->msg[nFinalLen] = 0;
		pRoomMsg->msgLen         = nFinalLen + 1;

	}//endif
	else if(SYSMSG_TYPE_STR == pData->msgType)
	{
		memcpy(pRoomMsg->msg, pData->msg, pData->msgLen);
		pRoomMsg->msg[pData->msgLen] = '\0';
	}//end else
	
	if(pData->showType & MSG_SHOWTYPE_ROOM)
		CoreDataChanged( GDCNI_RECV_CHAT_DATE_R2P, pData->showRoom, (int)pRoomMsg );

	if(pData->showType & MSG_SHOWTYPE_MIDDLESCREEN)
		ShowChatErrorMsg((const char*)pRoomMsg->msg);

	if(pData->showType & MSG_SHOWTYPE_TOPSCREEN)
	{
		CommonStyle	 style;
		const char * szColor = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_top_message,0);
		
		if (szColor == 0 || szColor[0] == 0)
			style.color  = 0xffffffff;
		else
			style.color  = StringToColor(szColor);

		const char * szSpeed = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_top_message,1);
		if (szSpeed == 0 || szSpeed[0] == 0)
			style.speed  = 40;
		else
		{
			style.speed  = 0;
			int   nSpeedIndex = 0;

			while (szSpeed[nSpeedIndex] && szSpeed[nSpeedIndex] >= '0' && szSpeed[nSpeedIndex] <= '9')
			{
				style.speed = style.speed * 10 + (szSpeed[nSpeedIndex] - '0');
				nSpeedIndex ++;
			}//end for while
		}

		style.speed  = 40;
		style.second = 1;

		CoreDataChanged(GDCNI_TOPMESSAGE, (unsigned int)pRoomMsg->msg, (int)&style);
	}//endif

	if (pData->showType & MSG_SHOWTYPE_SPECIAL)
	{
		CoreDataChanged(GDCNI_TOP_MESSAGE,(unsigned int)pRoomMsg->msg,0);
	}//endif
}

void ChatCenter_C::OnRecvSysNpcMsg(BYTE *pMsg)
{
	CHAT_SYSTEM_NPCMSG	*pSysMsg = (CHAT_SYSTEM_NPCMSG*)pMsg;

	// Wait for UI interface 
	// NPC名字不可操作性
	// 需要使用时必须把NPC名字放到对话中再将NPC名字至空
}

void ChatCenter_C::OnRecvMsgByName(BYTE *pMsg)
{
	// Notify client to show chat message
	PCHATMSG_BY_NAME	pData = (PCHATMSG_BY_NAME)pMsg;
	
	if (!pData)
		return;

	if (pData->isRecive)
		m_RencentObjMgr.NotifyAttach(pData->name);

	char	cFilter = '*';
	const char * szFlterString = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_textfilter_char);
	if (szFlterString[0])
	{
		cFilter = szFlterString[0];
	}//endif

	if( !g_FilterChatText((const char*)pData->msg , cFilter) )
	{
		return;
	}
	
	if( !g_FilterChatRecvText((const char*)pData->msg ,cFilter))
	{
		return;
	}

	if( !g_FilterUserChatText((const char*)pData->msg ,cFilter))
	{
		return;
	}

	if (pData->camou != 0)
	{
		CoreDataChanged( GDCNI_RECV_CHAT_DATE_P2P_TO_CHAT_WINDOW, (UINT)pData, NULL);
	}
	else
	{
		// 目前不需要管理说过话的玩家的列表，如果需要，放开下面两行, 勿删!!!
 		if( !m_ChatObjMgr.HasObject(pData->name) )
 		{
 			m_ChatObjMgr.AddTempObjectName(pData->name);
 		}

		//发送到左下角的聊天界面
		CoreDataChanged( GDCNI_RECV_CHAT_DATE_P2P_TO_CHAT_WINDOW, (UINT)pData, NULL);

		//传统私聊与二人世界分开，只有双方都是好友状态的时候才打开PrivateRoom
		int nGroupID = m_ChatObjMgr.GetObjGroupId(pData->name);
		if (nGroupID!=INVALID_GROUP_ID && (nGroupID == GROUPID_NONE || nGroupID >= GROUPID_NUM ) )
			CoreDataChanged( GDCNI_RECV_CHAT_DATE_P2P, (unsigned int)pMsg, NULL );

		CoreDataChanged( GDCNI_FRIENDLIST_NOTIFY, NULL, NULL );
	}
}

void ChatCenter_C::OnCreateChatRoomNotify(BYTE *pMsg)
{
	PCHAT_CREATEROOM_RST pRst = (PCHAT_CREATEROOM_RST)pMsg;

	m_ChatRoomMgr.OnCreateChatRoomNotify(pRst);

	// Notify client to save room id
	CoreDataChanged( GDCNI_CHAT_ROOM_CREATE, pRst->roomId, (int)pMsg );
}

void ChatCenter_C::OnAddRoomMemberNotify(BYTE *pMsg)
{
	PCHATROOM_ADD_MEMBER pData = (PCHATROOM_ADD_MEMBER)pMsg;

	m_ChatRoomMgr.OnAddRoomMemberNotify(pData->roomId, pData->name);

	// Notify client to flush room member
	CoreDataChanged( GDCNI_CHAT_ROOM_ADD, pData->roomId, (int)pMsg );
}

void ChatCenter_C::ProcessError(BYTE *pMsg)
{
	PCHAT_ERR_CODE	pData = (PCHAT_ERR_CODE)pMsg;

	if(pData->errorCode > chat_err_none && pData->errorCode < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[pData->errorCode]);
	}
}

void ChatCenter_C::ProcessItemError(BYTE *pMsg)
{
	PCHAT_ERR_CODE	pData = (PCHAT_ERR_CODE)pMsg;

	if(pData->errorCode >= item_inlay_ok_normal && pData->errorCode < item_inlay_error_count)
	{
		ShowChatErrorMsg(g_szItemErrMsg[pData->errorCode]);

		ConfigManager& cm = ConfigManager::Singleton();
		int nScreenEffect = 0;
		if ( pData->errorCode == item_inlay_ok_normal )
		{
			nScreenEffect = cm.GetGlobalVariable( global_var_item_inlay_ok_normal );
		}
		else if ( pData->errorCode == item_inlay_ok_yin )
		{
			nScreenEffect = cm.GetGlobalVariable( global_var_item_inlay_ok_yin );
		}
		else if ( pData->errorCode == item_inlay_ok_yang )
		{
			nScreenEffect = cm.GetGlobalVariable( global_var_item_inlay_ok_yang );
		}
		else
		{
			// to do nothing.
		}
		if ( nScreenEffect > 0 )
		{
			ScreenEffectMgr& sem = ScreenEffectMgr::Singleton();
			sem.Player( nScreenEffect, BeforeUi );
		}
	}
}

void ChatCenter_C::OnJoinRoomNotify(BYTE *pMsg)
{
	PCHAT_JOIN_ROOM	pJoinRoom = (PCHAT_JOIN_ROOM)pMsg;

	m_ChatRoomMgr.OnJoinRoomNotify(pJoinRoom);

	ShowChatErrorMsg(g_szChatErrMsg[chat_err_joinroomnotify]);

	// Notify client to show room
	CoreDataChanged( GDCNI_CHAT_ROOM_JOIN, pJoinRoom->roomId, (int)pMsg );
}

#define MAX_ROOM_MSG_LEN 2048

void ChatCenter_C::RecvMsgFromRoom(BYTE *pMsg)
{
	PCHATROOMMSG_TO_SOMEONE	pRoomMsg = (PCHATROOMMSG_TO_SOMEONE)pMsg;
	
	//Decompression.....................................................................................
	/*int                     nSize            = pRoomMsg->protocol.len;
	int                     nCompressionSize = nSize + PROTOCOL_SIZE - sizeof(VARLEN_PROTOCOL_HEADER);
	
	unsigned char szBuff[MAX_ROOM_MSG_LEN];
	PCHATROOMMSG_TO_SOMEONE pDecompression   = (PCHATROOMMSG_TO_SOMEONE)szBuff;
	pDecompression->protocol.len             = pRoomMsg->protocol.len;
	pDecompression->protocol.protocol        = pRoomMsg->protocol.protocol;
	pDecompression->protocol.subProtocol     = pRoomMsg->protocol.subProtocol;

	unsigned char * szDest   = (unsigned char *)((VARLEN_PROTOCOL_HEADER *)pDecompression + 1) ; 
	unsigned char * szSource = (unsigned char *)((VARLEN_PROTOCOL_HEADER *)pRoomMsg + 1) ;

	unsigned int    nLen     = MAX_ROOM_MSG_LEN - sizeof(VARLEN_PROTOCOL_HEADER);
	
	lzo1x_decompress(
		szSource,
		nCompressionSize,
		(unsigned char *)szDest,
		&nLen,
		NULL);
	
	pRoomMsg                                = pDecompression;
	*/
	//Decompression end.................................................................................

	// 目前不需要像msn聊天室那样管理成员，仅显示消息即可，如果需要成员管理
	// 放开下面这行, 勿删!!!
	// m_ChatRoomMgr.OnAddRoomMemberNotify(pRoomMsg->roomId, pRoomMsg->senderName);

	char	cFilter = '*';
	const char * szFlterString = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_textfilter_char);
	if (szFlterString[0])
	{
		cFilter = szFlterString[0];
	}//endif

	if( !g_FilterChatText((const char*)pRoomMsg->msg , cFilter) )
	{
		return;
	}
	
	if( !g_FilterChatRecvText((const char*)pRoomMsg->msg ,cFilter))
	{
		return;
	}
	
	if( !g_FilterUserChatText((const char*)pRoomMsg->msg ,cFilter))
	{
		return;
	}

	if ( pRoomMsg )
	{
		if ( pRoomMsg->stringID > 0 && pRoomMsg->stringID <= 60000  )
		{
			char	buf[MAXSIZE_ROLENAME + MAXSIZE_CHAT_MSG + sizeof(CHATROOMMSG_TO_SOMEONE)];
			PCHATROOMMSG_TO_SOMEONE	pData = (PCHATROOMMSG_TO_SOMEONE)buf;
			pData->protocol.protocol = pRoomMsg->protocol.protocol;
			pData->protocol.subProtocol = pRoomMsg->protocol.subProtocol;
			//strncpy(pData->senderName, pRoomMsg->senderName, sizeof(pData->senderName));
			//pData->senderPlayerId = pRoomMsg->senderPlayerId;
			pData->senderPlayerId = INVALID_PLAYER_INDEX;
			pData->roomId = pRoomMsg->roomId;
			
			pData->stringID = pRoomMsg->stringID;
			char pBuff[MAXSIZE_CHAT_MSG];
			g_GetStringRes( pRoomMsg->stringID, pBuff, MAXSIZE_CHAT_MSG );
			pBuff[MAXSIZE_CHAT_MSG-1] = 0;

			// NPC消息喊话内容将名字改到喊话内容里
			// 避免客户端NPC名字在聊天区域可操作
			sprintf((char*)pData->msg, "[%s]:%s", pRoomMsg->senderName, pBuff);
			ZeroMemory(pData->senderName, sizeof(pData->senderName));
			//strncpy( (char*)pData->msg, pBuff, MAXSIZE_CHAT_MSG );
			pData->msgLen = strlen( (char*)pData->msg );
			pData->protocol.len = sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_ROLENAME + MAXSIZE_CHAT_MSG - 1 - PROTOCOL_SIZE;

			CoreDataChanged( GDCNI_RECV_CHAT_DATE_R2P, pRoomMsg->roomId, (int)pData );
		}
		else
		{
			CoreDataChanged( GDCNI_RECV_CHAT_DATE_R2P, pRoomMsg->roomId, (int)pRoomMsg );
		}
	}
}

void ChatCenter_C::OnMemberLeaveRoomNotify(BYTE *pMsg)
{
	PCHAT_LEAVEROOM_NOTIFY	pNotify = (PCHAT_LEAVEROOM_NOTIFY)pMsg;

	m_ChatRoomMgr.OnMemberLeaveRoomNotify(pNotify->roomId, pNotify->name);

	CoreDataChanged( GDCNI_CHAT_ROOM_LEAVE, (unsigned int )pNotify->roomId, (int)pMsg );
}

void ChatCenter_C::OnKickRoomMemberNotify(BYTE *pMsg)
{
	PCHATROOM_KICKMEMBER_NOTIFY	pNotify = (PCHATROOM_KICKMEMBER_NOTIFY)pMsg;
	
	m_ChatRoomMgr.OnKickRoomMemberNotify(pNotify->roomId, pNotify->name);

	CoreDataChanged( GDCNI_CHAT_ROOM_KICK, (unsigned int )pNotify->roomId, (int)pMsg );
}

void ChatCenter_C::OnChangeRoomOwnerNotify(BYTE *pMsg)
{
	PCHATROOM_CHANGE_OWNER ps2cRet = (PCHATROOM_CHANGE_OWNER)pMsg;

	m_ChatRoomMgr.OnChangeRoomOwnerNotify(ps2cRet);
	CoreDataChanged( GDCNI_CHAT_ROOM_CHANGEOWNER, (unsigned int )ps2cRet->roomId, (int)pMsg );
}

void ChatCenter_C::OnForbidChatInRoomNotify(BYTE *pMsg)
{
	PCHATROOM_CHATPRIVOPE_RET ps2cRet = (PCHATROOM_CHATPRIVOPE_RET)pMsg;

	if(ps2cRet->bIsOperator)
	{
		// 房主将别人禁言或解禁

		if(ps2cRet->bForbid)
		{
			// 禁言
			// MSG_CHAT_FORBIDCHATSUCCEED
		}
		else
		{
			// 解禁
			// MSG_CHAT_UNFORBIDCHATSUCCEED
		}
	}
	else
	{
		// 你被房主禁言了

		if(ps2cRet->bForbid)
		{
			// 禁言
			// MSG_CHAT_FORBIDCHATINROOM
		}
		else
		{
			// 解禁
			// MSG_CHAT_UNFORBIDCHATINROOM
		}
	}

}

void ChatCenter_C::OnPKValueChangeNotify(BYTE * pMsg)
{
    CHAT_PKCHANGE_NOTIFY * pNotify = (CHAT_PKCHANGE_NOTIFY *)pMsg;
	m_ChatObjMgr.PkValueChangeNotify(pNotify->playerName,pNotify->pkValue);
}

void ChatCenter_C::GetPkInfo()
{
    m_ChatObjMgr.UpdatePkValue();
}

void ChatCenter_C::OnAddObjectNotify(BYTE *pMsg)
{
	PCHAT_ADDOBJECT_NOTIFY	pData = (PCHAT_ADDOBJECT_NOTIFY)pMsg;

	m_ChatObjMgr.AddObject(pData);
	CoreDataChanged( GDCNI_FRIENDLIST_NOTIFY, NULL, NULL );
}

void ChatCenter_C::OnCreateGroupNotify(BYTE *pMsg)
{
	PCHAT_CREATEGROUP_RST	pData = (PCHAT_CREATEGROUP_RST)pMsg;
	
	m_ChatObjMgr.CreateGroup(pData->dwId, pData->name);

	CoreDataChanged( GDCNI_FRIENDLIST_NOTIFY, NULL, NULL );
}

//--------------- Functions request operation to server ------------------------------------------

void ChatCenter_C::ChatToSomeoneByName(const string &strReceiver, BYTE *pMsg, int nMsgLen)
{
	if( strReceiver == GetPlayerName(CLIENT_PLAYER_INDEX) || !CheckChatTime())
		return;

	int nRet = m_ChatObjMgr.ChatToSomeoneByName(strReceiver, pMsg, nMsgLen);
	
	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
	else
	{
		m_RencentObjMgr.NotifyAttach(strReceiver.c_str());
	}//end else

}

void ChatCenter_C::AddObjectReq(const string &strObjName, 
	int nGroupId, /* = GROUPID_NONE */ 
	int nRelation /* = PR_FRIEND */)
{
	
	int nOldGroupId = m_ChatObjMgr.GetObjGroupId(strObjName.c_str());
	if (nOldGroupId == INVALID_GROUP_ID && IsObjectInGroup(strObjName,GROUPID_TEMP))
	{
        nOldGroupId = GROUPID_TEMP;
	}//endif

	if (nOldGroupId != INVALID_GROUP_ID )
	{
        ChangeGroupReq(strObjName,nOldGroupId,nGroupId);
		return ;
	}//endif
	else
	{
		
		int nRet = m_ChatObjMgr.AddObjectReq(strObjName, nGroupId, nRelation);
		if(nRet > chat_err_none && nRet < chat_err_end)
		{
			ShowChatErrorMsg(g_szChatErrMsg[nRet]);
		}//endif

	}//endelse

}

void ChatCenter_C::ChangeRelationReq(const string &strName, int nRelation)
{
	int nRet = m_ChatObjMgr.ChangeRelationReq(strName, nRelation);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::CreateChatRoomReq(const char *szRoomName)
{
	if(NULL == szRoomName)
		return;

	int nRet = m_ChatObjMgr.CanCreateRoom();

	if(chat_err_none == nRet)
	{
		nRet = m_ChatRoomMgr.CreateRoomReq(szRoomName);
	}

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::AddMemberToRoomReq(DWORD dwRoomId, const string &strName)
{
	int nRet = m_ChatRoomMgr.AddMemberToRoomReq(dwRoomId, strName);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::ChatInRoom(DWORD dwRoomId, BYTE *pMsg, int nMsgLen)
{

	bool needCheck = true;
	if (IsValidPlayer(CLIENT_PLAYER_INDEX) && Player[CLIENT_PLAYER_INDEX].IsGM())
		needCheck = false;

	// Just For Debug, Delete it when release
#ifdef CLIENT_SCRIPT

	if(SYSTEM_ROOM_ID == dwRoomId)
	{
		// Filter client gm command
		char GMCmd[MAXSIZE_CHAT_MSG];
		sscanf((const char *)pMsg, "<N=%[^>|^\n]", GMCmd);
		if( nMsgLen > 4 && !strncmp( (const char*)GMCmd, "c?gm", 4) )
		{
			if ( TextGMFilter(CLIENT_PLAYER_INDEX, GMCmd + 1, strlen(GMCmd) - 1) )
				return;
		}
	}

#endif
	//

	/*if(LOCAL_ROOM_ID == dwRoomId)
	{
		static ConfigManager &cfg = ConfigManager::Singleton();
		
		DWORD dwTimeInterval = cfg.GetGlobalVariable(global_var_localroomchat_timeinterval);
		DWORD dwPreChatTime = m_ChatObjMgr.GetPreChatTimeInLocalRoom();
		DWORD dwCurTime = time(NULL);

		if(dwCurTime - dwPreChatTime < dwTimeInterval)
		{
			ShowMsgInSysRoom(MSG_CHAT_TOOFAST);
			return;
		}
		else
		{
			m_ChatObjMgr.SetPreChatTimeInLocalRoom(dwCurTime);
		}
	}//*/

	if (needCheck && !CheckChatTime()  )
		return;

	int nRet = m_ChatObjMgr.ChatInRoom(dwRoomId, pMsg, nMsgLen);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

bool ChatCenter_C::CheckChatTime()
{
	// check time interval
	DWORD curTime = ::GetTickCount();
	if (curTime - m_preChatTime <= 1000)
	{
		ShowChatErrorMsg(g_szChatErrMsg[chat_err_chattoofast]);
		return false;
	}
	else
		m_preChatTime = curTime;

	return true;
}

void ChatCenter_C::LeaveRoom(DWORD dwRoomId)
{
	int nRet = m_ChatRoomMgr.LeaveRoom(dwRoomId);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::KickRoomMemberReq(DWORD dwRoomId, const string &strName)
{
	int nRet = m_ChatRoomMgr.KickRoomMemberReq(dwRoomId, strName);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::ChangeRoomOwnerReq(DWORD dwRoomId, const char *szNewOwnerName)
{
	int nRet = m_ChatRoomMgr.ChangeRoomOwner(dwRoomId, szNewOwnerName);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::ForbitChatInRoomReq(DWORD dwRoomId, const char *szPlayerName, bool bForbitChat)
{
	int nRet = m_ChatRoomMgr.ForbitChatInRoomReq(dwRoomId, szPlayerName, bForbitChat);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

const char * ChatCenter_C::GetRoomOwnerName(DWORD dwRoomId)
{
	return m_ChatRoomMgr.GetRoomOwnerName(dwRoomId);
}

void ChatCenter_C::CreateGroupReq(const string &strGroupName)
{
	int nRet = m_ChatObjMgr.CreateGroupReq(strGroupName);
	
	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::DeleteGroupReq(DWORD dwGroupId)
{
	int	nRet = chat_err_none;

	if( ChatUtil::IsInnerGroup(dwGroupId) )
		nRet = chat_err_delgroupfailed;
	else
		nRet = m_ChatObjMgr.DeleteGroupReq(dwGroupId);
	
	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::RemoveObjectReq(const string &strName)
{
	int nRet = m_ChatObjMgr.RemoveObjectReq(strName);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::ChangeGroupReq(const string &strName, DWORD dwOldGroupId, DWORD dwNewGroupId)
{
	int nRet = m_ChatObjMgr.ChangeGroupReq(strName, dwOldGroupId, dwNewGroupId);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

bool ChatCenter_C::IsObjectInGroup(const string & strName , int nGroupId)
{
	return m_ChatObjMgr.IsObjectInGroup(strName,nGroupId);
}

bool ChatCenter_C::IsObjectOnline(const string & strName)
{
	return m_ChatObjMgr.IsObjectOnline(strName);
}

void ChatCenter_C::RenameGroupReq(DWORD dwGroupId, const string &strNewName)
{
	int nRet = chat_err_none;

	if( ChatUtil::IsInnerGroup(dwGroupId) )
		nRet = chat_err_renamegroupfailed;
	else
		nRet = m_ChatObjMgr.RenameGroupReq(dwGroupId, strNewName);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::SendMail(const MAIL_PARAM *pMailParam)
{
	int	nRet = chat_err_none;

	if( m_ChatObjMgr.IsPreventSendMsg() )
	{
		nRet = chat_err_noaccesstosend;
	}
	else
	{
		nRet = m_MailManager.SendMail(pMailParam);
	}

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
	else
	{
		m_RencentObjMgr.NotifyAttach(pMailParam->strReceiver);
	}
}

void ChatCenter_C::LoadMailListReq(int nPage /*= 0*/)
{
	int	nRet = chat_err_none;

	if( m_ChatObjMgr.IsPreventRecvMsg() )
	{
		nRet = chat_err_noaccesstosend;
	}
	else
	{
		nRet = m_MailManager.LoadMailListReq(nPage);
	}

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::OnLoadFriendsDataNotify(BYTE *pMsg)
{
	PCHAT_LIST_FRIEND_RET	pData = (PCHAT_LIST_FRIEND_RET)pMsg;
	//Decompression.....................................................................................
	int                     nSize = pData->protocol.len;
	int                     nCompressionSize = nSize - sizeof(CHAT_LIST_FRIEND_RET) + 1 + PROTOCOL_SIZE;

	unsigned char szBuff[SAVETEMPBUFLEN];
	unsigned int  nLen = SAVETEMPBUFLEN;
	
	lzo1x_decompress(
		pData->data,
		nCompressionSize,
		(unsigned char *)szBuff,
		&nLen,
		NULL);

	//Decompression end.................................................................................

	int nRet = m_ChatObjMgr.OnLoadFriendsDataNotify(szBuff);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::LoadMailReq(DWORD dwMailId)
{
	int		nRet = chat_err_none;

	if( m_ChatObjMgr.IsPreventRecvMsg() )
	{
		nRet = chat_err_noaccesstorecv;
	}
	else
	{
		// Get mail data from local storage first.
		BYTE *pMailData = m_MailManager.GetMailData(dwMailId);

		if(NULL == pMailData)
		{
			nRet = m_MailManager.LoadMailReq(dwMailId);
		}
		else
		{
			CoreDataChanged( GDCNI_RECV_MAIL, (unsigned int)pMailData, NULL );
		}
	}

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::OnLoadMailNotify(BYTE *pMsg)
{
	PCHAT_STRUCTURED_DATABLOCK	pProtoData = (PCHAT_STRUCTURED_DATABLOCK)pMsg;

	//Decompression.......................................................
	int nOldSize = pProtoData->protocol.len - sizeof(CHAT_STRUCTURED_DATABLOCK) + 1 + PROTOCOL_SIZE;
	
	unsigned char szCompressionBuff[MAXSIZE_SEND_BUF];
	unsigned int nLen = MAXSIZE_SEND_BUF;
		lzo1x_decompress(
		pProtoData->data,
		nOldSize,
		(unsigned char *)szCompressionBuff,
		&nLen,
		NULL);
	
	//Decompression end...................................................
	
	PDBTASK_GETMAILDATA_RET		pDBMailData = (PDBTASK_GETMAILDATA_RET)szCompressionBuff;

	m_MailManager.LoadMailRet((unsigned char *)pDBMailData);

	BYTE *pMailData = m_MailManager.GetMailData(pDBMailData->unMailID);

	if(pMailData)
	{
		PCHAT_MAILDATA_CLIENT pData = (PCHAT_MAILDATA_CLIENT) pMailData;
		if (pData->header.tagMailInfo.enSenderType==enMailSenderType_Player)
			m_RencentObjMgr.NotifyAttach(pData->header.tagMailInfo.szSenderName);

		CoreDataChanged( GDCNI_RECV_MAIL, (unsigned int)pMailData, NULL );
	}
	else
	{
		ShowChatErrorMsg(g_szChatErrMsg[chat_err_mailnotfound]);
	}

	CoreDataChanged(GDCNI_OPEN_MAIL_NOTIFY, NULL, NULL);
}

void ChatCenter_C::OnAddChannel(BYTE *pMsg)
{
	PCHAT_NOTIFY_BUILDINROOMID	pData = (PCHAT_NOTIFY_BUILDINROOMID)pMsg;

	Ui_Channel_Param	uiParam;
	memset(&uiParam, 0, sizeof(uiParam));
	uiParam.dwChannelID = pData->roomId;

	int nSize = pData->nameLen >= sizeof(uiParam.szChannelName) ? 
		sizeof(uiParam.szChannelName) - 1 : pData->nameLen;

	strncpy(uiParam.szChannelName, pData->name, nSize);

	if(chat_addchannel == pData->protocol.subProtocol)
		CoreDataChanged(GDCNI_CHAT_CHANNEL_CREATE, (int)&uiParam, 0);
	else if(chat_delchannel == pData->protocol.subProtocol)
		CoreDataChanged(GDCNI_CHAT_CHANNEL_CLOSE, (int)&uiParam, 0);
}

void ChatCenter_C::GetOutItemReq(DWORD dwMailId, int nIndex)
{
	int nRet = m_MailManager.GetOutItemReq(dwMailId, nIndex);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::GetOutMoneyReq(DWORD dwMailId)
{
	int nRet = m_MailManager.GetOutMoneyReq(dwMailId);

	if(nRet > chat_err_none && nRet < chat_err_end)
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
}

void ChatCenter_C::CloseMailReq(DWORD dwMailId)
{
	int nRet = m_MailManager.CloseMailReq(dwMailId);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::DelMailReq(DWORD dwMailId)
{
	int nRet = m_MailManager.DelMailReq(dwMailId);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
	}
}

void ChatCenter_C::ReturnMailReq(DWORD dwMailId)
{
	int nRet = m_MailManager.ReturnMailReq(dwMailId);

	if(nRet > chat_err_none && nRet < chat_err_end)
		ShowChatErrorMsg(g_szChatErrMsg[nRet]);
}

void ChatCenter_C::OnChangeRelationNotify(BYTE *pMsg)
{
	PCHAT_CHANGERELATION_RET	pRet = (PCHAT_CHANGERELATION_RET)pMsg;

	m_ChatObjMgr.ChangeRelation(pRet->szName, pRet->relation);
	CoreDataChanged( GDCNI_FRIENDLIST_NOTIFY, NULL, NULL );
}

void ChatCenter_C::OnDeleteGroupNotify(BYTE *pMsg)
{
	PCHAT_DELETE_GROUP	pRet = (PCHAT_DELETE_GROUP)pMsg;
	
	m_ChatObjMgr.DeleteGroup(pRet->dwGroupId);
	CoreDataChanged( GDCNI_FRIENDLIST_NOTIFY, NULL, NULL );
}

void ChatCenter_C::OnRemoveObjectNotify(BYTE *pMsg)
{
	PCHAT_REMOVEOBJECT_RET	pRet = (PCHAT_REMOVEOBJECT_RET)pMsg;

	m_ChatObjMgr.RemoveObject(pRet->szName);	
	CoreDataChanged( GDCNI_FRIENDLIST_NOTIFY, NULL, NULL );
}

void ChatCenter_C::OnChangeGroupNotify(BYTE *pMsg)
{
	PCHAT_CHANGEGROUP_RET	pRet = (PCHAT_CHANGEGROUP_RET)pMsg;

	m_ChatObjMgr.ChangeGroup(pRet->szPlayerName, pRet->dwOldGroupId, pRet->dwNewGroupId);
	CoreDataChanged( GDCNI_FRIENDLIST_NOTIFY, NULL, NULL );
}

void ChatCenter_C::OnRenameGroupNotify(BYTE *pMsg)
{
	PCHAT_RENAME_GROUP	pRet = (PCHAT_RENAME_GROUP)pMsg;

	m_ChatObjMgr.RenameGroup(pRet->dwGroupId, pRet->name);
	CoreDataChanged( GDCNI_FRIENDLIST_NOTIFY, NULL, NULL );
}

void ChatCenter_C::OnDelMailNotify(BYTE *pMsg)
{
	PCHAT_DELMAIL_REQ	pRet = (PCHAT_DELMAIL_REQ)pMsg;
	
	m_MailManager.DelMailRet(pRet->mailId);
	CoreDataChanged( GDCNI_DEL_MAIL_RET, true, NULL );
}

void ChatCenter_C::OnLoadMailListNotify(BYTE *pMsg)
{
	PCHAT_STRUCTURED_DATABLOCK	pData = (PCHAT_STRUCTURED_DATABLOCK)pMsg;
	//Decompression.......................................................
	int nOldSize = pData->protocol.len - sizeof(CHAT_STRUCTURED_DATABLOCK) + 1 + PROTOCOL_SIZE;
	
	unsigned char szCompressionBuff[MAXSIZE_SEND_BUF];
	unsigned int nLen = MAXSIZE_SEND_BUF;
		lzo1x_decompress(
		pData->data,
		nOldSize,
		(unsigned char *)szCompressionBuff,
		&nLen,
		NULL);
	
	//Decompression end...................................................

	m_MailManager.LoadMailListRet(szCompressionBuff);	
	CoreDataChanged( GDCNI_RECV_MAIL_LIST, (unsigned int)szCompressionBuff, NULL );
}

void ChatCenter_C::OnNewMailNotify(BYTE *pMsg)
{
	PCHAT_NEWMAIL_NOTIFY	ps2cNotify = (PCHAT_NEWMAIL_NOTIFY)pMsg;

	const	int	TMP_BUF_SIZE = 512;
	char		buf[TMP_BUF_SIZE];

	if(1 == ps2cNotify->newMailCnt && ps2cNotify->senderName[0] != '\0' )
		_snprintf(buf, TMP_BUF_SIZE, MSG_ONENEWMAIL_NOTIFY, ps2cNotify->senderName);
	else
		_snprintf(buf, TMP_BUF_SIZE, MSG_MULTINEWMAIL_NOTIFY, ps2cNotify->newMailCnt);

	CoreDataChanged(GDCNI_ERROR_MESSAGE, (unsigned int)buf, 0);
	CoreDataChanged(GDCNI_NEW_MAIL_NOTIFY, 0, 0);
}

void ChatCenter_C::GetOutPlusRet(BYTE *pMsg)
{
	PCHAT_OPE_NOTIFY	ps2cNotify = (PCHAT_OPE_NOTIFY)pMsg;

	m_MailManager.GetOutPlusRet(pMsg);

	// Notify UI
	CoreDataChanged(GDCNI_NOTIFY_PLUSITEM_STATE, FALSE, NULL );
}

void ChatCenter_C::GetOutMoneyRet(BYTE *pMsg)
{
	PCHAT_OPE_NOTIFY	ps2cNotify = (PCHAT_OPE_NOTIFY)pMsg;

	m_MailManager.GetOutMoneyRet(pMsg);
	
	// Notify UI
	CoreDataChanged(GDCNI_NOTIFY_PLUSITEM_STATE, TRUE, NULL );
}

void ChatCenter_C::OnPlayerStatusNotify(BYTE *pMsg)
{
	PCHAT_ONLINESTATUS_NOTIFY ps2cNotify = (PCHAT_ONLINESTATUS_NOTIFY)pMsg;
	m_ChatObjMgr.SetOnlineStatus(ps2cNotify->playerName, ps2cNotify->bOnline,ps2cNotify->nSeries,ps2cNotify->nLevel);

	CoreDataChanged( GDCNI_FRIENDLIST_NOTIFY, NULL, NULL );

	ObjectGroup *pGroup = m_ChatObjMgr.GetGroupByPlayerName(ps2cNotify->playerName);

	if(NULL != pGroup)
	{
		const char *szGroupName = pGroup->GetGroupName();
		const char *szPlayerName = ps2cNotify->playerName;
		const static int MSG_BUF_SIZE = 64;

		if(NULL != szGroupName && NULL != szPlayerName)
		{
			char buf[MSG_BUF_SIZE];

			if(ps2cNotify->bOnline)
				snprintf(buf, MSG_BUF_SIZE, MSG_CHAT_FRIENDS_ONLINE, szGroupName, szPlayerName);
			else
				snprintf(buf, MSG_BUF_SIZE, MSG_CHAT_FRIENDS_OFFLINE, szGroupName, szPlayerName);

			ShowChatErrorMsg(buf);
		}
	}
}

//--------------------------------------------------------------------------------------------

ChatCenter_C	g_ChatCenterC;


