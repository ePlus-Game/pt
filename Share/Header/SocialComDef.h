//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2006
//
//      Created_datetime : 2007:1:8   15:36
//      File_base        : SocialComDef
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
#ifndef _SocialComDef_h
#define _SocialComDef_h

#include "GlobalDef.h"
#include "GameDataDef.h"

namespace SocialRelation
{

#define		MAXNUM_SOCIALUNIT_PROC			20
#define		MAXCOUNT_SUBLIST_ONETIMEGET		TONGMEMBER_MAX_NUM
#define		MAXSIZE_UNCONFIRMREQ			1020
#define		MAXSIZE_HINT_MSG				128
#define		MAXSIZE_ANNOUNCEMENT			512

#define		CURRENT_SOCIALDATA_DB_VERSIONNO	    1
#define     CURRENT_SOCIALDATA_ATTR_VERSIONNO   1
#define     CURRENT_SOCIALDATA_PRIV_VERSIONNO   1

#define		MAX_RELATION_LAYER_COUNT		5
#define		MAX_RELATION_TEMPLATE_COUNT		5

#define     MAX_SOCIAL_RECRUIT_INFO_NUM_PER_PAGE 10
#define     MAX_SOCIAL_RECRUIT_INFO_NUM_Gens     200
#define     MAX_SOCIAL_RECRUIT_INFO_NUM_TONG     20
#define     MAX_SOCIAL_SCRIPT_DATA_NUM_NEEDSAVE  20
#define     MAX_SOCIAL_SCRIPT_DATA_NUM_NONEEDSAVE 20

#define		SOCIAL_EMPTY_STRING				""

#define     LEAGUE_LAYER_DEFAULT_SUBUNIT_MAX     2

enum	enSRProcotol
{
	enSRProtocol_None = -1,
	
	enSRProtocol_UnitOperation,
	enSRProtocol_MsgCode2Client,
	enSRProtocol_Msg2Client,
	enSRProtocol_ReqToConfirm,
	enSRProtocol_ReqConfirmRet,

	enSRProtocol_ReqCityInfo,
	enSRProtocol_ContributeCityRes,
	enSRProtocol_DistillCityRes,
	enSRProtocol_SetCityTaxRate,

	enSRProtocol_GetUnitInfo,
	enSRProtocol_GetRecruitInfo,

	enSRProtocol_InfoBroadCast,

	enSRProtocol_Num,
};

enum	enSocialUnitTplId
{
	// Note: Don't change the values, Config file had used them
	enSUTplId_None = 0,

	enSUTplId_Tong,
	enSUTplId_Team,

	enSUTplId_Num,
};

enum	enSUTongLayer
{
	// Note: Don't change the values, Config file had used them
	enSULayer_None = 0,
	enSULayer_Player,
	enSULayer_Gens,
//	enSULayer_Seigneur,
	enSULayer_Tong,
	enSULayer_League,

	enSUTong_LayerNum,
};

enum	enSUTeamLayer
{
	// Note: Don't change the values, Config file had used them
	enSULayer_Team  = enSULayer_Player + 1,

	enSUTeam_LayerNum,
};

enum	enSocialUnitOperation
{
	// Note: Don't change the values, Config file had used them
	enSUO_None = -1,
	
	enSUO_CreateUnit,
	enSUO_AddSubUnit,
	enSUO_RemoveSubUnit,
	enSUO_ChatInUnit,
	enSUO_ForbidChat,
	enSUO_UnForbidChat,
	enSUO_BreakUnit,
	enSUO_LeaveUnit,
	enSUO_PubAnnouncement,
	enSUO_GetSubList,
	enSUO_GetAnnouncement,
	enSUO_GetPrePageSubList,
	enSUO_GetNextPageSubList,
	enSUO_ChangeOwner,
	enSUO_ReqJoinHigherLevel,
	enSUO_GetRecruitInfo,
	enSUO_AddRecruitInfo,
	enSUO_DelRecruitInfo,

	enSUO_ContributeCityRes,
	enSUO_DistillCityRes,
	enSUO_ReqCityInfo,
	enSUO_SetCityTaxRate,

	enSUO_Num,
};

enum	enSocialUnitAttrId
{
	// Note: Don't change the values, Config file had used them

	enSUAttr_None = -1,
	
	enSUAttr_UnitName,
	enSUAttr_Announcement,
	enSUAttr_ChatChannel,
	enSUAttr_PlayerInfo,
	enSUAttr_CityMap,
	enSUAttr_PoolMap,
	enSUAttr_InstanceInfo,
	enSUAttr_ForceSubUnitMaxNum,
	enSUAttr_ScriptData,
	
	enSUAttr_Num,
};

enum	enSocialDBOpeType
{
	enSoc_DBOpe_Begin = enSocial_DBOpe_Begin,

	enSoc_DBOpe_LoadOwnTree,
	enSoc_DBOpe_LoadAllSubUnit,
	enSoc_DBOpe_AddUnit,
	enSoc_DBOpe_UpdateUnit,
	enSoc_DBOpe_RemoveUnit,
	enSoc_DBOpe_UpdatePGuid,
	enSoc_DBOpe_UpdateAttr,
	enSoc_DBOpe_UpdateAppData,
	enSoc_DBOpe_UpdateSUCnt,
	enSoc_DBOpe_CheckUnitName,
	
	// Add new item before this
	enSoc_DBOpe_LastUsed,

	enSoc_DBOpe_Num = enSoc_DBOpe_LastUsed - enSoc_DBOpe_Begin - 1,
};

enum	enSocialDBRecordCol
{
	enSocDBRec_Col_None = -1,

	enSocDBRec_Col_TplId,
	enSocDBRec_Col_Layer,
	enSocDBRec_Col_UnitGuid,
	enSocDBRec_Col_PGuid,
	enSocDBRec_Col_UnitName,
	enSocDBRec_Col_OwnerName,
	enSocDBRec_Col_SubUnitCnt,
	enSocDBRec_Col_UnitAttr,
	enSocDBRec_Col_AppendData,

	enSocDBRec_Col_Num,
};

enum	enSUReqConfirmCode
{
	enSUReqConfirm_None,
	enSUReqConfirm_Ok,
    enSUReqConfirm_Ignore,
	enSUReqConfirm_Refuse
};

enum	enSUGetSubListType
{
	enSUGetSubList_Type_Belong,
	enSUGetSubList_Type_Other,
};

// common data structure definition
#pragma	pack(push, 1)
typedef struct _SU_Player_Info
{
	BYTE	isOnline;
	BYTE	profession;
	WORD	level;

} SU_PLAYER_INFO;

typedef struct _SU_ComUnit_Info
{
	char	unitName[MAXSIZE_ORGNAME];
	char	ownerName[MAXSIZE_ROLENAME];
	FSGUID	unitGuid;

} SU_COMUNIT_INFO;

#define     MAX_CHAT_OPERATION_FORBIDDEN_LAYER 3
typedef struct _SU_PLAYER_INFO_EX
{
	BYTE    topOwnerLayer;
	BYTE    ForbidChatState[MAX_CHAT_OPERATION_FORBIDDEN_LAYER];
}SU_PLAYER_INFO_EX;

typedef struct _SU_TONG_INFO_EX
{
	WORD    nCityMapId;
	WORD    nPoolMapId;
	WORD    nSubUnitNum;
}SU_TONG_INFO_EX;
typedef struct _SU_UnitTransfer_Info
{
	SU_COMUNIT_INFO		comInfo;
	SU_PLAYER_INFO		playerInfo;
	SU_PLAYER_INFO_EX   playerInfoEx;
	SU_TONG_INFO_EX     tongInfoEx;
} SU_UNITTRANSFER_INFO;

typedef struct _SU_ComOpe_Param
{
	BYTE		opeId;
	BYTE		unitTplId;
	BYTE		unitLayer;
	char		confirmCode;

} SU_COMOPE_PARAM;

typedef struct _UI2Core_Req_Param
{
	SU_COMOPE_PARAM	comParam;
	void			*pParam;

} UI2CORE_REQ_PARAM;

typedef struct _c2s_ComOpe_Header
{
	VARLEN_PROTOCOL_HEADER	proHeader;
	SU_COMOPE_PARAM			comParam;

} C2S_COMOPE_HEADER;

typedef C2S_COMOPE_HEADER S2C_COMOPE_HEADER;

typedef struct _c2s_NeedConfirmOpe_Header
{
	C2S_COMOPE_HEADER	ch;
	char				szLauncherName[MAXSIZE_ROLENAME];

} C2S_NEEDCONFIRMOPE_HEADER;

// db save structure

typedef struct _db_AttrData
{
	BYTE	attrId;
	WORD	attrSize;
	char	attrData[1];

	int		size()
	{
		return attrSize + ((char*)attrData - (char*)&attrId);
	}

	int	size(int nDataSize)
	{
		return nDataSize + ((char*)attrData - (char*)&attrId);
	}

} DB_ATTRDATA;

typedef struct _db_Unit_Attr
{
	BYTE			version;

	// Note: add data below here
	BYTE			attrCount;
	char			attrData[1];

	int		size()
	{
		int		nSize = 0;

		for(BYTE loop = 0; loop < attrCount; ++loop)
		{
			DB_ATTRDATA	*pAttr = (DB_ATTRDATA*)(attrData + nSize);
			nSize += pAttr->size();
		}

		return nSize + ((char*)attrData - (char*)&version);
	}

} DB_UNIT_ATTR;

typedef struct _db_Unit_Data
{
	BYTE				version;

	// Note: Add data below here
	BYTE				tplId;
	BYTE				layer;
	BYTE				maxJoinedLayer;
	FSGUID				unitGuid;
	char				ownerName[MAXSIZE_ROLENAME];
	BYTE				attrSize;
	BYTE				privSize;
	char				data[1];

	int		size()
	{
		return attrSize + privSize + ((char*)data - (char*)&version);
	}

} DB_UNIT_DATA;

//关系的权限
typedef struct tagRelationPrivilege 
{
	BYTE TemplateId;
	BYTE Layer;
	BYTE OperationId;

} RelationPrivilege, *PRelationPrivilege;

typedef struct _db_Unit_Priv
{
	BYTE				version;
	BYTE				privCount;
	RelationPrivilege	priv[1];

	int	size() const
	{
		return privCount * sizeof(RelationPrivilege) + ((char*)&priv - (char*)&version);
	}

	int	size(int nPrivCnt)
	{
		return nPrivCnt * sizeof(RelationPrivilege) + ((char*)&priv - (char*)&version);
	}

} DB_UNIT_PRIV;

// server to db request data structure
typedef struct _s2db_LoadOwnTree_Param
{
	int				tplId;
	const FSGUID	*pGuid;

} S2DB_LOADOWNTREE_PARAM;

typedef struct _s2db_LoadSubUnit_Param
{
	int				tplId;
	int				parentLayer;
	const FSGUID	*pGuid;

} S2DB_LOADSUBUNIT_PARAM;

// client to server protocol data structure defition

typedef struct _c2s_CreateUnit_Req
{
	// Note: this structure can't be variable length.
	C2S_COMOPE_HEADER	comHeader;
	DWORD				tag;
	char				unitName[MAXSIZE_ORGNAME];

} C2S_CREATEUNIT_REQ;

typedef struct _c2s_AddSubUnit_Req
{
	C2S_NEEDCONFIRMOPE_HEADER	nh;
	char						receiverName[MAXSIZE_ROLENAME];

} C2S_ADDSUBUNIT_REQ;

typedef struct _c2s_HighLevel_Join_Req
{
   C2S_NEEDCONFIRMOPE_HEADER    nh;
   char						    receiverName[MAXSIZE_ROLENAME];
}C2S_HIGH_LEVEL_JOIN_REQ;

typedef struct _c2s_RemoveSubUnit_Req
{
	C2S_COMOPE_HEADER		comHeader;
	FSGUID					unitGuid;

} C2S_REMOVESUBUNIT_REQ;

typedef struct _c2s_ChangeUnitOwner_Req
{
    C2S_COMOPE_HEADER		comHeader;
	FSGUID					unitGuid;
} C2S_CHANGE_UNIT_OWNER_REQ;

typedef struct _c2s_BreakUnit_Req
{
	C2S_COMOPE_HEADER		comHeader;

} C2S_BREAKUNIT_REQ;

typedef struct _c2s_LeaveUnit_Req
{
	C2S_COMOPE_HEADER		comHeader;

} C2S_LEAVEUNIT_REQ;

typedef struct _c2s_ChatInUnit_Req
{
	C2S_COMOPE_HEADER		comHeader;
	WORD					msgSize;
	char					msg[1];

} C2S_CHATINUNIT_REQ;

typedef struct _c2s_ForbidChat_Req
{
	C2S_COMOPE_HEADER		comHeader;
	FSGUID					unitGuid;

} C2S_FORBIDCHAT_REQ;

typedef struct _c2s_UnForbidChat_Req
{
	C2S_COMOPE_HEADER		comHeader;
	FSGUID					unitGuid;

} C2S_UNFORBIDCHAT_REQ;

typedef struct _c2s_PubAnnouncement_Req
{
	C2S_COMOPE_HEADER		comHeader;
	WORD					msgSize;
	char					msg[1];

} C2S_PUBANNOUNCEMENT_REQ;

typedef struct _c2s_GetSubList_Req
{
	C2S_COMOPE_HEADER		comHeader;
	BYTE					listType;
	BYTE					curPageNo;

	// guid or empty determine by listType
	char					data[1];	

} C2S_GETSUBLIST_REQ;

typedef struct _c2s_ReqRecruitInfo
{
	C2S_COMOPE_HEADER		comHeader;
	BYTE                    curPageNo;
	BYTE                    reqLayer;
}C2S_RECRUIT_INFO_REQ;

typedef struct _c2s_AddRecruitInfo
{
    C2S_COMOPE_HEADER		comHeader;
}C2S_ADD_RECRUIT_INFO;

typedef struct _c2s_DelRecruitInfo
{
    C2S_COMOPE_HEADER		comHeader;
}C2S_DEL_RECRUIT_INFO;

typedef struct _c2s_GetAnnouncement_Req
{
	C2S_COMOPE_HEADER		comHeader;
    int                     anaucementversion;
} C2S_GETANNOUNCEMENT_REQ;

typedef struct _c2s_City_Res_Req 
{
	C2S_COMOPE_HEADER		comHeader;
	int						arrayCityRes[CITY_RES_TYPE_COUNT];
} C2S_CITY_RES_REQ;

typedef struct _c2s_City_Info_Req 
{
	C2S_COMOPE_HEADER		comHeader;
	int						nType;
} C2S_CITY_INFO_REQ;

typedef struct _c2s_Set_CityTaxRate_Req
{
	C2S_COMOPE_HEADER		comHeader;
	int						nTaxRate;

} C2S_SET_CITYTAXRATE_REQ;

typedef struct _c2s_Req_Unit_Info
{
	C2S_COMOPE_HEADER       comHeader;
	char                    szOwnerName[MAXSIZE_ROLENAME];
	BYTE                    nLayer;

	TongUIReqeustCode       code;
}C2S_REQ_UNIT_INFO;

// server to client protocol data structure definition

typedef struct _s2c_Social_City_Info 
{
	S2C_COMOPE_HEADER		comHeader;
	char					szOwnerName[COMMON_CLIENT_MSG_LEN_16+1];
	int						nCityMapId;
	int						nCityLordId;
	int						nCityProduceBuffTemplateID;
	int						nCityConsumeBuffTemplateID;
	int						nInfoType;
	int						nTaxRate;
	int						arrayCityRes[CITY_RES_TYPE_COUNT];	
	int                     nTongMemberCount;
	int                     nShizuCount;
	int						nDevelopment;
	int						nTiredness;

} S2C_SOCIAL_CITY_INFO;

typedef struct _s2c_Social_MsgCode
{
	S2C_COMOPE_HEADER		comHeader;
	BYTE					msgCode;

} S2C_SOCIAL_MSGCODE;

typedef struct _s2c_Social_Msg
{
	S2C_COMOPE_HEADER		comHeader;
	WORD					msgLen;
	char					msg[1];
	
} S2C_SOCIAL_MSG;

typedef struct _s2c_Req_Confirm
{
	S2C_COMOPE_HEADER		comHeader;
	WORD					reqSize;
	WORD					hitMsgLen;
	char					data[1];

} S2C_REQ_COMFIRM;

typedef struct _s2c_GetSubList_Ret
{
	S2C_COMOPE_HEADER		comHeader;
	WORD					maxPlayerCount;
	WORD					onlinePlayerCount;
	BYTE					unitCount;
	BYTE					listType;
	char					listLayer;
	BYTE					curPageNo;
	int                     cityWorldId;
	int                     poolWorldId;

	// array of SU_UNITTRANSFER_INFO
	char					data[1];

} S2C_GETSUBLIST_RET;

typedef struct _s2c_SocialInfoBroadCast
{
	S2C_COMOPE_HEADER		comHeader;
	
	int  nNpcID;
	char szGensName[MAXSIZE_ORGNAME];
	char szTongName[MAXSIZE_ORGNAME];
	BYTE isGensOwner;
	BYTE isTongOwner;

}S2C_SOCIAL_INFO_BROAD_CAST;

typedef struct _s2c_GetRecruit_Ret
{
	S2C_COMOPE_HEADER		comHeader;
	BYTE                    nInfoCount;
	BYTE                    nPageNo;
	// array of S2C_SOCIAL_RECRUIT_INFO
	char					data[1];
	
} S2C_GETRECRUIT_RET;

typedef struct _s2c_GetAnnouncement_Ret
{
	S2C_COMOPE_HEADER		comHeader;
	WORD					announcementLen;
	int                     announceversion;
	char					data[1];

} S2C_GETANNOUNCEMENT_RET;

typedef struct _s2c_Unit_Info_Ret
{
   S2C_COMOPE_HEADER		comHeader;
   char                     szUnitName[MAXSIZE_ORGNAME];
   char                     szOwnerName[MAXSIZE_ROLENAME];
   int                      nSubUnitNum;
   int                      nLayer;

   int                      nPlayerAvgLevel; //shizu only
   int                      nCityMapID;      //zhuhou only
   int                      nPoolMapID;      //zhuhou only

   TongUIReqeustCode        code;
}S2C_UNIT_INFO_RET;

// other data structure
#define MAX_SOCIETY_LAYER_COUNT 5

struct SocietyTemplateInfo
{
	SocietyTemplateInfo()
	{
		Id = 0;
		memset(Name, 0 ,sizeof(Name));
		memset(Desc, 0 ,sizeof(Desc));
		LayerCount = 0;
	}

	int Id;																//模版编号
	char Name[COMMON_CLIENT_MSG_LEN_128];								//模版名称
	char Desc[COMMON_CLIENT_MSG_LEN_128];								//模版描述
	int LayerCount;														//层数
};

#define MAX_OPERATION_CONTROLLER_COUNT 5

struct SocietyLayerOperationController
{
	SocietyLayerOperationController()
	{
		memset(Controller, 0, sizeof(Controller));
		memset(Name, 0, sizeof(Name));
		memset(Desc, 0, sizeof(Desc));
		memset(Event, 0, sizeof(Event));		
		memset(ReturnController, 0, sizeof(ReturnController));
		NeedConfirm = 0;
		Id = 0;		//操作
		LayerID = 0;	//层号

	}
	
	int	Id;		//操作
	int LayerID;	//层号
	char Controller[COMMON_CLIENT_MSG_LEN_128];							//控件
	char Event[COMMON_CLIENT_MSG_LEN_128];								//触发事件
	char Name[COMMON_CLIENT_MSG_LEN_128];								//名称
	char Desc[COMMON_CLIENT_MSG_LEN_512];								//描述		
	char ReturnController[COMMON_CLIENT_MSG_LEN_128];					//返回数据的控件
	int NeedConfirm;													//是否需要确认
};

struct SocietyLayerOperation
{
	SocietyLayerOperation()
	{
		Id = 0;		
	}
	
	int Id;			//操作编号
	SocietyLayerOperationController Controllers[MAX_OPERATION_CONTROLLER_COUNT];	//控件
};

struct SocietyLayerInfo
{
	SocietyLayerInfo()
	{
		memset(Controller, 0, sizeof(Controller));
		memset(Name, 0, sizeof(Name));
		memset(Desc, 0, sizeof(Desc));
		OperationCount = 0;
	}

	char Controller[COMMON_CLIENT_MSG_LEN_128];							//控件
	char Name[CLIENT_NAME_AND_TITLE_MAX];								//层次名称
	char Desc[COMMON_CLIENT_MSG_LEN_512];								//层次描述
	int OperationCount;													//操作数量
	int nLayerID;
	SocietyLayerOperation Operations[enSUO_Num * 2];//层次操作
};

struct SocietyInfoIndex
{
	SocietyInfoIndex()
	{
		TemplateId = 0;
		Layer = 0;
		Operation = 0;
	}

	int TemplateId;
	int Layer;
	int Operation;
};

struct PlayerLayerPrivilegeInfo
{
	PlayerLayerPrivilegeInfo()
	{
		OperationCount = 0;
		memset(Operation, 0, sizeof(Operation));
	}

	int OperationCount;
	int Operation[enSUO_Num];
};

typedef struct tagSocialRecruitBaseInfo
{
    FSGUID               guid;
	unsigned long        dueTime;

	tagSocialRecruitBaseInfo()
		:dueTime(0){/*DoNothing at all*/}
}SocialRecruitBaseInfo;

typedef struct tagSocialCommonRecruitInfo
{
   char                     szUnitName[MAXSIZE_ORGNAME];
   char                     szOwnerName[MAXSIZE_ROLENAME];
   unsigned short           nSubUnitCount;
   BYTE                     nLayer;
   BYTE                     bOwnerOnline;
   tagSocialCommonRecruitInfo():nSubUnitCount(0),nLayer(0),bOwnerOnline(false)
   {/*Do Nothing at all*/}
}SocialCommonRecruitInfo;

typedef struct tagSocialTongInfo
{
	signed short          nCityMapID;
	signed short          nPoolMapID;
	tagSocialTongInfo():nCityMapID(0),nPoolMapID(0){/*Do Nothing at all*/}
}SocialTongInfo;

typedef struct tagSocialRecruitInfo
{
    SocialRecruitBaseInfo   baseInfo;
    SocialCommonRecruitInfo comInfo;
	SocialTongInfo          tongInfo;

	bool operator < (const tagSocialRecruitInfo &);
}SocialRecruitInfo;

typedef struct tagS2C_SOCIAL_RECRUIT_INFO
{
	int                     dueTime;
    SocialCommonRecruitInfo comInfo;
	SocialTongInfo          tongInfo;

}S2C_SOCIAL_RECRUIT_INFO;

typedef struct tagSCRIPT_DATA
{
	unsigned long nCount;
	unsigned long Data[MAX_SOCIAL_SCRIPT_DATA_NUM_NEEDSAVE];
}SCRIPT_DATA;

#pragma pack(pop)
} // end of namespace SocialRelation

using namespace SocialRelation;

#endif