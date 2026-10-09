/************************************************************************
	filename: 	CEGUIPushButton.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for PushButton widget
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
#ifndef _CEGUIPushButton_h_
#define _CEGUIPushButton_h_

#include "CEGUIBase.h"
#include "elements/CEGUIButtonBase.h"
#include "elements/CEGUIPushButtonProperties.h"
#include "CEGUIRenderableImage.h"

#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
    Base class to provide logic for push button type widgets.
*/
class CEGUIEXPORT PushButton : public ButtonBase
{
public:

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	PushButton(const String& type, const String& name);

	virtual ~PushButton(void);

    /*************************************************************************
    	Common Public Interface
    *************************************************************************/

	const Image*	getNormalImage(void) const;

	const Image*	getHoverImage(void) const;

	const Image*	getPushedImage(void) const;

	const Image*	getDisabledImage(void) const;

    float			getTextXOffset(void) const;

	virtual void	setNormalImage(const Image* image);

	virtual void	setHoverImage(const Image* image);

	virtual void	setPushedImage(const Image* image);

	void			setDisabledImage(const Image* image);

    void			setTextXOffset(float offset);

protected:
	/*************************************************************************
		New Event Handlers
	*************************************************************************/

	virtual void	onClicked(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event Handlers
	*************************************************************************/
	virtual void	onMouseButtonUp(MouseEventArgs& e);
    virtual void    onSized(WindowEventArgs& e);


	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	void	addPushButtonEvents(void);

	virtual bool	testClassName_impl(const String& class_name) const
	{
		if (class_name==(const utf8*)"PushButton")	return true;
		return ButtonBase::testClassName_impl(class_name);
	}


    /*************************************************************************
    	Data Fields
    *************************************************************************/
	
    // button RenderableImage objects
	const Image*		d_normalImage;		//!< RenderableImage used when rendering an image in the normal state.
	const Image*		d_hoverImage;		//!< RenderableImage used when rendering an image in the highlighted state.
	const Image*		d_pushedImage;		//!< RenderableImage used when rendering an image in the pushed state.
	const Image*		d_disabledImage;	//!< RenderableImage used when rendering an image in the disabled state.
	
    float				d_textXOffset;		//!< offset applied to the x co-ordinate of the text label.

private:
    /*************************************************************************
        Static Properties for this class
    *************************************************************************/
    static PushButtonProperties::NormalImage    d_normalImageProperty;
    static PushButtonProperties::PushedImage    d_pushedImageProperty;
    static PushButtonProperties::HoverImage     d_hoverImageProperty;
    static PushButtonProperties::DisabledImage  d_disabledImageProperty;
    static PushButtonProperties::TextXOffset    d_textXOffsetProperty;

    /*************************************************************************
        Private methods
    *************************************************************************/
    void	addPushButtonProperties(void);
};


} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIPushButton_h_
