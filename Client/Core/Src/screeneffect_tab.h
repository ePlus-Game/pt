//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/01/2007 10:42
//      File_base        : screeneffect_tab
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _screeneffect_tab_h

#define  _screeneffect_tab_h

#ifndef _SERVER

#include "screeneffect_def.h"
#include "buff_tab.h"

/************************************************************************/
/*                      declare _BaseTable			                    */
/************************************************************************/
/*enum
{
	tabfile_rec_type_string,
	tabfile_rec_type_int,
};//*/

struct ScreenEffect_Record 
{
	char	szName[MAX_SCREENEFFECT_TITLENAME];
	int		nRecordType;
	void*	pStoreBuf;
	int		nSize;
};

class _BaseTable
{
public:
	_BaseTable( void );
	~_BaseTable( void );

public:
	
	int OpenTable( char* szFileName );
	
	int GetRecordCount( void );
	
	void CloseTable( void );

	int LoadRecord(
		int nRow,
		ScreenEffect_Record	Rec[],
		int nMaxCount );

private:
	KTabFile	m_TabFile;
	BOOL		m_bOpen;
};


/************************************************************************/
/*                      declare ScreenEffectTab		                    */
/************************************************************************/

typedef struct _ScreenEffect_Template
{
	int		nEffectID;
	int		nX;
	int		nY;
	char	szImage[MAX_SCREENEFFECT_IMAGE];
	char	szSound[MAX_SCREENEFFECT_SOUND];
}ST,*PST;

class ScreenEffectTab : public _BaseTable
{
public:
	ScreenEffectTab( void );
	~ScreenEffectTab( void );

public:
	static ScreenEffectTab& Singleton( void );

	PST	GetScreenEffect( int nID );
	
	int LoadScreenEffect( void );

private:
	ST		m_ST[MAX_SCREENEFFECT_COUNT];
	int		m_nScreenEffectCount;
	int		m_nLoad;
};

#endif

#endif