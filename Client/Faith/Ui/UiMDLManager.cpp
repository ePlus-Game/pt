#include "UiMDLManager.h"

namespace UIMDL
{

int UIMDL::CreateMDL ( void )
{
	new DatasetManager();
	return success_errorcode;
}

int UIMDL::GetMDLPtr( IUIMDL** pUIMDLInterface )
{
	*pUIMDLInterface = DatasetManager::getSingletonPtr();
	if ( *pUIMDLInterface )
	{
		return success_errorcode;
	}
	return no_create_datasetmgr_errorcode;
}

int UIMDL::ReleaseMDL( void )
{
	if ( DatasetManager::getSingletonPtr() )
	{
		delete DatasetManager::getSingletonPtr();
	}
	return success_errorcode;
}

template<> 
DatasetManager* Singleton<DatasetManager>::ms_Singleton = NULL;

DatasetManager::DatasetManager():Singleton<DatasetManager>()
{
}

DatasetManager::~DatasetManager()
{
	DatasetSet::iterator it = m_datasetSet.begin();
	while ( it != m_datasetSet.end() )
	{
		if ( (*it).second )
		{
			delete (*it).second;
		}
		it++;
	}
}

int DatasetManager::createDataSet( const char* szKey )
{
	DatasetSet::iterator it = m_datasetSet.find( szKey );
	if ( it != m_datasetSet.end() )
	{
		return dataset_areadycreated_errorcode;
	}
 	m_datasetSet[szKey] = new Dataset;
	if ( m_datasetSet[szKey] == NULL )
	{
		return create_dataset_errorcode;
	}
 	return success_errorcode; 
}

int	DatasetManager::releaseDataSet( const char* szKey	)
{
	DatasetSet::iterator it = m_datasetSet.find( std::string( szKey ) );
	if ( it != m_datasetSet.end() )
	{
		delete (*it).second;
		m_datasetSet.erase( it );
		return success_errorcode;
	}
	return not_find_dataset_errorcode; 
}

int	DatasetManager::queryDataSet( const char* szKey, IUIMDLDataset** iDataset	)
{
	DatasetSet::iterator it = m_datasetSet.find( std::string( szKey ) );
	if ( it != m_datasetSet.end() )
	{
		*iDataset = (*it).second;
		return success_errorcode;
	}
	return not_find_dataset_errorcode; 
}

}