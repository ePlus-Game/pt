//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-12 9:36
//      File_base        : ChatGroupMgr
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

#include "ChatObjectMgr_S.h"
#include "CoreRelated.h"
#include "ChatCenter_S.h"
#include "KSubWorld.h"
#include "cfs_db_interface.h"

#define MIN_CHAT_LEVEL_IN_GLOBAL_ROOM 30
#define MIN_CHAT_LEVEL_IN_MAP_ROOM    30


void ObjectGroup::Init(DWORD dwGroupId, const char *szGroupName)
{
	m_GroupId = dwGroupId;
	m_ObjectNameCont.clear();
	strncpy(m_GroupName, szGroupName, sizeof(m_GroupName));
}

int ObjectGroup::GetMemberCount()
{
	return m_ObjectNameCont.size();
}


int ObjectGroup::SaveGroupData(PDB_FRIENDSGROUP_DATA pData)
{
	strncpy(pData->szGroupName, m_GroupName, sizeof(pData->szGroupName));
	pData->GroupId = m_GroupId;

	pData->MemberCount = GetMemberCount();

	CHATOBJECTNAME* pMemberName = (CHATOBJECTNAME*)pData->MemberName;
	
	OBJECTNAMECONT::iterator it = m_ObjectNameCont.begin();
	while(it != m_ObjectNameCont.end())
	{
		memset(pMemberName->name, 0, MAXSIZE_ROLENAME);
		strncpy(pMemberName->name, it->c_str(), sizeof(pMemberName->name));
		pMemberName->name[MAXSIZE_ROLENAME-1] = '\0';
		pMemberName++;
		++it;
	}

	return DBFriendsDataParser::GroupDataSize(pData);
}

void ChatObjectMgr_S::Init()
{
	memset(m_ObjectCont, 0, sizeof(m_ObjectCont));

	for(int i = 0; i < DEF_MAX_PLAYERCHATROOM; ++i)
	{
		m_JoinRoomIdCont[i].roomId = INVALID_ROOM_ID;
		m_JoinRoomIdCont[i].canSendMsg = 1;
	}

	for(int j = 0; j < MAX_FRIENDGROUP_COUNT; ++j)
	{
		m_GroupCont[j].Init(INVALID_GROUP_ID, "");
	}

	m_NewGroupId = DYNAMIC_GROUPID_BEGIN;
	m_bPreventRecvMsg = false;
	m_bPreventSendMsg = false;
	m_PlayerIdx = INVALID_PLAYER_INDEX;
	m_PreChatTimeInLocalRoom = 0;
	m_PreChatTimeInGlobalRoom = 0;
	m_PreChatTimeInMapRoom = 0;
	m_preChatTime = 0;
	m_CheckOnlineIter = 0;
	m_LoadSucess      = true;
}

void ChatObjectMgr_S::PlayerOffLine()
{
	ChatRoomMgr_S &roomMgr = g_ChatCenterS.GetRoomMgr();

	for(int nRoom = 0; nRoom < DEF_MAX_PLAYERCHATROOM; ++nRoom)
	{
		if(INVALID_ROOM_ID != m_JoinRoomIdCont[nRoom].roomId)
		{
			roomMgr.LeaveRoom(m_PlayerIdx, m_JoinRoomIdCont[nRoom].roomId);
		}
	}

	Init();
}

void ChatObjectMgr_S::LoadFriendsDataReq()
{
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = Player[m_PlayerIdx].GetNetConnectIdx( );
	DBHeader.ProcType = Proc_GetFriendData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_GETROLEDATA );
	pParam->Push( (char*)GetPlayerName(m_PlayerIdx) );
	pParam->Push( COLNAMEFRIED );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_role, pParam );
}

void ChatObjectMgr_S::SaveFriendsDataReq()
{
	char szBuffer[SAVETEMPBUFLEN];
	int nSize = SaveFriendsData( (BYTE*)szBuffer );
	
	if( nSize <= 0 || nSize > SAVETEMPBUFLEN )
		return;
	
	_DBProcHeader DBHeader = {0};
	DBHeader.ulNetID = Player[m_PlayerIdx].GetNetConnectIdx( );
	DBHeader.ProcType = Proc_SetFriendData;
	
	IProcParam* pParam = g_pController->GetProcParam( );
	
	pParam->BeginPush( PN_SETROLEDATA );
	pParam->Push( (char*)GetPlayerName(m_PlayerIdx) );
	pParam->Push( COLNAMEFRIED );
	pParam->Push( BinPair( szBuffer, nSize ) );
	pParam->EndPush( (char*)&DBHeader, sizeof(DBHeader) );
	
	g_pController->CallProc( cfs_db_cnn_role, pParam );
}

int ChatObjectMgr_S::LoadFriendsData(BYTE *pData,int nSize)
{
	int nRet = chat_err_none;
	
	BYTE * nVerson = pData;
	switch(*nVerson)
	{
	case 1:
		nRet = LoadFriendsDataV1(pData,nSize);
		break;
	case 2:
		nRet = LoadFriendsDataCurVersion(pData,nSize);
		RemoveAllObjectInGroup(GROUPID_ENEMY);
		break;
	case CUR_FRIDATA_VERSION:
		nRet = LoadFriendsDataCurVersion(pData,nSize);
		break;
	default:
		nRet = chat_err_loadfriendsdatafailed;
	    break;
	}

	if (nRet == chat_err_loadfriendsdatafailed )
		m_LoadSucess = false ;

	return nRet;
}

int	ChatObjectMgr_S::LoadFriendsDataCurVersion(BYTE *pData,int nSize)
{
	if (nSize < sizeof(DB_FRIENDS_DATA_HEADER))
		return chat_err_loadfriendsdatafailed;

	DBFriendsDataParser	parser((BYTE*)pData);

	PDB_FRIENDS_DATA_HEADER	pHeader = parser.GetHeader();
	m_bPreventRecvMsg = pHeader->bPreventRecvMsg ? true : false;
	m_bPreventSendMsg = pHeader->bPreventSendMsg ? true : false;
	int nGroupCount = pHeader->GroupCount;

	_ASSERT(pHeader->ObjectCount <= MAX_OBJECT_COUNT && pHeader->ObjectCount >= 0);
	_ASSERT(nGroupCount <= MAX_FRIENDGROUP_COUNT && nGroupCount >= 0);

	if( pHeader->ObjectCount < 0 || nGroupCount < 0 ||
		pHeader->ObjectCount > MAX_OBJECT_COUNT || nGroupCount > MAX_FRIENDGROUP_COUNT)
	{
		return chat_err_loadfriendsdatafailed;
	}

	if (nSize < sizeof(DB_FRIENDS_DATA_HEADER) + pHeader->ObjectCount * sizeof (CHATOBJECT) )
		return chat_err_loadfriendsdatafailed;

	int nGroupSize = nSize - sizeof(DB_FRIENDS_DATA_HEADER) - pHeader->ObjectCount * sizeof(CHATOBJECT);

	PDB_FRIENDSGROUP_DATA pGroupData = parser.GetFirstGroupData();

	for(int i = 0; i < nGroupCount && i < MAX_FRIENDGROUP_COUNT; ++i)
	{
		if (nGroupSize < sizeof (DB_FRIENDSGROUP_DATA) - 1)
			return chat_err_loadfriendsdatafailed;

		int nThisGroupSize = sizeof (DB_FRIENDSGROUP_DATA) + pGroupData->MemberCount * sizeof (CHATOBJECTNAME) - 1;
		if (nGroupSize < nThisGroupSize)
			return chat_err_loadfriendsdatafailed;

		nGroupSize -= nThisGroupSize;

		if(pGroupData->MemberCount > pHeader->ObjectCount)
		{
			_ASSERT(false);
			continue;
		}

		// Build-in group id is remain unchanged.
		DWORD	dwNewGroupId = (pGroupData->GroupId < DYNAMIC_GROUPID_BEGIN) ? pGroupData->GroupId : GetNewGroupId();

		m_GroupCont[i].Init(dwNewGroupId, pGroupData->szGroupName);

		// Because group id must be assigned on server, so here set this value
		// and then update it to client
		pGroupData->GroupId = dwNewGroupId;

		CHATOBJECTNAME* member = (CHATOBJECTNAME*)pGroupData->MemberName;
		for(int j = 0; j < pGroupData->MemberCount; ++j)
		{
			m_GroupCont[i].AddMember(member->name);
			member++;
		}

		pGroupData = parser.GetNextGroupData(pGroupData);
	}

	PCHATOBJECT	pObject = parser.GetObjectData();

	for(int nObjLoop = 0; nObjLoop < pHeader->ObjectCount && nObjLoop < MAX_OBJECT_COUNT; ++nObjLoop)
	{
		string strName(pObject[nObjLoop].name);
		bool bOnline = g_PlayerInfoToIndex.IsPlayerOnline(strName);
		ChatUtil::SetOnlineTag(pObject[nObjLoop].info.onlineTag, bOnline);
		//memcpy(&m_ObjectCont[nObjLoop], &pObject[nObjLoop], sizeof(CHATOBJECT));
	}

	memcpy(m_ObjectCont, pObject, pHeader->ObjectCount * sizeof(CHATOBJECT));

	return chat_err_none;	
}

int ChatObjectMgr_S::LoadFriendsDataV1(BYTE * pData,int nSize)
{
	if (nSize < sizeof(DB_FRIENDS_DATA_HEADERV1))
		return chat_err_loadfriendsdatafailed;

	DBFriendsDataParserV1	parser((BYTE*)pData);
	
	PDB_FRIENDS_DATA_HEADERV1	pHeader = parser.GetHeader();
	m_bPreventRecvMsg = pHeader->bPreventRecvMsg ? true : false;
	m_bPreventSendMsg = pHeader->bPreventSendMsg ? true : false;
	int nGroupCount = pHeader->GroupCount;
	
	_ASSERT(pHeader->ObjectCount <= MAX_OBJECT_COUNT && pHeader->ObjectCount >= 0);
	_ASSERT(nGroupCount <= MAX_FRIENDGROUP_COUNT && nGroupCount >= 0);
	
	if( pHeader->ObjectCount < 0 || nGroupCount < 0 ||
		pHeader->ObjectCount > MAX_OBJECT_COUNT || nGroupCount > MAX_FRIENDGROUP_COUNT)
	{
		return chat_err_loadfriendsdatafailed;
	}

	if (nSize < sizeof(DB_FRIENDS_DATA_HEADERV1) + pHeader->ObjectCount * sizeof(CHATOBJECTV1))
		return chat_err_loadfriendsdatafailed;

	int nGroupSize = nSize - sizeof(DB_FRIENDS_DATA_HEADERV1) - pHeader->ObjectCount * sizeof(CHATOBJECTV1);

	PDB_FRIENDSGROUP_DATAV1 pGroupData = parser.GetFirstGroupData();
	
	for(int i = 0; i < nGroupCount && i < MAX_FRIENDGROUP_COUNT; ++i)
	{
		if (nGroupSize < sizeof(DB_FRIENDSGROUP_DATAV1) - 1 )
			return chat_err_loadfriendsdatafailed;

		int nThisGroupSize = sizeof (DB_FRIENDSGROUP_DATAV1) + pGroupData->MemberCount * sizeof (CHATOBJECTNAMEV1) - 1;

		if (nGroupSize < nThisGroupSize)
			return chat_err_loadfriendsdatafailed;

		nGroupSize -= nThisGroupSize;

		if(pGroupData->MemberCount > pHeader->ObjectCount)
		{
			_ASSERT(false);
			continue;
		}
		
		// Build-in group id is remain unchanged.
		DWORD	dwNewGroupId = (pGroupData->GroupId < DYNAMIC_GROUPID_BEGIN) ? pGroupData->GroupId : GetNewGroupId();
		
		m_GroupCont[i].Init(dwNewGroupId, pGroupData->szGroupName);
		
		// Because group id must be assigned on server, so here set this value
		// and then update it to client
		pGroupData->GroupId = dwNewGroupId;
		
		CHATOBJECTNAMEV1* member = (CHATOBJECTNAMEV1*)pGroupData->MemberName;
		for(int j = 0; j < pGroupData->MemberCount; ++j)
		{
			m_GroupCont[i].AddMember(member->name);
			member++;
		}
		
		pGroupData = parser.GetNextGroupData(pGroupData);
	}
	
	PCHATOBJECTV1	pObject = parser.GetObjectData();
	
	for(int nObjLoop = 0; nObjLoop < pHeader->ObjectCount && nObjLoop < MAX_OBJECT_COUNT; ++nObjLoop)
	{
		string strName(pObject[nObjLoop].name);
		bool bOnline = g_PlayerInfoToIndex.IsPlayerOnline(strName);
		ChatUtil::SetOnlineTag(pObject[nObjLoop].info.onlineTag, bOnline);

		//Notice: This place may need be update when new verion is comming !!!!!!!
		strncpy(m_ObjectCont[nObjLoop].name,pObject[nObjLoop].name,sizeof(m_ObjectCont[nObjLoop].name));
		m_ObjectCont[nObjLoop].name[MAXSIZE_ROLENAME - 1] = 0 ;
		m_ObjectCont[nObjLoop].info.bPreventRecvMyMsg     = pObject[nObjLoop].info.bPreventRecvMyMsg;
		m_ObjectCont[nObjLoop].info.bPreventSendMsgToMe   = pObject[nObjLoop].info.bPreventSendMsgToMe;
		m_ObjectCont[nObjLoop].info.relation              = pObject[nObjLoop].info.relation;
		m_ObjectCont[nObjLoop].info.onlineTag             = pObject[nObjLoop].info.onlineTag;
		m_ObjectCont[nObjLoop].info.groupId               = pObject[nObjLoop].info.groupId;
	}
	
	return chat_err_none;	
}

int ChatObjectMgr_S::SaveFriendsData(BYTE *pData)
{
	if (!m_LoadSucess) //防止当读取有问题时存入无效数据，保护数据库中的数据现场供分析与恢复
		return 0 ;

	int	nUsedSize = 0;

	DBFriendsDataParser	parser(pData);

	PDB_FRIENDS_DATA_HEADER	pHeader = parser.GetHeader();
	pHeader->Version		 = CUR_FRIDATA_VERSION;
	pHeader->bPreventSendMsg = m_bPreventSendMsg;
	pHeader->bPreventRecvMsg = m_bPreventRecvMsg;
	pHeader->ObjectCount = 0;
	pHeader->GroupCount = 0;

	nUsedSize += sizeof(DB_FRIENDS_DATA_HEADER);

	PCHATOBJECT	pObjectData = parser.GetObjectData();

	for(int j = 0; j < MAX_OBJECT_COUNT; ++j)
	{
		if( IsObjValid(m_ObjectCont[j]) )
		{
			memcpy(pObjectData, &m_ObjectCont[j], sizeof(CHATOBJECT));
			nUsedSize += sizeof(CHATOBJECT);
			++pHeader->ObjectCount;
			++pObjectData;
		}
	}

	PDB_FRIENDSGROUP_DATA	pGroupData = parser.GetFirstGroupData();

	for(int i = 0; i < MAX_FRIENDGROUP_COUNT; ++i)
	{
		if( INVALID_GROUP_ID != m_GroupCont[i].GetGroupId() )
		{
			nUsedSize += m_GroupCont[i].SaveGroupData(pGroupData);
			pGroupData = parser.GetNextGroupData(pGroupData);
			++pHeader->GroupCount;
		}
	}

	return nUsedSize;
}	

bool ChatObjectMgr_S::HasGroup(const char *szGroupName)
{
	if(szGroupName)
	{
		for(int i = 0; i < MAX_FRIENDGROUP_COUNT; ++i)
		{
			if( !strcmp(szGroupName, m_GroupCont[i].GetGroupName()) )
			{
				return true;
			}
		}
	}

	return false;
}

PCHATOBJECT	ChatObjectMgr_S::GetChatObject(const char* szPlayerName)
{
	for(int i = 0; i < MAX_OBJECT_COUNT; ++i)
	{
		if(strcmp(szPlayerName, m_ObjectCont[i].name) == 0)
		{
			return &m_ObjectCont[i];
		}
	}

	return NULL;
}

ObjectGroup*	ChatObjectMgr_S::GetGroup(DWORD dwId)
{
	if(INVALID_GROUP_ID == dwId)
		return NULL;

	for(int i = 0; i < MAX_FRIENDGROUP_COUNT; ++i)
	{
		if( m_GroupCont[i].GetGroupId() == dwId )
		{
			return &m_GroupCont[i];
		}
	}

	return NULL;	
}

ObjectGroup*	ChatObjectMgr_S::GetEmptyGroup()
{
	for(int i = 0; i < MAX_FRIENDGROUP_COUNT; ++i)
	{
		if( m_GroupCont[i].GetGroupId() == INVALID_GROUP_ID )
		{
			return &m_GroupCont[i];
		}
	}

	return NULL;	
}

int ChatObjectMgr_S::ChangeGroup(const char* szPlayerName, DWORD dwOldGroupId, DWORD dwNewGroupId)
{
	int nRet =  chat_err_none;
	
	PCHATOBJECT	pObject = GetChatObject(szPlayerName);

	if (dwOldGroupId == dwNewGroupId)
		return  nRet;

	if(NULL == pObject)
	{
		nRet = char_err_membernotexist;
	}
	else
	{
		ObjectGroup	*pOldGroup = GetGroup(dwOldGroupId);
		ObjectGroup *pNewGroup = GetGroup(dwNewGroupId);

		if(pOldGroup && pNewGroup)
		{
			pOldGroup->RemoveMember(pObject->name);
			pNewGroup->AddMember(pObject->name);
			pObject->info.groupId = dwNewGroupId;	

			if(GROUPID_BLACK == dwOldGroupId)
				pObject->info.bPreventSendMsgToMe = false;
			
			if(GROUPID_BLACK == dwNewGroupId /*|| GROUPID_ENEMY == dwNewGroupId*/)
				pObject->info.bPreventSendMsgToMe = true;
			

			CHAT_CHANGEGROUP_RET ret;
			ret.protocol.protocol = s2c_chat_family;
			ret.protocol.subProtocol = chat_changegroup;
			ret.protocol.len = sizeof(ret) - PROTOCOL_SIZE;
			ret.dwOldGroupId = dwOldGroupId;
			ret.dwNewGroupId = dwNewGroupId;
			strncpy(ret.szPlayerName, pObject->name, sizeof(ret.szPlayerName));

			SendDataToClient(m_PlayerIdx, &ret, ret.protocol.len + PROTOCOL_SIZE);
		}
		else
		{
			nRet = char_err_groupnotexist;
		}
	}

	return nRet;
}

int ChatObjectMgr_S::RenameGroup(DWORD dwGroupId, const char *szGroupName)
{
	int	nRet = chat_err_none;

	ObjectGroup	*pGroup = GetGroup(dwGroupId);

	if(pGroup)
	{
		pGroup->SetGroupName(szGroupName);
		
		CHAT_RENAME_GROUP	ret;
		ret.protocol.protocol = s2c_chat_family;
		ret.protocol.subProtocol = chat_renamegroup;
		ret.protocol.len = sizeof(ret) - PROTOCOL_SIZE;
		ret.dwGroupId = dwGroupId;
		strncpy(ret.name, szGroupName, sizeof(ret.name));

		SendDataToClient(m_PlayerIdx, &ret, ret.protocol.len + PROTOCOL_SIZE);
	}
	else
	{
		nRet = char_err_groupnotexist;
	}

	return nRet;
}

int ChatObjectMgr_S::RemoveObject(const char* szPlayerName)
{
	int nRet = chat_err_none;

	PCHATOBJECT	pObject = GetChatObject(szPlayerName);

	if(pObject)
	{
		ObjectGroup	*pGroup = GetGroup(pObject->info.groupId);

		if(pGroup)
		{
			pGroup->RemoveMember(pObject->name);

			CHAT_REMOVEOBJECT_RET	ret;
			ret.protocol.protocol = s2c_chat_family;
			ret.protocol.subProtocol = chat_rmobject;
			ret.protocol.len = sizeof(ret) - PROTOCOL_SIZE;
			strncpy(ret.szName, pObject->name, sizeof(ret.szName));
			
			SendDataToClient(m_PlayerIdx, &ret, ret.protocol.len + PROTOCOL_SIZE);

			MakeInvalid(pObject);
		}
		else
		{
			_ASSERT(false);
			nRet = char_err_groupnotexist;
		}
	}
	else
	{
		nRet = char_err_membernotexist;
	}

	return nRet;
}

int ChatObjectMgr_S::RemoveAllObjectInGroup(DWORD dwGroupId)
{
	int    nRet = chat_err_none;
 
	ObjectGroup	*pObjGrp = GetGroup(dwGroupId);

	if (pObjGrp == NULL)
		return char_err_groupnotexist;

	for(int j = 0; j < MAX_OBJECT_COUNT; ++j)
	{
		if( IsObjValid(m_ObjectCont[j]) && m_ObjectCont[j].info.groupId == dwGroupId)
		{
			pObjGrp->RemoveMember(m_ObjectCont[j].name);
			MakeInvalid(&m_ObjectCont[j]);
		}//endif

	}//end for j

	return nRet;
}

int ChatObjectMgr_S::DeleteGroup(int nPlayerIdx, DWORD dwGroupId)
{
	int nRet = chat_err_none;

	ObjectGroup	*pObjGrp = GetGroup(dwGroupId);

	if(pObjGrp)
	{
		if(pObjGrp->GetMemberCount() > 0)
		{
			nRet = char_err_groupnotempty;
		}
		else
		{
			pObjGrp->Init(INVALID_GROUP_ID, "");

			CHAT_DELETE_GROUP	ret;
			ret.protocol.protocol = s2c_chat_family;
			ret.protocol.subProtocol = chat_deletegroup;
			ret.protocol.len = sizeof(ret) - PROTOCOL_SIZE;
			ret.dwGroupId = dwGroupId;

			SendDataToClient(nPlayerIdx, &ret, ret.protocol.len + PROTOCOL_SIZE);
		}
	}
	else
	{
		nRet = char_err_groupnotexist;
	}

	return nRet;
}

// Assistant functions

int	ChatObjectMgr_S::CanCreateRoom()
{
	if(m_bPreventSendMsg || m_bPreventRecvMsg)
	{
		return chat_err_noaccesscreateroom;
	}

	if( IsRoomFull() )
	{
		return chat_err_maxroom;
	}

	return chat_err_none;
}

int	ChatObjectMgr_S::CanBeAddToRoom(const string &strSender)
{
	if(m_bPreventRecvMsg)
	{
		return chat_err_denyaddmembertoroom;
	}

	if( IsPreventSendMsgToMe(strSender.c_str()) )
	{
		return chat_err_denyaddmembertoroom;
	}

	if( IsRoomFull() )
	{
		return chat_err_memberroomfull;
	}

	return chat_err_none;
}

void ChatObjectMgr_S::MarkPlayerLevelAndSeries(int nPlayerIdx,BYTE & nLevelRet,BYTE & nSeriesRet)
{
	nLevelRet   = 0;
	nSeriesRet  = 0;

	if (IsValidPlayer(nPlayerIdx))
	{
		int iSeries =Npc[Player[nPlayerIdx].m_nIndex].m_Series;
		int iLevel  =Player[nPlayerIdx].GetLevel();
	
		nLevelRet  =  (BYTE) iLevel;
	    nSeriesRet =  (BYTE) iSeries;
	}//endif

}

void ChatObjectMgr_S::AddFriendToGroup(DWORD dwPlayerIndex,DWORD dwGroupId)
{
	if (IsValidPlayer(dwPlayerIndex))
	{
		if(Npc[Player[dwPlayerIndex].m_nIndex].GetComoflag() != 0)
			return;

		const char* szPlayerName = GetPlayerName(dwPlayerIndex);
		
		ObjectGroup* pDestGroup = GetGroup(dwGroupId);
		
        if (pDestGroup)
		{
			PCHATOBJECT pObject = GetChatObject(szPlayerName);
			
			if(pObject)
			{
				ChangeGroup(szPlayerName, pObject->info.groupId, dwGroupId);
			}
			else
			{
				
				PCHATOBJECT	pObj = GetEmptyObject();
				
				if(NULL == pObj)  //max friend ... ...
					return ;
				
				pObj->info.bPreventRecvMyMsg = false;
				pObj->info.bPreventSendMsgToMe = false;
				
				ChatUtil::SetOnlineTag(pObj->info.onlineTag, true);
				
				pObj->info.relation = (BYTE)PR_FRIEND;
				pObj->info.groupId =  dwGroupId;

				strncpy(pObj->name, Player[dwPlayerIndex].GetPlayerName(), sizeof(pObj->name));
				
				pDestGroup->AddMember(pObj->name);
				
				CHAT_ADDOBJECT_NOTIFY	data;
				data.protocol.protocol = s2c_chat_family;
				data.protocol.subProtocol = chat_notifyobjectid;
				data.groupId = pObj->info.groupId;
				data.relation = pObj->info.relation;
				data.bOnline = ChatUtil::IsOnline(pObj->info.onlineTag);
				strncpy(data.name, pObj->name, sizeof(data.name));
				MarkPlayerLevelAndSeries(dwPlayerIndex,data.nLevel,data.nSeries);
				data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
				SendDataToClient(m_PlayerIdx, &data, data.protocol.len + PROTOCOL_SIZE);

			}//end else

		}//endif

	}//endif
}

bool ChatObjectMgr_S::IsRoomFull()
{
	for(int i = 0; i < DEF_MAX_PLAYERCHATROOM; ++i)
	{
		if(m_JoinRoomIdCont[i].roomId == INVALID_ROOM_ID)
		{
			return false;
		}
	}

	return true;
}

int ChatObjectMgr_S::JoinRoom(DWORD dwRoomId)
{
	for(int i = 0; i < DEF_MAX_PLAYERCHATROOM; ++i)
	{
		if(INVALID_ROOM_ID == m_JoinRoomIdCont[i].roomId)
		{
			m_JoinRoomIdCont[i].roomId = dwRoomId;
			m_JoinRoomIdCont[i].canSendMsg = 1;
			return chat_err_none;
		}
	}

	return chat_err_memberroomfull;
}

int	ChatObjectMgr_S::CanChatInRoom(DWORD dwRoomId)
{
	if( IsPreventSendMsg() )
		return chat_err_noaccesstosend;

	// check chat interval time
	static ConfigManager &cfg = ConfigManager::Singleton();
	if(dwRoomId == GLOBAL_ROOM_ID)
	{
		// check chat interval time in global room 
		DWORD dwTimeInterval = cfg.GetGlobalVariable(global_var_globalroomchat_timeinterval);
		DWORD dwCurTime = UNIX_TMIE_STAMP;
		DWORD dwPreChatTime = m_PreChatTimeInGlobalRoom;

		if(dwCurTime - dwPreChatTime < dwTimeInterval)
			return chat_err_chattoofast;
		
		m_PreChatTimeInGlobalRoom = dwCurTime;

		// check player level
		if (IsValidPlayer(m_PlayerIdx))
		{
			int nLevel = GetPlayerLevel(m_PlayerIdx);
			if (nLevel < MIN_CHAT_LEVEL_IN_GLOBAL_ROOM)
			{
				return chat_err_level_invalid;
			}//endif

		}//endif
		
	}
	else if(dwRoomId == MAP_ROOM_ID)
	{
		// check chat interval time in global room 
		DWORD dwTimeInterval = cfg.GetGlobalVariable(global_var_maproomchat_timeinterval);
		if (IsValidPlayer(m_PlayerIdx))
		{
			int nNpcIndex = Player[m_PlayerIdx].GetNpcIndex();
			if (IsValidNpc(nNpcIndex))
			{
				int nSubworldIndex = Npc[nNpcIndex].GetSubWorldIndex();
				if ( nSubworldIndex >= 0 && nSubworldIndex < MAX_SUBWORLD)
				{
					dwTimeInterval = SubWorld[nSubworldIndex].GetMapChatInterval();
				}//endif

			}//endif

		}//endif

		DWORD dwCurTime = UNIX_TMIE_STAMP;
		DWORD dwPreChatTime = m_PreChatTimeInMapRoom;

		if(dwCurTime - dwPreChatTime < dwTimeInterval)
			return chat_err_chattoofast;
		
		m_PreChatTimeInMapRoom = dwCurTime;

		// check player level
		if (IsValidPlayer(m_PlayerIdx))
		{
			int nLevel = GetPlayerLevel(m_PlayerIdx);
			if (nLevel < MIN_CHAT_LEVEL_IN_MAP_ROOM)
			{
				return chat_err_level_invalid;
			}//endif
			
		}//endif
	}
	else if(dwRoomId == LOCAL_ROOM_ID)
	{
		// check chat interval time in local room 
		DWORD dwTimeInterval = cfg.GetGlobalVariable(global_var_localroomchat_timeinterval);
		DWORD dwCurTime = UNIX_TMIE_STAMP;
		DWORD dwPreChatTime = m_PreChatTimeInLocalRoom;

		if(dwCurTime - dwPreChatTime < dwTimeInterval)
			return chat_err_chattoofast;
		
		m_PreChatTimeInLocalRoom = dwCurTime;
	}
	
	if(dwRoomId <= DYNAMIC_ROOMID_BEGIN)
		return chat_err_none;

	for(int nRoom = 0; nRoom < DEF_MAX_PLAYERCHATROOM; ++nRoom)
	{
		if(m_JoinRoomIdCont[nRoom].roomId == dwRoomId)
		{
			if(m_JoinRoomIdCont[nRoom].canSendMsg)
				return chat_err_none;
			else
				break;
		}
	}

	return chat_err_noaccesstosend;
}

int ChatObjectMgr_S::ChgRoomChatPrivilege(DWORD dwRoomId, bool bCanChat)
{
	for(int nRoom = 0; nRoom < DEF_MAX_PLAYERCHATROOM; ++nRoom)
	{
		if(m_JoinRoomIdCont[nRoom].roomId == dwRoomId)
		{
			m_JoinRoomIdCont[nRoom].canSendMsg = bCanChat;
			return chat_err_none;
		}
	}

	return char_err_chgchatprivfailed;
}

void ChatObjectMgr_S::LeaveRoom(DWORD dwRoomId)
{
	for(int i = 0; i < DEF_MAX_PLAYERCHATROOM; ++i)
	{
		if(m_JoinRoomIdCont[i].roomId == dwRoomId)
		{
			m_JoinRoomIdCont[i].roomId = INVALID_ROOM_ID;
			break;
		}
	}
}

// Functions process package from client

int ChatObjectMgr_S::AddObject(int nSenderIdx, CHAT_ADDOBJECT_REQ *pc2sReq)
{	
	int playerIdx = g_PlayerInfoToIndex.GetIndexByName(pc2sReq->name);
	
	if(IsValidPlayer(playerIdx) && Npc[Player[playerIdx].m_nIndex].GetComoflag() != 0)
	{
		return chat_err_playerbecamou;
	}
	
	if( 0 == strcmp(GetPlayerName(nSenderIdx), pc2sReq->name) )
	{
		return chat_err_cantaddself;
	}
	
	PCHATOBJECT pObject = GetChatObject(pc2sReq->name);
	
	if(pObject)
	{
		return chat_err_objalreadyexist;
	}
	
	PCHATOBJECT	pObj = GetEmptyObject();
	
	if(NULL == pObj)
		return chat_err_maxfriend;
	
	ObjectGroup *pGroup = GetGroup(pc2sReq->groupId);
	
	if(NULL == pGroup)
		return char_err_groupnotexist;
	
	pObj->info.bPreventRecvMyMsg = false;
	
	if(GROUPID_BLACK == pc2sReq->groupId)
		pObj->info.bPreventSendMsgToMe = true;
	else
		pObj->info.bPreventSendMsgToMe = false;
	
	int nTargetIdx = playerIdx;
	bool bOnline   = (INVALID_PLAYER_INDEX != nTargetIdx);
	ChatUtil::SetOnlineTag(pObj->info.onlineTag, bOnline);
	
	pObj->info.relation = (BYTE)pc2sReq->relation;
	pObj->info.groupId = pc2sReq->groupId;
	strncpy(pObj->name, pc2sReq->name, sizeof(pObj->name));
	
	pGroup->AddMember(pObj->name);
	
	CHAT_ADDOBJECT_NOTIFY	data;
	data.protocol.protocol = s2c_chat_family;
	data.protocol.subProtocol = chat_notifyobjectid;
	data.groupId = pObj->info.groupId;
	data.relation = pObj->info.relation;
	data.bOnline = bOnline;
	strncpy(data.name, pObj->name, sizeof(data.name));
	MarkPlayerLevelAndSeries(playerIdx,data.nLevel,data.nSeries);
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
	SendDataToClient(nSenderIdx, &data, data.protocol.len + PROTOCOL_SIZE);
	
	return chat_err_none;
}

int	ChatObjectMgr_S::ChangeRelation(int nSenderIdx, const char* szPlayerName, int nRelation)
{
	if(nRelation <= PR_BEGIN || nRelation >= PR_END || PR_SYSTEM == nRelation)
	{
		return char_err_invalidrelation;
	}

	PCHATOBJECT	 pObject = GetChatObject(szPlayerName);

	if(pObject)
	{
		pObject->info.relation = (BYTE)nRelation;

		CHAT_CHANGERELATION_RET	ret;
		ret.protocol.protocol = s2c_chat_family;
		ret.protocol.subProtocol = chat_changerelation;
		ret.protocol.len = sizeof(ret) - PROTOCOL_SIZE;
		strncpy(ret.szName, pObject->name, sizeof(ret.szName));

		SendDataToClient(nSenderIdx, &ret, ret.protocol.len + PROTOCOL_SIZE);
	}
	else
	{
		return chat_err_playernotexist;
	}

	return chat_err_none;
}

int	ChatObjectMgr_S::CreateGroup(int nSenderIdx, const char *szGroupName)
{
	int nNameLen = szGroupName ? strlen(szGroupName) : 0;

	if(nNameLen >= MAXLEN_GROUPNAME || nNameLen < MINLEN_GROUPNAME)	
	{
		return chat_err_groupnamelengtherr;
	}
	
	if( HasGroup(szGroupName) )
	{
		return chat_err_groupexist;
	}

	ObjectGroup *pGroup = GetEmptyGroup();

	if(NULL == pGroup)
	{
		return chat_err_exceedmaxgroup;
	}

	DWORD	dwGroupId = GetNewGroupId();
	pGroup->Init(dwGroupId, szGroupName);

	SendCreateGroupNotify(nSenderIdx, dwGroupId, szGroupName);

	return chat_err_none;
}

void ChatObjectMgr_S::SendCreateGroupNotify(int nPlayerIdx, DWORD dwGroupId, const char *szGroupName)
{
	CHAT_CREATEGROUP_RST  data;
	data.protocol.protocol = s2c_chat_family;
	data.protocol.subProtocol = chat_creategroup;
	data.dwId = dwGroupId;
	strncpy(data.name, szGroupName, sizeof(data.name));
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
	SendDataToClient(nPlayerIdx, &data, data.protocol.len + PROTOCOL_SIZE);	
}

int ChatObjectMgr_S::SendFriendsDataToClient(BYTE * pData)
{
	//Notice this function is used by current verion DB structrue
	CHAT_LIST_FRIEND_RET * pRetMsg = (CHAT_LIST_FRIEND_RET *)pData;
	pRetMsg->protocol.protocol     = s2c_chat_family;
	pRetMsg->protocol.subProtocol  = chat_loadfriendsdata;

	int	nUsedSize = 0;

	DBFriendsDataParser	parser(pRetMsg->data);
	
	PDB_FRIENDS_DATA_HEADER	pHeader = parser.GetHeader();
	pHeader->Version		 = CUR_FRIDATA_VERSION;
	pHeader->bPreventSendMsg = m_bPreventSendMsg;
	pHeader->bPreventRecvMsg = m_bPreventRecvMsg;
	pHeader->ObjectCount = 0;
	pHeader->GroupCount = 0;
	
	nUsedSize += sizeof(DB_FRIENDS_DATA_HEADER);
	
	PS2C_CHAT_OBJ_INFO pObjectData = (PS2C_CHAT_OBJ_INFO) parser.GetObjectData();
	
	for(int j = 0; j < MAX_OBJECT_COUNT; ++j)
	{
		if( IsObjValid(m_ObjectCont[j]) )
		{	
			strncpy(pObjectData->name,m_ObjectCont[j].name,sizeof(pObjectData->name));
			pObjectData->bPreventRecvMyMsg   = m_ObjectCont[j].info.bPreventRecvMyMsg;
			pObjectData->bPreventSendMsgToMe = m_ObjectCont[j].info.bPreventSendMsgToMe;
			pObjectData->groupId             = m_ObjectCont[j].info.groupId;
			pObjectData->onlineTag           = m_ObjectCont[j].info.onlineTag;
			pObjectData->relation            = m_ObjectCont[j].info.relation;
			pObjectData->nLevel  = 0;
			pObjectData->nSeries = 0;
			
			if (ChatUtil::IsOnline(m_ObjectCont[j].info.onlineTag))
			{
				int nPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(m_ObjectCont[j].name);
				MarkPlayerLevelAndSeries(nPlayerIndex,pObjectData->nLevel,pObjectData->nSeries);
			}//endif

			nUsedSize += sizeof(S2C_CHAT_OBJ_INFO);
			++pObjectData;
			++pHeader->ObjectCount;
		}//endif
	}
	
	PDB_FRIENDSGROUP_DATA	pGroupData = (PDB_FRIENDSGROUP_DATA)(pRetMsg->data + sizeof(DB_FRIENDS_DATA_HEADER) + pHeader->ObjectCount * sizeof(S2C_CHAT_OBJ_INFO) );
	
	for(int i = 0; i < MAX_FRIENDGROUP_COUNT; ++i)
	{
		if( INVALID_GROUP_ID != m_GroupCont[i].GetGroupId() )
		{
			nUsedSize += m_GroupCont[i].SaveGroupData(pGroupData);
			pGroupData = parser.GetNextGroupData(pGroupData);
			++pHeader->GroupCount;
		}
	}

	pRetMsg->protocol.len = nUsedSize + sizeof(CHAT_LIST_FRIEND_RET) - 1 - PROTOCOL_SIZE;
	//Compression package..............................................................................
	unsigned char szBuff[SAVETEMPBUFLEN];
	unsigned int nLen = SAVETEMPBUFLEN;
	lzo1x_1_compress( 
		pRetMsg->data,
		nUsedSize,
		szBuff,
		&nLen,
		wrkmem);
	
	if (nLen >= SAVETEMPBUFLEN - sizeof(CHAT_LIST_FRIEND_RET) )
		return chat_err_sendbufoverflow;
	
	memcpy(pRetMsg->data,szBuff,nLen);          
	pRetMsg->protocol.len = nLen + sizeof(CHAT_LIST_FRIEND_RET) - 1 - PROTOCOL_SIZE;
	//Compression end....................................................................................
	SendDataToClient(m_PlayerIdx, pData, pRetMsg->protocol.len + PROTOCOL_SIZE);
	
	return chat_err_none;
}

int	ChatObjectMgr_S::LoadFriendsDataRet(BYTE *pMsg, int nSize)
{
	if(0 == nSize)
	{
		// The following three groups must exist,
		// here check and create if not exist when the player 
		// load friends data first time.
		CreateInnerGroup(GROUPID_NONE, DEF_NONEGROUP_NAME);
		CreateInnerGroup(GROUPID_BLACK, DEF_BLACKGROUP_NAME);
		CreateInnerGroup(GROUPID_ENEMY, DEF_ENEMYGROUP_NAME);

		return chat_err_none;
	}
	
	int nRet = LoadFriendsData(pMsg,nSize);
	
	if(chat_err_none == nRet)
	{
		char szNewBuffer[SAVETEMPBUFLEN] = "";
		nRet = SendFriendsDataToClient((BYTE *)szNewBuffer);
	}

	return nRet;
}

void ChatObjectMgr_S::CreateInnerGroup(DWORD dwGroupId, const char *szName)
{
	if( NULL == GetGroup(dwGroupId) )
	{
		ObjectGroup *pGroup = GetEmptyGroup();

		if(pGroup)
		{
			pGroup->Init(dwGroupId, szName);
			SendCreateGroupNotify(m_PlayerIdx, dwGroupId, szName);
		}
		else
			_ASSERT(NULL != pGroup);
	}
}

#define  MAX_CHECK_SEARCH_CONT 16

void ChatObjectMgr_S::CheckOnlineState(int nObjIndex)
{
	_ASSERT(nObjIndex >= 0 && nObjIndex < MAX_OBJECT_COUNT );
	if (nObjIndex < 0 || nObjIndex >= MAX_OBJECT_COUNT)
		return ;

	PCHATOBJECT pObj        = &m_ObjectCont[nObjIndex];
	int       nTargetIndex  = g_PlayerInfoToIndex.GetIndexByName(pObj->name);
	bool      bOnline       = nTargetIndex != INVALID_PLAYER_INDEX;

	if(NULL != pObj)
	{
		if( bOnline != ChatUtil::IsOnline(pObj->info.onlineTag) )
		{
			ChatUtil::SetOnlineTag(pObj->info.onlineTag, bOnline);
			
			CHAT_ONLINESTATUS_NOTIFY s2cNotify;
			s2cNotify.protocol.protocol = s2c_chat_family;
			s2cNotify.protocol.subProtocol = chat_onlinestatusnotify;
			s2cNotify.protocol.len = sizeof(s2cNotify) - PROTOCOL_SIZE;
			strncpy(s2cNotify.playerName, pObj->name, sizeof(s2cNotify.playerName));
			s2cNotify.bOnline = bOnline;

			s2cNotify.nLevel  = 0;
			s2cNotify.nSeries = 0;

			if (bOnline)
			{
				int nPlayerIndex = nTargetIndex;
				MarkPlayerLevelAndSeries(nPlayerIndex,s2cNotify.nLevel,s2cNotify.nSeries);
			}//endif

			SendDataToClient(m_PlayerIdx, &s2cNotify, s2cNotify.protocol.len + PROTOCOL_SIZE);

		}//endif

	}//endif
}

void ChatObjectMgr_S::CheckFriendOnlineFlag()
{
	if (m_CheckOnlineIter < MAX_OBJECT_COUNT && m_CheckOnlineIter >= 0 )
	{
		int  nMaxCheckSearchCount = MAX_CHECK_SEARCH_CONT;

		bool bSearched = false;

		while (nMaxCheckSearchCount > 0)
		{
			if ( m_ObjectCont[m_CheckOnlineIter].name[0] != 0)
			{
				CheckOnlineState(m_CheckOnlineIter);
				bSearched = true;
			}//endif

			m_CheckOnlineIter    ++;
			m_CheckOnlineIter    = m_CheckOnlineIter % MAX_OBJECT_COUNT;

			if (bSearched)
				break;

			nMaxCheckSearchCount -- ;
		}//endif

	}//endif
	else
	{
		_ASSERT(FALSE);
	}

}

void ChatObjectMgr_S::SetObjOnlineState(const char* szPlayerName, bool bOnline)
{
	PCHATOBJECT pObj = GetChatObject(szPlayerName);

	if(NULL != pObj)
	{
		if( bOnline != ChatUtil::IsOnline(pObj->info.onlineTag) )
		{
			ChatUtil::SetOnlineTag(pObj->info.onlineTag, bOnline);
			
			CHAT_ONLINESTATUS_NOTIFY s2cNotify;
			s2cNotify.protocol.protocol = s2c_chat_family;
			s2cNotify.protocol.subProtocol = chat_onlinestatusnotify;
			s2cNotify.protocol.len = sizeof(s2cNotify) - PROTOCOL_SIZE;
			strncpy(s2cNotify.playerName, szPlayerName, sizeof(s2cNotify.playerName));
			s2cNotify.bOnline = bOnline;

			s2cNotify.nLevel  = 0;
			s2cNotify.nSeries = 0;

			if (bOnline)
			{
				int nPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(szPlayerName);
				MarkPlayerLevelAndSeries(nPlayerIndex,s2cNotify.nLevel,s2cNotify.nSeries);
			}//endif

			SendDataToClient(m_PlayerIdx, &s2cNotify, s2cNotify.protocol.len + PROTOCOL_SIZE);

			//PkValue Sync
			/*if (bOnline && pObj->info.groupId == GROUPID_ENEMY)
			{
				int      nPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(szPlayerName);
				
				if ( IsValidPlayer(nPlayerIndex))
				{
					CHAT_PKCHANGE_NOTIFY s2cNotify;
					s2cNotify.protocol.protocol    = s2c_chat_family;
					s2cNotify.protocol.subProtocol = chat_pkvaluechange;
					s2cNotify.protocol.len = sizeof(s2cNotify) - PROTOCOL_SIZE;
					strncpy(s2cNotify.playerName, szPlayerName, sizeof(s2cNotify.playerName));
					s2cNotify.pkValue  = Player[nPlayerIndex].GetPkValue();
					
					SendDataToClient(m_PlayerIdx, &s2cNotify, s2cNotify.protocol.len + PROTOCOL_SIZE);
				}//endif

			}//endif
			*/

		}//endif

	}
}
/*
void ChatObjectMgr_S::ObjPkValueChangeNotify(const char* szPlayerName)
{
   if (NULL == szPlayerName)
	   return ;
  
   PCHATOBJECT	pObj     = GetChatObject(szPlayerName);
   if (NULL == pObj)
	   return;
   
   int      nPlayerIndex = g_PlayerInfoToIndex.GetIndexByName(pObj->name);
   
   if (pObj && pObj->info.groupId == GROUPID_ENEMY && IsValidPlayer(nPlayerIndex))
   {
       CHAT_PKCHANGE_NOTIFY s2cNotify;
	   s2cNotify.protocol.protocol = s2c_chat_family;
	   s2cNotify.protocol.subProtocol = chat_pkvaluechange;
	   s2cNotify.protocol.len = sizeof(s2cNotify) - PROTOCOL_SIZE;
	   strncpy(s2cNotify.playerName, szPlayerName, sizeof(s2cNotify.playerName));
	   s2cNotify.pkValue  = Player[nPlayerIndex].GetPkValue();
	   
	   SendDataToClient(m_PlayerIdx, &s2cNotify, s2cNotify.protocol.len + PROTOCOL_SIZE);
   }//endif

}
*/

PCHATOBJECT ChatObjectMgr_S::GetEmptyObject()
{
	for(int nObj = 0; nObj < MAX_OBJECT_COUNT; ++nObj)
	{
		if( !IsObjValid(m_ObjectCont[nObj]) )
			return &m_ObjectCont[nObj];
	}

	return NULL;
}