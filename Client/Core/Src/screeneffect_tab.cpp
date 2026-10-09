//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/01/2007 11:35
//      File_base        : screeneffect_tab
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "screeneffect_tab.h"

#ifndef _SERVER

#define SCREENEFFECT_TABLE_NAME					"/settings/screeneffect.txt"

/************************************************************************/
/*                      implement _BaseTable                            */
/************************************************************************/
_BaseTable::_BaseTable( )
{
	m_bOpen = FALSE;
}

_BaseTable::~_BaseTable( )
{
}

int _BaseTable::OpenTable( char* szFileName )
{
	if( m_bOpen )
		return FALSE;

	m_bOpen = m_TabFile.Load( szFileName );

	return m_bOpen;
}

int _BaseTable::GetRecordCount( )
{
	if( !m_bOpen )
		return FALSE;

	return m_TabFile.GetHeight( );
}

void _BaseTable::CloseTable( )
{
	if( m_bOpen )
	{
		m_TabFile.Clear( );
		m_bOpen = FALSE;
	}
}

int _BaseTable::LoadRecord(
	int nRow,
	ScreenEffect_Record	Rec[],
	int nMaxCount )
{
	if( !m_bOpen )
		return FALSE;

	int nCount = m_TabFile.GetWidth( );

	nCount = nCount > nMaxCount ? nMaxCount : nCount;

	for( int nLoopCount = 0; nLoopCount < nCount; nLoopCount++ )
	{
		switch( Rec[nLoopCount].nRecordType ) 
		{
		case tabfile_rec_type_string:
			{
				m_TabFile.GetString( 
					nRow, Rec[nLoopCount].szName, "", 
					(LPSTR)Rec[nLoopCount].pStoreBuf, 
					Rec[nLoopCount].nSize );
			}	
			break;
		case tabfile_rec_type_int:
			{
				m_TabFile.GetInteger( 
					nRow, Rec[nLoopCount].szName, 0, 
					(int*)Rec[nLoopCount].pStoreBuf );
			}
			break;
		default:
			break;
		}
	}

	return TRUE;
}

/************************************************************************/
/*                      implement _BaseTable                            */
/************************************************************************/
ScreenEffectTab::ScreenEffectTab( )
{
	memset( m_ST, -1, sizeof(m_ST) );
	m_nScreenEffectCount	=	0;
	m_nLoad = FALSE;
}

ScreenEffectTab::~ScreenEffectTab( )
{
}

ScreenEffectTab& ScreenEffectTab::Singleton( void )
{
	static ScreenEffectTab st;
	
	return st;
}

int ScreenEffectTab::LoadScreenEffect( void )
{
	int nRet = OpenTable( SCREENEFFECT_TABLE_NAME );
	
	if( nRet )
	{
		m_nScreenEffectCount = GetRecordCount( );

		ST ScreenEffectTemp;
		for( int nLoopCount = 1; nLoopCount < m_nScreenEffectCount; nLoopCount++ )
		{
			memset(&ScreenEffectTemp, 0 ,sizeof(ScreenEffectTemp) );

			ScreenEffect_Record	rec[] = 
			{
				{ "ID",			tabfile_rec_type_int, &ScreenEffectTemp.nEffectID,		sizeof(int) },
				{ "ScreenX",	tabfile_rec_type_int, &ScreenEffectTemp.nX,		sizeof(int) },
				{ "ScreenY",	tabfile_rec_type_int, &ScreenEffectTemp.nY,		sizeof(int) },
				{ "Image",		tabfile_rec_type_string, &ScreenEffectTemp.szImage,	MAX_SCREENEFFECT_IMAGE },
				{ "Sound",	tabfile_rec_type_string, &ScreenEffectTemp.szSound, MAX_SCREENEFFECT_SOUND },
			};

			this->LoadRecord( nLoopCount + 1, rec, sizeof( rec ) / sizeof( ScreenEffect_Record ) );

			if( ScreenEffectTemp.nEffectID >0 && ScreenEffectTemp.nEffectID < MAX_SCREENEFFECT_COUNT )
			{
				m_ST[ScreenEffectTemp.nEffectID] = ScreenEffectTemp;
			}

		}

		this->CloseTable( );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


PST ScreenEffectTab::GetScreenEffect( int nID )
{

	if( nID < 1 || nID > MAX_SCREENEFFECT_COUNT )
		return NULL;
	else
	{
		if( m_ST[nID].nEffectID!= SCREENEFFECT_INVALID )
			return &m_ST[nID];
		else
			return NULL;
	}

	return NULL;
}

#endif