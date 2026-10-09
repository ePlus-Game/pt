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


#ifndef _CEGUISoundSet_H_
#define _CEGUISoundSet_H_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUISoundObj.h"
#include "CEGUIIteratorBase.h"
#include <map>

namespace CEGUI
{
class CEGUIEXPORT SoundSet 
{
	friend class Soundset_xmlHandler;
	public:
		/*!
		\简述
			SoundSet构造函数

		\参数 name
			soundset name.

		\参数 fname
			soundset path.		
			
		\return
			nothing.	
		*/
		SoundSet( const String &name, const String &fName );

		/*!
		\简述
			SoundSet析构函数

		\return
			nothing.	
		*/
		~SoundSet();

		/*!
		\简述
			获取SoundSet名

		\return
			返回SoundSet名.	
		*/
		String	GetSoundSetName( void ) const;


		/*!
		\简述
			获取SoundSet路径名

		\return
			返回SoundSet路径名.	
		*/
		String	GetSoundSetFileName( void ) const;

		/*!
		\简述
			获取SoundSet中Sound的个数

		\return
			返回unsinged int 类型，SoundSet中Sound的个数.	
		*/
		unsigned int GetSoundCount( void ) const;

		/*!
		\简述
			获取SoundSet中的一个Sound对象

		\参数 SoundName
			sound name.
			
		\return
			Sound对象.	
		*/
		const Sound	&GetSound( const String &SoundName ) const;

		/*!
		\简述
			删除一个Sound对象

		\参数 SoundName
			sound name.
			
		\return
			nothing.	
		*/
		void	RemoveSound( const String &SoundName );

		/*!
		\简述
			删除map中所有Sound
		
		\return
			nothing	
		*/
		void	RemoveAllSound( );

		/*!
		\简述
			创建一个Sound对象

		\参数 SoundName
			sound 名.

		\参数 SoundPath
			sound 路径.

		\return
			Sound对象.	
		*/
		void CreateSound( const String &SoundName, const String &SoundPath );

		/*!
		\简述
			播放一个sound

		\参数 SoundName
			sound 名.

		\return
			nothing.	
		*/
		void	PlayASound( const String &SoundName );

		/*!
		\简述
			判断一个sound是否存在

		\参数 SoundName
			sound 名.

		\return
			如果存在返回true.	
		*/
		bool	IsSoundPresent( const String &SoundName );

	protected:
		static  const char SoundSetSechma[];

		/*!
		\简述
			通过文件SoundSetFileName加载SoundSet所有的Sound

		\参数 SoundSetFileName
			soundset文件名.

		\return
			nothing	
		*/
		void	Load(const String &SoundSetFileName );

		void	UnLoad( void );

	private:
		/********************************************************
			数据
		********************************************************/
		typedef std::map<String, Sound> SoundRegistor;

		SoundRegistor	d_soundMap;

		String			d_soundSetName;

		String			d_soundSetFile;
    public:
    	typedef	ConstBaseIterator<SoundRegistor>	SoundIterator;	//!< Iterator type for this collection
    	SoundIterator	getIterator(void) const;

};
}

#endif