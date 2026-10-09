/************************************************************************
	filename: 	CEGUIProgressBarProperties.h
	created:	10/7/2004
	author:		Paul D Turner
	
	purpose:	Interface to properties for the ProgressBar class
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
#ifndef _CEGUIProgressBarProperties_h_
#define _CEGUIProgressBarProperties_h_

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{

// Start of ProgressBarProperties namespace section
/*!
\brief
	Namespace containing all classes that make up the properties interface for the ProgressBar class
*/
namespace ProgressBarProperties
{
/*!
\brief
	Property to access the current progress of the progress bar.

	\par Usage:
		- Name: CurrentProgress
		- Format: "[float]".

	\par Where:
		- [float] is the current progress of the bar expressed as a value between 0 and 1.
*/
class CurrentProgress : public Property
{
public:
	CurrentProgress() : Property(
		"CurrentProgress",
		"Property to get/set the current progress of the progress bar.  Value is a float  value between 0.0 and 1.0 specifying the progress.",
		"0.000000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};


/*!
\brief
	Property to access the step size setting for the progress bar.

	\par Usage:
		- Name: StepSize
		- Format: "[float]".

	\par Where:
		- [float] is the size of the invisible sizing border in screen pixels.
*/
class StepSize : public Property
{
public:
	StepSize() : Property(
		"StepSize",
		"Property to get/set the step size setting for the progress bar.  Value is a float value.",
		"0.010000")
	{}

	String	get(const PropertyReceiver* receiver) const;
	void	set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class ProgressBarLeft : public Property
{
public:
   ProgressBarLeft() : Property(
	   "ProgressBarLeft",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:ProgressBarLeft")
   {}

   void   set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class ProgressBarMiddle : public Property
{
public:
   ProgressBarMiddle() : Property(
	   "ProgressBarMiddle",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:ProgressBarMiddle")
   {}

   void   set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class ProgressBarRight : public Property
{
public:
   ProgressBarRight() : Property(
	   "ProgressBarRight",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:ProgressBarRight")
   {}

   void   set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class ProgressBarDimSegment : public Property
{
public:
   ProgressBarDimSegment() : Property(
	   "ProgressBarDimSegment",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:ProgressBarDimSegment")
   {}

   void   set(PropertyReceiver* receiver, const String& value);
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class ProgressBarLitSegment : public Property
{
public:
   ProgressBarLitSegment() : Property(
	   "ProgressBarLitSegment",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:ProgressBarLitSegment")
   {}

   void   set(PropertyReceiver* receiver, const String& value);
   String	get(const PropertyReceiver* receiver) const;
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class ProgressBarImage : public Property
{
public:
   ProgressBarImage() : Property(
	   "ProgressBarImage",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:ProgressBarImage")
   {}

   void   set(PropertyReceiver* receiver, const String& value);
   String	get(const PropertyReceiver* receiver) const;
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class ProgressBarSegment : public Property
{
public:
   ProgressBarSegment() : Property(
	   "ProgressBarSegment",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:ProgressBarSegment")
   {}

   void   set(PropertyReceiver* receiver, const String& value);
   String	get(const PropertyReceiver* receiver) const;
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class ProgressBarSegmentTop : public Property
{
public:
   ProgressBarSegmentTop() : Property(
	   "ProgressBarSegmentTop",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:ProgressBarSegmentTop")
   {}

   void   set(PropertyReceiver* receiver, const String& value);
   String	get(const PropertyReceiver* receiver) const;
};

/*!
\brief
   Property to access the pushed image of the button

   \par Usage:
      - Name: PushedImage
      - Format: "set:<imageset> image:<imagename>".

*/
class ProgressBarSegmentLeft : public Property
{
public:
   ProgressBarSegmentLeft() : Property(
	   "ProgressBarSegment",
	   "Property to get/set the pushed image for the PushButton widget.  Value should be \"set:[imageset name] image:[image name]\".",
	   "set:TaharezLook image:ProgressBarSegment")
   {}

   void   set(PropertyReceiver* receiver, const String& value);
   String	get(const PropertyReceiver* receiver) const;
};

} // End of  ProgressBarProperties namespace section

} // End of  CEGUI namespace section


#endif	// end of guard _CEGUIProgressBarProperties_h_
