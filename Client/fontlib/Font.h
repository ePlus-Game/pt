/***********************************************************************
    filename:   Font.h
    created:    23/07/2007
    author:     LiuSiliang

    purpose:    Class Font
*************************************************************************/

#ifndef _FONT_H_
#define _FONT_H_

#include "fontinterface.h"
#include <ft2build.h>
#include <freetype/ftglyph.h>
#include <freetype/fttrigon.h>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4251)
#endif

class Font
{
protected:    
    typedef std::map<utf32, FT_Bitmap*> CodepointMap;
public:
	Font( void );
    Font (const std::string& name, const std::string& fontname);
	Font (const Font& rFnt);
    virtual ~Font () {};
public:
	virtual void		rasterize( utf32 codepoint ) = 0;
    virtual void		updateFont( utf32 codepoint ) = 0;
    const FT_Bitmap*	getGlyphData (utf32 codepoint);
    const std::string&	getName( void ) const { return d_name; }
protected:
	static const argb_t DefaultColour;
    CodepointMap		d_cp_map;
    std::string			d_name;
    std::string			d_fileName;
    float				d_ascender;
    float				d_descender;
    float				d_height;
    bool				d_autoScale;
    float				d_horzScaling;
    float				d_vertScaling;
    float				d_nativeHorzRes;
    float				d_nativeVertRes;
	int					d_pixSize;
};

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif


#endif
