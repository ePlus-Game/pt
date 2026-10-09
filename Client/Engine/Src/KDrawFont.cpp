//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawFont.cpp
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Alpha Sprite Font Drawing Functions
//--------------------------------------------------------------------------
#include "KWin32.h"
#include "KWin32Wnd.h"
#include "KDrawFont.h"
#include <ASSERT.H>

//---------------------------------------------------------------------------
// 函数:	DrawFont
// 功能:	绘制带8级Alpha的字体
// 参数:	KDrawNode*, KCanvas* 
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawFont(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of font
	long nHeight = pNode->m_nHeight;// height of font
	long nColor = pNode->m_nColor;// color of font
	long nAlpha = pNode->m_nAlpha&0x001f;// nAlpha值
	long nTmpAlpha = 0;// nAlpha值2
	void* lpFont = pNode->m_pBitmap;// font pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	long nMask32 = pCanvas->m_nMask32;// RGB mask 32bit
	
	// 计算屏幕下一行的偏移
	long nNextLine = nPitch - Clipper.width * 2;
	
	__asm
	{
//---------------------------------------------------------------------------
// 计算 EDI 指向屏幕起点的偏移量 (以字节计)
// edi = nPitch * Clipper.y + nX * 2 + lpBuffer
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov 	edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
// 初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
//---------------------------------------------------------------------------
		mov		esi, lpFont
		mov		ecx, Clipper.top
		or		ecx, ecx
		jz		loc_DrawFont_0011
		xor		eax, eax

loc_DrawFont_0008:

		mov		edx, nWidth

loc_DrawFont_0009:

		mov     al, [esi]
		inc     esi
		and		al, 0x1f
		sub		edx, eax
		jg		loc_DrawFont_0009
		dec     ecx
		jg  	loc_DrawFont_0008
//---------------------------------------------------------------------------
// jump acorrd Clipper.left, Clipper.right
//---------------------------------------------------------------------------
loc_DrawFont_0011:

		mov		eax, Clipper.left
		or		eax, eax
		jz		loc_DrawFont_0012
		jmp		loc_DrawFont_exit

loc_DrawFont_0012:

		mov		eax, Clipper.right
		or		eax, eax
		jz		loc_DrawFont_0100
		jmp		loc_DrawFont_exit
//---------------------------------------------------------------------------
// Clipper.left  == 0
// Clipper.right == 0
//---------------------------------------------------------------------------
loc_DrawFont_0100:

		mov		edx, Clipper.width

loc_DrawFont_0101:

		xor		eax, eax
		mov     al, [esi]
		inc     esi
		mov		ebx, eax
		shr		ebx, 5
		or		ebx, ebx
		jnz		loc_DrawFont_0102

		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawFont_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFont_0100
		jmp		loc_DrawFont_exit

loc_DrawFont_0102:

        shl     ebx, 2    //Alpha1 
		
		push    eax
		push    edx 
		mov     eax,nAlpha
		mul     ebx
        shr     eax,5 
        mov		nTmpAlpha, eax
		pop     edx  
		pop     eax 
   
		and		eax, 0x1f
		mov     ecx, eax
		push	eax
		push    edx

loc_DrawFont_Loop1:

		push	ecx
		mov     ecx, nColor				// ecx = ...rgb
		mov		ax, cx					// eax = ...rgb
		sal		eax, 16					// eax = rgb...
		mov		ax, cx					// eax = rgbrgb
		and		eax, nMask32			// eax = .g.r.b

		mov		cx, [edi]				// ecx = ...rgb
		mov		bx, cx					// ebx = ...rgb
		sal		ebx, 16					// ebx = rgb...
		mov		bx, cx					// ebx = rgbrgb
		and		ebx, nMask32			// ebx = .g.r.b

		mov		ecx, nTmpAlpha 			// ecx = nAlpha
		mul		ecx						// eax = eax*ecx
		neg		ecx						// ecx = -nAlpha
		add		ecx, 0x20				// ecx = 32-nAlpha
		xchg	eax, ebx				// exchange eax,ebx
		mul		ecx						// eax = eax*ecx
		add		eax, ebx				// eax = eax + ebx
		sar		eax, 5					// eax = eax/32
		and     eax, nMask32			// eax = .g.r.b

		mov     cx, ax					// ecx = ...r.b
		sar     eax, 16					// eax = ....g.
		or      ax, cx					// eax = ...rgb
        stosw                       

		pop		ecx
        loop    loc_DrawFont_Loop1

        pop     edx
        pop     eax

		sub		edx, eax
		jg		loc_DrawFont_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFont_0100
		jmp		loc_DrawFont_exit

loc_DrawFont_exit:
	}
	pCanvas->UnlockCanvas();
}

//---------------------------------------------------------------------------
// 函数:	DrawFont
// 功能:	绘制带8级Alpha的字体
// 参数:	KDrawNode*, KCanvas* 
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawFontWithBorder(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of font
	long nHeight = pNode->m_nHeight;// height of font
	long nColor = pNode->m_nColor;// color of font
	long nBorderColor = pNode->m_nAlpha;// 边缘的颜色值
	void* lpFont = pNode->m_pBitmap;// font pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	long nMask32 = pCanvas->m_nMask32;// RGB mask 32bit

	// 计算屏幕下一行的偏移
	long nNextLine = nPitch - Clipper.width * 2;
	
	__asm
	{
//---------------------------------------------------------------------------
// 计算 EDI 指向屏幕起点的偏移量 (以字节计)
// edi = nPitch * Clipper.y + nX * 2 + lpBuffer
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov 	edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
// 初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
//---------------------------------------------------------------------------
		mov		esi, lpFont
		mov		ecx, Clipper.top
		or		ecx, ecx
		jz		loc_DrawFont_0011
		xor		eax, eax

loc_DrawFont_0008:

		mov		edx, nWidth

loc_DrawFont_0009:

		mov     al, [esi]
		inc     esi
		and		al, 0x1f
		sub		edx, eax
		jg		loc_DrawFont_0009
		dec     ecx
		jg  	loc_DrawFont_0008
//---------------------------------------------------------------------------
// jump acorrd Clipper.left, Clipper.right
//---------------------------------------------------------------------------
loc_DrawFont_0011:

		mov		eax, Clipper.left
		or		eax, eax
		jz		loc_DrawFont_0012
		jmp		loc_DrawFont_exit

loc_DrawFont_0012:

		mov		eax, Clipper.right
		or		eax, eax
		jz		loc_DrawFont_0100
		jmp		loc_DrawFont_exit
//---------------------------------------------------------------------------
// Clipper.left  == 0
// Clipper.right == 0
//---------------------------------------------------------------------------
loc_DrawFont_0100:

		mov		edx, Clipper.width

loc_DrawFont_0101:

		xor		eax, eax
		mov     al, [esi]
		inc     esi
		mov		ebx, eax
		shr		ebx, 5
		or		ebx, ebx
		jnz		loc_DrawFont_0102

		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawFont_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFont_0100
		jmp		loc_DrawFont_exit

loc_DrawFont_0102:
		and		eax, 0x1f
		mov		ecx, eax
		cmp		ebx, 7
		mov		ebx, ecx
		jl		DrawFrontWithBorder_DrawBorder

		//绘制字符点
		mov		eax, nColor
		rep		stosw

		sub		edx, ebx
		jg		loc_DrawFont_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFont_0100
		jmp		loc_DrawFont_exit
 
DrawFrontWithBorder_DrawBorder:		//绘制字符的边缘
		mov		eax, nBorderColor
		rep		stosw

		sub		edx, ebx
		jg		loc_DrawFont_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFont_0100
		jmp		loc_DrawFont_exit

loc_DrawFont_exit:
	}
	pCanvas->UnlockCanvas();
}

//---------------------------------------------------------------------------
// 函数:	DrawFont
// 功能:	绘制带8级Alpha的字体
// 参数:	KDrawNode*, KCanvas* 
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawFontWithBorderToBuff(void* node, void* canvas,void* lpBuffer)
{
	return;
	KDrawNode* pNode = (KDrawNode *)node;
	
	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of font
	long nHeight = pNode->m_nHeight;// height of font
	long nColor = pNode->m_nColor;// color of font
	long nBorderColor = pNode->m_nAlpha;// 边缘的颜色值
	void* lpFont = pNode->m_pBitmap;// font pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	KCanvas* pCanvas = (KCanvas*)canvas;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;
	int nCanvasWidth =  pCanvas->m_ClipRect.right - pCanvas->m_ClipRect.left;
	int nPitch = nCanvasWidth * 2;
	if (lpBuffer == NULL)
		return;

	long nMask32 = pCanvas->m_nMask32;// RGB mask 32bit

	// 计算屏幕下一行的偏移
	long nNextLine = nPitch - Clipper.width * 2;
	
	__asm
	{
//---------------------------------------------------------------------------
// 计算 EDI 指向屏幕起点的偏移量 (以字节计)
// edi = nPitch * Clipper.y + nX * 2 + lpBuffer
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov 	edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
// 初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
//---------------------------------------------------------------------------
		mov		esi, lpFont
		mov		ecx, Clipper.top
		or		ecx, ecx
		jz		loc_DrawFont_0011
		xor		eax, eax

loc_DrawFont_0008:

		mov		edx, nWidth

loc_DrawFont_0009:

		mov     al, [esi]
		inc     esi
		and		al, 0x1f
		sub		edx, eax
		jg		loc_DrawFont_0009
		dec     ecx
		jg  	loc_DrawFont_0008
//---------------------------------------------------------------------------
// jump acorrd Clipper.left, Clipper.right
//---------------------------------------------------------------------------
loc_DrawFont_0011:

		mov		eax, Clipper.left
		or		eax, eax
		jz		loc_DrawFont_0012
		jmp		loc_DrawFont_exit

loc_DrawFont_0012:

		mov		eax, Clipper.right
		or		eax, eax
		jz		loc_DrawFont_0100
		jmp		loc_DrawFont_exit
//---------------------------------------------------------------------------
// Clipper.left  == 0
// Clipper.right == 0
//---------------------------------------------------------------------------
loc_DrawFont_0100:

		mov		edx, Clipper.width

loc_DrawFont_0101:

		xor		eax, eax
		mov     al, [esi]
		inc     esi
		mov		ebx, eax
		shr		ebx, 5
		or		ebx, ebx
		jnz		loc_DrawFont_0102

		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawFont_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFont_0100
		jmp		loc_DrawFont_exit

loc_DrawFont_0102:
		and		eax, 0x1f
		mov		ecx, eax
		cmp		ebx, 7
		mov		ebx, ecx
		jl		DrawFrontWithBorder_DrawBorder

		//绘制字符点
		mov		eax, nColor
		rep		stosw

		sub		edx, ebx
		jg		loc_DrawFont_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFont_0100
		jmp		loc_DrawFont_exit
 
DrawFrontWithBorder_DrawBorder:		//绘制字符的边缘
		mov		eax, nBorderColor
		rep		stosw

		sub		edx, ebx
		jg		loc_DrawFont_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFont_0100
		jmp		loc_DrawFont_exit

loc_DrawFont_exit:
	}
}


//---------------------------------------------------------------------------
// 函数:	DrawFontSolid
// 功能:	绘制实心字体
// 参数:	KDrawNode*, KCanvas* 
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawFontSolid(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of font
	long nHeight = pNode->m_nHeight;// height of font
	long nColor = pNode->m_nColor;// color of font
	long nAlpha = pNode->m_nAlpha&0x001f;// nAlpha值
	void* lpFont = pNode->m_pBitmap;// font pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;

	long nMask32 = pCanvas->m_nMask32;// RGB mask 32bit

	// 计算屏幕下一行的偏移
	long nNextLine = nPitch - Clipper.width * 2;
	
	__asm
	{
//---------------------------------------------------------------------------
// 计算 EDI 指向屏幕起点的偏移量 (以字节计)
// edi = nPitch * Clipper.y + nX * 2 + lpBuffer
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, Clipper.x
		add		ebx, ebx
		add     eax, ebx
		mov 	edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
// 初始化 ESI 指向图块数据起点 (跳过 Clipper.top 行图形数据)
//---------------------------------------------------------------------------
		mov		esi, lpFont
		mov		ecx, Clipper.top
		or		ecx, ecx
		jz		loc_DrawFontSolid_0011
		xor		eax, eax

loc_DrawFontSolid_0008:

		mov		edx, nWidth

loc_DrawFontSolid_0009:

		mov     al, [esi]
		inc     esi
		and		al, 0x1f
		sub		edx, eax
		jg		loc_DrawFontSolid_0009
		dec     ecx
		jg  	loc_DrawFontSolid_0008
//---------------------------------------------------------------------------
// jump acorrd Clipper.left, Clipper.right
//---------------------------------------------------------------------------
loc_DrawFontSolid_0011:

		mov		eax, Clipper.left
		or		eax, eax
		jz		loc_DrawFontSolid_0012
		jmp		loc_DrawFontSolid_exit

loc_DrawFontSolid_0012:

		mov		eax, Clipper.right
		or		eax, eax
		jz		loc_DrawFontSolid_0100
		jmp		loc_DrawFontSolid_exit
//---------------------------------------------------------------------------
// Clipper.left  == 0
// Clipper.right == 0
//---------------------------------------------------------------------------
loc_DrawFontSolid_0100:

		mov		edx, Clipper.width

loc_DrawFontSolid_0101:

		xor		eax, eax
		mov     al, [esi]
		inc     esi
		mov		ebx, eax
		shr		ebx, 5
		or		ebx, ebx
		jnz		loc_DrawFontSolid_0102

		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawFontSolid_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFontSolid_0100
		jmp		loc_DrawFontSolid_exit

loc_DrawFontSolid_0102:

        and		eax, 0x1f
		mov     ecx, eax
		push	eax
		push    edx

loc_DrawFontSolid_Loop1:

		push	ecx
		mov     ecx, nColor				// ecx = ...rgb
		mov		ax, cx					// eax = ...rgb
		sal		eax, 16					// eax = rgb...
		mov		ax, cx					// eax = rgbrgb
		and		eax, nMask32			// eax = .g.r.b

		mov		cx, [edi]				// ecx = ...rgb
		mov		bx, cx					// ebx = ...rgb
		sal		ebx, 16					// ebx = rgb...
		mov		bx, cx					// ebx = rgbrgb
		and		ebx, nMask32			// ebx = .g.r.b

		mov		ecx, nAlpha 			// ecx = nAlpha
		mul		ecx						// eax = eax*ecx
		neg		ecx						// ecx = -nAlpha
		add		ecx, 0x20				// ecx = 32-nAlpha
		xchg	eax, ebx				// exchange eax,ebx
		mul		ecx						// eax = eax*ecx
		add		eax, ebx				// eax = eax + ebx
		sar		eax, 5					// eax = eax/32
		and     eax, nMask32			// eax = .g.r.b

		mov     cx, ax					// ecx = ...r.b
		sar     eax, 16					// eax = ....g.
		or      ax, cx					// eax = ...rgb
        stosw                       

		pop		ecx
        loop    loc_DrawFontSolid_Loop1

        pop     edx
        pop     eax

		sub		edx, eax
		jg		loc_DrawFontSolid_0101
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawFontSolid_0100
		jmp		loc_DrawFontSolid_exit

loc_DrawFontSolid_exit:
	}
	pCanvas->UnlockCanvas();
}

/************************************************************************/
/*					FSOnline2 new function                              */
/************************************************************************/




void g_DrawFontEx_ASM(void* node, void* canvas)
{
	KDrawNodeFontEx* pNode = (KDrawNodeFontEx *)node;
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
	long BitmapOffset = pNode->m_nPitch - Clipper.width;
	
	long fontcolor = pNode->m_nColor;
	BYTE r = (BYTE)(fontcolor >> 16);
	BYTE g = (BYTE)(fontcolor >> 8);
	BYTE b = (BYTE)(fontcolor);
	unsigned short fontcolor16 =((r >> 3) << 11) | ((g >> 2) << 5) | (b>>3);

	long nMask32 = pCanvas->m_nMask32;// RGB mask 32bit
	long nColor  = ((fontcolor16 << 16) | fontcolor16) & nMask32;
//	long nColor  = 0x7f7f7f00 & nMask32;

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
			mov		ax, [esi]
			or		ax, ax
		/**/
		/*不为0转移到绘画一点完成*/
			jz		Paint_Point_Finish

			push	edx
			push	ecx
			push	ebx

			mov		cx, [edi]				// ecx = ...rgb
			mov		bx, cx					// ebx = ...rgb
			sal		ebx, 16					// ebx = rgb...
			mov		bx, cx					// ebx = rgbrgb
			and		ebx, nMask32			// ebx = .g.r.b

			mov		eax, nColor				// eax = .g.r.b
			xor		ecx, ecx
			mov		cl, [esi] 				// ecx = nAlpha
			sar     ecx, 3
			mul		ecx						// eax = eax*ecx
			neg		ecx						// ecx = -nAlpha
			add		ecx, 0x20				// ecx = 32-nAlpha
			xchg	eax, ebx				// exchange eax,ebx
			mul		ecx						// eax = eax*ecx
			add		eax, ebx				// eax = eax + ebx
			sar		eax, 5					// eax = eax/32
			and     eax, nMask32			// eax = .g.r.b

			mov     cx, ax					// ecx = ...r.b
			sar     eax, 16					// eax = ....g.
			or      ax, cx					// eax = ...rgb
			pop     ebx
			pop     ecx
			pop     edx

			mov		[edi], ax

		/**/
Paint_Point_Finish:
        add		esi, 1
		add		edi, 2
		dec		ecx
		jnz		loc_DrawBitmap16Alpha_Paint_Line

//loc_DrawBitmap16Alpha_Next_Line:
		add		esi, BitmapOffset
		add		edi, ScreenOffset
		dec		edx
		jnz		loc_DrawBitmap16Alpha_Init
	}
}

void g_DrawFontEx(void* node, void* canvas)
{
	KCanvas* pCanvas = (KCanvas *)canvas;

	int nPitch;
	unsigned short* lpBuffer = (unsigned short*)pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;


	KDrawNodeFontEx* pNode		= (KDrawNodeFontEx *)node;
	long nX						= pNode->m_nX;						// x coord
	long nY						= pNode->m_nY;						// y coord
	long nWidth					= pNode->m_nWidth;					// width of sprite
	long nHeight				= pNode->m_nHeight;					// height of sprite

	long fontcolor = pNode->m_nColor;
	BYTE r = (BYTE)(fontcolor >> 16);
	BYTE g = (BYTE)(fontcolor >> 8);
	BYTE b = (BYTE)fontcolor;
	unsigned short fontboldcolor16 = 0;
	//unsigned short fontboldcolor16 =((53 >> 3) << 11) | ((30 >> 2) << 5) | (12>>3);
	unsigned short fontcolor16 =((r >> 3) << 11) | ((g >> 2) << 5) | (b>>3);
	
	unsigned char*	lpBitmap	= (unsigned char*)pNode->m_pBitmap;	// sprite pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	//到目标区域（全屏）的裁剪（begin）
	int leftOff = 0;
	int rightOff = 0;
	int topOff = 0;
	if(Clipper.x < 0)
	{
		leftOff = -Clipper.x;
	}
	if(Clipper.x + Clipper.width > nPitch / 2)
	{
		rightOff = Clipper.x + Clipper.width - nPitch / 2;
	}
	if(Clipper.y < 0)
	{
		topOff = -Clipper.y;
	}

	Clipper.x += leftOff;
	Clipper.y += topOff;

	Clipper.left += leftOff;
	Clipper.right += rightOff;
	Clipper.width -= (leftOff + rightOff);
	Clipper.top += topOff;
	Clipper.height -= (topOff);

	assert(Clipper.x >= 0);
	assert(Clipper.y >= 0);
	assert(Clipper.x + Clipper.width <= nPitch / 2);
	//到目标区域（全屏）的裁剪（end）

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch / 2 - Clipper.width;

	// 计算位图下一行的偏移
	long BitmapOffset = pNode->m_nPitch - Clipper.width;
	
	int nLoopY = 0;
	int nLoopX = 0;

	//正常绘制
	lpBuffer += (nPitch/2 * Clipper.y + Clipper.x );
	lpBitmap += (pNode->m_nWidth * Clipper.top + Clipper.left );

 	unsigned short usnRSrcR = (fontcolor16 >> SHIFTNUM_RGB565_RCOLOR) & 0x001F;
	unsigned short usnRSrcG = (fontcolor16 >> SHIFTNUM_RGB565_GCOLOR) & 0x003F;
	unsigned short usnRSrcB = fontcolor16 & 0x001F;

	for ( nLoopY = 0; nLoopY < Clipper.height; ++nLoopY )
	{
		nLoopX = Clipper.width;
		while( nLoopX > 0 )
//		for ( nLoopX = 0; nLoopX < Clipper.width; ++nLoopX )
		{
			int nSameAlpha = *lpBitmap++;
			nLoopX --;
			int nSameCount = 1;
			while(nSameAlpha == *lpBitmap && nLoopX > 0 && nSameCount < 4)
			{
				nSameCount ++;
				lpBitmap ++;
				nLoopX --;
			}
			nSameAlpha = (nSameAlpha * pNode->m_nAlpha) / 255;
			if (  nSameAlpha )
			{
				if ( nSameAlpha == 255 )
				{
					__DrawFontCoreMin(fontcolor16, &lpBuffer, nSameCount);
/*					while(nSameCount > 0)
					{
						*lpBuffer++ = fontcolor16;
						nSameCount --;
					}*/
				}
				else
				{
 					unsigned short usnSrcR = nSameAlpha * usnRSrcR;
					unsigned short usnSrcG = nSameAlpha * usnRSrcG;
					unsigned short usnSrcB = nSameAlpha * usnRSrcB;
					int nSameAlphaD = 255 - nSameAlpha;

					__DrawFontCore(usnSrcR, usnSrcG, usnSrcB, nSameCount, nSameAlphaD, &lpBuffer);
/*					while(nSameCount > 0)
					{
						unsigned short usnDstClr = *lpBuffer;
						unsigned short usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
						unsigned short usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
						*lpBuffer++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
						nSameCount --;
					}*/
				}
			}
			else
			{
				lpBuffer += nSameCount;
			}
		}
		lpBuffer += ScreenOffset;
		lpBitmap += BitmapOffset;
	}
}

void g_DrawFontBorderEx(void* node, void* canvas)
{
	KCanvas* pCanvas = (KCanvas *)canvas;

	int nPitch;
	unsigned short* lpBuffer = (unsigned short*)pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;


	KDrawNodeFontEx* pNode		= (KDrawNodeFontEx *)node;
	long nX						= pNode->m_nX;						// x coord
	long nY						= pNode->m_nY;						// y coord
	long nWidth					= pNode->m_nWidth;					// width of sprite
	long nHeight				= pNode->m_nHeight;					// height of sprite

	long fontcolor = pNode->m_nColor;
	BYTE r = (BYTE)(fontcolor >> 16);
	BYTE g = (BYTE)(fontcolor >> 8);
	BYTE b = (BYTE)fontcolor;
	unsigned short fontboldcolor16 = 0;
	//unsigned short fontboldcolor16 =((53 >> 3) << 11) | ((30 >> 2) << 5) | (12>>3);
	unsigned short fontcolor16 =((r >> 3) << 11) | ((g >> 2) << 5) | (b>>3);
	
	unsigned char*	lpBitmap	= (unsigned char*)pNode->m_pBitmap;	// sprite pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;

	//到目标区域（全屏）的裁剪（begin）
	int leftOff = 0;
	int rightOff = 0;
	int topOff = 0;
	if(Clipper.x < 0)
	{
		leftOff = -Clipper.x;
	}
	if(Clipper.x + Clipper.width > nPitch / 2)
	{
		rightOff = Clipper.x + Clipper.width - nPitch / 2;
	}
	if(Clipper.y < 0)
	{
		topOff = -Clipper.y;
	}

	Clipper.x += leftOff;
	Clipper.y += topOff;

	Clipper.left += leftOff;
	Clipper.right += rightOff;
	Clipper.width -= (leftOff + rightOff);
	Clipper.top += topOff;
	Clipper.height -= (topOff);

	assert(Clipper.x >= 0);
	assert(Clipper.y >= 0);
	assert(Clipper.x + Clipper.width <= nPitch / 2);
	//到目标区域（全屏）的裁剪（end）

	// 计算屏幕下一行的偏移
	long ScreenOffset = nPitch / 2 - Clipper.width;

	// 计算位图下一行的偏移
	long BitmapOffset = pNode->m_nPitch - Clipper.width;
	
	int nLoopY = 0;
	int nLoopX = 0;

	// 边缘绘制 ...Begin
	lpBuffer += (nPitch/2 * Clipper.y + Clipper.x );
	lpBitmap += (pNode->m_nWidth * Clipper.top + Clipper.left );
	
 	unsigned short SrcR = (fontboldcolor16 >> 10) & 0x001F;
	unsigned short SrcG = (fontboldcolor16 >> 5) & 0x001F;
	unsigned short SrcB = fontboldcolor16 & 0x001F;

	for ( nLoopY = 0; nLoopY < Clipper.height; ++nLoopY )
	{
		nLoopX = Clipper.width;
		while( nLoopX > 0 )
		{
			int nSameAlpha = *lpBitmap++;
			nLoopX --;
			int nSameCount = 1;

			while(nSameAlpha == *lpBitmap && nLoopX > 0 && nSameCount < 4)
			{
				nSameCount ++;
				lpBitmap ++;
				nLoopX --;
			}
			
			nSameAlpha = (nSameAlpha * pNode->m_nAlpha) / 255;
			if (  nSameAlpha )
			{
				// 暂时如此判断是否越界 
				if ( Clipper.x + Clipper.width + 1 <= nPitch/2 )
				{
					if ( nSameAlpha == 255 )
					{
						lpBuffer += (ScreenOffset+Clipper.width+1);
						__DrawFontCoreMin(fontboldcolor16, &lpBuffer, nSameCount);
						lpBuffer -= (ScreenOffset+Clipper.width+1);
					}
					else
					{
 						unsigned short usnSrcR = nSameAlpha * SrcR;
						unsigned short usnSrcG = nSameAlpha * SrcG;
						unsigned short usnSrcB = nSameAlpha * SrcB;
						int nSameAlphaD = 255 - nSameAlpha;
						lpBuffer += (ScreenOffset+Clipper.width+1);
						__DrawFontCore(usnSrcR, usnSrcG, usnSrcB, nSameCount, nSameAlphaD, &lpBuffer);
						lpBuffer -= (ScreenOffset+Clipper.width+1);
					}
				}
			}
			else
			{
				lpBuffer += nSameCount;
			}
		}
		lpBuffer += ScreenOffset;
		lpBitmap += BitmapOffset;
	}//*/
	// 边缘绘制 ...End --刘思亮

	lpBuffer = (unsigned short*)pCanvas->LockCanvas(nPitch);
	lpBitmap = (unsigned char*)pNode->m_pBitmap;	// sprite pointer

	//正常绘制
	lpBuffer += (nPitch/2 * Clipper.y + Clipper.x );
	lpBitmap += (pNode->m_nWidth * Clipper.top + Clipper.left );

 	unsigned short usnRSrcR = (fontcolor16 >> SHIFTNUM_RGB565_RCOLOR) & 0x001F;
	unsigned short usnRSrcG = (fontcolor16 >> SHIFTNUM_RGB565_GCOLOR) & 0x003F;
	unsigned short usnRSrcB = fontcolor16 & 0x001F;

	for ( nLoopY = 0; nLoopY < Clipper.height; ++nLoopY )
	{
		nLoopX = Clipper.width;
		while( nLoopX > 0 )
		{
			int nSameAlpha = *lpBitmap++;
			nLoopX --;
			int nSameCount = 1;
			while(nSameAlpha == *lpBitmap && nLoopX > 0 && nSameCount < 4)
			{
				nSameCount ++;
				lpBitmap ++;
				nLoopX --;
			}
			nSameAlpha = (nSameAlpha * pNode->m_nAlpha) / 255;
			if (  nSameAlpha )
			{
				if ( nSameAlpha == 255 )
				{
					__DrawFontCoreMin(fontcolor16, &lpBuffer, nSameCount);
				}
				else
				{
 					unsigned short usnSrcR = nSameAlpha * usnRSrcR;
					unsigned short usnSrcG = nSameAlpha * usnRSrcG;
					unsigned short usnSrcB = nSameAlpha * usnRSrcB;
					int nSameAlphaD = 255 - nSameAlpha;

					__DrawFontCore(usnSrcR, usnSrcG, usnSrcB, nSameCount, nSameAlphaD, &lpBuffer);
				}
			}
			else
			{
				lpBuffer += nSameCount;
			}
		}
		lpBuffer += ScreenOffset;
		lpBitmap += BitmapOffset;
	}
}
/*
void g_DrawFontBorderEx(void* node, void* canvas)
{
	KCanvas* pCanvas = (KCanvas *)canvas;

	int nPitch;
	unsigned short* lpBuffer = (unsigned short*)pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;


	KDrawNodeFontEx* pNode		= (KDrawNodeFontEx *)node;
	long nX						= pNode->m_nX;						// x coord
	long nY						= pNode->m_nY;						// y coord
	long nWidth					= pNode->m_nWidth;					// width of sprite
	long nHeight				= pNode->m_nHeight;					// height of sprite

	long fontcolor = pNode->m_nColor;
	BYTE r = (BYTE)(fontcolor >> 16);
	BYTE g = (BYTE)(fontcolor >> 8);
	BYTE b = (BYTE)fontcolor;
	unsigned short fontcolor16 =((r >> 3) << 11) | ((g >> 2) << 5) | (b>>3);
	unsigned short fontboldcolor16 =((53 >> 3) << 11) | ((30 >> 2) << 5) | (12>>3);


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
			int nColourKey = *lpBitmap++;
			nSameAlpha = (nSameAlpha * pNode->m_nAlpha) / 255;
			if (  nSameAlpha )
			{
				unsigned short usnSrcClr = fontcolor16;
				if ( nColourKey == 0x84 )
				{
					usnSrcClr = fontboldcolor16;
				}
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
		lpBitmap += BitmapOffset*2;
	}
}
//*/
void g_DrawFontWithBorderEx(void* node, void* canvas)
{
	KCanvas* pCanvas = (KCanvas *)canvas;

	int nPitch;
	unsigned short* lpBuffer = (unsigned short*)pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;


	KDrawNodeFontEx* pNode = (KDrawNodeFontEx *)node;
	int nX = pNode->m_nX;
	int nY = pNode->m_nY;

	long fontcolor = pNode->m_nColor;
	BYTE r = (BYTE)(fontcolor >> 16);
	BYTE g = (BYTE)(fontcolor >> 8);
	BYTE b = (BYTE)fontcolor;
	unsigned short fontcolor16 =((r >> 3) << 11) | ((g >> 2) << 5) | (b>>3);
	unsigned short fontboldcolor16 =((53 >> 3) << 11) | ((30 >> 2) << 5) | (12>>3);


	// 左上
	pNode->m_nX = nX;
	pNode->m_nY = nY;
	g_DrawFontWithBorderEx_Core( pNode, canvas, nPitch, lpBuffer, fontboldcolor16 );

	// 右上
	pNode->m_nX = nX + 2;
	pNode->m_nY = nY;
	g_DrawFontWithBorderEx_Core( pNode, canvas, nPitch, lpBuffer, fontboldcolor16 );

	// 右下
	pNode->m_nX = nX + 2;
	pNode->m_nY = nY + 2;
	g_DrawFontWithBorderEx_Core( pNode, canvas, nPitch, lpBuffer, fontboldcolor16 );

	// 左下
	pNode->m_nX = nX;
	pNode->m_nY = nY + 2;
	g_DrawFontWithBorderEx_Core( pNode, canvas, nPitch, lpBuffer, fontboldcolor16 );



	// 上
	pNode->m_nX = nX + 1;
	pNode->m_nY = nY;
	g_DrawFontWithBorderEx_Core( pNode, canvas, nPitch, lpBuffer, fontboldcolor16 );

	// 下
	pNode->m_nX = nX + 1 ;
	pNode->m_nY = nY + 2;
	g_DrawFontWithBorderEx_Core( pNode, canvas, nPitch, lpBuffer, fontboldcolor16 );

	// 右
	pNode->m_nX = nX + 2;
	pNode->m_nY = nY + 1;
	g_DrawFontWithBorderEx_Core( pNode, canvas, nPitch, lpBuffer, fontboldcolor16 );

	// 左
	pNode->m_nX = nX;
	pNode->m_nY = nY + 1;
	g_DrawFontWithBorderEx_Core( pNode, canvas, nPitch, lpBuffer, fontboldcolor16 );

	//正常绘制
	pNode->m_nX = nX + 1;
	pNode->m_nY = nY + 1;//*/
	g_DrawFontWithBorderEx_Core( pNode, canvas, nPitch, lpBuffer, fontcolor16 );
}




//---------------------------------------------------------------------------
