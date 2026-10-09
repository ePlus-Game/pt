/***********************************************************************
    filename:   FontManager.h
    created:    24/07/2007
    author:     LiuSiliang

    purpose:    Class FontManager
*************************************************************************/

#include "CommonDef.h"
#include "FontManager.h"
#include <math.h>

 FT_Library	g_FTlib;

/**********************************************************************
	Construction  And Destruction
**********************************************************************/
FontManager::FontManager()
{

}

FontManager::~FontManager()
{
	DestroyAllFont();
    FT_Done_FreeType (g_FTlib);
}

/**********************************************************************
	
**********************************************************************/
FontManager &FontManager::getSingleton()
{
	static FontManager ms_Singleton;
	return ms_Singleton;
}

bool	FontManager::IAddFont( const char *name, const char* fileName )
{
	FreeTypeFont fnt(name, fileName);
	d_FontMap[name] = fnt;
	d_FontMap[name].load();
	return true;
}

void	FontManager::DestroyFont( const std::string &name )
{
	FontMap::iterator pos = d_FontMap.find( name );
	if ( pos != d_FontMap.end())
	{
		d_FontMap.erase( name );
	}
}


void	FontManager::DestroyAllFont( void )
{
	d_FontMap.clear();
}

const FreeTypeFont*	FontManager::GetFont( const std::string &name )
{
	FontMap::iterator pos = d_FontMap.find(name);
	if (pos == d_FontMap.end())
	{
		return NULL;
	}
	return &pos->second;
}

bool	FontManager::IsFontPresent( const std::string &name ) const 
{
	if ( d_FontMap.size() > 0)
	{
		return d_FontMap.find(name) != d_FontMap.end();
	}

	return false;
}

uint	FontManager::utf32_length(const utf16* utf32_str) const
{
	uint cnt = 0;
	while (*utf32_str++)
		cnt++;

	return cnt;
}

bool FontManager::Bitmap16Bit565ToBitmap1Bit(
	unsigned short* pBuffer, 
	unsigned uWidth, 
	unsigned uHeight , 
	unsigned char* pDestBuffer )
{
	if( pBuffer == NULL ||
		uWidth == 0 ||
		uHeight == 0 ||
		pDestBuffer == NULL )
	{
		return NULL;
	}
	
	unsigned uDestWidth = ( uWidth + 31 )/32*4;
	unsigned uDestHeight = uHeight;
	if( ( uDestWidth * uDestHeight  ) > BIT_BUFFER_SIZE )
	{
		//所给的缓冲过少
		return false;
	}

	unsigned char* pBitmap1BitBufferTemp = pDestBuffer;

	unsigned char* pBitmap1BitHelpBuffer = NULL;

	unsigned short* pSrcBuffer = pBuffer;
	unsigned short* pSrcHelpBuffer = NULL;

	unsigned uBitCount = 0;

	unsigned char uColorByte = 0;

	for( unsigned uIndex_y = 0 ; uIndex_y < uHeight ; ++uIndex_y )
	{
		uBitCount = 0;
		pBitmap1BitHelpBuffer = pBitmap1BitBufferTemp;
		pSrcHelpBuffer = pSrcBuffer;
		for( unsigned uIndex_x = 0; uIndex_x < uWidth; ++uIndex_x )
		{
			//只要不是8的倍数就记录其颜色
			if( *pSrcBuffer == 0xffff )
			{
				//白色
				uColorByte |= ( 1 << ( 7 - uBitCount ) );
			}
			++uBitCount;
			if( uBitCount == 8 )
			{
				//到达8个了，压进目标缓冲
				*pBitmap1BitBufferTemp = uColorByte;
				uColorByte = 0;
				uBitCount = 0;
				++pBitmap1BitBufferTemp;
			}
			++pSrcBuffer;
		}
		//检查末尾了。。
		if( uBitCount > 0 )
		{
			*pBitmap1BitBufferTemp =  uColorByte;
			uColorByte = 0;	
		}
		pBitmap1BitBufferTemp = pBitmap1BitHelpBuffer + uDestWidth;
		pSrcBuffer = pSrcHelpBuffer + uWidth; 
	}

	return true;
}	

void	FontManager::FillBMPBuff(int fntSize, int colourType, const utf16* outString, void* outBuffer, const TextureFont* font, int pixel, unsigned short bgcolor16)
{
	if ( font == NULL )
	{
		return;
	}
	int length = utf32_length(outString);
	length = length < MAX_STR_LEN ? length : MAX_STR_LEN;
	length = MAX_STR_LEN;
	int BMPWidth = RGB565_WIDTH( pixel * length );
	int snAddWidth = 0;
	for ( int nNum = 0; nNum < length; nNum++ )
	{
		utf32 ch = outString[nNum];
		int widthOffset = nNum*pixel;
		
		switch(colourType)
		{
		case FMT_BIT:	
		case FMT_R5G6B5:
			{
				uint16* filebuf = (uint16*)outBuffer;
				
				int space = font[nNum].d_space;

				if ( font[nNum].d_space < 0 )
				{
					space = 0;
				}
				if ( font[nNum].d_space >= pixel  )
				{
					space = 0;
				}

				if ( nNum == 0 )
				{
					space = 0;
				}
								
				
				std::string fontName = font[nNum].d_fontName;
				FreeTypeFont* rFnt = (FreeTypeFont*)GetFont(fontName);
				if ( rFnt == NULL )
				{
					return;
				}
				FT_Bitmap* dest = ( FT_Bitmap*)rFnt->drawText(ch, fntSize, font[nNum].d_angle, font[nNum].d_xTrans, font[nNum].d_yTrans, font[nNum].d_xScalc, font[nNum].d_yScalc );
				if ( dest == NULL ||dest->pixel_mode == FT_PIXEL_MODE_MONO)
				{
					return;
				}
				
				FT_Bitmap *glyph_bitmap = dest;
				
				int h = glyph_bitmap->rows < pixel ? glyph_bitmap->rows : pixel;
				int w = glyph_bitmap->width < pixel ? glyph_bitmap->width : pixel;
				
				
				for (int nIndex_y = 0; nIndex_y < h; ++nIndex_y )
				{
					uchar *src = glyph_bitmap->buffer  +( ( h - 1 - nIndex_y ) * glyph_bitmap->pitch );
					switch (glyph_bitmap->pixel_mode)
					{
					case FT_PIXEL_MODE_GRAY:
						{
							ushort *dst = filebuf + nIndex_y * BMPWidth + snAddWidth;
							for (int nIndex_x = 0; nIndex_x < w; ++nIndex_x )
							{
								uchar alpha = *src++;
								if ( alpha > 0 && *dst == bgcolor16 )
								{
									unsigned short usnSrcClr = font[nNum].d_colour;
									unsigned short usnSrcR = (usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F;
									unsigned short usnSrcG = (usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F;
									unsigned short usnSrcB = usnSrcClr & 0x001F;
									
									*dst = (usnSrcB << SHIFTNUM_RGB565_RCOLOR) | (usnSrcG << SHIFTNUM_RGB565_GCOLOR) | usnSrcR;
								}
								dst++;
							}
						}
						break;	
					}
				}
				snAddWidth += pixel - space;
			}
			break;
		default:
			return;
		}
	}
}
void FontManager::IGetFontSize( int fontSize,
						int& width, 
						int& height )
{
    uint horzdpi = 96;
    uint vertdpi = 96;
	width = fnt_max((int)ceil(fontSize * FT_POS_COEF * horzdpi), (int)ceil(fontSize * FT_POS_COEF * vertdpi));
	height = fnt_max((int)ceil(fontSize * FT_POS_COEF * horzdpi), (int)ceil(fontSize * FT_POS_COEF * vertdpi));
}

bool	FontManager::IFontGetTextBmp(
						unsigned char* buffer,
						int& size,
						int	fontSize,
						FONTCOLOURTYPE colourType,
						int bgColour,
						int	plusColour,
						int	plusPersent,
						const utf16* outString, 
						const TextureFont* font )
{
	if ( buffer == NULL )
	{
		return false;
	}

	if ( fontSize <= 0 || fontSize > 40 )
	{
		fontSize = 20;
	}

	BITMAPFILEHEADER bmpfileheader;
	BITMAPINFOHEADER bmpinfohdr;
	memset(&bmpfileheader,0,sizeof(bmpfileheader));
	memset(&bmpinfohdr,0,sizeof(bmpinfohdr));

	int length = utf32_length(outString);
	length = length < MAX_STR_LEN ? length : MAX_STR_LEN;	

	length = MAX_STR_LEN;

    uint horzdpi = 96;
    uint vertdpi = 96;
	int pixel = fnt_max((int)ceil(fontSize * FT_POS_COEF * horzdpi), (int)ceil(fontSize * FT_POS_COEF * vertdpi));

	pixel = pixel < MAX_FONT_PIXEL ? pixel : MAX_FONT_PIXEL;

	int biBitCount = 0;
	int memsize = 0;
	int bitmemsize = 0;
	uint16 filebuf[ RGB565_BIT_MAX_SIZE ];
	uchar bitFilebuf[ BIT_BUFFER_SIZE ];
	memset( bitFilebuf, 0xff, sizeof( uchar ) * BIT_BUFFER_SIZE );

	switch(colourType)
	{
	case FMT_R5G6B5:
		{
			memsize =  RGB565_WIDTH( pixel * length ) * 2* pixel;
			uint16* buf = (uint16*)filebuf;

			uchar r = (uchar)(bgColour >> 16);
			uchar g = (uchar)(bgColour >> 8);
			uchar b = (uchar)bgColour;
			unsigned short bgcolor16 = bgColour;	
			RGB565_FROM_RGB( bgcolor16, r, g, b)
				
			int nIdx = 0;
			int pixelCnt = RGB565_WIDTH( pixel * length ) * pixel;
			for ( nIdx = 0; nIdx < pixelCnt; ++nIdx )
			{
				if ( g_fntRandPercent( plusPersent ) )
				{
					buf[nIdx] = plusColour;
				}
				else
				{
					buf[nIdx] = bgcolor16;
				}
			}

			biBitCount = 16;
			
			FillBMPBuff( fontSize, colourType, outString, filebuf, font, pixel, bgcolor16 );
		}
		break;
	case FMT_BIT:
		{
			uint16* buf = (uint16*)filebuf;

			uchar r = (uchar)(bgColour >> 16);
			uchar g = (uchar)(bgColour >> 8);
			uchar b = (uchar)bgColour;
			unsigned short bgcolor16 = bgColour;	
			RGB565_FROM_RGB( bgcolor16, r, g, b)
				
			int nIdx = 0;
			int pixelCnt = RGB565_WIDTH( pixel * length ) * pixel;
			for ( nIdx = 0; nIdx < pixelCnt; ++nIdx )
			{
				if ( g_fntRandPercent( plusPersent ) )
				{
					buf[nIdx] = plusColour;
				}//*/
				else
				{
					buf[nIdx] = bgcolor16;
				}
			}
			biBitCount = 1;
			
			FillBMPBuff( fontSize, colourType, outString, filebuf, font, pixel, bgcolor16 );
			
			bitmemsize = (pixel*length+31)/32*4*pixel;

			Bitmap16Bit565ToBitmap1Bit( ( uint16* )filebuf, RGB565_WIDTH( pixel * length ), pixel, bitFilebuf);
		}
		break;
	default:
		return false;
	}



	bmpinfohdr.biSize = sizeof(bmpinfohdr);
	bmpinfohdr.biWidth = pixel*length; 
	bmpinfohdr.biHeight = pixel;
	bmpinfohdr.biBitCount = biBitCount;
	bmpinfohdr.biPlanes = 1;
	bmpinfohdr.biSizeImage = 0;

	bmpfileheader.bfType = 0x4D42;
	bmpfileheader.bfReserved1 = 0;
	bmpfileheader.bfReserved2 = 0;
	bmpfileheader.bfOffBits = sizeof(bmpfileheader) + sizeof(bmpinfohdr);
	RGBQUAD bmpQuad[2];
	if ( colourType == FMT_BIT )
	{
		bmpfileheader.bfOffBits += sizeof(bmpQuad);
	}
	else
	{
		 bmpfileheader.bfOffBits += 3 * sizeof( unsigned long );
	}
	if ( colourType == FMT_BIT )
	{
		bmpfileheader.bfSize = bmpfileheader.bfOffBits + bitmemsize;
	}
	else
	{
		bmpinfohdr.biCompression = BI_BITFIELDS;
		bmpfileheader.bfSize = bmpfileheader.bfOffBits + memsize + 3 * sizeof( unsigned long );
	}
	

	size = 0;
	memcpy( buffer, &bmpfileheader, sizeof(bmpfileheader) );
	buffer += sizeof(bmpfileheader);
	size  += sizeof(bmpfileheader);

	memcpy( buffer, &bmpinfohdr, sizeof(bmpinfohdr) );
	buffer += sizeof(bmpinfohdr);
	size += sizeof(bmpinfohdr);

	if ( colourType == FMT_BIT )
	{
		bmpinfohdr.biClrUsed = 2;

		bmpQuad[0].rgbBlue = 0x00;
		bmpQuad[0].rgbGreen = 0x00;
		bmpQuad[0].rgbRed = 0x00;
		bmpQuad[0].rgbReserved = 0x00;
		bmpQuad[1].rgbBlue = 0xff;
		bmpQuad[1].rgbGreen = 0xff;
		bmpQuad[1].rgbRed = 0xff;
		bmpQuad[1].rgbReserved = 0x00;
		memcpy( buffer, &bmpQuad, sizeof(bmpQuad) );
		buffer += sizeof(bmpQuad);
		size += sizeof(bmpQuad);
	}
	if (  colourType == FMT_BIT  )
	{
		memcpy( buffer, bitFilebuf , sizeof(char) * bitmemsize );
		buffer += sizeof(char) * bitmemsize;
		size += sizeof(char) * bitmemsize;
	}
	else
	{
		unsigned long red = 0xf800;
		unsigned long green = 0x7e0;
		unsigned long blue = 0x1f;
		memcpy( buffer, &red, sizeof( unsigned long ) );
		buffer += sizeof(unsigned long);
		size += sizeof(unsigned long);
		memcpy( buffer, &green, sizeof( unsigned long ) );
		buffer += sizeof(unsigned long);
		size += sizeof(unsigned long);
		memcpy( buffer, &blue, sizeof( unsigned long ) );
		buffer += sizeof(unsigned long);
		size += sizeof(unsigned long);
		memcpy( buffer, filebuf , sizeof(uchar) * memsize );
		buffer += sizeof(char) * memsize;
		size  += sizeof(char) * memsize;
	}
	return true;
}

void CreateFontManager(IFontManager** pFontManager)
{
	 FT_Init_FreeType (&g_FTlib);
	*pFontManager = &FontManager::getSingleton();
}

void CreatePlugInterface( )
{
}

uint g_fntRandom( uint nMax)
{
	static unsigned int nRandomSeed = 42;
	if (nMax)
	{
#ifdef _USENEWRANDOMFUNC
		unsigned int f = nRandomSeed * 0x08088405 + 1;
		nRandomSeed = f;
#ifndef _WIN32
		long long t = (long long)f * (long long)nMax;
#else
		_int64 t = (_int64)f * (_int64)nMax;
#endif
		t = t >> 32;
		return (unsigned int)t;
#else
		nRandomSeed = nRandomSeed * 214013L + 2531011L;
		return ((nRandomSeed >> 16) & 0x7fff) % nMax;
#endif		
	}
	else
	{
		return 0;
	}
}

bool g_fntRandPercent(int nPercent)
{
	return ((int)g_fntRandom(100) < nPercent);
}


bool	FontManager::IFontGetTextBmpPosition(
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
						const TextureFont* font )
{
	if ( buffer == NULL )
	{
		return false;
	}

	if ( fontSize <= 0 || fontSize > 40 )
	{
		fontSize = 20;
	}

	BITMAPFILEHEADER bmpfileheader;
	BITMAPINFOHEADER bmpinfohdr;
	memset(&bmpfileheader,0,sizeof(bmpfileheader));
	memset(&bmpinfohdr,0,sizeof(bmpinfohdr));

	int length = utf32_length(outString);
	length = length < MAX_STR_LEN ? length : MAX_STR_LEN;	

	length = MAX_STR_LEN;

    uint horzdpi = 96;
    uint vertdpi = 96;
	int pixel = fnt_max((int)ceil(fontSize * FT_POS_COEF * horzdpi), (int)ceil(fontSize * FT_POS_COEF * vertdpi));

	pixel = pixel < MAX_FONT_PIXEL ? pixel : MAX_FONT_PIXEL;

	int biBitCount = 0;
	int memsize = 0;
	int bitmemsize = 0;
	static uint16 filebuf[ RGB565_BIT_MAX_SIZE ];
	static uchar bitFilebuf[ BIT_BUFFER_SIZE ];
	memset( bitFilebuf, 0xff, sizeof( uchar ) * BIT_BUFFER_SIZE );

	if ( width <= 0)
	{
		width = pixel * length;
	}
	if ( height <= 0)
	{
		height = pixel * length;
	}

	int nWidth = RGB565_WIDTH( width ) < RGB565_WIDTH( pixel * length ) ? RGB565_WIDTH( width ) : RGB565_WIDTH( pixel * length );
	int nHeight = height < 	pixel * length ? height : pixel * length;



	switch(colourType)
	{
	case FMT_R5G6B5:
		{
			memsize =  nWidth * 2 * nHeight ;
			uint16* buf = (uint16*)filebuf;

			uchar r = (uchar)(bgColour >> 16);
			uchar g = (uchar)(bgColour >> 8);
			uchar b = (uchar)bgColour;
			unsigned short bgcolor16 = bgColour;	
			RGB565_FROM_RGB( bgcolor16, r, g, b)
				
			int nIdx = 0;
			int pixelCnt = nWidth * nHeight;
			for ( nIdx = 0; nIdx < pixelCnt; ++nIdx )
			{
				if ( g_fntRandPercent( plusPersent ) )
				{
					buf[nIdx] = plusColour;
				}
				else
				{
					buf[nIdx] = bgcolor16;
				}
			}

			biBitCount = 16;
			
			FillBMPBuffPosition( nWidth, nHeight, fontSize, colourType, outString, filebuf, font, pixel, bgcolor16 );
		}
		break;
	case FMT_BIT:
		{
			uint16* buf = (uint16*)filebuf;

			uchar r = (uchar)(bgColour >> 16);
			uchar g = (uchar)(bgColour >> 8);
			uchar b = (uchar)bgColour;
			unsigned short bgcolor16 = bgColour;	
			RGB565_FROM_RGB( bgcolor16, r, g, b)
				
			int nIdx = 0;
			int pixelCnt = nWidth * nHeight;
			for ( nIdx = 0; nIdx < pixelCnt; ++nIdx )
			{
				if ( g_fntRandPercent( plusPersent ) )
				{
					buf[nIdx] = plusColour;
				}//*/
				else
				{
					buf[nIdx] = bgcolor16;
				}
			}
			biBitCount = 1;

			randomTPCount = randomTPCount < MAX_RANDOM_TP ? randomTPCount : MAX_RANDOM_TP;
			for ( int nRandomIdx = 0;nRandomIdx < randomTPCount; ++nRandomIdx)
			{
				TextureFont randomFont;
				randomFont.d_fontName = randomTPName;
				randomFont.d_xScalc = randomTpScalc;
				randomFont.d_yScalc = randomTpScalc;
				if ( nRandomIdx >= 0 &&  nRandomIdx < length )
				{
					randomFont.d_x = font[nRandomIdx].d_x + g_fntRandom(pixel);
					if ( (g_fntRandPercent(50) || font[nRandomIdx].d_y == 0) && font[nRandomIdx].d_y + pixel < nHeight )
					{
						randomFont.d_y = font[nRandomIdx].d_y + pixel;
					}
					else
					{
						randomFont.d_y = font[nRandomIdx].d_y - pixel;
					}
				}
				else
				{
					randomFont.d_x = font[nRandomIdx].d_x + g_fntRandom(pixel);
					if ( (g_fntRandPercent(50) || font[nRandomIdx].d_y == 0) && font[nRandomIdx].d_y + pixel < nHeight )
					{
						randomFont.d_y = font[nRandomIdx].d_y + pixel;
					}
					else
					{
						randomFont.d_y = font[nRandomIdx].d_y - pixel;
					}
				}
				utf16 f16[2];
				f16[0] = g_fntRandom(0xff);
				f16[1] = 0;
				FillBMPBuffPosition(  nWidth, nHeight, fontSize, colourType, f16, filebuf, &randomFont, pixel, bgcolor16 );
			}
			
			FillBMPBuffPosition(  nWidth, nHeight, fontSize, colourType, outString, filebuf, font, pixel, bgcolor16 );

			
			bitmemsize = (nWidth+31)/32*4*nHeight ;

			Bitmap16Bit565ToBitmap1BitPosition( ( uint16* )filebuf, nWidth, nHeight, bitFilebuf);
		}
		break;
	default:
		return false;
	}



	bmpinfohdr.biSize = sizeof(bmpinfohdr);
	bmpinfohdr.biWidth = nWidth; 
	bmpinfohdr.biHeight = nHeight;
	bmpinfohdr.biBitCount = biBitCount;
	bmpinfohdr.biPlanes = 1;
	bmpinfohdr.biSizeImage = 0;

	bmpfileheader.bfType = 0x4D42;
	bmpfileheader.bfReserved1 = 0;
	bmpfileheader.bfReserved2 = 0;
	bmpfileheader.bfOffBits = sizeof(bmpfileheader) + sizeof(bmpinfohdr);
	RGBQUAD bmpQuad[2];
	if ( colourType == FMT_BIT )
	{
		bmpfileheader.bfOffBits += sizeof(bmpQuad);
	}
	else
	{
		 bmpfileheader.bfOffBits += 3 * sizeof( unsigned long );
	}
	if ( colourType == FMT_BIT )
	{
		bmpfileheader.bfSize = bmpfileheader.bfOffBits + bitmemsize;
	}
	else
	{
		bmpinfohdr.biCompression = BI_BITFIELDS;
		bmpfileheader.bfSize = bmpfileheader.bfOffBits + memsize + 3 * sizeof( unsigned long );
	}
	

	size = 0;
	memcpy( buffer, &bmpfileheader, sizeof(bmpfileheader) );
	buffer += sizeof(bmpfileheader);
	size  += sizeof(bmpfileheader);

	memcpy( buffer, &bmpinfohdr, sizeof(bmpinfohdr) );
	buffer += sizeof(bmpinfohdr);
	size += sizeof(bmpinfohdr);

	if ( colourType == FMT_BIT )
	{
		bmpinfohdr.biClrUsed = 2;

		bmpQuad[0].rgbBlue = 0x00;
		bmpQuad[0].rgbGreen = 0x00;
		bmpQuad[0].rgbRed = 0x00;
		bmpQuad[0].rgbReserved = 0x00;
		bmpQuad[1].rgbBlue = 0xff;
		bmpQuad[1].rgbGreen = 0xff;
		bmpQuad[1].rgbRed = 0xff;
		bmpQuad[1].rgbReserved = 0x00;
		memcpy( buffer, &bmpQuad, sizeof(bmpQuad) );
		buffer += sizeof(bmpQuad);
		size += sizeof(bmpQuad);
	}
	if (  colourType == FMT_BIT  )
	{
		memcpy( buffer, bitFilebuf , sizeof(char) * bitmemsize );
		buffer += sizeof(char) * bitmemsize;
		size += sizeof(char) * bitmemsize;
	}
	else
	{
		unsigned long red = 0xf800;
		unsigned long green = 0x7e0;
		unsigned long blue = 0x1f;
		memcpy( buffer, &red, sizeof( unsigned long ) );
		buffer += sizeof(unsigned long);
		size += sizeof(unsigned long);
		memcpy( buffer, &green, sizeof( unsigned long ) );
		buffer += sizeof(unsigned long);
		size += sizeof(unsigned long);
		memcpy( buffer, &blue, sizeof( unsigned long ) );
		buffer += sizeof(unsigned long);
		size += sizeof(unsigned long);
		memcpy( buffer, filebuf , sizeof(uchar) * memsize );
		buffer += sizeof(char) * memsize;
		size  += sizeof(char) * memsize;
	}
	return true;	
}

void	FontManager::FillBMPBuffPosition(		
						int width,
						int height, 
						int fntSize, 
						int colourType, 
						const utf16* outString, 
						void* outBuffer, 
						const TextureFont* font, 
						int pixel, 
						unsigned short bgcolor16)
{
	if ( font == NULL )
	{
		return;
	}
	int length = utf32_length(outString);
	length = length < MAX_STR_LEN ? length : MAX_STR_LEN;
	int BMPWidth = width;
	int BMPHeight = height;
	for ( int nNum = 0; nNum < length; nNum++ )
	{
		utf32 ch = outString[nNum];
		int widthOffset = nNum*pixel;
		
		switch(colourType)
		{
		case FMT_BIT:	
		case FMT_R5G6B5:
			{
				uint16* filebuf = (uint16*)outBuffer;
				
				int space = font[nNum].d_space;
				
				if ( font[nNum].d_space < 0 )
				{
					space = 0;
				}
				if ( font[nNum].d_space >= pixel  )
				{
					space = 0;
				}
				
				if ( nNum == 0 )
				{
					space = 0;
				}
				
				
				std::string fontName = font[nNum].d_fontName;
				FreeTypeFont* rFnt = (FreeTypeFont*)GetFont(fontName);
				if ( rFnt == NULL )
				{
					return;
				}
				FT_Bitmap* dest = ( FT_Bitmap*)rFnt->drawText(ch, fntSize, font[nNum].d_angle, font[nNum].d_xTrans, font[nNum].d_yTrans, font[nNum].d_xScalc, font[nNum].d_yScalc );
				if ( dest == NULL ||dest->pixel_mode == FT_PIXEL_MODE_MONO)
				{
					return;
				}
				
				FT_Bitmap *glyph_bitmap = dest;
				
				int h = glyph_bitmap->rows < pixel ? glyph_bitmap->rows : pixel;
				int w = glyph_bitmap->width < pixel ? glyph_bitmap->width : pixel;
				
				
				int nOffsetX = font[nNum].d_x;
				int nOffsetY = font[nNum].d_y;
				nOffsetY  = BMPHeight - nOffsetY - h;
				if( nOffsetX < 0 )
				{
					nOffsetX = 0;
				}
				else
				if( nOffsetX > BMPWidth - w -1 )	
				{
					nOffsetX = BMPWidth - w - 1;
				}

				if( nOffsetY < 0 )
				{
					nOffsetY = 0;//pixel * length - h - 1;
				}
				else
				if(  nOffsetY > BMPHeight - h -1 )
				{
					nOffsetY = BMPHeight - h - 1;
				}

				filebuf += nOffsetX + nOffsetY * BMPWidth;	
				for (int nIndex_y = 0; nIndex_y < h; ++nIndex_y )
				{
					uchar *src = glyph_bitmap->buffer  +( ( h - 1 - nIndex_y ) * glyph_bitmap->pitch );
					switch (glyph_bitmap->pixel_mode)
					{
					case FT_PIXEL_MODE_GRAY:
						{
							ushort *dst = filebuf + nIndex_y * BMPWidth;
							for (int nIndex_x = 0; nIndex_x < w; ++nIndex_x )
							{
								uchar alpha = *src++;
								if ( alpha > 0 && *dst == bgcolor16 )
								{
									unsigned short usnSrcClr = font[nNum].d_colour;
									unsigned short usnSrcR = (usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F;
									unsigned short usnSrcG = (usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F;
									unsigned short usnSrcB = usnSrcClr & 0x001F;
									
									*dst = (usnSrcB << SHIFTNUM_RGB565_RCOLOR) | (usnSrcG << SHIFTNUM_RGB565_GCOLOR) | usnSrcR;
								}
								dst++;
							}
						}
						break;	
					}
				}
				//snAddWidth += pixel - space;
			}
			break;
		default:
			return;
		}
	}
}

bool FontManager::Bitmap16Bit565ToBitmap1BitPosition(
													 unsigned short* pBuffer, 
													 unsigned uWidth, 
													 unsigned uHeight , 
													 unsigned char* pDestBuffer )
{
	if( pBuffer == NULL ||
		uWidth == 0 ||
		uHeight == 0 ||
		pDestBuffer == NULL )
	{
		return NULL;
	}
	
	unsigned uDestWidth = ( uWidth + 31 )/32*4;
	unsigned uDestHeight = uHeight;
	if( ( uDestWidth * uDestHeight  ) > BIT_BUFFER_SIZE )
	{
		//所给的缓冲过少
		return false;
	}
	
	unsigned char* pBitmap1BitBufferTemp = pDestBuffer;
	
	unsigned char* pBitmap1BitHelpBuffer = NULL;
	
	unsigned short* pSrcBuffer = pBuffer;
	unsigned short* pSrcHelpBuffer = NULL;
	
	unsigned uBitCount = 0;
	
	unsigned char uColorByte = 0;
	
	for( unsigned uIndex_y = 0 ; uIndex_y < uHeight ; ++uIndex_y )
	{
		uBitCount = 0;
		pBitmap1BitHelpBuffer = pBitmap1BitBufferTemp;
		pSrcHelpBuffer = pSrcBuffer;
		for( unsigned uIndex_x = 0; uIndex_x < uWidth; ++uIndex_x )
		{
			//只要不是8的倍数就记录其颜色
			if( *pSrcBuffer == 0xffff )
			{
				//白色
				uColorByte |= ( 1 << ( 7 - uBitCount ) );
			}
			++uBitCount;
			if( uBitCount == 8 )
			{
				//到达8个了，压进目标缓冲
				*pBitmap1BitBufferTemp = uColorByte;
				uColorByte = 0;
				uBitCount = 0;
				++pBitmap1BitBufferTemp;
			}
			++pSrcBuffer;
		}
		//检查末尾了。。
		if( uBitCount > 0 )
		{
			*pBitmap1BitBufferTemp =  uColorByte;
			uColorByte = 0;	
		}
		pBitmap1BitBufferTemp = pBitmap1BitHelpBuffer + uDestWidth;
		pSrcBuffer = pSrcHelpBuffer + uWidth; 
	}
	
	return true;
}	