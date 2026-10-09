/***********************************************************************
    filename:   FreeTypeFont.h
    created:    24/7/2007
    author:     LIU Siliang

    purpose:    Class FreeTypeFont
*************************************************************************/

#ifndef _CEGUIFREETYPEFONT_H_
#define _CEGUIFREETYPEFONT_H_

#include "Font.h"
#include "fontinterface.h"
#include FT_FREETYPE_H

class FreeTypeFont : public Font
{
public:
	FreeTypeFont( void );
    FreeTypeFont( 
		const std::string& name, 
		const std::string& filename );

    FreeTypeFont( 
		const FreeTypeFont& rFnt );
	
    virtual ~FreeTypeFont( void );

public:
	void			load( void );
	int				getFontPixel( void );
	FT_Bitmap*		drawText( utf32 outChar, int size, int angle, int xTrans, int yTrans, int xScalc, int yScalc );
private:
    void			free ( void );
public:
	virtual void	updateFont ( utf32 codepoint );
	virtual void	rasterize (utf32 codepoint);
protected:
	FT_Face			d_fontFace;	
	int				d_angle;
	int				d_xTrans;
	int				d_yTrans;
	int				d_xScalc;
	int				d_yScalc;
    float			d_ptSize;
    bool			d_antiAliased;
	unsigned char*	d_fontBuff;
	long			d_buffSize;
};


#endif
