/////////////////////////////////////////////////////////////////////////////
//  FileName    :   fontinterface.h
//  Creator     :   LIUSiliang
//  Date        :   2007-08-07 9:54:00
//  Comment     :   Interface Declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////
#ifndef _FONT_INTERFACE_H_
#define _FONT_INTERFACE_H_

#define interface struct

/////////////////////////////////////////////////////////////////////////////
//
//              Interface Declare
//
/////////////////////////////////////////////////////////////////////////////
//字体全部为UTF8

#pragma warning(disable: 4786)   

#include <string>
#include <map>
#include <vector>

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef	unsigned long	ulong;

typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;

typedef	uint8			utf8;
typedef	uint16			utf16;
typedef uint32			utf32;
typedef uint32			argb_t;
typedef uint16			r5g6b5_t;

#define fnt_max(a,b)    (((a) > (b)) ? (a) : (b))

#ifndef WIN32

#define BI_BITFIELDS  3L

#pragma pack(1)

typedef struct tagBITMAPFILEHEADER {
	unsigned short    bfType;
	unsigned long   bfSize;
	unsigned short    bfReserved1;
	unsigned short    bfReserved2;
	unsigned long   bfOffBits;
} BITMAPFILEHEADER,  *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER{
	unsigned long      biSize;
	long       biWidth;
	long       biHeight;
	unsigned short       biPlanes;
	unsigned short       biBitCount;
	unsigned long      biCompression;
	unsigned long      biSizeImage;
	long       biXPelsPerMeter;
	long       biYPelsPerMeter;
	unsigned long      biClrUsed;
	unsigned long      biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

typedef struct tagRGBQUAD 
{
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
} RGBQUAD;
#pragma pack()
#else
#include <windows.h>
#endif

#define MAX_FONT_PIXEL 60
#define MAX_STR_LEN	   10
#define MAX_RANDOM_TP  5

#define RGB565_BIT_MAX_SIZE    ( MAX_FONT_PIXEL * MAX_STR_LEN + MAX_FONT_PIXEL * MAX_STR_LEN%2 )* MAX_FONT_PIXEL * 2 * MAX_STR_LEN//* ( MAX_FONT_PIXEL * MAX_STR_LEN  + 3 ) / 4 * 4 * MAX_FONT_PIXEL
//#define RGB565_BIT_MAX_WIDTH     ( MAX_FONT_PIXEL * MAX_STR_LEN + MAX_FONT_PIXEL * MAX_STR_LEN%2 ) * 2
#define RGB565_WIDTH( width )  ( (width) + ( width%2 ) )
#define RGB565_BUFF_MAX RGB565_BIT_MAX_SIZE + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(unsigned long)*3

#define BIT_BUFFER_SIZE ( MAX_FONT_PIXEL * MAX_STR_LEN + 31 ) / 32 * 4 * MAX_FONT_PIXEL * MAX_STR_LEN
//#define BIT_BUFFER_WIDTH ( MAX_FONT_PIXEL + 31 ) / 32 * 4
#define BIT_BUFFER_MAX BIT_BUFFER_SIZE + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD)*2

// Pixels to put between glyphs
#define INTER_GLYPH_PAD_SPACE 0
// A multiplication coefficient to convert FT_Pos values into normal floats
#define FT_POS_COEF  (1.0/64.0)

#define PixelAligned(x)	( (float)(int)(( x ) + 0.5f) )

#define SHIFTNUM_RGB565_RCOLOR		11
#define SHIFTNUM_RGB565_GCOLOR		5

#define RGB565_FROM_RGB(Pixel, r, g, b)					\
{									\
	Pixel = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);			\
}

enum FONTCOLOURTYPE
{
	FMT_BIT,
	FMT_R5G6B5,
};

struct TextureFont 
{	
	TextureFont()
	: d_colour(0xff000000)
	,d_angle(0)
	,d_xTrans(0)
	,d_yTrans(0)
	,d_space(0)
	,d_fontName("")
	,d_xScalc(0)
	,d_yScalc(0)
	,d_x(0)
	,d_y(0)
	{
	}
	int				d_x;
	int				d_y;
	std::string		d_fontName;
	int				d_colour;
	int				d_angle;
	int				d_xTrans; 
	int				d_yTrans;
	int				d_space;
	int				d_xScalc;
    int				d_yScalc;
};
//接口用到的结构…………end


interface IFontManager
{
	virtual bool	IAddFont( 
						const char *name, 
						const char* fileName ) = 0;

	virtual void	IGetFontSize( 
						int fontSize,
						int& width, 
						int& height ) = 0;

	virtual bool	IFontGetTextBmp(
						unsigned char* buffer,
						int& size,
						int	fontSize,
						FONTCOLOURTYPE colourType,
						int bgColour,
						int	plusColour,
						int	plusPersent,
						const utf16* outString, 
						const TextureFont* font ) = 0;//*/

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
						const TextureFont* font ) = 0;//*/
};

void CreateFontManager(IFontManager** pFont);
uint g_fntRandom( uint nMax);
bool g_fntRandPercent(int nPercent);



#endif