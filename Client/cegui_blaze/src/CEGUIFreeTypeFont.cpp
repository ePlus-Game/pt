/***********************************************************************
    filename:   CEGUIFont.cpp
    created:    21/2/2004
    author:     Paul D Turner

    purpose:    Implements FreeTypeFont class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#include "KWin32Wnd.h"
#include "shlwapi.h"
#include "CEGUIFreeTypeFont.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"
#include "CEGUITexture.h"
#include "CEGUIImageset.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIXMLAttributes.h"
#include "CEGUIFontManager.h"
#include "CEGUIPropertyHelper.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#pragma comment(lib, "shlwapi.lib.")


#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace CEGUI
{

// Pixels to put between glyphs
#define INTER_GLYPH_PAD_SPACE 0
// A multiplication coefficient to convert FT_Pos values into normal floats
#define FT_POS_COEF  (1.0/64.0)

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

// Font objects usage count
static int ft_usage_count = 0;
// A handle to the FreeType library
static FT_Library ft_lib;

static const String FontSizeAttribute ("Size");
static const String FontAntiAliasedAttribute ("AntiAlias");

/*************************************************************************
 Create a FreeTypeFont object from scratch
 *************************************************************************/
FreeTypeFont::FreeTypeFont (const String& name, const String& filename,
    const String& resourceGroup) :
    Font (name, filename, resourceGroup),
    d_ptSize (10.0),
    d_antiAliased (true),
    d_fontFace (0)
{
    if (!ft_usage_count++)
        FT_Init_FreeType (&ft_lib);
    addFreeTypeFontProperties ();
}


/*************************************************************************
 Create a FreeTypeFont object from a XML file
 *************************************************************************/
FreeTypeFont::FreeTypeFont (const XMLAttributes& attributes) :
    Font (attributes),
    d_ptSize (float(attributes.getValueAsInteger (FontSizeAttribute, 12))),
    d_antiAliased (attributes.getValueAsBool (FontAntiAliasedAttribute, true)),
    d_fontFace (0)
{
    if (!ft_usage_count++)
        FT_Init_FreeType (&ft_lib);
    addFreeTypeFontProperties ();
}


/*************************************************************************
 Destroys a FreeTypeFont object
 *************************************************************************/
FreeTypeFont::~FreeTypeFont ()
{
    free ();

    if (!--ft_usage_count)
        FT_Done_FreeType (ft_lib);
}


/*************************************************************************
 Load the font
 *************************************************************************/

void FreeTypeFont::load ()
{
    updateFont ();
	// June
	InitCharCache();
}//*/

/*************************************************************************
 Copy the FreeType glyph bitmap into the given memory buffer
 *************************************************************************/
void FreeTypeFont::drawGlyphToBuffer_impl (argb_t *buffer, uint buf_width)
{
    FT_Bitmap *glyph_bitmap = &d_fontFace->glyph->bitmap;

    for (int i = 0; i < glyph_bitmap->rows; ++i)
    {
        uchar *src = glyph_bitmap->buffer + (i * glyph_bitmap->pitch);
        switch (glyph_bitmap->pixel_mode)
        {
            case FT_PIXEL_MODE_GRAY:
                {
                    uchar *dst = reinterpret_cast<uchar*> (buffer);
                    for (int j = 0; j < glyph_bitmap->width; ++j)
                    {
                        // RGBA
                        *dst++ = 0xFF;
                        *dst++ = 0xFF;
                        *dst++ = 0xFF;
                        *dst++ = *src++;
                    }
                }
                break;

            case FT_PIXEL_MODE_MONO:
				{
					for (int j = 0; j < glyph_bitmap->width; ++j)
					    buffer [j] = (src [j / 8] & (0x80 >> (j & 7))) ? 0xFFFFFFFF : 0x00000000;
				}
                break;

            default:
                throw InvalidRequestException("Font::drawGlyphToBuffer - The glyph could not be drawn because the pixel mode is unsupported.");
                break;
        }

        buffer += buf_width;
    }
}


/*************************************************************************
 Copy the FreeType glyph bitmap into the given memory buffer
 *************************************************************************/
void FreeTypeFont::drawGlyphToBufferBorder_impl (argb_t *buffer, uint buf_width, int x, int y, uchar colourkey )
{
    FT_Bitmap *glyph_bitmap = &d_fontFace->glyph->bitmap;

	argb_t* tmpbuffer = buffer + y * buf_width + x;

    for (int i = 0; i < glyph_bitmap->rows; ++i)
    {
        uchar *src = glyph_bitmap->buffer + (i * glyph_bitmap->pitch);
        switch (glyph_bitmap->pixel_mode)
        {
            case FT_PIXEL_MODE_GRAY:
                {
                    uchar *dst = reinterpret_cast<uchar*> (tmpbuffer);
                    for (int j = 0; j < glyph_bitmap->width; ++j)
                    {
                        // RGBA
						uchar dstAlpha = dst[3];
						uchar dstB = dst[2];
						uchar srcAlpha = *src++;

						if ( colourkey == 0x84  )
						{
							if ( srcAlpha > 0 )
							{
								*dst++ = colourkey;
								*dst++ = colourkey;
								*dst++ = colourkey;
								*dst++ = (srcAlpha + 255) >> 1;
							}
							else
							{
								dst += 4;
							}
						}
						else
						{
							if ( dstB == 0x84 )
							{
								// font 
								if ( srcAlpha > 0 )
								{
	
									int tempAlpha = (dstAlpha * srcAlpha +  srcAlpha * ( 255 - dstAlpha )) >> 8;
									if ( tempAlpha > 0 )
									{
										*dst++ = colourkey;
										*dst++ = colourkey;
										*dst++ = colourkey;
										*dst++ = tempAlpha;
									}
									else
									{
										*dst++ = 0x84;
										*dst++ = 0x84;
										*dst++ = 0x84;
										*dst++;
									}									
								}
								//font border
								else
								{
									*dst++ = 0x84;
									*dst++ = 0x84;
									*dst++ = 0x84;
									*dst++ = dstAlpha;// = dstAlpha;//255;//(dstAlpha * srcAlpha +  srcAlpha * ( 255 - dstAlpha )) >> 8;//((dstAlpha + srcAlpha) >> 1);
								}//*/
							}
							else
							{
								dst += 4;
							}
						}
                    }
                }
                break;
			/*
            case FT_PIXEL_MODE_MONO:
				{
					for (int j = 0; j < glyph_bitmap->width; ++j)
					    tmpbuffer [j] = (src [j / 8] & (0x80 >> (j & 7))) ? colourkey : 0x00000000;
				}//*/
                break;

            default:
                throw InvalidRequestException("Font::drawGlyphToBuffer - The glyph could not be drawn because the pixel mode is unsupported.");
                break;
        }

        tmpbuffer += buf_width;
    }
}

/*************************************************************************
 Copy the FreeType glyph bitmap into the given memory buffer
 *************************************************************************/
void FreeTypeFont::drawGlyphToBuffer (argb_t *buffer, uint buf_width)
{
	drawGlyphToBuffer_impl(buffer, buf_width);
}

/*************************************************************************
 Copy the FreeType glyph bitmap into the given memory buffer
 *************************************************************************/
void FreeTypeFont::drawGlyphToBufferBorder (argb_t *buffer, uint buf_width)
{
	// 左上
	drawGlyphToBufferBorder_impl (buffer, d_pixSize, 0, 0, 0x84);
	// 右上
	drawGlyphToBufferBorder_impl (buffer, d_pixSize, d_borderCount, 0, 0x84);
	// 右下
	drawGlyphToBufferBorder_impl (buffer, d_pixSize,d_borderCount,d_borderCount, 0x84);
	// 左下
	drawGlyphToBufferBorder_impl (buffer, d_pixSize,0,d_borderCount, 0x84);
	// 上
	drawGlyphToBufferBorder_impl (buffer, d_pixSize,d_borderCount/2,0, 0x84);
	// 下
	drawGlyphToBufferBorder_impl (buffer, d_pixSize,d_borderCount/2,d_borderCount, 0x84);
	// 右
	drawGlyphToBufferBorder_impl (buffer, d_pixSize,d_borderCount,d_borderCount/2, 0x84);
	// 左
	drawGlyphToBufferBorder_impl (buffer, d_pixSize,d_borderCount,d_borderCount/2, 0x84);
	//正常绘制
	drawGlyphToBufferBorder_impl (buffer, d_pixSize,d_borderCount/2,d_borderCount/2);
}

/*************************************************************************
 Free all resources we have allocated
 *************************************************************************/
void FreeTypeFont::free ()
{
    if (!d_fontFace)
        return;

    d_cp_map.clear ();

    FT_Done_Face (d_fontFace);
    d_fontFace = 0;
	
    //System::getSingleton ().getResourceProvider ()->unloadRawDataContainer (d_fontData);

	// TODO 释放缓冲
	FreeCharCache();
}


/*************************************************************************
 Update the font as required according to the current scaling
 *************************************************************************/
void FreeTypeFont::updateFont ()
{
    free ();

 //   System::getSingleton ().getResourceProvider ()->loadRawDataContainer (
 //       d_fileName, d_fontData, d_resourceGroup.empty () ?
 //       getDefaultResourceGroup () : d_resourceGroup);

    // create face using input font
	char szFontPath[256];
	::GetWindowsDirectoryA( szFontPath, 256 );
	std::string strFontPath = szFontPath;
	strFontPath += "\\Fonts\\";

	//if (FT_New_Face(ft_lib, Utf8ToAnsi( d_fileName ), 0, &d_fontFace) != 0)
	{
    //if (FT_New_Memory_Face (ft_lib, d_fontData.getDataPtr (), static_cast<FT_Long>(d_fontData.getSize ()), 0, &d_fontFace) != 0)

		std::string strOldFontPath = Utf8ToAnsi( d_fileName );
		int nBegin = strOldFontPath.find_last_of("\\");
		if ( nBegin == -1 )
		{
			nBegin = strOldFontPath.find_last_of("/");
		}

		std::string strFontName = strOldFontPath.substr(nBegin + 1, d_fileName.size() - nBegin + 1 );

		std::string tempPath = strFontPath + strFontName;
		if ( PathFileExistsA( tempPath.c_str() ) )
		{
			strFontPath += strFontName; 
		}
		else
		{
			if (d_ptSize >= 14)
			{
				d_ptSize = 13;
			}
			if ( d_ptSize < 9 )
			{
				d_ptSize = 9;
			}
			strFontPath += "simsun.ttc"; 
		}
		

		if (FT_New_Face(ft_lib, Utf8ToAnsi( strFontPath.c_str() ), 0, &d_fontFace) != 0)
		{
			throw GenericException ("FreeTypeFont::load - The source font file '" + strFontPath +"' does not contain a valid FreeType font.");
		}        
	}

    // check that default Unicode character map is available
    if (!d_fontFace->charmap)
    {
        FT_Done_Face (d_fontFace);
        d_fontFace = 0;
        throw GenericException ("FreeTypeFont::load - The font '" + d_name +"' does not have a Unicode charmap, and cannot be used.");
    }

    uint horzdpi = System::getSingleton ().getRenderer ()->getHorzScreenDPI ();
    uint vertdpi = System::getSingleton ().getRenderer ()->getVertScreenDPI ();

    float hps = d_ptSize * 64;
    float vps = d_ptSize * 64;
    if (d_autoScale)
    {
        hps *= d_horzScaling;
        vps *= d_vertScaling;
    }

    if (FT_Set_Char_Size (d_fontFace, FT_F26Dot6 (hps), FT_F26Dot6 (vps), horzdpi, vertdpi))
    {
        // For bitmap fonts we can render only at specific point sizes.
        // Try to find nearest point size and use it, if that is possible
        float ptSize_72 = (d_ptSize * 72.0f) / vertdpi;
        float best_delta = 99999;
        float best_size = 0;
        for (int i = 0; i < d_fontFace->num_fixed_sizes; i++)
        {
            float size = d_fontFace->available_sizes [i].size * float(FT_POS_COEF);
            float delta = fabs (size - ptSize_72);
            if (delta < best_delta)
            {
                best_delta = delta;
                best_size = size;
            }
        }

        if ((best_size <= 0) ||
            FT_Set_Char_Size (d_fontFace, 0, FT_F26Dot6 (best_size * 64), 0, 0))
        {
            char size [20];
            snprintf (size, sizeof (size), "%g", d_ptSize);
            throw GenericException ("FreeTypeFont::load - The font '" + d_name +"' cannot be rasterized at a size of " + size + " points, and cannot be used.");
        }
    }

    if (d_fontFace->face_flags & FT_FACE_FLAG_SCALABLE)
    {
		if ( d_haveBorder )
		{
		   //float x_scale = d_fontFace->size->metrics.x_scale * FT_POS_COEF * (1.0/65536.0);
			float y_scale = d_fontFace->size->metrics.y_scale * float(FT_POS_COEF) * (1.0f/65536.0f);
			d_ascender = (d_fontFace->ascender + d_borderCount / 2) * y_scale;
			d_descender = (d_fontFace->descender + d_borderCount / 2) * y_scale;
			d_height = (d_fontFace->height + d_borderCount / 2) * y_scale;
		}
		else
		{
			//float x_scale = d_fontFace->size->metrics.x_scale * FT_POS_COEF * (1.0/65536.0);
			float y_scale = d_fontFace->size->metrics.y_scale * float(FT_POS_COEF) * (1.0f/65536.0f);
			d_ascender = d_fontFace->ascender * y_scale;
			d_descender = d_fontFace->descender * y_scale;
			d_height = d_fontFace->height * y_scale;
		}
    }
    else
    {
		if ( d_haveBorder )
		{
			d_ascender = ( d_fontFace->size->metrics.ascender + d_borderCount / 2) * float(FT_POS_COEF);
			d_descender = ( d_fontFace->size->metrics.descender + d_borderCount / 2) * float(FT_POS_COEF);
			d_height = (d_fontFace->size->metrics.height + d_borderCount / 2) * float(FT_POS_COEF);
		}
		else
		{
			d_ascender = d_fontFace->size->metrics.ascender * float(FT_POS_COEF);
			d_descender = d_fontFace->size->metrics.descender * float(FT_POS_COEF);
			d_height = d_fontFace->size->metrics.height * float(FT_POS_COEF);
		}
    }

    // Create an empty FontGlyph structure for every glyph of the font
    FT_UInt gindex;
    FT_ULong codepoint = FT_Get_First_Char (d_fontFace, &gindex);
    FT_ULong max_codepoint = codepoint;
    while (gindex)
    {
        if (max_codepoint < codepoint)
            max_codepoint = codepoint;

        // load-up required glyph metrics (don't render)
        if (FT_Load_Char (d_fontFace, codepoint, FT_LOAD_DEFAULT))
            continue; // glyph error

        float adv = d_fontFace->glyph->metrics.horiAdvance * float(FT_POS_COEF);

        // create a new empty FontGlyph with given character code
       if ( d_haveBorder )
       {
		   d_cp_map[codepoint] = FontGlyph (adv + d_borderCount/2);
       }
	   else
	   {
			d_cp_map[codepoint] = FontGlyph (adv);
	   }

        // proceed to next glyph
        codepoint = FT_Get_Next_Char (d_fontFace, codepoint, &gindex);
    }

	if ( d_haveBorder )
	{
		d_pixSize = max((int)ceil(d_ptSize * FT_POS_COEF * horzdpi), (int)ceil(d_ptSize * FT_POS_COEF * vertdpi)) + d_borderCount;
	}
	else
	{
		d_pixSize = max((int)ceil(d_ptSize * FT_POS_COEF * horzdpi), (int)ceil(d_ptSize * FT_POS_COEF * vertdpi));
	}
}

void FreeTypeFont::rasterize(utf32 codepoint)
{
	//CodepointMap::const_iterator s = d_cp_map.upper_bound (codepoint);
	//CodepointMap::const_iterator s = d_cp_map[codepoint];
	//if (s == d_cp_map.end ())
	//	return;

	unsigned short sCodePoint = codepoint;
	unsigned short nPosInTexture;
	unsigned short nNewCharIntoTable[2];	//0 is Index, 1 is CodePoint
	int nCount;

	// Refresh MRU
	d_mruTable.Commit(&sCodePoint, 1, &nPosInTexture, nNewCharIntoTable, nCount);

	// Calculate Texture
		
	if (nCount > 0)
	{
		// Create a memory buffer where we will render our glyphs
		argb_t *mem_buffer = new argb_t [d_pixSize * d_pixSize];
		memset (mem_buffer, 0, d_pixSize * d_pixSize * sizeof (argb_t));

		// Go ahead, line by line, top-left to bottom-right
		uint x = INTER_GLYPH_PAD_SPACE;
		uint y = INTER_GLYPH_PAD_SPACE;
		uint yb = INTER_GLYPH_PAD_SPACE;

		int xxx = 0;
		int yyy = 0;

        if (FT_Load_Char (d_fontFace, codepoint, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT |
                                  (d_antiAliased ? FT_LOAD_TARGET_NORMAL : FT_LOAD_TARGET_MONO)) == 0)
		{
			uint glyph_w = 0;
			uint glyph_h = 0;

			if ( (d_fontFace->glyph->bitmap.width == 0 ) && codepoint != 0x20 && codepoint != 0x0d && codepoint != 0x0a && codepoint != 0x09 && codepoint != 0x3000 )
			{
				memset (mem_buffer, 0xffffffff, d_pixSize * d_pixSize * sizeof (argb_t));
				d_fontFace->glyph->bitmap.width = d_pixSize;
				d_fontFace->glyph->bitmap.rows = d_pixSize;
				glyph_w = d_pixSize;
				glyph_h= d_pixSize;				

				d_fontFace->glyph->metrics.horiBearingX = d_pixSize;
				d_fontFace->glyph->metrics.horiBearingY = d_fontFace->glyph->metrics.vertAdvance;
				CodepointMap::iterator it = d_cp_map.find(codepoint);
				if ( it != d_cp_map.end() )
				{
					(*it).second.setAdvance( d_pixSize );
				}
				else
				{
					d_cp_map[codepoint] = FontGlyph (d_pixSize);
				}
			}
			else
			{
				// Copy rendered glyph to memory buffer in RGBA format
				glyph_w = d_fontFace->glyph->bitmap.width + INTER_GLYPH_PAD_SPACE;
				glyph_h = d_fontFace->glyph->bitmap.rows + INTER_GLYPH_PAD_SPACE;
				drawGlyphToBuffer (mem_buffer, d_pixSize);
			}
			/*if ( d_haveBorder )
			{
				glyph_w = d_fontFace->glyph->bitmap.width + INTER_GLYPH_PAD_SPACE + d_borderCount;
				glyph_h = d_fontFace->glyph->bitmap.rows + INTER_GLYPH_PAD_SPACE + d_borderCount;
				if ( g_IsHighFontQuality() )
				{
					drawGlyphToBuffer (mem_buffer, d_pixSize);
				}
				else
				{
					drawGlyphToBufferBorder (mem_buffer, d_pixSize);
				}				
			}
			else
			{
				glyph_w = d_fontFace->glyph->bitmap.width + INTER_GLYPH_PAD_SPACE;
				glyph_h = d_fontFace->glyph->bitmap.rows + INTER_GLYPH_PAD_SPACE;
				drawGlyphToBuffer (mem_buffer, d_pixSize);
			}//*/

			// Set image in the imageset
			String name;
			char Index[10] = {0};
			sprintf(Index, "%d", nNewCharIntoTable[0]);
			name.assign(Index);

			int nTileCount = d_textureSideWidth / d_pixSize;

			xxx = (nNewCharIntoTable[0] % nTileCount) * d_pixSize;
			yyy = (nNewCharIntoTable[0] / nTileCount) * d_pixSize;

			if ( d_haveBorder && !g_IsHighFontQuality() )
			{
				xxx *= sizeof(ushort);
				yyy *= sizeof(ushort);
			}

			d_charCacheTexture->undefineImage(name);
			Rect area (float(xxx), float(yyy), float(xxx + glyph_w - INTER_GLYPH_PAD_SPACE),
				float(yyy + glyph_h - INTER_GLYPH_PAD_SPACE));
			Point offset (d_fontFace->glyph->metrics.horiBearingX * float(FT_POS_COEF),
				-d_fontFace->glyph->metrics.horiBearingY * float(FT_POS_COEF));

			d_charCacheTexture->defineImage (name, area, offset);
			((FontGlyph &)d_cp_map[codepoint]).setImage (&d_charCacheTexture->getImage (name));

			// Copy our memory buffer into the texture and free it
			d_charCacheTexture->getTexture()->updateDbcsChar( xxx, yyy, mem_buffer,d_pixSize, d_pixSize );
			/*if ( d_haveBorder )
			{
				if ( g_IsHighFontQuality() )
				{
					d_charCacheTexture->getTexture()->updateDbcsChar( xxx, yyy, mem_buffer,d_pixSize, d_pixSize );
				}
				else
				{
					d_charCacheTexture->getTexture()->updateDbcsCharBorder(	xxx, yyy, mem_buffer,d_pixSize, d_pixSize );
				}			
			}
			else
			{
				d_charCacheTexture->getTexture()->updateDbcsChar( xxx, yyy, mem_buffer,d_pixSize, d_pixSize );
			}//*/
		}

		delete [] mem_buffer;

	}
	
}
} // End of  CEGUI namespace section
