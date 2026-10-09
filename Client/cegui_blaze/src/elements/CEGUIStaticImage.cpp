/************************************************************************
	filename: 	CEGUIStaticImage.cpp
	created:	4/6/2004
	author:		Paul D Turner
	
	purpose:	Implementation of the static image widget class.
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
#include "elements/CEGUIStaticImage.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"

// Start of CEGUI namespace section
namespace CEGUI
{
const String StaticImage::EventNamespace("StaticImage");

/*************************************************************************
	Definitions of Properties for this class
*************************************************************************/
StaticImageProperties::Image				StaticImage::d_imageProperty;
StaticImageProperties::ImageEx				StaticImage::d_imageExProperty;
StaticImageProperties::DragMovingEnabled	StaticImage::d_dragMovingEnabled;
StaticImageProperties::HelpPlaneName		StaticImage::d_helpPlaneName;
StaticImageProperties::HelpToolTip			StaticImage::d_helpToolTip;
/*************************************************************************
	Constructor for static image widgets.	
*************************************************************************/
StaticImage::StaticImage(const String& type, const String& name) :
	Static(type, name)
{
	d_stop		= true;
	d_frameIdx			= 0;
	d_dragging			= false;
	d_dragEnabled		= false;
	d_cyc				= false;
	d_cycCount			= 0;
	d_sHelpPlane		= "";
	d_sHelpToolTip		= "";
	d_bFirstPlay		= true;
	d_bHideAfterPlay	= false;
	d_bPartPlay			= false;
	d_beginFrameIdx		= -1;
	d_endFrameIdx		= -1;
	addStaticImageProperties();

	d_image = NULL;
	d_nextImage = NULL;
}


/*************************************************************************
	Destructor for static image widgets.
*************************************************************************/
StaticImage::~StaticImage(void)
{
}


/*************************************************************************
	Set the Image object to be drawn by this widget.	
*************************************************************************/
void StaticImage::setImage(const Image* image)
{
	if(d_image == image)
	{
		return;
	}
	d_image = image;

	if(d_image != NULL)
	{
		d_nInterval = const_cast<Imageset*>(d_image->getImageset())->getSpr()->GetInterval();
		int frameCount = const_cast<Imageset*>(d_image->getImageset())->getSpr()->GetFrames();
		if (frameCount > 1)
			d_lastDrawTime = ::GetTickCount();
	}

	requestRedraw();
}


/*************************************************************************
	Set the Image object to be drawn by this widget.	
*************************************************************************/
void StaticImage::setImage(const String& imageset, const String& image)
{
	if(ImagesetManager::getSingleton().isImagesetPresent(imageset) == false)
	{
		return;
	}

	Imageset* ims = ImagesetManager::getSingleton().getImageset(imageset);
	if(NULL == ims)
	{
		return;
	}

	if(ims->isImageDefined(image) == false)
	{
		return;
	}
	setImage(&ims->getImage(image));
}

void StaticImage::play(bool hide)
{
	d_stop				= false;
	d_frameIdx			= 0;
	d_bPartPlay			= false;
	d_bHideAfterPlay	= hide;
	show();
}

void StaticImage::play(int beginFrame, int endFrame, bool hide)
{
	int frameCount = 0;
	if ( d_image && d_image->getImageset() && const_cast<Imageset*>(d_image->getImageset())->getSpr() )
	{
		frameCount = const_cast<Imageset*>(d_image->getImageset())->getSpr()->GetFrames();
	}
	
		
	if ( beginFrame < 0 || beginFrame > frameCount || 
		 endFrame < 0 || endFrame > frameCount || beginFrame > endFrame )
	{
		 return;
	}

	d_stop				= false;
	d_frameIdx			= 0;
	d_cyc				= false;
	d_beginFrameIdx		= beginFrame;
	d_endFrameIdx		= endFrame;
	d_bPartPlay			= true;
	d_bHideAfterPlay	= hide;

	show();
	requestRedraw();
}

void StaticImage::stop(void)
{
	d_stop		= true;
	d_frameIdx	= 0;
	d_bPartPlay	= false;
	d_bHideAfterPlay	= false;
	d_bFirstPlay = true;
	//hide();
}


void StaticImage::drawSelf(KRenderCache* panelCache, Point* panelAbsPos)
{	
	Static::drawSelf(panelCache, panelAbsPos);

	if(d_image == NULL)
	{
		if(d_bHideAfterPlay)
		{
			hide();
		}
		return;
	}

	Rect clipper = getInnerRect();
	Point offPos(-panelAbsPos->d_x, -panelAbsPos->d_y);
	clipper.offset(offPos);

	Point pos = getUnclippedInnerRect().getPosition() += offPos;

	int frameCount = const_cast<Imageset*>(d_image->getImageset())->getSpr()->GetFrames();

	//当 没有停止 并且 不止一帧 的时候 图片继续播放
	if(d_stop == false && frameCount > 1)
	{
		if ( d_bFirstPlay && !d_cyc && frameCount > 1 )
		{
			d_lastDrawTime = ::GetTickCount();
			d_bFirstPlay = false;
		}
		
		d_frameIdx = (int)((float)(::GetTickCount() - d_lastDrawTime ) / (float)d_nInterval);
		/*if ( d_frameIdx > frameCount )
		{
			d_frameIdx = 0;
			d_lastDrawTime = ::GetTickCount();
			requestRedraw();
		}//*/

		if ( d_bPartPlay )
		{
			// 只播放SPR的一部分
			d_frameIdx += d_beginFrameIdx;
			if(d_frameIdx > d_endFrameIdx)
			{
				if(false == d_cyc)
				{
					d_bFirstPlay	= true;
					d_stop			= true;
					d_frameIdx = d_endFrameIdx;
					if ( d_bHideAfterPlay )
					{
						d_frameIdx = 0;
						d_stop = true;
						d_bHideAfterPlay = false;
						hide();
					}
				}
				else
				{
					d_frameIdx = 0;
					d_lastDrawTime = ::GetTickCount();
					//还需要继续播放的话得通知panel刷新
					requestRedraw();
				}
			}
			else
			{
				requestRedraw();
			}
		}
		else
		{
			if(d_frameIdx >= frameCount)
			{
				if(d_cycCount > 0)
				{	
					--d_cycCount;
				}
				if(d_cyc || d_cycCount > 0)
				{
					d_frameIdx = 0;
					d_lastDrawTime = ::GetTickCount();
					//还需要继续播放的话得通知panel刷新
					requestRedraw();
				}
				else
				{
					d_frameIdx = 0;
					d_bFirstPlay = true;
					d_stop = true;
					if ( d_bHideAfterPlay )
					{
						d_bHideAfterPlay = false;
						hide();
					}
					if (d_nextImage)
					{						
						// 只此一处使用
						d_nextImage->show();
						d_nextImage->play();
					}
				}
				/*else
				{
					d_frameIdx = 0;
					d_lastDrawTime = ::GetTickCount();
					//还需要继续播放的话得通知panel刷新
					requestRedraw();
				}//*/
			}
			else
			{
				requestRedraw();
			}
		}
	}

	panelCache->cacheImage(d_image, pos, clipper, d_frameIdx);
	
}

void StaticImage::drawSelf()
{	
	//先画边框
	if (d_needsRedraw)
	{
		// dispose of already cached imagery.
		d_renderCache.clearCachedImagery();
		// get derived class to re-populate cache.
		populateRenderCache();
		// mark ourselves as no longer needed a redraw.
		d_needsRedraw = false;
	}
	
	// if render cache contains imagery.
	if (d_renderCache.hasCachedImagery())
	{
		Point absPos(getUnclippedPixelRect().getPosition());
		// calculate clipping area for this window
		Rect clipper(getPixelRect());
		
		// If window is not totally clipped.
		if (clipper.getWidth())
		{
			// send cached imagery to the renderer.
			d_renderCache.render(absPos, 0, clipper, getAlpha());
		}
	}


	//再画图片
	if(d_image == NULL)
	{
		return;
	}

	const Image *img =  d_image;
	
	int frameCount = const_cast<Imageset*>(img->getImageset())->getSpr()->GetFrames();
	
	Rect dest_rect(getUnclippedPixelRect());
	Rect clipper(getPixelRect());
	img->draw(dest_rect, 0, clipper, ColourRect(1,1,1,1),TopLeftToBottomRight,getEffectiveAlpha(), d_frameIdx);

	if(d_stop || frameCount < 2)
	{
		return;
	}

	//当没有停止或者不止一帧的时候图片继续播放
	if(d_bFirstPlay && !d_cyc)
	{
		d_lastDrawTime = ::GetTickCount();
		d_bFirstPlay = false;
	}
	
	d_frameIdx = (int)((float)(::GetTickCount() - d_lastDrawTime ) / (float)d_nInterval);
	
	if(d_bPartPlay)
	{
		// 只播放SPR的一部分
		d_frameIdx += d_beginFrameIdx;
		if(d_frameIdx > d_endFrameIdx)
		{
			if(d_cycCount > 0)
			{
				--d_cycCount;
			}

			if(d_cyc || d_cycCount > 0)
			{
				d_frameIdx = 0;
				d_lastDrawTime = ::GetTickCount();
			}
			else
			{
				d_bFirstPlay	= true;
				d_stop			= true;
				d_frameIdx = d_endFrameIdx;
				if(d_bHideAfterPlay)
				{
					d_frameIdx = 0;
					d_stop = true;
					d_bHideAfterPlay = false;
					hide();
				}
			}
		}
	}
	else
	{
		if(d_frameIdx >= frameCount)
		{
			if(d_cycCount > 0)
			{	
				--d_cycCount;
			}
			if(d_cyc || d_cycCount > 0)
			{
				d_frameIdx = 0;
				d_lastDrawTime = ::GetTickCount();
			}
			else
			{
				d_frameIdx = 0;
				d_bFirstPlay = true;
				d_stop = true;
				if(d_bHideAfterPlay)
				{
					d_bHideAfterPlay = false;
					hide();
				}
			}
		}
	}
}


int	StaticImage::getAlphaAtPixel(int frame, int x, int y)
{
	if(d_frameEnabled)
	{
		x -= d_left_width;
		y -= d_top_height;
	}
	
	if(d_image)
	{
		Image* image = const_cast<Image*>(d_image);
		return image->getAlphaAtPixel(frame, x, y);
	}

	return 0;
}

void StaticImage::onMouseHover(MouseEventArgs& e)
{
	if(d_handleMsg)
	{
		e.handled = true;
	}
	fireEvent(EventMouseHover, e);
}

void StaticImage::setVisible(bool setting)
{
	Static::setVisible(setting);
	if (!d_visible)
	{
		stop();
	}
}

/*************************************************************************
	Add properties for static image
*************************************************************************/
void StaticImage::addStaticImageProperties(void)
{
	addProperty(&d_imageProperty);
	addProperty(&d_imageExProperty);
	addProperty(&d_dragMovingEnabled);
	addProperty(&d_helpPlaneName);
	addProperty(&d_helpToolTip);
}


} // End of  CEGUI namespace section
