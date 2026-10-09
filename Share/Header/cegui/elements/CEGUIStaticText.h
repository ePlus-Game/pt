/************************************************************************
	filename: 	CEGUIStaticText.h
	created:	4/6/2004
	author:		Paul D Turner
	
	purpose:	Defines interface for a static text widget
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
#ifndef _CEGUIStaticText_h_
#define _CEGUIStaticText_h_

#include "elements/CEGUIStatic.h"
#include "elements/CEGUIStaticTextProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Base class for a static text widget.
*/
class CEGUIEXPORT StaticText : public Static
{
public:
//	void setText(String& newText);
	static const String EventNamespace;				//!< Namespace for global events


	/*************************************************************************
		Formatting Enumerations
	*************************************************************************/
	/*!
	\brief
		Enumeration of horizontal formatting options for static text widgets
	*/
	enum HorzFormatting
	{
		LeftAligned,			//!< Text is output as a single line of text with the first character aligned with the left edge of the widget.
		RightAligned,			//!< Text is output as a single line of text with the last character aligned with the right edge of the widget.
		HorzCentred,			//!< Text is output as a single line of text horizontally centred within the widget.
		HorzJustified,			//!< Text is output as a single line of text with the first and last characters aligned with the edges of the widget.
		WordWrapLeftAligned,	//!< Text is output as multiple word-wrapped lines of text with the first character of each line aligned with the left edge of the widget.
		WordWrapRightAligned,	//!< Text is output as multiple word-wrapped lines of text with the last character of each line aligned with the right edge of the widget.
		WordWrapCentred,		//!< Text is output as multiple word-wrapped lines of text with each line horizontally centered within the widget.
		WordWrapJustified		//!< Text is output as multiple word-wrapped lines of text with the first and last characters of each line aligned with the edges of the widget.
	};


	/*!
	\brief
		Enumeration of vertical formatting options for a static text widgets
	*/
	enum VertFormatting
	{
		TopAligned,		//!< Text is output with the top of first line of text aligned with the top edge of the widget.
		BottomAligned,	//!< Text is output with the bottom of last line of text aligned with the bottom edge of the widget.
		VertCentred     //!< Text is output vertically centred within the widget.
	};

	StaticText(const String& type, const String& name);
	virtual ~StaticText(void);

	//TEXT相关
	void			setTextColours(const colour& colours);
	colour			getTextColours(void) const	{return d_textCols;}

	HorzFormatting	getHorizontalFormatting(void) const		{return	d_horzFormatting;}
	void			setHorizontalFormatting(HorzFormatting h_fmt);

	VertFormatting	getVerticalFormatting(void) const		{return	d_vertFormatting;}
	void			setVerticalFormatting(VertFormatting v_fmt);

	void			setFormatting(HorzFormatting h_fmt, VertFormatting v_fmt);

	//text滚动相关
	void			setRollSpeedH(float speed)		{ d_rollSpeedH = speed;};
	float			getRollSpeedH(void) const		{return d_rollSpeedH;}

	void			setOff(int xoff, int yoff);
protected:
	/*************************************************************************
		Overridden from base class
	*************************************************************************/
	virtual void	drawSelf(KRenderCache* panelCache, Point* panelAbsPos);
	virtual void	populateRenderCache();

	/*************************************************************************
		Implementation methods
	*************************************************************************/

	virtual	Rect	getTextRenderArea(void) const;


	virtual bool	testClassName_impl(const String& class_name) const
	{
		if (class_name==(const utf8*)"StaticText")	return true;
		return Static::testClassName_impl(class_name);
	}


	/*************************************************************************
		更新字符串状态
	*************************************************************************/
	virtual void updateSelf(float elapsed);
	
	/*************************************************************************
		Implementation Data
	*************************************************************************/
	HorzFormatting	d_horzFormatting;		//!< Horizontal formatting to be applied to the text.
	VertFormatting	d_vertFormatting;		//!< Vertical formatting to be applied to the text.
	colour			d_textCols;				//!< Colours used when rendering the text.

	
	//横向滚动参数
	Point			d_offset;
	float			d_rollSpeedH;
	
private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static StaticTextProperties::TextColours	d_textColoursProperty;
	static StaticTextProperties::VertFormatting	d_vertFormattingProperty;
	static StaticTextProperties::HorzFormatting	d_horzFormattingProperty;
	static StaticTextProperties::RollSpeedH		d_rollSpeedHProperty;
	

	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addStaticTextProperties(void);
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIStaticText_h_
