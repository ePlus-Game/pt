//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-7-20 15:15
//      File_base        : ChatRoomMgr_S
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
#include "ChatRoomMgr_S.h"
#include "ChatDataDef.h"
#include "buff_alloc.h"

using namespace CHAT;

__allocator<ChatRoom_S, CHATROOMS_ALLOC_GRANULARITY, MAXCOUNT_CHATROOMS>	g_ChatRooomAlloc;

DWORD ChatRoomMgr_S::m_UsedRoomId = DYNAMIC_ROOMID_BEGIN;

ChatRoom_S*	ChatRoomMgr_S::Alloc()
{
	return g_ChatRooomAlloc._alloc();
}

void ChatRoomMgr_S::Free(ChatRoom_S	*pRoom)
{
	g_ChatRooomAlloc._free(pRoom);
}

void ChatRoom_S::Init(const char *szCreater, const char *szRoomName, DWORD dwRoomId, int nMemberSize)
{
	m_RoomId = dwRoomId;

	if( m_MemberIndex.size() < nMemberSize )
	{
		m_MemberIndex.resize(nMemberSize, INVALID_PLAYER_INDEX);
	}

	if(NULL != szCreater)
		strncpy(m_CreaterName, szCreater, sizeof(m_CreaterName));
	else
		memset(m_CreaterName, 0, sizeof(m_CreaterName));

	if(NULL != szRoomName)
		strncpy(m_RoomName, szRoomName, sizeof(m_RoomName));
	else
		memset(m_RoomName, 0, sizeof(m_RoomName));
}

bool ChatRoom_S::IsInRoom(int nPlayerIdx)
{
	if(INVALID_PLAYER_INDEX == nPlayerIdx)
		return false;

	ROOMMEMBERCONT::size_type pos;
	ROOMMEMBERCONT::size_type size = m_MemberIndex.size();

	for(pos = 0; pos < size; ++pos)
	{
		if(m_MemberIndex[pos] == nPlayerIdx)
			return true;
	}

	return false;
}

int ChatRoom_S::CanbeAddedToRoom(int nPlayerIdx)
{
	if(!IsValidPlayer(nPlayerIdx))
		return chat_err_membernotonline;

	if (IsOwner(nPlayerIdx))
		return chat_err_denyaddmembertoroom;

	if (IsInRoom(nPlayerIdx))
		return chat_err_objalreadyexist;

	return chat_err_none;
}

bool ChatRoom_S::IsOwner(int nPlayerIdx)
{
	return 0 == strcmp(m_CreaterName, GetPlayerName(nPlayerIdx));
}

void ChatRoom_S::RandomOwner()
{
	ROOMMEMBERCONT::size_type pos;
	ROOMMEMBERCONT::size_type size = m_MemberIndex.size();

	for(pos = 0; pos < size; ++pos)
	{
		if(INVALID_PLAYER_INDEX != m_MemberIndex[pos])
		{
			SetOwner( GetPlayerName(m_MemberIndex[pos]) );
			return;
		}
	}
}

int ChatRoom_S::AddMember(int nPlayerIdx)
{
	if(INVALID_PLAYER_INDEX == nPlayerIdx)
	{
		return chat_err_membernotonline;
	}

	ROOMMEMBERCONT::size_type pos;
	ROOMMEMBERCONT::size_type size = m_MemberIndex.size();

	for(pos = 0; pos < size; ++pos)
	{
		if(INVALID_PLAYER_INDEX == m_MemberIndex[pos])
		{
			m_MemberIndex[pos] = nPlayerIdx;
			return chat_err_none;
		}
	}

	if( m_MemberIndex.size() >= MAX_ROOM_MEMBERNUM )
	{
		return chat_err_roommemberfull;
	}
	else
	{
		m_MemberIndex.push_back(nPlayerIdx);
		return chat_err_none;
	}
}

void ChatRoom_S::SyncNewOwner()
{
	CHATROOM_CHANGE_OWNER	s2cRet;
	
	s2cRet.protocol.protocol = s2c_chat_family;
	s2cRet.protocol.subProtocol = chat_changeroomowner;
	s2cRet.protocol.len = sizeof(s2cRet) - PROTOCOL_SIZE;
	s2cRet.roomId = m_RoomId;
	strncpy(s2cRet.newOwnerName, m_CreaterName, sizeof(s2cRet.newOwnerName));

	ROOMMEMBERCONT::size_type pos;
	ROOMMEMBERCONT::size_type size = m_MemberIndex.size();

	for(pos = 0; pos < size; ++pos)
	{
		if(INVALID_PLAYER_INDEX != m_MemberIndex[pos])
			SendDataToClient(m_MemberIndex[pos], &s2cRet, s2cRet.protocol.len + PROTOCOL_SIZE);
	}
}

void ChatRoom_S::JoinRoomSync(int nPlayerIdx)
{
	CHAT_JOIN_ROOM joinRoom;
	joinRoom.protocol.protocol = s2c_chat_family;
	joinRoom.protocol.subProtocol = chat_joinroom;
	joinRoom.roomId = m_RoomId;
	strncpy(joinRoom.createrName, m_CreaterName, sizeof(joinRoom.createrName));
	strncpy(joinRoom.roomName, m_RoomName, sizeof(joinRoom.roomName));
	joinRoom.protocol.len = sizeof(joinRoom) - PROTOCOL_SIZE;

	SendDataToClient(nPlayerIdx, &joinRoom, joinRoom.protocol.len + PROTOCOL_SIZE);	
}

void ChatRoom_S::AddMemberSync(int nAddedMemIdx)
{
	ROOMMEMBERCONT::size_type pos;
	ROOMMEMBERCONT::size_type size = m_MemberIndex.size();

	CHATROOM_ADD_MEMBER data1;
	data1.protocol.protocol = s2c_chat_family;
	data1.protocol.subProtocol = chat_addmembertoroom;
	data1.roomId = m_RoomId;
	strncpy(data1.name, GetPlayerName(nAddedMemIdx), sizeof(data1.name));
	data1.protocol.len = sizeof(data1) - PROTOCOL_SIZE;

	CHATROOM_ADD_MEMBER data2;
	data2.protocol.protocol = s2c_chat_family;
	data2.protocol.subProtocol = chat_addmembertoroom;
	data2.roomId = m_RoomId;
	data2.protocol.len = sizeof(data2) - PROTOCOL_SIZE;

	for(pos = 0; pos < size; ++pos)
	{
		if(INVALID_PLAYER_INDEX != m_MemberIndex[pos])
		{
			SendDataToClient(m_MemberIndex[pos], &data1, data1.protocol.len + PROTOCOL_SIZE);
				
			strncpy(data2.name, GetPlayerName(m_MemberIndex[pos]), sizeof(data2.name));
			SendDataToClient(nAddedMemIdx, &data2, data2.protocol.len + PROTOCOL_SIZE);	
		}
	}	
}

void ChatRoom_S::ForEach(CHATCALLBACK pCallBack,  const void *pCallbackParam,
				BROADCASTFILTER pFilter /* = NULL */, unsigned int uFilterPassby /* = 0 */)
{
	ROOMMEMBERCONT::size_type pos;
	ROOMMEMBERCONT::size_type size = m_MemberIndex.size();

	for(pos = 0; pos < size; ++pos)
	{
		if(INVALID_PLAYER_INDEX != m_MemberIndex[pos])
		{
			if(NULL == pFilter || pFilter(m_MemberIndex[pos], uFilterPassby) )
				pCallBack(pCallbackParam, m_MemberIndex[pos]);
		}
	}
}

int ChatRoom_S::LeaveRoom(int nPlayerIndex)
{
	ROOMMEMBERCONT::size_type pos;
	ROOMMEMBERCONT::size_type size = m_MemberIndex.size();

	for(pos = 0; pos < size; ++pos)
	{
		if(m_MemberIndex[pos] == nPlayerIndex)
		{
			m_MemberIndex[pos] = INVALID_PLAYER_INDEX;
			break;
		}
	}

	int nCount = 0;

	CHAT_LEAVEROOM_NOTIFY	data;
	data.protocol.protocol = s2c_chat_family;
	data.protocol.subProtocol = chat_leaveroom;
	data.roomId = m_RoomId;
	strncpy(data.name, GetPlayerName(nPlayerIndex), sizeof(data.name));
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

	for(pos = 0; pos < size; ++pos)
	{
		if(m_MemberIndex[pos] != INVALID_PLAYER_INDEX)
		{
			SendDataToClient(m_MemberIndex[pos], &data, data.protocol.len + PROTOCOL_SIZE);
			++nCount;
		}
	}

	return nCount;
}

void ChatRoom_S::DelMemberDirect(int nPlayerIdx)
{
	ROOMMEMBERCONT::size_type pos;
	ROOMMEMBERCONT::size_type size = m_MemberIndex.size();

	for(pos = 0; pos < size; ++pos)
	{
		if(m_MemberIndex[pos] == nPlayerIdx)
		{
			m_MemberIndex[pos] = INVALID_PLAYER_INDEX;
			break;
		}
	}	
}

int ChatRoom_S::KickMember(int nSenderIdx, int nPlayerIdx)
{
	ROOMMEMBERCONT::size_type	pos;
	ROOMMEMBERCONT::size_type	size = m_MemberIndex.size();

	// Notify all member this player was kicked from room
	CHATROOM_KICKMEMBER_NOTIFY	data;
	data.protocol.protocol = s2c_chat_family;
	data.protocol.subProtocol = chat_memberkicknotify;
	data.roomId = m_RoomId;
	strncpy(data.name, GetPlayerName(nPlayerIdx), sizeof(data.name));
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

	int nCount = 0;

	for(pos = 0; pos < size; ++pos)
	{
		if(INVALID_PLAYER_INDEX != m_MemberIndex[pos])
		{
			SendDataToClient(m_MemberIndex[pos], &data, data.protocol.len + PROTOCOL_SIZE);
			++nCount;
		}
	}	

	for(pos = 0; pos < size; ++pos)
	{
		if(m_MemberIndex[pos] == nPlayerIdx)
		{
			m_MemberIndex[pos] = INVALID_PLAYER_INDEX;
			--nCount;
		}
	}

	return nCount;;
}

ChatRoomMgr_S::~ChatRoomMgr_S()
{
	// 这里不要施放资源了，因为allcator会自动施放
	// 如果allocator先于这里施放，这里就会当掉
// 	MAPID2ROOM::iterator	itRoom = m_id2Room.begin();
// 	MAPID2ROOM::iterator	itEnd = m_id2Room.end();
// 
// 	for(; itRoom != itEnd; ++itRoom)
// 		Free(itRoom->second);
}

int ChatRoomMgr_S::CreateRoom(const char *szCreaterName, const char *szRoomName, DWORD &dwOutRoomId)
{
	ChatRoom_S	*pChatRoom = Alloc();

	if(NULL == pChatRoom)
		return chat_err_serverroomfull;

	DWORD dwRoomId = GetNewRoomId();
	pChatRoom->Init(szCreaterName, szRoomName, dwRoomId, DEF_ROOM_MEMBERNUM);

	bool bAdd = AddRoom(dwRoomId, pChatRoom);
	
	if(!bAdd)
	{
		_ASSERT(bAdd);
		return chat_err_createroomfailed;
	}

	dwOutRoomId = dwRoomId;

	return chat_err_none;
}

// Functions process package from client

int ChatRoomMgr_S::AddMemberToRoom(int nSenderIdx, DWORD dwRoomId, int nMemberIdx)
{
	int	nRet = chat_err_none;
	ChatRoom_S *pRoom = GetChatRoom(dwRoomId);
	
	if(pRoom)
	{
		nRet = pRoom->CanbeAddedToRoom(nMemberIdx);

		if (chat_err_none != nRet)
			return nRet;
		
		nRet = pRoom->AddMember(nMemberIdx);
		
		if(chat_err_none == nRet)
		{
			pRoom->JoinRoomSync(nMemberIdx);
			pRoom->AddMemberSync(nMemberIdx);
		}
	}

	return nRet;
}

void ChatRoomMgr_S::LeaveRoom(int nPlayerIdx, DWORD dwRoomId)
{
	if(INVALID_ROOM_ID != dwRoomId)
	{
		ChatRoom_S	*pChatRoom = GetChatRoom(dwRoomId);

		if(pChatRoom)
		{
			int nCount = pChatRoom->LeaveRoom(nPlayerIdx);

			if(nCount <= 0)
				DeleteRoom(dwRoomId);
			else
			{
				if( pChatRoom->IsOwner(nPlayerIdx) )
				{
					pChatRoom->RandomOwner();
					pChatRoom->SyncNewOwner();
				}
			}
		}
	}
}

int	ChatRoomMgr_S::KickRoomMember(int nPlayerIdx, DWORD dwRoomId, int nMemberIdx)
{
	int nRet = chat_err_none;
	ChatRoom_S	*pRoom = GetChatRoom(dwRoomId);

	if(pRoom)
	{
		if( pRoom->IsOwner(nPlayerIdx) )
		{
			int	nCount = pRoom->KickMember(nPlayerIdx, nMemberIdx);	

			if(nCount <= 0)
				DeleteRoom(dwRoomId);
			else
			{
				if(nMemberIdx == nPlayerIdx)
				{
					pRoom->RandomOwner();
					pRoom->SyncNewOwner();
				}
			}
		}
		else
		{
			nRet = chat_err_noaccesskickmember;
		}
	}

	return nRet;
}

int	ChatRoomMgr_S::ChangeRoomOwner(int nSenderIdx, DWORD dwRoomId, int nNewOwnerIdx)
{
	ChatRoom_S *pRoom = GetChatRoom(dwRoomId);

	if(NULL == pRoom)
		return chat_err_roomnotexist;
		
	if( !pRoom->IsOwner(nSenderIdx) )
		return chat_err_noprivchgroomowner;

	if( !pRoom->IsInRoom(nNewOwnerIdx) )
		return chat_err_membernotinroom;

	pRoom->SetOwner( GetPlayerName(nNewOwnerIdx) );
	pRoom->SyncNewOwner();

	return chat_err_none;
}