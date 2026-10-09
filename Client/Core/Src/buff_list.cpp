//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-08-28 17:00
//      File_base        : buff_list
//      File_ext         : .cpp
//      Author           : zolazuo(zuolizhi)
//      Description      : 
//
//////////////////////////////////////////////////////////////////////

#include "KCore.h"
#include "KNpc.h"
#include "KSkills.h"
#include "ConfigManager.h"

#include "buff_list.h"
#include "buff_man.h"

#define KillBuff( Buff )	\
	do{						\
	Buff->KillSelf( );		\
	Lock( );				\
	Buff->Deffect( );		\
	UnLock( );				\
	Buff->SyncBuffToDel( );	\
	}while(0)				\

/////////////////////////////////////////////////////////////////////////////
//
//              Global variables and Macro and Structure Definitions
//
/////////////////////////////////////////////////////////////////////////////
BuffList::BuffList( )
{
	Clear( );
}

void BuffList::Clear( )
{
	m_nLock			=	FALSE;
	m_pEventList	=	NULL;
	m_pNoEventList	=	NULL;
	m_nClearNoEventBuff = FALSE;
	m_nClearAll		=	FALSE;
	memset( m_EventMask, 0, sizeof(m_EventMask) );
}

int BuffList::CoverBuff( 
	Buff* pNewBuff, 
	Buff* pBuff,
	unsigned long& ulBuffID )
{
	if( !pBuff->IsValid( ) )
		return buff_ret_keep;

	if( pNewBuff->GetGroup( ) == pBuff->GetGroup( ) )
	{
		if( pNewBuff->IsPileSpec( ) )
		{
			if( pNewBuff->GetSender( ) == pBuff->GetSender( ) )
			{
				if( pNewBuff->GetTempID( ) == pBuff->GetTempID( ) )
				{
					if( pNewBuff->IsReplace( ) )
					{
						//------------------------------------
						//Buff DebugLog
						/*
						if( pBuff->GetTempID( ) == g_ulTimingBuffID )
							CFS_FILELOGS::WriteLog(
							"Buff Critical Error : BuffList::CoverBuff Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
						*/
						//------------------------------------

						KillBuff( pBuff );
						if( !pBuff->GetEventMask( ) )
							m_nClearNoEventBuff = TRUE;

						return buff_ret_keep;
					}
					else
					{
						pBuff->PileOne( );
						ulBuffID	=	pBuff->GetBuffID( );
						return buff_ret_del;
					}
				}
				else
				{
					if( pNewBuff->GetLevel( ) >= pBuff->GetLevel( ) )
					{
						//------------------------------------
						//Buff DebugLog
						/*
						if( pBuff->GetTempID( ) == g_ulTimingBuffID )
							CFS_FILELOGS::WriteLog(
							"Buff Critical Error : BuffList::CoverBuff Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
						*/
						//------------------------------------
						KillBuff( pBuff );
						if( !pBuff->GetEventMask( ) )
							m_nClearNoEventBuff = TRUE;
						return buff_ret_keep;
					}
					else
						return buff_ret_del;
				}
			}
			else
				return buff_ret_keep;
		}
		else
		{
			if( pNewBuff->GetTempID( ) == pBuff->GetTempID( ) )
			{
				if( pNewBuff->IsReplace( ) )
				{
					//------------------------------------
					//Buff DebugLog
					/*
					if( pBuff->GetTempID( ) == g_ulTimingBuffID )
						CFS_FILELOGS::WriteLog(
						"Buff Critical Error : BuffList::CoverBuff Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
					*/
					//------------------------------------
					KillBuff( pBuff );
					if( !pBuff->GetEventMask( ) )
						m_nClearNoEventBuff = TRUE;
					return buff_ret_keep;
				}
				else
				{
					pBuff->PileOne( );
					ulBuffID	=	pBuff->GetBuffID( );
					return buff_ret_del;
				}
			}
			else
			{
				if( pNewBuff->GetLevel( ) >= pBuff->GetLevel( ) )
				{
					//------------------------------------
					//Buff DebugLog
					/*
					if( pBuff->GetTempID( ) == g_ulTimingBuffID )
						CFS_FILELOGS::WriteLog(
						"Buff Critical Error : BuffList::CoverBuff Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
					*/
					//------------------------------------
					KillBuff( pBuff );
					if( !pBuff->GetEventMask( ) )
						m_nClearNoEventBuff = TRUE;
					return buff_ret_keep;
				}
				else
					return buff_ret_del;
			}
		}
	}

	return buff_ret_keep;
}

/////////////////////////////////////////////////////////////////////////////

unsigned long BuffList::AddBuff(
	int nSender,
	int nBuffTemplateID,
	PBUFF_PARAM pLocalVar )
{
	if( m_nLock )
		return NULL;
	
	int nRecver = m_nIndex;

	//Delay Add	
	PBAT pBuffTemplate = BuffTable::Singleton().GetBuff(nBuffTemplateID);
	if (pBuffTemplate)
	{
		if (pBuffTemplate->nDelayAdd > 0)
		{
			if (pLocalVar == NULL || pLocalVar->nParam[1] == 0)//如果不是从DelayAction中调用的，则需要添加DelayAction；否则直接添加
			{
				int playerIndex = Npc[m_nIndex].GetPlayerIdx();
				if (playerIndex > 0)
				{
					DelayedAction* pDelayedAction = Player[playerIndex].GetActionDelayer().GetAction(delayed_action_add_buff);
					if (pDelayedAction == NULL || pDelayedAction->GetAddBuffParam().m_BuffId != nBuffTemplateID)
					{
						Player[playerIndex].DelayAddBuff(nSender, nBuffTemplateID, pBuffTemplate->nDelayAdd);
					}
				}
				
				return NULL;
			}
		}
	}

	BuffMgr& BMgr = BuffMgr::Singleton( );
	//filter buff in out
	//=============================================
	BUFF_ENV_PARAM Env;
	Env.nEventSender	=	nSender;
	Env.nEventRecever	=	nRecver;
	Env.nEvent			=	nBuffTemplateID;
	Env.nEventFormat	=	buff_event_format_buffid;

	//buff out
	Env.nEventType		=	buff_event_type_buffout;
	Env.nEventRelation	=	buff_event_relation_sender;
	BMgr.FilterEvent( Env );

	if( !Env.nEvent )
		return NULL;

	//buff in
	Env.nEventType		=	buff_event_type_buffin;
	Env.nEventRelation	=	buff_event_relation_recver;
	BMgr.FilterEvent( Env );

	if( !Env.nEvent )
		return NULL;
	//=============================================
	
	Buff* pBuff = CreateBuff( Env.nEvent );

	if( pBuff )
	{
		
		Env.nBuffSender		=	nSender;
		Env.nBuffRecever	=	nRecver;
		Env.nBuffTempID		=	nBuffTemplateID;

		//前置条件
		if( !pBuff->PreCond( Env ) )
		{
			pBuff->ErrProc( Env );
			pBuff->Release( );
			return FALSE;
		}

		//cover
		//==========================

		unsigned long ulCoveredID = 0;
		int nRet = 0;

		Buff* pOldBuff = m_pEventList;
		
		while( pOldBuff )
		{
			nRet = CoverBuff( pBuff, pOldBuff, ulCoveredID );
			
			if( nRet & buff_ret_del )
			{
				pBuff->Release( );
				return ulCoveredID;
			}
			pOldBuff = (*pOldBuff)++;
		}
		
		pOldBuff = m_pNoEventList;
		
		while( pOldBuff )
		{
			nRet = CoverBuff( pBuff, pOldBuff, ulCoveredID );
			
			if( nRet & buff_ret_del )
			{
				pBuff->Release( );
				return ulCoveredID;
			}
			pOldBuff = (*pOldBuff)++;
		}

		if(pLocalVar)
		{
			pBuff->SetLocalVar( *pLocalVar );
		}
		//filter buff in out
		//=============================================
		Env.nBuffSender		=	nSender;
		Env.nBuffRecever	=	nRecver;
		Env.nEvent			=	(int)pBuff;
		Env.nEventFormat	=	buff_event_format_buffobj;
		
		//buff out
		Env.nEventType		=	buff_event_type_buffout;
		Env.nEventRelation	=	buff_event_relation_sender;
		BMgr.FilterEvent( Env );

		//buff in
		Env.nEventType		=	buff_event_type_buffin;
		Env.nEventRelation	=	buff_event_relation_recver;
		BMgr.FilterEvent( Env );

		//=============================================

		pBuff->SetSender( nSender );
		pBuff->SetRecver( nRecver );
		
		pBuff->Effect( );

		if( AddEventCount( pBuff->GetEventMask( ) ) )
		{
			m_pEventList ? (*m_pEventList)=pBuff : 0 ;
			m_pEventList = pBuff;
		}
		else
		{
			m_pNoEventList ? (*m_pNoEventList)=pBuff : 0 ;
			m_pNoEventList = pBuff;
		}
		
		pBuff->SyncBuffToAdd( );

		return pBuff->GetBuffID( );
	}

	return NULL;
}

int BuffList::ClearBuffByTempID(
	int nBuffTemplateID )
{
	if( m_nLock )
		return FALSE;

	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == nBuffTemplateID )
		{
			//------------------------------------
			//Buff DebugLog
			/*
			if( pBuff->GetTempID( ) == g_ulTimingBuffID )
				CFS_FILELOGS::WriteLog(
				"Buff Critical Error : BuffList::ClearBuffByTempID Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
			*/
			//------------------------------------

			KillBuff( pBuff );
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == nBuffTemplateID )
		{
			KillBuff( pBuff );
			m_nClearNoEventBuff = TRUE;
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return FALSE;
}

int BuffList::ClearBuffByGroup(
	int nGroup )
{
	if( m_nLock )
		return FALSE;

	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			//------------------------------------
			//Buff DebugLog
			/*
			if( pBuff->GetTempID( ) == g_ulTimingBuffID )
				CFS_FILELOGS::WriteLog(
				"Buff Critical Error : BuffList::ClearBuffByGroup Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
			*/
			//------------------------------------
			
			KillBuff( pBuff );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			KillBuff( pBuff );
			m_nClearNoEventBuff = TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return FALSE;
}

int BuffList::ClearBuffByCate(
	int nCate,
	int nIndex )
{
	if( m_nLock )
		return FALSE;

	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			//------------------------------------
			//Buff DebugLog
			/*
			if( pBuff->GetTempID( ) == g_ulTimingBuffID )
				CFS_FILELOGS::WriteLog(
				"Buff Critical Error : BuffList::ClearBuffByCate Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
			*/
			//------------------------------------
			KillBuff( pBuff );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			KillBuff( pBuff );
			m_nClearNoEventBuff = TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

int BuffList::ClearBuffByID(
	unsigned long ulBuffID )
{
	if( m_nLock )
		return FALSE;

	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			//------------------------------------
			//Buff DebugLog
			/*
			if( pBuff->GetTempID( ) == g_ulTimingBuffID )
				CFS_FILELOGS::WriteLog(
				"Buff Critical Error : BuffList::ClearBuffByID Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
			*/
			//------------------------------------
			KillBuff( pBuff );
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			KillBuff( pBuff );
			m_nClearNoEventBuff = TRUE;
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

int BuffList::ClearBuffByClient(
	unsigned long ulBuffID )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID &&
			pBuff->IsPlus( ) )
		{
			//------------------------------------
			//Buff DebugLog
			/*
			if( pBuff->GetTempID( ) == g_ulTimingBuffID )
				CFS_FILELOGS::WriteLog(
				"Buff Critical Error : BuffList::ClearBuffByClient Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
			*/
			//------------------------------------
			KillBuff( pBuff );
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID &&
			pBuff->IsPlus( ) )
		{
			KillBuff( pBuff );
			m_nClearNoEventBuff = TRUE;
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return FALSE;
}

int BuffList::DecBuffPileByTempID(
	unsigned long ulTempID )
{
	if( m_nLock )
		return FALSE;

	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == ulTempID )
		{
			if( pBuff->DecPile( ) & buff_ret_kill )
			{
				KillBuff( pBuff );
			}
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == ulTempID )
		{
			if( pBuff->DecPile( ) & buff_ret_kill )
			{
				KillBuff( pBuff );
				m_nClearNoEventBuff = TRUE;
			}
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return FALSE;
}

int BuffList::DecBuffPileByID(
	unsigned long ulBuffID )
{
	if( m_nLock )
		return FALSE;

	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			if( pBuff->DecPile( ) & buff_ret_kill )
			{
				//------------------------------------
				//Buff DebugLog
				/*
				if( pBuff->GetTempID( ) == g_ulTimingBuffID )
					CFS_FILELOGS::WriteLog(
					"Buff Critical Error : BuffList::DecBuffPileByID Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
				*/
				//------------------------------------
				KillBuff( pBuff );
			}
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			if( pBuff->DecPile( ) & buff_ret_kill )
			{
				KillBuff( pBuff );
				m_nClearNoEventBuff = TRUE;
			}
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return FALSE;
}

int BuffList::ModifyBuffTByGroup(
	int nGroup,
	int nPercent )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			unsigned long ulPersist = pBuff->GetPersist( );
			ulPersist = ulPersist * nPercent / 100;
			
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			unsigned long ulPersist = pBuff->GetPersist( );
			ulPersist = ulPersist * nPercent / 100;
			
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

int BuffList::ModifyBuffTByCate(
	int nCate,
	int nIndex,
	int nPercent )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			unsigned long ulPersist = pBuff->GetPersist( );
			ulPersist = ulPersist * nPercent / 100;
			
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			unsigned long ulPersist = pBuff->GetPersist( );
			ulPersist = ulPersist * nPercent / 100;
			
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

unsigned long BuffList::GetBuffPersist(
	unsigned long ulBuffID )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			return pBuff->GetCurPersist();
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			return pBuff->GetCurPersist();
		}
		pBuff = (*pBuff)++;
	}

	return 0;
}

unsigned long BuffList::GetBuffPersistByGroup(
	int nGroup )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			return pBuff->GetCurPersist();
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			return pBuff->GetCurPersist();
		}
		pBuff = (*pBuff)++;
	}

	return 0;
}

unsigned long BuffList::GetBuffPersistByCate(
	int nCate,
	int nIndex )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			return pBuff->GetCurPersist();
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			return pBuff->GetCurPersist();
		}
		pBuff = (*pBuff)++;
	}

	return 0;
}

int BuffList::SetBuffPersist(
	unsigned long ulBuffID,
	unsigned long ulPersist )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

int BuffList::SetBuffPersistByGroup(
	int nGroup,
	unsigned long ulPersist )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}

	return TRUE;
}

int BuffList::SetBuffPersistByCate(
	int nCate,
	int nIndex,
	unsigned long ulPersist )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			pBuff->SetPersist(ulPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}

	return TRUE;
}

int BuffList::AddBuffPersist(
	unsigned long ulBuffID,
	int nPersist )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			pBuff->AddPersist(nPersist);
			pBuff->SyncBuffToInf( );
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetBuffID( ) == ulBuffID )
		{
			pBuff->AddPersist(nPersist);
			pBuff->SyncBuffToInf( );
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

int BuffList::AddBuffPersistByGroup(
	int nGroup,
	int nPersist )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			pBuff->AddPersist(nPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			pBuff->AddPersist(nPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

int BuffList::AddBuffPersistByCate(
	int nCate,
	int nIndex,
	int nPersist )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			pBuff->AddPersist(nPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			pBuff->AddPersist(nPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

int BuffList::AddBuffPersistByGroupLimit(
	int nGroup,
	int nPersist )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			pBuff->AddPersistLimit(nPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetGroup( ) == nGroup )
		{
			pBuff->AddPersistLimit(nPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

int BuffList::AddBuffPersistByCateLimit(
	int nCate,
	int nIndex,
	int nPersist )
{
	if( m_nLock )
		return FALSE;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			pBuff->AddPersistLimit(nPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetCategory( nIndex ) == nCate )
		{
			pBuff->AddPersistLimit(nPersist);
			pBuff->SyncBuffToInf( );
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

void BuffList::ClearAll( )
{
	if( m_nClearAll )
	{
		Buff* pBuff = m_pEventList;
		
		while( pBuff )
		{
			if( pBuff->IsValid( ) )
				KillBuff( pBuff );

			pBuff = (*pBuff)++;
		}
		
		pBuff = m_pNoEventList;
		
		while( pBuff )
		{
			if( pBuff->IsValid( ) )
				KillBuff( pBuff );

			pBuff = (*pBuff)++;
		}
		
		m_nClearNoEventBuff = TRUE;

		m_nClearAll = FALSE;
	}
}

int BuffList::IsHaveBuff(
	int nTempID )
{
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == nTempID )
		{
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == nTempID )
		{
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return FALSE;
}

int BuffList::IsEqualPile(
	int nTempID,
	int nPileCount )
{
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == nTempID &&
			pBuff->GetCurPileCount( ) == nPileCount )
		{
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == nTempID &&
			pBuff->GetCurPileCount( ) == nPileCount )
		{
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return FALSE;
}

int BuffList::GetBuffPileCount(
	int nTempID,
	int& nPileCount )
{
	nPileCount = 0;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == nTempID )
		{
			nPileCount = pBuff->GetCurPileCount( );
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->GetTempID( ) == nTempID )
		{
			nPileCount = pBuff->GetCurPileCount( );
			return TRUE;
		}
		pBuff = (*pBuff)++;
	}
	
	return FALSE;
}

int BuffList::GetNpcSyncBuffID(
	_BuffPair* pBuffPair,
	int& nCount )
{
	int nMaxCount = nCount;
	nCount = 0;

	Buff* pBuff = m_pEventList;
	
	while( pBuff && nCount < nMaxCount )
	{
		if( pBuff->IsValid( ) && 
			pBuff->IsSync( ) )
		{
			pBuffPair[nCount].ulBuffTempID	= pBuff->GetTempID( );
			pBuffPair[nCount].ulBuffID		= pBuff->GetBuffID( );
			nCount++;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff && nCount < nMaxCount )
	{
		if( pBuff->IsValid( ) && 
			pBuff->IsSync( ) )
		{
			pBuffPair[nCount].ulBuffTempID	= pBuff->GetTempID( );
			pBuffPair[nCount].ulBuffID		= pBuff->GetBuffID( );
			nCount++;
		}
		pBuff = (*pBuff)++;
	}

	return TRUE;
}

int BuffList::GetNpcAllBuffID(
	_BuffPairExt* pBuffPair,
	int& nCount )
{
	int nMaxCount = nCount;
	nCount = 0;
	
	Buff* pBuff = m_pEventList;
	
	while( pBuff && nCount < nMaxCount )
	{
		if( pBuff->IsValid( ) )
		{
			pBuffPair[nCount].ulBuffTempID	= pBuff->GetTempID( );
			pBuffPair[nCount].ulBuffID		= pBuff->GetBuffID( );
			pBuffPair[nCount].nBuffPileCount	= pBuff->GetCurPileCount();
			nCount++;
		}
		pBuff = (*pBuff)++;
	}
	
	pBuff = m_pNoEventList;
	
	while( pBuff && nCount < nMaxCount )
	{
		if( pBuff->IsValid( ) )
		{
			pBuffPair[nCount].ulBuffTempID	= pBuff->GetTempID( );
			pBuffPair[nCount].ulBuffID		= pBuff->GetBuffID( );
			pBuffPair[nCount].nBuffPileCount	= pBuff->GetCurPileCount();
			nCount++;
		}
		pBuff = (*pBuff)++;
	}
	
	return TRUE;
}

int BuffList::SaveBuff( 
	unsigned char* pStream,
	int& nMaxSize )
{
	_BUFF_SAVE_S* pSS = (_BUFF_SAVE_S*)pStream;
	pSS->nVersion = BUFF_VERSION;
	pSS->nCount = 0;

	Buff* pBuff = m_pEventList;

	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->IsSave( ) )
		{
			pBuff->Save( &pSS->BS[pSS->nCount++] );
		}
		pBuff = (*pBuff)++;
	}

	pBuff = m_pNoEventList;

	while( pBuff )
	{
		if( pBuff->IsValid( ) && 
			pBuff->IsSave( ) )
		{
			pBuff->Save( &pSS->BS[pSS->nCount++] );
		}
		pBuff = (*pBuff)++;
	}
	
	nMaxSize = ( pSS->nCount * sizeof(_BUFF_SAVE) ) + 
		sizeof(_BUFF_SAVE_S) - sizeof(_BUFF_SAVE);

	return TRUE;
}

int BuffList::LoadBuff( 
	unsigned char* pStream,
	int nMaxSize )
{
	if( nMaxSize >= sizeof(_BUFF_SAVE_S) )
	{
		_BUFF_SAVE_S* pSS = (_BUFF_SAVE_S*)pStream;

		for( int nLoopCount = 0; nLoopCount < pSS->nCount; nLoopCount++ )
		{
			Buff* pBuff = CreateBuff( pSS->BS[nLoopCount].nTemplateID );
			
			if( pBuff )
			{
				if( pBuff->Load( &pSS->BS[nLoopCount] ) )
				{
					pBuff->SetSender( m_nIndex );
					pBuff->SetRecver( m_nIndex );

					pBuff->Effect( );
					
					if( AddEventCount( pBuff->GetEventMask( ) ) )
					{
						m_pEventList ? (*m_pEventList)=pBuff : 0 ;
						m_pEventList = pBuff;
					}
					else
					{
						m_pNoEventList ? (*m_pNoEventList)=pBuff : 0 ;
						m_pNoEventList = pBuff;
					}
					
					pBuff->SyncBuffToAdd( );
				}
				else
					pBuff->Release( );
				
				/////////////////////////////////////
			}
		}
	}

	return TRUE;
}

void BuffList::DoEventSpecial( 
	BUFF_ENV_PARAM& Env )
{
	switch( Env.nEventType ) 
	{
	case buff_event_type_npcdeathin:
		{
			//delete death buff
			if( Npc[m_nIndex].IsPlayer( ) )
			{
				Buff* pBuff = m_pEventList;
				while( pBuff )
				{
					if( pBuff->IsValid( ) && 
						pBuff->IsDead( ) )
					{
						KillBuff( pBuff );
					}
					pBuff = (*pBuff)++;
				}
				
				pBuff = m_pNoEventList;
				
				while( pBuff )
				{
					if( pBuff->IsValid( ) && 
						pBuff->IsDead( ) )
					{
						KillBuff( pBuff );
						m_nClearNoEventBuff = TRUE;
					}
					pBuff = (*pBuff)++;
				}
			}
			else
			{
				ClearAllBuff( TRUE );
			}
		}
		break;
	case buff_event_type_skillin:
		{			
			KSkill* pSkill = g_SkillManager.GetSkill(Env.nEvent);			
			if (pSkill != NULL)
			{
				//增加NPC仇恨
				KNpc& receiver = Npc[Env.nEventRecever];
				KNpc& sender = Npc[Env.nEventSender];
				if (receiver.GetController().IsActive() && sender.IsVisibleToNpc())
				{
					if (pSkill->GetSkillType() == skill_type_hostile)
					{
						if ( Env.nEventRecever != Env.nEventSender )
						{
							int npcId = Npc[Env.nEventSender].GetId();
							int threatValue = pSkill->GetThreat();
							int threatPercentage = 100;
							
							NpcSkillList& SL = sender.GetSkillList();
							int skillInfoIndex = SL.FindSkill(pSkill->GetSkillId());
							if (skillInfoIndex != INVALID_SKILL_INDEX)
							{
								threatValue +=  SL.GetThreatValueByIdx(skillInfoIndex);
								threatPercentage = SL.GetThreatPercentageByIdx(skillInfoIndex);
							}
							
							int actualThreat = threatValue * threatPercentage / 100;
							receiver.GetController().GetThreatMonitor().ChangeEnemyThreat(
								npcId, Env.nEventSender, actualThreat);
						}				
					}		
				}
				
				//技能附加BUFF
				ConfigManager& cm = ConfigManager::Singleton();
				BuffMgr& bm = BuffMgr::Singleton();
				int skillCasterNpcIndex = Env.nEventSender;
				int skillTargetNpcIndex = Env.nEventRecever;
				for (int categoryIndexLoopCount = 0; categoryIndexLoopCount < CATEGORY_COUNT; categoryIndexLoopCount++)
				{
					int categoryId = pSkill->GetCategory(categoryIndexLoopCount);
					for (int buffIndexLoopCount = 0; buffIndexLoopCount < MAX_SKILL_ADDITIONAL_BUFF; buffIndexLoopCount++)
					{
						int buffId = cm.GetSkillAdditionalBuff(categoryIndexLoopCount, categoryId, buffIndexLoopCount);
						if (buffId > 0)
						{
							bm.AddNpcBuff(skillCasterNpcIndex, skillTargetNpcIndex, buffId);
						}
					}
				}
				
				for (int buffIndexLoopCount = 0; buffIndexLoopCount < MAX_SKILL_ADDITIONAL_BUFF; buffIndexLoopCount++)
				{
					int buffId = cm.GetCommonSkillAdditionalBuff(buffIndexLoopCount);
					if (buffId > 0)
					{
						bm.AddNpcBuff(skillCasterNpcIndex, skillTargetNpcIndex, buffId);
					}
				}
			}
		}
		break;
	default:
		break;
	}
}

int BuffList::FilterEvent( 
	BUFF_ENV_PARAM& Env )
{
	if( m_nLock )
		return FALSE;

	if( m_EventMask[Env.nEventType] )
	{
		Buff* pBuff = m_pEventList;
		while( pBuff )
		{
			if( pBuff->FilterEvent( Env ) & buff_ret_kill )
			{
				//------------------------------------
				//Buff DebugLog
				/*
				if( pBuff->GetTempID( ) == g_ulTimingBuffID )
					CFS_FILELOGS::WriteLog(
					"Buff Critical Error : BuffList::FilterEvent Kill Timing Buff! Filter Buff : %d Killed Buff : %d!\n", pBuff->GetTempID( ) , g_ulTimingBuffID );
				*/
				//------------------------------------

				KillBuff( pBuff );
			}
			
			pBuff = (*pBuff)++;
		}
	}

	DoEventSpecial( Env );

	return TRUE;
}

//------------------------------------
//Buff DebugLog
//unsigned long g_ulTimingBuffID = 0;
//------------------------------------

int BuffList::Time( int nInterval )
{
	BUFF_ENV_PARAM Env;
	Env.nEventValue = nInterval;

	if( m_EventMask[buff_event_type_timer] )
	{
		Buff* pDel = NULL;
		Buff* pBuff = m_pEventList;
		while( pBuff )
		{
			//------------------------------------
			//Buff DebugLog
			/*
			unsigned long ulCap = pBuff->GetCurCapability( );
			if( pBuff->IsEmpty( ) )
				CFS_FILELOGS::WriteLog("Buff Critical Error : Empty Buff be in List!\n");
			else
				g_ulTimingBuffID = pBuff->GetTempID( );
			*/
			//------------------------------------
			
			int nRet = pBuff->Timer( nInterval );
			if( nRet & buff_ret_del )
			{
				pDel = pBuff;
				pBuff = (*pBuff)++;
				if( pDel == m_pEventList )
					m_pEventList = pBuff;

				//------------------------------------
				//Buff DebugLog
				/*
				if( pDel->IsEmpty( ) )
				{
					CFS_FILELOGS::WriteLog("Buff Critical Error : Buff be Release twice!\nBuff ID : %d\n Cap : %d\n", g_ulTimingBuffID, ulCap );

					CFS_FILELOGS::WriteLog(
					"Player Name : %s\n", Npc[m_nIndex].Name );
				}
				*/
				//------------------------------------

				if( nRet & buff_ret_kill )
					KillBuff( pDel );

				//------------------------------------
				//Buff DebugLog
				//g_ulTimingBuffID = 0;
				//------------------------------------
				
				DelEventCount( pDel->GetEventMask( ) );
				pDel->Release( );
			}
			else
				pBuff = (*pBuff)++;

			//------------------------------------
			//Buff DebugLog
			//g_ulTimingBuffID = 0;
			//------------------------------------
		}
	}

	if( m_nClearNoEventBuff )
	{
		Buff* pDel = NULL;
		Buff* pBuff = m_pNoEventList;
		while( pBuff )
		{
			int nRet = pBuff->Timer( nInterval );
			if( nRet & buff_ret_del )
			{
				pDel = pBuff;
				pBuff = (*pBuff)++;

				if( pDel == m_pNoEventList )
					m_pNoEventList = pBuff;

				if( nRet & buff_ret_kill )
					KillBuff( pBuff );

				pDel->Release( );
			}
			else
				pBuff = (*pBuff)++;
		}
		
		m_nClearNoEventBuff = FALSE;
	}

	return Env.nRet;
}

int BuffList::AddEventCount( 
	int nEventMask )
{
	int nRet = FALSE;

	for( 
		int nLoopCount = 0; 
		nLoopCount < buff_event_type_end; 
		nLoopCount++ )
	{
		if( nEventMask & ( 1 << nLoopCount ) )
		{
			m_EventMask[nLoopCount]++;
			nRet |= buff_ret_true;
		}

	}

	return nRet;
}

void BuffList::DelEventCount( 
	int nEventMask )
{
	for( 
		int nLoopCount = 0; 
		nLoopCount < buff_event_type_end; 
		nLoopCount++ )
	{
		if( nEventMask & ( 1 << nLoopCount ) )
			m_EventMask[nLoopCount]--;
	}
}
