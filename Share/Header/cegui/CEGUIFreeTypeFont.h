/***********************************************************************
	filename: 	CEGUIFreeTypeFont.h
	created:	3/6/2006
	author:		Andrew Zabolotny
	
	purpose:    Implementation of the Font class via the FreeType library
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
#ifndef _CEGUIFreeTypeFont_h_
#define _CEGUIFreeTypeFont_h_

#include "CEGUIFont.h"
#include "CEGUIImage.h"
#include "CEGUIDataContainer.h"
#include <vector>
#include <ft2build.h>
#include FT_FREETYPE_H

// Start of CEGUI namespace section
namespace CEGUI
{

// Forward declarations for font properties
namespace FontProperties
{
    class FreeTypePointSize;
    class FreeTypeAntialiased;
};

/*!
\brief
    Implementation of the Font class interface using the FreeType library.

    This implementation tries to provide maximal support for any kind of
    fonts supported by FreeType. It has been tested on outline font formats
    like TTF and PS as well as on bitmap font formats like PCF and FON.

    Glyphs are rendered dynamically on demand, so a large font with lots
    of glyphs won't slow application startup time.
*/
class FreeTypeFont : public Font
{
protected:
    friend class FontManager;
    friend class FontProperties::FreeTypePointSize;
    friend class FontProperties::FreeTypeAntialiased;

    /// Point size of font.
    float d_ptSize;
    /// True if the font should be rendered as anti-alaised by freeType.
    bool d_antiAliased;

    /// FreeType-specific font handle
    FT_Face d_fontFace;

//    RawDataContainer d_fontData;

    /*!
    \brief
        Copy the current glyph data into \a buffer, which has a width of
        \a buf_width pixels (not bytes).

    \param buffer
        Memory buffer large enough to receive the imagery for the currently
        loaded glyph.

    \param buf_width
        Width of \a buffer in pixels (where each pixel is a argb_t).

    \return
        Nothing.
    */
    void drawGlyphToBuffer_impl (argb_t* buffer, uint buf_width);

    /*!
    \brief
        Copy the current glyph data into \a buffer, which has a width of
        \a buf_width pixels (not bytes).

    \param buffer
        Memory buffer large enough to receive the imagery for the currently
        loaded glyph.

    \param buf_width
        Width of \a buffer in pixels (where each pixel is a argb_t).

    \return
        Nothing.
    */
    void drawGlyphToBufferBorder_impl (argb_t* buffer, uint buf_width, int x, int y, uchar colourkey = 0xff );

    /*!
    \brief
        Copy the current glyph data into \a buffer, which has a width of
        \a buf_width pixels (not bytes).

    \param buffer
        Memory buffer large enough to receive the imagery for the currently
        loaded glyph.

    \param buf_width
        Width of \a buffer in pixels (where each pixel is a argb_t).

    \return
        Nothing.
    */
    void drawGlyphToBuffer (argb_t* buffer, uint buf_width);


    /*!
    \brief
        Copy the current glyph data into \a buffer, which has a width of
        \a buf_width pixels (not bytes).

    \param buffer
        Memory buffer large enough to receive the imagery for the currently
        loaded glyph.

    \param buf_width
        Width of \a buffer in pixels (where each pixel is a argb_t).

    \return
        Nothing.
    */
    void drawGlyphToBufferBorder (argb_t* buffer, uint buf_width);

    /// \copydoc Font::Font
    FreeTypeFont (const String& name, const String& filename,
                  const String& resourceGroup);

    /// \copydoc Font::Font
    FreeTypeFont (const XMLAttributes& attributes);

    /// \copydoc Font::~Font
    virtual ~FreeTypeFont ();

    /// \copydoc Font::updateFont
    virtual void updateFont ();

	virtual float getSize() { return d_ptSize; }
	virtual void  setSize(float size) { d_ptSize = size; }

    /*!
    \brief
        Register all properties of this class.
    */
    void addFreeTypeFontProperties ();

    /*!
    \brief
        Free all allocated font data.
    */
    void free ();

public:
    /// \copydoc Font::load
    virtual void load ();
	// Here added by June
	virtual void rasterize (utf32 codepoint);
};

} // End of  CEGUI namespace section

#endif	// end of guard _CEGUIFreeTypeFont_h_
