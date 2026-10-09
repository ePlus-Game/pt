//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 04/16/2007 10:13
//      File_base        : CEGUISoundSet.h
//      File_ext         : .h
//      Author           : likun
//      Description      : 为CEGUi音效集进行封装
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "CEGUISoundSet.h"
#include "CEGUISoundset_xmlHandler.h"
#include "CEGUISystem.h"
#include "CEGUIDataContainer.h"
#include "CEGUIXMLParser.h"



namespace CEGUI
{
	const char SoundSet::SoundSetSechma[] = "Soundset.xsd";
	
	const Sound rSound( NULL, String(""), String("") );
	/********************************************************************
			construction and destruction
	********************************************************************/
	SoundSet::SoundSet( const String &name, const String &fName )
	{
		d_soundSetFile = fName;
		d_soundSetName = name;
		Load( fName );
	}

	SoundSet::~SoundSet()
	{
		UnLoad();
	}

	/*********************************************************************
			Load Soundset
	*********************************************************************/
	void SoundSet::Load( const String &SoundSetFileName )
	{
		UnLoad();

		if ( SoundSetFileName.empty() || SoundSetFileName == (utf8*)"")
		{
			return;
		}
		    
		Soundset_xmlHandler handler(this);

		try
		{
			System::getSingleton().getXMLParser()->parseXMLFile(handler, SoundSetFileName, SoundSetSechma, "");
		}
		catch(...)
		{
			UnLoad();
			return;
		}
	}

	void SoundSet::UnLoad()
	{
		RemoveAllSound();
	}


	String	SoundSet::GetSoundSetName() const
	{
		return d_soundSetName;
	}


	String	SoundSet::GetSoundSetFileName(void ) const
	{
		return d_soundSetFile;
	}

	
	unsigned int SoundSet::GetSoundCount() const
	{
		return d_soundMap.size();
	}


	const Sound	&SoundSet::GetSound( const String &SoundName ) const
	{
		std::map<String, Sound>::const_iterator pos = d_soundMap.find( SoundName );
		if ( pos  != d_soundMap.end())
		{
			return pos->second;
		}
		return rSound;
	}


	void	SoundSet::RemoveSound( const String &SoundName )
	{
		SoundRegistor::iterator pos = d_soundMap.find( SoundName );
		if ( pos != d_soundMap.end())
		{
			d_soundMap.erase(SoundName);
		}
	}

	void	SoundSet::RemoveAllSound( )
	{
		d_soundMap.clear();
	}

	void SoundSet::CreateSound( const String &SoundName, const String &SoundPath )
	{
		if ( IsSoundPresent(SoundName ))
		{
			return;
		}
		d_soundMap[SoundName] = Sound(this, SoundName, SoundPath);
	}

	void	SoundSet::PlayASound( const String &SoundName )
	{
		SoundRegistor::iterator pos = d_soundMap.find(SoundName);
		if ( pos == d_soundMap.end())
		{
			return;
		}
		pos->second.DoPlay();
	}

	bool SoundSet::IsSoundPresent( const String &SoundName )
	{
		SoundRegistor::iterator pos = d_soundMap.find(SoundName);

		if (pos != d_soundMap.end())
		{
			return true;
		}
		return false;
	}
    
   	SoundSet::SoundIterator	SoundSet::getIterator(void) const
    {
    	return SoundIterator(d_soundMap.begin(), d_soundMap.end());
    }

}

