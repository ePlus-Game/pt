//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 12/11/2006 14:00
//      File_base        : KSimulation
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KCore.h"
#include "KSimulation.h"

#ifndef _SERVER

/************************************************************************/
/*                                                                      */
/************************************************************************/

KProtocolSimulationSet g_ProtocolSimulationSet;

KProtocolSimulationSet::~KProtocolSimulationSet()
{
	Clear();
}

void KProtocolSimulationSet::Clear()
{
	_SimulationSet::iterator it = m_SimulationSet.begin();
	while ( it != m_SimulationSet.end() )
	{
		if ( (*it).second )
		{
			delete (*it).second;
			(*it).second = NULL;
		}

		it++;
	}
	m_SimulationSet.clear();
}

IProtocolSimulation* KProtocolSimulationSet::getSimulation( const std::string& strKey )
{
	if ( IsExisting( strKey ) )
	{
		return m_SimulationSet[strKey];
	}
	return NULL;
}

bool KProtocolSimulationSet::registerSimulation( const std::string& strKey, IProtocolSimulation* iSimulation  )
{
	if ( !IsExisting( strKey ) )
	{
		m_SimulationSet[strKey] = iSimulation;
		return true;
	}
	return false;
}

bool KProtocolSimulationSet::IsExisting( const std::string& strKey )
{
	_SimulationSet::iterator it = m_SimulationSet.find( strKey );
	if ( it != m_SimulationSet.end() )
	{
		return true;
	}
	return false;
}

void KProtocolSimulationSet::Breathe()
{
	_SimulationSet::iterator it = m_SimulationSet.begin();
	while ( it != m_SimulationSet.end() )
	{
		if ( (*it).second != NULL )
		{
			(*it).second->Breathe();
		}
		it++;
	}
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

bool	KItemGroupCDSimulation::_findGroupCD( void* pParam0, void* pParam1 )
{
	KItemGroupCD_C* pRecord		= (KItemGroupCD_C*)pParam0;
	KItemGroupCD_C*	pGroup		= (KItemGroupCD_C*)pParam1;
	if ( pRecord )
	{
		if ( pRecord->nGroup == pGroup->nGroup && 
			 pRecord->eType == pGroup->eType )
		{
			return true;
		}
	}
	return false;
}

KItemGroupCDSimulation::KItemGroupCDSimulation(  const std::string& strDatasetName  )
{
	m_pDataset = NULL;	
	m_strDatasetName = strDatasetName;
	int nRet = m_pMDL->queryDataSet( m_strDatasetName.c_str(), &m_pDataset );
	if ( success_errorcode != nRet && dataset_areadycreated_errorcode != nRet )
	{
		m_pMDL->createDataSet( strDatasetName.c_str() );
		m_pMDL->queryDataSet( m_strDatasetName.c_str(), &m_pDataset );
	}		
}

KItemGroupCDSimulation::~KItemGroupCDSimulation()
{
	ClearAllGroupCD();
}

void	KItemGroupCDSimulation::onCreate( UIMDLEvent& rEvent )
{

}

void	KItemGroupCDSimulation::onRelease( UIMDLEvent& rEvent )
{

}

void	KItemGroupCDSimulation::onChange( UIMDLEvent& rEvent )
{

}

void	KItemGroupCDSimulation::Breathe( void )
{
	if ( m_pDataset == NULL )
	{
		return;
	}
	int nRecordCount = m_pDataset->getRecordCount();
	for ( int nIdx = 0; nIdx < nRecordCount; ++nIdx )
	{
		UIMDLDatasetRecord& tagRecord = m_pDataset->getDataRecord( nIdx );
		KItemGroupCD_C* pCD = (KItemGroupCD_C*)tagRecord.pRecordData;
		if ( pCD )
		{
			if ( pCD->ulCDTime > 0 && ::GetTickCount() - pCD->dwStartCount >= 1000 )
			{
				pCD->dwStartCount = ::GetTickCount();
				pCD->ulCDTime--;
				m_pDataset->updateRecord( tagRecord.nIndex, pCD, sizeof(KItemGroupCD_C) );
			}
		}
	}
}

void	KItemGroupCDSimulation::AddGroupCD	( KItemGroupCD_C& rCD	)
{
	if ( m_pDataset == NULL )
	{
		return;
	}

	rCD.dwStartCount = ::GetTickCount();
	UIMDLDatasetRecord& tagRecord = m_pDataset->findDataRecord( _findGroupCD, &rCD );
	if ( tagRecord.pRecordData )
	{
		m_pDataset->updateRecord( tagRecord.nIndex, &rCD, sizeof(KItemGroupCD_C) );
	}
	else
	{
		m_pDataset->addDataRecord( &rCD, sizeof( KItemGroupCD_C) );
	}
	
}

void	KItemGroupCDSimulation::DelGroupCD	( KItemGroupCD_C& rCD	)
{
	UIMDLDatasetRecord& tagRecord = m_pDataset->findDataRecord( _findGroupCD, (void*)&rCD );
	if ( tagRecord.pRecordData && m_pDataset )
	{
		rCD.dwStartCount = 0;
		rCD.ulCDTime	 = 0;
		m_pDataset->updateRecord( tagRecord.nIndex, &rCD, sizeof(KItemGroupCD_C) );
	}
}

void	KItemGroupCDSimulation::ClearAllGroupCD( void )
{
	if ( m_pDataset )
	{
		m_pDataset->delAllDataRecord();	
	}
}

#endif