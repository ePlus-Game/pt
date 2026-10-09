/************************************************************************
	filename: 	TLEditbox.cpp
	created:	29/5/2004
	author:		Paul D Turner
	
	purpose:	Implementation of Taharez Look Editbox widget
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
#include "TLEditbox.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"
#include "CEGUIFont.h"
#include <time.h>
#include "Ui/LayoutRender.h"
#ifndef _LAYOUT_EDITOR_
#include "Ui/UiCase/UiItemTip.h"
#include "Ui\UiCase\UiToolsControlBar.h"
#endif // _LAYOUT_EDITOR_
#include "CEGUISystem.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*************************************************************************
	Constants
*************************************************************************/
// type name for this widget
const utf8	TLEditbox::WidgetTypeName[]	= "TaharezLook/Editbox";

// image name constants
const utf8	TLEditbox::ImagesetName[]				= "TaharezLook";
const utf8	TLEditbox::ContainerLeftImageName[]		= "EditBoxLeft";
const utf8	TLEditbox::ContainerMiddleImageName[]	= "EditBoxMiddle";
const utf8	TLEditbox::ContainerRightImageName[]	= "EditBoxRight";
const utf8	TLEditbox::CaratImageName[]				= "EditBoxCarat";
const utf8	TLEditbox::SelectionBrushImageName[]	= "TextSelectionBrush";
const utf8	TLEditbox::MouseCursorImageName[]		= "MouseTextBar";

// layout values
const float	TLEditbox::TextPaddingRatio		= 0.1f;

// implementation constantss
const uint	TLEditbox::SelectionLayer	= 1;
const uint	TLEditbox::TextLayer		= 2;
const uint	TLEditbox::CaratLayer		= 3;


/*************************************************************************
	Constructor for Taharez edit box widgets	
*************************************************************************/
TLEditbox::TLEditbox(const String& type, const String& name) :
	Editbox(type, name),
	d_lastTextOffset(0)
	,d_bIsOnlyNumber(false)
	,d_bShowCaret(false)
{
	Imageset* iset = ImagesetManager::getSingleton().getImageset(ImagesetName);
	
	// cache images to be used
	d_left		= &iset->getImage(ContainerLeftImageName);
	d_middle	= &iset->getImage(ContainerMiddleImageName);
	d_right		= &iset->getImage(ContainerRightImageName);
	d_carat		= &iset->getImage(CaratImageName);
	d_selection	= &iset->getImage(SelectionBrushImageName);
	d_caratflag	= true;
	d_tickcount	= 0;
	d_layout = NULL;
	d_layoutRender = NULL;
	d_bAscIICharacters = false;
	setMouseCursor(&iset->getImage(MouseCursorImageName));
}


/*************************************************************************
	Destructor for Taharez edit box widgets	
*************************************************************************/
TLEditbox::~TLEditbox(void)
{
	if(d_layout != NULL)
	{
		d_layout->Release();
		delete d_layout;
		d_layout = NULL;
	}
}


/*************************************************************************
	Return the text code point index that is rendered closest to screen
	position 'pt'.	
*************************************************************************/
size_t TLEditbox::getTextIndexFromPosition(const Point& pt) const
{
	//
	// calculate final window position to be checked
	//
	float wndx = screenToWindowX(pt.d_x);

	if (getMetricsMode() == Relative)
	{
		wndx = relativeToAbsoluteX(wndx);
	}
	
	wndx -= d_lastTextOffset;
	
	if (d_rightToRightFormat)
		wndx -= getAbsoluteWidth() - const_cast<Font *>(getFont())->getTextExtent(d_text) -  const_cast<Font *>(getFont())->getTextExtent("_");
	
	//
	// Return the proper index
	//
	if (isTextMasked())
	{
		return const_cast<Font*>(getFont())->getCharAtPixel(String(d_text.length(), getMaskCodePoint()), wndx);
	}
	else
	{
		return const_cast<Font*>(getFont())->getCharAtPixel(d_text, wndx);
	}
	
}


/*************************************************************************
	return text padding value to use in pixels	
*************************************************************************/
float TLEditbox::getTextPaddingPixels(void) const
{
	return PixelAligned(d_left->getWidth() * TextPaddingRatio);
}

const DWORD				s_caratSlashTime	= 500;
const DWORD				s_immCaratSlashTime = 2000;
bool					s_isImmShowCarat	= false;

void TLEditbox::setLayoutOffset(int x, int y)
{
	d_layoutXoff = x;
	d_layoutYoff = y;
}

Point TLEditbox::getLayoutOffset()
{
	return Point(d_layoutXoff, d_layoutYoff);
}

void TLEditbox::useLayout()	
{
	if(d_layout != NULL)
		return;
	
	if(d_layoutRender != NULL)
	{
		d_layoutRender->Release();
		delete d_layoutRender;
	}

	d_layoutRender = new LayoutRender();
	ILayout* layout = NULL;
	CreateLayout(&layout, d_layoutRender);

	if(NULL == layout)
		return;
	
	d_layout = layout;
};

void TLEditbox::onCharacter(KeyEventArgs& e)
{
	// base class processing
	Window::onCharacter(e);
	
	if(d_layout != NULL)
		return;
	
	if ( !getFont()->isCodepointAvailable(e.codepoint) )
	{
		((Font*)(getFont()))->getGlyphData(e.codepoint);
	}
	
	// only need to take notice if we have focus
	//if (hasInputFocus() && !isReadOnly())
	if (hasInputFocus() && getFont()->isCodepointAvailable(e.codepoint) && !isReadOnly() && e.codepoint >= 32 )
	{
		// backup current text
		String tmp(d_text);
		tmp.erase(getSelectionStartIndex(), getSelectionLength());

		// if there is room
		if (tmp.length() < d_maxTextLen)
		{
			tmp.insert(getSelectionStartIndex(), 1, e.codepoint);
			
			if ( d_bIsOnlyNumber || d_numberOnly )
			{
				if ( e.codepoint < 48 ||
					 e.codepoint > 57)
				{
					WindowEventArgs args(this);
					onInvalidEntryAttempted(args);
					e.handled = true;
					return;
				}
			}
			else
			{	
				if ( d_bAscIICharacters )
				{

					// 只有英文字符和数字
					if ( !((e.codepoint>47&&e.codepoint<58) || 
						   (e.codepoint>64&&e.codepoint<91) || 
						  (e.codepoint>96&&e.codepoint<123)) )
					{
						WindowEventArgs args(this);
						onInvalidEntryAttempted(args);
						e.handled = true;
						return;
					}
				}
			}
			if (isStringValid(tmp))
			{
				// erase selection using mode that does not modify d_text (we just want to update state)
				eraseSelectedText(false);

                // advance carat (done first so we can "do stuff" in event handlers!)
                d_caratPos++;

                // set text to the newly modified string
				setText(tmp);
			}
			else
			{
				// Trigger invalid modification attempted event.
				WindowEventArgs args(this);
				onInvalidEntryAttempted(args);
			}

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

void TLEditbox::injectTimePulse(DWORD curUpdateTime)
{
	updateSelf( ::GetTickCount() );
}

void TLEditbox::updateSelf(float elapsed)
{
	if(s_isImmShowCarat)
	{
		d_caratflag = true;
		d_tickcount = elapsed;
		requestRedraw();
		s_isImmShowCarat = false;
		return;
	}

	if((elapsed - d_tickcount) >= s_caratSlashTime)
	{
		d_caratflag = !d_caratflag;
		d_tickcount = elapsed;	
		requestRedraw();
	}

}

void TLEditbox::showCaratImm()
{
	d_tickcount = ::GetTickCount();
	d_caratflag = true;
	s_isImmShowCarat = true;
	requestRedraw();
}

/*************************************************************************
	Overridden Event Handling Functions
*************************************************************************/
void TLEditbox::onMouseButtonDown(MouseEventArgs& e)
{
	Editbox::onMouseButtonDown(e);
	d_tickcount = ::GetTickCount() - s_caratSlashTime;
	d_caratflag = false;
}

void TLEditbox::onMouseEnters(MouseEventArgs& e)
{
	Editbox::onMouseEnters(e);

	if ( isActive() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiMiniNaviation::GetSingletonPtr()->setMiniNavMouseStatus(MOUSE_NORMAL_STATUS);
#endif // _LAYOUT_EDITOR_
	}

	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip::GetSingleton();
		KUiItemTip::GetSingleton().show(Utf8ToAnsi( d_tooltipText ), getUnclippedInnerRect(), KUiItemTip::BottomLeft);
#endif // _LAYOUT_EDITOR_
	}
}

void TLEditbox::onMouseMove(MouseEventArgs& e)
{
	Editbox::onMouseMove(e);

	if ( isActive() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiMiniNaviation::GetSingletonPtr()->setMiniNavMouseStatus(MOUSE_NORMAL_STATUS);
#endif // _LAYOUT_EDITOR_
	}
}

void TLEditbox::onMouseLeaves(MouseEventArgs& e)
{
	Editbox::onMouseLeaves(e);

	if ( isActive() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiMiniNaviation::GetSingletonPtr()->setMiniNavMouseStatus(MOUSE_NORMAL_STATUS);
#endif // _LAYOUT_EDITOR_
	}

	if ( !d_tooltipText.empty() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiItemTip::Hide();
#endif // _LAYOUT_EDITOR_
	}
}

void TLEditbox::onKeyDown(KeyEventArgs& e)
{
	//使用排版就不用控件默认的消息处理
	if(d_layout != NULL)
	{
		Window::onKeyDown(e);
		return;
	}
	else if ( d_bIsOnlyNumber )
	{
		// 只能输入字符和数字的时候禁止粘贴
		if (hasInputFocus() && !isReadOnly())
		{
			WindowEventArgs args(this);
			switch (e.scancode)
			{
			case Key::V:
				return;
			}
		}
	}

	Editbox::onKeyDown(e);

	if( (e.scancode >= Key::Q && e.scancode <= Key::M)||(e.scancode >= Key::One && e.scancode <= Key::Zero)
		)
	{
		e.handled = true;
	}
}

/*void TLEditbox::onShown(WindowEventArgs& e)
{
	//System::getSingleton().plusShowEditNum();
	Editbox::onShown(e);
}
*/
void TLEditbox::onHidden(WindowEventArgs& e)
{
	if ( isActive() )
	{
#ifndef _LAYOUT_EDITOR_
		KUiMiniNaviation::GetSingletonPtr()->setMiniNavMouseStatus(MOUSE_NORMAL_STATUS);
#endif // _LAYOUT_EDITOR_
	}

	Editbox::onHidden(e);
}
/*************************************************************************
	Perform the actual rendering for this Window.
*************************************************************************/
void TLEditbox::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{
	TextFormatting coreFmt = LeftAligned;
/*	if (d_rightToRightFormat)
		coreFmt = RightAligned;
*/
	Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
	Rect clipper(getPixelRect());
	clipper.offset(offPos);

	if (clipper.getWidth() == 0)
	{
		return;
	}

	Rect absrect(getUnclippedPixelRect());
	absrect.offset(offPos);

	if(d_layout != NULL)
	{
		d_layoutRender->setDrawPanel(panelCache);
		Point absPos(absrect.getPosition());
		d_layout->Render(absPos.d_x + d_layoutXoff, absPos.d_y + d_layoutYoff, 0);
		if ( d_bShowCaret )
		{
			d_layout->showCarat(d_caratflag, true);
		}
		else
		{
			d_layout->showCarat(false, true);
		}
		return;
	}
	
	const Font* fnt = getFont();
	Renderer*	renderer = System::getSingleton().getRenderer();

	bool hasFocus = hasInputFocus();

	//
	// render container
	//
	// calculate widths for container segments
	float leftWidth		= d_left->getWidth();
	float rightWidth	= d_right->getWidth();
	float midWidth		= absrect.getWidth() - leftWidth - rightWidth;

	//
	// Required preliminary work for main rendering operations
	//
	// Create a 'masked' version of the string if needed.
	String editText;

	if (isTextMasked())
	{
		editText.insert(0, d_text.length(), getMaskCodePoint());
	}
	else
	{
		editText = d_text;
	}

	// calculate new area rect considering text padding value.
	float textpadding = getTextPaddingPixels();

	absrect.d_left		+= textpadding;
	absrect.d_top		+= textpadding;
	absrect.d_right		-= textpadding;
	absrect.d_bottom	-= textpadding;

	// calculate best position to render text to ensure carat is always visible
	float textOffset;
	float extentToCarat = const_cast<Font *>(fnt)->getTextExtent(editText.substr(0, getCaratIndex()));

	if (!d_rightToRightFormat)
	{
		// if box is inactive
		if (!hasFocus)
		{
			textOffset = d_lastTextOffset;
		}
		// if carat is to the left of the box
		else if ((d_lastTextOffset + extentToCarat) < 0)
		{
			textOffset = -extentToCarat;
		}
		// if carat is off to the right.
		else if ((d_lastTextOffset + extentToCarat) >= (absrect.getWidth() - d_carat->getWidth()))
		{
			textOffset = absrect.getWidth() - extentToCarat - d_carat->getWidth();
		}
		// else carat is already within the box
		else
		{
			textOffset = d_lastTextOffset;
		}

	}//endif
	else
	{
		if (!hasFocus)
		{
			textOffset = d_lastTextOffset;
		}
		// if carat is to the left of the box
		else if ((d_lastTextOffset + extentToCarat + absrect.getWidth() - const_cast<Font *>(fnt)->getTextExtent(d_text)) < 0)
		{
			textOffset = const_cast<Font *>(fnt)->getTextExtent(d_text) - absrect.getWidth() -extentToCarat;
		}
		// if carat is off to the right.
		else if ((d_lastTextOffset + extentToCarat + absrect.getWidth() - const_cast<Font *>(fnt)->getTextExtent(d_text)) >= absrect.getWidth())
		{
			textOffset = const_cast<Font *>(fnt)->getTextExtent(d_text) - extentToCarat;
		}
		// else carat is already within the box
		else
		{
			textOffset = d_lastTextOffset;
		}//end else
			

	}//end else
	
	// adjust clipper for new target area
	clipper = absrect.getIntersection(clipper);

	//
	// Draw label text
	//
	// setup initial rect for text formatting
	Rect text_rect(absrect);
	text_rect.d_top  += PixelAligned((text_rect.getHeight() - getFont()->getLineSpacing()) * 0.5f);
	text_rect.d_left += textOffset;

	if (d_rightToRightFormat)
		text_rect.d_left += absrect.d_right - absrect.d_left - const_cast<Font *>(fnt)->getTextExtent(d_text);

	// draw pre-highlight text
	String sect = editText.substr(0, getSelectionStartIndex());
	panelCache->cacheText(sect, text_rect, clipper, getFont(), coreFmt, d_normalTextColour);
	
	text_rect.d_left += const_cast<Font *>(fnt)->getTextExtent(sect);

	// draw highlight text
	sect = editText.substr(getSelectionStartIndex(), getSelectionLength());
	panelCache->cacheText(sect, text_rect, clipper, getFont(), coreFmt, d_normalTextColour);

	text_rect.d_left += const_cast<Font *>(fnt)->getTextExtent(sect);

	// draw post-highlight text
	sect = editText.substr(getSelectionEndIndex());
	
	panelCache->cacheText(sect, text_rect, clipper, getFont(), coreFmt, d_normalTextColour);
	
	//
	// Render carat
	//
	float extentToText = const_cast<Font *>(fnt)->getTextExtent(d_text);
	if ((!isReadOnly()) && hasFocus  )
	{
		Point pos = Point(absrect.d_left + textOffset + extentToCarat, absrect.d_top);
		if (d_rightToRightFormat)
		{
			pos.d_x += absrect.d_right - absrect.d_left - const_cast<Font *>(fnt)->getTextExtent(d_text);
			
			//if (d_text.length() == 0)
			pos.d_x	-=  const_cast<Font *>(fnt)->getTextExtent("_");
		}//endif
		
		Size sz(d_carat->getWidth(), absrect.getHeight());
		
		if ( d_caratflag && d_selectionEnd == d_selectionStart)
		{
			//d_carat->draw(pos, sz, clipper, colours);
			text_rect.d_left = pos.d_x;
			text_rect.d_top = pos.d_y;
			panelCache->cacheText("_", text_rect, clipper, getFont(), coreFmt, d_normalTextColour);
			text_rect.d_top = pos.d_y + 1;
			panelCache->cacheText("_", text_rect, clipper, getFont(), coreFmt, d_normalTextColour);
			/*
			text_rect.d_top = pos.d_y;
			text_rect.d_left = pos.d_x+2;
			panelCache->cacheText("|", text_rect, clipper, getFont(), LeftAligned, d_normalTextColour);
			text_rect.d_left = pos.d_x+3;
			panelCache->cacheText("|", text_rect, clipper, getFont(), LeftAligned, d_normalTextColour);
			//*/
		}
	}

	//
	// Render selection brush
	//
	if (getSelectionLength() != 0)
	{
		// calculate required start and end offsets
		float selStartOffset	= const_cast<Font *>(fnt)->getTextExtent(editText.substr(0, getSelectionStartIndex()));
		float selEndOffset		= const_cast<Font *>(fnt)->getTextExtent(editText.substr(0, getSelectionEndIndex()));

		// calculate highlight area
		Rect hlarea;
		hlarea.d_left	= absrect.d_left + textOffset + selStartOffset;
		hlarea.d_right	= absrect.d_left + textOffset + selEndOffset;

		if (d_rightToRightFormat)
		{
			hlarea.d_left  += absrect.d_right - absrect.d_left - const_cast<Font *>(fnt)->getTextExtent(d_text);
			hlarea.d_right += absrect.d_right - absrect.d_left - const_cast<Font *>(fnt)->getTextExtent(d_text);
		}//endif

		hlarea.d_top	= text_rect.d_top;
		hlarea.d_bottom = hlarea.d_top + fnt->getLineSpacing();

		if ( hlarea.d_left < absrect.d_left )
			hlarea.d_left = absrect.d_left;
		if ( hlarea.d_right > absrect.d_right )
			hlarea.d_right = absrect.d_right;

		// render the highlight
		panelCache->cacheImage(d_selection, hlarea.getPosition(), hlarea);
	}

	d_lastTextOffset = textOffset;	
}	

void TLEditbox::resetText(String& newText)
{
	d_text = newText;
	clearSelection();

	if (getCaratIndex() > d_text.length())
	{
		setCaratIndex(d_text.length());
	}
}


void TLEditbox::onDeactivated(ActivationEventArgs& e)
{
	Window::onDeactivated(e);
	if ( d_layout != NULL )
	{
		d_bShowCaret = false;
	}
}

void TLEditbox::onActivated(ActivationEventArgs& e)
{
	Window::onActivated(e);
	if ( d_layout != NULL )
	{
		d_bShowCaret = true;
	}
}
//////////////////////////////////////////////////////////////////////////
/*************************************************************************

	Factory Methods

*************************************************************************/
//////////////////////////////////////////////////////////////////////////
/*************************************************************************
	Create, initialise and return a TLEditbox
*************************************************************************/
Window* TLEditboxFactory::createWindow(const String& name)
{
	return new TLEditbox(d_type, name);
}

} // End of  CEGUI namespace section
