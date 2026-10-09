/************************************************************************
	filename: 	TLRadioButton.cpp
	created:	21/5/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Taharez look Radio Button widget
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
#include "TLRadioButton.h"
#include "CEGUIImageset.h"
#include "CEGUIFont.h"
#include "CEGUIPropertyHelper.h"
#ifndef _LAYOUT_EDITOR_
#include "Ui/UiCase/UiItemTip.h"
#endif //_LAYOUT_EDITOR_

namespace TLRadioButtonProperties
{
	using namespace CEGUI;
	
	String TLRadioButtonSelectedTextColour::get(const PropertyReceiver* receiver) const
	{
		return PropertyHelper::colourToString(static_cast<const TLRadioButton*>(receiver)->getSelectedTextColour());
	}


	void TLRadioButtonSelectedTextColour::set(PropertyReceiver* receiver, const String& value)
	{
		static_cast<TLRadioButton*>(receiver)->setSelectedTextColour(PropertyHelper::stringToColour(value));
	}
}

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const utf8	TLRadioButton::WidgetTypeName[]	= "TaharezLook/RadioButton";

const utf8	TLRadioButton::ImagesetName[]			= "TaharezLook";
const utf8	TLRadioButton::NormalImageName[]		= "RadioButtonNormal";
const utf8	TLRadioButton::HighlightImageName[]		= "RadioButtonHover";
const utf8	TLRadioButton::SelectMarkImageName[]	= "RadioButtonMark";
const colour TLRadioButton::DefaultSelectedLabelColour	= 0xFFFFFFFF;

const float	TLRadioButton::LabelPadding				= 0.0f;
const int	TextYOffset = 1;

TLRadioButtonProperties::TLRadioButtonSelectedTextColour	TLRadioButton::d_selectedTextColour;
/*************************************************************************
	Constructor for Taharez Look Radio Button objects.
*************************************************************************/
TLRadioButton::TLRadioButton(const String& type, const String& name) :
	RadioButton(type, name)
{
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);

	// setup cache of image pointers
	d_normalSectionImage		= &iset->getImage(NormalImageName);
	d_hoverSectionImage			= &iset->getImage(HighlightImageName);
	d_selectMarkSectionImage	= &iset->getImage(SelectMarkImageName);
	d_selectedColour			= DefaultSelectedLabelColour;

	d_textHFormatting			= Centred;
	d_hoverYOff = 0;
	d_pushedYOff = 0;
	addTLRadioButtonProperties();
}


/*************************************************************************
	Destructor for TLRadioButton objects.
*************************************************************************/
TLRadioButton::~TLRadioButton(void)
{
}

const colour TLRadioButton::getSelectedTextColour(void) const
{
	return d_selectedColour;
}

void TLRadioButton::setSelectedTextColour(const colour& colour)
{
	if (d_selectedColour != colour)
	{
		d_selectedColour = colour;
		requestRedraw();
	}
}

void TLRadioButton::addTLRadioButtonProperties()
{
	addProperty( &d_selectedTextColour );
}

void TLRadioButton::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{
	if ( d_selected )
	{
		drawNormal(panelCache, panelAbsPos);
	}
	else if (isHovering())
	{
		drawHover(panelCache, panelAbsPos);
	}
	else if (isPushed())
	{
		drawPushed(panelCache, panelAbsPos);
	}
	else if (isDisabled())
	{
		drawDisabled(panelCache, panelAbsPos);
	}
	else
	{
		drawNormal(panelCache, panelAbsPos);
	}
}

/*************************************************************************
	render the radio button in the normal state.	
*************************************************************************/
void TLRadioButton::drawNormal(KRenderCache* panelCache, Point* panelAbsPos)
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

	if (d_selected)
	{
		panelCache->cacheImage(d_pushedImage.getImage(), absrect.getPosition(), clipper);
	}
	else
	{
		panelCache->cacheImage(d_normalImage.getImage(), absrect.getPosition(), clipper);
	}

	//
	// Draw label text
	//
	/*float textHeight = getFont()->getLineSpacing();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}
	absrect.d_top  += d_yOffset;//*/

	absrect.d_top  += TextYOffset;
	if ( !d_selected )
		panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_normalColour);
	else
		panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_selectedColour);
}


/*************************************************************************
	render the radio button in the hover / highlighted state.
*************************************************************************/
void TLRadioButton::drawHover(KRenderCache* panelCache, Point* panelAbsPos)
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
	// draw the images
	//
	if (d_selected)
	{
		panelCache->cacheImage(d_pushedImage.getImage(), absrect.getPosition(), clipper);
	}
	else
	{
		panelCache->cacheImage(d_hoverImage.getImage(), absrect.getPosition(), clipper);
	}

	//
	// Draw label text
	//
	/*float textHeight = getFont()->getLineSpacing();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}//*/
	absrect.d_top += d_hoverYOff;
	absrect.d_top  += TextYOffset;
	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_hoverColour);
}


/*************************************************************************
	render the radio button in the pushed state.	
*************************************************************************/
void TLRadioButton::drawPushed(KRenderCache* panelCache, Point* panelAbsPos)
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


	panelCache->cacheImage(d_pushedImage.getImage(), absrect.getPosition(), clipper);
	
	//
	// Draw label text
	//
	/*float textHeight = getFont()->getLineSpacing();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}//*/
	absrect.d_top += d_pushedYOff;
	absrect.d_top  += TextYOffset;
	
	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_pushedColour);
}


/*************************************************************************
	render the radio button in the disabled state	
*************************************************************************/
void TLRadioButton::drawDisabled(KRenderCache* panelCache, Point* panelAbsPos)
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

	panelCache->cacheImage(d_disabledImage.getImage(), absrect.getPosition(), clipper);
	
	//
	// Draw label text
	//
	/*float textHeight = getFont()->getLineSpacing();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}//*/
	absrect.d_top  += d_yOffset;
	absrect.d_top  += TextYOffset;
	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_disabledColour);
}

void TLRadioButton::drawNormal()
{
	Rect clipper(getPixelRect());

	// do nothing if the widget is totally clipped.
	if (clipper.getWidth() == 0)
	{
		return;
	}

	// get the destination screen rect for this window
	Rect absrect(getUnclippedPixelRect());

	// calculate colours to use.
	float alpha_comp = getEffectiveAlpha();
	ColourRect colours(colour(1, 1, 1, alpha_comp));

	//
	// draw the images
	//
	if ( d_useStandardImagery )
	{
		Vector3 pos(absrect.d_left, absrect.d_top + PixelAligned((absrect.getHeight() - d_normalSectionImage->getHeight()) * 0.5f), 0);
		d_normalSectionImage->draw(pos, clipper, colours);

		if (d_selected)
		{
			d_selectMarkSectionImage->draw(pos, clipper, colours);
		}
	}
	else
	{
		if (d_selected)
		{
			colours = d_normalImage.getColours();
			colours.setAlpha(alpha_comp);
			d_pushedImage.setColours(colours);
			Vector3 imgpos(absrect.d_left, absrect.d_top, System::getSingleton().getRenderer()->getZLayer(1));
			d_pushedImage.draw(imgpos, clipper, getEffectiveAlpha());
		}
		else
		{
			colours = d_hoverImage.getColours();
			colours.setAlpha(alpha_comp);
			d_normalImage.setColours(colours);
			Vector3 imgpos(absrect.d_left, absrect.d_top, System::getSingleton().getRenderer()->getZLayer(1));
			d_normalImage.draw(imgpos, clipper, getEffectiveAlpha());
		}
	}
	//
	// Draw label text
	//
	/*float textHeight = getFont()->getLineSpacing();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}//*/
	absrect.d_top  += d_yOffset;
	absrect.d_top  += TextYOffset;
	colours.setColours(d_normalColour);
	colours.modulateAlpha(alpha_comp);
	const_cast<Font *>(getFont())->drawText(getText(), absrect, System::getSingleton().getRenderer()->getZLayer(1), clipper, d_textHFormatting, colours);
}


/*************************************************************************
	render the radio button in the hover / highlighted state.
*************************************************************************/
void TLRadioButton::drawHover()
{
	Rect clipper(getPixelRect());

	// do nothing if the widget is totally clipped.
	if (clipper.getWidth() == 0)
	{
		return;
	}

	// get the destination screen rect for this window
	Rect absrect(getUnclippedPixelRect());

	// calculate colours to use.
	float alpha_comp = getEffectiveAlpha();
	ColourRect colours(colour(1, 1, 1, alpha_comp));

	//
	// draw the images
	//
	if ( d_useStandardImagery )
	{
		Vector3 pos(absrect.d_left, absrect.d_top + PixelAligned((absrect.getHeight() - d_hoverSectionImage->getHeight()) * 0.5f), 0);
		d_hoverSectionImage->draw(pos, clipper, colours);

		if (d_selected)
		{
			d_selectMarkSectionImage->draw(pos, clipper, colours);
		}
	}
	else
	{
		if (d_selected)
		{
			colours = d_normalImage.getColours();
			colours.setAlpha(alpha_comp);
			d_pushedImage.setColours(colours);
			Vector3 imgpos(absrect.d_left, absrect.d_top, System::getSingleton().getRenderer()->getZLayer(1));
			d_pushedImage.draw(imgpos, clipper, getEffectiveAlpha());
		}
		else
		{
			colours = d_hoverImage.getColours();
			colours.setAlpha(alpha_comp);
			d_hoverImage.setColours(colours);
			Vector3 imgpos(absrect.d_left, absrect.d_top, System::getSingleton().getRenderer()->getZLayer(1));
			d_hoverImage.draw(imgpos, clipper, getEffectiveAlpha());
		}
	}
	//
	// Draw label text
	//
	/*float textHeight = getFont()->getLineSpacing();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}//*/
	absrect.d_top += d_hoverYOff;
	absrect.d_top  += TextYOffset;
	colours.setColours(d_hoverColour);
	colours.modulateAlpha(alpha_comp);
	const_cast<Font *>(getFont())->drawText(getText(), absrect, System::getSingleton().getRenderer()->getZLayer(1), clipper, d_textHFormatting, colours);
}


/*************************************************************************
	render the radio button in the pushed state.	
*************************************************************************/
void TLRadioButton::drawPushed()
{
	Rect clipper(getPixelRect());

	// do nothing if the widget is totally clipped.
	if (clipper.getWidth() == 0)
	{
		return;
	}

	// get the destination screen rect for this window
	Rect absrect(getUnclippedPixelRect());

	// calculate colours to use.
	float alpha_comp = getEffectiveAlpha();
	ColourRect colours(colour(1, 1, 1, alpha_comp));

	//
	// draw the images
	//
	if ( d_useStandardImagery )
	{
		Vector3 pos(absrect.d_left, absrect.d_top + PixelAligned((absrect.getHeight() - d_normalSectionImage->getHeight()) * 0.5f), 0);
		d_normalSectionImage->draw(pos, clipper, colours);

		if (d_selected)
		{
			d_selectMarkSectionImage->draw(pos, clipper, colours);
		}
	}
	else
	{
		if (d_selected)
		{
			colours = d_normalImage.getColours();
			colours.setAlpha(alpha_comp);
			d_pushedImage.setColours(colours);
			Vector3 imgpos(absrect.d_left, absrect.d_top, System::getSingleton().getRenderer()->getZLayer(1));
			d_pushedImage.draw(imgpos, clipper, getEffectiveAlpha());
		}
		else
		{
			colours = d_hoverImage.getColours();
			colours.setAlpha(alpha_comp);
			d_pushedImage.setColours(colours);
			Vector3 imgpos(absrect.d_left, absrect.d_top, System::getSingleton().getRenderer()->getZLayer(1));
			d_pushedImage.draw(imgpos, clipper, getEffectiveAlpha());
		}
	}
	//
	// Draw label text
	//
	/*float textHeight = getFont()->getLineSpacing();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}//*/
	absrect.d_top += d_pushedYOff;
	absrect.d_top  += TextYOffset;
	colours.setColours(d_pushedColour);
	colours.modulateAlpha(alpha_comp);
	const_cast<Font *>(getFont())->drawText(getText(), absrect, System::getSingleton().getRenderer()->getZLayer(1), clipper, d_textHFormatting, colours);
}


/*************************************************************************
	render the radio button in the disabled state	
*************************************************************************/
void TLRadioButton::drawDisabled()
{
	Rect clipper(getPixelRect());

	// do nothing if the widget is totally clipped.
	if (clipper.getWidth() == 0)
	{
		return;
	}

	// get the destination screen rect for this window
	Rect absrect(getUnclippedPixelRect());

	// calculate colours to use.
	float alpha_comp = getEffectiveAlpha();
	ColourRect colours(colour(1, 1, 1, alpha_comp));

	//
	// draw the images
	//
	if ( d_useStandardImagery )
	{
		Vector3 pos(absrect.d_left, absrect.d_top + PixelAligned((absrect.getHeight() - d_normalSectionImage->getHeight()) * 0.5f), 0);
		d_normalSectionImage->draw(pos, clipper, colours);

		if (d_selected)
		{
			d_selectMarkSectionImage->draw(pos, clipper, colours);
		}
	}
	else
	{
		colours = d_hoverImage.getColours();
		colours.setAlpha(alpha_comp);
		d_pushedImage.setColours(colours);
		Vector3 imgpos(absrect.d_left, absrect.d_top, System::getSingleton().getRenderer()->getZLayer(1));
		d_disabledImage.draw(imgpos, clipper, getEffectiveAlpha());
	}
	//
	// Draw label text
	//
	/*float textHeight = getFont()->getLineSpacing();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}//*/
	absrect.d_top  += d_yOffset;
	absrect.d_top  += TextYOffset;
	colours.setColours(d_disabledColour);
	colours.modulateAlpha(alpha_comp);
	const_cast<Font *>(getFont())->drawText(getText(), absrect, System::getSingleton().getRenderer()->getZLayer(1), clipper, d_textHFormatting, colours);
}
/*************************************************************************
	Handler for when WM_CHAR arrive
*************************************************************************/
void TLRadioButton::onCharacter(KeyEventArgs& e)
{
	e.handled = true;
}

void TLRadioButton::onMouseEnters(MouseEventArgs& e)
{
	RadioButton::onMouseEnters(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip& tip = KUiItemTip::GetSingleton();
		tip.show(Utf8ToAnsi( d_tooltipText ), getUnclippedInnerRect(), KUiItemTip::Bottom);
#endif //#ifndef _LAYOUT_EDITOR_
	}
}

void TLRadioButton::onMouseLeaves(MouseEventArgs& e)
{
	RadioButton::onMouseLeaves(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip::Hide();
#endif //#ifndef _LAYOUT_EDITOR_
	}
}

//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Factory Methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Create, initialise and return a TLRadioButton
*************************************************************************/
Window* TLRadioButtonFactory::createWindow(const String& name)
{
	return new TLRadioButton(d_type, name);
}

} // End of  CEGUI namespace section
