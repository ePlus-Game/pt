/************************************************************************
	filename: 	CEGUIListboxTextItem.h
	created:	12/6/2004
	author:		Paul D Turner
	
	purpose:	Interface for list box text items
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
#ifndef _CEGUIListboxTextItem_h_
#define _CEGUIListboxTextItem_h_
#include "elements/CEGUIListboxItem.h"


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Class used for textual items in a list box.
*/
class CEGUIEXPORT ListboxTextItem : public ListboxItem
{
public:
	/*************************************************************************
		Constants
	*************************************************************************/
	static const colour	DefaultTextColour;			//!< Default text colour.


	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		base class constructor
	*/
	ListboxTextItem(const String& text, uint item_id = 0, void* item_data = NULL, bool disabled = false, bool auto_delete = true);


	/*!
	\brief
		base class destructor
	*/
	virtual ~ListboxTextItem(void) {}


	/*************************************************************************
		Accessor methods
	*************************************************************************/
	/*!
	\brief
		Return a pointer to the font being used by this ListboxTextItem

		This method will try a number of places to find a font to be used.  If no font can be
		found, NULL is returned.

	\return
		Font to be used for rendering this item
	*/
	const Font*	getFont(void) const;


	/*!
	\brief
		Return the current colours used for text rendering.

	\return
		ColourRect object describing the currently set colours
	*/
	ColourRect	getTextColours(void) const		{return d_textCols;}


	/*************************************************************************
		Manipulator methods
	*************************************************************************/
	/*!
	\brief
		Set the font to be used by this ListboxTextItem

	\param font
		Font to be used for rendering this item

	\return
		Nothing
	*/
	void	setFont(const Font* font)		{d_font = font;}


	/*!
	\brief
		Set the font to be used by this ListboxTextItem

	\param font_name
		String object containing the name of the Font to be used for rendering this item

	\return
		Nothing
	*/
	void	setFont(const String& font_name);


	/*!
	\brief
		Set the colours used for text rendering.

	\param cols
		ColourRect object describing the colours to be used.

	\return
		Nothing.
	*/
	void	setTextColours(const colour& cols)			{d_textCols = cols;}





	/*************************************************************************
		Required implementations of pure virtuals from the base class.
	*************************************************************************/
    Size getPixelSize(void) const;
    void draw(KRenderCache* drawPanel, const Point* pos, const Rect* clipper) const;

protected:
	/*************************************************************************
		Implementation Data
	*************************************************************************/
	colour		d_textCols;			//!< Colours used for rendering the text.
	const Font*		d_font;				//!< Font used for rendering text.
};

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIListboxTextItem_h_
