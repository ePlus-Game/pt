/************************************************************************
	filename: 	dxdraw7texture.cpp
	created:	22/5/2006
	author:		SamSun
	
	purpose:	Implements texture class for directdraw 7
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
#include "renderers/directx7GUIRenderer/dxdraw7texture.h"
#include "renderers/directx7GUIRenderer/dxdraw7renderer.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"
#include "ImageOperation.h"
#include "KColors.h"
#include "KWin32Wnd.h"
#undef max

// Start of CEGUI namespace section
namespace CEGUI
{

#define SHIFTNUM_RGB565_RCOLOR		11
#define SHIFTNUM_RGB565_GCOLOR		5

#define SHIFTNUM_RGB555_RCOLOR		10
#define SHIFTNUM_RGB555_GCOLOR		5

#define RGB1555_FROM_ARGB(Pixel, a, r, g, b)					\
{				\
	Pixel = (a<<15)&((r>>3)<<10)|((g>>3)<<5)|(b>>3);			\
}

#define RGB565_FROM_RGB(Pixel, r, g, b)					\
{									\
	Pixel = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);			\
}
#define RGB555_FROM_RGB(Pixel, r, g, b)					\
{									\
	Pixel = ((r>>3)<<10)|((g>>3)<<5)|(b>>3);			\
}

#define RGB_FROM_RGB555(Pixel, r, g, b)					\
{									\
	r = (((Pixel&0x7C00)>>10)<<3);		 			\
	g = (((Pixel&0x03E0)>>5)<<3); 					\
	b = ((Pixel&0x001F)<<3); 					\
}
#define RGB_FROM_RGB565(Pixel, r, g, b)					\
{									\
	r = (((Pixel&0xF800)>>11)<<3);		 			\
	g = (((Pixel&0x07E0)>>5)<<2); 					\
	b = ((Pixel&0x001F)<<3); 					\
}

list<void*>  DirectX7Texture::textureList;
/*************************************************************************
	Constructor
*************************************************************************/
DirectX7Texture::DirectX7Texture(Renderer* owner) :
	Texture(owner)
{
	d_drawmem	= NULL;
	d_scalex	= 1;		//!< cached width of the texture
	d_scaley	= 1;		//!< cached width of the texture
	d_pitch		= 0;
	d_lpDrawmenSurface = 0;
	d_pSurface = 0;
	surfacePitch = 0;
	m_bIsSpr = false;
	m_pBitmap = NULL;
}

/*************************************************************************
	Destructor
*************************************************************************/
DirectX7Texture::~DirectX7Texture(void)
{
	
	freeTexture();
}
//add by render
void DirectX7Texture::lockBuffer()
{
	if(d_lpDrawmenSurface)
	{
		if(d_pSurface == 0)
		{
			DDSURFACEDESC ddsd;
			memset(&ddsd,0,sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			if(FAILED(d_lpDrawmenSurface->Lock(NULL,&ddsd,DDLOCK_WAIT|DDLOCK_SURFACEMEMORYPTR,NULL)))
				return;
			d_pSurface = ddsd.lpSurface;
			surfacePitch = ddsd.lPitch;
		}
	}
}
void DirectX7Texture::ReCreateTexture()
{
	if(d_lpDrawmenSurface)
		releaseTexture();
	{
		if(d_width == 0||d_height == 0)
			return;
		d_lpDrawmenSurface = g_pDirectDraw->CreateSurfaceEx(d_width,d_height,DDSCAPS_SYSTEMMEMORY,g_RGB565(255,0,255));
		if(d_lpDrawmenSurface)
		{
			DDBLTFX ddblt;
			memset(&ddblt,0,sizeof(ddblt));
			ddblt.dwSize = sizeof(ddblt);
			ddblt.dwFillColor = g_RGB565(255,0,255);
			d_lpDrawmenSurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&ddblt);
		}
	}
}
void DirectX7Texture::releaseTexture()
{
	if(d_lpDrawmenSurface)
	{
		d_lpDrawmenSurface->Release();
		d_lpDrawmenSurface = 0;
	}
}
void DirectX7Texture::unlockBuffer()
{
	if(d_lpDrawmenSurface)
	{
		if(d_pSurface)
		{
			d_lpDrawmenSurface->Unlock(NULL);
			

		}
	}
	d_pSurface = 0;
	surfacePitch = 0;
}
///
void	DirectX7Texture::createTexture(uint buffWidth, uint buffHeight, uint pitch,TextureType type)
{
	if ( buffHeight == 0 || buffWidth == 0 )
	{
		return;
	}
	if(type == argb565_texture_dxSurface || type == argb565_texture_dxFontSurface)
	{
		if(!d_lpDrawmenSurface)
		{
			d_lpDrawmenSurface = g_pDirectDraw->CreateSurfaceEx(buffWidth,buffHeight,DDSCAPS_SYSTEMMEMORY,g_RGB565(255,0,255));
			if(d_lpDrawmenSurface)
			{
				
				textureList.push_back(this);
				d_type = type;
				DDBLTFX ddbltfx;
				memset(&ddbltfx,0,sizeof(ddbltfx));
				ddbltfx.dwSize = sizeof(ddbltfx);
				ddbltfx.dwFillColor = g_RGB565(255,0,255);
				d_width = buffWidth;
				d_height = buffHeight;
				d_lpDrawmenSurface->Blt(NULL,NULL,NULL,DDBLT_WAIT|DDBLT_COLORFILL,&ddbltfx);

			}
		}
	}
	else
	{
		if ( !d_drawmem )
		{
			int textLen	= buffWidth * pitch * buffHeight;
			d_drawmem	= new uchar[textLen];
			d_pitch		= pitch;
			d_type		= type;
			if ( d_drawmem )
			{
				// store new size;
				d_width		= buffWidth;
				d_height	= buffHeight;
				
				memset( d_drawmem, 0xff, textLen * sizeof(uchar) );
			}
			else
			{
				throw RendererException((utf8*)"Failed to create texture .");
			}
			
		}
	}
}

/*************************************************************************
	Load texture from file.  Texture is made to be same size as image in
	file.
*************************************************************************/
void DirectX7Texture::loadFromFile(const String& filename, const String& resourceGroup)
{
	freeTexture();
	if( FileStringCmp( const_cast<char*>( filename.c_str( ) ) , ".spr" ) )
	{
		d_sprite.Load( const_cast<char*>(filename.c_str()) );
		d_height = d_sprite.GetHeight();
		d_width	 = d_sprite.GetWidth();	
		m_bIsSpr = true;
	}
	else
	if( FileStringCmp( const_cast<char*>( filename.c_str( ) ) , ".bmp" ) )	
	{
		m_pBitmap = new Bitmap;
		m_pBitmap->Load( const_cast<char*>( filename.c_str( ) )  );
		d_width		= m_pBitmap->GetWidth( );
		d_height    = m_pBitmap->GetHeight( );
		m_bIsSpr    = false;
	}
//	d_sprite.Load( const_cast<char*>(filename.c_str()) );
//	d_height = d_sprite.GetHeight();
//	d_width	 = d_sprite.GetWidth();		 	
}

void 
DirectX7Texture::loadFromMemory( void* pBuffer ,bool bIsSpr /* = false  */)

{
	if( pBuffer == NULL )
	{
		return ;
	}
	m_bIsSpr = bIsSpr;
	if( bIsSpr == false )
	{
		m_pBitmap = new Bitmap;
		m_pBitmap->LoadFromMemory( pBuffer );
		d_width = m_pBitmap->GetWidth( );
		d_height = m_pBitmap->GetHeight( );	
	}
	else
	{
		//for bmp only now;
		return;
	}
}

bool   
DirectX7Texture::FileStringCmp( const char* pFileName, const char* pFormat )
{
	if( pFormat == NULL || pFileName == NULL )
	{
		return false;
	}

	int nFileStrLen = strlen( pFileName );
	int nFormatStrLen = strlen( pFormat );
	if( nFileStrLen <= nFormatStrLen )
	{
		return false;
	}
	if( pFormat[0] != '.' ||
		pFormat[0] != pFileName[ nFileStrLen  - nFormatStrLen]  )
	{
		return false;
	}
	int nFormatIdx = nFormatStrLen - 1 ;
	int nFileIdx = nFileStrLen - 1;
	for(  ; nFormatIdx >= 0; --nFormatIdx,--nFileIdx )
	{
		if( tolower( pFileName[ nFileIdx ] ) != tolower( pFormat[ nFormatIdx ] ) )
		{
			return false;
		}

	}
	return true;
}

/*************************************************************************
	safely release texture associated with this Texture
*************************************************************************/
void DirectX7Texture::freeTexture(void)
{
	if (d_drawmem != NULL)
	{
		delete [] d_drawmem;
		d_drawmem = NULL;
	}

	d_sprite.Free();
	if(d_lpDrawmenSurface)
	{
		std::list<void*>::iterator iter = 0;
		for(iter = textureList.begin();iter!=textureList.end();iter++)
		{
			void* addressValue = *iter;
			if(addressValue == (void*)this)
			{
				textureList.erase(iter);
				break;
			}

		}
		d_lpDrawmenSurface->Release();
		d_lpDrawmenSurface = NULL;
	}
	if( m_pBitmap )
	{
		delete m_pBitmap;
	}
}

bool DirectX7Texture::makeClip(long nX, long nY, long nWidth, long nHeight, _clipper* pClipper)
{
	// 初始化裁减量
	pClipper->x = nX;
	pClipper->y = nY;
	pClipper->width = nWidth;
	pClipper->height = nHeight;
	pClipper->top = 0;
	pClipper->left = 0;
	pClipper->right = 0;

	// 上边界裁减
	if (pClipper->y < d_clipRect.top)
	{
		pClipper->y = d_clipRect.top;
		pClipper->top = d_clipRect.top - nY;
		pClipper->height -= pClipper->top;
	}
	if (pClipper->height <= 0)
		return FALSE;
	
	// 下边界裁减
	if (pClipper->height > d_clipRect.bottom - pClipper->y)
	{
		pClipper->height = d_clipRect.bottom - pClipper->y;
	}
	if (pClipper->height <= 0)
		return FALSE;
	
	// 左边界裁减
	if (pClipper->x < d_clipRect.left)
	{
		pClipper->x = d_clipRect.left;
		pClipper->left = d_clipRect.left - nX;
		pClipper->width -= pClipper->left;
	}
	if (pClipper->width <= 0)
		return FALSE;
	
	// 右边界裁减
	if (pClipper->width > d_clipRect.right - pClipper->x)
	{
		pClipper->right = pClipper->width + pClipper->x - d_clipRect.right;
		pClipper->width -= pClipper->right;
	}
	if (pClipper->width <= 0)
		return FALSE;
	
	return TRUE;
}

void DirectX7Texture::updateDbcsChar(uint x, uint y, const void* buffPtr, uint buffWidth, uint buffHeight )
{
	// copy data from buffer into texture
	uchar* dst = d_drawmem + y * d_width + x;
	ulong* src = (ulong*)buffPtr;

	for (uint i = 0; i < buffHeight; ++i)
	{
		uchar *tmp = dst;
		for(uint j = 0; j < buffWidth; ++j, src++) {
			uchar *pixel = (uchar *)src;
			*tmp++ = *(pixel+3);
			//*tmp++ = *(pixel+2);
		}

		dst += d_width; //next row
	}
}

void DirectX7Texture::updateDbcsCharBorder(uint x, uint y, const void* buffPtr, uint buffWidth, uint buffHeight )
{
	// copy data from buffer into texture
	ushort* dst = (ushort*)(d_drawmem + y * d_width + x);
	ulong* src = (ulong*)buffPtr;

	for (uint i = 0; i < buffHeight; ++i)
	{
		uchar *tmp = (uchar*)dst;
		for(uint j = 0; j < buffWidth; ++j, src++) 
		{
			uchar *pixel = (uchar *)src;
			*tmp++ = *(pixel+3);
			*tmp++ = *(pixel+2);
		}

		dst += d_width; //next row
	}//*/
}

struct _bmp24 
{
	unsigned char byAlpha;
	unsigned short colour;
};

void DirectX7Texture::copySpriteTo24Buf(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette)
{
}

void DirectX7Texture::clearTexture( uchar nColour )
{
	if ( d_drawmem )
	{
		memset(d_drawmem, nColour,sizeof(char)*d_pitch*d_width*d_height );
	}
	if(d_lpDrawmenSurface)
	{
		DDBLTFX ddblt;
		memset(&ddblt,0,sizeof(ddblt));
		ddblt.dwSize = sizeof(ddblt);
		ddblt.dwFillColor = g_RGB565(255,0,255);
		d_lpDrawmenSurface->Blt(NULL,NULL,NULL,DDBLT_COLORFILL|DDBLT_WAIT,&ddblt);
	}
}

bool DirectX7Texture::getFinalImageClipper(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper,  
		_clipper& tagClipper)
{
		// 对绘制区域进行裁剪
	// Get the real draw map
	d_clipRect.left = pos.d_x;
	d_clipRect.right = pos.d_x + clipper.d_width;
	d_clipRect.top = pos.d_y;
	d_clipRect.bottom = pos.d_y + clipper.d_height;
	if( makeClip(pos.d_x - clipper.d_offsetX, pos.d_y - clipper.d_offsetY, 
		size.d_width, size.d_height, &tagClipper ) == 0 )
	{
		return false;
	}

	int leftOff = 0;
	int rightOff = 0;
	int topOff = 0;
	int bottomOff = 0;
	if(tagClipper.x < 0)
	{
		leftOff = -tagClipper.x;
	}
	if(tagClipper.x + tagClipper.width > d_width)
	{
		rightOff = tagClipper.x + tagClipper.width - d_width;
	}
	if(tagClipper.y < 0)
	{
		topOff = -tagClipper.y;
	}
	if(tagClipper.y + tagClipper.height > d_height)
	{
		bottomOff = tagClipper.y + tagClipper.height - d_height;
	}

	tagClipper.x += leftOff;
	tagClipper.y += topOff;

	tagClipper.left += leftOff;
	tagClipper.right += rightOff;
	tagClipper.width -= (leftOff + rightOff);
	tagClipper.top += topOff;
	tagClipper.height -= (topOff + bottomOff);

	assert(tagClipper.x >= 0);
	assert(tagClipper.y >= 0);
	assert(tagClipper.x + tagClipper.width <= d_width);
	assert(tagClipper.y + tagClipper.height <= d_height);
	return true;
}

void DirectX7Texture::copySpriteTo32Buf(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette)
{
	if ( pSprite == NULL ||d_drawmem == NULL || d_width == 0 || d_height == 0)
	{
		return;
	}

	// Get the real draw map
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}
	
	KPAL24 *pSrcPal = (KPAL24 *)pPalette;
	unsigned char  *pSrcBuf = (unsigned char *)pSprite;
	unsigned short *pDstBuf = (unsigned short*)d_drawmem;
	pDstBuf += tagClipper.y * d_width + tagClipper.x;
	int nBackBufLinePixel = d_width;

	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = size.d_width * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount;
		}
	}

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned short *pDstCurLineHead = pDstBuf;
		nRemainSrcHeight --;

		int nCurLinePos		= 0;
		int nCurUnitEndPos	= 0;
		
		nCurLinePos			= 0;
		nCurUnitEndPos		= tagClipper.left;
		
		// Deal with left spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			// Skip left clip spr data first
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;
			
			int nFrontCount = nSameCount - ( nCurLinePos - nCurUnitEndPos );
			int nLoopCount = (((nCurLinePos - nCurUnitEndPos) > tagClipper.width) ? tagClipper.width : (nCurLinePos - nCurUnitEndPos));
			
			if ( nLoopCount > 0)
			{
				if(nSameAlpha > 0 )
				{
					pSrcBuf += (nFrontCount);

					if ( nSameAlpha == 255 )
					{
						while(nLoopCount > 0)
						{
							int nPalIndex = *pSrcBuf++;
							unsigned short r = pSrcPal[nPalIndex].Red;
							unsigned short g = pSrcPal[nPalIndex].Green;
							unsigned short b = pSrcPal[nPalIndex].Blue;
							unsigned short colour1555 = 0;
							RGB555_FROM_RGB( colour1555, r, g, b )
							*pDstBuf++ = colour1555 & 0x7fff;
//							*pDstBuf++ = pSrcPal[*pSrcBuf++] & 0x7fff;
							nLoopCount--;
						}					
					}
					else
					{
						while(nLoopCount > 0)
						{
							int nPalIndex = *pSrcBuf++;
							unsigned short r = pSrcPal[nPalIndex].Red;
							unsigned short g = pSrcPal[nPalIndex].Green;
							unsigned short b = pSrcPal[nPalIndex].Blue;
							unsigned short colour1555 = 0;
							RGB555_FROM_RGB( colour1555, r, g, b )
							unsigned short usnSrcClr = colour1555;
							unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
							unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
							unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

							unsigned short usnDstClr = *pDstBuf;
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
 							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
							*pDstBuf++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB & 0x7fff;

							/*
							unsigned short usnSrcClr = pSrcPal[*pSrcBuf++];
							unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
							unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
							unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

							unsigned short usnDstClr = *pDstBuf;
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
 							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
							*pDstBuf++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB & 0x7fff;//*/
							nLoopCount--;
						}
					}//*/
					if ( (nCurLinePos - nCurUnitEndPos) > tagClipper.width )
					{
						pSrcBuf += (nSameCount - nFrontCount - tagClipper.width);
					}//*/
				}
				else
				{
					pDstBuf += nLoopCount;
				}					
			}
			else
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += nSameCount;
				}
			}
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;

			int nSameCountLoop = 0;
			if ( nCurLinePos > nCurUnitEndPos )
			{
				nSameCountLoop = nSameCount - (nCurLinePos - nCurUnitEndPos);
			}
			else
			{
				nSameCountLoop = nSameCount;
			}
			if(nSameAlpha > 0 )
			{
				if ( nSameAlpha == 255 )
				{
					while(nSameCountLoop > 0)
					{
						int nPalIndex = *pSrcBuf++;
						unsigned short r = pSrcPal[nPalIndex].Red;
						unsigned short g = pSrcPal[nPalIndex].Green;
						unsigned short b = pSrcPal[nPalIndex].Blue;
						unsigned short colour1555 = 0;
						RGB555_FROM_RGB( colour1555, r, g, b )
						*pDstBuf++ = colour1555 & 0x7fff;

						//*pDstBuf++ = pSrcPal[*pSrcBuf++] & 0x7fff;
						nSameCountLoop--;
					}
				}
				else
				{
					while ( nSameCountLoop > 0 )
					{
						int nPalIndex = *pSrcBuf++;
						unsigned short r = pSrcPal[nPalIndex].Red;
						unsigned short g = pSrcPal[nPalIndex].Green;
						unsigned short b = pSrcPal[nPalIndex].Blue;
						unsigned short colour1555 = 0;
						RGB555_FROM_RGB( colour1555, r, g, b )
						unsigned short usnSrcClr = colour1555;
						unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
						unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
						unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

						unsigned short usnDstClr = *pDstBuf;
						unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
 						unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
						*pDstBuf++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB & 0x7fff;

						/*
						unsigned short usnSrcClr = pSrcPal[*pSrcBuf++];
						unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
						unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
						unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

						unsigned short usnDstClr = *pDstBuf;
						unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
 						unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
						*pDstBuf++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB & 0x7fff;//*/
						nSameCountLoop--;
					}
				}//*/
				if ( nCurLinePos - nCurUnitEndPos > 0)
				{
					pSrcBuf += (nCurLinePos - nCurUnitEndPos);
				}
				
			}
			else
			{
				pDstBuf += nSameCount;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0)
			{
				pSrcBuf += nSameCount;
			}
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}
}

void DirectX7Texture::copySpriteTo32BufNoAlpha(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette)
{
	if ( pSprite == NULL ||d_drawmem == NULL || d_width == 0 || d_height == 0)
	{
		return;
	}

	// Get the real draw map
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}
	
	KPAL24 *pSrcPal			= (KPAL24 *)pPalette;
	unsigned char  *pSrcBuf = (unsigned char *)pSprite;
	unsigned short *pDstBuf = (unsigned short*)d_drawmem;
	pDstBuf += tagClipper.y * d_width + tagClipper.x;
	int nBackBufLinePixel = d_width;

	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = size.d_width * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount;
		}
	}

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned short *pDstCurLineHead = pDstBuf;
		nRemainSrcHeight --;

		int nCurLinePos		= 0;
		int nCurUnitEndPos	= 0;
		
		nCurLinePos			= 0;
		nCurUnitEndPos		= tagClipper.left;
		
		// Deal with left spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			// Skip left clip spr data first
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;

			int nFrontCount = nSameCount - ( nCurLinePos - nCurUnitEndPos );
			int nLoopCount = (((nCurLinePos - nCurUnitEndPos) > tagClipper.width) ? tagClipper.width : (nCurLinePos - nCurUnitEndPos));

			if ( nLoopCount > 0)
			{
				if(nSameAlpha > 0 )
				{
					pSrcBuf += (nFrontCount);

					while(nLoopCount > 0)
					{
						int nPalIndex = *pSrcBuf++;
						unsigned short r = pSrcPal[nPalIndex].Red;
						unsigned short g = pSrcPal[nPalIndex].Green;
						unsigned short b = pSrcPal[nPalIndex].Blue;
						unsigned short colour1555 = 0;
						RGB555_FROM_RGB( colour1555, r, g, b )
						*pDstBuf++ = colour1555 & 0x7fff;

						//*pDstBuf++ = pSrcPal[*pSrcBuf++] & 0x7fff;
						nLoopCount--;
					}
					if ( (nCurLinePos - nCurUnitEndPos) > tagClipper.width )
					{
						pSrcBuf += ((nSameCount - nFrontCount - tagClipper.width));
					}//*/
				}
				else
				{
					pDstBuf += nLoopCount;
				}	
				
			}
			else
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += nSameCount;
				}
			}
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;

			int nSameCountLoop = 0;
			if ( nCurLinePos > nCurUnitEndPos )
			{
				nSameCountLoop = nSameCount - (nCurLinePos - nCurUnitEndPos);
			}
			else
			{
				nSameCountLoop = nSameCount;
			}
			if(nSameAlpha > 0 )
			{
				while(nSameCountLoop > 0)
				{
					int nPalIndex = *pSrcBuf++;
					unsigned short r = pSrcPal[nPalIndex].Red;
					unsigned short g = pSrcPal[nPalIndex].Green;
					unsigned short b = pSrcPal[nPalIndex].Blue;
					unsigned short colour1555 = 0;
					RGB555_FROM_RGB( colour1555, r, g, b )
					*pDstBuf++ = colour1555 & 0x7fff;
					//*pDstBuf++ = pSrcPal[*pSrcBuf++] & 0x7fff;
					nSameCountLoop--;
				}
				if ( nCurLinePos - nCurUnitEndPos > 0)
				{
					pSrcBuf += (nCurLinePos - nCurUnitEndPos);
				}
				
			}
			else
			{
				pDstBuf += nSameCount;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0)
			{
				pSrcBuf += nSameCount;
			}
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}
}

void DirectX7Texture::copy16SpriteTo32Buf(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite)
{
	if ( pSprite == NULL ||d_drawmem == NULL || d_width == 0 || d_height == 0)
	{
		return;
	}

	// Get the real draw map
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}
	
	unsigned char  *pSrcBuf = (unsigned char *)pSprite;
	unsigned short *pDstBuf = (unsigned short *)d_drawmem;
	pDstBuf += tagClipper.y * d_width + tagClipper.x;
	int nBackBufLinePixel = d_width;
	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = size.d_width * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount * 2;
		}
	}

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned short *pDstCurLineHead = pDstBuf;
		nRemainSrcHeight --;

		int nCurLinePos		= 0;
		int nCurUnitEndPos	= 0;
		
		nCurLinePos			= 0;
		nCurUnitEndPos		= tagClipper.left;
		
		// Deal with left spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			// Skip left clip spr data first
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;
			
			int nFrontCount = nSameCount - ( nCurLinePos - nCurUnitEndPos );
			int nLoopCount = (((nCurLinePos - nCurUnitEndPos) > tagClipper.width) ? tagClipper.width : (nCurLinePos - nCurUnitEndPos));
			if ( nLoopCount > 0)
			{
				if(nSameAlpha > 0 )
				{				  
					pSrcBuf += (nFrontCount*2);
					if (nSameAlpha == 255)
					{
						while(nLoopCount > 0)
						{
							unsigned short r = 0;
							unsigned short g = 0;
							unsigned short b = 0;
							unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							RGB_FROM_RGB565( usnSrcClr, r, g, b )
							unsigned short colour1555 = 0;
							RGB555_FROM_RGB( colour1555, r, g, b )
							*pDstBuf++ = colour1555  & 0x7fff;;
							nLoopCount--;
						}
					}
					else
					{
						while(nLoopCount > 0)
						{
							unsigned short r = 0;
							unsigned short g = 0;
							unsigned short b = 0;
							unsigned short usnColour = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							RGB_FROM_RGB565( usnColour, r, g, b )
							unsigned short usnSrcClr = 0;
							RGB555_FROM_RGB( usnSrcClr, r, g, b )

							unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
							unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
							unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

							unsigned short usnDstClr = *pDstBuf;
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
 							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
							*pDstBuf++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB  & 0x7fff;;
							nLoopCount--;//*/
						}
					}
					if ( (nCurLinePos - nCurUnitEndPos) > tagClipper.width )
					{
						pSrcBuf += ((nSameCount - nFrontCount - tagClipper.width) * 2);
					}//*/
				}
				else
				{
					pDstBuf += nLoopCount;
				}	
			}
			else
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += nSameCount*2;
				}
			}
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;
			int nSameCountLoop = 0;
			if ( nCurLinePos > nCurUnitEndPos )
			{
				nSameCountLoop = nSameCount - (nCurLinePos - nCurUnitEndPos);
			}
			else
			{
				nSameCountLoop = nSameCount;
			}
			if(nSameAlpha > 0 )
			{
				if ( nSameAlpha == 255 )
				{
					while(nSameCountLoop > 0)
					{
						unsigned short r = 0;
						unsigned short g = 0;
						unsigned short b = 0;
						unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;
						RGB_FROM_RGB565( usnSrcClr, r, g, b )
						unsigned short colour1555 = 0;
						RGB555_FROM_RGB( colour1555, r, g, b )
						*pDstBuf++ = colour1555  & 0x7fff;;
						nSameCountLoop--;
					}
				}
				else
				{
					while(nSameCountLoop > 0)
					{	
						unsigned short r = 0;
						unsigned short g = 0;
						unsigned short b = 0;
						unsigned short usnColour = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;
						RGB_FROM_RGB565( usnColour, r, g, b )
						unsigned short usnSrcClr = 0;
						RGB555_FROM_RGB( usnSrcClr, r, g, b )

						unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
						unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
						unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

						unsigned short usnDstClr = *pDstBuf;
						unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
 						unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
						*pDstBuf++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB & 0x7fff;;
						nSameCountLoop--;
					}
				}
				if ( nCurLinePos - nCurUnitEndPos > 0)
				{
					pSrcBuf += (nCurLinePos - nCurUnitEndPos)*2;
				}
				
			}
			else
			{
				pDstBuf += nSameCount;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0)
			{
				pSrcBuf += nSameCount*2;
			}
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}	
}

void DirectX7Texture::copy16SpriteTo32BufNoAlpha(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite)
{
	if ( pSprite == NULL ||d_drawmem == NULL || d_width == 0 || d_height == 0)
	{
		return;
	}

	// Get the real draw map
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}
	
	unsigned char  *pSrcBuf = (unsigned char *)pSprite;
	unsigned short *pDstBuf = (unsigned short *)d_drawmem;
	pDstBuf += tagClipper.y * d_width + tagClipper.x;
	int nBackBufLinePixel = d_width;
	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = size.d_width * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount * 2;
		}
	}

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned short *pDstCurLineHead = pDstBuf;
		nRemainSrcHeight --;

		int nCurLinePos		= 0;
		int nCurUnitEndPos	= 0;
		
		nCurLinePos			= 0;
		nCurUnitEndPos		= tagClipper.left;
		
		// Deal with left spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			// Skip left clip spr data first
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;

			int nFrontCount = nSameCount - ( nCurLinePos - nCurUnitEndPos );
			int nLoopCount = (((nCurLinePos - nCurUnitEndPos) > tagClipper.width) ? tagClipper.width : (nCurLinePos - nCurUnitEndPos));

			if ( nLoopCount > 0)
			{
				if(nSameAlpha > 0 )
				{
					pSrcBuf += (nFrontCount*2);
					while(nLoopCount > 0)
					{
						unsigned short r = 0;
						unsigned short g = 0;
						unsigned short b = 0;
						unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;
						RGB_FROM_RGB565( usnSrcClr, r, g, b )
						unsigned short colour1555 = 0;
						RGB555_FROM_RGB( colour1555, r, g, b )
						*pDstBuf++ = colour1555 & 0x7fff;
						nLoopCount--;
					}
					if ( (nCurLinePos - nCurUnitEndPos) > tagClipper.width )
					{
						pSrcBuf += ((nSameCount - nFrontCount - tagClipper.width) * 2);
					}//*/
				}
				else
				{
					pDstBuf += nLoopCount;
				}					
			}
			else
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += nSameCount*2;
				}
			}
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;
			int nSameCountLoop = 0;
			if ( nCurLinePos > nCurUnitEndPos )
			{
				nSameCountLoop = nSameCount - (nCurLinePos - nCurUnitEndPos);
			}
			else
			{
				nSameCountLoop = nSameCount;
			}
			if(nSameAlpha > 0 )
			{
				while(nSameCountLoop > 0)
				{
					unsigned short r = 0;
					unsigned short g = 0;
					unsigned short b = 0;
					unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
					pSrcBuf+=2;
					RGB_FROM_RGB565( usnSrcClr, r, g, b )
					unsigned short colour1555 = 0;
					RGB555_FROM_RGB( colour1555, r, g, b )
					*pDstBuf++ = colour1555 & 0x7fff;;
					nSameCountLoop--;
				}
				if ( nCurLinePos - nCurUnitEndPos > 0)
				{
					pSrcBuf += (nCurLinePos - nCurUnitEndPos)*2;
				}
				
			}
			else
			{
				pDstBuf += nSameCount;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0)
			{
				pSrcBuf += nSameCount*2;
			}
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}	
}

void DirectX7Texture::copyFontTo32Buf(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, const TextureFont& font,  bool underLine )
{
	if ( font.d_font == NULL ||d_drawmem == NULL )
	{
		return;
	}
	unsigned short* lpBuffer = (unsigned short*)d_drawmem;
	unsigned char*	lpBitmap = (unsigned char*)font.d_font;

	BYTE r = (BYTE)(font.d_fontcolour >> 16);
	BYTE g = (BYTE)(font.d_fontcolour >> 8);
	BYTE b = (BYTE)font.d_fontcolour;
	unsigned short fontcolor16 = 0;	
	unsigned short fontboldcolor16 = 0;
	//unsigned short fontboldcolor16 =((53 >> 3) << 11) | ((30 >> 2) << 5) | (12>>3);
	RGB555_FROM_RGB( fontcolor16, r, g, b)

	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}

	if ( tagClipper.x + tagClipper.width + 1 >= d_width )
	{
		tagClipper.width = d_width - tagClipper.x - 1;
	}

	if ( tagClipper.y + tagClipper.height + 1 >= d_height )
	{
		tagClipper.height = d_height - tagClipper.y - 1;
	}

	// 计算屏幕下一行的偏移
	long ScreenOffset = d_width - tagClipper.width;

	// 计算位图下一行的偏移
	long BitmapOffset = size.d_width - tagClipper.width;
	
	int nLoopY = 0;
	int nLoopX = 0;

	//正常绘制
	lpBuffer = (unsigned short*)d_drawmem;
	lpBitmap = (unsigned char*)font.d_font;

	lpBuffer += (d_width * tagClipper.y + tagClipper.x );
	lpBitmap += (size.d_width * tagClipper.top + tagClipper.left );


	for ( nLoopY = 0; nLoopY < tagClipper.height; ++nLoopY )
	{
		for ( nLoopX = 0; nLoopX < tagClipper.width; ++nLoopX )
		{
			int nSameAlpha = *lpBitmap++;
			if ( nLoopY == tagClipper.height - 1 && underLine )
			{
				nSameAlpha = 255;
			}
			nSameAlpha = (nSameAlpha * font.d_alpha) / 255;
			if (  nSameAlpha )
			{
				if ( nSameAlpha >= 255)
				{
					unsigned short usnSrcClr = fontcolor16;
 					unsigned short usnSrcR = (usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F;
					unsigned short usnSrcG = (usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F;
					unsigned short usnSrcB = usnSrcClr & 0x001F;

					*lpBuffer++ = (usnSrcR << SHIFTNUM_RGB555_RCOLOR) | (usnSrcG << SHIFTNUM_RGB555_GCOLOR) | usnSrcB;
					// 字体右下加边 ...Begin
					if ( g_IsFontWithBorder() && tagClipper.x+tagClipper.width < d_width )
					{
						lpBuffer += (ScreenOffset+tagClipper.width);
						*lpBuffer = fontboldcolor16;
						lpBuffer -= (ScreenOffset+tagClipper.width);//*/
					}
					// 字体右下加边 ...End --刘思亮
				}
				else
				{
					unsigned short usnSrcClr = fontcolor16;
 					unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
					unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
					unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

					unsigned short usnDstClr = *lpBuffer;
					int nSameAlphaD = 255 - nSameAlpha;
					unsigned short usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
					unsigned short usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
					unsigned short usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
					*lpBuffer++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB;
				
					// 字体右下加边 ...Begin
					if ( g_IsFontWithBorder() && tagClipper.x+tagClipper.width < d_width )
					{
						lpBuffer += (ScreenOffset+tagClipper.width);
						usnSrcClr = fontboldcolor16;
 						usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
						usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
						usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

						usnDstClr = *lpBuffer;
						nSameAlphaD = 255 - nSameAlpha;
						usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
						usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
						usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
						*lpBuffer = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB;
						
						lpBuffer -= (ScreenOffset+tagClipper.width);//*/
					}
					// 字体右下加边 ...End --刘思亮
				}
			}
			else
			{
				lpBuffer++;
			}
 
		}
		lpBuffer += ScreenOffset;
		lpBitmap += BitmapOffset;
	}
}

void DirectX7Texture::copyFontTo32BufBorder(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, const TextureFont& font,  bool underLine )
{
	if ( font.d_font == NULL ||d_drawmem == NULL )
	{
		return;
	}
	unsigned short* lpBuffer = (unsigned short*)d_drawmem;
	unsigned char*	lpBitmap = (unsigned char*)font.d_font;

	BYTE r = (BYTE)(font.d_fontcolour >> 16);
	BYTE g = (BYTE)(font.d_fontcolour >> 8);
	BYTE b = (BYTE)font.d_fontcolour;
	unsigned short fontcolor16 = 0;	
	unsigned short fontboldcolor16 = 0;
	//unsigned short fontboldcolor16 =((53 >> 3) << 11) | ((30 >> 2) << 5) | (12>>3);
	RGB555_FROM_RGB( fontcolor16, r, g, b)

	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}

	if ( tagClipper.x + tagClipper.width + 1 >= d_width )
	{
		tagClipper.width = d_width - tagClipper.x - 1;
	}

	if ( tagClipper.y + tagClipper.height + 1 >= d_height )
	{
		tagClipper.height = d_height - tagClipper.y - 1;
	}


	// 计算屏幕下一行的偏移
	long ScreenOffset = d_width - tagClipper.width;

	// 计算位图下一行的偏移
	long BitmapOffset = size.d_width - tagClipper.width;
	
	int nLoopY = 0;
	int nLoopX = 0;

	// 边缘绘制 ...Begin
	lpBuffer += (d_width * tagClipper.y + tagClipper.x );
	lpBitmap += (size.d_width * tagClipper.top + tagClipper.left );
	
	for ( nLoopY = 0; nLoopY < tagClipper.height; ++nLoopY )
	{
		for ( nLoopX = 0; nLoopX < tagClipper.width; ++nLoopX )
		{
			int nSameAlpha = *lpBitmap++;
			if ( nLoopY == tagClipper.height - 1 && underLine )
			{
				nSameAlpha = 255;
			}
			nSameAlpha = (nSameAlpha * font.d_alpha) / 255;
			if ( nSameAlpha )
			{
				if ( nSameAlpha >= 255 )
				{
					// 左
					lpBuffer --;
					*lpBuffer = fontboldcolor16;
					lpBuffer ++;
					// 右
					lpBuffer ++;
					*lpBuffer = fontboldcolor16;
					lpBuffer --;
					// 上
					lpBuffer -= (ScreenOffset+tagClipper.width);
					*lpBuffer = fontboldcolor16;
					lpBuffer += (ScreenOffset+tagClipper.width);
					// 下
					lpBuffer += (ScreenOffset+tagClipper.width);
					*lpBuffer++ = fontboldcolor16;
					lpBuffer -= (ScreenOffset+tagClipper.width);
				}
				else
				{
					// 左
					lpBuffer --;
					unsigned short usnSrcClr = fontboldcolor16;
 					unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
					unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
					unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

					unsigned short usnDstClr = *lpBuffer;
					int nSameAlphaD = 255 - nSameAlpha;
					unsigned short usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
					unsigned short usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
					unsigned short usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
					
					*lpBuffer = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB;
					lpBuffer ++;

					// 右
					lpBuffer ++;
					usnSrcClr = fontboldcolor16;
 					usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
					usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
					usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

					usnDstClr = *lpBuffer;
					nSameAlphaD = 255 - nSameAlpha;
					usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
					usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
					usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
					
					*lpBuffer = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB;
					lpBuffer --;

					// 上
					lpBuffer -= (ScreenOffset+tagClipper.width);
					usnSrcClr = fontboldcolor16;
 					usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
					usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
					usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

					usnDstClr = *lpBuffer;
					nSameAlphaD = 255 - nSameAlpha;
					usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
					usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
					usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
					
					*lpBuffer = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB;
					lpBuffer += (ScreenOffset+tagClipper.width);

					// 下
					lpBuffer += (ScreenOffset+tagClipper.width);
					usnSrcClr = fontboldcolor16;
 					usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
					usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
					usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

					usnDstClr = *lpBuffer;
					nSameAlphaD = 255 - nSameAlpha;
					usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
					usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
					usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
					
					*lpBuffer++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB;
					lpBuffer -= (ScreenOffset+tagClipper.width);
				}
			}
			else
			{
				lpBuffer++;
			}
		}
		lpBuffer += ScreenOffset;
		lpBitmap += BitmapOffset;
	}//*/
	// 边缘绘制 ...End --刘思亮

	//正常绘制
	lpBuffer = (unsigned short*)d_drawmem;
	lpBitmap = (unsigned char*)font.d_font;

	lpBuffer += (d_width * tagClipper.y + tagClipper.x );
	lpBitmap += (size.d_width * tagClipper.top + tagClipper.left );

	for ( nLoopY = 0; nLoopY < tagClipper.height; ++nLoopY )
	{
		for ( nLoopX = 0; nLoopX < tagClipper.width; ++nLoopX )
		{
			int nSameAlpha = *lpBitmap++;
			nSameAlpha = (nSameAlpha * font.d_alpha) / 255;
			if (  nSameAlpha )
			{
				if ( nSameAlpha >= 255)
				{
					unsigned short usnSrcClr = fontcolor16;
 					unsigned short usnSrcR = (usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F;
					unsigned short usnSrcG = (usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F;
					unsigned short usnSrcB = usnSrcClr & 0x001F;

					*lpBuffer++ = (usnSrcR << SHIFTNUM_RGB555_RCOLOR) | (usnSrcG << SHIFTNUM_RGB555_GCOLOR) | usnSrcB;
				}
				else
				{
					unsigned short usnSrcClr = fontcolor16;
 					unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
					unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
					unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

					unsigned short usnDstClr = *lpBuffer;
					int nSameAlphaD = 255 - nSameAlpha;
					unsigned short usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
					unsigned short usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
					unsigned short usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
					*lpBuffer++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB;
				}
			}
			else
			{
				lpBuffer++;
			}
		}
		lpBuffer += ScreenOffset;
		lpBitmap += BitmapOffset;
	}
}

void DirectX7Texture::updateGdiDbcsChar(uint offsetx, uint offsety,  uint buffWidth, uint buffHeight, unsigned char* pCharImage, bool border)
{
  // the format of texture is D3DFMT_A4R4G4B4
  int nTexPitch = d_width * d_pitch;
  // clear first
  /*pcDist=d_drawmem;
  
  for(y=-1;y<buffHeight+1;y++)
  {
    memset(pcDist,0,2*(buffWidth+2));
    pcDist+=nTexPitch;
  }//*/
  // draw outline border second
  if(border)
  {
	/*
    int d;
    pcDist=d_drawmem + y * nTexPitch + x * nTexPitch;
    for(y=0;y<buffHeight;y++)
    {
      for(x=0;x<buffWidth;x++)
      {
        if(c=*(pCharImage+y*buffWidth+x))
        {
          d = *(WORD*)(pcDist+2            + 2*(x+1));
          *(WORD*)(pcDist+2            + 2*(x+1)) = d+(255-d)*c/255; // right
          d = *(WORD*)(pcDist+2            + 2*(x-1));
          *(WORD*)(pcDist+2            + 2*(x-1)) = d+(255-d)*c/255; // left
          d = *(WORD*)(pcDist+2            + 2*(x  ));
          *(WORD*)(pcDist+2            + 2*(x  )) = d+(255-d)*c/255; // self
          d = *(WORD*)(pcDist+2 + nTexPitch+ 2*(x  ));
          *(WORD*)(pcDist+2 + nTexPitch+ 2*(x  )) = d+(255-d)*c/255; // down
          d = *(WORD*)(pcDist+2 - nTexPitch+ 2*(x  ));
          *(WORD*)(pcDist+2 - nTexPitch+ 2*(x  )) = d+(255-d)*c/255; // up
        }
      }
      pcDist+=nTexPitch;
    }
	
    // draw outline second
    pcDist=pTexData;
    for(y=-1;y<buffHeight+1;y++)
    {
      for(x=-1;x<buffWidth+1;x++)
      {
        d = *(WORD*)(pcDist+2            + 2*(x  ));
        if(x>=0 && y>=0 && x<buffWidth && y<buffHeight)
        {
          if(c=*(pCharImage+y*buffWidth+x))
          {
            d = d+(255-d)*c/255;
            d &= 0xf0;
            c = 15*c/255;
            *(WORD*)(pcDist+2            + 2*(x  )) = c|(c<<4)|(c<<8)|(d<<8);
          }
          else
          {
            d &= 0xf0;
            *(WORD*)(pcDist+2            + 2*(x  )) = d<<8;
          }
        }
        else
        {
          d &= 0xf0;
          *(WORD*)(pcDist+2            + 2*(x  )) = d<<8;
        }
      }
      pcDist+=nTexPitch;
    }
	//*/
  }
  else
  {
	// copy data from buffer into texture
	uchar* dst = d_drawmem + offsety * d_width + offsetx;
	uchar* src = (uchar*)pCharImage;
	for (uint i = 0; i < buffHeight; ++i)
	{
		uchar *tmp = dst;
		for(uint j = 0; j < buffWidth; ++j)
		{
			
			*tmp++ = *src++;
			*src++;
			
		}

		dst += d_width; //next row
	}//*/
	
/*
    // draw outline second
    pcDist = d_drawmem + offsety * d_width + offsetx;
	unsigned char* pHead = pcDist;
    for(y=0;y<d_height;y++)
    {
      for(x=0;x<d_width;x++)
      {
		 *pcDist++ = *(pCharImage+y*buffWidth+x);
      }
      pcDist= pHead + nTexPitch;
    }
	//*/
  }
}

void DirectX7Texture::copySprite16To24BufWithAlpha(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, void* pSprite)
{
//	return;
	if(pSprite == NULL || d_drawmem == NULL || d_width == 0 || d_height == 0)
	{
		return;
	}

	// Get the real draw map
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}
	
	unsigned char* pSrcBuf = (unsigned char *)pSprite;
	unsigned char* pDstBuf = (unsigned char*)d_drawmem;

	pDstBuf += (tagClipper.y * d_width + tagClipper.x) * 3;//24位要乘以3
	int nBackBufLinePixel = d_width;

	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = size.d_width * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		if(nSameAlpha > 0)
		{
			pSrcBuf += (nSameCount * 2);
		}
	}

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned char* pDstCurLineHead = pDstBuf;
		--nRemainSrcHeight;

		int nCurLinePos			= 0;
		int nCurUnitEndPos		= tagClipper.left;
		
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;
			
			int nFrontCount = nSameCount - (nCurLinePos - nCurUnitEndPos);
			int nLoopCount = (((nCurLinePos - nCurUnitEndPos) > tagClipper.width) ? tagClipper.width : (nCurLinePos - nCurUnitEndPos));
			
			if(nLoopCount > 0)
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += (nFrontCount * 2);

					while(nLoopCount > 0)
					{
						unsigned short colour565 = *((unsigned short*)pSrcBuf);
						pSrcBuf += 2;
						if(!*pDstBuf || nSameAlpha == 255)
						{
							*pDstBuf++ = nSameAlpha;
							*((unsigned short*)pDstBuf) = colour565;
						}
						else
						{
							*pDstBuf++ = (*pDstBuf * 255 + nSameAlpha * 255 - *pDstBuf * nSameAlpha) >> 8;
							unsigned short usnSrcClr = colour565;
							unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
							unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
							unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);
							
							unsigned short usnDstClr = *((unsigned short*)pDstBuf);
							
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
							
							*((unsigned short*)pDstBuf) = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
						}
						
						pDstBuf += 2;

						nLoopCount--;
					}
					
					if ( (nCurLinePos - nCurUnitEndPos) > tagClipper.width )
					{
						pSrcBuf += ((nSameCount - nFrontCount - tagClipper.width)*2);
					}
				}
				else
				{
					pDstBuf += nLoopCount * 3;
				}					
			}
			else
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += (nSameCount*2);
				}
			}
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;

			int nSameCountLoop = 0;
			if ( nCurLinePos > nCurUnitEndPos )
			{
				nSameCountLoop = nSameCount - (nCurLinePos - nCurUnitEndPos);
			}
			else
			{
				nSameCountLoop = nSameCount;
			}
			if(nSameAlpha > 0 )
			{
				while(nSameCountLoop > 0)
				{
					unsigned short colour565 = *((unsigned short*)pSrcBuf);
					pSrcBuf+=2;
					if(!*pDstBuf || nSameAlpha == 255)
					{
						*pDstBuf++ = nSameAlpha;
						*((unsigned short*)pDstBuf) = colour565;
					}
					else
					{
						*pDstBuf++ = (*pDstBuf * 255 + nSameAlpha * 255 - *pDstBuf * nSameAlpha) >> 8;
						
						unsigned short usnSrcClr = colour565;
						unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
						unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
						unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);
						
						unsigned short usnDstClr = *((unsigned short*)pDstBuf);
						
						unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
						unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
						
						*((unsigned short*)pDstBuf) = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
					}

					pDstBuf += 2;
					
					nSameCountLoop--;
				}
				if ( nCurLinePos - nCurUnitEndPos > 0)
				{
					pSrcBuf += ((nCurLinePos - nCurUnitEndPos)*2);
				}
				
			}
			else
			{
				pDstBuf += nSameCount * 3;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0)
			{
				pSrcBuf += (nSameCount*2);
			}
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel * 3;
	}
}


void DirectX7Texture::copySpriteTo24BufWithAlpha(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, void* pSprite, void* pPalette)
{
	if(pSprite == NULL || d_drawmem == NULL || d_width == 0 || d_height == 0)
	{
		return;
	}

	// Get the real draw map
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}
	
	KPAL24 *pSrcPal = (KPAL24 *)pPalette;

	unsigned char* pSrcBuf = (unsigned char *)pSprite;
	unsigned char* pDstBuf = (unsigned char*)d_drawmem;

	pDstBuf += (tagClipper.y * d_width + tagClipper.x) * 3;//24位要乘以3
	int nBackBufLinePixel = d_width;

	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = size.d_width * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount;
		}
	}

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned char* pDstCurLineHead = pDstBuf;
		--nRemainSrcHeight;

		int nCurLinePos			= 0;
		int nCurUnitEndPos		= tagClipper.left;
		
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;
			
			int nFrontCount = nSameCount - (nCurLinePos - nCurUnitEndPos);
			int nLoopCount = (((nCurLinePos - nCurUnitEndPos) > tagClipper.width) ? tagClipper.width : (nCurLinePos - nCurUnitEndPos));
			
			if(nLoopCount > 0)
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += (nFrontCount);

					while(nLoopCount > 0)
					{
						int nPalIndex = *pSrcBuf++;
						unsigned short r = pSrcPal[nPalIndex].Red;
						unsigned short g = pSrcPal[nPalIndex].Green;
						unsigned short b = pSrcPal[nPalIndex].Blue;
						unsigned short colour565 = 0;
						RGB565_FROM_RGB( colour565, r, g, b );
						
						if(!*pDstBuf || nSameAlpha == 255)
						{
							*pDstBuf++ = nSameAlpha;
							*((unsigned short*)pDstBuf) = colour565;
						}
						else
						{
							*pDstBuf++ = (*pDstBuf * 255 + nSameAlpha * 255 - *pDstBuf * nSameAlpha) >> 8;
						
							unsigned short usnSrcClr = colour565;
							unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
							unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
							unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);
							
							unsigned short usnDstClr = *((unsigned short*)pDstBuf);
							
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
							
							*((unsigned short*)pDstBuf) = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
						}
						
						pDstBuf += 2;

						nLoopCount--;
					}
					
					if ( (nCurLinePos - nCurUnitEndPos) > tagClipper.width )
					{
						pSrcBuf += (nSameCount - nFrontCount - tagClipper.width);
					}
				}
				else
				{
					pDstBuf += nLoopCount * 3;
				}					
			}
			else
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += nSameCount;
				}
			}
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			int nDSameAlpha = 255 - nSameAlpha;
			nCurLinePos += nSameCount;

			int nSameCountLoop = 0;
			if ( nCurLinePos > nCurUnitEndPos )
			{
				nSameCountLoop = nSameCount - (nCurLinePos - nCurUnitEndPos);
			}
			else
			{
				nSameCountLoop = nSameCount;
			}
			if(nSameAlpha > 0 )
			{
				while(nSameCountLoop > 0)
				{
					int nPalIndex = *pSrcBuf++;
					unsigned short r = pSrcPal[nPalIndex].Red;
					unsigned short g = pSrcPal[nPalIndex].Green;
					unsigned short b = pSrcPal[nPalIndex].Blue;
					unsigned short colour565 = 0;
					RGB565_FROM_RGB( colour565, r, g, b );
					
					if(!*pDstBuf || nSameAlpha == 255)
					{
						*pDstBuf++ = nSameAlpha;
						*((unsigned short*)pDstBuf) = colour565;
					}
					else
					{
						*pDstBuf++ = (*pDstBuf * 255 + nSameAlpha * 255 - *pDstBuf * nSameAlpha) >> 8;
						
						unsigned short usnSrcClr = colour565;
						unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
						unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
						unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);
						
						unsigned short usnDstClr = *((unsigned short*)pDstBuf);
						
						unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
						unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
						
						*((unsigned short*)pDstBuf) = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
					}

					pDstBuf += 2;
					
					nSameCountLoop--;
				}
				if ( nCurLinePos - nCurUnitEndPos > 0)
				{
					pSrcBuf += (nCurLinePos - nCurUnitEndPos);
				}
				
			}
			else
			{
				pDstBuf += nSameCount * 3;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0)
			{
				pSrcBuf += nSameCount;
			}
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel * 3;
	}
}

void DirectX7Texture::copyFontTo24BufWithAlpha(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, const TextureFont& font, bool underLine )
{
	if(font.d_font == NULL || d_drawmem == NULL )
	{
		return;
	}

	BYTE r = (BYTE)(font.d_fontcolour >> 16);
	BYTE g = (BYTE)(font.d_fontcolour >> 8);
	BYTE b = (BYTE)font.d_fontcolour;
	unsigned short fontcolor16 = 0;	
	unsigned short fontboldcolor16 = 0;
	//unsigned short fontboldcolor16 =((53 >> 3) << 11) | ((30 >> 2) << 5) | (12>>3);
	RGB565_FROM_RGB( fontcolor16, r, g, b)

	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}

	if ( tagClipper.x + tagClipper.width + 1 >= d_width )
	{
		tagClipper.width = d_width - tagClipper.x - 1;
	}

	if ( tagClipper.y + tagClipper.height + 1 >= d_height )
	{
		tagClipper.height = d_height - tagClipper.y - 1;
	}

	// 计算屏幕下一行的偏移
	long ScreenOffset = d_width - tagClipper.width;

	// 计算位图下一行的偏移
	long BitmapOffset = size.d_width - tagClipper.width;
	
	int nLoopY = 0;
	int nLoopX = 0;

	//正常绘制
	unsigned char* lpBuffer = (unsigned char*)d_drawmem;
	unsigned char*	lpBitmap = (unsigned char*)font.d_font;

	lpBuffer += (d_width * tagClipper.y + tagClipper.x) * 3;
	lpBitmap += (size.d_width * tagClipper.top + tagClipper.left);


	for ( nLoopY = 0; nLoopY < tagClipper.height; ++nLoopY )
	{
		for ( nLoopX = 0; nLoopX < tagClipper.width; ++nLoopX )
		{
			int nSameAlpha = *lpBitmap++;
			if ( nLoopY == tagClipper.height - 1 && underLine )
			{
				nSameAlpha = 255;
			}
			int nSameAlphaD = 255 - nSameAlpha;
			if ( nSameAlpha )
			{
				if ( nSameAlpha == 255 )
				{
					*lpBuffer = nSameAlpha;
					lpBuffer++;
					*((unsigned short*)lpBuffer) = fontcolor16;
					lpBuffer += 2;
				}
				else
				{
					/*
							unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
							unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
							unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

							unsigned short usnDstClr = *pDstBuf;
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;//*/


					*lpBuffer = (255 * nSameAlpha + nSameAlphaD * *lpBuffer) >> 8;
					lpBuffer++;
					unsigned short usnSrcClr = fontcolor16;
					unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
					unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
					unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

					unsigned short usnDstClr = *((unsigned short*)lpBuffer);
					
					unsigned short usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
					unsigned short usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
					unsigned short usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
					
					*((unsigned short*)lpBuffer) = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
					lpBuffer += 2;
				}
			}
			else
			{
				lpBuffer += 3;
			}
 
		}
		lpBuffer += ScreenOffset * 3;
		lpBitmap += BitmapOffset;
	}
}



//////add by render 2007-11-13
void        DirectX7Texture::copySprite8BitToBitmap16BitRGB565(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette)
{
	if ( pSprite == NULL || d_width == 0 || d_height == 0|| d_pSurface == NULL || surfacePitch == 0)
	{
		return;
	}

	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}
	void* buffer = NULL;
	int sPitch = 0;
	if(d_type == argb565_texture_dxSurface||d_type == argb565_texture_dxFontSurface)
	{
		buffer = d_pSurface;
		sPitch = surfacePitch>>1;
	}
	else
	{
		buffer = d_drawmem;
		sPitch = d_width;
	}
	if(buffer == NULL||sPitch == 0)
		return;

	unsigned char* pSrcBuffer = (unsigned char* )pSprite;
	unsigned short* pDest  = (unsigned short* ) buffer;
	KPAL24* palettle = (KPAL24*)pPalette;
	pDest += tagClipper.y*sPitch+tagClipper.x;
	unsigned int  allPixels  = d_width*d_height;
	unsigned int  drawPixels = tagClipper.y*d_width + tagClipper.x; 

	int topClipPixels = tagClipper.top*size.d_width;
	int pixelCount = 0;
	int pixelAlpha = 0;
	//先定到要绘制区域的y//
	while(topClipPixels>0)
	{
		pixelCount = *pSrcBuffer;
		pSrcBuffer++;
		pixelAlpha = *pSrcBuffer;
		pSrcBuffer++;
		topClipPixels-=pixelCount;
		if(pixelAlpha>0)
			pSrcBuffer+=(pixelCount);
	}
	///接下来就一步一步绘制了
	int copyHeight =  tagClipper.height;
	for(int index_y = 0 ; index_y < copyHeight;index_y++)
	{
		unsigned short* pDestStartLine  = (unsigned short*)pDest;
		int currentPos = 0;
		int targetPos = tagClipper.left;
		//先定位到开始绘制的位置/
		while(currentPos<targetPos)
		{
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			UCHAR destAlpha = (255 - pixelAlpha);
			currentPos+=pixelCount;
			int loopCount = currentPos  - targetPos;
			if(loopCount>0)
			{
				//到达开始位置时候，上一个排列点已经超出开始位置 了，这个时候需要处理
				if(loopCount > tagClipper.width)
				{
					//超出了被绘制的区域了
					//累计象素//
					drawPixels += tagClipper.width;
					if(drawPixels > allPixels)
						return;
					if(pixelAlpha>0)
					{
						pSrcBuffer+=(pixelCount - loopCount);
						//
						if(drawPixels)
						if(pixelAlpha == 255)
						{
							for(int index_x = 0; index_x < tagClipper.width;index_x++)
							{
								UCHAR paleIdx = pSrcBuffer[index_x];
								KPAL24& pale = palettle[paleIdx];
								unsigned char src_r = (pale.Red>>3);
								unsigned char src_g = (pale.Green>>2);
								unsigned char src_b = (pale.Blue>>3);
								pDest[index_x] =  (src_r<<11) + (src_g<<5) + src_b;
							}

						}
						else
						{
							unsigned short* pSrcTable = GetAlphaAddr(pixelAlpha);
							unsigned short* pDestTable = GetAlphaAddr(destAlpha);
							for(int index_x = 0; index_x < tagClipper.width;index_x++)
							{
								UCHAR paleIdx = pSrcBuffer[index_x];
								KPAL24& pale = palettle[paleIdx];
								unsigned char src_r = (pale.Red>>3);
								unsigned char src_g = (pale.Green>>2);
								unsigned char src_b = (pale.Blue>>3);
								unsigned short src_color = (src_r<<11) + (src_g<<5) + (src_b);
								pDest[index_x] = pSrcTable[src_color] + pDestTable[pDest[index_x]];
							}
						}
						pSrcBuffer+=((loopCount));

					}
					pDest+=tagClipper.width;
				}
				else
				{
					drawPixels += loopCount;
					if(drawPixels > allPixels)
							return;
					if(pixelAlpha>0)
					{
						pSrcBuffer+=((pixelCount - loopCount));
						if(pixelAlpha == 255)
						{
							for(int index_x = 0; index_x< loopCount;index_x++)
							{
								UCHAR paleIdx = pSrcBuffer[index_x];
								KPAL24& pale = palettle[paleIdx];
								unsigned char src_r = (pale.Red>>3);
								unsigned char src_g = (pale.Green>>2);
								unsigned char src_b = (pale.Blue>>3);
								pDest[index_x] =  (src_r<<11) + (src_g<<5) + src_b;
							}
						}
						else
						{
							unsigned short* pSrcTable = GetAlphaAddr(pixelAlpha);
							unsigned short* pDestTable = GetAlphaAddr(destAlpha);
							for(int index_x= 0; index_x < loopCount;index_x++)
							{
								UCHAR paleIdx = pSrcBuffer[index_x];
								KPAL24& pale = palettle[paleIdx];
								unsigned char src_r = (pale.Red>>3);
								unsigned char src_g = (pale.Green>>2);
								unsigned char src_b = (pale.Blue>>3);
								unsigned short src_color = (src_r<<11) + (src_g<<5) + (src_b);
								pDest[index_x] = pSrcTable[src_color] + pDestTable[pDest[index_x]];
							}
						}
						pSrcBuffer+=loopCount;
					}
					pDest+=loopCount;

				}
				   

			}
			else
			{
				if(pixelAlpha>0)
					pSrcBuffer += pixelCount;
			}
		}
		//////////////////////////////
		////下一步绘制//////////////////////////////////////////////////////////////////////////
		targetPos +=tagClipper.width;
		while(currentPos < targetPos)
		{
			//绘制中间的了
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			UCHAR destAlpha = (255 - pixelAlpha);
			currentPos += pixelCount;
			int loopCount = 0;
			if(currentPos > targetPos)
				loopCount = pixelCount - (currentPos - targetPos);
			else
				loopCount = pixelCount;
			//绘制loopcount//
			drawPixels += loopCount;
			if(drawPixels > allPixels)
				return;
			if(pixelAlpha>0)
			{
				if(pixelAlpha == 255)
				{
					for(int index_x = 0; index_x < loopCount; index_x++)
					{
						UCHAR paleIdx = pSrcBuffer[index_x];
						KPAL24& pale = palettle[paleIdx];
						unsigned char src_r = (pale.Red>>3);
						unsigned char src_g = (pale.Green>>2);
						unsigned char src_b = (pale.Blue>>3);
						pDest[index_x] =  (src_r<<11) + (src_g<<5) + src_b;
					}
				}
				else
				{
					unsigned short* pSrcTable = GetAlphaAddr(pixelAlpha);
					unsigned short* pDestTable = GetAlphaAddr(destAlpha);
					for(int index_x = 0; index_x < loopCount; index_x++)
					{
						UCHAR paleIdx = pSrcBuffer[index_x];
						KPAL24& pale = palettle[paleIdx];
						unsigned char src_r = (pale.Red>>3);
						unsigned char src_g = (pale.Green>>2);
						unsigned char src_b = (pale.Blue>>3);
						unsigned short src_color = (src_r<<11) + (src_g<<5) + (src_b);
						pDest[index_x] = pSrcTable[src_color] + pDestTable[pDest[index_x]];
					}
				}
				pSrcBuffer+=pixelCount;
			}		
			pDest += loopCount;
		}
		//末尾处理
		targetPos+=tagClipper.right;
		while(currentPos<targetPos)
		{
			
			pixelCount=*pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			currentPos+=pixelCount;
			if(pixelAlpha>0)
				pSrcBuffer+=pixelCount;
			
		}
		pDest = pDestStartLine + sPitch;
	}
}
void        DirectX7Texture::copySprite16BitToBitmap16BitRGB565(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite)
{
	if ( pSprite == NULL || d_width == 0 || d_height == 0)
	{
		return;
	}

	// Get the real draw map
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}
	void* buffer = NULL;
	int sPitch = 0;
	if(d_type == argb565_texture_dxSurface||d_type == argb565_texture_dxFontSurface)
	{
		buffer = d_pSurface;
		sPitch = surfacePitch>>1;
	}
	else
	{
		buffer = d_drawmem;
		sPitch = d_width;
	}
	if(buffer == NULL)
		return;

	unsigned char* pSrcBuffer = (unsigned char* )pSprite;
	unsigned short* pDest  = (unsigned short* ) buffer;
	pDest += tagClipper.y*sPitch+tagClipper.x;

	int topClipPixels = tagClipper.top*size.d_width;
	int pixelCount = 0;
	int pixelAlpha = 0;
	//先定到要绘制区域的y//
	while(topClipPixels>0)
	{
		pixelCount = *pSrcBuffer;
		pSrcBuffer++;
		pixelAlpha = *pSrcBuffer;
		pSrcBuffer++;
		topClipPixels-=pixelCount;
		if(pixelAlpha>0)
			pSrcBuffer+=(pixelCount<<1);
	}
	///接下来就一步一步绘制了
	int copyHeight = tagClipper.height;
	for(int i = 0 ; i < copyHeight;i++)
	{
		unsigned short* pDestStartLine  = (unsigned short*)pDest;
		int currentPos = 0;
		int targetPos = tagClipper.left;
		//先定位到开始绘制的位置/
		while(currentPos<targetPos)
		{
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			int destAlpha = (255 - pixelAlpha);
			currentPos+=pixelCount;
			int loopCount = currentPos  - targetPos;
			if(loopCount>0)
			{
				//到达开始位置时候，上一个排列点已经超出开始位置 了，这个时候需要处理
				if(loopCount > tagClipper.width)
				{
					//超出了被绘制的区域了
					if(pixelAlpha>0)
					{

						pSrcBuffer+=((pixelCount - loopCount)<<1);
						unsigned short * pSrcTemp = (unsigned short*)pSrcBuffer;
						if(pixelAlpha == 255)
						{
							for(int i = 0; i < tagClipper.width; i++)
								pDest[i] = pSrcTemp[i];
						}
						else
						{
							float src_alpha_f = (float)pixelAlpha/255.0f;
					        float dest_alpha_f = 1.0f - src_alpha_f;
							for(int i = 0; i < tagClipper.width;i++)
							{
								unsigned short src_color = pSrcTemp[i];
								float src_r = (float)((src_color>>11)<<3);
								float src_g = (float)(((src_color>>5)&63)<<2);
								float src_b = (float)((src_color&31)<<3);

								unsigned short destColor = pDest[i];
								float dest_r = (float)((destColor>>11)<<3);
								float dest_g = (float)(((destColor>>5)&63)<<2);
								float dest_b = (float)((destColor&31)<<3);

								unsigned char red =  (unsigned char)(src_r*src_alpha_f+dest_r*dest_alpha_f+0.5f);
								unsigned char green = (unsigned char)(src_g*src_alpha_f+dest_g*dest_alpha_f+0.5f);
								unsigned char blue =  (unsigned char)(src_b*src_alpha_f+dest_b*dest_alpha_f+0.5f);
								red>>=3;green>>=2;blue>>=3;
								pDest[i] = (red<<11)|(green<<5)|blue;
							//	pDest[i] = pSrcTable[pSrcTemp[i]] + pDestTable[pDest[i]];
							}
						}
						pSrcBuffer+=((loopCount)<<1);
					}
					pDest+=tagClipper.width;
				}
				else
				{
					if(pixelAlpha>0)
					{
						pSrcBuffer+=((pixelCount - loopCount)<<1);
						unsigned short * pSrcTemp = (unsigned short*)pSrcBuffer;
						if(pixelAlpha == 255)
						{
							for(int i = 0; i < loopCount; i++)
								pDest[i] = pSrcTemp[i];
						}
						else
						{
							float src_alpha_f = (float)pixelAlpha/255.0f;
					        float dest_alpha_f = 1.0f -src_alpha_f;
							for(int i = 0; i < loopCount;i++)
							{
								unsigned short src_color = pSrcTemp[i];
								float src_r = (float)((src_color>>11)<<3);
								float src_g = (float)(((src_color>>5)&63)<<2);
								float src_b = (float)((src_color&31)<<3);

								unsigned short destColor = pDest[i];
								float dest_r = (float)((destColor>>11)<<3);
								float dest_g = (float)(((destColor>>5)&63)<<2);
								float dest_b = (float)((destColor&31)<<3);

								unsigned char red =  (unsigned char)(src_r*src_alpha_f+dest_r*dest_alpha_f+0.5f);
								unsigned char green = (unsigned char)(src_g*src_alpha_f+dest_g*dest_alpha_f+0.5f);
								unsigned char blue =  (unsigned char)(src_b*src_alpha_f+dest_b*dest_alpha_f+0.5f);

								red>>=3;green>>=2;blue>>=3;
								pDest[i] = (red<<11)|(green<<5)|blue;
						//		pDest[i] = pSrcTable[pSrcTemp[i]] + pDestTable[pDest[i]];
							}
						
						}
						pSrcBuffer+=(loopCount<<1);
					}
					pDest+=loopCount;

				}
				   

			}
			else
			{
				if(pixelAlpha>0)
					pSrcBuffer += (pixelCount<<1);
			}
		}
		//////////////////////////////
		////下一步绘制//////////////////////////////////////////////////////////////////////////
		targetPos +=tagClipper.width;
		while(currentPos < targetPos)
		{
			//绘制中间的了
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			int destAlpha = (255 - pixelAlpha);
			currentPos += pixelCount;
			int loopCount = 0;
			if(currentPos > targetPos)
				loopCount = pixelCount - (currentPos - targetPos);
			else
				loopCount = pixelCount;
			//绘制loopcount//
			if(pixelAlpha>0)
			{
				unsigned short * pSrcTemp = (unsigned short*)pSrcBuffer;
				if(pixelAlpha == 255)
				{
					for(int i = 0; i < loopCount;i++)
						pDest[i] = pSrcTemp[i];
				}
				else
				{
					float src_alpha_f = (float)pixelAlpha/255.0f;
					float dest_alpha_f = 1.0f -src_alpha_f;
					for(int i = 0; i < loopCount; i++)
					{
						unsigned short src_color = pSrcTemp[i];
						float src_r = (float)((src_color>>11)<<3);
						float src_g = (float)(((src_color>>5)&63)<<2);
						float src_b = (float)((src_color&31)<<3);

						unsigned short destColor = pDest[i];
						float dest_r = (float)((destColor>>11)<<3);
						float dest_g = (float)(((destColor>>5)&63)<<2);
						float dest_b = (float)((destColor&31)<<3);

						unsigned char red =  (unsigned char)(src_r*src_alpha_f+dest_r*dest_alpha_f+0.5f);
						unsigned char green = (unsigned char)(src_g*src_alpha_f+dest_g*dest_alpha_f+0.5f);
						unsigned char blue =  (unsigned char)(src_b*src_alpha_f+dest_b*dest_alpha_f+0.5f);

						red>>=3;green>>=2;blue>>=3;
						pDest[i] = (red<<11)|(green<<5)|blue;
					//	pDest[i] = pSrcTable[pSrcTemp[i]] + pDestTable[pDest[i]];
					}
						
				}
				pSrcBuffer+=(pixelCount<<1);
			}		
			pDest += loopCount;
		}
		//末尾处理
		targetPos+=tagClipper.right;
		while(currentPos<targetPos)
		{
			pixelCount=*pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			currentPos+=pixelCount;
			if(pixelAlpha>0)
				pSrcBuffer+=(pixelCount<<1);
			
		}
		pDest = pDestStartLine + sPitch;
	}

}



void 
DirectX7Texture::copyBitmap16Bit565ToBitmap24Bit8565(
	const TexturePosition&	pos, 
	const TextureSize&		size, 
	const TextureRect&		clipper, 
	void*					pBuffer )
{
	if ( pBuffer == NULL || d_width == 0 || d_height == 0)
	{
		return;
	}
	
	// Get the real draw map
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}
	void* buffer = NULL;
	int sPitch = 0;
	if(d_type == argb565_texture_dxSurface||d_type == argb565_texture_dxFontSurface)
	{
		return;
	}
	else
	{
		buffer = d_drawmem;
		sPitch = d_width;
	}
	if( buffer == NULL )
		return;
	
	unsigned char* pDestBuffer = ( unsigned char* )buffer + ( sPitch * tagClipper.y + tagClipper.x ) * 3;

	unsigned char* pDestHelpBuffer = NULL;
	
	unsigned short* pSrcBuffer = ( unsigned short* ) pBuffer + tagClipper.left + tagClipper.top*size.d_width; 

	unsigned short* pSrcHelpBuffer = NULL;
	
	
	for( int index_y = 0; index_y < tagClipper.height; ++index_y )
	{
		pDestHelpBuffer = pDestBuffer;
		pSrcHelpBuffer = pSrcBuffer;

		for( int index_x = 0; index_x < tagClipper.width; ++index_x )
		{
			*pDestBuffer = 0xff;
			++pDestBuffer;
			*( ( unsigned short* )pDestBuffer ) = *pSrcBuffer;
			pDestBuffer += 2;
			++pSrcBuffer;
		}

		pDestBuffer = pDestHelpBuffer + sPitch*3;
		pSrcBuffer  = pSrcHelpBuffer + size.d_width;
	 }
}

 void        
 DirectX7Texture::copyBitmap16Bit565ToBitmap16Bit565(
	const TexturePosition& pos, 
	const TextureSize& size, 
	const TextureRect& clipper, 
	void* pBuffer )
{
	 if ( pBuffer == NULL || d_width == 0 || d_height == 0)
	 {
		 return;
	 }
	 
	 // Get the real draw map
	 _clipper tagClipper;
	 if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	 {
		 return;
	 }
	 void* buffer = NULL;
	 int sPitch = 0;
	 if(d_type == argb565_texture_dxSurface||d_type == argb565_texture_dxFontSurface)
	 {
		 buffer = d_pSurface;
		 sPitch = surfacePitch>>1;
	 }
	 else
	 {
		 buffer = d_drawmem;
		 sPitch = d_width;
	 }
	 if(buffer == NULL)
		return;

	 unsigned short* pDestBuffer = ( unsigned short* )buffer + sPitch * tagClipper.y + tagClipper.x;

	 unsigned short* pSrcBuffer = ( unsigned short* ) pBuffer + tagClipper.left + tagClipper.top*size.d_width; 
	 

	 for( int index_y = 0; index_y < tagClipper.height; ++index_y )
	 {
		 memcpy( 
			 pDestBuffer + index_y * sPitch,
			 pSrcBuffer  + index_y * size.d_width, 
			 tagClipper.width * sizeof( unsigned short ) );
	 }
}

/* to do*/
 /*
void        
DirectX7Texture::copyBitmap1BitToBitmap16Bit565(
	const TexturePosition& pos, 
	const TextureSize& size, 
	const TextureRect& clipper, 
	void* pBuffer, 
	void* pPalette ) 
 {
	 if ( pBuffer == NULL || d_width == 0 || d_height == 0 || pPalette == NULL )
	 {
		 return;
	 }
	 
	 // Get the real draw map
	 _clipper tagClipper;
	 if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	 {
		 return;
	 }
	 void* buffer = NULL;
	 int sPitch = 0;
	 if(d_type == argb565_texture_dxSurface||d_type == argb565_texture_dxFontSurface)
	 {
		 buffer = d_pSurface;
		 sPitch = surfacePitch>>1;
	 }
	 else
	 {
		 buffer = d_drawmem;
		 sPitch = d_width;
	 }
	 if(buffer == NULL)
		 return;
	 
	 unsigned short* pDestBuffer = ( unsigned short* )buffer + sPitch * tagClipper.y + tagClipper.x;
	 unsigned short* 
	 
	 unsigned short* pSrcBuffer = ( unsigned short* ) pBuffer + tagClipper.left + tagClipper.top*size.d_width; 


	 KPAL32* pPal32 = ( KPAL32* )pPalette;

	 unsigned short uPalColor[2] = { 0 };
	 uPalColor[0] = ( ( pPal32[0].Red >> 3 ) << 11 ) |
		 ( ( pPal32[0].Green >> 2 ) << 6 ) |
		 ( ( pPal32[0].Blue >> 3 ) );
	 
	 uPalColor[1] = ( ( pPal32[1].Red >> 3 ) << 11 ) |
		 ( ( pPal32[1].Green >> 2 ) << 6 ) |
		 ( ( pPal32[1].Blue >> 3 ) );

	 for( int index_y = 0; index_y < tagClipper.height; ++index_y )
	 {
		 
	 }
}//*/

void        DirectX7Texture::copyFont16ToBitmap16BitRGB565(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, const TextureFont& font, bool underLine)
{
	if(font.d_font == 0)
		return ;
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}

	if ( tagClipper.x + tagClipper.width + 1 >= d_width )
	{
		tagClipper.width = d_width - tagClipper.x - 1;
	}

	if ( tagClipper.y + tagClipper.height + 1 >= d_height )
	{
		tagClipper.height = d_height - tagClipper.y - 1;
	}
	void* buffer = 0;
	int sPitch = 0;
	if(d_type == argb565_texture_dxSurface||d_type == argb565_texture_dxFontSurface)
	{
		if(d_pSurface)
		{
			buffer = d_pSurface;
			sPitch = (surfacePitch>>1);
		}
	}
	else
	{
		if(d_drawmem)
		{
			buffer = d_drawmem;
			sPitch = d_width; 
		}
	}
	if(d_pSurface == 0||sPitch == 0)
		return;
	int index_x = 0;
	int index_y = 0;
	int fontAlpha = font.d_alpha;
	unsigned short fontColor = g_ARGB32BIT_TO_RGB16BIT565(font.d_fontcolour); 
	int alphaPersent = (int)((fontAlpha<<16)/(255));
	unsigned short color16 = (unsigned short)fontColor;
	bool haveBorder = g_IsFontWithBorder();
	
	unsigned short* pDest = (unsigned short*)buffer + sPitch*tagClipper.y + tagClipper.x;
	unsigned char* pSrcFontBuffer = (unsigned char*)font.d_font + size.d_width*tagClipper.top+tagClipper.left;

	for(index_y = 0; index_y < tagClipper.height; index_y ++)
	{

		for(index_x = 0; index_x < tagClipper.width ; index_x++)
		{
			int grayValue = pSrcFontBuffer[index_x];
			if ( index_y == tagClipper.height - 1 && underLine )
			{
				grayValue = 255;
			}
			int alphaValue = ((grayValue*alphaPersent)>>16);
			if(alphaValue)
			{
				if(alphaValue == 255)
				{
					pDest[index_x] = color16;
					//是否绘制边框//只在右 下++
					if(haveBorder&&tagClipper.x + tagClipper.width < sPitch && tagClipper.y+index_y<size.d_height)
					{
						unsigned short* pTemp = pDest + index_x+1;
						pTemp += sPitch;
						*pTemp  = 0;


					}
				}
				else
				{
					unsigned short* pSrcAlphaTable = GetAlphaAddr(alphaValue);
					unsigned short* pDestAlphaTable = GetAlphaAddr(255 - alphaValue);
					
					if(haveBorder&&tagClipper.x + tagClipper.width < sPitch && tagClipper.y+index_y<size.d_height)
					{
						unsigned short* pTemp = pDest + index_x+1;
						pTemp+=sPitch;
						*pTemp  = pSrcAlphaTable[0] + pDestAlphaTable[*pTemp];


					}
					pDest[index_x] = pSrcAlphaTable[color16] + pDestAlphaTable[pDest[index_x]];
				}
			}
		}
		pDest += sPitch;
		pSrcFontBuffer += size.d_width;
	}
}
void        DirectX7Texture::copyFont16ToBitmap16BitRGB565WithNoAlpha(const TexturePosition& pos,const TextureSize&size,const TextureRect& clipper,const TextureFont& font, bool underLine )
{
	if(font.d_font == 0)
		return ;
	_clipper tagClipper;
	if(getFinalImageClipper(pos, size, clipper, tagClipper) == false)
	{
		return;
	}

	if ( tagClipper.x + tagClipper.width + 1 >= d_width )
	{
		tagClipper.width = d_width - tagClipper.x - 1;
	}

	if ( tagClipper.y + tagClipper.height + 1 >= d_height )
	{
		tagClipper.height = d_height - tagClipper.y - 1;
	}
	void* buffer = 0;
	int sPitch = 0;
	if(d_type == argb565_texture_dxFontSurface||d_type == argb565_texture_dxSurface)
	{
		if(d_pSurface)
		{
			buffer = d_pSurface;
			sPitch = (surfacePitch>>1);
		}
	}
	else
	{
		if(d_drawmem)
		{
			buffer = d_drawmem;
			sPitch = d_width; 
		}
	}
	if(d_pSurface == 0||sPitch == 0)
		return;
	int index_x = 0;
	int index_y = 0;
	int fontAlpha = font.d_alpha;
	unsigned short fontColor = (unsigned short)g_ARGB32BIT_TO_RGB16BIT565(font.d_fontcolour);
	int alphaPersent = (int)((fontAlpha<<16)/(255));
	unsigned short color16 = (unsigned short)fontColor;
	bool haveBorder = g_IsFontWithBorder();
	
	unsigned short* pDest = (unsigned short*)buffer + sPitch*tagClipper.y + tagClipper.x;
	unsigned char* pSrcFontBuffer = (unsigned char*)font.d_font + size.d_width*tagClipper.top+tagClipper.left;
	unsigned char red = (color16>>11)<<3;
	unsigned char green =  ((color16>>5)&63)<<2;
	unsigned char blue = ((color16&31)<<3);
	for(index_y = 0; index_y < tagClipper.height; index_y ++)
	{

		for(index_x = 0; index_x < tagClipper.width ; index_x++)
		{
			int grayValue = pSrcFontBuffer[index_x];
			if ( index_y == tagClipper.height - 1 && underLine )
			{
				grayValue = 255;
			}
			int alphaValue = ((grayValue*alphaPersent)>>16);
			if(alphaValue>80)
			{
				if(alphaValue == 255)
				{
					pDest[index_x] = color16;
					//是否绘制边框//只在右 下++
					if(haveBorder&&tagClipper.x + tagClipper.width < sPitch&&tagClipper.y+index_y<size.d_height)
					{
						unsigned short* pTemp = pDest + index_x+1;
						pTemp += sPitch;
						*pTemp  = ((20>>3)<11)+((20>>2)<<5)+(20>>3);//((gray>>3)<<11)+((gray>>2)<<5)+(gray>>3);


					}
				}
				else
				{
					unsigned short* pSrcAlphaTable = GetAlphaAddr(alphaValue);
					unsigned short* pDestAlphaTable = GetAlphaAddr(255 - alphaValue);
					int destAlpha = (255 - alphaValue);
					int r = (red-127)*1.5+127 - destAlpha;
					int g = (green-127)*1.5+127 - destAlpha;
					int b = (blue-127)*1.5+127 - destAlpha;
					if(r<0) r = 0;
					if(g<0) g = 0;
					if(b<0) b = 0;
					if(r>255) r = 255;
					if(g>255) g = 255;
					if(b>255) b = 255;
					unsigned short color = ((r>>3)<<11)+((g>>2)<<5)+(b>>3);
					if(haveBorder&&tagClipper.x + tagClipper.width < sPitch&&tagClipper.y+index_y<size.d_height)
					{
						unsigned short* pTemp = pDest + index_x+1;
						pTemp+=sPitch;
					//	int pixelGray = PIXEL_GREY(r,g,b);
					//	unsigned short gray_color = ((pixelGray>>3)<<11)+((pixelGray>>2)<<5)+(pixelGray>>3);
						*pTemp  = ((20>>3)<11)+((20>>2)<<5)+(20>>3);// + pDestAlphaTable[gray_color];//+ pDestAlphaTable[color];//+ pDestAlphaTable[65535];


					}
					pDest[index_x] = color  ;//+ pDestAlphaTable[65535];
				}
			}
		}
		pDest += sPitch;
		pSrcFontBuffer += size.d_width;
	}

}
} // End of  CEGUI namespace section
