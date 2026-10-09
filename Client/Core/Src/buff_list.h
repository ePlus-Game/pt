//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_list
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _BUFF_LIST_H_
#define _BUFF_LIST_H_

/////////////////////////////////////////////////////////////////////////////
//
//              Header Include
//
/////////////////////////////////////////////////////////////////////////////
#include "buff_def.h"
#include "buff_item.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Declare
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//              Class Declare
//
/////////////////////////////////////////////////////////////////////////////

class BuffList
{
public:

	BuffList( );
	~BuffList( ){}

	int Init( int nIndex )
	{ m_nIndex = nIndex; return TRUE; }

	unsigned long AddBuff(
		int nSender,
		int nBuffTemplateID,
		PBUFF_PARAM pLocalVar );

	int ClearBuffByTempID(
		int nBuffTemplateID );

	int ClearBuffByGroup(
		int nGroup );

	int ClearBuffByCate(
		int nCate,
		int nIndex );
	
	int ClearBuffByID(
		unsigned long ulBuffID );

	int ClearBuffByClient(
		unsigned long ulBuffID );

	int DecBuffPileByID(
		unsigned long ulBuffID );

	int DecBuffPileByTempID(
		unsigned long ulTempID );

	int ModifyBuffTByGroup(
		int nGroup,
		int nPercent );
	
	int ModifyBuffTByCate(
		int nCate,
		int nIndex,
		int nPercent );

	//

	unsigned long GetBuffPersist(
		unsigned long ulBuffID );
	
	unsigned long GetBuffPersistByGroup(
		int nGroup );
	
	unsigned long GetBuffPersistByCate(
		int nCate,
		int nIndex );

	//
	int SetBuffPersist(
		unsigned long ulBuffID,
		unsigned long ulPersist );

	int SetBuffPersistByGroup(
		int nGroup,
		unsigned long ulPersist );

	int SetBuffPersistByCate(
		int nCate,
		int nIndex,
		unsigned long ulPersist );
	//

	int AddBuffPersist(
		unsigned long ulBuffID,
		int nPersist );

	int AddBuffPersistByGroup(
		int nGroup,
		int nPersist );

	int AddBuffPersistByCate(
		int nCate,
		int nIndex,
		int nPersist );

	int AddBuffPersistByGroupLimit(
		int nGroup,
		int nPersist );
	
	int AddBuffPersistByCateLimit(
		int nCate,
		int nIndex,
		int nPersist );
	//

	void ClearAllBuff( BOOL bImm = FALSE )
	{
		m_nClearAll = TRUE;
		if( bImm ) 
			ClearAll( ); 
	}

	void ClearAll( );

	int IsHaveBuff(
		int nTempID );

	int GetBuffPileCount(
		int nTempID,
		int& nPileCount );

	int IsEqualPile(
		int nTempID,
		int nPileCount );

	int FilterEvent( 
		BUFF_ENV_PARAM& );

	int GetNpcSyncBuffID(
		_BuffPair* pBuffPair,
		int& nCount );

	int GetNpcAllBuffID(
		_BuffPairExt* pBuffPair,
		int& nCount );

	int SaveBuff( 
		unsigned char* pStream,
		int& nMaxSize );

	int LoadBuff( 
		unsigned char* pStream,
		int nMaxSize );
	
	int Time( int nInterval );

private:

	int AddEventCount( int nEventMask );
	void DelEventCount( int nEventMask );
	void DoEventSpecial( BUFF_ENV_PARAM& );
	void Clear( );

	int CoverBuff( 
		Buff* pNewBuff,
		Buff* pBuff,
		unsigned long& ulBuffID );
	
	void Lock( )
	{ m_nLock = TRUE; }
	void UnLock( )
	{ m_nLock = FALSE; }

private:

	int			m_nClearAll;
	int			m_nLock;
	int			m_nIndex;
	int			m_EventMask[buff_event_type_end];

	Buff*		m_pEventList;		//被动事件相关
	Buff*		m_pNoEventList;		//什么都不相关
	int			m_nClearNoEventBuff;
};

//------------------------------------
//Buff DebugLog
//extern unsigned long g_ulTimingBuffID;
//------------------------------------
#endif