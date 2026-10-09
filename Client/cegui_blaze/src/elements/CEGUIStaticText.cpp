/************************************************************************
	filename: 	CEGUIStaticText.cpp
	created:	4/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of the static text widget class
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
#include "elements/CEGUIStaticText.h"
#include "CEGUIFont.h"
#include "CEGUIWindowManager.h"
#include "CEGUIExceptions.h"
#include "elements/CEGUIScrollbar.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String StaticText::EventNamespace("StaticText");

/*************************************************************************
Static Properties for this class
*************************************************************************/
StaticTextProperties::TextColours		StaticText::d_textColoursProperty;
StaticTextProperties::VertFormatting	StaticText::d_vertFormattingProperty;
StaticTextProperties::HorzFormatting	StaticText::d_horzFormattingProperty;
StaticTextProperties::RollSpeedH		StaticText::d_rollSpeedHProperty;


/*************************************************************************
	Constructor for static text widgets.	
*************************************************************************/
StaticText::StaticText(const String& type, const String& name) :
	Static(type, name),
	d_horzFormatting(LeftAligned),
	d_vertFormatting(VertCentred),
    d_textCols(0xFFFFFFFF),
	d_offset(0,0),
	d_rollSpeedH(0)
{
	addStaticTextProperties();
}


/*************************************************************************
	Destructor for static text widgets.
*************************************************************************/
StaticText::~StaticText(void)
{
}


/*************************************************************************
	Sets the colours to be applied when rendering the text.	
*************************************************************************/
void StaticText::setTextColours(const colour& colours)
{
	d_textCols = colours;
	requestRedraw();
}


/*************************************************************************
	Set the formatting required for the text.
*************************************************************************/
void StaticText::setFormatting(HorzFormatting h_fmt, VertFormatting v_fmt)
{
	d_horzFormatting = h_fmt;
	d_vertFormatting = v_fmt;
	requestRedraw();
}


/*************************************************************************
	Set the formatting required for the text.	
*************************************************************************/
void StaticText::setVerticalFormatting(VertFormatting v_fmt)
{
	d_vertFormatting = v_fmt;
	requestRedraw();
}


/*************************************************************************
	Set the formatting required for the text.	
*************************************************************************/
void StaticText::setHorizontalFormatting(HorzFormatting h_fmt)
{
	d_horzFormatting = h_fmt;
	requestRedraw();
}


/*************************************************************************
	Perform the actual rendering for this Window.
*************************************************************************/
void StaticText::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{
	Static::drawSelf(panelCache, panelAbsPos);

	const Font* font = getFont();

    if (font == 0)
        return;

	Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
	//计算出相对于panel的位置
	Rect clipper(getTextRenderArea());
	Rect absarea(getTextRenderArea());
	clipper.offset(offPos);
	float textHeight = const_cast<Font *>(font)->getFormattedLineCount(d_text, absarea, (TextFormatting)d_horzFormatting) * font->getLineSpacing();

	switch(d_vertFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absarea.d_top += absarea.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absarea.d_top += (absarea.getHeight() - textHeight) / 2;
		}
		break;
	}

	panelCache->cacheText(d_text, absarea, clipper,
		font, (TextFormatting)d_horzFormatting, d_textCols);
}

void StaticText::populateRenderCache()
{
	// get whatever base class needs to render.
	Static::populateRenderCache();

	const Font* font = getFont();
    // can't render text without a font :)
    if (font == 0)
        return;

	// get destination area for the text.
	Rect absarea(getTextRenderArea());
	Rect clipper(absarea);

	float textHeight = const_cast<Font *>(font)->getFormattedLineCount(d_text, absarea, (TextFormatting)d_horzFormatting) * font->getLineSpacing();
	
    // calculate final colours
    ColourRect final_cols(d_textCols);
    final_cols.modulateAlpha(getEffectiveAlpha());
    // cache the text for rendering.
	
	switch(d_vertFormatting)
	{
	case TopAligned:
		{
			//
		}
		break;
	case BottomAligned:
		{
			absarea.d_top += absarea.getHeight() - textHeight;
		}
		break;
	case VertCentred:
		{
			absarea.d_top += (absarea.getHeight() - textHeight) / 2;
		}
		break;
	}
	d_renderCache.cacheText(d_text, font, (TextFormatting)d_horzFormatting, 
			absarea.offset(d_offset), 0.0f, final_cols, &clipper);
}
/*************************************************************************
	Add properties for static text
*************************************************************************/
void StaticText::addStaticTextProperties(void)
{
	addProperty(&d_textColoursProperty);
	addProperty(&d_vertFormattingProperty);
	addProperty(&d_horzFormattingProperty);
	addProperty(&d_rollSpeedHProperty);
}


/*************************************************************************
	Return a Rect object describing, in un-clipped pixels, the window
	relative area that the text should be rendered in to.
*************************************************************************/
Rect StaticText::getTextRenderArea(void) const
{
	Rect area(Point(0,0), getAbsoluteSize());

	if (d_frameEnabled)
	{
		area.d_bottom -= d_bottom_height;
	}

	if (d_frameEnabled)
	{
		area.d_right -= d_right_width;
	}

	if (d_frameEnabled)
	{
		area.d_left	+= d_left_width;
		area.d_top	+= d_top_height;
	}

	return area;
}


void StaticText::updateSelf(float elapsed)
{
	Static::updateSelf(elapsed);
	
	if(d_rollSpeedH > 0)
	{
		d_offset.d_x -= d_rollSpeedH * elapsed;
		if(-d_offset.d_x > ((Font*)getFont())->getTextExtent(d_text))
			d_offset.d_x = this->getTextRenderArea().d_right;
		requestRedraw();
	}
}

void StaticText::setOff(int xoff, int yoff)
{
	d_offset.d_x = xoff;
	d_offset.d_y = yoff;
}

} // End of  CEGUI namespace section
