/************************************************************************
	filename: 	CEGUIStaticImage.h
	created:	4/6/2004
	author:		Paul D Turner
	
	purpose:	Interface for the static image widget.
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
#ifndef _CEGUIStaticImage_h_
#define _CEGUIStaticImage_h_

#include "elements/CEGUIStatic.h"
#include "CEGUIRenderableImage.h"
#include "elements/CEGUIStaticImageProperties.h"


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif


// Start of CEGUI namespace section
namespace CEGUI
{
class CEGUIEXPORT StaticImage : public Static
{
public:
	static const String EventNamespace;				//!< Namespace for global events

	/*************************************************************************
		Construction and Destruction
	*************************************************************************/

	StaticImage(const String& type, const String& name);

	virtual ~StaticImage(void);


	const Image*	getImage(void) const		{return d_image;}

	void	setImage(const Image* image);

	void	setNextImage(StaticImage* image)	{ d_nextImage = image; }

	void	setImage(const String& imageset, const String& image);

	bool	isDragMovingEnabled() const 
	{
		return d_dragEnabled;
	}

	void	setDragMovingEnabled( bool moveEnabled )
	{
		d_dragEnabled = moveEnabled;
	}

	void	play(bool hide = false);
	void	play(int beginFrame, int endFrame, bool hide = false);
	void	stop(void);

	int		getCurFrameIdx() { return d_frameIdx; }

	void	setCyc( bool cyc ) { d_cyc = cyc; }
	bool	getCyc( void ) { return d_cyc; }
	
	void	setCycCount( int cycCount ) { d_cycCount = cycCount; }
	int		getCycCount( void ) { return d_cycCount; }

	void	setHelpPlane( const String &helpName )
	{ 
		d_sHelpPlane = helpName;
	}
	String  getHelpPlane( void ) const 
	{ 
		return d_sHelpPlane;
	}
	
	void	setHelpToolTip( const String &helpToolTip )
	{ 
		d_sHelpToolTip = helpToolTip;
	}
	String  getHelpToolTip( void ) const 
	{ 
		return d_sHelpToolTip;
	} 
	
	int		getAlphaAtPixel(int frame, int x, int y);

	virtual void	setVisible(bool setting);

protected:
	/*************************************************************************
		Overridden from base class
	*************************************************************************/
	virtual void drawSelf(KRenderCache* panelCache, Point* panelAbsPos);
	virtual void drawSelf();

	virtual void	onMouseHover(MouseEventArgs& e);

	/*************************************************************************
		Implementation Methods
	*************************************************************************/

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
		if (class_name==(const utf8*)"StaticImage")	return true;
		return Static::testClassName_impl(class_name);
	}


	/*************************************************************************
		Implementation Data
	*************************************************************************/
	const Image*	d_image;

	bool			d_stop;
	int				d_frameIdx;
	bool			d_dragEnabled;
	bool			d_dragging;
	Point			d_dragPoint;
	bool			d_cyc;
	int				d_cycCount;
	String			d_sHelpPlane;
	String			d_sHelpToolTip;
	int				d_nInterval;
	long			d_lastDrawTime;

	bool			d_bFirstPlay;		// 是否第一次播放（只对非循环播放有效）
	bool			d_bHideAfterPlay;	// 结束是否隐藏

	bool			d_bPartPlay;		// 是否只播放SPR的一部分（只对非循环播放有效）
	int				d_beginFrameIdx;	// 开始播放帧数（只对非循环播放有效）
	int				d_endFrameIdx;		// 结束播放帧数（只对非循环播放有效）
	
	StaticImage*	d_nextImage;		// 只用于登录界面（特例，其他地方勿用）
private:
	/*************************************************************************
		Static Properties for this class
	*************************************************************************/
	static StaticImageProperties::Image				d_imageProperty;
	static StaticImageProperties::ImageEx			d_imageExProperty;
	static StaticImageProperties::DragMovingEnabled	d_dragMovingEnabled;
	static StaticImageProperties::HelpPlaneName		d_helpPlaneName;
	static StaticImageProperties::HelpToolTip		d_helpToolTip;
	/*************************************************************************
		Private methods
	*************************************************************************/
	void	addStaticImageProperties(void);
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// end of guard _CEGUIStaticImage_h_
