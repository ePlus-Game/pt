//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_man
//      File_ext         : .h
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////
#ifndef _BUFF_MAN_H_
#define _BUFF_MAN_H_

/////////////////////////////////////////////////////////////////////////////
//
//              Header Include
//
/////////////////////////////////////////////////////////////////////////////

#include "KNpc.h"
#include "buff_list.h"

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Declare
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Declare
//
/////////////////////////////////////////////////////////////////////////////

class BuffMgr
{
public:

	BuffMgr( );
	~BuffMgr( );
	
public:
	
	int Init( );
	static BuffMgr& Singleton( );
	void Breathe( );

	//========================================

	unsigned long AddNpcBuff(
		int nSender,
		int nRecver,
		int nBuffTempID,
		PBUFF_PARAM pLocalVar = NULL );
	
	int ClearBuffByTempID(
		int nNpcIndex,
		int nBuffTemplateID );
	
	int ClearBuffByGroup(
		int nNpcIndex,
		int nGroup );

	int ClearBuffByCate(
		int nNpcIndex,
		int nCate,
		int nIndex );
	
	int ClearBuffByID(
		int nNpcIndex,
		unsigned long ulBuffID );

	int ClearBuffByClient(
		int nNpcIndex,
		unsigned long ulBuffID );

	void ClearAllBuff(
		int nNpcIndex,
		BOOL bImm = FALSE );

	int DecBuffPile(
		int nNpcIndex,
		unsigned long ulBuffID );

	int DecBuffPileByTemp(
		int nNpcIndex,
		unsigned long ulTempID );

	int ModifyBuffTByGroup(
		int nNpcIndex,
		int nGroup,
		int nPercent );
	
	int ModifyBuffTByCate(
		int nNpcIndex,
		int nCate,
		int nIndex,
		int nPercent );

	//

	unsigned long GetBuffPersist(
		int nNpcIndex,
		unsigned long ulBuffID );
	
	unsigned long GetBuffPersistByGroup(
		int nNpcIndex,
		int nGroup );
	
	unsigned long GetBuffPersistByCate(
		int nNpcIndex,
		int nCate,
		int nIndex );

	//
	int SetBuffPersist(
		int nNpcIndex,
		unsigned long ulBuffID,
		unsigned long ulPersist );

	int SetBuffPersistByGroup(
		int nNpcIndex,
		int nGroup,
		unsigned long ulPersist );
	
	int SetBuffPersistByCate(
		int nNpcIndex,
		int nCate,
		int nIndex,
		unsigned long ulPersist );

	//

	int AddBuffPersist(
		int nNpcIndex,
		unsigned long ulBuffID,
		int nPersist );

	int AddBuffPersistByGroup(
		int nNpcIndex,
		int nGroup,
		int nPersist );

	int AddBuffPersistByCate(
		int nNpcIndex,
		int nCate,
		int nIndex,
		int nPersist );

	int AddBuffPersistByGroupLimit(
		int nNpcIndex,
		int nGroup,
		int nPersist );
	
	int AddBuffPersistByCateLimit(
		int nNpcIndex,
		int nCate,
		int nIndex,
		int nPersist );
	//========================================

	int GetBuffPileCount(
		int nNpcIndex,
		int nBuffTempID,
		int& nPileCount );

	int GetSyncBuffID(
		int nNpcIndex,
		_BuffPair BuffPair[],
		int& nCount );

	int GetAllBuffID(
		int nNpcIndex,
		_BuffPairExt BuffPair[],
		int& nCount );

	int PlayerOffline( 
		int nNpcIndex );

	int IsHaveBuff(
		int nNpcIndex,
		int nBuffTempID);

	int IsEqualPile(
		int nNpcIndex,
		int nBuffTempID,
		int nPileCount );

	int SaveBuff( 
		int nNpcIndex,
		unsigned char* pStream,
		int& nMaxSize );
	
	int LoadBuff( 
		int nNpcIndex,
		unsigned char* pStream,
		int nMaxSize );
	
	//========================================
	
	int FilterEvent( 
		BUFF_ENV_PARAM& );

	//========================================

	int AddTrap(
		int nTrapID,
		int nSendNpcIndex,
		int nX = 0,
		int nY = 0,
		int nDir = 0);
	
	int RemoveTrap(
		int nObjIndex );

	//========================================

private:

	void Timer( );

private:
	
	unsigned long	m_ulFPS;

	BuffList		m_BuffList[MAX_NPC];
};

#endif