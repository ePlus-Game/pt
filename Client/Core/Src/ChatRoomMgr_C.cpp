//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 15:14
//      File_base        : ChatRoomMgr_C
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
#include "ChatRoomMgr_C.h"
#include "ChatDataDef.h"

using namespace CHAT;

ChatRoom_C::ChatRoom_C(const string &strCreaterName, DWORD dwRoomId, const string &strRoomName) 
	   				  : m_CreaterName(strCreaterName), m_RoomId(dwRoomId), m_RoomName(strRoomName)
{
}

void ChatRoomMgr_C::Init()
{
	m_MaxRoomCount = DEF_MAX_PLAYERCHATROOM;
	m_MapIdToRoom.clear();
}

int	ChatRoomMgr_C::LeaveRoom(DWORD dwRoomId)
{
	int nRet = chat_err_none;

	MAPIDTOROOM::iterator itRoom = m_MapIdToRoom.find(dwRoomId);

	if( itRoom == m_MapIdToRoom.end() )
	{
		nRet = chat_err_roomnotexist;
	}
	else
	{
		m_MapIdToRoom.erase(itRoom);

		CHAT_LEAVE_ROOM	data;
		data.protocol.protocol = c2s_chat_family;
		data.protocol.subProtocol = chat_leaveroom;
		data.roomId = dwRoomId;
		data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

		SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);
	}

	return nRet;
}

int	ChatRoomMgr_C::KickRoomMemberReq(DWORD dwRoomId, const string &strName)
{
	int nRet = chat_err_none;

	MAPIDTOROOM::iterator itRoom = m_MapIdToRoom.find(dwRoomId);

	if(itRoom != m_MapIdToRoom.end())
	{
		if(itRoom->second.GetCreater() != GetPlayerName(CLIENT_PLAYER_INDEX))
		{
			nRet = chat_err_noaccesskickmember;
			return nRet;
		}

		if (itRoom->second.GetCreater() == strName)
		{
            return chat_err_none;
		}
	}
	else
	{
		nRet = chat_err_roomnotexist;
		return nRet;
	}

	if(chat_err_none == nRet)
	{
		CHAT_ROOM_KICKMEMBER	data;
		data.protocol.protocol = c2s_chat_family;
		data.protocol.subProtocol = chat_kickmemberfromroom;
		data.roomId = dwRoomId;
		strncpy(data.name, strName.c_str(), sizeof(data.name));
		data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

		SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);	
	}

	return nRet;
}

// Functions request operation to server

int ChatRoomMgr_C::CreateRoomReq(const char *szRoomName)
{
	if( m_MapIdToRoom.size() >= m_MaxRoomCount )
	{
		return chat_err_maxroom;
	}

	CHAT_CREATEROOM_REQ	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_createroom;
	strncpy(data.roomName, szRoomName, sizeof(data.roomName));
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);
	
	return chat_err_none;
}

int	ChatRoomMgr_C::AddMemberToRoomReq(DWORD dwRoomId, const string &strName)
{
	MAPIDTOROOM::iterator itRoom = m_MapIdToRoom.find(dwRoomId);

	if( itRoom == m_MapIdToRoom.end() )
	{
		return chat_err_none;
	}

	if (itRoom->second.GetCreater() == strName)
	{
		return chat_err_none; 
	}

	if( itRoom->second.GetCreater() != GetPlayerName(CLIENT_PLAYER_INDEX) )
	{
		return chat_err_denyaddmembertoroom;
	}

	CHATROOM_ADD_MEMBER data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_addmembertoroom;
	data.roomId = dwRoomId;
	strncpy(data.name, strName.c_str(), sizeof(data.name));
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

int ChatRoomMgr_C::ForbitChatInRoomReq(DWORD dwRoomId, const char *szPlayerName, bool bForbitChat)
{
	if(NULL == szPlayerName)
		return chat_err_none;

	MAPIDTOROOM::iterator	itRoom = m_MapIdToRoom.find(dwRoomId);	

	if(m_MapIdToRoom.end() == itRoom)
		return chat_err_roomnotexist;

	if( itRoom->second.GetCreater() != GetPlayerName(CLIENT_PLAYER_INDEX) )
		return chat_err_noaccesstooperate;

	CHATROOM_CHATPRIVOPE_REQ c2sReq;
	c2sReq.protocol.protocol = c2s_chat_family;
	c2sReq.protocol.subProtocol = chat_chatroomprivope;
	c2sReq.protocol.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.roomId = dwRoomId;
	strncpy(c2sReq.name, szPlayerName, sizeof(c2sReq.name));
	c2sReq.bForbid = bForbitChat;

	SendDataToServer(&c2sReq, c2sReq.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

const char * ChatRoomMgr_C::GetRoomOwnerName(DWORD dwRoomId)
{
	MAPIDTOROOM::iterator itRoom = m_MapIdToRoom.find(dwRoomId);
	
	if( itRoom == m_MapIdToRoom.end() )
		return NULL;
    else 
		return itRoom->second.GetCreater().c_str();
}

int	ChatRoomMgr_C::ChangeRoomOwner(DWORD dwRoomId, const char *szNewOwnerName)
{
	if(NULL == szNewOwnerName)
		return chat_err_none;

	MAPIDTOROOM::iterator itRoom = m_MapIdToRoom.find(dwRoomId);
	
	if( itRoom == m_MapIdToRoom.end() )
		return chat_err_none;

	if( itRoom->second.GetCreater() != GetPlayerName(CLIENT_PLAYER_INDEX) )
		return chat_err_noprivchgroomowner;

	CHATROOM_CHANGE_OWNER c2sReq;
	c2sReq.protocol.protocol = c2s_chat_family;
	c2sReq.protocol.subProtocol = chat_changeroomowner;
	c2sReq.protocol.len = sizeof(c2sReq) - PROTOCOL_SIZE;
	c2sReq.roomId = dwRoomId;
	strncpy(c2sReq.newOwnerName, szNewOwnerName, sizeof(c2sReq.newOwnerName));

	SendDataToServer(&c2sReq, c2sReq.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

// Functions process package from server

void ChatRoomMgr_C::OnCreateChatRoomNotify(PCHAT_CREATEROOM_RST ps2cRet)
{
	ChatRoom_C room(GetPlayerName(CLIENT_PLAYER_INDEX), ps2cRet->roomId, ps2cRet->roomName);
	
	room.AddMember( GetPlayerName(CLIENT_PLAYER_INDEX) );
	m_MapIdToRoom.insert(MAPIDTOROOM::value_type(ps2cRet->roomId, room));
}

void ChatRoomMgr_C::OnJoinRoomNotify(PCHAT_JOIN_ROOM ps2cRet)
{
	ChatRoom_C	room(ps2cRet->createrName, ps2cRet->roomId, ps2cRet->roomName);	

	m_MapIdToRoom.insert(MAPIDTOROOM::value_type(ps2cRet->roomId, room));
}

void ChatRoomMgr_C::OnMemberLeaveRoomNotify(DWORD dwRoomId, const string &strName)
{
	MAPIDTOROOM::iterator itRoom = m_MapIdToRoom.find(dwRoomId);

	if(itRoom != m_MapIdToRoom.end())
	{
		itRoom->second.LeaveRoom(strName);
	}
}

void ChatRoomMgr_C::OnAddRoomMemberNotify(DWORD dwRoomId, const string &strName)
{
	MAPIDTOROOM::iterator itRoom = m_MapIdToRoom.find(dwRoomId);

	if( itRoom != m_MapIdToRoom.end() )
	{
		itRoom->second.AddMember(strName);
	}	
}

void ChatRoomMgr_C::OnKickRoomMemberNotify(DWORD dwRoomId, const string &strName)
{
	MAPIDTOROOM::iterator itRoom = m_MapIdToRoom.find(dwRoomId);

	if(itRoom != m_MapIdToRoom.end())
	{
		if(strName == GetPlayerName(CLIENT_PLAYER_INDEX))
		{
			m_MapIdToRoom.erase(itRoom);
		}
		else
		{
			itRoom->second.LeaveRoom(strName);
		}
	}
}

void ChatRoomMgr_C::OnChangeRoomOwnerNotify(PCHATROOM_CHANGE_OWNER ps2cRet)
{
	MAPIDTOROOM::iterator itRoom = m_MapIdToRoom.find(ps2cRet->roomId);
	
	if( itRoom != m_MapIdToRoom.end() )
	{
		itRoom->second.SetOwner( ps2cRet->newOwnerName );
	}
}

