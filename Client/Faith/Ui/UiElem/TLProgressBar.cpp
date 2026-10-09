/************************************************************************
	filename: 	TLProgressBar.cpp
	created:	23/5/2004
	author:		Paul D Turner
	
	purpose:	Implementation of the Taharez Progress Bar
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
#include "TLProgressBar.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#ifndef _LAYOUT_EDITOR_
#include "Ui/UiCase/UiItemTip.h"
#endif // _LAYOUT_EDITOR_


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const utf8	TLProgressBar::WidgetTypeName[]	= "TaharezLook/ProgressBar";

// Progress bar image names
const utf8	TLProgressBar::ImagesetName[]				= "TaharezLook";
const utf8	TLProgressBar::ContainerBarImageName[]		= "RoleBlood";//"ProgressBar";
const utf8	TLProgressBar::SegmentImageName[]		= "RoleBlood";//"ProgressSegment";

// some offsets into imagery
const float	TLProgressBar::FirstSegmentOffsetRatioX		= 0.28571f;
const float	TLProgressBar::SegmentOverlapRatio			= 0.25f;


/*************************************************************************
	Constructor for Taharez progress bar objects
*************************************************************************/
TLProgressBar::TLProgressBar(const String& type, const String& name) :
	ProgressBar(type, name)
{
	d_bar = NULL;
	// cache images to be used
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);
	try
	{
		d_SegmentTop = 0;
		d_SegmentLeft = 0;
//		d_bar		= &iset->getImage(ContainerBarImageName);
//		d_Segment	= &iset->getImage(SegmentImageName);
	}
	catch (...)
	{
		
	}

}


/*************************************************************************
	Destructor for Taharez progress bar objects
*************************************************************************/
TLProgressBar::~TLProgressBar(void)
{
}


void	TLProgressBar::setProgressBar(const Image* image)
{
	d_bar = image;
	requestRedraw();
}

void	TLProgressBar::setProgressBarSegment(const Image* image)
{
	d_Segment = image;	
	requestRedraw();
}


/*************************************************************************
	Perform rendering for this widget
*************************************************************************/
void TLProgressBar::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{
	if ( d_bar )
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
		
		clipper.setWidth( clipper.getWidth() * d_progress );
		
		panelCache->cacheImage(d_bar, absrect.getPosition(), clipper);
	}
	

}

/*************************************************************************
	Handler for when WM_CHAR arrive
*************************************************************************/
void TLProgressBar::onCharacter(KeyEventArgs& e)
{
	e.handled = true;
}


void TLProgressBar::onMouseEnters(MouseEventArgs& e)
{
	ProgressBar::onMouseEnters(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
        KUiItemTip::GetSingleton();
		KUiItemTip::GetSingleton().show(Utf8ToAnsi( d_tooltipText ), getUnclippedInnerRect(), KUiItemTip::Top);
#endif // _LAYOUT_EDITOR_
	}
}

void TLProgressBar::onMouseLeaves(MouseEventArgs& e)
{
	ProgressBar::onMouseLeaves(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip::Hide();
#endif // _LAYOUT_EDITOR_
	}
}

//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Factory Methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Create, initialise and return a TLProgressBar
*************************************************************************/
Window* TLProgressBarFactory::createWindow(const String& name)
{
	return new TLProgressBar(d_type, name);
}

} // End of  CEGUI namespace section
