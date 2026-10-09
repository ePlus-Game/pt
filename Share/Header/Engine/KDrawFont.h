//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawFont.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KDrawFont_H
#define KDrawFont_H

#include "KCanvas.h"

#define BYTECOUNT_DSTSCREENPIXEL	2
#define CLRMASK_RGB565_RB			0x001F001F
#define CLRMASK_RGB565_G			0x003F003F
#define CLRMASKR_RGB565_R			0x07FF07FF
#define CLRMASKR_RGB565_G			0xF81FF81F
#define CLRMASKR_RGB565_B			0xFFE0FFE0

#define SHIFTNUM_RGB565_RCOLOR		11
#define SHIFTNUM_RGB565_GCOLOR		5

//---------------------------------------------------------------------------

void	g_DrawFontEx(void* node, void* canvas);
void	g_DrawFontBorderEx(void* node, void* canvas);
void	g_DrawFontWithBorderEx(void* node, void* canvas);

void	g_DrawFont(void* node, void* canvas);
void	g_DrawFontWithBorder(void* node, void* canvas);
void	g_DrawFontWithBorderToBuff(void* node, void* canvas, void* lpBuffer);
void	g_DrawFontSolid(void* node, void* canvas);

inline void __DrawFontCoreMin(unsigned short unFontColor, 
							  unsigned short **ppDstBuf, 
							  int nSameCount)
{
	unsigned short *pDstBuf = *ppDstBuf;

	__asm
	{
		MOV EDI, pDstBuf; // target buffer
		PXOR MM6, MM6;
		XOR EAX, EAX;
		MOV AX, [unFontColor];
		MOVD MM6, EAX;	// storage source color
		PUNPCKLWD MM6, MM6;
		PUNPCKLDQ MM6, MM6;

		MOV ECX, [nSameCount];
		__CurrentSameBegin:
		CMP ECX, 0;
		JLE __CurrentSameExit;

		CMP ECX, 4;
		JL __DoHalfWrite;
		SUB ECX, 4;
		MOVQ [EDI], MM6;
		ADD EDI, 8;
		ADD ESI, 8;
		JMP __DoFinishWrite;
		__DoHalfWrite:
		MOV EBX, ECX;
		XOR ECX, ECX;
		CMP EBX, 2;
		JL __DoLeftWrite;
		MOVD [EDI], MM6;
		ADD EDI, 4;
		ADD ESI, 4;
		SUB EBX, 2;
		PSRLQ MM6, 32;
		__DoLeftWrite:
		CMP EBX, 0;
		JLE __DoFinishWrite;
		MOVD EAX, MM6;
		MOV [EDI], AX;
		ADD EDI, 2;
		ADD ESI, 2;
		__DoFinishWrite:

		JMP __CurrentSameBegin;
		__CurrentSameExit:
		MOV [pDstBuf], EDI;
		EMMS;
	}

	*ppDstBuf = pDstBuf;
}

inline void __DrawFontCore(unsigned short nSrcR, 
						   unsigned short nSrcG, 
						   unsigned short nSrcB, 
						   int nSameCount, 
						   int nSameAlphaD, 
						   unsigned short **ppDstBuf)
{
	unsigned short *pDstBuf = *ppDstBuf;

	__int64 n64ClrMask1F = 0x001F001F001F001F;
	__int64 n64ClrMask3F = 0x003F003F003F003F;
	__asm
	{
		MOV EDI, pDstBuf; // target buffer
		PXOR MM6, MM6;
		PXOR MM5, MM5;
		PXOR MM4, MM4;
		XOR EAX, EAX;
		MOV AX, [nSrcR];
		MOVD MM6, EAX;
		PUNPCKLWD MM6, MM6;
		PUNPCKLDQ MM6, MM6;
		XOR EAX, EAX;
		MOV AX, [nSrcG];
		MOVD MM5, EAX;
		PUNPCKLWD MM5, MM5;
		PUNPCKLDQ MM5, MM5;
		XOR EAX, EAX;
		MOV AX, [nSrcB];
		MOVD MM4, EAX;
		PUNPCKLWD MM4, MM4;
		PUNPCKLDQ MM4, MM4;

		MOV EAX, [nSameCount];
		__CurrentSameBegin:
		CMP EAX, 0;
		JLE __CurrentSameExit;
		MOV EBX, 4;
		CMP EAX, 4;
		JGE __EnoughClrIndex;
		MOV EBX, EAX;
		__EnoughClrIndex:
		SUB EAX, EBX;

		// Process begin
		// {
		MOVQ MM3, [EDI];	// MM3 storage target R color
		MOVQ MM2, MM3;		// MM2 storage target G color
		MOVQ MM1, MM3;		// MM1 storage target B color
		PSRLW MM3, 11;
		PSRLW MM2, 5;
		MOVQ MM0, [n64ClrMask3F];
		PAND MM2, MM0;
		MOVQ MM0, [n64ClrMask1F];
		PAND MM1, MM0;
		MOVD MM0, [nSameAlphaD];
		PUNPCKLWD MM0, MM0;
		PUNPCKLDQ MM0, MM0;
		PMULLW MM3, MM0;
		PMULLW MM2, MM0;
		PMULLW MM1, MM0;
		PADDUSW MM3, MM6;
		PADDUSW MM2, MM5;
		PADDUSW MM1, MM4;
		PSRLW MM3, 8;
		PSRLW MM2, 8;
		PSRLW MM1, 8;
		PSLLW MM3, 11;
		PSLLW MM2, 5;
		POR MM3, MM2;
		POR MM3, MM1;
		// Write back to memory
		CMP EBX, 4;
		JL __DoHalfWrite;
		MOVQ [EDI], MM3;
		ADD EDI, 8;
		JMP __DoFinishWrite;
		__DoHalfWrite:
		CMP EBX, 2;
		JL __DoLeftWrite;
		MOVD [EDI], MM3;
		ADD EDI, 4;
		SUB EBX, 2;
		PSRLQ MM3, 32;
		__DoLeftWrite:
		CMP EBX, 0;
		JLE __DoFinishWrite;
		MOVD EBX, MM3;
		MOV [EDI], BX;
		ADD EDI, 2;
		__DoFinishWrite:
		// }
		// Process finished
		JMP __CurrentSameBegin;
		__CurrentSameExit:
		MOV [pDstBuf], EDI;
		EMMS;
	}

	*ppDstBuf = pDstBuf;
}

inline void g_DrawFontWithBorderEx_Core(void* node, void* canvas, int nPitch, unsigned short* lpBuffer, unsigned short fontcolor16 )
{
	KDrawNodeFontEx* pNode		= (KDrawNodeFontEx *)node;
	KCanvas* pCanvas			= (KCanvas *)canvas;
	long nX						= pNode->m_nX;						// x coord
	long nY						= pNode->m_nY;						// y coord
	long nWidth					= pNode->m_nWidth;					// width of sprite
	long nHeight				= pNode->m_nHeight;					// height of sprite
	unsigned char*	lpBitmap	= (unsigned char*)pNode->m_pBitmap;	// sprite pointer


	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch / 2 - Clipper.width;

	// 计算位图下一行的偏移
	long BitmapOffset = pNode->m_nPitch - Clipper.width;
	
	int nLoopY = 0;
	int nLoopX = 0;

	//正常绘制
	lpBuffer += (nPitch/2 * Clipper.y + Clipper.x );

	for ( nLoopY = 0; nLoopY < Clipper.height; ++nLoopY )
	{
		for ( nLoopX = 0; nLoopX < Clipper.width; ++nLoopX )
		{
			int nSameAlpha = *lpBitmap++;
			nSameAlpha = (nSameAlpha * pNode->m_nAlpha) / 255;
			if (  nSameAlpha )
			{
				unsigned short usnSrcClr = fontcolor16;
 				unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
				unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
				unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

				unsigned short usnDstClr = *lpBuffer;
				int nSameAlphaD = 255 - nSameAlpha;
				unsigned short usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
				unsigned short usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
				unsigned short usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
				*lpBuffer++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;

			}
			else
			{
				lpBuffer++;
			}
 
		}
		lpBuffer += ScreenOffset;
		lpBitmap += BitmapOffset;
	}

}
//---------------------------------------------------------------------------
#endif
