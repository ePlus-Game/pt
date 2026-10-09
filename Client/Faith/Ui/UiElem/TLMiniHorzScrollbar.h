/************************************************************************
	filename: 	TLMiniHorzScrollbar.h
	created:	2/6/2004
	author:		Paul D Turner
	
	purpose:	Interface to Taharez mini horizontal scroll bar.
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
#ifndef _TLMiniHorzScrollbar_h_
#define _TLMiniHorzScrollbar_h_

#include "TLModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIScrollbar.h"

//李鲲2007-3-21加设置图片属性
namespace TLMiniHorzScrollbarProperties
{
	using namespace CEGUI;

	/*!
	\brief
		Property to set MiniHorzScrollBody body image.

		This property offers set MiniHorzScrollBody  body image.

		\par Usage:
			- Name: MiniHorzScrollBody
			- Format: "[text]".

		\par Where text is:
			- the path of body image.
	*/
	class MiniHorzScrollBody : public Property
	{
	public:
		MiniHorzScrollBody()
			:Property( "MiniHorzScrollBody", "set HorzScrollbody", "set:TaharezLook image:MiniHorzScrollBarSegment")
		{}
		
		String	get( const PropertyReceiver *receiver ) const;
		void	set( PropertyReceiver *receiver, const String &Value );		
	};

	/*!
	\brief
		Property to set MiniHorzScrollBody LeftNormalButton image.

		This property offers set MiniHorzScrollBody  LeftNormalButton image.

		\par Usage:
			- Name: MiniHorzLeftNormal
			- Format: "[text]".

		\par Where text is:
			- the path of LeftNormalButton image.
	*/
	class MiniHorzLeftNormal : public Property
	{
	public:
		MiniHorzLeftNormal()
			:Property( "MiniHorzLeftNormal", "set LeftNormal", "set:TaharezLook image:MiniHorzScrollLeftNormal" )
		{}

		String	get( const PropertyReceiver *receiver ) const;
		void	set( PropertyReceiver *receiver, const String &Value );		
	};

	/*!
	\brief
		Property to set MiniHorzScrollBody LeftPushedButton image.

		This property offers set MiniHorzScrollBody  LeftPushedButton image.

		\par Usage:
			- Name: MiniHorzLeftPushed
			- Format: "[text]".

		\par Where text is:
			- the path of LeftPushedButton image.
	*/
	class MiniHorzLeftPushed : public Property
	{
	public:
		MiniHorzLeftPushed()
			:Property( "MiniHorzLeftPushed", "set LeftPushed", "set:TaharezLook image:MiniHorzScrollLeftNormal" )
		{}

		String	get( const PropertyReceiver *receiver ) const;
		void	set( PropertyReceiver *receiver, const String &Value );		
	};

	/*!
	\brief
		Property to set MiniHorzScrollBody LeftHoverButton image.

		This property offers set MiniHorzScrollBody  LeftHoverButton image.

		\par Usage:
			- Name: MiniHorzLeftHover
			- Format: "[text]".

		\par Where text is:
			- the path of LeftHoverButton image.
	*/
	class MiniHorzLeftHover : public Property
	{
	public:
		MiniHorzLeftHover()
			:Property( "MiniHorzLeftHover", "set LeftHover", "set:TaharezLook image:MiniHorzScrollLeftHover" )
		{}

		String	get( const PropertyReceiver *receiver ) const;
		void	set( PropertyReceiver *receiver, const String &Value );		
	};


	/*!
	\brief
		Property to set MiniHorzScrollBody RightNormalButton image.

		This property offers set MiniHorzScrollBody  RightNormalButton image.

		\par Usage:
			- Name: MiniHorzRightNormal
			- Format: "[text]".

		\par Where text is:
			- the path of RightNormalButton image.
	*/
	class MiniHorzRightNormal : public Property
	{
	public:
		MiniHorzRightNormal()
			:Property( "MiniHorzRightNormal", "set RightNormal", "set:TaharezLook image:MiniHorzScrollRightNormal" )
		{}

		String	get( const PropertyReceiver *receiver ) const;
		void	set( PropertyReceiver *receiver, const String &Value );		
	};

	/*!
	\brief
		Property to set MiniHorzScrollBody RightPushedButton image.

		This property offers set MiniHorzScrollBody  RightPushedButton image.

		\par Usage:
			- Name: MiniHorzRightPushed
			- Format: "[text]".

		\par Where text is:
			- the path of RightPushedButton image.
	*/
	class MiniHorzRightPushed : public Property
	{
	public:
		MiniHorzRightPushed()
			:Property( "MiniHorzRightPushed", "set RightPushed", "set:TaharezLook image:MiniHorzScrollRightNormal")
		{}

		String	get( const PropertyReceiver *receiver ) const;
		void	set( PropertyReceiver *receiver, const String &Value );		
	};
	

	/*!
	\brief
		Property to set MiniHorzScrollBody RightHoverButton image.

		This property offers set MiniHorzScrollBody  RightHoverButton image.

		\par Usage:
			- Name: MiniHorzRightHover
			- Format: "[text]".

		\par Where text is:
			- the path of RightHoverButton image.
	*/
	class MiniHorzRightHover : public Property
	{
	public:
		MiniHorzRightHover()
			:Property( "MiniHorzRightHover", "set RightHover","set:TaharezLook image:MiniHorzScrollRightHover")
		{}

		String	get( const PropertyReceiver *receiver ) const;
		void	set( PropertyReceiver *receiver, const String &Value );		
	};

	/*!
	\brief
		Property to set MiniHorzScrollBody MiniHorzScrollThumbNormal image.

		This property offers set MiniHorzScrollBody  MiniHorzScrollThumbNormal image.

		\par Usage:
			- Name: MiniHorzScrollThumbNormal
			- Format: "[text]".

		\par Where text is:
			- the path of MiniHorzScrollThumbNormal image.
	*/
	class MiniHorzScrollThumbNormal : public Property
	{
	public:
		MiniHorzScrollThumbNormal()
			:Property( "MiniHorzScrollThumbNormal", "set Normal MiniHorzScrollThumb", "set:TaharezLook image:MiniHorzScrollThumbNormal" )
		{}
		
		String	get( const PropertyReceiver *receiver ) const;
		void	set( PropertyReceiver *receiver, const String &Value );
	};


	/*!
	\brief
		Property to set MiniHorzScrollBody MiniHorzScrollThumbHover image.

		This property offers set MiniHorzScrollBody  MiniHorzScrollThumbHover image.

		\par Usage:
			- Name: MiniHorzScrollThumbHover
			- Format: "[text]".

		\par Where text is:
			- the path of MiniHorzScrollThumbHover image.
	*/
	class MiniHorzScrollThumbHover :public Property
	{
	public:
		MiniHorzScrollThumbHover()
			:Property( "MiniHorzScrollThumbHover", "set Hover MiniHorzScrollThumb", "set:TaharezLook image:MiniHorzScrollThumbNormal")
		{}

		String	get( const PropertyReceiver *receiver ) const;
		void	set( PropertyReceiver *receiver, const String &Value );
	};
}


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Mini horizontal scroll-bar bar for the Taharez Gui Scheme.
*/
class TAHAREZLOOK_API TLMiniHorzScrollbar: public Scrollbar
{
public:
	/*************************************************************************
		Constants
	*************************************************************************/
	// type name for this widget
	static const utf8	WidgetTypeName[];				//!< The unique typename of this widget

	// Progress bar image names
	static const utf8	ImagesetName[];					//!< Name of the imageset to use for rendering.
	static const utf8	ScrollbarBodyImageName[];		//!< Name of image to use for the main body of the scroll bar
	static const utf8	LeftButtonNormalImageName[];	//!< Name of image to use for the left button in normal state.
	static const utf8	LeftButtonPushedImageName[];	//!< Name of image to use for the left button in pushed state.
	static const utf8	LeftButtonHighlightImageName[];	//!< Name of image to use for the left button in highlighted state.
	static const utf8	RightButtonNormalImageName[];	//!< Name of image to use for the right button in normal state.
	static const utf8	RightButtonPushedImageName[];	//!< Name of image to use for the right button in pushed state.
	static const utf8	RightButtonHighlightImageName[];//!< Name of image to use for the right button in the highlighted state.
	// some layout stuff
	static const float	ThumbPositionY;			//!< Relative Y co-ordinate for the thumb.
	static const float	ThumbHeight;			//!< Relative height of the thumb.
	static const float	BodyPositionY;			//!< Relative Y co-ordinate for the body imagery.
	static const float	BodyHeight;				//!< Relative height for the body imagery.

	// type names for the component widgets
	static const utf8*	ThumbWidgetType;			//!< Type of widget to create for the scroll bar thumb;
	static const utf8*	IncreaseButtonWidgetType;	//!< Type of widget to create for the increase button (down arrow).
	static const utf8*	DecreaseButtonWidgetType;	//!< Type of widget to create for the decrease button (up arrow).
	

	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for Taharez mini horizontal scroll bar widgets
	*/
	TLMiniHorzScrollbar(const String& type, const String& name);


	/*!
	\brief
		Destructor for Taharez mini horizontal scroll bar widgets
	*/
	virtual ~TLMiniHorzScrollbar(void);

	/*************************************************************************
		set picture to MiniHorzScroll
	*************************************************************************/
	//set or get body image 
	const Image *getScrollBodyImage( void ) const;
	void  setScrollBodyImage( const Image *pNewImage );

	//set or get thumb normal image
	const Image *getScrollThumbNormalImage( void ) const;
	void  setScrollThumbNormalImage( const Image *pNewImage );

	//set or get thumb hover image
	const Image *getScrollThumbHoverImage( void ) const;
	void  setScrollThumbHoverImage( const Image *pNewImage );

	//set or get left button normal image
	const Image *getScrollLeftNormalImage( void ) const;
	void  setScrollLeftNormalImage( const Image *pNewImage );

	//set or get left button pushed image
	const Image *getScrollLeftPushedImage( void ) const;
	void  setScrollLeftPushedImage( const Image *pNewImage );

	//set or get left button hover image
	const Image *getScrollLeftHoverImage( void ) const;
	void  setScrollLeftHoverImage( const Image *pNewImage );

	//set or get right button normal image
	const Image *getScrollRightNormalImage( void ) const;
	void  setScrollRightNormalImage( const Image *pNewImage );

	//set or get right button pushed image
	const Image *getScrollRightPushedImage( void ) const;
	void  setScrollRightPushedImage( const Image *pNewImage );

	//set or get right button hover image
	const Image *getScrollRightHoverImage( void ) const;
	void  setScrollRightHoverImage( const Image *pNewImage );
	
private:
	void addMiniHorzScrollProperties();
	static TLMiniHorzScrollbarProperties::MiniHorzScrollBody		d_miniScrolBody;
	static TLMiniHorzScrollbarProperties::MiniHorzScrollThumbHover	d_miniScrolThumbHorver;
	static TLMiniHorzScrollbarProperties::MiniHorzScrollThumbNormal d_miniScrolThumbNormal;
	static TLMiniHorzScrollbarProperties::MiniHorzLeftHover			d_miniScrolLeftHover;
	static TLMiniHorzScrollbarProperties::MiniHorzLeftPushed		d_miniScrolLeftPushed;
	static TLMiniHorzScrollbarProperties::MiniHorzLeftNormal		d_miniScrolLeftNormal;
	static TLMiniHorzScrollbarProperties::MiniHorzRightHover		d_miniScrolRightHover;
	static TLMiniHorzScrollbarProperties::MiniHorzRightPushed		d_miniScrolRightPushed;
	static TLMiniHorzScrollbarProperties::MiniHorzRightNormal		d_miniScrolRightNormal;
protected:
	/*************************************************************************
		Implementation functions
	*************************************************************************/
	/*
	\brief
		create a PushButton based widget to use as the increase button for this scroll bar.
	*/
	virtual PushButton*	createIncreaseButton(const String& name) const;


	/*!
	\brief
		create a PushButton based widget to use as the decrease button for this scroll bar.
	*/
	virtual PushButton*	createDecreaseButton(const String& name) const;


	/*!
	\brief
		create a Thumb based widget to use as the thumb for this scroll bar.
	*/
	virtual Thumb*	createThumb(const String& name) const;


	/*!
	\brief
		layout the scroll bar component widgets
	*/
	virtual void	performChildWindowLayout();


	/*!
	\brief
		update the size and location of the thumb to properly represent the current state of the scroll bar
	*/
	virtual void	updateThumb(void);


	/*!
	\brief
		return value that best represents current scroll bar position given the current location of the thumb.

	\return
		float value that, given the thumb widget position, best represents the current position for the scroll bar.
	*/
	virtual float	getValueFromThumb(void) const;


	/*!
	\brief
		Given window location \a pt, return a value indicating what change should be 
		made to the scroll bar.

	\param pt
		Point object describing a pixel position in window space.

	\return
		- -1 to indicate scroll bar position should be moved to a lower value.
		-  0 to indicate scroll bar position should not be changed.
		- +1 to indicate scroll bar position should be moved to a higher value.
	*/
	virtual float	getAdjustDirectionFromPoint(const Point& pt) const;


	/*************************************************************************
		Overridden Implementation Rendering Functions
	*************************************************************************/
	/*!
	\brief
		Perform rendering for this widget
	*/
	virtual void	drawSelf(KRenderCache* panelCache, Point* panelAbsPos);

	/*************************************************************************
		Overridden Event Handling Functions
	*************************************************************************/
	virtual void	onCharacter(KeyEventArgs& e);


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	const Image*	d_body;			//!< Image for body segment.
};


/*!
\brief
	Factory class for producing TLMiniHorzScrollbar objects
*/
class TAHAREZLOOK_API TLMiniHorzScrollbarFactory : public WindowFactory
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	TLMiniHorzScrollbarFactory(void) : WindowFactory(TLMiniHorzScrollbar::WidgetTypeName) { }
	~TLMiniHorzScrollbarFactory(void){}


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


#endif	// end of guard _TLMiniHorzScrollbar_h_
