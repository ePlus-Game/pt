/************************************************************************
	filename: 	TLMiniVertScrollbar.cpp
	created:	2/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Taharez mini vertical scroll bar widget.
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
#include "TLMiniVertScrollbar.h"
#include "TLMiniVertScrollbarThumb.h"
#include "TLButton.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const utf8	TLMiniVertScrollbar::WidgetTypeName[]	= "TaharezLook/VerticalScrollbar";

// Progress bar image names
const utf8	TLMiniVertScrollbar::ImagesetName[]					= "anniu2";
const utf8	TLMiniVertScrollbar::ScrollbarBodyImageName[]		= "yj_tuodong_mianban";
const utf8	TLMiniVertScrollbar::UpButtonNormalImageName[]		= "anniu2_xiangshangla_normal";
const utf8	TLMiniVertScrollbar::UpButtonHighlightImageName[]	= "anniu2_xiangshangla_hover";
const utf8	TLMiniVertScrollbar::DownButtonNormalImageName[]	= "anniu2_xiangxiala_normal";
const utf8	TLMiniVertScrollbar::DownButtonHighlightImageName[]	= "anniu2_xiangxiala_hover";

// some layout stuff
const float	TLMiniVertScrollbar::ThumbPositionX	= 0.15f;
const float	TLMiniVertScrollbar::ThumbWidth		= 1.0f;				
const float	TLMiniVertScrollbar::BodyPositionX	= 0.3f;			
const float	TLMiniVertScrollbar::BodyWidth		= 0.4f;

// type names for the component widgets
const utf8*	TLMiniVertScrollbar::ThumbWidgetType			= TLMiniVertScrollbarThumb::WidgetTypeName;
const utf8*	TLMiniVertScrollbar::IncreaseButtonWidgetType	= TLButton::WidgetTypeName;
const utf8*	TLMiniVertScrollbar::DecreaseButtonWidgetType	= TLButton::WidgetTypeName;


/*************************************************************************
	Constructor for Taharez mini vertical scroll bar widgets
*************************************************************************/
TLMiniVertScrollbar::TLMiniVertScrollbar(const String& type, const String& name) :
	Scrollbar(type, name)
{
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ScrollbarBodyImageName);

	// setup cache of image pointers
	d_body = &iset->getImage("full_image");
}


/*************************************************************************
	Destructor for Taharez mini vertical scroll bar widgets
*************************************************************************/
TLMiniVertScrollbar::~TLMiniVertScrollbar(void)
{
}


/*************************************************************************
	create a PushButton based widget to use as the increase button for
	this scroll bar.
*************************************************************************/
PushButton* TLMiniVertScrollbar::createIncreaseButton(const String& name) const
{
	// create the widget
	TLButton* btn = (TLButton*)WindowManager::getSingleton().createWindow(IncreaseButtonWidgetType, name);

	// perform some initialisation
	btn->setZLevel(Window::Top);

	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);
	btn->setNormalImage(&iset->getImage(DownButtonNormalImageName));
	btn->setDisabledImage(&iset->getImage(DownButtonNormalImageName));

	btn->setHoverImage(&iset->getImage(DownButtonHighlightImageName));
	btn->setPushedImage(&iset->getImage(DownButtonHighlightImageName));

	return btn;
}


/*************************************************************************
	create a PushButton based widget to use as the decrease button for
	this scroll bar.
*************************************************************************/
PushButton* TLMiniVertScrollbar::createDecreaseButton(const String& name) const
{
	// create the widget
	TLButton* btn = (TLButton*)WindowManager::getSingleton().createWindow(DecreaseButtonWidgetType, name);

	// perform some initialisation
	btn->setZLevel(Window::Top);

	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);
	btn->setNormalImage(&iset->getImage(UpButtonNormalImageName));
	btn->setDisabledImage(&iset->getImage(UpButtonNormalImageName));

	btn->setHoverImage(&iset->getImage(UpButtonHighlightImageName));
	btn->setPushedImage(&iset->getImage(UpButtonHighlightImageName));

	return btn;
}


/*************************************************************************
	create a Thumb based widget to use as the thumb for this scroll bar.
*************************************************************************/
Thumb* TLMiniVertScrollbar::createThumb(const String& name) const
{
	// create the widget
	TLMiniVertScrollbarThumb* thumb = (TLMiniVertScrollbarThumb*)WindowManager::getSingleton().createWindow(ThumbWidgetType, name);

	// perform some initialisation
	thumb->setVertFree(true);
	thumb->setXPosition(ThumbPositionX);
	thumb->setWidth(ThumbWidth);

	return thumb;
}


/*************************************************************************
	layout the scroll bar component widgets
*************************************************************************/
void TLMiniVertScrollbar::performChildWindowLayout()
{
    Scrollbar::performChildWindowLayout();

	Size bsz;
	bsz.d_width = bsz.d_height = getAbsoluteWidth();

	// install button sizes
	d_increase->setSize(absoluteToRelative(bsz));
	d_decrease->setSize(absoluteToRelative(bsz));

	// position buttons
	d_decrease->setPosition(Point(0.0f, 0.0f));
	d_increase->setPosition(Point(0.0f, absoluteToRelativeY(getAbsoluteHeight() - bsz.d_height)));

	// this will configure thumb widget appropriately
	updateThumb();
}


/*************************************************************************
	update the size and location of the thumb to properly represent the
	current state of the scroll bar
*************************************************************************/
void TLMiniVertScrollbar::updateThumb(void)
{
	// calculate actual padding values to use.
	float slideTrackYPadding = d_decrease->getAbsoluteHeight();

	// calculate maximum extents for thumb positioning.
	float posExtent		= d_documentSize - d_pageSize;
	float slideExtent	= max(0.0f, getAbsoluteHeight() - (2 * slideTrackYPadding) - d_thumb->getAbsoluteHeight());

	// Thumb does not change size with document length, we just need to update position and range
	d_thumb->setVertRange(absoluteToRelativeY(slideTrackYPadding), absoluteToRelativeY(slideTrackYPadding + slideExtent));
	d_thumb->setYPosition(absoluteToRelativeY(slideTrackYPadding + (d_position * (slideExtent / posExtent))));
	
	requestRedraw();
}


/*************************************************************************
	return value that best represents current scroll bar position given
	the current location of the thumb.
*************************************************************************/
float TLMiniVertScrollbar::getValueFromThumb(void) const
{
	// calculate actual padding values to use.
	float slideTrackYPadding = d_decrease->getAbsoluteHeight();

	// calculate maximum extents for thumb positioning.
	float posExtent		= d_documentSize - d_pageSize;
	float slideExtent	= getAbsoluteHeight() - (2 * slideTrackYPadding) - d_thumb->getAbsoluteHeight();

	return	(d_thumb->getAbsoluteYPosition() - slideTrackYPadding) / (slideExtent / posExtent);
}


/*************************************************************************
	Given window location 'pt', return a value indicating what change
	should be made to the scroll bar.
*************************************************************************/
float TLMiniVertScrollbar::getAdjustDirectionFromPoint(const Point& pt) const
{
	Rect absrect(d_thumb->getUnclippedPixelRect());

	float slideTrackYPadding = d_decrease->getAbsoluteHeight();
	
	// calculate maximum extents for thumb positioning.
	float posExtent		= d_documentSize - d_pageSize;
	float slideExtent	= getAbsoluteHeight() - (2 * slideTrackYPadding) - d_thumb->getAbsoluteHeight();

	if (pt.d_y < absrect.d_top)
	{
		return -(absrect.d_top - pt.d_y) / (slideExtent / posExtent);
	}
	else if (pt.d_y > absrect.d_bottom)
	{
		return (pt.d_y - absrect.d_bottom) / (slideExtent / posExtent);
	}
	else
	{
		return 0.0f;
	}

}


/*************************************************************************
	Perform rendering for this widget
*************************************************************************/
void TLMiniVertScrollbar::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
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
	//
	// Render bar body
	//
	float button_height = d_decrease->getAbsoluteHeight();

	Point pos(absrect.d_left + PixelAligned(absrect.getWidth() * BodyPositionX), absrect.d_top + button_height);
	Size	sz(PixelAligned(absrect.getWidth() * BodyWidth), absrect.getHeight() - PixelAligned(button_height * 0.5f));

	//panelCache->cacheImage(d_body, pos, clipper);
	//d_body->draw(pos, pos, clipper, colours);
}

/*************************************************************************
	Handler for when WM_CHAR arrive
*************************************************************************/
void TLMiniVertScrollbar::onCharacter(KeyEventArgs& e)
{
	e.handled = true;
}



//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Factory Methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Create, initialise and return a TLMiniVertScrollbar
*************************************************************************/
Window* TLMiniVertScrollbarFactory::createWindow(const String& name)
{
	return new TLMiniVertScrollbar(d_type, name);
}

} // End of  CEGUI namespace section
