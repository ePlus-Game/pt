//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 12/10/2006 18:21
//      File_base        : UiMDLManager
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UIMDLMANAGER_H
#define	UIMDLMANAGER_H

#include "UiMDLInterface.h"
#include "UiMDLDataset.h"
#include <map>
#include <string>

namespace UIMDL
{

template <typename T> class Singleton
{
protected:
    static T* ms_Singleton;

public:
    Singleton( void )
    {
        ms_Singleton = static_cast<T*>(this);
    }
   ~Singleton( void )
        {  ms_Singleton = 0;  }
    static T& getSingleton( void )
        {  return ( *ms_Singleton );  }
    static T* getSingletonPtr( void )
        {  return ( ms_Singleton );  }
};

typedef std::map<std::string, IUIMDLDataset*> DatasetSet;
 
class DatasetManager : public IUIMDL, public Singleton<DatasetManager> 
{
	
	friend int CreateMDL ( IUIMDL* pUIMDLInterface	);
	friend int GetMDLPtr ( IUIMDL* pUIMDLInterface	);
	friend int ReleaseMDL( void						);

public:
	DatasetManager();
	~DatasetManager();

public:
	int							createDataSet	( const char* szKey 							);
	int							releaseDataSet	( const char* szKey								);
	int							queryDataSet	( const char* szKey, IUIMDLDataset** iDataset	);

private:
 	DatasetSet					m_datasetSet;
};
	
}

#endif