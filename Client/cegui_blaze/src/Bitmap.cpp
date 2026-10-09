/*@@

	Copyright (c) Kingsoft Blaze Game Studio. All rights reserved. 

	Created_datetime : 2008-6-26

	File Name :	KBitmap.cpp

	Author : zhangpengcheng (zhangpengcheng@kingsoft.com)

	Description : 

	Change List :

	1.create file (zhangpengcheng)

@@*/

//////////////////////////////////////////////////////////////////////////
//
//		include files
//
//////////////////////////////////////////////////////////////////////////

#include "Bitmap.h"
#include "KPakFile.h"

Bitmap::Bitmap( )
{

	m_Type		= BITMAP_TYPE_R5G6B5;
	m_uHeight	= 0;
	m_uWidth	= 0;
	m_uPitch	= 0;
}

Bitmap::~Bitmap( )
{
	Free( );
}


bool
Bitmap::LoadFromMemory( void* pBuffer, bool bTrans /* = true  */)
{
	Free( );

	unsigned char* pSrcBuffer = ( unsigned char* )pBuffer;

	BITMAPFILEHEADER* pFileHead = ( BITMAPFILEHEADER* ) pSrcBuffer;
	if( pFileHead ->bfType != 0x4d42 )
	{
		return false;
	}

	BITMAPINFOHEADER* pInfoHead = ( BITMAPINFOHEADER* )( pSrcBuffer + sizeof( BITMAPFILEHEADER ) );

	switch( pInfoHead->biBitCount )
	{
	case 1:
		{
			
			if( bTrans )
			{
				memcpy( m_pPal32,pSrcBuffer + sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ), sizeof( KPAL32 ) * 2 );
				
				unsigned int uImageSize  = pFileHead->bfSize - sizeof( BITMAPFILEHEADER ) - sizeof( BITMAPINFOHEADER ) - sizeof( KPAL32 ) * 2;
				
				unsigned char pDestBuffer[ _MAX_BITMAP_SIZE ];

				if( uImageSize > _MAX_BITMAP_SIZE )
				{
					return false;
				}
				memcpy( pDestBuffer, pSrcBuffer + pFileHead->bfOffBits,uImageSize );
				
		    	unsigned uPitch = uImageSize / pInfoHead->biHeight;

				if( !_Bitmap1BitToBitmapRGB565(
						pDestBuffer,
						pInfoHead->biWidth,
						pInfoHead->biHeight,
						uPitch,
						m_pPal32 ) )
				{

					return false;
				}
			
				return true;
			}


		}
		break;
	default:
		return false;
	}
	return false;
}

bool
Bitmap::Load( const char* pFileName, bool bTrans )
{

	Free( );
	if ( pFileName == NULL )
	{
		return FALSE;
	}
	char szFileName[256];
	strcpy( szFileName, pFileName );
	if ( szFileName[0] != '\\' )
	{
		sprintf( szFileName, "\\%s", pFileName );
	}

	int nStrLen = strlen( szFileName );
	for ( int nIdx = 0; nIdx < nStrLen; ++nIdx )
	{
		if ( szFileName[nIdx] == '/')
		{
			szFileName[nIdx] = '\\';
		}
	}//*/


	KPakFile	File;

	// open the file
	if ( !File.Open( szFileName ) )
		return FALSE;

	BITMAPFILEHEADER BMFH;
	BITMAPINFOHEADER BMIH;
	File.Read( &BMFH, sizeof( BITMAPFILEHEADER ) );

	if( BMFH.bfType != 0x4D42 )
	{
		return false;
	}

	File.Read( &BMIH, sizeof( BITMAPINFOHEADER ) );
	switch( BMIH.biBitCount )
	{
	case 1:
		{

		
			if( bTrans )
			{
				File.Read( m_pPal32, sizeof( KPAL32 ) * 2 );
				unsigned int uImageSize  = BMFH.bfSize - sizeof( BITMAPFILEHEADER ) - sizeof( BITMAPINFOHEADER ) - sizeof( KPAL32 ) * 2;
				unsigned char pBuffer[ _MAX_BITMAP_SIZE ];
				if( uImageSize > _MAX_BITMAP_SIZE )
				{
					return false;
				}
				//			File.Seek( -( ( int )BMIH.biSizeImage ), FILE_END );
		    	File.Read( pBuffer, uImageSize );
				unsigned uPitch = uImageSize / BMIH.biHeight;
				if( !_Bitmap1BitToBitmapRGB565( 
						pBuffer,
						BMIH.biWidth,
						BMIH.biHeight,
						uPitch,
						m_pPal32 ) )
				{

					return false;
				}

				return true;

			}
			else
			{
				File.Read( m_pPal32, sizeof( KPAL32 ) * 2 );
				unsigned int uImageSize  = BMFH.bfSize - sizeof( BITMAPFILEHEADER ) - sizeof( BITMAPINFOHEADER ) - sizeof( KPAL32 ) * 2;
				if( uImageSize > _MAX_BITMAP_SIZE )
				{
					return false;
				}
				//			File.Seek( -( ( int )BMIH.biSizeImage ), FILE_END );
				File.Read( m_pBuffer, uImageSize );
				unsigned uPitch = uImageSize / BMIH.biHeight;
				
				m_uWidth    = BMIH.biWidth;
				m_uHeight   = BMIH.biHeight;
				m_Type		= BITMAP_TYPE_1BIT;
				m_uPitch	= uImageSize / BMIH.biHeight;
				return true;
			}
		}
		return false;	
	}

	return false;
}


void
Bitmap::Free( )
{



}


unsigned int
Bitmap::GetWidth( ) const
{
	return m_uWidth;
}

unsigned int
Bitmap::GetHeight( ) const
{
	 return m_uHeight;
}

BitmapType
Bitmap::GetBitmapType( ) const
{
	return m_Type;
}

const unsigned char* 
Bitmap::GetBuffer( ) const
{
	return m_pBuffer;
}

const KPAL32* 
Bitmap::GetPal32( ) const
{
	return m_pPal32;
}


bool 
Bitmap::_Bitmap1BitToBitmapRGB565(
	void*		 pBuffer, 
	unsigned int uWidth, 
	unsigned int uHeight, 
	unsigned int uPitch,
	KPAL32*		 pPal32 )
{
	if( uWidth == 0 || 
		uHeight == 0 || 
		pPal32 == 0 )
	{
		return false;
	}

	Free( );

	m_uWidth = uWidth;
	m_uHeight = uHeight;
	m_Type    = BITMAP_TYPE_R5G6B5;
	m_uPitch = uPitch;

	unsigned int uBitmapSize = m_uWidth * m_uHeight ;

	unsigned short* pDestBuffer = ( unsigned short* )m_pBuffer;

	unsigned short* pDestHelpBuffer = pDestBuffer;


	unsigned char* pSrcBuffer = ( unsigned char* )pBuffer + uPitch * ( uHeight -1 );


	unsigned char* pSrcHelpBuffer = pSrcBuffer;
	unsigned int uCount = 0;

	unsigned short uPalColor[2] = { 0 };
	uPalColor[0] = ( ( pPal32[0].Red >> 3 ) << 11 ) |
				   ( ( pPal32[0].Green >> 2 ) << 6 ) |
				   ( ( pPal32[0].Blue >> 3 ) );

	uPalColor[1] = ( ( pPal32[1].Red >> 3 ) << 11 ) |
				   ( ( pPal32[1].Green >> 2 ) << 6 ) |
				   ( ( pPal32[1].Blue >> 3 ) );

	for( unsigned uIndex_y = 0; uIndex_y < m_uHeight; ++uIndex_y )
	{
		pSrcHelpBuffer = pSrcBuffer;
		pDestHelpBuffer = pDestBuffer;
		for( unsigned uIndex_x = 0; uIndex_x < m_uWidth; )
		{
			unsigned char colorIdx = *pSrcBuffer;
			if( uIndex_x + 8 < m_uWidth )
			{
				for( unsigned uIdx = 0; uIdx < 8; ++uIdx )
				{
					unsigned uPalIdx = ( ( colorIdx >> ( 7 - uIdx ) ) & 1 );
					*pDestBuffer = uPalColor[ uPalIdx ];
					++pDestBuffer;
				}
			}
			else
			{
				uCount = m_uWidth - uIndex_x;

				for( unsigned uIdx = 0; uIdx < uCount; ++uIdx )
				{
					unsigned uPalIdx = ( ( colorIdx >> ( 7 -  uIdx ) ) & 1 );
					*pDestBuffer = uPalColor[ uPalIdx ];
					++pDestBuffer;
				}
			}
			++pSrcBuffer;
			uIndex_x += 8;
		}
		pDestBuffer = pDestHelpBuffer + m_uWidth;
		pSrcBuffer  = pSrcHelpBuffer  - uPitch;
	}
	return true;	
}