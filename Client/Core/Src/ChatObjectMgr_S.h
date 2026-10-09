//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-6-8 10:01
//      File_base        : ChatGroupMgr
//      File_ext         : h
//      Author           : chenshanglin
//      Description      : 
//
//      <Change_list>
//
//      {
//      Change_datetime  : 
//      Change_by        : 
//      Change_purpose   : 
//      }
//
//////////////////////////////////////////////////////////////////////
#ifndef _ChatGroupMgr_h
#define _ChatGroupMgr_h

#include <map>
#include <vector>
#include <set>
#include <string>

#include "CoreRelated.h"
#include "ChatDataDef.h"

using std::map;
using std::vector;
using std::string;
using std::set;

using namespace CHAT;

//----------------------------------------------------------------------------------------------

class ObjectGroup
{
public:
	void	Init(DWORD dwGroupId, const char *szGroupName);
	
	inline void	AddMember(const char *szName)
	{	
		m_ObjectNameCont.insert(szName);
	}

	inline void	RemoveMember(const char *szName)
	{
		m_ObjectNameCont.erase(szName);
	}

	void	SetGroupName(const char *szGroupName);
	int		SaveGroupData(PDB_FRIENDSGROUP_DATA	pData);

	DWORD	GetGroupId();
	int		GetMemberCount();
	const char*	GetGroupName();

private:

	typedef set<string>	OBJECTNAMECONT;
	OBJECTNAMECONT	m_ObjectNameCont;

	DWORD			m_GroupId;
	char			m_GroupName[MAXSIZE_GROUPNAME];
};

inline const char*	ObjectGroup::GetGroupName()
{
	return m_GroupName;
}

inline DWORD ObjectGroup::GetGroupId()
{
	return m_GroupId;
}

inline void ObjectGroup::SetGroupName(const char *szGroupName)
{
	if(szGroupName)
	{
		strncpy(m_GroupName, szGroupName, sizeof(m_GroupName));
	}
	else
	{
		memset(m_GroupName, 0, sizeof(m_GroupName));
	}
}

//-------------------------------------------------------------------------------------------------------

class ChatObjectMgr_S
{
public:
	void	Init();
	bool	PlayerOnLine(int nPlayerIdx);
	void	PlayerOffLine();
	void	LoadFriendsDataReq();
	void	SaveFriendsDataReq();
	int		LoadFriendsDataRet(BYTE *pMsg, int nSize);
	int		SaveFriendsData(BYTE *pData);
	int		ChgRoomChatPrivilege(DWORD dwRoomId, bool bCanChat);
	void	SetObjOnlineState(const char* szPlayerName, bool bOnline);
	void    CheckFriendOnlineFlag(void);
//	void    ObjPkValueChangeNotify(const char* szPlayerName);
	
	// Functions process package from client
	int		AddObject(int nSenderIdx, CHAT_ADDOBJECT_REQ *pc2sReq);
	int		ChangeRelation(int nSenderIdx, const char* szPlayerName, int nRelation);
	int		CreateGroup(int nSenderIdx, const char *szGroupName);
	int		DeleteGroup(int nSenderIdx, DWORD dwGroupId);
	int		RenameGroup(DWORD dwGroupId, const char *szGroupName);
	int		ChangeGroup(const char* szPlayerName, DWORD dwOldGroupId, DWORD dwNewGroupId);
	int		RemoveObject(const char* szPlayerName);
	int     RemoveAllObjectInGroup(DWORD dwGroupId);
	
	// Assistant functions
	int		CanCreateRoom();
	int		CanBeAddToRoom(const string &strSender);
	int		JoinRoom(DWORD dwRoomId);
	int		CanChatInRoom(DWORD dwRoomId);
	bool	IsJoinRoom(DWORD dwRoomId);
	bool	ForbidChatInRoom(DWORD dwRoomId, bool bForbid);
	void	LeaveRoom(DWORD dwRoomId);
	bool	IsPreventSendMsg();
	bool	IsPreventRecvMsg();
	bool	IsPreventSendMsgToMe(const char* szPlayerName);
	bool	IsPreventRecvMyMsg(const char* szPlayerName);
	bool	IsInUse();
	DWORD	GetPreChatTimeInLocalRoom();
	void	SetPreChatTimeInLocalRoom(DWORD dwTime);
	void    AddFriendToGroup(DWORD dwPlayerIndex,DWORD dwGroupId);
	bool    IsMyFriend(int nPlayerIndex);

	int		CheckChatTime();

	const PCHATOBJECT GetChatObjectList(int & maxObjCount);
protected:
	int		LoadFriendsData(BYTE *pData,int nSize);
	// Load Different Data Version
	int     LoadFriendsDataV1(BYTE * pData , int nSize);
	int		LoadFriendsDataCurVersion(BYTE *pData,int nSize);
	int     SendFriendsDataToClient(BYTE * pData);
	
	bool	HasGroup(const char *szGroupName);
	void	MakeInvalid(PCHATOBJECT pObject);
	bool	IsObjValid(const CHATOBJECT &object);
	DWORD	GetNewGroupId();
	bool	IsRoomFull();
	int		GetGroupCount();
	void	SendCreateGroupNotify(int nPlayerIdx, DWORD dwGroupId, const char *szGroupName);
	void	CreateInnerGroup(DWORD dwGroupId, const char *szName);
	void    MarkPlayerLevelAndSeries(int nPlayerIdx,BYTE & nLevelRet,BYTE & nSeriesRet);
	void    CheckOnlineState(int nObjIndex);
 
	ObjectGroup*		GetGroup(DWORD dwId);
	ObjectGroup*		GetEmptyGroup();
	PCHATOBJECT			GetChatObject(const char* szPlayerName);
	PCHATOBJECT			GetEmptyObject();

private:
	typedef struct _JoinedRoomInfo
	{
		DWORD	roomId;
		char	canSendMsg;

	} JoinedRoomInfo;

	bool			m_bPreventRecvMsg;
	bool			m_bPreventSendMsg;
	DWORD			m_NewGroupId;
	int				m_PlayerIdx;
	DWORD			m_PreChatTimeInLocalRoom;
	DWORD			m_PreChatTimeInGlobalRoom;
	DWORD			m_PreChatTimeInMapRoom;
	
	DWORD			m_preChatTime;	//通用聊天时间限制
	ObjectGroup		m_GroupCont[MAX_FRIENDGROUP_COUNT];
	CHATOBJECT		m_ObjectCont[MAX_OBJECT_COUNT];
	JoinedRoomInfo	m_JoinRoomIdCont[DEF_MAX_PLAYERCHATROOM];
	int             m_CheckOnlineIter;
	bool            m_LoadSucess;
};

inline bool ChatObjectMgr_S::IsPreventSendMsg()
{
	return m_bPreventSendMsg || !Player[m_PlayerIdx].CanChat();
}

inline bool ChatObjectMgr_S::IsPreventRecvMsg()
{
	return m_bPreventRecvMsg;
}

inline bool ChatObjectMgr_S::IsPreventSendMsgToMe(const char* szPlayerName)
{
	PCHATOBJECT pObject = GetChatObject(szPlayerName);
	return pObject ? pObject->info.bPreventSendMsgToMe : false;
}

inline bool ChatObjectMgr_S::IsPreventRecvMyMsg(const char* szPlayerName)
{
	PCHATOBJECT pObject = GetChatObject(szPlayerName);
	return pObject ? pObject->info.bPreventRecvMyMsg : false;
}

inline bool ChatObjectMgr_S::IsMyFriend(int nPlayerIndex)
{
	if (!IsValidPlayer(nPlayerIndex))
		return 0;

	int nNpcIndex = Player[nPlayerIndex].m_nIndex;
	if (!IsValidNpc(nNpcIndex))
		return 0;

	PCHATOBJECT pObject = GetChatObject(Npc[nNpcIndex].Name);
	if (pObject)
	{
		DWORD dwGounpId = pObject->info.groupId;

		if (dwGounpId != GROUPID_TEMP && dwGounpId != GROUPID_BLACK && dwGounpId != GROUPID_ENEMY)
			return 1;

		return 0;
	}

	return 0;
}

inline void ChatObjectMgr_S::MakeInvalid(PCHATOBJECT pObject)
{
	memset(pObject->name, 0, MAXSIZE_ROLENAME);
}

inline bool ChatObjectMgr_S::IsObjValid(const CHATOBJECT &object)
{
	if (strcmp(object.name, "") == 0)
	{
		return false;
	}
	return true;
}

inline DWORD ChatObjectMgr_S::GetNewGroupId()
{
	return ++m_NewGroupId;
}

inline int	ChatObjectMgr_S::GetGroupCount()
{
	int nCount = 0;

	for(int i = 0; i < MAX_FRIENDGROUP_COUNT; ++i)
	{
		if(INVALID_GROUP_ID != m_GroupCont[i].GetGroupId())
		{
			++nCount;
		}
	}

	return nCount;
}

inline bool ChatObjectMgr_S::IsInUse()
{
	return INVALID_PLAYER_INDEX != m_PlayerIdx;
}

inline bool ChatObjectMgr_S::PlayerOnLine(int nPlayerIdx)
{
	if(INVALID_PLAYER_INDEX == m_PlayerIdx && INVALID_PLAYER_INDEX != nPlayerIdx)
	{
		m_PlayerIdx = nPlayerIdx;
		LoadFriendsDataReq();
		return true;
	}
	else
	{
		return false;
	}
}

inline DWORD ChatObjectMgr_S::GetPreChatTimeInLocalRoom()
{
	return m_PreChatTimeInLocalRoom;
}

inline void	ChatObjectMgr_S::SetPreChatTimeInLocalRoom(DWORD dwTime)
{
	m_PreChatTimeInLocalRoom = dwTime;
}

inline bool	ChatObjectMgr_S::IsJoinRoom(DWORD dwRoomId)
{
	for(int nRoom = 0; nRoom < DEF_MAX_PLAYERCHATROOM; ++nRoom)
	{
		if(m_JoinRoomIdCont[nRoom].roomId == dwRoomId)
			return true;
	}

	return false;
}

inline bool	ChatObjectMgr_S::ForbidChatInRoom(DWORD dwRoomId, bool bForbid)
{
	for(int nRoom = 0; nRoom < DEF_MAX_PLAYERCHATROOM; ++nRoom)
	{
		if(m_JoinRoomIdCont[nRoom].roomId == dwRoomId)
		{
			m_JoinRoomIdCont[nRoom].canSendMsg = !bForbid;
			return true;
		}
	}

	return false;
}

inline const PCHATOBJECT ChatObjectMgr_S::GetChatObjectList(int & maxObjCount)
{
	maxObjCount = MAX_OBJECT_COUNT;
	return m_ObjectCont;
}

inline int ChatObjectMgr_S::CheckChatTime()
{
	DWORD	curTime = UNIX_TMIE_STAMP;
	if (curTime - m_preChatTime < 1)
		return chat_err_chattoofast;
	else
		m_preChatTime = curTime;

	return chat_err_none;
}

#endif





