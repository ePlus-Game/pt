//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 12/10/2006 17:36
//      fire_base        : UiMDLDataset
//      fire_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "UIMDLDataset.h"

namespace UIMDL
{

Dataset::Dataset()
{
	createDataSet();
}

Dataset::~Dataset()
{
	releaseDataSet();
}


bool	Dataset::createDataSet( void )
{
	m_dataSet.clear();
	m_eventHandlerList.clear();
	UIMDLEvent tagEvent;
	tagEvent.pDataSet		= this;
	tagEvent.nRecordIndex	= -1;
	fireCreateEvent( tagEvent );
	return true;
}

bool	Dataset::releaseDataSet	( void )
{
	UIMDLEvent tagEvent;
	tagEvent.pDataSet		= this;
	tagEvent.nRecordIndex	= -1;
	fireReleaseEvent( tagEvent );
	DataRecordSet::iterator it = m_dataSet.begin();
	while ( it != m_dataSet.end() )
	{
		if ( it->pRecordData )
		{
			delete[] it->pRecordData;
		}
		it++;
	}
	return false;
}

UIMDLDatasetRecord&	Dataset::getDataRecord( int nIndex )
{
	return m_dataSet[nIndex];
}

void	Dataset::updateRecord( int nIndex, void* pDataRecord, int nDataRecordLen )
{
	if ( nIndex >= m_dataSet.size() )
	{
		return;
	}
	memcpy( m_dataSet[nIndex].pRecordData, pDataRecord, nDataRecordLen );	
	UIMDLEvent tagEvent;
	tagEvent.pDataSet		= this;
	tagEvent.nRecordIndex	= nIndex;
	tagEvent.nOperation		= update_umdl;
	fireChangeEvent( tagEvent );
}

void	Dataset::addDataRecord( void* pDataRecord, int nDataRecordLen	)
{
	if ( pDataRecord && nDataRecordLen > 0 )
	{
		UIMDLDatasetRecord tagRecord;
		tagRecord.nIndex		= (int)m_dataSet.size();
		tagRecord.pRecordData	= new char[nDataRecordLen];
		memcpy( tagRecord.pRecordData, pDataRecord, nDataRecordLen );	
		tagRecord.nRecordLength	= nDataRecordLen;
		m_dataSet.push_back( tagRecord );
		UIMDLEvent tagEvent;
		tagEvent.pDataSet		= this;
		tagEvent.nRecordIndex	= tagRecord.nIndex;
		tagEvent.nOperation		= insert_umdl;
		fireChangeEvent( tagEvent );
	}
}

void	Dataset::delDataRecord( int nIndex )
{
	int nCount = 0;
	DataRecordSet::iterator it = m_dataSet.begin();
	while( it != m_dataSet.end() )
	{
		if ( nCount == nIndex )
		{
			if ( it->pRecordData )
			{
				delete[] it->pRecordData;
				m_dataSet.erase(it);
				UIMDLEvent tagEvent;
				tagEvent.pDataSet		= this;
				tagEvent.nRecordIndex	= nIndex;
				tagEvent.nOperation		= delete_umdl;
				fireChangeEvent( tagEvent );
			}

		}
		nCount++;
		it++;
	}
	
}

void	Dataset::delAllDataRecord( void )
{
	DataRecordSet::iterator it = m_dataSet.begin();
	while ( it != m_dataSet.end() )
	{
		if ( it->pRecordData )
		{
			delete[] it->pRecordData;
		}
		it++;
	}
	m_dataSet.clear();
}

UIMDLDatasetRecord&	Dataset::findDataRecord( findFunc pFunc, void* pFindParam )
{
	
	DataRecordSet::iterator it = m_dataSet.begin();
	while( it != m_dataSet.end() )
	{
		UIMDLDatasetRecord& tagRecord = (*it);
		if ( pFunc( tagRecord.pRecordData, pFindParam ) )
		{
			return (*it);
		}
		it++;
	}
	static UIMDLDatasetRecord rsDataset;
	memset( &rsDataset, 0, sizeof( UIMDLDatasetRecord ) );
	return rsDataset;
}

void	Dataset::setEventHandle( IUIMDLEvent* pEventHandler )
{
	m_eventHandlerList.push_back( pEventHandler );
}

int	Dataset::getRecordCount( void	)
{
	return m_dataSet.size();
}

void	Dataset::fireCreateEvent( UIMDLEvent& rEvent )
{
	EventHandlerList::iterator it = m_eventHandlerList.begin();
	for ( ; it != m_eventHandlerList.end(); it++ )
	{
		(*it)->onCreate( rEvent );
	}
}

void	Dataset::fireChangeEvent( UIMDLEvent& rEvent )
{
	EventHandlerList::iterator it = m_eventHandlerList.begin();
	for ( ; it != m_eventHandlerList.end(); it++ )
	{
		(*it)->onChange( rEvent );
	}
}

void	Dataset::fireReleaseEvent( UIMDLEvent& rEvent )
{
	EventHandlerList::iterator it = m_eventHandlerList.begin();
	for ( ; it != m_eventHandlerList.end(); it++ )
	{
		(*it)->onRelease( rEvent );
	}
}

}