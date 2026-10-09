//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 04/16/2007 9:50
//      File_base        : UIDXSound
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 音效实现
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _UIDX_SOUND_H_
#define _UIDX_SOUND_H_

#include "CEGUISound.h"
#include "CEGUIString.h"
#include <map>

class KWavSound;
// Start of CEGUI namespace section
namespace CEGUI
{
	class CEGUIEXPORT UIDXSound : public ISound
	{
	public:
		UIDXSound();

		virtual ~UIDXSound();

		// 创建一个音效
		virtual bool CreateSound(const String& pathName);
		
		// 添加音效到列表
		virtual void AddSound();

		// 清除音效列表
		virtual void ClearSoundList();

		// 播放音效
		virtual bool DoPlay( const String& pathName );

		// 设置声音大小
		virtual void SetVolume( int vol ) { m_SoundVolume = (float)vol/100.f; };

	private:
		typedef std::map<String,KWavSound*> SoundList;
		SoundList	m_wavSoundList;
		float		m_SoundVolume;
	};
}

#endif