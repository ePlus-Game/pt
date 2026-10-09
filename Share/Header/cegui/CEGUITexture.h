//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/21/2007 22:14
//      File_base        : CEGUITexture
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _CEGUITexture_h_
#define _CEGUITexture_h_

#include "CEGUIBase.h"
#include "CEGUIString.h"
#include "KEngine.h"
#include "CEGUIcolour.h"
#include "Bitmap.h"

// Start of CEGUI namespace section
namespace CEGUI
{
enum TextureType
{
	font_texture,
	spr_texture,
	argb8888_texture,
	argb1555_texture,
	argb565_texture_dxSurface,
	argb565_texture_dxFontSurface,
};

struct TextureRect 
{
	TextureRect()
	{
		d_offsetX = 0;
		d_offsetY =	0;
		d_width	= 0;
		d_height = 0;
	}
	TextureRect( int offsetX,
				int offsetY,
				int	width,
				int height )
	{
		d_offsetX = offsetX;
		d_offsetY =	offsetY;
		d_width	= width;
		d_height = height;
	}
	int d_offsetX;
	int d_offsetY;
	int	d_width;
	int d_height;
};

struct TextureSize 
{
	TextureSize()
	{
		d_width	= 0;
		d_height = 0;
	}
	TextureSize( int width, int height )
	{
		d_width	= width;
		d_height = height;
	}
	int	d_width;
	int	d_height;
};

struct TexturePosition 
{
	TexturePosition()
	{
		d_x = 0;
		d_y = 0;
	}
	TexturePosition( int x,	int y)
	{
		d_x = x;
		d_y = y;
	}
	int d_x;
	int d_y;
};

struct TextureFont 
{
	TextureFont()
	{
		d_font				= NULL;
		d_fontcolour		= 0;
		d_fontbordercolour	= 0;
		d_alpha				= 0;
	}
	TextureFont( void* font,
				int fontcolour,
				int	fontbordercolour,
				int alpha )
	{
		d_font				= font;
		d_fontcolour		= fontcolour;
		d_fontbordercolour	= fontbordercolour;
		d_alpha				= alpha;
	}
	void* d_font;
	int d_fontcolour;
	int	d_fontbordercolour;
	int d_alpha;
};

#define TEXTURE_PITCH_TYP_A8RGB565  3
#define TEXTURE_PICTH_TYP_BLT_RGB565  2
#define TEXTURE_PICTH_TYP_BLT_PLAYERHEADINFO -1
class CEGUIEXPORT Texture
{
protected:
	Texture(Renderer* owner) : d_owner(owner) {}

public:
	virtual ~Texture(void) {}

public:
	inline int	getPitch(){	return d_pitch;	};
	virtual void		createTexture(uint buffWidth, uint buffHeight, uint pitch, TextureType type) = 0;
	virtual void		clearTexture( uchar nColour ) = 0;
	virtual void		loadFromFile(const String& filename, const String& resourceGroup) = 0;
	virtual void       loadFromMemory( void* pBuffer,bool bIsSpr = false ) = 0;
	Renderer*			getRenderer(void) const	{return d_owner;}
	virtual	void*		getBuffer(void) const { return NULL; };
	virtual KSprite*	getSpr(void) = 0;
	virtual Bitmap*     getBitmap( void ) = 0;
	virtual	ushort		getWidth(void) const = 0;	
	virtual	ushort		getHeight(void) const = 0;
	virtual void		updateDbcsChar(uint x, uint y, const void* buffPtr, uint buffWidth, uint buffHeight ) = 0;
	virtual void		updateDbcsCharBorder(uint x, uint y, const void* buffPtr, uint buffWidth, uint buffHeight ) = 0;
	virtual void		copySpriteTo24Buf(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette) = 0;

	virtual void		copySprite16To24BufWithAlpha(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite) = 0;
	virtual void		copySpriteTo24BufWithAlpha(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette) = 0;
	virtual void		copyFontTo24BufWithAlpha(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, const TextureFont& font, bool underLine ) = 0;

	virtual void		copySpriteTo32Buf(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette) = 0;
	virtual void		copySpriteTo32BufNoAlpha(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette) = 0;

	virtual void		copy16SpriteTo32Buf(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite) = 0; 
	virtual void		copy16SpriteTo32BufNoAlpha(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite) = 0; 

	virtual void		copyFontTo32Buf(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, const TextureFont& font, bool underLine ) = 0;
	virtual void		copyFontTo32BufBorder(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, const TextureFont& font, bool underLine ) = 0;
	virtual void		updateGdiDbcsChar(uint offsetx, uint offsety,  uint buffWidth, uint buffHeight, unsigned char* pCharImage, bool border) = 0;
	


	//////add by render 2007-11-13
	virtual void        copySprite8BitToBitmap16BitRGB565(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite, void* pPalette) = 0;  
	virtual void        copySprite16BitToBitmap16BitRGB565(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pSprite) = 0;

	virtual void        copyBitmap16Bit565ToBitmap24Bit8565( const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pBuffer ) = 0;
	virtual void        copyBitmap16Bit565ToBitmap16Bit565( const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pBuffer ) = 0;
//	virtual void        copyBitmap1BitToBitmap16Bit565( const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, void* pBitmap, void* pPalette ) = 0;
	virtual void        copyFont16ToBitmap16BitRGB565(const TexturePosition& pos, const TextureSize& size, const TextureRect& clipper, const TextureFont& font,  bool underLine) = 0;
	virtual void        copyFont16ToBitmap16BitRGB565WithNoAlpha(const TexturePosition& pos,const TextureSize&size,const TextureRect& clipper,const TextureFont& font,  bool underLine) = 0;
	virtual void        lockBuffer() = 0;
	virtual void        unlockBuffer() = 0;
	virtual int        getType() const {return d_type;}
	virtual void*       getSurface() const=0;
	virtual void        releaseTexture() = 0;
	virtual void        ReCreateTexture() = 0;
	virtual bool        IsSpr( ) const  = 0;
protected:
	Renderer*			d_owner;
	TextureType			d_type;
	int					d_pitch;
};

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUITexture_h_
