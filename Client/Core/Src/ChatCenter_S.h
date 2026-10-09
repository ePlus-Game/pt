//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-9 18:37
//      File_base        : ChatCenter
//      File_ext         : h
//      Author           : chenshanglin
//      Description      : Note: Class end with "_S" means this class used in server.
//								 Class end with "_C" means this class used in client.
//
//      <Change_list>
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//////////////////////////////////////////////////////////////////////
#ifndef _ChatCenter_h
#define _ChatCenter_h

#include "CoreRelated.h"
#include "MailManager_S.h"
#include "ChatObjectMgr_S.h"
#include "ChatRoomMgr_S.h"

class SocialUnit;

//------------------------------ Declaration of class ChatCenter_S ---------------------------------

class ChatCenter_S
{
public:
	ChatCenter_S();
	~ChatCenter_S();

	bool	Init();
	void	ProcessProtocol(int nIndex, BYTE *pMsg, int nSize);
	void	PlayerOnLine(int nPlayerIdx);
	void	PlayerOffLine(int nPlayerIdx);
	ChatObjectMgr_S*	GetChatObjMgr(int nPlayerIdx);
	MailManager_S*		GetMailMgr(int nPlayerIdx);
	ChatRoomMgr_S&		GetRoomMgr();

	void	ProcessDBOpeRet(
				int nDbOpeRst, 
				int nPlayerIdx, 
				IProcRet* pRet );

	// system message to someone or room
	void	SysMsgToSomeone(int nRecverIdx, 
		int nMsgType, 
		const BYTE *pMsg, 
		int nMsgLen,
		DWORD dwShowRoom = SYSTEM_ROOM_ID,
		BYTE showType = MSG_SHOWTYPE_ROOM
		);

	void    SysMsgToMap( int nSubworldIndex,
		int          nMsgType,
		const BYTE * pMsg,
		int          nMsgLen,
		DWORD        dwShowRoom = SYSTEM_ROOM_ID,
		BYTE         showType   = MSG_SHOWTYPE_ROOM
		);

	void	SysMsgToTeam(int nTeamId, 
		int nMsgType, 
		const BYTE *pMsg, 
		int nMsgLen,
		DWORD dwShowRoom = SYSTEM_ROOM_ID,
		BYTE showType = MSG_SHOWTYPE_ROOM
		);

	void	SysMsgToTong(SocialUnit *pUnit, 
		int nMsgType, 
		const BYTE *pMsg, 
		int nMsgLen,
		DWORD dwShowRoom = SYSTEM_ROOM_ID,
		BYTE showType = MSG_SHOWTYPE_ROOM
		);

	void	SysMsgToFaction(int nFaction, 
		int nMsgType, 
		const BYTE *pMsg, 
		int nMsgLen,
		DWORD dwShowRoom = SYSTEM_ROOM_ID,
		BYTE showType = MSG_SHOWTYPE_ROOM
		);

	void	SysMsgToAll(int nMsgType, 
		const BYTE *pMsg, 
		int nMsgLen,
		DWORD dwShowRoom = SYSTEM_ROOM_ID,
		BYTE showType = MSG_SHOWTYPE_ROOM
		);

	void	SysNpcMsg(int nNpcIdx, int nMsgType, const BYTE *pMsg, int nMsgLen);

	void	s2cChannelOpe(int nPlayerIdx, DWORD dwChannelId, int nOpeType, const char *szName = NULL);
	void	ChatInRoomByStringID(int nSenderIdx, int nRoomID, int nStringID);
//	void	ChatInRoomByString(int nSenderIdx, int nRoomID, char *pMsg, int nSize);

	void	MonitorGlobalChat(bool on);
	void	Active();

	// Functions process data from db
public:
	void	LoadFriendsDataRet(int nIndex, BYTE *pMsg, int nSize, int nOpResult);
	void	SaveFriendsDataReq(int nPlayerIdx);
	
protected:
	void	LoadFriendsDataReq(int nPlayerIdx);
	
	// Functions process package from client

	// chat
	void	ChatToSomeoneByName(int nSenderIdx, BYTE *pMsg, int nSize);
	void	ChatInRoom(int nSenderIdx, BYTE *pMsg, int nSize);
	//xiehong add begin(2007-10-16)
	void	getItemIdsFromMsg(const char* msg, int msgLen);//返回聊天消息里带的物品链接id
	//xiehong add end
	int		FillSysMsgStruct(char *pBuf, 
		int nMsgType, 
		const BYTE *pMsg, 
		int nMsgLen,
		DWORD dwShowRoom,
		BYTE showType
		);

	// friend object manager
	void	AddObject(int nSenderIdx, BYTE *pMsg, int nSize);
	void	ChangeRelation(int nSenderIdx, BYTE *pMsg, int nSize);
	void	CreateGroup(int nSenderIdx, BYTE *pMsg, int nSize);
	void	DeleteGroup(int nSenderIdx, BYTE *pMsg, int nSize);
	void	RemoveObject(int nSenderIdx, BYTE *pMsg, int nSize);
	void	ChangeGroup(int nSenderIdx, BYTE *pMsg, int nSize);
	void	RenameGroup(int nSenderIdx, BYTE *pMsg, int nSize);

	// chat room manager
	void	CreateRoom(int nSenderIdx, BYTE *pMsg, int nSize);
	void	AddMemberToRoom(int nSenderIdx, BYTE *pMsg, int nSize);
	void	LeaveRoom(int nSenderIdx, BYTE *pMsg, int nSize);
	void	KickRoomMember(int nSenderIdx, BYTE *pMsg, int nSize);
	void	ChangeRoomOwner(int nSenderIdx, BYTE *pMsg, int nSize);
	void	ForbidChatInRoom(int nSenderIdx, BYTE *pMsg, int nSize);
	
	// mail manager
	void	SendMailReq(int nSenderIdx, BYTE *pMsg, int nSize);
	void	LoadMailListReq(int nPlayerIdx, BYTE *pMsg, int nSize);
	void	LoadMailReq(int nPlayerIdx, BYTE *pMsg, int nSize);
	void	GetOutPlusReq(int nPlayerIdx, BYTE *pMsg, int nSize);
	void	GetOutMoneyReq(int nPlayerIdx, BYTE *pMsg, int nSize);
	void	CloseMailReq(int nPlayerIdx, BYTE *pMsg, int nSize);
	void	DelMailReq(int nPlayerIdx, BYTE *pMsg, int nSize);
	void	ReturnMailReq(int nPlayerIdx, BYTE *pMsg, int nSize);

	// process DB procedure return
	void	OnDBCheckRoleNameRet(int nDBOpeRst, int nPlayerIdx, char* pPassBy, IProcRet* pRet );
	
	//	Assistant functions
private:
	int		CanSendMessage(int nSenderIdx, int nReceiverIdx);
	ChatObjectMgr_S* GetUnUsedObjMgr(int nPlayerIdx);
//	void    SyncPKValue(int nIndex,BYTE * pMsg);
	void	MailOpeReq(enMailDBOpe enOpe, int nPlayerIdx, BYTE *pMsg, int nSize);

	void	FindRoleInfoReq(
		const char *szRoleName, 
		int nNetId, 
		DWORD nDBProcCode, 
		int nPassbySize, 
		void *pPassby
		);

	void	ReportMessage(
		const char* sender,
		const char* receiver,
		const char* message,
		size_t messageLength
		);

private:
	typedef void (ChatCenter_S::*PCLIENTREQPROC)(int nPlayerIdx, BYTE *pMsg, int nSize);
	typedef void (ChatCenter_S::*PDBPROCRET)(int nDBOpeRst, int nPlayerIdx, char* pPassBy, IProcRet* pRet);

	ChatObjectMgr_S		*m_ChatObjMgrCont;
	ChatRoomMgr_S		m_ChatRoomMgr;
	MailManager_S		*m_MailMgrCont;

	bool	m_MonitorGlobalChat;

	static PCLIENTREQPROC	m_ClientReqProc[chat_subprotocol_end];
	static PDBPROCRET		m_DBProcRet[enChat_DBProcCode_Num];

	//xiehong add begin(2007-10-16)
	int						m_curIndexes[MAXSIZE_SYNC_ITEM_COUNT];		//用来记录本次收到的话语中包含的物品链接的index
	//xiehong add end

	DWORD m_NextCheckMailTime;
	DWORD m_NextClearChatLogTime;
};

inline ChatRoomMgr_S& ChatCenter_S::GetRoomMgr()
{
	return	m_ChatRoomMgr;
}

inline ChatObjectMgr_S* ChatCenter_S::GetChatObjMgr(int nPlayerIdx)
{
	if(nPlayerIdx > 0 && nPlayerIdx < CHAT_MAX_PLAYER)
	{
		if( m_ChatObjMgrCont[nPlayerIdx].IsInUse() )
		{
			return &m_ChatObjMgrCont[nPlayerIdx];
		}
	}

	return NULL;
}

inline ChatObjectMgr_S* ChatCenter_S::GetUnUsedObjMgr(int nPlayerIdx)
{
	if(nPlayerIdx > 0 && nPlayerIdx < CHAT_MAX_PLAYER)
	{
		if( !m_ChatObjMgrCont[nPlayerIdx].IsInUse() )
		{
			return &m_ChatObjMgrCont[nPlayerIdx];
		}
	}

	return NULL;
}

inline MailManager_S* ChatCenter_S::GetMailMgr(int nPlayerIdx)
{
	return (nPlayerIdx > 0 && nPlayerIdx < CHAT_MAX_PLAYER) ? &m_MailMgrCont[nPlayerIdx] : NULL;
}

inline void ChatCenter_S::LoadFriendsDataReq(int nPlayerIdx)
{
	ChatObjectMgr_S	*pObjMgr = GetChatObjMgr(nPlayerIdx);

	if(pObjMgr)
	{
		pObjMgr->LoadFriendsDataReq();
	}
	else
	{
		ChatErrCodeToClient(nPlayerIdx, chat_err_membernotonline);
	}
}

inline void ChatCenter_S::SaveFriendsDataReq(int nPlayerIdx)
{
	ChatObjectMgr_S	*pObjMgr = GetChatObjMgr(nPlayerIdx);

	if(pObjMgr)
	{
		pObjMgr->SaveFriendsDataReq();
	}
}

struct _RoleDBHeader : _DBProcHeader
{
	int nOp;
	CHAT_ADDOBJECT_REQ CObj;
};

inline void	ChatCenter_S::ProcessDBOpeRet(
	int nDbOpeRst, 
	int nPlayerIdx, 
	IProcRet* pRet )
{
	int nSize = 0;
	char* pPassBy = pRet->GetPassBy( nSize );
	
	if( pPassBy == NULL || 
		nSize == 0 || 
		nSize != sizeof(_RoleDBHeader) )
		return;
	
	_RoleDBHeader* pHeader = (_RoleDBHeader*)pPassBy;

	int uDBOpeType = pHeader->nOp;

	if(NULL != m_DBProcRet[uDBOpeType - enChat_DBOpe_Begin - 1])
		(this->*m_DBProcRet[uDBOpeType - enChat_DBOpe_Begin - 1])(nDbOpeRst,
			nPlayerIdx,
			pPassBy,
			pRet );
}

inline void ChatCenter_S::MonitorGlobalChat(bool on)
{
	m_MonitorGlobalChat = on;
}

extern ChatCenter_S	g_ChatCenterS;

#endif
