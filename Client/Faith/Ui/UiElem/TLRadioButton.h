/************************************************************************
	filename: 	TLRadioButton.h
	created:	21/5/2004
	author:		Paul D Turner
	
	purpose:	Interface to Taharez look Radio Button widget.
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
#ifndef _TLRadioButton_h_
#define _TLRadioButton_h_

#include "TLModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIRadioButton.h"

namespace TLRadioButtonProperties
{
	using namespace CEGUI;
	class TLRadioButtonSelectedTextColour : public Property
	{
	public:
		TLRadioButtonSelectedTextColour() 
			: Property(
			"SelectedTextColour",
			"Property to get/set the colour to use when rendering label text for selected state.  Value is \"aarrggbb\" (hex).",
			"FFFFFFFF")
		{}

		String	get(const PropertyReceiver* receiver) const;
		void	set(PropertyReceiver* receiver, const String& value);
	};
}

// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Radio Button class for the TaharezLook GUI scheme
*/
class TAHAREZLOOK_API TLRadioButton : public RadioButton
{
public:
	/*************************************************************************
		Constants
	*************************************************************************/
	// type name for this widget
	static const utf8	WidgetTypeName[];				//!< The unique typename of this widget

	static const utf8	ImagesetName[];				//!< Name of the imageset to use for rendering.
	static const utf8	NormalImageName[];			//!< Name of the image to use for the normal state.
	static const utf8	HighlightImageName[];		//!< Name of the image to use for the highlighted state.
	static const utf8	SelectMarkImageName[];		//!< Name of the image to use for the check / selected mark.

	static const float	LabelPadding;				//!< Pixel padding value for text label (space between image and text label).
	static const colour	DefaultSelectedLabelColour;	//!< Default colour used when rendering label text in selected state.

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for Taharez Look Radio Button objects.

	\param type
		String object that specifies a type for this window, usually provided by a factory class.

	\param name
		String object that specifies a unique name that will be used to identify the new Window object
	*/
	TLRadioButton(const String& type, const String& name);


	/*!
	\brief
		Destructor for TLRadioButton objects.
	*/
	virtual ~TLRadioButton(void);

	const colour getSelectedTextColour(void) const;
	void	setSelectedTextColour(const colour& colour);
	
protected:
	/*************************************************************************
		Implementation Rendering Functions
	*************************************************************************/
	virtual	void	drawSelf(KRenderCache* panelCache, Point* panelAbsPos);

	virtual void	drawNormal(KRenderCache* panelCache, Point* panelAbsPos);

	virtual void	drawHover(KRenderCache* panelCache, Point* panelAbsPos);

	virtual void	drawPushed(KRenderCache* panelCache, Point* panelAbsPos);

	virtual void	drawDisabled(KRenderCache* panelCache, Point* panelAbsPos);

		
	virtual void	drawNormal();

	virtual void	drawHover();

	virtual void	drawPushed();

	virtual void	drawDisabled();
	/*************************************************************************
		Overridden Event Handling Functions
	*************************************************************************/
	virtual void	onCharacter(KeyEventArgs& e);

	virtual	void	onMouseEnters(MouseEventArgs& e);
	virtual void	onMouseLeaves(MouseEventArgs& e);
	
private:
	static	TLRadioButtonProperties::TLRadioButtonSelectedTextColour	d_selectedTextColour;
	void	addTLRadioButtonProperties();

private:
	// standard button rendering images
	const Image*	d_normalSectionImage;			//!< Image to use when rendering the button left section (normal state).
	const Image*	d_hoverSectionImage;			//!< Image to use when rendering the button left section (hover state).
	const Image*	d_selectMarkSectionImage;		//!< Image to use when rendering the button left section (pushed state).

	colour	d_selectedColour;						//!< Colour used for label text when rendering in selected state
};


/*!
\brief
	Factory class for producing TLRadioButton objects
*/
class TAHAREZLOOK_API TLRadioButtonFactory : public WindowFactory
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	TLRadioButtonFactory(void) : WindowFactory(TLRadioButton::WidgetTypeName) { }
	~TLRadioButtonFactory(void){}


	/*!
	\brief
		Create a new Window object of whatever type this WindowFactory produces.

	\param name
		A unique name that is to be assigned to the newly created Window object

	\return
		Pointer to the new Window object.
	*/
	Window*	createWindow(const String& name);


	/*!
	\brief
		Destroys the given Window object.

	\param window
		Pointer to the Window object to be destroyed.

	\return
		Nothing.
	*/
	virtual void	destroyWindow(Window* window)	 { if (window->getType() == d_type) delete window; }
};


} // End of  CEGUI namespace section


#endif	// end of guard _TLRadioButton_h_
