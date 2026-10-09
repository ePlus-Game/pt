/*@@

	Copyright (c) Kingsoft Blaze Game Studio. All rights reserved. 

	Created_datetime : 2008-6-26

	File Name :	KBitmap.h

	Author : zhangpengcheng (zhangpengcheng@kingsoft.com)

	Description : KBitmap

	Change List :

	1.create file (zhangpengcheng)

@@*/


#ifndef  _KBITMAP_H_
#define  _KBITMAP_H_
//////////////////////////////////////////////////////////////////////////
//
//		Include files
//
//////////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KPalette.h"

//////////////////////////////////////////////////////////////////////////
//
//		Class Declare
//
//////////////////////////////////////////////////////////////////////////

/*
 * 所有被加载的BITMAP文件，加载时候都默认被转换成Bitmap565格式
 */
enum BitmapType
{
	BITMAP_TYPE_A8R8G8B8 = 0,
	BITMAP_TYPE_R5G5B5,
	BITMAP_TYPE_R5G6B5,
	BITMAP_TYPE_8BIT,
	BITMAP_TYPE_1BIT,
};

#define _MAX_BITMAP_WIDTH  600
#define _MAX_BITMAP_HEIGHT 100
#define _MAX_BITMAP_COLORBPP 16

#define _MAX_BITMAP_SIZE ( _MAX_BITMAP_WIDTH * _MAX_BITMAP_HEIGHT * ( _MAX_BITMAP_COLORBPP >> 3 ) )
class Bitmap
{
public:
	Bitmap( );

	~Bitmap( );
public:
	bool  Load( const char* pFileName, bool bTrans = true );

	bool  LoadFromMemory( void* pBuffer, bool bTrans = true );

	void  Free( );

public:
	unsigned int	GetWidth( ) const;
	unsigned int	GetHeight( ) const;
	const unsigned char* GetBuffer( ) const;
	BitmapType		GetBitmapType( ) const;
	const KPAL32*			GetPal32( ) const;

private:

	bool _Bitmap1BitToBitmapRGB565(  
		void*			pBuffer,
		unsigned int	uWidth,
		unsigned int	uHeight,
		unsigned int    uPitch,
		KPAL32*			pPal );

private:
	unsigned int	m_uWidth;

	unsigned int	m_uHeight;

	unsigned int    m_uPitch;

	unsigned char	m_pBuffer[_MAX_BITMAP_SIZE ];

	BitmapType      m_Type;

	KPAL32         m_pPal32[ 256 ];
};
#endif