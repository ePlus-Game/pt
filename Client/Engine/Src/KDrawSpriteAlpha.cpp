//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawSpriteAlpha.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Modify:	add g_DrawSpriteAlphaEx function by cooler
// Desc:	Sprite Alpha Drawing Function
//---------------------------------------------------------------------------
#include "KWin32.h"
#include "KCanvas.h"
#include "KDrawSpriteAlpha.h"
#include "DrawSpriteMP.inc"
#include "KWin32Wnd.h"
#include "KColors.h"
#include <ASSERT.H> 
#include "KPalette.h"


//---------------------------------------------------------------------------
// 函数:	Draw Sprite nAlpha
// 功能:	绘制256色Sprite位图(不带预渲染)
// 参数:	KDrawNode*, KCanvas* 
// 返回:	void
//---------------------------------------------------------------------------
void g_DrawSpriteAlpha(void* node, void* canvas)
{

	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

//	if (Clipper.height == pNode->m_nHeight && Clipper.width == pNode->m_nWidth)
//	{
//		SprCopy32(node, canvas);
//		return;		
//	}

	// pBuffer指向屏幕绘制行的头一个像点处 
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch;
	void* pPalette	= pNode->m_pPalette;// palette pointer
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	long nMask32 = pCanvas->m_nMask32;	// rgb mask32
	long nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	long nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;
	int	 nAlpha;

	__asm
	{
        mov     eax, pPalette
        movd    mm0, eax        // mm0: pPalette

        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width

        mov     eax, nMask32
        movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha

        // mm4: temp use

        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax


		//使edi指向buffer绘制起点,	(以字节计)	
		mov		edi, pBuffer
		mov		eax, Clipper.x
		add		edi, eax
		add		edi, eax
        

		//使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		esi, pSprite

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_:
				{
					add		esi, eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx
					neg		edx
					jmp		_SkipSpriteAheadContentEnd_
				}
			}
		}
		_SkipSpriteAheadContentEnd_:

		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_	//if (nSprSkipPerLine) goto _DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//因为sprite不会跨行压缩，则运行到此处edx必为0，如sprite会跨行压缩则_DrawFullLineSection_需改			
			_DrawFullLineSection_Line_:
			{
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    lea     edi, [edi + eax * 2]
					sub		edx, eax
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
				
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						sub		edx, eax
						mov		ecx, eax

						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							movd     ebx, mm0   // mm0: pPalette
                            
                            sub ecx, 4
                            jl  _DrawFullLineSection_CopyPixel_continue
							_DrawFullLineSection_CopyPixel4_:
							{
								copy_4pixel_use_eax
                                
                                sub ecx, 4
                                jg     _DrawFullLineSection_CopyPixel4_
							}
							_DrawFullLineSection_CopyPixel_continue:
                            add ecx, 4
                            jz _DrawFullLineSection_CopyPixel_End 

						    _DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax
                                dec     ecx
                                jnz     _DrawFullLineSection_CopyPixel_
							}
                            _DrawFullLineSection_CopyPixel_End:

							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							movd    mm6, edx
							shr		ebx, 3
                            movd    mm3, ebx    // mm3: nAlpha
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_alpha_use_eabdx
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							movd    edx, mm6
							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_

							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
					}
				}
			}
		}

		_DrawPartLineSection_:
		{
			mov		eax, Clipper.left
			or		eax, eax
			jz		_DrawPartLineSection_SkipRight_Line_

			mov		eax, Clipper.right
			or		eax, eax
			jz		_DrawPartLineSection_SkipLeft_Line_
		}

		_DrawPartLineSection_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_LineLocal_CheckAlpha_
			_DrawPartLineSection_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
				neg		edx
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_Line_
					_DrawPartLineSection_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_Line_
					}
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					movd     ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_
					}
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_HalfAlphaPixel_
					}
					movd    edx, mm6
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					movd    ebx,  mm0   // mm0: pPalette
					_DrawPartLineSection_CopyPixel_Part_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_Part_
					}
			
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
					mov		ebx, 255
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_HalfAlphaPixel_Part_
					}
					movd    edx, mm6
					neg		edx
					mov		ebx, nAlpha
					shl		ebx, 3			//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					add		ebx, 1
					dec		Clipper.height
					jg		_DrawPartLineSection_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipLeft_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
			}
			
			_DrawPartLineSection_SkipLeft_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		edx, nSprSkipPerLine
				_DrawPartLineSection_SkipLeft_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_SkipLeft_Line_
					_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_SkipLeft_Line_
					}
				}
			}
			_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
			{
				sub		edx, eax		;先把eax减了，这样後面就可以不需要保留eax了
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_
						
				//_DrawPartLineSection_SkipLeft_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
					_DrawPartLineSection_SkipLeft_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipLeft_CopyPixel_
					}
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}

				_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipLeft_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipLeft_HalfAlphaPixel_
					}
					movd    edx, mm6
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			movd	edx, mm1    // mm1: Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
				neg		edx
			}
			
			_DrawPartLineSection_SkipRight_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_SkipRight_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
					jmp		_DrawPartLineSection_SkipRight_Line_
					_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
						jmp		_DrawPartLineSection_SkipRight_Line_
					}
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipRight_CopyPixel_
					}
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_
					}
					movd	edx, mm6
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_Part_:
				{
					movd    ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_Part_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipRight_CopyPixel_Part_
					}
					neg		edx
					mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_
					jmp		_EXIT_WAY_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
					shr		ebx, 3
                    movd    mm3, ebx    // mm3: nAlpha
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_
					}
					movd	edx, mm6
					neg		edx
					mov		ebx, 128
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_EXIT_WAY_
				}
			}
		}
		_EXIT_WAY_:
        emms
	}
//	pCanvas->UnlockCanvas();
}

void g_DrawSpriteAlpha16(void* pNodeData, void* pCanvasData)
{
	KDrawNode *ptagNode = (KDrawNode *)pNodeData;
	KCanvas *pclsCanvas = (KCanvas *)pCanvasData;

	// Get the real draw map
	KClipper tagClipper;
	if( pclsCanvas->MakeClip(ptagNode->m_nX, ptagNode->m_nY, ptagNode->m_nWidth, ptagNode->m_nHeight, &tagClipper ) == 0 )
	{
		return;
	}

		int leftOff = 0;
	int rightOff = 0;
	int topOff = 0;
	int bottomOff = 0;
	if(tagClipper.x < 0)
	{
		leftOff = -tagClipper.x;
	}
	if(tagClipper.x + tagClipper.width > pclsCanvas->GetWidth())
	{
		rightOff = tagClipper.x + tagClipper.width - pclsCanvas->GetWidth();
	}
	if(tagClipper.y < 0)
	{
		topOff = -tagClipper.y;
	}
	if(tagClipper.y + tagClipper.height > pclsCanvas->GetHeight())
	{
		bottomOff = tagClipper.y + tagClipper.height - pclsCanvas->GetHeight();
	}

	tagClipper.x += leftOff;
	tagClipper.y += topOff;

	tagClipper.left += leftOff;
	tagClipper.right += rightOff;
	tagClipper.width -= (leftOff + rightOff);
	tagClipper.top += topOff;
	tagClipper.height -= (topOff + bottomOff);

	assert(tagClipper.x >= 0);
	assert(tagClipper.y >= 0);
	assert(tagClipper.x + tagClipper.width <= pclsCanvas->GetWidth());
	assert(tagClipper.y + tagClipper.height <= pclsCanvas->GetHeight());

	// Get the screen back buffer
	int nBackBufferWidth = 0;
	void* pBackBuffer = pclsCanvas->LockCanvas(nBackBufferWidth);
	if(pBackBuffer == NULL)
	{
		return;
	}

	unsigned char  *pSrcBuf = (unsigned char *)ptagNode->m_pBitmap;
	unsigned short *pDstBuf = (unsigned short *)pBackBuffer;
	pDstBuf += tagClipper.y * (nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL) + tagClipper.x;
	int nBackBufLinePixel = nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL;

	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = ptagNode->m_nWidth * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount * 2;
		}
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
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			int nSameCountLoop = nCurLinePos - nCurUnitEndPos; 
			if ( nSameCountLoop > 0)
			{
				if(nSameAlpha > 0 )
				{
					pSrcBuf += (nSameCount - nSameCountLoop)*2;
					if ( nSameAlpha == 255 )
					{
						if ( nSameCountLoop > tagClipper.width )
						{
							__Draw16CoreMin((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width);
							pSrcBuf += ((nSameCountLoop - tagClipper.width)*2);
						}
						else
						{
							__Draw16CoreMin((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
						}
/*						while(nSameCountLoop > 0)
						{
							*pDstBuf++ = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							nSameCountLoop--;
						}*/
					}
					else
					{
						if ( nSameCountLoop > tagClipper.width )
						{
							__Draw16Core((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width, nSameAlpha);
							pSrcBuf += ((nSameCountLoop - tagClipper.width)*2);
						}
						else
						{
							__Draw16Core((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop, nSameAlpha);
						}
/*						int nDSameAlpha = 255 - nSameAlpha;
						while(nSameCountLoop > 0)
						{
							unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;

							unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
							unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
							unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

							unsigned short usnDstClr = *pDstBuf;
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
							*pDstBuf++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
							nSameCountLoop--;
						}*/
					}
				}
				else
				{
					pDstBuf += nSameCountLoop;
				}					
			}
			else
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += nSameCount*2;
				}
			}
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			int nSameCountLoop = 0;
			if ( nCurLinePos > nCurUnitEndPos )
			{
				nSameCountLoop = nSameCount - (nCurLinePos - nCurUnitEndPos);
			}
			else
			{
				nSameCountLoop = nSameCount;
			}
			if(nSameAlpha > 0 )
			{
				if ( nSameAlpha == 255 )
				{
					__Draw16CoreMin((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
/*					while(nSameCountLoop > 0)
					{
						*pDstBuf++ = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;
						nSameCountLoop--;
					}*/
				}
				else
				{
					__Draw16Core((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop, nSameAlpha);
/*					int nDSameAlpha = 255 - nSameAlpha;
					while(nSameCountLoop > 0)
					{	
						unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;

						unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
						unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
						unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

						unsigned short usnDstClr = *pDstBuf;
						unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 						unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
						*pDstBuf++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
						nSameCountLoop--;
					}*/
				}
				if ( nCurLinePos - nCurUnitEndPos > 0)
				{
					pSrcBuf += (nCurLinePos - nCurUnitEndPos)*2;
				}
			}
			else
			{
				pDstBuf += nSameCount;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0)
			{
				pSrcBuf += nSameCount*2;
			}
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}	
}

void g_DrawSpriteAlpha(void* node, void* canvas, int nExAlpha)
{
	if (nExAlpha == 0)
		return;

	if (nExAlpha >= 32)
	{
		g_DrawSpriteAlpha(node, canvas);
		return;
	}

	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	long nX = pNode->m_nX;// x coord
	long nY = pNode->m_nY;// y coord
	long nWidth = pNode->m_nWidth;// width of sprite
	long nHeight = pNode->m_nHeight;// height of sprite
	void* lpSprite = pNode->m_pBitmap;// sprite pointer
	void* lpPalette	= pNode->m_pPalette;// palette pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(nX, nY, nWidth, nHeight, &Clipper))
		return;
	//当前代码图形左右同时被裁减时有误
	if (Clipper.left && Clipper.right)
		return;

	int nPitch;
	void* lpBuffer = pCanvas->LockCanvas(nPitch);
	if (lpBuffer == NULL)
		return;
	long nNextLine = nPitch - nWidth * 2;// next line add
	long nAlpha = pNode->m_nAlpha;// alpha value
	long nMask32 = pCanvas->m_nMask32;// rgb mask32
	WORD wAlpha = (WORD)nExAlpha;

	// 绘制函数的汇编代码
	__asm
	{
//---------------------------------------------------------------------------
// 计算 EDI 指向屏幕起点的偏移量 (以字节计)
// edi = lpBuffer + nPitch * Clipper.y + nX * 2;
//---------------------------------------------------------------------------
		mov		eax, nPitch
		mov		ebx, Clipper.y
		mul		ebx
		mov     ebx, nX
		add		ebx, ebx
		add		eax, ebx
		mov		edi, lpBuffer
		add		edi, eax
//---------------------------------------------------------------------------
// 初始化 ESI 指向图块数据起点 
// (跳过Clipper.top行压缩图形数据)
//---------------------------------------------------------------------------
		mov		esi, lpSprite
		mov		ecx, Clipper.top
		or		ecx, ecx
		jz		loc_DrawSpriteAlpha_0011

loc_DrawSpriteAlpha_0008:

		mov		edx, nWidth

loc_DrawSpriteAlpha_0009:

//		movzx	eax, byte ptr[esi]
//		inc		esi
//		movzx	ebx, byte ptr[esi]
//		inc		esi
//		use uv, change to below
		xor		eax, eax
		xor		ebx, ebx
		mov		al,	 byte ptr[esi]
		inc		esi
		mov		bl,  byte ptr[esi]
		inc		esi
//		change	end
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0010
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0009
		dec     ecx
		jnz		loc_DrawSpriteAlpha_0008
		jmp		loc_DrawSpriteAlpha_0011

loc_DrawSpriteAlpha_0010:

		add		esi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0009
		dec     ecx
		jnz		loc_DrawSpriteAlpha_0008
//---------------------------------------------------------------------------
// 根据 Clipper.left, Clipper.right 分 4 种情况
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0011:

		mov		eax, Clipper.left
		or		eax, eax
		jnz		loc_DrawSpriteAlpha_0012
		mov		eax, Clipper.right
		or		eax, eax
		jnz		loc_DrawSpriteAlpha_0013
		jmp		loc_DrawSpriteAlpha_0100

loc_DrawSpriteAlpha_0012:

		mov		eax, Clipper.right
		or		eax, eax
		jnz		loc_DrawSpriteAlpha_0014
		jmp		loc_DrawSpriteAlpha_0200

loc_DrawSpriteAlpha_0013:

		jmp		loc_DrawSpriteAlpha_0300

loc_DrawSpriteAlpha_0014:

		jmp		loc_DrawSpriteAlpha_0400
//---------------------------------------------------------------------------
// 左边界裁剪量 == 0
// 右边界裁剪量 == 0
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0100:

		mov		edx, Clipper.width

loc_DrawSpriteAlpha_0101:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0102
		
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0101
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0100
		jmp		loc_DrawSpriteAlpha_exit

loc_DrawSpriteAlpha_0102:
		push	eax
		push	edx
		mov		ax, wAlpha
		mul		bx
		shr		eax, 5
		mov		ebx, eax
		pop		edx
		pop		eax		
		jg		loc_lgzone
		mov		ebx, 0
loc_lgzone:
		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0110
		push	eax
		push	edx
		mov		ecx, eax
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_0103:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov		dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0103

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0101
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0100
		jmp		loc_DrawSpriteAlpha_exit

loc_DrawSpriteAlpha_0110:

		push	eax
		push	edx
		mov		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0111:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0111

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0101
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0100
		jmp		loc_DrawSpriteAlpha_exit

//---------------------------------------------------------------------------
// 左边界裁剪量 != 0
// 右边界裁剪量 == 0
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0200:

		mov		edx, Clipper.left

loc_DrawSpriteAlpha_0201:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0202
//---------------------------------------------------------------------------
// 处理nAlpha == 0 的像素 (左边界外)
//---------------------------------------------------------------------------
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0201
		jz		loc_DrawSpriteAlpha_0203
		neg		edx
		mov		eax, edx
		mov		edx, Clipper.width
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit
//---------------------------------------------------------------------------
// 处理nAlpha != 0 的像素 (左边界外)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0202:

		add		esi, eax
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0201
		jz		loc_DrawSpriteAlpha_0203
//---------------------------------------------------------------------------
// 把多减的宽度补回来
//---------------------------------------------------------------------------
		neg		edx
		sub		esi, edx
		sub		edi, edx
		sub		edi, edx

		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0210

		push	eax
		push	edx
		mov		ecx, edx
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_Loop20:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg      loc_DrawSpriteAlpha_Loop20

		pop		edx
		pop		eax
		mov		ecx, edx
		mov		edx, Clipper.width
		sub		edx, ecx
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit

loc_DrawSpriteAlpha_0210:

		push	eax
		push	edx
		mov		ecx, edx
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0211:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0211

		pop		edx
		pop		eax
		mov		ecx, edx
		mov		edx, Clipper.width
		sub		edx, ecx
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit

//---------------------------------------------------------------------------
// 已处理完剪裁区 下面的处理相对简单
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0203:

		mov		edx, Clipper.width

loc_DrawSpriteAlpha_0204:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0206
//---------------------------------------------------------------------------
// 处理nAlpha == 0的像素 (左边界内)
//---------------------------------------------------------------------------
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit
//---------------------------------------------------------------------------
// 处理nAlpha != 0的像素 (左边界内)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0206:

		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0220

		push	eax
		push	edx
		mov		ecx, eax
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_Loop21:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg		loc_DrawSpriteAlpha_Loop21

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jg		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit

loc_DrawSpriteAlpha_0220:

		push	eax
		push	edx
		mov		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0221:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0221

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0204
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0200
		jmp		loc_DrawSpriteAlpha_exit

//---------------------------------------------------------------------------
// 左边界裁剪量 == 0
// 右边界裁剪量 != 0
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0300:

		mov		edx, Clipper.width

loc_DrawSpriteAlpha_0301:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0303
//---------------------------------------------------------------------------
// 处理 nAlpha == 0 的像素 (右边界内)
//---------------------------------------------------------------------------
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0301
		neg		edx
		jmp		loc_DrawSpriteAlpha_0305
//---------------------------------------------------------------------------
// 处理 nAlpha != 0 的像素 (右边界内)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0303:

		cmp		edx, eax
		jl		loc_DrawSpriteAlpha_0304

		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0310
		
		push	eax
		push	edx
		mov		ecx, eax
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_Loop30:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg      loc_DrawSpriteAlpha_Loop30

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0301
		neg		edx
		jmp		loc_DrawSpriteAlpha_0305

loc_DrawSpriteAlpha_0310:

		push	eax
		push	edx
		mov		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0311:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0311

		pop		edx
		pop		eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0301
		neg		edx
		jmp		loc_DrawSpriteAlpha_0305

//---------------------------------------------------------------------------
// 连续点的个数 (eax) > 裁减后的宽度 (edx)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0304:

		cmp		ebx, 255
		jl		loc_DrawSpriteAlpha_0320

		push	eax
		push	edx
		mov		ecx, edx
		mov     ebx, lpPalette

loc_DrawSpriteAlpha_Loop31:

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg      loc_DrawSpriteAlpha_Loop31

		pop		edx
		pop		eax
		sub		eax, edx
		mov		edx, eax
		add		esi, eax
		add		edi, eax
		add		edi, eax
		jmp		loc_DrawSpriteAlpha_0305

loc_DrawSpriteAlpha_0320:

		push	eax
		push	edx
		mov		ecx, edx
		shr     ebx, 3
		mov		nAlpha, ebx

loc_DrawSpriteAlpha_0321:

		push	ecx
		mov     ebx, lpPalette

		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb

		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_DrawSpriteAlpha_0321

		pop		edx
		pop		eax
		sub		eax, edx
		mov		edx, eax
		add		esi, eax
		add		edi, eax
		add		edi, eax
		jmp		loc_DrawSpriteAlpha_0305

//---------------------------------------------------------------------------
// 处理超过了右边界的部分, edx = 超过右边界部分的长度
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0305:

		mov		eax, edx
		mov		edx, Clipper.right
		sub		edx, eax
		jle		loc_DrawSpriteAlpha_0308

loc_DrawSpriteAlpha_0306:

		movzx	eax, byte ptr[esi]
		inc		esi
		movzx	ebx, byte ptr[esi]
		inc		esi
		or		ebx, ebx
		jnz		loc_DrawSpriteAlpha_0307
//---------------------------------------------------------------------------
// 处理 nAlpha == 0 的像素 (右边界外)
//---------------------------------------------------------------------------
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0306
		jmp		loc_DrawSpriteAlpha_0308
//---------------------------------------------------------------------------
// 处理 nAlpha != 0 的像素 (右边界外)
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0307:

		add		esi, eax
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jg		loc_DrawSpriteAlpha_0306

loc_DrawSpriteAlpha_0308:

		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0300
		jmp		loc_DrawSpriteAlpha_exit

//---------------------------------------------------------------------------
// 左边界裁剪量 != 0
// 右边界裁剪量 != 0
//---------------------------------------------------------------------------
loc_DrawSpriteAlpha_0400:		// Line Begin

		mov		edx, Clipper.left

loc_Draw_GetLength:						// edx 记录该行压缩数据到裁剪左端的长度，可能是负值

		movzx	eax, byte ptr[esi]		// 取出压缩数据的长度
		inc		esi
		movzx	ebx, byte ptr[esi]		// 取出Alpha值
		inc		esi
		cmp		edx, eax
		jge		loc_Draw_AllLeft		// edx >= eax, 所有数据都在左边界外
		mov		ecx, Clipper.width		// ecx 得到Clipper宽度
		add		ecx, edx				// ecx = Clipper宽度 + 压缩数据左端被剪长度
		cmp		ecx, 0
		jle		loc_Draw_AllRight		// ecx <= 0，所有数据都在右边界外
		sub		ecx, eax				// 比较压缩数据长度和 ecx ，ecx小于0的话，ecx值为该段右端裁剪长度
		jge		loc_Draw_GetLength_0	// ecx >= eax 说明右端无裁剪
		cmp		edx, 0
		jl		loc_Draw_RightClip		// 左端有裁剪，右端也有
		jmp		loc_Draw_AllClip
loc_Draw_GetLength_0:
		cmp		edx, 0
		jl		loc_Draw_NoClip			// 左右都没裁剪
		jmp		loc_Draw_LeftClip
//---------------------------------------------------------------------------
// 全在左端外
//---------------------------------------------------------------------------
loc_Draw_AllLeft:
		or		ebx, ebx
		jnz		loc_Draw_AllLeft_1
//loc_Draw_AllLeft_0:	// alpha == 0
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
loc_Draw_AllLeft_1: // alpha != 0
		add		edi, eax
		add		edi, eax
		add		esi, eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
//---------------------------------------------------------------------------
// 全在右端外
//---------------------------------------------------------------------------
loc_Draw_AllRight:
		or		ebx, ebx
		jnz		loc_Draw_AllRight_1
//loc_Draw_AllRight_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		mov		ebx, edx
		add		ebx, Clipper.width
		add		ebx, Clipper.right
		cmp		ebx, 0
		jl		loc_Draw_GetLength
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400	// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit		
loc_Draw_AllRight_1:
		add		edi, eax
		add		edi, eax
		add		esi, eax
		sub		edx, eax
		mov		ebx, edx
		add		ebx, Clipper.width
		add		ebx, Clipper.right
		cmp		ebx, 0
		jl		loc_Draw_GetLength
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400	// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit
//---------------------------------------------------------------------------
// 处理左右端都不用裁剪的压缩段
//---------------------------------------------------------------------------
loc_Draw_NoClip:
		or		ebx, ebx
		jnz		loc_Draw_NoClip_1
//loc_Draw_NoClip_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
loc_Draw_NoClip_1:
		cmp		ebx, 255
		jl		loc_Draw_NoClip_Alpha
		push	eax
		push	edx
		mov		ecx, eax
		mov		ebx, lpPalette

loc_Draw_NoClip_Copy:
		movzx	eax, byte ptr[esi]
		inc		esi
		mov		dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec		ecx
		jnz		loc_Draw_NoClip_Copy
		
		pop		edx
		pop		eax
		sub		edx, eax
		jmp		loc_Draw_GetLength

loc_Draw_NoClip_Alpha:
		push	eax
		push	edx
		mov		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx
			
loc_Draw_NoClip_Alpha_LOOP:
		
		push	ecx
		mov     ebx, lpPalette
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb
		
		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_Draw_NoClip_Alpha_LOOP
		
		pop		edx
		pop		eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
//---------------------------------------------------------------------------
// 处理左右端同时要裁剪的压缩段
//---------------------------------------------------------------------------
loc_Draw_AllClip:
		or		ebx, ebx				// 设置标志位
		jnz		loc_Draw_AllClip_1		// Alpha值不为零的处理
//loc_Draw_AllClip_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		neg		ecx
		cmp		ecx, Clipper.right
		jl		loc_Draw_GetLength		// Spr该行没完，接着处理
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit
loc_Draw_AllClip_1:
		add		edi, eax
		add		edi, eax
		add		esi, eax
		sub		edx, eax				// edx - eax < 0

		add		edi, edx				// 补回前面多减的部分
		add		edi, edx				// edi和esi指向实际要
		add		esi, edx				// 绘制的部分
		
		cmp		ebx, 255
		jl		loc_Draw_AllClip_Alpha
		push	eax
		push	edx
		push	ecx
		mov		ecx, Clipper.width		// 前后都被裁剪，所以绘制长度为Clipper.width
		mov		ebx, lpPalette
		
loc_Draw_AllClip_Copy:
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jnz		loc_Draw_AllClip_Copy
		
		pop		ecx
		pop		edx
		pop		eax
		jmp		loc_Draw_AllClip_End

loc_Draw_AllClip_Alpha:
		push	eax
		push	edx
		push	ecx
		mov		ecx, Clipper.width
		shr     ebx, 3
		mov		nAlpha, ebx
			
loc_Draw_AllClip_Alpha_LOOP:
		
		push	ecx
		mov     ebx, lpPalette
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb
		
		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_Draw_AllClip_Alpha_LOOP
		
		pop		ecx
		pop		edx
		pop		eax
loc_Draw_AllClip_End:
		neg		ecx
		add		edi, ecx				// 把edi、esi指针指向下一段
		add		edi, ecx
		add		esi, ecx
		cmp		ecx, Clipper.right
		jl		loc_Draw_GetLength		// Spr该行没完，接着处理
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit
//---------------------------------------------------------------------------
// 处理只有左端要裁剪的压缩段
//---------------------------------------------------------------------------
loc_Draw_LeftClip:
		or		ebx, ebx
		jnz		loc_Draw_LeftClip_1

//loc_Draw_LeftClip_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		jmp		loc_Draw_GetLength
loc_Draw_LeftClip_1:
		add		edi, eax
		add		edi, eax
		add		esi, eax
		sub		edx, eax
		add		edi, edx
		add		edi, edx
		add		esi, edx

		cmp		ebx, 255
		jl		loc_Draw_LeftClip_Alpha
		push	eax
		push	edx
		mov		ecx, edx
		neg		ecx
		mov     ebx, lpPalette
		
loc_Draw_LeftClip_Copy:
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec     ecx
		jg      loc_Draw_LeftClip_Copy
		
		pop		edx
		pop		eax
		jmp		loc_Draw_GetLength

loc_Draw_LeftClip_Alpha:
		push	eax
		push	edx
		mov		ecx, edx
		neg		ecx
		shr     ebx, 3
		mov		nAlpha, ebx
		
loc_Draw_LeftClip_Alpha_LOOP:
		
		push	ecx
		mov     ebx, lpPalette
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb
		
		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_Draw_LeftClip_Alpha_LOOP
		
		pop		edx
		pop		eax
		jmp		loc_Draw_GetLength
//---------------------------------------------------------------------------
// 处理只有右端要裁剪的压缩段
//---------------------------------------------------------------------------
loc_Draw_RightClip:
		or		ebx, ebx
		jnz		loc_Draw_RightClip_1

//loc_Draw_RightClip_0:
		add		edi, eax
		add		edi, eax
		sub		edx, eax
		neg		ecx
		cmp		ecx, Clipper.right
		jl		loc_Draw_GetLength
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400	// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit

loc_Draw_RightClip_1:
		sub		edx, eax
		cmp		ebx, 255
		jl		loc_Draw_RightClip_Alpha
		push	eax
		push	edx
		push	ecx
		add		ecx, eax					// 得到实际绘制的长度
		mov		ebx, lpPalette
		
loc_Draw_RightClip_Copy:
		movzx	eax, byte ptr[esi]
		inc		esi
		mov		dx, [ebx + eax * 2]
		mov		[edi], dx
		inc		edi
		inc		edi
		dec		ecx
		jnz		loc_Draw_RightClip_Copy
		
		pop		ecx
		pop		edx
		pop		eax
		jmp		loc_Draw_RightClip_End

loc_Draw_RightClip_Alpha:
		add		edi, eax
		add		edi, eax
		add		esi, eax
		jmp		loc_Draw_RightClip_End
		push	eax
		push	edx
		push	ecx
		add		ecx, eax
		shr     ebx, 3
		mov		nAlpha, ebx
			
loc_Draw_RightClip_Alpha_LOOP:
		
		push	ecx
		mov     ebx, lpPalette
		
		movzx	eax, byte ptr[esi]
		inc		esi
		mov     cx, [ebx + eax * 2]    // ecx = ...rgb
		mov		ax, cx                 // eax = ...rgb
		shl		eax, 16                // eax = rgb...
		mov		ax, cx                 // eax = rgbrgb
		and		eax, nMask32           // eax = .g.r.b
		mov		cx, [edi]              // ecx = ...rgb
		mov		bx, cx                 // ebx = ...rgb
		shl		ebx, 16                // ebx = rgb...
		mov		bx, cx                 // ebx = rgbrgb
		and		ebx, nMask32           // ebx = .g.r.b
		mov		ecx, nAlpha            // ecx = alpha
		mul		ecx                    // eax:edx = eax*ecx
		neg		ecx                    // ecx = -alpha
		add		ecx, 32                // ecx = 32 - alpha
		xchg	eax, ebx               // exchange eax,ebx
		mul		ecx                    // eax = eax * (32 - alpha)
		add		eax, ebx               // eax = eax + ebx
		shr		eax, 5                 // c = (c1 * alpha + c2 * (32 - alpha)) / 32
		and     eax, nMask32           // eax = .g.r.b
		mov     cx, ax                 // ecx = ...r.b
		shr     eax, 16                // eax = ....g.
		or      ax, cx                 // eax = ...rgb
		
		mov		[edi], ax
		inc		edi
		inc		edi
		pop		ecx
		dec		ecx
		jnz		loc_Draw_RightClip_Alpha_LOOP

		pop		ecx
		pop		edx
		pop		eax
		
loc_Draw_RightClip_End:
		neg		ecx
		add		edi, ecx				// 把edi、esi指针指向下一段
		add		edi, ecx
		add		esi, ecx
		cmp		ecx, Clipper.right
		jl		loc_Draw_GetLength		// Spr该行没完，接着处理
		add		edi, nNextLine
		dec		Clipper.height
		jnz		loc_DrawSpriteAlpha_0400// 行结束，下一行开始
		jmp		loc_DrawSpriteAlpha_exit
			
loc_DrawSpriteAlpha_exit:
	}
//	pCanvas->UnlockCanvas();
}

void g_DrawSpriteAlpha16(void* pNodeData, void* pCanvasData, int nCommonAlpha)
{
	KDrawNode *ptagNode = (KDrawNode *)pNodeData;
	KCanvas *pclsCanvas = (KCanvas *)pCanvasData;

	// Get the real draw map
	KClipper tagClipper;
	if( pclsCanvas->MakeClip(ptagNode->m_nX, ptagNode->m_nY, ptagNode->m_nWidth, ptagNode->m_nHeight, &tagClipper ) == 0 )
	{
		return;
	}

	int leftOff = 0;
	int rightOff = 0;
	int topOff = 0;
	int bottomOff = 0;
	if(tagClipper.x < 0)
	{
		leftOff = -tagClipper.x;
	}
	if(tagClipper.x + tagClipper.width > pclsCanvas->GetWidth())
	{
		rightOff = tagClipper.x + tagClipper.width - pclsCanvas->GetWidth();
	}
	if(tagClipper.y < 0)
	{
		topOff = -tagClipper.y;
	}
	if(tagClipper.y + tagClipper.height > pclsCanvas->GetHeight())
	{
		bottomOff = tagClipper.y + tagClipper.height - pclsCanvas->GetHeight();
	}

	tagClipper.x += leftOff;
	tagClipper.y += topOff;

	tagClipper.left += leftOff;
	tagClipper.right += rightOff;
	tagClipper.width -= (leftOff + rightOff);
	tagClipper.top += topOff;
	tagClipper.height -= (topOff + bottomOff);

	assert(tagClipper.x >= 0);
	assert(tagClipper.y >= 0);
	assert(tagClipper.x + tagClipper.width <= pclsCanvas->GetWidth());
	assert(tagClipper.y + tagClipper.height <= pclsCanvas->GetHeight());

	// Get the screen back buffer
	int nBackBufferWidth = 0;
	void* pBackBuffer = pclsCanvas->LockCanvas(nBackBufferWidth);
	if(pBackBuffer == NULL)
	{
		return;
	}

	unsigned char  *pSrcBuf = (unsigned char *)ptagNode->m_pBitmap;
	unsigned short *pDstBuf = (unsigned short *)pBackBuffer;
	pDstBuf += tagClipper.y * (nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL) + tagClipper.x;
	int nBackBufLinePixel = nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL;

	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = ptagNode->m_nWidth * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount * 2;
		}
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
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			int nSameCountLoop = nCurLinePos - nCurUnitEndPos; 
			if ( nSameCountLoop > 0)
			{
				if(nSameAlpha > 0 && nCommonAlpha > 0)
				{
					nSameAlpha = (nSameAlpha * nCommonAlpha) / 255;
					pSrcBuf += (nSameCount - nSameCountLoop)*2;
					if ( nSameAlpha == 255 )
					{
						if ( nSameCountLoop > tagClipper.width )
						{
							__Draw16CoreMin((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width);
							pSrcBuf += ((nSameCountLoop - tagClipper.width)*2);
						}
						else
						{
							__Draw16CoreMin((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
						}
/*						while(nSameCountLoop > 0)
						{
							*pDstBuf++ = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							nSameCountLoop--;
						}*/
					}
					else
					{
						if ( nSameCountLoop > tagClipper.width )
						{
							__Draw16Core((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width, nSameAlpha);
							pSrcBuf += ((nSameCountLoop - tagClipper.width)*2);
						}
						else
						{
							__Draw16Core((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop, nSameAlpha);
						}
/*						int nDSameAlpha = 255 - nSameAlpha;
						while(nSameCountLoop > 0)
						{
							unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;

							unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
							unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
							unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

							unsigned short usnDstClr = *pDstBuf;
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
							*pDstBuf++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
							nSameCountLoop--;
						}*/
					}
				}
				else
				{
					pDstBuf += nSameCountLoop;
				}					
			}
			else
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += nSameCount*2;
				}
			}
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			int nSameCountLoop = 0;
			if ( nCurLinePos > nCurUnitEndPos )
			{
				nSameCountLoop = nSameCount - (nCurLinePos - nCurUnitEndPos);
			}
			else
			{
				nSameCountLoop = nSameCount;
			}
			if(nSameAlpha > 0 && nCommonAlpha > 0)
			{
				nSameAlpha = (nSameAlpha * nCommonAlpha) / 255;
				if ( nSameAlpha == 255 )
				{
					__Draw16CoreMin((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
/*					while(nSameCountLoop > 0)
					{
						*pDstBuf++ = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;
						nSameCountLoop--;
					}*/
				}
				else
				{
					__Draw16Core((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop, nSameAlpha);
/*					int nDSameAlpha = 255 - nSameAlpha;
					while(nSameCountLoop > 0)
					{	
						unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;

						unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
						unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
						unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

						unsigned short usnDstClr = *pDstBuf;
						unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 						unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
						*pDstBuf++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
						nSameCountLoop--;
					}*/
				}
				if ( nCurLinePos - nCurUnitEndPos > 0)
				{
					pSrcBuf += (nCurLinePos - nCurUnitEndPos)*2;
				}	
			}
			else
			{
				pDstBuf += nSameCount;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0)
			{
				pSrcBuf += nSameCount*2;
			}
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}	
}

void g_DrawSpriteAlphaEx(void* pNodeData, void* pCanvasData, int nCommonAlpha)
{
	KDrawNodeEx *ptagNode = (KDrawNodeEx *)pNodeData;
	KCanvas *pclsCanvas = (KCanvas *)pCanvasData;

	// Get the real draw map
	KClipper tagClipper;
	if(pclsCanvas->MakeClip(ptagNode->m_nX, 
		ptagNode->m_nY, 
		ptagNode->m_nWidth, 
		ptagNode->m_nHeight, 
		&tagClipper) == 0)
	{
		return;
	}

	int leftOff = 0;
	int rightOff = 0;
	int topOff = 0;
	int bottomOff = 0;
	if(tagClipper.x < 0)
	{
		leftOff = -tagClipper.x;
	}
	if(tagClipper.x + tagClipper.width > pclsCanvas->GetWidth())
	{
		rightOff = tagClipper.x + tagClipper.width - pclsCanvas->GetWidth();
	}
	if(tagClipper.y < 0)
	{
		topOff = -tagClipper.y;
	}
	if(tagClipper.y + tagClipper.height > pclsCanvas->GetHeight())
	{
		bottomOff = tagClipper.y + tagClipper.height - pclsCanvas->GetHeight();
	}

	tagClipper.x += leftOff;
	tagClipper.y += topOff;

	tagClipper.left += leftOff;
	tagClipper.right += rightOff;
	tagClipper.width -= (leftOff + rightOff);
	tagClipper.top += topOff;
	tagClipper.height -= (topOff + bottomOff);

	assert(tagClipper.x >= 0);
	assert(tagClipper.y >= 0);
	assert(tagClipper.x + tagClipper.width <= pclsCanvas->GetWidth());
	assert(tagClipper.y + tagClipper.height <= pclsCanvas->GetHeight());

	// Get the screen back buffer
	int nBackBufferWidth = 0;
	void* pBackBuffer = pclsCanvas->LockCanvas(nBackBufferWidth);
	if(pBackBuffer == NULL)
	{
		return;
	}

	unsigned short *pSrcPal = (unsigned short *)ptagNode->m_pPalette;
	unsigned char  *pSrcBuf = (unsigned char *)ptagNode->m_pBitmap;
	unsigned short *pDstBuf = (unsigned short *)((char*)(pBackBuffer) + 
		tagClipper.y * nBackBufferWidth + tagClipper.x * BYTECOUNT_DSTSCREENPIXEL);
	int nBackBufLinePixel = nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL;

	int nRemainCount = 0;
	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = ptagNode->m_nWidth * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		// nSameAlpha == 0 means fully transparent
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount;
		}
	}

	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0) 
	{
		unsigned char *pSrcCurLineHead = pSrcBuf;
		nRemainSrcHeight --;

		int nCurLinePos = 0;
		int nCurUnitEndPos = 0;
		pSrcBuf = pSrcCurLineHead;
		nCurLinePos = 0;
		nCurUnitEndPos = tagClipper.left;
		unsigned short *pDstCurLineHead = pDstBuf;
		// Deal with left spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			// Skip left clip spr data first
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			nRemainCount  = nCurLinePos - nCurUnitEndPos;
			if(nSameAlpha > 0 && nCommonAlpha > 0)
			{
				if(nCurLinePos > nCurUnitEndPos)
				{
					// Draw left clip remain spr data
					nSameAlpha = (nSameAlpha * nCommonAlpha) / 255;
					pSrcBuf += nCurUnitEndPos - (nCurLinePos - nSameCount);

					if(nRemainCount > tagClipper.width)
					{
						__DrawAlphaCore(&pSrcBuf, pSrcPal, &pDstBuf, 
							tagClipper.width, nSameAlpha);
						pSrcBuf += nRemainCount - tagClipper.width;
					}
					else
					{
						__DrawAlphaCore(&pSrcBuf, pSrcPal, &pDstBuf, 
						nRemainCount, nSameAlpha);
					}
				}
				else
				{
					pSrcBuf += nSameCount;
				}
			}
			else
			{
				if(nCurLinePos > nCurUnitEndPos)
				{
					pDstBuf += nCurLinePos - nCurUnitEndPos;
				}
			}
		}
		
		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0 && nCommonAlpha > 0)
			{
				if(nCurLinePos <= nCurUnitEndPos)
				{
					// Draw middle spr fully unit
					nSameAlpha = (nSameAlpha * nCommonAlpha) / 255;

					__DrawAlphaCore(&pSrcBuf, pSrcPal, &pDstBuf, 
						nSameCount, nSameAlpha);

				}
				else
				{
					// Draw middle spr half unit
					nSameAlpha = (nSameAlpha * nCommonAlpha) / 255;
					nRemainCount  = nCurUnitEndPos - (nCurLinePos - nSameCount);

					__DrawAlphaCore(&pSrcBuf, pSrcPal, &pDstBuf, 
						nRemainCount, nSameAlpha);

					pSrcBuf += nCurLinePos - nCurUnitEndPos;
				}
			}
			else
			{
				pDstBuf += nSameCount;
			}
		}

		pDstBuf = pDstCurLineHead + nBackBufLinePixel;

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			// nSameAlpha == 0 means fully transparent
			if(nSameAlpha > 0)
			{
				pSrcBuf += nSameCount;
			}
		}
	}

//	pclsCanvas->UnlockCanvas();
}

void g_DrawSpriteAlphaEx16(void* pNodeData, void* pCanvasData, int nCommonAlpha)
{
	KDrawNode *ptagNode = (KDrawNode *)pNodeData;
	KCanvas *pclsCanvas = (KCanvas *)pCanvasData;

	// Get the real draw map
	KClipper tagClipper;
	if( pclsCanvas->MakeClip(ptagNode->m_nX, ptagNode->m_nY, ptagNode->m_nWidth, ptagNode->m_nHeight, &tagClipper ) == 0 )
	{
		return;
	}

		int leftOff = 0;
	int rightOff = 0;
	int topOff = 0;
	int bottomOff = 0;
	if(tagClipper.x < 0)
	{
		leftOff = -tagClipper.x;
	}
	if(tagClipper.x + tagClipper.width > pclsCanvas->GetWidth())
	{
		rightOff = tagClipper.x + tagClipper.width - pclsCanvas->GetWidth();
	}
	if(tagClipper.y < 0)
	{
		topOff = -tagClipper.y;
	}
	if(tagClipper.y + tagClipper.height > pclsCanvas->GetHeight())
	{
		bottomOff = tagClipper.y + tagClipper.height - pclsCanvas->GetHeight();
	}

	tagClipper.x += leftOff;
	tagClipper.y += topOff;

	tagClipper.left += leftOff;
	tagClipper.right += rightOff;
	tagClipper.width -= (leftOff + rightOff);
	tagClipper.top += topOff;
	tagClipper.height -= (topOff + bottomOff);

	assert(tagClipper.x >= 0);
	assert(tagClipper.y >= 0);
	assert(tagClipper.x + tagClipper.width <= pclsCanvas->GetWidth());
	assert(tagClipper.y + tagClipper.height <= pclsCanvas->GetHeight());

	// Get the screen back buffer
	int nBackBufferWidth = 0;
	void* pBackBuffer = pclsCanvas->LockCanvas(nBackBufferWidth);
	if(pBackBuffer == NULL)
	{
		return;
	}

	unsigned char  *pSrcBuf = (unsigned char *)ptagNode->m_pBitmap;
	unsigned short *pDstBuf = (unsigned short *)pBackBuffer;
	pDstBuf += tagClipper.y * (nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL) + tagClipper.x;
	int nBackBufLinePixel = nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL;

	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = ptagNode->m_nWidth * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount * 2;
		}
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
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			int nSameCountLoop = nCurLinePos - nCurUnitEndPos; 
			if ( nSameCountLoop > 0)
			{
				if(nSameAlpha > 0 && nCommonAlpha > 0)
				{
					nSameAlpha = (nSameAlpha * nCommonAlpha) / 255;
					pSrcBuf += (nSameCount - nSameCountLoop)*2;

					if ( nSameAlpha == 255 )
					{
						if ( nSameCountLoop > tagClipper.width )
						{
							__Draw16CoreMin((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width);
							pSrcBuf += ((nSameCountLoop - tagClipper.width)*2);
						}
						else
						{
							__Draw16CoreMin((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
						}
						
/*						while(nSameCountLoop > 0)
						{
							*pDstBuf++ = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							nSameCountLoop--;
						}*/
					}
					else
					{
						if ( nSameCountLoop > tagClipper.width )
						{
							__Draw16Core((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width, nSameAlpha);
							pSrcBuf += ((nSameCountLoop - tagClipper.width)*2);
						}
						else
						{
							__Draw16Core((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop, nSameAlpha);
						}
						
/*						int nDSameAlpha = 255 - nSameAlpha;
						while(nSameCountLoop > 0)
						{
							unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
							unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
							unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

							unsigned short usnDstClr = *pDstBuf;
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
							*pDstBuf++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
							nSameCountLoop--;
						}*/
					}
				}
				else
				{
					pDstBuf += nSameCountLoop;
				}					
			}
			else
			{
				if(nSameAlpha > 0)
				{
					pSrcBuf += nSameCount*2;
				}
			}
		}

		nCurUnitEndPos += tagClipper.width;
		// Deal with middle spr data in current line
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			int nSameCountLoop = 0;
			if ( nCurLinePos > nCurUnitEndPos )
			{
				nSameCountLoop = nSameCount - (nCurLinePos - nCurUnitEndPos);
			}
			else
			{
				nSameCountLoop = nSameCount;
			}
			if(nSameAlpha > 0 && nCommonAlpha > 0)
			{
				nSameAlpha = (nSameAlpha * nCommonAlpha) / 255;
				if ( nSameAlpha == 255 )
				{
					__Draw16CoreMin((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
/*					while(nSameCountLoop > 0)
					{
						*pDstBuf++ = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;
						nSameCountLoop--;
					}*/
				}
				else
				{
					__Draw16Core((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop, nSameAlpha);
/*					int nDSameAlpha = 255 - nSameAlpha;
					while(nSameCountLoop > 0)
					{	
						unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;

						unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
						unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
						unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

						unsigned short usnDstClr = *pDstBuf;
						unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
						unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 						unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
						*pDstBuf++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
						nSameCountLoop--;
					}*/
				}
				if ( nCurLinePos - nCurUnitEndPos > 0)
				{
					pSrcBuf += (nCurLinePos - nCurUnitEndPos)*2;
				}
			}
			else
			{
				pDstBuf += nSameCount;
			}			
		}

		// Skip right clip spr data
		nCurUnitEndPos += tagClipper.right;
		while(nCurLinePos < nCurUnitEndPos)
		{
			nSameCount = *pSrcBuf++;
			nSameAlpha = *pSrcBuf++;
			nCurLinePos += nSameCount;
			if(nSameAlpha > 0)
			{
				pSrcBuf += nSameCount*2;
			}
		}
		pDstBuf = pDstCurLineHead + nBackBufLinePixel;
	}	
}

void g_DrawSpriteAlphaScaleEx(void* pNodeData, void* pCanvasData, int nCommonAlpha)
{
	KDrawNodeEx *ptagNode = (KDrawNodeEx *)pNodeData;
	KCanvas *pclsCanvas = (KCanvas *)pCanvasData;

	// Get the real draw map
	KClipper tagClipper;
	if(pclsCanvas->MakeClip(ptagNode->m_nX, 
		ptagNode->m_nY, 
		ptagNode->m_nWidth, 
		ptagNode->m_nHeight, 
		&tagClipper) == 0)
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

	unsigned short *pSrcPal = (unsigned short *)ptagNode->m_pPalette;
	unsigned char  *pSrcBuf = (unsigned char *)ptagNode->m_pBitmap;
	unsigned short *pDstBuf = (unsigned short *)((char*)(pBackBuffer) + 
		tagClipper.y * nBackBufferWidth + tagClipper.x * BYTECOUNT_DSTSCREENPIXEL);
	int nBackBufLinePixel = nBackBufferWidth / BYTECOUNT_DSTSCREENPIXEL;

	int nScaleX = ptagNode->m_nScaleX;
	int nScaleY = ptagNode->m_nScaleY;
	int nRemainCount = 0;
	int nSameCount = 0;
	int nSameAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = ptagNode->m_nWidth * tagClipper.top;
	while(nClipSkip > 0)
	{
		nSameCount = *pSrcBuf++;
		nSameAlpha = *pSrcBuf++;
		nClipSkip -= nSameCount;
		// nSameAlpha == 0 means fully transparent
		if(nSameAlpha > 0)
		{
			pSrcBuf += nSameCount;
		}
	}

	int nRemainScaleHeight = ptagNode->m_nScaleHeight;
	int nRemainSrcHeight = tagClipper.height;
	while(nRemainSrcHeight > 0 && nRemainScaleHeight > 0) 
	{
		unsigned char *pSrcCurLineHead = pSrcBuf;
		int nCurScaleY = min(nScaleY, nRemainScaleHeight);
		nRemainScaleHeight -= nCurScaleY;
		nRemainSrcHeight --;

		int nCurLinePos = 0;
		int nCurUnitEndPos = 0;
		for(; nCurScaleY > 0; nCurScaleY--)
		{
			pSrcBuf = pSrcCurLineHead;
			nCurLinePos = 0;
			nCurUnitEndPos = tagClipper.left;
			unsigned short *pDstCurLineHead = pDstBuf;
			/*int nCurScaleX = 0;*/
			int nRemainScaleWidth = ptagNode->m_nScaleWidth;
			// Deal with left spr data in current line
			while(nCurLinePos < nCurUnitEndPos)
			{
				// Skip left clip spr data first
				nSameCount = *pSrcBuf++;
				nSameAlpha = *pSrcBuf++;
				nCurLinePos += nSameCount;
				nRemainCount  = nCurLinePos - nCurUnitEndPos;
				if(nSameAlpha > 0 && nCommonAlpha > 0)
				{
					if(nCurLinePos > nCurUnitEndPos)
					{
						// Draw left clip remain spr data
						nSameAlpha = (nSameAlpha * nCommonAlpha) / 255;
						pSrcBuf += nCurUnitEndPos - (nCurLinePos - nSameCount);

						__DrawScaleAlphaCore(&pSrcBuf, pSrcPal, &pDstBuf, 
							nRemainCount, nSameAlpha, &nRemainScaleWidth, nScaleX);

/*						if(nSameAlpha < 255)
						{
							int nSameAlphaD = 255 - nSameAlpha;

							while(nRemainCount > 0)
							{
								unsigned short usnSrcClr = pSrcPal[*pSrcBuf++];
 								unsigned short usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F);
								unsigned short usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F);
								unsigned short usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

								nCurScaleX = min(nScaleX, nRemainScaleWidth);
								nRemainScaleWidth -= nCurScaleX;
								for(; nCurScaleX > 0; nCurScaleX--)
								{
									unsigned short usnDstClr = *pDstBuf;
 									unsigned short usnR = (usnSrcR + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB565_RCOLOR) & 0x001F)) >> 8;
 									unsigned short usnG = (usnSrcG + nSameAlphaD * ((usnDstClr >> SHIFTNUM_RGB565_GCOLOR) & 0x003F)) >> 8;
 									unsigned short usnB = (usnSrcB + nSameAlphaD * (usnDstClr & 0x001F)) >> 8;
									*pDstBuf++ = (usnR << SHIFTNUM_RGB565_RCOLOR) | (usnG << SHIFTNUM_RGB565_GCOLOR) | usnB;
								}

								nRemainCount--;
							}
						}
						else 
						{
							while(nRemainCount > 0)
							{
								unsigned short usnSrcClr = pSrcPal[*pSrcBuf++];
								nCurScaleX = min(nScaleX, nRemainScaleWidth);
								nRemainScaleWidth -= nCurScaleX;
								for(; nCurScaleX > 0; nCurScaleX--)
								{
									*pDstBuf++ = usnSrcClr;
								}

								nRemainCount--;
							}
						}*/
					}
					else
					{
						pSrcBuf += nSameCount;
					}
				}
				else
				{
					if(nCurLinePos > nCurUnitEndPos)
					{
						nRemainCount = (nCurLinePos - nCurUnitEndPos) * nScaleX;
						pDstBuf += nRemainCount;
						nRemainScaleWidth -= nRemainCount;
					}
				}
			}
			
			nCurUnitEndPos += tagClipper.width;
			// Deal with middle spr data in current line
			while(nCurLinePos < nCurUnitEndPos)
			{
				nSameCount = *pSrcBuf++;
				nSameAlpha = *pSrcBuf++;
				nCurLinePos += nSameCount;
				if(nSameAlpha > 0 && nCommonAlpha > 0)
				{
					pSrcBuf += (nCurLinePos - nCurUnitEndPos)*3;
				}
				else
				{
					pSrcBuf += nSameCount*3;
				}				
			}
		}
	}
}

void g_DrawSprite3LevelAlpha(void* node, void* canvas)
{
	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;

	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	void* pPalette	= pNode->m_pPalette;// palette pointer

	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (!pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper))
		return;

	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;

	long nMask32 = pCanvas->m_nMask32;	// rgb mask32

	// pBuffer指向屏幕起点的偏移位置 (以字节计)
	pBuffer = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
	long nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	long nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;

	__asm
	{
        mov     eax, pPalette
        movd    mm0, eax        // mm0: pPalette

        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width

        mov     eax, nMask32
        movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha
        // mm4: 32 - nAlpha

        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax

		//使edi指向canvas绘制起点,使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		edi, pBuffer
		mov		esi, pSprite

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_:
				{
					add		esi, eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx
					neg		edx
					jmp		_SkipSpriteAheadContentEnd_
				}
			}
		}
		_SkipSpriteAheadContentEnd_:

		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_	//if (nSprSkipPerLine) goto _DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//因为sprite不会跨行压缩，则运行到此处edx必为0，如sprite会跨行压缩则_DrawFullLineSection_需改			
			_DrawFullLineSection_Line_:
			{
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
					add		edi, eax
					add		edi, eax
					sub		edx, eax
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
					
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						movd    mm5, eax
						mov		ecx, eax

						cmp		ebx, 200
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							movd    ebx, mm0    // mm0: pPalette
							_DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax
								loop	_DrawFullLineSection_CopyPixel_
							}

							movd    eax, mm5
							sub		edx, eax
							jg		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
        					movd    mm6, edx
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_use_eabdx
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
        					movd	edx, mm6
							movd    eax, mm5
							sub		edx, eax
							jg		_DrawFullLineSection_LineLocal_

							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
					}
				}
			}
		}

		_DrawPartLineSection_:
		{
			_DrawPartLineSection_Line_:
			{
				mov		eax, edx
				movd	edx, mm1    // mm1: Clipper.width
				or		eax, eax
				jnz		_DrawPartLineSection_LineLocal_CheckAlpha_

				_DrawPartLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					_DrawPartLineSection_LineLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_LineLocal_Alpha_
					add		edi, eax
					add		edi, eax
					sub		edx, eax
					jg		_DrawPartLineSection_LineLocal_

					dec		Clipper.height
					jz		_EXIT_WAY_

					add		edi, edx
					add		edi, edx
					neg		edx
				}
				
				_DrawPartLineSection_LineSkip_:
				{
					add		edi, nBuffNextLine
					//跳过nSprSkipPerLine像素的sprite内容
					mov		eax, edx
					mov		edx, nSprSkipPerLine
					or		eax, eax
					jnz		_DrawPartLineSection_LineSkipLocal_CheckAlpha_

					_DrawPartLineSection_LineSkipLocal_:
					{
						read_alpha_2_ebx_run_length_2_eax
						
						_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
						or		ebx, ebx
						jnz		_DrawPartLineSection_LineSkipLocal_Alpha_
						sub		edx, eax
						jg		_DrawPartLineSection_LineSkipLocal_
						neg		edx
						jmp		_DrawPartLineSection_Line_

						_DrawPartLineSection_LineSkipLocal_Alpha_:
						{
							add		esi, eax
							sub		edx, eax
							jg		_DrawPartLineSection_LineSkipLocal_
							add		esi, edx
							neg		edx
							jmp		_DrawPartLineSection_Line_
						}
					}
				}

				_DrawPartLineSection_LineLocal_Alpha_:
				{
					cmp		eax, edx
					jnl		_DrawPartLineSection_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

					movd	mm5, eax
					mov		ecx, eax
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_:
					{
						movd    ebx, mm0    // mm0: pPalette
						_DrawPartLineSection_CopyPixel_:
						{
							copy_pixel_use_eax
							loop	_DrawPartLineSection_CopyPixel_
						}						
						movd    eax, mm5
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_:
					{
    					movd    mm6, edx
						_DrawPartLineSection_HalfAlphaPixel_:
						{
							mix_2_pixel_color_use_eabdx
							loop	_DrawPartLineSection_HalfAlphaPixel_
						}
       					movd	edx, mm6
						movd    eax, mm5
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
				}

				_DrawPartLineSection_LineLocal_Alpha_Part_:
				{
					movd    mm5, eax
					mov		ecx, edx
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
					{
						movd    ebx, mm0    // mm0: pPalette
						_DrawPartLineSection_CopyPixel_Part_:
						{
							copy_pixel_use_eax
							loop	_DrawPartLineSection_CopyPixel_Part_
						}						
						movd    eax, mm5
				
						dec		Clipper.height
						jz		_EXIT_WAY_

						sub		eax, edx
						mov		edx, eax
						mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
						jmp		_DrawPartLineSection_LineSkip_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
					{
    					movd    mm6, edx
						_DrawPartLineSection_HalfAlphaPixel_Part_:
						{
							mix_2_pixel_color_use_eabdx
							loop	_DrawPartLineSection_HalfAlphaPixel_Part_
						}
       					movd	edx, mm6
						movd    eax, mm5
						dec		Clipper.height
						jz		_EXIT_WAY_
						sub		eax, edx
						mov		edx, eax
						mov		ebx, 128	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
						jmp		_DrawPartLineSection_LineSkip_
					}
				}
			}
		}
		_EXIT_WAY_:
        emms
	}
	
//	pCanvas->UnlockCanvas();
}

void g_DrawSpriteAlphaWithTable8(void* node,void* canvas)
{
	KDrawNode *ptagNode = (KDrawNode *)node;
	KCanvas *pclsCanvas = (KCanvas *)canvas;

	// Get the real draw map
	KClipper tagClipper;
	if( pclsCanvas->MakeClip(ptagNode->m_nX, ptagNode->m_nY, ptagNode->m_nWidth, ptagNode->m_nHeight, &tagClipper ) == 0 )
	{
		return;
	}

	int leftOff = 0;
	int rightOff = 0;
	int topOff = 0;
	int bottomOff = 0;
	if(tagClipper.x < 0)
	{
		leftOff = -tagClipper.x;
	}
	if(tagClipper.x + tagClipper.width > pclsCanvas->GetWidth())
	{
		rightOff = tagClipper.x + tagClipper.width - pclsCanvas->GetWidth();
	}
	if(tagClipper.y < 0)
	{
		topOff = -tagClipper.y;
	}
	if(tagClipper.y + tagClipper.height > pclsCanvas->GetHeight())
	{
		bottomOff = tagClipper.y + tagClipper.height - pclsCanvas->GetHeight();
	}

	tagClipper.x += leftOff;
	tagClipper.y += topOff;

	tagClipper.left += leftOff;
	tagClipper.right += rightOff;
	tagClipper.width -= (leftOff + rightOff);
	tagClipper.top += topOff;
	tagClipper.height -= (topOff + bottomOff);

	// Get the screen back buffer
	int nBackBufferPitch = 0;
	void* pBackBuffer = pclsCanvas->LockCanvas(nBackBufferPitch);
	if(pBackBuffer == NULL)
	{
		return;
	}
	nBackBufferPitch>>=1;

	unsigned char  *pSrcBuffer = (unsigned char *)ptagNode->m_pBitmap;
	unsigned short *pDest = (unsigned short *)pBackBuffer;
	pDest += tagClipper.y * (nBackBufferPitch) + tagClipper.x;
	int nBackBufLinePixel = nBackBufferPitch ;

	int pixelCount = 0;
	int pixelAlpha = 0;
	// Skip top clip spr data
	int nClipSkip = ptagNode->m_nWidth * tagClipper.top;

	KPAL16* pPalettle16 = (KPAL16*)ptagNode->m_pPalette;
	//////////////////////////////////////////////////////
	while(nClipSkip>0)
	{
		pixelCount = *pSrcBuffer;
		pSrcBuffer++;
		pixelAlpha = *pSrcBuffer;
		pSrcBuffer++;
		nClipSkip-=pixelCount;
		if(pixelAlpha>0)
			pSrcBuffer+=(pixelCount);
	}
	///接下来就一步一步绘制了
	int copyHeight = tagClipper.height;
	for(int i = 0 ; i < copyHeight;i++)
	{
		unsigned short* pDestStartLine  = (unsigned short*)pDest;
		int currentPos = 0;
		int targetPos = tagClipper.left;
		//先定位到开始绘制的位置/
		while(currentPos<targetPos)
		{
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			int destAlpha = (255 - pixelAlpha);
			currentPos+=pixelCount;
			int loopCount = currentPos  - targetPos;
			if(loopCount>0)
			{
				//到达开始位置时候，上一个排列点已经超出开始位置 了，这个时候需要处理
				if(loopCount > tagClipper.width)
				{
					//超出了被绘制的区域了
					if(pixelAlpha>0)
					{

						pSrcBuffer+=((pixelCount - loopCount));
						if(pixelAlpha == 255)
						{
							for(int i = 0; i < tagClipper.width; i++)
								pDest[i] = pPalettle16[pSrcBuffer[i]];
						}
						else
						{
							unsigned short* pSrcTable = GetAlphaAddr(pixelAlpha);
							unsigned short* pDestTable = GetAlphaAddr(destAlpha);
							
							for(int i = 0; i < tagClipper.width;i++)
								pDest[i] = pSrcTable[pPalettle16[pSrcBuffer[i]]] + pDestTable[pDest[i]];
						}
						pSrcBuffer+=((loopCount));
					}
					pDest+=tagClipper.width;
				}
				else
				{
					if(pixelAlpha>0)
					{
						pSrcBuffer+=((pixelCount - loopCount));
						if(pixelAlpha == 255)
						{
							for(int i = 0; i < loopCount; i++)
								pDest[i] = pPalettle16[pSrcBuffer[i]];
						}
						else
						{
							unsigned short* pSrcTable = GetAlphaAddr(pixelAlpha);
							unsigned short* pDestTable = GetAlphaAddr(destAlpha);
							for(int i = 0; i < loopCount;i++)
								pDest[i] = pSrcTable[pPalettle16[pSrcBuffer[i]]] + pDestTable[pDest[i]];
						
						}
						pSrcBuffer+=(loopCount);
					}
					pDest+=loopCount;

				}
				   

			}
			else
			{
				if(pixelAlpha>0)
					pSrcBuffer += (pixelCount);
			}
		}
		//////////////////////////////
		////下一步绘制//////////////////////////////////////////////////////////////////////////
		targetPos +=tagClipper.width;
		while(currentPos < targetPos)
		{
			//绘制中间的了
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			int destAlpha = (255 - pixelAlpha);
			currentPos += pixelCount;
			int loopCount = 0;
			if(currentPos > targetPos)
				loopCount = pixelCount - (currentPos - targetPos);
			else
				loopCount = pixelCount;
			//绘制loopcount//
			if(pixelAlpha>0)
			{
				if(pixelAlpha == 255)
				{
					for(int i = 0; i < loopCount;i++)
						pDest[i] = pPalettle16[pSrcBuffer[i]];
//					memcpy(pDest,pSrcTemp,loopCount<<1);
				}
				else
				{
					unsigned short* pSrcTable = GetAlphaAddr(pixelAlpha);
					unsigned short* pDestTable = GetAlphaAddr(destAlpha);
					for(int i = 0; i < loopCount; i++)
						pDest[i] = pSrcTable[pPalettle16[pSrcBuffer[i]]] + pDestTable[pDest[i]];
						
				}
				pSrcBuffer+=(pixelCount);
			}		
			pDest += loopCount;
		}
		//末尾处理
		targetPos+=tagClipper.right;
		while(currentPos<targetPos)
		{
			pixelCount=*pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			currentPos+=pixelCount;
			if(pixelAlpha>0)
				pSrcBuffer+=(pixelCount);
			
		}
		pDest = pDestStartLine + nBackBufLinePixel;
	}
}
void g_DrawSpriteAlphaWithTable16(void* node,void* canvas)
{
	
}

void g_DrawSpriteAlphaWidthAlphaTable16(void* node, void* canvas)
{
/*	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

	// pBuffer指向屏幕绘制行的头一个像点处 
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch;
	void* pPalette	= pNode->m_pPalette;// palette pointer
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	long nMask32 = pCanvas->m_nMask32;	// rgb mask32
	long nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	long nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;
//	int	 nAlpha;

	__asm
	{

        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width
		movd    mm5, alphaTableAddPoint


        // mm3: nAlpha

        // mm4: temp use

        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax


		//使edi指向buffer绘制起点,	(以字节计)	
		mov		edi, pBuffer
		mov		eax, Clipper.x
		add		edi, eax
		add		edi, eax
        

		//使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		esi, pSprite

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_:
				{
					//16位的要相加两次
					add		esi, eax
					add     esi,eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx
					add     esi, edx
					neg		edx
					jmp		_SkipSpriteAheadContentEnd_
				}
			}
		}
		_SkipSpriteAheadContentEnd_:

		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_	//if (nSprSkipPerLine) goto _DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//因为sprite不会跨行压缩，则运行到此处edx必为0，如sprite会跨行压缩则_DrawFullLineSection_需改			
			_DrawFullLineSection_Line_:
			{
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    lea     edi, [edi + eax * 2]
					sub		edx, eax
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
				
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						sub		edx, eax   //count计数
						mov		ecx, eax  // loopcount//

						cmp		ebx, 255 //
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//绘制alpha == 255
						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
                            
                            sub ecx, 4
                            jl  _DrawFullLineSection_CopyPixel_continue
							_DrawFullLineSection_CopyPixel4_:
							{
								//用mmx同时运算四个的能力//
								movq mm3,[esi] 
								movq [edi],mm3
							//	copy_4pixel_use_eax
							    add esi,8
								add edi,8                             
                                sub ecx, 4
                                jg     _DrawFullLineSection_CopyPixel4_
							}
							_DrawFullLineSection_CopyPixel_continue:
                            add ecx, 4
                            jz _DrawFullLineSection_CopyPixel_End 

						    _DrawFullLineSection_CopyPixel_:
							{
						//		copy_pixel_use_eax
								mov ax,word ptr[esi]
								mov [edi],ax
								add esi,2
								add edi,2
								dec     ecx
                                jnz     _DrawFullLineSection_CopyPixel_
							}
                            _DrawFullLineSection_CopyPixel_End:

							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							movd    mm6, edx
//							shr		ebx, 3
 //                          movd    mm3, ebx    // mm3: nAlpha
						    movd    edx,mm5 //源地址
					        shl    ebx,2
							mov    eax,[edx+ebx]
							neg    ebx    
							add    ebx,1020 // ebx = 255-ebx
							mov    ebx,[edx+ebx]//目标地址
							
							_DrawFullLineSection_HalfAlphaPixel_:
							{
				//				//用alpha表取代//
								movd mm7,ecx
								xor ecx,ecx
								xor edx,edx
								mov cx,word ptr[esi]//源值
								shl ecx,1
								mov cx,word ptr[eax + ecx]
								mov dx,word ptr[edi]//目标值
								shl edx,1
								mov dx,word ptr[ebx + edx]
								add cx,dx//cx为混合最终值
								mov [edi],cx
								add esi,2
								add edi,2
								movd ecx,mm7
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							movd    edx, mm6
							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_

							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
					}
				}
			}
		}

		_DrawPartLineSection_:
		{
			mov		eax, Clipper.left
			or		eax, eax
			jz		_DrawPartLineSection_SkipRight_Line_

			mov		eax, Clipper.right
			or		eax, eax
			jz		_DrawPartLineSection_SkipLeft_Line_
		}

		_DrawPartLineSection_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_LineLocal_CheckAlpha_
			_DrawPartLineSection_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
				neg		edx
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_Line_
					_DrawPartLineSection_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						add     esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_LineSkipLocal_
						add		esi, edx
						add     esi, edx
						neg		edx
						jmp		_DrawPartLineSection_Line_
					}
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					_DrawPartLineSection_CopyPixel_:
					{
						//copy_pixel_use_eax
						mov ax,word ptr[esi]
						mov [edi],ax
						add esi,2
						add edi,2
						loop	_DrawPartLineSection_CopyPixel_
					}
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
					xor edx,edx
//					shr		ebx, 3
 //                   movd    mm3, ebx    // mm3: nAlpha
                    movd    edx,mm5 //源地址
					shl    ebx,2
					mov    eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址

					_DrawPartLineSection_HalfAlphaPixel_:
					{
						movd mm7,ecx
						xor ecx,ecx
						xor edx,edx
						mov cx,word ptr[esi]//源值
						shl ecx,1
						mov cx,word ptr[eax + ecx]
						mov dx,word ptr[edi]//目标值
						shl edx,1
						mov dx,word ptr[ebx + edx]
						add cx,dx//cx为混合最终值
						mov [edi],cx
						add esi,2
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_HalfAlphaPixel_
					}
					movd    edx, mm6
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					_DrawPartLineSection_CopyPixel_Part_:
					{
					//	copy_pixel_use_eax
						mov ax,word ptr[esi]
						mov [edi],ax
						add esi,2
						add edi,2

						loop	_DrawPartLineSection_CopyPixel_Part_
					}
			
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
//					mov		ebx, 255
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
//					shr		ebx, 3
 //                   movd    mm3, ebx    // mm3: nAlpha
                    movd    edx,mm5 //源地址
					shl    ebx,2
					mov    eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址
					_DrawPartLineSection_HalfAlphaPixel_Part_:
					{
						movd mm7,ecx
						xor ecx,ecx
						xor edx,edx
						mov cx,word ptr[esi]//源值
						shl ecx,1
						mov cx,word ptr[eax + ecx]
						mov dx,word ptr[edi]//目标值
						shl edx,1
						mov dx,word ptr[ebx + edx]
						add cx,dx//cx为混合最终值
						mov [edi],cx
						add esi,2
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_HalfAlphaPixel_Part_
					}
					movd    edx, mm6
					neg		edx
					dec		Clipper.height
					jg		_DrawPartLineSection_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipLeft_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
			}
			
			_DrawPartLineSection_SkipLeft_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		edx, nSprSkipPerLine
				_DrawPartLineSection_SkipLeft_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_SkipLeft_Line_
					_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						add     esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
						add		esi, edx
						add     esi,edx
						neg		edx
						jmp		_DrawPartLineSection_SkipLeft_Line_
					}
				}
			}
			_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
			{
				sub		edx, eax		;先把eax减了，这样後面就可以不需要保留eax了
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_
						
				//_DrawPartLineSection_SkipLeft_LineLocal_DirectCopy_:
				{
					_DrawPartLineSection_SkipLeft_CopyPixel_:
					{
			//			copy_pixel_use_eax
						mov ax,word ptr[esi]
						mov [edi],ax
						add esi,2
						add edi,2
						loop	_DrawPartLineSection_SkipLeft_CopyPixel_
					}
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}

				_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
				{
					movd    mm6, edx
//					shr		ebx, 3
//                  movd    mm3, ebx    // mm3: nAlpha
                    movd    edx,mm5 //源地址
					shl    ebx,2
					mov    eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址
					_DrawPartLineSection_SkipLeft_HalfAlphaPixel_:
					{
						movd mm7,ecx
						xor ecx,ecx
						xor edx,edx
						mov cx,word ptr[esi]//源值
						shl ecx,1
						mov cx,word ptr[eax + ecx]
						mov dx,word ptr[edi]//目标值
						shl edx,1
						mov dx,word ptr[ebx + edx]
						add cx,dx//cx为混合最终值
						mov [edi],cx
						add esi,2
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_SkipLeft_HalfAlphaPixel_
					}
					movd    edx, mm6
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			movd	edx, mm1    // mm1: Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
				neg		edx
			}
			
			_DrawPartLineSection_SkipRight_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_SkipRight_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
					jmp		_DrawPartLineSection_SkipRight_Line_
					_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						add     esi,eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
						jmp		_DrawPartLineSection_SkipRight_Line_
					}
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_:
					{
	//					copy_pixel_use_eax
						mov ax,word ptr[esi]
						mov [edi],ax
						add esi,2
						add edi,2

						loop	_DrawPartLineSection_SkipRight_CopyPixel_
					}
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
//					shr		ebx, 3
 //                   movd    mm3, ebx    // mm3: nAlpha
					movd    edx,mm5 //源地址
					shl    ebx,2
					mov    eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_:
					{
						movd mm7,ecx
						xor ecx,ecx
						xor edx,edx
						mov cx,word ptr[esi]//源值
						shl ecx,1
						mov cx,word ptr[eax + ecx]
						mov dx,word ptr[edi]//目标值
						shl edx,1
						mov dx,word ptr[ebx + edx]
						add cx,dx//cx为混合最终值
						mov [edi],cx
						add esi,2
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_
					}
					movd	edx, mm6
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_Part_:
				{
					movd    ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_Part_:
					{
//						copy_pixel_use_eax
						mov ax,word ptr[esi]
						mov [edi],ax
						add esi,2
						add edi,2
						loop	_DrawPartLineSection_SkipRight_CopyPixel_Part_
					}
					neg		edx
					mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_
					jmp		_EXIT_WAY_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
//					shr		ebx, 3
  //                  movd    mm3, ebx    // mm3: nAlpha
                    movd    edx,mm5 //源地址
					shl    ebx,2
					mov    eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_:
					{
						movd mm7,ecx
						xor ecx,ecx
						xor edx,edx
						mov cx,word ptr[esi]//源值
						shl ecx,1
						mov cx,word ptr[eax + ecx]
						mov dx,word ptr[edi]//目标值
						shl edx,1
						mov dx,word ptr[ebx + edx]
						add cx,dx//cx为混合最终值
						mov [edi],cx
						add esi,2
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_
					}
					movd	edx, mm6
					neg		edx
//					mov		ebx, 128
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_EXIT_WAY_
				}
			}
		}
		_EXIT_WAY_:
        emms
	}*/

}
void g_DrawSpriteAlphaWidthAlphaTable8(void* node, void* canvas)
{
/*	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

//	if (Clipper.height == pNode->m_nHeight && Clipper.width == pNode->m_nWidth)
//	{
//		SprCopy32(node, canvas);
//		return;		
//	}

	// pBuffer指向屏幕绘制行的头一个像点处 
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch;
	void* pPalette	= pNode->m_pPalette;// palette pointer
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	long nMask32 = pCanvas->m_nMask32;	// rgb mask32
	long nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	long nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;
//	int	 nAlpha;

	__asm
	{
        mov     eax, pPalette
        movd    mm0, eax        // mm0: pPalette

        mov     eax, Clipper.width
        movd    mm1, eax        // mm1: Clipper.width
		movd    mm5 ,alphaTableAddPoint

 //       mov     eax, nMask32
 //       movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha

        // mm4: temp use

        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax


		//使edi指向buffer绘制起点,	(以字节计)	
		mov		edi, pBuffer
		mov		eax, Clipper.x
		add		edi, eax
		add		edi, eax
        

		//使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		esi, pSprite

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_:
				{
					add		esi, eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx
					neg		edx
					jmp		_SkipSpriteAheadContentEnd_
				}
			}
		}
		_SkipSpriteAheadContentEnd_:

		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_	//if (nSprSkipPerLine) goto _DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//因为sprite不会跨行压缩，则运行到此处edx必为0，如sprite会跨行压缩则_DrawFullLineSection_需改			
			_DrawFullLineSection_Line_:
			{
				movd	edx, mm1    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    lea     edi, [edi + eax * 2]
					sub		edx, eax
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
				
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						sub		edx, eax
						mov		ecx, eax

						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							movd     ebx, mm0   // mm0: pPalette
                            
                            sub ecx, 4
                            jl  _DrawFullLineSection_CopyPixel_continue
							_DrawFullLineSection_CopyPixel4_:
							{
								copy_4pixel_use_eax
                                
                                sub ecx, 4
                                jg     _DrawFullLineSection_CopyPixel4_
							}
							_DrawFullLineSection_CopyPixel_continue:
                            add ecx, 4
                            jz _DrawFullLineSection_CopyPixel_End 

						    _DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax
                                dec     ecx
                                jnz     _DrawFullLineSection_CopyPixel_
							}
                            _DrawFullLineSection_CopyPixel_End:

							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							movd    mm6, edx
                            movd    edx,mm5 //源地址
					        shl    ebx,2
							mov  eax,[edx+ebx]
							neg    ebx    
							add    ebx,1020 // ebx = 255-ebx
							mov    ebx,[edx+ebx]//目标地址
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								movd mm7,ecx
                                movd  edx,mm0 //调色ban
								mov  cl,byte ptr[esi] //调色板索引
								and  ecx,0xff
								shl  ecx,1  //绝对不会超过16位
								mov  cx, [edx+ecx] //源索引已经确定
								shl  ecx,1         //颜色索引确定
								mov  cx,word ptr[eax + ecx] //目标值确定
								mov  dx, word ptr[edi]  //源颜色索引
								and  edx,0xffff         //
								shl edx,1         //目标颜色索引
								mov dx,word ptr[ebx+edx] //目标值
								add cx,dx //不必担心溢出，查表有很高的精确度
								mov [edi],cx
								mov [edi],cx
								inc esi
								add edi,2
								movd ecx,mm7

								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							movd    edx, mm6
							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_

							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
					}
				}
			}
		}

		_DrawPartLineSection_:
		{
			mov		eax, Clipper.left
			or		eax, eax
			jz		_DrawPartLineSection_SkipRight_Line_

			mov		eax, Clipper.right
			or		eax, eax
			jz		_DrawPartLineSection_SkipLeft_Line_
		}

		_DrawPartLineSection_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_LineLocal_CheckAlpha_
			_DrawPartLineSection_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
				neg		edx
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_Line_
					_DrawPartLineSection_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_Line_
					}
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					movd     ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_
					}
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{

					movd    mm6, edx
                    movd    edx,mm5 //源地址
				    shl    ebx,2
					mov  eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址
					_DrawPartLineSection_HalfAlphaPixel_:
					{
						movd mm7,ecx
                        movd  edx,mm0 //调色ban
						mov  cl,byte ptr[esi] //调色板索引
						and  ecx,0xff
						shl  ecx,1  //绝对不会超过16位
						mov  cx, [edx+ecx] //源索引已经确定
						shl  ecx,1         //颜色索引确定
						mov  cx,word ptr[eax + ecx] //目标值确定
						mov  dx, word ptr[edi]  //源颜色索引
						and  edx,0xffff         //
						shl edx,1         //目标颜色索引
						mov dx,word ptr[ebx+edx] //目标值
						add cx,dx //不必担心溢出，查表有很高的精确度
						mov [edi],cx
						mov [edi],cx
						inc esi
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_HalfAlphaPixel_
					}
					movd    edx, mm6
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					movd    ebx,  mm0   // mm0: pPalette
					_DrawPartLineSection_CopyPixel_Part_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_Part_
					}
			
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
					mov		ebx, 255
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
                    movd    edx,mm5 //源地址
				    shl    ebx,2
					mov  eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址
					_DrawPartLineSection_HalfAlphaPixel_Part_:
					{
						movd mm7,ecx
                        movd  edx,mm0 //调色ban
						mov  cl,byte ptr[esi] //调色板索引
						and  ecx,0xff
						shl  ecx,1  //绝对不会超过16位
						mov  cx, [edx+ecx] //源索引已经确定
						shl  ecx,1         //颜色索引确定
						mov  cx,word ptr[eax + ecx] //目标值确定
						mov  dx, word ptr[edi]  //源颜色索引
						and  edx,0xffff         //
						shl edx,1         //目标颜色索引
						mov dx,word ptr[ebx+edx] //目标值
						add cx,dx //不必担心溢出，查表有很高的精确度
						mov [edi],cx
						mov [edi],cx
						inc esi
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_HalfAlphaPixel_Part_
					}
					movd    edx, mm6
					neg		edx
					dec		Clipper.height
					jg		_DrawPartLineSection_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			movd	edx, mm1    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipLeft_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
			}
			
			_DrawPartLineSection_SkipLeft_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		edx, nSprSkipPerLine
				_DrawPartLineSection_SkipLeft_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_SkipLeft_Line_
					_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_SkipLeft_Line_
					}
				}
			}
			_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
			{
				sub		edx, eax		;先把eax减了，这样後面就可以不需要保留eax了
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_
						
				//_DrawPartLineSection_SkipLeft_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
					_DrawPartLineSection_SkipLeft_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipLeft_CopyPixel_
					}
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}

				_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
				{
					movd    mm6, edx
                    movd    edx,mm5 //源地址
				    shl    ebx,2
					mov  eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址
					_DrawPartLineSection_SkipLeft_HalfAlphaPixel_:
					{
						movd mm7,ecx
                        movd  edx,mm0 //调色ban
						mov  cl,byte ptr[esi] //调色板索引
						and  ecx,0xff
						shl  ecx,1  //绝对不会超过16位
						mov  cx, [edx+ecx] //源索引已经确定
						shl  ecx,1         //颜色索引确定
						mov  cx,word ptr[eax + ecx] //目标值确定
						mov  dx, word ptr[edi]  //源颜色索引
						and  edx,0xffff         //
						shl edx,1         //目标颜色索引
						mov dx,word ptr[ebx+edx] //目标值
						add cx,dx //不必担心溢出，查表有很高的精确度
						mov [edi],cx
						mov [edi],cx
						inc esi
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_SkipLeft_HalfAlphaPixel_
					}
					movd    edx, mm6
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			movd	edx, mm1    // mm1: Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
		/*		add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
				neg		edx
			}
			
			_DrawPartLineSection_SkipRight_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_SkipRight_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
					jmp		_DrawPartLineSection_SkipRight_Line_
					_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
						jmp		_DrawPartLineSection_SkipRight_Line_
					}
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_:
				{
					movd    ebx, mm0    // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipRight_CopyPixel_
					}
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					movd    mm6, edx
                    movd    edx,mm5 //源地址
				    shl    ebx,2
					mov  eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_:
					{
						movd mm7,ecx
                        movd  edx,mm0 //调色ban
						mov  cl,byte ptr[esi] //调色板索引
						and  ecx,0xff
						shl  ecx,1  //绝对不会超过16位
						mov  cx, [edx+ecx] //源索引已经确定
						shl  ecx,1         //颜色索引确定
						mov  cx,word ptr[eax + ecx] //目标值确定
						mov  dx, word ptr[edi]  //源颜色索引
						and  edx,0xffff         //
						shl edx,1         //目标颜色索引
						mov dx,word ptr[ebx+edx] //目标值
						add cx,dx //不必担心溢出，查表有很高的精确度
						mov [edi],cx
						mov [edi],cx
						inc esi
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_
					}
					movd	edx, mm6
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_Part_:
				{
					movd    ebx, mm0 // mm0: pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_Part_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipRight_CopyPixel_Part_
					}
					neg		edx
					mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_
					jmp		_EXIT_WAY_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
				{
					movd    mm6, edx
                    movd    edx,mm5 //源地址
				    shl    ebx,2
					mov  eax,[edx+ebx]
					neg    ebx    
					add    ebx,1020 // ebx = 255-ebx
					mov    ebx,[edx+ebx]//目标地址
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_:
					{
						movd mm7,ecx
                        movd  edx,mm0 //调色ban
						mov  cl,byte ptr[esi] //调色板索引
						and  ecx,0xff
						shl  ecx,1  //绝对不会超过16位
						mov  cx, [edx+ecx] //源索引已经确定
						shl  ecx,1         //颜色索引确定
						mov  cx,word ptr[eax + ecx] //目标值确定
						mov  dx, word ptr[edi]  //源颜色索引
						and  edx,0xffff         //
						shl edx,1         //目标颜色索引
						mov dx,word ptr[ebx+edx] //目标值
						add cx,dx //不必担心溢出，查表有很高的精确度
						mov [edi],cx
						mov [edi],cx
						inc esi
						add edi,2
						movd ecx,mm7
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_
					}
					movd	edx, mm6
					neg		edx
					mov		ebx, 128
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_EXIT_WAY_
				}
			}
		}
		_EXIT_WAY_:
        emms
	}*/
}

void g_DrawSpriteAlphaUseMMX8(void* node,void* canvas)
{
/*	KDrawNode* pNode = (KDrawNode *)node;
	KCanvas* pCanvas = (KCanvas *)canvas;
	
	// 对绘制区域进行裁剪
	KClipper Clipper;
	if (pCanvas->MakeClip(pNode->m_nX, pNode->m_nY, pNode->m_nWidth, pNode->m_nHeight, &Clipper) == 0)
		return;

//	if (Clipper.height == pNode->m_nHeight && Clipper.width == pNode->m_nWidth)
//	{
//		SprCopy32(node, canvas);
//		return;		
//	}

	// pBuffer指向屏幕绘制行的头一个像点处 
	int nPitch;
	void* pBuffer = pCanvas->LockCanvas(nPitch);
	if (pBuffer == NULL)
		return;
	pBuffer = (char*)(pBuffer) + Clipper.y * nPitch;
	void* pPalette	= pNode->m_pPalette;// palette pointer
	void* pSprite = pNode->m_pBitmap;	// sprite pointer
	long nMask32 = pCanvas->m_nMask32;	// rgb mask32
	long nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	long nSprSkip = pNode->m_nWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;
//	int	 nAlpha;
	__int64 red = 0xf800f800f800f800;
/*	int num = Clipper.width*Clipper.height;
	int color = 255;
	__asm
	{
		mov edi,pBuffer
		mov ecx,num
        Draw:
		{

			mov eax,0x000000ff
//			mov eax,color
//			mov eax,color
			mov [edi],eax
			add edi,2
			loop Draw
		}

	}
	return;*/

/*	__asm
	{
  //      mov     eax, pPalette
   //     movd    mm7, eax        // mm0: pPalette

 //       mov     eax, Clipper.width
 //       movd    mm1, eax        // mm1: Clipper.width

 //       mov     eax, nMask32
 //       movd    mm2, eax        // mm2: nMask32

        // mm3: nAlpha

        // mm4: temp use

        // mm7: push ecx, pop ecx
        // mm6: push edx, pop edx
        // mm5: push eax, pop eax

		//mm6，mm7为掩码
		movq mm6,rgb565Mask;
		movq mm7,rgb565MaskG;
		//使edi指向buffer绘制起点,	(以字节计)	
		mov		edi, pBuffer
		mov		eax, Clipper.x
		add		edi, eax
		add		edi, eax
        

		//使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		esi, pSprite

		//_SkipSpriteAheadContent_:
		{
			mov		edx, nSprSkip
			or		edx, edx
			jz		_SkipSpriteAheadContentEnd_

			_SkipSpriteAheadContentLocalStart_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_SkipSpriteAheadContentLocalAlpha_
				sub		edx, eax
				jg		_SkipSpriteAheadContentLocalStart_
				neg		edx
				jmp		_SkipSpriteAheadContentEnd_

				_SkipSpriteAheadContentLocalAlpha_:
				{
					add		esi, eax
					sub		edx, eax
					jg		_SkipSpriteAheadContentLocalStart_
					add		esi, edx
					neg		edx
					jmp		_SkipSpriteAheadContentEnd_
				}
			}
		}
		_SkipSpriteAheadContentEnd_:

		mov		eax, nSprSkipPerLine
		or		eax, eax
		jnz		_DrawPartLineSection_	//if (nSprSkipPerLine) goto _DrawPartLineSection_

		//_DrawFullLineSection_:
		{
			//因为sprite不会跨行压缩，则运行到此处edx必为0，如sprite会跨行压缩则_DrawFullLineSection_需改			
			_DrawFullLineSection_Line_:
			{
				mov	edx, Clipper.width    // mm1: Clipper.width
				_DrawFullLineSection_LineLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax

					or		ebx, ebx
					jnz		_DrawFullLineSection_LineLocal_Alpha_
                    lea     edi, [edi + eax * 2]
					sub		edx, eax
					jg		_DrawFullLineSection_LineLocal_

					add		edi, nBuffNextLine
					dec		Clipper.height
					jnz		_DrawFullLineSection_Line_
					jmp		_EXIT_WAY_
				
					_DrawFullLineSection_LineLocal_Alpha_:
					{
						sub		edx, eax
						mov		ecx, eax

						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							mov      ebx, pPalette // mm0: pPalette
							movd     mm4,edx
					        sub      ecx, 4
					        
                            jl  _DrawFullLineSection_CopyPixel_continue
							_DrawFullLineSection_CopyPixel4_:
							{
								xor eax,eax
								//最后一个点
								mov al,byte ptr[esi+3]
								shl ax,1
								mov dx,word ptr[ebx+eax] //cx中存放的是最后点的颜色
								shl edx,16        
								xor eax,eax
										//第2个点
								mov al,byte ptr[esi+2]
								shl ax,1
								mov dx,word ptr[ebx+eax]
								movd mm1,edx //把前两个颜色存放在mm1
								psllq mm1,32

								xor eax,eax
								xor edx,edx
								mov al,byte ptr[esi+1]
								shl ax,1
								mov dx,word ptr[ebx+eax]
								shl edx,16
								xor eax,eax
								mov al,byte ptr[esi]
								shl ax,1
								mov dx,word ptr[ebx+eax]
								movd mm2,edx
								por  mm1,mm2 //////目标颜色获取成功在mm1中
								add  esi,4
								movq [edi],mm1
								add edi,8
							//	add esi,4
								sub ecx,4
                                jg     _DrawFullLineSection_CopyPixel4_
							}
							_DrawFullLineSection_CopyPixel_continue:
                            add ecx, 4
                            jz _DrawFullLineSection_CopyPixel_End 

						    _DrawFullLineSection_CopyPixel_:
							{
								xor eax,eax
								mov al,byte ptr[esi]
								shl ax,1
								mov ax,word ptr[ebx+eax]
								mov [edi],ax
								add edi,2
								inc esi
								dec ecx
                                jnz     _DrawFullLineSection_CopyPixel_
							}
                            _DrawFullLineSection_CopyPixel_End:
							movd edx,mm4
							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							
							shr      ebx, 3 //变成32alpha值
							sub      ecx, 4
							jl _DrawFullLineSection_LineLocal_HalfAlpha_Continue_

							//构造alpha
							xor eax,eax
						//	mov bx,32
							mov ax,bx
							shl eax,16
							mov ax,bx
							movd mm5,eax
							psllq mm5,32
							movd mm4,eax
							por mm5,mm4 //混合值存放在mm5中

                            _DrawFullLineSection_LineLocal_HalfAlpha_4:
							{
								movd    mm2 , edx //腾出edx来
								movd    mm3 , ecx //腾出ecx来
								xor ecx,ecx
								movq mm0,[edi] //mm2中存放了目标4个象素
								//对源目标进行处理，读取四个点
								mov edx,pPalette //这调色板
								xor eax,eax
								//最后一个点
								mov al,byte ptr[esi+3]
								shl ax,1
								mov cx,word ptr[edx+eax] //cx中存放的是最后点的颜色
								shl ecx,16        
								xor eax,eax
								//第2个点
								mov al,byte ptr[esi+2]
								shl ax,1
								mov cx,word ptr[edx+eax]
								movd mm1,ecx //把前两个颜色存放在mm1
								psllq mm1,32

								xor eax,eax
								xor ecx,ecx
								mov al,byte ptr[esi+1]
								shl ax,1
								mov cx,word ptr[edx+eax]
								shl ecx,16
								xor eax,eax
								mov al,byte ptr[esi]
								shl ax,1
								mov cx,word ptr[edx+eax]
								movd mm4,ecx
								por  mm1,mm4 //////目标颜色获取成功在mm1中
								movd edx,mm2
								movd ecx,mm3
								add esi,4
								///以下是四个一起混合的汇编
								//mm0 --- 目标
								//mm3 -- 最终的g
								//mm4 -- 最终的r
								//mm5 -- alpha
								//mm6 -- mask1
								//mm7 -- mask2
								//mm1 --- 最终的b
								//mm2 --- 做临时混合用
								///g
								movq mm2,mm0
								psrlw mm2,5  //把g留下来
								pand  mm2,mm7
								movq mm3,mm1
								psrlw mm3,5
								pand mm3,mm7

								psubsw mm3,mm2
								PMULLW mm3,mm5
								psllw mm2,5
								paddsw mm3,mm2//g混合完成 结果保留在mm3中
								psrlw mm3,5

								///r
								movq mm2,mm0
								psrlw mm2,11
								pand mm2,mm6
								movq mm4,mm1
								psrlw mm4,11
								pand mm4,mm6

								psubsw mm4,mm2
								pmullw mm4,mm5
								psllw mm2,5
								paddsw mm4,mm2
								psrlw mm4,5

								//b
								pand mm0,mm6
								pand mm1,mm6

								psubsw mm1,mm0
								pmullw mm1,mm5
								psllw  mm0,5
								paddsw mm1,mm0
								psrlw  mm1,5

								psllw mm4,11
								psllw mm3,5

								por mm1,mm3
								por mm1,mm4

							//	movq mm0,red
								movq [edi] ,mm1
							//	add esi,4
								add edi,8																							
								sub ecx,4

								jg _DrawFullLineSection_LineLocal_HalfAlpha_4
							}
                            _DrawFullLineSection_LineLocal_HalfAlpha_Continue_:
							add ecx,4
							jz _DrawFullLineSection_LineLocal_HalfAlpha_End
							movd mm4,edx
							movd mm3,ebx
							movd mm0,pPalette

							movd mm1,nMask32
                            _DrawFullLineSection_LineLocal_HalfAlpha_pixel:
							{
								//这里混合一个像素//
								movd	mm2, ecx    								
								xor	    eax, eax    								
								movd    ebx, mm0    			 //pPalette 		
								mov  	al, byte ptr[esi]							
								inc		esi											
								mov     dx, [ebx + eax * 2]		//edx = ...rgb	
								movd	ecx, mm1    		   //nMask32 		
								mov		ax, dx					//eax = ...rgb	
								shl		eax, 16					//eax = rgb...	
								mov		ax, dx					//eax = rgbrgb	
								and		eax, ecx				//eax = .g.r.b	
								mov		dx, [edi]				//edx = ...rgb
								mov		bx, dx					//ebx = ...rgb	
								shl		ebx, 16					//ebx = rgb...	
								mov		bx, dx					//ebx = rgbrgb	
								movd	edx, mm3				//	nAlpha    	
								and		ebx, ecx				//ebx = .g.r.b	
								sub     eax, ebx              //eax = c1 - c2
								imul    eax, edx				//eax = (c1 - c2)*nAlpha 
								shr		eax, 5					//c=(c1 - c2)*nAlpha/
								add     eax, ebx               //c=(c1 - c2)*nAlpha/32 + c2
								and     eax, ecx				//eax = .g.r.b	
								mov     dx, ax					//edx = ...r.b	
								shr     eax, 16					//eax = ....g.	
								add		edi, 2										
								or      ax, dx					//eax = ...rgb	
								movd    ecx, mm2    		                      
								mov		[edi - 2], ax	
							//	inc    esi
							//	add      edi,2
								dec ecx
								jnz _DrawFullLineSection_LineLocal_HalfAlpha_pixel
							}
							movd edx,mm4
                            _DrawFullLineSection_LineLocal_HalfAlpha_End:
							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_

							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}
					}
				}
			}
		}

		_DrawPartLineSection_:
		{
/*			mov		eax, Clipper.left
			or		eax, eax
			jz		_DrawPartLineSection_SkipRight_Line_

			mov		eax, Clipper.right
			or		eax, eax
			jz		_DrawPartLineSection_SkipLeft_Line_*/
//		}

	//	_DrawPartLineSection_Line_:
//		{
	/*		mov		eax, edx
			mov	edx, Clipper.width    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_LineLocal_CheckAlpha_
			_DrawPartLineSection_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
				neg		edx
			}
			
			_DrawPartLineSection_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_Line_
					_DrawPartLineSection_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_Line_
					}
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_LineLocal_DirectCopy_:
				{
					mov      ebx, pPalette // mm0: pPalette
					movd     mm4,edx
					sub      ecx, 4
					jl  _DrawPartLineSection_CopyPixel_Continue
					_DrawPartLineSection_CopyPixel_4:
					{
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov dx,word ptr[ebx+eax] //cx中存放的是最后点的颜色
						shl edx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor edx,edx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						shl edx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm2,ecx
						por  mm1,mm2 //////目标颜色获取成功在mm1中
						add  esi,4
						movq [edi],mm1
						add edi,8
						sub ecx,4
						jg	_DrawPartLineSection_CopyPixel_4
					}
                    _DrawPartLineSection_CopyPixel_Continue:
					add ecx,4
					jz _DrawPartLineSection_CopyPixel_End
                    _DrawPartLineSection_CopyPixel_:
					{
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov ax,word ptr[ebx+eax]
						mov [edi],ax
						add edi,2
						inc  esi
						dec ecx
						jnz _DrawPartLineSection_CopyPixel_
						
					}
                    _DrawPartLineSection_CopyPixel_End:
					movd edx,mm4
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					shr     ebx, 3 //变成32alpha值
					sub      ecx, 4
					jl _DrawPartLineSection_LineLocal_HalfAlpha_Continue
                    _DrawPartLineSection_LineLocal_HalfAlpha_Pixel_4:
					{
						movd    mm2 , edx //腾出edx来
						movd    mm3 , ecx //腾出ecx来
						xor ecx,ecx
						movq mm0,[edi] //mm2中存放了目标4个象素
						//对源目标进行处理，读取四个点
						mov edx,pPalette //这调色板
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov cx,word ptr[edx+eax] //cx中存放的是最后点的颜色
						shl ecx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor ecx,ecx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov cx,word ptr[edx+eax]
						shl ecx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm4,ecx
						por  mm1,mm4 //////目标颜色获取成功在mm1中
						movd edx,mm2
						movd ecx,mm3
						add esi,4
						//构造alpha
						xor eax,eax
						mov ax,bx
						shl eax,16
						mov ax,bx
						movd mm5,eax
						psllq mm5,32
						movd mm4,eax
						por mm5,mm4 //混合值存放在mm5中

								///以下是四个一起混合的汇编
								//mm0 --- 目标
								//mm3 -- 最终的g
								//mm4 -- 最终的r
								//mm5 -- alpha
								//mm6 -- mask1
								//mm7 -- mask2
								//mm1 --- 最终的b
								//mm2 --- 做临时混合用
								///g
						movq mm2,mm0
						psrlw mm2,5  //把g留下来
						pand  mm2,mm7
						movq mm3,mm1
						psrlw mm3,5
						pand mm3,mm7

						psubsw mm3,mm2
						PMULLW mm3,mm5
						psrlw mm3,5
						paddusw mm3,mm2//g混合完成 结果保留在mm3中

						///r
						movq mm2,mm0
						psrlw mm2,11
						pand mm2,mm6
						movq mm4,mm1
						psrlw mm4,11
						pand mm4,mm6

						psubsw mm4,mm2
						pmullw mm4,mm5
						psrlw mm4,5
						paddusw mm4,mm2

						//b
						pand mm0,mm6
						pand mm1,mm6

						psubsw mm1,mm0
						pmullw mm1,mm5
						psrlw  mm1,5
						paddusw mm1,mm0

						psllw mm4,11
						psllw mm3,5

						por mm1,mm3
						por mm1,mm4

						movq [edi] ,mm1
						add edi,8																							
						sub ecx,4

						jg _DrawPartLineSection_LineLocal_HalfAlpha_Pixel_4

					}
                    _DrawPartLineSection_LineLocal_HalfAlpha_Continue:
					add ecx,4
					jz _DrawPartLineSection_LineLocal_HalfAlpha_End
					movd mm4,edx
					movd mm3,ebx
					movd mm0,pPalette
					movd mm1,nMask32
                    _DrawPartLineSection_LineLocal_HalfAlpha_Pixel:
					{
						//这里混合一个像素//
						movd	mm2, ecx    								
						xor	    eax, eax    								
						movd    ebx, mm0    			
					
						dec ecx
						jnz _DrawPartLineSection_LineLocal_HalfAlpha_Pixel
					}
					movd edx,mm4
                    _DrawPartLineSection_LineLocal_HalfAlpha_End:
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					mov      ebx, pPalette // mm0: pPalette
					movd     mm4,edx
					sub      ecx, 4
					jl   _DrawPartLineSection_CopyPixel_Part_alpha255_Continue
					_DrawPartLineSection_CopyPixel_Part_Pixel4_alpha255:
					{
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov dx,word ptr[ebx+eax] //cx中存放的是最后点的颜色
						shl edx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor edx,edx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						shl edx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm2,ecx
						por  mm1,mm2 //////目标颜色获取成功在mm1中
						add  esi,4
						movq [edi],mm1
						add edi,8
						sub ecx,4
						jg	_DrawPartLineSection_CopyPixel_Part_Pixel4_alpha255
					}
                   _DrawPartLineSection_CopyPixel_Part_alpha255_Continue:
					add ecx,4
					jz _DrawPartLineSection_CopyPixel_Part_alpha255_End
                    _DrawPartLineSection_CopyPixel_Part_alpha_255_Pixel:
					{
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov ax,word ptr[ebx+eax]
						mov [edi],ax
						add edi,2
						inc esi
						dec ecx
						jnz _DrawPartLineSection_CopyPixel_Part_alpha_255_Pixel
					}

                    _DrawPartLineSection_CopyPixel_Part_alpha255_End:

					movd edx,mm4
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
					mov		ebx, 255
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					shr     ebx, 3 //变成32alpha值
					sub      ecx, 4
					jl _DrawPartLineSection_LineLocal_HalfAlpha_Part_Continue
					_DrawPartLineSection_HalfAlphaPixel_Part_Pixel4:
					{
						movd    mm2 , edx //腾出edx来
						movd    mm3 , ecx //腾出ecx来
						xor ecx,ecx
						movq mm0,[edi] //mm2中存放了目标4个象素
						//对源目标进行处理，读取四个点
						mov edx,pPalette //这调色板
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov cx,word ptr[edx+eax] //cx中存放的是最后点的颜色
						shl ecx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor ecx,ecx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov cx,word ptr[edx+eax]
						shl ecx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm4,ecx
						por  mm1,mm4 //////目标颜色获取成功在mm1中
						movd edx,mm2
						movd ecx,mm3
						add esi,4
						//构造alpha
						xor eax,eax
						mov ax,bx
						shl eax,16
						mov ax,bx
						movd mm5,eax
						psllq mm5,32
						movd mm4,eax
						por mm5,mm4 //混合值存放在mm5中

								///以下是四个一起混合的汇编
								//mm0 --- 目标
								//mm3 -- 最终的g
								//mm4 -- 最终的r
								//mm5 -- alpha
								//mm6 -- mask1
								//mm7 -- mask2
								//mm1 --- 最终的b
								//mm2 --- 做临时混合用
								///g
						movq mm2,mm0
						psrlw mm2,5  //把g留下来
						pand  mm2,mm7
						movq mm3,mm1
						psrlw mm3,5
						pand mm3,mm7

						psubsw mm3,mm2
						PMULLW mm3,mm5
						psrlw mm3,5
						paddusw mm3,mm2//g混合完成 结果保留在mm3中

						///r
						movq mm2,mm0
						psrlw mm2,11
						pand mm2,mm6
						movq mm4,mm1
						psrlw mm4,11
						pand mm4,mm6

						psubsw mm4,mm2
						pmullw mm4,mm5
						psrlw mm4,5
						paddusw mm4,mm2

						//b
						pand mm0,mm6
						pand mm1,mm6

						psubsw mm1,mm0
						pmullw mm1,mm5
						psrlw  mm1,5
						paddusw mm1,mm0

						psllw mm4,11
						psllw mm3,5

						por mm1,mm3
						por mm1,mm4

						movq [edi] ,mm1
						add edi,8																							
						sub ecx,4

						jg _DrawPartLineSection_HalfAlphaPixel_Part_Pixel4
					}
                    _DrawPartLineSection_LineLocal_HalfAlpha_Part_Continue:
					add ecx,4
					jz _DrawPartLineSection_CopyPixel_Part_End
					movd mm4,edx
					movd mm3,ebx
					movd mm0,pPalette
					movd mm1,nMask32

                    _DrawPartLineSection_CopyPixel_Part_Pixel:
					{
						movd	mm2, ecx    								
						
						dec ecx
						jnz _DrawPartLineSection_CopyPixel_Part_Pixel

					}

                    _DrawPartLineSection_CopyPixel_Part_End:
					movd    edx, mm4
					neg		edx
					dec		Clipper.height
					mov ebx,255
					jg		_DrawPartLineSection_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			mov	edx, Clipper.width    // mm1: Clipper.width
			or		eax, eax
			jnz		_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_
			_DrawPartLineSection_SkipLeft_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				_DrawPartLineSection_SkipLeft_LineLocal_CheckAlpha_:
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipLeft_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_
			}
			
			_DrawPartLineSection_SkipLeft_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		edx, nSprSkipPerLine
				_DrawPartLineSection_SkipLeft_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
					neg		edx
					jmp		_DrawPartLineSection_SkipLeft_Line_
					_DrawPartLineSection_SkipLeft_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipLeft_LineSkipLocal_
						add		esi, edx
						neg		edx
						jmp		_DrawPartLineSection_SkipLeft_Line_
					}
				}
			}
			_DrawPartLineSection_SkipLeft_LineLocal_Alpha_:
			{
				sub		edx, eax		;先把eax减了，这样後面就可以不需要保留eax了
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_
						
				//_DrawPartLineSection_SkipLeft_LineLocal_DirectCopy_:
				{
					mov      ebx, pPalette // mm0: pPalette
					movd     mm4,edx
					sub      ecx, 4
					jl   _DrawPartLineSection_SkipLeft_CopyPixel_alpha_255_Continue
					_DrawPartLineSection_SkipLeft_CopyPixel_alpha255_Pixel4:
					{
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov dx,word ptr[ebx+eax] //cx中存放的是最后点的颜色
						shl edx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor edx,edx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						shl edx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm2,ecx
						por  mm1,mm2 //////目标颜色获取成功在mm1中
						add  esi,4
						movq [edi],mm1
						add edi,8
						sub ecx,4
						jg	_DrawPartLineSection_SkipLeft_CopyPixel_alpha255_Pixel4
					}
                    _DrawPartLineSection_SkipLeft_CopyPixel_alpha_255_Continue:
					add ecx,4
					jz _DrawPartLineSection_SkipLeft_CopyPixel_alpha_255_End
                    _DrawPartLineSection_SkipLeft_CopyPixel_alpha_255_:
					{
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov ax,word ptr[ebx+eax]
						mov [edi],ax
						add edi,2
						inc esi
						dec ecx
						jnz _DrawPartLineSection_SkipLeft_CopyPixel_alpha_255_
					}
                    _DrawPartLineSection_SkipLeft_CopyPixel_alpha_255_End:
					movd edx,mm4
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}

				_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_:
				{
					shr     ebx, 3 //变成32alpha值
					sub      ecx, 4
					jl _DrawPartLineSection_SkipLeft_LineLocal_nAlpha_Continue
					_DrawPartLineSection_SkipLeft_LineLocal_nAlpha_Pixel4:
					{
						movd    mm2 , edx //腾出edx来
						movd    mm3 , ecx //腾出ecx来
						xor ecx,ecx
						movq mm0,[edi] //mm2中存放了目标4个象素
						//对源目标进行处理，读取四个点
						mov edx,pPalette //这调色板
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov cx,word ptr[edx+eax] //cx中存放的是最后点的颜色
						shl ecx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor ecx,ecx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov cx,word ptr[edx+eax]
						shl ecx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm4,ecx
						por  mm1,mm4 //////目标颜色获取成功在mm1中
						movd edx,mm2
						movd ecx,mm3
						add esi,4
						//构造alpha
						xor eax,eax
						mov ax,bx
						shl eax,16
						mov ax,bx
						movd mm5,eax
						psllq mm5,32
						movd mm4,eax
						por mm5,mm4 //混合值存放在mm5中

								///以下是四个一起混合的汇编
								//mm0 --- 目标
								//mm3 -- 最终的g
								//mm4 -- 最终的r
								//mm5 -- alpha
								//mm6 -- mask1
								//mm7 -- mask2
								//mm1 --- 最终的b
								//mm2 --- 做临时混合用
								///g
						movq mm2,mm0
						psrlw mm2,5  //把g留下来
						pand  mm2,mm7
						movq mm3,mm1
						psrlw mm3,5
						pand mm3,mm7

						psubsw mm3,mm2
						PMULLW mm3,mm5
						psrlw mm3,5
						paddusw mm3,mm2//g混合完成 结果保留在mm3中

						///r
						movq mm2,mm0
						psrlw mm2,11
						pand mm2,mm6
						movq mm4,mm1
						psrlw mm4,11
						pand mm4,mm6

						psubsw mm4,mm2
						pmullw mm4,mm5
						psrlw mm4,5
						paddusw mm4,mm2

						//b
						pand mm0,mm6
						pand mm1,mm6

						psubsw mm1,mm0
						pmullw mm1,mm5
						psrlw  mm1,5
						paddusw mm1,mm0

						psllw mm4,11
						psllw mm3,5

						por mm1,mm3
						por mm1,mm4

						movq [edi] ,mm1
						add edi,8																							
						sub ecx,4

						jg _DrawPartLineSection_SkipLeft_LineLocal_nAlpha_Pixel4
					}
                    _DrawPartLineSection_SkipLeft_LineLocal_nAlpha_Continue:
					add ecx,4
					jz _DrawPartLineSection_SkipLeft_LineLocal_nAlpha_End
					movd mm4,edx
					movd mm3,ebx
					movd mm0,pPalette
					movd mm1,nMask32

                    _DrawPartLineSection_SkipLeft_LineLocal_nAlpha_Pixel:
					{
						
					}

                    _DrawPartLineSection_SkipLeft_LineLocal_nAlpha_End:
					movd    edx, mm4
					or		edx, edx
					jnz		_DrawPartLineSection_SkipLeft_LineLocal_
					dec		Clipper.height
					mov    ebx,255
					jg		_DrawPartLineSection_SkipLeft_LineSkip_
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			mov	edx, Clipper.width    // mm1: Clipper.width
			_DrawPartLineSection_SkipRight_LineLocal_:
			{
				read_alpha_2_ebx_run_length_2_eax
				or		ebx, ebx
				jnz		_DrawPartLineSection_SkipRight_LineLocal_Alpha_
				add		edi, eax
				add		edi, eax
				sub		edx, eax
				jg		_DrawPartLineSection_SkipRight_LineLocal_

				dec		Clipper.height
				jz		_EXIT_WAY_

				add		edi, edx
				add		edi, edx
				neg		edx
			}
			
			_DrawPartLineSection_SkipRight_LineSkip_:
			{
				add		edi, nBuffNextLine
				//跳过nSprSkipPerLine像素的sprite内容
				mov		eax, edx
				mov		edx, nSprSkipPerLine
				or		eax, eax
				jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_
				_DrawPartLineSection_SkipRight_LineSkipLocal_:
				{
					read_alpha_2_ebx_run_length_2_eax
					
					_DrawPartLineSection_SkipRight_LineSkipLocal_CheckAlpha_:
					or		ebx, ebx
					jnz		_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_
					sub		edx, eax
					jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
					jmp		_DrawPartLineSection_SkipRight_Line_
					_DrawPartLineSection_SkipRight_LineSkipLocal_Alpha_:
					{
						add		esi, eax
						sub		edx, eax
						jg		_DrawPartLineSection_SkipRight_LineSkipLocal_
						jmp		_DrawPartLineSection_SkipRight_Line_
					}
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_:
			{
				sub		edx, eax
				jle		_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_		//不能全画这eax个相同alpha值的像点，后面有点已经超出区域

				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_
						
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_:
				{
					mov      ebx, pPalette // mm0: pPalette
					movd     mm4,edx
					sub      ecx, 4
					jl   _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Continue
					_DrawPartLineSection_SkipRight_LineLocal_Alpha255_Pixel4:
					{
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov dx,word ptr[ebx+eax] //cx中存放的是最后点的颜色
						shl edx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor edx,edx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						shl edx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm2,ecx
						por  mm1,mm2 //////目标颜色获取成功在mm1中
						add  esi,4
						movq [edi],mm1
						add edi,8
						sub ecx,4
						jg	_DrawPartLineSection_SkipRight_LineLocal_Alpha255_Pixel4
					}
                    _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Continue:
					add ecx,4
					jz _DrawPartLineSection_SkipRight_LineLocal_Alpha255_End
                   _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Pixel:
					{
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov ax,word ptr[ebx+eax]
						mov [edi],ax
						add edi,2
						inc esi
						dec ecx
						jnz _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Pixel

					}
                    _DrawPartLineSection_SkipRight_LineLocal_Alpha255_End:
					movd edx,mm4

					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					shr     ebx, 3 //变成32alpha值
					sub      ecx, 4
					jl _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Continue
					_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Pixel4:
					{
						movd    mm2 , edx //腾出edx来
						movd    mm3 , ecx //腾出ecx来
						xor ecx,ecx
						movq mm0,[edi] //mm2中存放了目标4个象素
						//对源目标进行处理，读取四个点
						mov edx,pPalette //这调色板
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov cx,word ptr[edx+eax] //cx中存放的是最后点的颜色
						shl ecx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor ecx,ecx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov cx,word ptr[edx+eax]
						shl ecx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm4,ecx
						por  mm1,mm4 //////目标颜色获取成功在mm1中
						movd edx,mm2
						movd ecx,mm3
						add esi,4
						//构造alpha
						xor eax,eax
						mov ax,bx
						shl eax,16
						mov ax,bx
						movd mm5,eax
						psllq mm5,32
						movd mm4,eax
						por mm5,mm4 //混合值存放在mm5中

								///以下是四个一起混合的汇编
								//mm0 --- 目标
								//mm3 -- 最终的g
								//mm4 -- 最终的r
								//mm5 -- alpha
								//mm6 -- mask1
								//mm7 -- mask2
								//mm1 --- 最终的b
								//mm2 --- 做临时混合用
								///g
						movq mm2,mm0
						psrlw mm2,5  //把g留下来
						pand  mm2,mm7
						movq mm3,mm1
						psrlw mm3,5
						pand mm3,mm7

						psubsw mm3,mm2
						PMULLW mm3,mm5
						psrlw mm3,5
						paddusw mm3,mm2//g混合完成 结果保留在mm3中

						///r
						movq mm2,mm0
						psrlw mm2,11
						pand mm2,mm6
						movq mm4,mm1
						psrlw mm4,11
						pand mm4,mm6

						psubsw mm4,mm2
						pmullw mm4,mm5
						psrlw mm4,5
						paddusw mm4,mm2

						//b
						pand mm0,mm6
						pand mm1,mm6

						psubsw mm1,mm0
						pmullw mm1,mm5
						psrlw  mm1,5
						paddusw mm1,mm0

						psllw mm4,11
						psllw mm3,5

						por mm1,mm3
						por mm1,mm4

						movq [edi] ,mm1
						add edi,8																							
						sub ecx,4

						jg _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Pixel4
					}
                    _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Continue:
					add ecx,4
					jz _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_End
					movd mm4,edx
					movd mm3,ebx
					movd mm0,pPalette
					movd mm1,nMask32
                    _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Pixel:
					{
						

					}

                    _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_End:
					movd	edx, mm4
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				cmp		ebx, 255
				jl		_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_SkipRight_LineLocal_DirectCopy_Part_:
				{
					mov      ebx, pPalette // mm0: pPalette
					movd     mm4,edx
					sub      ecx, 4
					jl   _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Continue1
					_DrawPartLineSection_SkipRight_LineLocal_Alpha255_Part_Pixel4:
					{
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov dx,word ptr[ebx+eax] //cx中存放的是最后点的颜色
						shl edx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor edx,edx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						shl edx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov dx,word ptr[ebx+eax]
						movd mm2,ecx
						por  mm1,mm2 //////目标颜色获取成功在mm1中
						add  esi,4
						movq [edi],mm1
						add edi,8
						sub ecx,4
						jg	_DrawPartLineSection_SkipRight_LineLocal_Alpha255_Part_Pixel4
					}
                   _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Continue1:
					add ecx,4
					jz _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Part_End
                   _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Part_Pixel:
					{
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov ax,word ptr[ebx+eax]
						mov [edi],ax
						add edi,2
						inc esi
						dec ecx
						jnz _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Part_Pixel

					}
                    _DrawPartLineSection_SkipRight_LineLocal_Alpha255_Part_End:
					movd edx,mm4
					neg		edx
					mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_
					jmp		_EXIT_WAY_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_:
				{
					shr     ebx, 3 //变成32alpha值
					sub      ecx, 4
					jl _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_Continue
					_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_Pixel4:
					{
						movd    mm2 , edx //腾出edx来
						movd    mm3 , ecx //腾出ecx来
						xor ecx,ecx
						movq mm0,[edi] //mm2中存放了目标4个象素
						//对源目标进行处理，读取四个点
						mov edx,pPalette //这调色板
						xor eax,eax
						//最后一个点
						mov al,byte ptr[esi+3]
						shl ax,1
						mov cx,word ptr[edx+eax] //cx中存放的是最后点的颜色
						shl ecx,16        
						xor eax,eax
								//第2个点
						mov al,byte ptr[esi+2]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm1,cx //把前两个颜色存放在mm1
						psllq mm1,32

						xor eax,eax
						xor ecx,ecx
						mov al,byte ptr[esi+1]
						shl ax,1
						mov cx,word ptr[edx+eax]
						shl ecx,16
						xor eax,eax
						mov al,byte ptr[esi]
						shl ax,1
						mov cx,word ptr[edx+eax]
						movd mm4,ecx
						por  mm1,mm4 //////目标颜色获取成功在mm1中
						movd edx,mm2
						movd ecx,mm3
						add esi,4
						//构造alpha
						xor eax,eax
						mov ax,bx
						shl eax,16
						mov ax,bx
						movd mm5,eax
						psllq mm5,32
						movd mm4,eax
						por mm5,mm4 //混合值存放在mm5中

								///以下是四个一起混合的汇编
								//mm0 --- 目标
								//mm3 -- 最终的g
								//mm4 -- 最终的r
								//mm5 -- alpha
								//mm6 -- mask1
								//mm7 -- mask2
								//mm1 --- 最终的b
								//mm2 --- 做临时混合用
								///g
						movq mm2,mm0
						psrlw mm2,5  //把g留下来
						pand  mm2,mm7
						movq mm3,mm1
						psrlw mm3,5
						pand mm3,mm7

						psubsw mm3,mm2
						PMULLW mm3,mm5
						psrlw mm3,5
						paddusw mm3,mm2//g混合完成 结果保留在mm3中

						///r
						movq mm2,mm0
						psrlw mm2,11
						pand mm2,mm6
						movq mm4,mm1
						psrlw mm4,11
						pand mm4,mm6

						psubsw mm4,mm2
						pmullw mm4,mm5
						psrlw mm4,5
						paddusw mm4,mm2

						//b
						pand mm0,mm6
						pand mm1,mm6

						psubsw mm1,mm0
						pmullw mm1,mm5
						psrlw  mm1,5
						paddusw mm1,mm0

						psllw mm4,11
						psllw mm3,5

						por mm1,mm3
						por mm1,mm4

						movq [edi] ,mm1
						add edi,8																							
						sub ecx,4

						jg _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_Pixel4
					}
                    _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_Continue:
					add ecx,4
					jz _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_End
					movd mm4,edx
					movd mm3,ebx
					movd mm0,pPalette
					movd mm1,nMask32
                    _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_Pixel:
					{
						movd	mm2, ecx    								
						xor	    eax, eax    								
						movd    ebx, mm0    			/* pPalette 		
						mov  	al, byte ptr[esi]							
						inc		esi											
						mov     dx, [ebx + eax * 2]		/*edx = ...rgb	
						movd	ecx, mm1    		  /* nMask32 	
						mov		ax, dx					/*eax = ...rgb	
						shl		eax, 16					/*eax = rgb...	
						mov		ax, dx					/*eax = rgbrgb	
						and		eax, ecx				/*eax = .g.r.b	
						mov		dx, [edi]				/*edx = ...rgb	
						mov		bx, dx					/*ebx = ...rgb	
						shl		ebx, 16					/*ebx = rgb...	
						mov		bx, dx					/*ebx = rgbrgb	
						movd	edx, mm3				/*	nAlpha    	
						and		ebx, ecx				/*ebx = .g.r.b	
						sub     eax, ebx              /*eax = c1 - c2 
						imul    eax, edx				/*eax = (c1 - c2)*nAlpha 
						shr		eax, 5					/*c=(c1 - c2)*nAlpha/32
						add     eax, ebx               /*c=(c1 - c2)*nAlpha/32 + c2
						and     eax, ecx				/*eax = .g.r.	
						mov     dx, ax					/*edx = ...r.b	
						shr     eax, 16					/*eax = ....g.	
						add		edi, 2										
						or      ax, dx					/*eax = ...rgb	
						movd    ecx, mm2    		                      
						mov		[edi - 2], ax		
						dec ecx
						jnz _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_Pixel

					}
                    _DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_Part_End:
					movd	edx, mm4
					neg		edx
					mov		ebx, 128
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_EXIT_WAY_
				}
			}
		}*/
 //        }
//		_EXIT_WAY_:
//        emms
//	}

}
void g_DrawSpriteAlphaUseMMX16(void* node,void* canvas)
{

}
