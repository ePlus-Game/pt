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

#ifndef _CEGUISound_H_
#define _CEGUISound_H_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include <map>

namespace CEGUI
{

	
friend class SoundSet;

class CEGUIEXPORT Sound
{
	public:
		Sound(){}
		
		Sound( const Sound &rSound );

		Sound( const String &SoundName, const String &SoundPath );

		Sound( const SoundSet *pOwner, const String &SoundName, const String &SoundPath );

		~Sound();

		void	DoPlay();

		void	SetSoundName( const String &soundName );

		String	GetSoundName( void ) const;

		void	SetSoundPath( const String &soundPath );

		String	GetSoundPath( void ) const;
		
		String	GetSoundSetName( void ) const;
		
		friend  class std::map<String, Sound>;
		
	private:
		
		const SoundSet	*d_pSoundSet;

		String		d_soundName;

		String		d_soundPath;

		int			d_iSoundLound;
};



}
#endif