//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_tab
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "buff_tab.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
//
//              Class Definitions
//
/////////////////////////////////////////////////////////////////////////////

BaseTable::BaseTable( )
{
	m_bOpen = FALSE;
}

BaseTable::~BaseTable( )
{
}

int BaseTable::OpenTable( char* szFileName )
{
	if( m_bOpen )
		return FALSE;

	m_bOpen = m_TabFile.Load( szFileName );

	return m_bOpen;
}

int BaseTable::GetRecordCount( )
{
	if( !m_bOpen )
		return FALSE;

	return m_TabFile.GetHeight( );
}

void BaseTable::CloseTable( )
{
	if( m_bOpen )
	{
		m_TabFile.Clear( );
		m_bOpen = FALSE;
	}
}

int BaseTable::LoadRecord(
	int nRow,
	_Record	Rec[],
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

/////////////////////////////////////////////////////////////////////////////

BuffTable::BuffTable( )
{
	memset( m_BAT, -1, sizeof(m_BAT) );
	memset( m_PEAT, -1, sizeof(m_PEAT) );
	memset( m_IEAT, -1, sizeof(m_IEAT) );

	m_nBuffCount	=	0;
	m_nPEATCount	=	0;
	m_nIEATCount	=	0;

	m_nLoad = FALSE;
}

BuffTable::~BuffTable( )
{
}


int BuffTable::Load(  )
{
	if( m_nLoad )
		return TRUE;

	if( !LoadBuff( ) )
		goto fatal;

	if( !LoadPEffect( ) )
		goto fatal;

	if( !LoadIEffect( ) )
		goto fatal;

	m_nLoad = TRUE;
	return TRUE;

fatal:

	return FALSE;
}

int BuffTable::LoadBuff( )
{
	int nRet = this->OpenTable( BUFF_TABLE_NAME );
	
	if( nRet )
	{
		m_nBuffCount = this->GetRecordCount( );

		BAT BuffTemp;
		char szPreCond[MAX_BUFF_DESC];
		char szErrProc[MAX_BUFF_DESC];

		for( int nLoopCount = 1; nLoopCount < m_nBuffCount; nLoopCount++ )
		{
			memset(&BuffTemp, 0 ,sizeof(BuffTemp) );

			_Record	rec[] = 
			{
				{ "ID",			tabfile_rec_type_int, &BuffTemp.nBuffID,		sizeof(int) },
				{ "Desc",		tabfile_rec_type_string, &BuffTemp.szDesc,	MAX_BUFF_DESC },
				{ "Persist",	tabfile_rec_type_int, &BuffTemp.ulPersistTime,sizeof(int) },
				{ "Offline",	tabfile_rec_type_int, &BuffTemp.nOffline,	sizeof(int) },
				{ "Group",		tabfile_rec_type_int, &BuffTemp.nGroup,		sizeof(int) },
				{ "Level",		tabfile_rec_type_int, &BuffTemp.nLevel,		sizeof(int) },
				{ "Category",	tabfile_rec_type_int, &BuffTemp.nCategory[0],	sizeof(int) },
				{ "Category1",	tabfile_rec_type_int, &BuffTemp.nCategory[1],	sizeof(int) },
				{ "Category2",	tabfile_rec_type_int, &BuffTemp.nCategory[2],	sizeof(int) },
				{ "Capability", tabfile_rec_type_int, &BuffTemp.nCap,			sizeof(int) },
				{ "Stack",		tabfile_rec_type_int, &BuffTemp.nPileCount,	sizeof(int) },
				{ "Precond",	tabfile_rec_type_string, &szPreCond,			MAX_BUFF_DESC },
				{ "ErrProc",	tabfile_rec_type_string, &szErrProc,			MAX_BUFF_DESC },
				{ "SpecialStack", tabfile_rec_type_int, &BuffTemp.nPileSpec,	sizeof(int) },
				{ "Sync",		tabfile_rec_type_int, &BuffTemp.nSync,		sizeof(int) },
				{ "Save",		tabfile_rec_type_int, &BuffTemp.nSave,		sizeof(int) },
				{ "Delay",		tabfile_rec_type_int, &BuffTemp.ulDelayTime,	sizeof(int) },
				{ "Plus",		tabfile_rec_type_int, &BuffTemp.nPlus,		sizeof(int) },
				{ "Send",		tabfile_rec_type_int, &BuffTemp.nSender,		sizeof(int) },
				{ "Death",		tabfile_rec_type_int, &BuffTemp.nDead,		sizeof(int) },
				{ "Icon",		tabfile_rec_type_string, &BuffTemp.szImage,		MAX_BUFF_IMAGE },
				{ "ReplaceRes", tabfile_rec_type_int, &BuffTemp.nDispID,		sizeof(int) },
				{ "OnceRes",	tabfile_rec_type_int, &BuffTemp.nOnceID,		sizeof(int) },
				{ "HeadRes",	tabfile_rec_type_int, &BuffTemp.nHSpecID,		sizeof(int) },
				{ "BodyRes",	tabfile_rec_type_int, &BuffTemp.nBSpecID,		sizeof(int) },
				{ "BottomRes",	tabfile_rec_type_int, &BuffTemp.nFSpecID,		sizeof(int) },
				{ "ie1",		tabfile_rec_type_int, &BuffTemp.nIEffect[0],	sizeof(int) },
				{ "ie2",		tabfile_rec_type_int, &BuffTemp.nIEffect[1],	sizeof(int) },
				{ "ie3",		tabfile_rec_type_int, &BuffTemp.nIEffect[2],	sizeof(int) },
				{ "ie4",		tabfile_rec_type_int, &BuffTemp.nIEffect[3],	sizeof(int) },
				{ "ie5",		tabfile_rec_type_int, &BuffTemp.nIEffect[4],	sizeof(int) },
				{ "pe1",		tabfile_rec_type_int, &BuffTemp.nPEffect[0],	sizeof(int) },
				{ "pe2",		tabfile_rec_type_int, &BuffTemp.nPEffect[1],	sizeof(int) },
				{ "pe3",		tabfile_rec_type_int, &BuffTemp.nPEffect[2],	sizeof(int) },
				{ "pe4",		tabfile_rec_type_int, &BuffTemp.nPEffect[3],	sizeof(int) },
				{ "pe5",		tabfile_rec_type_int, &BuffTemp.nPEffect[4],	sizeof(int) },
				{ "DelayAdd",	tabfile_rec_type_int, &BuffTemp.nDelayAdd,	sizeof(int) },
				{ "DelayAddType",	tabfile_rec_type_int, &BuffTemp.nDelayAddType,	sizeof(int) },
				{ "SoundRes",	tabfile_rec_type_int, &BuffTemp.nSoundID,	sizeof(int) },
				{ "OnlySoundRes",	tabfile_rec_type_int, &BuffTemp.nOnceSoundID,	sizeof(int) },				
			};

			this->LoadRecord( nLoopCount + 1, rec, sizeof( rec ) / sizeof( _Record ) );

			if( BuffTemp.nBuffID >0 && BuffTemp.nBuffID < BUFF_MAX_TAB_COUNT )
			{
#ifdef _SERVER
				if( !szPreCond[0] )
					strcpy( szPreCond, DEFAUTPRECOND );

				if( !szErrProc[0] )
					strcpy( szErrProc, DEFAUTPRECOND );

				BuffTemp.PreCond.ParseAction( szPreCond );
				BuffTemp.ErrProc.ParseAction( szErrProc );
#endif
				m_BAT[BuffTemp.nBuffID] = BuffTemp;
			}
		}

		this->CloseTable( );
		return TRUE;
	}
	else
		return FALSE;
}

int BuffTable::LoadPEffect( )
{
#ifdef _SERVER
	
	int nRet = this->OpenTable( PASSIVE_EFFECT_TABLE_NAME );
	
	if( nRet )
	{
		m_nPEATCount = this->GetRecordCount( );
		
		PEAT EffectTemplate;
		char szType[MAX_ACTION_STRING];
		char szOutAction[MAX_ACTION_STRING];
		char szFilterAttr[MAX_ACTION_STRING];
		char szModAttr[MAX_ACTION_STRING];
		

		for( int nLoopCount = 1; nLoopCount < m_nPEATCount; nLoopCount++ )
		{
			memset(&EffectTemplate, 0 ,sizeof(EffectTemplate) );
			
			_Record	rec[] = 
			{
				{ "ID",			tabfile_rec_type_int, &EffectTemplate.nEffectID,	sizeof(int) },
				{ "Event",		tabfile_rec_type_string, &szType,					MAX_ACTION_STRING },
				{ "OutAction",	tabfile_rec_type_string, &szOutAction,				MAX_ACTION_STRING },
				{ "Percent",	tabfile_rec_type_int, &EffectTemplate.nFilterPercent,		sizeof(int) },
				{ "Condition",	tabfile_rec_type_string, &szFilterAttr,				MAX_ACTION_STRING },
				{ "EventOP",	tabfile_rec_type_string, &szModAttr,				MAX_ACTION_STRING },
			};

			this->LoadRecord( nLoopCount + 1, rec, sizeof( rec ) / sizeof( _Record ) );

			if( EffectTemplate.nEffectID >0 && EffectTemplate.nEffectID < BUFF_MAX_EF_P_COUNT )
			{
				EffectTemplate.FilterAttribute.ParseAction( szFilterAttr );
				EffectTemplate.ModAttribute.ParseAction( szModAttr );
				EffectTemplate.OutAction.ParseAction( szOutAction );
				
				EffectTemplate.nEventType = buff_str::FindEffectEventType( szType );

#ifdef _DEBUG
				if( EffectTemplate.nEventType == BUFF_INVALID )
					printf( "Passive Effect Event Type Not Found : %s\n", szType );
#endif	
				m_PEAT[EffectTemplate.nEffectID] = EffectTemplate;
			}
		}
		
		this->CloseTable( );
		return TRUE;
	}
	else
		return FALSE;
#else
	return TRUE;
#endif

}

int BuffTable::LoadIEffect( )
{
#ifdef _SERVER

	#define DELIMTER	':'

	int nRet = this->OpenTable( INITIATIVE_EFFECT_TABLE_NAME );
	
	if( nRet )
	{
		m_nIEATCount = this->GetRecordCount( );

		IEAT EffectTemplate;

		char szAction[MAX_ACTION_STRING];
		char szDeAction[MAX_ACTION_STRING];
		char szPrecond[MAX_ACTION_STRING];
		char szTime[MAX_ACTION_STRING];
		
		for( int nLoopCount = 1; nLoopCount < m_nIEATCount; nLoopCount++ )
		{
			memset(&EffectTemplate, 0 ,sizeof(EffectTemplate) );
			memset( szTime, 0 ,sizeof(szTime) );

			_Record	rec[] = 
			{
				{ "ID",			tabfile_rec_type_int,		&EffectTemplate.nEffectID,	sizeof(int) },
				{ "Interval",	tabfile_rec_type_string,	&szTime,					MAX_ACTION_STRING },
				{ "LaunchType",	tabfile_rec_type_int,		&EffectTemplate.nLaunchType,sizeof(int) },
				{ "Precond",	tabfile_rec_type_string,	&szPrecond,					MAX_ACTION_STRING },
				{ "OP(+)",		tabfile_rec_type_string,	&szAction,					MAX_ACTION_STRING },
				{ "OP(-)",		tabfile_rec_type_string,	&szDeAction,				MAX_ACTION_STRING },
			};
			
			this->LoadRecord( nLoopCount + 1, rec, sizeof( rec ) / sizeof( _Record ) );

			if( EffectTemplate.nEffectID >0 && EffectTemplate.nEffectID < BUFF_MAX_EF_I_COUNT )
			{
				if( !szPrecond[0] )
					strcpy( szPrecond, DEFAUTPRECOND );
					
				EffectTemplate.ActionAdd.ParseAction( szAction );
				EffectTemplate.ActionDec.ParseAction( szDeAction );
				EffectTemplate.Precond.ParseAction( szPrecond );

				if( EffectTemplate.nLaunchType == effect_i_lanuch_timepoint )
				{
					int nCount = 0;
					while( szTime[nCount] )
					{
						if( szTime[nCount] == DELIMTER )
						{
							szTime[nCount]	= 0;
							EffectTemplate.nInterval =	atoi( szTime ) * 3600;
							EffectTemplate.nInterval += atoi( szTime + nCount + 1 ) * 60;
							break;
						}
						else
							nCount++;
					}
				}
				else
				{
					EffectTemplate.nInterval = atoi( szTime );
				}

				m_IEAT[EffectTemplate.nEffectID] = EffectTemplate;
			}

		}
		
		this->CloseTable( );
		return TRUE;
	}
	else
		return FALSE;
#else
	return TRUE;
#endif
	
}

PBAT BuffTable::GetBuff( int nID )
{

	if( nID <= 1 || nID > BUFF_MAX_TAB_COUNT )
		return NULL;
	else
	{
		if( m_BAT[nID].nBuffID != BUFF_INVALID )
			return &m_BAT[nID];
		else
			return NULL;
	}

	return NULL;
}

PPEAT BuffTable::GetPEffect( int nID )
{
	if( nID <= 1 || nID > BUFF_MAX_EF_P_COUNT )
		return NULL;
	else
	{
		if( m_PEAT[nID].nEffectID != BUFF_INVALID )
			return &m_PEAT[nID];
		else
			return NULL;
	}
	
	return NULL;
}

PIEAT BuffTable::GetIEffect( int nID )
{
	if( nID <= 1 || nID > BUFF_MAX_EF_I_COUNT )
		return NULL;
	else
	{
		if( m_IEAT[nID].nEffectID != BUFF_INVALID )
			return &m_IEAT[nID];
		else
			return NULL;
	}
	
	return NULL;
}

BuffTable& BuffTable::Singleton( )
{
	static BuffTable BT;
	
	return BT;
}
