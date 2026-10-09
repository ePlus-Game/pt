//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 04/16/2007 9:50
//      File_base        : UIDXSound
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 音效实现
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "./Sounder/UIDXSound.h"
#include "KWin32.h"
#include "CEGUIString.h"
#include "KWavSound.h"
#include "KMp3Music.h"
#include "KMpgMusic.h"
#include "KWavMusic.h"
#include "math.h"

// 根据DSOUND例子里的大小吧音效大小设定为 -5000 到 0
//#define SOUND_VOLUME (-5000 + (m_SoundVolume)*50)
#define SOUND_VOLUME ((m_SoundVolume > 0) ? (LONG)(log10(m_SoundVolume) * 2000.0f) : -10000)

// Start of CEGUI namespace section
namespace CEGUI
{

UIDXSound::UIDXSound()
{
	m_SoundVolume = 0;
}

UIDXSound::~UIDXSound()
{
	ClearSoundList();
}

bool UIDXSound::CreateSound(const String& pathName)
{
	KWavSound* pSound = new KWavSound;

	if ( pSound )
	{
		LPSTR path = const_cast<char*>(pathName.c_str());

		if ( pSound->Load( path ) )
		{
			m_wavSoundList[pathName] = pSound;
			return true;
		} 
		else
		{
			goto errexit;
		}
	}
	else
		goto errexit;

errexit:
	if( pSound )
		delete pSound;
	return false;
}
		
void UIDXSound::AddSound()
{

}

void UIDXSound::ClearSoundList()
{
	SoundList::iterator it = m_wavSoundList.begin();
	for ( it; it != m_wavSoundList.end(); it++ )
	{
		if ( it->second )
		{
			delete (it->second);
			it->second = NULL;
		}
	}
	m_wavSoundList.clear();
}

bool UIDXSound::DoPlay( const String& pathName )
{
	SoundList::iterator it = m_wavSoundList.find( pathName );

	if ( it != m_wavSoundList.end() )
	{
		if ( it->second )
		{
			int nVol = SOUND_VOLUME;
			if ( (it->second)->IsPlaying() )
				(it->second)->Stop();
			
			(it->second)->SetVolume(nVol);
			(it->second)->Play(0, nVol, 0);
			return true;
			/*{
				(it->second)->Stop();
				(it->second)->Play(0, nVol, 0);
				return true;
				//(it->second)->SetVolume(SOUND_VOLUME);
				//return false;
			}
			else
			{
				int nVol = SOUND_VOLUME;
				(it->second)->Play(0, nVol, 0);
				return true;
			}//*/
		}
		else
			return false;
	}
	else
	{
		if ( CreateSound(pathName) )
		{
			it = m_wavSoundList.find( pathName );
			(it->second)->Play(0, SOUND_VOLUME, 0);
			return true;
		}
		else
			return false;
	}
}

}	// namespace CEGUI