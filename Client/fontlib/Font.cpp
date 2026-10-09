/***********************************************************************
    filename:   Font.h
    created:    24/07/2007
    author:     LiuSiliang

    purpose:    Class Font
*************************************************************************/

#include "Font.h"

const argb_t Font::DefaultColour		= 0xFFFFFFFF;
static const float DefaultNativeHorzRes	= 640.0f;	
static const float DefaultNativeVertRes	= 480.0f;	

Font::Font( void ):
d_ascender( 0 ),
d_descender( 0 ),
d_height( 0 ),
d_autoScale( false ),
d_horzScaling( 1.0f ),
d_vertScaling( 1.0f ),
d_nativeHorzRes( DefaultNativeHorzRes ),
d_nativeVertRes( DefaultNativeVertRes ),
d_pixSize( 0 )
{
}

Font::Font (const std::string& name, const std::string& fontname) :
d_name( name ),
d_fileName( fontname ),
d_ascender( 0 ),
d_descender( 0 ),
d_height( 0 ),
d_autoScale( false ),
d_horzScaling( 1.0f ),
d_vertScaling( 1.0f ),
d_nativeHorzRes( DefaultNativeHorzRes ),
d_nativeVertRes( DefaultNativeVertRes ),
d_pixSize( 0 )
{
}

Font::Font (const Font& rFnt)
{
	d_cp_map = rFnt.d_cp_map;
 	d_name = rFnt.d_name;
 	d_fileName = rFnt.d_fileName;
 	d_ascender = rFnt.d_ascender;
 	d_descender = rFnt.d_descender;
 	d_height = rFnt.d_height;
 	d_autoScale = rFnt.d_autoScale;
 	d_horzScaling = rFnt.d_horzScaling;
 	d_vertScaling = rFnt.d_vertScaling;
 	d_nativeHorzRes = rFnt.d_nativeHorzRes;
 	d_nativeVertRes = rFnt.d_nativeVertRes;
	d_pixSize = rFnt.d_pixSize;
}

const FT_Bitmap* Font::getGlyphData (utf32 codepoint)
{
	rasterize(codepoint);
    CodepointMap::iterator pos = d_cp_map.find (codepoint);
    return ( (pos != d_cp_map.end()) && ((*pos).second) ) ? pos->second : NULL;
}



