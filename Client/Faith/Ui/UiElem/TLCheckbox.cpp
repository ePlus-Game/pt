/************************************************************************
	filename: 	TLCheckbox.cpp
	created:	21/5/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Taharez Checkbox
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
#include "TLCheckbox.h"
#include "CEGUIImageset.h"
#include "CEGUIFont.h"
#include "CEGUIPropertyHelper.h"
#ifndef _LAYOUT_EDITOR_
#include "Ui/UiCase/UiItemTip.h"
#endif // _LAYOUT_EDITOR_

namespace TLCheckBoxProperties
{
	using namespace CEGUI;
	String TLCheckBoxNormal::get(const PropertyReceiver *receiver) const
	{
		const Image *tImage = static_cast<const TLCheckbox *>(receiver)->getCheckboxNormal();
		return tImage ? PropertyHelper::imageToString(tImage) : String("");
	}

	void TLCheckBoxNormal::set( PropertyReceiver *receiver, const String &value )
	{
		static_cast<TLCheckbox *>(receiver)->setCheckboxNormal(PropertyHelper::stringToImage(value));
	}

	String TLCheckBoxDisable::get(const PropertyReceiver *receiver) const
	{
		const Image *tImage = static_cast<const TLCheckbox *>(receiver)->getCheckboxDisable();
		return tImage ? PropertyHelper::imageToString(tImage) : String("");
	}
	
	void TLCheckBoxDisable::set( PropertyReceiver *receiver, const String &value )
	{
		static_cast<TLCheckbox *>(receiver)->setCheckboxDisable(PropertyHelper::stringToImage(value));
	}

	String TLCheckBoxHover::get(const PropertyReceiver *receiver) const
	{
		const Image *tImage = static_cast<const TLCheckbox *>(receiver)->getCheckboxHover();
		return tImage ? PropertyHelper::imageToString(tImage) : String("");
	}

	void TLCheckBoxHover::set(PropertyReceiver *receiver, const String &value )
	{
		static_cast<TLCheckbox *>(receiver)->setCheckboxHover(PropertyHelper::stringToImage(value));
	}

	String TLCheckBoxMark::get(const PropertyReceiver *receiver) const
	{
		const Image *tImage = static_cast<const TLCheckbox *>(receiver)->getCheckboxMark();
		return tImage ? PropertyHelper::imageToString(tImage) : String("");
	}

	void TLCheckBoxMark::set( PropertyReceiver *receiver, const String &value )
	{
		static_cast<TLCheckbox *>(receiver)->setCheckboxMark(PropertyHelper::stringToImage(value));
	}

		
	String TLCheckBoxSelectedTextColour::get(const PropertyReceiver* receiver) const
	{
		return PropertyHelper::colourToString(static_cast<const TLCheckbox*>(receiver)->getSelectedTextColour());
	}


	void TLCheckBoxSelectedTextColour::set(PropertyReceiver* receiver, const String& value)
	{
		static_cast<TLCheckbox*>(receiver)->setSelectedTextColour(PropertyHelper::stringToColour(value));
	}
}

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const utf8	TLCheckbox::WidgetTypeName[]		= "TaharezLook/Checkbox";

const utf8	TLCheckbox::ImagesetName[]			= "TaharezLook";
const utf8	TLCheckbox::NormalImageName[]		= "CheckboxNormal";
const utf8	TLCheckbox::HighlightImageName[]	= "CheckboxHover";
const utf8	TLCheckbox::CheckMarkImageName[]	= "CheckboxMark";
const utf8	TLCheckbox::DisableImageName[]		= "CheckboxNormal";

const colour TLCheckbox::DefaultSelectedLabelColour	= 0xFFFFFFFF;

const float	TLCheckbox::LabelPadding			= 4.0f;

TLCheckBoxProperties::TLCheckBoxNormal				TLCheckbox::d_chkNormalImage;
TLCheckBoxProperties::TLCheckBoxDisable				TLCheckbox::d_chkDisableImage;
TLCheckBoxProperties::TLCheckBoxHover				TLCheckbox::d_chkHoverImage;
TLCheckBoxProperties::TLCheckBoxMark				TLCheckbox::d_chkMarkImage;
TLCheckBoxProperties::TLCheckBoxSelectedTextColour	TLCheckbox::d_selectedTextColour;
/*************************************************************************
	Constructor for Taharez Look Checkbox objects.
*************************************************************************/
TLCheckbox::TLCheckbox(const String& type, const String& name) :
	Checkbox(type, name)
{
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);

	// setup cache of image pointers
	d_normalImage		= &iset->getImage(NormalImageName);
	d_hoverImage		= &iset->getImage(HighlightImageName);
	d_checkMarkImage	= &iset->getImage(CheckMarkImageName);
	d_disableImage		= &iset->getImage(DisableImageName);
	d_selectedColour	= DefaultSelectedLabelColour;
	d_textHFormatting	= LeftAligned;
	addCheckboxProperties();
}


/*************************************************************************
	Destructor for TLCheckbox objects.
*************************************************************************/
TLCheckbox::~TLCheckbox(void)
{
}

/*************************************************************************
	set  checkbox  normal, hover and mark image
*************************************************************************/
const Image *TLCheckbox::getCheckboxNormal() const 
{
	return d_normalImage;
}

void TLCheckbox::setCheckboxNormal( const Image *pNewImage )
{
	d_normalImage = pNewImage;
}

const Image *TLCheckbox::getCheckboxDisable() const 
{
	return d_disableImage;
}

void TLCheckbox::setCheckboxDisable( const Image *pNewImage )
{
	d_disableImage = pNewImage;
}

const Image *TLCheckbox::getCheckboxHover() const
{
	return d_hoverImage;
}

void TLCheckbox::setCheckboxHover( const Image *pNewImage )
{
	d_hoverImage = pNewImage;
}

const Image *TLCheckbox::getCheckboxMark() const
{
	return d_checkMarkImage;
}

void TLCheckbox::setCheckboxMark( const Image *pNewImage )
{
	d_checkMarkImage = pNewImage;
}

const colour TLCheckbox::getSelectedTextColour(void) const
{
	return d_selectedColour;
}

void TLCheckbox::setSelectedTextColour(const colour& colour)
{
	if (d_selectedColour != colour)
	{
		d_selectedColour = colour;
		requestRedraw();
	}
}

void TLCheckbox::addCheckboxProperties()
{
	addProperty( &d_chkNormalImage );
	addProperty( &d_chkDisableImage );
	addProperty( &d_chkHoverImage );
	addProperty( &d_chkMarkImage );
	addProperty( &d_selectedTextColour );
}

/*************************************************************************
	Perform the rendering for this widget.	
*************************************************************************/
void TLCheckbox::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{
	if ( d_selected )
	{
		drawSelected(panelCache, panelAbsPos);
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
	render the Checkbox in the normal state.	
*************************************************************************/
void TLCheckbox::drawNormal(KRenderCache* panelCache, Point* panelAbsPos)
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
	if (d_selected)
	{
		panelCache->cacheImage(d_checkMarkImage, absrect.getPosition(), clipper);
	}

	//
	// Draw label text
	//
	absrect.d_left += d_normalImage->getWidth();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}

	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_normalColour);
}


/*************************************************************************
	render the Checkbox in the hover / highlighted state.
*************************************************************************/
void TLCheckbox::drawHover(KRenderCache* panelCache, Point* panelAbsPos)
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
		panelCache->cacheImage(d_checkMarkImage, absrect.getPosition(), clipper);
	}
	else
	{
		panelCache->cacheImage(d_hoverImage, absrect.getPosition(), clipper);
	}

	//
	// Draw label text
	//
	absrect.d_left += d_hoverImage->getWidth();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}
	absrect.d_top += d_hoverYOff;

	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_hoverColour);
}


/*************************************************************************
	render the Checkbox in the pushed state.	
*************************************************************************/
void TLCheckbox::drawPushed(KRenderCache* panelCache, Point* panelAbsPos)
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
	panelCache->cacheImage(d_normalImage, absrect.getPosition(), clipper);
	if (d_selected)
	{
		panelCache->cacheImage(d_checkMarkImage, absrect.getPosition(), clipper);
	}

	//
	// Draw label text
	//
	absrect.d_left += d_normalImage->getWidth();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}
	absrect.d_top += d_pushedYOff;

	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_pushedColour);
}


/*************************************************************************
	render the Checkbox in the disabled state	
*************************************************************************/
void TLCheckbox::drawDisabled(KRenderCache* panelCache, Point* panelAbsPos)
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
	panelCache->cacheImage(d_disableImage, absrect.getPosition(), clipper);
	if (d_selected)
	{
		panelCache->cacheImage(d_checkMarkImage, absrect.getPosition(), clipper);
	}

	//
	// Draw label text
	//
	absrect.d_left += d_disableImage->getWidth();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}

	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_disabledColour);
}

/*************************************************************************
	render the Checkbox in the Selected state.	
*************************************************************************/
void TLCheckbox::drawSelected(KRenderCache* panelCache, Point* panelAbsPos)
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
	panelCache->cacheImage(d_checkMarkImage, absrect.getPosition(), clipper);

	//
	// Draw label text
	//
	absrect.d_left += d_normalImage->getWidth();

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
			absrect.d_top += (absrect.getHeight() - textHeight) / 2;
		}
		break;
	}

	panelCache->cacheText(getText(), absrect, clipper, getFont(), d_textHFormatting, d_selectedColour);
}

/*************************************************************************
	Handler for when WM_CHAR arrive
*************************************************************************/
void TLCheckbox::onCharacter(KeyEventArgs& e)
{
	e.handled = true;
}

void TLCheckbox::onMouseEnters(MouseEventArgs& e)
{
	Checkbox::onMouseEnters(e);
	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip& tip = KUiItemTip::GetSingleton();
		tip.show(Utf8ToAnsi( d_tooltipText ), getUnclippedInnerRect(), KUiItemTip::BottomLeft);
#endif // _LAYOUT_EDITOR_
	}
}

void TLCheckbox::onMouseLeaves(MouseEventArgs& e)
{
	Checkbox::onMouseLeaves(e);
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
	Create, initialise and return a TLCheckbox
*************************************************************************/
Window* TLCheckboxFactory::createWindow(const String& name)
{
	return new TLCheckbox(d_type, name);
}

} // End of  CEGUI namespace section
