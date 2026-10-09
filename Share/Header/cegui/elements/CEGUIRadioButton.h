/************************************************************************
	filename: 	CEGUIRadioButton.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for RadioButton widget
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
#ifndef _CEGUIRadioButton_h_
#define _CEGUIRadioButton_h_

#include "CEGUIBase.h"
#include "elements/CEGUIButtonBase.h"
#include "elements/CEGUIRadioButtonProperties.h"
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
	Base class to provide the logic for Radio Button widgets.

*/
class CEGUIEXPORT RadioButton : public ButtonBase
{
public:


	/*************************************************************************
		Accessor Functions
	*************************************************************************/ 

	/*!
	\brief
		return whether or not rendering of the standard imagery is enabled.

	\return
		true if the standard button imagery will be rendered, false if no standard rendering will be performed.
	*/
	bool	isStandardImageryEnabled(void) const;


    /*!
	\brief
		returns a pointer to a read-only RenderableImage object holding the details
        of the image set to render for the button in the normal state, or 0 if no
        image is currently set for this state.

	\return
		Pointer to a const RenderableImage object with all the details for the image.  
	*/
	const RenderableImage*	getNormalImage(void) const;

	/*!
	\brief
        returns a pointer to a read-only RenderableImage object holding the details
        of the image set to render for the button in the highlighted state, or 0 if no
        image is currently set for this state.

	\return
		Pointer to a const RenderableImage object with all the details for the image.  
	*/
	const RenderableImage*	getHoverImage(void) const;

	/*!
	\brief
        returns a pointer to a read-only RenderableImage object holding the details
        of the image set to render for the button in the pushed state, or 0 if no
        image is currently set for this state.

	\return
		Pointer to a const RenderableImage object with all the details for the image.  
	*/
	const RenderableImage*	getPushedImage(void) const;

	/*!
	\brief
        returns a pointer to a read-only RenderableImage object holding the details
        of the image set to render for the button in the disabled state, or 0 if no
        image is currently set for this state.

	\return
		Pointer to a const RenderableImage object with all the details for the image.  
	*/
	const RenderableImage*	getDisabledImage(void) const;

	/*!
	\brief
		return true if the radio button is selected (has the checkmark)

	\return
		true if this widget is selected, false if the widget is not selected.
	*/
	bool	isSelected(void) const				{return d_selected;}

	
	/*!
	\brief
		return the groupID assigned to this radio button

	\return
		ulong value that identifies the Radio Button group this widget belongs to.
	*/
	ulong	getGroupID(void) const				{return d_groupID;}


	/*!
	\brief
		Return a pointer to the RadioButton object within the same group as this RadioButton, that
		is currently selected.

	\return
		Pointer to the RadioButton object that is the RadioButton within the same group as this RadioButton,
		and is attached to the same parent window as this RadioButton, that is currently selected.
		Returns NULL if no button within the group is selected, or if 'this' is not attached to a parent window.
	*/
	RadioButton*	getSelectedButtonInGroup(void) const;

	/*!
	\brief
		Return yOffset

	\return
	*/
	int	getYoffset(void) const
	{
		return d_yOffset;
	}

	/*************************************************************************
		Manipulator Functions
	*************************************************************************/
	/*!
	\brief
		set whether the radio button is selected or not

	\param select
		true to put the radio button in the selected state, false to put the radio button in the
		deselected state.  If changing to the selected state, any previously selected radio button
		within the same group is automatically deselected.

	\return
		Nothing.
	*/
	void	setSelected(bool select);

	
	/*!
	\brief
		set the groupID for this radio button

	\param group
		ulong value specifying the radio button group that this widget belongs to.

	\return	
		Nothing.
	*/
	void	setGroupID(ulong group);

	/*!
	\brief
		set the details of the image to render for the button in the normal state.

	\param image
		RenderableImage object with all the details for the image.  Note that an internal copy of the Renderable image is made and
		ownership of \a image remains with client code.  If this parameter is NULL, rendering of an image for this button state is
		disabled.

	\return
		Nothing.
	*/
	void	setNormalImage(const RenderableImage* image);

	/*!
	\brief
		set the details of the image to render for the button in the highlighted state.

	\param image
		RenderableImage object with all the details for the image.  Note that an internal copy of the Renderable image is made and
		ownership of \a image remains with client code.  If this parameter is NULL, rendering of an image for this button state is
		disabled.

	\return
		Nothing.
	*/
	void	setHoverImage(const RenderableImage* image);

	/*!
	\brief
		set the details of the image to render for the button in the pushed state.

	\param image
		RenderableImage object with all the details for the image.  Note that an internal copy of the Renderable image is made and
		ownership of \a image remains with client code.  If this parameter is NULL, rendering of an image for this button state is
		disabled.

	\return
		Nothing.
	*/
	void	setPushedImage(const RenderableImage* image);

	/*!
	\brief
		set the details of the image to render for the button in the disabled state.

	\param image
		RenderableImage object with all the details for the image.  Note that an internal copy of the Renderable image is made and
		ownership of \a image remains with client code.  If this parameter is NULL, rendering of an image for this button state is
		disabled.

	\return
		Nothing.
	*/
	void	setDisabledImage(const RenderableImage* image);

	/*!
	\brief
		set whether or not to render the standard imagery for the button

	\param setting
		true to have the standard button imagery drawn, false to have no standard imagery drawn.

	\return
		Nothing.
	*/
	void	setStandardImageryEnabled(bool setting);


	/*!
	\brief
		set yOffset

	\param iYoffset

	\return
		Nothing.
	*/
	void	setYOffSet(int iYoffset)
	{
		d_yOffset = iYoffset;
	}

	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	RadioButton(const String& type, const String& name);
	virtual ~RadioButton(void);


protected:
	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	/*!
	\brief
		Add radio button specific events
	*/
	void	addRadioButtonEvents(void);


	/*!
	\brief
		Deselect any selected radio buttons attached to the same parent within the same group
		(but not do not deselect 'this').
	*/
	void	deselectOtherButtonsInGroup(void) const;


	/*!
	\brief
		Return whether this window was inherited from the given class name at some point in the inheritance heirarchy.

	\param class_name
		The class name that is to be checked.

	\return
		true if this window was inherited from \a class_name. false if not.
	*/
	virtual bool	testClassName_impl(const String& class_name) const
	{
		if (class_name==(const utf8*)"RadioButton")	return true;
		return ButtonBase::testClassName_impl(class_name);
	}


	/*************************************************************************
		New Radio Button Events
	*************************************************************************/
	/*!
	\brief
		event triggered internally when the select state of the button changes.
	*/
	virtual void	onSelectStateChanged(WindowEventArgs& e);


	/*************************************************************************
		Overridden Event handlers
	*************************************************************************/
	virtual void	onMouseButtonUp(MouseEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	bool		d_selected;				// true when radio button is selected (has checkmark)
	long		d_groupID;				// radio button group ID
    bool	d_useStandardImagery;			//!< true if button standard imagery should be drawn.
    bool	d_useNormalImage;				//!< true if an image should be drawn for the normal state.
    bool	d_useHoverImage;				//!< true if an image should be drawn for the highlighted state.
    bool	d_usePushedImage;				//!< true if an image should be drawn for the pushed state.
    bool	d_useDisabledImage;				//!< true if an image should be drawn for the disabled state.

    RenderableImage		d_normalImage;		//!< RenderableImage used when rendering an image in the normal state.
    RenderableImage		d_hoverImage;		//!< RenderableImage used when rendering an image in the highlighted state.
    RenderableImage		d_pushedImage;		//!< RenderableImage used when rendering an image in the pushed state.
    RenderableImage		d_disabledImage;	//!< RenderableImage used when rendering an image in the disabled state.

	int		d_yOffset;
private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static RadioButtonProperties::Selected	d_selectedProperty;
	static RadioButtonProperties::GroupID	d_groupIDProperty;
    static RadioButtonProperties::NormalImage    d_normalImageProperty;
    static RadioButtonProperties::PushedImage    d_pushedImageProperty;
    static RadioButtonProperties::HoverImage     d_hoverImageProperty;
    static RadioButtonProperties::DisabledImage  d_disabledImageProperty;
    static RadioButtonProperties::UseStandardImagery d_useStandardImageryProperty;
	static RadioButtonProperties::FontYOffSet	 d_fontYoffsetProperty;

	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addRadioButtonProperties(void);
};


} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIRadioButton_h_
