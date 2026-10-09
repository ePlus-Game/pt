/************************************************************************
	filename: 	TLButton.cpp
	created:	11/5/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Taharez look Button widget.
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
#include "TLButton.h"
#include "CEGUIImageset.h"
#include "CEGUIFont.h"
#include "TLStatic.h"
#ifndef _LAYOUT_EDITOR_
#include "Ui/UiCase/UiItemTip.h"
#endif // _LAYOUT_EDITOR_
#include "CoreUseNameDef.h"

namespace TLButtonProperties
{
	using namespace CEGUI;
	

}
// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const utf8	TLButton::WidgetTypeName[]				= "TaharezLook/Button";

const utf8	TLButton::ImagesetName[]				= "TaharezLook";
const utf8	TLButton::LeftNormalImageName[]			= "ButtonLeftNormal";
const utf8	TLButton::MiddleNormalImageName[]		= "ButtonMiddleNormal";
const utf8	TLButton::RightNormalImageName[]		= "ButtonRightNormal";
const utf8	TLButton::LeftHighlightImageName[]		= "ButtonLeftHighlight";
const utf8	TLButton::MiddleHighlightImageName[]	= "ButtonMiddleHighlight";
const utf8	TLButton::RightHighlightImageName[]		= "ButtonRightHighlight";
const utf8	TLButton::LeftPushedImageName[]			= "ButtonLeftPushed";
const utf8	TLButton::MiddlePushedImageName[]		= "ButtonMiddlePushed";
const utf8	TLButton::RightPushedImageName[]		= "ButtonRightPushed";
const utf8  TLButton::MouseCursorImageName[]		= "MouseArrow";

const int TLButtonYOffSet = 1;

/*************************************************************************
	Constructor
*************************************************************************/
TLButton::TLButton(const String& type, const String& name) :
	PushButton(type, name)
{
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);

	// setup cache of image pointers
	d_leftSectionNormal		= &iset->getImage(LeftNormalImageName);
	d_middleSectionNormal	= &iset->getImage(MiddleNormalImageName);
	d_rightSectionNormal	= &iset->getImage(RightNormalImageName);

	d_leftSectionHover		= &iset->getImage(LeftHighlightImageName);
	d_middleSectionHover	= &iset->getImage(MiddleHighlightImageName);
	d_rightSectionHover		= &iset->getImage(RightHighlightImageName);

	d_leftSectionPushed		= &iset->getImage(LeftPushedImageName);
	d_middleSectionPushed	= &iset->getImage(MiddlePushedImageName);
	d_rightSectionPushed	= &iset->getImage(RightPushedImageName);

	setMouseCursor(&iset->getImage(MouseCursorImageName));

	d_activeImage = (TLStaticImage*)WindowManager::getSingleton().createWindow(TLStaticImage::WidgetTypeName, d_name + "/__auto_activeImage" );
	if ( d_activeImage )
	{
		addChildWindow( d_activeImage );
		//d_activeImage->setPosition(Absolute, Point(0,0));
		//d_activeImage->setSize(Absolute, Size(0, 0));
		d_activeImage->disable();
		d_activeImage->setDummyWnd( true );
		d_activeImage->setBackgroundEnabled( false );
		d_activeImage->setFrameEnabled(false);
 		d_activeImage->setCyc( false );	
 		d_activeImage->setImage( "activebutton", UI_FULL_IMAGESET );
		d_activeImage->setSize(Absolute, Size(d_activeImage->getImage()->getWidth(), d_activeImage->getImage()->getHeight()));
		d_activeImage->stop();
		d_activeImage->hide();
	}
	d_hoverYOff = 0;
	d_pushedYOff = 1;
}


/*************************************************************************
	Destructor
*************************************************************************/
TLButton::~TLButton(void)
{
}

void TLButton::activeImage(int count)
{
	if ( d_activeImage )
	{
		int w = (int)getWidth(Absolute)/2;
		int h = (int)getHeight(Absolute)/2;
		int sw = (int)d_activeImage->getImage()->getWidth()/2;
		int sh = (int)d_activeImage->getImage()->getHeight()/2;
		
		Point p( w - sw, h - sh );
		d_activeImage->setPosition(Absolute, p);

		d_activeImage->setCycCount(count);
		d_activeImage->setCyc(false);
		d_activeImage->play(true);
		requestRedraw();
	}
}

void TLButton::deactivateImage()
{
	if ( d_activeImage )
	{
		d_activeImage->stop();
	}
}

/*************************************************************************
	render Widget in normal state	
*************************************************************************/
void TLButton::drawNormal(KRenderCache* panelCache, Point* panelAbsPos)
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

	if(d_normalImage)
	{
		panelCache->cacheImage(d_normalImage, absrect.getPosition(), clipper);
	}

	//
	// Draw label text
	//
	absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());

	float textHeight = getFont()->getLineSpacing();

	switch(d_textVFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absrect.d_top += absrect.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absrect.d_top += (absrect.getHeight() - textHeight) / 2 + TLButtonYOffSet;
		}
		break;
	}

	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_normalColour);
}

void TLButton::drawHover(KRenderCache* panelCache, Point* panelAbsPos)
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

	if(d_hoverImage)
	{
		panelCache->cacheImage(d_hoverImage, absrect.getPosition(), clipper);
	}
	//
	// Draw label text
	//
	
	absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());

	float textHeight = getFont()->getLineSpacing();

	switch(d_textVFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absrect.d_top += absrect.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absrect.d_top += (absrect.getHeight() - textHeight) / 2 + TLButtonYOffSet;
		}
		break;
	}
	absrect.d_top += d_hoverYOff;

	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_hoverColour);
}

/*************************************************************************
	render Widget in Pushed state	
*************************************************************************/
void TLButton::drawPushed(KRenderCache* panelCache, Point* panelAbsPos)
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
	
	if(d_pushedImage)
	{
		panelCache->cacheImage(d_pushedImage, absrect.getPosition(), clipper);
	}
	//
	// Draw label text
	//
	
	absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());

	float textHeight = getFont()->getLineSpacing();

	switch(d_textVFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absrect.d_top += absrect.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absrect.d_top += (absrect.getHeight() - textHeight) / 2 + TLButtonYOffSet;
		}
		break;
	}
	absrect.d_top += d_pushedYOff;

	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_pushedColour);
}


/*************************************************************************
	render Widget in disabled state	
*************************************************************************/
void TLButton::drawDisabled(KRenderCache* panelCache, Point* panelAbsPos)
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

	if(d_disabledImage)
	{
		panelCache->cacheImage(d_disabledImage, absrect.getPosition(), clipper);
	}

	//
	// Draw label text
	//
	//
	// Draw label text
	//
	absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());

	float textHeight = getFont()->getLineSpacing();

	switch(d_textVFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absrect.d_top += absrect.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absrect.d_top += (absrect.getHeight() - textHeight) / 2 + TLButtonYOffSet;
		}
		break;
	}

	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_disabledColour);
}

void TLButton::drawNormal()
{
	Rect clipper(getPixelRect());

	if (clipper.getWidth() == 0)
	{
		return;
	}
	
	Rect absrect(getUnclippedPixelRect());
	
	Vector3 imgpos(absrect.d_left, absrect.d_top, 0);

	if(d_normalImage)
	{
		d_normalImage->draw(imgpos, clipper, getEffectiveAlpha());
	}

	absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());
	
	float alpha_comp = getEffectiveAlpha();
	ColourRect colours(colour(1, 1, 1, alpha_comp));
	colours.setColours(d_normalColour);
	colours.modulateAlpha(alpha_comp);

	float textHeight = getFont()->getLineSpacing();

	switch(d_textVFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absrect.d_top += absrect.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absrect.d_top += (absrect.getHeight() - textHeight) / 2 + TLButtonYOffSet;
		}
		break;
	}

	const_cast<Font *>(getFont())->drawText(getText(), absrect, 0, clipper, d_textHFormatting, colours);
}

void TLButton::drawHover()
{
	Rect clipper(getPixelRect());

	if (clipper.getWidth() == 0)
	{
		return;
	}
	
	Rect absrect(getUnclippedPixelRect());
	
	Vector3 imgpos(absrect.d_left, absrect.d_top, 0);
	
	if(d_hoverImage)
	{
		d_hoverImage->draw(imgpos, clipper, getEffectiveAlpha());
	}

	//absrect.d_top += PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
	absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());
	
	float alpha_comp = getEffectiveAlpha();
	ColourRect colours(colour(1, 1, 1, alpha_comp));
	colours.setColours(d_hoverColour);
	colours.modulateAlpha(alpha_comp);

	float textHeight = getFont()->getLineSpacing();

	switch(d_textVFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absrect.d_top += absrect.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absrect.d_top += (absrect.getHeight() - textHeight) / 2 + TLButtonYOffSet;
		}
		break;
	}
	absrect.d_top += d_hoverYOff;
	const_cast<Font *>(getFont())->drawText(getText(), absrect, 0, clipper, d_textHFormatting, colours);
}


/*************************************************************************
	render Widget in Pushed state	
*************************************************************************/
void TLButton::drawPushed()
{
	Rect clipper(getPixelRect());

	if (clipper.getWidth() == 0)
	{
		return;
	}
	
	Rect absrect(getUnclippedPixelRect());
	
	Vector3 imgpos(absrect.d_left, absrect.d_top, 0);

	if(d_pushedImage)
	{
		d_pushedImage->draw(imgpos, clipper, getEffectiveAlpha());
	}

	//absrect.d_top += PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
	
	float alpha_comp = getEffectiveAlpha();
	ColourRect colours(colour(1, 1, 1, alpha_comp));
	colours.setColours(d_pushedColour);
	colours.modulateAlpha(alpha_comp);
	
	absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());

	float textHeight = getFont()->getLineSpacing();

	switch(d_textVFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absrect.d_top += absrect.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absrect.d_top += (absrect.getHeight() - textHeight) / 2 + TLButtonYOffSet;
		}
		break;
	}
	absrect.d_top += d_pushedYOff;

	const_cast<Font *>(getFont())->drawText(getText(), absrect, 0, clipper, d_textHFormatting, colours);
}


/*************************************************************************
	render Widget in disabled state	
*************************************************************************/
void TLButton::drawDisabled()
{
	Rect clipper(getPixelRect());

	if (clipper.getWidth() == 0)
	{
		return;
	}
	
	Rect absrect(getUnclippedPixelRect());
	
	Vector3 imgpos(absrect.d_left, absrect.d_top, 0);

	if(d_disabledImage)
	{
		d_disabledImage->draw(imgpos, clipper, getEffectiveAlpha());
	}

	absrect.d_top += PixelAligned((absrect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
	
	float alpha_comp = getEffectiveAlpha();
	ColourRect colours(colour(1, 1, 1, alpha_comp));
	colours.setColours(d_disabledColour);
	colours.modulateAlpha(alpha_comp);
	
	absrect.d_left += PixelAligned(d_textXOffset * absrect.getWidth());

	float textHeight = getFont()->getLineSpacing();

	switch(d_textVFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absrect.d_top += absrect.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absrect.d_top += (absrect.getHeight() - textHeight) / 2 + TLButtonYOffSet;
		}
		break;
	}

	const_cast<Font *>(getFont())->drawText(getText(), absrect, 0, clipper, d_textHFormatting, colours);
}
/*************************************************************************
	Handler for when WM_CHAR arrive
*************************************************************************/
void TLButton::onCharacter(KeyEventArgs& e)
{
	e.handled = true;
}

void TLButton::onShown(WindowEventArgs& e)
{
	Window::onShown(e);
	/*if ( d_activeImage )
	{
		d_activeImage->setSize(Absolute, getSize(Absolute));
	}//*/
}

void TLButton::onMouseEnters(MouseEventArgs& e)
{
	PushButton::onMouseEnters(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip& tip = KUiItemTip::GetSingleton();
		tip.show(Utf8ToAnsi( d_tooltipText ), getUnclippedInnerRect(), KUiItemTip::BottomLeft);
#endif // _LAYOUT_EDITOR_
	}
}

void TLButton::onMouseLeaves(MouseEventArgs& e)
{
	PushButton::onMouseLeaves(e);
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
	Create, initialise and return a TLButton
*************************************************************************/
Window* TLButtonFactory::createWindow(const String& name)
{
	return new TLButton(d_type, name);
}

} // End of  CEGUI namespace section
