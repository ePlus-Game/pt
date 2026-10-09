//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 15:13
//      File_base        : ChatRoomMgr_S
//      File_ext         : h
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
#ifndef _ChatRoomMgr_S_h
#define _ChatRoomMgr_S_h

#include "GlobalDef.h"

class ChatRoom_S
{
public:
	void	Init(const char *szCreater, const char *szRoomName, DWORD dwRoomId, int nMemberSize);

	// Functions process package from client
	int		KickMember(int nSenderIdx, int nPlayerIndex);
	int		LeaveRoom(int nPlayerIndex);
	int		AddMember(int nPlayerIdx);

	// Assistant functions
	void	ForEach(CHATCALLBACK pCallBack,  const void *pCallbackParam,
				BROADCASTFILTER pFilter = NULL, unsigned int uFilterPassby = 0);

	DWORD	GetRoomId();
	void	JoinRoomSync(int nPlayerIdx);
	void	AddMemberSync(int nAddedMemIdx);
	void	SyncNewOwner();
	void	DelMemberDirect(int nPlayerIdx);
	bool	IsInRoom(int nPlayerIdx);
	void	SetOwner(const char *szOwnerName);
	void	RandomOwner();
	
	int     CanbeAddedToRoom(int nPlayerIdx);
	bool	IsOwner(int nPlayerIdx);
	const char* GetCreater();
	
private:
	typedef vector<int>	ROOMMEMBERCONT;

	ROOMMEMBERCONT	m_MemberIndex;
	char			m_CreaterName[MAXSIZE_ROLENAME];
	char			m_RoomName[MAXSIZE_CHATROOMNAME];
	DWORD			m_RoomId;
};

inline DWORD ChatRoom_S::GetRoomId()
{
	return m_RoomId;
}

inline const char* ChatRoom_S::GetCreater()
{
	return m_CreaterName;
}

inline void ChatRoom_S::SetOwner(const char *szOwnerName)
{
	if(NULL != szOwnerName)
		strncpy(m_CreaterName, szOwnerName, sizeof(m_CreaterName));
}

class ChatRoomMgr_S
{
public:
	~ChatRoomMgr_S();

	// Functions process package from client
	int		AddMemberToRoom(int nSenderIdx, DWORD dwRoomId, int nMemberIdx);
	void	LeaveRoom(int nPlayerIdx, DWORD dwRoomId);
	int		KickRoomMember(int nPlayerIdx, DWORD dwRoomId, int nMemberIdx);
	int		CreateRoom(const char *szCreaterName, const char *szRoomName, DWORD &dwOutRoomId);
	int		ChangeRoomOwner(int nSenderIdx, DWORD dwRoomId, int nNewOwnerIdx);

	// Used by social relation
	void	DeleteRoom(DWORD dwRoomId);
	
	// Assistant functions
	ChatRoom_S*	GetChatRoom(DWORD dwRoomId);	
	
protected:
	DWORD	GetNewRoomId();
	bool	AddRoom(DWORD dwRoomId, ChatRoom_S *pRoom);

	static ChatRoom_S*	Alloc();
	static void			Free(ChatRoom_S *pRoom);

private:
	typedef map<DWORD, ChatRoom_S*>	MAPID2ROOM;

	MAPID2ROOM		m_id2Room;
	static DWORD	m_UsedRoomId;
};

inline DWORD ChatRoomMgr_S::GetNewRoomId()
{
	return ++m_UsedRoomId;
}

inline bool	ChatRoomMgr_S::AddRoom(DWORD dwRoomId, ChatRoom_S *pRoom)
{
	if(pRoom)
	{
		pair<MAPID2ROOM::iterator, bool> ret;
		ret = m_id2Room.insert( MAPID2ROOM::value_type(dwRoomId, pRoom) );
		return ret.second;
	}

	return false;
}

inline void ChatRoomMgr_S::DeleteRoom(DWORD dwRoomId)
{
	MAPID2ROOM::iterator itRoom = m_id2Room.find(dwRoomId);

	if( itRoom != m_id2Room.end() )
	{
		Free(itRoom->second);
		m_id2Room.erase(itRoom);
	}
}

inline ChatRoom_S*	ChatRoomMgr_S::GetChatRoom(DWORD dwRoomId)
{
	MAPID2ROOM::iterator itRoom = m_id2Room.find(dwRoomId);

	return itRoom == m_id2Room.end() ? NULL : itRoom->second;
}

#endif