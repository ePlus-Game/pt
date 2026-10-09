//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/17/2007 19:44
//      File_base        : CEGUIGdiFont
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _CEGUIGdiFont_h_
#define _CEGUIGdiFont_h_

#include "CEGUIFont.h"
#include "CEGUIImage.h"
#include "CEGUIDataContainer.h"
#include <vector>

// Start of CEGUI namespace section
namespace CEGUI
{

// Forward declarations for font properties
namespace FontProperties
{
    class GdiPointSize;
    class GdiAntialiased;
};

class GdiFont : public Font
{
    friend class FontManager;
    friend class FontProperties::GdiPointSize;
    friend class FontProperties::GdiAntialiased;
public:
    GdiFont(const String& name, const String& filename, const String& resourceGroup);
    GdiFont(const XMLAttributes& attributes);
    virtual ~GdiFont( void );

public:
    virtual void load ();
	virtual void rasterize (utf32 codepoint);

private:
	virtual void updateFont( void );
    void	addGdiFontProperties ( void );
    void	free ( void );
	BOOL	GetFont(WORD code);
	int		PointSizetoLogical( int point, int divisor = 1);

	virtual float getSize() { return d_ptSize; }
	virtual void setSize(float size) { d_ptSize = size; }
private:
    float	d_ptSize;
    float	d_width;
    bool	d_antiAliased;
	int		d_advance;
	HFONT	d_hFont;  
    HWND	d_hWnd;
	int		d_format;

	int HZDepth;
	int HZWidth;		
	int HZByteSize;  
	BYTE* bytebuf;	
	int HZBitSize;  
	int HZFontSize;  
	WORD* bitbuf;	
	HDC hdc;
	BYTE textcolor;
    int baseline;
	GLYPHMETRICS lpgm;
	MAT2 lpmat2;

/*
	BYTE	d_lighten;
	bool	d_bUseOldFont;
	BYTE*	d_fontData;
	DWORD	d_dataSize;//*/

};

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIGdiFont_h_
