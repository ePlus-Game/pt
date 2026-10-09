//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/17/2006 15:29
//      File_base        : Header1
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef TLGAMEOBJECT_H
#define TLGAMEOBJECT_H
#include "TLModule.h"
#include "elements/CEGUIPushButton.h"
#include "CEGUIWindowFactory.h"
#include "CEGUIRenderableImage.h"
#include "CEGUIProperty.h"
#include "TLToolTip.h"
#include "TLStatic.h"

namespace CEGUI
{

	#define BACKGROUND_IMAGE "Background_Image"
	namespace TLGameObjectProperties
	{

		class DisabledEndImage : public Property
		{
		public:
		   DisabledEndImage() : Property(
			   "DisabledEndImage", 
			   "Property to get/set the normal image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
			   "set:GameObject image:DisabledEffect")
		   {}

		   //String   get(const PropertyReceiver* receiver) const;
		   void   set(PropertyReceiver* receiver, const String& value);
		};

		class CoolingImage : public Property
		{
		public:
		   CoolingImage() : Property(
			   "CoolingImage",
			   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
			   "set:GameObject image:CoolingEffect")
		   {}

		   String   get(const PropertyReceiver* receiver) const;
		   void   set(PropertyReceiver* receiver, const String& value);
		};

		class CoolingEndImage : public Property
		{
		public:
		   CoolingEndImage() : Property(
			   "CoolingEndImage",
			   "Property to get/set the hover image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
			   "set:GameObject image:CoolingEffect")
		   {}

		   String   get(const PropertyReceiver* receiver) const;
		   void   set(PropertyReceiver* receiver, const String& value);
		};

		class IntonateImage : public Property
		{
		public:
		   IntonateImage() : Property(
			   "IntonateImage",
			   "Property to get/set the disabled image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
			   "set:GameObject image:IntonateEffect")
		   {}

		   String   get(const PropertyReceiver* receiver) const;
		   void   set(PropertyReceiver* receiver, const String& value);
		};

		class IntonateEndImage : public Property
		{
		public:
		   IntonateEndImage() : Property(
			   "IntonateEndImage",
			   "Property to get/set the disabled image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
			   "set:GameObject image:IntonateEffect")
		   {}

		   String   get(const PropertyReceiver* receiver) const;
		   void   set(PropertyReceiver* receiver, const String& value);
		};

	}

	typedef	 bool ( *StateNotify )( unsigned int );      


	class TAHAREZLOOK_API TLGameObject : public PushButton
	{
	static TLGameObject DragItem;
	public:
		enum ObjectState
		{
			idleState,
			normalState,
			hoverState,
			selectState,
			pushedState,
			intonateState,
			coolingState,
			disableState,
		};

		enum ObjectType
		{
			idle,
			buffer,
			shortcut,
			item,
			skill,
			minibuffer,
		};

		struct ItemPos
		{
			ItemPos()
			{
				nContainer	= 0;
				nX			= 0;
				nY			= 0;
			}
			ItemPos( const ItemPos& rPos )
			{
				nContainer	= rPos.nContainer;
				nX			= rPos.nX;
				nY			= rPos.nY;
			}
			int nContainer;
			int nX;
			int nY;
		};

		struct GameObject 
		{
			GameObject()
			{
				d_state			= idleState;
				d_type			= idle;
				d_count			= 0;
				d_intonateTime	= 0;
				d_coolingTime	= 0;
				d_disabledTime	= 0;
				d_bufferIdx		= 0;
				d_bufferID		= 0;
				d_skillID		= 0;
				d_EdgeframeIdx	= 0;
				d_passivity		= false;
				d_genre			= 0;
				d_detail		= 0;
				d_particular	= 0;
				d_level			= 0;
				d_group			= 0;
				d_gameobjectSet = "";
				d_gameobject	= "";
			}
			GameObject( const GameObject& rGO )
			{
				d_gameobject	= rGO.d_gameobject;
				d_gameobjectSet	= rGO.d_gameobjectSet;
				d_state			= rGO.d_state;
				d_type			= rGO.d_type;
				d_count			= rGO.d_count;
				d_intonateTime	= rGO.d_intonateTime;
				d_coolingTime	= rGO.d_coolingTime;
				d_disabledTime	= rGO.d_disabledTime;
				d_bufferIdx		= rGO.d_bufferIdx;
				d_bufferID		= rGO.d_bufferID;
				d_skillID		= rGO.d_skillID;
				d_EdgeframeIdx	= rGO.d_EdgeframeIdx;
				d_passivity		= rGO.d_passivity;
				d_genre			= rGO.d_genre;
				d_detail		= rGO.d_detail;
				d_particular	= rGO.d_particular;
				d_level			= rGO.d_level;
				d_group			= rGO.d_group;
				memcpy( &d_itempos, &rGO.d_itempos, sizeof(ItemPos) );
			}

			String			d_gameobject;
			String			d_gameobjectSet;
			ObjectState		d_state;
			ObjectType		d_type;
			int				d_count;
			DWORD			d_intonateTime;
			DWORD			d_coolingTime;
			DWORD			d_disabledTime;
			int				d_bufferIdx;
			int				d_bufferID;
			int				d_skillID;
			int				d_EdgeframeIdx;
			bool			d_passivity;
			ItemPos			d_itempos;
			int				d_genre;
			int				d_detail;
			int				d_particular;
			int				d_level;
			int				d_group;
		};


		TLGameObject(const String& type, const String& name);
		virtual ~TLGameObject(void);

	public:
		void				lock( void )
		{
			if ( (d_type == shortcut || d_type == item) && !d_locked )
			{
				d_lockPlayer->play();
			}
			d_locked = true;
		}
		void				unlock( void )
		{
			if ( (d_type == shortcut || d_type == item) && d_locked )
			{
				d_lockPlayer->stop();
			}
			d_locked = false;
			
		}
		bool				isLock( void )
		{
			return d_locked;
		}
		void				setTooltipText(const String& tip);
		void				SetHand				( bool bHand					)
		{
			d_hand = bHand;
		}
		void				setCount			( int count )
		{
			d_count = count;
			requestRedraw();
		}
		void				setCoolingTime		( int ntime, int npasstime = 0  );
		void				setObject			( const GameObject &rGO			);
		void				getObject			( GameObject &rGO				) const;
		GameObject			getObject			(								);
		void				setState			( ObjectState					);
		inline void			setStateNotifyFun	( StateNotify fun				);
		inline ObjectState	getState			( void							) const;
		void				clear				( void							);
		void				intonate			( void							);
		void				disable				( bool bDisable					);
		void				pickup				( GameObject &rGO			);
		void				putdown				( const GameObject &rGO			);
		inline void			setBufferIdx		( int idx						);
		void				setNormalImage		( const Image* image	);
		void				setHoverImage		( const Image* image	);
		void				setPushedImage		( const Image* image	);
		void				setDisabledImage	( const Image* image	);
		void				setFrameImage		( const Image* image	);
		void				setIntonateImage	( const Image* image	);
		void				setCoolingImage		( const Image* image	);
		void				setType				( ObjectType eType				)
		{
			d_type = eType;
		}
		ObjectType			getGameObjectType( void )
		{
			return d_type;
		}
		bool				isEmpty				( void							)
		{
			if ( d_gameObject.size() && d_state != idleState &&
				strcmp( Utf8ToAnsi( d_gameObject ), BACKGROUND_IMAGE ) )
			{
				return false;
			}
			else
			{
				return true;
			}
		}
		bool				isPassivity			( void							)
		{
			return d_passivity;
		}
		void setCanDrag(bool canDrag);
		bool getCanDrag(){	return d_canDrag;	};
		int	getBuffIndex( void )
		{
			return d_bufferIdx;
		}
		int getBuffID( void )
		{
			return d_bufferID;
		}
		int		getGenre() const { return d_genre; }
		int		getDetail() const { return d_detail; }
		int		getParticular() const { return d_particular; }
		int		getLevel() const { return d_level; }
		int		getGroup() const { return d_group; }
		bool	isSkill( void ) { return d_skillID > 0 ? true : false; }
		int		getType( void ) { return d_type; }
		int		getCount() const { return d_count; }
		int		setEdgeFrameIdx( int nIdx ) { d_EdgeframeIdx = nIdx; }


	protected:
		virtual void		breathe				( void							);
		virtual void		drawSelf			(KRenderCache* panelCache, Point* panelAbsPos);
		virtual void		drawIdle			(KRenderCache* panelCache, Point* panelAbsPos);
		virtual void		drawNormal			(KRenderCache* panelCache, Point* panelAbsPos);
		virtual void		drawHover			(KRenderCache* panelCache, Point* panelAbsPos);
		virtual void		drawPushed			(KRenderCache* panelCache, Point* panelAbsPos);
		virtual void		drawDisabled		(KRenderCache* panelCache, Point* panelAbsPos);
		virtual void		drawCooling			(KRenderCache* panelCache, Point* panelAbsPos);
		virtual void		drawBufferCooling	(KRenderCache* panelCache, Point* panelAbsPos);
		virtual void		drawIntonate		(KRenderCache* panelCache, Point* panelAbsPos);
		virtual void		onCharacter			( KeyEventArgs& e				);
		virtual	void		onMouseMove			( MouseEventArgs& e				);
		virtual void		updateInternalState	( const Point& mouse_pos		);
		virtual void		onMouseButtonDown	( MouseEventArgs& e				);
		virtual void		onMouseButtonUp		( MouseEventArgs& e				);
		virtual void		onCaptureLost		( WindowEventArgs& e			);
		virtual void		onMouseLeaves		( MouseEventArgs& e				);
		virtual void		onMouseEnters		( MouseEventArgs& e				);
		virtual void		onClicked			( WindowEventArgs& e			);
		virtual	void		onSized(WindowEventArgs& e);
		void				offsetPixelPosition	( const Vector2& offset			);
		virtual void		updateSelf			( float elapsed                 );

	public:
		static const utf8	WidgetTypeName[];	
		static const utf8	ImageSetName[];
		static const utf8	HoverEffectName[];
		static const utf8	DisabledEffectName[];		
		static const utf8	CoolingEffectName[];
		static const utf8	BufferCoolingEffectName[];
		static const utf8	IntonateEffectName[];
		static const utf8	BigFrameName[];
		static const utf8	FrameName[];
		static const utf8	MiniFrameName[];
		static const utf8	BufferBG[];
		
	private:
		const Image*		d_coolingImage;
		const Image*		d_intonateImage;
		const Image*		d_frameImage;
		const Image*		d_bufferBG;
		ObjectState			d_state;
		ObjectType			d_type;
		int					d_count;
		String				d_gameObject;
		String				d_gameobjectSet;
		bool				d_passivity;
		int					d_bufferIdx;
		int					d_bufferID;
		DWORD				d_startIntonateTime;		
		DWORD				d_passIntonateTime;
		DWORD				d_intonateTime;

		DWORD				d_startCoolingTime;
		DWORD				d_passCoolingTime;
		DWORD				d_coolingTime;

		TLStaticImage*		d_minPlayer;
		TLStaticImage*				d_player;
		TLStaticImage*				d_bigPlayer;
		TLStaticImage*		d_lockPlayer;
		TLStaticImage*		d_Edge;
		bool				d_locked;

		DWORD				d_disabledTime;
		bool				d_playFinished;
		const Image*		d_playImage;
		int					d_frameIdx;
		StateNotify			d_stateNotifyfun;
		bool				d_dragging;
		bool				d_canDrag;
		Point				d_dragPoint;
		int					d_skillID;
		bool				d_hand;
		ItemPos				d_itempos;
		int					d_EdgeframeIdx;
		int d_genre;
		int d_detail;
		int d_particular;
		int	d_level;
		int	d_group;
	};

	inline void TLGameObject::setStateNotifyFun( StateNotify fun )
	{
		d_stateNotifyfun = fun;
	}
	inline TLGameObject::ObjectState TLGameObject::getState( void ) const
	{
		return d_state;
	}

	inline void TLGameObject::setBufferIdx( int idx )
	{
		d_bufferIdx	= idx;
	}

	/************************************************************************/
	/*					TLGameObject Factory                                */
	/************************************************************************/
	class TAHAREZLOOK_API TLGameObjectFactory : public WindowFactory
	{
	public:
		TLGameObjectFactory(void) : WindowFactory(TLGameObject::WidgetTypeName) { }
		~TLGameObjectFactory(void){}
	public:
		Window*			createWindow	(const String& name);
		virtual void	destroyWindow	(Window* window)	 
		{ 
			if ( window->getType() == d_type ) 
			{
				delete window; 
			}
		}
	};


}


#endif