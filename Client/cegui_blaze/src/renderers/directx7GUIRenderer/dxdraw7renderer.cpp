/************************************************************************
	filename: 	dxdraw7renderer.cpp
	created:	22/5/2006
	author:		SamSun
	
	purpose:	Main source file for Renderer class using DirectX 7.0
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
#include "renderers/directx7GUIRenderer/dxdraw7renderer.h"
#include "renderers/directx7GUIRenderer/dxdraw7texture.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"
#include "CEGUIWindow.h"
#include "KWin32Wnd.h"

#include <algorithm>
#include "CEGUIFont.h"

#undef min


// Start of CEGUI namespace section
namespace CEGUI
{

/*************************************************************************
	Constructor
*************************************************************************/
DirectX7Renderer::DirectX7Renderer(iRepresentShell* representshell)
{
	d_representShell = representshell;

	Size size(getViewportSize());

	constructor_impl(representshell, size);
}

/*************************************************************************
	return size of device view port (if possible)	
*************************************************************************/
Size DirectX7Renderer::getViewportSize(void)
{
	int nScreenW = g_GetScreenWidth();
	int nScreenH = g_GetScreenHeight();
	return Size(nScreenW, nScreenH);
}

/*************************************************************************
	method to do work of constructor
*************************************************************************/
void DirectX7Renderer::constructor_impl(iRepresentShell* representshell, const Size& display_size)
{
	d_representShell = representshell;
	d_queueing		 = true;

	// initialise renderer display area
	d_display_area.d_left	= 0;
	d_display_area.d_top	= 0;
	d_display_area.setSize(display_size);

	// set max texture size the the smaller of max width and max height.
	d_maxTextureSize = 1024;

    // set ID string
    //d_identifierString = "CEGUI::DirectX7Renderer - Official DirectDraw 7 based renderer module for CEGUI";
}


/*************************************************************************
	Destructor
*************************************************************************/
DirectX7Renderer::~DirectX7Renderer(void)
{
	destroyAllTextures();
}


/*************************************************************************
	add's a quad to the list to be rendered
*************************************************************************/
void DirectX7Renderer::addQuad(const Rect& dest_rect, const Rect& clipper, float z, const Texture* tex, const Rect& texture_rect, const ColourRect& colours, const ColourRect& bordercolours, bool border, QuadSplitMode quad_split_mode, int frame)
{
	// if not queueing, render directly (as in, right now!)
//	if (d_queueing)
	{
	//	renderQuadDirect(dest_rect, z, tex, texture_rect, clipper, colours, bordercolours, border, quad_split_mode, frame);
	}
	/*
	else
	{
		QuadInfo quad;

		quad.position		= dest_rect;
		quad.clipper		= clipper;
		quad.z				= z;
		quad.texture		= const_cast<Texture*>(tex);
		quad.texPosition	= texture_rect;
		quad.rectColour		= colours;
		quad.pSpr			= ((DirectX7Texture*)tex)->getSpr();
		RectEx* tex_rect = (RectEx*)(&texture_rect);
		quad.scalex			= tex_rect->getScaleX();
		quad.scaley			= tex_rect->getScaleY();
		quad.alpha			= tex_rect->getAlpha();
		quad.frame			= frame;
        // set quad split mode
        quad.splitMode = quad_split_mode;

		d_quadlist.insert(quad);
	}//*/

}


/*************************************************************************
	perform final rendering for all queued renderable quads.
*************************************************************************/
void DirectX7Renderer::doRender(void)
{
	int nScreenW = g_GetScreenWidth();
	int nScreenH = g_GetScreenHeight();
	 

	::tagRECT oldrect;
	g_pCanvas->GetClipRect( &oldrect );
	for (QuadList::iterator i = d_quadlist.begin(); i != d_quadlist.end(); ++i)
	{
		const QuadInfo& quad = (*i);
		::tagRECT newrect;
		newrect.top		= quad.clipper.d_top;
		newrect.bottom	= quad.clipper.d_bottom;
/*
		if ( newrect.bottom > nScreenH )
		{
			newrect.bottom = nScreenH;
		}//*/
		newrect.left	= quad.clipper.d_left;
		newrect.right	= quad.clipper.d_right;
/*
		if ( newrect.right > nScreenW )
		{
			newrect.right = nScreenW;
		}//*/

//		continue;

		g_pCanvas->SetClipRect( &newrect );

		quad.pSpr->DrawAlphaEx( quad.position.d_left - quad.texPosition.d_left, quad.position.d_top - quad.texPosition.d_top, 0, quad.scalex, quad.scaley, quad.position.getWidth(), quad.position.getHeight(), quad.frame, quad.alpha*255);//*/
		if ( (const_cast<Texture*>(quad.texture))->getBuffer() )
 		{
			DirectX7Texture * ptex = (DirectX7Texture*)quad.texture;
			uchar * buffer = (uchar*)ptex->getBuffer() + (int)quad.texPosition.d_top * (int)ptex->getWidth() + (int)quad.texPosition.d_left ;
			g_pCanvas->DrawFontEx( quad.position.d_left , quad.position.d_top ,
 				quad.texPosition.getWidth(), quad.texPosition.getHeight(), ptex->getWidth(), quad.rectColour.d_top_left.getARGB(), 255*(quad.rectColour.d_top_left.getAlpha()), (void*)buffer );	


		}//*/

	}
	g_pCanvas->SetClipRect( &oldrect );
}


/*************************************************************************
	clear the queue
*************************************************************************/
void DirectX7Renderer::clearRenderList(void)
{
	d_quadlist.clear();
}


/*************************************************************************
	create an empty texture
*************************************************************************/
Texture* DirectX7Renderer::createTexture(void)
{
	DirectX7Texture* tex = new DirectX7Texture(this);
	d_texturelist.push_back(tex);
	return tex;
}


/*************************************************************************
	Create a new Texture object and load a file into it.
*************************************************************************/
Texture* DirectX7Renderer::createTexture(const String& filename, const String& resourceGroup)
{
	DirectX7Texture* tex = (DirectX7Texture*)createTexture();
	tex->loadFromFile(filename, resourceGroup);
	d_texturelist.push_back(tex);
	return tex;
}

Texture*
DirectX7Renderer::createTextureFromMemory( void* pBuffer )
{
	DirectX7Texture* tex = (DirectX7Texture*)createTexture();
	tex->loadFromMemory( pBuffer );
	d_texturelist.push_back(tex);
	return tex;
}


/*************************************************************************
	Create a new texture with the given dimensions
*************************************************************************/
Texture* DirectX7Renderer::createTexture(float size)
{
	DirectX7Texture* tex = (DirectX7Texture*)createTexture();
//	tex->setD3DTextureSize((uint)size);
	d_texturelist.push_back(tex);
	return tex;
}

/*************************************************************************
	Destroy a texture
*************************************************************************/
void DirectX7Renderer::destroyTexture(Texture* texture)
{
	if (texture != NULL)
	{
		d_texturelist.remove(texture);
		delete texture;
		texture = NULL;
	}

}


/*************************************************************************
	destroy all textures still active
*************************************************************************/
void DirectX7Renderer::destroyAllTextures(void)
{
	while (!d_texturelist.empty())
	{
		destroyTexture(*(d_texturelist.begin()));
	}
}



/*************************************************************************
	sort quads list according to texture
*************************************************************************/
void DirectX7Renderer::sortQuads(void)
{
}

void DirectX7Renderer::renderDirect(const Point& pos, const Texture* tex, int alpha )
{
	int nScreenW = g_GetScreenWidth();
	int nScreenH = g_GetScreenHeight();
	::tagRECT oldrect;
	g_pCanvas->GetClipRect( &oldrect );
	::tagRECT newrect;
	newrect.top		= 0;
	newrect.bottom	= nScreenH;
	newrect.left	= 0;
	newrect.right	= nScreenW;
	g_pCanvas->SetClipRect( &newrect );

/*	if(const_cast<Texture*>(tex)->getPitch() == 2)
	{
		g_pCanvas->DrawBitmap16Alpha(pos.d_x, pos.d_y, tex->getWidth(), tex->getHeight(), tex->getBuffer());
	}
	else if(const_cast<Texture*>(tex)->getPitch() == 3)
	{
		g_pCanvas->DrawBitmap24(pos.d_x, pos.d_y, tex->getWidth(), tex->getHeight(), tex->getBuffer());
	}*/

	if(const_cast<Texture*>(tex)->getType()== argb565_texture_dxFontSurface||
		const_cast<Texture*>(tex)->getType() == argb565_texture_dxSurface)
	{
		RECT dest,src;
		RECT clipRect= {0,0,KWin32App::m_uScreenWidth,KWin32App::m_uScreenHeight};
		dest.left = pos.d_x;
		dest.right = pos.d_x + tex->getWidth();
		dest.top = pos.d_y;
		dest.bottom = pos.d_y+tex->getHeight();
		int dx = 0,dy = 0;
		int width = 0,height = 0;
		if(dest.left<clipRect.left)
		{
			dx = clipRect.left - dest.left;
			dest.left = clipRect.left;
		}
		if(dest.right>clipRect.right)
			dest.right = clipRect.right;
		if(dest.top<clipRect.top)
		{
			dy = clipRect.top - dest.top;
			dest.top = clipRect.top;
		}
		if(dest.bottom>clipRect.bottom)
			dest.bottom = clipRect.bottom;
		width = dest.right - dest.left;
		height = dest.bottom - dest.top;
		if(width<=0||height<=0)
			return;
		src.left = dx;
		src.right = dx + width;
		src.top = dy;
		src.bottom = dy + height;
		g_pCanvas->BltBitmapFastToBackBuffer((LPDIRECTDRAWSURFACE)tex->getSurface(),&dest,&src,true);


	}
	else
	{
		if(const_cast<Texture*>(tex)->getPitch() == 2)
		{
			g_pCanvas->DrawBitmap16Alpha(pos.d_x, pos.d_y, tex->getWidth(), tex->getHeight(), tex->getBuffer());
		}
		else if(const_cast<Texture*>(tex)->getPitch() == 3)
		{
			g_pCanvas->DrawBitmap24(pos.d_x, pos.d_y, tex->getWidth(), tex->getHeight(), tex->getBuffer());
		}

		g_pCanvas->SetClipRect( &oldrect );
	}
}

void DirectX7Renderer::renderQuadDirect(RenderData* renderData)
{
	int nScreenW = g_GetScreenWidth();
	int nScreenH = g_GetScreenHeight();

	::tagRECT oldrect;
	g_pCanvas->GetClipRect( &oldrect );

	::tagRECT newrect;
	newrect.top		= renderData->clipper.d_top;
	newrect.bottom	= renderData->clipper.d_bottom;
	newrect.left	= renderData->clipper.d_left;
	newrect.right	= renderData->clipper.d_right;
	

	g_pCanvas->SetClipRect( &newrect );	
	RectEx * tex_rect = (RectEx *)&renderData->texture_rect;

	((DirectX7Texture*)renderData->tex)->getSpr()->DrawAlphaEx( renderData->dest_rect.d_left - renderData->texture_rect.d_left, 
		renderData->dest_rect.d_top - renderData->texture_rect.d_top, 
		0, 1, 1,  renderData->dest_rect.getWidth(),  renderData->dest_rect.getHeight(), renderData->frame, renderData->alpha);//*/

	if ((const_cast<Texture*>(renderData->tex))->getBuffer())
	{
		DirectX7Texture * ptex = (DirectX7Texture*)renderData->tex;
		uchar * buffer = (uchar*)ptex->getBuffer() + (int)renderData->texture_rect.d_top * (int)ptex->getWidth() + (int)renderData->texture_rect.d_left;
		
		if ( !g_IsFontWithBorder() )
			renderData->showBorder = false;
		
		if (renderData->showBorder)
		{
			g_pCanvas->DrawFontBorderEx(renderData->dest_rect.d_left, renderData->dest_rect.d_top,
						renderData->texture_rect.getWidth(), renderData->texture_rect.getHeight(), ptex->getWidth(), 
						renderData->imageColor.getARGB(), 
						renderData->borderColor.getARGB(), 
						renderData->alpha, buffer);
		}
		else
		{
			g_pCanvas->DrawFontEx( renderData->dest_rect.d_left, renderData->dest_rect.d_top,
						renderData->texture_rect.getWidth(), renderData->texture_rect.getHeight(), ptex->getWidth(), 
						renderData->imageColor.getARGB(), 
						renderData->alpha, buffer);
		}

	}//*/
	g_pCanvas->SetClipRect( &oldrect );
}


/*************************************************************************
	Set the size of the display in pixels.	
*************************************************************************/
void DirectX7Renderer::setDisplaySize(const Size& sz)
{
	if (d_display_area.getSize() != sz)
	{
		d_display_area.setSize(sz);

		EventArgs args;
		fireEvent(Window::EventDisplaySizeChanged, args);
	}

}


} // End of  CEGUI namespace section

