//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/17/2006 15:29
//      File_base        : Source1
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "TLGameObject.h"
#include "CEGUIImageset.h"
#include "CEGUIFont.h"
#include "CEGUIPropertyHelper.h"
#include "Time.h"
#include "CEGUIWindowManager.h"
#include "CEGUIExceptions.h"
#include "Ui/UiConfigManager.h"
#ifndef _LAYOUT_EDITOR_
#include "Ui/UiCase/UiItemTip.h"
#include "KWin32Wnd.h"
#endif // _LAYOUT_EDITOR_
#include "CoreUseNameDef.h"

// Start of CEGUI namespace section
namespace CEGUI
{

	namespace TLGameObjectProperties
	{
		void CoolingImage::set(PropertyReceiver* receiver, const String &value)
		{
			const Image* image = PropertyHelper::stringToImage(value);
			static_cast<TLGameObject*>(receiver)->setCoolingImage(image);
		}

		void IntonateImage::set(PropertyReceiver* receiver, const String &value)
		{
			const Image* image = PropertyHelper::stringToImage(value);
			static_cast<TLGameObject*>(receiver)->setIntonateImage(image);
		}

	}

	

	/*************************************************************************
	Constants
	*************************************************************************/
	// type name for this widget
	const utf8	TLGameObject::WidgetTypeName[]				= "TaharezLook/GameObject";
	const utf8	TLGameObject::ImageSetName[]				= "GameObject";		
	const utf8	TLGameObject::HoverEffectName[]				= "HoverEffect";	
	const utf8	TLGameObject::DisabledEffectName[]			= "DisabledEffect";		
	const utf8	TLGameObject::BigFrameName[]				= "ty_biankuangbianse";
	const utf8	TLGameObject::FrameName[]					= "GOFrameImage";
	const utf8	TLGameObject::CoolingEffectName[]			= "CoolingEffect";
	const utf8	TLGameObject::BufferCoolingEffectName[]		= "BuffCoolingEffect";
	const utf8	TLGameObject::IntonateEffectName[]			= "IntonateEffect";
	const utf8	TLGameObject::MiniFrameName[]				= "MiniGOFrameImage";
	const utf8  TLGameObject::BufferBG[]					= "buff_mianban";

	/*************************************************************************
	Constructor
	*************************************************************************/
	TLGameObject::TLGameObject(const String& type, const String& name) :
	PushButton(type, name)
	{
		d_locked				= false;
		d_type					= idle;
		d_state					= idleState;
		d_count					= 0;
		d_intonateTime			= 0;
		d_coolingTime			= 0;
		d_disabledTime			= 0;
		d_playFinished			= false;
		d_frameIdx				= 0;
		d_playImage				= NULL;	
		d_startIntonateTime		= 0;
		d_bufferID				= 0;
		d_passIntonateTime		= 0;
		d_bufferIdx				= 0;
		d_stateNotifyfun		= NULL;
		d_dragging				= false;	
		d_canDrag				= false;
		d_hand					= false;
		d_skillID				= 0;
		d_EdgeframeIdx			= 0;
		d_passivity				= false;
		d_startCoolingTime		= 0;
		d_passCoolingTime		= 0;
		d_gameObject			= "";
		d_gameobjectSet			= "";

		
		d_coolingImage	= NULL;
		d_intonateImage = NULL;
		d_frameImage	= NULL;
		d_bufferBG		= NULL;
		
		Imageset* imgSet = ImagesetManager::getSingleton().getImageset( CoolingEffectName );
		setCoolingImage( &imgSet->getImage( "full_image" ) );
		
		imgSet = ImagesetManager::getSingleton().getImageset( DisabledEffectName );
		setDisabledImage( &imgSet->getImage( "full_image" ) );

//		imgSet = ImagesetManager::getSingleton().getImageset( HoverEffectName );
//		setHoverImage( &imgSet->getImage( "full_image" ) );

		imgSet = ImagesetManager::getSingleton().getImageset( IntonateEffectName );
		setIntonateImage( &imgSet->getImage( "full_image" ) );

		imgSet = ImagesetManager::getSingleton().getImageset( BufferBG );
		if ( imgSet )
			d_bufferBG = &imgSet->getImage( "buff_mianban_da" );

		d_minPlayer = (TLStaticImage*)WindowManager::getSingleton().createWindow(TLStaticImage::WidgetTypeName, d_name + "/__auto_mingoplayer" );
		if ( d_minPlayer )
		{
			addChildWindow( d_minPlayer );
			d_minPlayer->setPosition( Absolute, Point(0,0));
			d_minPlayer->setSize( Absolute, Size( 14, 14));
			d_minPlayer->disable();
			d_minPlayer->setDummyWnd( true );
			d_minPlayer->setBackgroundEnabled( false );
			d_minPlayer->setFrameEnabled(false);
 			d_minPlayer->setCyc( false );	
 			d_minPlayer->setImage( "mingoend", UI_FULL_IMAGESET );
			d_minPlayer->stop();

		}

		d_player = (TLStaticImage*)WindowManager::getSingleton().createWindow(TLStaticImage::WidgetTypeName, d_name + "/__auto_goplayer" );
		if ( d_player )
		{
			addChildWindow( d_player );
			d_player->setPosition( Absolute, Point(0,0));
			d_player->setSize( Absolute, Size( 20, 20));
			d_player->disable();
			d_player->setDummyWnd( true );
			d_player->setBackgroundEnabled( false );
			d_player->setFrameEnabled(false);
 			d_player->setCyc( false );	
 			d_player->setImage( "goend", UI_FULL_IMAGESET );
			d_player->stop();
		}
		
		d_bigPlayer = (TLStaticImage*)WindowManager::getSingleton().createWindow(TLStaticImage::WidgetTypeName, d_name + "/__auto_biggoplayer" );
		if ( d_bigPlayer )
		{
			addChildWindow( d_bigPlayer );
			d_bigPlayer->setPosition( Absolute, Point(0,0));
			d_bigPlayer->setSize( Absolute, Size( 30, 30));
			d_bigPlayer->disable();
			d_bigPlayer->setDummyWnd( true );
			d_bigPlayer->setBackgroundEnabled( false );
			d_bigPlayer->setFrameEnabled(false);
 			d_bigPlayer->setCyc( false );	
 			d_bigPlayer->setImage( "biggoend", UI_FULL_IMAGESET );
			d_bigPlayer->stop();
		}

		d_lockPlayer = (TLStaticImage*)WindowManager::getSingleton().createWindow(TLStaticImage::WidgetTypeName, d_name + "/__auto_lockplayer" );
		if ( d_lockPlayer )
		{
			addChildWindow( d_lockPlayer );
			d_lockPlayer->setPosition( Absolute, Point(0,0));
			d_lockPlayer->setSize( Absolute, Size( 30, 30));
			d_lockPlayer->disable();
			d_lockPlayer->setDummyWnd( true );
			d_lockPlayer->setBackgroundEnabled( false );
			d_lockPlayer->setFrameEnabled(false);
 			d_lockPlayer->setCyc( true );	
 			d_lockPlayer->setImage( "lockeffect", UI_FULL_IMAGESET );
			d_lockPlayer->stop();
		}

		d_Edge = (TLStaticImage*)WindowManager::getSingleton().createWindow(TLStaticImage::WidgetTypeName, d_name + "/__buff_edge" );
		if ( d_Edge )
		{
			addChildWindow( d_Edge );
			d_Edge->setPosition( Absolute, Point(0,0));
			d_Edge->setSize( Absolute, Size( 20, 20));
			d_Edge->disable();
			d_Edge->setDummyWnd( true );
			d_Edge->setBackgroundEnabled( false );
			d_Edge->setFrameEnabled(false);
			d_Edge->setImage( "buff_edge", UI_FULL_IMAGESET );
		}

		addEvent(EventObjectChanged);
	}


	/*************************************************************************
	Destructor
	*************************************************************************/
	TLGameObject::~TLGameObject(void)
	{
	}

	void TLGameObject::setCoolingTime( int ntime, int npasstime )
	{
		d_startCoolingTime = ::GetTickCount() - npasstime * 1000;
		d_passCoolingTime = npasstime;
		
		if ( ntime != 0 )
			setState( coolingState );
		
		d_coolingTime = ntime;
		requestRedraw();
	}

	void TLGameObject::setObject( const GameObject &rGO )
	{
		clear();
		d_intonateTime		= rGO.d_intonateTime;
		d_coolingTime		= rGO.d_coolingTime;
		d_disabledTime		= rGO.d_disabledTime;
		d_count				= rGO.d_count;
		d_state				= rGO.d_state;
		d_type				= rGO.d_type;
		d_bufferIdx			= rGO.d_bufferIdx;
//		d_bufferID			= ::atoi(Utf8ToAnsi( rGO.d_gameobject ));
		d_gameObject		= rGO.d_gameobject;
		d_gameobjectSet		= rGO.d_gameobjectSet;
		d_bufferID			= rGO.d_bufferID;
		d_skillID			= rGO.d_skillID;
		d_EdgeframeIdx		= rGO.d_EdgeframeIdx;
		d_passivity			= rGO.d_passivity;
		d_itempos			= rGO.d_itempos;
		d_genre				= rGO.d_genre;
		d_detail			= rGO.d_detail;
		d_particular		= rGO.d_particular;
		d_level				= rGO.d_level;
		d_group				= rGO.d_group;
				
		Imageset* imgSet = NULL;

		if(rGO.d_gameobject == BACKGROUND_IMAGE)
		{
			imgSet = NULL;
		}
		else
		{
			if ( d_type == minibuffer )
			{
				String name("Mini");
				try
				{
					imgSet = ImagesetManager::getSingleton().getImageset( d_gameobjectSet );
				}
				catch (UnknownObjectException)
				{
					imgSet = ImagesetManager::getSingleton().getImageset( name + ImageSetName );
				}
			}
			else if ( d_type == buffer )
			{
				String name("Middle");
				try
				{
					imgSet = ImagesetManager::getSingleton().getImageset( d_gameobjectSet );
				}
				catch (UnknownObjectException)
				{
					imgSet = ImagesetManager::getSingleton().getImageset( name + ImageSetName );
				}
				d_Edge->show();
			}
			else
			{
				try
				{
					imgSet = ImagesetManager::getSingleton().getImageset( d_gameobjectSet );
				}
				catch (UnknownObjectException)
				{
					imgSet = ImagesetManager::getSingleton().getImageset( ImageSetName );
				}
			}
		}

		if ( imgSet )
		{
			try
			{
				setNormalImage( &imgSet->getImage( rGO.d_gameobject + "_normal" ) );
				setPushedImage( &imgSet->getImage( rGO.d_gameobject + "_pushed" ) );
				setDisabledImage( &imgSet->getImage( rGO.d_gameobject + "_disabled" ) );
			}
			catch (UnknownObjectException)
			{
			}
		}
		else
		{
			d_normalImage = NULL;
			d_pushedImage = NULL;
			d_disabledImage = NULL;
		}

		if ( d_type == item || d_type == shortcut )
		{
			imgSet = ImagesetManager::getSingleton().getImageset( BigFrameName );
			setFrameImage( &imgSet->getImage( "full_image" ) );
		}

		if ( d_type == buffer )
		{
			//d_startIntonateTime = ::time( NULL );
			d_startIntonateTime = ::GetTickCount();
			setState( intonateState );
			Font* fnt = const_cast<Font *>(getFont());
			if ( fnt )
			{
				char szTime[16];
				sprintf( szTime, "%d", d_intonateTime );
				setText( szTime );
				setSize( Absolute, Size( getWidth(Absolute), getHeight(Absolute) + 20));
			}
			imgSet = ImagesetManager::getSingleton().getImageset( FrameName );
			setFrameImage( &imgSet->getImage( "full_image" ) );
		}
		else if ( d_type == minibuffer )
		{
			imgSet = ImagesetManager::getSingleton().getImageset( MiniFrameName );
			setFrameImage( &imgSet->getImage( "full_image" ) );
		}
		else if ( d_hand )
		{
			d_dragging = true;
			d_dragPoint = MouseCursor::getSingleton().getPosition();

			if (getMetricsMode() == Relative)
			{
				d_dragPoint = relativeToAbsolute(d_dragPoint);
			}
			setState( normalState );
		}
		else if ( d_type == shortcut || d_type == item )
		{
		}
		else
		{
			setState( normalState );
		}
		show();
		requestRedraw();
		WindowEventArgs e(this);
		fireEvent(EventObjectChanged, e);
	}

	void TLGameObject::getObject( GameObject &rGO  ) const 
	{
		rGO.d_intonateTime	= d_intonateTime;
		rGO.d_coolingTime	= d_coolingTime;
		rGO.d_disabledTime	= d_disabledTime;
		rGO.d_count			= d_count;
		rGO.d_state			= d_state;
		rGO.d_type			= d_type;
		rGO.d_gameobject	= d_gameObject;
		rGO.d_bufferID		= d_bufferID;
		rGO.d_bufferIdx		= d_bufferIdx;
		rGO.d_skillID		= d_skillID;
		rGO.d_EdgeframeIdx	= d_EdgeframeIdx;
		rGO.d_passivity		= d_passivity;
		rGO.d_itempos		= d_itempos;
		rGO.d_genre			= d_genre;
		rGO.d_detail		= d_detail;
		rGO.d_particular	= d_particular;
		rGO.d_level			= d_level;
		rGO.d_group			= d_group;
		rGO.d_gameobjectSet = d_gameobjectSet;
	}

	TLGameObject::GameObject TLGameObject::getObject() 
	{
		GameObject rGO;
		rGO.d_intonateTime	= d_intonateTime;
		rGO.d_coolingTime	= d_coolingTime;
		rGO.d_disabledTime	= d_disabledTime;
		rGO.d_count			= d_count;
		rGO.d_state			= d_state;
		rGO.d_type			= d_type;
		rGO.d_gameobject	= d_gameObject;
		rGO.d_bufferID		= d_bufferID;
		rGO.d_bufferIdx		= d_bufferIdx;
		rGO.d_skillID		= d_skillID;
		rGO.d_EdgeframeIdx	= d_EdgeframeIdx;
		rGO.d_passivity		= d_passivity;
		rGO.d_itempos		= d_itempos;
		rGO.d_genre			= d_genre;
		rGO.d_detail		= d_detail;
		rGO.d_particular	= d_particular;
		rGO.d_level			= d_level;
		rGO.d_group			= d_group;
		rGO.d_gameobjectSet = d_gameobjectSet;
		return rGO;
	}

	void TLGameObject::setState	( ObjectState eState )
	{
		d_state			= eState;
		d_playFinished	= false;
		d_frameIdx		= 0;
		switch( d_state )
		{
		case selectState:
		case hoverState:
			d_playImage = d_hoverImage;
			break;
		case disableState:
			d_playImage = d_disabledImage;
		    break;
		case intonateState:
			d_playImage = d_intonateImage;
			break;
		case coolingState:
			d_playImage = d_coolingImage;
		    break;
		default:
			d_playImage = NULL;
		    break;
		}
		if ( d_playImage )
		{
			beginUpdate();
		}
		else
		{
			stopUpdate();
		}
		requestRedraw();
	}

	void TLGameObject::clear( void )
	{
		d_type					= idle;
		d_state					= idleState;
		d_count					= 0;
		d_intonateTime			= 0;
		d_coolingTime			= 0;
		d_disabledTime			= 0;
		d_playFinished			= false;
		d_frameIdx				= 0;
		d_playImage				= NULL;
		d_bufferIdx				= 0;
		d_bufferID				= 0;
		d_skillID				= 0;
		d_EdgeframeIdx			= 0;
		d_passivity				= false;
		d_normalImage = NULL;
		d_pushedImage = NULL;
		d_gameObject.clear();
		d_startCoolingTime		= 0;
		d_passCoolingTime		= 0;
		d_startIntonateTime		= 0;
		d_passIntonateTime		= 0;	
		memset( &d_itempos, 0, sizeof(ItemPos) );				
		d_genre					= 0;
		d_detail				= 0;
		d_particular			= 0;
		d_level					= 0;
		d_group					= 0;
		d_minPlayer->stop();
		d_player->stop();
		d_bigPlayer->stop();
		d_lockPlayer->stop();
		d_Edge->hide();
		d_tooltipText.clear();
// 		d_disabledImage.setImage( NULL );
// 		d_disabledEndImage.setImage( NULL );			
//		d_coolingImage.setImage( NULL );
// 		d_coolingEndImage.setImage( NULL );
// 		d_intonateImage.setImage( NULL );
// 		d_intonateEndImage.setImage( NULL );

		hide();

	}
	void TLGameObject::intonate( void )
	{
		switch( d_type )
		{
		case item:
			setState( intonateState );
			break;
		case buffer:
			break;
		case shortcut:
			setState( intonateState );
		case skill:
			setState( intonateState );
		    break;
		}
		requestRedraw();
	}
	void TLGameObject::disable( bool bDisable )
	{
		if ( bDisable )
		{
			switch( d_type )
			{
			case buffer:
				break;
			case item:
			case shortcut:
			case skill:
				setState( disableState );
				break;
			}
		}
		else
		{
			switch( d_type )
			{
			case item:
				break;
			case buffer:
				break;
			case shortcut:
			case skill:
				setState( normalState );
				break;
			}
		}
		
		requestRedraw();
	}
	void TLGameObject::pickup( GameObject &rGO )
	{
		getObject( rGO );
		requestRedraw();
	}	
	void TLGameObject::putdown( const GameObject &rGO )
	{
		requestRedraw();
	}

	void TLGameObject::setNormalImage(const Image* image)
	{
		if (image)
		{
			d_normalImage = image;
			setSize( Absolute, d_normalImage->getSize());
			setState( normalState );
		}
		requestRedraw();
	}

	void TLGameObject::setHoverImage(const Image* image)
	{
		if (image)
		{
			d_hoverImage = image;
			setSize( Absolute, d_hoverImage->getSize());
		}
		requestRedraw();
	}

	void TLGameObject::setPushedImage(const Image* image)
	{
		if (image)
		{
			d_pushedImage = image;
			setSize( Absolute, d_pushedImage->getSize());
		}
		requestRedraw();
	}

	void TLGameObject::setDisabledImage(const Image* image)
	{
		if (image)
		{
			d_disabledImage = image;
			setSize( Absolute, d_disabledImage->getSize());
		}
		requestRedraw();
	}

	void TLGameObject::setFrameImage(const Image* image)
	{
		if (image)
		{
			d_frameImage = image;
			if ( d_type == buffer && d_bufferBG )
			{
				setSize( Absolute, Size(d_bufferBG->getSize().d_width, d_bufferBG->getSize().d_height));	
			} 
			else
			{
				setSize( Absolute, d_frameImage->getSize());	
			}		
		}
		requestRedraw();
	}

	void TLGameObject::setIntonateImage(const Image* image)
	{
		if (image)
		{
			d_intonateImage = image;
		}
		requestRedraw();
	}

	void TLGameObject::setCoolingImage(const Image* image)
	{
		if (image)
		{
			d_coolingImage = image;
		}
		requestRedraw();
	}

	/*************************************************************************
		Perform the rendering for this widget.	
	*************************************************************************/
	void TLGameObject::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
	{
		drawNormal(panelCache, panelAbsPos);
		if ( d_type == minibuffer )
		{
			return;
		}
		switch( d_state )
		{
		case hoverState:
			drawHover(panelCache, panelAbsPos);
			break;
		case selectState:
			drawHover(panelCache, panelAbsPos);
			break;
		case pushedState:
			drawPushed(panelCache, panelAbsPos);
			break;
		case disableState:
			drawDisabled(panelCache, panelAbsPos);
		    break;
		case intonateState:
			drawIntonate(panelCache, panelAbsPos);
			break;
		case coolingState:
			drawCooling(panelCache, panelAbsPos);
		    break;
		default:
			drawIdle(panelCache, panelAbsPos);
		    break;
		}
	}

	void TLGameObject::setTooltipText(const String& tip)
	{
		d_tooltipText = tip;
	}


	/*************************************************************************
	render Widget in normal state	
	*************************************************************************/
	void TLGameObject::drawIdle(KRenderCache* panelCache, Point* panelAbsPos)
	{
		
	}

	void TLGameObject::drawNormal(KRenderCache* panelCache, Point* panelAbsPos)
	{
		Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
		Rect clipper(getPixelRect());
		clipper.offset(offPos);
		
		if (clipper.getWidth() == 0)
		{
			return;
		}
		
		Rect absrect(getUnclippedPixelRect());
		absrect.offset(offPos);

		panelCache->cacheImage(d_normalImage, absrect.getPosition(), clipper);
		
		if(d_type == buffer || d_type == minibuffer )
		{
			panelCache->cacheImage(d_frameImage, absrect.getPosition(), clipper);
		}

		if ( d_type == item || d_type == shortcut )
		{
			panelCache->cacheImage(d_frameImage, absrect.getPosition(), clipper, d_EdgeframeIdx);
		}
		
		//
		// Draw label text
		//
		//if(d_type == buffer || d_type == shortcut)
		if(d_type == shortcut)
		{
			char szTime[16];
			if ( d_count > 1 )
			{
				sprintf( szTime, "%d", d_count );
				
				// LSL物品在快捷栏数量显示在左下角
				Font* font = const_cast<Font *>(getFont());
				Rect numRect;
				numRect.d_left = absrect.d_right - font->getTextExtent(szTime);
				numRect.d_right = absrect.d_right;
				numRect.d_top = absrect.d_bottom - font->getLineSpacing();
				numRect.d_bottom = absrect.d_bottom;

				panelCache->cacheText(AnsiToUtf8(szTime), numRect, numRect, font, LeftAligned, d_normalColour);
			}
		}
		else if(d_type == item)
		{
			char szCount[16];
			if ( d_count > 1 )
			{
				sprintf( szCount, "%d", d_count );
				Font* font = const_cast<Font *>(getFont());

				Rect numRect;
				numRect.d_left = absrect.d_right - font->getTextExtent(szCount);
				numRect.d_right = absrect.d_right;
				numRect.d_top = absrect.d_bottom - font->getLineSpacing();
				numRect.d_bottom = absrect.d_bottom;

				panelCache->cacheText(AnsiToUtf8(szCount), numRect, numRect, font, LeftAligned, d_normalColour);
			}
		}
		absrect.d_top += /*getAbsoluteWidth() - const_cast<Font *>(getFont())->getTextExtent( d_text);*/PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
		absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());

		if ( d_type == buffer )
		{
			d_Edge->show();
			drawBufferCooling(panelCache, panelAbsPos);
			/*char szTime[16];
			if ( (d_intonateTime - d_passIntonateTime) > 0 )
			{
				if ( d_intonateTime - d_passIntonateTime > 86400 )
				{
					sprintf( szTime, "" );
					//sprintf( szTime, "%dd", (d_intonateTime - d_passIntonateTime + 86399) / ( 3600 * 24 ) );		
				}
				else if ( d_intonateTime - d_passIntonateTime > 3600 )
				{
					sprintf( szTime, "%dh", ( d_intonateTime - d_passIntonateTime + 3599 ) / 3600 );
				}
				else if ( d_intonateTime - d_passIntonateTime > 60 )
				{			
					sprintf( szTime, "%dm", ( d_intonateTime - d_passIntonateTime + 59 ) / 60 );
				}	
				else
				{
					sprintf( szTime, "%ds", d_intonateTime - d_passIntonateTime );
				}
				
				// 高度位置偏移在uicfg.ini里设置
				absrect.d_top += KUiCfgLoader::getSingleton().getBuffRestTime().topOffset;
				// 显示时间变色在uicfg.ini里设置
				colour textColor = d_normalColour;
				if( d_intonateTime - d_passIntonateTime <= KUiCfgLoader::getSingleton().getBuffRestTime().warningTime )
					textColor = colour(1, 0, 0, 1);
				
				panelCache->cacheText(AnsiToUtf8(szTime), absrect, clipper, getFont(), Centred, textColor);
				if ( d_count > 1 )
				{
					sprintf( szTime, "%d", d_count );
					absrect.d_top += getAbsoluteWidth() - const_cast<Font *>(getFont())->getTextExtent( d_text);//PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
					absrect.d_left += getAbsoluteHeight() - const_cast<Font *>(getFont())->getTextExtent( AnsiToUtf8( szTime ) );
					
					panelCache->cacheText(AnsiToUtf8(szTime), absrect, clipper, getFont(), Centred, textColor);
				}
			}
			else
			{
				if ( d_stateNotifyfun && d_intonateTime != -1 )
				{
// 					d_stateNotifyfun( d_bufferIdx );
// 					clear();
				}
				

			}//*/
		}
		else
		{
			d_Edge->hide();
		}
		/*if ( (d_type == shortcut || d_type == item )&& d_state == coolingState )
		{
			char szTime[16];
			if ( (d_coolingTime - d_passCoolingTime) > 0 )
			{
				if ( d_coolingTime - d_passCoolingTime > 86400 )
				{
					sprintf( szTime, "%dd", (d_coolingTime - d_passCoolingTime + 86399) / ( 3600 * 24 ) );		
				}
				else if ( d_coolingTime - d_passCoolingTime > 3600 )
				{
					sprintf( szTime, "%dh", ( d_coolingTime - d_passCoolingTime + 3599 ) / 3600 );
				}
				else if ( d_coolingTime - d_passCoolingTime > 60 )
				{			
					sprintf( szTime, "%dm", ( d_coolingTime - d_passCoolingTime + 59 ) / 60 );
				}	
				else
				{
					sprintf( szTime, "%ds", d_coolingTime - d_passCoolingTime );
				}				
				panelCache->cacheText(AnsiToUtf8(szTime), absrect, clipper, getFont(), Centred, d_normalColour);
				if ( d_count > 1 )
				{
					sprintf( szTime, "%d", d_count );
					absrect.d_top += getAbsoluteWidth() - const_cast<Font *>(getFont())->getTextExtent( d_text);//PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
					absrect.d_left += getAbsoluteHeight() - const_cast<Font *>(getFont())->getTextExtent( AnsiToUtf8( szTime ) );
					panelCache->cacheText(AnsiToUtf8(szTime), absrect, clipper, getFont(), Centred, d_normalColour);
				}
			}
		}//*/
	}


	/*************************************************************************
	render Widget in hover / highlight state	
	*************************************************************************/
	void TLGameObject::drawHover(KRenderCache* panelCache, Point* panelAbsPos)
	{
		if ( isEmpty() && d_type != item)
		{
			return;
		}
		Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
		Rect clipper(getPixelRect());
		clipper.offset(offPos);
		
		if (clipper.getWidth() == 0)
		{
			return;
		}
		
		Rect absrect(getUnclippedPixelRect());
		absrect.offset(offPos);

		int frameCount = ImagesetManager::getSingleton().getImageset( HoverEffectName )->getSpr()->GetFrames();
		if ( frameCount > 1 )
		{
			panelCache->cacheImage(d_hoverImage, absrect.getPosition(), clipper, ++d_frameIdx);
		}
		else
		{
			panelCache->cacheImage(d_hoverImage, absrect.getPosition(), clipper);
		}
		
	}


	/*************************************************************************
	render Widget in Pushed state	
	*************************************************************************/
	void TLGameObject::drawPushed(KRenderCache* panelCache, Point* panelAbsPos)
	{
		Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
		Rect clipper(getPixelRect());
		clipper.offset(offPos);
		
		if (clipper.getWidth() == 0)
		{
			return;
		}
		
		Rect absrect(getUnclippedPixelRect());
		absrect.offset(offPos);
		
		// 		panelCache->cacheImage(d_pushedImage, absrect.getPosition(), clipper);
		// 		panelCache->cacheImage(d_pushedImage, absrect.getPosition(), clipper);
		panelCache->cacheImage(d_hoverImage, absrect.getPosition(), clipper);
		panelCache->cacheImage(d_hoverImage, absrect.getPosition(), clipper);
		panelCache->cacheImage(d_hoverImage, absrect.getPosition(), clipper);
	}


	/*************************************************************************
	render Widget in disabled state	
	*************************************************************************/
	void TLGameObject::drawDisabled(KRenderCache* panelCache, Point* panelAbsPos)
	{
		Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
		Rect clipper(getPixelRect());
		clipper.offset(offPos);
		
		if (clipper.getWidth() == 0)
		{
			return;
		}
		
		Rect absrect(getUnclippedPixelRect());
		absrect.offset(offPos);

		if (!d_disabledImage)
		{
			// 如果找不到图标的话找一张默认图代替
			Imageset* imgSet = ImagesetManager::getSingleton().getImageset( DisabledEffectName );
			setDisabledImage( &imgSet->getImage( "full_image" ) );
		}
		panelCache->cacheImage(d_disabledImage, absrect.getPosition(), clipper);

	}

	void TLGameObject::drawCooling(KRenderCache* panelCache, Point* panelAbsPos)
	{
		if ( isEmpty() )
		{
			return;
		}
		Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
		Rect clipper(getPixelRect());
		clipper.offset(offPos);
		
		if (clipper.getWidth() == 0)
		{
			return;
		}
		
		Rect absrect(getUnclippedPixelRect());
		absrect.offset(offPos);
		
		int frameCount = ImagesetManager::getSingleton().getImageset( CoolingEffectName )->getSpr()->GetFrames();
		if ( frameCount > 1 )
		{
			panelCache->cacheImage(d_coolingImage, absrect.getPosition(), clipper, ((float)d_passCoolingTime / (float)d_coolingTime * 100.0f));
		}
		else
		{
			panelCache->cacheImage(d_coolingImage, absrect.getPosition(), clipper);
		}
		
		absrect.d_top += PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
		absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());
		
		if ( (d_type == shortcut || d_type == item )&& d_state == coolingState )
		{
			char szTime[16];
			if ( (d_coolingTime - d_passCoolingTime) > 0 )
			{
				if ( d_coolingTime - d_passCoolingTime > 86400 )
				{
					sprintf( szTime, "%dd", (d_coolingTime - d_passCoolingTime + 86399) / ( 3600 * 24 ) );		
				}
				else if ( d_coolingTime - d_passCoolingTime > 3600 )
				{
					sprintf( szTime, "%dh", ( d_coolingTime - d_passCoolingTime + 3599 ) / 3600 );
				}
				else if ( d_coolingTime - d_passCoolingTime > 60 )
				{			
					sprintf( szTime, "%dm", ( d_coolingTime - d_passCoolingTime + 59 ) / 60 );
				}	
				else
				{
					sprintf( szTime, "%ds", d_coolingTime - d_passCoolingTime );
				}				
				panelCache->cacheText(AnsiToUtf8(szTime), absrect, clipper, getFont(), Centred, d_normalColour);
				if ( d_count > 1 )
				{
					sprintf( szTime, "%d", d_count );
					absrect.d_top += getAbsoluteWidth() - const_cast<Font *>(getFont())->getTextExtent( d_text);//PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
					absrect.d_left += getAbsoluteHeight() - const_cast<Font *>(getFont())->getTextExtent( AnsiToUtf8( szTime ) );
					panelCache->cacheText(AnsiToUtf8(szTime), absrect, clipper, getFont(), Centred, d_normalColour);
				}
			}
		}

	}
	
	void TLGameObject::drawBufferCooling(KRenderCache* panelCache, Point* panelAbsPos)
	{
		return;
		if ( isEmpty() )
		{
			return;
		}
		Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
		Rect clipper(getPixelRect());
		clipper.offset(offPos);
		
		if (clipper.getWidth() == 0)
		{
			return;
		}
		
		Rect absrect(getUnclippedPixelRect());
		absrect.offset(offPos);
				
		Imageset* imgSet = ImagesetManager::getSingleton().getImageset( BufferCoolingEffectName );
		int frameCount = imgSet->getSpr()->GetFrames();
		const Image* buffCoolingImage = &imgSet->getImage( "full_image" );
		if ( frameCount > 1 )
		{
			if ( d_intonateTime - d_passIntonateTime > 86400 )
				return;

			int i = (int)((float)d_passIntonateTime / (float)d_intonateTime * 100.0f);
			if ( i < 0 || i == frameCount )
					return;
			
			panelCache->cacheImage(buffCoolingImage, absrect.getPosition(), clipper, i);
		}
		else
		{
			panelCache->cacheImage(buffCoolingImage, absrect.getPosition(), clipper);
		}
		
	}

	void TLGameObject::drawIntonate(KRenderCache* panelCache, Point* panelAbsPos)
	{
		Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
		Rect clipper(getPixelRect());
		clipper.offset(offPos);
		
		if (clipper.getWidth() == 0)
		{
			return;
		}
		
		Rect absrect(getUnclippedPixelRect());
		absrect.offset(offPos);
		
		if ( d_type == buffer )
		{
			if ( d_intonateTime - d_passIntonateTime <= 86400 )
				panelCache->cacheImage(d_bufferBG, absrect.getPosition(), clipper);
			
			char szTime[16];
			if ( (d_intonateTime - d_passIntonateTime) > 0 )
			{
				if ( d_intonateTime - d_passIntonateTime > 86400 )
				{
					sprintf( szTime, "" );
					//sprintf( szTime, "%dd", (d_intonateTime - d_passIntonateTime + 86399) / ( 3600 * 24 ) );		
				}
				else if ( d_intonateTime - d_passIntonateTime > 3600 )
				{
					sprintf( szTime, "%dh", ( d_intonateTime - d_passIntonateTime + 3599 ) / 3600 );
				}
				else if ( d_intonateTime - d_passIntonateTime > 60 )
				{			
					sprintf( szTime, "%dm", ( d_intonateTime - d_passIntonateTime + 59 ) / 60 );
				}	
				else
				{
					sprintf( szTime, "%ds", d_intonateTime - d_passIntonateTime );
				}
				
				// 高度位置偏移在uicfg.ini里设置
				absrect.d_top += KUiCfgLoader::getSingleton().getBuffRestTime().topOffset;
				absrect.d_top += 10;
				// 显示时间变色在uicfg.ini里设置
				colour textColor = d_normalColour;
				if( d_intonateTime - d_passIntonateTime <= KUiCfgLoader::getSingleton().getBuffRestTime().warningTime )
					textColor = colour(1, 0, 0, 1);
				
				panelCache->cacheText(AnsiToUtf8(szTime), absrect, clipper, getFont(), Centred, textColor);
				if ( d_count > 1 )
				{
					setFont("arial-9");
					sprintf( szTime, "%d", d_count );
					absrect.d_top -= ((KUiCfgLoader::getSingleton().getBuffRestTime().topOffset) + 2);//PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
					//absrect.d_left -= 5;
					panelCache->cacheText(AnsiToUtf8(szTime), absrect, clipper, getFont(false), LeftAligned, textColor);
				}
			}
		}
		else
		{
			panelCache->cacheImage(d_intonateImage, absrect.getPosition(), clipper);
		}
	}
	
	void TLGameObject::breathe()
	{
		if ( d_intonateTime )
		{
			//DWORD passTime = (time( NULL ) - d_startIntonateTime);
			DWORD passTime = (::GetTickCount() - d_startIntonateTime)/1000;
			if ( d_intonateTime >= passTime )
			{
				d_passIntonateTime = passTime;		
			}
			else
			{
				//d_startCoolingTime = ::time( NULL );
				d_startCoolingTime = ::GetTickCount();
				// buff不至cooling状态
				if ( d_type != buffer )
					setState( coolingState );
			}
		}
		else
		{
			if ( d_state == intonateState && d_coolingTime == 0 )
			{
				setState( normalState );
			}
		}

		if ( d_intonateTime == 0 && d_coolingTime != 0 &&  d_state == intonateState )
		{
			//d_startCoolingTime = ::time( NULL );
			d_startCoolingTime = ::GetTickCount();
			setState( coolingState );
		}

		if ( d_coolingTime && (d_type == shortcut || d_type == item) && d_state == coolingState ) 
		{
			//DWORD passTime = (time( NULL ) - d_startCoolingTime);
			DWORD passTime = ( GetTickCount() - d_startCoolingTime )/1000;
			if ( d_coolingTime > passTime )
			{
				d_passCoolingTime = passTime;		
			}
			else
			{
				setState( normalState );
				switch( d_type )
				{
				case minibuffer:
					if (d_minPlayer && !d_minPlayer->isPlaying())
					{
						d_minPlayer->play();
					}					
					break;					
				case buffer:
					if (d_player && !d_player->isPlaying())
					{
						d_player->play();
					}					
					break;
				case shortcut:
				case item:
				case skill:
					if (d_bigPlayer && !d_bigPlayer->isPlaying() )
					{
						d_bigPlayer->play();
					}					
				    break;
				}
			}
		}

		if ( !d_playFinished )
		{
			int frameCount = 1;
			if ( d_playImage )
			{
				const Image *img = d_playImage;
				if ( img )
				{
					String imgSetName = img->getImagesetName();
					frameCount = ImagesetManager::getSingleton().getImageset( imgSetName )->getSpr()->GetFrames();
				}
			}
			if (frameCount > 1)
			{
				if(d_frameIdx >= frameCount)
				{
					d_frameIdx = 0;
				}
			}
		}
		requestRedraw();
		PushButton::breathe();
	}

	/*************************************************************************
	Handler for when WM_CHAR arrive
	*************************************************************************/
	void TLGameObject::onCharacter(KeyEventArgs& e)
	{
		e.handled = true;
	}

	/*************************************************************************
		Handler for when the mouse moves
	*************************************************************************/
	void TLGameObject::onMouseMove(MouseEventArgs& e)
	{
		// this is needed to discover whether mouse is in the widget area or not.
		// The same thing used to be done each frame in the rendering method,
		// but in this version the rendering method may not be called every frame
		// so we must discover the internal widget state here - which is actually
		// more efficient anyway.

		// base class processing
		Window::onMouseMove(e);

		updateInternalState(e.position);

		if (d_dragging && d_hand  )
		{
// 			Vector2 delta(screenToWindow(e.position));
// 
// 			if (getMetricsMode() == Relative)
// 			{
// 				delta = relativeToAbsolute(delta);
// 			}
// 
// 			// calculate amount that window has been moved
// 			delta -= d_dragPoint;
// 
// 			// move the window.  *** Again: Titlebar objects should only be attached to FrameWindow derived classes. ***
// 			offsetPixelPosition(delta);
		}
		e.handled = true;
	}

	/*************************************************************************
		Update the internal state of the Widget
	*************************************************************************/
	void TLGameObject::updateInternalState(const Point& mouse_pos)
	{
		if ( d_state == disableState || d_state == coolingState || d_state == intonateState )
		{
			return;
		}
		bool oldstate = (d_state == hoverState );

		// if input is captured, but not by 'this', then we never hover highlight
		const Window* capture_wnd = getCaptureWindow();

		if ((capture_wnd == NULL) || (capture_wnd == this))
		{
			Window* sheet = getRoot();

			if (sheet != NULL)
			{
				// check if hovering highlight is required, which is basically ("mouse over widget" XOR "widget pushed").
				if ((this == sheet->getChildAtPosition(mouse_pos)) && d_state != pushedState && d_state != selectState)
				{
					setState( hoverState );
				}

			}

		}

		// if state has changed, trigger a re-draw
		if (oldstate != (d_state == hoverState))
		{
			requestRedraw();
		}

	}

	/*************************************************************************
		Handler for mouse button pressed events
	*************************************************************************/
	void TLGameObject::onMouseButtonDown(MouseEventArgs& e)
	{
		if (e.button == LeftButton)
		{
		//	if (captureInput())  xiehong 2007-11-23 点击一个物品、快捷栏或者buff没必要抢输入焦点
			{
				if ( d_state != disableState && d_state != coolingState )
				{
					// 为BUFF解决点击时计时消失问题
					if ( d_state != intonateState && d_type != buffer)
					{
						setState( pushedState );
					}
				}
				updateInternalState(e.position);
				requestRedraw();
			}
			
			// event was handled by us.
			e.handled = true;
		}

		// default processing
		Window::onMouseButtonDown(e);


// 		if ( e.button == LeftButton && d_parent )
// 		{
// 			if (d_hand)
// 			{
// 				d_dragging = true;
// 				d_dragPoint = screenToWindow(e.position);
// 
// 				if (getMetricsMode() == Relative)
// 				{
// 					d_dragPoint = relativeToAbsolute(d_dragPoint);
// 				}
// 				e.handled = true;	
// 			}		
// 		}

	}

	/*************************************************************************
		Handler for when mouse capture is lost
	*************************************************************************/
	void TLGameObject::onCaptureLost(WindowEventArgs& e)
	{
		// Default processing
		Window::onCaptureLost(e);
		if ( d_state != selectState && d_state != disableState 
			&& d_state != coolingState && d_state != intonateState )
		{
			if ( d_state != idleState )
				setState( normalState );
			else
				setState( idleState );
		}

		updateInternalState(MouseCursor::getSingleton().getPosition());
		requestRedraw();

		// event was handled by us.
		e.handled = true;
	}

	/*************************************************************************
		Handler for when mouse leaves the widget
	*************************************************************************/
	void TLGameObject::onMouseLeaves(MouseEventArgs& e)
	{
		// deafult processing
		Window::onMouseLeaves(e);

		if ( d_state != selectState && d_state != disableState 
			&& d_state != coolingState && d_state != intonateState )
		{
			if ( d_state != idleState )
				setState( normalState );
			else
				setState( idleState );
		}
		requestRedraw();

		if ( !d_tooltipText.empty() )
		{
#ifndef _LAYOUT_EDITOR_
			KUiItemTip::Hide();
#endif //_LAYOUT_EDITOR_
		}

		e.handled = true;
	}

	void	TLGameObject::onMouseButtonUp( MouseEventArgs& e )
	{
		if ((e.button == LeftButton) && d_state == pushedState)
		{
			Window* sheet = getRoot();

			if (sheet != NULL)
			{
				// if mouse was released over this widget
				if (this == sheet->getChildAtPosition(e.position))
				{
					// fire event
					WindowEventArgs args(this);
					onClicked(args);
				}

			}
			setState(hoverState);
			e.handled = true;
		}

		if ( e.button == LeftButton && d_hand )
		{
			d_dragging = false;
		}

		// default handling
		ButtonBase::onMouseButtonUp(e);

	}

	void TLGameObject::onClicked(WindowEventArgs& e)
	{
		PushButton::onClicked(e);
	}

	void TLGameObject::onMouseEnters(MouseEventArgs& e)
	{
		// set the mouse cursor
		MouseCursor::getSingleton().setImage(getMouseCursor());

		TLTooltip*tip = static_cast<TLTooltip*>(getTooltip());

		if (tip && !d_tooltipText.empty() )
		{
			Window* pWin = getParent();
			if ( pWin )
			{
				Point pos;
				if ( d_type == shortcut )
				{
					Window* pPWin = pWin->getParent();
					if ( pPWin )
					{
						pos = getPosition(Absolute) + pWin->getPosition( Absolute ) + pPWin->getPosition(Absolute);
					}
				}
				else
				{
					pos = getPosition(Absolute) + pWin->getPosition( Absolute );
				}

#ifndef _LAYOUT_EDITOR_
				KUiItemTip::GetSingleton();
				KUiItemTip::GetSingleton().show( Utf8ToAnsi( d_tooltipText ), getUnclippedInnerRect() );
#endif  //_LAYOUT_EDITOR_
			}
//			tip->setTargetWindow(this);
//			tip->show();
		}

		e.handled = true;
	}

	void TLGameObject::onSized(WindowEventArgs& e)
	{
		// default processing
		PushButton::onSized(e);
	}

	void TLGameObject::offsetPixelPosition(const Vector2& offset)
	{
		UVector2 uOffset;

		if (getMetricsMode() == Relative)
		{
			Size sz = getParentSize();

			uOffset.d_x = cegui_reldim((sz.d_width != 0) ? offset.d_x / sz.d_width : 0);
			uOffset.d_y = cegui_reldim((sz.d_height != 0) ? offset.d_y / sz.d_height : 0);
		}
		else
		{
			uOffset.d_x = cegui_absdim(PixelAligned(offset.d_x));
			uOffset.d_y = cegui_absdim(PixelAligned(offset.d_y));
		}

		setWindowPosition(d_area.getPosition() + uOffset);

		///    
/*		UVector2 uOffset;

    if (getMetricsMode() == Relative)
    {
        Size sz = getParentSize();

        uOffset.d_x = cegui_reldim((sz.d_width != 0) ? offset.d_x / sz.d_width : 0);
        uOffset.d_y = cegui_reldim((sz.d_height != 0) ? offset.d_y / sz.d_height : 0);
    }
    else
    {
        uOffset.d_x = cegui_absdim(PixelAligned(offset.d_x));
        uOffset.d_y = cegui_absdim(PixelAligned(offset.d_y));
    }
	UVector2 oldPoint = getWindowPosition();

	UVector2 newPoint = d_area.getPosition() + uOffset;
	setWindowPosition(newPoint);		
		
	Point newPos = getPosition( Absolute );
	Size sz = getSize( Absolute );
	if ((newPos.d_x >= 0 && (newPos.d_x + sz.d_width) < g_GetScreenWidth()) && 
		(newPos.d_y >= 0 && (newPos.d_y + sz.d_height) < g_GetScreenHeight()))
	{
		setWindowPosition(newPoint);
	}
	else
	{
		if ( newPos.d_x < 0 )
		{
			newPos.d_x = 0;
		}
		if ((newPos.d_x + sz.d_width) > g_GetScreenWidth() )
		{
			newPos.d_x = g_GetScreenWidth() - sz.d_width;
		}
		if ( newPos.d_y < 0 )
		{
			newPos.d_y = 0;
		}
		if ( (newPos.d_y + sz.d_height) > g_GetScreenHeight())
		{
			newPos.d_y = g_GetScreenHeight() - sz.d_height;
		}
		setPosition(Absolute,newPos);
	}*/
	}

	void TLGameObject::updateSelf(float elapsed)
	{
		breathe();
	}

	void TLGameObject::setCanDrag(bool canDrag)
	{
		if(canDrag)
		{
			show();
			Window* parent = d_parent;
			if(parent)
			{
				parent->removeChildWindow(this);
				parent->addChildWindow(this);
			}
			setPosition(Absolute, MouseCursor::getSingleton().getPosition()
				- Point(getWidth(Absolute)/2, getHeight(Absolute)/2+5));
		}
		else
		{
			hide();
		}
		d_canDrag = canDrag;
	}

	//////////////////////////////////////////////////////////////////////////
	/*************************************************************************

	Factory Methods

	*************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	/*************************************************************************
	Create, initialise and return a TLGameObject
	*************************************************************************/
	Window* TLGameObjectFactory::createWindow(const String& name)
	{
		return new TLGameObject(d_type, name);
	}
} // End of  CEGUI namespace section
