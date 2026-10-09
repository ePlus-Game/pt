/************************************************************************
	filename: 	TLVertScrollbar.cpp
	created:	2/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Taharez Vertical Scrollbar widget
				(Large version of scrollbar)
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
#include "TLVertScrollbar.h"
#include "TLVertScrollbarThumb.h"
#include "TLButton.h"
#include "CEGUIPropertyHelper.h"

namespace TLVertScrollbarProperties
{
	using namespace CEGUI;
	String ContainerTopImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getContainerTopImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void ContainerTopImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setContainerTopImage(PropertyHelper::stringToImage(value));
    }

	String ContainerBottomImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getContainerBottomImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void ContainerBottomImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setContainerBottomImage(PropertyHelper::stringToImage(value));
    }

	String ContainerMiddleImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getContainerMiddleImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void ContainerMiddleImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setContainerMiddleImage(PropertyHelper::stringToImage(value));
    }

	String UpNormalImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getUpNormalImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void UpNormalImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setUpNormalImage(PropertyHelper::stringToImage(value));
    }

	String UpHoverImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getUpHoverImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void UpHoverImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setUpHoverImage(PropertyHelper::stringToImage(value));
    }

	String UpPushedImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getUpPushedImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void UpPushedImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setUpPushedImage(PropertyHelper::stringToImage(value));
    }

	String DownNormalImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getDownNormalImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void DownNormalImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setDownNormalImage(PropertyHelper::stringToImage(value));
    }

	String DownHoverImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getDownHoverImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void DownHoverImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setDownHoverImage(PropertyHelper::stringToImage(value));
    }

	String DownPushedImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getDownPushedImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void DownPushedImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setDownPushedImage(PropertyHelper::stringToImage(value));
    }

	String ThumbNormalImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getThumbNormalImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void ThumbNormalImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setThumbNormalImage(PropertyHelper::stringToImage(value));
    }

	String ThumbHoverImageName::get(const PropertyReceiver* receiver) const
    {
        const Image* img = static_cast<const TLVertScrollbar*>(receiver)->getThumbHoverImage();
        return img ? PropertyHelper::imageToString(img) : String("");
    }
	
    void ThumbHoverImageName::set(PropertyReceiver* receiver, const String &value)
    {
        static_cast<TLVertScrollbar*>(receiver)->setThumbHoverImage(PropertyHelper::stringToImage(value));
    }
}
// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
const utf8	TLVertScrollbar::WidgetTypeName[]	= "TaharezLook/LargeVerticalScrollbar";

// Progress bar image names
const utf8	TLVertScrollbar::ImagesetName[]					= "TaharezLook";
const utf8	TLVertScrollbar::ContainerTopImageName[]		= "VertScrollTop";
const utf8	TLVertScrollbar::ContainerMiddleImageName[]		= "VertScrollMiddle";
const utf8	TLVertScrollbar::ContainerBottomImageName[]		= "VertScrollBottom";

// some layout stuff
const float	TLVertScrollbar::ThumbWidth			= 0.4f;
const float	TLVertScrollbar::ThumbPositionX		= 0.325f;
const float	TLVertScrollbar::TrackWidthRatio	= 0.2f;
const float	TLVertScrollbar::TrackOffsetXRatio	= 0.45f;
const float	TLVertScrollbar::ButtonWidth		= 0.6f;
const float	TLVertScrollbar::ButtonPositionX	= 0.25f;
const float	TLVertScrollbar::ButtonOffsetYRatio	= 0.5f;

// type names for the component widgets
const utf8*	TLVertScrollbar::ThumbWidgetType			= TLVertScrollbarThumb::WidgetTypeName;
const utf8*	TLVertScrollbar::IncreaseButtonWidgetType	= TLButton::WidgetTypeName;
const utf8*	TLVertScrollbar::DecreaseButtonWidgetType	= TLButton::WidgetTypeName;


TLVertScrollbarProperties::ContainerTopImageName	TLVertScrollbar::d_topImageName;
TLVertScrollbarProperties::ContainerMiddleImageName	TLVertScrollbar::d_middleImageName;
TLVertScrollbarProperties::ContainerBottomImageName	TLVertScrollbar::d_bottomImageName;
TLVertScrollbarProperties::UpNormalImageName		TLVertScrollbar::d_upNormalImageName;
TLVertScrollbarProperties::UpHoverImageName			TLVertScrollbar::d_upHoverImageName;
TLVertScrollbarProperties::UpPushedImageName		TLVertScrollbar::d_upPushedImageName;
TLVertScrollbarProperties::DownNormalImageName		TLVertScrollbar::d_downNormalImageName;
TLVertScrollbarProperties::DownHoverImageName		TLVertScrollbar::d_downHoverImageName;
TLVertScrollbarProperties::DownPushedImageName		TLVertScrollbar::d_downPushedImageName;
TLVertScrollbarProperties::ThumbNormalImageName		TLVertScrollbar::d_thumbNormalImageName;
TLVertScrollbarProperties::ThumbHoverImageName		TLVertScrollbar::d_thumbHoverImageName;
/*************************************************************************
	Constructor for Taharez vertical scroll bar widgets
*************************************************************************/
TLVertScrollbar::TLVertScrollbar(const String& type, const String& name) :
	Scrollbar(type, name)
{
	Imageset* ims = ImagesetManager::getSingleton().getImageset(ImagesetName);
	d_containerTop = &ims->getImage(ContainerTopImageName);
	d_containerBottom = &ims->getImage(ContainerBottomImageName);
	d_containerMiddle = &ims->getImage(ContainerMiddleImageName);

	d_needRelayout = false;
	addTLVertScrollbarProperties();
}


/*************************************************************************
	Destructor for Taharez vertical scroll bar widgets
*************************************************************************/
TLVertScrollbar::~TLVertScrollbar(void)
{
}

const Image* TLVertScrollbar::getContainerTopImage(void) const
{
    return d_containerTop;
}

void TLVertScrollbar::setContainerTopImage(const Image* newImage)
{
	d_containerTop = newImage;
}

const Image* TLVertScrollbar::getContainerBottomImage(void) const
{
    return d_containerBottom;
}

void TLVertScrollbar::setContainerBottomImage(const Image* newImage)
{
	d_containerBottom = newImage;
}

const Image* TLVertScrollbar::getContainerMiddleImage(void) const
{
    return d_containerMiddle;
}

void TLVertScrollbar::setContainerMiddleImage(const Image* newImage)
{
	d_containerMiddle = newImage;
}

const Image* TLVertScrollbar::getUpNormalImage(void) const
{
    return d_decrease->getNormalImage();
}

void TLVertScrollbar::setUpNormalImage(const Image* newImage)
{
	d_needRelayout = true;
	d_decrease->setNormalImage(newImage);
	d_decrease->setWidth(Absolute, newImage->getWidth());
	d_decrease->setHeight(Absolute, newImage->getHeight());
}

const Image* TLVertScrollbar::getUpHoverImage(void) const
{
    return d_decrease->getHoverImage();
}

void TLVertScrollbar::setUpHoverImage(const Image* newImage)
{
	d_decrease->setHoverImage(newImage);
}

const Image* TLVertScrollbar::getUpPushedImage(void) const
{
    return d_decrease->getPushedImage();
}

void TLVertScrollbar::setUpPushedImage(const Image* newImage)
{
	d_decrease->setPushedImage(newImage);
}

const Image* TLVertScrollbar::getDownNormalImage(void) const
{
    return d_increase->getNormalImage();
}

void TLVertScrollbar::setDownNormalImage(const Image* newImage)
{
	d_needRelayout = true;
	d_increase->setNormalImage(newImage);
	d_increase->setWidth(Absolute, newImage->getWidth());
	d_increase->setHeight(Absolute, newImage->getHeight());
}

const Image* TLVertScrollbar::getDownHoverImage(void) const
{
    return d_increase->getHoverImage();
}

void TLVertScrollbar::setDownHoverImage(const Image* newImage)
{
	d_increase->setHoverImage(newImage);
}

const Image* TLVertScrollbar::getDownPushedImage(void) const
{
    return d_increase->getPushedImage();
}

void TLVertScrollbar::setDownPushedImage(const Image* newImage)
{
	d_increase->setPushedImage(newImage);
}

const Image* TLVertScrollbar::getThumbNormalImage(void) const
{
    return d_thumb->getNormalImage();
}

void TLVertScrollbar::setThumbNormalImage(const Image* newImage)
{
	d_thumbWidth = newImage->getWidth();
	d_thumbHeight = newImage->getHeight();
	d_thumb->setHeight(Absolute, newImage->getHeight());
	d_thumb->setWidth(Absolute, newImage->getWidth() );

	d_thumb->setNormalImage(newImage);

 	float xPos = (getWidth(Absolute) - d_thumb->getWidth(Absolute)) / 2;
 	d_thumb->setXPosition(Absolute, xPos);
}

const Image* TLVertScrollbar::getThumbHoverImage(void) const
{
    return d_thumb->getHoverImage();
}

void TLVertScrollbar::setThumbHoverImage(const Image* newImage)
{
	d_thumb->setHoverImage(newImage);
}

void TLVertScrollbar::addTLVertScrollbarProperties()
{
	addProperty(&d_topImageName);
	addProperty(&d_middleImageName);
	addProperty(&d_bottomImageName);
	addProperty(&d_upNormalImageName);
	addProperty(&d_upHoverImageName);
	addProperty(&d_upPushedImageName);
	addProperty(&d_downNormalImageName);
	addProperty(&d_downHoverImageName);
	addProperty(&d_downPushedImageName);
	addProperty(&d_thumbNormalImageName);
	addProperty(&d_thumbHoverImageName);
}
/*************************************************************************
	create a PushButton based widget to use as the increase button for
	this scroll bar.
*************************************************************************/
PushButton* TLVertScrollbar::createIncreaseButton(const String& name) const
{
	// create the widget
	TLButton* btn = (TLButton*)WindowManager::getSingleton().createWindow(IncreaseButtonWidgetType, name);

	// perform some initialisation
	return btn;
}


/*************************************************************************
	create a PushButton based widget to use as the decrease button for
	this scroll bar.
*************************************************************************/
PushButton* TLVertScrollbar::createDecreaseButton(const String& name) const
{
	// create the widget
	TLButton* btn = (TLButton*)WindowManager::getSingleton().createWindow(DecreaseButtonWidgetType, name);

	return btn;
}


/*************************************************************************
	create a Thumb based widget to use as the thumb for this scroll bar.
*************************************************************************/
Thumb* TLVertScrollbar::createThumb(const String& name) const
{
	// create the widget
	TLVertScrollbarThumb* thumb = (TLVertScrollbarThumb*)WindowManager::getSingleton().createWindow(ThumbWidgetType, name);

	// perform some initialisation
	thumb->setVertFree(true);
	thumb->setXPosition(ThumbPositionX);
	thumb->setWidth(ThumbWidth);

	return thumb;
}


/*************************************************************************
	layout the scroll bar component widgets
*************************************************************************/
void TLVertScrollbar::performChildWindowLayout()
{
    Scrollbar::performChildWindowLayout();

//	d_thumb->setXPosition(ThumbPositionX);
	
 	float xPos = (getWidth(Absolute) - d_thumb->getWidth(Absolute)) / 2;
 	d_thumb->setXPosition(Absolute, xPos);

	float x = (getAbsoluteWidth() - d_decrease->getAbsoluteWidth()) / 2;
	float y = 0;
	d_decrease->setPosition(Absolute, Point(x, y));

	x = (getAbsoluteWidth() - d_increase->getAbsoluteWidth()) / 2;
	y = getAbsoluteHeight() - d_increase->getAbsoluteHeight();
	d_increase->setPosition(Absolute, Point(x, y));

	updateThumb();
}


/*************************************************************************
	update the size and location of the thumb to properly represent the
	current state of the scroll bar
*************************************************************************/
void TLVertScrollbar::updateThumb(void)
{
	// calculate actual padding values to use.
	//float slideTrackYPadding = d_decrease->getAbsoluteHeight() + (d_containerTop->getHeight() * 0.5f);
	float slideTrackTopPadding = d_containerTop ? d_containerTop->getHeight() : 0;
	float slideTrackBottomPadding = d_containerBottom ? d_containerBottom->getHeight() : 0;

	// calculate maximum extents for thumb positioning.
	float middlePadding = max(0.0f, getAbsoluteHeight() - slideTrackTopPadding - slideTrackBottomPadding);
	float slideExtent	= max(0.0f, getAbsoluteHeight() - slideTrackTopPadding - slideTrackBottomPadding - d_thumb->getAbsoluteHeight());

	// Thumb does not change size with document length, we just need to update position and range
	d_thumb->setVertRange(absoluteToRelativeY(slideTrackTopPadding), absoluteToRelativeY(slideTrackTopPadding + slideExtent));
	d_thumb->setYPosition(absoluteToRelativeY(slideTrackTopPadding + d_position * slideExtent));

	requestRedraw();
}


/*************************************************************************
	return value that best represents current scroll bar position given
	the current location of the thumb.
*************************************************************************/
float TLVertScrollbar::getValueFromThumb(void) const
{
	// calculate actual padding values to use.
	float slideTrackTopPadding = d_containerTop->getHeight();
	float slideTrackBottomPadding = d_containerBottom->getHeight();

	// calculate maximum extents for thumb positioning.
	float slideExtent	= getAbsoluteHeight() - (slideTrackTopPadding + slideTrackBottomPadding) - d_thumb->getAbsoluteHeight();

	return	(d_thumb->getAbsoluteYPosition() - slideTrackTopPadding) / slideExtent;
}


/*************************************************************************
	Given window location \a pt, return a value indicating what change
	should be made to the scroll bar.
*************************************************************************/
float TLVertScrollbar::getAdjustDirectionFromPoint(const Point& pt) const
{
	Rect absrect(d_thumb->getUnclippedPixelRect());

	float slideTrackTopPadding = d_containerTop->getHeight();
	float slideTrackBottomPadding = d_containerBottom->getHeight();
	
	// calculate maximum extents for thumb positioning.
	float slideExtent	= getAbsoluteHeight() - (slideTrackTopPadding + slideTrackBottomPadding) - d_thumb->getAbsoluteHeight();
	
	if (pt.d_y < absrect.d_top)
	{
		return -(absrect.d_top - pt.d_y) / slideExtent;
	}
	else if (pt.d_y > absrect.d_bottom)
	{
		return (pt.d_y - absrect.d_bottom) / slideExtent;
	}
	else
	{
		return 0.0f;
	}

}


/*************************************************************************
	Perform rendering for this widget
*************************************************************************/
void TLVertScrollbar::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{
	if(d_needRelayout)
	{
		d_thumb->setSize( Absolute, Size(d_thumbWidth, d_thumbHeight));
		performChildWindowLayout();
		d_needRelayout = false;
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
	Point pos = absrect.getPosition();

	float mid_height = absrect.getHeight() - d_containerTop->getHeight() - d_containerBottom->getHeight();

	//
	// Render scroll bar container
	//
	clipper.setPosition(absrect.getPosition());
	clipper.setHeight(d_containerTop->getHeight());
	panelCache->cacheImage(d_containerTop, pos, clipper);

	pos.d_y += d_containerTop->getHeight();
	clipper.setPosition(pos);
	clipper.setHeight(mid_height);
	panelCache->cacheImage(d_containerMiddle, pos, clipper);

	pos.d_y += mid_height;
	clipper.setPosition(absrect.getPosition());
	clipper.setHeight(d_containerBottom->getHeight());
	panelCache->cacheImage(d_containerBottom, pos, clipper);

	return;//xiehong 2007-3-21
// 	//
// 	// render slide-track
// 	//
// 	float slideTrackYPadding = d_decrease->getAbsoluteHeight() + PixelAligned(d_containerTop->getHeight() * 0.5f);
// 
// 	// calculate a new clipper for the slide track area
// 	absrect.d_top		+= slideTrackYPadding;
// 	absrect.d_bottom	-= slideTrackYPadding;
// 	clipper = absrect.getIntersection(clipper);
// 
// 	pos.d_x += absrect.getWidth() * TrackOffsetXRatio;
// 	pos.d_y = absrect.d_top;
// 	pos.d_z = System::getSingleton().getRenderer()->getZLayer(1);
// 
// 	sz.d_height	= d_thumbTrack->getHeight();
// 	sz.d_width	= absrect.getWidth() * TrackWidthRatio;
// 
// 	int segments = (int)((absrect.getHeight() / d_thumbTrack->getHeight()) + 0.99f);
// 
// 	for (int i = 0; i < segments; ++i)
// 	{
// 		d_thumbTrack->draw(pos, sz, clipper, colours);
// 		pos.d_y += sz.d_height;
// 	}

}

void TLVertScrollbar::drawSelf()
{
	
	if(d_needRelayout)
	{
		d_thumb->setSize( Absolute, Size(d_thumbWidth, d_thumbHeight));
		performChildWindowLayout();		
	}

	Rect clipper(getPixelRect());

	// do nothing if the widget is totally clipped.
	if (clipper.getWidth() == 0)
	{
		return;
	}

	// get the destination screen rect for this window
	Rect absrect(getUnclippedPixelRect());

	// calculate colours to use.
	ColourRect colours(colour(1, 1, 1, getEffectiveAlpha()));

	float mid_height = absrect.getHeight() - d_containerTop->getHeight() - d_containerBottom->getHeight();

	//
	// Render scroll bar container
	//
	Vector3 pos(absrect.d_left, absrect.d_top, 0);
	Size	sz(absrect.getWidth(), d_containerTop->getHeight());
	if ( d_containerTop )
		d_containerTop->draw(pos, sz, clipper, colours);

	pos.d_y += sz.d_height;
	sz.d_height = mid_height;
	if ( d_containerMiddle )
		d_containerMiddle->draw(pos, sz, clipper, colours);

	pos.d_y += sz.d_height;
	sz.d_height = d_containerBottom->getHeight();
	if ( d_containerBottom )
		d_containerBottom->draw(pos, sz, clipper, colours);

	return;//xiehong 2007-3-21
	//
	// render slide-track
	//
	float slideTrackYPadding = d_decrease->getAbsoluteHeight() + PixelAligned(d_containerTop->getHeight() * 0.5f);

	// calculate a new clipper for the slide track area
	absrect.d_top		+= slideTrackYPadding;
	absrect.d_bottom	-= slideTrackYPadding;
	clipper = absrect.getIntersection(clipper);

	pos.d_x += absrect.getWidth() * TrackOffsetXRatio;
	pos.d_y = absrect.d_top;
	pos.d_z = System::getSingleton().getRenderer()->getZLayer(1);

	sz.d_height	= d_thumbTrack->getHeight();
	sz.d_width	= absrect.getWidth() * TrackWidthRatio;

	int segments = (int)((absrect.getHeight() / d_thumbTrack->getHeight()) + 0.99f);

	for (int i = 0; i < segments; ++i)
	{
		d_thumbTrack->draw(pos, sz, clipper, colours);
		pos.d_y += sz.d_height;
	}

}


/*************************************************************************
	Handler for when WM_CHAR arrive
*************************************************************************/
void TLVertScrollbar::onCharacter(KeyEventArgs& e)
{
	e.handled = true;
}


//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Factory Methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Create, initialise and return a TLVertScrollbar
*************************************************************************/
Window* TLVertScrollbarFactory::createWindow(const String& name)
{
    return new TLVertScrollbar(d_type, name);
}

} // End of  CEGUI namespace section
