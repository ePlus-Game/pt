/************************************************************************
	filename: 	CEGUIStatic.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Static widget base class
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
#include "elements/CEGUIStatic.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "elements/CEGUITooltip.h"


// Start of CEGUI namespace section
namespace CEGUI
{
const String Static::EventNamespace("Static");

/*************************************************************************
	Definitions of Properties for this class
*************************************************************************/
StaticProperties::FrameEnabled				Static::d_frameEnabledProperty;
StaticProperties::BackgroundEnabled			Static::d_backgroundEnabledProperty;
StaticProperties::BackgroundImage			Static::d_backgroundImageProperty;
StaticProperties::TopLeftFrameImage			Static::d_topLeftFrameProperty;
StaticProperties::TopRightFrameImage		Static::d_topRightFrameProperty;
StaticProperties::BottomLeftFrameImage		Static::d_bottomLeftFrameProperty;
StaticProperties::BottomRightFrameImage		Static::d_bottomRightFrameProperty;
StaticProperties::LeftFrameImage			Static::d_leftFrameProperty;
StaticProperties::RightFrameImage			Static::d_rightFrameProperty;
StaticProperties::TopFrameImage				Static::d_topFrameProperty;
StaticProperties::BottomFrameImage			Static::d_bottomFrameProperty;
StaticProperties::FadeInTime				Static::d_fadeInTimeProperty;
StaticProperties::FadeOutTime				Static::d_fadeOutTimeProperty;
StaticProperties::AutoCloseTime				Static::d_autoCloseTimeProperty;
StaticProperties::DummyWnd					Static::d_dummyWndProperty;


/*************************************************************************
	Constructor for static widget base class
*************************************************************************/
Static::Static(const String& type, const String& name) :
	Window(type, name),
	d_frameEnabled(false),
	d_backgroundEnabled(false),
	d_background(NULL),
	d_left_width(0.0f),
	d_right_width(0.0f),
	d_top_height(0.0f),
	d_bottom_height(0.0f),
	d_origAlpha(d_alpha),
	d_fadeOutTime(0.0f),
	d_fadeInTime(0.0f),
	d_fading(false),
	d_fadingOut(false),
	d_isOpen(false),
	d_autoCloseTime(0.0f),
	d_needClose(false),
	d_handleMsg(true)
{
	d_topleft = NULL;
	d_topright = NULL;
	d_bottomleft = NULL;
	d_bottomright = NULL;
	d_left = NULL;
	d_top = NULL;
	d_right = NULL;
	d_bottom = NULL;
	addStaticProperties();
	addEvent(EventFadingInEnd);
}


/*************************************************************************
	Destructor for static widget base class.
*************************************************************************/
Static::~Static(void)
{
}


/*************************************************************************
	overridden so derived classes are auto-clipped to within the inner
	area of the frame when it's active.
*************************************************************************/
Rect Static::getUnclippedInnerRect(void) const
{
	// if frame is enabled, return rect for area inside frame
	if (d_frameEnabled)
	{
		Rect tmp(Window::getUnclippedInnerRect());
		tmp.d_left		+= d_left_width;
		tmp.d_right		-= d_right_width;
		tmp.d_top		+= d_top_height;
		tmp.d_bottom	-= d_bottom_height;
		return tmp;
	}
	// no frame, so return default inner rect.
	else
	{
		return Window::getUnclippedInnerRect();
	}

}


/*************************************************************************
	Enable or disable rendering of the frame for this static widget.
*************************************************************************/
void Static::setFrameEnabled(bool setting)
{
	if (d_frameEnabled != setting)
	{
		d_frameEnabled = setting;
		WindowEventArgs args(this);
		onStaticFrameChanged(args);
		requestRedraw();
	}
}

/*************************************************************************
	specify the Image objects to use for each part of the frame.
	A NULL may be used to omit any part.	
*************************************************************************/
void Static::setFrameImages(const Image* topleft, const Image* topright, const Image* bottomleft, const Image* bottomright, const Image* left, const Image* top, const Image* right, const Image* bottom)
{	
	d_topleft		= topleft;
	d_topright		= topright;
	d_bottomleft	= bottomleft;
	d_bottomright	= bottomright;
	d_left			= left;
	d_right			= right;
	d_top			= top;
	d_bottom		= bottom;

	// get sizes of frame edges
	d_left_width	= (left != NULL) ? left->getWidth() : 0.0f;
	d_right_width	= (right != NULL) ? right->getWidth() : 0.0f;
	d_top_height	= (top != NULL) ? top->getHeight() : 0.0f;
	d_bottom_height	= (bottom != NULL) ? bottom->getHeight() : 0.0f;

	// redraw only if change would be seen.
	if (d_frameEnabled)
	{
		WindowEventArgs args(this);
		onStaticFrameChanged(args);
		requestRedraw();
	}
}



/*************************************************************************
	Enable or disable rendering of the background for this static widget.	
*************************************************************************/
void Static::setBackgroundEnabled(bool setting)
{
	if (d_backgroundEnabled != setting)
	{
		d_backgroundEnabled = setting;
		requestRedraw();
	}

}


/*************************************************************************
	Set the image to use as the background for the static widget.
*************************************************************************/
void Static::setBackgroundImage(const Image* image)
{
	d_background = image;

	if (d_backgroundEnabled)
	{
		requestRedraw();
	}

}


/*************************************************************************
	Set the image to use as the background for the static widget.	
*************************************************************************/
void Static::setBackgroundImage(const String& imageset, const String& image)
{
	setBackgroundImage(&ImagesetManager::getSingleton().getImageset(imageset)->getImage(image));
}


/*************************************************************************
	given an ARGB colour value and a alpha float value return the colour
	value with the alpha component modulated by the given alpha float.
*************************************************************************/
colour Static::calculateModulatedAlphaColour(const colour& col, float alpha) const
{
	colour temp(col);
	temp.setAlpha(temp.getAlpha() * alpha);
	return temp;
}

void Static::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{
	Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);

	//计算出相对于panel的位置
	Rect backgroundRect(getUnclippedPixelRect());
	backgroundRect.offset(offPos);
	
	Rect ctrlClipper(getPixelRect());
	ctrlClipper.offset(offPos);

	if(d_frameEnabled)
	{
		Rect clipper;
		
		//上
		if(d_top != NULL)
		{
			clipper.d_left	= backgroundRect.d_left + d_topleft->getWidth();
			clipper.d_top	= backgroundRect.d_top;
			clipper.setWidth(backgroundRect.getWidth() - d_topleft->getWidth() - d_topright->getWidth());
			clipper.setHeight(d_top->getHeight());
			panelCache->cacheImage(d_top, clipper.getPosition(), clipper.getIntersection(ctrlClipper));
		}
		
		//左
		if(d_left != NULL)
		{
			clipper.d_left	= backgroundRect.d_left;
			clipper.d_top	= backgroundRect.d_top + d_topleft->getHeight();
			clipper.setWidth(d_left->getWidth());
			clipper.setHeight(backgroundRect.getHeight() - d_topleft->getHeight() - d_bottomleft->getHeight());
			panelCache->cacheImage(d_left, clipper.getPosition(), clipper.getIntersection(ctrlClipper));
		}
		
		//右
		if(d_right != NULL)
		{
			clipper.d_left	= backgroundRect.d_right - d_right->getWidth();
			clipper.d_top	= backgroundRect.d_top + d_topright->getHeight();
			clipper.setWidth(d_right->getWidth());
			clipper.setHeight(backgroundRect.getHeight() - d_topright->getHeight() - d_bottomright->getHeight());
			panelCache->cacheImage(d_right, clipper.getPosition(), clipper.getIntersection(ctrlClipper));
		}

		//下
		if(d_bottom != NULL)
		{
			clipper.d_left	= backgroundRect.d_left + d_bottomleft->getWidth();
			clipper.d_top	= backgroundRect.d_bottom - d_bottom->getHeight();
			clipper.setWidth(backgroundRect.getWidth() - d_bottomleft->getWidth() - d_bottomright->getWidth());
			clipper.setHeight(d_bottom->getHeight());
			panelCache->cacheImage(d_bottom, clipper.getPosition(), clipper.getIntersection(ctrlClipper));
		}

        backgroundRect.d_left		+= d_left_width;
        backgroundRect.d_right		-= d_right_width;
        backgroundRect.d_top		+= d_top_height;
        backgroundRect.d_bottom		-= d_bottom_height;
	}

	// draw backdrop
	if (d_backgroundEnabled && d_background != NULL)
	{
        // cache image for drawing
		panelCache->cacheImage(d_background, backgroundRect.getPosition(), backgroundRect);
	}

	if(d_frameEnabled)
	{
		Rect clipper;
		Rect backgroundRect(getUnclippedPixelRect());
		backgroundRect.offset(offPos);

		//左上角
		if(d_topleft != NULL)
		{
			clipper.d_left	= backgroundRect.d_left;
			clipper.d_top	= backgroundRect.d_top;
			clipper.setWidth(d_topleft->getWidth());
			clipper.setHeight(d_topleft->getHeight());
			panelCache->cacheImage(d_topleft, clipper.getPosition(), clipper.getIntersection(ctrlClipper));
		}
		//右上角
		if(d_topright != NULL)
		{
			clipper.d_left	= backgroundRect.d_right - d_topright->getWidth();
			clipper.d_top	= backgroundRect.d_top;
			clipper.setWidth(d_topright->getWidth());
			clipper.setHeight(d_topright->getHeight());
			panelCache->cacheImage(d_topright, clipper.getPosition(), clipper.getIntersection(ctrlClipper));
		}
		//左下
		if(d_bottomleft != NULL)
		{
			clipper.d_left	= backgroundRect.d_left;
			clipper.d_top	= backgroundRect.d_bottom - d_bottomleft->getHeight();
			clipper.setWidth(d_bottomleft->getWidth());
			clipper.setHeight(d_bottomleft->getHeight());
			panelCache->cacheImage(d_bottomleft, clipper.getPosition(), clipper.getIntersection(ctrlClipper));
		}
		//右下
		if(d_bottomright != NULL)
		{
			clipper.d_left	= backgroundRect.d_right - d_bottomright->getWidth();
			clipper.d_top	= backgroundRect.d_bottom - d_bottomright->getHeight();
			clipper.setWidth(d_bottomright->getWidth());
			clipper.setHeight(d_bottomright->getHeight());
			panelCache->cacheImage(d_bottomright, clipper.getPosition(), clipper.getIntersection(ctrlClipper));
		}
	}
}

void Static::populateRenderCache()
{
	Rect backgroundRect(Point(0,0), getAbsoluteSize());

   	if(d_frameEnabled)
	{
		Rect clipper;

		//上
		if(d_top != NULL)
		{
			clipper.d_left	= backgroundRect.d_left + d_topleft->getWidth();
			clipper.d_top	= backgroundRect.d_top;
			clipper.setWidth(backgroundRect.getWidth() - d_topleft->getWidth() - d_topright->getWidth());
			clipper.setHeight(d_top->getHeight());
			d_renderCache.cacheImage(*d_top, clipper, 0, colour(1, 1, 1, d_alpha));
		}

		//左
		if(d_left != NULL)
		{
			clipper.d_left	= backgroundRect.d_left;
			clipper.d_top	= backgroundRect.d_top + d_topleft->getHeight();
			clipper.setWidth(d_left->getWidth());
			clipper.setHeight(backgroundRect.getHeight() - d_topleft->getHeight() - d_bottomleft->getHeight());
			d_renderCache.cacheImage(*d_left, clipper, 0, colour(1, 1, 1, d_alpha));
		}
		
		//右
		if(d_right != NULL)
		{
			clipper.d_left	= backgroundRect.d_right - d_right->getWidth();
			clipper.d_top	= backgroundRect.d_top + d_topright->getHeight();
			clipper.setWidth(d_right->getWidth());
			clipper.setHeight(backgroundRect.getHeight() - d_topright->getHeight() - d_bottomright->getHeight());
			d_renderCache.cacheImage(*d_right, clipper, 0, colour(1, 1, 1, d_alpha));
		}
		

		//下
		if(d_bottom != NULL)
		{
			clipper.d_left	= backgroundRect.d_left + d_bottomleft->getWidth();
			clipper.d_top	= backgroundRect.d_bottom - d_bottom->getHeight();
			clipper.setWidth(backgroundRect.getWidth() - d_bottomleft->getWidth() - d_bottomright->getWidth());
			clipper.setHeight(d_bottom->getHeight());
			d_renderCache.cacheImage(*d_bottom, clipper, 0, colour(1, 1, 1, d_alpha));
		}
		
        backgroundRect.d_left		+= d_left_width;
        backgroundRect.d_right		-= d_right_width;
        backgroundRect.d_top		+= d_top_height;
        backgroundRect.d_bottom		-= d_bottom_height;
	}

	// draw backdrop
	if (d_backgroundEnabled && d_background != NULL)
	{
        // cache image for drawing
		d_renderCache.cacheImage(*d_background, backgroundRect, 0, colour(1, 1, 1, d_alpha));
	}

	if(d_frameEnabled)
	{
		Rect clipper;
		Rect backgroundRect(Point(0,0), getAbsoluteSize());
		//左上角
		if(d_topleft != NULL)
		{
			clipper.d_left	= backgroundRect.d_left;
			clipper.d_top	= backgroundRect.d_top;
			clipper.setWidth(d_topleft->getWidth());
			clipper.setHeight(d_topleft->getHeight());
			d_renderCache.cacheImage(*d_topleft, clipper, 0, colour(1, 1, 1, d_alpha));
		}

		//右上角
		if(d_topright != NULL)
		{
			clipper.d_left	= backgroundRect.d_right - d_topright->getWidth();
			clipper.d_top	= backgroundRect.d_top;
			clipper.setWidth(d_topright->getWidth());
			clipper.setHeight(d_topright->getHeight());
			d_renderCache.cacheImage(*d_topright, clipper, 0, colour(1, 1, 1, d_alpha));
		}

		//左下
		if(d_bottomleft != NULL)
		{
			clipper.d_left	= backgroundRect.d_left;
			clipper.d_top	= backgroundRect.d_bottom - d_bottomleft->getHeight();
			clipper.setWidth(d_bottomleft->getWidth());
			clipper.setHeight(d_bottomleft->getHeight());
			d_renderCache.cacheImage(*d_bottomleft, clipper, 0, colour(1, 1, 1, d_alpha));
		}

		//右下
		if(d_bottomright != NULL)
		{
			clipper.d_left	= backgroundRect.d_right - d_bottomright->getWidth();
			clipper.d_top	= backgroundRect.d_bottom - d_bottomright->getHeight();
			clipper.setWidth(d_bottomright->getWidth());
			clipper.setHeight(d_bottomright->getHeight());
			d_renderCache.cacheImage(*d_bottomright, clipper, 0, colour(1, 1, 1, d_alpha));
		}
	}
}

void Static::onMouseButtonDown(MouseEventArgs& e)
{
	e.handled = d_handleMsg;
	Window::onMouseButtonDown(e);
}


void Static::onMouseButtonUp(MouseEventArgs& e)
{
	e.handled = d_handleMsg;
	Window::onMouseButtonUp(e);
}

void Static::onMouseMove(MouseEventArgs& e)
{
	e.handled = d_handleMsg;
	Window::onMouseMove(e);
}

void Static::onMouseHover(MouseEventArgs& e)
{
	if(d_handleMsg)
	{
		e.handled = true;
	}
	fireEvent(EventMouseHover, e);
}

void Static::onMouseClicked(MouseEventArgs& e)
{
	e.handled = d_handleMsg;
	fireEvent(EventMouseClick, e);
}


void Static::onMouseDoubleClicked(MouseEventArgs& e)
{
	e.handled = d_handleMsg;
	fireEvent(EventMouseDoubleClick, e);
}


void Static::onMouseTripleClicked(MouseEventArgs& e)
{
	e.handled = d_handleMsg;
	fireEvent(EventMouseTripleClick, e);
}


/*************************************************************************
	Return the Image being used for the specified location of the frame.	
*************************************************************************/
const Image* Static::getImageForFrameLocation(FrameLocation location) const
{
	const Image* image = NULL;
	switch(location)
	{
	case TopLeftCorner:
		{
			image = d_topleft;
		}
		break;
	case TopRightCorner:
		{
			image = d_topright;			
		}
		break;
	case BottomLeftCorner:
		{
			image = d_bottomleft;			
		}
		break;
	case BottomRightCorner:
		{
			image = d_bottomright;			
		}
		break;
	case LeftEdge:
		{
			image = d_left;			
		}
		break;
	case TopEdge:
		{
			image = d_top;			
		}
		break;
	case RightEdge:
		{
			image = d_right;
		}
		break;
	case BottomEdge:
		{
			image = d_bottom;			
		}
		break;
	default:
		break;
	}
	return image;
}


/*************************************************************************
	Return the Image currently set as the background image for the widget.
*************************************************************************/
const Image* Static::getBackgroundImage(void) const
{
	return d_background;
}


/*************************************************************************
	Set the Image to use for the specified location of the frame.	
*************************************************************************/
void Static::setImageForFrameLocation(FrameLocation location, const Image* image)
{

	// update our record of image size
	switch (location)
	{
	case LeftEdge:
		d_left = image;
		d_left_width = (image != NULL) ? image->getWidth() : 0;
		break;

	case RightEdge:
		d_right = image;
		d_right_width = (image != NULL) ? image->getWidth() : 0;
		break;

	case TopEdge:
		d_top = image;
		d_top_height = (image != NULL) ? image->getHeight() : 0;
		break;

	case BottomEdge:
		d_bottom = image;
		d_bottom_height = (image != NULL) ? image->getHeight() : 0;
		break;
	case TopLeftCorner:
		d_topleft = image;
		break;
	case TopRightCorner:
		d_topright = image;
		break;
	case BottomLeftCorner:
		d_bottomleft = image;
		break;
	case BottomRightCorner:
		d_bottomright = image;
		break;
	default:
		break;
	}

}

/*************************************************************************
	Adds properties for the static widget base class
*************************************************************************/
void Static::addStaticProperties(void)
{
	addProperty(&d_frameEnabledProperty);
	addProperty(&d_backgroundEnabledProperty);
	addProperty(&d_backgroundImageProperty);
	addProperty(&d_topLeftFrameProperty);
	addProperty(&d_topRightFrameProperty);
	addProperty(&d_bottomLeftFrameProperty);
	addProperty(&d_bottomRightFrameProperty);
	addProperty(&d_leftFrameProperty);
	addProperty(&d_topFrameProperty);
	addProperty(&d_rightFrameProperty);
	addProperty(&d_bottomFrameProperty);
	addProperty(&d_fadeInTimeProperty);
	addProperty(&d_fadeOutTimeProperty);
	addProperty(&d_autoCloseTimeProperty);
	addProperty(&d_dummyWndProperty);
}

void Static::OpenBox()
{
	if (d_fading && d_fadingOut)
	{
		if (d_fadeInTime>0.0f&&d_fadeOutTime>0.0f)
		{
			d_fadeElapsed = ((d_fadeOutTime-d_fadeElapsed)/d_fadeOutTime)*d_fadeInTime;
		}
		else
		{
			d_fadeElapsed = 0;
		}
		d_fadingOut=false;
	}
	else if (d_fadeInTime>0.0f)
	{
		d_fading = true;
		d_fadingOut=false;
		//setAlpha(0.0f);
		//d_fadeElapsed = 0;
		d_fadeElapsed = d_fadeInTime * d_alpha / d_origAlpha;
	}
	else
	{
		d_fading = false;
		setAlpha(d_origAlpha);
	}
	
	show();
//	moveToFront();
}


void Static::CloseBox()
{
	if (d_fading && !d_fadingOut)
	{
		// make sure the "fade back out" is smooth - if possible !
		if (d_fadeOutTime>0.0f&&d_fadeInTime>0.0f)
		{
			// jump to the point of the fade in that has the same alpha as right now - this keeps it smooth
			d_fadeElapsed = ((d_fadeInTime-d_fadeElapsed)/d_fadeInTime)*d_fadeOutTime;
		}
		else
		{
			// start the fade in from the beginning
			d_fadeElapsed = 0;
		}
		// change to fade out
		d_fadingOut=true;
	}
	// otherwise just start normal fade out!
	else if (d_fadeOutTime>0.0f)
	{
		d_fading = true;
		d_fadingOut = true;
		//setAlpha(d_origAlpha);
		//d_fadeElapsed = 0;
		d_fadeElapsed = d_fadeOutTime * (d_origAlpha - d_alpha) / d_origAlpha;
	}
	// should not fade!
	else
	{
		d_fading = false;
		hide();
	}
}

void Static::StopFiding()
{
	d_fading = false;
}

void Static::updateSelf(float elapsed)
{
	Window::updateSelf(elapsed);
	
	if (d_fading)
	{
		d_fadeElapsed+=elapsed;
		
		if (d_fadingOut)
		{
			if (d_fadeElapsed>=d_fadeOutTime)
			{
				d_fading=false;
				setAlpha(d_origAlpha); // set real alpha so users can show directly without having to restore it
				hide();
			}
			else
			{
				setAlpha(d_origAlpha*(d_fadeOutTime-d_fadeElapsed)/d_fadeOutTime);
			}
			
		}
		else
		{
			if (d_fadeElapsed>=d_fadeInTime)
			{
				d_fading=false;
				setAlpha(d_origAlpha);
				EventArgs arg;
				fireEvent(EventFadingInEnd, arg);
			}
			else
			{
				setAlpha(d_origAlpha*d_fadeElapsed/d_fadeInTime);
			}
		}
	}

	if(d_autoCloseTime > 0.0f)
	{
		d_needClose = true;
		d_autoCloseTime -= elapsed;
	}
	else if(d_needClose)
	{
		d_needClose = false;
		CloseBox();
	}
}


} // End of  CEGUI namespace section

