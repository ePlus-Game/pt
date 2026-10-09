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

#include "ChatObjectMgr_C.h"
#include "CoreRelated.h"
#include "ChatDataDef.h"

void ChatObjectMgr_C::Init()
{
	m_bPreventSendMsg = false;
	m_bPreventRecvMsg = false;
	m_PreChatTimeInLocalRoom = 0;

	m_GroupCont.clear();
	m_ObjNameToInfo.clear();

	CreateGroup(GROUPID_TEMP, DEF_TEMPGROUP_NAME);
}

bool ChatObjectMgr_C::HasGroup(const string &strName)
{
	MAPIDTOGROUP::iterator	itGroup;
	MAPIDTOGROUP::iterator	itEnd = m_GroupCont.end();

	for(itGroup = m_GroupCont.begin(); itGroup != itEnd; ++itGroup)
	{
		if(strName == itGroup->second.GetGroupName())
		{
			return true;
		}
	}

	return false;
}

ObjectGroup* ChatObjectMgr_C::GetGroup(const string &strName)
{
	MAPIDTOGROUP::iterator itGroup;
	MAPIDTOGROUP::iterator itEnd = m_GroupCont.end();

	for(itGroup = m_GroupCont.begin(); itGroup != itEnd; ++itGroup)
	{
		if( strName == itGroup->second.GetGroupName() )
		{
			return &(itGroup->second);
		}
	}

	return NULL;
}

int ChatObjectMgr_C::ChangeGroupReq(const string &strName, DWORD dwOldGroupId, DWORD dwNewGroupId)
{
	int nRet =  chat_err_none;

	PCHATOBJECT_INFO	pInfo = GetObjectInfo(strName);

	//Notice : Server didn't have the GROUP_TEMP

	if(NULL == pInfo)
	{
		if (dwOldGroupId==GROUPID_TEMP)
		{
			ObjectGroup	*pOldGroup = GetGroup(GROUPID_TEMP);
			
			if (pOldGroup)
			{
                pOldGroup->RemoveMember(strName.c_str());
			}//endif
			
			AddObjectReq(strName,dwNewGroupId,PR_FRIEND);
			
			return nRet;
		}//endif

		nRet = char_err_membernotexist;
	}
	else
	{

		CHAT_CHANGE_GROUP	data;
		data.protocol.protocol = c2s_chat_family;
		data.protocol.subProtocol = chat_changegroup;
		data.dwOldGroupId = dwOldGroupId;
		data.dwNewGroupId = dwNewGroupId;
		strncpy(data.szPlayerName, strName.c_str(), sizeof(data.szPlayerName));
		data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

		SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);
	}

	return nRet;
}

void ChatObjectMgr_C::ChangeGroup(const string &strName, DWORD dwOldGroupId, DWORD dwNewGroupId)
{
	PCHATOBJECT_INFO	pInfo = GetObjectInfo(strName);

	if(pInfo)
	{
		ObjectGroup	*pOldGroup = GetGroup(dwOldGroupId);
		ObjectGroup *pNewGroup = GetGroup(dwNewGroupId);

		if(pOldGroup && pNewGroup)
		{
			pOldGroup->RemoveMember(strName.c_str());
			pNewGroup->AddMember(strName.c_str());
			pInfo->groupId = dwNewGroupId;
		}
	}
}

int ChatObjectMgr_C::RenameGroupReq(DWORD dwGroupId, const string &strName)
{
	int	nRet = chat_err_none;

	ObjectGroup	*pGroup = GetGroup(dwGroupId);

	if(pGroup)
	{
		CHAT_RENAME_GROUP	data;
		data.protocol.protocol = c2s_chat_family;
		data.protocol.subProtocol = chat_renamegroup;
		data.dwGroupId = dwGroupId;
		strncpy(data.name, strName.c_str(), sizeof(data.name));
		data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

		SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);	
	}
	else
	{
		nRet = char_err_groupnotexist;
	}

	return nRet;
}

int ChatObjectMgr_C::RemoveObjectReq(const string &strName)
{
	int nRet = chat_err_none;

	PCHATOBJECT_INFO	pInfo = GetObjectInfo(strName);

	if(pInfo)
	{
		ObjectGroup	*pGroup = GetGroup(pInfo->groupId);

		if(pGroup)
		{
			pGroup->RemoveMember(strName.c_str());
		}

		CHAT_REMOVE_OBJECT	data;
		data.protocol.protocol = c2s_chat_family;
		data.protocol.subProtocol = chat_rmobject;
		strncpy(data.szName, strName.c_str(), sizeof(data.szName));
		data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

		SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);

		m_ObjNameToInfo.erase(strName);
	}
	else
	{
		nRet = char_err_membernotexist;
	}

	return nRet;
}

int ChatObjectMgr_C::DeleteGroupReq(DWORD dwGroupId)
{
	ObjectGroup	*pObjGrp = GetGroup(dwGroupId);

	if(NULL == pObjGrp)
		return char_err_groupnotexist;

	if(pObjGrp->GetMemberCount() > 0)
		return char_err_groupnotempty;

	CHAT_DELETE_GROUP	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_deletegroup;
	data.dwGroupId = dwGroupId;
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);	

	return chat_err_none;
}

// Functions request operation to server

int	ChatObjectMgr_C::ChatToSomeoneByName(const string &strReceiver, BYTE *pMsg, int nMsgLen)
{
	if(m_bPreventSendMsg)
	{
		return chat_err_noaccesstosend;
	}

	if( IsPreventRecvMyMsg(strReceiver) )
	{
		return chat_err_preventreceiver;
	}

	if(nMsgLen > MAXSIZE_CHAT_MSG)
	{
		return chat_err_contentlenexceed;
	}

	char         cFilter = '*';
	const char * szFlterString = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_textfilter_char);
	if (szFlterString[0])
	{
		cFilter = szFlterString[0];
	}//endif

	if( !g_FilterChatText((const char*)pMsg ,cFilter))
	{
		return char_err_lawlesstext;
	}
		
	char	buf[sizeof(CHATMSG_BY_NAME) + MAXSIZE_CHAT_MSG];
				
	PCHATMSG_BY_NAME	pChatMsg = (PCHATMSG_BY_NAME)buf;
	pChatMsg->protocol.protocol = c2s_chat_family;
	pChatMsg->protocol.subProtocol = chat_msgtosomeonebyname;
	//Ãû×Ö
	//strcpy( pChatMsg->name, strReceiver.c_str());
	strncpy( pChatMsg->name, strReceiver.c_str(), sizeof(pChatMsg->name) );
	
	pChatMsg->msgLen = nMsgLen;
	memcpy(pChatMsg->msg, pMsg, nMsgLen);
	pChatMsg->msg[nMsgLen] = '\0';
	pChatMsg->protocol.len = sizeof(CHATMSG_BY_NAME) + nMsgLen - PROTOCOL_SIZE;

	if( !HasObject(strReceiver) )
	{
		AddTempObjectName(strReceiver.c_str());
	}
			
	SendDataToServer(buf, pChatMsg->protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

int ChatObjectMgr_C::AddObjectReq(const string &strName, int nGroupId, int nRelation)
{
	if( m_ObjNameToInfo.size() >= MAX_OBJECT_COUNT )
	{
		return chat_err_maxfriend;
	}

	if( strName == GetPlayerName(CLIENT_PLAYER_INDEX) )
	{
		return chat_err_cantaddself;	
	}

	PCHATOBJECT_INFO	pInfo = GetObjectInfo(strName);

	if(pInfo)
	{
		return chat_err_objalreadyexist;
	}

	CHAT_ADDOBJECT_REQ	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_addobjectreq;
	data.groupId = nGroupId;
	data.relation = (BYTE)nRelation;
	data.serverTag = 0;
	strncpy(data.name, strName.c_str(), sizeof(data.name));
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;

	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

void ChatObjectMgr_C::AddObject(CHAT_ADDOBJECT_NOTIFY *ps2cNotify)
{
	CHATOBJECT_INFO info;
	memset(&info,0,sizeof(info));
	
	info.relation = ps2cNotify->relation;
	info.bPreventRecvMyMsg = false;
	info.bPreventSendMsgToMe = false;
	ChatUtil::SetOnlineTag(info.onlineTag, ps2cNotify->bOnline);
	info.groupId = ps2cNotify->groupId;
	
	if (ps2cNotify->bOnline)
	{
		info.nLevel  = ps2cNotify->nLevel;
		info.nSeries = ps2cNotify->nSeries;
	}//endif

	m_ObjNameToInfo.insert(MAPOBJNAMETOINFO::value_type(ps2cNotify->name, info));

	ObjectGroup	*pGroup = GetGroup(info.groupId);

	if(pGroup)
	{
		pGroup->AddMember(ps2cNotify->name);
	}
}

int	ChatObjectMgr_C::ChangeRelationReq(const string &strName, int nRelation)
{
	PCHATOBJECT_INFO	pInfo = GetObjectInfo(strName);

	if(NULL == pInfo)
	{
		return chat_err_none;
	}

	if(pInfo->relation == PR_SYSTEM)
	{
		return chat_err_none;
	}

	if(pInfo->relation == (BYTE)nRelation)
	{
		return chat_err_none;
	}

	CHAT_CHANGERELATION_REQ	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_changerelation;
	data.relation = nRelation;
	strncpy(data.name, strName.c_str(), sizeof(data.name));
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

int	ChatObjectMgr_C::ChatInRoom(DWORD dwRoomId, BYTE *pMsg, int nMsgLen)
{
	if(m_bPreventSendMsg)
		return chat_err_noaccesstosend;

	if(nMsgLen > MAXSIZE_CHAT_MSG)
		return chat_err_contentlenexceed;

	char         cFilter = '*';
	const char * szFlterString = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_textfilter_char);
	if (szFlterString[0])
	{
		cFilter = szFlterString[0];
	}//endif

	if( !g_FilterChatText((const char*)pMsg , cFilter) )
	{
		return char_err_lawlesstext;
	}

	char	buf[MAXSIZE_CHAT_MSG + sizeof(CHATMSG_TO_ROOM)];
	
	PCHATMSG_TO_ROOM pRoomMsg = (PCHATMSG_TO_ROOM)buf;
	pRoomMsg->protocol.protocol = c2s_chat_family;
	pRoomMsg->protocol.subProtocol = chat_msgtoroom;
	pRoomMsg->roomId = dwRoomId;
	pRoomMsg->msgLen = nMsgLen;
	memcpy(pRoomMsg->msg, pMsg, nMsgLen);
	pRoomMsg->msg[nMsgLen] = '\0';
	pRoomMsg->protocol.len = sizeof(CHATMSG_TO_ROOM) + nMsgLen - PROTOCOL_SIZE;

	SendDataToServer(buf, pRoomMsg->protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}	

void ChatObjectMgr_C::PkValueChangeNotify(const char* szPlayerName, const int nPkValue)
{
   CHATOBJECT_INFO * pObjInfo = GetObjectInfo(szPlayerName);
   
   if (pObjInfo && pObjInfo->groupId == GROUPID_ENEMY)
   {
	   string key(szPlayerName);
	   m_ObjNameToPkValue[key] = nPkValue;
	   CoreDataChanged( GDCNI_PK_VALUE_CHANGE, (unsigned int)szPlayerName, nPkValue );
   }//endif
}

void ChatObjectMgr_C::UpdatePkValue(void)
{
	MAPNAMETOPKVALUE::const_iterator	itPK;
	MAPNAMETOPKVALUE::const_iterator	itPKEnd(m_ObjNameToPkValue.end());
	
	for(itPK = m_ObjNameToPkValue.begin(); itPK != itPKEnd; ++itPK)
	{
		string ObjName = (*itPK).first;
		CoreDataChanged( GDCNI_PK_VALUE_CHANGE, (unsigned int)ObjName.c_str(), (*itPK).second );
	}//end for itPK
}

int	ChatObjectMgr_C::CreateGroupReq(const string &strName)
{
	if(strName.length() >= MAXLEN_GROUPNAME || strName.length() < MINLEN_GROUPNAME)
	{
		return chat_err_groupnamelengtherr;
	}

	if( HasGroup(strName) )
	{
		return chat_err_groupexist;
	}

	if( m_GroupCont.size() >= MAX_FRIENDGROUP_COUNT )
	{
		return chat_err_exceedmaxgroup;
	}

	CHAT_CREATE_GROUP	data;
	data.protocol.protocol = c2s_chat_family;
	data.protocol.subProtocol = chat_creategroup;
	strncpy(data.name, strName.c_str(), sizeof(data.name));
	data.protocol.len = sizeof(data) - PROTOCOL_SIZE;
	SendDataToServer(&data, data.protocol.len + PROTOCOL_SIZE);

	return chat_err_none;
}

bool ChatObjectMgr_C::IsObjectInGroup(const string &strName,int nGroupId)
{ 
	MAPIDTOGROUP::iterator it= m_GroupCont.find(nGroupId);
	if (it!=m_GroupCont.end())
	{
        return it->second.HasObj(strName.c_str());
	}//endif

	return false;	
}

int ChatObjectMgr_C::OnLoadFriendsDataNotify(BYTE *pFriendsData)
{
	int	nRet = chat_err_none;

	PDB_FRIENDS_DATA_HEADER	pHeader = (PDB_FRIENDS_DATA_HEADER)pFriendsData;
	
	switch(pHeader->Version)
	{
	case CUR_FRIDATA_VERSION:
		nRet = LoadFriendsDataCurVersion(pFriendsData);
		break;
	default:
		nRet = chat_err_loadfriendsdatafailed;
	    break;
	}

	return nRet;
}

// Load Different Data Version
int	ChatObjectMgr_C::LoadFriendsDataCurVersion(BYTE *pData)
{
	int nRet = chat_err_none;

	DBFriendsDataParser	parser(pData);
	PDB_FRIENDS_DATA_HEADER	pHeader = parser.GetHeader();
	
	if(pHeader->ObjectCount > MAX_OBJECT_COUNT || pHeader->GroupCount > MAX_FRIENDGROUP_COUNT)
	{
		nRet = chat_err_loadfriendsdatafailed;
	}
	else
	{
		m_bPreventSendMsg = ((TRUE == pHeader->bPreventSendMsg) ? true : false);
		m_bPreventRecvMsg = ((TRUE == pHeader->bPreventRecvMsg) ? true : false);
		
		PS2C_CHAT_OBJ_INFO pObject = (PS2C_CHAT_OBJ_INFO)parser.GetObjectData();

		for(int i = 0; i < pHeader->ObjectCount; ++i)
		{
			CHATOBJECT_INFO info;

			info.bPreventRecvMyMsg   = pObject->bPreventRecvMyMsg;
			info.bPreventSendMsgToMe = pObject->bPreventSendMsgToMe;
			info.groupId             = pObject->groupId;
			info.onlineTag           = pObject->onlineTag;
			info.relation            = pObject->relation;
			info.nLevel              = pObject->nLevel;
			info.nSeries             = pObject->nSeries;
			m_ObjNameToInfo.insert(MAPOBJNAMETOINFO::value_type(pObject->name, info));

		    ++pObject;
		}//end for i

		PDB_FRIENDSGROUP_DATA	pGroup = (PDB_FRIENDSGROUP_DATA)(pData + sizeof(DB_FRIENDS_DATA_HEADER) + pHeader->ObjectCount * sizeof(S2C_CHAT_OBJ_INFO));

		for(int j = 0; j < pHeader->GroupCount; ++j)
		{
			ObjectGroup	group(pGroup->szGroupName);

			CHATOBJECTNAME* Member = (CHATOBJECTNAME*)pGroup->MemberName;
			for(int k = 0; k < pGroup->MemberCount; ++k)
			{
				group.AddMember(Member->name);
				Member++;
			}

			m_GroupCont.insert(MAPIDTOGROUP::value_type(pGroup->GroupId, group));

			pGroup = parser.GetNextGroupData(pGroup);
		}
	}
	
	return nRet;
}

int ChatObjectMgr_C::GetGroupIdAndNames(DWORD *pGroupIds, char *pGroupNames)
{
	MAPIDTOGROUP::const_iterator	itGroup;
	MAPIDTOGROUP::const_iterator	itGroupEnd(m_GroupCont.end());

	for(itGroup = m_GroupCont.begin(); itGroup != itGroupEnd; ++itGroup)
	{
		*pGroupIds = itGroup->first;
		strncpy(pGroupNames, itGroup->second.GetGroupName(), MAXSIZE_GROUPNAME);
		++pGroupIds;
		pGroupNames += MAXSIZE_GROUPNAME;
	}

	return m_GroupCont.size();
}

int	ChatObjectMgr_C::GetGroupMemberInfo(DWORD dwGroupId, char *pOutBuffer)
{
	ObjectGroup	*pGroup = GetGroup(dwGroupId);

	if(NULL == pGroup)
	{
		return 0;
	}

	int nCount = pGroup->GetObjectNames(pOutBuffer);
	UI_CHAT_OBJINFO *pUIObjInfo = (UI_CHAT_OBJINFO*)pOutBuffer;

	for(int nLoop = 0; nLoop < nCount; ++nLoop)
	{
		CHATOBJECT_INFO *pObj = GetObjectInfo(pUIObjInfo[nLoop].szName);

		if(NULL != pObj)
		{
			pUIObjInfo[nLoop].bOnline = ChatUtil::IsOnline(pObj->onlineTag);
			pUIObjInfo[nLoop].nSeries = pObj->nSeries;
		    pUIObjInfo[nLoop].nLevel  = pObj->nLevel;
		}//endif
		else
			pUIObjInfo[nLoop].bOnline = false;

	}//end for nLoop

	return nCount;
}

void ChatObjectMgr_C::RemoveObject(const string &strName)
{
	PCHATOBJECT_INFO	pInfo = GetObjectInfo(strName);

	if(pInfo)
	{
		ObjectGroup	*pGroup = GetGroup(pInfo->groupId);

		if(pGroup)
		{
			pGroup->RemoveMember(strName.c_str());
		}

		m_ObjNameToInfo.erase(strName);	
	}
}
