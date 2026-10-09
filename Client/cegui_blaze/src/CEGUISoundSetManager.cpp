//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 04/16/2007 10:13
//      File_base        : CEGUISoundSetManager.h
//      File_ext         : .h
//      Author           : likun
//      Description      : 为CEGUi音效管理层进行封装
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "CEGUISoundSetManager.h"

namespace CEGUI
{

template<> SoundSetManager* Singleton<SoundSetManager>::ms_Singleton = NULL;

/**********************************************************************
	Construction  And Destruction
**********************************************************************/
SoundSetManager::SoundSetManager()
{

}

SoundSetManager::~SoundSetManager()
{
	DestroyAllSoundSet();
}


/**********************************************************************
	
**********************************************************************/
SoundSetManager &SoundSetManager::getSingleton()
{
	return Singleton<SoundSetManager>::getSingleton();
}

SoundSetManager *SoundSetManager::getSingletonPtr()
{
	return Singleton<SoundSetManager>::getSingletonPtr();
}

SoundSet *SoundSetManager::CreateSoundSet(const String &name, const String &fileName)
{
	if ( ms_Singleton != NULL)
	{
		d_soundSetMap[name] = new SoundSet(name, fileName);
	}
	return NULL;
}

void	SoundSetManager::DestroySoundSet( const String &name )
{
	SoundSetRegisty::iterator pos = d_soundSetMap.find( name );
	if ( pos != d_soundSetMap.end())
	{
		if (pos->second != NULL)
		{
			delete pos->second;
			pos->second = NULL;
			d_soundSetMap.erase( name );
		}
	}
}


void	SoundSetManager::DestroyAllSoundSet( void )
{
	SoundSetRegisty::iterator pos = d_soundSetMap.begin();
	while ( pos != d_soundSetMap.end())
	{
		if (pos->second != NULL)
		{
			delete pos->second;
			pos->second = NULL;
		}
		pos++;
	}

	d_soundSetMap.clear();
}

SoundSet *SoundSetManager::GetSoundSet( const String &name ) 
{
	SoundSetRegisty::iterator pos = d_soundSetMap.find(name);
	if (pos == d_soundSetMap.end())
	{
		return NULL;
	}
	return pos->second;
}

bool	SoundSetManager::IsSoundSetPresent( const String &name ) const 
{
	try
	{
		return d_soundSetMap.find(name) != d_soundSetMap.end();
	}
	catch (Exception* e)
	{
		e;
		return false;
	}
}


SoundSetManager::SoundsetIterator SoundSetManager::getIterator(void) const
{
	return SoundsetIterator(d_soundSetMap.begin(), d_soundSetMap.end());
}


}


