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

#ifndef _BUFF_ITEM_H_
#define _BUFF_ITEM_H_

/////////////////////////////////////////////////////////////////////////////
//
//              Header Include
//
/////////////////////////////////////////////////////////////////////////////

#include "buff_def.h"
#include "buff_alloc.h"
#include "buff_tab.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Macro Declare
//
/////////////////////////////////////////////////////////////////////////////

#define GLOBAL_MEMBER_PREFIX	"g_"

#define DECLARE_ALLOC( Type, Name )	\
	__allocator<Type, BUFF_GRANULARITY, MAX_BUFF_COUNT> GLOBAL_MEMBER_PREFIX##Name;

#define ALLOCATOR( Name )	\
	GLOBAL_MEMBER_PREFIX##Name._alloc( )

#define DESTORY( Name, Ptr )	\
	GLOBAL_MEMBER_PREFIX##Name._free( Ptr )


/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Declare
//
/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
//
//              Effect_i Class Declare
//
/////////////////////////////////////////////////////////////////////////////

class Effect_i
{
public:
	Effect_i( )
	{ Clear( ); }

	void SetTemplate(
		PIEAT pIEAT )
	{ m_pIEAT = pIEAT; }
	
	void Release( );

	int Effect(
		BUFF_ENV_PARAM& Env )
	{
#ifdef _SERVER
		if( m_pIEAT->nLaunchType == effect_i_lanuch_interval ||
			m_pIEAT->nLaunchType == effect_i_lanuch_timepoint )
		{
			if( m_pIEAT->Precond( Env ) )
				return m_pIEAT->ActionAdd( Env );
			else
				return FALSE;
		}
		else
			return m_pIEAT->ActionAdd( Env );
#else
		return FALSE;
#endif
	}
	
	int DeEffect(
		BUFF_ENV_PARAM& Env )
	{
#ifdef _SERVER
		return m_pIEAT->ActionDec( Env );
#else
		return FALSE;
#endif
	}

	int GetLaunchType( )
	{ return m_pIEAT->nLaunchType; }

	int GetEventType( )
	{ 
		return 
		m_pIEAT->nLaunchType == effect_i_lanuch_interval || 
		m_pIEAT->nLaunchType == effect_i_lanuch_timepoint 
		? buff_event_type_timer
		: buff_event_type_none; 
	}

	unsigned long  GetInterval( )
	{ return m_pIEAT->nInterval; }

private:

	void Clear( )
	{ m_pIEAT = NULL; }

private:

	PIEAT			m_pIEAT;
};

/////////////////////////////////////////////////////////////////////////////
//
//              Effect_p Class Declare
//
/////////////////////////////////////////////////////////////////////////////

class Effect_p
{
public:
	Effect_p( )
	{ Clear( ); }
	
	void SetTemplate(
		PPEAT pPEAT )
	{ m_pPEAT = pPEAT; }

	int GetEventType( )
	{
		return m_pPEAT->nEventType;
	}
	
	int FilterEvent( 
		BUFF_ENV_PARAM& Env );

private:

	void Clear( )
	{ m_pPEAT = NULL; }
	
private:

	PPEAT			m_pPEAT;
};

/////////////////////////////////////////////////////////////////////////////
//
//              Buff Class Declare
//
/////////////////////////////////////////////////////////////////////////////

class Buff
{
public:
	
	Buff( );
	~Buff( );

public:

	unsigned long GetBuffID( )
	{ return m_ulBuffID; }

	void SetBuffID( 
		unsigned long ulBuffID )
	{ m_ulBuffID = ulBuffID; }

	unsigned long GetTempID( )
	{ return m_nTemplateID;	}

	unsigned long GetPersist( )
	{ return m_pBAT->ulPersistTime; }

	unsigned long GetCurPersist( )
	{ return m_ulCurPersist; }

	void SetPersist(
		unsigned long ulPersist )
	{ 
		m_ulCurPersist = ulPersist;
	}

	void AddPersist(
		int nPersist )
	{
		if( nPersist < 0 )
		{
			if( m_ulCurPersist > -nPersist )
				m_ulCurPersist += nPersist;
			else
				m_ulCurPersist = 0;
		}
		else
		{
			m_ulCurPersist += nPersist;
		}
	}

	void AddPersistLimit(
		int nPersist )
	{
		if( nPersist < 0 )
		{
			if( m_ulCurPersist > -nPersist )
				m_ulCurPersist += nPersist;
			else
				m_ulCurPersist = 0;
		}
		else
		{
			m_ulCurPersist += nPersist;

			if( m_ulCurPersist > m_pBAT->ulPersistTime )
				m_ulCurPersist = m_pBAT->ulPersistTime;
		}
	}

	int GetLevel( )
	{ return m_pBAT->nLevel; }

	int GetGroup( )
	{ return m_pBAT->nGroup; }

	int GetCategory( int nIndex )
	{
		if( nIndex >=0 && nIndex < CATEGORY_COUNT )
			return m_pBAT->nCategory[nIndex];
		else
			return 0;
	}

	int GetCurPileCount( )
	{ return m_nCurPileCount; }

	int IsValid( )
	{ return m_nCurCap != 0; }

	int GetCurCapability( )
	{ return m_nCurCap;	}

	void SetSender( 
		int nSenderIndex)
	{ m_nSender	= nSenderIndex; }

	int GetSender( )
	{ return m_nSender; }

	void SetRecver( 
		int nRecverIndex)
	{ m_nRecver = nRecverIndex; }

	int GetRecver( )
	{ return m_nRecver; }

	void KillSelf( )
	{ m_nCurCap = 0; }
	
	int IsSave( )
	{ return m_pBAT->nSave; }
	
	int IsPileSpec( )
	{ return m_pBAT->nPileSpec; }

	int IsReplace( )
	{ return !m_pBAT->nPileCount; }
	
	int IsSync( )
	{ return m_pBAT->nSync; }
	
	int IsPlus( )
	{ return m_pBAT->nPlus; }
	
	int IsDead( )
	{ return m_pBAT->nDead; }

	int IsOffline( )
	{ return m_pBAT->nOffline; }

	int GetEventMask( )
	{ return m_nEventMask; }

	void SetLocalVar( BUFF_PARAM& Param )
	{ m_LocalVar = Param; }

	//====================
	//Debug Info

	int IsEmpty( )
	{ return ( m_pBAT == NULL ); }

	//====================

	int IsHaveEvent( );

	int PileOne( );
	int DecPile( );
	int Effect( );
	int Deffect( );

	int PreCond( BUFF_ENV_PARAM& Env )
	{
#ifdef _SERVER
		return m_pBAT->PreCond( Env ); 
#else
		return FALSE;
#endif
	}
	int ErrProc( BUFF_ENV_PARAM& Env )
	{
#ifdef _SERVER
		return m_pBAT->ErrProc( Env ); 
#else
		return FALSE;
#endif
	}

	int FilterEvent( 
		BUFF_ENV_PARAM& );

	int Timer(  
		int nElapse );

	void SyncBuffToAdd( );
	void SyncBuffToDel( );
	void SyncBuffToInf( );

	int LoadFromTemplate(
		PBAT pBAT );
	int Save( 
		_BUFF_SAVE* pSave);
	int Load(
		_BUFF_SAVE* pLoad);

 	void Release( );

	//====================
	Buff* operator++( int )
	{
		return m_pNext;
	}

	Buff* operator--( int )
	{
		return m_pPrev;
	}
	
	Buff* operator=( Buff* pBuff )
	{
		if( pBuff )
		{
			pBuff->m_pNext = this;
			pBuff->m_pPrev = m_pPrev;
		}

		if( m_pPrev )
			m_pPrev->m_pNext = pBuff;
		
		m_pPrev = pBuff;

		return pBuff;
	}
	//====================

private:
	void _breakaway( )
	{
		if( m_pNext && m_pPrev )
		{
			m_pNext->m_pPrev = m_pPrev;
			m_pPrev->m_pNext = m_pNext;
			return;
		}
		
		if( m_pNext )
		{
			m_pNext->m_pPrev = NULL;
			return;
		}
		
		if( m_pPrev )
		{
			m_pPrev->m_pNext = NULL;
			return;
		}
	}
	void Clear( );

private:

	Buff*	m_pNext;
	Buff*	m_pPrev;

	unsigned long m_ulCurPersist;
	unsigned long m_ulBuffID;
	unsigned long m_ulTickCount;
	unsigned long m_ulLife;
	unsigned long m_ulCurDelay;
	unsigned long m_ulLastDoTimePoint;
	int		m_nCurCap;
	int		m_nCurPileCount;
	int		m_nTemplateID;
	int		m_nSender;
	int		m_nRecver;
	PBAT	m_pBAT;
	int		m_nEventMask;

	int		m_nPECount;
	int		m_nIECount;

	BUFF_PARAM m_LocalVar;

	Effect_p m_PEffect[MAX_BUFF_EFFECT_COUNT];
	Effect_i m_IEffect[MAX_BUFF_EFFECT_COUNT];
};

/////////////////////////////////////////////////////////////////////////////

#endif