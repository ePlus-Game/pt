/************************************************************************
	filename: 	CEGUIRadioButtonProperties.h
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to properties for RadioButton class
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
#ifndef _CEGUIRadioButtonProperties_h_
#define _CEGUIRadioButtonProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of RadioButtonProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the RadioButton class
*/
namespace RadioButtonProperties
{
/*!
\brief
	Property to access the selected state of the RadioButton.

	\par Usage:
		- Name: Selected
		- Format: "[text]".

	\par Where [Text] is:
		- "True" to indicate the radio button is selected.
		- "False" to indicate the radio button is not selected.
*/
class Selected : public Property
{
public:
	Selected() : Property(
		"Selected",
		"Property to get/set the selected state of the RadioButton.  Value is either \"True\" or \"False\".",
		"False")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the radio button group ID.

	\par Usage:
		- Name: GroupID
		- Format: "[uint]".

	\par Where:
		- [uint] is any unsigned integer value.
*/
class GroupID : public Property
{
public:
	GroupID() : Property(
		"GroupID",
		"Property to get/set the radio button group ID.  Value is an unsigned integer number.",
		"0")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access the normal image of the button

   \par Usage:
      - Name: NormalImage
      - Format: "set:<imageset> image:<imagename>".

*/
class NormalImage : public Property
{
public:
   NormalImage() : Property(
	   "NormalImage", 
	   "Property to get/set the normal image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:NormalImage")
   {}

   String   get(const PropertyReceiver* receiver) const;
   void   set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class PushedImage : public Property
{
public:
   PushedImage() : Property(
	   "PushedImage",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:PushedImage")
   {}

   String   get(const PropertyReceiver* receiver) const;
   void   set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access the hover image of the button

   \par Usage:
      - Name: HoverImage
      - Format: "set:<imageset> image:<imagename>".

*/
class HoverImage : public Property
{
public:
   HoverImage() : Property(
	   "HoverImage",
	   "Property to get/set the hover image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:HoverImage")
   {}

   String   get(const PropertyReceiver* receiver) const;
   void   set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access the disabled image of the button

   \par Usage:
      - Name: DisabledImage
      - Format: "set:<imageset> image:<imagename>".

*/
class DisabledImage : public Property
{
public:
   DisabledImage() : Property(
	   "DisabledImage",
	   "Property to get/set the disabled image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:DisabledImage")
   {}

   String   get(const PropertyReceiver* receiver) const;
   void   set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access whether to use the standard images or the supplied custom ones

   \par Usage:
      - Name: UseStandardImagery
      - Format: "True|False".

*/
class UseStandardImagery : public Property
{
public:
   UseStandardImagery() : Property(
	   "UseStandardImagery",
	   "Property to get/set whether to use the standard imagery for the PushButton widget.  Value should be \"True\" or \"False\".",
	   "True")
   {}

   String   get(const PropertyReceiver* receiver) const;
   void   set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to use yoffset

   \par Usage:
      - Name: FontYOffSet
      - Format: "0".

*/
class FontYOffSet : public Property
{
public:
   FontYOffSet() : Property(
	   "FontYOffSet",
	   "Property to get/set yOffset.",
	   "0")
   {}

   String   get(const PropertyReceiver* receiver) const;
   void   set(PropertyReceiver* receiver, const String& value);
};

} // End of  RadioButtonProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIRadioButtonProperties_h_
