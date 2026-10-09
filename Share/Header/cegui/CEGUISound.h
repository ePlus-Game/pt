//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 04/16/2007 9:50
//      File_base        : CEGUISound
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 音效接口
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _ISOUND_H_
#define _ISOUND_H_

#include "CEGUIBase.h"

// Start of CEGUI namespace section
namespace CEGUI
{
	class CEGUIEXPORT ISound
	{
	public:
		virtual ~ISound(){}

		virtual bool CreateSound( const String& pathName ) = 0;
		
		virtual void AddSound() = 0;

		virtual void ClearSoundList() = 0;

		virtual bool DoPlay( const String& pathName ) = 0;

		virtual void SetVolume( int vol ) = 0;

	};
}

#endif