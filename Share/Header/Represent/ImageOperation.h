/*****************************************************************************************
//  图形到内存区域的操作
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-11
------------------------------------------------------------------------------------------
*****************************************************************************************/

#pragma once

//设置16位位图使用的格式565或555
void RIO_Set16BitImageFormat(int b565);
//覆盖形式复制spr图形到缓冲区，不做alpha计算。
void RIO_CopySprToBuffer(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
						 void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nPixelBit);
void RIO_CopySprToAlphaBuffer(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
							  void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nPixelBit);
//带alpha计算地复制spr图形到缓冲区
void RIO_CopySprToBufferAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
							  void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nPixelBit);
void RIO_CopySprToAlphaBufferAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
								   void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY, int nPixelBit);
//三阶alpha形式地复制spr图形到缓冲区
void RIO_CopySprToBuffer3LevelAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
									void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY);
void RIO_CopySprToAlphaBuffer3LevelAlpha(void* pSpr, int nSprWidth, int nSprHeight, void* pPalette,
										 void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY);
//覆盖形式复制16位位图到缓冲区
void RIO_CopyBitmap16ToBuffer(void* pBitmap, int nBmpWidth, int nBmpHeight,
							  void* pBuffer, int nBufferWidth, int nBufferHeight, int nX, int nY);
//文件名转化为字符串
unsigned int ImageNameToId(const char* pszName);

// --> Rocker Edit Start 2005/10/19
/*
pDstBuffer: 5551每象素格式的bitmap buffer
pSrcBitmap: 5551每象素格式的bitmap buffer
*/
void RIO_BltBitmap555ToBuffer(void* pSrcBitmap, int nSrcPitch, int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight,
							  void* pDstBuffer, int nDstPitch, int nDstX, int nDstY);

/*
pSrcAlpha: 8 bit每象素格式的alpha buffer
覆盖的方法改变pDstBuffer的象素alpha
*/
void RIO_BltAlphaToBuffer(void* pSrcAlpha, int nSrcPitch, int nSrcX, int nSrcY, int nSrcWidth, int nSrcHeight, 
							  void* pDstBuffer, int nDstPitch, int nDstX, int nDstY);
// <-- Rocker End

inline void __SprReplaceCopy(unsigned short **ppSrcBuf, 
							 unsigned short **ppDstBuf, 
							 int nSameCount)
{
	unsigned short *pSrcBuf = *ppSrcBuf;
	unsigned short *pDstBuf = *ppDstBuf;

	__asm
	{
		MOV ESI, pSrcBuf; // source buffer
		MOV EDI, pDstBuf; // target buffer
		XOR EBX, EBX;
		PXOR MM6, MM6;

		MOV ECX, [nSameCount];
		__CurrentSameBegin:
		CMP ECX, 0;
		JLE __CurrentSameExit;
//		MOVQ MM6, [ESI];	// storage source color这里移动是危险的，必须先判断

		CMP ECX, 4;
		JL __DoHalfWrite;
		//四个象素//
		SUB ECX, 4;
		MOVQ MM6,[ESI];
		MOVQ [EDI], MM6;
		ADD EDI, 8;
		ADD ESI, 8;
		JMP __DoFinishWrite;
		__DoHalfWrite:
		MOV EBX, ECX;
		XOR ECX, ECX;
		CMP EBX, 2;
		JL __DoLeftWrite;
		//两个象素
		MOVD MM6,[ESI];
		MOVD [EDI], MM6;
		ADD EDI, 4;
		ADD ESI, 4;
		SUB EBX, 2;
		PSRLQ MM6, 32;
		__DoLeftWrite:
		CMP EBX, 0;
		JLE __DoFinishWrite;
		//一个象素
		
		XOR EAX,EAX
		MOV AX,WORD PTR[ESI]
	//	MOVD EAX, MM6;
		MOV [EDI], AX;
		ADD EDI, 2;
		ADD ESI, 2;
		__DoFinishWrite:

		JMP __CurrentSameBegin;
		__CurrentSameExit:
		MOV [pDstBuf], EDI;
		MOV [pSrcBuf], ESI;
		EMMS;
	}

	*ppSrcBuf = pSrcBuf;
	*ppDstBuf = pDstBuf;
}

inline void __Spr565To1555Copy(unsigned short **ppSrcBuf, 
							   unsigned short **ppDstBuf, 
							   int nSameCount)
{
	unsigned short *pSrcBuf = *ppSrcBuf;
	unsigned short *pDstBuf = *ppDstBuf;

	__asm
	{
		MOV EAX, 0x001F001F;
		MOVD MM0, EAX;
		PUNPCKLDQ MM0, MM0;
		MOV EAX, 0xFFE0FFE0;
		MOVD MM1, EAX;
		PUNPCKLDQ MM1, MM1;

		MOV ESI, pSrcBuf; // source buffer
		MOV EDI, pDstBuf; // target buffer
		XOR EBX, EBX;
		PXOR MM6, MM6;

		MOV ECX, [nSameCount];
		__CurrentSameBegin:
		CMP ECX, 0;
		JLE __CurrentSameExit;
		CMP ECX,4
		JGE _Process4Pixel
		CMP ECX,3
		JGE _Process3Pixel
		CMP ECX,2
		JGE _Process2Pixel
		//一个像素//
		XOR EDX,EDX
		MOV dx,WORD PTR[ESI]
		MOVD MM6,EDX
		JMP _Process
        _Process2Pixel:
		//两个像素
		MOVD MM6,[ESI]
		JMP _Process
        _Process3Pixel:
		MOVD MM6,[ESI]
		ADD ESI,4
		XOR EDX,EDX
		MOV dx,WORD PTR[ESI]
		MOVD MM7,EDX
		PSLLQ MM7,32
		POR MM6,MM7
		SUB ESI,4
		JMP _Process
        _Process4Pixel:
		//四个像素
		MOVQ MM6, [ESI];// storage source color
        _Process:
		MOVQ MM7, MM6;
		PSRLW MM7, 1;
		PAND MM6, MM0;
		PAND MM7, MM1;
		POR MM6, MM7;

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
		MOV [pSrcBuf], ESI;
		EMMS;
	}

	*ppSrcBuf = pSrcBuf;
	*ppDstBuf = pDstBuf;
}

inline void __SprAlphaCopy(unsigned short **ppSrcBuf, 
						   unsigned short **ppDstBuf, 
						   int nSameCount, 
						    int nSameAlpha)
{
	unsigned short *pSrcBuf = *ppSrcBuf;
	unsigned short *pDstBuf = *ppDstBuf;

	__int64 n64ClrMask1F = 0x001F001F001F001F;
	__int64 n64ClrMask3F = 0x003F003F003F003F;
	int nSameAlphaD = 255 - nSameAlpha;
	__asm
	{
		MOV ESI, pSrcBuf; // source buffer
		MOV EDI, pDstBuf; // target buffer
		XOR EBX, EBX;
		PXOR MM6, MM6;

		__CurrentSameBegin:
		MOV EAX, [nSameCount];
		CMP EAX, 0;
		JLE __CurrentSameExit;
		CMP EAX,4
		JGE _PreProcess4Pixel
		CMP EAX,3
		JGE _PreProcess3Pixel
		CMP EAX,2
		JGE _PreProcess2Pixel
		//一个像素预处理
		XOR EBX,EBX
		MOV BX,WORD PTR[ESI]
		MOVD MM6,EBX
		MOV EBX,1
		JMP __EnoughClrIndex
        _PreProcess2Pixel:
		//2个像素
		MOVD MM6,[ESI]
		MOV EBX,2
		JMP __EnoughClrIndex
        _PreProcess3Pixel:
		//三个像素
		MOVD MM6,[ESI]
		ADD ESI,4
		XOR EBX,EBX
		MOV BX,WORD PTR[ESI]
		MOVD MM7,EBX;
		PSLLQ MM7,32
		POR MM6,MM7
		SUB ESI,4
		MOV EBX,3
		JMP __EnoughClrIndex
		//四个像素
        _PreProcess4Pixel:
		MOVQ MM6,[ESI]
		MOV EBX,4
		JMP __EnoughClrIndex
	/*	MOVQ MM6, [ESI]	// MM6 storage source R color
		MOV EBX, 4;
		CMP EAX, 4;
		JGE __EnoughClrIndex;
		MOV EBX, EAX;*/
		__EnoughClrIndex:
		SUB EAX, EBX;
		MOV [nSameCount], EAX;

		// Process begin
		// {
		MOVQ MM5, MM6;		// MM5 storage source G color
		MOVQ MM4, MM6;		// MM4 storage source B color
		PSRLW MM6, 11;
		PSRLW MM5, 5;
		MOVQ MM0, [n64ClrMask3F];
		PAND MM5, MM0;
		MOVQ MM0, [n64ClrMask1F];
		PAND MM4, MM0;
		MOVD MM0, [nSameAlpha];
		PUNPCKLWD MM0, MM0;
		PUNPCKLDQ MM0, MM0;
		PMULLW MM6, MM0;
		PMULLW MM5, MM0;
		PMULLW MM4, MM0;
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
		ADD ESI, 8;
		JMP __DoFinishWrite;
		__DoHalfWrite:
		CMP EBX, 2;
		JL __DoLeftWrite;
		MOVD [EDI], MM3;
		ADD EDI, 4;
		ADD ESI, 4;
		SUB EBX, 2;
		PSRLQ MM3, 32;
		__DoLeftWrite:
		CMP EBX, 0;
		JLE __DoFinishWrite;
		MOVD EBX, MM3;
		MOV [EDI], BX;
		ADD EDI, 2;
		ADD ESI, 2;
		__DoFinishWrite:
		XOR EBX, EBX;
		PXOR MM6, MM6;
		// }
		// Process finished
		JMP __CurrentSameBegin;
		__CurrentSameExit:
		MOV [pDstBuf], EDI;
		MOV [pSrcBuf], ESI;
		EMMS;
	}

	*ppSrcBuf = pSrcBuf;
	*ppDstBuf = pDstBuf;
}

inline void __Spr565To1555AlphaCopy(unsigned short **ppSrcBuf, 
									unsigned short **ppDstBuf, 
									int nSameCount, 
									int nSameAlpha)
{
	unsigned short *pSrcBuf = *ppSrcBuf;
	unsigned short *pDstBuf = *ppDstBuf;

	__int64 n64ClrMask1F = 0x001F001F001F001F;
	int nSameAlphaD = 255 - nSameAlpha;
	__asm
	{
		MOV ESI, pSrcBuf; // source buffer
		MOV EDI, pDstBuf; // target buffer
		XOR EBX, EBX;
		PXOR MM6, MM6;

		__CurrentSameBegin:
		MOV EAX, [nSameCount];
		CMP EAX, 0;
		JLE __CurrentSameExit;
		CMP EAX,4
		JGE __PreProcess4Pixel
		CMP EAX,3
		JGE __PreProcess3Pixel
		CMP EAX,2
		JGE __PreProcess2Pixel
		//一个像素
		XOR EDX,EDX
		MOV dx,WORD PTR[ESI]
		MOVD MM6,EDX
		MOV EBX,1
		JMP __EnoughClrIndex
        __PreProcess2Pixel://2个//
		MOVD MM6,[ESI]
		MOV EBX,2
		JMP __EnoughClrIndex
        __PreProcess3Pixel:
		//3个
		MOVD MM6,[ESI]//
		ADD ESI,4
		XOR EDX,EDX
		MOV DX,WORD PTR[ESI];
		MOVD MM7,EDX
		PSLLQ MM7,32
		POR MM6,MM7
		SUB ESI,4
		MOV EBX,3
		JMP __EnoughClrIndex
        __PreProcess4Pixel:
		MOVQ MM6,[ESI]
		MOV EBX,4
		JMP __EnoughClrIndex
/*		MOVQ MM6, [ESI];	// MM6 storage source R color
		MOV EBX, 4;
		CMP EAX, 4;
		JGE __EnoughClrIndex;
		MOV EBX, EAX;*/
		__EnoughClrIndex:
		SUB EAX, EBX;
		MOV [nSameCount], EAX;

		// Process begin
		// {
		MOVQ MM5, MM6;		// MM5 storage source G color
		MOVQ MM4, MM6;		// MM4 storage source B color
		PSRLW MM6, 11;
		PSRLW MM5, 6;
		MOVQ MM0, [n64ClrMask1F];
		PAND MM5, MM0;
		PAND MM4, MM0;
		MOVD MM0, [nSameAlpha];
		PUNPCKLWD MM0, MM0;
		PUNPCKLDQ MM0, MM0;
		PMULLW MM6, MM0;
		PMULLW MM5, MM0;
		PMULLW MM4, MM0;
		MOVQ MM3, [EDI];	// MM3 storage target R color
		MOVQ MM2, MM3;		// MM2 storage target G color
		MOVQ MM1, MM3;		// MM1 storage target B color
		PSRLW MM3, 10;
		PSRLW MM2, 5;
		MOVQ MM0, [n64ClrMask1F];
		PAND MM2, MM0;
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
		PSLLW MM3, 10;
		PSLLW MM2, 5;
		POR MM3, MM2;
		POR MM3, MM1;
		// Write back to memory
		CMP EBX, 4;
		JL __DoHalfWrite;
		MOVQ [EDI], MM3;
		ADD EDI, 8;
		ADD ESI, 8;
		JMP __DoFinishWrite;
		__DoHalfWrite:
		CMP EBX, 2;
		JL __DoLeftWrite;
		MOVD [EDI], MM3;
		ADD EDI, 4;
		ADD ESI, 4;
		SUB EBX, 2;
		PSRLQ MM3, 32;
		__DoLeftWrite:
		CMP EBX, 0;
		JLE __DoFinishWrite;
		MOVD EBX, MM3;
		MOV [EDI], BX;
		ADD EDI, 2;
		ADD ESI, 2;
		__DoFinishWrite:
		XOR EBX, EBX;
		PXOR MM6, MM6;
		// }
		// Process finished
		JMP __CurrentSameBegin;
		__CurrentSameExit:
		MOV [pDstBuf], EDI;
		MOV [pSrcBuf], ESI;
		EMMS;
	}

	*ppSrcBuf = pSrcBuf;
	*ppDstBuf = pDstBuf;
}
