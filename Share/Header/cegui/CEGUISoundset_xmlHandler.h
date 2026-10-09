//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 04/16/2007 10:13
//      File_base        : CEGUISoundset_xmlHandler.h
//      File_ext         : .h
//      Author           : likun
//      Description      : CEGUi“Ù–ßΩ‚Œˆ∆˜
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _CEGUISoundset_xmlHandler_H_
#define _CEGUISoundset_xmlHandler_H_

#include "CEGUISoundSet.h"
#include "CEGUIXMLHandler.h"

namespace CEGUI
{
	class Soundset_xmlHandler : public XMLHandler
	{
	public:

		Soundset_xmlHandler( SoundSet *pSoundSet )	: d_pSoundSet(pSoundSet){}

		virtual ~Soundset_xmlHandler( void ){}

		SoundSet *GetSoundSet( void ) const	{ return d_pSoundSet;}

		virtual void elementStart( const String& element, const XMLAttributes& attributes);

		virtual void elementEnd( const String& element);
		
	private:
		static	const String	SoundSetNameElement;
		static	const String	SoundNameElement;
		static	const char		SoundSetNameAttribute[];
		static	const char		SoundCountAttribute[];
		static	const char		SoundNameAttribute[];
		static	const char		SoundValueAttribute[];
		SoundSet				*d_pSoundSet;
	};
}

#endif