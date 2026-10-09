/***********************************************************************
    filename:   FontManager.h
    created:    24/7/2007
    author:     LIU Siliang

    purpose:    Manager Font
*************************************************************************/

#ifndef _FONTMANAGER_H_
#define _FONTMANAGER_H_

#include "FreeTypeFont.h"
#include "fontinterface.h"

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4251)
#endif

class FontManager : public IFontManager
{
private:
	FontManager();
	
public:
	~FontManager();

	static  FontManager &getSingleton( void );

	bool	IAddFont( const char *name, const char* fileName );

	void	DestroyFont( const std::string &name );

	void	DestroyAllFont( void );

	const FreeTypeFont*	GetFont( const std::string &name );

	bool	IsFontPresent( const std::string &name ) const;

	
	virtual void	IGetFontSize( 
						int fontSize,
						int& width, 
						int& height );

	virtual bool	IFontGetTextBmp(
						unsigned char* buffer,
						int& size,
						int	fontSize,
						FONTCOLOURTYPE colourType,
						int bgColour,
						int	plusColour,
						int	plusPersent,
						const utf16* outString, 
						const TextureFont* font );

	virtual bool	IFontGetTextBmpPosition(
						int width,
						int height,
						unsigned char* buffer,
						int& size,
						int	fontSize,
						FONTCOLOURTYPE colourType,
						int bgColour,
						int	plusColour,
						int	plusPersent,
						const char* randomTPName, 
						int randomTPCount,
						int randomTpScalc,
						const utf16* outString, 
						const TextureFont* font );//*/
private:
	uint	utf32_length( const utf16* utf32_str ) const;

	bool	Bitmap16Bit565ToBitmap1Bit(
				unsigned short* pBuffer, 
				unsigned uWidth, 
				unsigned uHeight ,
				unsigned char* pDestBuffer );
	
	void	FillBMPBuff(
				int fntSize,
				int colourType,
				const utf16* outString,
				void* outBuffer,
				const TextureFont* font,
				int pixel,
				unsigned short bgcolor16 );

	
	bool	Bitmap16Bit565ToBitmap1BitPosition(
				unsigned short* pBuffer, 
				unsigned uWidth, 
				unsigned uHeight ,
				unsigned char* pDestBuffer );//*/

	void	FillBMPBuffPosition(
				int width,
				int height,
				int fntSize,
				int colourType,
				const utf16* outString,
				void* outBuffer,
				const TextureFont* font,
				int pixel,
				unsigned short bgcolor16 );//*/

private:
	typedef std::map<std::string, FreeTypeFont> FontMap;
	FontMap	d_FontMap;	
};



#if defined(_MSC_VER)
#   pragma warning(pop)
#endif


#endif  // end of guard _FONTMANAGER_H_
