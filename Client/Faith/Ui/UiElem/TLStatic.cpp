/************************************************************************
	filename: 	TLStatic.cpp
	created:	5/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Taharez Look static widgets & factories.
*************************************************************************/
/*************************************************************************
    Crazy Eddie's GUI System (http://www.cegui.org.uk)
    Copyright (C)2004 - 2005 Paul D Turner (paul@cegui.org.uk)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*************************************************************************/
#include "TLStatic.h"
#include "elements/CEGUIStaticText.h"
#include "elements/CEGUIStaticImage.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUIWindowManager.h"
#include "TLMiniVertScrollbar.h"
#include "TLMiniHorzScrollbar.h"
#include "Ui/LayoutRender.h"

#ifndef _LAYOUT_EDITOR_
#include "Ui/UiCase/UiItemTip.h"
#include "Ui/UiCase/UiFSBible.h"
#endif  //_LAYOUT_EDITOR_

#include "KWin32Wnd.h"


#define  BTN_TYPE "TaharezLook/Button"
#define  BTN_NAME "_HelpBtn"
namespace TLStaticImageProperty
{
	using namespace CEGUI;
	
	String	StaticImageName::get( const PropertyReceiver *receiver ) const
	{
		return static_cast<const TLStaticImage*>(receiver)->getStaticImageName();
	}
	
	void	StaticImageName::set( PropertyReceiver* receiver, const String& value )
	{
		static_cast<TLStaticImage *>(receiver)->setStaticImageName(value);
	}

	String	HelpButtonXPos::get( const PropertyReceiver *receiver ) const
	{
		float temp = static_cast<const TLStaticImage*>(receiver)->getHelpBtnXPos();
		return PropertyHelper::floatToString(temp);
	}
	
	void	HelpButtonXPos::set( PropertyReceiver* receiver, const String& value )
	{
		static_cast<TLStaticImage *>(receiver)->setHelpBtnXPos(PropertyHelper::stringToFloat(value));
	}

	String	HelpButtonYPos::get( const PropertyReceiver *receiver ) const
	{
		float temp = static_cast<const TLStaticImage*>(receiver)->getHelpBtnYPos();
		return PropertyHelper::floatToString(temp);
	}
	
	void	HelpButtonYPos::set( PropertyReceiver* receiver, const String& value )
	{
		static_cast<TLStaticImage *>(receiver)->setHelpBtnYPos(PropertyHelper::stringToFloat(value));
	}

	String	HelpButtonWidth::get( const PropertyReceiver *receiver ) const
	{
		float temp = static_cast<const TLStaticImage*>(receiver)->getHelpBtnWidth();
		return PropertyHelper::floatToString(temp);
	}
	
	void	HelpButtonWidth::set( PropertyReceiver* receiver, const String& value )
	{
		static_cast<TLStaticImage *>(receiver)->setHelpBtnWdith(PropertyHelper::stringToFloat(value));
	}

	String	HelpButtonHeight::get( const PropertyReceiver *receiver ) const
	{
		float temp = static_cast<const TLStaticImage*>(receiver)->getHelpBtnHeight();
		return PropertyHelper::floatToString(temp);
	}
	
	void	HelpButtonHeight::set( PropertyReceiver* receiver, const String& value )
	{
		static_cast<TLStaticImage *>(receiver)->setHelpBtnHeight(PropertyHelper::stringToFloat(value));
	}

	//get help button normal image
	String HelpBtnNormalImage::get( const PropertyReceiver *receiver ) const
	{
		const Image *pImage = static_cast<const TLStaticImage*>(receiver)->getHelpBtnNormalImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set help button normal image
	void   HelpBtnNormalImage::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLStaticImage *>(receiver)->setHelpBtnNormalImage(PropertyHelper::stringToImage(Value));
	}

	//get help button hover image
	String HelpBtnHoverImage::get( const PropertyReceiver *receiver ) const
	{
		const Image *pImage = static_cast<const TLStaticImage*>(receiver)->getHelpBtnHoverImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set help button hover image
	void   HelpBtnHoverImage::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLStaticImage *>(receiver)->setHelpBtnHoverImage(PropertyHelper::stringToImage(Value));
	}

	//get help button pushed image
	String HelpBtnPushedImage::get( const PropertyReceiver *receiver ) const
	{
		const Image *pImage = static_cast<const TLStaticImage*>(receiver)->getHelpBtnPushedImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set help button pushed image
	void   HelpBtnPushedImage::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLStaticImage *>(receiver)->setHelpBtnPushedImage(PropertyHelper::stringToImage(Value));
	}

	//get help button enable property
	String HelpBtnEnable::get( const PropertyReceiver *receiver ) const
	{
		const bool bEnable = static_cast<const TLStaticImage*>(receiver)->getHelpBtnEnable();
		return PropertyHelper::boolToString(bEnable);
	}

	//set help button enable property
	void   HelpBtnEnable::set( PropertyReceiver *receiver, const String &value )
	{
		static_cast<TLStaticImage *>(receiver)->setHelpBtnEnable(PropertyHelper::stringToBool(value));
	}
}
// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
const utf8	TLStaticText::WidgetTypeName[]		= "TaharezLook/StaticText";
const utf8	TLStaticImage::WidgetTypeName[]		= "TaharezLook/StaticImage";


// component widget type names
const utf8*	TLStaticText::HorzScrollbarTypeName	= TLMiniHorzScrollbar::WidgetTypeName;
const utf8*	TLStaticText::VertScrollbarTypeName	= TLMiniVertScrollbar::WidgetTypeName;


/*************************************************************************
	Routine to do some common initialisation of static widgets
*************************************************************************/
void initTaharezStatic(Static* s)
{
	Imageset* iset = ImagesetManager::getSingleton().getImageset((utf8*)"TaharezLook");
	
	s->setFrameImages(
		&iset->getImage((utf8*)"StaticTopLeft"),
		&iset->getImage((utf8*)"StaticTopRight"),
		&iset->getImage((utf8*)"StaticBottomLeft"),
		&iset->getImage((utf8*)"StaticBottomRight"),
		&iset->getImage((utf8*)"StaticLeft"),
		&iset->getImage((utf8*)"StaticTop"),
		&iset->getImage((utf8*)"StaticRight"),
		&iset->getImage((utf8*)"StaticBottom")
		);

	s->setBackgroundImage(&iset->getImage((utf8*)"StaticBackdrop"));

	s->setFrameEnabled(false);
	s->setBackgroundEnabled(false);
}


//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	
	TLStaticText methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
TLStaticText::TLStaticText(const String& type, const String& name) : StaticText(type, name)
{
	d_layoutRender = NULL;
}

TLStaticText::~TLStaticText()
{
	if(d_layout != NULL)
	{
		d_layout->Release();
		delete d_layout;
		d_layout = NULL;
	}
}

void TLStaticText::useLayout()
{
	if(d_layout != NULL)
		return;
	
	if(d_layoutRender != NULL)
	{
		d_layoutRender->Release();
		delete d_layoutRender;
	}

	d_layoutRender = new LayoutRender();
	ILayout* layout = NULL;
	CreateLayout(&layout, d_layoutRender);

	if(NULL == layout)
		return;
	
	d_layout = layout;
}

void TLStaticText::fitLayoutSize(bool adjWdith)
{
	LORect area = d_layout->getRenderArea(adjWdith);
	if (d_frameEnabled)
	{
		area.setWidth(area.getWidth() + d_left_width + d_right_width);
		area.setHeight(area.getHeight() + d_top_height + d_bottom_height);
	}
	this->setWidth(Absolute, area.getWidth());
	this->setHeight(Absolute, area.getHeight());
	requestRedraw();
}

void TLStaticText::populateRenderCache()
{
// 	if(d_layout)
// 	{
// 		Static::populateRenderCache();
// 		d_layout->Render(0,0);
// 	}
// 	else
// 	{
		StaticText::populateRenderCache();
//	}
}

void TLStaticText::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{	
	Static::drawSelf(panelCache, panelAbsPos);

	Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
	Rect clipper(getPixelRect());
	clipper.offset(offPos);

	if (clipper.getWidth() == 0)
	{
		return;
	}

	Rect absarea(getUnclippedPixelRect());
	absarea.offset(offPos);

	const Font* font = getFont();
    if (font == 0)
        return;
	
	panelCache->cacheText(d_text, absarea.offset(d_offset), clipper,
		font, (TextFormatting)d_horzFormatting, d_textCols);

	if(NULL == d_layout)
		return;
	
	Point absPos(absarea.getPosition());

	d_layoutRender->setDrawPanel(panelCache);
	d_layout->Render(absPos.d_x + d_layoutXoff, absPos.d_y + d_layoutYoff, 0);
}

void TLStaticText::drawSelf()
{
	Window::drawSelf();
	if(NULL == d_layout)
		return;
	
	Point absPos(getUnclippedPixelRect().getPosition());
// 	if (d_frameEnabled)
// 	{
// 		absPos.d_x	+= d_left_width;
// 		absPos.d_y	+= d_top_height;
// 	}
	d_layout->Render(absPos.d_x + d_layoutXoff, absPos.d_y + d_layoutYoff, 0);
}

void TLStaticText::setTextBorderMode(int borderMode)
{
	d_layout->setBorderMode(borderMode);
}

void TLStaticText::setLayoutOffset(int x, int y)
{
	d_layoutXoff = x;
	d_layoutYoff = y;
}

Point TLStaticText::getLayoutOffset()
{
	return Point(d_layoutXoff, d_layoutYoff);
}

/*************************************************************************
	create and return a pointer to a Scrollbar widget for use as
	vertical scroll bar	
*************************************************************************/
Scrollbar* TLStaticText::createVertScrollbar(const String& name) const
{
	Scrollbar* sbar = (Scrollbar*)WindowManager::getSingleton().createWindow(VertScrollbarTypeName, name);

	// set min/max sizes
	sbar->setMinimumSize(Size(0.0125f, 0.0f));
	sbar->setMaximumSize(Size(0.0125f, 1.0f));

	return sbar;
}


/*************************************************************************
	create and return a pointer to a Scrollbar widget for use as
	horizontal scroll bar	
*************************************************************************/
Scrollbar* TLStaticText::createHorzScrollbar(const String& name) const
{
	Scrollbar* sbar = (Scrollbar*)WindowManager::getSingleton().createWindow(HorzScrollbarTypeName, name);

	// set min/max sizes
	sbar->setMinimumSize(Size(0.0f, 0.016667f));
	sbar->setMaximumSize(Size(1.0f, 0.016667f));

	return sbar;
}


/*************************************************************************
	Initialises the Window based object ready for use.
*************************************************************************/
void TLStaticText::initialise(void)
{
	d_layout = NULL;
	d_layoutXoff = 0;
	d_layoutYoff = 0;
	StaticText::initialise();
	initTaharezStatic(this);
}


/*************************************************************************
	Setup size and position for the component widgets attached to this
	TLStaticText
*************************************************************************/
void TLStaticText::performChildWindowLayout()
{
//     // base class layout
//     Static::performChildWindowLayout();
// 
// 	// set desired size for vertical scroll-bar
// 	Size v_sz(0.05f, 1.0f);
// 	d_vertScrollbar->setSize(v_sz);
// 
// 	// get the actual size used for vertical scroll bar.
// 	v_sz = absoluteToRelative(d_vertScrollbar->getAbsoluteSize());
// 
// 
// 	// set desired size for horizontal scroll-bar
// 	Size h_sz(1.0f, 0.0f);
// 
// 	if (getAbsoluteHeight() != 0.0f)
// 	{
// 		h_sz.d_height = (getAbsoluteWidth() * v_sz.d_width) / getAbsoluteHeight();
// 	}
// 
// 	// adjust length to consider width of vertical scroll bar if that is visible
// 	if (d_vertScrollbar->isVisible())
// 	{
// 		h_sz.d_width -= v_sz.d_width;
// 	}
// 
// 	d_horzScrollbar->setSize(h_sz);
// 
// 	// get actual size used
// 	h_sz = absoluteToRelative(d_horzScrollbar->getAbsoluteSize());
// 
// 
// 	// position vertical scroll bar
// 	d_vertScrollbar->setPosition(Point(1.0f - v_sz.d_width, 0.0f));
// 
// 	// position horizontal scroll bar
// 	d_horzScrollbar->setPosition(Point(0.0f, 1.0f - h_sz.d_height));
}

/*************************************************************************
	Overridden Event Handling Functions
*************************************************************************/
void	TLStaticText::onCharacter(KeyEventArgs& e)
{
	e.handled = true;
}



void TLStaticText::onMouseEnters(MouseEventArgs& e)
{
	StaticText::onMouseEnters(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip::GetSingleton();
		KUiItemTip::GetSingleton().show(Utf8ToAnsi( d_tooltipText ), getUnclippedInnerRect(), KUiItemTip::BottomLeft);
#endif //_LAYOUT_EDITOR_
	}
}

void TLStaticText::onMouseLeaves(MouseEventArgs& e)
{
	StaticText::onMouseLeaves(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip::Hide();
#endif //_LAYOUT_EDITOR_
	}
}


//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	
	TLStaticImage methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////

/*************************************************************************
	Initialises the Window based object ready for use.
*************************************************************************/
const utf8 *TLStaticImage::HelpBtnWidgetType		= TLButton::WidgetTypeName;

const float TLStaticImage::HelpBtnPositionY						= 0.0f;
const float TLStaticImage::HelpBtnPositionX						= 0.0f;
const float TLStaticImage::HelpBtnSizeHeight					= 0.0f;
const float TLStaticImage::HelpBtnSizeWidth						= 0.0f;

TLStaticImageProperty::StaticImageName		TLStaticImage::d_sImageName;
TLStaticImageProperty::HelpButtonHeight		TLStaticImage::d_sHelpBtnHeight;
TLStaticImageProperty::HelpButtonWidth		TLStaticImage::d_sHelpBtnWidth;
TLStaticImageProperty::HelpButtonXPos		TLStaticImage::d_sHelpBtnXPos;
TLStaticImageProperty::HelpButtonYPos		TLStaticImage::d_sHelpBtnYPos;
TLStaticImageProperty::HelpBtnNormalImage	TLStaticImage::d_sHelpBtnNormalImage;
TLStaticImageProperty::HelpBtnHoverImage	TLStaticImage::d_sHelpBtnHoverImage;
TLStaticImageProperty::HelpBtnPushedImage	TLStaticImage::d_sHelpBtnPushedImage;
TLStaticImageProperty::HelpBtnEnable		TLStaticImage::d_sHelpEnable;

void TLStaticImage::initialise(void)
{
	StaticImage::initialise();
	initTaharezStatic(this);
	addStaticProperties();
	d_firstShow = true;
}

void TLStaticImage::show()
{
	StaticImage::show();
}

void TLStaticImage::onMouseButtonDown(MouseEventArgs& e)
{
	StaticImage::onMouseButtonDown( e );
	if ( e.button == LeftButton && d_parent )
	{
		if (d_dragEnabled)
		{
			// we want all mouse inputs from now on
			if (captureInput())
			{
				// initialise the dragging state
				d_dragging = true;
				d_dragPoint = screenToWindow(e.position);

				if (getMetricsMode() == Relative)
				{
					d_dragPoint = relativeToAbsolute(d_dragPoint);
				}
				e.handled = true;	
			}
		}		
	}
}

void TLStaticImage::onMouseButtonUp(MouseEventArgs& e)
{
	StaticImage::onMouseButtonUp( e );
	if ( e.button == LeftButton && d_dragEnabled )
	{
		releaseInput();
		d_dragging = false;
		e.handled = true;
	}
}

void TLStaticImage::onMouseMove(MouseEventArgs& e)
{
	StaticImage::onMouseMove( e );	

	if (d_dragging && d_dragEnabled  )
	{
		Vector2 delta(screenToWindow(e.position));

		if (getMetricsMode() == Relative)
		{
			delta = relativeToAbsolute(delta);
		}

		// calculate amount that window has been moved
		delta -= d_dragPoint;

		// move the window.  *** Again: Titlebar objects should only be attached to FrameWindow derived classes. ***
		offsetPixelPosition(delta);

		e.handled = true;
	}
}

void TLStaticImage::onMouseEnters(MouseEventArgs& e)
{
	StaticImage::onMouseEnters(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip::GetSingleton();
		KUiItemTip::GetSingleton().show(Utf8ToAnsi( d_tooltipText ), getUnclippedInnerRect(), KUiItemTip::BottomLeft);
#endif //_LAYOUT_EDITOR_
	}
}

void TLStaticImage::onMouseLeaves(MouseEventArgs& e)
{
	StaticImage::onMouseLeaves(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip::Hide();
#endif //_LAYOUT_EDITOR_
	}
}


void TLStaticImage::offsetPixelPosition(const Vector2& offset)
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
	}
		
}




bool TLStaticImage::handleHelp( const CEGUI::EventArgs& args )
{
#ifndef _LAYOUT_EDITOR_
	
	String tHelpName = getHelpPlane();
	if (tHelpName == String(""))
	{
		return false;
	}
	hide();
	KUiFSBible::getSingleton().SetHelpItemSelectStatus(tHelpName);
	KUiFSBible::getSingleton().ShowHelp();

#endif //_LAYOUT_EDITOR_
	return true;
}

void TLStaticImage::addStaticProperties()
{
	addProperty( &d_sHelpBtnYPos );
	addProperty( &d_sHelpBtnXPos );
	addProperty( &d_sHelpBtnWidth );
	addProperty( &d_sHelpBtnHeight );
	addProperty( &d_sImageName );
	addProperty( &d_sHelpEnable );
	addProperty( &d_sHelpBtnNormalImage);
	addProperty( &d_sHelpBtnHoverImage);
	addProperty( &d_sHelpBtnPushedImage);
}

//get help button x position
const float TLStaticImage::getHelpBtnXPos() const
{
	return d_btnPos.d_x;
}

//set help button x position
void TLStaticImage::setHelpBtnXPos( const float xpos )
{
	d_btnPos.d_x = xpos;
}

//get help button y position
const float TLStaticImage::getHelpBtnYPos() const
{
	return d_btnPos.d_y;
}

//set help button y position
void TLStaticImage::setHelpBtnYPos( const float ypos )
{
	d_btnPos.d_y = ypos;
}

//get help button width
const float TLStaticImage::getHelpBtnWidth() const
{
	return d_btnSize.d_width;
}

//set help button width
void TLStaticImage::setHelpBtnWdith( const float width )
{
	d_btnSize.d_width = width;
}

//get help button height
const float TLStaticImage::getHelpBtnHeight() const
{
	return d_btnSize.d_height;
}

//set help button height
void TLStaticImage::setHelpBtnHeight( const float height )
{
	d_btnSize.d_height = height;
}


const String TLStaticImage::getStaticImageName() const
{
	return d_windowName;
}

void TLStaticImage::setStaticImageName( const String staticname )
{
	d_windowName = staticname;
}

const Image *TLStaticImage::getHelpBtnNormalImage( void ) const
{
	//return d_pHelpBtn->getNormalImage()->getImage();
	return d_pHelpBtnNormal;
}

void  TLStaticImage::setHelpBtnNormalImage( const Image *pNewImage )
{
	//RenderableImage image;
	//image.setImage( pNewImage );
	//d_pHelpBtn->setNormalImage( &image );
	d_pHelpBtnNormal = const_cast<Image *>(pNewImage);
}

const Image *TLStaticImage::getHelpBtnHoverImage( void ) const
{
	//return d_pHelpBtn->getHoverImage()->getImage();
	return d_pHelpBtnHover; 
}

void  TLStaticImage::setHelpBtnHoverImage( const Image *pNewImage )
{
	//RenderableImage image;
	//image.setImage( pNewImage );
	//d_pHelpBtn->setHoverImage( &image );
	d_pHelpBtnHover =  const_cast<Image *>(pNewImage);
}

const Image *TLStaticImage::getHelpBtnPushedImage( void ) const
{
	//return d_pHelpBtn->getPushedImage()->getImage();
	return d_pHelpBtnPushed;
}

void  TLStaticImage::setHelpBtnPushedImage( const Image *pNewImage )
{
	//RenderableImage image;
	//image.setImage( pNewImage );
	//d_pHelpBtn->setPushedImage( &image );
	d_pHelpBtnPushed = const_cast<Image *>(pNewImage);
}

const bool TLStaticImage::getHelpBtnEnable( void ) const
{
	return d_bBtnEnable;
}

void TLStaticImage::setHelpBtnEnable( const bool bEnable )
{
	d_bBtnEnable = bEnable;
}

void TLStaticImage::CreateHelpBtn( const String& name )
{
	if (d_pHelpBtn == NULL)
	{
		d_pHelpBtn = static_cast<PushButton *>(WindowManager::getSingleton().createWindow( HelpBtnWidgetType, name ));
		this->addChildWindow(d_pHelpBtn);
		d_pHelpBtn->setSize( Absolute, d_btnSize );
		d_pHelpBtn->setPosition( Absolute, d_btnPos );

		d_pHelpBtn->setNormalImage( d_pHelpBtnNormal );

		d_pHelpBtn->setHoverImage( d_pHelpBtnHover );
		
		d_pHelpBtn->setPushedImage( d_pHelpBtnPushed );
	}
}

void TLStaticImage::onShown(WindowEventArgs& e)
{
	StaticImage::onShown(e);
	String btnName;

	if (getName() != String(""))
	{
		btnName =  getName() + BTN_NAME;
	}
	const char *temp = btnName.c_str();
	try
	{
		if ( d_bBtnEnable )
		{
			d_pHelpBtn = static_cast<CEGUI::TLButton *>(getChild( btnName ));
			if ( d_pHelpBtn != NULL )
			{
				d_pHelpBtn->show();
				if ( d_sHelpToolTip != String("") )
				{
					d_pHelpBtn->setTooltipText(d_sHelpToolTip);
				}
				
				if(d_firstShow)
				{
					d_firstShow = false;
					d_pHelpBtn->subscribeEvent( TLButton::EventMouseClick, Event::Subscriber(&TLStaticImage::handleHelp, this ));
				}
			}
		}
	}
	catch (...)
	{
		
	}
}

void TLStaticImage::DoShow()
{
	WindowEventArgs e(NULL);
	onShown(e);
}
//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Factory Methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Create, initialise and return a StaticText for the Taharez Scheme
*************************************************************************/
Window* TLStaticTextFactory::createWindow(const String& name)
{
	return new TLStaticText(d_type, name);
}


/*************************************************************************
	Create, initialise and return a StaticImage for the Taharez Scheme
*************************************************************************/
Window* TLStaticImageFactory::createWindow(const String& name)
{
	return new TLStaticImage(d_type, name);
}

/*************************************************************************]
    Shake properties
 **************************************************************************/
//Shake Properties
#define MAX_SHAKE_INTERVAL 10

TLStaticImage::tagSHAKE_EFFECT_INFO::tagSHAKE_EFFECT_INFO()
:m_bShakeFlag(false),m_Distance(0),m_DirX(false),m_DirY(false),m_State(false),m_dwTimer(0)
{
	m_InitPos.d_x=0;
    m_InitPos.d_y=0;
}

void TLStaticImage::Shake(const const bool bXShake,const bool bYShake,unsigned long dwDistance)
{
   
   if (!m_ShakeOption.m_bShakeFlag && (bXShake || bYShake ) && dwDistance!=0 )
   {
	   	beginUpdate();
	    //Init
        m_ShakeOption.m_bShakeFlag=true;

		if (bXShake)
			m_ShakeOption.m_DirX=true;

		if (bYShake)
			m_ShakeOption.m_DirY=true;

		m_ShakeOption.m_Distance=dwDistance;

		//Set Timer
		m_ShakeOption.m_dwTimer=GetTickCount();

		//Set Begin Pos
        CEGUI::Point cur=getPosition(Absolute);
		m_ShakeOption.m_InitPos.d_x=cur.d_x;
		m_ShakeOption.m_InitPos.d_y=cur.d_y;
		
		int iDx=0;
		int iDy=0;

		if (bXShake)
			iDx=dwDistance;
		if (bYShake)
			iDy=dwDistance;

        cur.d_x=m_ShakeOption.m_InitPos.d_x+(float)iDx;
		cur.d_y=m_ShakeOption.m_InitPos.d_y+(float)iDy;
		
		setPosition(Absolute,cur);
		requestRedraw();

		m_ShakeOption.m_State=true;
   }
}

void TLStaticImage::MoveTo(const int iDestX,const int iDestY, const int iDx,const int iDy,const bool bShake)
{	
	Point CurPos=getAbsolutePosition();
	if (( abs(CurPos.d_x-iDestX)<=abs(iDx)
		&& abs(CurPos.d_y-iDestY)<=abs(iDy)
		)
		|| (iDx>0 && iDestX - CurPos.d_x<0)
		|| (iDx<0 && iDestX - CurPos.d_x>0)
		|| (iDy>0 && CurPos.d_y-iDestY>0)
		|| (iDy<0 && CurPos.d_y-iDestY<0)
		)
	{
		Point finalPos;
		finalPos.d_x = iDestX;
		finalPos.d_y = iDestY;

		setPosition(Absolute,finalPos);
		return ;
	}//endif
	
	m_MoveOption.m_bMoveFlag=true;
	m_MoveOption.m_DestX=iDestX;
	m_MoveOption.m_DestY=iDestY;
	m_MoveOption.m_Dx=iDx;
	m_MoveOption.m_Dy=iDy;
	m_MoveOption.m_MoveTimer=GetTickCount();
	m_MoveOption.m_bDragable=isDragMovingEnabled();	
	
	setDragMovingEnabled(false);
	beginUpdate();
}

void TLStaticImage::ShakeBreathe()
{
	if (m_ShakeOption.m_bShakeFlag)
	{
		if (GetTickCount()-m_ShakeOption.m_dwTimer>=MAX_SHAKE_INTERVAL)
		{
			if (m_ShakeOption.m_Distance!=0)
			{
				//Set Begin Pos
                CEGUI::Point cur;	
				int iDx=0;
				int iDy=0;
				
				if (m_ShakeOption.m_DirX)
				{
					if (m_ShakeOption.m_State)
					{
					   iDx=-m_ShakeOption.m_Distance;
					   m_ShakeOption.m_State=false;
					   m_ShakeOption.m_Distance-=1;
					}//endif
					else
					{
						iDx=m_ShakeOption.m_Distance;
						m_ShakeOption.m_State=true;
					}//end else
				}//endif

				if (m_ShakeOption.m_DirY)
				{
					if (m_ShakeOption.m_State)
					{
						iDy=-m_ShakeOption.m_Distance;
						m_ShakeOption.m_State=false;
						m_ShakeOption.m_Distance-=1;
					}//endif
					else
					{
						iDy=m_ShakeOption.m_Distance;
						m_ShakeOption.m_State=true;
					}//end else
				}//endif
				
				cur.d_x=m_ShakeOption.m_InitPos.d_x+(float)iDx;
				cur.d_y=m_ShakeOption.m_InitPos.d_y+(float)iDy;
	 	        setPosition(Absolute,cur);
				requestRedraw();
			}
			else
			{
				m_ShakeOption.m_bShakeFlag=false;
				m_ShakeOption.m_DirX=false;
				m_ShakeOption.m_DirY=false;
				m_ShakeOption.m_Distance=0;
				m_ShakeOption.m_dwTimer=0;

				setPosition(Absolute,m_ShakeOption.m_InitPos);
				stopUpdate();
			}//end else

		}//endif
		else
		{
			m_ShakeOption.m_dwTimer=GetTickCount();	
		}//end else
		
	}//endif
}


/*************************************************************************]
    move properties
 **************************************************************************/

TLStaticImage::tagMOVE_INFO::tagMOVE_INFO()
:m_Dx(0),m_Dy(0),m_DestX(0),m_DestY(0),m_MoveTimer(GetTickCount()),m_bMoveFlag(false),m_bShakeable(false)
{	}


void TLStaticImage::updateSelf(float elapsed)
{
	StaticImage::updateSelf(elapsed);
	
	if (m_ShakeOption.m_bShakeFlag)
		ShakeBreathe();
	
	if (m_MoveOption.m_bMoveFlag)
		PosMoveBreathe();
}

#define NORMAL_POS_MOVE_INTERVAL 2
void TLStaticImage::PosMoveBreathe()
{
	unsigned long dwCurrent=GetTickCount();
	if (dwCurrent-m_MoveOption.m_MoveTimer>=NORMAL_POS_MOVE_INTERVAL)
	{
		Point CurPos=getAbsolutePosition();
		if (( abs(CurPos.d_x-m_MoveOption.m_DestX)<=abs(m_MoveOption.m_Dx)
			&& abs(CurPos.d_y-m_MoveOption.m_DestY)<=abs(m_MoveOption.m_Dy)
			)
			|| (m_MoveOption.m_Dx>0 && CurPos.d_x+m_MoveOption.m_Dx-m_MoveOption.m_DestX>0)
			|| (m_MoveOption.m_Dx<0 && CurPos.d_x-m_MoveOption.m_Dx-m_MoveOption.m_DestX<0)
			|| (m_MoveOption.m_Dy>0 && CurPos.d_y+m_MoveOption.m_Dy-m_MoveOption.m_DestY>0)
			|| (m_MoveOption.m_Dy<0 && CurPos.d_y-m_MoveOption.m_Dy-m_MoveOption.m_DestY<0)
			)
		{
			Point finalPos;
			finalPos.d_x = m_MoveOption.m_DestX;
			finalPos.d_y = m_MoveOption.m_DestY;

			setPosition(Absolute,finalPos);

			bool bXShake=false;
			bool bYShake=false;
			
			if (m_MoveOption.m_Dx!=0)
				bXShake=true;
			
			if (m_MoveOption.m_Dy!=0)
				bYShake=true;

			if (m_MoveOption.m_bShakeable)
				Shake(bXShake,bYShake,4);
			
			m_MoveOption.m_DestX=0;
			m_MoveOption.m_DestY=0;
			m_MoveOption.m_Dx=0;
			m_MoveOption.m_Dy=0;
			m_MoveOption.m_MoveTimer=0;
			m_MoveOption.m_bMoveFlag=false;
			requestRedraw();
			setDragMovingEnabled(m_MoveOption.m_bMoveFlag);
			stopUpdate();
		}//endif
		else
		{
			CurPos.d_x+=m_MoveOption.m_Dx;
			CurPos.d_y+=m_MoveOption.m_Dy;
			setPosition(Absolute,CurPos);
			requestRedraw();
		}//end else 
		
		m_MoveOption.m_MoveTimer=GetTickCount();
	}//endif

}

} // End of  CEGUI namespace section
