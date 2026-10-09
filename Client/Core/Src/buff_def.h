//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_def
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _BUFF_DEF_H_
#define _BUFF_DEF_H_

#define interface struct

class Buff;
class BuffList;

#define MAX_BUFF_COUNT			(45000 * 100)
#define BUFF_GRANULARITY		( 10000 )
#define MAX_BUFF_DESC			( 512 )
#define MAX_BUFF_IMAGE			( 16 )
#define MAX_ACTION_STRING		( 128 )
#define MAX_TITLE_NAME			( 32 )
#define MAX_BUFF_EFFECT_COUNT	( 5 )
#define MAX_ATTRIBUTE			(10)
#define BUFF_INFINITE_TIME		(-1)
#define BUFF_INVALID			(-1)
#define BUFF_VERSION			(1)
#define BUFF_TEMP_SIZE			(1024)
#define BUFF_MININTERVAL		(1)

#define BUFF_MAX_TAB_COUNT		(8000)
#define BUFF_MAX_EF_I_COUNT		(8000)
#define BUFF_MAX_EF_P_COUNT		(1500)


#define MAX_BUFFFUN				8
#define BUFF_MAX_PARAM			5
#define BUFF_SCAN_INTERVAL		9
#define BUFF_MAX_ADDIOP			3
#define PATTERN					"%[^(]"
#define FUNPDELIMITER			","
#define FUNLDELIMITER			"("
#define FUNLDELIMITERC			'('
#define FUNRDELIMITERC			')'
#define ANDDELIMITERC			'&'
#define ORDELIMITERC			'|'
#define NOTDELIMITERC			'!'
#define FUNPDELIMITERC			','

#define FUNMULDELIMITER			'*'
#define FUNDIVDELIMITER			'/'
#define FUNADDDELIMITER			'+'
#define FUNSUBDELIMITER			'-'

#define MISTAKETIME (5)

//注意操作符顺序

enum
{
	effect_i_lanuch_start,			//开始触发
	effect_i_lanuch_final,			//结束触发
	effect_i_lanuch_persistent,		//持续作用
	effect_i_lanuch_interval,		//间隔触发
	effect_i_lanuch_timepoint,		//定时触发
};

enum
{
	buff_event_type_none,
	buff_event_type_skillin,
	buff_event_type_skillout,
	buff_event_type_damagein,
	buff_event_type_damageout,
	buff_event_type_npcdeathin,
	buff_event_type_npcdeathout,
	buff_event_type_buffin,
	buff_event_type_buffout,
	buff_event_type_creaturedeath,
	buff_event_type_explodeout,
	buff_event_type_blood,
	buff_event_type_mana,
	buff_event_type_chgmap,
	buff_event_type_explodecalc,
	buff_event_type_delayskillout,
	buff_event_type_finalskillout,

	//--------------------------------->
	buff_event_type_timer,
	buff_event_type_end
};

enum
{
	buff_event_format_skillid	= 0x1,
	buff_event_format_damageid	= 0x2,
	buff_event_format_buffid	= 0x4,
	buff_event_format_buffobj	= 0x8,
	buff_event_format_event		= 0x10,
	buff_event_format_blood		= 0x20,
	buff_event_format_mana		= 0x40,

	buff_event_format_all		= 
	buff_event_format_skillid	|
	buff_event_format_damageid	|
	buff_event_format_buffid	|
	buff_event_format_buffobj	|
	buff_event_format_event		|
	buff_event_format_blood		|
	buff_event_format_mana		,
	
	buff_event_format_end
};

enum
{
	effect_p_op_none,
	effect_p_op_and,
	effect_p_op_or,
	effect_p_op_not,
	effect_p_op_end
};

enum
{
	buff_ret_false	=	0x0,
	buff_ret_true	=	0x1,
	buff_ret_keep	=	0x2,
	buff_ret_del	=	0x4,
	buff_ret_break	=	0x8,
	buff_ret_force	=	0x10,
	buff_ret_kill	=	0x20,
};

enum
{
	buff_event_relation_recver,
	buff_event_relation_sender,
};

enum
{
	buff_addi_op_none,
	buff_addi_op_add,
	buff_addi_op_sub,
	buff_addi_op_div,
	buff_addi_op_mul,

	buff_addi_op_end
};

typedef struct _Buff_Param 
{
	_Buff_Param( ):nCount(0){ memset( nParam, 0, sizeof(nParam) ); }
	void Clear( ){ nCount = 0; memset( nParam, 0, sizeof(nParam) ); }
	int nParam[BUFF_MAX_PARAM];
	int nCount;
	int& operator[]( int nIndex )
	{ return  ( nIndex >= 0 && nIndex < BUFF_MAX_PARAM ) ? nParam[nIndex] : nParam[0]; }
	int operator=( int nValue )
	{ nParam[nCount++] = nValue;  return nCount; }
	int operator( )( )
	{ return nCount; }
	
}BUFF_PARAM,*PBUFF_PARAM;

typedef struct _Buff_Env_Param 
{
	_Buff_Env_Param( ) : LocalVar(),
		nEventSender(0),
		nEventRecever(0),
		nEvent(0),
		nEventFormat(0),
		nEventType(0),
		nEventValue(0),
		nEventRelation(0),
		nBuffSender(0),
		nBuffRecever(0),
		nBuffCap(0),
		nBuffPileCount(0),
		nBuffTempID(0),
		nBuffID(0),
		nBuffGroup(0),
		nBuffCate(0),
		pBuff(0),
		pStream(0),
		nVolume(0),
		nRet(0),
		nBeOPDec( 0 )
	{ }
	///////////////////
	int nEventSender;
	int nEventRecever;
	int nEvent;
	int nEventFormat;
	int nEventType;
	int nEventValue;
	int nEventRelation;
	///////////////////
	int nBuffSender;
	int nBuffRecever;
	int nBuffCap;
	int nBuffPileCount;
	int nBuffTempID;
	int nBuffID;
	int nBuffGroup;
	int nBuffCate;
	int nBeOPDec;
	Buff* pBuff;
	///////////////////
	unsigned char* pStream;
	int nVolume;
	///////////////////
	int nRet;
	///////////////////
	BUFF_PARAM	LocalVar;
	///////////////////
}BUFF_ENV_PARAM,*PBUFF_ENV_PARAM;

typedef int (*PBUFFACTION)(
	BUFF_ENV_PARAM&,
	BUFF_PARAM&);

struct _Buff_Action
{
	char szName[MAX_ACTION_STRING];
	PBUFFACTION	BuffAction;
};

struct _BuffPair {
	unsigned long ulBuffTempID;
	unsigned long ulBuffID;
};

struct _BuffPairExt : _BuffPair {
	int nBuffPileCount;
};

Buff* CreateBuff(
	int nBuffTempID );

#pragma	pack(push, 1)

struct _BUFF_SAVE 
{
	int				nTemplateID;
	int				nPileCount;
	int				nCap;
	unsigned long	ulTime;
	unsigned long	ulTimeStamp;
};

struct _BUFF_SAVE_S 
{
	int nVersion;
	int nCount;
	_BUFF_SAVE BS[1];
};
#pragma pack(pop)
#endif