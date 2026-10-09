//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/17/2007 19:44
//      File_base        : CEGUIGdiFont
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32Wnd.h"
#include "CEGUIGdiFont.h"
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

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

namespace CEGUI
{
#ifndef H_CODE
#define H_CODE	0x80
#endif

WORD   expand_table1[16] = {0x8000,0x4000,0x2000,0x1000,0x0800,0x0400,0x0200,0x0100,
                             0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};
DWORD  expand_table2[24] = {0x800000,0x400000,0x200000,0x100000,0x080000,0x040000,0x020000,0x010000,
                             0x008000,0x004000,0x002000,0x001000,0x000800,0x000400,0x000200,0x000100,
                             0x000080,0x000040,0x000020,0x000010,0x000008,0x000004,0x000002,0x000001};
DWORD  expand_table3[32] = {0x80000000,0x40000000,0x20000000,0x10000000,0x08000000,0x04000000,0x02000000,0x01000000,
							 0x00800000,0x00400000,0x00200000,0x00100000,0x00080000,0x00040000,0x00020000,0x00010000,
                             0x00008000,0x00004000,0x00002000,0x00001000,0x00000800,0x00000400,0x00000200,0x00000100,
                             0x00000080,0x00000040,0x00000020,0x00000010,0x00000008,0x00000004,0x00000002,0x00000001};

// Pixels to put between glyphs
#define INTER_GLYPH_PAD_SPACE 0
// A multiplication coefficient to convert FT_Pos values into normal floats
#define FT_POS_COEF  (1.0/64.0)

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

// Font objects usage count
static const String FontSizeAttribute ("Size");
static const String FontAntiAliasedAttribute ("AntiAlias");

/*************************************************************************
 Create a GdiFont object from scratch
 *************************************************************************/
GdiFont::GdiFont (const String& name, const String& filename,
    const String& resourceGroup) :
    Font (name, filename, resourceGroup),
    d_ptSize (10.0),
    d_antiAliased (true),
	d_hFont(NULL),
	d_hWnd(NULL),
	d_advance(0)
{
	d_format = GGO_GRAY8_BITMAP;
	d_hWnd = g_GetDrawHWnd();
    addGdiFontProperties ();
	hdc = ::GetDC( d_hWnd );

	ZeroMemory(&lpgm, sizeof(GLYPHMETRICS));
	ZeroMemory(&lpmat2, sizeof(MAT2));
	lpmat2.eM11.value = 1;
	lpmat2.eM22.value = 1;

	bytebuf		= NULL;
	bitbuf		= NULL;
	textcolor	= 255;

    baseline    = 0;
	/*
	d_lighten = 1;
	d_bUseOldFont = true;
	d_fontData = 0;
	d_dataSize = 0;//*/
}


/*************************************************************************
 Create a GdiFont object from a XML file
 *************************************************************************/
GdiFont::GdiFont (const XMLAttributes& attributes) :
    Font (attributes),
    d_ptSize (float(attributes.getValueAsInteger (FontSizeAttribute, 12))),
    d_antiAliased (attributes.getValueAsBool (FontAntiAliasedAttribute, true)),
	d_hFont(NULL),
	d_hWnd(NULL),
	d_advance(0)
{
	d_format = GGO_GRAY8_BITMAP;
	d_hWnd = g_GetDrawHWnd();
    addGdiFontProperties ();

	hdc = ::GetDC( d_hWnd );

	ZeroMemory(&lpgm, sizeof(GLYPHMETRICS));
	ZeroMemory(&lpmat2, sizeof(MAT2));
	lpmat2.eM11.value = 1;
	lpmat2.eM22.value = 1;

	bytebuf		= NULL;
	bitbuf		= NULL;
	textcolor	= 255;

    baseline    = 0;
	/*
	d_lighten = 1;
	d_bUseOldFont = true;
	d_fontData = 0;
	d_dataSize = 0;//*/
}


/*************************************************************************
 Destroys a GdiFont object
 *************************************************************************/
GdiFont::~GdiFont ()
{
	if ( hdc )
	{
		::ReleaseDC(d_hWnd,hdc);
	}
    if( d_hFont )
    {
        ::DeleteObject(d_hFont);
        d_hFont = NULL;
    }
    free ();
}


/*************************************************************************
 Load the font
 *************************************************************************/
void GdiFont::load ()
{
    updateFont ();
	// June
	InitCharCache();
}

/*************************************************************************
 Free all resources we have allocated
 *************************************************************************/
void GdiFont::free ()
{

		if(bytebuf) {
			delete[] bytebuf;
			bytebuf = NULL;
		}
		if(bitbuf) {
            delete[] bitbuf;
            bitbuf = NULL;
        }
    d_cp_map.clear ();

	// TODO 释放缓冲
	FreeCharCache();
}

/*************************************************************************
 Update the font as required according to the current scaling
 *************************************************************************/
void GdiFont::updateFont ()
{
    free ();

	/*
	LOGFONTA lf; 
	memset(&lf, 0, sizeof(LOGFONTA));
	lf.lfHeight			= -d_ptSize;
	lf.lfWidth			= 0;
	lf.lfWeight			= FW_NORMAL;
	lf.lfCharSet		= DEFAULT_CHARSET;
	lf.lfOutPrecision	= OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision	= CLIP_DEFAULT_PRECIS;
	lf.lfQuality		= d_antiAliased ? ANTIALIASED_QUALITY : NONANTIALIASED_QUALITY;
	lf.lfEscapement		= 900;
	lf.lfOrientation	= 900;
	lf.lfPitchAndFamily	= FIXED_PITCH;
	memcpy( lf.lfFaceName, Utf8ToAnsi(d_fileName), LF_FACESIZE);
	lf.lfFaceName[LF_FACESIZE-1] = 0;

	d_hFont = ::CreateFontIndirectA(&lf); 

	char hzname[LF_FACESIZE];
	memcpy( hzname, Utf8ToAnsi(d_fileName), LF_FACESIZE);

	d_hFont = ::CreateFontA(d_ptSize,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,FIXED_PITCH,hzname);
	if ( d_hFont )
	{
		TEXTMETRICA tm;
		HDC dc = ::GetDC( d_hWnd );
		HFONT hOldFont = (HFONT)::SelectObject(dc,(HGDIOBJ)d_hFont);
		::GetTextMetricsA(dc,&tm);
		d_pixSize	= d_ptSize;
		d_ascender	= tm.tmAscent;
		d_descender	= tm.tmDescent;
		d_height	= tm.tmHeight;
		d_width		= tm.tmMaxCharWidth;
		d_advance	= 1;

		::SelectObject(dc,(HGDIOBJ)hOldFont);
		::ReleaseDC(d_hWnd,dc);
	}//*/

	d_ptSize = PointSizetoLogical( d_ptSize );

	HZDepth 	= d_ptSize;
	HZWidth 	= d_ptSize;
	d_pixSize	= d_ptSize;
	
	char hzname[LF_FACESIZE];
	memcpy( hzname, Utf8ToAnsi(d_fileName), LF_FACESIZE);

	if(d_hFont = ::CreateFontA(-HZDepth,0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,0,0,ANTIALIASED_QUALITY,VARIABLE_PITCH,hzname))  
	{
		HFONT hOldFont = (HFONT)::SelectObject(hdc,(HGDIOBJ)d_hFont);

		TEXTMETRICA tm;
		::GetTextMetricsA(hdc,&tm);
		d_ascender	= tm.tmAscent;
		d_descender	= tm.tmDescent;
		d_height	= tm.tmHeight;
		d_width		= tm.tmMaxCharWidth;
		d_advance	= 1;

		HZByteSize  = 4 * HZWidth * HZDepth;	
		bytebuf		= new BYTE [HZByteSize];
		HZBitSize   = HZWidth * HZDepth * sizeof(WORD);
		HZFontSize  = HZWidth * HZDepth / 8; // 从字库读取
		bitbuf		= new WORD [HZWidth * HZDepth];

		::SelectObject(hdc,(HGDIOBJ)hOldFont);
	}

}

void GdiFont::rasterize(utf32 codepoint)
{
	unsigned short sCodePoint = codepoint;
	unsigned short nPosInTexture;
	unsigned short nNewCharIntoTable[2];	//0 is Index, 1 is CodePoint
	int nCount;

	// Refresh MRU
	d_mruTable.Commit(&sCodePoint, 1, &nPosInTexture, nNewCharIntoTable, nCount);

	// Calculate Texture
		
	if (nCount > 0)
	{
		if(d_hFont)
		{
			if ( GetFont( codepoint ) )
			{

				// Set image in the imageset
				String name;
				char Index[10] = {0};
				sprintf(Index, "%d", nNewCharIntoTable[0]);
				name.assign(Index);

				int nTileCount = d_textureSideWidth / d_pixSize;

				int xxx = (nNewCharIntoTable[0] % nTileCount) * d_pixSize;
				int yyy = (nNewCharIntoTable[0] / nTileCount) * d_pixSize;


				d_charCacheTexture->undefineImage(name);
				Rect area (float(xxx), float(yyy), float(xxx + lpgm.gmBlackBoxX),
					float(yyy + lpgm.gmBlackBoxY));
				Point poffset( lpgm.gmptGlyphOrigin.x, -lpgm.gmptGlyphOrigin.y );

				d_charCacheTexture->defineImage (name, area, poffset);
				d_cp_map[codepoint] = FontGlyph(d_advance+ lpgm.gmBlackBoxX);
				((FontGlyph &)d_cp_map[codepoint]).setImage (&d_charCacheTexture->getImage (name));

				// Copy our memory buffer into the texture and free it
				d_charCacheTexture->getTexture()->updateGdiDbcsChar( xxx, yyy, lpgm.gmBlackBoxX, lpgm.gmBlackBoxY,(BYTE*)bitbuf, false );

			}	
		}
	}
}

BOOL GdiFont::GetFont(WORD code)
{
	register  UINT i,j;
    WORD    *p;
	BYTE	 *q;
	DWORD	 ch;
	DWORD	size;

    if(code < H_CODE) 
	{
        HFONT hOldFont = (HFONT)::SelectObject(hdc,(HGDIOBJ)d_hFont);

		memset(bytebuf,0, HZByteSize);

    	size = ::GetGlyphOutline(hdc, code, d_format, &lpgm, HZByteSize, bytebuf, &lpmat2);
	    if(size <= 0) return FALSE;

    	memset(bitbuf,0,HZBitSize);
	    p	= bitbuf;
		q	= bytebuf;
		if ( d_format == GGO_BITMAP )
		{
			for(i = 0; i < lpgm.gmBlackBoxY; i++) {
				ch = *q++;
				ch = (ch << 8) | (*q++);
				ch = (ch << 8) | (*q++);
				ch = (ch << 8) | (*q++);
				for(j = 0;j < lpgm.gmBlackBoxX; j++,p++) {
					if(expand_table3[j] & ch) {
						*p = textcolor;
					}
				}
			}
		}
		else
		{
			BYTE* pTemp = bytebuf;
			BYTE* pFontData = (BYTE*)bitbuf;
			DWORD swidth=lpgm.gmBlackBoxX;
//			DWORD offset=(swidth+3)&~3;
			int x,y;//,p;
			for(y=0;y<lpgm.gmBlackBoxY;y++)
			{
				for(x=0;x<lpgm.gmBlackBoxX;x++)
				{
					//if(c<128) *(pFontData+p+x)=(*pTemp>=64?255:(*pTemp*255/64));
					//else
					*pFontData++ = (*pTemp>=64?255:(*pTemp*255/64));//*pTemp++;//>=?255:(*pTemp*255);
					pTemp++;
				}
				//pTemp+=offset-swidth;
			}

			/*
			for(i = 0; i < lpgm.gmBlackBoxY; i++) {
				ch = *q++;
				for(j = 0;j < lpgm.gmBlackBoxX; j++,p++) 
				{
					{
						*p = ch >= 64? 255 :(ch*255/64);
					}
				}
			}//*/
		}

		::SelectObject(hdc,(HGDIOBJ)hOldFont);
    }
    else 
	{
		{
		    HFONT hOldFont = (HFONT)::SelectObject(hdc,(HGDIOBJ)d_hFont);

		    memset(bytebuf,0, HZByteSize);

    	    size = ::GetGlyphOutline(hdc, code, d_format, &lpgm, HZByteSize, bytebuf, &lpmat2);
	    	if(size <= 0) return FALSE;

    		memset(bitbuf,0,HZBitSize);
	    	p	= bitbuf;
		    q	= bytebuf;
			if ( d_format = GGO_BITMAP )
			{
				for(i = 0; i < lpgm.gmBlackBoxY; i++) 
				{
	    			ch = *q++;
		    		ch = (ch << 8) | (*q++);
					ch = (ch << 8) | (*q++);
					ch = (ch << 8) | (*q++);
					for(j = 0;j < lpgm.gmBlackBoxX; j++,p++) 
					{
						if(expand_table3[j] & ch) 
						{
							*p = textcolor;
						}
					}
				}
			}
			else
			{
				BYTE* pTemp = bytebuf;
				BYTE* pFontData = (BYTE*)bitbuf;
				DWORD swidth=lpgm.gmBlackBoxX;
				DWORD offset=(swidth+3)&~3;
				int x,y;//,p;
				//p=(TM.tmAscent-lpgm.gmptGlyphOrigin.y)*HZWidth+lpgm.gmptGlyphOrigin.x;
				for(y=0;y<lpgm.gmBlackBoxY;y++)
				{
					for(x=0;x<lpgm.gmBlackBoxX;x++)
					{
						//if(c<128) *(pFontData+p+x)=(*pTemp>=64?255:(*pTemp*255/64));
						//else
						*pFontData++ = 255;//(*pTemp>=64?255:(*pTemp*255/64));//*pTemp++;//>=?255:(*pTemp*255);
						pTemp++;
					}
					//pTemp+=offset-swidth;
				}

				/*
				for(i = 0; i < lpgm.gmBlackBoxY; i++) {
					ch = *q++;
					for(j = 0;j < lpgm.gmBlackBoxX; j++,p++) 
					{
						{
							*p = ch >= 64? 255 :(ch*255/64);
						}
					}
				}//*/
			}

			::SelectObject(hdc,(HGDIOBJ)hOldFont);
        }
    }

	return TRUE;
}

int GdiFont::PointSizetoLogical( int point, int divisor)
{
	::POINT P[2] =
	{
		{0,0},
		{0,::GetDeviceCaps(hdc, LOGPIXELSY)*point/72/divisor}
	};
	::DPtoLP(hdc, P, 2);
	return ::abs(P[1].y - P[0].y);
}


//*/

} // End of  CEGUI namespace section
