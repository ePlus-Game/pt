//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_tab
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _BUFF_TAB_H_
#define _BUFF_TAB_H_

/////////////////////////////////////////////////////////////////////////////
//
//              Header Include
//
/////////////////////////////////////////////////////////////////////////////

#include "buff_def.h"
#include "buff_action.h"
#include "KTabFile.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Declare
//
/////////////////////////////////////////////////////////////////////////////

#define BUFF_TABLE_NAME					"/settings/buff/buff.txt"
#define PASSIVE_EFFECT_TABLE_NAME		"/settings/buff/effect_p.txt"
#define INITIATIVE_EFFECT_TABLE_NAME	"/settings/buff/effect_i.txt"
#define PRECOND			"PreCond"
#define DEFAUTPRECOND	"none(0)"
#define INVERSE '!'

enum
{
	tabfile_rec_type_string,
	tabfile_rec_type_int,
};

/////////////////////////////////////////////////////////////////////////////
//
//              Attribute Struct Declare
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//              Action Parser Declare
//
/////////////////////////////////////////////////////////////////////////////
/*
 *	Buff 属性模版
 */
#define CATEGORY_COUNT	3

typedef struct _Buff_Attribute_Template
{
	int nBuffID;
	char szDesc[MAX_BUFF_DESC];
	unsigned long ulPersistTime;
	int	nOffline;
	int nGroup;
	int nLevel;
	int nCategory[CATEGORY_COUNT];
	int nCap;
	int nPileCount;
	int nPileSpec;
	int nSync;
	int nSave;
	unsigned long ulDelayTime;
	int	nPlus;
	int nSender;
	int nDead;
	char szImage[MAX_BUFF_IMAGE];
	int nDispID;
	int nOnceID;
	int nHSpecID;
	int nBSpecID;
	int nFSpecID;
	int nIEffect[MAX_BUFF_EFFECT_COUNT];
	int nPEffect[MAX_BUFF_EFFECT_COUNT];
	int	nSoundID;
	int	nOnceSoundID;
	int nDelayAdd;
	int nDelayAddType;
#ifdef _SERVER
	BuffAction	PreCond;
	BuffAction	ErrProc;
#endif
}BAT,*PBAT;

/*
 *	被动Effect 属性模版
 */
typedef struct _P_Effect_Attribute_Template
{
	int		nEffectID;
	//char	szDesc[MAX_BUFF_DESC];
	int		nFilterPercent;
	int		nEventType;
#ifdef _SERVER
	BuffAction	FilterAttribute;
	BuffAction	ModAttribute;
	BuffAction	OutAction;
#endif
}PEAT,*PPEAT;

/*
 *	主动Effect 属性模版
 */
typedef struct _I_Effect_Attribute_Template
{
	int nEffectID;
	//char szDesc[MAX_BUFF_DESC];
	int nInterval;
	int nLaunchType;
#ifdef _SERVER
	BuffAction	Precond;
	BuffAction	ActionAdd;
	BuffAction	ActionDec;
#endif

}IEAT,*PIEAT;

struct _Record 
{
	char szName[MAX_TITLE_NAME];
	int nRecordType;
	void* pStoreBuf;
	int nSize;
};

/////////////////////////////////////////////////////////////////////////////
//
//              Class Declare
//
/////////////////////////////////////////////////////////////////////////////

class BaseTable
{

public:

	BaseTable( );
	~BaseTable( );

public:

	int OpenTable( char* szFileName );
	int GetRecordCount( );
	void CloseTable( );

	int LoadRecord(
		int nRow,
		_Record	Rec[],
		int nMaxCount );

private:
	KTabFile	m_TabFile;
	BOOL		m_bOpen;
};

/*
 *	
 */

class BuffTable : public BaseTable
{
public:

	BuffTable( );
	~BuffTable( );

public:

	int Load(  );

	PBAT	GetBuff( int nID );
	PPEAT	GetPEffect( int nID );
	PIEAT	GetIEffect( int nID );

	static BuffTable& Singleton( );
private:

	int LoadBuff( );
	int LoadPEffect( );
	int LoadIEffect( );
private:

	BAT		m_BAT[BUFF_MAX_TAB_COUNT];
	PEAT	m_PEAT[BUFF_MAX_EF_P_COUNT];
	IEAT	m_IEAT[BUFF_MAX_EF_I_COUNT];

	int		m_nBuffCount;
	int		m_nPEATCount;
	int		m_nIEATCount;

	int		m_nLoad;
};

#endif