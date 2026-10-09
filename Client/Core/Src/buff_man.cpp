//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_man
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "buff_man.h"
#include "buff_tab.h"
#include "GlobalDef.h"
#include "KPlayer.h"
#include "KSubWorld.h"
#include "KObjSet.h"
#include "KSkills.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////

BuffMgr::BuffMgr( )
{
	m_ulFPS		=	0;
}

BuffMgr::~BuffMgr( )
{
	
}

int BuffMgr::Init( )
{
	if( !BuffTable::Singleton( ).Load( ) )
		return FALSE;
	
	for( int nLoopCount = 0; nLoopCount < MAX_NPC; nLoopCount++ )
		m_BuffList[nLoopCount].Init( nLoopCount );
	
	return TRUE;
}

BuffMgr& BuffMgr::Singleton( )
{
	static BuffMgr BM;

	return BM;
}

void BuffMgr::Breathe( )
{
	m_ulFPS++;
	if( !( m_ulFPS % BUFF_SCAN_INTERVAL ) )
		Timer( );
}

void BuffMgr::Timer( )
{
	static unsigned long ulStartTime = 0;

	if( UNIX_TMIE_STAMP - ulStartTime >= BUFF_MININTERVAL )
	{
		for( int nLoopCount = 0; nLoopCount < MAX_NPC; nLoopCount++ )
		{
			m_BuffList[nLoopCount].Time( BUFF_MININTERVAL );
		}
		ulStartTime = UNIX_TMIE_STAMP;
	}
}

int BuffMgr::FilterEvent( 
	BUFF_ENV_PARAM& Env )
{

	if( Env.nEventRelation == buff_event_relation_recver )
	{
		if( Env.nEventRecever > 0 && Env.nEventRecever < MAX_NPC )
			return m_BuffList[Env.nEventRecever].FilterEvent( Env );
		else
			return FALSE;
	}

	if( Env.nEventRelation == buff_event_relation_sender )
	{
		if( Env.nEventSender > 0 && Env.nEventSender < MAX_NPC )
			return m_BuffList[Env.nEventSender].FilterEvent( Env );
		else
			return FALSE;
	}

	return FALSE;
}

unsigned long BuffMgr::AddNpcBuff(
	int nSender,
	int nRecver,
	int nBuffTempID,
	PBUFF_PARAM pLocalVar )
{
	if( nRecver > 0 && nRecver < MAX_NPC )
	{
		return
		m_BuffList[nRecver].AddBuff( 
			nSender, 
			nBuffTempID, 
			pLocalVar );
	}

	return FALSE;
}

int BuffMgr::ClearBuffByTempID(
	int nNpcIndex,
	int nBuffTemplateID )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].ClearBuffByTempID( nBuffTemplateID );
	
	return FALSE;
}

int BuffMgr::ClearBuffByGroup(
	int nNpcIndex,
	int nGroup )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].ClearBuffByGroup( nGroup );
	
	return FALSE;
}

int BuffMgr::ClearBuffByCate(
	int nNpcIndex,
	int nCate,
	int nIndex )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].ClearBuffByCate( nCate, nIndex );
	
	return FALSE;
}

int BuffMgr::ClearBuffByID(
	int nNpcIndex,
	unsigned long ulBuffID )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].ClearBuffByID( ulBuffID );
	
	return FALSE;
}

int BuffMgr::ClearBuffByClient(
	int nNpcIndex,
	unsigned long ulBuffID )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].ClearBuffByClient( ulBuffID );

	return FALSE;
}

void BuffMgr::ClearAllBuff(
	int nNpcIndex,
	BOOL bImm )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		m_BuffList[nNpcIndex].ClearAllBuff( bImm );
}

int BuffMgr::DecBuffPile(
	int nNpcIndex,
	unsigned long ulBuffID )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].DecBuffPileByID( ulBuffID );
	
	return FALSE;
}

int BuffMgr::DecBuffPileByTemp(
	int nNpcIndex,
	unsigned long ulTempID )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].DecBuffPileByTempID( ulTempID );
	
	return FALSE;
}

int BuffMgr::ModifyBuffTByGroup(
	int nNpcIndex,
	int nGroup,
	int nPercent )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].ModifyBuffTByGroup( nGroup, nPercent );

	return FALSE;
}

int BuffMgr::ModifyBuffTByCate(
	int nNpcIndex,
	int nCate,
	int nIndex,
	int nPercent )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].ModifyBuffTByCate( nCate, nIndex, nPercent );
	
	return FALSE;
}

unsigned long BuffMgr::GetBuffPersist(
	int nNpcIndex,
	unsigned long ulBuffID )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].GetBuffPersist( ulBuffID );

	return 0;
}

unsigned long BuffMgr::GetBuffPersistByGroup(
	int nNpcIndex,
	int nGroup )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].GetBuffPersistByGroup( nGroup );
	
	return 0;
}

unsigned long BuffMgr::GetBuffPersistByCate(
	int nNpcIndex,
	int nCate,
	int nIndex )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].GetBuffPersistByCate( nCate, nIndex );
	
	return 0;
}

int BuffMgr::SetBuffPersist(
	int nNpcIndex,
	unsigned long ulBuffID,
	unsigned long ulPersist )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].SetBuffPersist( ulBuffID, ulPersist );

	return FALSE;
}

int BuffMgr::SetBuffPersistByGroup(
	int nNpcIndex,
	int nGroup,
	unsigned long ulPersist )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].SetBuffPersistByGroup( nGroup, ulPersist );
	
	return FALSE;
}

int BuffMgr::SetBuffPersistByCate(
	int nNpcIndex,
	int nCate,
	int nIndex,
	unsigned long ulPersist )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].SetBuffPersistByCate( nCate, nIndex, ulPersist );
	
	return FALSE;
}

int BuffMgr::AddBuffPersist(
	int nNpcIndex,
	unsigned long ulBuffID,
	int nPersist )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].AddBuffPersist( ulBuffID, nPersist );
	
	return FALSE;
}

int BuffMgr::AddBuffPersistByGroup(
	int nNpcIndex,
	int nGroup,
	int nPersist )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].AddBuffPersistByGroup( nGroup, nPersist );
	
	return FALSE;
}

int BuffMgr::AddBuffPersistByCate(
	int nNpcIndex,
	int nCate,
	int nIndex,
	int nPersist )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].AddBuffPersistByCate( nCate, nIndex, nPersist );
	
	return FALSE;
}


int BuffMgr::AddBuffPersistByGroupLimit(
	int nNpcIndex,
	int nGroup,
	int nPersist )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].AddBuffPersistByGroupLimit( nGroup, nPersist );
	
	return FALSE;
}

int BuffMgr::AddBuffPersistByCateLimit(
	int nNpcIndex,
	int nCate,
	int nIndex,
	int nPersist )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].AddBuffPersistByCateLimit( nCate, nIndex, nPersist );
	
	return FALSE;
}

//========================================

int BuffMgr::GetSyncBuffID(
	int nNpcIndex,
	_BuffPair BuffPair[],
	int& nCount )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		m_BuffList[nNpcIndex].GetNpcSyncBuffID( BuffPair, nCount );
	
	return FALSE;
}

int BuffMgr::GetAllBuffID(
	int nNpcIndex,
	_BuffPairExt BuffPair[],
	int& nCount )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		m_BuffList[nNpcIndex].GetNpcAllBuffID( BuffPair, nCount );
	
	return FALSE;
}

int BuffMgr::PlayerOffline( 
	int nNpcIndex )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		m_BuffList[nNpcIndex].ClearAllBuff( TRUE );

	return FALSE;
}

int BuffMgr::IsHaveBuff(
	int nNpcIndex,
	int nBuffTempID)
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].IsHaveBuff( nBuffTempID );

	return FALSE;
}

int BuffMgr::IsEqualPile(
	int nNpcIndex,
	int nBuffTempID,
	int nPileCount )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].IsEqualPile( nBuffTempID, nPileCount );
	
	return FALSE;
}

int BuffMgr::GetBuffPileCount(
	int nNpcIndex,
	int nBuffTempID,
	int& nPileCount )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].GetBuffPileCount( nBuffTempID, nPileCount );
	
	return FALSE;
}


int BuffMgr::SaveBuff( 
	int nNpcIndex,
	unsigned char* pStream,
	int& nMaxSize )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].SaveBuff( pStream, nMaxSize );
	
	return FALSE;
}

int BuffMgr::LoadBuff( 
	int nNpcIndex,
	unsigned char* pStream,
	int nMaxSize )
{
	if( nNpcIndex > 0 && nNpcIndex < MAX_NPC )
		return m_BuffList[nNpcIndex].LoadBuff( pStream, nMaxSize );
	
	return FALSE;
}

//==============================================================================

int BuffMgr::AddTrap(
	int nTrapID,
	int nSendNpcIndex,
	int nX,
	int nY,
	int nDir)
{
	if ( nSendNpcIndex < 0 || nSendNpcIndex > MAX_NPC )
		return FALSE;

	KMapPos	Pos;
	KObjItemInfo ObjInfo;

	if( nX && nY )
	{
		int nSubWorldIndex = Npc[nSendNpcIndex].m_SubWorldIndex;

		SubWorld[nSubWorldIndex].Mps2Map(nX, nY, 
			&Pos.nRegion, &Pos.nMapX, &Pos.nMapY, 
			&Pos.nOffX, &Pos.nOffY);

		Pos.nSubWorld = nSubWorldIndex;
	}
	else
	{
		Pos.nSubWorld = Npc[nSendNpcIndex].m_SubWorldIndex;
		Pos.nRegion = Npc[nSendNpcIndex].m_RegionIndex;
		Pos.nMapX = Npc[nSendNpcIndex].GetMapX();
		Pos.nMapY = Npc[nSendNpcIndex].GetMapY();
		Pos.nOffX =	Npc[nSendNpcIndex].GetOffX();
		Pos.nOffY = Npc[nSendNpcIndex].GetOffY();
	}
	
	ObjInfo.m_nColorID = 1;
	ObjInfo.m_nItemID = 0;
	ObjInfo.m_nMoneyNum = 0;
	ObjInfo.m_nMovieFlag = 1;
	ObjInfo.m_nSoundFlag = 1;	
	ObjInfo.m_nLauncher = nSendNpcIndex;
	memset(ObjInfo.m_szName, 0, sizeof(ObjInfo.m_szName));
	//strcpy( ObjInfo.m_szName, "trap" );
	int nIndex = ObjSet.Add( nTrapID, Pos, ObjInfo );
	if (nIndex >= 0)
	{
		Object[nIndex].SetDir(nDir);
	}

	return TRUE;
}

int BuffMgr::RemoveTrap(
	int nObjIndex )
{
	int nNpcIndex = Object[nObjIndex].m_nLauncher;
	Object[nObjIndex].Remove( FALSE );
	return TRUE;
}
//////////////////////////////////////////////////////
