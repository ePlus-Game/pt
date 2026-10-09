 /*****************************************************************************************
//  表现模块的对外接口的二维版本实现。
//	Copyright : Kingsoft 2002
//	Author	:   Spe(huyi)
//	CreateTime:	2002-11-11
*****************************************************************************************/
#define _REPRESENT_INTERNAL_SIGNATURE_
#include "KRepresentShell2.h"
#include "KRepresentUnit.h"
#include "KColors.h"
#include "KFont2.h"
#include "ImageOperation.h"
#include "RepresentUtility.h"
#include "KWin32Wnd.h"
#include "KBmpFile24.h"
#include <assert.h>
#include "KTimer.h"

static KTimer	s_Timer;
static int		s_FastDrawMode = false;

//根据SPR头指针，获取调色版缓冲区指针
#define GET_SPR_PALETTE(pHeader)	( ((char*)pHeader) + sizeof(SPRHEAD))

//=========创建一个iRepresentShell接口的实例===============
extern "C" __declspec(dllexport)
iRepresentShell* CreateRepresentShell()
{
	return (new KRepresentShell2);
}

IInlinePicEngineSink* g_pIInlinePicSinkRP = NULL;	//嵌入式图片的处理接口[wxb 2003-6-20]
HRESULT KRepresentShell2::AdviseRepresent(IInlinePicEngineSink* pSink)
{
	assert(NULL == g_pIInlinePicSinkRP);	//一般不会挂接两次
	g_pIInlinePicSinkRP = pSink;
	return S_OK;
}

HRESULT KRepresentShell2::UnAdviseRepresent(IInlinePicEngineSink* pSink)
{
	if (pSink == g_pIInlinePicSinkRP)
		g_pIInlinePicSinkRP = NULL;
	return S_OK;
}

//##ModelId=3DD20C90004D
KRepresentShell2::KRepresentShell2()
{
	m_bHighQuality = true;
	m_nLeft = 0;
	m_nTop = 0;

	// add by chenshanglin for game video on 2005-10-17
	m_nDrawFlag = true;
	// add end

	memset(m_FontTable, 0, sizeof(m_FontTable));
}

//##ModelId=3DD20C900089
KRepresentShell2::~KRepresentShell2()
{
	m_DirectDraw.Exit();
	m_ImageStore.Free();
	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].pFontObj)
		{
			m_FontTable[i].pFontObj->Release();
			m_FontTable[i].pFontObj = NULL;
		}
	}
}

//设置偏色列表
unsigned int KRepresentShell2::SetAdjustColorList(unsigned int* puColorList, unsigned int uCount)
{
	return m_ImageStore.SetAdjustColorList(puColorList, uCount);
}



bool KRepresentShell2::Create(int nWidth, int nHeight, bool bFullScreen)
{
	m_DirectDraw.Mode(bFullScreen, nWidth, nHeight);

	if (m_DirectDraw.Init())
	{
		m_Canvas.Init(nWidth, nHeight);
		m_ImageStore.Init();
		RIO_Set16BitImageFormat(m_DirectDraw.GetRGBBitMask16() == RGB_565);
		// 初始化Gdi+
		InitGdiplus();
		s_Timer.Start();
		return true;
	}
	return false;
}

//##ModelId=3DCA0B230317
bool KRepresentShell2::CreateAFont(const char* pszFontFile, CHARACTER_CODE_SET CharaSet, int nId)
{
	int nFirstFree = -1;
	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].pFontObj == NULL && nFirstFree == -1)
			nFirstFree = i;
		else if (m_FontTable[i].nId == nId)
		{
			nFirstFree = i;
			break;
		}
	}
	if (nFirstFree == -1 || pszFontFile == NULL || nId == 0)
		return false;

	if (m_FontTable[nFirstFree].pFontObj)
	{
		m_FontTable[nFirstFree].pFontObj->Release();
		m_FontTable[nFirstFree].pFontObj = NULL;
	}

	if (pszFontFile[0] == '#')
	{
		//共享已经打开的字库
		int nShareWithId = atoi(pszFontFile + 1);
		for (int j = 0; j < RS2_MAX_FONT_ITEM_NUM; j++)
		{
			if (nFirstFree != j &&	m_FontTable[j].nId == nShareWithId &&
				m_FontTable[j].pFontObj)
			{
				m_FontTable[nFirstFree].nId = nId;
				m_FontTable[nFirstFree].pFontObj = m_FontTable[j].pFontObj->Clone();
				return true;
			}
		}
		return false;
	}

	if ((m_FontTable[nFirstFree].pFontObj = new KFont2) == NULL)
		return false;

	m_FontTable[nFirstFree].pFontObj->Init(&m_Canvas);
	if (m_FontTable[nFirstFree].pFontObj->Load((LPSTR)pszFontFile))
	{
		m_FontTable[nFirstFree].nId = nId;
		m_FontTable[nFirstFree].pFontObj->SetOutputSize(nId, nId + 1);
	}
	else
	{
		m_FontTable[nFirstFree].pFontObj->Release();
		m_FontTable[nFirstFree].pFontObj = NULL;
	}
	
	return (m_FontTable[nFirstFree].pFontObj != NULL);
}

//##ModelId=3DCD8DEA01BB
unsigned int KRepresentShell2::CreateImage(const char* pszName, int nWidth, int nHeight, int nType)
{
	return m_ImageStore.CreateImage(pszName, nWidth, nHeight, nType);
}


void KRepresentShell2::DrawPrimitives(int nPrimitiveCount, KRepresentUnit* pPrimitives, unsigned int uGenre, int bSinglePlaneCoord)
{
	// add by chenshanglin on 2005-10-17 for game video
	if(m_nDrawFlag == 0)
	{
		return;
	}
	// add end

	int i = 0;
	switch(uGenre)
	{
	case RU_T_IMAGE_ON_DC:
		{
			KRUImagePart* pTemp = (KRUImagePart *)pPrimitives;
			RECT rcOld;
			m_Canvas.GetClipRect(&rcOld);
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{	
				switch(pTemp->nType)
				{
				case ISI_T_BITMAP16:
					{
						if(pTemp->bRenderStyle == IMAGE_RENDER_STYLE_WITH_HANDLE&&pTemp->hEffectBitmap)
						{
							int nX = pTemp->oPosition.nX;
							int nY = pTemp->oPosition.nY;
							if (bSinglePlaneCoord == false)
								CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
							m_Canvas.DrawBitmapOnDc(pTemp->hEffectDrawDC,pTemp->hEffectBitmap,nX,nY);
							
						}
					}
					break;
				}
			}
		}
		return;
		
	case RU_T_IMAGE:

		{
			int	FastDraw = (!bSinglePlaneCoord && s_FastDrawMode);			
			KRUImage* pTemp = (KRUImage*)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{
				switch(pTemp->nType)
				{
				case ISI_T_SPR:
					{
						SPRFRAME* pFrame = NULL;
						SPRHEAD* pSprHeader;
						short nISPosition = IMAGE_IS_POSITION_INIT;
						pSprHeader = (SPRHEAD*)m_ImageStore.GetImage(
							pTemp->szImage,	pTemp->uImage, nISPosition,
							pTemp->nFrame, pTemp->nType, (void*&)pFrame, pTemp->bMultiThreadLoad);
						if ( nISPosition < 0 )
						{
							break;
						}
							
						if (pSprHeader == NULL ||  pFrame == NULL  )
							break;

						int nX = pTemp->oPosition.nX;
						int nY = pTemp->oPosition.nY;
						if (bSinglePlaneCoord == false)
							CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
						if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
						{
					//****to be modify****
#define CENTERX		256
#define	CENTERY		312
							int nCenterX = pSprHeader->CenterX;
							int nCenterY = pSprHeader->CenterY;
							if (nCenterX || nCenterY)
							{
								nX -= nCenterX;
								nY -= nCenterY;
							}
							else if (pSprHeader->Width > CENTERX)
							{
								nX -= CENTERX;
								nY -= CENTERY;
							}
						}
						//****to be modify end****

//						Check Current Draw Device??;
						if ((pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW) == 0)
						{
							nX += pFrame->OffsetX;
							nY += pFrame->OffsetY;
						}

						char* pPalette = GET_SPR_PALETTE(pSprHeader);
						
						switch(pTemp->bRenderStyle)
						{
						case IMAGE_RENDER_STYLE_ALPHA:
						case IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT:
							if (FastDraw)
							{
								m_Canvas.DrawSprite3LevelAlpha(nX, nY, pFrame->Width, pFrame->Height,
									pFrame->Sprite, pPalette, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							}
							else
							{
								m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
									pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8 ,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							}
							break;
						case IMAGE_RENDER_STYLE_3LEVEL:
							if ( pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED] == BITS_PIXEL_16 )
							{
								m_Canvas.DrawSprite(nX, nY, pFrame->Width, pFrame->Height,
									pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							}
							else
							{
								m_Canvas.DrawSprite3LevelAlpha(nX, nY, pFrame->Width, pFrame->Height,
									pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);							
							}
							break;
						case IMAGE_RENDER_STYLE_OPACITY:
							m_Canvas.DrawSprite(nX, nY, pFrame->Width, pFrame->Height,
								pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_BORDER:
								m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
									pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST:
							{
								short nISPosition = IMAGE_IS_POSITION_INIT;
								pPalette = m_ImageStore.GetAdjustColorPalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
								if ( nISPosition < 0 )
								{
									break;
								}

								if (FastDraw)
								{
									m_Canvas.DrawSprite3LevelAlpha(nX, nY, pFrame->Width, pFrame->Height,
										pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
								}
								else
								{
									m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
										pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
								}
							}
							break;
						// lixuewu 新增绘图方式 2004.11.18
						case IMAGE_RENDER_STYLE_HUE_ADJUST:
							{
								short nISPosition = IMAGE_IS_POSITION_INIT;
								if ( BITS_PIXEL_16 != pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED] )
								{
									pPalette = m_ImageStore.GetHuePalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
								}
						//		if ( nISPosition < 0 )
							//	{
							//		break;
							//	}
								if (FastDraw)
								{
									m_Canvas.DrawSprite3LevelAlpha(nX, nY, pFrame->Width, pFrame->Height,
										pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
								}
								else
								{
						//			m_Canvas.DrawSpriteAlphaWithTable(nX, nY, pFrame->Width, pFrame->Height,
							//			pFrame->Sprite, pPalette, 0,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
									m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
										pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
								}
							}
							break;
						}
					}
					break;
				case ISI_T_BITMAP16:
					{
						void* pFrame;
						short nISPosition = IMAGE_IS_POSITION_INIT;
						KSGImageContent* pBitmap = (KSGImageContent *)m_ImageStore.GetImage(
							pTemp->szImage,	pTemp->uImage, nISPosition,
							0, ISI_T_BITMAP16, pFrame, pTemp->bMultiThreadLoad);
						if ( nISPosition < 0 )
						{
							break;
						}
						if (pBitmap)
						{
							int nX = pTemp->oPosition.nX;
							int nY = pTemp->oPosition.nY;
							if (bSinglePlaneCoord == false)
								CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
							m_Canvas.DrawBitmap16(nX, nY,
								pBitmap->nWidth, pBitmap->nHeight, pBitmap->Data);
						}
					}
					break;
				case ISI_T_BITMAP16_ALPHA:
					{
						void* pFrame;
						short nISPosition = IMAGE_IS_POSITION_INIT;
						KSGImageContent* pBitmap = (KSGImageContent *)m_ImageStore.GetImage(
							pTemp->szImage,	pTemp->uImage, nISPosition,
							0, ISI_T_BITMAP16_ALPHA, pFrame, pTemp->bMultiThreadLoad);
						if ( nISPosition < 0 )
						{
							break;
						}

						if (pBitmap)
						{
							int nX = pTemp->oPosition.nX;
							int nY = pTemp->oPosition.nY;
							if (bSinglePlaneCoord == false)
								CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
							m_Canvas.DrawBitmap16Alpha(nX, nY,
								pBitmap->nWidth, pBitmap->nHeight, pBitmap->Data);
						}
					}
					break;
				}
			}
		}
		break;//*/

	case RU_T_IMAGE_4:
		{
			KRUImage4* pTemp = (KRUImage4*)pPrimitives;
			RECT rcOld;
			m_Canvas.GetClipRect(&rcOld);
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{
				//_ASSERT(pTemp->nType == ISI_T_SPR);
					{
						SPRFRAME* pFrame;
						short nISPosition = IMAGE_IS_POSITION_INIT;
						SPRHEAD* pSprHeader = (SPRHEAD*)m_ImageStore.GetImage(
							pTemp->szImage,	pTemp->uImage, nISPosition,
							pTemp->nFrame, pTemp->nType, (void*&)pFrame, pTemp->bMultiThreadLoad);
						if (pSprHeader == NULL)
							break;
						if (  nISPosition < 0 )
						{
							break;
						}

						int nX = pTemp->oPosition.nX;
						int nY = pTemp->oPosition.nY;
						if (!bSinglePlaneCoord)
							CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
						if ((pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW) == 0)
						{
							nX += pFrame->OffsetX;
							nY += pFrame->OffsetY;
						}

						RECT	rc;
						rc.left  = nX;
						rc.top   = nY;
						nX -= pTemp->oImgLTPos.nX;
						nY -= pTemp->oImgLTPos.nY;
						rc.right = nX + pTemp->oImgRBPos.nX;
						rc.bottom= nY + pTemp->oImgRBPos.nY;
						if (rc.left < rcOld.left)
							rc.left = rcOld.left;
						if (rc.right > rcOld.right)
							rc.right = rcOld.right;
						if (rc.top < rcOld.top)
							rc.top = rcOld.top;
						if (rc.bottom > rcOld.bottom)
							rc.bottom = rcOld.bottom;
						m_Canvas.SetClipRect(&rc);

						char* pPalette = GET_SPR_PALETTE(pSprHeader);

						switch(pTemp->bRenderStyle)
						{
						case IMAGE_RENDER_STYLE_ALPHA:
						case IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT:
							m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
								pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_3LEVEL:
							if ( pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED] == BITS_PIXEL_16 )
							{
								m_Canvas.DrawSprite(nX, nY, pFrame->Width, pFrame->Height,
									pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							}
							else
							{
								m_Canvas.DrawSprite3LevelAlpha(nX, nY, pFrame->Width,
									pFrame->Height, pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							}
							break;
						case IMAGE_RENDER_STYLE_OPACITY:
							m_Canvas.DrawSprite(nX, nY, pFrame->Width, pFrame->Height,
								pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_BORDER:
//							m_Canvas.DrawSpriteBorder(nX, nY, pFrame->Width, pFrame->Height,
//								///g_RGB(pTemp->Color.Color_b.r, pTemp->Color.Color_b.g, pTemp->Color.Color_b.b),
//								g_RGB(200, 200, 0),
//								pFrame->Sprite);
							break;
						case IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST:
							pPalette = m_ImageStore.GetAdjustColorPalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
							if ( nISPosition < 0 )
							{
								break;
							}
							m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
								pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						// 调色板选择方式绘制
						case IMAGE_RENDER_STYLE_HUE_ADJUST:
							pPalette = m_ImageStore.GetHuePalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
							if ( nISPosition < 0 )
							{
								break;
							}
							m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
								pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						}
					}
			}
			m_Canvas.SetClipRect(&rcOld);
		}
		break;
	case RU_T_IMAGE_PART_ON_DC:
		{
			KRUImagePart* pTemp = (KRUImagePart *)pPrimitives;
			RECT rcOld;
			m_Canvas.GetClipRect(&rcOld);
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{	
				switch(pTemp->nType)
				{
				case ISI_T_BITMAP16:
					{
						void* pFrame;
						short nISPosition = IMAGE_IS_POSITION_INIT;
						KSGImageContent* pBitmap = (KSGImageContent*)m_ImageStore.GetImage(
							pTemp->szImage,	pTemp->uImage, nISPosition,
							pTemp->nFrame, pTemp->nType, pFrame, pTemp->bMultiThreadLoad);
						if (  nISPosition < 0 )
						{
							break;
						}
						if (pBitmap)
						{
							int nX = pTemp->oPosition.nX;
							int nY = pTemp->oPosition.nY;
							if (bSinglePlaneCoord == false)
								CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
	

							int dx = pTemp->oImgLTPos.nX;
							int dy = pTemp->oImgLTPos.nY;
							int width = pTemp->oImgRBPos.nX - dx;
							int height = pTemp ->oImgRBPos.nY - dy;
							m_Canvas.DrawBitmap16OnDc(nX, nY,
								pBitmap->nWidth, pBitmap->nHeight,dx,dy,width,height, pBitmap->Data,pTemp->hEffectDrawDC);
						}
					}
					break;
				}
			}
				
		}
		break;
	case RU_T_IMAGE_PART:
		{
			KRUImagePart* pTemp = (KRUImagePart *)pPrimitives;
			RECT rcOld;
			m_Canvas.GetClipRect(&rcOld);
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{				
				switch(pTemp->nType)
				{
				case ISI_T_SPR:
					{
						short nISPosition = IMAGE_IS_POSITION_INIT;
						SPRFRAME* pFrame = NULL;
						SPRHEAD* pSprHeader = (SPRHEAD*)m_ImageStore.GetImage(
							pTemp->szImage,	pTemp->uImage, nISPosition,
							pTemp->nFrame, pTemp->nType, (void*&)pFrame,pTemp->bMultiThreadLoad);
						if (  nISPosition < 0 )
						{
							break;
						}
						if (pSprHeader == NULL ||  pFrame == NULL )
							break;

						int nX = pTemp->oPosition.nX;
						int nY = pTemp->oPosition.nY;
						if (bSinglePlaneCoord == false)
							CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
						if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
						{
							nX -= pSprHeader->CenterX;
							nY -= pSprHeader->CenterY;
						}
//						Check Current Draw Device??;

						// Clipper
						if ((pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW) == 0)
						{
							nX += pFrame->OffsetX;
							nY += pFrame->OffsetY;
						}

						RECT	rc;
						//<---- Modified By Ray [Luoliang] [2005-11-16]
						
						//修正只能画从左往右的和从上往下画ImagePart的bug
						rc.left  = nX + pTemp->oImgLTPos.nX;
						rc.top   = nY + pTemp->oImgLTPos.nY;
//						nX -= pTemp->oImgLTPos.nX;
//						nY -= pTemp->oImgLTPos.nY;

						// End. Ray [LuoLiang] [2005-11-16] ---->
						
						rc.right = nX + pTemp->oImgRBPos.nX;
						rc.bottom= nY + pTemp->oImgRBPos.nY;
						if (rc.left < rcOld.left)
							rc.left = rcOld.left;
						if (rc.right > rcOld.right)
							rc.right = rcOld.right;
						if (rc.top < rcOld.top)
							rc.top = rcOld.top;
						if (rc.bottom > rcOld.bottom)
							rc.bottom = rcOld.bottom;
						m_Canvas.SetClipRect(&rc);

						char* pPalette = GET_SPR_PALETTE(pSprHeader);

						switch(pTemp->bRenderStyle)
						{
						case IMAGE_RENDER_STYLE_ALPHA:
						case IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT:
							m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
								pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_3LEVEL:
							if ( pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED] == BITS_PIXEL_16 )
							{
								m_Canvas.DrawSprite(nX, nY, pFrame->Width, pFrame->Height,
									pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							}
							else
							{
								m_Canvas.DrawSprite3LevelAlpha(nX, nY, pFrame->Width,
									pFrame->Height, pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							}
							break;
						case IMAGE_RENDER_STYLE_OPACITY:
							m_Canvas.DrawSprite(nX, nY, pFrame->Width, pFrame->Height,
								pFrame->Sprite, pPalette,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST:
							pPalette = m_ImageStore.GetAdjustColorPalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
							if ( nISPosition < 0 )
							{
								break;
							}
							m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
								pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						// 调色板选择方式绘制
						case IMAGE_RENDER_STYLE_HUE_ADJUST:
							pPalette = m_ImageStore.GetHuePalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
							if ( nISPosition < 0 )
							{
								break;
							}
							m_Canvas.DrawSpriteAlpha(nX, nY, pFrame->Width, pFrame->Height,
								pFrame->Sprite, pPalette, pTemp->Color.Color_b.a / 8,pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						}
					}
					break;
				case ISI_T_BITMAP16:
					{
						void* pFrame;
						short nISPosition = IMAGE_IS_POSITION_INIT;
						KSGImageContent* pBitmap = (KSGImageContent*)m_ImageStore.GetImage(
							pTemp->szImage,	pTemp->uImage, nISPosition,
							pTemp->nFrame, pTemp->nType, pFrame, pTemp->bMultiThreadLoad);
						if (  nISPosition < 0 )
						{
							break;
						}
						if (pBitmap)
						{
							int nX = pTemp->oPosition.nX;
							int nY = pTemp->oPosition.nY;
							if (bSinglePlaneCoord == false)
								CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
	
							RECT	rc;
							rc.left  = nX;
							rc.top   = nY;
							nX -= pTemp->oImgLTPos.nX;
							nY -= pTemp->oImgLTPos.nY;
							rc.right = nX + pTemp->oImgRBPos.nX;
							rc.bottom= nY + pTemp->oImgRBPos.nY;
							if (rc.left < rcOld.left)
								rc.left = rcOld.left;
							if (rc.right > rcOld.right)
								rc.right = rcOld.right;
							if (rc.top < rcOld.top)
								rc.top = rcOld.top;
							if (rc.bottom > rcOld.bottom)
								rc.bottom = rcOld.bottom;
							m_Canvas.SetClipRect(&rc);

							m_Canvas.DrawBitmap16(nX, nY,
								pBitmap->nWidth, pBitmap->nHeight, pBitmap->Data);
						}
					}
					break;
				//-------> Ray [Luoliang] 2005-1-15
				//支持画带Alpha的Bitmap
				case ISI_T_BITMAP16_ALPHA:
					{
						void* pFrame;
						short nISPosition = IMAGE_IS_POSITION_INIT;
						KSGImageContent* pBitmap = (KSGImageContent*)m_ImageStore.GetImage(
							pTemp->szImage,	pTemp->uImage, nISPosition,
							pTemp->nFrame, pTemp->nType, pFrame, pTemp->bMultiThreadLoad);
						if (  nISPosition < 0 )
						{
							break;
						}
						if (pBitmap)
						{
							int nX = pTemp->oPosition.nX;
							int nY = pTemp->oPosition.nY;
							if (bSinglePlaneCoord == false)
								CoordinateTransform(nX, nY, pTemp->oPosition.nZ);
	
							RECT	rc;
							rc.left  = nX;
							rc.top   = nY;
							nX -= pTemp->oImgLTPos.nX;
							nY -= pTemp->oImgLTPos.nY;
							rc.right = nX + pTemp->oImgRBPos.nX;
							rc.bottom= nY + pTemp->oImgRBPos.nY;
							if (rc.left < rcOld.left)
								rc.left = rcOld.left;
							if (rc.right > rcOld.right)
								rc.right = rcOld.right;
							if (rc.top < rcOld.top)
								rc.top = rcOld.top;
							if (rc.bottom > rcOld.bottom)
								rc.bottom = rcOld.bottom;
							m_Canvas.SetClipRect(&rc);

							m_Canvas.DrawBitmap16Alpha(nX, nY,
								pBitmap->nWidth, pBitmap->nHeight, pBitmap->Data);
						}
					}
					break;
				//<------- End [Ray]
				}
			}
			m_Canvas.SetClipRect(&rcOld);
		}
		break;
	case RU_T_POINT:
		{
			KRUPoint* pTemp = (KRUPoint *)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{				
				int nX = pTemp->oPosition.nX;
				int nY = pTemp->oPosition.nY;
				if (!bSinglePlaneCoord)
					CoordinateTransform(nX, nY, pTemp->oPosition.nZ);				
				m_Canvas.DrawPixel(nX, nY, g_RGB(pTemp->Color.Color_b.r,
					pTemp->Color.Color_b.g, pTemp->Color.Color_b.b));
			}
		}
		break;
	case RU_T_LINE:
		{
			KRULine* pTemp = (KRULine *)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{
				int	nX1 = pTemp->oPosition.nX;
				int nY1 = pTemp->oPosition.nY;
					
				int nX2 = pTemp->oEndPos.nX;
				int nY2 = pTemp->oEndPos.nY;
				if (!bSinglePlaneCoord)
				{
					CoordinateTransform(nX1, nY1, pTemp->oPosition.nZ);
					CoordinateTransform(nX2, nY2, pTemp->oEndPos.nZ);
				}
				if (pTemp->Color.Color_b.a >= 248)
				{
					m_Canvas.DrawLine(nX1, nY1, nX2, nY2, g_RGB(pTemp->Color.Color_b.r,
						pTemp->Color.Color_b.g, pTemp->Color.Color_b.b));
				}
				else if (pTemp->Color.Color_b.a >= 8)
				{
					m_Canvas.DrawLineAlpha(nX1, nY1, nX2, nY2,
						g_RGB(pTemp->Color.Color_b.r, pTemp->Color.Color_b.g, pTemp->Color.Color_b.b),
						31 - pTemp->Color.Color_b.a / 8);
				}
			}
		}
		break;
	case RU_T_RECT:
		{
			KRURect* pTemp = (KRURect *)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{
				int	nX1 = pTemp->oPosition.nX;
				int nY1 = pTemp->oPosition.nY;
					
				int nX2 = pTemp->oEndPos.nX;
				int nY2 = pTemp->oEndPos.nY;
				if (!bSinglePlaneCoord)
				{
					CoordinateTransform(nX1, nY1, pTemp->oPosition.nZ);
					CoordinateTransform(nX2, nY2, pTemp->oEndPos.nZ);
				}
				int	Color = g_RGB(pTemp->Color.Color_b.r,
					pTemp->Color.Color_b.g, pTemp->Color.Color_b.b);
				m_Canvas.DrawLine(nX1, nY1, nX2, nY1, Color);	//上边
				m_Canvas.DrawLine(nX1, nY2, nX2, nY2, Color);	//下边
				m_Canvas.DrawLine(nX1, nY1, nX1, nY2, Color);	//左边
				m_Canvas.DrawLine(nX2, nY1, nX2, nY2, Color);	//右边
			}
		}
		break;
	case RU_T_SHADOW:
		{
			KRUShadow* pTemp =(KRUShadow *)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{				
				int nX1 = pTemp->oPosition.nX;
				int nY1 = pTemp->oPosition.nY;
				int nX2 = pTemp->oEndPos.nX;
				int	nY2 = pTemp->oEndPos.nY;
				if (!bSinglePlaneCoord)
				{
					CoordinateTransform(nX1, nY1, pTemp->oPosition.nZ);
					CoordinateTransform(nX2, nY2, pTemp->oEndPos.nZ);
				}
				m_Canvas.ClearAlpha(nX1, nY1, nX2 - nX1, nY2 - nY1, g_RGB(pTemp->Color.Color_b.r,
					pTemp->Color.Color_b.g, pTemp->Color.Color_b.b), pTemp->Color.Color_b.a);
			}
		}
		break;
	case RU_T_IMAGE_STRETCH:
		if (bSinglePlaneCoord)
		{
			KRUImageStretch* pTemp = (KRUImage*)pPrimitives;
			for (i = 0; i < nPrimitiveCount; i++, pTemp++)
			{
				if (pTemp->nType == ISI_T_BITMAP16)
				{
					LPDIRECTDRAWSURFACE pSurface;
					short nISPosition = IMAGE_IS_POSITION_INIT;
					KSGImageContent* pBitmap = (KSGImageContent*)m_ImageStore.GetImage(
							pTemp->szImage,	pTemp->uImage, nISPosition,
							0, ISI_T_BITMAP16, (void*&)pSurface, pTemp->bMultiThreadLoad);
						if (  nISPosition < 0 )
						{
							break;
						}

					if (pBitmap)
					{
						if (pSurface)
						{
							RECT	rc;
							rc.left = pTemp->oPosition.nX;
							rc.top = pTemp->oPosition.nY;
							rc.right = pTemp->oEndPos.nX;
							rc.bottom = pTemp->oEndPos.nY;
							m_Canvas.BltSurface(pSurface, &rc);
						}
						else
						{
							m_ImageStore.CreateBitmapSurface(pTemp->szImage, pTemp->uImage, nISPosition);
						}
					}
				}
			}
		}
		break;//*/
	}
}

void KRepresentShell2::DrawPrimitivesOnImage(int nPrimitiveCount, KRepresentUnit* pPrimitives, 
        unsigned int uGenre, const char* pszImage, unsigned int uImage, short& nImagePosition)
{
	// add by chenshanglin on 2005-10-17 for game video
	if(m_nDrawFlag == 0)
	{
		return;
	}
	// add end

	int nType;
	nImagePosition = IMAGE_IS_POSITION_INIT;
	KSGImageContent* pDestBitmap = (KSGImageContent*)m_ImageStore.GetExistedCreateBitmap(
		pszImage, uImage, nImagePosition, &nType);

	if (pDestBitmap == NULL)
		return;

	int   i = 0;
	int   nDestWidth  = pDestBitmap->nWidth;
	int   nDestHeight = pDestBitmap->nHeight;
	void* pDestBuffer = pDestBitmap->Data;

	switch(uGenre)
	{
	case RU_T_IMAGE:
	{
		KRUImage* pTemp = (KRUImage*)pPrimitives;
		for (i = 0; i < nPrimitiveCount; i++, pTemp++)
		{
			switch(pTemp->nType)
			{
			case ISI_T_SPR:
				{
					SPRFRAME* pFrame = NULL;
					short nISPosition = IMAGE_IS_POSITION_INIT;
					SPRHEAD* pSprHeader = (SPRHEAD*)m_ImageStore.GetImage(
						pTemp->szImage,	pTemp->uImage, nISPosition,
						pTemp->nFrame, pTemp->nType, (void*&)pFrame, pTemp->bMultiThreadLoad);
					if (pSprHeader == NULL ||  pFrame == NULL )
						break;
						if (  nISPosition < 0 )
						{
							break;
						}
					
					int nX = pTemp->oPosition.nX;
					int nY = pTemp->oPosition.nY;
					
					if ((pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_FRAME_DRAW) == 0)
					{
						nX += pFrame->OffsetX;
						nY += pFrame->OffsetY;
						if (pTemp->bRenderFlag & RUIMAGE_RENDER_FLAG_REF_SPOT)
						{
							nX -= pSprHeader->CenterX;
							nY -= pSprHeader->CenterY;
						}
					}
					
					char* pPalette = GET_SPR_PALETTE(pSprHeader);
					
					if(nType == ISI_T_BITMAP16)
					{
						switch(pTemp->bRenderStyle)
						{
						case IMAGE_RENDER_STYLE_ALPHA:
						case IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT:
							RIO_CopySprToBufferAlpha(pFrame->Sprite, pFrame->Width, pFrame->Height,
								pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_3LEVEL:
							if ( pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED] == BITS_PIXEL_16 )
							{
								RIO_CopySprToBuffer(pFrame->Sprite, pFrame->Width, pFrame->Height,
									pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							}
							else
							{
								RIO_CopySprToBuffer3LevelAlpha(pFrame->Sprite, pFrame->Width, pFrame->Height,
									pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY);
							}
							break;
						case IMAGE_RENDER_STYLE_OPACITY:
							RIO_CopySprToBuffer(pFrame->Sprite, pFrame->Width, pFrame->Height,
								pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST:
							pPalette = m_ImageStore.GetAdjustColorPalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
							if ( nISPosition < 0 )
							{
								break;
							}
							RIO_CopySprToBufferAlpha(pFrame->Sprite, pFrame->Width, pFrame->Height,
								pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
							// 调色板选择方式绘制
						case IMAGE_RENDER_STYLE_HUE_ADJUST:
							pPalette = m_ImageStore.GetHuePalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
							if ( nISPosition < 0 )
							{
								break;
							}
							RIO_CopySprToBufferAlpha(pFrame->Sprite, pFrame->Width, pFrame->Height,
								pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						}
					}
					else
					{
						switch(pTemp->bRenderStyle)
						{
						case IMAGE_RENDER_STYLE_ALPHA:
						case IMAGE_RENDER_STYLE_ALPHA_NOT_BE_LIT:
							RIO_CopySprToAlphaBufferAlpha(pFrame->Sprite, pFrame->Width, pFrame->Height,
								pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_3LEVEL:
							if ( pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED] == BITS_PIXEL_16 )
							{
								RIO_CopySprToAlphaBuffer(pFrame->Sprite, pFrame->Width, pFrame->Height,
									pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							}
							else
							{
								RIO_CopySprToAlphaBuffer3LevelAlpha(pFrame->Sprite, pFrame->Width, pFrame->Height,
									pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY);
							}
							break;
						case IMAGE_RENDER_STYLE_OPACITY:
							RIO_CopySprToAlphaBuffer(pFrame->Sprite, pFrame->Width, pFrame->Height,
								pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
						case IMAGE_RENDER_STYLE_ALPHA_COLOR_ADJUST:
							pPalette = m_ImageStore.GetAdjustColorPalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
							if ( nISPosition < 0 )
							{
								break;
							}
							RIO_CopySprToAlphaBufferAlpha(pFrame->Sprite, pFrame->Width, pFrame->Height,
								pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;
							// 调色板选择方式绘制
						case IMAGE_RENDER_STYLE_HUE_ADJUST:
							pPalette = m_ImageStore.GetHuePalette(pTemp->uImage, nISPosition, pTemp->Color.Color_dw);
							if ( nISPosition < 0 )
							{
								break;
							}
							RIO_CopySprToAlphaBufferAlpha(pFrame->Sprite, pFrame->Width, pFrame->Height,
								pPalette, pDestBuffer, nDestWidth, nDestHeight, nX, nY, pSprHeader->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED]);
							break;							
						}
					}
				}
				break;
			case ISI_T_BITMAP16:
			case ISI_T_BITMAP16_ALPHA:
				{
					void* pFrame;
					short nISPosition = IMAGE_IS_POSITION_INIT;
					KSGImageContent* pBitmap = (KSGImageContent*)m_ImageStore.GetImage(
						pTemp->szImage,	pTemp->uImage, nISPosition,
						pTemp->nFrame, pTemp->nType, pFrame, pTemp->bMultiThreadLoad);
						if (  nISPosition < 0 )
						{
							break;
						}
					if (pBitmap)
					{
						RIO_CopyBitmap16ToBuffer(pBitmap->Data, pBitmap->nWidth, pBitmap->nHeight, pDestBitmap,
							nDestWidth, nDestHeight, pTemp->oPosition.nX, pTemp->oPosition.nY);
					}
				}
				break;
			}
		}
	}
	break;
	// --> Rocker Edit Start 2005/10/19
	case RU_T_IMAGE_PART:
	{
		KRUImagePart* pTemp = (KRUImagePart *)pPrimitives;
		for (i = 0; i < nPrimitiveCount; i++, pTemp++)
		{
			switch(pTemp->nType)
			{
			case ISI_T_BITMAP16_ALPHA:
				{
					void* pFrame = NULL;
					short nISPosition = IMAGE_IS_POSITION_INIT;
					KSGImageContent* pBitmap = (KSGImageContent*)m_ImageStore.GetImage(
						pTemp->szImage,	pTemp->uImage, nISPosition,
						pTemp->nFrame, pTemp->nType, pFrame, pTemp->bMultiThreadLoad);
						if (  nISPosition < 0 )
						{
							break;
						}
					if (pBitmap)
					{
						int nDstX = pTemp->oPosition.nX;
						int nDstY = pTemp->oPosition.nY;
						int nSrcX = pTemp->oImgLTPos.nX;
						int nSrcY = pTemp->oImgLTPos.nY;
						int nSrcWidth = pTemp->oImgRBPos.nX - pTemp->oImgLTPos.nX;
						int nSrcHeight = pTemp->oImgRBPos.nY - pTemp->oImgLTPos.nY;

						if (nSrcWidth > pBitmap->nWidth)
							nSrcWidth = pBitmap->nWidth;
						if (nSrcHeight > pBitmap->nHeight)
							nSrcHeight = pBitmap->nHeight;
						if (nDstX + nSrcWidth > pDestBitmap->nWidth)
							nSrcWidth = pDestBitmap->nWidth - nDstX;
						if (nDstY + nSrcHeight > pDestBitmap->nHeight)
							nSrcHeight = pDestBitmap->nHeight - nDstY;
						nSrcWidth = (nSrcWidth < 0) ? 0 : nSrcWidth;
						nSrcHeight = (nSrcHeight < 0) ? 0 : nSrcHeight;
						RIO_BltBitmap555ToBuffer(pBitmap->Data, pBitmap->nWidth * 2, nSrcX, nSrcY, nSrcWidth, 
							nSrcHeight, pDestBitmap->Data, pDestBitmap->nWidth * 2, nDstX, nDstY);
					}
				}
				break;

			case ISI_T_8BIT_ALPHA:
				{
					void* pFrame = NULL;
					short nISPosition = IMAGE_IS_POSITION_INIT;
					KSGImageContent* pAlphaBitmap = (KSGImageContent*)m_ImageStore.GetImage(
						pTemp->szImage,	pTemp->uImage, nISPosition,
						pTemp->nFrame, pTemp->nType, pFrame, pTemp->bMultiThreadLoad);
						if (  nISPosition < 0 )
						{
							break;
						}
					if (pAlphaBitmap)
					{
						int nDstX = pTemp->oPosition.nX;
						int nDstY = pTemp->oPosition.nY;
						int nSrcX = pTemp->oImgLTPos.nX;
						int nSrcY = pTemp->oImgLTPos.nY;
						int nSrcWidth = pTemp->oImgRBPos.nX - pTemp->oImgLTPos.nX;
						int nSrcHeight = pTemp->oImgRBPos.nY - pTemp->oImgLTPos.nY;

						if (nSrcWidth > pAlphaBitmap->nWidth)
							nSrcWidth = pAlphaBitmap->nWidth;
						if (nSrcHeight > pAlphaBitmap->nHeight)
							nSrcHeight = pAlphaBitmap->nHeight;
						if (nDstX + nSrcWidth > pDestBitmap->nWidth)
							nSrcWidth = pDestBitmap->nWidth - nDstX;
						if (nDstY + nSrcHeight > pDestBitmap->nHeight)
							nSrcHeight = pDestBitmap->nHeight - nDstY;
						nSrcWidth = (nSrcWidth < 0) ? 0 : nSrcWidth;
						nSrcHeight = (nSrcHeight < 0) ? 0 : nSrcHeight;
						RIO_BltAlphaToBuffer(pAlphaBitmap->Data, pAlphaBitmap->nWidth, nSrcX, nSrcY, nSrcWidth, 
							nSrcHeight, pDestBitmap->Data, pDestBitmap->nWidth * 2, nDstX, nDstY);
					}
				}
				break;
			}
		}
	}
	break;
	// <-- Rocker End
	}
}

//##Documentation
//## 清除图形数据
void KRepresentShell2::ClearImageData(const char* pszImage, unsigned int uImage, short nImagePosition)
{
	void* pFrame;
	 nImagePosition = IMAGE_IS_POSITION_INIT;
	KSGImageContent* pBitmap = (KSGImageContent*)m_ImageStore.GetImage(
			pszImage,	uImage, nImagePosition, 0, ISI_T_BITMAP16, pFrame, false);
	if (  nImagePosition < 0 )
	{
		return;
	}
	if (pBitmap)
		memset(pBitmap->Data, 0, 2 * pBitmap->nWidth * pBitmap->nHeight);
	else
	{
		nImagePosition = IMAGE_IS_POSITION_INIT;
		pBitmap = (KSGImageContent*)m_ImageStore.GetImage(
			pszImage,	uImage, nImagePosition, 0, ISI_T_BITMAP16_ALPHA, pFrame, false);
						if ( nImagePosition < 0 )
						{
							return;
						}
		if(pBitmap)
		{
			memset(pBitmap->Data, 0x80, 2 * pBitmap->nWidth * pBitmap->nHeight);
		}
	}
}

//##ModelId=3DCD8E9200E8
void KRepresentShell2::FreeAllImage()
{
	m_ImageStore.Free();
}

//##ModelId=3DCD8EF60316
void KRepresentShell2::FreeImage(const char* pszImage)
{
	m_ImageStore.FreeImage(pszImage);
}

//##ModelId=3DCD8FA900EE
void* KRepresentShell2::GetBitmapDataBuffer(const char* pszImage, KBitmapDataBuffInfo* pInfo)
{
	unsigned int uImage = 0;
	short		nISPosition = -1;
	void*		pBuffer = NULL;

	LPDIRECTDRAWSURFACE pSurface;
	nISPosition = IMAGE_IS_POSITION_INIT;
	KSGImageContent* pDestBitmap = (KSGImageContent *)m_ImageStore.GetImage(
					pszImage, uImage, nISPosition, 0, ISI_T_BITMAP16, (void*&)pSurface, false);

	if ( nISPosition < 0 )
	{
		return NULL;
	}
	if (!pDestBitmap)
	{
		LPDIRECTDRAWSURFACE pSurface;
		nISPosition = IMAGE_IS_POSITION_INIT;
		pDestBitmap = (KSGImageContent *)m_ImageStore.GetImage(
			pszImage, uImage, nISPosition, 0, ISI_T_BITMAP16_ALPHA, (void*&)pSurface, false);
		if ( nISPosition < 0)
		{
			return NULL;
		}
	}

	if (pDestBitmap)
	{
		int nPitch = pDestBitmap->nWidth * 2;
		pBuffer = pDestBitmap->Data;
		if (pSurface)
		{
			DDSURFACEDESC	desc;
			desc.dwSize = sizeof(desc);
			if (pSurface->Lock(NULL, &desc, DDLOCK_WAIT, NULL) == DD_OK)
			{
				pBuffer = desc.lpSurface;
				nPitch = desc.lPitch;
			}
		}

		 if (pInfo)
		 {
			pInfo->nWidth = pDestBitmap->nWidth;
			pInfo->nHeight = pDestBitmap->nHeight;
			pInfo->nPitch = nPitch;
			pInfo->pData = pBuffer;
			pInfo->eFormat = (m_DirectDraw.GetRGBBitMask16() == RGB_565) ? BDBF_16BIT_565 : BDBF_16BIT_555;
		}
	}
	return pBuffer;
}

//##释放对(通过GetBitmapDataBuffer调用获取得的)图形像点数据缓冲区的控制
void KRepresentShell2::ReleaseBitmapDataBuffer(const char* pszImage, void* pBuffer)
{
	unsigned int uImage = 0;
	short		nISPosition = -1;
	LPDIRECTDRAWSURFACE pSurface;
	nISPosition = IMAGE_IS_POSITION_INIT;
	KSGImageContent* pDestBitmap = (KSGImageContent *)m_ImageStore.GetImage(
					pszImage, uImage, nISPosition, 0, ISI_T_BITMAP16, (void*&)pSurface, false);
	if ( nISPosition < 0 )
		return;
	if (pSurface && pDestBitmap)
	{
		pSurface->Unlock(NULL);
	}
}

//##ModelId=3DCA6EBC000F
bool KRepresentShell2::GetImageParam(const char* pszImage, KImageParam* pImageData, int nType) 
{
	return m_ImageStore.GetImageParam(pszImage, nType, pImageData);
}

bool KRepresentShell2::GetImageFrameParam(const char* pszImage, int nFrame,
			KRPosition2* pOffset, KRPosition2* pSize, int nType)
{
	return m_ImageStore.GetImageFrameParam(pszImage, nType, nFrame, pOffset, pSize);
}

//##ModelId=3DCA72620157
int KRepresentShell2::GetImagePixelAlpha(const char* pszImage, int nFrame, int nX, int nY, int nType)
{
	return m_ImageStore.GetImagePixelAlpha(pszImage, nType, nFrame, nX, nY);
}

//##ModelId=3DC0A08D0085
void KRepresentShell2::LookAt(int nX, int nY, int nZ)
{
	m_nLeft = nX - m_Canvas.GetWidth() / 2;
	m_nTop  = nY / 2 - ((nZ * 887) >> 10) - m_Canvas.GetHeight() / 2;
}

//##ModelId=3DCA0BAE00E4
void KRepresentShell2::OutputText(int nFontId, const char* psText, int nCount, int nX, int nY, unsigned int Color, int nLineWidth, int nZ, unsigned int BorderColor)
{
	// add by chenshanglin on 2005-10-17 for game video
	if(m_nDrawFlag == 0)
	{
		return;
	}
	// add end

	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i < RS2_MAX_FONT_ITEM_NUM && m_FontTable[i].pFontObj)
	{
		if (nZ != TEXT_IN_SINGLE_PLANE_COORD)
			CoordinateTransform(nX, nY, nZ);
		m_FontTable[i].pFontObj->SetBorderColor(BorderColor);
		m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
		m_FontTable[i].pFontObj->OutputText(psText, nCount, nX, nY, Color, nLineWidth);
	}
}

//##ModelId=3DCA0BAE00E4
void KRepresentShell2::OutputTextInRect(int nFontId, const char* psText, int nCount, int nX, int nY, RECT *pRect, unsigned int Color, int nLineWidth, int nZ, unsigned int BorderColor)
{
	// add by chenshanglin on 2005-10-17 for game video
	if(m_nDrawFlag == 0)
	{
		return;
	}
	// add end

	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i < RS2_MAX_FONT_ITEM_NUM && m_FontTable[i].pFontObj)
	{
		if (nZ != TEXT_IN_SINGLE_PLANE_COORD)
			CoordinateTransform(nX, nY, nZ);
		if ( pRect != NULL )
		{
			RECT old;
			m_Canvas.GetClipRect(&old);
			m_Canvas.SetClipRect(pRect);
			m_FontTable[i].pFontObj->SetBorderColor(BorderColor);
			m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
			m_FontTable[i].pFontObj->OutputText(psText, nCount, nX, nY, Color, nLineWidth);
			m_Canvas.SetClipRect(&old);
		}
		else
		{	
			m_FontTable[i].pFontObj->SetBorderColor(BorderColor);
			m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
			m_FontTable[i].pFontObj->OutputText(psText, nCount, nX, nY, Color, nLineWidth);
		}	
		
	}
}

//##ModelId=3DCA0BAE00E4
void KRepresentShell2::OutputTextOnImage( const char* pszImage, unsigned int uImage, short& nImagePosition, int nFontId, const char* psText, int nCount, int nX, int nY, unsigned int Color, int nLineWidth, int nZ, unsigned int BorderColor)
{
	// add by chenshanglin on 2005-10-17 for game video
	if(m_nDrawFlag == 0)
	{
		return;
	}
	// add end

	int nType;
	nImagePosition = IMAGE_IS_POSITION_INIT;
	KSGImageContent* pDestBitmap = (KSGImageContent*)m_ImageStore.GetExistedCreateBitmap(
		pszImage, uImage, nImagePosition, &nType);

	if (pDestBitmap == NULL)
		return;

	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i < RS2_MAX_FONT_ITEM_NUM && m_FontTable[i].pFontObj)
	{
		if (nZ != TEXT_IN_SINGLE_PLANE_COORD)
			CoordinateTransform(nX, nY, nZ);
		m_FontTable[i].pFontObj->SetBorderColor(BorderColor);
		m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);

		RECT rcOld;
		m_Canvas.GetClipRect(&rcOld);
		RECT rcClip;
		rcClip.left = 0;
		rcClip.top = 0;
		rcClip.right = pDestBitmap->nWidth;
		rcClip.bottom = pDestBitmap->nHeight;
		m_Canvas.SetClipRect(&rcClip);
		m_FontTable[i].pFontObj->OutputTextOnImage(pDestBitmap, psText, nCount, nX, nY, Color, nLineWidth);
		m_Canvas.SetClipRect(&rcOld);
	}
}

//##ModelId=3DB655B2000E
//##Documentation
//## 输出文字。
int KRepresentShell2::OutputRichText(int nFontId, KOutputTextParam* pParam, 
		const char* psText, int nCount, int nLineWidth)
{
	// add by chenshanglin on 2005-10-17 for game video
	if(m_nDrawFlag == 0)
	{
		return 0;
	}
	// add end
	
	if (pParam == NULL)
		return 0;
	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i < RS2_MAX_FONT_ITEM_NUM && m_FontTable[i].pFontObj)
	{
		KTextProcess	tp(psText, nCount, nLineWidth * 2 / nFontId);
		if (pParam->nZ != TEXT_IN_SINGLE_PLANE_COORD)
		{
			int x, y, z;
			x = pParam->nX;
			y = pParam->nY;
			z = pParam->nZ;
			CoordinateTransform(x, y, z);
			pParam->nX = x;
			pParam->nY = y;
		}
		m_FontTable[i].pFontObj->SetBorderColor(pParam->BorderColor);
		m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
		return tp.DrawTextLine(m_FontTable[i].pFontObj, nFontId, pParam);
	}
	return 0;
}


//##ModelId=3DB655B2000E
//##Documentation
//## 输出文字。
int KRepresentShell2::OutputRichTextInRect(int nFontId, KOutputTextParam* pParam,   
										   const char* psText, int nCount /* = KRF_ZERO_END */,  
										   RECT *pRect /* = NULL */,   
										   int nLineWidth /* = 0 */)
{
	// add by chenshanglin on 2005-10-17 for game video
	if(m_nDrawFlag == 0)
	{
		return 0;
	}
	// add end
	
	if (pParam == NULL)
		return 0;
	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i < RS2_MAX_FONT_ITEM_NUM && m_FontTable[i].pFontObj)
	{
		KTextProcess	tp(psText, nCount, nLineWidth * 2 / nFontId);
		if (pParam->nZ != TEXT_IN_SINGLE_PLANE_COORD)
		{
			int x, y, z;
			x = pParam->nX;
			y = pParam->nY;
			z = pParam->nZ;
			CoordinateTransform(x, y, z);
			pParam->nX = x;
			pParam->nY = y;
		}
		
		if ( pRect != NULL )
		{
			int nRet;
			RECT old;
			m_Canvas.GetClipRect(&old);
			m_Canvas.SetClipRect(pRect);
			m_FontTable[i].pFontObj->SetBorderColor(pParam->BorderColor);
			m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
			nRet = tp.DrawTextLine(m_FontTable[i].pFontObj, nFontId, pParam);
			m_Canvas.SetClipRect(&old);
			return nRet;
		}
		else
		{	
			m_FontTable[i].pFontObj->SetBorderColor(pParam->BorderColor);
			m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
			return tp.DrawTextLine(m_FontTable[i].pFontObj, nFontId, pParam);
		}
		
	}
	return 0;
}

//##ModelId=3DB655B2000E
//##Documentation
//## 输出文字。
int KRepresentShell2::OutputRichTextOnImage(const char* szImage, unsigned int uImage,
		short& nImagePosition, int nFontId, KOutputTextParam* pParam, 
		const char* psText, int nCount, int nLineWidth)
{
	// add by chenshanglin on 2005-10-17 for game video
	if(m_nDrawFlag == 0)
	{
		return 0;
	}
	// add end
	
	if (pParam == NULL)
		return 0;
	
	int nType;
	nImagePosition = IMAGE_IS_POSITION_INIT;
	KSGImageContent* pDestBitmap = (KSGImageContent*)m_ImageStore.GetExistedCreateBitmap(
		szImage, uImage, nImagePosition, &nType);

	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i < RS2_MAX_FONT_ITEM_NUM && m_FontTable[i].pFontObj)
	{
		KTextProcess	tp(psText, nCount, nLineWidth * 2 / nFontId);
		if (pParam->nZ != TEXT_IN_SINGLE_PLANE_COORD)
		{
			int x, y, z;
			x = pParam->nX;
			y = pParam->nY;
			z = pParam->nZ;
			CoordinateTransform(x, y, z);
			pParam->nX = x;
			pParam->nY = y;
		}
		m_FontTable[i].pFontObj->SetBorderColor(pParam->BorderColor);
		m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
				RECT rcOld;
		m_Canvas.GetClipRect(&rcOld);
		RECT rcClip;
		rcClip.left = 0;
		rcClip.top = 0;
		rcClip.right = pDestBitmap->nWidth;
		rcClip.bottom = pDestBitmap->nHeight;
		m_Canvas.SetClipRect(&rcClip);
		
		
		int nRet = tp.DrawTextLineOnImage(pDestBitmap, szImage, uImage, 
			nImagePosition, m_FontTable[i].pFontObj, nFontId, pParam);
		m_Canvas.SetClipRect(&rcOld);
		return nRet;
	}
	return 0;
}


//## 返回指定坐标在字符串中最近的字符偏移
int KRepresentShell2::LocateRichText(int nX, int nY,
						int nFontId, KOutputTextParam* pParam, 
						const char* psText, int nCount, int nLineWidth)
{
	if (pParam == NULL)
		return -1;
	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nFontId)
			break;
	}
	if (i < RS2_MAX_FONT_ITEM_NUM && m_FontTable[i].pFontObj)
	{
		KTextProcess	tp(psText, nCount, nLineWidth * 2 / nFontId);
		if (pParam->nZ != TEXT_IN_SINGLE_PLANE_COORD)
		{
			int x, y, z;
			x = pParam->nX;
			y = pParam->nY;
			z = pParam->nZ;
			CoordinateTransform(x, y, z);
			pParam->nX = x;
			pParam->nY = y;
		}
		m_FontTable[i].pFontObj->SetBorderColor(pParam->BorderColor);
		m_FontTable[i].pFontObj->SetOutputSize(nFontId, nFontId + 1);
		return tp.TransXYPosToCharOffset(nX, nY, m_FontTable[i].pFontObj, nFontId, pParam);
	}
	return -1;
}

//##ModelId=3DCA72E102FE
void KRepresentShell2::Release()
{
	m_Canvas.Terminate();
	ShutdownGdiplus();
	delete this;
}

//##ModelId=3DCA0B8102F3
void KRepresentShell2::ReleaseAFont(int nId)
{
	for (int i = 0; i < RS2_MAX_FONT_ITEM_NUM; i++)
	{
		if (m_FontTable[i].nId == nId)
		{
			m_FontTable[i].nId = 0;
			if (m_FontTable[i].pFontObj)
			{
				m_FontTable[i].pFontObj->Release();
				m_FontTable[i].pFontObj = NULL;
			}
			break;
		}
	}
}

//##ModelId=3DB69EC0023A
bool KRepresentShell2::ProcessSurfaceLost(int nWidth,int nHeight,bool bFullScreen)
{
	m_Canvas.Terminate();
//	m_DirectDraw.Mode(bFullScreen, nWidth, nHeight);
	if( m_DirectDraw.ProcessSurfaceLost())
	{
		m_Canvas.Init(nWidth, nHeight);
		///		m_ImageStore.Init();
		RIO_Set16BitImageFormat(m_DirectDraw.GetRGBBitMask16() == RGB_565);
		//		// 初始化Gdi+
		////		InitGdiplus();
		s_Timer.Start();
		if(m_DirectDraw.GetScreenMode() == WINDOWMODE)
		{
			// 窗口模式参数设定
			
			RECT r ={0,0, nWidth, nHeight};
			::AdjustWindowRect(&r, WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX, FALSE);
			
			HWND hWnd = g_GetMainHWnd();
			
			int nDesktopWidth = GetSystemMetrics(SM_CXSCREEN);
			int nDesktopHeight = GetSystemMetrics(SM_CYSCREEN);
			
			//--> Rocker 2004/08/16 使得窗口居中
			::MoveWindow(hWnd, (nDesktopWidth - (r.right - r.left)) / 2, 
				(nDesktopHeight - (r.bottom - r.top)) / 2, r.right - r.left, r.bottom - r.top, TRUE);
			//<-- End
		
		}
		return TRUE;
	}
	return FALSE;
}
bool KRepresentShell2::Reset(int nWidth, int nHeight, bool bFullScreen)
{
	m_Canvas.Terminate();
	m_DirectDraw.Exit();
	m_DirectDraw.Mode(bFullScreen, nWidth, nHeight);

	if (m_DirectDraw.Reset())
	{
		m_Canvas.Init(nWidth, nHeight);
///		m_ImageStore.Init();
		RIO_Set16BitImageFormat(m_DirectDraw.GetRGBBitMask16() == RGB_565);
//		// 初始化Gdi+
////		InitGdiplus();
		s_Timer.Start();
		if(m_DirectDraw.GetScreenMode() == WINDOWMODE)
		{
			// 窗口模式参数设定

			RECT r ={0,0, nWidth, nHeight};
			::AdjustWindowRect(&r, WS_VISIBLE | WS_SYSMENU | WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX, FALSE);

			HWND hWnd = g_GetMainHWnd();
			
			int nDesktopWidth = GetSystemMetrics(SM_CXSCREEN);
			int nDesktopHeight = GetSystemMetrics(SM_CYSCREEN);
			
			//--> Rocker 2004/08/16 使得窗口居中
			::MoveWindow(hWnd, (nDesktopWidth - (r.right - r.left)) / 2, 
				(nDesktopHeight - (r.bottom - r.top)) / 2, r.right - r.left, r.bottom - r.top, TRUE);
			//<-- End
		}
		return true;
	}
	return false;
}

//##ModelId=3DCD90910361
bool KRepresentShell2::SaveImage(const char* pszFile, const char* pszImage, int nFileType)
{
	return m_ImageStore.SaveImage(pszFile, pszImage, nFileType);
}

//##ModelId=3DCD90F30011
void KRepresentShell2::SetImageStoreBalanceParam(int nNumImage, unsigned int uCheckPoint)
{
	m_ImageStore.SetBalanceParam(nNumImage, uCheckPoint);
}

//##ModelId=3DD00EEE0149
bool KRepresentShell2::CopyDeviceImageToImage(const char* pszName, int nDeviceX, int nDeviceY, int nImageX, int nImageY, int nWidth, int nHeight)
{
	if (nWidth > m_Canvas.GetWidth() - nDeviceX || nHeight > m_Canvas.GetHeight() - nDeviceY)
		return false;

	short nISPosition = IMAGE_IS_POSITION_INIT;
	KSGImageContent* pBitmap = (KSGImageContent*)m_ImageStore.GetExistedCreateBitmap(
		pszName, 0, nISPosition);

	if (pBitmap)
	{
		if (pBitmap->nWidth >= nImageX + nWidth && pBitmap->nHeight >= nImageY + nHeight)
		{
			int nPitch;
			void* pDevice = m_Canvas.LockCanvas(nPitch);
			if (pDevice)
			{
				unsigned short*	pBuffer = &pBitmap->Data[pBitmap->nWidth * nImageY + nImageX];
				pDevice = (char*)pDevice + nDeviceY * nPitch + nDeviceX * 2;
				int nCopyLen = nWidth * 2;
				for (int i = 0; i < nHeight; i++)
				{
					memcpy(pBuffer, pDevice, nCopyLen);
					pBuffer += pBitmap->nWidth;
					pDevice = (char*)pDevice + nPitch;
				}
				m_Canvas.UnlockCanvas();
				return true;
			}
		}
	}
	return false;
}

//##ModelId=3DCFED410049
void KRepresentShell2::CoordinateTransform(int& nX, int& nY, int nZ)
{
	nX = nX - m_nLeft;
	nY = nY / 2 - m_nTop - ((nZ * 887) >> 10);	// * sqrt(3) / 2
}

//##ModelId=3DD20C45002A
bool KRepresentShell2::RepresentBegin(int bClear, unsigned int Color)
{
	static int	s_nFrameRate = 30;
	//s_Timer.GetFPS(&s_nFrameRate);
	static	int snLoop = 0;
//	if ((++snLoop) == 36)
//	{
//		snLoop = 0;
//		s_FastDrawMode = (s_nFrameRate < 18) && (!m_bHighQuality);
//	}
	s_FastDrawMode = !m_bHighQuality;
	m_ImageStore.NextTrun();
	
//	m_Canvas.LockSurface();

	//	if (bClear)
//	{
//		KRColor	c;
//		c.Color_dw = Color;
//		m_Canvas.FillCanvas(g_RGB(c.Color_b.r, c.Color_b.g, c.Color_b.b));
//	}
	int nPitch;
	m_Canvas.LockCanvas(nPitch);
	return true;
}

//##ModelId=3DD20C450066
void KRepresentShell2::RepresentEnd()
{
//	m_Canvas.UnLockSurface();
	m_Canvas.UnlockCanvas();
//	m_Canvas.Changed(true);
//	m_Canvas.UpdateScreen();
}
void KRepresentShell2::BltBackBufferToSurface(void* pSurface,int x,int y,int width,int height)
{

	m_Canvas.BitBltToSurface((LPDIRECTDRAWSURFACE)pSurface,x,y,width,height);
}
void KRepresentShell2::UpdataSceen()
{
	m_Canvas.Changed(true);
	m_Canvas.UpdateScreen();
}
//视图/绘图设备坐标 转化为空间坐标
void KRepresentShell2::ViewPortCoordToSpaceCoord(int& nX,	int& nY, int  nZ)
{
	nX = nX + m_nLeft;
	nY = (nY + m_nTop + ((nZ * 887) >> 10)) * 2;
}
/*
bool KRepresentShell2::SaveScreenToFile(const char* pszName)
{
	if(!pszName || !pszName[0])
		return 0;

	DWORD n = m_Canvas.m_nWidth;

	int nPicWidth, nPicHeight, nDesktopWidth, nDesktopHeight, nPicOffX, nPicOffY;
	{
		//全屏模式参数设定
		nPicOffX = 0;
		nPicOffY = 0;
		nDesktopWidth = nPicWidth = m_Canvas.m_nWidth;
		nDesktopHeight = nPicHeight = m_Canvas.m_nHeight;
	}

	WORD *pSrc;
	BYTE *pDes, *pTemp;

	// 分配r8g8b8缓冲区	
	pTemp = pDes = new BYTE[nPicWidth * nPicHeight * 3];
	if(!pDes)
		return false;

	pSrc = (WORD*)m_Canvas.m_pCanvas;
	
	// 拷贝屏幕数据到缓冲区
	for(int i=0; i<nPicHeight; i++)
	{
		for(int j=0; j<nPicWidth; j++)
		{
			if(m_DirectDraw.GetRGBBitMask16() == RGB_565)
			{
				pDes[2] = ((*pSrc) & 0xf800) >> 8;
				pDes[1] = ((*pSrc) & 0x07e0) >> 3;
				pDes[0] = ((*pSrc) & 0x001f) << 3;
			}
			else
			{
				pDes[2] = ((*pSrc) & 0x7c00) >> 7;
				pDes[1] = ((*pSrc) & 0x03e0) >> 2;
				pDes[0] = ((*pSrc) & 0x001f) << 3;
			}
			pDes += 3;
			pSrc++;
		}
	}

	// 生成24位bmp文件
	if(!KBmpFile24::SaveBuffer24((char*)pszName, pTemp, nPicWidth*3, nPicWidth, nPicHeight))
	{
		delete[] pTemp;
		return false;
	}

	delete[] pTemp;
	return true;
}*/

bool KRepresentShell2::SaveScreenToFile(const char* pszName, ScreenFileType eType, unsigned int nQuality)
{
	if(!pszName || !pszName[0])
		return 0;

	DWORD n = m_Canvas.GetWidth();

	int nPicWidth, nPicHeight, nDesktopWidth, nDesktopHeight, nPicOffX, nPicOffY;
	if(m_DirectDraw.GetScreenMode() == WINDOWMODE)
	{
		// 窗口模式参数设定
		RECT rect;
		POINT ptLT, ptRB;
		HWND hWnd = g_GetMainHWnd();
		GetClientRect(hWnd, &rect);
		ptLT.x = rect.left, ptLT.y = rect.top;
		ptRB.x = rect.right, ptRB.y = rect.bottom;
		ClientToScreen(hWnd, &ptLT);
		ClientToScreen(hWnd, &ptRB);

		nDesktopWidth = m_DirectDraw.GetScreenWidth();
		nDesktopHeight = m_DirectDraw.GetScreenHeight();

		// 如果窗口客户区超出屏幕则返回
		if(ptLT.x >= nDesktopWidth || ptLT.y >= nDesktopHeight || ptRB.x <= 0 || ptRB.y <= 0)
			return false;
		if(ptLT.x < 0)
			ptLT.x = 0;
		if(ptLT.y < 0)
			ptLT.y = 0;
		if(ptRB.x > nDesktopWidth)
			ptRB.x = nDesktopWidth - 1;
		if(ptRB.y > nDesktopHeight)
			ptRB.y = nDesktopHeight - 1;

		nPicOffX = ptLT.x;
		nPicOffY = ptLT.y;
		nPicWidth = ptRB.x - ptLT.x;
		nPicHeight = ptRB.y - ptLT.y;
	}
	else
	{
		//全屏模式参数设定
		nPicOffX = 0;
		nPicOffY = 0;
		nDesktopWidth = nPicWidth = m_Canvas.GetWidth();
		nDesktopHeight = nPicHeight = m_Canvas.GetHeight();
	}

	WORD *pSrc;
	BYTE *pDes, *pTemp;

	// 分配r8g8b8缓冲区	
	pTemp = pDes = new BYTE[nPicWidth * nPicHeight * 3];
	if(!pDes)
		return false;

	// --> Rocker Edit Start 2006/03/24
	//if((pSrc = (WORD*)m_DirectDraw.LockPrimaryBuffer()) == NULL)
	int nPitch = 0;
	if((pSrc = (WORD*)m_Canvas.LockCanvas(nPitch)) == NULL)
	{
		delete[] pTemp;
		return false;
	}
	//	int nPitch = m_DirectDraw.GetScreenPitch();
	nPicOffX = 0;		// LOCK的是后缓冲，显示区域大小，没有偏移量
	nPicOffY = 0;
	// <-- Rocker End

	pSrc += nPicOffY * nPitch / 2 + nPicOffX;
	int nLineAdd = nPitch / 2 - nPicWidth;
	
	// 拷贝屏幕数据到缓冲区
	for(int i=0; i<nPicHeight; i++)
	{
		for(int j=0; j<nPicWidth; j++)
		{
			if(m_DirectDraw.GetRGBBitMask16() == RGB_565)
			{
				pDes[2] = ((*pSrc) & 0xf800) >> 8;
				pDes[1] = ((*pSrc) & 0x07e0) >> 3;
				pDes[0] = ((*pSrc) & 0x001f) << 3;
			}
			else
			{
				pDes[2] = ((*pSrc) & 0x7c00) >> 7;
				pDes[1] = ((*pSrc) & 0x03e0) >> 2;
				pDes[0] = ((*pSrc) & 0x001f) << 3;
			}
			pDes += 3;
			pSrc++;
		}
		pSrc += nLineAdd;
	}

	//m_DirectDraw.UnLockPrimaryBuffer();
	m_Canvas.UnlockCanvas();

	BOOL bRet;
	if(eType == SCRFILETYPE_BMP)
		// 保存24位bmp文件
		bRet = KBmpFile24::SaveBuffer24((char*)pszName, pTemp, nPicWidth*3, nPicWidth, nPicHeight);
	else
		// 保存24位jpg文件
		bRet = SaveBufferToJpgFile24((char*)pszName, pTemp, nPicWidth*3, nPicWidth, nPicHeight, nQuality);
	if(!bRet)
	{
		delete[] pTemp;
		return false;
	}

	delete[] pTemp;
	return true;
}

//## 设置表现模块选项
void KRepresentShell2::SetOption(RepresentOption eOption, INT bOn)
{
	switch(eOption)
	{
	case HIGH_QUALITY_PAINTING:
		m_bHighQuality = bOn;
		break;
	case FRAMECUT_LEVE:
		g_FrameCutLevel = bOn;
		break;
	}
}

/*
//## 获取显示参数
void KRepresentShell2::GetParam(RepresentParam eParam, int nParam)
{
	switch(eParam)
	{
	case HIGH_QUALITY_PAINTING_PARAM:
		{
			int *pnNum = (int *)nParam;
			*pnNum = m_bHighQuality;
		}
		break;
	}
}
*/
bool KRepresentShell2::PreLoadImage(const char* pszImageFile, unsigned int nType, MemoryResidentParam* pResidentParam)
{
	KImageParam ImageParam;
	if (m_ImageStore.GetImageParam(pszImageFile , nType, &ImageParam))
	{
		for(int i = 0; i < ImageParam.nNumFrames; i++)
		{
			m_ImageStore.GetImageFrameParam(pszImageFile, nType, i, NULL, NULL);
		}
		return true;
	}	
	return false;
}

void KRepresentShell2::FreeImageByID(unsigned int nImageID)
{
	m_ImageStore.FreeImage(nImageID);
}
