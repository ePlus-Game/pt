/************************************************************************
	filename: 	TLMiniHorzScrollbar.cpp
	created:	2/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Taharez mini horizontal scroll bar.
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
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUIWindowManager.h"
#include "TLMiniHorzScrollbar.h"
#include "TLMiniHorzScrollbarThumb.h"
#include "TLButton.h"
#include "CEGUIPropertyHelper.h"

namespace TLMiniHorzScrollbarProperties
{
	using namespace CEGUI;
	//get scroll body image
	String MiniHorzScrollBody::get( const PropertyReceiver *receiver ) const
	{
		const Image *pImage = static_cast<const TLMiniHorzScrollbar*>(receiver)->getScrollBodyImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set scroll body image
	void   MiniHorzScrollBody::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLMiniHorzScrollbar *>(receiver)->setScrollBodyImage(PropertyHelper::stringToImage(Value));
	}

	//get scroll normal left button image
	String MiniHorzLeftNormal::get( const PropertyReceiver *receiver ) const
	{
		const Image *pImage = static_cast<const TLMiniHorzScrollbar*>(receiver)->getScrollLeftNormalImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set scroll normal left button image
	void   MiniHorzLeftNormal::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLMiniHorzScrollbar *>(receiver)->setScrollLeftNormalImage(PropertyHelper::stringToImage(Value));
	}

	//get scroll pushed left button image
		String MiniHorzLeftPushed::get( const PropertyReceiver *receiver ) const
	{
		const Image *pImage = static_cast<const TLMiniHorzScrollbar*>(receiver)->getScrollLeftPushedImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set scroll pushed left button image
	void   MiniHorzLeftPushed::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLMiniHorzScrollbar *>(receiver)->setScrollLeftPushedImage(PropertyHelper::stringToImage(Value));
	}

	//get scroll hover left button image
	String MiniHorzLeftHover::get( const PropertyReceiver *receiver ) const
	{
		const Image *pImage = static_cast<const TLMiniHorzScrollbar*>(receiver)->getScrollLeftHoverImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set scroll pushed left button image
	void   MiniHorzLeftHover::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLMiniHorzScrollbar *>(receiver)->setScrollLeftHoverImage(PropertyHelper::stringToImage(Value));
	}

	//get scroll normal right button image
	String MiniHorzRightNormal::get( const PropertyReceiver *receiver ) const 
	{
		const Image *pImage = static_cast<const TLMiniHorzScrollbar*>(receiver)->getScrollRightNormalImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set scroll normal right button image
	void   MiniHorzRightNormal::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLMiniHorzScrollbar *>(receiver)->setScrollRightNormalImage(PropertyHelper::stringToImage(Value));
	}

	//get scroll pushed right button image
	String MiniHorzRightPushed::get( const PropertyReceiver *receiver ) const 
	{
		const Image *pImage = static_cast<const TLMiniHorzScrollbar*>(receiver)->getScrollRightPushedImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set scroll pushed right button image
	void   MiniHorzRightPushed::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLMiniHorzScrollbar *>(receiver)->setScrollRightPushedImage(PropertyHelper::stringToImage(Value));
	}

	//get scroll hover right button image
	String MiniHorzRightHover::get( const PropertyReceiver *receiver ) const 
	{
		const Image *pImage = static_cast<const TLMiniHorzScrollbar*>(receiver)->getScrollRightHoverImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set scroll hover right button image
	void   MiniHorzRightHover::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLMiniHorzScrollbar *>(receiver)->setScrollRightHoverImage(PropertyHelper::stringToImage(Value));
	}

	//get scroll hover thumb  button image
	String MiniHorzScrollThumbHover::get( const PropertyReceiver *receiver ) const
	{
		const Image *pImage = static_cast<const TLMiniHorzScrollbar*>(receiver)->getScrollThumbNormalImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set scroll hover thumb  button image
	void   MiniHorzScrollThumbHover::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLMiniHorzScrollbar *>(receiver)->setScrollThumbNormalImage(PropertyHelper::stringToImage(Value));
	}

	//get scroll normal thumb  button image
	String MiniHorzScrollThumbNormal::get( const PropertyReceiver *receiver ) const
	{
		const Image *pImage = static_cast<const TLMiniHorzScrollbar*>(receiver)->getScrollThumbHoverImage();
		return pImage ? PropertyHelper::imageToString(pImage) : String("");
	}

	//set scroll normal thumb  button image
	void   MiniHorzScrollThumbNormal::set( PropertyReceiver *receiver, const String &Value )
	{
		static_cast<TLMiniHorzScrollbar *>(receiver)->setScrollThumbHoverImage(PropertyHelper::stringToImage(Value));
	}
}

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const utf8	TLMiniHorzScrollbar::WidgetTypeName[]	= "TaharezLook/HorizontalScrollbar";

// Progress bar image names
const utf8	TLMiniHorzScrollbar::ImagesetName[]						= "TaharezLook";
const utf8	TLMiniHorzScrollbar::ScrollbarBodyImageName[]			= "MiniHorzScrollBarSegment";
const utf8	TLMiniHorzScrollbar::LeftButtonNormalImageName[]		= "MiniHorzScrollLeftNormal";
const utf8	TLMiniHorzScrollbar::LeftButtonPushedImageName[]		= "MiniHorzScrollLeftNormal";
const utf8	TLMiniHorzScrollbar::LeftButtonHighlightImageName[]		= "MiniHorzScrollLeftHover";
const utf8	TLMiniHorzScrollbar::RightButtonNormalImageName[]		= "MiniHorzScrollRightNormal";
const utf8	TLMiniHorzScrollbar::RightButtonPushedImageName[]		= "MiniHorzScrollRightNormal";
const utf8	TLMiniHorzScrollbar::RightButtonHighlightImageName[]	= "MiniHorzScrollRightHover";

// some layout stuff
const float	TLMiniHorzScrollbar::ThumbPositionY	= 0.15f;
const float	TLMiniHorzScrollbar::ThumbHeight	= 0.7f;				
const float	TLMiniHorzScrollbar::BodyPositionY	= 0.3f;			
const float	TLMiniHorzScrollbar::BodyHeight		= 0.4f;

// type names for the component widgets
const utf8*	TLMiniHorzScrollbar::ThumbWidgetType			= TLMiniHorzScrollbarThumb::WidgetTypeName;
const utf8*	TLMiniHorzScrollbar::IncreaseButtonWidgetType	= TLButton::WidgetTypeName;
const utf8*	TLMiniHorzScrollbar::DecreaseButtonWidgetType	= TLButton::WidgetTypeName;

/*************************************************************************
	likun
*************************************************************************/
// Properties names for the control
TLMiniHorzScrollbarProperties::MiniHorzScrollBody			TLMiniHorzScrollbar::d_miniScrolBody;
TLMiniHorzScrollbarProperties::MiniHorzScrollThumbHover		TLMiniHorzScrollbar::d_miniScrolThumbHorver;
TLMiniHorzScrollbarProperties::MiniHorzScrollThumbNormal	TLMiniHorzScrollbar::d_miniScrolThumbNormal;
TLMiniHorzScrollbarProperties::MiniHorzLeftHover			TLMiniHorzScrollbar::d_miniScrolLeftHover;
TLMiniHorzScrollbarProperties::MiniHorzLeftPushed			TLMiniHorzScrollbar::d_miniScrolLeftPushed;
TLMiniHorzScrollbarProperties::MiniHorzLeftNormal			TLMiniHorzScrollbar::d_miniScrolLeftNormal;
TLMiniHorzScrollbarProperties::MiniHorzRightHover			TLMiniHorzScrollbar::d_miniScrolRightHover;
TLMiniHorzScrollbarProperties::MiniHorzRightPushed			TLMiniHorzScrollbar::d_miniScrolRightPushed;
TLMiniHorzScrollbarProperties::MiniHorzRightNormal			TLMiniHorzScrollbar::d_miniScrolRightNormal;


/*************************************************************************
	Constructor for Taharez mini horizontal scroll bar widgets
*************************************************************************/
TLMiniHorzScrollbar::TLMiniHorzScrollbar(const String& type, const String& name) :
	Scrollbar(type, name)
{
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);

	// setup cache of image pointers
	d_body = &iset->getImage(ScrollbarBodyImageName);
	addMiniHorzScrollProperties();
}


/*************************************************************************
	Destructor for Taharez mini horizontal scroll bar widgets
*************************************************************************/
TLMiniHorzScrollbar::~TLMiniHorzScrollbar(void)
{
}

/*************************************************************************
	Likun Implement
*************************************************************************/
const Image *TLMiniHorzScrollbar::getScrollBodyImage() const
{
	return d_body;
}

void  TLMiniHorzScrollbar::setScrollBodyImage( const Image *pNewImage )
{
	d_body = pNewImage;
}

void  TLMiniHorzScrollbar::setScrollLeftNormalImage( const Image *pNewImage )
{
	d_decrease->setHeight(Absolute, pNewImage->getHeight());
	d_decrease->setWidth(Absolute, pNewImage->getWidth());

	d_decrease->setNormalImage( pNewImage );
	d_decrease->setDisabledImage( pNewImage );
}

const Image *TLMiniHorzScrollbar::getScrollLeftNormalImage() const
{
	return d_decrease->getNormalImage();
}

const Image *TLMiniHorzScrollbar::getScrollLeftPushedImage() const
{
	return d_decrease->getPushedImage();
}

void  TLMiniHorzScrollbar::setScrollLeftPushedImage( const Image *pNewImage )
{
	d_decrease->setPushedImage( pNewImage );
}

void  TLMiniHorzScrollbar::setScrollLeftHoverImage( const Image *pNewImage )
{
	d_decrease->setHoverImage( pNewImage );
}

const  Image *TLMiniHorzScrollbar::getScrollLeftHoverImage() const
{
	return d_decrease->getHoverImage();
}

void  TLMiniHorzScrollbar::setScrollRightNormalImage( const Image *pNewImage )
{
	d_increase->setWidth(Absolute, pNewImage->getWidth());
	d_increase->setHeight(Absolute, pNewImage->getHeight());

	d_increase->setPosition(Point(absoluteToRelativeX(getAbsoluteWidth() - pNewImage->getWidth()), 0.0f));
	d_increase->setNormalImage( pNewImage );
	d_increase->setDisabledImage( pNewImage );
}

const  Image *TLMiniHorzScrollbar::getScrollRightNormalImage() const
{
	return d_increase->getNormalImage();
}


void  TLMiniHorzScrollbar::setScrollRightPushedImage( const Image *pNewImage )
{
	d_increase->setPushedImage( pNewImage );
}

const  Image *TLMiniHorzScrollbar::getScrollRightPushedImage() const
{
	return d_increase->getPushedImage();
}

void  TLMiniHorzScrollbar::setScrollRightHoverImage( const Image *pNewImage )
{
	d_increase->setHoverImage( pNewImage );
}


const  Image *TLMiniHorzScrollbar::getScrollRightHoverImage() const
{
	return d_increase->getHoverImage();
}

const Image *TLMiniHorzScrollbar::getScrollThumbHoverImage() const
{
	return d_thumb->getHoverImage();
}

void  TLMiniHorzScrollbar::setScrollThumbHoverImage( const Image *pNewImage )
{
	const char *temp = pNewImage->getName().c_str();

	float  height = pNewImage->getHeight();
	float  width  = pNewImage->getWidth();
	d_thumb->setHeight( Absolute, pNewImage->getHeight() );
	d_thumb->setWidth( Absolute, pNewImage->getWidth() );

	d_thumb->setHoverImage( pNewImage );

	float yPos = (getHeight(Absolute)	- d_thumb->getHeight(Absolute)) / 2;
	d_thumb->setYPosition( Absolute, yPos );
}

const Image *TLMiniHorzScrollbar::getScrollThumbNormalImage() const
{
	return d_thumb->getNormalImage();
}

void  TLMiniHorzScrollbar::setScrollThumbNormalImage( const Image *pNewImage )
{
	d_thumb->setHeight( Absolute, pNewImage->getHeight() );
	d_thumb->setWidth( Absolute, pNewImage->getWidth() );

	d_thumb->setNormalImage( pNewImage );

	float yPos = (getHeight(Absolute) - d_thumb->getHeight(Absolute)) / 2;
	d_thumb->setYPosition( Absolute, yPos);
}

void  TLMiniHorzScrollbar::addMiniHorzScrollProperties()
{
	addProperty(&d_miniScrolBody);
	addProperty(&d_miniScrolLeftNormal);
	addProperty(&d_miniScrolLeftPushed);
	addProperty(&d_miniScrolLeftHover);
	addProperty(&d_miniScrolRightNormal);
	addProperty(&d_miniScrolRightPushed);
	addProperty(&d_miniScrolRightHover);
	addProperty(&d_miniScrolThumbNormal);
	addProperty(&d_miniScrolThumbHorver);
}
/*************************************************************************
	create a PushButton based widget to use as the increase button for
	this scroll bar.
*************************************************************************/
PushButton* TLMiniHorzScrollbar::createIncreaseButton(const String& name) const
{
	// create the widget
	TLButton* btn = (TLButton*)WindowManager::getSingleton().createWindow(IncreaseButtonWidgetType, name);

	// perform some initialisation
	btn->setZLevel(Window::Top);
	
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);
	btn->setNormalImage(&iset->getImage(RightButtonNormalImageName));
	btn->setDisabledImage(&iset->getImage(RightButtonNormalImageName));

	btn->setHoverImage(&iset->getImage(RightButtonHighlightImageName));
	btn->setPushedImage(&iset->getImage(RightButtonHighlightImageName));

	btn->setWidth(Absolute, btn->getNormalImage()->getWidth());
	btn->setHeight(Absolute, btn->getNormalImage()->getHeight());

	return btn;
}


/*************************************************************************
	create a PushButton based widget to use as the decrease button for
	this scroll bar.
*************************************************************************/
PushButton* TLMiniHorzScrollbar::createDecreaseButton(const String& name) const
{
	// create the widget
	TLButton* btn = (TLButton*)WindowManager::getSingleton().createWindow(DecreaseButtonWidgetType, name);

	// perform some initialisation
	btn->setZLevel(Window::Top);

	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);
	btn->setNormalImage(&iset->getImage(LeftButtonNormalImageName));
	btn->setDisabledImage(&iset->getImage(LeftButtonNormalImageName));

	btn->setHoverImage(&iset->getImage(LeftButtonHighlightImageName));
	btn->setPushedImage(&iset->getImage(LeftButtonHighlightImageName));
		
	btn->setWidth(Absolute, btn->getNormalImage()->getWidth());
	btn->setHeight(Absolute, btn->getNormalImage()->getHeight());
	//btn->setEnabled(false);

	return btn;
}


/*************************************************************************
	create a Thumb based widget to use as the thumb for this scroll bar.
*************************************************************************/
Thumb* TLMiniHorzScrollbar::createThumb(const String& name) const
{
	// create the widget
	TLMiniHorzScrollbarThumb* thumb = (TLMiniHorzScrollbarThumb*)WindowManager::getSingleton().createWindow(ThumbWidgetType, name);

	// perform some initialisation
	thumb->setHorzFree(true);
	thumb->setYPosition(ThumbPositionY);
	thumb->setHeight(ThumbHeight);

	return thumb;
}


/*************************************************************************
	layout the scroll bar component widgets
*************************************************************************/
void TLMiniHorzScrollbar::performChildWindowLayout()
{
    Scrollbar::performChildWindowLayout();

// 	Size bsz;
// 	bsz.d_width = bsz.d_height = 0;
// 	// install button sizes
// 	d_increase->setSize(absoluteToRelative(bsz));
// 	d_decrease->setSize(absoluteToRelative(bsz));

	// position buttons
	d_decrease->setPosition(Point(0.0f, 0.0f));
	d_increase->setPosition(Point(0.0f, 0.0f));

	// this will configure thumb widget appropriately
	updateThumb();
}


/*************************************************************************
	update the size and location of the thumb to properly represent the
	current state of the scroll bar
*************************************************************************/
void TLMiniHorzScrollbar::updateThumb(void)
{
	// calculate actual padding values to use.
	float slideTrackXPadding = d_decrease->getAbsoluteWidth();

	// calculate maximum extents for thumb positioning.
	float posExtent		= d_documentSize - d_pageSize;
	float slideExtent	= max(0.0f, getAbsoluteWidth() - (2 * slideTrackXPadding) - d_thumb->getAbsoluteWidth());

	// Thumb does not change size with document length, we just need to update position and range
	d_thumb->setHorzRange(absoluteToRelativeX(slideTrackXPadding), absoluteToRelativeX(slideTrackXPadding + slideExtent));
	d_thumb->setXPosition(absoluteToRelativeX(slideTrackXPadding + (d_position * (slideExtent / posExtent))));

	requestRedraw();
}


/*************************************************************************
	return value that best represents current scroll bar position given
	the current location of the thumb.
*************************************************************************/
float TLMiniHorzScrollbar::getValueFromThumb(void) const
{
	// calculate actual padding values to use.
	float slideTrackXPadding = d_decrease->getAbsoluteWidth();

	// calculate maximum extents for thumb positioning.
	float posExtent		= d_documentSize - d_pageSize;
	float slideExtent	= getAbsoluteWidth() - (2 * slideTrackXPadding) - d_thumb->getAbsoluteWidth();

	return	(d_thumb->getAbsoluteXPosition() - slideTrackXPadding) / (slideExtent / posExtent);
}


/*************************************************************************
	Given window location 'pt', return a value indicating what change
	should be made to the scroll bar.
*************************************************************************/
float TLMiniHorzScrollbar::getAdjustDirectionFromPoint(const Point& pt) const
{
	Rect absrect(d_thumb->getUnclippedPixelRect());
	float slideTrackXPadding = d_decrease->getAbsoluteWidth();
	
	// calculate maximum extents for thumb positioning.
	float posExtent		= d_documentSize - d_pageSize;
	float slideExtent	= getAbsoluteWidth() - (2 * slideTrackXPadding) - d_thumb->getAbsoluteWidth();

	
	if (pt.d_x < absrect.d_left)
	{
		return -(absrect.d_left - pt.d_x) / (slideExtent / posExtent);
	}
	else if (pt.d_x > absrect.d_right)
	{
		return (pt.d_x - absrect.d_right) / (slideExtent / posExtent);
	}
	else
	{
		return 0.0f;
	}

}


/*************************************************************************
	Perform rendering for this widget
*************************************************************************/
void TLMiniHorzScrollbar::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{
	Size bsz;
	//bsz.d_width = bsz.d_height = getAbsoluteHeight();
	bsz.d_width = d_increase->getNormalImage()->getWidth();
	bsz.d_height= d_increase->getNormalImage()->getHeight();

	if ( d_increase->getSize(Absolute) == Size(0, 0) && 
		 d_decrease->getSize(Absolute) == Size(0, 0) &&
		 bsz != Size(0, 0))
	{
		// install button sizes
		d_increase->setSize(absoluteToRelative(bsz));
		d_decrease->setSize(absoluteToRelative(bsz));

		// position buttons
		d_decrease->setPosition(Point(0.0f, 0.0f));
		d_increase->setPosition(Point(absoluteToRelativeX(getAbsoluteWidth() - bsz.d_width), 0.0f));
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

	//
	// Render bar body
	//
	float button_width = d_decrease->getAbsoluteWidth();

	Point pos(absrect.d_left + button_width, absrect.d_top + PixelAligned(absrect.getHeight() * BodyPositionY));
	Size	sz(absrect.getWidth() - PixelAligned(button_width * 0.5f), PixelAligned(absrect.getHeight() * BodyHeight));
	
	panelCache->cacheImage(d_body, pos, clipper);
	//d_body->draw(pos, sz, clipper, colours);

	/*//
	// Render slider-track
	//
	Vector3 pos1(absrect.d_left, absrect.d_top, z);
	pos1.d_y += absrect.getHeight() * ThumbPositionY;
	pos1.d_x = absrect.getWidth();
	pos1.d_z = System::getSingleton().getRenderer()->getZLayer(1);

	sz.d_height	= absrect.getHeight() * ThumbHeight;
	sz.d_width	= d_thumbTrack->getWidth();

	int segments = (int)((absrect.getWidth() / d_thumbTrack->getWidth()) + 0.99f);

	for (int i = 0; i < segments; ++i)
	{
		d_thumbTrack->draw(pos1, sz, clipper, colours);
		pos.d_x += sz.d_width;
	}*/
}

/*************************************************************************
	Handler for when WM_CHAR arrive
*************************************************************************/
void TLMiniHorzScrollbar::onCharacter(KeyEventArgs& e)
{
	e.handled = true;
}



//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Factory Methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Create, initialise and return a TLMiniHorzScrollbar
*************************************************************************/
Window* TLMiniHorzScrollbarFactory::createWindow(const String& name)
{
	return new TLMiniHorzScrollbar(d_type, name);
}

} // End of  CEGUI namespace section
