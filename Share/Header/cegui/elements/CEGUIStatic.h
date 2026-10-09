/************************************************************************
	filename: 	CEGUIStatic.h
	created:	13/4/2004
	author:		Paul D Turner
	
	purpose:	Interface to base class for Static widget
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
#ifndef _CEGUIStatic_h_
#define _CEGUIStatic_h_

#include "CEGUIBase.h"
#include "CEGUIWindow.h"
#include "CEGUIRenderableFrame.h"
#include "elements/CEGUIStaticProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
/*!
\brief
	Base class for static widgets.
*/
class CEGUIEXPORT Static : public Window
{
public:
	void	StopFiding();
	static const String EventNamespace;

	//FRAME相关
	void	setFrameEnabled(bool setting);
	bool	isFrameEnabled(void) const		{return d_frameEnabled;}

	void	setFrameImages(const Image* topleft, const Image* topright, const Image* bottomleft, const Image* bottomright, const Image* left, const Image* top, const Image* right, const Image* bottom);
	void	setImageForFrameLocation(FrameLocation location, const Image* image);
	const Image* getImageForFrameLocation(FrameLocation location) const;

	int		getLeftFrameWidth()	{	return d_left_width;	}
	int		getRightFrameWidth(){	return d_right_width;	}
	int		getTopFrameHeight()	{	return d_top_height;	}
	int		getBottomFrameHeight(){	return d_bottom_height;	}

	//background相关
	bool	isBackgroundEnabled(void) const		{return d_backgroundEnabled;}
	void	setBackgroundEnabled(bool setting);

	void	setBackgroundImage(const Image* image);
	void	setBackgroundImage(const String& imageset, const String& image);
	const Image* getBackgroundImage(void) const;

	virtual Rect	getUnclippedInnerRect(void) const;

	
	void	setFadeInTime(float fadetime)		{d_fadeInTime=fadetime;}
	void	setFadeOutTime(float fadetime)		{d_fadeOutTime=fadetime;}

	float	getFadeInTime(void) const			{return d_fadeInTime;}
	float	getFadeOutTime(void) const			{return d_fadeOutTime;}

	void	setAutoCloseTime(float closeTime)	{d_autoCloseTime = closeTime;}
	float   getAutoCloseTime(void) const		{return d_autoCloseTime;}

	virtual void OpenBox();
	
	virtual void CloseBox();

	bool	isDummyWnd() const
	{
		return !d_handleMsg;
	}

	void	setDummyWnd( bool bDummyWnd )
	{
		d_handleMsg = !bDummyWnd;
	}

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/
	/*!
	\brief
		Constructor for static widget base class
	*/
	Static(const String& type, const String& name);


	/*!
	\brief
		Destructor for static widget base class.
	*/
	virtual ~Static(void);

protected:
	/*************************************************************************
		Overridden from base class
	*************************************************************************/
	virtual void populateRenderCache();
	/*************************************************************************
		Event handling
	*************************************************************************/
	virtual void onMouseButtonDown(MouseEventArgs& e);
	virtual void onMouseButtonUp(MouseEventArgs& e);
	virtual void onMouseMove(MouseEventArgs& e);
	virtual void onMouseHover(MouseEventArgs& e);
	virtual void onMouseClicked(MouseEventArgs& e);
	virtual void onMouseDoubleClicked(MouseEventArgs& e);
	virtual void onMouseTripleClicked(MouseEventArgs& e);


	/*************************************************************************
		Implementation methods
	*************************************************************************/



	/*!
	\brief
		return ARGB colour value \a col, with its alpha component modulated by the value specified in float \a alpha.
	*/
	colour	calculateModulatedAlphaColour(const colour& col, float alpha) const;


	/*!
	\brief
		This is used internally to indicate that the frame for the static widget has been modified, and as such
		derived classes may need to adjust their layouts or reconfigure their rendering somehow.
	\note
		This does not currently fire an external event.
	*/
	virtual void	onStaticFrameChanged(WindowEventArgs& e)	{}


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
		if (class_name==(const utf8*)"Static")	return true;
		return Window::testClassName_impl(class_name);
	}
	
	virtual void updateSelf(float elapsed);
	virtual void drawSelf(KRenderCache* panelCache, Point* panelAbsPos);

	/*************************************************************************
		Implementation Data
	*************************************************************************/
	bool				d_frameEnabled;		//!< True when the frame is enabled.

	const Image*		d_topleft;
	const Image*		d_topright;
	const Image*		d_bottomleft;
	const Image*		d_bottomright;
	const Image*		d_left;
	const Image*		d_top;
	const Image*		d_right;
	const Image*		d_bottom;

	bool				d_backgroundEnabled;//!< true when the background is enabled.
	const Image*		d_background;		//!< Image to use for widget background.

	//渐入渐出
	float d_origAlpha;			//!< The original alpha of this window.
	float d_fadeElapsed;		//!< The time in seconds this popup menu has been fading.
	float d_fadeOutTime;		//!< The time in seconds it takes for this popup menu to fade out.
	float d_fadeInTime;			//!< The time in seconds it takes for this popup menu to fade in.
	bool d_fading;				//!< true if this popup menu is fading in/out. false if not
	bool d_fadingOut;			//!< true if this popup menu is fading out. false if fading in.
    bool d_isOpen;				//!< true if this popup menu is open. false if not.
	
	//自动关闭
	float d_autoCloseTime;
	bool  d_needClose;
	bool  d_handleMsg;

	// cache of frame edge sizes
	float	d_left_width;			//!< Width of the left edge image for the current frame.
	float	d_right_width;			//!< Width of the right edge image for the current frame.
	float	d_top_height;			//!< Height of the top edge image for the current frame.
	float	d_bottom_height;		//!< Height of the bottom edge image for the current frame.

private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static StaticProperties::FrameEnabled				d_frameEnabledProperty;
	static StaticProperties::BackgroundEnabled			d_backgroundEnabledProperty;

	static StaticProperties::BackgroundImage			d_backgroundImageProperty;
	static StaticProperties::TopLeftFrameImage			d_topLeftFrameProperty;
	static StaticProperties::TopRightFrameImage			d_topRightFrameProperty;
	static StaticProperties::BottomLeftFrameImage		d_bottomLeftFrameProperty;
	static StaticProperties::BottomRightFrameImage		d_bottomRightFrameProperty;
	static StaticProperties::LeftFrameImage				d_leftFrameProperty;
	static StaticProperties::RightFrameImage			d_rightFrameProperty;
	static StaticProperties::TopFrameImage				d_topFrameProperty;
	static StaticProperties::BottomFrameImage			d_bottomFrameProperty;

	static StaticProperties::FadeInTime					d_fadeInTimeProperty;
	static StaticProperties::FadeOutTime				d_fadeOutTimeProperty;

	static StaticProperties::AutoCloseTime				d_autoCloseTimeProperty;
	
	static StaticProperties::DummyWnd					d_dummyWndProperty;


	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addStaticProperties(void);
};


} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIStatic_h_
