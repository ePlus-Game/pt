/************************************************************************
	filename: 	CEGUIRadioButtonProperties.cpp
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Implements properties for Radio Button class
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
#include "elements/CEGUIRadioButtonProperties.h"
#include "elements/CEGUIRadioButton.h"
#include "CEGUIPropertyHelper.h"

// Start of CEGUI namespace section
namespace CEGUI
{

// Start of RadioButtonProperties namespace section
namespace RadioButtonProperties
{
String Selected::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::boolToString(static_cast<const RadioButton*>(receiver)->isSelected());
}


void Selected::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<RadioButton*>(receiver)->setSelected(PropertyHelper::stringToBool(value));
}


String GroupID::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::uintToString(static_cast<const RadioButton*>(receiver)->getGroupID());
}


void GroupID::set(PropertyReceiver* receiver, const String& value)
{
	static_cast<RadioButton*>(receiver)->setGroupID(PropertyHelper::stringToUint(value));
}

String NormalImage::get(const PropertyReceiver* receiver) const
{
    const RenderableImage* img = static_cast<const RadioButton*>(receiver)->getNormalImage();
    return img ? PropertyHelper::imageToString(img->getImage()) : String("");
}

void NormalImage::set(PropertyReceiver* receiver, const String &value)
{
    RenderableImage image;
    image.setImage(PropertyHelper::stringToImage(value));
    image.setHorzFormatting(RenderableImage::HorzStretched);
    image.setVertFormatting(RenderableImage::VertStretched);
    static_cast<RadioButton*>(receiver)->setNormalImage(&image);
}

String PushedImage::get(const PropertyReceiver* receiver) const
{
    const RenderableImage* img = static_cast<const RadioButton*>(receiver)->getPushedImage();
    return img ? PropertyHelper::imageToString(img->getImage()) : String("");
}

void PushedImage::set(PropertyReceiver* receiver, const String &value)
{
    RenderableImage image;
    image.setImage(PropertyHelper::stringToImage(value));
    image.setHorzFormatting(RenderableImage::HorzStretched);
    image.setVertFormatting(RenderableImage::VertStretched);
    static_cast<RadioButton*>(receiver)->setPushedImage(&image);
}

String HoverImage::get(const PropertyReceiver* receiver) const
{
    const RenderableImage* img = static_cast<const RadioButton*>(receiver)->getHoverImage();
    return img ? PropertyHelper::imageToString(img->getImage()) : String("");
}

void HoverImage::set(PropertyReceiver* receiver, const String &value)
{
    RenderableImage image;
    image.setImage(PropertyHelper::stringToImage(value));
    image.setHorzFormatting(RenderableImage::HorzStretched);
    image.setVertFormatting(RenderableImage::VertStretched);
    static_cast<RadioButton*>(receiver)->setHoverImage(&image);
}

String DisabledImage::get(const PropertyReceiver* receiver) const
{
    const RenderableImage* img = static_cast<const RadioButton*>(receiver)->getDisabledImage();
    return img ? PropertyHelper::imageToString(img->getImage()) : String("");
}

void DisabledImage::set(PropertyReceiver* receiver, const String &value)
{
    RenderableImage image;
    image.setImage(PropertyHelper::stringToImage(value));
    image.setHorzFormatting(RenderableImage::HorzStretched);
    image.setVertFormatting(RenderableImage::VertStretched);
    static_cast<RadioButton*>(receiver)->setDisabledImage(&image);
}

String UseStandardImagery::get(const PropertyReceiver* receiver) const
{
    return PropertyHelper::boolToString(static_cast<const RadioButton*>(receiver)->isStandardImageryEnabled());
}

void UseStandardImagery::set(PropertyReceiver* receiver, const String &value)
{
    static_cast<RadioButton*>(receiver)->setStandardImageryEnabled(PropertyHelper::stringToBool(value));
}

String   FontYOffSet::get(const PropertyReceiver* receiver) const
{
	return PropertyHelper::intToString(static_cast<const RadioButton*>(receiver)->getYoffset());
}

void   FontYOffSet::set(PropertyReceiver* receiver, const String& value)
{
    static_cast<RadioButton*>(receiver)->setYOffSet(PropertyHelper::stringToInt(value));
}


} // End of  RadioButtonProperties namespace section

} // End of  CEGUI namespace section
