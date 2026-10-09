//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawBitmap16.cpp
// Date:	2000.08.08
// Code:	Daniel Wang, Wooy(Wu yue)
// Desc:	Bitmap Drawing Functions
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KCanvas.h"
#include "KDrawBitmap16.h"
#include "KDDraw.h"
#include <windowsx.h>
#include "KColors.h"

#define  BYTECOUNT_DSTSCREENPIXEL 2
#define SHIFTNUM_RGB565_RCOLOR		11
#define SHIFTNUM_RGB565_GCOLOR		5
#define SHIFTNUM_RGB555_RCOLOR		10
#define SHIFTNUM_RGB555_GCOLOR		5

#define RGB_FROM_RGB555(Pixel, a, r, g, b)					\
{									\
	a = (Pixel & 0x8000)>>15;								\
	r = ((Pixel&0x7C00)>>10);		 			\
	g = ((Pixel&0x03E0)>>5); 					\
	b = (Pixel&0x001F); 					\
}

#define RGB565_FROM_RGB(Pixel, r, g, b)					\
{									\
	Pixel = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);			\
}
#define RGB555_FROM_RGB(Pixel, r, g, b)					\
{									\
	Pixel = ((r>>3)<<10)|((g>>3)<<5)|(b>>3);			\
}

void g_DrawBitmap24Alpha(void* pNodeData, void* pCanvasData)
{
	KDrawNode *ptagNode = (KDrawNode *)pNodeData;
	KCanvas *pclsCanvas = (KCanvas *)pCanvasData;

	// Get the real draw map
	KClipper tagClipper;
	if( pclsCanvas->MakeClip(ptagNode->m_nX, ptagNode->m_nY, ptagNode->m_nWidth, ptagNode->m_nHeight, &tagClipper ) == 0 )
	{
		return;
	}

	// Get the screen back buffer
	int nBackBufferWidth = 0;
	void* pBackBuffer = pclsCanvas->LockCanvas(nBackBufferWidth);
	if(pBackBuffer == NULL)
	{
		return;
	}

	byte  *pSrcBuf = (byte *)ptagNode->m_pBitmap;
	unsigned short *pDstBuf = (unsigned short *)pBackBuffer;
	pDstBuf += tagClipper.y * (nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL) + tagClipper.x;
	int nBackBufLinePixel = nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL;

	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data

	int nClipSkip = ptagNode->m_nWidth * tagClipper.top;
	pSrcBuf += nClipSkip;	

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned short *pDstCurLineHead = pDstBuf;
		nRemainSrcHeight --;

		int nCurLinePos		= 0;
		int nCurUnitEndPos	= 0;
		
		nCurLinePos			= 0;
		nCurUnitEndPos		= tagClipper.left;
		
		// Deal with left spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			// Skip left clip spr data first
			*pSrcBuf += 3;
			nCurLinePos++;
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nCurLinePos++;
			nSameAlpha = *pSrcBuf++;
			unsigned short* pColor = (unsigned short*)pSrcBuf;
			pSrcBuf+=2;
			int nDSameAlpha = 255 - nSameAlpha;
			if ( nSameAlpha )
			{
				if ( nSameAlpha == 255 )
				{
					*pDstBuf++ = *pColor;
				}
				else
				{
					unsigned short usnSrcClr = *pColor;
						unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
						unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
						unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

						unsigned short usnDstClr = *pDstBuf;
						unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 						unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
						*pDstBuf++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
				}
			}
			else
			{
				pDstBuf++;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			pSrcBuf+=3;
			nCurLinePos++;
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}	
}

void g_DrawBitmap24Alpha_MMX(void* pNodeData, void* pCanvasData)
{
	KDrawNode *ptagNode = (KDrawNode *)pNodeData;
	KCanvas *pclsCanvas = (KCanvas *)pCanvasData;

	// Get the real draw map
	KClipper tagClipper;
	if( pclsCanvas->MakeClip(ptagNode->m_nX, ptagNode->m_nY, ptagNode->m_nWidth, ptagNode->m_nHeight, &tagClipper ) == 0 )
	{
		return;
	}

	// Get the screen back buffer
	int nBackBufferWidth = 0;
	void* pBackBuffer = pclsCanvas->LockCanvas(nBackBufferWidth);
	if(pBackBuffer == NULL)
	{
		return;
	}

	byte  *pSrcBuf = (byte *)ptagNode->m_pBitmap;
	unsigned short *pDstBuf = (unsigned short *)pBackBuffer;
	pDstBuf += tagClipper.y * (nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL) + tagClipper.x;
	int nBackBufLinePixel = nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL;

	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = ptagNode->m_nWidth * tagClipper.top;
	while(nClipSkip > 0)
	{
		pSrcBuf += nClipSkip;
		nClipSkip--;
	}

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned short *pDstCurLineHead = pDstBuf;
		nRemainSrcHeight --;

		int nCurLinePos		= 0;
		int nCurUnitEndPos	= 0;
		
		nCurLinePos			= 0;
		nCurUnitEndPos		= tagClipper.left;
		
		// Deal with left spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			// Skip left clip spr data first
			*pSrcBuf += 3;
			nCurLinePos++;
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nCurLinePos++;
			nSameAlpha = *pSrcBuf++;
			unsigned short* pColor = (unsigned short*)pSrcBuf;
			pSrcBuf+=2;
			int nDSameAlpha = 255 - nSameAlpha;
			if ( nSameAlpha )
			{
				if ( nSameAlpha == 255 )
				{
					*pDstBuf++ = *pColor;
					CMMXUnsigned16Saturated src;

				}
				else
				{
					/*
					unsigned short usnSrcClr = *pColor;
					unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
					unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
					unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

					unsigned short usnDstClr = *pDstBuf;
					unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
					unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 					unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
					*pDstBuf++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
					//*/

					/*
					CMMXUnsigned16Saturated red;
					CMMXUnsigned16Saturated green;
					CMMXUnsigned16Saturated blue;//*/
				}
			}
			else
			{
				pDstBuf++;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			*pSrcBuf++;
			nCurLinePos++;
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}	
}


_declspec (align(16)) const short Alpha_mask[]={0x8000,0x8000,0x8000,0x8000,0,0,0,0};
_declspec (align(16)) const short Mask_5_5_0[] = {0x7fe0,0x7fe0,0x7fe0,0x7fe0,0,0,0,0};
_declspec (align(16)) const short Mask_1111[]  = {0x1f,0x1f,0x1f,0x1f,0,0,0,0};

void g_DrawBitmap16Alpha_SSE2(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpBitmap = pNode->m_pBitmap;// bitmap pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch - Clipper.width * 2;

	// 计算位图下一行的偏移
	long BitmapOffset = nWidth * 2 - Clipper.width * 2;

__asm
	{
//---------------------------------------------------------------------------
//  计算 EDI 指向屏幕起点的偏移量 (以字节计)
//  edi = (nPitch*Clipper.y + nX)*2 + lpBuffer
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx				// eax = nPitch*Clipper.y
		mov     ebx, Clipper.x
		add		ebx, ebx		// ebx = 2*Clipper.x
		add     eax, ebx
		mov		edi, lpBuffer	
		add		edi, eax		// edi = lpBuffer + nPitch*Clipper.y + 2*Clipper.y
//---------------------------------------------------------------------------
//  初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
//  esi += (nWidth * Clipper.top + Clipper.left) * 2
//---------------------------------------------------------------------------
		mov		ecx, Clipper.top
		mov		eax, nWidth
		mul     ecx				// ecx = Clipper.top*nWidth
		add     eax, Clipper.left
		add		eax, eax		// eax = 2*(Clipper.top*nWidth + Clipper.left)
		mov		esi, lpBitmap
		add     esi, eax		// esi = lpBitmap + 2*(Clipper.top*nWidth + Clipper.left)
//---------------------------------------------------------------------------
// 以一次1个点的方式来绘制位图，遇到Alpha位为1的点就忽略不绘制
//---------------------------------------------------------------------------
		mov		edx, Clipper.height
		movdqa  xmm5, Alpha_mask	// xmm5 = [0x8000,0x8000,0x8000,0x8000,0,0,0,0]
		movdqa	xmm4, Mask_5_5_0	// xmm4 = [0,0,0,0,0x7fe0,0x7fe0,0x7fe0,0x7fe0]
		movdqa	xmm7, Mask_1111		// xmm7 = [0,0,0,0,0x1f,0x1f,0x1f,0x1f];

loc_DrawBitmap16Alpha_Init:
		mov		ecx, Clipper.width

		//cmp		ecx, 4
		//jl		One_pix_loop		// if ecx < 4, jmp One_pix_loop

DrawBitmap16Alpha_Paint_Line_4_pix:
		sub		ecx, 4
		jl		One_pix_loop

		// do four pixel per iteration
		movq	xmm0, qword ptr [esi]	// xmm0 = [0,0,0,0,a3b3g3r3, a2b2g2r2,a1b1g1r1,a0b0g0r0]
		movdqa	xmm1, xmm0
		pand	xmm0, xmm5				// xmm0 = [0,0,0,0,a3,a2,a1,a0]
		pmovmskb eax, xmm0
			
		cmp eax, 42					// if 4 alpha != 0?  42 = [10101010]
		je	 Paint_4_Point_Finish		// jmp Paint_4_Point_Finish: if all 4 alpha != 0

		// at least one alpha == 0 at this point

		movdqa	xmm2, xmm1			// xmm2 = [0,0,0,0,a3b3g3r3, a2b2g2r2,a1b1g1r1,a0b0g0r0]
		
		/*移位，把1555格式变为565格式*/
		//取5-14位(1555的左边那个55)
		pand	xmm1, xmm4			// xmm1 = 0,0,0,0,0 b3 g3 0, 0 b2 g2 0,0 b1g1,0 b0 g0 0]

		//把5-14位变为6-15位(把1555的最左边那个55变为565的最左边那个56)
		psllw	xmm1, 1				// xmm1 = 0,0,0,0,b3 g3 0, b2 g2 0,b1g1,b0 g0 0]

		//取原图形点的0-4位
		pand	xmm2, xmm7			// xmm2 = [0,0,0,0,r3, r2,r1,r0]

		por		xmm1,xmm2			// xmm1 = [0,0,0,0,a3b3g3r3, a2b2g2r2,a1b1g1r1,a0b0g0r0], 565 form
	
		/*移位完成，1555变为了565*/
		/*绘制4个点*/
		
		test	eax, eax			// if all alpha == 0?
		jne		store_one_by_one    // at leat one alpha != 0, we need to store some pixel

		movq	qword ptr [edi], xmm1 // all alph = 0, store all 4 pixels
		jmp		Paint_4_Point_Finish

store_one_by_one:
		push	edx
		movd	edx, xmm1		// edx = [a1b1g1r1,a0b0g0r0]
		psrlq	xmm1, 32		// xmm1 = [a3b3g3r3, a2b2g2r2]
		shr	 eax, 2
		jc second_pix
		mov		[edi], dx		// store two bytes

second_pix:
		shr		edx, 16			// edx = [a0b0g0r0]
		shr	 eax, 2	
		jc third_pix
		mov		[edi+2], dx		// store two bytes

third_pix:
		movd	edx, xmm1		// edx = [a3b3g3r3, a2b2g2r2]
		shr	 eax, 2	
		jc fourth_pix
		mov		[edi+4], dx		// store two bytes

fourth_pix:
		shr		edx, 16			// edx = [a3b3g3r3]
		shr	 eax, 2	
		jc store_one_by_one_done
		mov		[edi+6], dx		// store two bytes

store_one_by_one_done:
		pop		edx

Paint_4_Point_Finish:
        add		esi, 8
		add		edi, 8

		jmp DrawBitmap16Alpha_Paint_Line_4_pix

One_pix_loop:
		add	ecx, 4
		jle loc_DrawBitmap16Alpha_Next_Line

		push	edx
next_pix:
		mov		edx, 0x8000
		mov		ax, [esi]		// ax [ a,r,g,b]
		and		dx, ax
		jnz		next_pix_continue
		
		mov		bx, 0x1f
		and		bx, ax		// bx = 0x1f and [esi]

		and		ax, 0x7fe0
		shl		ax, 1
		add		ax, bx
		mov		[edi], ax
		
next_pix_continue:
		add		esi, 2
		add		edi, 2
		sub		ecx, 1
		jne		next_pix
		pop		edx

loc_DrawBitmap16Alpha_Next_Line:
		add		esi, BitmapOffset
		add		edi, ScreenOffset
		dec		edx
		jnz		loc_DrawBitmap16Alpha_Init
	}
}

void g_DrawBitmap16_MMX(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	// Get the real draw map
	KClipper tagClipper;
	if( pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &tagClipper ) == 0 )
	{
		return;
	}

	// Get the screen back buffer
	int nBackBufferWidth = 0;
	void* pBackBuffer = pCanvas->LockCanvas(nBackBufferWidth);
	if(pBackBuffer == NULL)
	{
		return;
	}

	unsigned short *pSrcBuf = (unsigned short *)pNode->m_pBitmap;
	unsigned short *pDstBuf = (unsigned short *)pBackBuffer;
	pDstBuf += tagClipper.y * (nBackBufferWidth / 2) + tagClipper.x;
	int nBackBufLinePixel = nBackBufferWidth / 2;

	// Skip top clip spr data
	int nClipSkip = pNode->m_nWidth * tagClipper.top;
	while(nClipSkip > 0)
	{
		*pSrcBuf++;
	}

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned short *pDstCurLineHead = pDstBuf;
		nRemainSrcHeight --;

		int nCurLinePos		= 0;
		int nCurUnitEndPos	= 0;
		 
		nCurLinePos			= 0;
		nCurUnitEndPos		= tagClipper.left;
		
		// Deal with left spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			// Skip left clip spr data first
			nCurLinePos++;
			*pSrcBuf++;
		}
		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nCurLinePos++;
			unsigned short A = 0;
			unsigned short R = 0;
			unsigned short G = 0;
			unsigned short B = 0;
			unsigned short colour1555 = *pSrcBuf++;
			RGB_FROM_RGB555(colour1555,A,R,G,B)
			if( A )		
			{
				*pDstBuf++ = ( R<< SHIFTNUM_RGB565_RCOLOR) | (G << SHIFTNUM_RGB565_GCOLOR) | B;
			}
			else
			{
				pDstBuf++;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nCurLinePos++;
			*pSrcBuf++;
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}	
}

//---------------------------------------------------------------------------
// 函数:	DrawBitmap16mmx
// 功能:	绘制16位色位图
// 参数:	node, canvas
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawBitmap16mmx(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpBitmap = pNode->m_pBitmap;// bitmap pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch - Clipper.width * 2;

	// 计算位图下一行的偏移
	long BitmapOffset = nWidth * 2 - Clipper.width * 2;

	// 绘制函数的汇编代码
	__asm
	{
//---------------------------------------------------------------------------
//  计算 EDI 指向屏幕起点的偏移量 (以字节计)
//  edi = (nPitch*Clipper.y + nX)*2 + lpBuffer
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov		edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
//  初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
//  esi += (nWidth * Clipper.top + Clipper.left) * 2
//---------------------------------------------------------------------------
		mov		ecx, Clipper.top
		mov		eax, nWidth
		mul     ecx
		add     eax, Clipper.left
		add		eax, eax
		mov		esi, lpBitmap
		add     esi, eax
//---------------------------------------------------------------------------
// 以一次4个点的方式来绘制位图
//---------------------------------------------------------------------------
		mov		edx, Clipper.height
		mov		ebx, Clipper.width
		mov		eax, 8
loc_DrawBitmap16mmx_0001:

		mov		ecx, ebx
		shr		ecx, 2
        jz      loc_DrawBitmap16mmx_0003

loc_DrawBitmap16mmx_0002:
		prefetchnta [esi + 512]
		movq	mm0, [esi]
		add		esi, eax
		movntq	[edi], mm0
		add		edi, eax
		dec		ecx
		jnz		loc_DrawBitmap16mmx_0002
loc_DrawBitmap16mmx_0003:
		mov		ecx, ebx
		and		ecx, 3
		rep		movsw
		add     esi, BitmapOffset
		add		edi, ScreenOffset
		dec		edx
		jnz		loc_DrawBitmap16mmx_0001
		sfence
		emms
	}
//	pCanvas->UnlockCanvas();
}
//---------------------------------------------------------------------------
// 函数:	DrawBitmap16Alpha
// 功能:	绘制16位色位图，1555颜色格式，1位透明位
// 参数:	node, canvas
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawBitmap16Alpha(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpBitmap = pNode->m_pBitmap;// bitmap pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch - Clipper.width * 2;

	// 计算位图下一行的偏移
	long BitmapOffset = nWidth * 2 - Clipper.width * 2;

	// 绘制函数的汇编代码
	__asm
	{
//---------------------------------------------------------------------------
//  计算 EDI 指向屏幕起点的偏移量 (以字节计)
//  edi = (nPitch*Clipper.y + nX)*2 + lpBuffer
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov		edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
//  初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
//  esi += (nWidth * Clipper.top + Clipper.left) * 2
//---------------------------------------------------------------------------
		mov		ecx, Clipper.top
		mov		eax, nWidth
		mul     ecx
		add     eax, Clipper.left
		add		eax, eax
		mov		esi, lpBitmap
		add     esi, eax
//---------------------------------------------------------------------------
// 以一次1个点的方式来绘制位图，遇到Alpha位为1的点就忽略不绘制
//---------------------------------------------------------------------------
		mov		edx, Clipper.height
		mov		ebx, Clipper.width

loc_DrawBitmap16Alpha_Init:
		mov		ecx, ebx

loc_DrawBitmap16Alpha_Paint_Line:

		/*判断最高位(透明位)是否有值，有就是透明*/
			mov		ax, 0x8000
			and		ax, [esi]
		/**/
		/*不为0转移到绘画一点完成*/
			jnz		Paint_Point_Finish
		/**/
		/*移位，把1555格式变为565格式*/
			//取5-14位(1555的左边那个55)
			mov		ax, 0x7fe0
			and		ax, [esi]
			//把5-14位变为6-15位(把1555的最左边那个55变为565的最左边那个56)
			shl		ax, 1
			//取原图形点的0-4位
			shl		ebx, 16
			mov		bx, 0x1f
			and		bx, [esi]
			add		ax, bx
			shr		ebx, 16
		/*移位完成，1555变为了565*/
		/*绘制一个点*/
			mov		[edi], ax
		/**/
Paint_Point_Finish:
        add		esi, 2
		add		edi, 2
		dec		ecx
		jnz		loc_DrawBitmap16Alpha_Paint_Line

//loc_DrawBitmap16Alpha_Next_Line:
		add		esi, BitmapOffset
		add		edi, ScreenOffset
		dec		edx
		jnz		loc_DrawBitmap16Alpha_Init
	}
//	pCanvas->UnlockCanvas();
}
//---------------------------------------------------------------------------
// 函数:	DrawBitmap16win
// 功能:	绘制16位色位图
// 参数:	node, canvas
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawBitmap16win(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpBitmap = pNode->m_pBitmap;// bitmap pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch - Clipper.width * 2;

	// 计算位图下一行的偏移
	long BitmapOffset = nWidth * 2 + Clipper.width * 2;
	long BitmapStarts = nWidth * (nHeight - 1) * 2;


	// 绘制函数的汇编代码
	__asm
	{
//---------------------------------------------------------------------------
//  计算 EDI 指向屏幕起点的偏移量 (以字节计)
//  edi = (nPitch*Clipper.y + nX)*2 + lpBuffer
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov		edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
//  初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
//  esi += (nWidth * Clipper.top + Clipper.left) * 2
//---------------------------------------------------------------------------
		mov		ecx, Clipper.top
		mov		eax, nWidth
		mul     ecx
		add     eax, Clipper.left
		add		eax, eax
		mov		esi, lpBitmap
		mov		ebx, BitmapStarts
		add		esi, ebx
		sub     esi, eax
//---------------------------------------------------------------------------
// 以一次4个点的方式来绘制位图
//---------------------------------------------------------------------------
		mov		edx, Clipper.height
		mov		ebx, Clipper.width
		mov		eax, 8

loc_DrawBitmap16win_0001:

		mov		ecx, ebx
		shr		ecx, 2
        jz      loc_DrawBitmap16win_0003

loc_DrawBitmap16win_0002:
		prefetchnta [esi + 512]
		movq	mm0, [esi]
		movntq	[edi], mm0
		add		esi, eax
		add		edi, eax
		dec		ecx
		jnz		loc_DrawBitmap16win_0002

loc_DrawBitmap16win_0003:
		mov		ecx, ebx
		and		ecx, 3
		rep		movsw
		sub     esi, BitmapOffset
		add		edi, ScreenOffset
		dec		edx
		jnz		loc_DrawBitmap16win_0001
		sfence
		emms
	}
//	pCanvas->UnlockCanvas();
}
//---------------------------------------------------------------------------
void g_DrawBitmap16OnDc32(void* node,void* canvas,int dx,int dy,int dWidth,int dHeight,unsigned long hdclong)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	HDC hdc = (HDC)hdclong;
	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	unsigned short* lpBitmap = (unsigned short*)pNode->m_pBitmap;// bitmap pointer


	unsigned int* pBitmapBuffer = new unsigned int[nWidth*nHeight];
	unsigned int* pTempBuffer = pBitmapBuffer;
	for(int y = 0;y < nHeight;y++)
	{
		for(int x = 0;x<nWidth;x++)
		{
			///转换绘制32位很麻烦。。
			*pTempBuffer = g_RGB565BIT_TO_ARGB32BIT(*lpBitmap);
			pTempBuffer++;
			lpBitmap++;
		}
	}
//	memcpy(pBitmapBuffer,lpBitmap,nWidth*nHeight*2);
//	Flip_Bitmap(pBitmapBuffer,nWidth<<1,nHeight);
	HBITMAP hBitmap = CreateBitmap(nWidth,nHeight,1,32,pBitmapBuffer);
	delete [] pBitmapBuffer;
	HDC hdc1 = CreateCompatibleDC(hdc);
	HBITMAP hTemp = SelectBitmap(hdc1,hBitmap);
	BitBlt(hdc,nX,nY,dWidth,dHeight,hdc1,dx,dy,SRCCOPY);
	SelectBitmap(hdc1,hTemp);
	DeleteObject(hBitmap);
	DeleteDC(hdc1);
}
void g_DrawBitmap16OnDc16(void* node,void* canvas,int dx,int dy,int dWidth,int dHeight,unsigned long hdclong)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	HDC hdc = (HDC)hdclong;
	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpBitmap = pNode->m_pBitmap;// bitmap pointer


	unsigned char* pBitmapBuffer = new unsigned char[nWidth*nHeight*2];
	memcpy(pBitmapBuffer,lpBitmap,nWidth*nHeight*2);
//	Flip_Bitmap(pBitmapBuffer,nWidth<<1,nHeight);
	HBITMAP hBitmap = CreateBitmap(nWidth,nHeight,1,16,pBitmapBuffer);
	delete [] pBitmapBuffer;
	HDC hdc1 = CreateCompatibleDC(hdc);
	HBITMAP hTemp = SelectBitmap(hdc1,hBitmap);
	BitBlt(hdc,nX,nY,dWidth,dHeight,hdc1,dx,dy,SRCCOPY);
	SelectBitmap(hdc1,hTemp);
	DeleteObject(hBitmap);
	DeleteDC(hdc1);

}
void Flip_Bitmap(unsigned char* bitmap_buffer, int bytes_per_line,int height)
{
	unsigned char* tempBuffer=new unsigned char[bytes_per_line*height];
	if(tempBuffer==0)
		return ;
	memcpy(tempBuffer,bitmap_buffer,bytes_per_line*height);
	for(int index=0;index<height;index++)
	{
		memcpy(bitmap_buffer+index*bytes_per_line,tempBuffer+(height-index-1)*bytes_per_line,bytes_per_line);
	}
	delete [] tempBuffer;
	return ;
}
void g_DrawBitmapOnDC(unsigned long hDc,unsigned long hBitmap,int nX,int nY)
{
	HDC hdc = (HDC)hDc;
	HBITMAP hBitmap1 = (HBITMAP)hBitmap;
	BITMAP bm;
	GetObject(hBitmap1,sizeof(bm),&bm);
	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hTemp = SelectBitmap(hdcBuffer,hBitmap1);
	BitBlt(hdc,nX,nY,bm.bmWidth,bm.bmHeight,hdcBuffer,0,0,SRCCOPY);
	SelectBitmap(hdcBuffer,hTemp);
	DeleteDC(hdcBuffer);
	
}
void g_DrawBitmap32OnDc16WidthAlpha(unsigned long hdc,unsigned long hBitmap,int nX,int nY)
{
	HDC hDestDC = (HDC)hdc;
	HBITMAP hDestBitmap = (HBITMAP)GetCurrentObject(hDestDC,OBJ_BITMAP);
	BITMAP destBitmap;
	GetObject(hDestBitmap,sizeof(BITMAP),&destBitmap);
	unsigned short* pDestBitmapBuffer = new unsigned short[destBitmap.bmWidth*destBitmap.bmHeight];
	GetBitmapBits(hDestBitmap,(destBitmap.bmWidth*destBitmap.bmHeight)<<1,(void*)pDestBitmapBuffer);

	HBITMAP hSrcBitmap =(HBITMAP)hBitmap;
	BITMAP srcBitmap;
	GetObject(hSrcBitmap,sizeof(BITMAP),&srcBitmap);
	unsigned int* pSrcBitmapBuffer = new unsigned int[srcBitmap.bmWidth*srcBitmap.bmHeight];
	GetBitmapBits(hSrcBitmap,(srcBitmap.bmHeight*srcBitmap.bmWidth)<<2,(void*)pSrcBitmapBuffer);

	unsigned short* pFinalBuffer = new unsigned short[srcBitmap.bmWidth*srcBitmap.bmHeight];
	memset(pFinalBuffer,255,srcBitmap.bmWidth*srcBitmap.bmHeight*2);


	int left = nX;
	int top = nY;
	int right = left + srcBitmap.bmWidth;
	int bottom = top + srcBitmap.bmHeight;
	if(left> destBitmap.bmWidth -1||
		right < 0||
		top > destBitmap.bmHeight -1||
		bottom<0)
	{
		delete [] pFinalBuffer;
		delete [] pDestBitmapBuffer;
		delete [] pSrcBitmapBuffer;
		return;
	}
	int srcClipperLeft = 0,srcClipperRight = 0,srcClipperTop = 0,srcClipperBottom = 0;
	if(left<0)
	{
		srcClipperLeft = -left;
		left = 0;
	}
	if(right>destBitmap.bmWidth-1)
	{
		srcClipperRight =  right - destBitmap.bmWidth ;
		right = destBitmap.bmWidth -1;
	}
	if(top < 0)
	{
		srcClipperTop = -top;
		top = 0;
	}
	if(bottom > destBitmap.bmHeight -1)
	{
		srcClipperBottom = bottom - destBitmap.bmHeight ;
		bottom = destBitmap.bmHeight - 1;
	}
	///剪彩好了//开始绘制了
	unsigned short* pDestBufferIndex = pDestBitmapBuffer + left+top*destBitmap.bmWidth;
	unsigned int* pSrcBufferIndex = pSrcBitmapBuffer+srcClipperLeft+srcClipperTop*srcBitmap.bmWidth;
	unsigned short* pFinalBufferIndex = pFinalBuffer+srcClipperLeft+srcClipperTop*srcBitmap.bmWidth;


	int drawWidth = srcBitmap.bmWidth - srcClipperLeft - srcClipperRight;
	int drawHeight = srcBitmap.bmHeight -srcClipperTop - srcClipperBottom;

	for(int index_y = 0 ; index_y < drawHeight;index_y++)
	{
		for(int index_x = 0; index_x < drawWidth;index_x++)
		{
			unsigned short destColor = pDestBufferIndex[index_x];
			float dest_r = (float)(((destColor>>11)&31)<<3);
			float dest_g  = (float)(((destColor>>5)&63)<<2);
			float dest_b = (float)((destColor&31)<<3);

			unsigned int srcColor = pSrcBufferIndex[index_x];
			float src_r = (float)((srcColor>>16)&0xff);
			float src_g = (float)((srcColor>>8)&0xff);
			float src_b = (float)((srcColor&0xff));
			float srcAlpha = ((float)(srcColor>>24))/255.0f;
			float destAlpha = 1.0f - srcAlpha;

			unsigned char final_r = (unsigned char)(src_r*srcAlpha+dest_r*destAlpha);
			unsigned char final_g = (unsigned char)(src_g*srcAlpha + dest_g*destAlpha);
			unsigned char final_b = (unsigned char)(src_b*srcAlpha + dest_b*destAlpha);
			final_r>>=3;
			final_g>>=2;
			final_b>>=3;
			pFinalBufferIndex[index_x] = (final_r<<11)|(final_g<<5)|(final_b);

		}
		pSrcBufferIndex+=srcBitmap.bmWidth;
		pDestBufferIndex+=destBitmap.bmWidth;
		pFinalBufferIndex+=srcBitmap.bmWidth;
	}

	//收尾//////////////////////////////////////////////////////////////////////////
	delete [] pDestBitmapBuffer;
	delete [] pSrcBitmapBuffer;
	HDC hBufferDC = CreateCompatibleDC(hDestDC);
	HBITMAP hFinalBitmap = CreateBitmap(srcBitmap.bmWidth,srcBitmap.bmHeight,1,16,pFinalBuffer);
	HBITMAP hTempBitmap = (HBITMAP)SelectBitmap(hBufferDC,hFinalBitmap);
	BitBlt(hDestDC,nX,nY,srcBitmap.bmWidth,srcBitmap.bmHeight,hBufferDC,0,0,SRCCOPY);
	SelectBitmap(hBufferDC,hTempBitmap);
	DeleteDC(hBufferDC);
	DeleteBitmap(hFinalBitmap);
	delete [] pFinalBuffer;	
}
void g_DrawBitmap32OnDc32WidthAlpha(unsigned long hdc,unsigned long hBitmap,int nX,int nY)
{
	HDC hDestDC = (HDC)hdc;
	HBITMAP hDestBitmap = (HBITMAP)GetCurrentObject(hDestDC,OBJ_BITMAP);
	BITMAP destBitmap;
	GetObject(hDestBitmap,sizeof(BITMAP),&destBitmap);
	unsigned int* pDestBitmapBuffer = new unsigned int[destBitmap.bmWidth*destBitmap.bmHeight];
	GetBitmapBits(hDestBitmap,(destBitmap.bmWidth*destBitmap.bmHeight)<<2,(void*)pDestBitmapBuffer);
	HBITMAP hSrcBitmap =(HBITMAP)hBitmap;
	BITMAP srcBitmap;
	GetObject(hSrcBitmap,sizeof(BITMAP),&srcBitmap);
	unsigned int* pSrcBitmapBuffer = new unsigned int[srcBitmap.bmWidth*srcBitmap.bmHeight];
	GetBitmapBits(hSrcBitmap,(srcBitmap.bmHeight*srcBitmap.bmWidth)<<2,(void*)pSrcBitmapBuffer);

	int left = nX;
	int top = nY;
	int right = left + srcBitmap.bmWidth;
	int bottom = top + srcBitmap.bmHeight;
	if(left> destBitmap.bmWidth -1||
		right < 0||
		top > destBitmap.bmHeight -1||
		bottom<0)
	{
		delete [] pSrcBitmapBuffer;
		delete [] pDestBitmapBuffer;
		return;
	}
	int srcClipperLeft = 0,srcClipperRight = 0,srcClipperTop = 0,srcClipperBottom = 0;
	if(left<0)
	{
		srcClipperLeft = -left;
		left = 0;
	}
	if(right>destBitmap.bmWidth-1)
	{
		srcClipperRight =  right - destBitmap.bmWidth ;
		right = destBitmap.bmWidth -1;
	}
	if(top < 0)
	{
		srcClipperTop = -top;
		top = 0;
	}
	if(bottom > destBitmap.bmHeight -1)
	{
		srcClipperBottom = bottom - destBitmap.bmHeight;
		bottom = destBitmap.bmHeight - 1;
	}
	///剪彩好了//开始绘制了
	unsigned int* pDestBufferIndex = pDestBitmapBuffer + left+top*destBitmap.bmWidth;
	unsigned int* pSrcBufferIndex = pSrcBitmapBuffer+srcClipperLeft+srcClipperTop*srcBitmap.bmWidth;

	int drawWidth = srcBitmap.bmWidth - srcClipperLeft - srcClipperRight;
	int drawHeight = srcBitmap.bmHeight -srcClipperTop - srcClipperBottom;

	for(int index_y = 0 ; index_y < drawHeight;index_y++)
	{
		for(int index_x = 0; index_x < drawWidth;index_x++)
		{
			unsigned int destColor = pDestBufferIndex[index_x];
			float dest_r = (float)((destColor>>16)&0xff);
			float dest_g  = (float)((destColor>>8)&0xff);
			float dest_b = (float)((destColor&0xff));

			unsigned int srcColor = pSrcBufferIndex[index_x];
			float src_r = (float)((srcColor>>16)&0xff);
			float src_g = (float)((srcColor>>8)&0xff);
			float src_b = (float)((srcColor&0xff));
			float srcAlpha = ((float)(srcColor>>24))/255.0f;
			float destAlpha = 1.0f - srcAlpha;

			unsigned char final_r = (unsigned char)(src_r*srcAlpha+dest_r*destAlpha);
			unsigned char final_g = (unsigned char)(src_g*srcAlpha + dest_g*destAlpha);
			unsigned char final_b = (unsigned char)(src_b*srcAlpha + dest_b*destAlpha);
			pSrcBufferIndex[index_x] = (0xff<<24)|(final_r<<16)|(final_g<<8)|(final_b);

		}
		pSrcBufferIndex+=srcBitmap.bmWidth;
		pDestBufferIndex+=destBitmap.bmWidth;
	}

	//收尾//////////////////////////////////////////////////////////////////////////
	delete [] pDestBitmapBuffer;
	HDC hBufferDC = CreateCompatibleDC(hDestDC);
	HBITMAP hFinalBitmap = CreateBitmap(srcBitmap.bmWidth,srcBitmap.bmHeight,1,32,pSrcBitmapBuffer);
	HBITMAP hTempBitmap = (HBITMAP)SelectBitmap(hBufferDC,hFinalBitmap);
	BitBlt(hDestDC,nX,nY,srcBitmap.bmWidth,srcBitmap.bmHeight,hBufferDC,0,0,SRCCOPY);
	SelectBitmap(hBufferDC,hTempBitmap);
	DeleteDC(hBufferDC);
	DeleteBitmap(hFinalBitmap);
	delete [] pSrcBitmapBuffer;	

}
void g_DrawBitmap320nDC(unsigned long hdc,unsigned long hBitmap,int nX,int nY)
{
	DEVMODE	devMode;
	::EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &devMode);
	if(devMode.dmBitsPerPel == 16)
	{
		g_DrawBitmap32OnDc16WidthAlpha(hdc,hBitmap,nX,nY);
	}
	else
	{
		g_DrawBitmap32OnDc32WidthAlpha(hdc,hBitmap,nX,nY);
		
	}
}





















