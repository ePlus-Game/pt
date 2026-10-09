//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2006-6-9 18:38
//      File_base        : ChatDataDef
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
#ifndef _ChatDataDef_h
#define _ChatDataDef_h

// Note: Only can include file that not depending any other files.

#include "cfs_fs2_savedef.h"
#include "GlobalDef.h"
#include "ItemCommonDef.h"
#include "layoutinterface.h"

//------------------ Common definitions -------------------------------------

namespace	CHAT
{

#define		INVALID_PLAYER_INDEX			-1
#define		INVALID_OBJECT_ID				0
#define		INVALID_ROOM_ID					0
#define		INVALID_GROUP_ID				0
#define		INVALID_MAIL_ID					0

#define		SYSTEM_PLAYER_INDEX				-1
#define		MINLEN_GROUPNAME				1
#define		MAXSIZE_GROUPNAME				17
#define		MAXLEN_GROUPNAME				MAXSIZE_GROUPNAME - 1
#define		MAXSIZE_CHATROOMNAME			17
#define		MAXSIZE_MAIL					8 * 1024
#define		MAX_OBJECT_COUNT				200
#define		DEF_MAX_PLAYERCHATROOM			10
#define		DEF_ROOM_MEMBERNUM				8
#define		MAX_ROOM_MEMBERNUM				256

	// 客户端多了一个临时好友组，在服务器端没有
#ifdef _SERVER
#define		MAX_FRIENDGROUP_COUNT			8
#else
#define		MAX_FRIENDGROUP_COUNT			9
#endif

#define		DYNAMIC_ROOMID_BEGIN			1024
#define		DYNAMIC_GROUPID_BEGIN			1024
#define		DEFAULT_GROUP_SIZE				32
#define		CHAT_MAX_PLAYER					MAX_PLAYER
#define		COSE_ROOM_ID					-10
#define		SYSTEM_ROOM_ID					1
#define		GLOBAL_ROOM_ID					2
#define		LOCAL_ROOM_ID					3
#define		MAP_ROOM_ID						4
#define		TEAM_ROOM_ID					5
#define		COMBAT_INFO_ROOM_ID				6

#define		MAXSIZE_SEND_BUF				12 * 1024
#define		MAXSIZE_ITEMNAME				32
#define		MAXSIZE_CHAT_MSG				256
#define		MAXSIZE_SYNC_ITEM_COUNT			5
#define		MAX_MAILCOUNT_PERPLAYER			16

#define		CHATROOMS_ALLOC_GRANULARITY		512
#define		MAXCOUNT_CHATROOMS				(512 * CHATROOMS_ALLOC_GRANULARITY)

#define		CUR_FRIDATA_VERSION				3

#define		SUCCESS_GETOUT_MAILPLUS			1
#define		SUCCESS_GETOUT_MAILMONEY		1
 
enum	enInnerGroupId
{
	GROUPID_BEGIN = 0,

	GROUPID_NONE = 1,
	GROUPID_TEMP = 2,		// Not save
	GROUPID_BLACK = 3,
	GROUPID_ENEMY = 4,

	GROUPID_NUM,
};

enum
{
	MSG_SHOWTYPE_ROOM = 0x1,			// Show the message in target room
	MSG_SHOWTYPE_MIDDLESCREEN = 0x2,	// Show the message on the middle of the screen
	MSG_SHOWTYPE_TOPSCREEN = 0x4,		// Show the message on the top of the screen scrolling
	MSG_SHOWTYPE_SPECIAL = 0x8,         // Show the message on the top of the screen fadin and fade out
};

enum	enObjectRelation
{
	PR_INVALID = -1,

	PR_BEGIN,
	// Add relation after here

	PR_FRIEND,
	PR_SYSTEM,

	// Add relation before here
	PR_END,
};

enum	enSysMsgType
{
	SYSMSG_TYPE_STR,
	SYSMSG_TYPE_ID,
};

enum	enMailDBOpe
{
	enMailDBOpe_None = -1,

	enMailDBOpe_LoadMailList,
	enMailDBOpe_LoadMail,
	enMailDBOpe_SendMail,
	enMailDBOpe_GetOutPlus,
	enMailDBOpe_GetOutMoney,
	enMailDBOpe_DelMail,
	enMailDBOpe_CloseMail,
	enMailDBOpe_QueryNewMail,
	enMailDBOpe_ReturnMail,

	enMailDBOpe_Num,
};

enum	enChatDBProcCode
{
	enChat_DBProcCode_CheckPlayerName = enChat_DBOpe_Begin + 1,

	// add new code before here
	enChat_DBProcCode_LastUsed,

	enChat_DBProcCode_Num = enChat_DBProcCode_LastUsed - enChat_DBOpe_Begin - 1,
};

typedef void (*CHATCALLBACK)(const void *Param, unsigned int uPassby);
typedef bool (*BROADCASTFILTER)(unsigned int nParam1, unsigned int uPaassby);

#pragma pack(push, 1)

typedef struct _CHAT_CallBack_Param
{
	void	*pChatCenter;
	BYTE	*pData;
	int		nSenderIdx;
	int		nDataLen;

} CHAT_CALLBACK_PARAM;

typedef struct _SysMsg_CallBack_Param
{
	char	*pData;
	int		nDataLen;

} SYSMSG_CALLBACK_PARAM;

typedef struct _Chat_MailPlus_Item
{
	TItemtransfersData data;

} CHAT_MAILPLUS_ITEM, *PCHAT_MAILPLUS_ITEM;

typedef struct _Chat_MailData_Client
{
	bool	bHasMailData;
	BYTE	plusCount;
	DBTASK_MAILLISTITEM	header;
	char	szContent[MAXSIZE_MAILTEXT];
	CHAT_MAILPLUS_ITEM	plusData[MAXCOUNT_MAILPLUS];

} CHAT_MAILDATA_CLIENT, *PCHAT_MAILDATA_CLIENT;

typedef struct _UI_Chat_ObjInfo
{
	char	szName[MAXSIZE_ROLENAME];
	bool	bOnline;
	BYTE    nSeries;
	BYTE    nLevel;
} UI_CHAT_OBJINFO;

typedef struct _UI_RECENT_OBJ_NAME
{
	char    szName[MAXSIZE_ROLENAME];
}UI_RECENT_OBJ_NAME;

#define MAX_RECENT_OBJ_RECORD_NUM 64

//--------------------- Protocol and related data structures -------------------

enum	CHAT_SUBPROTOCOL
{
	chat_subprotocol_begin = -1,

	chat_msgtosomeonebyname,	// one to one chat
	chat_addobjectreq,
	chat_rmobject,
	chat_notifyobjectid,
	chat_changerelation,
	chat_errcode,
	chat_itemerrcode,
	chat_createroom,
	chat_addmembertoroom,
	chat_joinroom,
	chat_msgtoroom,
	chat_leaveroom,
	chat_kickmemberfromroom,
	chat_memberkicknotify,
	chat_creategroup,
	chat_deletegroup,
	chat_changegroup,
	chat_renamegroup,
	chat_sendmail,
	chat_loadmaillist,
	chat_loadmail,
	chat_loadfriendsdata,
	chat_addchannel,
	chat_getoutitem,
	chat_closemail,
	chat_delmail,
	chat_playeronline,
	chat_playeroffline,
	chat_systemmsg,
	chat_systemnpcmsg,
	chat_getoutmoney,
	chat_newmailnotify,
	chat_delchannel,
	chat_onlinestatusnotify,
	chat_returnmail,
	chat_changeroomowner,
	chat_chatroomprivope,
	chat_pkvaluechange,

	chat_subprotocol_end,
};

//-------------------- data structures of mail and friends data---------------------------------

typedef struct _Chat_LoadMailList_Req
{
	VARLEN_PROTOCOL_HEADER protocol;
	BYTE                   nPage;

} CHAT_LOADMAILLIST_REQ, *PCHAT_LOADMAILLIST_REQ;

typedef struct _Chat_Structured_DataBlock
{
	VARLEN_PROTOCOL_HEADER protocol;
	BYTE	data[1];

} CHAT_STRUCTURED_DATABLOCK, *PCHAT_STRUCTURED_DATABLOCK;

typedef struct _Chat_ListFriendRet
{
	VARLEN_PROTOCOL_HEADER protocol;
	BYTE    data[1];
}CHAT_LIST_FRIEND_RET,*PCHAT_LIST_FRIEND_RET;

typedef struct _Chat_LoadMail_Req
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	mailId;

} CHAT_LOADMAIL_REQ, *PCHAT_LOADMAIL_REQ;

typedef struct _Chat_GetOut_Item
{
	VARLEN_PROTOCOL_HEADER protocol;
	BYTE	index;
	DWORD	mailId;

} CHAT_GETOUT_ITEM, *PCHAT_GETOUT_ITEM;

typedef struct _Chat_Ope_Notify
{
	VARLEN_PROTOCOL_HEADER	protocol;
	DWORD	param1;
	DWORD	param2;
	DWORD	param3;

} CHAT_OPE_NOTIFY, *PCHAT_OPE_NOTIFY;

typedef CHAT_LOADMAIL_REQ	CHAT_CLOSEMAIL_REQ;
typedef PCHAT_LOADMAIL_REQ	PCHAT_CLOSEMAIL_REQ;
typedef CHAT_LOADMAIL_REQ	CHAT_DELMAIL_REQ;
typedef PCHAT_LOADMAIL_REQ	PCHAT_DELMAIL_REQ;
typedef CHAT_LOADMAIL_REQ	CHAT_GETMONEY_REQ;
typedef PCHAT_LOADMAIL_REQ	PCHAT_GETMONEY_REQ;
typedef CHAT_LOADMAIL_REQ	CHAT_RETURNMAIL_REQ;
typedef CHAT_RETURNMAIL_REQ*	PCHAT_RETURNMAIL_REQ;

//--------------------------------------------------------------------------------------------

typedef struct _Chat_System_Msg
{
	VARLEN_PROTOCOL_HEADER	protocol;
	BYTE	msgType;
	BYTE	showType;
	WORD	msgLen;
	DWORD	showRoom;
	char	msg[1];

} CHAT_SYSTEM_MSG;

typedef struct _Chat_System_NpcMsg
{
	VARLEN_PROTOCOL_HEADER	protocol;
	DWORD	npcId;
	BYTE	msgType;
	WORD	msgLen;
	char	msg[1];

} CHAT_SYSTEM_NPCMSG;

typedef struct _ChatMsg_By_Name
{
	VARLEN_PROTOCOL_HEADER protocol;
	char	name[MAXSIZE_ROLENAME];
	int		npcId;
	BYTE	isRecive;
	WORD	msgLen;
	int		camou;
	char	msg[1];

} CHATMSG_BY_NAME, *PCHATMSG_BY_NAME;

typedef struct _Chat_AddObject_Req
{
	VARLEN_PROTOCOL_HEADER protocol;
	int		groupId;
	BYTE	relation;
	char	name[MAXSIZE_ROLENAME];
	DWORD	serverTag;

} CHAT_ADDOBJECT_REQ, *PCHAT_ADDOBJECT_REQ;

typedef struct _Chat_AddObject_Notify
{
	VARLEN_PROTOCOL_HEADER protocol;
	BYTE	relation;
	DWORD	groupId;
	bool	bOnline;
	char	name[MAXSIZE_ROLENAME];
	BYTE    nLevel;
	BYTE    nSeries;
} CHAT_ADDOBJECT_NOTIFY, *PCHAT_ADDOBJECT_NOTIFY;

typedef struct _Chat_ChangeRelation_Req
{
	VARLEN_PROTOCOL_HEADER protocol;
	BYTE	relation;
	char	name[MAXSIZE_ROLENAME];

} CHAT_CHANGERELATION_REQ, *PCHAT_CHANGERELATION_REQ;

typedef struct _Chat_ChangeRelation_Ret
{
	VARLEN_PROTOCOL_HEADER protocol;
	char	szName[MAXSIZE_ROLENAME];
	BYTE	relation;

} CHAT_CHANGERELATION_RET, *PCHAT_CHANGERELATION_RET;

typedef struct _Chat_Reomve_Object
{
	VARLEN_PROTOCOL_HEADER protocol;
	char	szName[MAXSIZE_ROLENAME];

} CHAT_REMOVE_OBJECT, *PCHAT_REMOVE_OBJECT;

typedef struct _Chat_RemoveObject_Ret
{
	VARLEN_PROTOCOL_HEADER protocol;
	char	szName[MAXSIZE_ROLENAME];

} CHAT_REMOVEOBJECT_RET, *PCHAT_REMOVEOBJECT_RET;

typedef struct _Chat_Err_Code
{
	VARLEN_PROTOCOL_HEADER protocol;
	short int	errorCode;

} CHAT_ERR_CODE, *PCHAT_ERR_CODE;

typedef struct _Chat_CreateRoom_Req
{
	VARLEN_PROTOCOL_HEADER protocol;
	char	roomName[MAXSIZE_CHATROOMNAME];

} CHAT_CREATEROOM_REQ, *PCHAT_CREATEROOM_REQ;

typedef struct _Chat_CreateRoom_Rst
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;
	char	roomName[MAXSIZE_CHATROOMNAME];

} CHAT_CREATEROOM_RST, *PCHAT_CREATEROOM_RST;

typedef struct _ChatRoom_Add_Member
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;
	char	name[MAXSIZE_ROLENAME];

} CHATROOM_ADD_MEMBER, *PCHATROOM_ADD_MEMBER;

typedef struct _ChatRoom_ChatPrivOpe_Req
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;
	char	name[MAXSIZE_ROLENAME];
	bool	bForbid;

} CHATROOM_CHATPRIVOPE_REQ, *PCHATROOM_CHATPRIVOPE_REQ;

typedef struct _ChatRoom_ChatPrivOpe_Ret
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;
	bool	bForbid;
	bool	bIsOperator;

} CHATROOM_CHATPRIVOPE_RET, *PCHATROOM_CHATPRIVOPE_RET;

typedef struct _ChatRoom_Change_Owner
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;
	char	newOwnerName[MAXSIZE_ROLENAME];

} CHATROOM_CHANGE_OWNER, *PCHATROOM_CHANGE_OWNER;

typedef struct _Chat_Join_Room
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;
	char	createrName[MAXSIZE_ROLENAME];
	char	roomName[MAXSIZE_CHATROOMNAME];

} CHAT_JOIN_ROOM, *PCHAT_JOIN_ROOM;

typedef struct _ChatMsg_To_Room
{
	VARLEN_PROTOCOL_HEADER protocol;	
	DWORD	roomId;
	WORD	msgLen;
	BYTE	gm;
	BYTE	msg[1];

} CHATMSG_TO_ROOM, *PCHATMSG_TO_ROOM;

typedef struct _ChatRoomMsg_To_Someone
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;
	BYTE	unitRank;		//社会关系阶层 1-普通成员 2-族长 3-侯主
	char	senderName[MAXSIZE_ROLENAME];
	int		senderPlayerId;
	int		stringID;
	WORD	msgLen;
	int		camou;
	BYTE	gm;
	BYTE	msg[1];

} CHATROOMMSG_TO_SOMEONE, *PCHATROOMMSG_TO_SOMEONE;

typedef struct _Chat_Leave_Room
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;

} CHAT_LEAVE_ROOM, *PCHAT_LEAVE_ROOM;

typedef struct _Chat_LeaveRoom_Notify
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;
	char	name[MAXSIZE_ROLENAME];

} CHAT_LEAVEROOM_NOTIFY, *PCHAT_LEAVEROOM_NOTIFY;

typedef CHAT_LEAVEROOM_NOTIFY	CHAT_ROOM_KICKMEMBER;
typedef PCHAT_LEAVEROOM_NOTIFY	PCHAT_ROOM_KICIMEMBER;

typedef CHAT_LEAVEROOM_NOTIFY	CHATROOM_KICKMEMBER_NOTIFY;
typedef PCHAT_LEAVEROOM_NOTIFY	PCHATROOM_KICKMEMBER_NOTIFY;

typedef struct _Chat_Create_Group
{
	VARLEN_PROTOCOL_HEADER protocol;
	char	name[MAXSIZE_GROUPNAME];

} CHAT_CREATE_GROUP, *PCHAT_CREATE_GROUP;

typedef struct _Chat_CreateGroup_Rst
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	dwId;
	char	name[MAXSIZE_GROUPNAME];

} CHAT_CREATEGROUP_RST, *PCHAT_CREATEGROUP_RST;

typedef struct _Chat_Delete_Group
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	dwGroupId;

} CHAT_DELETE_GROUP, *PCHAT_DELETE_GROUP;

typedef struct _Chat_Change_Group
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	dwOldGroupId;
	DWORD	dwNewGroupId;
	char	szPlayerName[MAXSIZE_ROLENAME];

} CHAT_CHANGE_GROUP, *PCHAT_CHANGE_GROUP;

typedef struct _Chat_ChangeGroup_Ret
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	dwOldGroupId;
	DWORD	dwNewGroupId;
	char	szPlayerName[MAXSIZE_ROLENAME];

} CHAT_CHANGEGROUP_RET, *PCHAT_CHANGEGROUP_RET;

typedef struct _Chat_Rename_Group
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	dwGroupId;
	char	name[MAXSIZE_GROUPNAME];

} CHAT_RENAME_GROUP, *PCHAT_RENAME_GROUP;

typedef struct _Chat_Notify_BuildinRoomId
{
	VARLEN_PROTOCOL_HEADER protocol;
	DWORD	roomId;
	BYTE	nameLen;
	char	name[1];

} CHAT_NOTIFY_BUILDINROOMID, *PCHAT_NOTIFY_BUILDINROOMID;

typedef struct _Chat_NewMail_Notify
{
	VARLEN_PROTOCOL_HEADER	protocol;
	BYTE	newMailCnt;
	char	senderName[MAXSIZE_ROLENAME];		

} CHAT_NEWMAIL_NOTIFY, *PCHAT_NEWMAIL_NOTIFY;

typedef struct _Chat_OnlineStatus_Notify
{
	VARLEN_PROTOCOL_HEADER	protocol;
	char	playerName[MAXSIZE_ROLENAME];
	bool	bOnline;
	BYTE    nLevel;
	BYTE    nSeries;
} CHAT_ONLINESTATUS_NOTIFY, *PCHAT_ONLINESTATUS_NOTIFY;

typedef struct _Chat_PKChange_Notify
{
	VARLEN_PROTOCOL_HEADER	protocol;
	char	playerName[MAXSIZE_ROLENAME];
	DWORD	pkValue;
}CHAT_PKCHANGE_NOTIFY,*PCHAT_PKCHANGE_NOTIFY;

//xiehong add -begin
enum ChatDataStyle
{
	CDS_None,
	CDS_CasterName,
		
};

struct CommonChatData
{
	LOGameObject gameObject;
	ChatDataStyle styleId;
	const char* msg;
	CommonChatData()
	{
		styleId = CDS_None;
		msg = NULL;
	}
	CommonChatData(const CommonChatData& other)
	{
		styleId = other.styleId;
		msg = other.msg;
	}
};
//xiehong add -end
//-----------------------------------------------------------------------------------------------

class ChatUtil
{
public:
	static inline unsigned int IsInnerGroup(DWORD dwGroupId)
	{
		return dwGroupId > GROUPID_BEGIN && dwGroupId < GROUPID_NUM;
	}

	static inline void SetOnlineTag(BYTE &onlineTag, bool bOnline)
	{
		if(bOnline)
			onlineTag |= 0x80;
		else
			onlineTag &= 0x7f;
	}

	static inline bool IsOnline(BYTE onlineTag)
	{
		return (onlineTag & 0x80) ? true : false;
	}
};

#pragma pack(pop)

//-----------------------------------------------------------------------------------------------
//	Begin (Save Friends & Friend Groups Data Version)
//-----------------------------------------------------------------------------------------------

//Version 1.................................................................................................
typedef	struct _ChatObject_InfoV1
{
	BYTE	relation;
	BYTE	onlineTag;
	bool	bPreventRecvMyMsg;
	bool	bPreventSendMsgToMe;
	DWORD	groupId;
	
} CHATOBJECT_INFOV1, *PCHATOBJECT_INFOV1;

typedef struct _ChatObjectV1
{
	char	name[MAXSIZE_ROLENAME];
	CHATOBJECT_INFOV1	info;
	
} CHATOBJECTV1, *PCHATOBJECTV1;

typedef struct _ChatObjectNameV1
{
	char	name[MAXSIZE_ROLENAME];
	
} CHATOBJECTNAMEV1, *PCHATOBJECTNAMEV1;

typedef struct _DB_FriendsGroup_DataV1
{
	char	szGroupName[MAXSIZE_GROUPNAME];
	WORD	MemberCount;
	DWORD	GroupId;
	char	MemberName[1];
	
} DB_FRIENDSGROUP_DATAV1, *PDB_FRIENDSGROUP_DATAV1;

typedef struct _DB_Friends_Data_HeaderV1
{
	BYTE	Version;
	BYTE	bPreventRecvMsg;
	BYTE	bPreventSendMsg;
	BYTE	GroupCount;
	WORD	ObjectCount;
	
} DB_FRIENDS_DATA_HEADERV1, *PDB_FRIENDS_DATA_HEADERV1;

class DBFriendsDataParserV1
{
public:
	explicit DBFriendsDataParserV1(BYTE *pBuf) : m_pBuf(pBuf)
	{
	}
	
	PDB_FRIENDS_DATA_HEADERV1 GetHeader()
	{
		return (PDB_FRIENDS_DATA_HEADERV1)m_pBuf;
	}
	
	PDB_FRIENDSGROUP_DATAV1	GetFirstGroupData()
	{
		return (PDB_FRIENDSGROUP_DATAV1)(m_pBuf + HeaderSize() + ObjectDataSize());
	}
	
	PDB_FRIENDSGROUP_DATAV1	GetNextGroupData(const PDB_FRIENDSGROUP_DATAV1 pGroup)
	{
		return (PDB_FRIENDSGROUP_DATAV1)((BYTE*)pGroup + GroupDataSize(pGroup));
	}
	
	PCHATOBJECTV1	GetObjectData()
	{
		return (PCHATOBJECTV1)(m_pBuf + HeaderSize());
	}
	
	static int	GroupDataSize(const PDB_FRIENDSGROUP_DATAV1 pGroup)
	{
		return sizeof(DB_FRIENDSGROUP_DATAV1) + pGroup->MemberCount * sizeof(CHATOBJECTNAMEV1) - 1;
	}
	
private:
	static int	HeaderSize()
	{
		return sizeof(DB_FRIENDS_DATA_HEADERV1);
	}
	
	int	ObjectDataSize()
	{
		return GetHeader()->ObjectCount * sizeof(CHATOBJECTV1);
	}
	
private:
	DBFriendsDataParserV1(const DBFriendsDataParserV1 &rhs);
	DBFriendsDataParserV1& operator= (const DBFriendsDataParserV1 &rhs);
	
private:
	BYTE	*m_pBuf;
};

//end version1....................................................................................................................................

#pragma	pack(push, 1)                  //CURRENT_VERSION
typedef	struct _ChatObject_Info
{
	BYTE	relation;
	BYTE	onlineTag;
	bool	bPreventRecvMyMsg;
	bool	bPreventSendMsgToMe;
	DWORD	groupId;

#ifndef _SERVER
	int     nSeries;
	int     nLevel;
#endif

} CHATOBJECT_INFO, *PCHATOBJECT_INFO;

typedef struct _ChatObject
{
	char	name[MAXSIZE_ROLENAME];
	CHATOBJECT_INFO	info;

} CHATOBJECT, *PCHATOBJECT;

typedef struct _S2C_CHAT_OBJ_INFO
{
	char	name[MAXSIZE_ROLENAME];
	BYTE	relation;
	BYTE	onlineTag;
	bool	bPreventRecvMyMsg;
	bool	bPreventSendMsgToMe;
	DWORD	groupId;
	BYTE    nSeries;
	BYTE    nLevel;
}S2C_CHAT_OBJ_INFO,*PS2C_CHAT_OBJ_INFO;

typedef struct _ChatObjectName
{
	char	name[MAXSIZE_ROLENAME];

} CHATOBJECTNAME, *PCHATOBJECTNAME;

typedef struct _DB_FriendsGroup_Data
{
	char	szGroupName[MAXSIZE_GROUPNAME];
	WORD	MemberCount;
	DWORD	GroupId;
	char	MemberName[1];

} DB_FRIENDSGROUP_DATA, *PDB_FRIENDSGROUP_DATA;

typedef struct _DB_Friends_Data_Header
{
	BYTE	Version;
	BYTE	bPreventRecvMsg;
	BYTE	bPreventSendMsg;
	BYTE	GroupCount;
	WORD	ObjectCount;

} DB_FRIENDS_DATA_HEADER, *PDB_FRIENDS_DATA_HEADER;

class DBFriendsDataParser
{
public:
	explicit DBFriendsDataParser(BYTE *pBuf) : m_pBuf(pBuf)
	{
	}
	
	PDB_FRIENDS_DATA_HEADER GetHeader()
	{
		return (PDB_FRIENDS_DATA_HEADER)m_pBuf;
	}

	PDB_FRIENDSGROUP_DATA	GetFirstGroupData()
	{
		return (PDB_FRIENDSGROUP_DATA)(m_pBuf + HeaderSize() + ObjectDataSize());
	}

	PDB_FRIENDSGROUP_DATA	GetNextGroupData(const PDB_FRIENDSGROUP_DATA pGroup)
	{
		return (PDB_FRIENDSGROUP_DATA)((BYTE*)pGroup + GroupDataSize(pGroup));
	}

	PCHATOBJECT	GetObjectData()
	{
		return (PCHATOBJECT)(m_pBuf + HeaderSize());
	}

	static int	GroupDataSize(const PDB_FRIENDSGROUP_DATA pGroup)
	{
		return sizeof(DB_FRIENDSGROUP_DATA) + pGroup->MemberCount * sizeof(CHATOBJECTNAME) - 1;
	}

private:
	static int	HeaderSize()
	{
		return sizeof(DB_FRIENDS_DATA_HEADER);
	}

	int	ObjectDataSize()
	{
		return GetHeader()->ObjectCount * sizeof(CHATOBJECT);
	}

private:
	DBFriendsDataParser(const DBFriendsDataParser &rhs);
	DBFriendsDataParser& operator= (const DBFriendsDataParser &rhs);

private:
	BYTE	*m_pBuf;
};

#pragma pack(pop)
//-----------------------------------------------------------------------------------------------
//	End (Save Friends & Friend Groups Data Version)
//-----------------------------------------------------------------------------------------------

}	// end of namespace CHAT

#endif
