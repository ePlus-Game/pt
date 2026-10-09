/************************************************************************
	filename: 	dxdraw7Texture.h
	created:	16/5/2006
	author:		Sam Sun
	
	purpose:	Defines concrete texture class for DX 7.0
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
#ifndef _DXDRAW7Texture_h_
#define _DXDRAW7Texture_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "CEGUITexture.h"
#include <ddraw.h>
#include "dxdraw7renderer.h"
#include "iRepresentShell.h"
#include "KRepresentUnit.h"
#include <list>
#include "Bitmap.h"
using std::list;

#ifdef DIRECTX7_GUIRENDERER_EXPORTS
#define DIRECTX7_GUIRENDERER_API __declspec(dllexport)
#else
#define DIRECTX7_GUIRENDERER_API __declspec(dllimport)
#endif

//Lucifer~yu(zhangjianyu) 05/24/2006 Modify
//Begin-------------------------------------------------------------------
#include "KWin32.h"
#include "KEngine.h"	
//End---------------------------------------------------------------------

// Start of CEGUI namespace section
//add by render for use blt//

namespace CEGUI
{


/*!
\brief
	Abstract base class specifying the required interface for Texture objects.

	Texture objects are created via the Renderer.  The actual inner workings of any Texture object
	are dependant upon the Renderer (and underlying API) in use.  This base class defines the minimal
	set of functions that is required for the rest of the system to work.  Texture objects are only
	created through the Renderer object's texture creation functions.
*/

class DIRECTX7_GUIRENDERER_API DirectX7Texture : public Texture
{
	struct _clipper
	{
		long		x;			// 裁减后的X坐标
		long		y;			// 裁减后的Y坐标
		long		width;		// 裁减后的宽度
		long		height;		// 裁减后的高度
		long		left;		// 左边界裁剪量
		long		right;		// 右边界裁剪量
		long		top;		// 上边界裁剪量
	} ;
private:
	/*************************************************************************
		Friends (to allow construction and destruction)
	*************************************************************************/
	friend	Texture* DirectX7Renderer::createTexture(void);
	friend	Texture* DirectX7Renderer::createTexture(const String& filename, const String& resourceGroup);
	friend	Texture* DirectX7Renderer::createTexture(float size);
	friend	void	 DirectX7Renderer::destroyTexture(Texture* texture);

	/*************************************************************************
		Construction & Destruction (by Renderer object only)
	*************************************************************************/

	DirectX7Texture(Renderer* owner);
	virtual ~DirectX7Texture(void);


public:

	/*************************************************************************
		Abstract Interface
	*************************************************************************/
	/*!
	\brief
		Returns the current pixel width of the texture

	\return
		ushort value that is the current width of the texture in pixels
	*/
	virtual	ushort	getWidth(void) const		{return d_width;}


	/*!
	\brief
		Returns the current pixel height of the texture

	\return
		ushort value that is the current height of the texture in pixels
	*/
	virtual	ushort	getHeight(void) const		{return d_height;}


	/*!
	\brief
		Create the specified image file into the texture.  The texture is resized as required to hold the image.

	\param filename
		The filename of the image file that is to be loaded into the texture

    \param resourceGroup
        Resource group identifier to be passed to the resource provider when loading the image file.

	\return
		Nothing.
	*/
	virtual void	createTexture(uint buffWidth, uint buffHeight, uint pitch, TextureType type);

	/*!
	\brief
		Loads the specified image file into the texture.  The texture is resized as required to hold the image.

	\param filename
		The filename of the image file that is to be loaded into the texture

    \param resourceGroup
        Resource group identifier to be passed to the resource provider when loading the image file.

	\return
		Nothing.
	*/
	virtual void	loadFromFile(const String& filename, const String& resourceGroup);

	virtual void       loadFromMemory( void* pBuffer,bool bIsSpr = false );


	/*!
	\brief
		Return a pointer to the Renderer object that created and owns this Texture

	\return
		Pointer to the Renderer object that owns the Texture
	*/
	Renderer*	getRenderer(void) const			{return d_owner;}


	virtual void*		getBuffer()	const {return (void*)d_drawmem;}
	virtual KSprite*	getSpr()  				{return &d_sprite;}
	virtual Bitmap*     getBitmap( void )       { return m_pBitmap; }
	virtual void		setScaleX( int scalex )		{d_scalex = scalex;}
	virtual void		setScaleY( int scaley )		{d_scaley = scaley;}
	virtual int			getScaleX() const			{return d_scalex;}
	virtual int			getScaleY()	const			{return d_scaley;}
	virtual void		clearTexture( uchar nColour );
	virtual void		copySprite16To24BufWithAlpha(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, void* pSprite);

	virtual void		copySpriteTo24BufWithAlpha(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, void* pSprite, void* pPalette);
	virtual void		copyFontTo24BufWithAlpha(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, const TextureFont& font,  bool underLine );

	virtual void		copySpriteTo32Buf(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, void* pSprite, void* pPalette);
	virtual void		copySpriteTo32BufNoAlpha(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, void* pSprite, void* pPalette);

	virtual void		copy16SpriteTo32Buf(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, void* pSprite);
	virtual void		copy16SpriteTo32BufNoAlpha(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, void* pSprite);

	virtual void		copyFontTo32Buf(const TexturePosition& pos, const TextureSize& size, 
		const TextureRect& clipper, const TextureFont& font, bool underLine );

	virtual void		copySpriteTo24Buf(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette);	
	virtual void		copyFontTo32BufBorder(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, const TextureFont& font, bool underLine );
	virtual void		updateDbcsChar(uint x, uint y, const void* buffPtr, uint buffWidth, uint buffHeight );	
	virtual void		updateDbcsCharBorder(uint x, uint y, const void* buffPtr, uint buffWidth, uint buffHeight );
	virtual void		updateGdiDbcsChar(uint offsetx, uint offsety,  uint buffWidth, uint buffHeight, unsigned char* pCharImage, bool border);

	virtual void        lockBuffer();
	virtual void        unlockBuffer();
	virtual void ReCreateTexture();
	virtual void releaseTexture();


	//////add by render 2007-11-13
	virtual void        copySprite8BitToBitmap16BitRGB565(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette);  
	virtual void        copySprite16BitToBitmap16BitRGB565(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite);
	virtual void        copyFont16ToBitmap16BitRGB565(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, const TextureFont& font,  bool underLine);
	virtual void        copyFont16ToBitmap16BitRGB565WithNoAlpha(const TexturePosition& pos,const TextureSize&size,const TextureRect& clipper,const TextureFont& font,  bool underLine);
	virtual void        copyBitmap16Bit565ToBitmap16Bit565( const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pBuffer ) ;
//	virtual void        copyBitmap1BitToBitmap16Bit565( const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pBitmap, void* pPalette ) ;
	
	virtual void        copyBitmap16Bit565ToBitmap24Bit8565( const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pBuffer ) ;
	virtual void*       getSurface() const {return d_lpDrawmenSurface;};
	static list<void*>  textureList;

	virtual bool        IsSpr( ) const { return m_bIsSpr; }
private:
	/*************************************************************************
		Implementation Functions
	*************************************************************************/
	// safely free direc3d texture (can be called multiple times with no ill effect)
	void	freeTexture(void);
	bool	makeClip(long nX, long nY, long nWidth, long nHeight, _clipper* pClipper);

	bool	getFinalImageClipper(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper,  
		_clipper& pClipper);

	bool   FileStringCmp( const char* pFileName, const char* pFormat );

private:
	
	/*************************************************************************
		Implementation Data
	*************************************************************************/
	Renderer* d_owner;		//<! Renderer object that created and owns this texture

	uchar	*d_drawmem;		//!< The 'real' texture.	//16bits 1555
	uchar	*d_drawmem32;		//!< The 'real' texture.	//16bits 1555
	
	SPRHEAD	d_sprhead;		//!< cached width of the texture
	int		d_scalex;		//!< cached width of the texture
	int		d_scaley;		//!< cached width of the texture
	
	KSprite d_sprite;		//!< cached width of the texture

	/*
	 * for Bitmap
	 */
	Bitmap*  m_pBitmap;

	ushort	d_width;		//!< cached width of the texture
	ushort	d_height;		//!< cached height of the texture
	RECT	d_clipRect;

	//add by render.. use dx surface;
	LPDIRECTDRAWSURFACE     d_lpDrawmenSurface;
	void*                   d_pSurface ;
	int                     surfacePitch;
	bool                    m_bIsSpr;
	//for use dx surface flags;
};

} // End of  CEGUI namespace section

#endif	// end of guard _DXDRAW7Texture_h_
