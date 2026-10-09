//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 15:12
//      File_base        : ChatRoomMgr_C
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
#ifndef _ChatRoomMgr_C_h
#define _ChatRoomMgr_C_h

class ChatRoom_C
{
public:
	ChatRoom_C(const string &strCreaterName, DWORD dwRoomId, const string &strRoomName);

	void	AddMember(const string &strName);
	void	KickMember(const string &strName);
	void	LeaveRoom(const string &strName);

	const string& GetCreater();
	void	SetOwner(const char *szOwner);
	
private:
	typedef	set<string>		ROOMMEMBERCONT;

	ROOMMEMBERCONT		m_RoomMemberCont;
	string				m_CreaterName;
	string				m_RoomName;
	DWORD				m_RoomId;
};


inline void ChatRoom_C::AddMember(const string &strName)
{
	if(m_RoomMemberCont.size() >= MAX_ROOM_MEMBERNUM)
	{
		m_RoomMemberCont.erase( m_RoomMemberCont.begin() );
	}

	m_RoomMemberCont.insert(strName);
}

inline void ChatRoom_C::LeaveRoom(const string &strName)
{
	m_RoomMemberCont.erase(strName);
}

inline const string& ChatRoom_C::GetCreater()
{
	return m_CreaterName;
}

inline void ChatRoom_C::SetOwner(const char *szOwner)
{
	m_CreaterName = szOwner;
}

//--------------------------------------------------------------------------------------

class ChatRoomMgr_C
{
public:
	void	Init();

	// Functions request operation to server
	int		KickRoomMemberReq(DWORD dwRoomId, const string &strName);
	int		LeaveRoom(DWORD dwRoomId);
	int		CreateRoomReq(const char *szRoomName);
	int		AddMemberToRoomReq(DWORD dwRoomId, const string &strName);
	int		ChangeRoomOwner(DWORD dwRoomId, const char *szNewOwnerName);
	int		ForbitChatInRoomReq(DWORD dwRoomId, const char *szPlayerName, bool bForbitChat);

	const char * GetRoomOwnerName (DWORD dwRoomId);
public:
	// Functions process package from server
	void	OnCreateChatRoomNotify(PCHAT_CREATEROOM_RST ps2cRet);
	void	OnJoinRoomNotify(PCHAT_JOIN_ROOM ps2cRet);
	void	OnMemberLeaveRoomNotify(DWORD dwRoomId, const string &strName);
	void	OnKickRoomMemberNotify(DWORD dwRoomId, const string &strName);
	void	OnAddRoomMemberNotify(DWORD dwRoomId, const string &strName);
	void	OnChangeRoomOwnerNotify(PCHATROOM_CHANGE_OWNER ps2cRet);

private:
	typedef	map<DWORD, ChatRoom_C>	MAPIDTOROOM;

	MAPIDTOROOM				m_MapIdToRoom;
	int						m_MaxRoomCount;
};

#endif
