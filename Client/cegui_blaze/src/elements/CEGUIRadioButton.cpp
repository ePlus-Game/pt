/************************************************************************
	filename: 	CEGUIRadioButton.cpp
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Implementation of RadioButton widget base class
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
#include "elements/CEGUIRadioButton.h"

// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Definitions of Properties for this class
*************************************************************************/
RadioButtonProperties::Selected				RadioButton::d_selectedProperty;
RadioButtonProperties::GroupID				RadioButton::d_groupIDProperty;
RadioButtonProperties::NormalImage			RadioButton::d_normalImageProperty;
RadioButtonProperties::PushedImage			RadioButton::d_pushedImageProperty;
RadioButtonProperties::HoverImage			RadioButton::d_hoverImageProperty;
RadioButtonProperties::DisabledImage		RadioButton::d_disabledImageProperty;
RadioButtonProperties::UseStandardImagery	RadioButton::d_useStandardImageryProperty;
RadioButtonProperties::FontYOffSet			RadioButton::d_fontYoffsetProperty;



/*************************************************************************
	Constructor
*************************************************************************/
RadioButton::RadioButton(const String& type, const String& name) :
	ButtonBase(type, name),
	d_selected(false),
	d_groupID(-1),
	d_yOffset(0)
{

    d_useStandardImagery	= true;
    d_useNormalImage		= false;
    d_useHoverImage			= false;
    d_usePushedImage		= false;
    d_useDisabledImage		= false;
	// add radio button specific events.
	addRadioButtonEvents();

	addRadioButtonProperties();
    d_useNormalImage = true;
}


/*************************************************************************
	Destructor
*************************************************************************/
RadioButton::~RadioButton(void)
{
}

bool RadioButton::isStandardImageryEnabled(void) const
{
    return d_useStandardImagery;
}

const RenderableImage* RadioButton::getNormalImage(void) const
{
    return d_useNormalImage ? &d_normalImage : static_cast<const RenderableImage*>(0);
}

const RenderableImage* RadioButton::getHoverImage(void) const
{
    return d_useHoverImage ? &d_hoverImage : static_cast<const RenderableImage*>(0);
}

const RenderableImage* RadioButton::getPushedImage(void) const
{
    return d_usePushedImage ? &d_pushedImage : static_cast<const RenderableImage*>(0);
}

const RenderableImage* RadioButton::getDisabledImage(void) const
{
    return d_useDisabledImage ? &d_disabledImage : static_cast<const RenderableImage*>(0);
}



/*************************************************************************
	set whether the radio button is selected or not	
*************************************************************************/
void RadioButton::setSelected(bool select)
{
	if (select != d_selected)
	{
		d_selected = select;

		requestRedraw();

		// if new state is 'selected', we must de-select any selected radio buttons within our group.
		if (d_selected)
		{
			deselectOtherButtonsInGroup();
		}

		WindowEventArgs args(this);
		onSelectStateChanged(args);
	}

}


/*************************************************************************
	set the groupID for this radio button	
*************************************************************************/
void RadioButton::setGroupID(ulong group)
{
	d_groupID = group;

	if (d_selected)
	{
		deselectOtherButtonsInGroup();
	}

}

void RadioButton::setNormalImage(const RenderableImage* image)
{
    if (image)
    {
        d_useNormalImage = true;
        d_normalImage = *image;
        d_normalImage.setRect(Rect(0, 0, getAbsoluteWidth(), getAbsoluteHeight()));
    }
    else
    {
        d_useNormalImage = false;
    }

    requestRedraw();
}

void RadioButton::setHoverImage(const RenderableImage* image)
{
    if (image)
    {
        d_useHoverImage = true;
        d_hoverImage = *image;
        d_hoverImage.setRect(Rect(0, 0, getAbsoluteWidth(), getAbsoluteHeight()));
    }
    else
    {
        d_useHoverImage = false;
    }

    requestRedraw();
}

void RadioButton::setPushedImage(const RenderableImage* image)
{
    if (image)
    {
        d_usePushedImage = true;
        d_pushedImage = *image;
        d_pushedImage.setRect(Rect(0, 0, getAbsoluteWidth(), getAbsoluteHeight()));
    }
    else
    {
        d_usePushedImage = false;
    }

    requestRedraw();
}

void RadioButton::setDisabledImage(const RenderableImage* image)
{
    if (image)
    {
        d_useDisabledImage = true;
        d_disabledImage = *image;
        d_disabledImage.setRect(Rect(0, 0, getAbsoluteWidth(), getAbsoluteHeight()));
    }
    else
    {
        d_useDisabledImage = false;
    }

    requestRedraw();
}

void RadioButton::setStandardImageryEnabled(bool setting)
{
    if (d_useStandardImagery != setting)
    {
        d_useStandardImagery = setting;
        requestRedraw();
    }
}


/*************************************************************************
	Add radio button specific events	
*************************************************************************/
void RadioButton::addRadioButtonEvents(void)
{
	addEvent(EventSelectStateChanged);
}


/*************************************************************************
	Deselect any selected radio buttons attached to the same parent
	within the same group (but not do not deselect 'this').
*************************************************************************/
void RadioButton::deselectOtherButtonsInGroup(void) const
{
	// nothing to do unless we are attached to another window.
	if (d_parent != NULL)
	{
		int child_count = d_parent->getChildCount();

		// scan all children
		for (int child = 0; child < child_count; ++child)
		{
			// is this child same type as we are?
			if (d_parent->getChildAtIdx(child)->getType() == getType())
			{
				RadioButton* rb = (RadioButton*)d_parent->getChildAtIdx(child);

				// is child same group, selected, but not 'this'?
				if (rb->isSelected() && (rb != this) && (rb->getGroupID() == d_groupID))
				{
					// deselect the radio button.
					rb->setSelected(false);
				}

			}

		}

	}

}


/*************************************************************************
	event triggered internally when the select state of the button changes.
*************************************************************************/
void RadioButton::onSelectStateChanged(WindowEventArgs& e)
{
	fireEvent(EventSelectStateChanged, e);
}


/*************************************************************************
	Handler called when mouse button gets released
*************************************************************************/
void RadioButton::onMouseButtonUp(MouseEventArgs& e)
{
	if ((e.button == LeftButton) && isPushed())
	{
		Window* sheet = getRoot();

		if (sheet != NULL)
		{
			// if mouse was released over this widget
			if (this == sheet->getChildAtPosition(e.position))
			{
				// select this button & deselect all others in the same group.
				if ( d_groupID == -1)
				{
					setSelected( !d_selected );
				}
				else
				{
					setSelected(true);
				}
			}

		}

		e.handled = true;
	}

	// default handling
	ButtonBase::onMouseButtonUp(e);
}


/*************************************************************************
	Return a pointer to the RadioButton object within the same group as
	this RadioButton, that is currently selected.
*************************************************************************/
RadioButton* RadioButton::getSelectedButtonInGroup(void) const
{
	// Only search we we are a child window
	if (d_parent != NULL)
	{
		int child_count = d_parent->getChildCount();

		// scan all children
		for (int child = 0; child < child_count; ++child)
		{
			// is this child same type as we are?
			if (d_parent->getChildAtIdx(child)->getType() == getType())
			{
				RadioButton* rb = (RadioButton*)d_parent->getChildAtIdx(child);

				// is child same group and selected?
				if (rb->isSelected() && (rb->getGroupID() == d_groupID))
				{
					// return the matching RadioButton pointer (may even be 'this').
					return rb;
				}

			}

		}

	}

	// no selected button attached to this window is in same group
	return NULL;
}

/*************************************************************************
	Add properties for radio button
*************************************************************************/
void RadioButton::addRadioButtonProperties(void)
{
	addProperty(&d_selectedProperty);
	addProperty(&d_groupIDProperty);
	addProperty(&d_useStandardImageryProperty);
	addProperty(&d_normalImageProperty);
	addProperty(&d_pushedImageProperty);
	addProperty(&d_hoverImageProperty);
	addProperty(&d_disabledImageProperty);
	addProperty(&d_fontYoffsetProperty);
}


} // End of  CEGUI namespace section
