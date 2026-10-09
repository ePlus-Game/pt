//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KDrawSpriteAlpha.h
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Modify:	add g_DrawSpriteAlphaEx function by cooler
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KDrawSpriteAlpha_H
#define KDrawSpriteAlpha_H

#define BYTECOUNT_DSTSCREENPIXEL	2
#define CLRMASK_RGB565_RB			0x001F001F
#define CLRMASK_RGB565_G			0x003F003F
#define CLRMASKR_RGB565_R			0x07FF07FF
#define CLRMASKR_RGB565_G			0xF81FF81F
#define CLRMASKR_RGB565_B			0xFFE0FFE0

#define SHIFTNUM_RGB565_RCOLOR		11
#define SHIFTNUM_RGB565_GCOLOR		5

//---------------------------------------------------------------------------
void g_DrawSpriteAlpha(void* node, void* canvas);
void g_DrawSpriteAlpha16(void* node, void* canvas);
///add by render 
//修改结构把alpha表添加进取
void g_DrawSpriteAlphaWidthAlphaTable8(void* node, void* canvas);
void g_DrawSpriteAlphaWidthAlphaTable16(void* node, void* canvas);
//用mmx优化
void g_DrawSpriteAlphaUseMMX8(void* node,void* canvas);
void g_DrawSpriteAlphaUseMMX16(void* node,void* canvas);


void g_DrawSpriteAlpha(void* node, void* canvas, int nExAlpha);
void g_DrawSpriteAlpha16(void* node, void* canvas, int nExAlpha);

void g_DrawSpriteAlphaEx(void* node, void* canvas, int nExAlpha);
void g_DrawSpriteAlphaEx16(void* node, void* canvas, int nExAlpha);

void g_DrawSprite3LevelAlpha(void* node, void* canvas);
////add by zpc
//新式绘制alpha函数，带alpha表//
void g_DrawSpriteAlphaWithTable16(void* node,void* canvas);
void g_DrawSpriteAlphaWithTable8(void* node,void* canvas);
//end//
//---------------------------------------------------------------------------

inline void __Draw16CoreMin(unsigned short **ppSrcBuf, 
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
//		MOVQ MM6, [ESI];	// storage source color先不用移动，判断了再移动，保险

		CMP ECX, 4;
		JL __DoHalfWrite;
		SUB ECX, 4;
		//四个像素一起写
		MOVQ MM6,[ESI]
		MOVQ [EDI], MM6;
		ADD EDI, 8;
		ADD ESI, 8;
		JMP __DoFinishWrite;
		__DoHalfWrite:
		MOV EBX, ECX;
		XOR ECX, ECX;
		CMP EBX, 2;
		JL __DoLeftWrite;
		//两个像素
		MOVD MM6,[ESI]
		MOVD [EDI], MM6;
		ADD EDI, 4;
		ADD ESI, 4;
		SUB EBX, 2;
//		PSRLQ MM6, 32;
		__DoLeftWrite:
		CMP EBX, 0;
		JLE __DoFinishWrite;
		//一个像素
		XOR EAX,EAX
		MOV ax,WORD PTR [ESI]

//		MOVD EAX, MM6;
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

inline void __Draw16Core(unsigned short **ppSrcBuf, 
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
		
		CMP EAX,4
		JGE  _PreDraw4Pixel//计数大于等于4预进行4个像素混合
		CMP EAX ,3
		JGE _PreDraw3Pixel
		CMP EAX,2
		JGE _PreDraw2Pixel//计数大于等于2预进行2个象素混合
		CMP EAX, 0;
		JLE __CurrentSameExit;//要是计数为0，则完成绘制
		//这里混合1个象素
		XOR ECX,ECX
		MOV CX,WORD PTR [ESI]//一个像素
		MOVD MM6,ECX
		MOV  EBX,1
		JMP __EnoughClrIndex
		//////////混合两个像素
        _PreDraw2Pixel:
		MOVD MM6,[ESI]//只移动两个像素
		MOV EBX,2
		JMP __EnoughClrIndex
        _PreDraw3Pixel:
		MOVD MM6,[ESI]
		ADD ESI,4
		XOR ECX,ECX
		MOV CX,WORD PTR[ESI]
		MOVD MM7,ECX
		PSLLQ MM7,32
		POR MM6,MM7
		SUB ESI,4
		MOV EBX,3
		JMP __EnoughClrIndex
        _PreDraw4Pixel:
		MOVQ MM6,[ESI]
		MOV EBX,4
		/*
		MOVQ MM6, [ESI];	// MM6 storage source R color
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
		PSRLW MM6, SHIFTNUM_RGB565_RCOLOR;
		PSRLW MM5, SHIFTNUM_RGB565_GCOLOR;
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
		PSRLW MM3, SHIFTNUM_RGB565_RCOLOR;
		PSRLW MM2, SHIFTNUM_RGB565_GCOLOR;
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
		PSLLW MM3, SHIFTNUM_RGB565_RCOLOR;
		PSLLW MM2, SHIFTNUM_RGB565_GCOLOR;
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

inline void __DrawAlphaCore(unsigned char  **ppSrcBuf, 
							unsigned short *pSrcPal, 
							unsigned short **ppDstBuf, 
							int nSameCount, 
							int nSameAlpha)
{
	unsigned char  *pSrcBuf = *ppSrcBuf;
	unsigned short *pDstBuf = *ppDstBuf;

	if(nSameAlpha < 255)
	{
		int JmpTab[4];

		__int64 n64ClrMask1F = 0x001F001F001F001F;
		__int64 n64ClrMask3F = 0x003F003F003F003F;
		int nSameAlphaD = 255 - nSameAlpha;
		__asm
		{
			MOV ESI, pSrcPal; // Source color palette
			MOV EDI, pDstBuf; // target buffer
			XOR EBX, EBX;
			PXOR MM6, MM6;
/*
			__CurrentSameBegin:
			MOV EAX, [nSameCount];
			CMP EAX, 0;
			JLE __CurrentSameExit;
			MOV ECX, 4;
			CMP EAX, 4;
			JGE __EnoughClrIndex;
			MOV ECX, EAX;
			__EnoughClrIndex:
			SUB EAX, ECX;
			MOV [nSameCount], EAX;
			MOV EAX, pSrcBuf;
			MOVQ MM7, [EAX];		// Read mult. color index to the MMX register
			ADD EAX, ECX;
			MOV [pSrcBuf], EAX;
			__MultClrIndexLoop:
			{
				MOVD EDX, MM7;
				PSRLQ MM7, 8;
				AND EDX, 0xFF;
				ADD EDX, EDX;
				XOR EAX, EAX;
				MOV AX, [ESI][EDX]; // Read source from color palette
				MOVD MM0, EAX;
				MOV EDX, EBX;
				__ShifBegin:
				CMP EDX, 0;
				JLE __ShiftEnd;
				PSLLQ MM0, 16;
				DEC EDX;
				JMP __ShifBegin;
				__ShiftEnd:
				POR MM6, MM0;		// MM6 storage source R color
				INC EBX;
				DEC ECX;
				CMP ECX, 0;
				JG __MultClrIndexLoop;
			}
*/

			MOV dword ptr JmpTab, offset PIXEL4
			MOV dword ptr JmpTab[4], offset PIXEL1
			MOV dword ptr JmpTab[8], offset PIXEL2
			MOV dword ptr JmpTab[12], offset PIXEL3

			__CurrentSameBegin:
			MOV ECX, [nSameCount];
			CMP ECX, 0;
			JLE __CurrentSameExit;
			MOV EAX,ECX
			MOV EDX,ECX

			AND EDX,0x3
			SAR EAX,2

			CMP EAX,0
			JG PIXEL4
			JMP JmpTab[EDX*4];

PIXEL1:

			SUB ECX, 1;
			MOV [nSameCount], ECX;

			MOV EAX, pSrcBuf;
			XOR EDX,EDX
			MOV DL,BYTE PTR[EAX]
			MOVD MM7, EDX;		// Read mult. color index to the MMX register
			ADD EAX, 1;
			MOV [pSrcBuf], EAX;		//ECX	Pixel Count
			MOV EBX, ECX;

			MOVD EDX, MM7;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM6, EAX;

			MOV EBX, 1;
			JMP AlphaBlending;

PIXEL2:

			SUB ECX, 2;
			MOV [nSameCount], ECX;

			MOV EAX, pSrcBuf;
			XOR EDX,EDX
			MOV DX,WORD PTR[EAX]
			MOVD MM7,EDX
//			MOVD MM7, [EAX];		// Read mult. color index to the MMX register
			ADD EAX, 2;
			MOV [pSrcBuf], EAX;		//ECX	Pixel Count
			MOV EBX, ECX;

			MOVD EDX, MM7;
			PSRLQ MM7,8;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM6, EAX;

			MOVD EDX, MM7;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM0, EAX;
			PSLLQ MM0,16;
			POR MM6, MM0;


			MOV EBX, 2;
			JMP AlphaBlending;

PIXEL3:
			SUB ECX, 3;
			MOV [nSameCount], ECX;

			MOV EAX, pSrcBuf;
			XOR EDX,EDX
			MOV DX,WORD PTR[EAX]
			MOVD MM7,EDX//先移动两个
			ADD EAX,2
			XOR EDX,EDX
			MOV DL,BYTE PTR[EAX]//移动一个
			MOVD MM6,EDX
			PSLLQ MM6,16
			POR  MM7,MM6
			ADD EAX,1
	//		MOVQ MM7, [EAX];		// Read mult. color index to the MMX register
	//		ADD EAX, 3;
			MOV [pSrcBuf], EAX;		//ECX	Pixel Count
			MOV EBX, ECX;

			MOVD EDX, MM7;
			PSRLQ MM7,8;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM6, EAX;
			
			MOVD EDX, MM7;
			PSRLQ MM7,8;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM0, EAX;
			PSLLQ MM0,16;
			POR MM6, MM0;

			
			MOVD EDX, MM7;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM0, EAX;
			PSLLQ MM0,32;
			POR MM6, MM0;
			
			MOV EBX, 3;
			JMP AlphaBlending;


PIXEL4:
			SUB ECX, 4;
			MOV [nSameCount], ECX;
			
			MOV EAX, pSrcBuf;
			MOVQ MM7, [EAX];		// Read mult. color index to the MMX register
			ADD EAX, 4;
			MOV [pSrcBuf], EAX;		//ECX	Pixel Count
			MOV EBX, ECX;

			MOVD EDX, MM7;
			PSRLQ MM7,8;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM6, EAX;
			
			MOVD EDX, MM7;
			PSRLQ MM7,8;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM0, EAX;
			PSLLQ MM0,16;
			POR MM6, MM0;
			
			
			MOVD EDX, MM7;
			PSRLQ MM7,8;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM0, EAX;
			PSLLQ MM0,32;
			POR MM6, MM0;

			MOVD EDX, MM7;
			AND EDX, 0xFF;
			ADD EDX, EDX;
			XOR EAX, EAX;
			MOV AX, [ESI][EDX];
			MOVD MM0, EAX;
			PSLLQ MM0,48;
			POR MM6, MM0;

			MOV EBX, 4;

AlphaBlending:
//			CMP EBX, 0;
//			JLE __ProcessFinished;
			// {					
				MOVQ MM5, MM6;		// MM5 storage source G color
				MOVQ MM4, MM6;		// MM4 storage source B color
				PSRLW MM6, SHIFTNUM_RGB565_RCOLOR;
				PSRLW MM5, SHIFTNUM_RGB565_GCOLOR;
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
				PSRLW MM3, SHIFTNUM_RGB565_RCOLOR;
				PSRLW MM2, SHIFTNUM_RGB565_GCOLOR;
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
				PSLLW MM3, SHIFTNUM_RGB565_RCOLOR;
				PSLLW MM2, SHIFTNUM_RGB565_GCOLOR;
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
				XOR EBX, EBX;
				PXOR MM6, MM6;
			// }
			//__ProcessFinished:
			JMP __CurrentSameBegin;
			__CurrentSameExit:
			MOV [pDstBuf], EDI;
			EMMS;
		}
	}
	else
	{
		while(nSameCount > 0)
		{
			*pDstBuf++ = pSrcPal[*pSrcBuf++];
			nSameCount --;
		}
	}

	*ppSrcBuf = pSrcBuf;
	*ppDstBuf = pDstBuf;
}

inline void __DrawScaleAlphaCore(unsigned char  **ppSrcBuf, 
								 unsigned short *pSrcPal, 
								 unsigned short **ppDstBuf, 
								 int nSameCount, 
								 int nSameAlpha, 
								 int *pnRemainScaleWidth, 
								 int nScaleX)
{
	unsigned char  *pSrcBuf = *ppSrcBuf;
	unsigned short *pDstBuf = *ppDstBuf;
	int nRemainScaleWidth = *pnRemainScaleWidth;

	if(nSameAlpha < 255)
	{
		__asm
		{
			MOV ESI, pSrcPal; // Source color palette
			MOV EDI, pDstBuf; // target buffer
			XOR EBX, EBX;
			PXOR MM6, MM6;

			MOVD MM7, [nSameAlpha];
			PUNPCKLWD MM7, MM7;
			PUNPCKLDQ MM7, MM7;
			MOV EAX, 0x00FF00FF;
			MOVD MM0, EAX;
			PUNPCKLDQ MM0, MM0;
			PSUBW MM0, MM7;

			__CurrentSameBegin:
			MOV EAX, [nSameCount];
			CMP EAX, 0;
			JLE __CurrentSameExit;
			MOV ECX, 4;
			CMP EAX, 4;
			JGE __EnoughClrIndex;
			MOV ECX, EAX;
			__EnoughClrIndex:
			SUB EAX, ECX;
			MOV [nSameCount], EAX;
			MOV EAX, pSrcBuf;
			MOV EDX, [EAX]; // Read mult. color index to the register
			ADD EAX, ECX;
			MOV [pSrcBuf], EAX;
			__MultClrIndexLoop:
			{
				MOV EAX, 0xFFFF;
				MOVD MM2, EAX;
				MOV EAX, EDX;
				SHR EDX, 8;
				AND EAX, 0xFF;
				ADD EAX, EAX;
				MOVD MM1, [ESI][EAX]; // Read source from color palette
				PAND MM1, MM2;
				CMP BX, 0;
				JLE __ShiftEnd;
				CMP BX, 3;
				JNE __Shift32;
				PSLLQ MM1, 48;
				JMP __ShiftEnd;
				__Shift32:
				CMP BX, 2;
				JNE __Shift16;
				PSLLQ MM1, 32;
				JMP __ShiftEnd;
				__Shift16:
				PSLLQ MM1, 16;
				__ShiftEnd:
				POR MM6, MM1;			// MM6 storage source R color
				INC BX;
				DEC ECX;
				CMP ECX, 0;
				JG __MultClrIndexLoop;
			}

			// Begin scale alpha
			MOVQ MM5, MM6;		// MM5 storage source G color
			MOVQ MM4, MM6;		// MM4 storage source B color
			PSRLW MM6, SHIFTNUM_RGB565_RCOLOR;
			PSRLW MM5, SHIFTNUM_RGB565_GCOLOR;
			MOV EDX, CLRMASK_RGB565_G;
			MOVD MM1, EDX;
			PUNPCKLDQ MM1, MM1;
			PAND MM5, MM1;
			MOV EDX, CLRMASK_RGB565_RB;
			MOVD MM1, EDX;
			PUNPCKLDQ MM1, MM1;
			PAND MM4, MM1;
			PMULLW MM6, MM7;
			PMULLW MM5, MM7;
			PMULLW MM4, MM7;
			// Begin process X scale
			MOV EDX, [nRemainScaleWidth];
			CMP EDX, 0;
			JLE __DoFinishScaleX;
			MOV EAX, [nScaleX];
			IMUL AX, BX;
			MOV CH, 0xFF;
			CMP EAX, EDX;
			JLE __NoChangeScaleX;
			MOV EAX, EDX;
			MOV CH, DL;
			__NoChangeScaleX:
			SUB EDX, EAX;
			MOV [nRemainScaleWidth], EDX;
			__4AlphaProcessBegion:
			MOV EAX, [nScaleX];
			__DoScaleXBegin:
			// {
				MOVQ MM3, [EDI];	// MM3 storage target color
				// Process target R color
				MOVQ MM2, MM3;
				MOV EDX, CLRMASKR_RGB565_R;
				MOVD MM1, EDX;
				PUNPCKLDQ MM1, MM1;
				PAND MM3, MM1;
				PSRLW MM2, SHIFTNUM_RGB565_RCOLOR;
				PMULLW MM2, MM0;
				MOVQ MM1, MM6;
				CMP EBX, 0;
				JL __DoMaxScaleLeft1;
				CMP EAX, 2;
				JL __DoFinishScaleXStep1;
				PUNPCKLWD MM1, MM1;
				CMP EAX, 2;
				JE __DoFinishScaleXStep1;
				PUNPCKLDQ MM1, MM1;
				JMP __DoFinishScaleXStep1;
				__DoMaxScaleLeft1:
				PUNPCKLWD MM1, MM1;
				PUNPCKLDQ MM1, MM1;
				__DoFinishScaleXStep1:
				PADDUSW MM2, MM1;
				PSRLW MM2, 8;
				PSLLW MM2, SHIFTNUM_RGB565_RCOLOR;
				POR MM3, MM2;
				// Process target G color
				MOVQ MM2, MM3;
				MOV EDX, CLRMASKR_RGB565_G;
				MOVD MM1, EDX;
				PUNPCKLDQ MM1, MM1;
				PAND MM3, MM1;
				PSRLW MM2, SHIFTNUM_RGB565_GCOLOR;
				MOV EDX, CLRMASK_RGB565_G;
				MOVD MM1, EDX;
				PUNPCKLDQ MM1, MM1;
				PAND MM2, MM1;
				PMULLW MM2, MM0;
				MOVQ MM1, MM5;
				CMP EBX, 0;
				JL __DoMaxScaleLeft2;
				CMP EAX, 2;
				JL __DoFinishScaleXStep2;
				PUNPCKLWD MM1, MM1;
				CMP EAX, 2;
				JE __DoFinishScaleXStep2;
				PUNPCKLDQ MM1, MM1;
				JMP __DoFinishScaleXStep2;
				__DoMaxScaleLeft2:
				PUNPCKLWD MM1, MM1;
				PUNPCKLDQ MM1, MM1;
				__DoFinishScaleXStep2:
				PADDUSW MM2, MM1;
				PSRLW MM2, 8;
				PSLLW MM2, SHIFTNUM_RGB565_GCOLOR;
				POR MM3, MM2;
				// Process target B color
				MOVQ MM2, MM3;
				MOV EDX, CLRMASKR_RGB565_B;
				MOVD MM1, EDX;
				PUNPCKLDQ MM1, MM1;
				PAND MM3, MM1;
				MOV EDX, CLRMASK_RGB565_RB;
				MOVD MM1, EDX;
				PUNPCKLDQ MM1, MM1;
				PAND MM2, MM1;
				PMULLW MM2, MM0;
				MOVQ MM1, MM4;
				CMP EBX, 0;
				JL __DoMaxScaleLeft3;
				CMP EAX, 1;
				JNE __DoScaleXNextStep31;
				MOV CL, BL;
				XOR BX, BX;
				JMP __DoFinishScaleXStep3;
				__DoScaleXNextStep31:
				CMP EAX, 2;
				JNE __DoScaleXNextStep32;
				PUNPCKLWD MM1, MM1;
				// {
					CMP BX, 2;
					JL __DoOneWord32;
					MOV CL, 4;
					CMP EAX, 4
					JG __NoShiftSrcAtPresent32;
					SUB BX, 2;
					PSRLQ MM6, 32;
					PSRLQ MM5, 32;
					PSRLQ MM4, 32;
					__NoShiftSrcAtPresent32:
					JMP __DoWordFinish32;
					__DoOneWord32:
					MOV CL, 2;
					XOR BX, BX;
					__DoWordFinish32:
				// }
				JMP __DoFinishScaleXStep3;
				__DoScaleXNextStep32:
				CMP EAX, 4
				JG __NoShiftSrcAtPresent33;
				DEC BX;
				PSRLQ MM6, 16;
				PSRLQ MM5, 16;
				PSRLQ MM4, 16;
				__NoShiftSrcAtPresent33:
				PUNPCKLWD MM1, MM1;
				PUNPCKLDQ MM1, MM1;
				CMP EAX, 3;
				JNE __DoScaleXNextStep33;
				MOV CL, 3;
				JMP __DoFinishScaleXStep3;
				__DoScaleXNextStep33:
				MOV CL, 4;
				JMP __DoFinishScaleXStep3;
				__DoMaxScaleLeft3:
				DEC BX;
				PSRLQ MM6, 16;
				PSRLQ MM5, 16;
				PSRLQ MM4, 16;
				PUNPCKLWD MM1, MM1;
				PUNPCKLDQ MM1, MM1;
				MOV CL, AL;
				__DoFinishScaleXStep3:
				PADDUSW MM2, MM1;
				PSRLW MM2, 8;
				POR MM3, MM2;
				// Write to target buffer
				CMP CH, 0xFF;
				JE __DoWriteDirectly;
				CMP CH, 0;
				JE __DoFinishScaleWrite;
				CMP CH, CL;
				JGE __DoPreWrite;
				MOV CL, CH;
				__DoPreWrite:
				SUB CH, CL;
				__DoWriteDirectly:
				CMP CL, 4
				JE __ScaleWriteWithFull;
				CMP CL, 2
				JL __SkipScaleWriteWithHalf;
				MOVD [EDI], MM3;
				ADD EDI, 4;
				PSRLQ MM3, 32;
				__SkipScaleWriteWithHalf:
				CMP CL, 2
				JE __DoFinishScaleWrite;
				MOVD EDX, MM3;
				MOV [EDI], DX;
				ADD EDI, 2;
				JMP __DoFinishScaleWrite;
				__ScaleWriteWithFull:
				MOVQ [EDI], MM3;
				ADD EDI, 8;
				__DoFinishScaleWrite:
				SUB EAX, 4;
				CMP BX, 0;
				JLE __DoFinishScaleX;
				CMP EAX, 4;
				JGE __SkipSetMaxScaleFlag;
				OR EBX, 0x80000000;
				__SkipSetMaxScaleFlag:
				CMP EAX, 0;
				JG __DoScaleXBegin;
			// }
			AND EBX, 0x0000FFFF;
			CMP BX, 0;
			JG __4AlphaProcessBegion;
			__DoFinishScaleX:
			XOR EBX, EBX;
			PXOR MM6, MM6;

			JMP __CurrentSameBegin;
			__CurrentSameExit:
			MOV [pDstBuf], EDI;

			EMMS;
		}
	}
	else
	{
		int nCurScaleX = 0;
		while(nSameCount > 0)
		{
			unsigned short usnSrcClr = pSrcPal[*pSrcBuf++];
			nCurScaleX = min(nScaleX, nRemainScaleWidth);
			nRemainScaleWidth -= nCurScaleX;
			for(; nCurScaleX > 0; nCurScaleX--)
			{
				*pDstBuf++ = usnSrcClr;
			}

			nSameCount -= 1;
		}
	}

	*ppSrcBuf = pSrcBuf;
	*ppDstBuf = pDstBuf;
	*pnRemainScaleWidth = nRemainScaleWidth;
}
#endif
