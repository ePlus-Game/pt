//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright   2008
//
//      Created_datetime : 2008-11-29
//      File_base        : PlayerRealInfo_ComDef
//      File_ext         : h
//      Author           : Wu Shaohui
//      Description      : 
//
//      <Change_list>
//
//////////////////////////////////////////////////////////////////////
#ifndef _PLAYER_REAL_INFO_COM_DEF_H_
#define _PLAYER_REAL_INFO_COM_DEF_H_

#include "GameDataDef.h"

enum enPlayerRealInfoSubProtocol
{
	enPlayerRealInfo_None = -1, 

	enPlayerRealInfo_SetInfo,
	enPlayerRealInfo_GetInfo,

	enPlayerRealInfo_Num,
};

enum enPlayerRealInfoOper
{
	enPlayerRealInfoOper_None = -1,

	enPlayerRealInfoOper_SetInfo,
	enPlayerRealInfoOper_GetInfo,

	enPlayerRealInfoOper_Num,
};

enum enPlayerRealInfoDBOper
{
	enPlayerRealInfoDBOper_None = -1,

	enPlayerRealInfoDBOper_SetInfo,
	enPlayerRealInfoDBOper_GetInfo,

	enPlayerRealInfoDBOper_Num,
};

#define IsValidPRIOper(oper) ((oper > enPlayerRealInfoOper_None) && (oper < enPlayerRealInfoOper_Num))

#pragma pack(push, 1)
/*---------------------NET Struct----------------------------*/
//Set PlayerRealInfo(上行结构)
typedef struct tagPlayerRealInfoForNet
{
	tagPlayerRealInfoForNet() : Sex(0), Age(0), Version(0)
	{
		ZeroMemory(Name,        sizeof(Name)       );
		ZeroMemory(QQNumber,    sizeof(QQNumber)   );
		ZeroMemory(TeleNumber,  sizeof(TeleNumber) );
		ZeroMemory(MobleNumber, sizeof(MobleNumber));
		ZeroMemory(Address,     sizeof(Address)    );
		ZeroMemory(MSNNumber,   sizeof(MSNNumber)  );
		ZeroMemory(ISNumber,    sizeof(ISNumber)   );
		ZeroMemory(UTNumber,    sizeof(UTNumber)   );
	}
	
	BYTE  Sex;									//		性别         byte(1)
	BYTE  Age;									//		年龄         byte(1)
	DWORD Version;
	char Name[MAXSIZE_ROLENAME];				//		姓名         char(17)	
	char Address[MAXSIZE_PALYER_INFO_STRING];	//		所在地区     char(25)
	char QQNumber[MAXSIZE_PALYER_INFO_STRING];	//		QQ           char(25)
	char MSNNumber[MAXSIZE_PLAYER_MSN_STRING];	//		Msn          char(37)
	char ISNumber[MAXSIZE_PALYER_INFO_STRING];	//		IS           char(25)
	char UTNumber[MAXSIZE_PALYER_INFO_STRING];	//		UT           char(25)
	char TeleNumber[MAXSIZE_PALYER_INFO_STRING];//		固定电话     char(25)
	char MobleNumber[MAXSIZE_PALYER_INFO_STRING];//		移动电话     char(25)
} PLAYER_REAL_INFO_UP, *PPLAYER_REAL_INFO_UP;	

//Get PlayerRealInfo(下行结构)
struct PLAYER_REAL_INFO_DOWN : public tagPlayerRealInfoForNet
{
	PLAYER_REAL_INFO_DOWN()
	{	
		ZeroMemory(Consort, sizeof(Consort));
	}	

	char Consort[MAXSIZE_ROLENAME];				//		配偶		 char(17)
};

//Get PlayerRealInfo(上行结构)
typedef struct tagPlayerRealInfoGet
{
	tagPlayerRealInfoGet()
	{
		ZeroMemory(&PlayerGUID, sizeof(PlayerGUID));
	}
	
	FSGUID PlayerGUID;
}PLAYER_REAL_INFO_GET;

typedef struct tagPRIGetReqProtocol
{
	VARLEN_PROTOCOL_HEADER protocHeader;
	PLAYER_REAL_INFO_GET   PlayerInfoGet;
} PLAYER_REAL_INFO_GET_REQ_PROTOC, *PPLAYER_REAL_INFO_GET_REQ_PROTOC;

typedef struct tagPRISetReqProtocol
{
	VARLEN_PROTOCOL_HEADER protocHeader;
	PLAYER_REAL_INFO_UP   playerInfo;
} PLAYER_REAL_INFO_SET_REQ_PROTOC, *PPLAYER_REAL_INFO_SET_REQ_PROTOC;

typedef struct tagPRIGetRetProtocol
{
	VARLEN_PROTOCOL_HEADER protocHeader;
	PLAYER_REAL_INFO_DOWN  playerInfo;
} PLAYER_REAL_INFO_GET_RET_PROTOC, *PPLAYER_REAL_INFO_GET_RET_PROTOC;


/*----------------------DB Struct-----------------*/
typedef struct tagPlayerRealInfoForDB
{
	tagPlayerRealInfoForDB() : Sex(0), Age(0), Version(0)
	{
		ZeroMemory(Name,        sizeof(Name)       );
		ZeroMemory(Consort,     sizeof(Consort)	   );
		ZeroMemory(QQNumber,    sizeof(QQNumber)   );
		ZeroMemory(TeleNumber,  sizeof(TeleNumber) );
		ZeroMemory(MobleNumber, sizeof(MobleNumber));
		ZeroMemory(Address,     sizeof(Address)    );
		ZeroMemory(MSNNumber,   sizeof(MSNNumber)  );
		ZeroMemory(ISNumber,    sizeof(ISNumber)   );
		ZeroMemory(UTNumber,    sizeof(UTNumber)   );
	}
	
	char Name[MAXSIZE_ROLENAME];				//		姓名         char(17)	
	BYTE  Sex;									//		性别         byte(1)
	char Consort[MAXSIZE_ROLENAME];				// 		配偶   	     char(17)	
	BYTE  Age;									//		年龄         byte(1)
	char Address[MAXSIZE_PALYER_INFO_STRING];	//		所在地区     char(25)
	char QQNumber[MAXSIZE_PALYER_INFO_STRING];	//		QQ           char(25)
	char MSNNumber[MAXSIZE_PLAYER_MSN_STRING];	//		Msn          char(37)
	char ISNumber[MAXSIZE_PALYER_INFO_STRING];	//		IS           char(25)
	char UTNumber[MAXSIZE_PALYER_INFO_STRING];	//		UT           char(25)
	char TeleNumber[MAXSIZE_PALYER_INFO_STRING];//		固定电话     char(25)
	char MobleNumber[MAXSIZE_PALYER_INFO_STRING];//		移动电话     char(25)
	DWORD Version;

	static int GetElemt()
	{
		return 12;
	}
}DB_PLAYER_REAL_INFO;

#pragma pack(pop)

#endif