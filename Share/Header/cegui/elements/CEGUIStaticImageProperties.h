/************************************************************************
	filename: 	CEGUIStaticImageProperties.h
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Interface for StaticImage property classes
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
#ifndef _CEGUIStaticImageProperties_h_
#define _CEGUIStaticImageProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of StaticImageProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the StaticImage class
*/
namespace StaticImageProperties
{
/*!
\brief
	Property to access the image for the StaticImage widget.

	\par Usage:
		- Name: Image
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class Image : public Property
{
public:
	Image() : Property(
		"Image",
		"Property to get/set the image for the StaticImage widget.  Value should be \"set:[imageset name] image:[image name]\".",
		"set:TaharezLook image:Image")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
	Property to access the image for the StaticImage widget.

	\par Usage:
		- Name: Image
		- Format: "set:[text] image:[text]".

	\par Where:
		- set:[text] is the name of the Imageset containing the image.  The Imageset name should not contain spaces.  The Imageset specified must already be loaded.
		- image:[text] is the name of the Image on the specified Imageset.  The Image name should not contain spaces.
*/
class ImageEx : public Property
{
public:
	ImageEx() : Property(
		"ImageEx",
		"Property to get/set the image for the StaticImage widget. ",
		"",false)
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the setting for whether the user may drag the window around by its title bar.

	\par Usage:
		- Name: DragMovingEnabled
		- Format: "[text]".

	\par Where [Text] is:
		- "True" to indicate the window may be repositioned by the user via dragging.
		- "False" to indicate the window may not be repositioned by the user.
*/
class DragMovingEnabled : public Property
{
public:
	DragMovingEnabled() : Property(
		"DragMovingEnabled",
		"Property to get/set the setting for whether the user may drag the window around by its title bar.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the setting for whether the user have help  plane .

	\par Usage:
		- Name: HelpPlaneName
		- Format: String.

	\par Where [Text] is:
*/
class HelpPlaneName : public Property
{
public:
	HelpPlaneName() : Property(
		"HelpPlaneName",
		"Property to get/set the setting for whether the user have help plane",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
	Property to access the setting for whether the user have help ToolTip .

	\par Usage:
		- Name: HelpToolTip
		- Format: String.

	\par Where [Text] is:
*/
class HelpToolTip : public Property
{
public:
	HelpToolTip() : Property(
		"HelpToolTip",
		"Property to get/set the setting for whether the user have help ToolTip",
		"")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

} // End of  StaticImageProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIStaticImageProperties_h_
