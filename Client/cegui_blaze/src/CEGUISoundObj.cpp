//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 04/16/2007 10:13
//      File_base        : CEGUISound.h
//      File_ext         : .h
//      Author           : likun
//      Description      : 为CEGUi音效进行封装
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "CEGUISoundObj.h"
#include "CEGUISoundSet.h"
#include "CEGUISystem.h"
namespace CEGUI
{

	Sound::Sound(const Sound &rSound)
	: d_pSoundSet(rSound.d_pSoundSet)
	, d_iSoundLound(rSound.d_iSoundLound)
	{
		d_soundName = rSound.d_soundName;
		d_soundPath = rSound.d_soundPath;
	}

	Sound::Sound( const String &SoundName, const String &SoundPath )
	: d_soundName(SoundName)
	, d_soundPath(SoundPath)
	, d_iSoundLound(0)
	{
	}

	Sound::Sound( const SoundSet *pOwner, const String &SoundName, const String &SoundPath )
	: d_pSoundSet(pOwner)
	, d_soundName(SoundName)
	, d_soundPath(SoundPath)
	, d_iSoundLound(0)
	{

	}

	Sound::~Sound(){}

	void Sound::DoPlay()
	{
		System::getSingleton().getSound()->DoPlay(d_soundPath);
	}
	
	void Sound::SetSoundName( const String &soundName )
	{
		d_soundName = soundName;
	}

	void Sound::SetSoundPath( const String &soundPath )
	{
		d_soundPath = soundPath;
	}

	String	Sound::GetSoundName() const
	{
		return d_soundName;
	}

	String  Sound::GetSoundPath() const
	{
		return d_soundPath;
	}


	String	Sound::GetSoundSetName() const
	{
		return d_pSoundSet->GetSoundSetName();
	}
}