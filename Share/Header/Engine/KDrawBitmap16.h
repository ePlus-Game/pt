//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawBitmap16.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KDrawBitmap16_H
#define KDrawBitmap16_H
//---------------------------------------------------------------------------
#include "mmintrin.h"
class CMMXUnsigned16Saturated
{
public:
	CMMXUnsigned16Saturated()
	{
	}
	CMMXUnsigned16Saturated( DWORD dw )
	{
		m_m64 = _m_from_int( dw );
	}
	CMMXUnsigned16Saturated( __m64 m64 )
	{
		m_m64 = m64;
	}
	CMMXUnsigned16Saturated( ULONGLONG qw )
	{
		m_m64.m64_u64 = qw;
	}

	CMMXUnsigned16Saturated& operator=( DWORD dw )
	{
		m_m64 = _m_from_int( dw );
		return( *this );
	}
	CMMXUnsigned16Saturated& operator=( const CMMXUnsigned16Saturated& m )
	{
		m_m64 = m.m_m64;
		return( *this );
	}
	CMMXUnsigned16Saturated& operator=( ULONGLONG qw )
	{
		m_m64.m64_u64 = qw;
		return( *this );
	}

	operator __m64() const
	{
		return( m_m64 );
	}

	operator ULONGLONG() const
	{
		return( m_m64.m64_u64 );
	}

	void Clear()
	{
		m_m64 = _mm_setzero_si64();
	}

	CMMXUnsigned16Saturated& operator+=( const CMMXUnsigned16Saturated& m )
	{
		m_m64 = _mm_adds_pu16( m_m64, m );
		return( *this );
	}
	CMMXUnsigned16Saturated& operator-=( const CMMXUnsigned16Saturated& m )
	{
		m_m64 = _mm_subs_pu16( m_m64, m );
		return( *this );
	}
	CMMXUnsigned16Saturated& operator>>=( int nBits )
	{
		m_m64 = _mm_srli_pi16( m_m64, nBits );
		return( *this );
	}

	CMMXUnsigned16Saturated& operator<<=( int nBits )
	{
		m_m64 = _mm_slli_pi16( m_m64, nBits );
		return( *this );
	}

	CMMXUnsigned16Saturated& operator&=( const CMMXUnsigned16Saturated& m )
	{
		m_m64 = _mm_and_si64(m_m64, m);
		return(*this);
	}

	CMMXUnsigned16Saturated& operator|=( const CMMXUnsigned16Saturated& m )
	{
		m_m64 = _mm_or_si64(m_m64, m);
		return(*this);
	}

	CMMXUnsigned16Saturated& AndNot( const CMMXUnsigned16Saturated& m )
	{
		m_m64 = _mm_andnot_si64(m, m_m64);
		return(*this);
	}

	DWORD PackBytes() const
	{
		return( _m_to_int( _mm_packs_pu16( m_m64, _mm_setzero_si64() ) ) );
	}

	ULONGLONG PackBytes( const CMMXUnsigned16Saturated& mUpper ) const
	{
		return( _mm_packs_pu16( m_m64, mUpper ).m64_u64 );
	}

	void UnpackBytesLo( DWORD dw )
	{
		m_m64 = _mm_unpacklo_pi8( _m_from_int( dw ), _mm_setzero_si64() );
	}
	void UnpackBytesHi( DWORD dw )
	{
		m_m64 = _mm_unpacklo_pi8( _mm_setzero_si64(), _m_from_int( dw ) );
	}

	void UnpackBytes(CMMXUnsigned16Saturated &mmLeft)
	{
		mmLeft = _mm_unpackhi_pi8( m_m64, _mm_setzero_si64() );
		m_m64 = _mm_unpacklo_pi8( m_m64, _mm_setzero_si64() );
	}

public:
	__m64 m_m64;
};

inline CMMXUnsigned16Saturated operator+( const CMMXUnsigned16Saturated& m1, const CMMXUnsigned16Saturated& m2 )
{
	return( _mm_adds_pu16( m1.m_m64, m2.m_m64 ) );
}

inline CMMXUnsigned16Saturated operator-( const CMMXUnsigned16Saturated& m1, const CMMXUnsigned16Saturated& m2 )
{
	return( _mm_subs_pu16( m1.m_m64, m2.m_m64 ) );
}

inline CMMXUnsigned16Saturated operator<<( const CMMXUnsigned16Saturated& m1, int nBits )
{
	return( _mm_slli_pi16( m1, nBits ) );
}

inline CMMXUnsigned16Saturated operator>>( const CMMXUnsigned16Saturated& m1, int nBits )
{
	return( _mm_srli_pi16( m1, nBits ) );
}

inline CMMXUnsigned16Saturated operator&( const CMMXUnsigned16Saturated& m1, const CMMXUnsigned16Saturated& m2 )
{
	return(_mm_and_si64(m1, m2));
}


//void g_DrawBitmap16(void* node, void* canvas);
void g_DrawBitmap24Alpha(void* pNodeData, void* pCanvasData);
void g_DrawBitmap24Alpha_MMX(void* pNodeData, void* pCanvasData);
void g_DrawBitmap24Alpha_SSE(void* pNodeData, void* pCanvasData);
void g_DrawBitmap24Alpha_SSE2(void* pNodeData, void* pCanvasData);
void g_DrawBitmap24Alpha_SSE3(void* pNodeData, void* pCanvasData);
void g_DrawBitmap24Alpha_SSE4(void* pNodeData, void* pCanvasData);

void g_DrawBitmap16Alpha_SSE2(void* node, void* canvas);
void g_DrawBitmap16_MMX(void* node, void* canvas);
void g_DrawBitmap16mmx(void* node, void* canvas);
void g_DrawBitmap16win(void* node, void* canvas);
void g_DrawBitmap16Alpha(void* node, void* canvas);
//add by render//
void g_DrawBitmap16OnDc16(void* node,void* canvas,int dx,int dy,int dWidth,int dHeight,unsigned long hdc);
void g_DrawBitmap16OnDc32(void* node,void* canvas,int dx,int dy,int dWidth,int dHeight,unsigned long hdc);
void  Flip_Bitmap(unsigned char* bitmap_buffer,
		          int bytes_per_line,int height);
void g_DrawBitmapOnDC(unsigned long hDc,unsigned long hBitmap,int nX,int nY);
void g_DrawBitmap320nDC(unsigned long hdc,unsigned long hBitmap,int nX,int nY);
void g_DrawBitmap32OnDc32WidthAlpha(unsigned long hdc,unsigned long hBitmap,int nX,int nY);
void g_DrawBitmap16OnDc32WidthAlpha(unsigned long hdc,unsigned long hBitmap,int nX,int nY);
//---------------------------------------------------------------------------
#endif
