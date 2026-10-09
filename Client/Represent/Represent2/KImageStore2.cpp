/*****************************************************************************************
//  无贴图的图形资源管理
//	Copyright : Kingsoft 2002-2003
//	Author	: Wooy(Wu yue)
//	CreateTime:	2002-11-11
*****************************************************************************************/
#include "KEngine.h"
#include "KImageStore2.h"
#include "KRepresentUnit.h"
#include "ImageOperation.h"
#include <crtdbg.h.>
#include "KColors.h"

#define CHECK_TIME 5000

int g_FrameCutLevel = 1;

// 取得指定调色板,不存在时返回默认调色板
char* KImageStore2::GetHuePalette(unsigned int uImage, short& nISPosition, unsigned int uHue)
{
	char * pPalette = NULL;
	uHue &= 0x00FFFFFF;
	if (uHue < 360)
	{
		nISPosition = FindImage(uImage, nISPosition);
		if (nISPosition >= 0)
		{
			_KISImageObj& Img = m_pObjectList[nISPosition];
			if ( Img.bLoading )
			{
				pPalette = (char*)Img.pObject + sizeof(SPRHEAD);
			}
			if (Img.pObject && Img.bType == ISI_T_SPR )
			{
				// 第一次创建调色板
				if (Img.pExtPalettes == NULL)
				{
					Img.nExtPalCount = 0;
					Img.pExtPalettes = (char*)malloc(sizeof(KPAL16) * ((SPRHEAD*)(Img.pObject))->Colors * MAX_HUE_ADJUST_PAL);
				}
				
				// 调色板空间存在
				if (Img.pExtPalettes != NULL)
				{
					// 查找指定Hue的调色板
					for (unsigned int i = 0; i < Img.nExtPalCount; i ++)
					{
						if (Img.nExtPalHue[i] == uHue)
						{
							pPalette = Img.pExtPalettes + (sizeof(KPAL16) * i * ((SPRHEAD*)Img.pObject)->Colors);
						}
					}
					// 没找到
					if (pPalette == NULL) 
					{
						if (uHue > 0)
						{
							if(Img.nExtPalCount < MAX_HUE_ADJUST_PAL)
							{
								const unsigned int uColors = ((SPRHEAD*)(Img.pObject))->Colors;
								char* pOrigPalette = (char*)Img.pObject + sizeof(SPRHEAD);
								KPAL24	OrigPalette[256];
								g_Pal16ToPal24((KPAL16*)pOrigPalette, OrigPalette, uColors);					
								pPalette = Img.pExtPalettes + (Img.nExtPalCount * sizeof(KPAL16) * uColors);
								g_Pal24To16AdjustHue((KPAL24*)OrigPalette, (KPAL16*)pPalette, uColors, uHue);
								Img.nExtPalHue[Img.nExtPalCount++] = uHue;
							}
							else
							{
								const unsigned int uColors = ((SPRHEAD*)(Img.pObject))->Colors;
								char* pOrigPalette = (char*)Img.pObject + sizeof(SPRHEAD);
								KPAL24	OrigPalette[256];
								g_Pal16ToPal24((KPAL16*)pOrigPalette, OrigPalette, uColors);					
								pPalette = Img.pExtPalettes ;//+ (Img.nExtPalCount * sizeof(KPAL16) * uColors);
								g_Pal24To16AdjustHue((KPAL24*)OrigPalette, (KPAL16*)pPalette, uColors, uHue);
								Img.nExtPalHue[0] = uHue;
							}
						}
						else
						{
							pPalette = (char*)Img.pObject + sizeof(SPRHEAD);
						}
					}
				}
			}
		}
	}
	
	return pPalette;	
}


char* KImageStore2::GetAdjustColorPalette(unsigned int uImage,
					short& nISPosition, unsigned uColor)
{
	char* pPalette = NULL;

	nISPosition = FindImage(uImage, nISPosition);
	if (nISPosition >= 0)
	{
		_KISImageObj& Img = m_pObjectList[nISPosition];
		if ( Img.bLoading )
		{
			pPalette = (char*)Img.pObject + sizeof(SPRHEAD);
		}
		if (Img.pObject && Img.bType == ISI_T_SPR )
		{
			if (Img.pcAdjustColorPalettes == NULL)
			{
				char* pOrigPalette = ((char*)Img.pObject) + sizeof(SPRHEAD);
				Img.pcAdjustColorPalettes = CreateAdjustColorPalette(
					pOrigPalette, ((SPRHEAD*)Img.pObject)->Colors);
			}
            if (Img.pcAdjustColorPalettes)
			{
				for (unsigned int i = 0; i < m_uNumSprAdustColor; i ++)
				{
					if ((uColor & 0xffffff) == m_uSprAdjustColorList[i])
					{
						pPalette = Img.pcAdjustColorPalettes + (2 * i * ((SPRHEAD*)Img.pObject)->Colors);
						break;
					}
				}
			}
			if (pPalette == NULL)
				pPalette = ((char*)Img.pObject) + sizeof(SPRHEAD);
		}
	}
	return pPalette;
}

char* KImageStore2::CreateAdjustColorPalette(const char* pOrigPalette, int nNumColor)
{
	return NULL;

	char* pPalette = NULL;
	if (pOrigPalette && nNumColor > 0 && m_uNumSprAdustColor)
	{
		_ASSERT(nNumColor <= 256);
		pPalette = (char*)malloc(2 * nNumColor * m_uNumSprAdustColor);
		if (pPalette)
		{
			KPAL24	OrigPalette[256];
			g_Pal16ToPal24((KPAL16*)pOrigPalette, OrigPalette, nNumColor);

			unsigned short*	pusACP = (unsigned short*)pPalette;
			for (unsigned int i = 0; i < m_uNumSprAdustColor; i++)
			{
				unsigned int	uAR = (m_uSprAdjustColorList[i] >> 16) & 0xff;
				unsigned int	uAG = (m_uSprAdjustColorList[i] >> 8) & 0xff;
				unsigned int	uAB = m_uSprAdjustColorList[i] & 0xff;
				for (int nIndex = 0; nIndex < nNumColor; nIndex++)
				{
					unsigned int uR = uAR * OrigPalette[nIndex].Red / 256;
					unsigned int uG = uAG * OrigPalette[nIndex].Green / 256;
					unsigned int uB = uAB * OrigPalette[nIndex].Blue / 256;
					*(pusACP++) = g_RGB(uR, uG, uB);
				}
			}
		}
	}
	return pPalette;
}

//设置偏色列表
unsigned int KImageStore2::SetAdjustColorList(unsigned int* puColorList, unsigned int uCount)
{
	if (uCount > MAX_ADJUSTABLE_COLOR_NUM)
		uCount = MAX_ADJUSTABLE_COLOR_NUM;
	ClearAllAdjustColorPalette();
	if (puColorList && uCount)
	{
		memcpy(m_uSprAdjustColorList, puColorList, sizeof(unsigned int) * uCount);
		for (unsigned int i = 0; i < uCount; i++)
		{
			m_uSprAdjustColorList[i] &= 0xffffff;
		}
		m_uNumSprAdustColor = uCount;
	}
	else
	{
		m_uNumSprAdustColor = 0;
	}
	return m_uNumSprAdustColor;
}

#define	KSG_ALPHAIMAGE_CONTENT_SIZE(w, h)    ((unsigned)((&((KSGImageContent *)0)->Data[0])) + w * h)
unsigned int KImageStore2::CreateImage(const char* pszName, int nWidth, int nHeight, int nType)
{
	unsigned int uImage = g_FileName2Id((char*)pszName);
	KSGImageContent* pBitmap = NULL;
	int nIdx;


    KAutoCriticalSection AutoLock(m_ImageProcessLock);
	if (nWidth > 0 && nHeight > 0 && uImage)
	{
		switch(nType) 
		{
		case ISI_T_BITMAP16:
		case ISI_T_BITMAP16_ALPHA:
			{
				nIdx = FindImage(uImage, 0);
				if (nIdx < 0 &&	//必须是不存在同id的图形
					(m_nNumImages < m_nNumReserved || ExpandSpace()))	//有空间存放图形对象
				{
					pBitmap = (KSGImageContent *)malloc(KSG_IMAGE_CONTENT_SIZE(nWidth, nHeight));
				}
				else if (nIdx >= 0)
				{
					// 如果找到1个，简单改变图的类型并返回
					m_pObjectList[nIdx].bType = nType;
					return m_pObjectList[nIdx].uId;
				}
			}
			break;
		case ISI_T_8BIT_ALPHA:
			{
				nIdx = FindImage(uImage, 0);
				if (nIdx < 0 &&	//必须是不存在同id的图形
					(m_nNumImages < m_nNumReserved || ExpandSpace()))	//有空间存放图形对象
				{
					pBitmap = (KSGImageContent *)malloc(KSG_ALPHAIMAGE_CONTENT_SIZE(nWidth, nHeight));
				}
				else if (nIdx >= 0)
				{
					m_pObjectList[nIdx].bType = nType;
					return m_pObjectList[nIdx].uId;
				}
			}
			break;
		default:
			break;
		}
	}

	if (pBitmap)
	{
		pBitmap->nWidth = nWidth;
		pBitmap->nHeight = nHeight;

		nIdx = - nIdx - 1;
		for (int i = m_nNumImages; i > nIdx; i--)
		{
			m_pObjectList[i] = m_pObjectList[i - 1];
		}
		m_pObjectList[nIdx].bNotCacheable = true;
		m_pObjectList[nIdx].bSingleFrameLoad = false;
		m_pObjectList[nIdx].bType = (KIS_IMAGE_TYPE)nType;
		m_pObjectList[nIdx].pObject = pBitmap;
		m_pObjectList[nIdx].pFrames = NULL;
		m_pObjectList[nIdx].uId = uImage;
		m_pObjectList[nIdx].pSurface = NULL;
		m_pObjectList[nIdx].nRef = ::GetTickCount();
		m_nNumImages++;
	}
	else
		uImage = 0;
	return uImage;
}

void KImageStore2::Free()
{
	KAutoCriticalSection AutoLock(m_ImageProcessLock);
	if (m_pObjectList)
	{
        for (int i = 0; i < m_nNumImages; i++)
			FreeImageObject(m_pObjectList[i]);
		free(m_pObjectList);
		m_pObjectList = NULL;
	}
	m_nNumImages = 0;
	m_nCacheMemUsed = 0;
	m_nNumReserved = 0;
	m_dwLastCheckTime = 0;

	m_uImageAccessCounter = 0;
}

void KImageStore2::FreeImage(const char* pszImage)
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	unsigned int uImage = g_FileName2Id((char*)pszImage);
	int nIdx = FindImage(uImage, 0);
	if (nIdx >= 0)
	{
		FreeImageObject(m_pObjectList[nIdx]);
		m_nNumImages--;
		for (int i = nIdx; i < m_nNumImages; i++)
			m_pObjectList[i] = m_pObjectList[i + 1];
	}
}

void KImageStore2::FreeImage(unsigned int nImageID)
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	int nIdx = FindImage(nImageID, 0);
	if (nIdx >= 0)
	{
		FreeImageObject(m_pObjectList[nIdx]);
		m_nNumImages--;
		for (int i = nIdx; i < m_nNumImages; i++)
			m_pObjectList[i] = m_pObjectList[i + 1];
	}
}

void* KImageStore2::GetExistedCreateBitmap(const char* pszImage, unsigned int uImage, short& nImagePosition, int *pType)
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	if (uImage == 0)
		uImage = g_FileName2Id((char*)pszImage);

	nImagePosition = FindImage(uImage, nImagePosition);
	void* pObject = NULL;
	if (nImagePosition >= 0 &&
		(m_pObjectList[nImagePosition].bType == ISI_T_BITMAP16 || m_pObjectList[nImagePosition].bType == ISI_T_BITMAP16_ALPHA) &&
		m_pObjectList[nImagePosition].bNotCacheable == true &&
		m_pObjectList[nImagePosition].pObject)
	{
		if(pType)
		{
			*pType = m_pObjectList[nImagePosition].bType;
		}
		pObject = (m_pObjectList[nImagePosition].pObject);
	}
	return pObject;
}

LPDIRECTDRAWSURFACE CreateDirectDrawSurfaceFromBitmap(KSGImageContent* pBitmap)
{
	LPDIRECTDRAWSURFACE pSurface = NULL;
	if (pBitmap && g_pDirectDraw)
	{
		pSurface = g_pDirectDraw->CreateSurface(pBitmap->nWidth, pBitmap->nHeight);
	}
	return pSurface;
}

bool KImageStore2::CreateBitmapSurface(const char* pszImage, unsigned int& uImage, short& nImagePosition)
{
	KAutoCriticalSection AutoLock(m_ImageProcessLock);

	if (uImage == 0)
		uImage = g_FileName2Id((char*)pszImage);

	bool bOk = false;

	nImagePosition = FindImage(uImage, nImagePosition);
	if (nImagePosition >= 0)
	{
		_KISImageObj& Img = m_pObjectList[nImagePosition];
        if (Img.bType == ISI_T_BITMAP16 && Img.bNotCacheable == true && Img.pObject)
		{
			if (Img.pSurface == NULL)
			{
				Img.pSurface = CreateDirectDrawSurfaceFromBitmap(
								(KSGImageContent *)(Img.pObject));
			}
			bOk = (Img.pSurface != NULL);
		}
	}
	return bOk;
}

void* KImageStore2::GetImage(const char* pszImage, unsigned int& uImage,
					short& nImagePosition, int nFrame, int nType, void*& pFrameData, bool bMultiThreadLoad )
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	pFrameData = NULL;
	if (uImage == 0)
	{
		if ((uImage = g_FileName2Id((char*)pszImage)) == 0)
			return NULL;
	}

	void* pObject = NULL;

	if ((nImagePosition = FindImage(uImage, nImagePosition)) >= 0)
	{
		_KISImageObj&	ImgObj = m_pObjectList[nImagePosition];
		if (ImgObj.bType == (unsigned char)nType)
		{
			if (ImgObj.pObject)
			{
				pObject = ImgObj.pObject;
				if (ImgObj.bType == ISI_T_SPR)
				{
					if ((pFrameData = GetSprFrame(pszImage, ImgObj, nFrame)) == NULL)
						pObject = NULL;
				}
				else if (ImgObj.bType == ISI_T_BITMAP16 || ImgObj.bType == ISI_T_BITMAP16_ALPHA)
				{
					pFrameData = ImgObj.pSurface;
				}
			}
			ImgObj.nRef = GetTickCount();
		}
		//else 略去同id但是不同图形类型情况的处理。因为这是受限情况



		//为了执行效率所以未把下面的判断放在CheckBalance函数体里面。下同。
		if (m_nCacheMemUsed > m_nBalanceNum && 
			(++m_uImageAccessCounter) > m_uCheckPoint || 
			(::GetTickCount() - m_dwLastCheckTime > CHECK_TIME ))
		{
			CheckBalance();
		}
		return pObject;
	}
//	if ( bMultiThreadLoad )
//	{
//		AddLoadImageReq(pszImage,nFrame, nType, uImage );
//		pFrameData = NULL;
//		return NULL;
//	}
//	else
	{		// 有空间
		if (m_nNumImages < m_nNumReserved || ExpandSpace())
		{
			nImagePosition = - nImagePosition - 1;	// FindImage时已经找好位置了
			for (int i = m_nNumImages; i > nImagePosition; i--)
			{
				m_pObjectList[i] = m_pObjectList[i - 1];
			}

			_KISImageObj& ImgObj = m_pObjectList[nImagePosition];
			ImgObj.bNotCacheable = false;
			ImgObj.nRef = GetTickCount();
			ImgObj.bSingleFrameLoad = false;
			ImgObj.bType = (unsigned char)nType;
			ImgObj.pFrames = NULL;
			ImgObj.pObject = NULL;
			ImgObj.uId = uImage;
			ImgObj.pcAdjustColorPalettes = NULL;
			ImgObj.nExtPalCount = 0;
			ImgObj.pExtPalettes = NULL;

			pObject = LoadImage(pszImage, ImgObj, nFrame, pFrameData);
			m_nNumImages++;

			if (m_nCacheMemUsed > m_nBalanceNum && 
				(++m_uImageAccessCounter) > m_uCheckPoint || 
			(::GetTickCount() - m_dwLastCheckTime > CHECK_TIME ))
				CheckBalance();
		}
		return pObject;
	}
}

void* KImageStore2::GetImageCheckLoadLimit(const char* pszImage, unsigned int& uImage,
					short& nImagePosition, int nFrame, int nType, void*& pFrameData)
{	
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	pFrameData = NULL;
	if (uImage == 0)
	{
		if ((uImage = g_FileName2Id((char*)pszImage)) == 0)
			return NULL;
	}

	void* pObject = NULL;

	return NULL;

	if ((nImagePosition = FindImage(uImage, nImagePosition)) >= 0)
	{
		_KISImageObj&	ImgObj = m_pObjectList[nImagePosition];
		if (ImgObj.bType == (unsigned char)nType)
		{
			if (ImgObj.pObject)
			{
				pObject = ImgObj.pObject;
				if (ImgObj.bType == ISI_T_SPR)
				{
					if ((pFrameData = GetSprFrameCheckLoadLimit(pszImage, ImgObj, nFrame)) == NULL)
						pObject = NULL;
				}
				else if (ImgObj.bType == ISI_T_BITMAP16 || ImgObj.bType == ISI_T_BITMAP16_ALPHA)
				{
					pFrameData = ImgObj.pSurface;
				}
			}
			ImgObj.nRef = GetTickCount();
		}
		//else 略去同id但是不同图形类型情况的处理。因为这是受限情况

		//为了执行效率所以未把下面的判断放在CheckBalance函数体里面。下同。
		if (m_nCacheMemUsed > m_nBalanceNum &&
			(++m_uImageAccessCounter) > m_uCheckPoint  || 
			(::GetTickCount() - m_dwLastCheckTime > CHECK_TIME ))
		{
			CheckBalance();
		}
		return pObject;
	}

	// 有空间
	if (m_nThisTrunLoadImgCount < ISBP_MAX_LOAD_IMG_IN_A_TURN &&
		(m_nNumImages < m_nNumReserved || ExpandSpace()))
	{
		m_nThisTrunLoadImgCount++;

		nImagePosition = - nImagePosition - 1;	// FindImage时已经找好位置了
		for (int i = m_nNumImages; i > nImagePosition; i--)
		{
			m_pObjectList[i] = m_pObjectList[i - 1];
		}

		_KISImageObj& ImgObj = m_pObjectList[nImagePosition];
		ImgObj.bNotCacheable = false;
		ImgObj.nRef = GetTickCount();
		ImgObj.bSingleFrameLoad = false;
		ImgObj.bType = (unsigned char)nType;
		ImgObj.pFrames = NULL;
		ImgObj.pObject = NULL;
		ImgObj.uId = uImage;
		ImgObj.pcAdjustColorPalettes = NULL;
		ImgObj.nExtPalCount = 0;
		ImgObj.pExtPalettes = NULL;

		pObject = LoadImage(pszImage, ImgObj, nFrame, pFrameData);
		m_nNumImages++;

		if (m_nCacheMemUsed > m_nBalanceNum && 
			(++m_uImageAccessCounter) > m_uCheckPoint || 
			(::GetTickCount() - m_dwLastCheckTime > CHECK_TIME ))
			CheckBalance();
	}
	return pObject;
}


void* KImageStore2::GetSprFrame(const char* pszImageFile, _KISImageObj& ImgObject, int nFrame)
{
	SPRHEAD* pSprHeader = (SPRHEAD*)ImgObject.pObject;
	void* pFrameData = NULL;

	//_ASSERT(pszImageFile && pSprHeader);
	if (nFrame >= 0 && nFrame < pSprHeader->Frames)
	{		
// 		if ((pszImageFile[1]=='S'||pszImageFile[1]=='s')&& (pszImageFile[5]=='n'||pszImageFile[5]=='N'||pszImageFile[5]=='s'||pszImageFile[5]=='S'))
// 		{
// 			UINT FramePerDir = pSprHeader->Frames / pSprHeader->Directions;
// 			nFrame = (nFrame / FramePerDir * FramePerDir)+(nFrame % FramePerDir) /g_FrameCutLevel * g_FrameCutLevel;
// 		}
		_KISImageFrameObj* pFrame;
		if (ImgObject.bSingleFrameLoad == false)
		{	//一次加载全部帧的图形
			pFrame = ImgObject.pFrames;
			//_ASSERT(pFrame);
			//_ASSERT(pFrame->pOffsetTable);
			pFrameData = (((char*)pFrame->pOffsetTable) + pFrame->sOffTableSize +
				((SPROFFS*)pFrame->pOffsetTable)[nFrame].Offset);
			pFrame->nRef = GetTickCount();
		}
		else
		{
			pFrame = &ImgObject.pFrames[nFrame];
			pFrame->nRef = GetTickCount();
			if ((pFrameData = pFrame->pFrameData) == NULL)
			{	//指定的帧数据还不存在
				pFrame->pFrameData = SprGetFrame((SPRHEAD*)ImgObject.pObject, nFrame);
				pFrameData = pFrame->pFrameData;
				// 因为SPR的大小已经无法得到了（ENGINE里太复杂了，:(），所以用展开的大小来近似
				pFrame->dwCacheSize = pSprHeader->Height * pSprHeader->Width * sizeof(WORD);
				m_nCacheMemUsed += pFrame->dwCacheSize;
			}
		}
	}
	return pFrameData;
}

void* KImageStore2::GetSprFrameCheckLoadLimit(const char* pszImageFile, _KISImageObj& ImgObject, int nFrame)
{
	SPRHEAD* pSprHeader = (SPRHEAD*)ImgObject.pObject;
	void* pFrameData = NULL;

	//_ASSERT(pszImageFile && pSprHeader);
	if (nFrame >= 0 && nFrame < pSprHeader->Frames)
	{
// 		if ((pszImageFile[1]=='S'||pszImageFile[1]=='s')&& (pszImageFile[5]=='n'||pszImageFile[5]=='N'||pszImageFile[5]=='s'||pszImageFile[5]=='S'))
// 		{
// 			UINT FramePerDir = pSprHeader->Frames / pSprHeader->Directions;
// 			nFrame = (nFrame / FramePerDir * FramePerDir)+(nFrame % FramePerDir) /g_FrameCutLevel * g_FrameCutLevel;
// 		}

		_KISImageFrameObj* pFrame;
		if (ImgObject.bSingleFrameLoad == false)
		{	//一次加载全部帧的图形
			pFrame = ImgObject.pFrames;
			//_ASSERT(pFrame);
			//_ASSERT(pFrame->pOffsetTable);
			pFrameData = (((char*)pFrame->pOffsetTable) + pFrame->sOffTableSize +
				((SPROFFS*)pFrame->pOffsetTable)[nFrame].Offset);
			pFrame->nRef = GetTickCount();
		}
		else
		{
			pFrame = &ImgObject.pFrames[nFrame];
			pFrame->nRef = GetTickCount();
			if ((pFrameData = pFrame->pFrameData) == NULL)
			{	//指定的帧数据还不存在
				if (m_nThisTrunLoadImgCount < ISBP_MAX_LOAD_IMG_IN_A_TURN)
				{
					m_nThisTrunLoadImgCount++;
					pFrame->pFrameData = SprGetFrame((SPRHEAD*)ImgObject.pObject, nFrame);
					pFrameData = pFrame->pFrameData;
					// 因为SPR的大小已经无法得到了（ENGINE里太复杂了，:(），所以用展开的大小来近似
					pFrame->dwCacheSize = pSprHeader->Height * pSprHeader->Width * sizeof(WORD);
					m_nCacheMemUsed += pFrame->dwCacheSize;
				}
				else
				{
					if (nFrame && ImgObject.pFrames[nFrame - 1].pFrameData)
						pFrameData = ImgObject.pFrames[nFrame - 1].pFrameData;
					else if (nFrame + 1 < pSprHeader->Frames && ImgObject.pFrames[nFrame + 1].pFrameData)
						pFrameData = ImgObject.pFrames[nFrame + 1].pFrameData;
				}
			}
		}
	}
	return pFrameData;
}

bool KImageStore2::GetImageParam(const char* pszImage, int nType, KImageParam* pImageData)
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	unsigned int uImage = g_FileName2Id((char*)pszImage);
	
	if (uImage == 0)
		return false;

	short nImagePosition = FindImage(uImage, -1);
	if (nImagePosition < 0)
	{
		void* pFrameData;
		if (GetImage(pszImage, uImage, nImagePosition, 0, nType, pFrameData, false) == NULL)
			nImagePosition = -1;
	}
	if (nImagePosition >= 0)
	{
		_KISImageObj&	ImgObj = m_pObjectList[nImagePosition];
		if (ImgObj.pObject && ImgObj.bType == (unsigned char)nType)
		{
			if (pImageData)
			{
				if (ImgObj.bType == ISI_T_SPR)
				{
					SPRHEAD* pSprHeader = (SPRHEAD*)ImgObj.pObject;
					pImageData->nWidth = pSprHeader->Width;
					pImageData->nHeight = pSprHeader->Height;
					pImageData->nInterval = pSprHeader->Interval;
					pImageData->nNumFrames = pSprHeader->Frames;
					pImageData->nNumFramesGroup = pSprHeader->Directions;
					pImageData->nReferenceSpotX = pSprHeader->CenterX;
					pImageData->nReferenceSpotY = pSprHeader->CenterY;				
				}
				else if (ImgObj.bType == ISI_T_BITMAP16)
				{
					pImageData->nWidth  = ((KSGImageContent*)(ImgObj.pObject))->nWidth;
					pImageData->nHeight = ((KSGImageContent*)(ImgObj.pObject))->nHeight;
					pImageData->nInterval = 0;
					pImageData->nNumFrames = 1;
					pImageData->nNumFramesGroup = 1;
					pImageData->nReferenceSpotX = 0;
					pImageData->nReferenceSpotY = 0;
				}
			}
			return true;
		}
	}
	return false;
}

bool KImageStore2::GetImageFrameParam(const char* pszImage, int nType,
		int nFrame, KRPosition2* pOffset, KRPosition2* pSize)
{	
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	bool bRet = false;
	short	nPos = -1;
	unsigned int uImage = 0;
	void* pFrame;
	void* pImage = GetImage(pszImage, uImage, nPos, 0, nType, pFrame, false);
	if (pImage)
	{
		if (nType == ISI_T_SPR)
		{
			if (pOffset)
			{
				pOffset->nX = ((SPRFRAME*)pFrame)->OffsetX;
				pOffset->nY = ((SPRFRAME*)pFrame)->OffsetY;
			}
			if (pSize)
			{
				pSize->nX = ((SPRFRAME*)pFrame)->Width;
				pSize->nY = ((SPRFRAME*)pFrame)->Height;
			}
		}
		else if (nType == ISI_T_BITMAP16)
		{
			if (pOffset)
			{
				pOffset->nX = 0;
				pOffset->nY = 0;
			}
			if (pSize)
			{
				pSize->nX = ((KSGImageContent*)pImage)->nWidth;
				pSize->nY = ((KSGImageContent*)pImage)->nHeight;
			}
		}
		bRet = true;
	}
	return bRet;
}

int KImageStore2::GetImagePixelAlpha(const char* pszImage, int nType, int nFrame, int nX, int nY)
{
	int nRet = 0;
	short	nPos = -1;
	unsigned int uImage = 0;
	SPRFRAME* pFrame;
	void* pImage = GetImage(pszImage, uImage, nPos, 0, nType, (void*&)pFrame, false);
	if (pImage)
	{
		if (nType == ISI_T_SPR)
		{
			nX -= pFrame->OffsetX;
			nY -= pFrame->OffsetY;
			if (nX >= 0  && nX < pFrame->Width && nY >= 0 && nY < pFrame->Height)
			{
				int	nNumPixels = pFrame->Width;
				void*	pSprite =  pFrame->Sprite;
				nY++;
				_asm
				{
					//使SDI指向sprite中的图形数据位置
					mov		esi, pSprite
				dec_line:
					dec		nY				//减掉一行
					jz		last_line
					
					mov		edx, nNumPixels
				skip_line:
					movzx	eax, byte ptr[esi]
					inc		esi
					movzx	ebx, byte ptr[esi]
					inc		esi
					or		ebx, ebx
					jz		skip_line_continue
					add		esi, eax
				skip_line_continue:
					sub		edx, eax
					jg		skip_line
					jmp		dec_line

				last_line:
					mov		edx, nX
				last_line_alpha_block:
					movzx	eax, byte ptr[esi]
					inc		esi
					movzx	ebx, byte ptr[esi]
					inc		esi
					or		ebx, ebx
					jz		last_line_continue
					add		esi, eax
				last_line_continue:
					sub		edx, eax
					jg		last_line_alpha_block

					mov		nRet, ebx
				}
			}
		}
		else if (nType == ISI_T_BITMAP16)
		{
			if (nX >= 0 && nY >= 0 &&
				nX < ((KSGImageContent*)pImage)->nWidth &&
				nY < ((KSGImageContent*)pImage)->nHeight)
			{
				nRet = 255;
			}
		}
	}
	return nRet;
}

bool KImageStore2::Init()
{
	Free();
	bool bRet = ExpandSpace();
//	m_bLoadThreadSwicth = true;
//	m_hLoadEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//	DWORD	ThreadId;
//	m_hLoadAndPreprocessThread = ::CreateThread(NULL, 0,
//		LoadImageThread, this, 0, &ThreadId);
	return bRet;
}

bool KImageStore2::SaveImage(const char* pszFile, const char* pszImage, int nFileType)
{
	return 0;
}

void KImageStore2::SetBalanceParam(int nNumImage, unsigned int uCheckPoint)
{
	m_nBalanceNum = nNumImage;
	m_uCheckPoint = uCheckPoint;
}

void KImageStore2::CheckBalance()
{
	m_dwLastCheckTime = ::GetTickCount();
	m_uImageAccessCounter = 0;
	int i, j;
    int nNewNumImages = 0;
	int nCurTime = GetTickCount();
	int nMaxInterval = 1000;
    
    for (i = 0; i < m_nNumImages; i++)
    {
		if (m_pObjectList[i].bNotCacheable == false || m_pObjectList[i].bType == ISI_T_SPR )
        {
			if (nCurTime - m_pObjectList[i].nRef >= nMaxInterval && 
				(m_pObjectList[i].bType == ISI_T_SPR && m_pObjectList[i].bSingleFrameLoad != true) )
			{
				FreeImageObject(m_pObjectList[i]);
                continue;
            }

			if ((nCurTime - m_pObjectList[i].nRef >= nMaxInterval) && 
                (m_pObjectList[i].bSingleFrameLoad == true) &&
				(m_pObjectList[i].pObject) &&
				(m_pObjectList[i].bType == ISI_T_SPR)
            )
			{
				int nNumFrame = ((SPRHEAD*)(m_pObjectList[i].pObject))->Frames;
				for (j = 0; j < nNumFrame; j++)
				{
					if (nCurTime - m_pObjectList[i].pFrames[j].nRef >= nMaxInterval)
					{
						FreeImageObject(m_pObjectList[i], j);
					}
				}
			}
        }

        m_pObjectList[nNewNumImages] = m_pObjectList[i];
        nNewNumImages++;
    }
    m_nNumImages =  nNewNumImages;
}

bool KImageStore2::ExpandSpace()
{
	_KISImageObj* pNewList = (_KISImageObj *)realloc(m_pObjectList,
						(m_nNumReserved + ISBP_EXPAND_SPACE_STEP) * sizeof(_KISImageObj));
	if (pNewList)
	{
		m_pObjectList = pNewList;
		m_nNumReserved += ISBP_EXPAND_SPACE_STEP;
		return true;
	}
	return false;
}

int KImageStore2::FindImage(unsigned int uImage, int nPossiblePosition)
{
	int nPP = nPossiblePosition;
	if (nPP < 0 || nPP >= m_nNumImages)
	{
		if (m_nNumImages <= 0)
		{
			return -1;
		}
		else
		{
			nPP = m_nNumImages / 2;
		}
	}
	if (m_pObjectList[nPP].uId == uImage)
		return nPP;
	int nFrom, nTo, nTryRange;
	nTryRange = ISBP_TRY_RANGE_DEF;
	if (m_pObjectList[nPP].uId > uImage)
	{
		nFrom = 0;
		nTo = nPP - 1;
		nPP -= nTryRange;
	}
	else
	{
		nFrom = nPP + 1;
		nTo = m_nNumImages - 1;
		nPP += nTryRange;
	}
	if (nFrom + nTryRange >= nTo)
		nPP = (nFrom + nTo) / 2;

	while (nFrom < nTo)
	{
		if (m_pObjectList[nPP].uId < uImage)
		{
			nFrom = nPP + 1;
		}
		else if (m_pObjectList[nPP].uId > uImage)
		{
			nTo = nPP - 1;
		}
		else
		{
			return nPP;
		}
		nPP = (nFrom + nTo) / 2;
	}
	if (nFrom == nTo)
	{
		if (m_pObjectList[nPP].uId > uImage)
		{
			nPP = - nPP - 1;
		}
		else if (m_pObjectList[nPP].uId < uImage)
		{
			nPP = - nPP - 2;
		}
	}
	else
	{
		nPP = - nFrom -1;
	}
	return nPP;
}

void KImageStore2::FreeImageObject(_KISImageObj& ImgObject, int nFrame/*=-1*/)
{
	if(ImgObject.pObject)
	{
		if (ImgObject.bType == ISI_T_SPR)
		{

			if (ImgObject.bSingleFrameLoad == false)
			{
				m_nCacheMemUsed -= ImgObject.pFrames->dwCacheSize;
				free (ImgObject.pFrames);
				ImgObject.pFrames = NULL;
				if (ImgObject.pcAdjustColorPalettes)
				{
					free(ImgObject.pcAdjustColorPalettes);
					ImgObject.pcAdjustColorPalettes = NULL;
				}
				if (ImgObject.pExtPalettes) 
				{
					free(ImgObject.pExtPalettes);
					ImgObject.pExtPalettes = NULL;
				}
				if (ImgObject.pObject)
				{
					SprReleaseHeader((SPRHEAD*)ImgObject.pObject);
					ImgObject.pObject = NULL;
				}
			}
			else
			{
				int nNumFrame = ((SPRHEAD*)ImgObject.pObject)->Frames;
				if (nFrame >= 0 && nFrame < nNumFrame)
				{
					m_nCacheMemUsed -= ImgObject.pFrames[nFrame].dwCacheSize;
					if (ImgObject.pFrames[nFrame].pFrameData)
					{
						SprReleaseFrame((SPRFRAME*)ImgObject.pFrames[nFrame].pFrameData);
						ImgObject.pFrames[nFrame].pFrameData = NULL;
					}
				}
				else if (nFrame < 0)
				{
					for (nFrame = 0; nFrame < nNumFrame; nFrame++)
					{
						m_nCacheMemUsed -= ImgObject.pFrames[nFrame].dwCacheSize;
						if (ImgObject.pFrames[nFrame].pFrameData)
						{
							SprReleaseFrame((SPRFRAME*)ImgObject.pFrames[nFrame].pFrameData);
						}
					}
					free (ImgObject.pFrames);
					ImgObject.pFrames = NULL;
					if (ImgObject.pcAdjustColorPalettes)
					{
						free(ImgObject.pcAdjustColorPalettes);
						ImgObject.pcAdjustColorPalettes = NULL;
					}
					if (ImgObject.pExtPalettes)
					{
						free(ImgObject.pExtPalettes);
						ImgObject.pExtPalettes = NULL;
					}
					SprReleaseHeader((SPRHEAD*)(ImgObject.pObject));
					ImgObject.pObject = NULL;
				}
			}
		}
		else if (ImgObject.bType == ISI_T_BITMAP16 || 
			ImgObject.bType == ISI_T_BITMAP16_ALPHA || 
			ImgObject.bType == ISI_T_8BIT_ALPHA)
		{
			if (ImgObject.pObject)
			{
				if (ImgObject.bNotCacheable == false)
					release_image((KSGImageContent *)ImgObject.pObject);
				else
					free(ImgObject.pObject);
				ImgObject.pObject = NULL;
			}
		}
		else
			ImgObject.pObject = NULL;//*/
	}
}

bool	KImageStore2::IsInLoadList( const char* pszImageFile, int nFrame )
{
	KAutoCriticalSection AutoLock(m_ImageProcessLock);
	_KISImageReqObjList::iterator it = m_imageReqList.find(pszImageFile);
	if ( it != m_imageReqList.end() )
	{
		if ( nFrame == it->second.nFrame )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool	KImageStore2::AddLoadImageReq( const char* pszImageFile, int nFrame, int nType, unsigned int uImage )
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);
	if ( !IsInLoadList( pszImageFile, nFrame ) )
	{
		_KISImageReqObj imageReqObj;
		memset(&imageReqObj, 0,sizeof(_KISImageReqObj) );
		strncpy( imageReqObj.pszImageFile, pszImageFile, 256 );
		imageReqObj.nFrame = nFrame;
		imageReqObj.nType = nType;
		imageReqObj.uImage = uImage;
		m_imageReqList[pszImageFile] = imageReqObj;
		::SetEvent( m_hLoadEvent );

		return true;
	}
	else
	{
		return false;
	}
}

DWORD WINAPI KImageStore2::LoadImageThread(void* pParam)
{
	KImageStore2* pImgStr = (KImageStore2*)pParam;

	if ( pImgStr != NULL )
	{
		while ( pImgStr->m_bLoadThreadSwicth )
		{
			DWORD dwRetCode = ::WaitForSingleObject(pImgStr->m_hLoadEvent, 1000);
			if ( dwRetCode == WAIT_OBJECT_0 )
			{
				KAutoCriticalSection AutoLock(	pImgStr->m_ImageProcessLock);
				_KISImageReqObjList::iterator it = pImgStr->m_imageReqList.begin();

				if ( it != pImgStr->m_imageReqList.end() )
				{
					DWORD curPaintTimer = ::GetTickCount();				
					static DWORD oldPaintTime = 0;
					//if ( curPaintTimer - oldPaintTime >= 1000 / 36 )
					{
						oldPaintTime = curPaintTimer;
						void* pFrame = NULL;

						_KISImageReqObj& imgReqObj = it->second;
						int	nImagePosition = pImgStr->FindImage(imgReqObj.uImage, -1);
						if ( nImagePosition >= 0 )
						{
							if ( pImgStr->m_pObjectList[nImagePosition].bType == ISI_T_SPR )
							{
								if ( pImgStr->m_pObjectList[nImagePosition].bSingleFrameLoad )
								{
									if ( pImgStr->m_pObjectList[nImagePosition].pFrames[imgReqObj.nFrame].pFrameData != NULL)
									{
										pImgStr->m_imageReqList.erase(it++);				
										continue;
									}
								}
								else
								{
									pImgStr->m_imageReqList.erase(it++);				
									continue;
								}
							}
							else
							{
								pImgStr->m_imageReqList.erase(it++);		
								continue;
							}
						}


						if (pImgStr->m_nNumImages < pImgStr->m_nNumReserved || pImgStr->ExpandSpace())
						{
							nImagePosition = - nImagePosition - 1;	// FindImage时已经找好位置了
							for (int i = pImgStr->m_nNumImages; i > nImagePosition; i--)
							{
								pImgStr->m_pObjectList[i] = pImgStr->m_pObjectList[i - 1];
							}

							_KISImageObj& ImgObj = pImgStr->m_pObjectList[nImagePosition];
							ImgObj.bNotCacheable = false;
							ImgObj.bLoading	= true;
							ImgObj.nRef = ::GetTickCount();
							ImgObj.bSingleFrameLoad = false;
							ImgObj.bType = (unsigned char)imgReqObj.nType;
							ImgObj.pFrames = NULL;
							ImgObj.pObject = NULL;
							ImgObj.uId = imgReqObj.uImage;
							ImgObj.pcAdjustColorPalettes = NULL;
							ImgObj.nExtPalCount = 0;
							ImgObj.pExtPalettes = NULL;

							void* pObject =	pImgStr->LoadImage( 
											imgReqObj.pszImageFile,
											ImgObj,
											imgReqObj.nFrame, pFrame );
							pImgStr->m_nNumImages++;

						}					
						pImgStr->m_imageReqList.erase(it++);	
					}
				}			
			}
			::ResetEvent( pImgStr->m_hLoadEvent );
		}
	}

	return 0;
}

void* KImageStore2::LoadImage(const char* pszImageFile, _KISImageObj& ImgObj, int nFrame, void*& pFrameData)
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);
	void* pRet = NULL;
	_KISImageFrameObj* pFrameObj = NULL;

	if (ImgObj.bType == ISI_T_SPR)
	{
		SPROFFS*	pOffsTable = NULL;
		SPRHEAD*  pSprHeader = SprGetHeader(pszImageFile, pOffsTable);
		if (pSprHeader)
		{
			if (pOffsTable)	//一次加载完整的spr图
			{
				pFrameObj = (_KISImageFrameObj*)malloc(sizeof(_KISImageFrameObj));
				if (pFrameObj)
				{
					ImgObj.pObject = pSprHeader;
					ImgObj.pFrames = pFrameObj;
					ImgObj.bNotCacheable = false;
					ImgObj.bSingleFrameLoad = false;
					pFrameObj->pOffsetTable = pOffsTable;
					pFrameObj->sOffTableSize = sizeof(SPROFFS) * pSprHeader->Frames;
					pFrameObj->nRef = GetTickCount();

					if (nFrame >= 0 && nFrame < pSprHeader->Frames)
					{
						pFrameData = ((char*)pOffsTable + pFrameObj->sOffTableSize +
							pOffsTable[nFrame].Offset);
						pRet = pSprHeader;
					}
					// 扩展的调色板
//					if (pSprHeader->ExtPalCount > 0)
//					{
//						const unsigned int uLastFrame = pSprHeader->Frames - 1;
//						ImgObj.pExtPalettes = ((char*)pOffsTable + pFrameObj->sOffTableSize +
//							pOffsTable[uLastFrame].Offset + pOffsTable[uLastFrame].Length);
//						g_Pal24ToPal16((KPAL24*)ImgObj.pExtPalettes, (KPAL16*)ImgObj.pExtPalettes, pSprHeader->ExtPalCount * pSprHeader->Colors); 
//					}
					// 因为SPR的大小已经无法得到了（ENGINE里太复杂了，:(），所以用展开的大小来近似
					pFrameObj->dwCacheSize = pSprHeader->Height * pSprHeader->Width * sizeof(WORD);
					m_nCacheMemUsed += pFrameObj->dwCacheSize;
				}
			}
			else	//分帧加载的图
			{
				int nSize = sizeof(_KISImageFrameObj) * pSprHeader->Frames;
				pFrameObj = (_KISImageFrameObj*)malloc(nSize);
				if (pFrameObj)
				{
					memset(pFrameObj, 0, nSize);
					ImgObj.pObject = pSprHeader;
					ImgObj.pFrames = pFrameObj;
					ImgObj.bNotCacheable = false;
					ImgObj.bSingleFrameLoad = true;
					if (nFrame >= 0 && nFrame < pSprHeader->Frames)
					{
						pFrameData = (SPRFRAME*)SprGetFrame(pSprHeader, nFrame);
						if (pFrameData)
						{
							ImgObj.pFrames[nFrame].nRef = GetTickCount();
							ImgObj.pFrames[nFrame].pFrameData = (SPRFRAME*)pFrameData;
							// 因为SPR的大小已经无法得到了（ENGINE里太复杂了，:(），所以用展开的大小来近似
							ImgObj.pFrames[nFrame].dwCacheSize = pSprHeader->Height * pSprHeader->Width * sizeof(WORD);
							m_nCacheMemUsed += ImgObj.pFrames[nFrame].dwCacheSize;
							pRet = pSprHeader;
						}
					}
//					if (pSprHeader->ExtPalCount > 0)
//					{
//						const unsigned int uExtPalSize = pSprHeader->ExtPalCount * pSprHeader->Colors;
//						ImgObj.pExtPalettes = (char*)malloc(uExtPalSize * sizeof(KPAL24));
//						if (ImgObj.pExtPalettes != NULL)
//						{
//							SprGetExtPal(pSprHeader, (KPAL24*)ImgObj.pExtPalettes, uExtPalSize);
//							g_Pal24ToPal16((KPAL24*)ImgObj.pExtPalettes, (KPAL16*)ImgObj.pExtPalettes, uExtPalSize); 
//						}
//					}
				}
			}
			if (ImgObj.pObject)
			{
				if ( ((SPRHEAD*)(ImgObj.pObject))->Reserved[BITSPIXEL_INDEX_STORE_IN_RESERVED] != BITS_PIXEL_16 )
				{
					g_Pal24ToPal16((KPAL24*)(&pSprHeader[1]),
						(KPAL16*)(&pSprHeader[1]), pSprHeader->Colors);
				}
			}
			else
			{
				SprReleaseHeader(pSprHeader);
				ImgObj.pcAdjustColorPalettes = NULL;
				ImgObj.nExtPalCount = 0;
				ImgObj.pExtPalettes = NULL;
			}
		}
	}
	else if (ImgObj.bType == ISI_T_BITMAP16)
	{
		ImgObj.pObject = get_jpg_image(pszImageFile);
		pRet = ImgObj.pObject;
	}
	ImgObj.bLoading = false;
	return pRet;
}

KImageStore2::KImageStore2()
{
	MEMORYSTATUS stat;
	GlobalMemoryStatus (&stat);
	if(stat.dwTotalPhys <= 134217728)
    {
		m_nBalanceNum = ISBP_BALANCE_NUM_DEF128;
//      m_nMaxReleaseCount = 16;
    }
	else if(stat.dwTotalPhys <= 134217728 * 2)
    {
		m_nBalanceNum = ISBP_BALANCE_NUM_DEF256;
//      m_nMaxReleaseCount = 32;
    }
	else if (stat.dwTotalPhys <= 134217728 * 4)
    {
		m_nBalanceNum = ISBP_BALANCE_NUM_DEF512;
//      m_nMaxReleaseCount = 64;
    }
	else
	{
		m_nBalanceNum = ISBP_BALANCE_NUM_DEF1024;
//		m_nMaxReleaseCount = 128;
	}	

	m_nBalanceNum = ISBP_BALANCE_NUM_DEF;
	m_pObjectList = NULL;
	m_nNumReserved = 0;
	m_nNumImages  = 0;
//	m_nBalanceNum = ISBP_BALANCE_NUM_DEF;
	m_uCheckPoint = ISBP_CHECK_POINT_DEF;
	m_uImageAccessCounter = 0;

	m_nThisTrunLoadImgCount = 0;
}

KImageStore2::~KImageStore2()
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);
//	m_bLoadThreadSwicth = false;
//	DWORD	dwExitCode;
//	if (::GetExitCodeThread(m_hLoadAndPreprocessThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
//		::WaitForSingleObject(m_hLoadAndPreprocessThread, INFINITE);
//	::CloseHandle(m_hLoadAndPreprocessThread);
//	m_hLoadAndPreprocessThread = NULL;
//	CloseHandle(m_hLoadEvent);
//	m_hLoadEvent = NULL;
	Free();
}

void KImageStore2::ClearAllAdjustColorPalette()
{
    KAutoCriticalSection AutoLock(m_ImageProcessLock);

	for (int i = 0; i < m_nNumImages; i++)
	{
		if (m_pObjectList[i].pcAdjustColorPalettes)
		{
			free (m_pObjectList[i].pcAdjustColorPalettes);
			m_pObjectList[i].pcAdjustColorPalettes = NULL;
		}
	}
}