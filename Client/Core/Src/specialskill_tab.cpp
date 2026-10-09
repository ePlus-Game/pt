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
#include "specialskill_tab.h"

#define SPECIALSKILL_TABLE_NAME					"/settings/specialskill.txt"

/************************************************************************/
/*                      implement _BaseSpecialSkillTable                */
/************************************************************************/

_BaseSpecialSkillTable::_BaseSpecialSkillTable( )
{
	m_bOpen = FALSE;
}

_BaseSpecialSkillTable::~_BaseSpecialSkillTable( )
{
}

int _BaseSpecialSkillTable::OpenTable( char* szFileName )
{
	if( m_bOpen )
		return FALSE;

	m_bOpen = m_TabFile.Load( szFileName );

	return m_bOpen;
}

int _BaseSpecialSkillTable::GetRecordCount( )
{
	if( !m_bOpen )
		return FALSE;

	return m_TabFile.GetHeight( );
}

void _BaseSpecialSkillTable::CloseTable( )
{
	if( m_bOpen )
	{
		m_TabFile.Clear( );
		m_bOpen = FALSE;
	}
}

int _BaseSpecialSkillTable::LoadRecord(
	int nRow,
	_SpecialSkill_Record	Rec[],
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
		case tabfile_specialskill_type_string:
			{
				m_TabFile.GetString( 
					nRow, Rec[nLoopCount].szName, "", 
					(LPSTR)Rec[nLoopCount].pStoreBuf, 
					Rec[nLoopCount].nSize );
			}	
			break;
		case tabfile_specialskill_type_int:
			{
				m_TabFile.GetInteger( 
					nRow, Rec[nLoopCount].szName, 0, 
					(int*)Rec[nLoopCount].pStoreBuf );
			}
			break;
		case tabfile_specialskill_type_byte:
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
/*                      implement SpecialSkillTab	                    */
/************************************************************************/

SpecialSkillTab::SpecialSkillTab( )
{
	m_nSpecialSkillCount	=	0;
	m_nLoad = FALSE;
}

SpecialSkillTab::~SpecialSkillTab( )
{
}

SpecialSkillTab& SpecialSkillTab::Singleton( void )
{
	static SpecialSkillTab st;
	
	return st;
}

int SpecialSkillTab::LoadSpecialSkill( void )
{
	int nRet = OpenTable( SPECIALSKILL_TABLE_NAME );
	
	if( nRet )
	{
		m_nSpecialSkillCount = GetRecordCount( );

		SST SpecialSkillTemp;
		for( int nLoopCount = 1; nLoopCount < m_nSpecialSkillCount; nLoopCount++ )
		{
			memset(&SpecialSkillTemp, 0 ,sizeof(SpecialSkillTemp) );

			_SpecialSkill_Record	rec[] = 
			{
				{ "Metier",	 tabfile_specialskill_type_byte, &SpecialSkillTemp.byMetier,		sizeof(int) },
				{ "Series",	 tabfile_specialskill_type_byte, &SpecialSkillTemp.bySeries,		sizeof(int) },
				{ "Level",	 tabfile_specialskill_type_byte, &SpecialSkillTemp.byLevel,		sizeof(int) },
				{ "SkillID", tabfile_specialskill_type_int, &SpecialSkillTemp.nSkillID,	sizeof(int) },
				{ "FirstBuffID", tabfile_specialskill_type_int, &SpecialSkillTemp.nFirstBuffID,	sizeof(int) },
				{ "LevelupBuffID", tabfile_specialskill_type_int, &SpecialSkillTemp.nLevelupBuffID,	sizeof(int) },
			};

			LoadRecord( nLoopCount + 1, rec, sizeof( rec ) / sizeof( _SpecialSkill_Record ) );

			if( (SpecialSkillTemp.byLevel > 0 && SpecialSkillTemp.byLevel <= MAX_LEVEL ) &&
				(SpecialSkillTemp.byMetier >= 0 && SpecialSkillTemp.byMetier < 3) &&
				(SpecialSkillTemp.bySeries >= 0 && SpecialSkillTemp.bySeries < 2) )
			{
				int nIdx = SpecialSkillTemp.byLevel * 100 + SpecialSkillTemp.byMetier * 10 + SpecialSkillTemp.bySeries;
				m_SSTIndex[nIdx] = nLoopCount-1;
				m_SST[nLoopCount-1] = SpecialSkillTemp;
			}//*/

		}

		this->CloseTable( );
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

int SpecialSkillTab::GetNearlySpecialSkill( BYTE byMetier, BYTE bySeries, BYTE byLevel )
{
	int nLevel = byLevel;
	if( (byLevel > 0 && byLevel <= MAX_LEVEL ) &&
		(byMetier >= 0 && byMetier < 3) &&
		(bySeries >= 0 && bySeries < 2) )
	{
		while ( nLevel > 0 )
		{
			int nIdx = nLevel * 100 + byMetier * 10 + bySeries;
			_SpecialSkillMap::iterator it = m_SSTIndex.find( nIdx );
			if ( it != m_SSTIndex.end() )
			{
				return m_SST[it->second].nSkillID;
			}
			nLevel--;
		}		
	}//*/
	return -1;
}

int SpecialSkillTab::GetSpecialSkill( BYTE byMetier, BYTE bySeries, BYTE byLevel )
{
	if( (byLevel > 0 && byLevel <= MAX_LEVEL ) &&
		(byMetier >= 0 && byMetier < 3) &&
		(bySeries >= 0 && bySeries < 2) )
	{
		int nIdx = byLevel * 100 + byMetier * 10 + bySeries;
		_SpecialSkillMap::iterator it = m_SSTIndex.find( nIdx );
	
		if ( it != m_SSTIndex.end() )
		{
			return m_SST[it->second].nSkillID;
		}
	}//*/
	return -1;
}


int SpecialSkillTab::GetNearlyFirstupBuffID( BYTE byMetier, BYTE bySeries, BYTE byLevel )
{
	int nLevel = byLevel;
	if( (byLevel > 0 && byLevel <= MAX_LEVEL ) &&
		(byMetier >= 0 && byMetier < 3) &&
		(bySeries >= 0 && bySeries < 2) )
	{
		while ( nLevel > 0 )
		{
			int nIdx = nLevel * 100 + byMetier * 10 + bySeries;
			_SpecialSkillMap::iterator it = m_SSTIndex.find( nIdx );
			if ( it != m_SSTIndex.end() )
			{
				return m_SST[it->second].nFirstBuffID;
			}
			nLevel--;
		}		
	}//*/
	return -1;
}

int SpecialSkillTab::GetLevelupBuffID( BYTE byMetier, BYTE bySeries, BYTE byLevel )
{
	if( (byLevel > 0 && byLevel <= MAX_LEVEL ) &&
		(byMetier >= 0 && byMetier < 3) &&
		(bySeries >= 0 && bySeries < 2) )
	{
		int nIdx = byLevel * 100 + byMetier * 10 + bySeries;
		_SpecialSkillMap::iterator it = m_SSTIndex.find( nIdx );
	
		if ( it != m_SSTIndex.end() )
		{
			return m_SST[it->second].nLevelupBuffID;
		}
	}//*/
	return -1;
}



