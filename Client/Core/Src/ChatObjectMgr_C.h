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

#include "ChatDataDef.h"

using std::map;
using std::vector;
using std::string;
using std::set;

using namespace CHAT;

//--------------------------------------------------------------------------------------------------------

class ObjectGroup
{
public:
	inline ObjectGroup(const char *szName)
	{
		strncpy(m_GroupName, szName, sizeof(m_GroupName));
	}
	
	inline void	AddMember(const char *szName)
	{
		m_ObjectNameCont.insert(szName);
	}

	inline void	RemoveMember(const char *szName)
	{
		m_ObjectNameCont.erase(szName);
	}

	inline int GetMemberCount()
	{
		return m_ObjectNameCont.size();
	}

	inline const char*	GetGroupName() const
	{
		return m_GroupName;
	}

	inline void	SetGroupName(const char *szName)
	{
		strncpy(m_GroupName, szName, sizeof(m_GroupName));
	}

	inline int GetObjectNames(char *pNames)
	{
		UI_CHAT_OBJINFO	*pObjInfo = (UI_CHAT_OBJINFO*)pNames;

		OBJECTIDCONT::const_iterator it;
		OBJECTIDCONT::const_iterator itEnd(m_ObjectNameCont.end());

		for(it = m_ObjectNameCont.begin(); it != itEnd; ++it)
		{
			strncpy(pObjInfo->szName, it->c_str(), sizeof(pObjInfo->szName));
			++pObjInfo;
		}

		return m_ObjectNameCont.size();
	}

	inline bool HasObj (const char * pNames)
	{
		if (pNames==0)
			return false;
		
		OBJECTIDCONT::const_iterator it;
		OBJECTIDCONT::const_iterator itEnd(m_ObjectNameCont.end());
		
		for(it = m_ObjectNameCont.begin(); it != itEnd; ++it)
		{
			if (strcmp(it->c_str(),pNames)==0)
				return true;
		}

		return false;
	}

private:
	typedef	set<string>	OBJECTIDCONT;
	
	OBJECTIDCONT	m_ObjectNameCont;
	char			m_GroupName[MAXSIZE_GROUPNAME];
};

//-------------------------------------------------------------------------------------------------------

class ChatObjectMgr_C
{
public:
	void	Init();

	int		OnLoadFriendsDataNotify(BYTE *pFriendsData);

	// Chat
	// Request operation to server
	int		ChatToSomeoneByName(const string &strReceiver, BYTE *pMsg, int nMsgLen);
	int		ChatInRoom(DWORD dwRoomId, BYTE *pMsg, int nMsgLen);
	void	AddTempObjectName(const string &strName);

	// Friends Management
	// Request operation to server
	int		AddObjectReq(const string &strName, int nGroupId, int nRelation);
	int		ChangeRelationReq(const string &strName, int nRelation);
	int		RemoveObjectReq(const string &strName);
	
	// Process package from server
	void	AddObject(CHAT_ADDOBJECT_NOTIFY *ps2cNotify);
	void	ChangeRelation(const string &strName, int nRelation);
	void	RemoveObject(const string &strName);
	void	ChangeGroup(const string &strName, DWORD dwOldGroupId, DWORD dwNewGroupId);
	void	RenameGroup(DWORD dwGroupId, const string &strName);
	void	SetOnlineStatus(const char* szPlayerName, bool bOnline,int nSeries,int nLevel);
	
	// Group Management
	// Request operation to server
	int		CreateGroupReq(const string &strName);
	int		ChangeGroupReq(const string &strName, DWORD dwOldGroupId, DWORD dwNewGroupId);
	int		DeleteGroupReq(DWORD dwGroupId);
	int		RenameGroupReq(DWORD dwGroupId, const string &strName);

	// Process package from server
	void	CreateGroup(DWORD dwGroupId, const string &strGroupName);
	void	DeleteGroup(DWORD dwGroupId);
	
	// Functions provide data for UI
	int		GetGroupIdAndNames(DWORD *pGroupIds, char *pGroupNames);
	int		GetGroupMemberInfo(DWORD dwGroupId, char *pOutBuffer);

	// Assistant functions
	int		CanCreateRoom();
	bool	HasObject(const string &strName);
	bool	IsPreventSendMsg();
	bool	IsPreventRecvMsg();
	bool	IsPreventSendMsgToMe(const string &strName);
	bool	IsPreventRecvMyMsg(const string &strName);
	bool    IsObjectInGroup(const string &strName,int nGroupId);
	DWORD	GetPreChatTimeInLocalRoom();
	void	SetPreChatTimeInLocalRoom(DWORD dwTime);
	ObjectGroup* GetGroupByPlayerName(const char* szPlayerName);
	DWORD	     GetObjGroupId(const char *szPlayerName);
	void         PkValueChangeNotify(const char* szPlayerName, const int nPkValue);
	void         UpdatePkValue(void);
	bool         IsObjectOnline(const string &strName);
	
private:
	bool	HasGroup(const string &strName);
    CHATOBJECT_INFO*	GetObjectInfo(const string &strName);
	ObjectGroup*		GetGroup(const string &strName);
	ObjectGroup*		GetGroup(DWORD dwId);

	// Load Different Data Version
	int		LoadFriendsDataCurVersion(BYTE *pData);

private:
	typedef map<string, CHATOBJECT_INFO>	MAPOBJNAMETOINFO;
	typedef	map<DWORD,	ObjectGroup>		MAPIDTOGROUP;
	typedef map<string,  DWORD>              MAPNAMETOPKVALUE;

	MAPIDTOGROUP			m_GroupCont;
	MAPOBJNAMETOINFO		m_ObjNameToInfo;
	MAPNAMETOPKVALUE          m_ObjNameToPkValue;

	bool			m_bPreventRecvMsg;
	bool			m_bPreventSendMsg;
	DWORD			m_PreChatTimeInLocalRoom;
};

inline DWORD ChatObjectMgr_C::GetObjGroupId(const char *szPlayerName)
{
	CHATOBJECT_INFO* pInfo = GetObjectInfo(szPlayerName);

	if(NULL != pInfo)
		return pInfo->groupId;
	else
		return INVALID_GROUP_ID;
}

inline bool ChatObjectMgr_C::HasObject(const string &strName)
{
	return m_ObjNameToInfo.find(strName) != m_ObjNameToInfo.end();
}

inline bool ChatObjectMgr_C::IsPreventSendMsg()
{
	return m_bPreventSendMsg;
}

inline bool ChatObjectMgr_C::IsPreventRecvMsg()
{
	return m_bPreventRecvMsg;
}

inline bool ChatObjectMgr_C::IsPreventSendMsgToMe(const string &strName)
{
	MAPOBJNAMETOINFO::iterator itInfo = m_ObjNameToInfo.find(strName);
	return itInfo != m_ObjNameToInfo.end() ? itInfo->second.bPreventSendMsgToMe : false;
}

inline bool ChatObjectMgr_C::IsObjectOnline(const string &strName)
{
	MAPOBJNAMETOINFO::iterator itInfo = m_ObjNameToInfo.find(strName);
	if( itInfo != m_ObjNameToInfo.end() )
	{
		 return ChatUtil::IsOnline(itInfo->second.onlineTag);
	}//endif
	else
		return false;
}

inline bool ChatObjectMgr_C::IsPreventRecvMyMsg(const string &strName)
{
	MAPOBJNAMETOINFO::iterator itInfo = m_ObjNameToInfo.find(strName);
	return itInfo != m_ObjNameToInfo.end() ? itInfo->second.bPreventRecvMyMsg : false;	
}

inline CHATOBJECT_INFO*	ChatObjectMgr_C::GetObjectInfo(const string &strName)
{
	MAPOBJNAMETOINFO::iterator itInfo = m_ObjNameToInfo.find(strName);
	return itInfo != m_ObjNameToInfo.end() ? &itInfo->second : NULL;
}

inline int ChatObjectMgr_C::CanCreateRoom()
{
	return m_bPreventSendMsg ? chat_err_noaccesscreateroom : chat_err_none;
}

inline void ChatObjectMgr_C::AddTempObjectName(const string &strName)
{
	ObjectGroup *pGroup = GetGroup(GROUPID_TEMP);

	if(pGroup)
	{
		pGroup->AddMember(strName.c_str());
	}
}

inline ObjectGroup*	ChatObjectMgr_C::GetGroup(DWORD dwId)
{
	MAPIDTOGROUP::iterator it = m_GroupCont.find(dwId);
	return it != m_GroupCont.end() ? &(it->second) : NULL;
}

inline void ChatObjectMgr_C::ChangeRelation(const string &strName, int nRelation)
{
	PCHATOBJECT_INFO	pInfo = GetObjectInfo(strName);
	
	if(pInfo)
	{
		pInfo->relation = (BYTE)nRelation;	
	}
}

inline void ChatObjectMgr_C::DeleteGroup(DWORD dwGroupId)
{
	m_GroupCont.erase(dwGroupId);
}

inline void ChatObjectMgr_C::RenameGroup(DWORD dwGroupId, const string &strName)
{
	ObjectGroup	*pGroup = GetGroup(dwGroupId);

	if(pGroup)
	{
		pGroup->SetGroupName(strName.c_str());
	}	
}

inline void ChatObjectMgr_C::CreateGroup(DWORD dwGroupId, const string &strGroupName)
{
	ObjectGroup	grp(strGroupName.c_str());
	m_GroupCont.insert(MAPIDTOGROUP::value_type(dwGroupId, grp));	
}

inline DWORD ChatObjectMgr_C::GetPreChatTimeInLocalRoom()
{
	return m_PreChatTimeInLocalRoom;
}

inline void	ChatObjectMgr_C::SetPreChatTimeInLocalRoom(DWORD dwTime)
{
	m_PreChatTimeInLocalRoom = dwTime;
}


inline void ChatObjectMgr_C::SetOnlineStatus(const char* szPlayerName, bool bOnline,int nSeries,int nLevel)
{
	PCHATOBJECT_INFO pInfo = GetObjectInfo(szPlayerName);

	if(NULL != pInfo)
	{
		pInfo->nSeries = nSeries;
    	pInfo->nLevel  = nLevel;
		ChatUtil::SetOnlineTag(pInfo->onlineTag, bOnline);
	}
}

inline ObjectGroup* ChatObjectMgr_C::GetGroupByPlayerName(const char* szPlayerName)
{
	PCHATOBJECT_INFO pInfo = GetObjectInfo(szPlayerName);

	if(NULL != pInfo)
		return GetGroup(pInfo->groupId);
	else
		return NULL;
}

#endif // #ifdef _ChatObjectMgr_C_h





