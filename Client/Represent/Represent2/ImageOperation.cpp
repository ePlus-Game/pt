/*****************************************************************************************
//  图形到内存区域的操作
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-11
*****************************************************************************************/
#include "ImageOperation.h"
#include "crtdbg.h"

#define SHIFTNUM_RGB565_RCOLOR		11
#define SHIFTNUM_RGB565_GCOLOR		5


#define SHIFTNUM_RGB555_RCOLOR		10
#define SHIFTNUM_RGB555_GCOLOR		5

#define RGB555_FROM_RGB(Pixel, r, g, b)					\
{									\
	Pixel = ((r>>3)<<10)|((g>>3)<<5)|(b>>3);			\
}


#define RGB565_FROM_RGB(Pixel, r, g, b)					\
{									\
	Pixel = ((r>>3)<<11)|((g>>2)<<5)|(b>>3);			\
}
#define RGB1555_FROM_ARGB(Pixel, a, r, g, b)					\
{									\
	Pixel = (a<<15)|((r>>3)<<10)|((g>>3)<<5)|(b>>3);			\
}

#define RGB_FROM_RGB565(Pixel, r, g, b)					\
{									\
	r = (((Pixel&0xF800)>>11)<<3);		 			\
	g = (((Pixel&0x07E0)>>5)<<2); 					\
	b = ((Pixel&0x001F)<<3); 					\
}

static unsigned int l_uRGBBitMask32 = 0x07e0f81f;

//设置16位位图使用的格式565或555
void RIO_Set16BitImageFormat(int b565)
{
	l_uRGBBitMask32 = b565 ? 0x07e0f81f : 0x03e07c1f;
}

struct KRClipperInfo
{
	int			x;			// 裁减后的X坐标
	int			y;			// 裁减后的Y坐标
	int			width;		// 裁减后的宽度
	int			height;		// 裁减后的高度
	int			left;		// 上边界裁剪量
	int			top;		// 左边界裁剪量
	int			right;		// 右边界裁剪量
};


//功能：计算剪切范围
//参数：nX,nY,nSrcWidth,nSrcHeight -> 给出源范围
//		nDestWidth,nDestHeight -> 目标的范围限定
//		pClipper -> 存储剪切计算的结果
static int  RIO_ClipCopyRect(int nX, int nY, int nSrcWidth, int nSrcHeight, int nDestWidth, int nDestHeight, KRClipperInfo* pClipper)
{
	_ASSERT(pClipper);
	// 初始化裁减量
	pClipper->x = nX;
	pClipper->y = nY;
	pClipper->width = nSrcWidth;
	pClipper->height = nSrcHeight;
	pClipper->top = 0;
	pClipper->left = 0;
	pClipper->right = 0;

	// 上边界裁减
	if (pClipper->y < 0)
	{
		pClipper->y = 0;
		pClipper->top = -nY;
		pClipper->height += nY;
	}
	if (pClipper->height <= 0)
		return 0;
	
	// 下边界裁减
	if (pClipper->height > nDestHeight - pClipper->y)
		pClipper->height = nDestHeight - pClipper->y;
	if (pClipper->height <= 0)
		return 0;

	// 左边界裁减
	if (pClipper->x < 0)
	{
		pClipper->x = 0;
		pClipper->left = -nX;
		pClipper->width += nX;
	}
	if (pClipper->width <= 0)
		return 0;

	// 右边界裁减
	if (pClipper->width > nDestWidth - pClipper->x)
	{
		pClipper->right = pClipper->width + pClipper->x - nDestWidth;
		pClipper->width -= pClipper->right;
	}
	if (pClipper->width <= 0)
		return 0;
	
	return 1;
}


//读取alpha数据到ebx，读取跳点距离到eax
#define		read_alpha_2_ebx_run_length_2_eax		\
				{	xor		eax, eax			}	\
				{	xor		ebx, ebx			}	\
				{	mov		al,	 byte ptr[esi]	}	\
				{	inc		esi					}	\
				{	mov		bl,  byte ptr[esi]	}	\
				{	inc		esi					}
//从调色板复制像点颜色到eax
#define		copy_pixel_use_eax	/*ebx指向调色板*/	\
				{	movzx	eax, byte ptr[esi]	}	\
				{	add		eax, eax			}	\
				{	inc		esi					}	\
				{	mov		ax, [ebx + eax]		}	\
				{	mov		[edi], ax			}	\
				{	inc		edi					}	\
				{	inc		edi					}
//从调色板复制像点颜色到eax 原颜色格式565目标颜色格式555版本
#define		copy_pixel_use_eax_1555/*ebx指向调色板*/\
				{	push	ecx					}	\
				{	movzx	eax, byte ptr[esi]	}	\
				{	add		eax, eax			}	\
				{	inc		esi					}	\
				{	mov		ax, [ebx + eax]		}	\
				{	mov		cx, ax				}	\
				{	shr		ax, 6				}	/*eax = ....rg*/	\
				{	shl		eax, 16				}	/*eax = .rg...*/	\
				{	mov		ax, 0x1f			}	/*eax = .rg..m(m=mask)*/	\
				{	and		ax, cx				}	/*eax = .rg..b*/	\
				{	shl		ax, 11				}	/*eax = .rgb..*/	\
				{	shr		eax,11				}	/*eax = ...rgb*/	\
				{	mov		[edi], ax			}	\
				{	inc		edi					}	\
				{	inc		edi					}	\
				{	pop		ecx					}
//固定alpha值合两个颜色，运算使用了eax,ebx,edx
#define		mix_2_pixel_color_use_eabdx									\
				{	push	ecx					}						\
				{	mov     ebx, pPalette		}						\
				{	movzx	eax, byte ptr[esi]	}						\
				{	mov		ecx, l_uRGBBitMask32}						\
				{	add		eax, eax			}						\
				{	inc		esi					}						\
				{	mov     dx, [ebx + eax]		}	/*edx = ...rgb*/	\
				{	mov		ax, dx				}	/*eax = ...rgb*/	\
				{	shl		eax, 16				}	/*eax = rgb...*/	\
				{	mov		ax, dx				}	/*eax = rgbrgb*/	\
				{	and		eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov		dx, [edi]			}	/*edx = ...rgb*/	\
				{	mov		bx, dx				}	/*ebx = ...rgb*/	\
				{	shl		ebx, 16				}	/*ebx = rgb...*/	\
				{	mov		bx, dx				}	/*ebx = rgbrgb*/	\
				{	and		ebx, ecx			}	/*ebx = .g.r.b*/	\
				{	add		eax, ebx			}						\
				{	add		ebx, ebx			}						\
				{	add		eax, ebx			}						\
				{	shr		eax, 2				}	/*c = (3xc1+c2)/4*/	\
				{	and     eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov     dx, ax				}	/*edx = ...r.b*/	\
				{	shr     eax, 16				}	/*eax = ....g.*/	\
				{	or      ax, dx				}	/*eax = ...rgb*/	\
				{	mov		[edi], ax			}						\
				{	inc		edi					}						\
				{	inc		edi					}						\
				{	pop		ecx					}
//固定alpha值合两个颜色，运算使用了eax,ebx,edx，源颜色格式565 目标颜色格式555版本
#define		mix_2_pixel_color_use_eabdx_1555							\
				{	push	ecx					}						\
				{	mov     ebx, pPalette		}						\
				{	movzx	eax, byte ptr[esi]	}						\
				{	add		eax, eax			}						\
				{	inc		esi					}						\
				{	mov     dx, [ebx + eax]		}	/*edx = ...rgb*/	\
				{	mov		ax, dx				}	/*565->555*/		\
				{	shr		dx, 6				}	/*edx = ....rg*/	\
				{	shl		edx, 16				}	/*edx = .rg...*/	\
				{	mov		dx, 0x1f			}	/*edx = .rg..m (m=mask)*/\
				{	and		dx, ax				}	/*edx = .rg..b*/	\
				{	shl		dx, 11				}	/*edx = .rgb..*/	\
				{	shl		edx, 5				}	/*edx = rgb...*/	\
				{	mov		eax, edx			}   /*eax = rgb...*/	\
				{	shr		eax, 16				}	/*eax = ...rgb*/	\
				{	or		eax, edx			}	/*eax = rgbrgb*/	\
				{	mov		ecx, 0x03e07c1f		}	/*555 mask*/		\
				{	and		eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov		dx, [edi]			}	/*edx = ...rgb*/	\
				{	mov		bx, dx				}	/*ebx = ...rgb*/	\
				{	shl		ebx, 16				}	/*ebx = rgb...*/	\
				{	mov		bx, dx				}	/*ebx = rgbrgb*/	\
				{	and		ebx, ecx			}	/*ebx = .g.r.b*/	\
				{	add		eax, ebx			}						\
				{	add		ebx, ebx			}						\
				{	add		eax, ebx			}						\
				{	shr		eax, 2				}	/*c = (3xc1+c2)/4*/	\
				{	and     eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov     dx, ax				}	/*edx = ...r.b*/	\
				{	shr     eax, 16				}	/*eax = ....g.*/	\
				{	or      ax, dx				}	/*eax = ...rgb*/	\
				{	mov		[edi], ax			}						\
				{	inc		edi					}						\
				{	inc		edi					}						\
				{	pop		ecx					}
//alpha混合两个颜色，运算使用了eax,ebx,edx
#define		mix_2_pixel_color_alpha_use_eabdx							\
				{	push	ecx					}						\
				{	mov     ebx, pPalette		}						\
				{	movzx	eax, byte ptr[esi]	}						\
				{	mov		ecx, l_uRGBBitMask32}						\
				{	add		eax, eax			}						\
				{	inc		esi					}						\
				{	mov     dx, [ebx + eax]		}	/*edx = ...rgb*/	\
				{	mov		ax, dx				}	/*eax = ...rgb*/	\
				{	shl		eax, 16				}	/*eax = rgb...*/	\
				{	mov		ax, dx				}	/*eax = rgbrgb*/	\
				{	and		eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov		dx, [edi]			}	/*edx = ...rgb*/	\
				{	mov		bx, dx				}	/*ebx = ...rgb*/	\
				{	shl		ebx, 16				}	/*ebx = rgb...*/	\
				{	mov		bx, dx				}	/*ebx = rgbrgb*/	\
				{	and		ebx, ecx			}	/*ebx = .g.r.b*/	\
				{	mov		edx, nAlpha			}						\
				{	imul	eax, edx			}	/*eax = c1*nAlpha*/	\
				{	neg		edx					}						\
				{	add		edx, 32				}						\
				{	imul	ebx, edx			} /*ebx=c2*(32-nAlpha)*/\
				{	add		eax, ebx			}						\
				{	shr		eax, 5				}	/*c=(c1*nAlpha+c2*(32-nAlpha))/32*/ \
				{	and     eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov     dx, ax				}	/*edx = ...r.b*/	\
				{	shr     eax, 16				}	/*eax = ....g.*/	\
				{	or      ax, dx				}	/*eax = ...rgb*/	\
				{	mov		[edi], ax			}						\
				{	inc		edi					}						\
				{	inc		edi					}						\
				{	pop		ecx					}
//alpha混合两个颜色，运算使用了eax,ebx,edx，源颜色格式565 目标颜色格式555版本
#define		mix_2_pixel_color_alpha_use_eabdx_1555						\
				{	push	ecx					}						\
				{	mov     ebx, pPalette		}						\
				{	movzx	eax, byte ptr[esi]	}						\
				{	add		eax, eax			}						\
				{	inc		esi					}						\
				{	mov     dx, [ebx + eax]		}	/*edx = ...rgb*/	\
				{	mov		ax, dx				}	/*565->555*/		\
				{	shr		dx, 6				}	/*edx = ....rg*/	\
				{	shl		edx, 16				}	/*edx = .rg...*/	\
				{	mov		dx, 0x1f			}	/*edx = .rg..m (m=mask)*/\
				{	and		dx, ax				}	/*edx = .rg..b*/	\
				{	shl		dx, 11				}	/*edx = .rgb..*/	\
				{	shl		edx, 5				}	/*edx = rgb...*/	\
				{	mov		eax, edx			}   /*eax = rgb...*/	\
				{	shr		eax, 16				}	/*eax = ...rgb*/	\
				{	or		eax, edx			}	/*eax = rgbrgb*/	\
				{	mov		ecx, 0x03e07c1f		}	/*555 mask*/		\
				{	and		eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov		dx, [edi]			}	/*edx = ...rgb*/	\
				{	mov		bx, dx				}	/*ebx = ...rgb*/	\
				{	shl		ebx, 16				}	/*ebx = rgb...*/	\
				{	mov		bx, dx				}	/*ebx = rgbrgb*/	\
				{	and		ebx, ecx			}	/*ebx = .g.r.b*/	\
				{	mov		edx, nAlpha			}						\
				{	imul	eax, edx			}	/*eax = c1*nAlpha*/	\
				{	neg		edx					}						\
				{	add		edx, 32				}						\
				{	imul	ebx, edx			} /*ebx=c2*(32-nAlpha)*/\
				{	add		eax, ebx			}						\
				{	shr		eax, 5				}	/*c=(c1*nAlpha+c2*(32-nAlpha))/32*/ \
				{	and     eax, ecx			}	/*eax = .g.r.b*/	\
				{	mov     dx, ax				}	/*edx = ...r.b*/	\
				{	shr     eax, 16				}	/*eax = ....g.*/	\
				{	or      ax, dx				}	/*eax = ...rgb*/	\
				{	mov		[edi], ax			}						\
				{	inc		edi					}						\
				{	inc		edi					}						\
				{	pop		ecx					}

// 				{	mov		cx, ax				}						\
// 				{	shr		ax, 6				}	/*eax = ....rg*/	\
// 				{	shl		eax, 16				}	/*eax = .rg...*/	\
// 				{	mov		ax, 0x1f			}	/*eax = .rg..m (m=mask)*/\
// 				{	and		ax, cx				}	/*eax = .rg..b*/	\
// 				{	shl		ax, 11				}	/*eax = .rgb..*/	\
// 				{	shr		eax,11				}	/*eax = ...rgb*/	\

//功能：覆盖形式复制spr图形到缓冲区，不做alpha计算。
void RIO_CopySprToBuffer(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight,
					 int nX, int nY,  int nPixelBit )
{
	_ASSERT(pSpr && pBuffer && pPalette);
	if ( nPixelBit == 16 )
	{
		// Get the real draw map
		KRClipperInfo tagClipper;	
		if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &tagClipper) == 0)
			return;

		// Get the screen back buffer
		int nBackBufferWidth = 0;

		unsigned char  *pSrcBuf = (unsigned char *)pSpr;
		unsigned short *pDstBuf = (unsigned short *)pBuffer;
		pDstBuf += tagClipper.y * nBufferWidth + tagClipper.x;
		int nBackBufLinePixel = nBufferWidth;

		int nSameCount = 0;
		int nSameAlpha = 0;
		// Skip top clip spr data
		int nClipSkip = nSprWidth * tagClipper.top;
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
				int nLoopCount = nCurLinePos - nCurUnitEndPos; 
				if ( nLoopCount > 0)
				{
					if(nSameAlpha > 0 )
					{
						pSrcBuf += (nSameCount - nLoopCount)*2;
						if ( nLoopCount > tagClipper.width )
						{
							__SprReplaceCopy((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width);
							pSrcBuf += ((nLoopCount - tagClipper.width)*2);
						}
						else
						{
							__SprReplaceCopy((unsigned short**)&pSrcBuf, &pDstBuf, nLoopCount);
						}
						
/*						while(nLoopCount > 0)
						{
							*pDstBuf++ = *((unsigned short*)pSrcBuf);
							pSrcBuf += 2; 
							nLoopCount--;
						}*/
					}
					else
					{
						pDstBuf += nLoopCount;
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
					__SprReplaceCopy((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
/*					while(nSameCountLoop > 0)
					{
						*pDstBuf++ = *((unsigned short*)pSrcBuf);
						pSrcBuf += 2; 
						nSameCountLoop--;
					}*/
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

		return;
	}
	// 对绘制区域进行裁剪
	KRClipperInfo Clipper;
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;

	long nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;
	long nBuffNextLine;

	__asm
	{
		//ebx = nPitch = nBufferWidth * 2
		mov		ebx, nBufferWidth
		add		ebx, ebx

		//使edi指向buffer绘制起点edi = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
		mov		edi, pBuffer
		mov		eax, Clipper.y
		mul		ebx
		mov		edx, Clipper.x
		add		eax, edx
		add		eax, edx
		add		edi, eax

		//nBuffNextLine = nPitch - Clipper.width * 2;
		mov		edx, Clipper.width
		add		edx, edx
		sub		ebx, edx
		mov		nBuffNextLine, ebx

		//使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		esi, pSpr
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
				mov		edx, Clipper.width
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
						sub		edx, eax
						mov		ecx, eax
						mov     ebx, pPalette
						_DrawFullLineSection_CopyPixel_:
						{
							copy_pixel_use_eax
							loop	_DrawFullLineSection_CopyPixel_
						}
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
			mov		edx, Clipper.width
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
				mov     ebx, pPalette

				_DrawPartLineSection_CopyPixel_:
				{
					copy_pixel_use_eax
					loop	_DrawPartLineSection_CopyPixel_
				}
				jmp		_DrawPartLineSection_LineLocal_
			}

			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				mov		ecx, eax
				add		eax, edx
				mov     ebx, pPalette
				_DrawPartLineSection_CopyPixel_Part_:
				{
					copy_pixel_use_eax
					loop	_DrawPartLineSection_CopyPixel_Part_
				}
			
				dec		Clipper.height
				jz		_EXIT_WAY_
				neg		edx
				mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
				jmp		_DrawPartLineSection_LineSkip_
			}
		}
				_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			mov		edx, Clipper.width
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
				mov     ebx, pPalette
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
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			mov		edx, Clipper.width
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
				mov     ebx, pPalette
				_DrawPartLineSection_SkipRight_CopyPixel_:
				{
					copy_pixel_use_eax
					loop	_DrawPartLineSection_SkipRight_CopyPixel_
				}
				jmp		_DrawPartLineSection_SkipRight_LineLocal_
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov     ebx, pPalette
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
		}
		_EXIT_WAY_:
	}
}
//功能：覆盖形式复制spr图形到带透明模式的缓冲区，不做alpha计算。
void RIO_CopySprToAlphaBuffer(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight,
					 int nX, int nY, int nPixelBit)
{
	_ASSERT(pSpr && pBuffer && pPalette);
	if ( nPixelBit == 16 )
	{
		// Get the real draw map
		KRClipperInfo tagClipper;	
		if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &tagClipper) == 0)
			return;

		// Get the screen back buffer
		int nBackBufferWidth = 0;

		unsigned char  *pSrcBuf = (unsigned char *)pSpr;
		unsigned short *pDstBuf = (unsigned short *)pBuffer;
		pDstBuf += tagClipper.y * nBufferWidth + tagClipper.x;
		int nBackBufLinePixel = nBufferWidth;

		int nSameCount = 0;
		int nSameAlpha = 0;
		// Skip top clip spr data
		int nClipSkip = nSprWidth * tagClipper.top;
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
				int nLoopCount = nCurLinePos - nCurUnitEndPos; 
				if ( nLoopCount > 0)
				{
					if(nSameAlpha > 0 )
					{
						pSrcBuf += (nSameCount - nLoopCount)*2;
						if ( nLoopCount > tagClipper.width )
						{
							__Spr565To1555Copy((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width);
							pSrcBuf += ((nLoopCount - tagClipper.width)*2);
						}
						else
						{
							__Spr565To1555Copy((unsigned short**)&pSrcBuf, &pDstBuf, nLoopCount);
						}
						
/*						while(nLoopCount > 0)
						{
							unsigned short A = 0;
							unsigned short R = 0;
							unsigned short G = 0;
							unsigned short B = 0;
							unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							RGB_FROM_RGB565( usnSrcClr, R, G, B )

							unsigned short colours;
							RGB1555_FROM_ARGB( colours,A, R, G, B )
							*pDstBuf++ = colours;
							nLoopCount--;
						}*/
					}
					else
					{
						pDstBuf += nLoopCount;
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
					__Spr565To1555Copy((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
/*					while(nSameCountLoop > 0)
					{
						unsigned short A = 0;
						unsigned short R = 0;
						unsigned short G = 0;
						unsigned short B = 0;
						unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
						pSrcBuf+=2;
						RGB_FROM_RGB565( usnSrcClr, R, G, B )
						unsigned short colours;
						RGB1555_FROM_ARGB( colours,A, R, G, B )
						*pDstBuf++ = colours;
						nSameCountLoop--;
					}*/
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

		return;
	}

	// 对绘制区域进行裁剪
	KRClipperInfo Clipper;
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;

	long nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;
	long nBuffNextLine;

	__asm
	{
		//ebx = nPitch = nBufferWidth * 2
		mov		ebx, nBufferWidth
		add		ebx, ebx

		//使edi指向buffer绘制起点edi = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
		mov		edi, pBuffer
		mov		eax, Clipper.y
		mul		ebx
		mov		edx, Clipper.x
		add		eax, edx
		add		eax, edx
		add		edi, eax

		//nBuffNextLine = nPitch - Clipper.width * 2;
		mov		edx, Clipper.width
		add		edx, edx
		sub		ebx, edx
		mov		nBuffNextLine, ebx

		//使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		esi, pSpr
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
				mov		edx, Clipper.width
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
						sub		edx, eax
						mov		ecx, eax
						mov     ebx, pPalette
						_DrawFullLineSection_CopyPixel_:
						{
							copy_pixel_use_eax_1555
							loop	_DrawFullLineSection_CopyPixel_
						}
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
			mov		edx, Clipper.width
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
				mov     ebx, pPalette

				_DrawPartLineSection_CopyPixel_:
				{
					copy_pixel_use_eax_1555
					loop	_DrawPartLineSection_CopyPixel_
				}
				jmp		_DrawPartLineSection_LineLocal_
			}

			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				mov		ecx, eax
				add		eax, edx
				mov     ebx, pPalette
				_DrawPartLineSection_CopyPixel_Part_:
				{
					copy_pixel_use_eax_1555
					loop	_DrawPartLineSection_CopyPixel_Part_
				}
			
				dec		Clipper.height
				jz		_EXIT_WAY_
				neg		edx
				mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
				jmp		_DrawPartLineSection_LineSkip_
			}
		}
				_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			mov		edx, Clipper.width
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
				mov     ebx, pPalette
				_DrawPartLineSection_SkipLeft_CopyPixel_:
				{
					copy_pixel_use_eax_1555
					loop	_DrawPartLineSection_SkipLeft_CopyPixel_
				}
				or		edx, edx
				jnz		_DrawPartLineSection_SkipLeft_LineLocal_
				dec		Clipper.height
				jg		_DrawPartLineSection_SkipLeft_LineSkip_
				jmp		_EXIT_WAY_
			}
		}

		_DrawPartLineSection_SkipRight_Line_:
		{
			mov		edx, Clipper.width
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
				mov     ebx, pPalette
				_DrawPartLineSection_SkipRight_CopyPixel_:
				{
					copy_pixel_use_eax_1555
					loop	_DrawPartLineSection_SkipRight_CopyPixel_
				}
				jmp		_DrawPartLineSection_SkipRight_LineLocal_
			}
			_DrawPartLineSection_SkipRight_LineLocal_Alpha_Part_:
			{
				add		eax, edx
				mov		ecx, eax
				mov     ebx, pPalette
				_DrawPartLineSection_SkipRight_CopyPixel_Part_:
				{
					copy_pixel_use_eax_1555
					loop	_DrawPartLineSection_SkipRight_CopyPixel_Part_
				}
				neg		edx
				mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
				dec		Clipper.height
				jg		_DrawPartLineSection_SkipRight_LineSkip_
				jmp		_EXIT_WAY_
			}
		}
		_EXIT_WAY_:
	}
}

//功能：带alpha计算地复制spr图形到缓冲区
void RIO_CopySprToBufferAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight,
					 int nX, int nY, int nPixelBit)
{

	if ( nSprHeight > 512 || nSprWidth > 512 )
	{
		return;
	}
	_ASSERT(pSpr && pBuffer && pPalette);
	if ( nPixelBit == 16 )
	{
		// Get the real draw map
		KRClipperInfo tagClipper;	
		if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &tagClipper) == 0)
			return;

		// Get the screen back buffer
		int nBackBufferWidth = 0;

		unsigned char  *pSrcBuf = (unsigned char *)pSpr;
		unsigned short *pDstBuf = (unsigned short *)pBuffer;
		pDstBuf += tagClipper.y * nBufferWidth + tagClipper.x;
		int nBackBufLinePixel = nBufferWidth;

		int nSameCount = 0;
		int nSameAlpha = 0;
		// Skip top clip spr data
		int nClipSkip = nSprWidth * tagClipper.top;
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
				int nDSameAlpha = 255 - nSameAlpha;
				nCurLinePos += nSameCount;
				int nLoopCount = nCurLinePos - nCurUnitEndPos; 
				if ( nLoopCount > 0)
				{
					if(nSameAlpha > 0 )
					{
						pSrcBuf += (nSameCount - nLoopCount)*2;
						if ( nSameAlpha == 255 )
						{
							if ( nLoopCount > tagClipper.width )
							{
								__SprReplaceCopy((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width);
								pSrcBuf += ((nLoopCount - tagClipper.width)*2);
							}
							else
							{
								__SprReplaceCopy((unsigned short**)&pSrcBuf, &pDstBuf, nLoopCount);
							}

							
/*							while(nLoopCount > 0)
							{
								*pDstBuf++ = *((unsigned short*)pSrcBuf);
								pSrcBuf+=2;
								nLoopCount--;
							}*/
						}
						else
						{
							if ( nLoopCount > tagClipper.width )
							{
								__SprAlphaCopy((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width, nSameAlpha);
								pSrcBuf += ((nLoopCount - tagClipper.width)*2);
							}
							else
							{
								__SprAlphaCopy((unsigned short**)&pSrcBuf, &pDstBuf, nLoopCount, nSameAlpha);
							}
							
/*							while(nLoopCount > 0)
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
								nLoopCount--;
							}*/
						}
					}
					else
					{
						pDstBuf += nLoopCount;
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
				int nDSameAlpha = 255 - nSameAlpha;
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
						__SprReplaceCopy((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
/*						while(nSameCountLoop > 0)
						{
							*pDstBuf++ = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							nSameCountLoop--;
						}*/
					}
					else
					{
						__SprAlphaCopy((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop, nSameAlpha);
/*						while(nSameCountLoop > 0)
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

		return;
	}
	// 对绘制区域进行裁剪
	KRClipperInfo Clipper;	
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;

	// pBuffer指向屏幕起点的偏移位置 (以字节计)
	long nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;
	int	 nBuffNextLine, nAlpha;

	__asm
	{
		//ebx = nPitch = nBufferWidth * 2
		mov		ebx, nBufferWidth
		add		ebx, ebx

		//使edi指向buffer绘制起点edi = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
		mov		edi, pBuffer
		mov		eax, Clipper.y
		mul		ebx
		mov		edx, Clipper.x
		add		eax, edx
		add		eax, edx
		add		edi, eax

		//nBuffNextLine = nPitch - Clipper.width * 2;
		mov		edx, Clipper.width
		add		edx, edx
		sub		ebx, edx
		mov		nBuffNextLine, ebx

		//使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		esi, pSpr
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
				mov		edx, Clipper.width
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
						sub		edx, eax
						mov		ecx, eax

						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							mov     ebx, pPalette
							_DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax
								loop	_DrawFullLineSection_CopyPixel_
							}
							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							push	edx
							shr		ebx, 3
							mov		nAlpha, ebx
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_alpha_use_eabdx
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							pop		edx
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
			mov		edx, Clipper.width
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
					mov     ebx, pPalette
					_DrawPartLineSection_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_
					}
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_HalfAlphaPixel_
					}
					pop		edx
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				mov		ecx, eax
				add		eax, edx
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					mov     ebx, pPalette
					_DrawPartLineSection_CopyPixel_Part_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_CopyPixel_Part_
					}
			
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
					mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_HalfAlphaPixel_Part_
					}
					pop		edx
					neg		edx
					mov		ebx, 128
					dec		Clipper.height
					jg		_DrawPartLineSection_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			mov		edx, Clipper.width
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
					mov     ebx, pPalette
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
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_SkipLeft_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipLeft_HalfAlphaPixel_
					}
					pop		edx
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
			mov		edx, Clipper.width
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
					mov     ebx, pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_:
					{
						copy_pixel_use_eax
						loop	_DrawPartLineSection_SkipRight_CopyPixel_
					}
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_
					}
					pop		edx
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
					mov     ebx, pPalette
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
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_
					}
					pop		edx
					neg		edx
					mov		ebx, 128
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_EXIT_WAY_
				}
			}
		}
		_EXIT_WAY_:
	}
}
//功能：带alpha计算地复制spr图形到带透明模式的缓冲区
void RIO_CopySprToAlphaBufferAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight,
					 int nX, int nY, int nPixelBit)
{
	if ( nPixelBit == 16 )
	{
		// Get the real draw map
		KRClipperInfo tagClipper;	
		if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &tagClipper) == 0)
			return;

		// Get the screen back buffer
		int nBackBufferWidth = 0;

		unsigned char  *pSrcBuf = (unsigned char *)pSpr;
		unsigned short *pDstBuf = (unsigned short *)pBuffer;
		pDstBuf += tagClipper.y * nBufferWidth + tagClipper.x;
		int nBackBufLinePixel = nBufferWidth;

		int nSameCount = 0;
		int nSameAlpha = 0;
		// Skip top clip spr data
		int nClipSkip = nSprWidth * tagClipper.top;
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
				int nDSameAlpha = 255 - nSameAlpha;
				nCurLinePos += nSameCount;
				int nLoopCount = nCurLinePos - nCurUnitEndPos; 
				if ( nLoopCount > 0)
				{
					if(nSameAlpha > 0 )
					{
						pSrcBuf += (nSameCount - nLoopCount)*2;
						if ( nSameAlpha >= 255 )
						{
							if (nLoopCount > tagClipper.width)
							{
								__Spr565To1555Copy((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width);
								pSrcBuf += ((nLoopCount - tagClipper.width)*2);
							}
							else
							{
								__Spr565To1555Copy((unsigned short**)&pSrcBuf, &pDstBuf, nLoopCount);
							}
							
/*							while(nLoopCount > 0)
							{
								unsigned short A = 0;
								unsigned short R = 0;
								unsigned short G = 0;
								unsigned short B = 0;
								unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
								pSrcBuf+=2;
								RGB_FROM_RGB565( usnSrcClr, R, G, B )

								unsigned short colour1555 = 0;
								RGB1555_FROM_ARGB( colour1555, A, R, G, B )
								*pDstBuf++ = colour1555;
								nLoopCount--;
							}*/
						}
						else
						{
							if (nLoopCount > tagClipper.width)
							{
								__Spr565To1555AlphaCopy((unsigned short**)&pSrcBuf, &pDstBuf, tagClipper.width, nSameAlpha);
								pSrcBuf += ((nLoopCount - tagClipper.width)*2);
							}
							else
							{
								__Spr565To1555AlphaCopy((unsigned short**)&pSrcBuf, &pDstBuf, nLoopCount, nSameAlpha);
							}
							
/*							while(nLoopCount > 0)
							{
								unsigned short usnSrcR = 0;
								unsigned short usnSrcG = 0;
								unsigned short usnSrcB = 0;
								unsigned short colour1555 = *((unsigned short*)pSrcBuf);
								pSrcBuf+=2;
								RGB_FROM_RGB565( colour1555, usnSrcR, usnSrcG, usnSrcB )

								unsigned short  usnSrcClr = 0;
								RGB555_FROM_RGB( usnSrcClr,usnSrcR, usnSrcG, usnSrcB )

								usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
								usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
								usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

								unsigned short usnDstClr = *pDstBuf;
								unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
								unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
 								unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
								*pDstBuf++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB & 0x7fff;
								
								nLoopCount--;
							}*/
						}
					}
					else
					{
						pDstBuf += nLoopCount;
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
				int nDSameAlpha = 255 - nSameAlpha;
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
					if ( nSameAlpha >= 255 )
					{
						__Spr565To1555Copy((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop);
/*						while(nSameCountLoop > 0)
						{
							unsigned short A = 0;
							unsigned short R = 0;
							unsigned short G = 0;
							unsigned short B = 0;
							unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							RGB_FROM_RGB565( usnSrcClr, R, G, B )

							unsigned short colour1555 = 0;
							RGB1555_FROM_ARGB( colour1555, A, R, G, B )
							*pDstBuf++ = colour1555;
							nSameCountLoop--;
						}*/
					}
					else
					{
						__Spr565To1555AlphaCopy((unsigned short**)&pSrcBuf, &pDstBuf, nSameCountLoop, nSameAlpha);
/*						while(nSameCountLoop > 0)
						{
							unsigned short usnSrcR = 0;
							unsigned short usnSrcG = 0;
							unsigned short usnSrcB = 0;
							unsigned short usnSrcClr = *((unsigned short*)pSrcBuf);
							pSrcBuf+=2;
							RGB_FROM_RGB565( usnSrcClr, usnSrcR, usnSrcG, usnSrcB )

							usnSrcClr = 0xffff;
							RGB555_FROM_RGB( usnSrcClr,usnSrcR, usnSrcG, usnSrcB )

							usnSrcR = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F);
							usnSrcG = nSameAlpha * ((usnSrcClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F);
							usnSrcB = nSameAlpha * (usnSrcClr & 0x001F);

							unsigned short usnDstClr = *pDstBuf;
							unsigned short usnR = (usnSrcR + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_RCOLOR) & 0x001F)) >> 8;
							unsigned short usnG = (usnSrcG + nDSameAlpha * ((usnDstClr >> SHIFTNUM_RGB555_GCOLOR) & 0x001F)) >> 8;
 							unsigned short usnB = (usnSrcB + nDSameAlpha * (usnDstClr & 0x001F)) >> 8;
							*pDstBuf++ = (usnR << SHIFTNUM_RGB555_RCOLOR) | (usnG << SHIFTNUM_RGB555_GCOLOR) | usnB  & 0x7fff;
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

		return;
	}
	_ASSERT(pSpr && pBuffer && pPalette);
	// 对绘制区域进行裁剪
	KRClipperInfo Clipper;	
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;

	// pBuffer指向屏幕起点的偏移位置 (以字节计)
	long nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;
	int	 nBuffNextLine, nAlpha;

	__asm
	{
		//ebx = nPitch = nBufferWidth * 2
		mov		ebx, nBufferWidth
		add		ebx, ebx

		//使edi指向buffer绘制起点edi = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
		mov		edi, pBuffer
		mov		eax, Clipper.y
		mul		ebx
		mov		edx, Clipper.x
		add		eax, edx
		add		eax, edx
		add		edi, eax

		//nBuffNextLine = nPitch - Clipper.width * 2;
		mov		edx, Clipper.width
		add		edx, edx
		sub		ebx, edx
		mov		nBuffNextLine, ebx

		//使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		esi, pSpr
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
				mov		edx, Clipper.width
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
						sub		edx, eax
						mov		ecx, eax

						cmp		ebx, 255
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							mov     ebx, pPalette
							_DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax_1555
								loop	_DrawFullLineSection_CopyPixel_
							}
							or		edx, edx
							jnz		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							push	edx
							shr		ebx, 3
							mov		nAlpha, ebx
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_alpha_use_eabdx_1555
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							pop		edx
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
			mov		edx, Clipper.width
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
					mov     ebx, pPalette
					_DrawPartLineSection_CopyPixel_:
					{
						copy_pixel_use_eax_1555
						loop	_DrawPartLineSection_CopyPixel_
					}
					jmp		_DrawPartLineSection_LineLocal_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_:
				{
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx_1555
						loop	_DrawPartLineSection_HalfAlphaPixel_
					}
					pop		edx
					jmp		_DrawPartLineSection_LineLocal_
				}
			}
			_DrawPartLineSection_LineLocal_Alpha_Part_:
			{
				mov		ecx, eax
				add		eax, edx
				cmp		ebx, 255
				jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
					
				//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
				{
					mov     ebx, pPalette
					_DrawPartLineSection_CopyPixel_Part_:
					{
						copy_pixel_use_eax_1555
						loop	_DrawPartLineSection_CopyPixel_Part_
					}
			
					dec		Clipper.height
					jz		_EXIT_WAY_
					neg		edx
					mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_DrawPartLineSection_LineSkip_
				}
				
				_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
				{
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx_1555
						loop	_DrawPartLineSection_HalfAlphaPixel_Part_
					}
					pop		edx
					neg		edx
					mov		ebx, 128
					dec		Clipper.height
					jg		_DrawPartLineSection_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_EXIT_WAY_
				}
			}
		}

		_DrawPartLineSection_SkipLeft_Line_:
		{
			mov		eax, edx
			mov		edx, Clipper.width
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
					mov     ebx, pPalette
					_DrawPartLineSection_SkipLeft_CopyPixel_:
					{
						copy_pixel_use_eax_1555
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
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_SkipLeft_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx_1555
						loop	_DrawPartLineSection_SkipLeft_HalfAlphaPixel_
					}
					pop		edx
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
			mov		edx, Clipper.width
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
					mov     ebx, pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_:
					{
						copy_pixel_use_eax_1555
						loop	_DrawPartLineSection_SkipRight_CopyPixel_
					}
					jmp		_DrawPartLineSection_SkipRight_LineLocal_
				}
				
				_DrawPartLineSection_SkipRight_LineLocal_HalfAlpha_:
				{
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_:
					{
						mix_2_pixel_color_alpha_use_eabdx_1555
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_
					}
					pop		edx
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
					mov     ebx, pPalette
					_DrawPartLineSection_SkipRight_CopyPixel_Part_:
					{
						copy_pixel_use_eax_1555
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
					push	edx
					shr		ebx, 3
					mov		nAlpha, ebx
					_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_:
					{
						mix_2_pixel_color_alpha_use_eabdx_1555
						loop	_DrawPartLineSection_SkipRight_HalfAlphaPixel_Part_
					}
					pop		edx
					neg		edx
					mov		ebx, 128
					dec		Clipper.height
					jg		_DrawPartLineSection_SkipRight_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					jmp		_EXIT_WAY_
				}
			}
		}
		_EXIT_WAY_:
	}
}




//功能：三阶alpha形式地复制spr图形到缓冲区
void RIO_CopyBitmap16ToBuffer(void* pBitmap, int nBmpWidth, int nBmpHeight,
					 void* pBuffer, int nBufferWidth, int nBufferHeight,
					 int nX, int nY)
{
	// 对绘制区域进行裁剪
	KRClipperInfo Clipper;
	_ASSERT(pBitmap && pBuffer);
	if (RIO_ClipCopyRect(nX, nY, nBmpWidth, nBmpHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;
	long nBitmapOffset, nBuffNextLine;

	// 绘制函数的汇编代码
	__asm
	{
		//ebx = nPitch = nBufferWidth * 2
		mov		ebx, nBufferWidth
		add		ebx, ebx

		//使edi指向buffer绘制起点edi = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
		mov		edi, pBuffer
		mov		eax, Clipper.y
		mul		ebx
		mov		edx, Clipper.x
		add		eax, edx
		add		eax, edx
		add		edi, eax

		//esi指向图块数据绘制的开始位置
		mov		esi, pBitmap
		mov		eax, Clipper.top
		mul		nBmpWidth
		add		eax, Clipper.left
		add		eax, eax
		add		esi, eax

		//计算缓冲区下一行的偏移=nPitch - Clipper.width * 2;
		mov		edx, Clipper.width
		add		edx, edx
		sub		ebx, edx
		mov		nBuffNextLine, ebx

		// 计算位图下一行的偏移
		mov		eax, nBmpWidth
		sub		eax, Clipper.width
		add		eax, eax

		mov		ebx, Clipper.height
		mov		edx, Clipper.width
		mov		ecx, edi
		sub		ecx, esi
		test	ecx, 2
		jz		_4BYTE_ALIGN_COPY_

		//_2BYTE_ALIGN_COPY_:
		{
			_2BYTE_ALIGN_COPY_LINE_:
			{
				mov		ecx, edx
				rep		movsw
				add		edi, nBuffNextLine
				add     esi, eax
				dec		ebx
				jne		_2BYTE_ALIGN_COPY_LINE_
				jmp		_EXIT_WAY_
			}
		}

		_4BYTE_ALIGN_COPY_:
		{
			mov		nBitmapOffset, eax
			_4BYTE_ALIGN_COPY_LINE_:
			{
				mov		eax, edx
				mov		ecx, edi
				shr		ecx, 1
				and		ecx, 1
				sub		eax, ecx
				rep		movsw
				mov		ecx, eax
				shr		ecx, 1
				rep		movsd
				adc		ecx, ecx
				rep		movsw
				add		edi, nBuffNextLine
				add     esi, nBitmapOffset
				dec		ebx
				jne		_4BYTE_ALIGN_COPY_LINE_
				jmp		_EXIT_WAY_
			}
		}
	_EXIT_WAY_:
	}
}

//功能：覆盖形式复制16位位图到缓冲区
void RIO_CopySprToBuffer3LevelAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY)
{
	_ASSERT(pSpr && pBuffer && pPalette);
	// 对绘制区域进行裁剪
	KRClipperInfo Clipper;	
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;

	int	nPitch = nBufferWidth + nBufferWidth;
	// pBuffer指向屏幕起点的偏移位置 (以字节计)
	pBuffer = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
	long nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	long nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;

	__asm
	{
		//使edi指向buffer绘制起点,使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		edi, pBuffer
		mov		esi, pSpr

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
				mov		edx, Clipper.width
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
						push	eax
						mov		ecx, eax

						cmp		ebx, 200
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							mov     ebx, pPalette
							_DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax
								loop	_DrawFullLineSection_CopyPixel_
							}

							pop		eax
							sub		edx, eax
							jg		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							push	edx							
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_use_eabdx
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							pop		edx
							pop		eax
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
				mov		edx, Clipper.width
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

					push	eax
					mov		ecx, eax
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_:
					{
						mov     ebx, pPalette
						_DrawPartLineSection_CopyPixel_:
						{
							copy_pixel_use_eax
							loop	_DrawPartLineSection_CopyPixel_
						}						
						pop		eax
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_:
					{
						push	edx
						_DrawPartLineSection_HalfAlphaPixel_:
						{
							mix_2_pixel_color_use_eabdx
							loop	_DrawPartLineSection_HalfAlphaPixel_
						}
						pop		edx
						pop		eax
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
				}

				_DrawPartLineSection_LineLocal_Alpha_Part_:
				{
					push	eax
					mov		ecx, edx
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
					{
						mov     ebx, pPalette
						_DrawPartLineSection_CopyPixel_Part_:
						{
							copy_pixel_use_eax
							loop	_DrawPartLineSection_CopyPixel_Part_
						}						
						pop		eax
				
						dec		Clipper.height
						jz		_EXIT_WAY_

						sub		eax, edx
						mov		edx, eax
						mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
						jmp		_DrawPartLineSection_LineSkip_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
					{
						push	edx
						_DrawPartLineSection_HalfAlphaPixel_Part_:
						{
							mix_2_pixel_color_use_eabdx
							loop	_DrawPartLineSection_HalfAlphaPixel_Part_
						}
						pop		edx
						pop		eax
						dec		Clipper.height
						jz		_EXIT_WAY_
						sub		eax, edx
						mov		edx, eax
						mov		ebx, 128
						jmp		_DrawPartLineSection_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					}
				}
			}
		}
		_EXIT_WAY_:
	}
}
//功能：覆盖形式复制16位位图到带透明模式的缓冲区
void RIO_CopySprToAlphaBuffer3LevelAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
					 void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY)
{
	_ASSERT(pSpr && pBuffer && pPalette);
	// 对绘制区域进行裁剪
	KRClipperInfo Clipper;
	if (RIO_ClipCopyRect(nX, nY, nSprWidth, nSprHeight, nBufferWidth, nBufferHeight, &Clipper) == 0)
		return;

	int	nPitch = nBufferWidth + nBufferWidth;
	// pBuffer指向屏幕起点的偏移位置 (以字节计)
	pBuffer = (char*)pBuffer + Clipper.y * nPitch + Clipper.x * 2;
	long nBuffNextLine = nPitch - Clipper.width * 2;// next line add
	long nSprSkip = nSprWidth * Clipper.top + Clipper.left;
	long nSprSkipPerLine = Clipper.left + Clipper.right;

	__asm
	{
		//使edi指向buffer绘制起点,使esi指向图块数据起点,(跳过nSprSkip个像点的图形数据)
		mov		edi, pBuffer
		mov		esi, pSpr

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
				mov		edx, Clipper.width
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
						push	eax
						mov		ecx, eax

						cmp		ebx, 200
						jl		_DrawFullLineSection_LineLocal_HalfAlpha_

						//_DrawFullLineSection_LineLocal_DirectCopy_:
						{
							mov     ebx, pPalette
							_DrawFullLineSection_CopyPixel_:
							{
								copy_pixel_use_eax_1555
								loop	_DrawFullLineSection_CopyPixel_
							}

							pop		eax
							sub		edx, eax
							jg		_DrawFullLineSection_LineLocal_
	
							add		edi, nBuffNextLine
							dec		Clipper.height
							jnz		_DrawFullLineSection_Line_
							jmp		_EXIT_WAY_
						}

						_DrawFullLineSection_LineLocal_HalfAlpha_:
						{
							push	edx							
							_DrawFullLineSection_HalfAlphaPixel_:
							{
								mix_2_pixel_color_use_eabdx_1555
								loop	_DrawFullLineSection_HalfAlphaPixel_
							}
							pop		edx
							pop		eax
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
				mov		edx, Clipper.width
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

					push	eax
					mov		ecx, eax
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_:
					{
						mov     ebx, pPalette
						_DrawPartLineSection_CopyPixel_:
						{
							copy_pixel_use_eax_1555
							loop	_DrawPartLineSection_CopyPixel_
						}						
						pop		eax
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_:
					{
						push	edx
						_DrawPartLineSection_HalfAlphaPixel_:
						{
							mix_2_pixel_color_use_eabdx_1555
							loop	_DrawPartLineSection_HalfAlphaPixel_
						}
						pop		edx
						pop		eax
						sub		edx, eax
						jmp		_DrawPartLineSection_LineLocal_
					}
				}

				_DrawPartLineSection_LineLocal_Alpha_Part_:
				{
					push	eax
					mov		ecx, edx
					cmp		ebx, 200
					jl		_DrawPartLineSection_LineLocal_HalfAlpha_Part_
						
					//_DrawPartLineSection_LineLocal_DirectCopy_Part_:
					{
						mov     ebx, pPalette
						_DrawPartLineSection_CopyPixel_Part_:
						{
							copy_pixel_use_eax_1555
							loop	_DrawPartLineSection_CopyPixel_Part_
						}						
						pop		eax
				
						dec		Clipper.height
						jz		_EXIT_WAY_

						sub		eax, edx
						mov		edx, eax
						mov		ebx, 255	//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
						jmp		_DrawPartLineSection_LineSkip_
					}
					
					_DrawPartLineSection_LineLocal_HalfAlpha_Part_:
					{
						push	edx
						_DrawPartLineSection_HalfAlphaPixel_Part_:
						{
							mix_2_pixel_color_use_eabdx_1555
							loop	_DrawPartLineSection_HalfAlphaPixel_Part_
						}
						pop		edx
						pop		eax
						dec		Clipper.height
						jz		_EXIT_WAY_
						sub		eax, edx
						mov		edx, eax
						mov		ebx, 128
						jmp		_DrawPartLineSection_LineSkip_//如果想要确切的原ebx(alpha)值可以在前头push ebx，此处pop获得
					}
				}
			}
		}
		_EXIT_WAY_:
	}
}




//文件名转化为字符串
unsigned ImageNameToId(const char* pszName)
{
	//															-哩函数....,偶其实好委哇嘎-
	unsigned long uId = 0;
	if (pszName && pszName[0])
	{
		for (int i = 1; (*pszName); pszName++)
		{
			if ((*pszName) < 'A' || (*pszName) > 'Z')
				uId = (uId + i * (*pszName)) % 0x8000000b * 0xffffffef;
			else						//中文字符的后字节亦可能被转换，人为地加大了重码的概率■
				uId = (uId + i * ((*pszName) + 0x20)) % 0x8000000b * 0xffffffef;
		}
		uId ^= 0x12345678;
	}
	return uId;
}


// --> Rocker Edit Start 2005/10/19
/*
pDstBuffer: 5551每象素格式的bitmap buffer
pSrcBitmap: 5551每象素格式的bitmap buffer
*/
void RIO_BltBitmap555ToBuffer(void* pSrcBitmap, int nSrcPitch, int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight,
							  void* pDstBuffer, int nDstPitch, int nDstX, int nDstY)
{
	if ((!pSrcBitmap)||(!pDstBuffer))
		return;

	char* pD = (char*)pDstBuffer + nDstY * nDstPitch + nDstX * 2;
	char* pS = (char*)pSrcBitmap + nSrcY * nSrcPitch + nSrcX * 2;

	for (int i=0; i<nSrcHeight; i++)
	{
		short* pDTemp = (short*)pD;
		short* pSTemp = (short*)pS;

		for (int j=0; i<nSrcWidth; j++)
		{
			if (*pSTemp & 0x8000)
				*pDTemp = *pSTemp;

			pDTemp++;
			pSTemp++;
		}

		pD += nDstPitch;
		pS += nSrcPitch;
	}
}

/*
pSrcAlpha: 8 bit每象素格式的alpha buffer
覆盖的方法改变pDstBuffer的象素alpha
*/
void RIO_BltAlphaToBuffer(void* pSrcAlpha, int nSrcPitch, int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight, 
							  void* pDstBuffer, int nDstPitch, int nDstX, int nDstY)
{
	if ((!pSrcAlpha)||(!pDstBuffer))
		return;

	char* pD = (char*)pDstBuffer + nDstY * nDstPitch + nDstX * 2;
	char* pS = (char*)pSrcAlpha + nSrcY * nSrcPitch + nSrcX;

	for (int i=0; i<nSrcHeight; i++)
	{
		short* pDTemp = (short*)pD;
		char* pSTemp = (char*)pS;

		for (int j=0; i<nSrcWidth; j++)
		{
			if (*pSTemp == 0)
				*pDTemp &= 0x7FFF;
			else
				*pDTemp |= 0x8000;

			pDTemp++;
			pSTemp++;
		}

		pD += nDstPitch;
		pS += nSrcPitch;
	}
}
// <-- Rocker End
