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
#include "ChatCenter_S.h"
#include "DBProcedureUtil.h"
#include "CoreUtil.h"
#include "SocialUnit.h"
#include "fseye_protocol.h"
#include "SocialUnit.h"

ChatCenter_S	g_ChatCenterS;

ChatCenter_S::PCLIENTREQPROC	ChatCenter_S::m_ClientReqProc[chat_subprotocol_end];
ChatCenter_S::PDBPROCRET		ChatCenter_S::m_DBProcRet[enChat_DBProcCode_Num];

//---------------------------- Implementation of class ChatCenter_S -------------------------------------------
ChatCenter_S::ChatCenter_S()
{
	memset(&m_ClientReqProc, 0, sizeof(m_ClientReqProc));
	memset(&m_DBProcRet, 0, sizeof(m_DBProcRet));

	// 目前的聊天部分不需要动态创建房间及管理，所以注掉下面的几行，勿删!!!
	m_ClientReqProc[chat_msgtosomeonebyname] = &ChatCenter_S::ChatToSomeoneByName;
	m_ClientReqProc[chat_addobjectreq] = &ChatCenter_S::AddObject;
	m_ClientReqProc[chat_createroom] = &ChatCenter_S::CreateRoom;
	m_ClientReqProc[chat_addmembertoroom] = &ChatCenter_S::AddMemberToRoom;
	m_ClientReqProc[chat_msgtoroom] = &ChatCenter_S::ChatInRoom;
	m_ClientReqProc[chat_leaveroom] = &ChatCenter_S::LeaveRoom;
	m_ClientReqProc[chat_kickmemberfromroom] = &ChatCenter_S::KickRoomMember;
	m_ClientReqProc[chat_creategroup] = &ChatCenter_S::CreateGroup;
	m_ClientReqProc[chat_deletegroup] = &ChatCenter_S::DeleteGroup;
	m_ClientReqProc[chat_rmobject] = &ChatCenter_S::RemoveObject;
	m_ClientReqProc[chat_changegroup] = &ChatCenter_S::ChangeGroup;
	m_ClientReqProc[chat_renamegroup] = &ChatCenter_S::RenameGroup;
	m_ClientReqProc[chat_sendmail] = &ChatCenter_S::SendMailReq;
	m_ClientReqProc[chat_loadmaillist] = &ChatCenter_S::LoadMailListReq;
	m_ClientReqProc[chat_loadmail] = &ChatCenter_S::LoadMailReq;
	m_ClientReqProc[chat_delmail] = &ChatCenter_S::DelMailReq;
	m_ClientReqProc[chat_getoutitem] = &ChatCenter_S::GetOutPlusReq;
	m_ClientReqProc[chat_closemail] = &ChatCenter_S::CloseMailReq;
	m_ClientReqProc[chat_getoutmoney] = &ChatCenter_S::GetOutMoneyReq;
	m_ClientReqProc[chat_returnmail] = &ChatCenter_S::ReturnMailReq;
	m_ClientReqProc[chat_changeroomowner] = &ChatCenter_S::ChangeRoomOwner;

	m_DBProcRet[enChat_DBProcCode_CheckPlayerName - enChat_DBOpe_Begin - 1] 
		= &ChatCenter_S::OnDBCheckRoleNameRet;
	
	m_MonitorGlobalChat = false;
}

ChatCenter_S::~ChatCenter_S()
{
	if(m_ChatObjMgrCont)
	{
		delete [] m_ChatObjMgrCont;
	}

	if(m_MailMgrCont)
	{
		delete [] m_MailMgrCont;
	}
}

bool ChatCenter_S::Init()
{
	m_ChatObjMgrCont = new ChatObjectMgr_S[CHAT_MAX_PLAYER];
	m_MailMgrCont = new MailManager_S[CHAT_MAX_PLAYER];

	if(NULL == m_ChatObjMgrCont || NULL == m_MailMgrCont)
	{
		return false;
	}

	for(int i = 0; i < CHAT_MAX_PLAYER; ++i)
	{
		m_ChatObjMgrCont[i].Init();
		m_MailMgrCont[i].ClearCacheInfo();
	}

	m_NextCheckMailTime = UNIX_TMIE_STAMP;
	m_NextClearChatLogTime = UNIX_TMIE_STAMP;

	return true;
}

// Functions process package from client

void ChatCenter_S::ProcessProtocol(int nIndex, BYTE *pMsg, int nSize)
{
	if(NULL == pMsg)
		return;

	PVARLEN_PROTOCOL_HEADER	pHeader = (PVARLEN_PROTOCOL_HEADER)pMsg;

	_ASSERT(pHeader->subProtocol > chat_subprotocol_begin && pHeader->subProtocol < chat_subprotocol_end);
	
	if(pHeader->subProtocol > chat_subprotocol_begin 
		&& pHeader->subProtocol < chat_subprotocol_end
		&& m_ClientReqProc[pHeader->subProtocol]
	  )
		(this->*m_ClientReqProc[pHeader->subProtocol])(nIndex, pMsg, nSize);	
}

void ChatCenter_S::ChatToSomeoneByName(int nSenderIdx, BYTE *pMsg, int nSize)
{
	bool needCheck = true;
	if (IsValidPlayer(nSenderIdx) && Player[nSenderIdx].IsGM())
		needCheck = false;

	PCHATMSG_BY_NAME	pChatMsg = (PCHATMSG_BY_NAME)pMsg;
	
	if (nSize > sizeof(CHATMSG_BY_NAME) + MAXSIZE_CHAT_MSG - 1)
		return;

	if(pChatMsg->msgLen >= MAXSIZE_CHAT_MSG)
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_contentlenexceed);
		return;
	}

	pMsg[nSize - 1] = '\0';
	pChatMsg->name[MAXSIZE_ROLENAME-1] = '\0';

	int nReceiverIdx = g_PlayerInfoToIndex.GetIndexByName(pChatMsg->name);

	ChatObjectMgr_S *pSenderObjMgr = GetChatObjMgr(nSenderIdx);
	ChatObjectMgr_S *pReceiverObjMgr = GetChatObjMgr(nReceiverIdx);
	
	if(NULL == pSenderObjMgr || NULL == pReceiverObjMgr)
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_membernotonline);
		return;
	}
	
	if (IsValidPlayer(nReceiverIdx) && Npc[Player[nReceiverIdx].m_nIndex].GetComoflag())
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_playerbecamou);
		return;
	}

	int nRet = pSenderObjMgr->CheckChatTime();
	if (needCheck)
	{
		if(chat_err_none != nRet)
		{
			ChatErrCodeToClient(nSenderIdx, nRet);
			return;
		}
		
		// system not allow this player sending message
		if( pSenderObjMgr->IsPreventSendMsg() )
		{
			ChatErrCodeToClient(nSenderIdx, chat_err_noaccesstosend);
			return;
		}
		
		// sender prevent receiver
		// 	if( pSenderObjMgr->IsPreventRecvMyMsg( GetPlayerId(nReceiverIdx) ) )
		// 	{
		// 		return;
		// 	}
		
		// system not allow this player receiving message
		if( pReceiverObjMgr->IsPreventRecvMsg() )
		{
			return;
		}
		
		// receiver prevent sender
		if( pReceiverObjMgr->IsPreventSendMsgToMe( GetPlayerName(nSenderIdx) ) )
		{
			ChatErrCodeToClient(nSenderIdx, chat_err_recverpreventsender);
			return;
		}
	}

	if( !g_IsChatPass((const char*)pChatMsg->msg) )
	{
		return;
	}

	// send chat message to receiver
	pChatMsg->protocol.protocol = s2c_chat_family;
	pChatMsg->protocol.subProtocol = chat_msgtosomeonebyname;

	int nCommoFlag = 0;

	if (IsValidPlayer(nSenderIdx))
	{
		nCommoFlag = Npc[Player[nSenderIdx].m_nIndex].GetComoflag();
	}

	//给接收方发消息
	if ( nCommoFlag == 0)
		strncpy(pChatMsg->name, GetPlayerName(nSenderIdx), sizeof(pChatMsg->name));
	else
	{
		const char * szPrivateStateName = ConfigManager::Singleton().GetPlayerPrivateStateName( nCommoFlag );
		_ASSERT(szPrivateStateName);
		strncpy(pChatMsg->name, szPrivateStateName, sizeof(pChatMsg->name));
	}//endif

	int senderNpcIndex = Player[nSenderIdx].GetNpcIndex();
	pChatMsg->npcId = Npc[senderNpcIndex].GetId();
	pChatMsg->isRecive = 1;
	pChatMsg->camou = Npc[senderNpcIndex].GetComoflag();
	SendDataToClient(nReceiverIdx, pMsg, pChatMsg->protocol.len + PROTOCOL_SIZE);

	//xiehong add begin(2007-10-16)
	getItemIdsFromMsg(pChatMsg->msg, pChatMsg->msgLen);
	for(int i = 0; i < MAXSIZE_SYNC_ITEM_COUNT; ++i)
	{
		if(m_curIndexes[i])
		{
			KItemList::syncItemToOther(nReceiverIdx, m_curIndexes[i]);
		}
	}
	//xiehong add end

	//给发送方发消息
	strncpy(pChatMsg->name, GetPlayerName(nReceiverIdx), sizeof(pChatMsg->name));
	int reciverNpcIndex = Player[nReceiverIdx].GetNpcIndex();
	pChatMsg->npcId = Npc[reciverNpcIndex].GetId();
	pChatMsg->isRecive = 0;
	SendDataToClient(nSenderIdx, pMsg, pChatMsg->protocol.len + PROTOCOL_SIZE);

	//自动回复
	/*
	pChatMsg->protocol.len -= pChatMsg->msgLen;
	char* str = "<N=autoCallBack>";
	pChatMsg->msgLen = strlen(str);
	strncpy(pChatMsg->msg, str, pChatMsg->msgLen+1);
	pChatMsg->msg[pChatMsg->msgLen] = '\0';
	pChatMsg->protocol.len += pChatMsg->msgLen;
	
	strncpy(pChatMsg->name, GetPlayerName(nSenderIdx), sizeof(pChatMsg->name));
	pChatMsg->npcId = Npc[senderNpcIndex].GetId();
	pChatMsg->isRecive = 0;
	SendDataToClient(nReceiverIdx, pMsg, pChatMsg->protocol.len + PROTOCOL_SIZE);
	
	strncpy(pChatMsg->name, GetPlayerName(nReceiverIdx), sizeof(pChatMsg->name));
	pChatMsg->npcId = Npc[reciverNpcIndex].GetId();
	pChatMsg->isRecive = 1;
	SendDataToClient(nSenderIdx, pMsg, pChatMsg->protocol.len + PROTOCOL_SIZE);
	//*/
}

void ChatCenter_S::AddObject(int nSenderIdx, BYTE *pMsg, int nSize)
{
	if (!IsValidPlayer(nSenderIdx))
		return;

	if (Player[nSenderIdx].IsPreventAddFriend())
	{
		ChatErrCodeToClient(nSenderIdx,chat_err_prevent_add_friend);
		return;
	}

	PCHAT_ADDOBJECT_REQ	pData = (PCHAT_ADDOBJECT_REQ)pMsg;
	pData->name[MAXSIZE_ROLENAME-1] = '\0';

	int nPlayerIdx = g_PlayerInfoToIndex.GetIndexByName(pData->name);

	// 如果对方不在线，则需要先去数据库验证角色名字是否存在
	if( ((DWORD)this != pData->serverTag) && (INVALID_PLAYER_INDEX == nPlayerIdx) )
	{
		pData->name[sizeof(pData->name) - 1] = '\0';
		pData->serverTag = (DWORD)this;

		FindRoleInfoReq(pData->name,
			GetNetConnectIdx(nSenderIdx),
			enChat_DBProcCode_CheckPlayerName,
			nSize,
			pData);
	}
	else
	{
		// playerId 不是由客户端发送过来的，而是由数据库查询返回的，
		// 所以如果不经过查询的话，这里要填上
		//if(INVALID_PLAYER_INDEX != nPlayerIdx)
		//	pData->playerId = GetPlayerId(nPlayerIdx);

		ChatObjectMgr_S *pChatObjMgr = GetChatObjMgr(nSenderIdx);

		if(pChatObjMgr)
		{
			int nErrCode = pChatObjMgr->AddObject(nSenderIdx, pData);

			if(chat_err_none != nErrCode)
			{
				ChatErrCodeToClient(nSenderIdx, nErrCode);
			}
		}
	}
}

void ChatCenter_S::CreateRoom(int nSenderIdx, BYTE *pMsg, int nSize)
{
	ChatObjectMgr_S *pChatObjMgr = GetChatObjMgr(nSenderIdx);

	if(NULL == pChatObjMgr)
	{
		return;
	}

	int nRet = pChatObjMgr->CanCreateRoom();

	if(chat_err_none != nRet)
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
		return;
	}

	DWORD	dwRoomId;
	PCHAT_CREATEROOM_REQ pCreateRoomReq = (PCHAT_CREATEROOM_REQ)pMsg;
	pCreateRoomReq->roomName[MAXSIZE_CHATROOMNAME-1] = '\0';

	nRet = m_ChatRoomMgr.CreateRoom(GetPlayerName(nSenderIdx), pCreateRoomReq->roomName, dwRoomId);

	if(chat_err_none == nRet)
	{
		ChatRoom_S *pRoom = m_ChatRoomMgr.GetChatRoom(dwRoomId);
		
		if(NULL != pRoom)
			pRoom->AddMember(nSenderIdx);

		pChatObjMgr->JoinRoom(dwRoomId);

		CHAT_CREATEROOM_RST	 data;
		data.protocol.protocol = s2c_chat_family;
		data.protocol.subProtocol = chat_createroom;
		data.roomId = dwRoomId;
		strncpy(data.roomName, pCreateRoomReq->roomName, sizeof(data.roomName));
		data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
		SendDataToClient(nSenderIdx, &data, data.protocol.len + PROTOCOL_SIZE);
	}
	else
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
	}
}

void ChatCenter_S::AddMemberToRoom(int nSenderIdx, BYTE *pMsg, int nSize)
{
	PCHATROOM_ADD_MEMBER pData = (PCHATROOM_ADD_MEMBER)pMsg;
	pData->name[MAXSIZE_ROLENAME-1] = '\0';

	int nMemberIdx = g_PlayerInfoToIndex.GetIndexByName(pData->name);
	ChatObjectMgr_S *pObjMgr = GetChatObjMgr(nMemberIdx);

	int nRet = chat_err_none;

	if(pObjMgr)
	{
		nRet = pObjMgr->CanBeAddToRoom(pData->name);
	}
	else
	{
		nRet = chat_err_membernotonline;
	}
	
	if(chat_err_none != nRet)
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
		return;
	}

	nRet = m_ChatRoomMgr.AddMemberToRoom(nSenderIdx, pData->roomId, nMemberIdx);

	if(chat_err_none != nRet)
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
		return;
	}

	pObjMgr->JoinRoom(pData->roomId);
}

void ChatCenter_S::ForbidChatInRoom(int nSenderIdx, BYTE *pMsg, int nSize)
{
	PCHATROOM_CHATPRIVOPE_REQ pc2sReq = (PCHATROOM_CHATPRIVOPE_REQ)pMsg;
	pc2sReq->name[sizeof(pc2sReq->name) - 1] = '\0';
	int nTargetIdx = g_PlayerInfoToIndex.GetIndexByName(pc2sReq->name);
	ChatObjectMgr_S *pTargetObjMgr = GetChatObjMgr(nTargetIdx);

	if(NULL == pTargetObjMgr)
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_membernotonline);
		return;
	}

	if( pTargetObjMgr->ForbidChatInRoom(pc2sReq->roomId, pc2sReq->bForbid) )
	{
		CHATROOM_CHATPRIVOPE_RET s2cRet;
		s2cRet.protocol.protocol = s2c_chat_family;
		s2cRet.protocol.subProtocol = chat_chatroomprivope;
		s2cRet.protocol.len = sizeof(s2cRet) - PROTOCOL_SIZE;
		s2cRet.roomId = pc2sReq->roomId;
		s2cRet.bForbid = pc2sReq->bForbid;

		s2cRet.bIsOperator = true;
		SendDataToClient(nSenderIdx, &s2cRet, s2cRet.protocol.len + PROTOCOL_SIZE);

		s2cRet.bIsOperator = false;
		SendDataToClient(nTargetIdx, &s2cRet, s2cRet.protocol.len + PROTOCOL_SIZE);
	}
	else
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_opefailed);
	}
}

void ChatCenter_S::ChangeRoomOwner(int nSenderIdx, BYTE *pMsg, int nSize)
{
	PCHATROOM_CHANGE_OWNER pc2sReq = (PCHATROOM_CHANGE_OWNER)pMsg;
	pc2sReq->newOwnerName[sizeof(pc2sReq->newOwnerName) - 1] = '\0';

	int nNewOwnerIdx = g_PlayerInfoToIndex.GetIndexByName(pc2sReq->newOwnerName);

	if(INVALID_PLAYER_INDEX == nNewOwnerIdx)
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_membernotonline);
		return;
	}

	ChatObjectMgr_S *pObjMgr = GetChatObjMgr(nNewOwnerIdx);

	if(NULL == pObjMgr)
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_membernotonline);
		return;
	}
	
	if( !pObjMgr->IsJoinRoom(pc2sReq->roomId) )
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_membernotinroom);
		return;
	}
	
	int nRet = m_ChatRoomMgr.ChangeRoomOwner(nSenderIdx, pc2sReq->roomId, nNewOwnerIdx);

	if(chat_err_none != nRet)
		ChatErrCodeToClient(nSenderIdx, nRet);
}

void ChatInRoomCallBack(const void *pCallBackParam, unsigned int uPassby)
{
	CHAT_CALLBACK_PARAM		*pParam = (CHAT_CALLBACK_PARAM*)pCallBackParam;
	PCHATROOMMSG_TO_SOMEONE pChatRoom = (PCHATROOMMSG_TO_SOMEONE)pParam->pData;

	// Normal chat need check access
	ChatCenter_S *pCenter = (ChatCenter_S*)pParam->pChatCenter;
	ChatObjectMgr_S *pSenderObjMgr = pCenter->GetChatObjMgr(pParam->nSenderIdx);
	ChatObjectMgr_S *pReceiverObjMgr = pCenter->GetChatObjMgr(uPassby);
	
	if(NULL == pSenderObjMgr || NULL == pReceiverObjMgr)
	{
		return;
	}

	if( pReceiverObjMgr->IsPreventRecvMsg() )
	{
		return;
	}

	if( pReceiverObjMgr->IsPreventSendMsgToMe( GetPlayerName(pParam->nSenderIdx) ) )
	{
		return;
	}

	if( pSenderObjMgr->IsPreventRecvMyMsg( GetPlayerName(uPassby) ) )
	{
		return;
	}

	if (MAP_ROOM_ID == pChatRoom->roomId)
	{
		if ( Npc[Player[pParam->nSenderIdx].GetNpcIndex()].GetSubWorldIndex() !=
			 Npc[Player[uPassby].GetNpcIndex()].GetSubWorldIndex() )
			return;
	}

	SendDataToClient(uPassby, pParam->pData, pParam->nDataLen);
}

//xiehong add begin(2007-10-16)
void SyncItemCallBack(const void *pCallBackParam, unsigned int uPassby)
{
	int* itemIndex = (int*)pCallBackParam;
	for(int i = 0; i < MAXSIZE_SYNC_ITEM_COUNT; ++i)
	{
		if(itemIndex[i])
		{
			KItemList::syncItemToOther(uPassby, itemIndex[i]);
		}
	}
}

//用来从一段文本中按指定的格式获取物品的index、第一、二个参数是消息和长度
void ChatCenter_S::getItemIdsFromMsg(const char* msg, int msgLen)
{
	memset(m_curIndexes, 0, MAXSIZE_SYNC_ITEM_COUNT * sizeof(int));

	static char aLink[MAXSIZE_CHAT_MSG];

	int len = msgLen < MAXSIZE_CHAT_MSG ? msgLen : MAXSIZE_CHAT_MSG;

	bool findItem = false;
	int startIndex = 0;
	int idCounts = 0;
	for(int i = 1; i < len; ++i)
	{
		if(msg[i - 1] == '<' && msg[i] == 'I')
		{
			findItem = true;
			startIndex = i;
		}
		if(findItem && msg[i] == '>')
		{
			findItem = false;
			int index = 0;
			for(int j = startIndex; j < i; ++j)
			{
				aLink[index++] = msg[j];
			}
			aLink[index] = 0;
			int itemId = 0;
			sscanf(aLink, "I=%*d|%*d|%d|%*s", &itemId);
			if(!itemId)
			{
				continue;
			}
			int itemIndex = ItemSet.SearchID(itemId);
			
			//默认物品不需要同步
			if(Item[itemIndex].isDefaultItem() 
				&& TalismanManager::Singleton().IsValidTalisman(itemIndex) == false 
				&& TalismanManager::Singleton().IsValidEnchaseItem(itemIndex) == false)
			{
				continue;
			}
			
			//看是否已经记录了该物品
			bool haveRecord = false;
			for(int k = 0; k < idCounts; ++k)
			{
				if(m_curIndexes[k] == itemIndex)
				{
					haveRecord = true;
					break;
				}
			}
			//记录一个需要同步的新物品
			if(!haveRecord)
			{
				m_curIndexes[idCounts++] = itemIndex;
				if(idCounts >= MAXSIZE_SYNC_ITEM_COUNT)
				{
					break;
				}
			}
		}
	}
}
//xiehong add end

void ChatInRoomByIDCallBack(const void *pCallBackParam, unsigned int uPassby)
{
	CHAT_CALLBACK_PARAM		*pParam = (CHAT_CALLBACK_PARAM*)pCallBackParam;
	PCHATROOMMSG_TO_SOMEONE pChatRoom = (PCHATROOMMSG_TO_SOMEONE)pParam->pData;

	if (MAP_ROOM_ID == pChatRoom->roomId)
	{
		if (!Npc[pParam->nSenderIdx].IsValid() ||
			!Npc[Player[uPassby].GetNpcIndex()].IsValid())
			return;

		if ( Npc[pParam->nSenderIdx].GetSubWorldIndex() != 
			 Npc[Player[uPassby].GetNpcIndex()].GetSubWorldIndex() )
			return;
	}

	SendDataToClient(uPassby, pParam->pData, pParam->nDataLen);
}

bool SameCombatOrg(unsigned int uPlayerIdx, unsigned int uCombatOrg)
{
	int nNpcIdx = Player[uPlayerIdx].m_nIndex;
	return Npc[nNpcIdx].m_WorldCombatOrg == uCombatOrg;
}

#define ROOM_MSG_MAX_SIZE 2048
void ChatCenter_S::ChatInRoom(int nSenderIdx, BYTE *pMsg, int nSize)
{
	bool needCheck = true;
	if (IsValidPlayer(nSenderIdx) && Player[nSenderIdx].IsGM())
		needCheck = false;

	PCHATMSG_TO_ROOM	pRoomMsg = (PCHATMSG_TO_ROOM)pMsg;
	ChatObjectMgr_S *pObjMgr = GetChatObjMgr(nSenderIdx);

	if (nSize > sizeof(CHATMSG_TO_ROOM) + MAXSIZE_CHAT_MSG - 1 || NULL == pObjMgr)
		return;

	// 保证后面的字符串最后一个字节是\0
	pMsg[nSize - 1] = '\0';

#ifndef GM_CMD
	if( !g_IsChatPass((const char*)pRoomMsg->msg) )
		return;
#endif

	pRoomMsg->gm = IsValidPlayer(nSenderIdx) && Player[nSenderIdx].IsGM() == true ? 1 : 0;

	if(SYSTEM_ROOM_ID == pRoomMsg->roomId)
	{
#ifdef GM_CMD
		char GMCmd[MAXSIZE_CHAT_MSG];
		sscanf((const char *)pRoomMsg->msg, "<N=%[^>|^\n]", GMCmd);
		TextGMFilter(nSenderIdx,(const char *) GMCmd, strlen(GMCmd));
#endif
		// 系统频道禁止发送消息
		return;
	}

	int nRet = pObjMgr->CheckChatTime();
	if (needCheck)
	{
		if(chat_err_none != nRet)
		{
			ChatErrCodeToClient(nSenderIdx, nRet);
			return;
		}
		else
		{
			nRet = pObjMgr->CanChatInRoom(pRoomMsg->roomId);
			if (chat_err_none != nRet)
			{
				ChatErrCodeToClient(nSenderIdx, nRet);
				return;
			}
		}
	}

	if(pRoomMsg->msgLen > MAXSIZE_CHAT_MSG)
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_contentlenexceed);
		return;
	}

	//记录频道聊天日志
	if (g_pController && TRUE == ConfigManager::Singleton().GetGlobalVariable(global_var_log_chat))
	{
		//目前只记录世界频道聊天
		if (GLOBAL_ROOM_ID == pRoomMsg->roomId)
		{
			_DBProcHeader DBHeader = {0};
			DBHeader.ulNetID = -1;
			DBHeader.ProcType = Proc_LogChat;
			
			const char* senderName = Player[nSenderIdx].m_PlayerName;
			const char* senderAccount = Player[nSenderIdx].m_AccoutName;
			const int type = pRoomMsg->roomId;
			const char* message = (char*)pRoomMsg->msg;
			
			IProcParam* pParam = g_pController->GetProcParam( );
			if (pParam && IsValidPlayer(nSenderIdx))
			{
				pParam->BeginPush( PN_LOGCHAT );
				pParam->Push( type );
				pParam->Push( senderName );
				pParam->Push( senderAccount );
				pParam->Push( "" );
				pParam->Push( message );
				pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
				
				g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
			}
		}
	}

	char	buf[ROOM_MSG_MAX_SIZE];
	_ASSERT( ROOM_MSG_MAX_SIZE >= sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG );
	if (ROOM_MSG_MAX_SIZE < sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG )
		return;

	PCHATROOMMSG_TO_SOMEONE	pData = (PCHATROOMMSG_TO_SOMEONE)buf;
	pData->protocol.protocol = s2c_chat_family;
	pData->protocol.subProtocol = chat_msgtoroom;
	pData->gm = Player[nSenderIdx].IsGM();
	
	int nCommoFlag = 0;
	
	if (IsValidPlayer(nSenderIdx))
	{
		nCommoFlag = Npc[Player[nSenderIdx].m_nIndex].GetComoflag();
	}//endif

	if (nCommoFlag == 0)
		strncpy(pData->senderName, GetPlayerName(nSenderIdx), sizeof(pData->senderName));
	else
	{
		const char * szPrivateStateName = ConfigManager::Singleton().GetPlayerPrivateStateName( nCommoFlag );
		_ASSERT(szPrivateStateName);
		strncpy(pData->senderName, szPrivateStateName, sizeof(pData->senderName));
	}//endif

	pData->senderPlayerId = Npc[Player[nSenderIdx].GetNpcIndex()].GetId();
	pData->roomId = pRoomMsg->roomId;
	pData->msgLen = pRoomMsg->msgLen;
	pData->stringID = 0;
	pData->camou = Npc[Player[nSenderIdx].m_nIndex].GetComoflag();
	memcpy(pData->msg, pRoomMsg->msg, pRoomMsg->msgLen);
	
	pData->protocol.len = sizeof(CHATROOMMSG_TO_SOMEONE) + pRoomMsg->msgLen - 1 - PROTOCOL_SIZE;

	getItemIdsFromMsg((char*)pRoomMsg->msg, pRoomMsg->msgLen);

	//Compression begin ..........................................................................
	/*unsigned char             szBuff[ROOM_MSG_MAX_SIZE];
	unsigned int              nLen = ROOM_MSG_MAX_SIZE;

	unsigned char *       szSrc    = (unsigned char *)((VARLEN_PROTOCOL_HEADER *)pData + 1);
	unsigned int          nSrcSize =  sizeof(CHATROOMMSG_TO_SOMEONE) - 1 + pRoomMsg->msgLen  - sizeof(VARLEN_PROTOCOL_HEADER);
	
	lzo1x_1_compress( 
		szSrc,
		nSrcSize,
		szBuff,
		&nLen,
		wrkmem);
	
	if (nLen >= ROOM_MSG_MAX_SIZE - sizeof(VARLEN_PROTOCOL_HEADER) )
		return ;
	
	memcpy(szSrc,szBuff,nLen);          
	pData->protocol.len = nLen + sizeof(VARLEN_PROTOCOL_HEADER) - PROTOCOL_SIZE;
	*/
	//Compression end   ..........................................................................

	CHAT_CALLBACK_PARAM	param;

	param.pChatCenter = this;
	param.pData = (BYTE*)buf;
	param.nSenderIdx = nSenderIdx;
	param.nDataLen = pData->protocol.len + PROTOCOL_SIZE;
		
	if(LOCAL_ROOM_ID == pRoomMsg->roomId)
	{
		//附近频道
		ForEachLocalPlayer(Player[nSenderIdx].m_nIndex, ChatInRoomCallBack, &param);
		ForEachLocalPlayer(Player[nSenderIdx].m_nIndex, SyncItemCallBack, m_curIndexes);
	}
	else if(MAP_ROOM_ID == pRoomMsg->roomId)
	{		
		DWORD org = Npc[Player[nSenderIdx].m_nIndex].m_WorldCombatOrg;
		if (Npc[Player[nSenderIdx].m_nIndex].IsInWorldCombatInstance())
		{
			//战场频道
			g_PlayerInfoToIndex.ForEach(ChatInRoomCallBack, &param, SameCombatOrg, org);
			g_PlayerInfoToIndex.ForEach(SyncItemCallBack, m_curIndexes, SameCombatOrg, org);
		}
		else
		{
			//地图频道
			g_PlayerInfoToIndex.ForEach(ChatInRoomCallBack, &param);
			g_PlayerInfoToIndex.ForEach(SyncItemCallBack, m_curIndexes);
		}
	}
	else if(GLOBAL_ROOM_ID == pRoomMsg->roomId)
	{
		//世界频道
		if (m_MonitorGlobalChat)
		{
			ReportMessage(pData->senderName, "global", (const char*)pData->msg, strlen((const char*)pData->msg));
		}

		g_PlayerInfoToIndex.ForEach(ChatInRoomCallBack, &param);
		g_PlayerInfoToIndex.ForEach(SyncItemCallBack, m_curIndexes);
	}
	else if(TEAM_ROOM_ID == pRoomMsg->roomId)
	{
		int nTeamId = GetTeamId(nSenderIdx);

		if(INVALID_TEAM_ID != nTeamId)
		{
			g_TeamS[nTeamId].ForEach(ChatInRoomCallBack, &param);
			g_TeamS[nTeamId].ForEach(SyncItemCallBack, m_curIndexes);
		}
	}
	else
	{
		// check social relation
		if (IsUnitOwner(nSenderIdx,enSUTplId_Tong,enSULayer_League))
		{
			pData->unitRank = enSULayer_League;
		}else if ( IsUnitOwner(nSenderIdx, enSUTplId_Tong, enSULayer_Tong) )
		{
			pData->unitRank = enSULayer_Tong;
		}
		else if ( IsUnitOwner(nSenderIdx, enSUTplId_Tong, enSULayer_Gens) )
		{
			pData->unitRank = enSULayer_Gens;
		}
		else
		{
			pData->unitRank = enSULayer_Player;
		}

		ChatRoom_S	*pRoom = m_ChatRoomMgr.GetChatRoom(pRoomMsg->roomId);

		if(pRoom)
		{
			pRoom->ForEach(ChatInRoomCallBack, &param);
			pRoom->ForEach(SyncItemCallBack, m_curIndexes);
		}
	}
}

int	ChatCenter_S::FillSysMsgStruct(char *pBuf, 
	int nMsgType, 
	const BYTE *pMsg, 
	int nMsgLen,
	DWORD dwShowRoom,
	BYTE showType
	)
{
	CHAT_SYSTEM_MSG	*pChatMsg = (CHAT_SYSTEM_MSG*)pBuf;
	pChatMsg->protocol.protocol = s2c_chat_family;
	pChatMsg->protocol.subProtocol = chat_systemmsg;
	pChatMsg->msgType = (BYTE)nMsgType;
	pChatMsg->showRoom = dwShowRoom;
	pChatMsg->showType = showType;
	pChatMsg->msgLen = nMsgLen;
	memcpy(pChatMsg->msg, pMsg, nMsgLen);
	pChatMsg->protocol.len = sizeof(CHAT_SYSTEM_MSG) + nMsgLen - 1 - PROTOCOL_SIZE;	

	return pChatMsg->protocol.len + PROTOCOL_SIZE;
}

void ChatCenter_S::SysMsgToSomeone(int nRecverIdx, 
	int nMsgType, 
	const BYTE *pMsg, 
	int nMsgLen,
	DWORD dwShowRoom,
	BYTE showType
	)
{
	_ASSERT(nMsgLen <= MAXSIZE_CHAT_MSG);

	if(nMsgLen <= MAXSIZE_CHAT_MSG)
	{
		char	buf[sizeof(CHAT_SYSTEM_MSG) + MAXSIZE_CHAT_MSG];
		int		nSize = FillSysMsgStruct(buf, nMsgType, pMsg, nMsgLen, dwShowRoom, showType);		

		SendDataToClient(nRecverIdx, buf, nSize);
	}
}

void SysMsgCallBack(const void *pCallbackParam, unsigned int uPassby)
{
	SYSMSG_CALLBACK_PARAM *pParam = (SYSMSG_CALLBACK_PARAM*)pCallbackParam;

	SendDataToClient(uPassby, pParam->pData, pParam->nDataLen);
}

bool SameFaction(unsigned int uPlayerIdx, unsigned int uFaction)
{
	int nNpcIdx = Player[uPlayerIdx].m_nIndex;

	return Npc[nNpcIdx].m_Series == uFaction;
}

bool SameWorld(unsigned int uPlayerIdx, unsigned int nWorldIndex)
{
	int nNpcIdx = Player[uPlayerIdx].m_nIndex;
	if (!IsValidNpc(nNpcIdx))
		return false;

	if (nWorldIndex == Npc[nNpcIdx].GetSubWorldIndex())
		return true;
	else
		return false;
}

void ChatCenter_S::SysMsgToMap(int nSubworldIndex, int nMsgType, const BYTE * pMsg, int nMsgLen, DWORD dwShowRoom /* = SYSTEM_ROOM_ID */, BYTE showType /* = MSG_SHOWTYPE_ROOM  */)
{
	_ASSERT(nMsgLen <= MAXSIZE_CHAT_MSG);
	
	if(nMsgLen <= MAXSIZE_CHAT_MSG)
	{
		char	buf[sizeof(CHAT_SYSTEM_MSG) + MAXSIZE_CHAT_MSG];
		int		nSize = FillSysMsgStruct(buf, nMsgType, pMsg, nMsgLen, dwShowRoom, showType);
		
		SYSMSG_CALLBACK_PARAM	param;
		param.pData             = buf;
		param.nDataLen          = nSize;
		
		g_PlayerInfoToIndex.ForEach(SysMsgCallBack, &param,SameWorld,nSubworldIndex);
	}//endif

}

void ChatCenter_S::SysMsgToTeam(int nTeamId, 
	int nMsgType, 
	const BYTE *pMsg, 
	int nMsgLen,
	DWORD dwShowRoom,
	BYTE showType
	)
{
	_ASSERT(nMsgLen <= MAXSIZE_CHAT_MSG);

	if(nMsgLen <= MAXSIZE_CHAT_MSG)
	{
		char	buf[sizeof(CHAT_SYSTEM_MSG) + MAXSIZE_CHAT_MSG];
		int		nSize = FillSysMsgStruct(buf, nMsgType, pMsg, nMsgLen, dwShowRoom, showType);

		SYSMSG_CALLBACK_PARAM	param;
		param.pData = buf;
		param.nDataLen = nSize;

		g_TeamS[nTeamId].ForEach(SysMsgCallBack, &param);
	}
}

void ChatCenter_S::SysMsgToTong(SocialUnit *pUnit, 
	int nMsgType, 
	const BYTE *pMsg, 
	int nMsgLen,
	DWORD dwShowRoom,
	BYTE showType
	)
{
	if(NULL != pUnit)
	{
		DWORD dwChatRoomId = GetChatRoomId( pUnit->GetUnitAttr() );
		ChatRoom_S *pRoom = m_ChatRoomMgr.GetChatRoom(dwChatRoomId);

		if(NULL != pRoom)
		{
			if(nMsgLen <= MAXSIZE_CHAT_MSG)
			{
				char buf[sizeof(CHAT_SYSTEM_MSG) + MAXSIZE_CHAT_MSG];
				int nSize = FillSysMsgStruct(buf, nMsgType, pMsg, nMsgLen, dwShowRoom, showType);

				SYSMSG_CALLBACK_PARAM param;
				param.pData = buf;
				param.nDataLen = nSize;

				pRoom->ForEach(SysMsgCallBack, &param);
			}
			else
				_ASSERT(false);
		}
	}
}

void ChatCenter_S::SysMsgToFaction(int nFaction, 
	int nMsgType, 
	const BYTE *pMsg, 
	int nMsgLen,
	DWORD dwShowRoom,
	BYTE showType
	)
{
	_ASSERT(nMsgLen <= MAXSIZE_CHAT_MSG);
	
	if(nMsgLen <= MAXSIZE_CHAT_MSG)
	{
		char	buf[sizeof(CHAT_SYSTEM_MSG) + MAXSIZE_CHAT_MSG];
		int		nSize = FillSysMsgStruct(buf, nMsgType, pMsg, nMsgLen, dwShowRoom, showType);

		SYSMSG_CALLBACK_PARAM	param;
		param.pData = buf;
		param.nDataLen = nSize;

		g_PlayerInfoToIndex.ForEach(SysMsgCallBack, &param, SameFaction, nFaction);
	}
}

void ChatCenter_S::SysMsgToAll(int nMsgType, 
	const BYTE *pMsg, 
	int nMsgLen,
	DWORD dwShowRoom,
	BYTE showType
	)
{
	_ASSERT(nMsgLen <= MAXSIZE_CHAT_MSG);

	if(nMsgLen <= MAXSIZE_CHAT_MSG)
	{
		char	buf[sizeof(CHAT_SYSTEM_MSG) + MAXSIZE_CHAT_MSG];
		int		nSize = FillSysMsgStruct(buf, nMsgType, pMsg, nMsgLen, dwShowRoom, showType);

		SYSMSG_CALLBACK_PARAM	param;
		param.pData = buf;
		param.nDataLen = nSize;

		if (m_MonitorGlobalChat)
		{
			ReportMessage("system", "global", (const char*)pMsg, nMsgLen);
		}

		g_PlayerInfoToIndex.ForEach(SysMsgCallBack, &param);
	}
}

void ChatCenter_S::SysNpcMsg(int nNpcIdx, int nMsgType, const BYTE *pMsg, int nMsgLen)
{
	_ASSERT(nMsgLen <= MAXSIZE_CHAT_MSG);

	if(nMsgLen <= MAXSIZE_CHAT_MSG)
	{
		char	buf[sizeof(CHAT_SYSTEM_NPCMSG) + MAXSIZE_CHAT_MSG];
		
		CHAT_SYSTEM_NPCMSG	*pData = (CHAT_SYSTEM_NPCMSG*)buf;
		pData->protocol.protocol = s2c_chat_family;
		pData->protocol.subProtocol = chat_systemnpcmsg;
		pData->npcId = Npc[nNpcIdx].m_dwID;
		pData->msgType = (BYTE)nMsgType;
		memcpy(pData->msg, pMsg, nMsgLen);
		pData->protocol.len = sizeof(CHAT_SYSTEM_NPCMSG) + nMsgLen - 1 - PROTOCOL_SIZE;	

		Npc[nNpcIdx].SendDataToNearRegion(buf, pData->protocol.len + PROTOCOL_SIZE);
	}
}

void ChatCenter_S::LeaveRoom(int nSenderIdx, BYTE *pMsg, int nSize)
{
	PCHAT_LEAVE_ROOM	pLeaveRoom = (PCHAT_LEAVE_ROOM)pMsg;
	ChatObjectMgr_S		*pObjMgr = GetChatObjMgr(nSenderIdx);

	if(pObjMgr)
	{
		pObjMgr->LeaveRoom(pLeaveRoom->roomId);
	}

	m_ChatRoomMgr.LeaveRoom(nSenderIdx, pLeaveRoom->roomId);
}

void ChatCenter_S::KickRoomMember(int nSenderIdx, BYTE *pMsg, int nSize)
{
	PCHAT_ROOM_KICIMEMBER	pData = (PCHAT_ROOM_KICIMEMBER)pMsg;
	pData->name[MAXSIZE_ROLENAME-1] = '\0';
	
	int nMemberIdx = g_PlayerInfoToIndex.GetIndexByName(pData->name);

	if(INVALID_PLAYER_INDEX != nMemberIdx)
	{
		int nRet = m_ChatRoomMgr.KickRoomMember(nSenderIdx, pData->roomId, nMemberIdx);

		if(chat_err_none != nRet)
		{
			ChatErrCodeToClient(nSenderIdx, nRet);
		}

		ChatObjectMgr_S *pObjMgr = GetChatObjMgr(nMemberIdx);

		if(NULL != pObjMgr)
			pObjMgr->LeaveRoom(pData->roomId);
	}
}

void ChatCenter_S::CreateGroup(int nSenderIdx, BYTE *pMsg, int nSize)
{
	int	 nRet = chat_err_none;

	ChatObjectMgr_S		*pObjMgr = GetChatObjMgr(nSenderIdx);

	if(NULL != pObjMgr)
	{
		PCHAT_CREATE_GROUP	pData = (PCHAT_CREATE_GROUP)pMsg;
		pData->name[MAXSIZE_GROUPNAME-1] = '\0';

		nRet = pObjMgr->CreateGroup(nSenderIdx, pData->name);

		if(chat_err_none != nRet)
		{
			ChatErrCodeToClient(nSenderIdx, nRet);
		}
	}
}

void ChatCenter_S::ChangeRelation(int nSenderIdx, BYTE *pMsg, int nSize)
{
	int nRet = chat_err_none;

	ChatObjectMgr_S		*pObjMgr = GetChatObjMgr(nSenderIdx);

	if(pObjMgr)
	{
		PCHAT_CHANGERELATION_REQ	pData = (PCHAT_CHANGERELATION_REQ)pMsg;
		nRet = pObjMgr->ChangeRelation(nSenderIdx, pData->name, pData->relation);
	}

	if(chat_err_none != nRet)
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
	}
}

void ChatCenter_S::DeleteGroup(int nSenderIdx, BYTE *pMsg, int nSize)
{
	int nRet = chat_err_none;

	ChatObjectMgr_S	*pObjMgr = GetChatObjMgr(nSenderIdx);

	if(pObjMgr)
	{
		PCHAT_DELETE_GROUP	pData = (PCHAT_DELETE_GROUP)pMsg;

		if( ChatUtil::IsInnerGroup(pData->dwGroupId) )
			nRet = chat_err_delgroupfailed;
		else
			nRet = pObjMgr->DeleteGroup(nSenderIdx, pData->dwGroupId);	
	}

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
	}
}

void ChatCenter_S::RemoveObject(int nSenderIdx, BYTE *pMsg, int nSize)
{
	int nRet = chat_err_none;

	ChatObjectMgr_S	*pObjMgr = GetChatObjMgr(nSenderIdx);

	if(pObjMgr)
	{
		PCHAT_REMOVE_OBJECT	pData = (PCHAT_REMOVE_OBJECT)pMsg;
		pData->szName[MAXSIZE_ROLENAME-1] = '\0';
		nRet = pObjMgr->RemoveObject(pData->szName);
	}

	if(chat_err_none != nRet)
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
	}
}

void ChatCenter_S::ChangeGroup(int nSenderIdx, BYTE *pMsg, int nSize)
{
	if (!IsValidPlayer(nSenderIdx))
		return ;

	if (Player[nSenderIdx].IsPreventAddFriend())
	{
		ChatErrCodeToClient(nSenderIdx,chat_err_prevent_add_friend);
		return ;
	}

	int	nRet = chat_err_none;

	ChatObjectMgr_S	*pObjMgr = GetChatObjMgr(nSenderIdx);

	if(pObjMgr)
	{
		PCHAT_CHANGE_GROUP	pData = (PCHAT_CHANGE_GROUP)pMsg;
		pData->szPlayerName[MAXSIZE_ROLENAME-1] = '\0';
		nRet = pObjMgr->ChangeGroup(pData->szPlayerName, pData->dwOldGroupId, pData->dwNewGroupId);
	}

	if(chat_err_none != nRet)
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
	}
}

void ChatCenter_S::RenameGroup(int nSenderIdx, BYTE *pMsg, int nSize)
{
	int nRet = chat_err_none;

	ChatObjectMgr_S *pObjMgr = GetChatObjMgr(nSenderIdx);

	if(pObjMgr)
	{
		PCHAT_RENAME_GROUP	pData = (PCHAT_RENAME_GROUP)pMsg;
		pData->name[MAXSIZE_GROUPNAME-1] = '\0';
			
		if( ChatUtil::IsInnerGroup(pData->dwGroupId) )
			nRet = chat_err_renamegroupfailed;
		else
			nRet = pObjMgr->RenameGroup(pData->dwGroupId, pData->name);
	}

	if(chat_err_none != nRet)
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
	}
}

void ChatCenter_S::SendMailReq(int nSenderIdx, BYTE *pMsg, int nSize)
{
	ChatObjectMgr_S	*pObjMgr = GetChatObjMgr(nSenderIdx);
	MailManager_S   *pMailMgr = GetMailMgr(nSenderIdx);

	if(NULL == pObjMgr || NULL == pMailMgr)
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_membernotonline);
		return;
	}

	if( pObjMgr->IsPreventSendMsg() )
	{
		ChatErrCodeToClient(nSenderIdx, chat_err_noaccesstosend);
		return;
	}

	int nRet = pMailMgr->ProcessProtocol(enMailDBOpe_SendMail, nSenderIdx, pMsg, nSize);

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ChatErrCodeToClient(nSenderIdx, nRet);
	}
}

//------------------------------ Assistant functions -------------------------------------------------

int ChatCenter_S::CanSendMessage(int nSenderIdx, int nReceiverIdx)
{
	// First check whether this player can send message
	ChatObjectMgr_S *pSender = GetChatObjMgr(nSenderIdx);
	ChatObjectMgr_S *pReceiver = GetChatObjMgr(nReceiverIdx);
	
	// system not allow this player sending message
	if( pSender->IsPreventSendMsg() )
	{
		return false;
	}

	// sender prevent receiver
	if( pSender->IsPreventRecvMyMsg( GetPlayerName(nReceiverIdx) ) )
	{
		return false;
	}

	// Secondly check whether the receiver is prevent receiving message by system
	// or the receiver prevent the sender's msg

	// system not allow this player receiving message
	if(pReceiver->IsPreventRecvMsg())
	{
		return false;
	}

	// receiver prevent sender
	if( pReceiver->IsPreventSendMsgToMe( GetPlayerName(nSenderIdx) ) )
	{
		return false;
	}
	
	return true;
}

void ChatCenter_S::PlayerOnLine(int nPlayerIdx)
{
	g_PlayerInfoToIndex.PlayerOnLine(nPlayerIdx);

	ChatObjectMgr_S	*pObjMgr = GetUnUsedObjMgr(nPlayerIdx);

	if(pObjMgr)
	{
		pObjMgr->PlayerOnLine(nPlayerIdx);
		s2cChannelOpe(nPlayerIdx, SYSTEM_ROOM_ID, chat_addchannel, CHAT_CHANNEL_NAME_SYSTEM);
		s2cChannelOpe(nPlayerIdx, GLOBAL_ROOM_ID, chat_addchannel, CHAT_CHANNEL_NAME_GLOBAL);
		s2cChannelOpe(nPlayerIdx, MAP_ROOM_ID, chat_addchannel, CHAT_CHANNEL_NAME_MAP);
		s2cChannelOpe(nPlayerIdx, LOCAL_ROOM_ID, chat_addchannel, CHAT_CHANNEL_NAME_LOCAL);
	}
	
	MailManager_S	*pMailMgr = GetMailMgr(nPlayerIdx);

	if(pMailMgr)
		pMailMgr->PlayerOnLine(nPlayerIdx);

}

void ChatCenter_S::PlayerOffLine(int nPlayerIdx)
{
	ChatObjectMgr_S	*pObjMgr = GetChatObjMgr(nPlayerIdx);

	if(pObjMgr)
	{
		pObjMgr->PlayerOffLine();
	}

	MailManager_S *pMailMgr = GetMailMgr(nPlayerIdx);

	if(pMailMgr)
	{
		pMailMgr->PlayerOffLine(nPlayerIdx);
	}

	// Clear operation should place at last.
	g_PlayerInfoToIndex.PlayerOffLine(nPlayerIdx);
}
/*
void ChatCenter_S::SyncPKValue(int nIndex,BYTE * pMsg)
{
	//Online
	ChatObjectMgr_S *pObjMgr = GetChatObjMgr(nIndex);
	if (NULL == pObjMgr)
		return ;
				
	//Send Enimy PkValue to Client if exist
	DBFriendsDataParser	parser((BYTE*)pMsg);
	
	PDB_FRIENDS_DATA_HEADER	pHeader = parser.GetHeader();
	if (NULL == pHeader)
		return;

	int nGroupCount                 = pHeader->GroupCount;
	
	_ASSERT(pHeader->ObjectCount <= MAX_OBJECT_COUNT && pHeader->ObjectCount >= 0);
	_ASSERT(nGroupCount <= MAX_FRIENDGROUP_COUNT && nGroupCount >= 0);
	
	if( pHeader->ObjectCount < 0 || nGroupCount < 0 ||
		pHeader->ObjectCount > MAX_OBJECT_COUNT || nGroupCount > MAX_FRIENDGROUP_COUNT)
	{
		return ;
	}//endif
	
	PDB_FRIENDSGROUP_DATA pGroupData = parser.GetFirstGroupData();
	if (NULL == pGroupData)
		return ;
	
	for(int i = 0; i < nGroupCount && i < MAX_FRIENDGROUP_COUNT && pGroupData; ++i)
	{
		if(pGroupData->MemberCount > pHeader->ObjectCount)
		{
			_ASSERT(false);
			break;
		}//endif
		
		if(pGroupData->GroupId == GROUPID_ENEMY)
		{
			CHATOBJECTNAME* member = (CHATOBJECTNAME*)pGroupData->MemberName;
			for(int j = 0; j < pGroupData->MemberCount; ++j)
			{
				pObjMgr->ObjPkValueChangeNotify(member->name);
				
				member++;
			}//end for j
			
			break;
		}//endif
		
		pGroupData = parser.GetNextGroupData(pGroupData);
	}//end for i

}
*/

void ChatCenter_S::LoadFriendsDataRet(int nIndex, BYTE *pMsg, int nSize, int nOpResult)
{
	int nRet = chat_err_none;
	
	if(nOpResult)
	{
		ChatObjectMgr_S *pObjMgr = GetChatObjMgr(nIndex);
		
		if(pObjMgr)
		{
			nRet = pObjMgr->LoadFriendsDataRet(pMsg, nSize);
		}
		else
		{
			nRet = chat_err_membernotonline;
		}

		if(nRet > chat_err_none && nRet < chat_err_end)
		{
			ChatErrCodeToClient(nIndex, nRet);
		}

	/*	else
		{
			if (nSize)
				SyncPKValue(nIndex,pMsg);
		}//end else
	*/

	}//endif

}

void ChatCenter_S::LoadMailListReq(int nPlayerIdx, BYTE *pMsg, int nSize)
{
	int nRet = chat_err_none;

	ChatObjectMgr_S *pObjMgr = GetChatObjMgr(nPlayerIdx);
	MailManager_S	*pMailMgr = GetMailMgr(nPlayerIdx);

	if(pObjMgr && pMailMgr)
	{
		if( pObjMgr->IsPreventRecvMsg() )
			nRet = chat_err_noaccesstosend;
		else
			nRet = pMailMgr->ProcessProtocol(enMailDBOpe_LoadMailList, nPlayerIdx, pMsg, nSize);
	}
	else
	{
		nRet = chat_err_membernotonline;
	}
	
	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ChatErrCodeToClient(nPlayerIdx, nRet);
	}
}

void ChatCenter_S::LoadMailReq(int nPlayerIdx, BYTE *pMsg, int nSize)
{
	int nRet = chat_err_none;

	ChatObjectMgr_S	*pObjMgr = GetChatObjMgr(nPlayerIdx);
	MailManager_S	*pMailMgr = GetMailMgr(nPlayerIdx);

	if(NULL == pObjMgr || NULL == pMailMgr)
	{
		nRet = chat_err_membernotonline;		
	}
	else
	{	
		if( pObjMgr->IsPreventRecvMsg() )
			nRet = chat_err_noaccesstorecv;
		else
			nRet = pMailMgr->ProcessProtocol(enMailDBOpe_LoadMail, nPlayerIdx, pMsg, nSize);
	}

	if(nRet > chat_err_none && nRet < chat_err_end)
	{
		ChatErrCodeToClient(nPlayerIdx, nRet);
	}
}

void ChatCenter_S::s2cChannelOpe(int nPlayerIdx, 
								 DWORD dwChannelId, 
								 int nOpeType, 
								 const char *szName /* = NULL */
								 )
{
	if(chat_addchannel != nOpeType && chat_delchannel != nOpeType)
		return;

	char	buf[sizeof(CHAT_NOTIFY_BUILDINROOMID) + MAXSIZE_CHATCHANNEL_NAME];

	PCHAT_NOTIFY_BUILDINROOMID	pData = (PCHAT_NOTIFY_BUILDINROOMID)buf;
	pData->protocol.protocol = s2c_chat_family;
	pData->protocol.subProtocol = (BYTE)nOpeType;
	pData->roomId = dwChannelId;
	
	if(NULL == szName)
	{
		pData->nameLen = 0;
	}
	else
	{
		int nLen = snprintf(pData->name, MAXSIZE_CHATCHANNEL_NAME, "%s", szName);

		if(nLen < 0 || nLen > MAXSIZE_CHATCHANNEL_NAME)
			nLen = MAXSIZE_CHATCHANNEL_NAME;

		pData->nameLen = (BYTE)nLen;
	}

	pData->protocol.len = sizeof(CHAT_NOTIFY_BUILDINROOMID) - 1 + pData->nameLen - PROTOCOL_SIZE;

	SendDataToClient(nPlayerIdx, pData, pData->protocol.len + PROTOCOL_SIZE);
}

void ChatCenter_S::ChatInRoomByStringID(int nSenderIdx,  int nRoomID, int nStringID)
{
	if(SYSTEM_ROOM_ID == nRoomID || TEAM_ROOM_ID == nRoomID )
	{
		return;
	}

	char	buf[ROOM_MSG_MAX_SIZE];
	_ASSERT( ROOM_MSG_MAX_SIZE >= sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG );
	if (ROOM_MSG_MAX_SIZE < sizeof(CHATROOMMSG_TO_SOMEONE) + MAXSIZE_CHAT_MSG )
		return;

	PCHATROOMMSG_TO_SOMEONE	pData = (PCHATROOMMSG_TO_SOMEONE)buf;
	pData->protocol.protocol = s2c_chat_family;
	pData->protocol.subProtocol = chat_msgtoroom;
	strncpy(pData->senderName, Npc[nSenderIdx].Name, sizeof(pData->senderName));
	pData->senderPlayerId = Npc[nSenderIdx].GetId();
	pData->roomId = nRoomID;
	pData->msgLen = 0;
	pData->msg[0] = 0;
	pData->gm	  = 0;
	pData->stringID = nStringID;
	pData->unitRank = enSULayer_Player;
	pData->protocol.len = sizeof(CHATROOMMSG_TO_SOMEONE) - PROTOCOL_SIZE;

	//Compression begin ..........................................................................
	/*unsigned char             szBuff[ROOM_MSG_MAX_SIZE];
	unsigned int              nLen = ROOM_MSG_MAX_SIZE;
	
	unsigned char *       szSrc    = (unsigned char *)((VARLEN_PROTOCOL_HEADER *)pData + 1);
	unsigned int          nSrcSize =  sizeof(CHATROOMMSG_TO_SOMEONE) - 1 + pData->msgLen  - sizeof(VARLEN_PROTOCOL_HEADER);
	
	lzo1x_1_compress( 
		szSrc,
		nSrcSize,
		szBuff,
		&nLen,
		wrkmem);
	
	if (nLen >= ROOM_MSG_MAX_SIZE - sizeof(VARLEN_PROTOCOL_HEADER) )
		return ;
	
	memcpy(szSrc,szBuff,nLen);          
	pData->protocol.len = nLen + sizeof(VARLEN_PROTOCOL_HEADER) - PROTOCOL_SIZE;
	*/
	//Compression end   ..........................................................................

	CHAT_CALLBACK_PARAM	param;
	param.pChatCenter = this;
	param.pData = (BYTE*)buf;
	param.nSenderIdx = nSenderIdx;
	param.nDataLen = pData->protocol.len + PROTOCOL_SIZE;
	
	if(LOCAL_ROOM_ID == nRoomID)
	{
		ForEachLocalPlayer(nSenderIdx, ChatInRoomByIDCallBack, &param);
	}
	else if(GLOBAL_ROOM_ID == nRoomID || MAP_ROOM_ID == nRoomID)
	{
		g_PlayerInfoToIndex.ForEach(ChatInRoomByIDCallBack, &param);
	}
	else
	{
		// error channel
	}
}
/*
void ChatCenter_S::ChatInRoomByString(int nSenderIdx, int nRoomID, char *pMsg, int size )
{
	BYTE	buf[sizeof(CHATMSG_TO_ROOM) + MAXSIZE_CHAT_MSG];
	PCHATMSG_TO_ROOM pRoomMsg = (PCHATMSG_TO_ROOM)buf;
	pRoomMsg->roomId = nRoomID;
	
	pMsg[MAXSIZE_CHAT_MSG-1] = '\0';
	memcpy(pRoomMsg->msg, pMsg, MAXSIZE_CHAT_MSG );
	pRoomMsg->msgLen = MAXSIZE_CHAT_MSG;

	ChatInRoom( nSenderIdx, buf, MAXSIZE_CHAT_MSG );
}//*/

void ChatCenter_S::GetOutPlusReq(int nPlayerIdx, BYTE *pMsg, int nSize)
{
	MailOpeReq(enMailDBOpe_GetOutPlus, nPlayerIdx, pMsg, nSize);	
}

void ChatCenter_S::GetOutMoneyReq(int nPlayerIdx, BYTE *pMsg, int nSize)
{
	MailOpeReq(enMailDBOpe_GetOutMoney, nPlayerIdx, pMsg, nSize);
}

void ChatCenter_S::CloseMailReq(int nPlayerIdx, BYTE *pMsg, int nSize)
{
	MailOpeReq(enMailDBOpe_CloseMail, nPlayerIdx, pMsg, nSize);
}

void ChatCenter_S::DelMailReq(int nPlayerIdx, BYTE *pMsg, int nSize)
{
	MailOpeReq(enMailDBOpe_DelMail, nPlayerIdx, pMsg, nSize);
}

void ChatCenter_S::ReturnMailReq(int nPlayerIdx, BYTE *pMsg, int nSize)
{
	MailOpeReq(enMailDBOpe_ReturnMail, nPlayerIdx, pMsg, nSize);
}

void ChatCenter_S::MailOpeReq(enMailDBOpe enOpe, int nPlayerIdx, BYTE *pMsg, int nSize)
{
	MailManager_S *pMailMgr = GetMailMgr(nPlayerIdx);

	if(NULL != pMailMgr)
	{
		int nRet = pMailMgr->ProcessProtocol(enOpe, nPlayerIdx, pMsg, nSize);

		if(nRet > chat_err_none && nRet < chat_err_end)
			ChatErrCodeToClient(nPlayerIdx, nRet);
	}	
}

void ChatCenter_S::OnDBCheckRoleNameRet(int nDBOpeRst, int nPlayerIdx, char* pPassBy, IProcRet* pRet )
{
	if(!nDBOpeRst)
	{
		ChatErrCodeToClient(nPlayerIdx, chat_err_addobjectfailed);
	}
	else
	{
		_RoleDBHeader* pDBHeader = (_RoleDBHeader*)(pPassBy);

		if( 1 == pRet->GetRowCount() )
		{
			DWORD	dwPlayerId;
			pRet->GetData( 0, 0, dwPlayerId );

			AddObject(nPlayerIdx, (BYTE*)&pDBHeader->CObj, sizeof(CHAT_ADDOBJECT_REQ));
		}
		else
			ChatErrCodeToClient(nPlayerIdx, chat_err_playernotexist);
	}
}

void ChatCenter_S::FindRoleInfoReq(
	const char *szRoleName, 
	int nNetId, 
	DWORD nDBProcCode,
	int nPassbySize, 
	void *pPassby )
{
	if(NULL == pPassby && nPassbySize <= 0)
		return;
	
	if( nPassbySize != sizeof(CHAT_ADDOBJECT_REQ) )
		return;
	
	if(NULL != szRoleName)
	{
		_RoleDBHeader DBHeader;
		memset( &DBHeader, 0, sizeof(DBHeader) );
		
		DBHeader.ulNetID = nNetId;
		DBHeader.ProcType = Proc_FindRole;
		DBHeader.nOp = nDBProcCode;
		memcpy( &DBHeader.CObj, pPassby, sizeof(DBHeader.CObj) );
		
		IProcParam* pParam = g_pController->GetProcParam( );
		
		//begin
		pParam->BeginPush( PN_FINDROLE );
		
		pParam->Push( szRoleName );
		//end push
		pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
		
		g_pController->CallProc( cfs_db_cnn_role, pParam );	
	}
}

void ChatCenter_S::ReportMessage(
		const char* sender,
		const char* receiver,
		const char* message,
		size_t messageLength)
{
	if (sender == NULL || receiver == NULL || message == NULL)
		return;

	static IEncrypt2* s_pBase64Encrypt = BlazeCreateBase64Encrypt();
	char messageBase64String[1024] = { 0 };
	int size = 0;
	MULTI_KEY key = { 0 };
	size_t maxInputSize = sizeof(messageBase64String) / 2;
	size_t inputSize = messageLength < maxInputSize ? messageLength : maxInputSize;
	s_pBase64Encrypt->Encode((void*)message, inputSize, messageBase64String, size, key);
	messageBase64String[sizeof(messageBase64String) - 1] = 0;

	l2e_info info = { 0 };
	info.Header.Protocol = l2e_header_def;
	info.Protocol = l2e_info_def;
	snprintf(info.Info, sizeof(info.Info), "content=ChatMessage\nsender=%s\nreceiver=%s\nmessage=%s", sender, receiver, messageBase64String);
	info.Info[sizeof(info.Info) - 1] = 0;
 
 	if (g_pController != NULL)
 		g_pController->PushData(protocol_type_guard, NULL, &info, sizeof(info));
}

#define CHECK_MAIL_INTERVAL_MIN 10
#define CHECK_MAIL_INTERVAL_DEFAULT 600
#define CHECK_MAIL_INTERVAL_MAX 86400
#define CLEAR_CAHT_LOG_INTERVAL_MIN 600
#define CLEAR_CAHT_LOG_INTERVAL_DEFAULT 3600
#define CLEAR_CAHT_LOG_INTERVAL_MAX 86400
#define KEEP_CAHT_LOG_INTERVAL_MIN 600
#define KEEP_CAHT_LOG_INTERVAL_DEFAULT 28800

void ChatCenter_S::Active()
{
	//定时检查过期邮件
	if (m_NextCheckMailTime < UNIX_TMIE_STAMP)
	{
		int checkMailInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_check_mail_interval);
		if (checkMailInterval < CHECK_MAIL_INTERVAL_MIN || checkMailInterval > CHECK_MAIL_INTERVAL_MAX)
		{
			checkMailInterval = CHECK_MAIL_INTERVAL_DEFAULT;
		}

		m_NextCheckMailTime = UNIX_TMIE_STAMP + checkMailInterval;

		if (g_pController)
		{
			_DBProcHeader DBHeader = {0};
			DBHeader.ulNetID = -1;
			DBHeader.ProcType = Proc_CheckMail;
			
			IProcParam* pParam = g_pController->GetProcParam( );
			if (pParam)
			{
				pParam->BeginPush( PN_CHECKMAIL );
				pParam->Push( MSG_MAIL_RETURN_TITLE );
				pParam->Push( MAIL_LIVINGDATA );
				pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
				
				g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
			}
		}
	}

	//定时清除聊天记录
	if (m_NextClearChatLogTime < UNIX_TMIE_STAMP)
	{
		int clearChatLogInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_clear_chat_log_interval);
		if (clearChatLogInterval < CLEAR_CAHT_LOG_INTERVAL_MIN || clearChatLogInterval > CLEAR_CAHT_LOG_INTERVAL_MAX)
		{
			clearChatLogInterval = CLEAR_CAHT_LOG_INTERVAL_DEFAULT;
		}

		m_NextClearChatLogTime = UNIX_TMIE_STAMP + clearChatLogInterval;

		int keepChatLogInterval = ConfigManager::Singleton().GetGlobalVariable(global_var_keep_chat_log_interval);
		if (keepChatLogInterval < KEEP_CAHT_LOG_INTERVAL_MIN)
		{
			keepChatLogInterval = KEEP_CAHT_LOG_INTERVAL_DEFAULT;
		}

		if (g_pController)
		{
			_DBProcHeader DBHeader = {0};
			DBHeader.ulNetID = -1;
			DBHeader.ProcType = Proc_ClearChatLog;
			
			IProcParam* pParam = g_pController->GetProcParam( );
			if (pParam)
			{
				pParam->BeginPush( PN_CLEAR_CHAT_LOG );
				pParam->Push( keepChatLogInterval );
				pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
				
				g_pController->CallProc( cfs_db_cnn_mail_auction, pParam );
			}
		}
	}
}