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
#include "KNpc.h"
#include "GlobalDef.h"
#include "KPlayer.h"
#include "KSubWorld.h"
#include "KObjSet.h"
#include "KSkills.h"

#include "buff_item.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

	DECLARE_ALLOC( Buff, Buff );
	
/////////////////////////////////////////////////////////////////////////////
//
//              Class Definitions
//
/////////////////////////////////////////////////////////////////////////////

Buff::Buff( )
{
	Clear( );

	time_t	curtime = UNIX_TMIE_STAMP;
	struct	tm* ctm = localtime(&curtime);
	m_ulTickCount = ( ctm->tm_hour * 3600 ) + ( ctm->tm_min * 60 ) + ctm->tm_sec;
}

Buff::~Buff( )
{

}

int Buff::Timer( int nElapse )
{
	if( !m_nCurCap )
		return buff_ret_del;

	if( !( m_nEventMask & ( 1 << buff_event_type_timer ) ) )
		return buff_ret_keep;					//next buff

	m_ulLife	  += nElapse;
	m_ulTickCount += nElapse;
	m_ulTickCount %= 24 * 3600;
	
	if( m_ulCurDelay )
	{	
		if( m_ulCurDelay > nElapse )
			m_ulCurDelay -= nElapse;
		else
			m_ulCurDelay = 0;

		//ÑÓ³Ù¼¤»î
		if( !m_ulCurDelay )
			Effect( );
		
		return buff_ret_keep;
	}
	else
	{
		//for effect_i
		BUFF_ENV_PARAM Env;
		Env.nBuffCap		= m_nCurCap;
		Env.nBuffPileCount	= m_nCurPileCount;
		Env.nBuffSender		= m_nSender;
		Env.nBuffRecever	= m_nRecver;
		Env.nBuffTempID		= m_nTemplateID;
		Env.LocalVar		= m_LocalVar;
		
		for( int nLoopCount = 0; nLoopCount < m_nIECount; nLoopCount++ )
		{
			int nLaunchType = m_IEffect[nLoopCount].GetLaunchType( );
			unsigned long ulInterval = m_IEffect[nLoopCount].GetInterval( );
			
			if( nLaunchType == effect_i_lanuch_interval &&
				ulInterval )
			{
				if( ( m_ulLife && !( m_ulLife % ulInterval ) ) ||
					( nElapse > ulInterval ) || 
					( ( m_ulLife % ulInterval + nElapse ) > ulInterval) )
					m_IEffect[nLoopCount].Effect( Env );
			}

			if( nLaunchType == effect_i_lanuch_timepoint )
			{
				if( ulInterval )
				{
					if( ( m_ulTickCount > ulInterval - MISTAKETIME &&
						m_ulTickCount < ulInterval + MISTAKETIME ) && 
						UNIX_TMIE_STAMP - m_ulLastDoTimePoint > 3 * MISTAKETIME )
					{
						m_IEffect[nLoopCount].Effect( Env );
						m_ulLastDoTimePoint = UNIX_TMIE_STAMP;
					}
				}
			}
		}

		m_LocalVar	= Env.LocalVar;

		// return with false to delete this buff
		
		if( m_ulCurPersist )
		{
			if( m_ulCurPersist != BUFF_INVALID )
			{
				if( m_ulCurPersist > nElapse )
					m_ulCurPersist -= nElapse;
				else
					m_ulCurPersist = 0;
			}

			return buff_ret_keep;
		}
		else
			return buff_ret_kill | buff_ret_del;
	}

	return buff_ret_keep;
}

int Buff::FilterEvent(
	BUFF_ENV_PARAM& Env )
{
	if( !( m_nEventMask & ( 1 << Env.nEventType ) ) )
		return buff_ret_true;					//next buff

	if( !m_nCurCap )
		return buff_ret_true;
	
	Env.nBuffPileCount	= m_nCurPileCount;
	Env.nBuffSender		= m_nSender;
	Env.nBuffRecever	= m_nRecver;
	Env.nBuffTempID		= m_nTemplateID;
	Env.LocalVar		= m_LocalVar;
	Env.nBuffCap		= m_nCurCap;

	for( int nLoopCount = 0; nLoopCount < m_nPECount; nLoopCount++ )
	{
		if( m_PEffect[nLoopCount].GetEventType( ) == Env.nEventType )
		{
			m_PEffect[nLoopCount].FilterEvent( Env );
		}
	}

	if( Env.nBuffCap == 0 )
	{
		return buff_ret_kill;
	}

	if( m_nCurCap != 0 )
	{
		m_nCurCap = Env.nBuffCap;
	}

	m_LocalVar	= Env.LocalVar;

	return buff_ret_true;
}

int Buff::Effect( )
{
	if( m_ulCurDelay )
		return TRUE;
	//for effect_i

	BUFF_ENV_PARAM Env;
	Env.nBuffCap		= m_nCurCap;
	Env.nBuffPileCount	= m_nCurPileCount;
	Env.nBuffSender		= m_nSender;
	Env.nBuffRecever	= m_nRecver;
	Env.nBuffTempID		= m_nTemplateID;
	Env.LocalVar		= m_LocalVar;

	for( int nLoopCount = 0; nLoopCount < m_nIECount; nLoopCount++ )
	{
		int nLaunchType = m_IEffect[nLoopCount].GetLaunchType( );
		int nRet = FALSE;

		if( ( nLaunchType == effect_i_lanuch_start ||
			nLaunchType == effect_i_lanuch_persistent ) )
			m_IEffect[nLoopCount].Effect( Env );
	}

	m_LocalVar			= Env.LocalVar;

	return TRUE;
}

int Buff::Deffect( )
{
	BUFF_ENV_PARAM Env;
	Env.nBuffCap		= m_nCurCap;
	Env.nBuffPileCount	= m_nCurPileCount;
	Env.nBuffSender		= m_nSender;
	Env.nBuffRecever	= m_nRecver;
	Env.nBuffTempID		= m_nTemplateID;
	Env.LocalVar		= m_LocalVar;
	Env.nBeOPDec		= TRUE;
	
	for( int nLoopCount = 0; nLoopCount < m_nIECount; nLoopCount++ )
	{
		int nLaunchType = m_IEffect[nLoopCount].GetLaunchType( );
		
		if( ( nLaunchType == effect_i_lanuch_final ||
			nLaunchType == effect_i_lanuch_persistent ) )
			m_IEffect[nLoopCount].DeEffect( Env );
	}

	m_LocalVar			= Env.LocalVar;
	
	return TRUE;
}

int Buff::DecPile( )
{
	if( m_nCurPileCount > 1 )
	{
		Deffect( );

		m_nCurPileCount--;
		
		Effect( );

		SyncBuffToInf( );

		return buff_ret_true;
	}

	return buff_ret_kill;
}

int Buff::PileOne( )
{
	m_ulCurPersist	= m_pBAT->ulPersistTime;
	m_nCurCap		= m_pBAT->nCap;

	if( m_pBAT->nPileCount > 1 )
	{
		Deffect( );

		if( m_nCurPileCount < m_pBAT->nPileCount )
			m_nCurPileCount++;

		Effect( );
	}

	SyncBuffToInf( );
	
	return buff_ret_true;
}

int Buff::IsHaveEvent( )
{
	for( 
		int nLoopCount = 0; 
		nLoopCount < buff_event_type_end; 
		nLoopCount++ )
	{
		if( m_nEventMask & ( 1 << nLoopCount ) )
			return TRUE;		
	}

	return FALSE;
}

void Buff::SyncBuffToAdd( )
{
	if( !IsSync( ) )
		return;
	
	int nNpcIndex = GetRecver( );
	
	_Buff_Add BADD;
	
	BADD.Protocol		= s2c_buff_family;
	BADD.ProtocolExtend	= buff_sync_add;
	BADD.wProtocolSize	= sizeof(_Buff_Add) - 1;
	BADD.dwNpcID		= Npc[nNpcIndex].m_dwID;
	
	BADD.ulBuffID	= GetBuffID( );
	BADD.ulTempID	= GetTempID( );
	BADD.ulTime		= GetCurPersist( );
	BADD.nPileCount	= GetCurPileCount( );
	
	int	nMaxCount = MAX_BROADCAST_COUNT_MIN;
	
	if(Npc[nNpcIndex].m_SubWorldIndex >= 0 && 
		Npc[nNpcIndex].m_SubWorldIndex < MAX_SUBWORLD && 
		SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_SubWorldID != -1) 
		SubWorld[Npc[nNpcIndex].m_SubWorldIndex].BroadCastRegion(
		&BADD, 
		sizeof(BADD), 
		nMaxCount, 
		Npc[nNpcIndex].m_RegionIndex, 
		Npc[nNpcIndex].GetMapX(), 
		Npc[nNpcIndex].GetMapY());
}

void Buff::SyncBuffToDel( )
{
	if( !IsSync( ) )
		return;
	
	int nNpcIndex = GetRecver( );
	
	_Buff_Del BDEL;
	
	BDEL.Protocol		= s2c_buff_family;
	BDEL.ProtocolExtend	= buff_sync_del;
	BDEL.wProtocolSize	= sizeof(_Buff_Del) - 1;
	BDEL.dwNpcID		= Npc[nNpcIndex].m_dwID;
	
	BDEL.ulBuffID = GetBuffID( );
	
	int	nMaxCount = MAX_BROADCAST_COUNT_MIN;
	
	if(Npc[nNpcIndex].m_SubWorldIndex >= 0 && 
		Npc[nNpcIndex].m_SubWorldIndex < MAX_SUBWORLD && 
		SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_SubWorldID != -1) 
		SubWorld[Npc[nNpcIndex].m_SubWorldIndex].BroadCastRegion(
		&BDEL, 
		sizeof(BDEL), 
		nMaxCount, 
		Npc[nNpcIndex].m_RegionIndex, 
		Npc[nNpcIndex].GetMapX(), 
		Npc[nNpcIndex].GetMapY());
}

void Buff::SyncBuffToInf( )
{
	if( !IsSync( ) )
		return;
	
	int nNpcIndex = GetRecver( );
	
	_Buff_Info BAIN;
	
	BAIN.Protocol		= s2c_buff_family;
	BAIN.ProtocolExtend	= buff_sync_info;
	BAIN.wProtocolSize	= sizeof(_Buff_Info) - 1;
	BAIN.dwNpcID		= Npc[nNpcIndex].m_dwID;
	
	
	BAIN.ulBuffID	=	GetBuffID( );
	BAIN.ulTempID	=	GetTempID( );
	BAIN.ulTime		=	GetCurPersist( );
	BAIN.nPileCount	=	GetCurPileCount( );
	
	int	nMaxCount = MAX_BROADCAST_COUNT_MIN;
	
	if(Npc[nNpcIndex].m_SubWorldIndex >= 0 && 
		Npc[nNpcIndex].m_SubWorldIndex < MAX_SUBWORLD && 
		SubWorld[Npc[nNpcIndex].m_SubWorldIndex].m_SubWorldID != -1) 
		SubWorld[Npc[nNpcIndex].m_SubWorldIndex].BroadCastRegion(
		&BAIN, 
		sizeof(BAIN), 
		nMaxCount, 
		Npc[nNpcIndex].m_RegionIndex, 
		Npc[nNpcIndex].GetMapX(), 
		Npc[nNpcIndex].GetMapY());
}

int Buff::Save(
	_BUFF_SAVE* pSave )
{
	pSave->nTemplateID	= m_nTemplateID;
	pSave->nPileCount	= m_nCurPileCount;
	pSave->nCap			= m_nCurCap;
	pSave->ulTime		= m_ulCurPersist;
	pSave->ulTimeStamp	= UNIX_TMIE_STAMP;

	return TRUE;
}

int Buff::Load(
	_BUFF_SAVE* pLoad )
{
	m_nTemplateID	=	pLoad->nTemplateID;
	m_nCurPileCount	=	pLoad->nPileCount;
	m_nCurCap		=	pLoad->nCap;
	m_ulCurPersist	=	pLoad->ulTime;

	if( IsOffline( ) )
	{
		unsigned long ulOffline = UNIX_TMIE_STAMP - pLoad->ulTimeStamp;
		
		if( ulOffline >= m_ulCurPersist )
			return FALSE;
		else
			m_ulCurPersist -= ulOffline;
	}

	return TRUE;
}

int Buff::LoadFromTemplate(
	PBAT pBAT )
{
	if( !pBAT )
		return FALSE;
	
	m_pBAT = pBAT;

	m_ulCurPersist	=	m_pBAT->ulPersistTime;
	m_ulCurDelay	=	m_pBAT->ulDelayTime;
	m_nCurCap		=	m_pBAT->nCap;
	m_nTemplateID	=	m_pBAT->nBuffID;
	m_nCurPileCount	=	1;

	BuffTable& BT = BuffTable::Singleton( );

	int nLoopCount = 0;

	for( nLoopCount = 0; nLoopCount < MAX_BUFF_EFFECT_COUNT; nLoopCount++ )
	{
		PPEAT pPEAT = BT.GetPEffect( m_pBAT->nPEffect[nLoopCount] );

		if( pPEAT )
		{
			m_PEffect[m_nPECount].SetTemplate( pPEAT );

			m_nEventMask |= 1 << pPEAT->nEventType;
			m_nPECount++;
		}
	}

	for( nLoopCount = 0; nLoopCount < MAX_BUFF_EFFECT_COUNT; nLoopCount++ )
	{
		PIEAT pIEAT = BT.GetIEffect( m_pBAT->nIEffect[nLoopCount] );
		
		if( pIEAT )
		{
			m_IEffect[m_nIECount].SetTemplate( pIEAT );

			if( pIEAT->nLaunchType == effect_i_lanuch_interval ||
				pIEAT->nLaunchType == effect_i_lanuch_timepoint )
				m_nEventMask |= 1 << buff_event_type_timer;
			
			m_nIECount++;
		}
	}

	if( pBAT->ulPersistTime != BUFF_INVALID )
		m_nEventMask |= 1 << buff_event_type_timer;

	return TRUE;
}

void Buff::Release( )
{
	//------------------------------------
	//Buff DebugLog
	/*
	if( m_nTemplateID == g_ulTimingBuffID )
	{
		#ifndef WIN32

		if( GetTempID( ) == g_ulTimingBuffID )
			CFS_FILELOGS::WriteLog(
			"Buff Critical Error : Buff::Release Exe Twice! Filter Buff : %d Killed Buff : %d!\n", GetTempID( ) , g_ulTimingBuffID );

		unsigned long ulStack[10];
		getcallstack( ulStack, 3 );
		CFS_FILELOGS::WriteLog("Trace BT : \n");
		for(int nLoopCount = 0; nLoopCount < 3; nLoopCount++)
			CFS_FILELOGS::WriteLog("%X\n",ulStack[nLoopCount]);
		#endif
	}
	*/
	//------------------------------------
	_breakaway( );
	Clear( );
	DESTORY( Buff, this );
}

void Buff::Clear( )
{
	m_pBAT			=	NULL;
	m_ulBuffID		=	0xFFFFFFFF;
	m_ulCurDelay	=	0;
	m_nTemplateID	=	0;
	m_nCurCap		=	0;
	m_nCurPileCount	=	0;
	m_ulLife		=	0;
	m_nSender		=	0xFFFFFFFF;
	m_nRecver		=	0xFFFFFFFF;
	m_ulTickCount	=	0;
	m_ulCurPersist	=	0;
	m_ulLastDoTimePoint	=	0;
	m_nEventMask	=	0;
	m_nPECount		=	0;
	m_nIECount		=	0;

	m_pNext			=	NULL;
	m_pPrev			=	NULL;
}

/////////////////////////////////////////////////////////////////////////////

int Effect_p::FilterEvent( 
	BUFF_ENV_PARAM& Env )
{
	if( m_pPEAT->FilterAttribute( Env ) &&
		( g_Random( 99 ) < m_pPEAT->nFilterPercent ) )
	{
		m_pPEAT->ModAttribute( Env );
		m_pPEAT->OutAction( Env );
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////

unsigned long g_ulBuffDynaID = 1;

Buff* CreateBuff(
	int nBuffTempID )
{
	Buff* pBuff = NULL;

	BuffTable& BT = BuffTable::Singleton( );
	PBAT pBAT = BT.GetBuff( nBuffTempID );

	if( pBAT )
	{
		pBuff = ALLOCATOR( Buff );

		if( pBuff )
		{
			pBuff->LoadFromTemplate( pBAT );
			pBuff->SetBuffID( g_ulBuffDynaID );
			g_ulBuffDynaID++;
		}
	}

	return pBuff;
}