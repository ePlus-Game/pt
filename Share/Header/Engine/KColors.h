//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KColors.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KColors_H
#define KColors_H
//---------------------------------------------------------------------------
extern ENGINE_API	BYTE	g_Red(WORD wColor);
extern ENGINE_API	BYTE	g_Green(WORD wColor);
extern ENGINE_API	BYTE	g_Blue(WORD wColor);
extern ENGINE_API	WORD	g_RGB555(int nRed, int nGreen, int nBlue);
extern ENGINE_API	WORD	g_RGB565(int nRed, int nGreen, int nBlue);
extern ENGINE_API	void	g_555To565(int nWidth, int nHeight, void* lpBitmap);
extern ENGINE_API	void	g_565To555(int nWidth, int nHeight, void* lpBitmap);
extern ENGINE_API	WORD	(*g_RGB)(int nRed, int nGreen, int nBlue);
//---------------------------------------------------------------------------
#define _RGB565FROM16BIT(RGB, r,g,b) { *r = ( ((RGB) >> 11) & 0x1f); *g = (((RGB) >> 5) & 0x3f); *b = ((RGB) & 0x1f); }

#define RGB16BIT565_ALPHA_LEVEL 32
extern ENGINE_API  void CreateAlphaTableForRGB16BIT565();
extern ENGINE_API unsigned short* GetAlphaAddr(unsigned char alpha);
extern ENGINE_API unsigned short g_ARGB32BIT_TO_RGB16BIT565(DWORD color);
extern ENGINE_API unsigned int   g_RGB565BIT_TO_ARGB32BIT(unsigned short color565);
extern ENGINE_API unsigned int* alphaTableAddPoint;
#endif
