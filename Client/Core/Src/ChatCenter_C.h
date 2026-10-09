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

#include "MailManager_C.h"
#include "CoreRelated.h"
#include "ChatObjectMgr_C.h"
#include "ChatRoomMgr_C.h"
#include "ChatDataDef.h"

#define MAIL_RECEIVERMAXMAIL_COUNT 100

#include <list>
#include <string>

class RecentlyObjRecorder
{
	typedef std::list<std::string> NAME_LIST;
	NAME_LIST     m_RecentObjs;

public:
	 RecentlyObjRecorder(void);
	~RecentlyObjRecorder(void);

	void          Init(void);
public:
	void          NotifyAttach(const char * szName);
	int           GetRecordNum( void )const;
	int           GetNames(void * pBuff,int nSize);
};

class ChatCenter_C
{
public:
	bool	Init();

	// Functions process package from server
	void	ProcessProtocol(BYTE *pMsg);

protected:
	// Load friends data
	void	OnLoadFriendsDataNotify(BYTE *pMsg);

	// chat
	void	OnRecvMsgByName(BYTE *pMsg);
	void	RecvMsgFromRoom(BYTE *pMsg);
	void	OnRecvSystemMsg(BYTE *pMsg);
	void	OnRecvSysNpcMsg(BYTE *pMsg);

	// chat object manager
	void	OnCreateGroupNotify(BYTE *pMsg);
	void	OnAddObjectNotify(BYTE *pMsg);
	void	OnChangeRelationNotify(BYTE *pMsg);
	void	OnDeleteGroupNotify(BYTE *pMsg);
	void	OnRemoveObjectNotify(BYTE *pMsg);
	void	OnChangeGroupNotify(BYTE *pMsg);
	void	OnRenameGroupNotify(BYTE *pMsg);
	void	OnPlayerStatusNotify(BYTE *pMsg);
	void    OnPKValueChangeNotify(BYTE * pMsg);

	// chat room manager
	void	OnCreateChatRoomNotify(BYTE *pMsg);
	void	OnJoinRoomNotify(BYTE *pMsg);
	void	OnMemberLeaveRoomNotify(BYTE *pMsg);
	void	OnAddRoomMemberNotify(BYTE *pMsg);
	void	OnKickRoomMemberNotify(BYTE *pMsg);
	void	OnAddChannel(BYTE *pMsg);
	void	OnChangeRoomOwnerNotify(BYTE *pMsg);
	void	OnForbidChatInRoomNotify(BYTE *pMsg);

	// Mail
	void	OnDelMailNotify(BYTE *pMsg);
	void	OnNewMailNotify(BYTE *pMsg);
	void	GetOutPlusRet(BYTE *pMsg);
	void	GetOutMoneyRet(BYTE *pMsg);
	
	// error process
	void	ProcessError(BYTE *pMsg);
	void	ProcessItemError(BYTE *pMsg);
	// Functions request operations to server
public:
	// chat
	void	ChatToSomeoneByName(const string &strReceiver, BYTE *pMsg, int nMsgLen);	
	void	ChatInRoom(DWORD dwRoomId, BYTE *pMsg, int nMsgLen);

	// chat room management
	void	CreateChatRoomReq(const char *szRoomName);
	void	AddMemberToRoomReq(DWORD dwRoomId, const string &strName);
	void	LeaveRoom(DWORD dwRoomId);
	void	KickRoomMemberReq(DWORD dwRoomId, const string &strName);
	void	ChangeRoomOwnerReq(DWORD dwRoomId, const char *szNewOwnerName);
	void	ForbitChatInRoomReq(DWORD dwRoomId, const char *szPlayerName, bool bForbitChat);

	const char * GetRoomOwnerName(DWORD dwRoomId);

	// friends management
	void	AddObjectReq(const string &strName, 
		    int nGroupId = CHAT::GROUPID_NONE, 
		    int nRelation = CHAT::PR_FRIEND);
	void	ChangeRelationReq(const string &strName, int nRelation);
	void	CreateGroupReq(const string &strGroupName);
	void	DeleteGroupReq(DWORD dwGroupId);
	void	RemoveObjectReq(const string &strName);
	void	ChangeGroupReq(const string &strName, DWORD dwOldGroupId, DWORD dwNewGroupId);
	void	RenameGroupReq(DWORD dwGroupId, const string &strNewName);
    bool    IsObjectInGroup(const string & strName , int nGroupId);
	bool    IsObjectOnline(const string & strName);
	// mail
	void	SendMail(const MAIL_PARAM *pMailParam);
	void	LoadMailListReq(int nPage = 0);
	void	OnLoadMailListNotify(BYTE *pMsg);
	void	LoadMailReq(DWORD dwMailId);
	void	OnLoadMailNotify(BYTE *pMsg);
	void	GetOutItemReq(DWORD dwMailId, int nIndex);
	void	GetOutMoneyReq(DWORD dwMailId);
	void	CloseMailReq(DWORD dwMailId);
	void	DelMailReq(DWORD dwMailId);
	void	ReturnMailReq(DWORD dwMailId);
	int		GetNewMailCount();
	int		GetSendTextMailCost();
	int		GetSendItemMailCost();

	// Functions provide data for client
	int     GetRecentlyObjNum(void);
	int     GetRecentlyObjs(void * pBuff,const int nSize);
public:
	int		GetGroupIdAndNames(DWORD *pGroupIds, char *pGroupNames);
	int		GetGroupMemberInfo(DWORD dwGroupId, char *pOutBuffer);
	void    GetPkInfo(void);
	DWORD	GetObjGroupId(const char *szPlayerName);

private:
	bool	CheckChatTime();

private:
	
	ChatObjectMgr_C		m_ChatObjMgr;
	ChatRoomMgr_C		m_ChatRoomMgr;
	MailManager_C		m_MailManager;

	RecentlyObjRecorder m_RencentObjMgr;

	DWORD				m_preChatTime;
};

extern ChatCenter_C	g_ChatCenterC;

inline DWORD ChatCenter_C::GetObjGroupId(const char *szPlayerName)
{
	return m_ChatObjMgr.GetObjGroupId(szPlayerName);	
}

inline int ChatCenter_C::GetGroupIdAndNames(DWORD *pGroupIds, char *pGroupNames)
{
	if(NULL == pGroupIds || NULL == pGroupNames)
	{
		return 0;
	}

	return m_ChatObjMgr.GetGroupIdAndNames(pGroupIds, pGroupNames);	
}

inline int ChatCenter_C::GetGroupMemberInfo(DWORD dwGroupId, char *pOutBuffer)
{
	if(NULL == pOutBuffer)
	{
		return 0;
	}

	return m_ChatObjMgr.GetGroupMemberInfo(dwGroupId, pOutBuffer);
}

inline int ChatCenter_C::GetNewMailCount()
{
	return m_MailManager.GetNewMailCount();
}

inline int ChatCenter_C::GetSendTextMailCost()
{
	return m_MailManager.GetSendTextTax();
}

inline int ChatCenter_C::GetSendItemMailCost()
{
	return m_MailManager.GetSendItemTax();
}

#endif
