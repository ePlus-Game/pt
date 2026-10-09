/***********************************************************************
    filename: 	CEGUIFont.cpp
    created:	21/2/2004
    author:		Paul D Turner

    purpose:	Implements Font class
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

#include "CEGUIFont.h"
#include "CEGUIExceptions.h"
#include "CEGUISystem.h"
#include "CEGUITextUtils.h"
#include "CEGUIXMLAttributes.h"
#include "CEGUIPropertyHelper.h"
#include <algorithm>
#include "KWin32Wnd.h"

// June
#include "CEGUITexture.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIFontManager.h"

namespace CEGUI
{

// amount of bits in a uint
#define BITS_PER_UINT   (sizeof (uint) * 8)
// must be a power of two
#define GLYPHS_PER_PAGE 256
// June: Default Character Cache Texture
#define DESIRE_TEXTURE_SIDE_WIDTH 512

static const String FontNameAttribute ("Name");
static const String FontFilenameAttribute ("Filename");
static const String FontResourceGroupAttribute ("ResourceGroup");
static const String FontAutoScaledAttribute ("AutoScaled");
static const String FontNativeHorzResAttribute ("NativeHorzRes");
static const String FontNativeVertResAttribute ("NativeVertRes");

static const String FontHaveBorderAttribute ("HaveBorder");

const argb_t Font::DefaultColour = 0xFFFFFFFF;
String Font::d_defaultResourceGroup;

Font::Font (const String& name, const String& fontname, const String& resourceGroup) :
    d_name (name),
    d_fileName (fontname),
    d_resourceGroup (resourceGroup),
    d_ascender (0),
    d_descender (0),
    d_height (0),
    d_autoScale (false),
    d_horzScaling (1.0f),
    d_vertScaling (1.0f),
    d_nativeHorzRes (DefaultNativeHorzRes),
    d_nativeVertRes (DefaultNativeVertRes),
	d_charCacheTexture(NULL),
	d_pixSize(0),
	d_haveBorder(false),
	d_borderColour(0x00000000),
	d_borderCount(2)
{
    addFontProperties ();
}


Font::Font (const XMLAttributes& attributes) :
    d_name (attributes.getValueAsString (FontNameAttribute)),
    d_fileName (attributes.getValueAsString (FontFilenameAttribute)),
    d_resourceGroup (attributes.getValueAsString (FontResourceGroupAttribute)),
    d_ascender (0),
    d_descender (0),
    d_height (0),
    //d_autoScale (attributes.getValueAsBool(FontAutoScaledAttribute, false)),
    d_autoScale (false),
	d_nativeHorzRes (attributes.getValueAsInteger (FontNativeHorzResAttribute, int (DefaultNativeHorzRes))),
    d_nativeVertRes (attributes.getValueAsInteger (FontNativeVertResAttribute, int (DefaultNativeVertRes))),
	d_charCacheTexture(0),
	d_pixSize(0),
	d_haveBorder(attributes.getValueAsBool(FontHaveBorderAttribute, false)),
	d_borderColour(0x00000000),
	d_borderCount(2)
{
    addFontProperties ();

    Size size = System::getSingleton ().getRenderer()->getSize ();
    d_horzScaling = size.d_width / d_nativeHorzRes;
    d_vertScaling = size.d_height / d_nativeVertRes;
}


/*************************************************************************
    Destroys a Font object
*************************************************************************/
Font::~Font(void)
{
}


/*************************************************************************
    Define a glyph mapping (handle a <Mapping /> XML element)
*************************************************************************/
void Font::defineMapping (const XMLAttributes& attributes)
{
    throw FileIOException("Font::defineMapping - The <Mapping> XML element is not supported for this font type");
}

/*************************************************************************
    Return a pointer to the glyphDat struct for the given codepoint,
    or 0 if the codepoint does not have a glyph defined.
*************************************************************************/
const FontGlyph *Font::getGlyphData (utf32 codepoint, bool checkAscII)
{
	// Test
	if (codepoint > 31 && codepoint < 128 && !checkAscII)
	{
		const FontGlyph* glyph = FontManager::getSingleton().getAscIIGlyphData(codepoint, getSize());
		if (glyph)
			return glyph;
	}

	rasterize(codepoint);

    CodepointMap::iterator pos = d_cp_map.find (codepoint);
    return (pos != d_cp_map.end() && (*pos).second.getImage() ) ? &pos->second : 0;
}


/*************************************************************************
    Return the pixel width of the specified text if rendered with this Font.
*************************************************************************/
float Font::getTextExtent(const String& text, float x_scale)
{
    const FontGlyph* glyph;
    float cur_extent = 0, adv_extent = 0, width;

    for (size_t c = 0; c < text.length(); ++c)
    {
        glyph = getGlyphData(text[c]);

        if (glyph)
        {
            width = glyph->getRenderedAdvance(x_scale);

            if (adv_extent + width > cur_extent)
                cur_extent = adv_extent + width;

            adv_extent += glyph->getAdvance(x_scale);
        }
    }

    return max(adv_extent, cur_extent);
}

float Font::getTextExtent(const wchar_t* text, int nWidthFix )
{
    const FontGlyph* glyph;
	int textLen = wcslen(text);
    float adv_extent = 0;
	float adwidth, width;
    for(size_t i = 0; i < textLen; ++i)
    {
		glyph = getGlyphData(text[i]);

        if (glyph)
        {
            width = glyph->getRenderedAdvance(1) + nWidthFix;

//             if (adv_extent + width > cur_extent)
//                 cur_extent = adv_extent + width;
			
			adwidth = glyph->getAdvance() + nWidthFix;

            adv_extent += width > adwidth ? width : adwidth;
        }
    }

    return adv_extent;
}


/*************************************************************************
    Return the index of the closest text character in String 'text' that
    corresponds to pixel location 'pixel' if the text were rendered.
*************************************************************************/
size_t Font::getCharAtPixel(const String& text, size_t start_char, float pixel, float x_scale)
{
    const FontGlyph* glyph;
    float cur_extent = 0;
    size_t char_count = text.length();

    // handle simple cases
    if ((pixel <= 0) || (char_count <= start_char))
        return start_char;

    for (size_t c = start_char; c < char_count; ++c)
    {
        glyph = getGlyphData(text[c]);

        if (glyph)
        {
            cur_extent += glyph->getAdvance(x_scale);

            if (pixel < cur_extent)
                return c;
        }
    }

    return char_count;
}

size_t Font::getCharAtPixel(const wchar_t* text, size_t start_char, float pixel, int wordExtSpace)
{
    const FontGlyph* glyph;
    float cur_extent = 0;
	float width, adwidth;
    size_t char_count = wcslen(text);

    // handle simple cases
    if ((pixel <= 0) || (char_count <= start_char))
        return start_char;

    for (size_t c = start_char; c < char_count; ++c)
    {
        glyph = getGlyphData(text[c]);

        if (glyph)
        {
			
            width = glyph->getRenderedAdvance(1);
			
			adwidth = glyph->getAdvance();

            cur_extent += (width > adwidth ? width : adwidth) + wordExtSpace;

            if (pixel < cur_extent)
                return c;
        }
    }

    return char_count;
}




/*************************************************************************
    Return the number of lines the given text would be formatted to.
*************************************************************************/
size_t Font::getFormattedLineCount(const String& text, const Rect& format_area, TextFormatting fmt, float x_scale)
{
    // handle simple non-wrapped cases.
    if ((fmt == LeftAligned) || (fmt == Centred) || (fmt == RightAligned) || (fmt == Justified))
    {
        return std::count(text.begin(), text.end(), static_cast<utf8>('\n')) + 1;
    }

    // handle wraping cases
    size_t lineStart = 0, lineEnd = 0;
    String	sourceLine;

    float	wrap_width = format_area.getWidth();
    String  whitespace = TextUtils::DefaultWhitespace;
    String	thisLine, thisWord;
    size_t	line_count = 0, currpos = 0;

    while (lineEnd < text.length())
    {
        if ((lineEnd = text.find_first_of('\n', lineStart)) == String::npos)
        {
            lineEnd = text.length();
        }

        sourceLine = text.substr(lineStart, lineEnd - lineStart);
        lineStart = lineEnd + 1;

        // get first word.
        currpos = getNextWord(sourceLine, 0, thisLine);

        // while there are words left in the string...
        while (String::npos != sourceLine.find_first_not_of(whitespace, currpos))
        {
            // get next word of the string...
            currpos += getNextWord(sourceLine, currpos, thisWord);

            // if the new word would make the string too long
            if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width)
            {
                // too long, so that's another line of text
                line_count++;

                // remove whitespace from next word - it will form start of next line
                thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));

                // reset for a new line.
                thisLine.clear();
            }

            // add the next word to the line
            thisLine += thisWord;
        }

        // plus one for final line
        line_count++;
    }

    return line_count;
}


/*************************************************************************
    Renders text on the display.  Return number of lines output.
*************************************************************************/
size_t Font::drawText(const String& text, const Rect& draw_area, float z, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale, float y_scale)
{
    size_t thisCount;
    size_t lineCount = 0;

    float	y_base = draw_area.d_top + getBaseline(y_scale);

    Rect tmpDrawArea(
        PixelAligned(draw_area.d_left),
        PixelAligned(draw_area.d_top),
        PixelAligned(draw_area.d_right),
        PixelAligned(draw_area.d_bottom)
    );

    size_t lineStart = 0, lineEnd = 0;
    String	currLine;

    while (lineEnd < text.length())
    {
        if ((lineEnd = text.find_first_of('\n', lineStart)) == String::npos)
            lineEnd = text.length();

        currLine = text.substr(lineStart, lineEnd - lineStart);
        lineStart = lineEnd + 1;	// +1 to skip \n char

        switch(fmt)
        {
        case LeftAligned:
            drawTextLine(currLine, Vector3(tmpDrawArea.d_left, y_base, z), clip_rect, colours, x_scale, y_scale);
            thisCount = 1;
            y_base += getLineSpacing(y_scale);
            break;

        case RightAligned:
            drawTextLine(currLine, Vector3(tmpDrawArea.d_right - getTextExtent(currLine, x_scale), y_base, z), clip_rect, colours, x_scale, y_scale);
            thisCount = 1;
            y_base += getLineSpacing(y_scale);
            break;

        case Centred:
            drawTextLine(currLine, Vector3(PixelAligned(tmpDrawArea.d_left + ((tmpDrawArea.getWidth() - getTextExtent(currLine, x_scale)) / 2.0f)), y_base, z), clip_rect, colours, x_scale, y_scale);
            thisCount = 1;
            y_base += getLineSpacing(y_scale);
            break;

        case Justified:
            // new function in order to keep drawTextLine's signature unchanged
            drawTextLineJustified(currLine, draw_area, Vector3(tmpDrawArea.d_left, y_base, z), clip_rect, colours, x_scale, y_scale);
            thisCount = 1;
            y_base += getLineSpacing(y_scale);
            break;

        case WordWrapLeftAligned:
            thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, LeftAligned, colours, x_scale, y_scale);
            tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
            break;

        case WordWrapRightAligned:
            thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, RightAligned, colours, x_scale, y_scale);
            tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
            break;

        case WordWrapCentred:
            thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, Centred, colours, x_scale, y_scale);
            tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
            break;

        case WordWrapJustified:
            // no change needed
            thisCount = drawWrappedText(currLine, tmpDrawArea, z, clip_rect, Justified, colours, x_scale, y_scale);
            tmpDrawArea.d_top += thisCount * getLineSpacing(y_scale);
            break;

        default:
            throw InvalidRequestException("Font::drawText - Unknown or unsupported TextFormatting value specified.");
        }

        lineCount += thisCount;

    }

    // should not return 0
    return max(lineCount, (size_t)1);
}


/*************************************************************************
    helper function for renderWrappedText to get next word of a string
*************************************************************************/
size_t Font::getNextWord(const String& in_string, size_t start_idx, String& out_string) const
{
    out_string = TextUtils::getNextWord(in_string, start_idx, TextUtils::DefaultWrapDelimiters);

    return out_string.length();
}


/*************************************************************************
    draws wrapped text
*************************************************************************/
size_t Font::drawWrappedText(const String& text, const Rect& draw_area, float z, const Rect& clip_rect, TextFormatting fmt, const ColourRect& colours, float x_scale, float y_scale)
{
    size_t	line_count = 0;
    Rect	dest_area(draw_area);
    float	wrap_width = draw_area.getWidth();

    String  whitespace = TextUtils::DefaultWhitespace;
    String	thisLine, thisWord;
    size_t	currpos = 0;

    // get first word.
    currpos += getNextWord(text, currpos, thisLine);

    // while there are words left in the string...
    while (String::npos != text.find_first_not_of(whitespace, currpos))
    {
        // get next word of the string...
        currpos += getNextWord(text, currpos, thisWord);

        // if the new word would make the string too long
        if ((getTextExtent(thisLine, x_scale) + getTextExtent(thisWord, x_scale)) > wrap_width)
        {
            // output what we had until this new word
            line_count += drawText(thisLine, dest_area, z, clip_rect, fmt, colours, x_scale, y_scale);

            // remove whitespace from next word - it will form start of next line
            thisWord = thisWord.substr(thisWord.find_first_not_of(whitespace));

            // reset for a new line.
            thisLine.clear();

            // update y co-ordinate for next line
            dest_area.d_top += getLineSpacing(y_scale);
        }

        // add the next word to the line
        thisLine += thisWord;
    }

    // Last line is left aligned
    TextFormatting last_fmt = (fmt == Justified ? LeftAligned : fmt);
    // output last bit of string
    line_count += drawText(thisLine, dest_area, z, clip_rect, last_fmt, colours, x_scale, y_scale);

    return line_count;
}


/*************************************************************************
    Draw a line of text.  No formatting is applied.
*************************************************************************/
void Font::drawTextLine(const String& text, const Vector3& position, const Rect& clip_rect, const ColourRect& colours, float x_scale, float y_scale)
{
    Vector3	cur_pos(position);

    const FontGlyph* glyph;
    float base_y = position.d_y;

    for (size_t c = 0; c < text.length(); ++c)
    {
        glyph = getGlyphData(text[c]);

        if (glyph)
        {
            const Image* img = glyph->getImage();
            cur_pos.d_y = base_y - (img->getOffsetY() - img->getOffsetY() * y_scale);
            img->draw(cur_pos, glyph->getSize(x_scale, y_scale), clip_rect, colours, TopLeftToBottomRight, colours.d_top_left.getAlpha());
            cur_pos.d_x += glyph->getAdvance(x_scale);
        }
    }
}

/*************************************************************************
	Draw a line of text.  输入为unicode编码方式的字符//add by xiehong
*************************************************************************/
void Font::drawTextLine(const wchar_t* text, const Vector3& position, const Rect& clip_rect, const ColourRect& colours, bool showBorder, int nWidthFix )
{
    Vector3	cur_pos(position);

    const FontGlyph* glyph;
    float base_y = position.d_y;
	float width, adwidth;

	int textLen = wcslen(text);
    for (size_t c = 0; c < textLen; ++c)
    {
        glyph = getGlyphData(text[c]);

        if (glyph)
        {
            const Image* img = glyph->getImage();
            cur_pos.d_y = base_y - (img->getOffsetY() - img->getOffsetY());
            img->draw(cur_pos, glyph->getSize(1, 1), clip_rect, colours, 
				TopLeftToBottomRight, colours.d_bottom_left.getAlpha(), showBorder);

			width = glyph->getRenderedAdvance(1) + nWidthFix;
			
			adwidth = glyph->getAdvance() + nWidthFix;

            cur_pos.d_x += width > adwidth ? width : adwidth;
        }
    }
}

Size Font::getWordSize(utf32 codePoint)
{
	Size wordSize;
	wordSize.d_width = 0;
	wordSize.d_height = 0;

	float width = 0, adwidth = 0;
	
    const FontGlyph* glyph;
	glyph = getGlyphData(codePoint);
	
	if (glyph)
	{
		width = glyph->getRenderedAdvance(1);
		adwidth = glyph->getAdvance();
		
		wordSize.d_width = width > adwidth ? width : adwidth;
		wordSize.d_height = this->getFontHeight();
	}
	return wordSize;
}
/*************************************************************************
    Draw a justified line of text.
*************************************************************************/
void Font::drawTextLineJustified (const String& text, const Rect& draw_area, const Vector3& position, const Rect& clip_rect, const ColourRect& colours, float x_scale, float y_scale)
{
    Vector3	cur_pos(position);

    const FontGlyph* glyph;
    float base_y = position.d_y;
    size_t char_count = text.length();

    // Calculate the length difference between the justified text and the same text, left aligned
    // This space has to be shared between the space characters of the line
    float lost_space = getFormattedTextExtent(text, draw_area, Justified, x_scale) - getTextExtent(text, x_scale);

    // The number of spaces and tabs in the current line
    uint space_count = 0;
    size_t c;
    for (c = 0; c < char_count; ++c)
        if ((text[c] == ' ') || (text[c] == '\t'))
            ++space_count;

    // The width that must be added to each space character in order to transform the left aligned text in justified text
    float shared_lost_space = 0.0;
    if (space_count > 0)
        shared_lost_space = lost_space / (float)space_count;

    for (c = 0; c < char_count; ++c)
    {
        glyph = getGlyphData(text[c]);

        if (glyph)
        {
            const Image* img = glyph->getImage();
            cur_pos.d_y = base_y - (img->getOffsetY() - img->getOffsetY() * y_scale);
            img->draw(cur_pos, glyph->getSize(x_scale, y_scale), clip_rect, colours);
            cur_pos.d_x += glyph->getAdvance(x_scale);

            // That's where we adjust the size of each space character
            if ((text[c] == ' ') || (text[c] == '\t'))
                cur_pos.d_x += shared_lost_space;
        }
    }
}


/*************************************************************************
    Set the native resolution for this Font
*************************************************************************/
void Font::setNativeResolution (const Size& size)
{
    d_nativeHorzRes = size.d_width;
    d_nativeVertRes = size.d_height;

    // re-calculate scaling factors & notify images as required
    notifyScreenResolution (System::getSingleton ().getRenderer()->getSize ());
}


/*************************************************************************
    Notify the Font of the current (usually new) display resolution.
*************************************************************************/
void Font::notifyScreenResolution (const Size& size)
{
    d_horzScaling = size.d_width / d_nativeHorzRes;
    d_vertScaling = size.d_height / d_nativeVertRes;

    if (d_autoScale)
        updateFont ();
}


/*************************************************************************
    Return the horizontal pixel extent given text would be formatted to.
*************************************************************************/
float Font::getFormattedTextExtent(const String& text, const Rect& format_area, TextFormatting fmt, float x_scale)
{
    float lineWidth;
    float widest = 0;

    size_t lineStart = 0, lineEnd = 0;
    String	currLine;

    while (lineEnd < text.length())
    {
        if ((lineEnd = text.find_first_of('\n', lineStart)) == String::npos)
        {
            lineEnd = text.length();
        }

        currLine = text.substr(lineStart, lineEnd - lineStart);
        lineStart = lineEnd + 1;	// +1 to skip \n char

        switch(fmt)
        {
        case Centred:
        case RightAligned:
        case LeftAligned:
            lineWidth = getTextExtent(currLine, x_scale);
            break;

        case Justified:
            // usually we use the width of the rect but we have to ensure the current line is not wider than that
            lineWidth = max(format_area.getWidth(), getTextExtent(currLine, x_scale));
            break;

        case WordWrapLeftAligned:
        case WordWrapRightAligned:
        case WordWrapCentred:
            lineWidth = getWrappedTextExtent(currLine, format_area.getWidth(), x_scale);
            break;

        case WordWrapJustified:
            // same as above
            lineWidth = max(format_area.getWidth(), getWrappedTextExtent(currLine, format_area.getWidth(), x_scale));
            break;

        default:
            throw InvalidRequestException("Font::getFormattedTextExtent - Unknown or unsupported TextFormatting value specified.");
        }

        if (lineWidth > widest)
        {
            widest = lineWidth;
        }

    }

    return widest;
}


/*************************************************************************
    returns extent of widest line of wrapped text.
*************************************************************************/
float Font::getWrappedTextExtent(const String& text, float wrapWidth, float x_scale)
{
    String  whitespace = TextUtils::DefaultWhitespace;
    String	thisWord;
    size_t	currpos;
    float	lineWidth, wordWidth;
    float	widest = 0;

    // get first word.
    currpos = getNextWord (text, 0, thisWord);
    lineWidth = getTextExtent (thisWord, x_scale);

    // while there are words left in the string...
    while (String::npos != text.find_first_not_of (whitespace, currpos))
    {
        // get next word of the string...
        currpos += getNextWord (text, currpos, thisWord);
        wordWidth = getTextExtent (thisWord, x_scale);

        // if the new word would make the string too long
        if ((lineWidth + wordWidth) > wrapWidth)
        {
            if (lineWidth > widest)
                widest = lineWidth;

            // remove whitespace from next word - it will form start of next line
            thisWord = thisWord.substr (thisWord.find_first_not_of (whitespace));
            wordWidth = getTextExtent (thisWord, x_scale);

            // reset for a new line.
            lineWidth = 0;
        }

        // add the next word to the line
        lineWidth += wordWidth;
    }

    if (lineWidth > widest)
        widest = lineWidth;

    return widest;
}


void Font::rasterize (utf32 start_codepoint, utf32 end_codepoint)
{
    // do nothing by default
}

//Lucifer~yu(zhangjianyu) 04/20/2007 Modify
//Begin-------------------------------------------------------------------
void Font::InitCharCache()
{
	// Create the cache buffer
	if (d_charCacheTexture == NULL)
	{
		// Detect size
		uint max_texsize = System::getSingleton ().getRenderer ()->getMaxTextureSize ();

		if(max_texsize >= DESIRE_TEXTURE_SIDE_WIDTH)
			d_textureSideWidth = DESIRE_TEXTURE_SIDE_WIDTH;
		else
			d_textureSideWidth = max_texsize;

		// Create Imageset
		String TextureName;
		TextureName = "char_cache_auto_glyph_image_";
		TextureName += d_name;
		d_charCacheTexture = ImagesetManager::getSingleton().createImageset(
			TextureName,
			System::getSingleton ().getRenderer ()->createTexture (), d_haveBorder);

		if ( d_haveBorder )
		{
			if ( g_IsHighFontQuality() )
			{
				d_charCacheTexture->getTexture ()->createTexture(d_textureSideWidth, d_textureSideWidth, sizeof(uchar),	argb1555_texture);
			}
			else
			{
				d_charCacheTexture->getTexture ()->createTexture(d_textureSideWidth, d_textureSideWidth, sizeof(ushort), argb1555_texture);
			}
			
		}
		else
		{
			d_charCacheTexture->getTexture ()->createTexture(d_textureSideWidth, d_textureSideWidth, sizeof(uchar), argb1555_texture);
		}//*/
		

		// TODO 这里这些数字是临时的
		int nTileCount = d_textureSideWidth / d_pixSize;
		d_mruTable.Init(nTileCount * nTileCount);
	}
}

void Font::FreeCharCache()
{
	d_mruTable.Clear();
	if (d_charCacheTexture != NULL)
	{
		ImagesetManager::getSingleton().destroyImageset(
			d_charCacheTexture->getName() );
		d_charCacheTexture = NULL;
	}
}

void Font::rasterize (utf32 codepoint)
{
	// do nothing by default
}	
//End---------------------------------------------------------------------


} // End of  CEGUI namespace section
