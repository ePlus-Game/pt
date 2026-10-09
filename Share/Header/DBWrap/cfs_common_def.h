//////////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-6-30 14:56
//      File_base        : cfs_common_def
//      File_ext         : h
//      Author           : Cooler(liuyujun@263.net)
//      Description      : 
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

#ifndef _CFS__COMMON___DEF____H_____
#define _CFS__COMMON___DEF____H_____

//////////////////////////////////////////////////////////////////////////
// Include region


//////////////////////////////////////////////////////////////////////////
// Macro define region
#define MAXLEN_ACCNAME				32
#define MAXLEN_ROLENAME				16
#define MAXLEN_TONGNAME				16
#define MAXLEN_DATETIME				32
#define MAXLEN_PASSWORD				16
#define MAXLEN_MAILTITLE			32
#define MAXLEN_LOGKEY				32

#define MAXSIZE_MAILTEXT			1024	// Mail text max length
#define MAXCOUNT_MAILPLUS			1		// Mail plus max count

#define SAVETEMPBUFLEN	( 75 * 1024 )
	// Role type
typedef enum enumROLETYPE
{
	enRoleType_Knight = 0, 
	enRoleType_Enchanter, 
	enRoleType_Monstrous, 
	enRoleType_Number,		// Enum type number
	
} enROLETYPE, *enPROLETYPE;

	// Role sex
enum enCFSROLESEX
{
	enRoleSex_Male = 0, 
	enRoleSex_Female
};

	// Mail related define
enum enCFSMAILSENDERTYPE
{
	enMailSenderType_Player = 0, 
	enMailSenderType_GM, 
	enMailSenderType_SYS
};

enum enCFSMAILTYPE
{
	enMailType_Text = 0, 
	enMailType_Plugin
};

enum enCFSMAILSTATE
{
	enMailState_New = 0, 
	enMailState_Read
};

	// Log related define
enum enCFSLOGMODEL
{
	enLogModel_System = 0, 
	enLogModel_Net, 
	enLogModel_DB, 
	enLogModel_Gateway, 
	enLogModel_Trade
};

enum enCFSLOGTYPE
{
	enLogType_Error = 0, 
	enLogType_Welcome, 
	enLogType_Info
};

enum
{
	Proc_Begin,
	Proc_GetRoleList,
	Proc_DeleteRole,
	Proc_CreateRole,
	Proc_GetRoleBaseData,
	Proc_SetRoleBaseData,
	Proc_GetSkillData,
	Proc_SetSkillData,
	Proc_GetItemData,
	Proc_SetItemData,
	Proc_GetTaskData,
	Proc_SetTaskData,
	Proc_GetEnhanceData,
	Proc_SetEnhanceData,
	Proc_GetFriendData,
	Proc_SetFriendData,
	Proc_GetReserveData,
	Proc_SetReserveData,
	Proc_FindRole,
	Proc_Online,
	Proc_Offline,
	Proc_SetGlobal,
	Proc_GetGlobal,

	Proc_Auction,
	Proc_WriteLog,
	Proc_WriteSysDbgLog,
	Proc_Mail,
	Proc_CheckMail,
	Proc_Npc,
	Proc_Social,
	Proc_RecPlayerAct,
	Proc_ClearItemCount,
	Proc_ItemCount,
	Proc_IBShop,
	Proc_SocialRecruit,
	Proc_RecordPlayerReward,
	Proc_RecordPlayerContact,	

	Proc_CreateDBLink,
	Proc_GM_GetGMCmd,

	Proc_GM_RecordPlayerQuestion,
	Proc_GM_GetGMReply,

	Proc_IB_AddCreditPoint,
	Proc_IB_DecCreditPoint,	
	Proc_IB_AddPoint,
	Proc_IB_DecPoint,
	Proc_IB_AllData,
	Proc_IB_Log_AddIbItem,
	Proc_IB_Log_TransferIbItem,	
	Proc_IB_Log_DelIbItem,
	Proc_IB_Log_LogIbItem,

	Proc_LogChat,
	Proc_NoEmploy,

	Proc_Post,
	Proc_Cancel,
	Proc_Employ,
	Proc_Pay,
	Proc_Fire,
	Proc_SearchEmploy,
	Proc_CheckEmploy,

	Proc_Withdraw,
	Proc_RecordC2SProtocol,

	Proc_AddStudent,
	Proc_UpdateStudent,
	Proc_ListStudent,
	Proc_ListMaster,
	Proc_GetMasterReward,
	Proc_SetMasterRewardCfg,

	Proc_SaveInsurance,
	Proc_ClearChatLog,
	Proc_LoadCharacterSet,
	Proc_GetSystemVar,

	Proc_NoChat,
	Proc_NoLogin,
	Proc_CreateRoleBadAnswer,

	Proc_PlusPointSaveToTaxis,
	Proc_PlusPointLoadTaxis,

	Proc_FlushPopularity,
	Proc_GetPopularity,
	Proc_ReCalcPopularity,
	Proc_RefreshShizuPopularity,
	Proc_RefreshZhuhouPopularity,
	Proc_RefreshCombatKillRank,
	Proc_ReCalcCombatKillRank,

	Proc_GetMarriageData,
	Proc_Marry,
	Proc_Divorce,

	Proc_PlayerRealInfo,

	Proc_End
};

#define PN_GETROLELIST			"GetRoleList"
#define PN_DELETEROLE			"DeleteRole"
#define PN_CREATEROLE			"CreateRole"
#define PN_GETROLEBD			"GetRoleBaseData"
#define PN_SETROLEBD			"SetRoleBaseData"//createtime,id
#define PN_GETROLEDATA			"GetRoleData"	//RoleName, FieldName, 
#define PN_SETROLEDATA			"SetRoleData"	//RoleName, FieldName, Value
#define PN_ONLINE				"Online"
#define PN_OFFLINE				"Offline"
#define	PN_FINDROLE				"FindRoleInfo"
#define PN_DONOTEMPLOY			"DoNotEmploy"
#define PN_SET_INSURANCE_DATA	"SetInsuranceData"
#define PN_LOAD_CHARACTER_SET	"LoadCharacterSet"
#define PN_GET_SYS_VAR_AS_INT	"GetSysVarAsInt"
#define PN_GET_SYS_VAR_AS_STRING		"GetSysVarAsString"
#define PN_GET_SYS_VAR_AS_BLOB	"GetSysVarAsBlob"
#define PN_NO_CHAT				"NoChat"
#define PN_NO_LOGIN				"NoLogin"
#define PN_CREATE_ROLE_BAD_ANSWER		"CreateRoleBadAnswer"

//social
#define	PN_SEARCHUNIT			"SearchUnit"
#define	PN_ADDUNIT				"AddUnit"
#define	PN_REMOVEUNIT			"RemoveUnit"
#define	PN_UPDATEUNIT			"UpdateUnit"
#define	PN_SEARCHTREEUP			"SearchTreeUp"
#define	PN_COUNTUNIT			"CountUnit"
#define PN_ADD_RECRUIT			"AddRecruit"
#define PN_DEL_RECRUIT			"DelRecruit"
#define PN_LIST_RECRUIT			"ListRecruit"
#define PN_FLUSH_POPULARITY		"FlushPopularity"
#define PN_GET_POPULARITY		"GetPopularity"
#define PN_RECALC_POPULARITY	"ReCalcPopularity"
#define PN_REFRESH_SHIZU_POPULARITY			"RefreshShizuPopularity"
#define PN_REFRESH_ZHUHOU_POPULARITY		"RefreshZhuhouPopularity"
#define PN_REFRESH_COMBAT_KILL_RANK			"RefreshCombatKillRank"
#define PN_RECALC_COMBAT_KILL_RANK			"ReCalcCombatKillRank"

//mail
#define PN_GETMAILLIST			"GetMailList"
#define PN_GETMAIL				"GetMail"
#define PN_SETMAILDATA			"SetMailData"
#define PN_SENDMAIL				"SendMail"
#define PN_RETURNMAIL			"ReturnMail"
#define PN_DELETEMAIL			"DeleteMail"
#define PN_COUNTNEWMAIL			"CountNewMail"
#define PN_SENDMAILTOALL		"SendMailToAll"
#define PN_CHECKMAIL			"CheckMail"

//auction
#define	PN_AUC_SEARCH			"SearchAuction"
#define	PN_AUC_SELLGOODS		"NewAuction"
#define	PN_AUC_LOCKGOODS		"LockAuction"
#define	PN_AUC_UPDATEPRICE		"BidAuction"
#define	PN_AUC_CHECKOVERDUE		"CheckAuction"	
#define	PN_AUC_UNLOCKGOODS		"UnlockAuction"
#define	PN_AUC_CANCELAUCTION	"CancelAuction"

//ibshop
#define PN_IB_LOADSHELF			"GetIbShelves"
#define	PN_IB_LOADPANEL			"GetIbPanels"
#define PN_IB_LOADCONTENTSTYLE	"GetIbStyles"
#define PN_IB_LOADITEM			"GetIbShelfItem"
#define PN_IB_ROLEDATA			"SetRoleIBData"
#define PN_IB_ADDIBITEM			"AddIbItem"
#define PN_IB_TRANSFERIBITEM	"TransferIbItem"
#define PN_IB_DELIBITEM			"DelIbItem"
#define PN_IB_LOGIBITEM			"LogIbItem"

//log
#define	PN_WRITELOG				"WriteLog"
#define	PN_SYSTEMLOG			"SystemLog"
#define PN_LOGCHAT				"LogChat"
#define PN_CLEAR_CHAT_LOG		"ClearChatLog"

//item bank
#define	PN_CLEARITEMCOUNT		"ClearItemCount"
#define	PN_ITEMCOUNT			"ItemCount"

//global
#define	PN_GETGLOBAL			"GetGlobal"
#define	PN_SETGLOBAL			"SetGlobal"

//npc
#define PN_SAVENPC				"SaveNpc"
#define PN_LOADNPC				"LoadNpc"
#define PN_DELETENPC			"DeleteNpc"

//player action
#define PN_RECPLAYERACTION		"RecordPlayerAction"	//记录玩家行为
#define PN_RECORDPROTOCOL		"RecordProtocol"		//记录上行协议

//player info
#define PN_RECORD_PLAYER_REWARD		"RecordPlayerReward"
#define PN_RECORD_PLAYER_CONTACT	"RecordPlayerContact"
#define PN_RECORD_PLAYER_QUESTION	"CommitQuestion"
#define PN_GET_GM_REPLY				"GetReply"

//other
#define PN_CREATE_DBLINK		"CreateDBLink"
#define PN_GET_GM_CMD			"GetGMCmd"
#define PN_WITHDRAW				"Withdraw"

//employ
#define PN_POST					"Post"
#define PN_CANCEL				"Cancel"
#define PN_EMPLOY				"Employ"
#define PN_PAY					"Pay"
#define PN_FIRE					"Fire"
#define PN_SEARCH_EMPLOY		"SearchEmploy"
#define PN_CHECK_EMPLOY			"CheckEmploy"

//recommender (master & student)
#define PN_ADD_STUDENT			"AddStudent"
#define PN_LIST_MASTER			"ListMaster"
#define PN_LIST_STUDENT			"ListStudent"
#define PN_UPDATE_STUDENT		"UpdateStudent"
#define PN_GET_MASTER_REWARD	"GetMasterReward"
#define PN_SET_MASTERREWARD_CFG	"SetMasterRewardCfg"

//pluspoint top n 
#define PN_SAVE_PLUS_POINT_TAXIS "SavePointPlusToTaxis"
#define PN_LOAD_PLUS_POINT_TAXIS "LoadPointPlusTaxis"

//marriage
#define PN_MARRY				"Marry"
#define PN_DIVORCE				"Divorce"
#define PN_GET_MARRIAGE_DATA	"GetMarriageData"

//col name
#define COLNAMESKILL			"SkillList"
#define COLNAMEITEM				"ItemList"
#define COLNAMETASK				"TaskList"
#define COLNAMEENHAN			"EnhanceList"
#define COLNAMEFRIED			"FriendsList"
#define COLNAMERESVE			"ReserveList"

//mail table
#define COLNAME_MAIL_MAILDATA	"MailData"
#define COLNAME_MAIL_MAILPLUS	"MailPlus"
#define COLNAME_MAIL_MAILMONEY	"PostMoney"
#define COLNAME_MAIL_MAILCOST	"MailCost"

//player real info
#define GET_PLAYER_REAL_INFO	"GetPlayerRealInfo"
#define SET_PLAYER_REAL_INFO	"SetPlayerRealInfo"
#define DELETE_PLAYER_REAL_INFO "DeletePlayerRealInfo"
#endif // _CFS__COMMON___DEF____H_____
