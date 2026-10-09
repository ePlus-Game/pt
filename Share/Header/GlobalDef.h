///////////////////////////////////////////////////////////////
//	File        : GlobalDef.h
//	Author      : chenshanglin
//	Create Time : 2006-6-1
//	Platform    : 
//	Remark      : Define some macros for global using. 
//				  Don't depend on any other header file.
//	History     : 
///////////////////////////////////////////////////////////////
#ifndef _GlobalDef_h
#define _GlobalDef_h

#define		GAME_FPS					18
#define		MAX_ROLE					3
#define		PROTOCOL_SIZE				1
#define		MAXSIZE_ORGNAME				17
#define		MAXSIZE_ROLENAME			17
#define		MAXSIZE_CITYNAME			17
#define		INVALID_TEAM_ID				-1
#define		MAXSIZE_CHATCHANNEL_NAME	5	

// skill related
#define		INVALID_SKILL_INDEX			-1
#define		INVALID_SKILL_ID			0
#define		DEFAULT_SKILL_SPECDATA		0

// buff related
#define		INVALID_BUFF_ID				0

// World related
#define		INVALID_WORLD_ID			-1
#define		INVALID_WORLD_INDEX			-1
#define		INVALID_WORLDLORDNPC_INDEX	-1
#define		MAXSIZE_WORLD_NAME			33

// City related
#define		CITYMAP_GROUP				1
#define		CITYMAP_CATE				1
#define		DEFAULT_CITY_DISCOUNT		0
#define		MIN_CITY_DISCOUNT			80
#define		MAX_CITY_DISCOUNT			100

// Player related
#define		INVALID_PLAYER_ID			0

#ifdef WIN32
#define		snprintf	_snprintf
#define		vsnprintf	_vsnprintf
#endif

enum	enDBProcOpeType
{
	enAuction_DBOpe_Begin = 1,
	enAuction_DBOpe_End = 1025,

	enSocial_DBOpe_Begin = 1026,
	enSocial_DBOpe_End = 2050,

	enNpcSave_DBOpe_Begin = 2051,
	enNpcSave_DBOpe_End =3075,

	enMail_DBOpe_Begin = 3076,
	enMail_DBOpe_End = 4100,

	enGlobal_DBOpe_Begin = 4101,
	enGlobal_DBOpe_End = 5125,

	enChat_DBOpe_Begin = 5126,
	enChat_DBOpe_End = 6150,

	enPlayerMonitor_DBOpe_Begin = 6151,
	enPlayerMonitor_DBOpe_End = 7175,
};

const unsigned short				g_GamePaintLimit		= 36;
const unsigned short				g_dwPingPerSecond		= 10;

#pragma pack(push, 1)

typedef struct _Chat_Protocol_Header
{
	unsigned char	protocol;
	unsigned short	len;
	unsigned char	subProtocol;

} VARLEN_PROTOCOL_HEADER, *PVARLEN_PROTOCOL_HEADER;

#pragma pack(pop)	// #pragma pack(push, 1)

typedef struct _Item_Identifier
{
	int nItemClass;
	int nDetailType;
	int nParticularType;
	int nLevel;
	int nCount;
	unsigned long dwCreditFlag;
	int isMailBind;
} Item_Identifier;

#endif