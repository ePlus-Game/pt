/************************************************************************
	filename: 	CEGUIMultiLineEditbox.cpp
	created:	30/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of the Multi-line edit box base class
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
#include "elements/CEGUIMultiLineEditbox.h"
#include "elements/CEGUIScrollbar.h"
#include "CEGUITextUtils.h"
#include "CEGUIImage.h"
#include "CEGUIExceptions.h"
#include "KWin32Wnd.h"
#include "CEGUISystem.h"

// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	TODO:

	Clipboard support
	Undo support
*************************************************************************/
/*************************************************************************
	Static Properties for this class
*************************************************************************/
MultiLineEditboxProperties::ReadOnly				MultiLineEditbox::d_readOnlyProperty;
MultiLineEditboxProperties::WordWrap				MultiLineEditbox::d_wordWrapProperty;
MultiLineEditboxProperties::CaratIndex				MultiLineEditbox::d_caratIndexProperty;
MultiLineEditboxProperties::SelectionStart			MultiLineEditbox::d_selectionStartProperty;
MultiLineEditboxProperties::SelectionLength			MultiLineEditbox::d_selectionLengthProperty;
MultiLineEditboxProperties::MaxTextLength			MultiLineEditbox::d_maxTextLengthProperty;
MultiLineEditboxProperties::NormalTextColour		MultiLineEditbox::d_normalTextColourProperty;
MultiLineEditboxProperties::SelectedTextColour		MultiLineEditbox::d_selectedTextColourProperty;
MultiLineEditboxProperties::ActiveSelectionColour	MultiLineEditbox::d_activeSelectionColourProperty;
MultiLineEditboxProperties::InactiveSelectionColour	MultiLineEditbox::d_inactiveSelectionColourProperty;
MultiLineEditboxProperties::FrameEnabled			MultiLineEditbox::d_frameEnabledProperty;
MultiLineEditboxProperties::IsActivateOnShow        MultiLineEditbox::d_IsActivateShowProperty;


/*************************************************************************
	Constants
*************************************************************************/

// default colours
const argb_t MultiLineEditbox::DefaultNormalTextColour			= 0xFFFFFFFF;
const argb_t MultiLineEditbox::DefaultSelectedTextColour		= 0xFF000000;
const argb_t MultiLineEditbox::DefaultNormalSelectionColour		= 0xFF6060FF;
const argb_t MultiLineEditbox::DefaultInactiveSelectionColour	= 0xFF808080;

// Static data initialisation
String MultiLineEditbox::d_lineBreakChars((utf8*)"\n");


/*************************************************************************
	Constructor for the MultiLineEditbox base class.
*************************************************************************/
MultiLineEditbox::MultiLineEditbox(const String& type, const String& name) :
	Window(type, name),
	d_readOnly(false),
	d_maxTextLen(String::max_size()),
	d_caratPos(0),
	d_selectionStart(0),
	d_selectionEnd(0),
	d_dragging(false),
	d_dragAnchorIdx(0),
	d_wordWrap(true),
	d_widestExtent(0.0f),
	d_forceVertScroll(false),
	d_forceHorzScroll(false),
	d_selectionBrush(NULL),
	d_normalTextColour(DefaultNormalTextColour),
	d_selectTextColour(DefaultSelectedTextColour),
	d_selectBrushColour(DefaultNormalSelectionColour),
	d_inactiveSelectBrushColour(DefaultInactiveSelectionColour),
	d_caratflag(true),
	d_tickcount(0),
	d_frameEnabled(true),
	d_IsActivateShow(false)
{
	// add events specific to this widget.
	addMultiLineEditboxEvents();

	addMultiLineEditboxProperties();

	// we always need a terminating \n
	d_text.append(1, '\n');
}


/*************************************************************************
	Destructor for the MultiLineEditbox base class.
*************************************************************************/
MultiLineEditbox::~MultiLineEditbox(void)
{
}


/*************************************************************************
	Initialise the Window based object ready for use.	
*************************************************************************/
void MultiLineEditbox::initialise(void)
{
	// create the component sub-widgets
	d_vertScrollbar = createVertScrollbar(getName() + "__auto_vscrollbar__");
	d_horzScrollbar = createHorzScrollbar(getName() + "__auto_hscrollbar__");

	addChildWindow(d_vertScrollbar);
	addChildWindow(d_horzScrollbar);

    d_vertScrollbar->subscribeEvent(EventScrollPositionChanged, Event::Subscriber(&MultiLineEditbox::handle_scrollChange, this));
    d_horzScrollbar->subscribeEvent(EventScrollPositionChanged, Event::Subscriber(&MultiLineEditbox::handle_scrollChange, this));

	formatText();
	performChildWindowLayout();
}


/*************************************************************************
	return true if the edit box has input focus.	
*************************************************************************/
bool MultiLineEditbox::hasInputFocus(void) const
{
	return isActive();
}


/*************************************************************************
	return the current selection start point.	
*************************************************************************/
size_t MultiLineEditbox::getSelectionStartIndex(void) const
{
	return (d_selectionStart != d_selectionEnd) ? d_selectionStart : d_caratPos;
}


/*************************************************************************
	return the current selection end point.
*************************************************************************/
size_t MultiLineEditbox::getSelectionEndIndex(void) const
{
	return (d_selectionStart != d_selectionEnd) ? d_selectionEnd : d_caratPos;
}


/*************************************************************************
	return the length of the current selection (in code points / characters).	
*************************************************************************/
size_t MultiLineEditbox::getSelectionLength(void) const
{
	return d_selectionEnd - d_selectionStart;
}


/*************************************************************************
	Add multi-line edit box specific events	
*************************************************************************/
void MultiLineEditbox::addMultiLineEditboxEvents(void)
{
	addEvent(ML_EventReadOnlyModeChanged);
	addEvent(ML_EventWordWrapModeChanged);
	addEvent(ML_EventMaximumTextLengthChanged);
	addEvent(ML_EventCaratMoved);
	addEvent(ML_EventTextSelectionChanged);
	addEvent(ML_EventEditboxFull);
	addEvent(ML_EventVertScrollbarModeChanged);
	addEvent(ML_EventHorzScrollbarModeChanged);
	addEvent(EventPastFailed);
}


/*************************************************************************
	Perform the actual rendering for this Window.	
*************************************************************************/
void MultiLineEditbox::populateRenderCache()
{
	// get the derived class to render general stuff before we handle the text itself
	cacheEditboxBaseImagery();

	//
	// Render edit box text
	//
	Rect textarea(getTextRenderArea());

	cacheTextLines(textarea);

	if (hasInputFocus() && !isReadOnly())
	{
		cacheCaratImagery(textarea);
	}

}

/*************************************************************************
	Enables or disables the frame for this window.	
*************************************************************************/
void MultiLineEditbox::setFrameEnabled(bool setting)
{
	d_frameEnabled = setting;
	requestRedraw();
}


/*************************************************************************
	Specify whether the edit box is read-only.	
*************************************************************************/
void MultiLineEditbox::setReadOnly(bool setting)
{
	// if setting is changed
	if (d_readOnly != setting)
	{
		d_readOnly = setting;
		WindowEventArgs args(this);
 		onReadOnlyChanged(args);
	}

}


/*************************************************************************
	Set the current position of the carat.	
*************************************************************************/
void MultiLineEditbox::setCaratIndex(size_t carat_pos)
{
	// make sure new position is valid
	if (carat_pos > d_text.length() - 1)
	{
		carat_pos = d_text.length() - 1;
	}

	// if new position is different
	if (d_caratPos != carat_pos)
	{
		d_caratPos = carat_pos;
		ensureCaratIsVisible();

		// Trigger "carat moved" event
		WindowEventArgs args(this);
		onCaratMoved(args);
	}

}


/*************************************************************************
	Define the current selection for the edit box	
*************************************************************************/
void MultiLineEditbox::setSelection(size_t start_pos, size_t end_pos)
{
	// ensure selection start point is within the valid range
	if (start_pos > d_text.length() - 1)
	{
		start_pos = d_text.length() - 1;
	}

	// ensure selection end point is within the valid range
	if (end_pos > d_text.length() - 1)
	{
		end_pos = d_text.length() - 1;
	}

	// ensure start is before end
	if (start_pos > end_pos)
	{
		size_t tmp = end_pos;
		end_pos = start_pos;
		start_pos = tmp;
	}

	// only change state if values are different.
	if ((start_pos != d_selectionStart) || (end_pos != d_selectionEnd))
	{
		// setup selection
		d_selectionStart = start_pos;
		d_selectionEnd	 = end_pos;

		// Trigger "selection changed" event
		WindowEventArgs args(this);
		onTextSelectionChanged(args);
	}

}


/*************************************************************************
	set the maximum text length for this edit box.	
*************************************************************************/
void MultiLineEditbox::setMaxTextLength(size_t max_len)
{
	if (d_maxTextLen != max_len)
	{
		d_maxTextLen = max_len;

		// Trigger max length changed event
		WindowEventArgs args(this);
		onMaximumTextLengthChanged(args);

		// trim string
		if (d_text.length() > d_maxTextLen)
		{
			d_text.resize(d_maxTextLen);
			onTextChanged(args);
		}

	}

}


/*************************************************************************
	Set the colour to be used for rendering edit box text in the normal,
	unselected state.	
*************************************************************************/
void MultiLineEditbox::setNormalTextColour(const colour& col)
{
	d_normalTextColour = col;
	requestRedraw();
}


/*************************************************************************
	Set the colour to be used for rendering the edit box text when
	within the selected region.	
*************************************************************************/
void MultiLineEditbox::setSelectedTextColour(const colour& col)
{
	d_selectTextColour = col;
	requestRedraw();
}


/*************************************************************************
	Set the colour to be used for rendering the edit box selection
	highlight when the edit box is active.	
*************************************************************************/
void MultiLineEditbox::setNormalSelectBrushColour(const colour& col)
{
	d_selectBrushColour = col;
	requestRedraw();
}


/*************************************************************************
	Set the colour to be used for rendering the edit box selection
	highlight when the edit box is inactive.	
*************************************************************************/
void MultiLineEditbox::setInactiveSelectBrushColour(const colour& col)
{
	d_inactiveSelectBrushColour = col;
	requestRedraw();
}


/*************************************************************************
	Scroll the view so that the current carat position is visible.
*************************************************************************/
void MultiLineEditbox::ensureCaratIsVisible(void)
{
	// calculate the location of the carat
	const Font* fnt = getFont();
	size_t caratLine = getLineNumberFromIndex(d_caratPos);

	if (caratLine < d_lines.size())
	{
		Rect textArea(getTextRenderArea());

		size_t caratLineIdx = d_caratPos - d_lines[caratLine].d_startIdx;

		float ypos = caratLine * fnt->getLineSpacing();

		float xpos = 0;
		if ( d_text.length() >= d_lines[caratLine].d_startIdx )
		{
			xpos = const_cast<Font*>(fnt)->getTextExtent(d_text.substr(d_lines[caratLine].d_startIdx, caratLineIdx));
		}
		else
		{
			return;
		}
		

		// adjust position for scroll bars
		xpos -= d_horzScrollbar->getScrollPosition();
		ypos -= d_vertScrollbar->getScrollPosition();

		// if carat is above window, scroll up
		if (ypos < 0)
		{
			d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition() + ypos);
		}
		// if carat is below the window, scroll down
		else if ((ypos += fnt->getLineSpacing()) > textArea.getHeight())
		{
			d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition() + (ypos - textArea.getHeight()) + fnt->getLineSpacing());
		}

		// if carat is left of the window, scroll left
		if (xpos < 0)
		{
			d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition() + xpos - 50);
		}
		// if carat is right of the window, scroll right
		else if (xpos > textArea.getWidth())
		{
			d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition() + (xpos - textArea.getWidth()) + 50);
		}

	}

}


/*************************************************************************
	Set whether the text will be word wrapped or not.
*************************************************************************/
void MultiLineEditbox::setWordWrapping(bool setting)
{
	if (setting != d_wordWrap)
	{
		d_wordWrap = setting;
		formatText();

		WindowEventArgs args(this);
		onWordWrapModeChanged(args);
	}

}

void MultiLineEditbox::injectTimePulse(DWORD curUpdateTime)
{
	if(d_readOnly || !d_enabled)
	{
		return;
	}
	DWORD elapsed = ::GetTickCount();
	if( (elapsed - d_tickcount) >= 500 )
	{
		d_caratflag = !d_caratflag;
		d_tickcount = elapsed;	
		requestRedraw();
	}
}

/*************************************************************************
	display required integrated scroll bars according to current state
	of the edit box and update their values.
*************************************************************************/
void MultiLineEditbox::configureScrollbars(void)
{
	float totalHeight	= (float)d_lines.size() * getFont()->getLineSpacing();
	float widestItem	= d_widestExtent;

	//
	// First show or hide the scroll bars as needed (or requested)
	//
	// show or hide vertical scroll bar as required (or as specified by option)
	if ((totalHeight > getTextRenderArea().getHeight()) || d_forceVertScroll)
	{
		d_vertScrollbar->show();

		// show or hide horizontal scroll bar as required (or as specified by option)
		if ((widestItem > getTextRenderArea().getWidth()) || d_forceHorzScroll)
		{
			d_horzScrollbar->show();
		}
		else
		{
			d_horzScrollbar->hide();
		}

	}
	else
	{
		// show or hide horizontal scroll bar as required (or as specified by option)
		if ((widestItem > getTextRenderArea().getWidth()) || d_forceHorzScroll)
		{
			d_horzScrollbar->show();

			// show or hide vertical scroll bar as required (or as specified by option)
			if ((totalHeight > getTextRenderArea().getHeight()) || d_forceVertScroll)
			{
				d_vertScrollbar->show();
			}
			else
			{
				d_vertScrollbar->hide();
			}

		}
		else
		{
			d_vertScrollbar->hide();
			d_horzScrollbar->hide();
		}

	}

	//
	// Set up scroll bar values
	//
	Rect renderArea(getTextRenderArea());

	d_vertScrollbar->setDocumentSize(totalHeight);
	d_vertScrollbar->setPageSize(renderArea.getHeight());
	d_vertScrollbar->setStepSize(max(1.0f, renderArea.getHeight() / 10.0f));
	d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition());

	d_horzScrollbar->setDocumentSize(widestItem);
	d_horzScrollbar->setPageSize(renderArea.getWidth());
	d_horzScrollbar->setStepSize(max(1.0f, renderArea.getWidth() / 10.0f));
	d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition());
}


/*************************************************************************
	Render text lines.	
*************************************************************************/
void MultiLineEditbox::cacheTextLines(const Rect& dest_area)
{
    // text is already formatted, we just grab the lines and render them with the required alignment.
    Rect drawArea(dest_area);
    drawArea.offset(Point(-d_horzScrollbar->getScrollPosition(), -d_vertScrollbar->getScrollPosition()));

    Renderer* renderer = System::getSingleton().getRenderer();
    const Font* fnt = getFont();

    if (fnt)
    {
        // get layers to use for rendering
        float textZ = renderer->getZLayer(4) - renderer->getCurrentZ();
        float selZ  = renderer->getZLayer(3) - renderer->getCurrentZ();

        // calculate final colours to use.
        ColourRect colours;
        float alpha = getEffectiveAlpha();
        colour normalTextCol = d_normalTextColour;
        normalTextCol.setAlpha(normalTextCol.getAlpha() * alpha);
        colour selectTextCol = d_normalTextColour;
        selectTextCol.setAlpha(selectTextCol.getAlpha() * alpha);
        colour selectBrushCol = hasInputFocus() ? d_selectBrushColour : d_inactiveSelectBrushColour;
        selectBrushCol.setAlpha(selectBrushCol.getAlpha() * alpha);

        // for each formatted line.
        for (size_t i = 0; i < d_lines.size(); ++i)
        {
            Rect lineRect(drawArea);
            const LineInfo& currLine = d_lines[i];
            String lineText(d_text.substr(currLine.d_startIdx, currLine.d_length));

            // if it is a simple 'no selection area' case
            if ((currLine.d_startIdx >= d_selectionEnd) ||
                ((currLine.d_startIdx + currLine.d_length) <= d_selectionStart) ||
                (d_selectionBrush == NULL))
            {
                colours.setColours(normalTextCol);
                // render the complete line.
                d_renderCache.cacheText(lineText, fnt, LeftAligned, lineRect, textZ, colours, &dest_area);
            }
            // we have at least some selection highlighting to do
            else
            {
                // Start of actual rendering section.
                String sect;
                size_t sectIdx = 0, sectLen;
                float selStartOffset = 0.0f, selAreaWidth = 0.0f;

                // render any text prior to selected region of line.
                if (currLine.d_startIdx < d_selectionStart)
                {
                    // calculate length of text section
                    sectLen = d_selectionStart - currLine.d_startIdx;

                    // get text for this section
                    sect = lineText.substr(sectIdx, sectLen);
                    sectIdx += sectLen;

                    // get the pixel offset to the beginning of the selection area highlight.
                    selStartOffset = const_cast<Font*>(fnt)->getTextExtent(sect);

                    // draw this portion of the text
                    colours.setColours(normalTextCol);
                    d_renderCache.cacheText(sect, fnt, LeftAligned, lineRect, textZ, colours, &dest_area);

                    // set position ready for next portion of text
                    lineRect.d_left += selStartOffset;
                }

                // calculate the length of the selected section
                sectLen = min(d_selectionEnd - currLine.d_startIdx, currLine.d_length) - sectIdx;

                // get the text for this section
                sect = lineText.substr(sectIdx, sectLen);
                sectIdx += sectLen;

                // get the extent to use as the width of the selection area highlight
                selAreaWidth = const_cast<Font*>(fnt)->getTextExtent(sect);

                // draw the text for this section
                colours.setColours(selectTextCol);
                d_renderCache.cacheText(sect, fnt, LeftAligned, lineRect, textZ, colours, &dest_area);

                // render any text beyond selected region of line
                if (sectIdx < currLine.d_length)
                {
                    // update render position to the end of the selected area.
                    lineRect.d_left += selAreaWidth;

                    // calculate length of this section
                    sectLen = currLine.d_length - sectIdx;

                    // get the text for this section
                    sect = lineText.substr(sectIdx, sectLen);

                    // render the text for this section.
                    colours.setColours(normalTextCol);
                    d_renderCache.cacheText(sect, fnt, LeftAligned, lineRect, textZ, colours, &dest_area);
                }

                // calculate area for the selection brush on this line
                lineRect.d_left = drawArea.d_left + selStartOffset;
                lineRect.d_right = lineRect.d_left + selAreaWidth;
                lineRect.d_bottom = lineRect.d_top + fnt->getLineSpacing();

                // render the selection area brush for this line
                colours.setColours(selectBrushCol);
                d_renderCache.cacheImage(*d_selectionBrush, lineRect, selZ, colours, &dest_area);
            }

            // update master position for next line in paragraph.
            drawArea.d_top += fnt->getLineSpacing();
        }
    }
}

void MultiLineEditbox::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{
	Point panelOff(-panelAbsPos->d_x, -panelAbsPos->d_y);

	Rect drawArea(getTextRenderArea());
	Rect clipper(getPixelRect().offset(panelOff));

    drawArea.offset(Point(
		-panelAbsPos->d_x - d_horzScrollbar->getScrollPosition(), 
		-panelAbsPos->d_y - d_vertScrollbar->getScrollPosition())).offset(getUnclippedPixelRect().getPosition());

    Renderer* renderer = System::getSingleton().getRenderer();
    const Font* fnt = getFont();

    if(NULL == fnt)
    {
		return;
	}
    
	bool bDrawCarat = false;
	bool bHasDrawn = false;
	int	 textPos = 0;
	
	// for each formatted line.
	for (size_t i = 0; i < d_lines.size(); ++i)
	{
		// if no selection area, calculate if we should draw carat here
		if ( !bHasDrawn && d_selectionStart == d_selectionEnd 
			 && !isReadOnly() && hasInputFocus() )
		{
			int length = textPos+d_lines[i].d_length;
			if ( length <= d_caratPos )
				textPos += d_lines[i].d_length;
			else
				bDrawCarat = true;
		}

		Rect lineRect(drawArea);
		const LineInfo& currLine = d_lines[i];
		String lineText(d_text.substr(currLine.d_startIdx, currLine.d_length));
		
		// if it is a simple 'no selection area' case
		if ((currLine.d_startIdx >= d_selectionEnd) ||
			((currLine.d_startIdx + currLine.d_length) <= d_selectionStart) ||
			(d_selectionBrush == NULL))
		{   
			panelCache->cacheText(lineText, lineRect, clipper, fnt, LeftAligned, d_normalTextColour);

			// draw carat
			if ( bDrawCarat && d_caratflag )
			{
				String clipText(d_text.substr(currLine.d_startIdx, (d_caratPos-textPos)));
				float extentToText = const_cast<Font *>(fnt)->getTextExtent(clipText);

				lineRect.d_left += extentToText;
				panelCache->cacheText("_", lineRect, clipper, fnt, LeftAligned, d_normalTextColour);
				lineRect.d_top += 0.5;
				panelCache->cacheText("_", lineRect, clipper, fnt, LeftAligned, d_normalTextColour);
				bDrawCarat = false;
				bHasDrawn = true;
			}
		}
		// we have at least some selection highlighting to do
		else
		{
			// Start of actual rendering section.
			String sect;
			size_t sectIdx = 0, sectLen;
			float selStartOffset = 0.0f, selAreaWidth = 0.0f;
			
			// render any text prior to selected region of line.
			if (currLine.d_startIdx < d_selectionStart)
			{
				// calculate length of text section
				sectLen = d_selectionStart - currLine.d_startIdx;
				
				// get text for this section
				sect = lineText.substr(sectIdx, sectLen);
				sectIdx += sectLen;
				
				// get the pixel offset to the beginning of the selection area highlight.
				selStartOffset = const_cast<Font*>(fnt)->getTextExtent(sect);
				
				// draw this portion of the text
				panelCache->cacheText(sect, lineRect, clipper, fnt, LeftAligned, d_normalTextColour);
				
				// set position ready for next portion of text
				lineRect.d_left += selStartOffset;
			}
			
			// calculate the length of the selected section
			sectLen = min(d_selectionEnd - currLine.d_startIdx, currLine.d_length) - sectIdx;
			
			// get the text for this section
			sect = lineText.substr(sectIdx, sectLen);
			sectIdx += sectLen;
			
			// get the extent to use as the width of the selection area highlight
			selAreaWidth = const_cast<Font*>(fnt)->getTextExtent(sect);
			
			// draw the text for this section
			panelCache->cacheText(sect, lineRect, clipper, fnt, LeftAligned, d_normalTextColour);
			
			// render any text beyond selected region of line
			if (sectIdx < currLine.d_length)
			{
				// update render position to the end of the selected area.
				lineRect.d_left += selAreaWidth;
				
				// calculate length of this section
				sectLen = currLine.d_length - sectIdx;
				
				// get the text for this section
				sect = lineText.substr(sectIdx, sectLen);
				
				// render the text for this section.
				panelCache->cacheText(sect, lineRect, clipper, fnt, LeftAligned, d_normalTextColour);
			}
			
			// calculate area for the selection brush on this line
			lineRect.d_left = drawArea.d_left + selStartOffset;
			lineRect.d_right = lineRect.d_left + selAreaWidth;
			lineRect.d_bottom = lineRect.d_top + fnt->getLineSpacing();
			
			float top = getUnclippedPixelRect().d_top - panelAbsPos->d_y;
			float bottom = getUnclippedPixelRect().d_bottom - panelAbsPos->d_y;
			if ( top <= lineRect.d_top && bottom >= lineRect.d_bottom )
				panelCache->cacheImage(d_selectionBrush, lineRect.getPosition(), lineRect);

			// render the selection area brush for this line
			//panelCache->cacheImage(d_selectionBrush, lineRect.getPosition(), lineRect);
		}
		
		// update master position for next line in paragraph.
		drawArea.d_top += fnt->getLineSpacing();
	}
}
/*************************************************************************
	Format the text into lines as needed by the current formatting options.
*************************************************************************/
void MultiLineEditbox::formatText(void)
{
	// clear old formatting data
	d_lines.clear();
	d_widestExtent = 0.0f;

	String paraText;

	const Font* fnt = getFont();

	if (fnt != NULL)
	{
		float areaWidth = getTextRenderArea().getWidth();

		String::size_type	currPos = 0;
		String::size_type	paraLen;
		LineInfo	line;

		while (currPos < d_text.length())
		{
			if ((paraLen = d_text.find_first_of(d_lineBreakChars, currPos)) == String::npos)
			{
				paraLen = d_text.length() - currPos;
			}
			else
			{
				++paraLen -= currPos;
			}

			paraText = d_text.substr(currPos, paraLen);

			if (!d_wordWrap || (areaWidth <= 0.0f))
			{
				// no word wrapping, so we are just one long line.
				line.d_startIdx = currPos;
				line.d_length	= paraLen;
				line.d_extent	= const_cast<Font*>(fnt)->getTextExtent(paraText); 
				d_lines.push_back(line);

				// update widest, if needed.
				if (line.d_extent > d_widestExtent)
				{
					d_widestExtent = line.d_extent;
				}

			}
			// must word-wrap the paragraph text
			else
			{
				String::size_type lineIndex = 0;

				// while there is text in the string
				while (lineIndex < paraLen)
				{
					String::size_type  lineLen = 0;
					float lineExtent = 0.0f;

					// loop while we have not reached the end of the paragraph string
					while (lineLen < (paraLen - lineIndex))
					{
						// get cp / char count of next token
						size_t nextTokenSize = getNextTokenLength(paraText, lineIndex + lineLen);

						// get pixel width of the token
						float tokenExtent  = const_cast<Font*>(fnt)->getTextExtent(paraText.substr(lineIndex + lineLen, nextTokenSize));

						// would adding this token would overflow the available width
						if ((lineExtent + tokenExtent) > areaWidth)
						{
							// Was this the first token?
							if (lineLen == 0)
							{
								// get point at which to break the token
								float rr = const_cast<Font*>(fnt)->getCharAtPixel(paraText.substr(lineIndex, nextTokenSize + lineLen), areaWidth, 1.0f);
								lineExtent += const_cast<Font*>(fnt)->getTextExtent(paraText.substr(lineIndex + lineLen, rr));
								lineLen = rr;
							}

							// text wraps, exit loop early with line info up until wrap point
							break;
						}

						// add this token to current line
						lineLen    += nextTokenSize;
						lineExtent += tokenExtent;
					}

					// set up line info and add to collection
					line.d_startIdx = currPos + lineIndex;
					line.d_length	= lineLen;
					line.d_extent	= lineExtent;
					d_lines.push_back(line);

					// update widest, if needed.
					if (lineExtent > d_widestExtent)
					{
						d_widestExtent = lineExtent;
					}

					// update position in string
					lineIndex += lineLen;
				}

			}

			// skip to next 'paragraph' in text
			currPos += paraLen;
		}

	}

	configureScrollbars();
	requestRedraw();
}


/*************************************************************************
	Return the length of the next token in String 'text' starting at
	index 'start_idx'.
*************************************************************************/
size_t MultiLineEditbox::getNextTokenLength(const String& text, size_t start_idx) const
{
	String::size_type pos = text.find_first_of(TextUtils::DefaultWrapDelimiters, start_idx);

	// handle case where no more whitespace exists (so this is last token)
	if (pos == String::npos)
	{
		return (text.length() - start_idx);
	}
	// handle 'delimiter' token cases
	else if ((pos - start_idx) == 0)
	{
		return 1;
	}
	else
	{
		return (pos - start_idx);
	}

}


/*************************************************************************
	Return the text code point index that is rendered closest to screen
	position 'pt'.	
*************************************************************************/
size_t MultiLineEditbox::getTextIndexFromPosition(const Point& pt) const
{
	//
	// calculate final window position to be checked
	//
	Point wndPt = screenToWindow(pt);

	if (getMetricsMode() == Relative)
	{
		wndPt = relativeToAbsolute(wndPt);
	}

	Rect textArea(getTextRenderArea());

	wndPt.d_x -= textArea.d_left;
	wndPt.d_y -= textArea.d_top;

	// factor in scroll bar values
	wndPt.d_x += d_horzScrollbar->getScrollPosition();
	wndPt.d_y += d_vertScrollbar->getScrollPosition();

	size_t lineNumber = static_cast<size_t>(wndPt.d_y / getFont()->getLineSpacing());

	if (lineNumber >= d_lines.size())
	{
		lineNumber = d_lines.size() - 1;
	}

	String lineText(d_text.substr(d_lines[lineNumber].d_startIdx, d_lines[lineNumber].d_length));

	size_t lineIdx = const_cast<Font*>(getFont())->getCharAtPixel(lineText, wndPt.d_x);

	if (lineIdx >= lineText.length() - 1)
	{
		lineIdx = lineText.length() - 1;
	}

	return d_lines[lineNumber].d_startIdx + lineIdx;
}


/*************************************************************************
	Return the line number a given index falls on with the current
	formatting.  Will return last line if index is out of range.
*************************************************************************/
size_t MultiLineEditbox::getLineNumberFromIndex(size_t index) const
{
	size_t lineCount = d_lines.size();

	if (lineCount == 0)
	{
		return 0;
	}
	else if (index >= d_text.length() - 1)
	{
		return lineCount - 1;
	}
	else
	{
		size_t indexCount = 0;
		size_t caratLine = 0;

		for (; caratLine < lineCount; ++caratLine)
		{
			indexCount += d_lines[caratLine].d_length;

			if (index < indexCount)
			{
				return caratLine;
			}

		}

	}

	throw InvalidRequestException((utf8*)"MultiLineEditbox::getLineNumberFromIndex - Unable to identify a line from the given, invalid, index.");
}



/*************************************************************************
	Clear the current selection setting
*************************************************************************/
void MultiLineEditbox::clearSelection(void)
{
	// perform action only if required.
	if (getSelectionLength() != 0)
	{
		setSelection(0, 0);
	}

}


/*************************************************************************
	Erase the currently selected text.
*************************************************************************/
void MultiLineEditbox::eraseSelectedText(bool modify_text)
{
	if (getSelectionLength() != 0)
	{
		// setup new carat position and remove selection highlight.
		setCaratIndex(getSelectionStartIndex());

		// erase the selected characters (if required)
		if (modify_text)
		{
			d_text.erase(getSelectionStartIndex(), getSelectionLength());

			// trigger notification that text has changed.
			WindowEventArgs args(this);
			onTextChanged(args);
		}

		clearSelection();
	}

}


/*************************************************************************
	Processing for backspace key
*************************************************************************/
void MultiLineEditbox::handleBackspace(void)
{
	if (!isReadOnly())
	{
		if (getSelectionLength() != 0)
		{
			eraseSelectedText();
		}
		else if (d_caratPos > 0)
		{
			d_text.erase(d_caratPos - 1, 1);
			setCaratIndex(d_caratPos - 1);

			WindowEventArgs args(this);
			onTextChanged(args);
		}

	}
}


/*************************************************************************
	Processing for Delete key
*************************************************************************/
void MultiLineEditbox::handleDelete(void)
{
	if (!isReadOnly())
	{
		if (getSelectionLength() != 0)
		{
			eraseSelectedText();
		}
		else if (getCaratIndex() < d_text.length() - 1)
		{
			d_text.erase(d_caratPos, 1);
			ensureCaratIsVisible();

			WindowEventArgs args(this);
			onTextChanged(args);
		}

	}

}


/*************************************************************************
	Processing to move carat one character left
*************************************************************************/
void MultiLineEditbox::handleCharLeft(uint sysKeys)
{
	if (d_caratPos > 0)
	{
		setCaratIndex(d_caratPos - 1);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caratPos, d_dragAnchorIdx);	
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move carat one word left
*************************************************************************/
void MultiLineEditbox::handleWordLeft(uint sysKeys)
{
	if (d_caratPos > 0)
	{
		setCaratIndex(TextUtils::getWordStartIdx(d_text, getCaratIndex()));
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caratPos, d_dragAnchorIdx);	
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move carat one character right
*************************************************************************/
void MultiLineEditbox::handleCharRight(uint sysKeys)
{
	if (d_caratPos < d_text.length() - 1)
	{
		setCaratIndex(d_caratPos + 1);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caratPos, d_dragAnchorIdx);	
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move carat one word right
*************************************************************************/
void MultiLineEditbox::handleWordRight(uint sysKeys)
{
	if (d_caratPos < d_text.length() - 1)
	{
		setCaratIndex(TextUtils::getNextWordStartIdx(d_text, getCaratIndex()));
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caratPos, d_dragAnchorIdx);	
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move carat to the start of the text.
*************************************************************************/
void MultiLineEditbox::handleDocHome(uint sysKeys)
{
	if (d_caratPos > 0)
	{
		setCaratIndex(0);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caratPos, d_dragAnchorIdx);	
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move carat to the end of the text
*************************************************************************/
void MultiLineEditbox::handleDocEnd(uint sysKeys)
{
	if (d_caratPos < d_text.length() - 1)
	{
		setCaratIndex(d_text.length() - 1);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caratPos, d_dragAnchorIdx);	
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move carat to the start of the current line.	
*************************************************************************/
void MultiLineEditbox::handleLineHome(uint sysKeys)
{
	size_t line = getLineNumberFromIndex(d_caratPos);

	if (line < d_lines.size())
	{
		size_t lineStartIdx = d_lines[line].d_startIdx;

		if (d_caratPos > lineStartIdx)
		{
			setCaratIndex(lineStartIdx);
		}

		if (sysKeys & Shift)
		{
			setSelection(d_caratPos, d_dragAnchorIdx);	
		}
		else
		{
			clearSelection();
		}

	}

}


/*************************************************************************
	Processing to move carat to the end of the current line
*************************************************************************/
void MultiLineEditbox::handleLineEnd(uint sysKeys)
{
	size_t line = getLineNumberFromIndex(d_caratPos);

	if (line < d_lines.size())
	{
		size_t lineEndIdx = d_lines[line].d_startIdx + d_lines[line].d_length - 1;

		if (d_caratPos < lineEndIdx)
		{
			setCaratIndex(lineEndIdx);
		}

		if (sysKeys & Shift)
		{
			setSelection(d_caratPos, d_dragAnchorIdx);	
		}
		else
		{
			clearSelection();
		}

	}

}


/*************************************************************************
	Processing to move carat up a line.
*************************************************************************/
void MultiLineEditbox::handleLineUp(uint sysKeys)
{
	size_t caratLine = getLineNumberFromIndex(d_caratPos);

	if (caratLine > 0)
	{
		float caratPixelOffset = const_cast<Font*>(getFont())->getTextExtent(d_text.substr(d_lines[caratLine].d_startIdx, d_caratPos - d_lines[caratLine].d_startIdx));

		--caratLine;

		size_t newLineIndex = const_cast<Font*>(getFont())->getCharAtPixel(d_text.substr(d_lines[caratLine].d_startIdx, d_lines[caratLine].d_length), caratPixelOffset);

		setCaratIndex(d_lines[caratLine].d_startIdx + newLineIndex);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caratPos, d_dragAnchorIdx);	
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to move carat down a line.
*************************************************************************/
void MultiLineEditbox::handleLineDown(uint sysKeys)
{
	size_t caratLine = getLineNumberFromIndex(d_caratPos);

	if ((d_lines.size() > 1) && (caratLine < (d_lines.size() - 1)))
	{
		float caratPixelOffset =  const_cast<Font*>(getFont())->getTextExtent(d_text.substr(d_lines[caratLine].d_startIdx, d_caratPos - d_lines[caratLine].d_startIdx));

		++caratLine;

		size_t newLineIndex = const_cast<Font*>(getFont())->getCharAtPixel(d_text.substr(d_lines[caratLine].d_startIdx, d_lines[caratLine].d_length), caratPixelOffset);

		setCaratIndex(d_lines[caratLine].d_startIdx + newLineIndex);
	}

	if (sysKeys & Shift)
	{
		setSelection(d_caratPos, d_dragAnchorIdx);	
	}
	else
	{
		clearSelection();
	}

}


/*************************************************************************
	Processing to insert a new line / paragraph.
*************************************************************************/
void MultiLineEditbox::handleNewLine(uint sysKeys)
{
	if (!isReadOnly())
	{
		// erase selected text
		eraseSelectedText();

		// if there is room
		if (d_text.length() - 1 < d_maxTextLen)
		{
			String str = d_text;
			String::size_type idx = str.find("\n");
			int entercount = 0;
			while ( idx+1 <= str.length() && idx != String::npos )
			{
				entercount++;
				idx = str.find("\n", idx+1);
			}

			d_text.insert(getCaratIndex(), 1, 0x0a);
			d_caratPos++;

			WindowEventArgs args(this);
			onTextChanged(args);
		}

	}

}


/*************************************************************************
	Handler for when a mouse button is pushed
*************************************************************************/
void MultiLineEditbox::onMouseButtonDown(MouseEventArgs& e)
{
	// base class handling
	Window::onMouseButtonDown(e);

	if (e.button == LeftButton)
	{
		// grab inputs
		if (captureInput())
		{
			// handle mouse down
			clearSelection();
			d_dragging = true;
			d_dragAnchorIdx = getTextIndexFromPosition(e.position);
			setCaratIndex(d_dragAnchorIdx);

			if (d_vertScrollbar)
				d_vertScrollbar->deactivate();

			if (d_horzScrollbar)
				d_horzScrollbar->deactivate();

		}

		e.handled = true;
	}

	d_tickcount = ::GetTickCount() - 500;
	d_caratflag = false;

}


/*************************************************************************
	Handler for when mouse button is released
*************************************************************************/
void MultiLineEditbox::onMouseButtonUp(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseButtonUp(e);

	if (e.button == LeftButton)
	{
		releaseInput();
		e.handled = true;
	}

}


/*************************************************************************
	Handler for when mouse button is double-clicked
*************************************************************************/
void MultiLineEditbox::onMouseDoubleClicked(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseDoubleClicked(e);

	if (e.button == LeftButton)
	{
		d_dragAnchorIdx = TextUtils::getWordStartIdx(d_text, (d_caratPos == d_text.length()) ? d_caratPos : d_caratPos + 1);
		d_caratPos		= TextUtils::getNextWordStartIdx(d_text, d_caratPos);

		// perform actual selection operation.
		setSelection(d_dragAnchorIdx, d_caratPos);

		e.handled = true;
	}

}


/*************************************************************************
	Handler for when mouse button is triple-clicked.
*************************************************************************/
void MultiLineEditbox::onMouseTripleClicked(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseTripleClicked(e);

	if (e.button == LeftButton)
	{
		size_t caratLine = getLineNumberFromIndex(d_caratPos);
		size_t lineStart = d_lines[caratLine].d_startIdx;

		// find end of last paragraph
		String::size_type paraStart = d_text.find_last_of(d_lineBreakChars, lineStart);

		// if no previous paragraph, selection will start at the beginning.
		if (paraStart == String::npos)
		{
			paraStart = 0;
		}

		// find end of this paragraph
		String::size_type paraEnd = d_text.find_first_of(d_lineBreakChars, lineStart);

		// if paragraph has no end, which actually should never happen, fix the
		// erroneous situation and select up to end at end of text.
		if (paraEnd == String::npos)
		{
			d_text.append(1, '\n');
			paraEnd = d_text.length() - 1;
		}

		// set up selection using new values.
		d_dragAnchorIdx = paraStart;
		setCaratIndex(paraEnd);
		setSelection(d_dragAnchorIdx, d_caratPos);
		e.handled = true;
	}

}


/*************************************************************************
	Handler for when mouse moves in the window.
*************************************************************************/
void MultiLineEditbox::onMouseMove(MouseEventArgs& e)
{
	// base class processing
	Window::onMouseMove(e);

	if (d_dragging)
	{
		setCaratIndex(getTextIndexFromPosition(e.position));
		setSelection(d_caratPos, d_dragAnchorIdx);
	}

	e.handled = true;
}


/*************************************************************************
	Handler for when capture is lost.
*************************************************************************/
void MultiLineEditbox::onCaptureLost(WindowEventArgs& e)
{
	d_dragging = false;

	// base class processing
	Window::onCaptureLost(e);

	e.handled = true;
}


/*************************************************************************
	Handler for when character (printable keys) are typed
*************************************************************************/
void MultiLineEditbox::onCharacter(KeyEventArgs& e)
{
	// base class processing
	Window::onCharacter(e);

	// only need to take notice if we have focus
	if (hasInputFocus() && !isReadOnly() && getFont()->isCodepointAvailable(e.codepoint) && e.codepoint >= 32 )
	{
		// erase selected text
		eraseSelectedText();

		// if there is room
		if (d_text.length() - 1 < d_maxTextLen)
		{
			d_text.insert(getCaratIndex(), 1, e.codepoint);
			d_caratPos++;

			WindowEventArgs args(this);
			onTextChanged(args);
		}
		else
		{
			// Trigger text box full event
			WindowEventArgs args(this);
			onEditboxFullEvent(args);
		}

	}

	e.handled = true;
}


/*************************************************************************
	Handler for when non-printable keys are typed.
*************************************************************************/
void MultiLineEditbox::onKeyDown(KeyEventArgs& e)
{
	// base class processing
	Window::onKeyDown(e);

	if (hasInputFocus() && !isReadOnly())
	{
		WindowEventArgs args(this);
		switch (e.scancode)
		{
		case Key::LeftShift:
		case Key::RightShift:
			if (getSelectionLength() == 0)
			{
				d_dragAnchorIdx = getCaratIndex();
			}
			break;

		case Key::Backspace:
			handleBackspace();
			break;

		case Key::Delete:
			handleDelete();
			break;

		case Key::Return:
		case Key::NumpadEnter:
			handleNewLine(e.sysKeys);
			break;

		case Key::ArrowLeft:
			if (e.sysKeys & Control)
			{
				handleWordLeft(e.sysKeys);
			}
			else
			{
				handleCharLeft(e.sysKeys);
			}
			break;

		case Key::ArrowRight:
			if (e.sysKeys & Control)
			{
				handleWordRight(e.sysKeys);
			}
			else
			{
				handleCharRight(e.sysKeys);
			}
			break;

		case Key::ArrowUp:
			handleLineUp(e.sysKeys);
			break;

		case Key::ArrowDown:
			handleLineDown(e.sysKeys);
			break;

		case Key::Home:
			if (e.sysKeys & Control)
			{
				handleDocHome(e.sysKeys);
			}
			else
			{
				handleLineHome(e.sysKeys);
			}
			break;

		case Key::End:
			if (e.sysKeys & Control)
			{
				handleDocEnd(e.sysKeys);
			}
			else
			{
				handleLineEnd(e.sysKeys);
			}
			break;
		case Key::C:
			if  ( e.sysKeys & Control )
			{
				copy();
			}
			break;
		case Key::X:
			if ( e.sysKeys & Control )
			{
				cut();
			}
			break;
		case Key::V:
			if ( e.sysKeys & Control )
			{
				int nRet = paste();
				if (nRet == 0)
				{
					fireEvent(EventPastFailed,e);	
				}//endif
				
			}
        // default case is now to leave event as (possibly) unhandled.
        default:
            return;
		}

		e.handled = true;
	}

}

void MultiLineEditbox::setIsActivatedOnShow(const bool bEnable)
{
    d_IsActivateShow = bEnable;
}

bool MultiLineEditbox::getIsActivatedOnShow()const
{
	return d_IsActivateShow;
}

int MultiLineEditbox::paste()
{
	int bPasted = false;
	if (OpenClipboard(NULL))
	{
		HGLOBAL hData = GetClipboardData(CF_TEXT);
		if (hData)
		{
			char* pszString = (char*)GlobalLock(hData);
            if ( !isReadOnly() && pszString )
            {
				String str = AnsiToUtf8(pszString);
				String::size_type idx = str.find("\n");
				if ( idx != String::npos )
				{
					GlobalUnlock(hData);
					CloseClipboard();
					return bPasted;
				}

				if ( d_selectionStart == d_selectionEnd )
				{
					// if it hasnot selected text
					d_text.insert( d_caratPos, AnsiToUtf8( pszString ) );
				}
				else
				{
					//d_text.erase( d_caratPos, d_selectionEnd - d_selectionStart );
					if (d_selectionEnd > d_selectionStart)
					{
						d_text.erase( d_selectionStart, d_selectionEnd - d_selectionStart );
						d_caratPos  = d_selectionStart; 
					}//endif
					else
					{
						d_text.erase( d_selectionEnd,d_selectionStart - d_selectionEnd);
						d_caratPos  = d_selectionEnd;
					}//endif

					d_text.insert( d_caratPos, AnsiToUtf8(pszString) );
					d_selectionStart = d_selectionEnd = 0;
					
				}
				
				size_t new_caretPos = d_caratPos + str.length();
				
				if ( d_text.length() > d_maxTextLen )
				{
					bPasted   =  false;	
				    d_text.erase(d_maxTextLen ,d_text.length() - d_maxTextLen);
				}//endif
				else
					bPasted   = true;
				
				d_caratPos = new_caretPos;
					
				if (d_caratPos>=d_text.length() - 1)
					d_caratPos = d_text.length() - 1;
					

				GlobalUnlock(hData);
				if (isReadOnly())
					EmptyClipboard();
            }
		}
		CloseClipboard();
		WindowEventArgs args(this);
		onTextChanged(args);
	}
	return bPasted;
}

int MultiLineEditbox::copy()
{
	int bCopyed = false;

	if ( !isReadOnly() && !d_text.empty() && d_selectionEnd != d_selectionStart )
	{
		if (OpenClipboard(g_GetMainHWnd()))
		{
			EmptyClipboard();
	
			String temp(d_text);

			int len = d_selectionEnd - d_selectionStart;
			temp.erase(0, d_selectionStart);
			temp.erase(len, temp.length() - len);

			String::size_type idx = temp.find("\n");
			if ( idx != String::npos )
			{
				CloseClipboard();
				return bCopyed;
			}

			char* src = Utf8ToAnsi( temp );
			len = strlen(src);
			d_selectionEnd = d_selectionStart = 0;
						
			HGLOBAL hData = (char*)GlobalAlloc(GMEM_MOVEABLE, len + 1);
			if (hData)
			{
				char* pszData = (char*)GlobalLock(hData);
				if (pszData)
				{
					memcpy(pszData, src, len);					
					pszData[len] = 0;
					GlobalUnlock(hData);
					SetClipboardData(CF_TEXT, hData);
					bCopyed = true;
				}
			}
			CloseClipboard();
		}
	}
	return bCopyed;
}

int MultiLineEditbox::cut()
{
	int bCopyed = false;
	if ( !isReadOnly() && !d_text.empty() )
	{
		if (OpenClipboard(g_GetMainHWnd()))
		{
			EmptyClipboard();
			
			int len = d_selectionEnd - d_selectionStart;
			String temp = d_text;
			temp.erase(0, d_selectionStart);
			temp.erase(len, temp.length() - len);

			String::size_type idx = temp.find("\n");
			if ( idx != String::npos )
			{
				CloseClipboard();
				return bCopyed;
			}

			d_text.erase( d_selectionStart, len );
			
			char* src = Utf8ToAnsi( temp );
			len = strlen(src);
						
			HGLOBAL hData = (char*)GlobalAlloc(GMEM_MOVEABLE, len + 1);
			if (hData)
			{
				char* pszData = (char*)GlobalLock(hData);
				if (pszData)
				{
					
					memcpy(pszData, src, len);
					pszData[len] = 0;
					d_caratPos = d_selectionStart;
					d_selectionStart = d_selectionEnd = 0;	
					GlobalUnlock(hData);
					SetClipboardData(CF_TEXT, hData);
					bCopyed = true;
				}
			}
			CloseClipboard();
			WindowEventArgs args(this);
			onTextChanged(args);
		}
	}
	//d_text.clear();
	return bCopyed;
}

/*************************************************************************
	Handler for when text is programmatically changed.
*************************************************************************/
void MultiLineEditbox::onTextChanged(WindowEventArgs& e)
{
    // ensure last character is a new line
    if ((d_text.length() == 0) || (d_text[d_text.length() - 1] != '\n'))
        d_text.append(1, '\n');

    // base class processing
    Window::onTextChanged(e);

    // clear selection
    clearSelection();
    // layout new text
    formatText();
    // layout child windows (scrollbars) since text layout may have changed
    performChildWindowLayout();
    // ensure carat is still within the text
    setCaratIndex(getCaratIndex());
    // ensure carat is visible
    // NB: this will already have been called at least once, but since we
    // may have changed the formatting of the text, it needs to be called again.
    ensureCaratIsVisible();

    e.handled = true;
}


/*************************************************************************
	Handler for when widget size is changed.
*************************************************************************/
void MultiLineEditbox::onSized(WindowEventArgs& e)
{
	formatText();

	// base class handling
	Window::onSized(e);

	e.handled = true;
}


/*************************************************************************
	Handler for mouse wheel changes
*************************************************************************/
void MultiLineEditbox::onMouseWheel(MouseEventArgs& e)
{
	// base class processing.
	Window::onMouseWheel(e);

	if (d_vertScrollbar->isVisible() && (d_vertScrollbar->getDocumentSize() > d_vertScrollbar->getPageSize()))
	{
		d_vertScrollbar->setScrollPosition(d_vertScrollbar->getScrollPosition() + d_vertScrollbar->getStepSize() * -e.wheelChange);
	}
	else if (d_horzScrollbar->isVisible() && (d_horzScrollbar->getDocumentSize() > d_horzScrollbar->getPageSize()))
	{
		d_horzScrollbar->setScrollPosition(d_horzScrollbar->getScrollPosition() + d_horzScrollbar->getStepSize() * -e.wheelChange);
	}

	e.handled = true;
}


/*************************************************************************
	Handler called when the read-only state of the edit box changes	
*************************************************************************/
void MultiLineEditbox::onReadOnlyChanged(WindowEventArgs& e)
{
	fireEvent(ML_EventReadOnlyModeChanged, e);
}


/*************************************************************************
	Handler called when the word wrap mode for the the edit box changes
*************************************************************************/
void MultiLineEditbox::onWordWrapModeChanged(WindowEventArgs& e)
{
	fireEvent(ML_EventWordWrapModeChanged, e);
}


/*************************************************************************
	Handler called when the maximum text length for the edit box changes	
*************************************************************************/
void MultiLineEditbox::onMaximumTextLengthChanged(WindowEventArgs& e)
{
	fireEvent(ML_EventMaximumTextLengthChanged, e);
}


/*************************************************************************
	Handler called when the carat moves.	
*************************************************************************/
void MultiLineEditbox::onCaratMoved(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(ML_EventCaratMoved, e);
}


/*************************************************************************
	Handler called when the text selection changes	
*************************************************************************/
void MultiLineEditbox::onTextSelectionChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(ML_EventTextSelectionChanged, e);
}


/*************************************************************************
	Handler called when the edit box is full	
*************************************************************************/
void MultiLineEditbox::onEditboxFullEvent(WindowEventArgs& e)
{
	fireEvent(ML_EventEditboxFull, e);
}


/*************************************************************************
	Handler called when the 'always show' setting for the vertical
	scroll bar changes.	
*************************************************************************/
void MultiLineEditbox::onVertScrollbarModeChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(ML_EventVertScrollbarModeChanged, e);
}


/*************************************************************************
	Handler called when 'always show' setting for the horizontal scroll
	bar changes.	
*************************************************************************/
void MultiLineEditbox::onHorzScrollbarModeChanged(WindowEventArgs& e)
{
	requestRedraw();
	fireEvent(ML_EventHorzScrollbarModeChanged, e);
}

void MultiLineEditbox::onShown(WindowEventArgs& e)
{
	System::getSingleton().plusShowEditNum( this->getName() );
	const char *name = this->getName().c_str();

	if (d_IsActivateShow)
		activate();

	beginUpdate();
	Window::onShown(e);
}

void MultiLineEditbox::onHidden(WindowEventArgs& e)
{
	System::getSingleton().minusShowEditNum( this->getName() );
	const char *name = this->getName().c_str();
	stopUpdate();
	Window::onHidden(e);
}
/*************************************************************************
	Return whether the text in the edit box will be word-wrapped.
*************************************************************************/
bool MultiLineEditbox::isWordWrapped(void) const
{
	return d_wordWrap;
}


/*************************************************************************
	Add new properties for this class
*************************************************************************/
void MultiLineEditbox::addMultiLineEditboxProperties(void)
{
	addProperty(&d_readOnlyProperty);
	addProperty(&d_wordWrapProperty);
	addProperty(&d_caratIndexProperty);
	addProperty(&d_selectionStartProperty);
	addProperty(&d_selectionLengthProperty);
	addProperty(&d_maxTextLengthProperty);
	addProperty(&d_normalTextColourProperty);
	addProperty(&d_selectedTextColourProperty);
	addProperty(&d_activeSelectionColourProperty);
	addProperty(&d_inactiveSelectionColourProperty);
	addProperty(&d_frameEnabledProperty);
	addProperty(&d_IsActivateShowProperty);
}

/*************************************************************************
    Handler for scroll position changes.
*************************************************************************/
bool MultiLineEditbox::handle_scrollChange(const EventArgs& args)
{
    // simply trigger a redraw of the Listbox.
    requestRedraw();
    return true;
}



} // End of  CEGUI namespace section
