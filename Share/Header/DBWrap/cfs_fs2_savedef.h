//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-6-23 15:57
//      File_base        : cfs_fs2_savedef
//      File_ext         : h
//      Author           : Cooler(liuyujun@263.net)
//      Description      : FSOnline2 DB Save struct define
//
//      <Change_list>
//
//      Example:
//      {
//      Change_datetime  : year-month-day hour:minute
//      Change_by        : changed by who
//      Change_purpose   : change reason
//      }
//////////////////////////////////////////////////////////////////////////

#ifndef _CFS__FS2___SAVEDEF____H_____
#define _CFS__FS2___SAVEDEF____H_____

//////////////////////////////////////////////////////////////////////////
// Include region
#include "cfs_common_def.h"

//////////////////////////////////////////////////////////////////////////
// Macro define region
#define VERSION_CURDBSAVEITEM		0x10000000

#pragma pack(push, 1)

//////////////////////////////////////////////////////////////////////////
// Struct define region

	// Role base info column
	//  db max size 256 bytes
typedef struct tagFS2DBBASEINFOCOL
{
	int		nEnterMapX;
	int		nEnterMapY;
	DWORD	dwEnterInstanceID;
	int		nRememberSubworldId;
	int		nRememberPosX;
	int		nRememberPosY;
}FS2DBBASEINFOCOL, *PFS2DBBASEINFOCOL;

	// Role numeric info column
	//  db max size 256 bytes
typedef struct tagFS2DBNUMINFOCOL
{
	DWORD			unExp;
	int				nStrength;
	int				nDexterity;
	int				nConstitution;
	int				nIntellect;
	int				nLife;
	int				nMana;
	int				nMaxLife;
	int				nMaxMana;
	BOOL			bCityAutoAddExp;
	int				nWeightMax;
	DWORD			nSkillExp;
	int				nPKValue;
	unsigned short	nMarriedTimes;
}FS2DBNUMINFOCOL, *PFS2DBNUMINFOCOL;

	// Some passwords use in game info column
	//  db max size 256 bytes
typedef struct tagFS2DBPSWINFOCOL
{
	char	szBoxPassword[MAXLEN_PASSWORD+1];
}FS2DBPSWINFOCOL, *PFS2DBPSWINFOCOL;

	// Tong and war info column
	//  db max size 256 bytes
typedef struct tagFS2DBTONGWARINFOCOL
{
}FS2DBTONGWARINFOCOL, *PFS2DBTONGWARINFOCOL;

	// Role's pet info column
	//  db max size 256 bytes
typedef struct tagFS2DBPETINFOCOL
{
}FS2DBPETINFOCOL, *PFS2DBPETINFOCOL;

	// Role offline playing info column
	//  db max size 256 bytes
// typedef struct tagFS2DBOFFPLAYINFOCOL
// {
// 	unsigned int uAntiEnthrallOfflineTime;
// 	unsigned int uAntiEnthrallOnlineTime;
// 	
//  	bool	bIsAutoUnlocking;
//  	long	lAutoUnlockTime;
// }FS2DBOFFPLAYINFOCOL, *PFS2DBOFFPLAYINFOCOL;

	// Role compensatory info column
	//  db max size 256 bytes
typedef struct tagFS2DBCOMINFOCOL
{
}FS2DBCOMINFOCOL, *PFS2DBCOMINFOCOL;

#define MAX_INSTANCE_TEMPLATE_ID 300//这个必须保证大于等于服务器端的INSTANCE_SUBWORLD_START

	// Role other info column
	//  db max size 256 bytes
typedef struct tagFS2DBOTHERINFOCOL
{
	unsigned long nTaisuiWheeledTimes;
	unsigned long nTaisuiAvailableTimes;
	unsigned long nResetPosInfo;
	unsigned long nReserved;
}FS2DBOTHERINFOCOL, *PFS2DBOTHERINFOCOL;

	// Role reserve info column
	//  db max size 256 bytes
typedef struct tagFS2DBRSVINFOCOL
{
}FS2DBRSVINFOCOL, *PFS2DBRSVINFOCOL;

	// Role client option info column
	//  db max size 256 bytes
typedef struct tagFS2DBSETINFOCOL
{
}FS2DBSETINFOCOL, *PFS2DBSETINFOCOL;

typedef struct tagFS2DBLISTCOLHEADER
{
	int nListSize;
	void *pListData;	// point to list data address
}FS2DBLISTCOLHEADER, *PFS2DBLISTCOLHEADER;

#define MAX_PLUS_POINT_COUNT 20
#define MAX_ACTIVE_DEGREE_DAY_COUNT 7
#define FS2DB_RESERVED1_COL_VERSION 1

typedef struct tagFS2DBRESERVED1COL
{
	DWORD BadAnswerStartTime;
	DWORD BadAnswerCount;
	DWORD BadAnswerState;
	DWORD BadAnswerStageStartTime;
	DWORD ExpInsuranceLastRewardTime;
	int   ExpInsuranceCurRewardExp;
	int   QuestInsurnaceValid;
	int   QuestInsuranceRewardValue;
	DWORD PlusPoint[MAX_PLUS_POINT_COUNT];	
	DWORD LongTermBadAnswerStartTime;
	DWORD LongTermBadAnswerCount;
	DWORD PlusPointRecord[MAX_PLUS_POINT_COUNT];
} FS2DBRESERVED1COL, *PFS2DBRESERVED1COL;

typedef struct tagFS2DBBASECOLSET
{
	// Mask MASK_SINGLECOLSET control follow member valid -->
	char			szTongGUID[MAXLEN_LOGKEY+1];
	char			szAccountName[MAXLEN_ACCNAME];
	char			szRoleName[MAXLEN_ROLENAME+1];
	char			szGUID[MAXLEN_LOGKEY+1];

	unsigned int	unVersion;			// Auto generate by DB Module
	unsigned int	unRoleID;			// Unique role ID, auto generate by MYSQL
	enCFSROLESEX	enRoleSex;
	enROLETYPE		enRoleType;
	int				nRoleLevel;
	unsigned int	unMoney;
	unsigned int	unMoneyInBox;
	unsigned int	unTicket;
	unsigned int	unPlayedTime;		// Total play game time (in seconds)
	unsigned int	unCreateDate;		// Auto generate by DB Module(time stamp)
	unsigned int	unLastPlayingDate;	// (time stamp)
	unsigned int	unLastPlayingIP;
	//unsigned int	unNoChatIn;			// Forbid chat in minutes
	//unsigned int	unNoLoginIn;		// Forbid login in minutes
	BOOL			bUseRevivePosition;
	int				nReviveMapID;
	int				nReviveMapX;
	int				nReviveMapY;
	int				nEnterMapID;
	int				nPortrait;
	int				nSpyLevel;
	int				nSkillSeries;
	DWORD			dwEmployTime;
	//IB System
	//Begin-------------------------------------------------------------------
	int				nMaxCreditPoint;
	int				nCreditPoint;
	unsigned char	uCreditState;
	DWORD			uReturnDate;
	int				nPoint;	
	int             nPointPlus;
	DWORD			dwTotolJinshabi;
	DWORD			dwRecentJinshabi;
	DWORD			dwRecentlyTime;	
	//End---------------------------------------------------------------------

	DWORD			dwNoChatTime;
	DWORD			dwCombatOrg;
	DWORD			dwCombatScore;
	// <-- End mask MASK_SINGLECOLSET control

	FS2DBBASEINFOCOL	tagBaseInfoCol;				// Mask MASK_BASEINFOCOL control
	FS2DBNUMINFOCOL		tagNumericInfoCol;			// Mask MASK_NUMINFOCOL control
	FS2DBPSWINFOCOL		tagPswInfoCol;				// Mask MASK_PSWINFOCOL control
	FS2DBTONGWARINFOCOL	tagTongWarInfoCol;			// Mask MASK_TONGWARINFOCOL control
	FS2DBPETINFOCOL		tagPetInfoCol;				// Mask MASK_PETINFOCOL control
//	FS2DBOFFPLAYINFOCOL	tagOffLineInfoCol;			// Mask MASK_OFFPLAYINFOCOL control
	FS2DBCOMINFOCOL		tagCompensatoryInfoCol;		// Mask MASK_COMINFOCOL control
	FS2DBOTHERINFOCOL	tagOtherInfoCol;			// Mask MASK_OTHERINFOCOL control
	FS2DBRSVINFOCOL		tagReserveInfoCol;			// Mask MASK_RSVINFOCOL control
	FS2DBSETINFOCOL		tagSettingInfoCol;			// Mask MASK_SETINFOCOL control
}FS2DBBASECOLSET, *PFS2DBBASECOLSET;

typedef struct tagFS2DBSAVEITEM
{
	FS2DBBASECOLSET			tagBaseColSet;
	FS2DBLISTCOLHEADER		tagSkillListCol;		// Mask MASK_SKILLLISTCOL control
	FS2DBLISTCOLHEADER		tagItemListCol;			// Mask MASK_ITEMLISTCOL control
	FS2DBLISTCOLHEADER		tagTaskListCol;			// Mask MASK_TASKLISTCOL control
	FS2DBLISTCOLHEADER		tagEnListCol;			// Mask MASK_ENHANCELISTCOL control
	FS2DBLISTCOLHEADER		tagFriListCol;			// Mask MASK_FRILISTCOL control
	FS2DBLISTCOLHEADER		tagRsvListCol;			// Mask MASK_RSVLISTCOL control
}FS2DBSAVEITEM, *PFS2DBSAVEITEM;

typedef struct tagDBTASK_ROLELISTITEM
{
	char szRoleName[MAXLEN_ROLENAME+1];
	enCFSROLESEX enRoleSex;
	enROLETYPE enRoleType;
	unsigned char byRoleLevel;
	unsigned int unDeadSeconds;
	unsigned int unNoLoginMinutes;
	unsigned int unNoChatMinutes;
	unsigned int unLastLoginMapID;
	int	nPortrait;
}DBTASK_ROLELISTITEM, *PDBTASK_ROLELISTITEM;

typedef struct tagDBTASK_MAILINFO
{
	char szSenderName[MAXLEN_ROLENAME+1];
	char szReceiverName[MAXLEN_ROLENAME+1];
	char szTile[MAXLEN_MAILTITLE+1];
	unsigned int nPostMoney;
	unsigned int nMailCost;
	enCFSMAILSENDERTYPE enSenderType;
	enCFSMAILTYPE enMailType;
	unsigned char bHasApp;//是否有附件
}DBTASK_MAILINFO, *PDBTASK_MAILINFO;

typedef struct tagDBTASK_MAILLISTITEM
{
	unsigned int unID;			// Unique mail ID, auto generate by MYSQL
	unsigned int unSendTime;	// Auto generate by DB Module(time stamp)
	unsigned int unDeadSeconds;
	enCFSMAILSTATE enState;
	DBTASK_MAILINFO tagMailInfo;
}DBTASK_MAILLISTITEM, *PDBTASK_MAILLISTITEM;

// enDBTask_SendMails
// Variant struct
typedef struct tagDBTASK_SENDMAILS_REQ
{
	DBTASK_MAILINFO tagMailInfo;
	int nMailTextSize;
	int nMailPlusSize;
	char pMailData[1];	// lead with mail data (text data, plus data in sequence)
}DBTASK_SENDMAILS_REQ, *PDBTASK_SENDMAILS_REQ;

typedef struct tagDBTASK_GETMAILDATA_RET
{
	unsigned int unMailID;
	int nMailTextSize;
	int nMailPlusSize;
	char pMailData[1];	// lead with mail data (text data, plus data in sequence)
}DBTASK_GETMAILDATA_RET, *PDBTASK_GETMAILDATA_RET;

typedef struct tagDBTASK_GETROLEMAILLIST_RET
{
	WORD nMailListCount;
	WORD nMailTotalCount;
	char pMailList[1];	// lead with DBTASK_MAILLISTITEM struct item list
}DBTASK_GETROLEMAILLIST_RET, *PDBTASK_GETROLEMAILLIST_RET;

typedef struct tagFS2DBSAVERESERVE
{
	unsigned long dwInstanceId[MAX_INSTANCE_TEMPLATE_ID];//副本编号
}FS2DBSAVERESERVE, *PFS2DBSAVERESERVE;

typedef struct tagFS2DBTitleInfo
{
	WORD TitleIndex;
	DWORD TitleData;
} FS2DB_TITLE_INFO, *PFS2DB_TITLE_INFO;

typedef struct tagFS2DBTitleInfoCol
{
	BYTE Version;
	short SelectedTitle;
	WORD TitleCount;
	FS2DB_TITLE_INFO InfoList[1];
} FS2DB_TITLE_INFO_COL, *PFS2DB_TITLE_INFO_COL;

#pragma pack(pop)

//--------------------------------------------------------------------------------------
struct _DBProcHeader 
{
	unsigned long ulNetID;
	char	szAccName[MAXLEN_LOGKEY+1];
	int		ProcType;
};

#endif // _CFS__FS2___SAVEDEF____H_____
