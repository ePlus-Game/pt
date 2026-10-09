//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 04/16/2007 10:13
//      File_base        : CEGUISoundset_xmlHandler.h
//      File_ext         : .h
//      Author           : likun
//      Description      : CEGUi音效xml文件解析器
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "CEGUISoundset_xmlHandler.h"

#include "CEGUIExceptions.h"
#include "CEGUISystem.h"
#include "CEGUIXMLAttributes.h"

namespace CEGUI
{
	const String Soundset_xmlHandler::SoundSetNameElement(AnsiToUtf8("Soundset"));
	const String Soundset_xmlHandler::SoundNameElement(AnsiToUtf8("Sound"));
	const char	 Soundset_xmlHandler::SoundSetNameAttribute[] = "Name";
	const char	 Soundset_xmlHandler::SoundCountAttribute[] = "Count";
	const char	 Soundset_xmlHandler::SoundNameAttribute[] = "Name";
	const char	 Soundset_xmlHandler::SoundValueAttribute[] = "Value";

	void Soundset_xmlHandler::elementStart(const String& element, const XMLAttributes& attributes)
	{
		if ( element == SoundNameElement )
		{
			String name(attributes.getValueAsString(SoundNameAttribute));

			String value(attributes.getValueAsString(SoundValueAttribute));

			d_pSoundSet->CreateSound( name, value );
		}
		else
		{
			if ( element == SoundSetNameElement )
			{
				d_pSoundSet->d_soundSetName = attributes.getValueAsString(SoundSetNameAttribute);
			}
		}
	}


	void Soundset_xmlHandler::elementEnd(const String& element)
	{
		if ( element == SoundSetNameElement)
		{
			return;
		}
	}
}