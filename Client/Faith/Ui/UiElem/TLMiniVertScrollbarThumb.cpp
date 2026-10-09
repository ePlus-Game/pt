/************************************************************************
	filename: 	TLMiniVertScrollbarThumb.cpp
	created:	2/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of thumb for Taharez mini vertical
				scroll bar.
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
#include "TLMiniVertScrollbarThumb.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const utf8	TLMiniVertScrollbarThumb::WidgetTypeName[]	= "TaharezLook/VerticalScrollbarThumb";

// Image names
const utf8	TLMiniVertScrollbarThumb::ImagesetName[]				= "TaharezLook";
const utf8	TLMiniVertScrollbarThumb::NormalImageName[]				= "MiniVertScrollThumbNormal";
const utf8	TLMiniVertScrollbarThumb::NormalTopImageName[]			= "MiniVertScrollThumbTopNormal";
const utf8	TLMiniVertScrollbarThumb::NormalMiddleImageName[]		= "MiniVertScrollThumbMiddleNormal";
const utf8	TLMiniVertScrollbarThumb::NormalBottomImageName[]		= "MiniVertScrollThumbBottomNormal";
const utf8	TLMiniVertScrollbarThumb::HighlightTopImageName[]		= "MiniVertScrollThumbTopHover";
const utf8	TLMiniVertScrollbarThumb::HighlightMiddleImageName[]	= "MiniVertScrollThumbMiddleHover";
const utf8	TLMiniVertScrollbarThumb::HighlightBottomImageName[]	= "MiniVertScrollThumbBottomHover";


/*************************************************************************
	Constructor
*************************************************************************/
TLMiniVertScrollbarThumb::TLMiniVertScrollbarThumb(const String& type, const String& name) :
	Thumb(type, name)
{
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);

	d_normalImage			= &ImagesetManager::getSingleton().getImageset("anniu2")->getImage("anniu2_huadongtiao1_normal");
	d_normalTopImage		= &ImagesetManager::getSingleton().getImageset("anniu2")->getImage("anniu2_huadongtiao1_normal");
	d_normalMiddleImage		= &ImagesetManager::getSingleton().getImageset("anniu2")->getImage("anniu2_huadongtiao1_normal");
	d_normalBottomImage		= &ImagesetManager::getSingleton().getImageset("anniu2")->getImage("anniu2_huadongtiao1_normal");
	d_highlightTopImage		= &ImagesetManager::getSingleton().getImageset("anniu2")->getImage("anniu2_huadongtiao1_hover");
	d_highlightMiddleImage	= &ImagesetManager::getSingleton().getImageset("anniu2")->getImage("anniu2_huadongtiao1_hover");
	d_highlightBottomImage	= &ImagesetManager::getSingleton().getImageset("anniu2")->getImage("anniu2_huadongtiao1_hover");
}


/*************************************************************************
	Destructor
*************************************************************************/
TLMiniVertScrollbarThumb::~TLMiniVertScrollbarThumb(void)
{
}


/*************************************************************************
	render the thumb in the normal state.
*************************************************************************/
void TLMiniVertScrollbarThumb::drawNormal(KRenderCache* panelCache, Point* panelAbsPos)
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

	// calculate segment sizes
	float minHeight		= PixelAligned(absrect.getHeight() * 0.5f);
	float topHeight		= min(d_normalTopImage->getHeight(), minHeight);
	float bottomHeight	= min(d_normalBottomImage->getHeight(), minHeight);
	float middleHeight	= absrect.getHeight() - topHeight - bottomHeight;


	// draw the images
	panelCache->cacheImage(d_normalTopImage, absrect.getPosition(), clipper);

	/*absrect.d_top += topHeight;
	clipper.d_top = absrect.d_top;
	clipper.setHeight(middleHeight);
	panelCache->cacheImage(d_normalMiddleImage, absrect.getPosition(), clipper);

	absrect.d_top += middleHeight;
	clipper.d_top = absrect.d_top;
	clipper.setHeight(bottomHeight);
	panelCache->cacheImage(d_normalBottomImage, absrect.getPosition(), clipper);//*/
}


/*************************************************************************
	render the thumb in the hover / highlighted state.
*************************************************************************/
void TLMiniVertScrollbarThumb::drawHover(KRenderCache* panelCache, Point* panelAbsPos)
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

	// calculate segment sizes
	float minHeight		= PixelAligned(absrect.getHeight() * 0.5f);
	float topHeight		= min(d_highlightTopImage->getHeight(), minHeight);
	float bottomHeight	= min(d_highlightBottomImage->getHeight(), minHeight);
	float middleHeight	= absrect.getHeight() - topHeight - bottomHeight;


	// draw the images
	panelCache->cacheImage(d_highlightTopImage, absrect.getPosition(), clipper);

	/*absrect.d_top += topHeight;
	clipper.d_top = absrect.d_top;
	clipper.setHeight(middleHeight);
	panelCache->cacheImage(d_highlightMiddleImage, absrect.getPosition(), clipper);

	absrect.d_top += middleHeight;
	clipper.d_top = absrect.d_top;
	clipper.setHeight(bottomHeight);
	panelCache->cacheImage(d_highlightBottomImage, absrect.getPosition(), clipper);//*/
}


/*************************************************************************
	render the thumb in the disabled state
*************************************************************************/
void TLMiniVertScrollbarThumb::drawDisabled(KRenderCache* panelCache, Point* panelAbsPos)
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

	// calculate segment sizes
	float minHeight		= PixelAligned(absrect.getHeight() * 0.5f);
	float topHeight		= min(d_highlightTopImage->getHeight(), minHeight);
	float bottomHeight	= min(d_highlightBottomImage->getHeight(), minHeight);
	float middleHeight	= absrect.getHeight() - topHeight - bottomHeight;


	// draw the images
	panelCache->cacheImage(d_highlightTopImage, absrect.getPosition(), clipper);

	/*absrect.d_top += topHeight;
	clipper.d_top = absrect.d_top;
	clipper.setHeight(middleHeight);
	panelCache->cacheImage(d_highlightMiddleImage, absrect.getPosition(), clipper);

	absrect.d_top += middleHeight;
	clipper.d_top = absrect.d_top;
	clipper.setHeight(bottomHeight);
	panelCache->cacheImage(d_highlightBottomImage, absrect.getPosition(), clipper);//*/
}


/*************************************************************************
	Handler for when size changes
*************************************************************************/
void TLMiniVertScrollbarThumb::onSized(WindowEventArgs& e)
{
	// calculate preferred height from width (which is known).
	float prefHeight = d_normalImage->getHeight() * (getAbsoluteWidth() / d_normalImage->getWidth());

	Window* par = getParent();

	// Only proceed if parent is not NULL.
	if (par != NULL)
	{
		// calculate scaled height.
		float scaledHeight = (par->getAbsoluteHeight() - (2 * par->getAbsoluteWidth())) * 0.575f;

		// use preferred height if there is room, else use the scaled height.
		if (scaledHeight < prefHeight)
		{
			prefHeight = scaledHeight;
		}
	}

	// install new height values.
    UVector2 sze(d_area.getSize());
    sze.d_y = cegui_absdim(prefHeight);
    setWindowArea_impl(d_area.getPosition(), sze, false, false);

	// base class processing.
	Thumb::onSized(e);

	e.handled = true;
}

/*************************************************************************
	Handler for when WM_CHAR arrive
*************************************************************************/
void TLMiniVertScrollbarThumb::onCharacter(KeyEventArgs& e)
{
	e.handled = true;
}




//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Factory Methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Create, initialise and return a TLMiniVertScrollbarThumb
*************************************************************************/
Window* TLMiniVertScrollbarThumbFactory::createWindow(const String& name)
{
	return new TLMiniVertScrollbarThumb(d_type, name);
}

} // End of  CEGUI namespace section
