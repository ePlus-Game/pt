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

#ifndef _CEGUISoundSetManager_H_
#define _CEGUISoundSetManager_H_

#include "CEGUIBase.h"
#include "CEGUISingleton.h"
#include "CEGUIString.h"
#include "CEGUISoundSet.h"
#include "CEGUIIteratorBase.h"
#include <map>

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4275)
#	pragma warning(disable : 4251)
#endif

namespace CEGUI
{

class CEGUIEXPORT SoundSetManager : public Singleton<SoundSetManager>
{
	typedef std::map<String, SoundSet*>		SoundSetRegisty;
	
	public:
		/*!
		\brief
			Construct a new SoundSet object.
		*/
		SoundSetManager( void );


		/*!
		\brief
		*/
		~SoundSetManager();


		/*!
		\brief
			Get SoundSetManager referrence from Singleton base class

		\return
			SoundSetManager Object referrence  that manage SoundSet
		*/
		static	SoundSetManager &getSingleton( void );


		/*!
		\brief
			Get SoundSetManager pointer from Singleton base class

		\return
			SoundSetManager Object pointer  that manage SoundSet
		*/
		static  SoundSetManager *getSingletonPtr( void );


		/*!
		\brief
			Create a SoundSet object pointer by soundset name and soundset path

		\param name
			soundset name.

		\param filename
			soundset path.		
			
		\return
			return SoundSet object pointer.	
		*/
		SoundSet *CreateSoundSet( const String &name, const String &fileName);


		/*!
		\brief
			Destroy a SoundSet object by soundset name.

		\param name
			soundset name.
	
		\return
			nothing.	
		*/
		void	DestroySoundSet( const String &name );


		/*!
		\brief
			Destroy All SoundSet object of map .

		\return
			nothing.	
		*/
		void	DestroyAllSoundSet( void );


		/*!
		\brief
			Get a Soundset object pointer.

		\param name
			soundset name.
	
		\return
			SoundSet object pointer.	
		*/
		SoundSet *GetSoundSet( const String &name ) ;


		/*!
		\brief
			Soundset object exist or not

		\param name
			soundset name.
	
		\return
			return true if exist .	
		*/
		bool	IsSoundSetPresent( const String &name ) const;

	
	private:

		/*************************************************************************
			Implementation Data
		*************************************************************************/


		
		SoundSetRegisty	d_soundSetMap;		//SoundSet	set

public:
	/*************************************************************************
		Iterator stuff
	*************************************************************************/
	typedef	ConstBaseIterator<SoundSetRegisty>	SoundsetIterator;

	/*!
	\brief
		Return a ImagesetManager::ImagesetIterator object to iterate over the available Imageset objects.
	*/
	SoundsetIterator	getIterator(void) const;
};



}

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif
