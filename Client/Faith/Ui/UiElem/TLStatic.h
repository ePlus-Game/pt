/************************************************************************
	filename: 	TLStatic.h
	created:	5/6/2004
	author:		Paul D Turner
	
	purpose:	Interface to Taharez look static widgets & factories
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
#ifndef _TLStatic_h_
#define _TLStatic_h_

#include "TLModule.h"
#include "CEGUIWindowFactory.h"
#include "elements/CEGUIStaticText.h"
#include "elements/CEGUIStaticImage.h"
#include "TLButton.h"
#include "layoutinterface.h"
#include "Ui/LayoutRender.h"

namespace TLStaticImageProperty
{
	using namespace CEGUI;
	
	class StaticImageName : public Property
	{
	public:
		StaticImageName()
			:Property( "StaticImageName", "set window image name", "", false)
		{}
		String	get( const PropertyReceiver *receive ) const;
		void	set( PropertyReceiver* receiver, const String& value );
	};

	//set help button x position
	class HelpButtonXPos : public Property
	{
	public:
		HelpButtonXPos()
			:Property( "HelpButtonXPos", "set help button x position", "1.0000" )
		{}
		String	get( const PropertyReceiver *receive ) const;
		void	set( PropertyReceiver* receiver, const String& value );
	};

	//set help button y position
	class HelpButtonYPos : public Property
	{
	public:
		HelpButtonYPos()
			:Property( "HelpButtonYPos", "set help button y position", "1.0000" )
		{}
		String	get( const PropertyReceiver *receive ) const;
		void	set( PropertyReceiver* receiver, const String& value );
	};

	//set help button width
	class HelpButtonWidth : public Property
	{
	public:
		HelpButtonWidth()
			:Property( "HelpButtonWidth", "set help button widht", "1.0000" )
		{}
		String	get( const PropertyReceiver *receive ) const;
		void	set( PropertyReceiver* receiver, const String& value );
	};
	
	//set help button height
	class HelpButtonHeight : public Property
	{
	public:
		HelpButtonHeight()
			:Property( "HelpButtonHeight", "set help button height", "1.0000" )
		{}
		String	get( const PropertyReceiver *receive ) const;
		void	set( PropertyReceiver* receiver, const String& value );
	};
	
	//set button normal image
	class HelpBtnNormalImage : public Property
	{
	public:
		HelpBtnNormalImage()
			:Property( "HelpBtnNormalImage", "set help button normal image", "set:TaharezLook image:HelpBtnNormalImage" )
		{}
		String	get( const PropertyReceiver *receive ) const;
		void	set( PropertyReceiver* receiver, const String& value );
	};

	//set button normal image
	class HelpBtnHoverImage : public Property
	{
	public:
		HelpBtnHoverImage()
			:Property( "HelpBtnHoverImage", "set help button hover image", "set:TaharezLook image:HelpBtnHoverImage" )
		{}
		String	get( const PropertyReceiver *receive ) const;
		void	set( PropertyReceiver* receiver, const String& value );
	};

	//set button pushed image
	class HelpBtnPushedImage : public Property
	{
	public:
		HelpBtnPushedImage()
			:Property( "HelpBtnPushedImage", "set help button pushed image", "set:TaharezLook image:HelpBtnPushedImage" )
		{}
		String	get( const PropertyReceiver *receive ) const;
		void	set( PropertyReceiver* receiver, const String& value );
	};

	//set button bool
	class HelpBtnEnable : public Property
	{
	public:
		HelpBtnEnable()
			:Property( "HelpBtnEnable", "set help button enable property", "False" )
		{}

		String  get( const PropertyReceiver* receiver ) const;
		void    set( PropertyReceiver* receiver, const String &value );
	};
}
// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
	StaticText class for the TaharezLook Gui Scheme
*/
class TAHAREZLOOK_API TLStaticText : public StaticText
{
	int d_layoutXoff;
	int d_layoutYoff;
	ILayout* d_layout;
	LayoutRender* d_layoutRender;
public:

	void			useLayout();
	ILayout*		getLayout(){	return d_layout;	}
	void			setTextBorderMode(int borderMode);
	void			fitLayoutSize(bool anjWidth = true);
	void			setLayoutOffset(int x, int y);
	Point			getLayoutOffset();
	
	virtual void	drawSelf(KRenderCache* panelCache, Point* panelAbsPos);
	virtual void	drawSelf();

	virtual void	populateRenderCache();

	/*************************************************************************
		Constants
	*************************************************************************/
	// type name for this widget
	static const utf8	WidgetTypeName[];				//!< The unique typename of this widget

	// component widget type names
	static const utf8*	HorzScrollbarTypeName;		//!< Type name of widget to be created as horizontal scroll bar.
	static const utf8*	VertScrollbarTypeName;		//!< Type name of widget to be created as vertical scroll bar.

	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for Taharez Look StaticText objects.

	\param type
		String object that specifies a type for this window, usually provided by a factory class.

	\param name
		String object that specifies a unique name that will be used to identify the new Window object
	*/
	TLStaticText(const String& type, const String& name);


	/*!
	\brief
		Destructor for TLStaticText objects.
	*/
	virtual ~TLStaticText(void);


	/*!
	\brief
		Initialises the Window based object ready for use.

	\note
		This must be called for every window created.  Normally this is handled automatically by the WindowFactory for each Window type.

	\return
		Nothing
	*/
	virtual void	initialise(void);


protected:
	/*************************************************************************
		Implementation Methods (abstract)
	*************************************************************************/
	/*!
	\brief
		create and return a pointer to a Scrollbar widget for use as vertical scroll bar

	\return
		Pointer to a Scrollbar to be used for scrolling vertically.
	*/
	virtual Scrollbar*	createVertScrollbar(const String& name) const;
 

	/*!
	\brief
		create and return a pointer to a Scrollbar widget for use as horizontal scroll bar

	\return
		Pointer to a Scrollbar to be used for scrolling horizontally.
	*/
	virtual Scrollbar*	createHorzScrollbar(const String& name) const;


	/*!
	\brief
		layout component widgets
	*/
	virtual void	performChildWindowLayout();

	/*************************************************************************
		Overridden Event Handling Functions
	*************************************************************************/
	virtual void	onCharacter(KeyEventArgs& e);
	
	virtual	void	onMouseEnters(MouseEventArgs& e);
	virtual void	onMouseLeaves(MouseEventArgs& e);
};


/*!
\brief
	StaticImage class for the TaharezLook Gui Scheme
*/
class TAHAREZLOOK_API TLStaticImage : public StaticImage
{
public:
	/*!
	 \brief
	  This function make the ui to shake 
	*/
	void Shake(const const bool bXShake,const bool bYShake,unsigned long dwDistance);
	void MoveTo(const int iDestX,const int iDestY, const int iDx,const int iDy,const bool bShake); //Move From cur pos to dest pos ,iDx and iDy control the speed
	
	bool isPlaying( void )
	{
		return !d_stop;
	}
	/*************************************************************************
		Constants
	*************************************************************************/
	// type name for this widget
	static const utf8	WidgetTypeName[];				//!< The unique typename of this widget
	static const utf8   *HelpBtnWidgetType;
	
	// some suff define
	static const float  HelpBtnPositionX;
	static const float  HelpBtnPositionY;
	static const float  HelpBtnSizeWidth;
	static const float	HelpBtnSizeHeight;

	/*************************************************************************
		Construction / Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for Taharez Look StaticImage objects.

	\param type
		String object that specifies a type for this window, usually provided by a factory class.

	\param name
		String object that specifies a unique name that will be used to identify the new Window object
	*/
	TLStaticImage(const String& type, const String& name) 
	: StaticImage(type, name)
	, d_pHelpBtn(NULL)
	, d_btnSize(0, 0)
	, d_btnPos(Point(0,0))
	, d_bBtnEnable(false)
    , d_pHelpBtnNormal(NULL)
    , d_pHelpBtnHover(NULL)
    , d_pHelpBtnPushed(NULL)
	{}

	/*!
	\brief
		Destructor for TLStaticImage objects.
	*/
	virtual ~TLStaticImage(void){}


	/*!
	\brief
		Initialises the Window based object ready for use.

	\note
		This must be called for every window created.  Normally this is handled automatically by the WindowFactory for each Window type.

	\return
		Nothing
	*/
	virtual void	initialise(void);
	
	/*!
	\brief
		Handler called when a mouse button has been depressed within this window's area.

	\param e
		MouseEventArgs object.  All fields are valid.
	*/
	virtual void	onMouseButtonDown(MouseEventArgs& e);


	/*!
	\brief
		Handler called when a mouse button has been released within this window's area.

	\param e
		MouseEventArgs object.  All fields are valid.
	*/
	virtual void	onMouseButtonUp(MouseEventArgs& e);

	/*!
	\brief
		Handler called when the mouse cursor has been moved within this window's area.

	\param e
		MouseEventArgs object.  All fields are valid.
	*/
	virtual void	onMouseMove(MouseEventArgs& e);
	virtual	void	onMouseEnters(MouseEventArgs& e);
	virtual void	onMouseLeaves(MouseEventArgs& e);
	void			show();

	/******************************************************************************
	******************************************************************************/
	virtual void onShown(WindowEventArgs& e);

	const float getHelpBtnXPos() const;
	void setHelpBtnXPos( const float xpos );

	const float getHelpBtnYPos() const;
	void setHelpBtnYPos( const float ypos );

	const float getHelpBtnWidth() const;
	void setHelpBtnWdith( const float width );

	const float getHelpBtnHeight() const;
	void setHelpBtnHeight( const float height );

	const String getStaticImageName() const;
	void setStaticImageName( const String staticname );
	
	const Image *getHelpBtnNormalImage( void ) const;
	void  setHelpBtnNormalImage( const Image *pNewImage );

	const Image *getHelpBtnHoverImage( void ) const;
	void  setHelpBtnHoverImage( const Image *pNewImage );

	const Image *getHelpBtnPushedImage( void ) const;
	void  setHelpBtnPushedImage( const Image *pNewImage );

	const bool getHelpBtnEnable( void ) const;
	void  setHelpBtnEnable( const bool bEnable );

	void CreateHelpBtn( const String& name );

	void DoShow();

private:
	static TLStaticImageProperty::StaticImageName	d_sImageName;
	static TLStaticImageProperty::HelpButtonHeight	d_sHelpBtnHeight;
	static TLStaticImageProperty::HelpButtonWidth	d_sHelpBtnWidth;
	static TLStaticImageProperty::HelpButtonXPos	d_sHelpBtnXPos;
	static TLStaticImageProperty::HelpButtonYPos	d_sHelpBtnYPos;
	static TLStaticImageProperty::HelpBtnEnable		d_sHelpEnable;
	static TLStaticImageProperty::HelpBtnNormalImage	d_sHelpBtnNormalImage;
	static TLStaticImageProperty::HelpBtnHoverImage		d_sHelpBtnHoverImage;
	static TLStaticImageProperty::HelpBtnPushedImage	d_sHelpBtnPushedImage;
private:
	void addStaticProperties();
	bool handleHelp( const CEGUI::EventArgs& args );
	void ShakeBreathe(void);
	void PosMoveBreathe(void);
private:
	void offsetPixelPosition(const Vector2& offset);
	
	PushButton *d_pHelpBtn;
	Image	   *d_pHelpBtnNormal;
	Image	   *d_pHelpBtnHover;
	Image	   *d_pHelpBtnPushed;
	String		d_windowName;
	Size		d_btnSize;
	Point		d_btnPos;
	bool		d_bBtnEnable;
	bool		d_firstShow;
private:
	//Shake effect used
	typedef struct tagSHAKE_EFFECT_INFO
	{
		bool          m_bShakeFlag;
		DWORD         m_Distance;
		bool          m_DirX;
		bool          m_DirY;
		bool          m_State;         
		DWORD         m_dwTimer;  
		Point         m_InitPos;
		tagSHAKE_EFFECT_INFO(void);
	}SHAKE_EFFECT_INFO;
	
 	SHAKE_EFFECT_INFO  m_ShakeOption;

	//Move effect used
	//Show Pos Moving info
	typedef struct tagMOVE_INFO
	{
		int                m_Dx;
		int                m_Dy;
		int                m_DestX;
		int                m_DestY;
		unsigned long      m_MoveTimer;
		bool               m_bMoveFlag;
		bool               m_bShakeable;
		bool               m_bDragable;
		tagMOVE_INFO();
	}MOVE_EFFECT_INFO;

    MOVE_EFFECT_INFO   m_MoveOption;

protected:
	void	updateSelf(float elapsed);
};



/*!
\brief
	Factory class for producing StaticText objects for the Taharez GUI Scheme
*/
class TAHAREZLOOK_API TLStaticTextFactory : public WindowFactory
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	TLStaticTextFactory(void) : WindowFactory(TLStaticText::WidgetTypeName) { }
	~TLStaticTextFactory(void){}


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


/*!
\brief
	Factory class for producing StaticImage objects for the Taharez GUI Scheme
*/
class TAHAREZLOOK_API TLStaticImageFactory : public WindowFactory
{
public:
	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	TLStaticImageFactory(void) : WindowFactory(TLStaticImage::WidgetTypeName) { }
	~TLStaticImageFactory(void){}


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


#endif	// end of guard _TLStatic_h_
