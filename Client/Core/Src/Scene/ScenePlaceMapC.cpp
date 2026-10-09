// ***************************************************************************************
// 地图的类定义实现
// Copyright : Kingsoft 2003
// Author    : wooy(wu yue)
// CreateTime: 2003-7-8
// ***************************************************************************************
#include "KCore.h"
#include "ScenePlaceMapC.h"
#include "crtdbg.h"
#include "../KPlayer.h"
#include "../ImgRef.h"
#include "iRepresentShell.h"
#include "../GameDataDef.h"
#include "KScenePlaceRegionC.h"
#include "SceneDataDef.h"
#include "KSG_StringProcess.h"
#include "KMath.h"
#include "MapNpcMgr.h"

#include "KDirtyNpcSet.h"
#include "KSubWorld.h"
#include "KWin32Wnd.h"

#ifndef _SERVER
#include "CoreShell.h"

#ifdef	_AUTO_ROBOT
#include "AutoRobotMgr.h"
#endif
#endif

#define	PLACE_MAP_FILE_NAME_APPEND			"24.jpg"
#define	SCENE_PLACE_MAP_FILE_NAME_APPEND	"scene.jpg"
#define	PLACE_MAP_SAVE_SECTION				"MAIN"


#define	RIGHT_BOTTOM_NO_LIMIT			0x7fffffff

enum MINI_MAP_ICON
{
	MMI_SELF = 0,
	MMI_MONSTER,
	MMI_DIALOGER,
	MMI_NOT_THE_SAME_SOCIAL,
	MMI_OTHER_TONG_OWNER,
	MMI_OTHER_GENS_OWNER,
	MMI_THE_SAME_GENS,
	MMI_THE_SAME_TONG,
	MMI_SAME_TONG_OWNER,
	MMI_SAME_GENS_OWNER,
	MMI_SAME_TEAM,
	MMI_TEAM_OWNER,
	MMI_RED_NAME,
	MMI_SCENE_MAP_SELF_ICON,
	MMI_END,
};

KAlphaBmp::KAlphaBmp()
{
	m_pColorBuffer = NULL;
	m_pAlphaBuffer = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
	strcpy(m_szImageName, "\\*PlaceMap_New*");
	m_uImageId = 0;
	m_nISPosition = 0;
}

KAlphaBmp::~KAlphaBmp()
{
	if (m_pColorBuffer)
	{
		delete[] m_pColorBuffer;
		m_pColorBuffer = NULL;
	}
	
	if (m_pAlphaBuffer)
	{
		delete[] m_pAlphaBuffer;
		m_pAlphaBuffer = NULL;
	}
}

BOOL KAlphaBmp::Init(const char* pszFilePath)
{
	KPakFile Pak;
	
	if (Pak.Open(pszFilePath))
	{
		BITMAPFILEHEADER header;
		Pak.Read(&header, sizeof(header));
		if (header.bfType != 0x4d42)
			return FALSE;
		
		BITMAPINFOHEADER info;
		Pak.Read(&info, sizeof(info));
		if (info.biBitCount != 32)
			return FALSE;
		
		Pak.Seek(header.bfOffBits, FILE_BEGIN);
		int nSize = info.biWidth * info.biHeight * 2;
		m_pColorBuffer = new BYTE[nSize];
		m_pAlphaBuffer = new BYTE[info.biWidth * info.biHeight];
		if (!m_pColorBuffer || !m_pAlphaBuffer)
			return FALSE;
		
		short* pD = (short*)m_pColorBuffer;
		BYTE* pA = m_pAlphaBuffer;
		for(int i=0; i<info.biWidth * info.biHeight; i++)
		{
			DWORD dwColor = 0;
			Pak.Read(&dwColor, sizeof(DWORD));
			*pD = ((dwColor & 0xFF) >> 3) | ((dwColor & 0xFF00) >> 11 << 5) | ((dwColor & 0xFF0000) >> 19 << 10);
#ifdef _New3DClient
			*pD |= (short)0x8000;
#endif
			*pA = (dwColor & 0xFF000000) >> 24;
			if ((*pA > 0)&&(*pA < 255))
				*pA = 0x1;
			pD++;
			pA++;
		}
		
		m_nWidth = info.biWidth;
		m_nHeight = info.biHeight;
		
		m_uImageId = g_pRepresent->CreateImage(m_szImageName, m_nWidth, m_nHeight, ISI_T_BITMAP16_ALPHA);
		KBitmapDataBuffInfo Info;
		BYTE *pBitmap = (BYTE*)g_pRepresent->GetBitmapDataBuffer(
			m_szImageName, &Info);
		BYTE* pD1 = (BYTE*)pBitmap;
		for (i=0; i<m_nHeight; i++)
		{
			pD = (short*)pD1;
			for (int j=0; j<m_nWidth; j++)
			{
#ifdef _New3DClient
				*pD = (short)0x0000;
#else
				*pD = (short)0x8000;
#endif
				pD++;
			}
			pD1 += Info.nPitch;
		}
		
		g_pRepresent->ReleaseBitmapDataBuffer(m_szImageName, pBitmap);
		
		//		g_pRepresent->ClearImageData(m_szImageName, m_uImageId, 0);
		m_nISPosition = 0;
		
		Pak.Close();
	}
	
	return TRUE;
}

void KAlphaBmp::Blt(void* pSrcColor, int nSrcX, int nSrcY, int nSrcPitch, int nDstX, int nDstY, int nWidth, int nHeight)
{
	KBitmapDataBuffInfo Info;
	BYTE *pBitmap = (BYTE*)g_pRepresent->GetBitmapDataBuffer(
		m_szImageName, &Info);
	if ((!pBitmap)||(!pSrcColor))
		return;
	if ((!m_pAlphaBuffer)||(!m_pColorBuffer))
		return;
	
	// 限制要写入的行和宽
	if (nWidth + nDstX > m_nWidth)
		nWidth = m_nWidth - nDstX;
	if (nHeight + nDstY > m_nHeight)
		nHeight = m_nHeight - nDstY;
	
	short* pD = (short*)pBitmap + nDstY * Info.nPitch / 2 + nDstX;		// 555 Format
	short* pS = (short*)pSrcColor + nSrcY * nSrcPitch + nSrcX;	// 565 Format
	BYTE* pA = m_pAlphaBuffer + nDstY * m_nWidth + nDstX;		// 8 byte format
	short* pC = (short*)m_pColorBuffer + nDstY * m_nWidth + nDstX;		// 555 format
	
	short* pD1 = NULL;
	BYTE* pA1 = NULL;
	short* pC1 = NULL;
	short* pS1 = NULL;
	for (int i=0; i<nHeight; i++)
	{
		pD1 = pD;
		pA1 = pA;
		pC1 = pC;
		pS1 = pS;
		for (int j=0; j<nWidth; j++)
		{
			
			short s = *pS1;
			BYTE a = *pA1;
			if (a == 0)
			{
#ifdef _New3DClient
				*pD1 = (short)0x0000;
#else
				*pD1 = (short)0x8000;
#endif
			}
			else if (a == 0x1)
			{
#ifdef _New3DClient
				*pD1 =  (s & 0x1F) | ((s & 0x7E0) >> 6 << 5) | ((s & 0xF800) >> 1) | (0x8000);	
#else
				*pD1 =  (s & 0x1F) | ((s & 0x7E0) >> 6 << 5) | ((s & 0xF800) >> 1);	
#endif
			}
			else
			{
				*pD1 = *pC1;
			}
			
			pS1++;
			pD1++;
			pA1++;
			pC1++;
		}
		
		pS+=nSrcPitch;
		pD+=(Info.nPitch / 2);
		pA+=m_nWidth;
		pC+=m_nWidth;
	}
	
	g_pRepresent->ReleaseBitmapDataBuffer(m_szImageName, pBitmap);
}

void KAlphaBmp::Draw(int nX, int nY, int nWidth, int nHeight)
{
	KRUImagePart	Img;
	Img.bRenderFlag = 0;
	Img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
	Img.Color.Color_dw = 0;
	Img.nFrame = 0;
	Img.nType = ISI_T_BITMAP16_ALPHA;
	Img.oPosition.nY = nY;
	Img.oPosition.nX = nX;
	Img.oImgLTPos.nX = 0;
	Img.oImgLTPos.nY = 0;
	Img.oImgRBPos.nX = nWidth;
	Img.oImgRBPos.nY = nHeight;
	strcpy(Img.szImage, m_szImageName);
	Img.nISPosition = m_nISPosition;
	Img.uImage = 0;
	
	g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE, true);
}

BYTE* KAlphaBmp::GetColorBuffer()
{
	return m_pColorBuffer;
}

BYTE* KAlphaBmp::GetAlphaBuffer()
{
	return m_pAlphaBuffer;
}

int KAlphaBmp::GetHeight()
{
	return m_nHeight;
}

int KAlphaBmp::GetWidth()
{
	return m_nWidth;
}

//////////////////////////////////////////////////////////////////////////

KScenePlaceMapC::KScenePlaceMapC()
{
	m_bHavePicMap = false;
	m_bHaveSceneMap = false;
	m_bInited = false;
	m_szEntireMapFile[0] = 0;
	m_EntireMapLTPosition.x = m_EntireMapLTPosition.y = 0;
	m_FocusPosition.x = m_FocusPosition.y = 0;
	m_FocusLimit.left = m_FocusLimit.right = m_FocusLimit.top = m_FocusLimit.bottom = 0;
	m_PicLoadedLTPosition.x = m_PicLoadedLTPosition.y = 0;
	m_Size.cx = m_Size.cy = 0;
	m_MapCoverArea.left = m_MapCoverArea.right = m_MapCoverArea.top = m_MapCoverArea.bottom = 0;
	m_PaintCell.left = m_PaintCell.right = m_PaintCell.top = m_PaintCell.bottom = 0;
	m_uMapShowElems = SCENE_PLACE_MAP_ELEM_NONE;
	memset(&m_ElemsList, 0, sizeof(m_ElemsList));
	m_pEntireMap = NULL;
	m_nSceneMapWidth = 0;
	m_nSceneMapHeight = 0;
	//	m_bSmallMap = true;
	
	IR_InitUiImageRef(m_sceneMapSelfImage);
	m_sceneMapSelfImage.nFrame = 0;
}

KScenePlaceMapC::~KScenePlaceMapC()
{
	Terminate();
}

//初始化
bool KScenePlaceMapC::Initialize()
{
	if (m_bInited == false && g_pRepresent)
	{
		int nCount = MAP_CELL_MAX_RANGE * MAP_CELL_MAX_RANGE;
		int	nIndex, i;
		MAP_CELL* pCell = &m_ElemsList[0][0];
		for (nIndex = i = 0; nIndex < nCount && i < 0x03335688; i++)
		{
			sprintf(pCell->szImageName, "*PlaceMap_%d*", i);
			pCell->uImageId = g_pRepresent->CreateImage(
				pCell->szImageName, MAP_CELL_MAP_WIDTH, MAP_CELL_MAP_HEIGHT, ISI_T_BITMAP16);
			if (pCell->uImageId)
			{
				pCell++;
				nIndex++;
			}
		}
		m_bInited = true;
		
		// --> Rocker Edit Start 2005/10/18 加载一个遮罩bmp图
		if (!m_AlphaBmp.Init(ALPHA_BMP_PATH))
		{
			return false;
		}
		// <-- Rocker End
		
		KIniFile	ColorSetting;
		char		szBuffer[64];
		if (ColorSetting.Load(MAP_SETTING_FILE))
		{
			const char* pcszTemp;
			KRColor		Color;
			Color.Color_dw = 0;
			
			// 主角颜色
			ColorSetting.GetString("Map", "SelfColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
            Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uSelfColor = Color.Color_dw;
			
			// 队友颜色
			ColorSetting.GetString("Map", "TeammateColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uTeammateColor = Color.Color_dw;
			
			// 其他玩家颜色
			ColorSetting.GetString("Map", "PlayerColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uPlayerColor = Color.Color_dw;
			
			// 战斗npc颜色
			ColorSetting.GetString("Map", "FightNpcColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uFightNpcColor = Color.Color_dw;
			
			// 普通npc颜色
			ColorSetting.GetString("Map", "NormalNpcColor", "255,255,255", szBuffer, sizeof(szBuffer));
            pcszTemp = szBuffer;
			Color.Color_b.r = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.g = KSG_StringGetInt(&pcszTemp, 255);
            KSG_StringSkipSymbol(&pcszTemp, ',');
			Color.Color_b.b = KSG_StringGetInt(&pcszTemp, 255);
			m_uNormalNpcColor = Color.Color_dw;
		}
	}
	
	return m_bInited;
}

//结束对象功能。释放对象的全部数据与动态构造的资源。
void KScenePlaceMapC::Terminate()
{
	if (m_bInited && g_pRepresent)
	{
		for (int v = 0; v < MAP_CELL_MAX_RANGE; v++)
		{
			for (int h = 0; h < MAP_CELL_MAX_RANGE; h++)
			{
				if (m_ElemsList[v][h].uImageId)
				{
					g_pRepresent->FreeImage(m_ElemsList[v][h].szImageName);
					m_ElemsList[v][h].uImageId = 0;
				}
				m_ElemsList[v][h].szImageName[0] = 0;
			}
		}
	}
	m_bInited = false;
}

//设置场景地图的包含的元素
void KScenePlaceMapC::SetShowElemsFlag(unsigned int uShowElemsFlag)
{
	m_uMapShowElems = uShowElemsFlag;
	if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
		SetFocusPosition(m_FocusPosition.x, m_FocusPosition.y, true);
}

//读取设置
void KScenePlaceMapC::Load(KIniFile* pSetting, const char* pszScenePlaceRootPath, const char* mapName)
{
	if (Initialize() == false)
		return;
	Free();
	m_bHavePicMap = false;
	if (pSetting && pszScenePlaceRootPath)
	{
		int len = strlen(mapName);
		memset(_mapName, 0, sizeof(_mapName));
		strncpy(_mapName, mapName, COMMON_CLIENT_MSG_LEN_64 > len ? len : COMMON_CLIENT_MSG_LEN_64);
		_mapName[COMMON_CLIENT_MSG_LEN_64 - 1] = 0;
		
		char tempTest[COMMON_CLIENT_MSG_LEN_64];
		pSetting->GetString("MAIN", "Size", "", tempTest, COMMON_CLIENT_MSG_LEN_64);
		sscanf(tempTest, "%d,%d", &m_nSceneMapWidth, &m_nSceneMapHeight);
		
		pSetting->GetString("MAIN", "Area", "", tempTest, COMMON_CLIENT_MSG_LEN_64);
		sscanf(tempTest, "%d,%d,%d,%d", &m_nSceneMapArea.x, &m_nSceneMapArea.y, 
			&m_nSceneMapArea.width, &m_nSceneMapArea.height);
		
		pSetting->GetString("MAIN", "rect", "", tempTest, COMMON_CLIENT_MSG_LEN_64);
		sscanf(tempTest, "%d,%d,%d,%d", &m_curSceneMapGridArea.x, &m_curSceneMapGridArea.y, 
			&m_curSceneMapGridArea.width, &m_curSceneMapGridArea.height);
		m_curSceneMapGridArea.width = m_curSceneMapGridArea.width - m_curSceneMapGridArea.x + 1;
		m_curSceneMapGridArea.height = m_curSceneMapGridArea.height - m_curSceneMapGridArea.y + 1;
		
		sprintf(m_szSceneMapFile, "%s"SCENE_PLACE_MAP_FILE_NAME_APPEND,pszScenePlaceRootPath);
        KImageParam imgParam  = {0};
        g_pRepresent->GetImageParam(m_szSceneMapFile, &imgParam, ISI_T_BITMAP16);
		//         m_nSceneMapWidth = imgParam.nWidth;
		//         m_nSceneMapHeight = imgParam.nHeight;
		sprintf(m_szEntireMapFile, "%s"PLACE_MAP_FILE_NAME_APPEND, pszScenePlaceRootPath);
		m_bHavePicMap = g_FileExists(m_szEntireMapFile);		
		if (m_bHavePicMap)
		{
			m_EntireMapLTPosition.x = -1;
			pSetting->GetInteger2(PLACE_MAP_SAVE_SECTION, "MapLTRegionIndex",
				(int*)&m_EntireMapLTPosition.x, (int*)&m_EntireMapLTPosition.y);
			if (m_EntireMapLTPosition.x == -1)
			{
				RECT	rc;
				pSetting->GetRect(PLACE_MAP_SAVE_SECTION, "rect", &rc);
				m_EntireMapLTPosition.x = rc.left;
				m_EntireMapLTPosition.y = rc.top;
			}
			if (m_EntireMapLTPosition.x != -1)
			{
				m_EntireMapLTPosition.x *= KScenePlaceRegionC::RWPP_AREGION_WIDTH;
				m_EntireMapLTPosition.y *= KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
				
				m_PicLoadedLTPosition.x = 0;
				m_PicLoadedLTPosition.y = 0;
				
				m_FocusLimit.left = m_EntireMapLTPosition.x + (m_Size.cx * MAP_SCALE_H / 2);
				m_FocusLimit.top  = m_EntireMapLTPosition.y + (m_Size.cy * MAP_SCALE_V / 2);
				m_FocusLimit.bottom = m_FocusLimit.right = RIGHT_BOTTOM_NO_LIMIT;
				
				m_bHavePicMap = true;
			}
			else
			{
				m_bHavePicMap = false;
			}
		}
		m_bHaveSceneMap	= g_FileExists(m_szSceneMapFile);
	}
}

BOOL KScenePlaceMapC::isMapExist(const char* mapName)
{
	char mapFullName[COMMON_CLIENT_MSG_LEN_64];
	sprintf(mapFullName, "\\Maps\\%s%s", mapName, SCENE_PLACE_MAP_FILE_NAME_APPEND);
	return g_FileExists(mapFullName);
}

//设置场景地图的焦点
void KScenePlaceMapC::SetFocusPosition(int nX, int nY, bool bChangedRegion)
{
	if (nX < m_FocusLimit.left)
		m_FocusPosition.x = m_FocusLimit.left;
	else if (nX > m_FocusLimit.right)
		m_FocusPosition.x = m_FocusLimit.right;
	else
		m_FocusPosition.x = nX;
	if (nY < m_FocusLimit.top)
		m_FocusPosition.y = m_FocusLimit.top;
	else if (nY > m_FocusLimit.bottom)
		m_FocusPosition.y = m_FocusLimit.bottom;
	else
		m_FocusPosition.y = nY;
	
	m_MapCoverArea.left = m_FocusPosition.x / MAP_SCALE_H - m_Size.cx / 2;
	m_MapCoverArea.top  = m_FocusPosition.y / MAP_SCALE_V - m_Size.cy / 2;
	m_MapCoverArea.right  = m_MapCoverArea.left + m_Size.cx;
	m_MapCoverArea.bottom = m_MapCoverArea.top  + m_Size.cy;
	
	if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
	{
		if (bChangedRegion && m_bHavePicMap)
		{
			POINT	CellIndex;
			CellIndex.x = (m_FocusPosition.x - m_PicLoadedLTPosition.x) / MAP_CELL_SCENE_WIDTH;
			CellIndex.y = (m_FocusPosition.y - m_PicLoadedLTPosition.y) / MAP_CELL_SCENE_HEIGHT;
			if (CellIndex.x != MAP_CELL_CENTRE_INDEX || //MAP_CELL_FOCUS_INDEX_MIN || CellIndex.x > MAP_CELL_FOCUS_INDEX_MAX ||
				CellIndex.y != MAP_CELL_CENTRE_INDEX)	//MAP_CELL_FOCUS_INDEX_MIN || CellIndex.y > MAP_CELL_FOCUS_INDEX_MAX)
			{
				m_PicLoadedLTPosition.x += (CellIndex.x - MAP_CELL_CENTRE_INDEX) * MAP_CELL_SCENE_WIDTH;
				m_PicLoadedLTPosition.y += (CellIndex.y - MAP_CELL_CENTRE_INDEX) * MAP_CELL_SCENE_HEIGHT;
				FillCellsPicInfo();
			}
		}
		CalcPicLayout();
	}
}

void KScenePlaceMapC::FillCellsPicInfo()
{
	if (m_pEntireMap == NULL)
	{
		if (m_bHavePicMap)
		{
			KBitmapDataBuffInfo	Info;
			short *pBuff = (short*)g_pRepresent->GetBitmapDataBuffer(
				m_ElemsList[0][0].szImageName, &Info);
			if (pBuff)
			{
				g_pRepresent->ReleaseBitmapDataBuffer(
					m_ElemsList[0][0].szImageName, pBuff);
				unsigned int uMask16 = -1;
				if (Info.eFormat == BDBF_16BIT_555)
					uMask16 = RGB_555;
				else if (Info.eFormat == BDBF_16BIT_565)
					uMask16 = RGB_565;
				if (uMask16 >= 0)
					m_pEntireMap = get_jpg_image(m_szEntireMapFile, uMask16);
			}			
		}
		if (m_pEntireMap)
		{
			m_FocusLimit.right = m_FocusLimit.left + (m_pEntireMap->nWidth - m_Size.cx)* MAP_SCALE_H;
			m_FocusLimit.bottom = m_FocusLimit.top + (m_pEntireMap->nHeight - m_Size.cy)* MAP_SCALE_V;
			if (m_FocusLimit.right < m_FocusLimit.left)
				m_FocusLimit.right = m_FocusLimit.left;
			if (m_FocusLimit.bottom < m_FocusLimit.top)
				m_FocusLimit.bottom = m_FocusLimit.top;
		}
		else
		{
			m_bHavePicMap = false;
			return;
		}
	}
	
	int	nStartX = (m_PicLoadedLTPosition.x - m_EntireMapLTPosition.x) / MAP_SCALE_H;
	int nStartY = (m_PicLoadedLTPosition.y - m_EntireMapLTPosition.y) / MAP_SCALE_V;
	
	SIZE	PicEntireSize;
	PicEntireSize.cx = m_pEntireMap->nWidth;
	PicEntireSize.cy = m_pEntireMap->nHeight;
	POINT	DestPos;
	
	int	h, v, x, y, nFromX, nFromY, nToX, nToY;
	for (v = 0, y = nStartY; v < MAP_CELL_MAX_RANGE; v++, y += MAP_CELL_MAP_HEIGHT)
	{
		nToY = y + MAP_CELL_MAP_HEIGHT;
		bool bCleared = false;
		if (y < 0 || nToY > PicEntireSize.cy)
		{//纵向没有全部落在图内
			for (h = 0; h < MAP_CELL_MAX_RANGE; h++)
			{
				g_pRepresent->ClearImageData(m_ElemsList[v][h].szImageName,
					m_ElemsList[v][h].uImageId, m_ElemsList[v][h].sISPosition);
			}
			bCleared = true;
		}
		if (y < PicEntireSize.cy && nToY > 0)
		{
			if (y >= 0)
			{
				nFromY = y;
				DestPos.y = 0;
			}
			else
			{
				nFromY = 0;
				DestPos.y = -y;
			}
			
			if (nToY > PicEntireSize.cy)
				nToY = PicEntireSize.cy;
			
			for (h = 0, x = nStartX; h < MAP_CELL_MAX_RANGE; h++, x += MAP_CELL_MAP_WIDTH)
			{
				nToX = x + MAP_CELL_MAP_WIDTH;
				if (bCleared == false && (x < 0 || nToX > PicEntireSize.cx))
				{//横向没有全部落在图内
					g_pRepresent->ClearImageData(m_ElemsList[v][h].szImageName,
						m_ElemsList[v][h].uImageId, m_ElemsList[v][h].sISPosition);
				}
				
				if (x < PicEntireSize.cx && nToX > 0)
				{
					if (x >= 0)
					{
						nFromX = x;
						DestPos.x = 0;
					}
					else
					{
						nFromX = 0;
						DestPos.x = -x;
					}
					if (nToX > PicEntireSize.cx)
						nToX = PicEntireSize.cx;
					
					short *pBuf = (short*)g_pRepresent->GetBitmapDataBuffer(
						m_ElemsList[v][h].szImageName, NULL);
					if (pBuf)
					{
						short* pEntire = (short*)(&m_pEntireMap->Data) +
							nFromY * PicEntireSize.cx + nFromX;
						short* pDest = pBuf + MAP_CELL_MAP_WIDTH * DestPos.y + DestPos.x;
						
						for (int i = nFromY; i < nToY; i++)
						{
							memcpy(pDest, pEntire, (nToX - nFromX) * 2);
							pEntire += PicEntireSize.cx;
							pDest += MAP_CELL_MAP_WIDTH;
						}
						g_pRepresent->ReleaseBitmapDataBuffer(
							m_ElemsList[v][h].szImageName, pBuf);
					}
				}
			}
		}
	}
}

void KScenePlaceMapC::CalcPicLayout()
{
	POINT	ShowLTPos;
	
	ShowLTPos.x = m_MapCoverArea.left - m_PicLoadedLTPosition.x / MAP_SCALE_H ;
	ShowLTPos.y = m_MapCoverArea.top  - m_PicLoadedLTPosition.y / MAP_SCALE_V ;
	//	ShowLTPos.x = (m_FocusPosition.x - m_PicLoadedLTPosition.x) / MAP_SCALE_H - m_Size.cx / 2;
	//	ShowLTPos.y = (m_FocusPosition.y - m_PicLoadedLTPosition.y) / MAP_SCALE_V - m_Size.cy / 2;
	
	m_PaintCell.left = ShowLTPos.x / MAP_CELL_MAP_WIDTH;
	m_PaintCell.top = ShowLTPos.y / MAP_CELL_MAP_HEIGHT;
	
	m_FirstCellSkipWidth.cx = ShowLTPos.x - m_PaintCell.left * MAP_CELL_MAP_WIDTH;
	m_FirstCellSkipWidth.cy = ShowLTPos.y - m_PaintCell.top * MAP_CELL_MAP_HEIGHT;
	
	int nTemp = m_Size.cx + m_FirstCellSkipWidth.cx + MAP_CELL_MAP_WIDTH - 1;
	m_PaintCell.right = nTemp / MAP_CELL_MAP_WIDTH;
	m_LastCellSkipHeight.cx = nTemp - MAP_CELL_MAP_WIDTH * m_PaintCell.right;
	m_PaintCell.right += m_PaintCell.left;
	
	nTemp = m_Size.cy + m_FirstCellSkipWidth.cy + MAP_CELL_MAP_HEIGHT - 1;
	m_PaintCell.bottom = nTemp / MAP_CELL_MAP_HEIGHT;
	m_LastCellSkipHeight.cy = nTemp - MAP_CELL_MAP_HEIGHT * m_PaintCell.bottom;
	m_PaintCell.bottom += m_PaintCell.top;
}

//清除
void KScenePlaceMapC::Free()
{
	if (m_pEntireMap)
	{
		release_image(m_pEntireMap);
		m_pEntireMap = NULL;
	}
}
#include "Macro.h"
//绘制

void KScenePlaceMapC::PaintMiniMapOnDC(ChatPoint& point,int nbShowCharacter)
{
	int nX = point.x;
	int nY = point.y;
	unsigned long hDrawDC = point.hdc;
	unsigned long hBitmap = 0;
	if (m_bHavePicMap && g_pRepresent)
	{
		//----绘制缩略图----
		if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
			PaintMiniMapPicOnDC(nX, nY,hDrawDC);
		if(nbShowCharacter)
		{
			//g_DirtyNpcSet.Check();
			
			PaintMiniMapCharactersOnDC(point);
			int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			if (nNpcIdx >= 0 && Npc[nNpcIdx].m_RegionIndex >= 0)
			{
				int nNpcX = LOWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H +
					Npc[nNpcIdx].GetMapX() * 2;
				int nNpcY = HIWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V +
					Npc[nNpcIdx].GetMapY();
				
				if (nNpcX >= m_MapCoverArea.left && nNpcX < m_MapCoverArea.right &&
					nNpcY >= m_MapCoverArea.top  && nNpcY < m_MapCoverArea.bottom)
				{ 
					//=====>Modifed by Ray 2004-3-17
					//让人物在小地图上的位置表示用Spr图像
					KRUImage	FootSpot;
					
					hBitmap = point.bitmapHandle[player_slef_bitmap_idx];
					//FootSpot.Color.Color_dw = m_uSelfColor;
					FootSpot.Color.Color_dw = 0xffffffff;
					FootSpot.nType = ISI_T_BITMAP16;
					

					
					//Lucifer~yu(zhangjianyu) 10/12/2006 Modify temp 
					//Begin-------------------------------------------------------------------
					FootSpot.nFrame = 0;
					//End---------------------------------------------------------------------
					FootSpot.bRenderFlag = 1;
					FootSpot.bRenderStyle = IMAGE_RENDER_STYLE_WITH_HANDLE;
					FootSpot.oPosition.nX = nX + nNpcX - m_MapCoverArea.left - 1;
					FootSpot.oPosition.nY = nY + nNpcY - m_MapCoverArea.top  - 1;
					FootSpot.oEndPos.nX = FootSpot.oPosition.nX + 3;
					FootSpot.oEndPos.nY = FootSpot.oPosition.nY + 3;
					FootSpot.hEffectDrawDC = hDrawDC;
					FootSpot.hEffectBitmap = hBitmap;
					g_pRepresent->DrawPrimitives(1, &FootSpot, RU_T_IMAGE_ON_DC, true);
				}
			}

		}

	}

}
void KScenePlaceMapC::PaintMiniMap(int nX, int nY, int nbShowCharacter)
{
	if (m_bHavePicMap && g_pRepresent)
	{
		//----绘制缩略图----
		if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
			PaintMiniMapPic(nX, nY);
		
		if(nbShowCharacter)
		{
			//g_DirtyNpcSet.Check();
			
			PaintMiniMapCharacters(nX, nY);
			
			//g_DirtyNpcSet.Sort();
			
			//PaintDirtyNpcSet(nX, nY);
			
			//g_DirtyNpcSet.ClearTemp();
			
			
			//---绘制自己位置----
			int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			if (nNpcIdx >= 0 && Npc[nNpcIdx].m_RegionIndex >= 0)
			{
				int nNpcX = LOWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H +
					Npc[nNpcIdx].GetMapX() * 2;
				int nNpcY = HIWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V +
					Npc[nNpcIdx].GetMapY();
				
				if (nNpcX >= m_MapCoverArea.left && nNpcX < m_MapCoverArea.right &&
					nNpcY >= m_MapCoverArea.top  && nNpcY < m_MapCoverArea.bottom)
				{
					KRUImage     FootSpot;
					const char * szImage = NULL; 

				/*	if (Npc[nNpcIdx].IsInWorldCombatInstance() && IsValidCombatID(Npc[nNpcIdx].m_WorldCombatOrg))
					{
						szImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_world_combat_minimap_image,Npc[nNpcIdx].m_WorldCombatOrg);
					}
					else*/
						szImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_mini_map_info,MMI_SELF);

					if (szImage)
					{
						strcpy(FootSpot.szImage, szImage);
						
						//FootSpot.Color.Color_dw = m_uSelfColor;
						FootSpot.Color.Color_dw = 0xffffffff;
						FootSpot.nType = ISI_T_SPR;
						
						int nTmp = Npc[nNpcIdx].m_ResDir;
						
						if(nTmp >= 0 && nTmp <=3				//取人物方向,在原来的KNpc.h中将m_ResDir改为public成员
							|| nTmp>=60 &&nTmp <=63	)
							FootSpot.nFrame = 0;
						else
							FootSpot.nFrame = (nTmp + 4) / 8;
						
						//Lucifer~yu(zhangjianyu) 10/12/2006 Modify temp 
						//Begin-------------------------------------------------------------------
						FootSpot.nFrame = 0;
						//End---------------------------------------------------------------------
						FootSpot.bRenderFlag = 1;
						FootSpot.bRenderStyle = 0;
						FootSpot.oPosition.nX = nX + nNpcX - m_MapCoverArea.left - 1;
						FootSpot.oPosition.nY = nY + nNpcY - m_MapCoverArea.top  - 1;
						FootSpot.oEndPos.nX = FootSpot.oPosition.nX + 3;
						FootSpot.oEndPos.nY = FootSpot.oPosition.nY + 3;
						
						g_pRepresent->DrawPrimitives(1, &FootSpot, RU_T_IMAGE, true);
					}
				}
			}
		}
		
		PaintMiniMapAutorunDest(nX, nY);
	}
}

void KScenePlaceMapC::PaintSceneMap(bool isCurrentMap,int nbShowCharacter, int nX, int nY)
{
	if (m_bHavePicMap && g_pRepresent)
	{
		//----绘制缩略图----
		//if (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PIC)
		{
			if ( nX == -1 && nY == -1 )
			{
				int nScreenW = g_GetScreenWidth();
				int	nScreenH = g_GetScreenHeight();
				nX = (nScreenW - m_nSceneMapWidth) * 0.5;
				nY = (nScreenH - m_nSceneMapHeight) * 0.5;
			}
			PaintSceneMapPic(nX, nY);
		}
		
		PaintSceneMapNpcName(nX, nY);
		
		if(nbShowCharacter)
		{
			//g_DirtyNpcSet.Check();
			
			PaintSceneMapSyncToWorldNpc(nX, nY);
			
			PaintSceneMapCharacters(nX, nY);
			
			//g_DirtyNpcSet.Sort();
			
			//PaintDirtyNpcSet(nX, nY);
			
			//g_DirtyNpcSet.ClearTemp();
			
			//---绘制自己位置----
			int nNpcIdx = Player[CLIENT_PLAYER_INDEX].m_nIndex;
			if (nNpcIdx >= 0 && Npc[nNpcIdx].m_RegionIndex >= 0)
			{
				int nWorldH = SubWorld[0].m_nWorldRegionHeight * REGION_PIXEL_HEIGHT;
				int nWorldW = SubWorld[0].m_nWorldRegionWidth * REGION_PIXEL_WIDTH;
				
				int nNpcX = 0;
				int nNpcY = 0;
				
				Npc[nNpcIdx].GetMpsPos( &nNpcX, &nNpcY );
				
				float xposPercent = ((float)(nNpcX - (SubWorld[0].m_nRegionBeginX) * REGION_PIXEL_WIDTH)) / nWorldW;
				float yposPercent = ((float)(nNpcY - (SubWorld[0].m_nRegionBeginY) * REGION_PIXEL_HEIGHT)) / nWorldH;
				nNpcX = m_nSceneMapArea.x + m_nSceneMapArea.width * xposPercent;
				nNpcY = m_nSceneMapArea.y + m_nSceneMapArea.height * yposPercent;
				
				// 				if (nNpcX >= m_MapCoverArea.left && nNpcX < m_MapCoverArea.right &&
				// 					nNpcY >= m_MapCoverArea.top  && nNpcY < m_MapCoverArea.bottom)
				{
					const char * szImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_mini_map_info, MMI_SCENE_MAP_SELF_ICON);
					if (szImage)
					{
						strcpy(m_sceneMapSelfImage.szImage, szImage);
						
						//FootSpot.Color.Color_dw = m_uSelfColor;
						m_sceneMapSelfImage.Color.Color_dw = 0xffffffff;
						m_sceneMapSelfImage.nType = ISI_T_SPR;
						
						int nTmp = Npc[nNpcIdx].m_ResDir;
						
// 						if(nTmp >= 0 && nTmp <=3				//取人物方向,在原来的KNpc.h中将m_ResDir改为public成员
// 							|| nTmp>=60 &&nTmp <=63	)
// 							FootSpot.nFrame = 0;
// 						else
// 							FootSpot.nFrame = (nTmp + 4) / 8;
						//Lucifer~yu(zhangjianyu) 10/12/2006 Modify temp 
						//Begin-------------------------------------------------------------------
//						m_sceneMapSelfImage.nFrame = 0;
						//End---------------------------------------------------------------------		
						m_sceneMapSelfImage.bRenderFlag = 1;
						m_sceneMapSelfImage.bRenderStyle = 0;
						m_sceneMapSelfImage.oPosition.nX = nX + nNpcX/* - m_MapCoverArea.left*/ - 1;
						m_sceneMapSelfImage.oPosition.nY = nY + nNpcY/* - m_MapCoverArea.top*/  - 1;
						m_sceneMapSelfImage.oEndPos.nX = m_sceneMapSelfImage.oPosition.nX + 3;
						m_sceneMapSelfImage.oEndPos.nY = m_sceneMapSelfImage.oPosition.nY + 3;
						
						g_pRepresent->DrawPrimitives(1, &m_sceneMapSelfImage, RU_T_IMAGE, true);

						IR_NextFrame(m_sceneMapSelfImage);
					}
				}
			}
		}
		///不是当前地图就不绘制光标////
		if(isCurrentMap)
			PaintSceneMapAutorunDest(nX, nY);
	}
}

void KScenePlaceMapC::gotoPosition(int x, int y)
{
	float xPos = ((float)(x - m_nSceneMapArea.x)) / m_nSceneMapArea.width;
	float yPos = ((float)(y - m_nSceneMapArea.y)) / m_nSceneMapArea.height;
	
	if(xPos < 0)
		return;
	if(yPos < 0)
		return;
	
	AutoRobotMgr::Singleton().AutoRunTo(xPos, yPos);
}

void KScenePlaceMapC::PaintDirtyNpcSet(int nX, int nY)
{
	// 	bool bCharacters = (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_CHARACTER) != 0;
	// 	bool bPartners = (m_uMapShowElems & SCENE_PLACE_MAP_ELEM_PARTNER) != 0;
	// 	if (bCharacters == false || bPartners == false)
	// 		return;
	// 
	// #define	MAX_NUM_PARTNER		16
	// 	int			nNumPartner = 0;
	// 	int			nIsInTeam = Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag;
	// 
	// #define	MAX_NUM_CHARACTER	40
	// 	KRUShadow	FootSpot[MAX_NUM_CHARACTER];
	// 	int			nNumSpot = 0;
	// 
	// 	// 显示其他玩家和普通npc
	// 	g_DirtyNpcSet.Front();
	// 	DirtyNpcItem* pItem = g_DirtyNpcSet.GetNextItem();
	// 	while (pItem)
	// 	{
	// 	//	if (pItem->bInCurRegion)
	// 	//	{
	// 	//		pItem = g_DirtyNpcSet.GetNextItem();
	// 	//		continue;
	// 	//	}
	// 
	// 		int nNpcX = LOWORD(pItem->dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H +
	// 						pItem->nMapX * 2;
	// 		int nNpcY = HIWORD(pItem->dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V +
	// 						pItem->nMapY;
	// 		if ((nNpcX < m_MapCoverArea.left + 8) || (nNpcX >= m_MapCoverArea.right - 8) ||
	// 			(nNpcY < m_MapCoverArea.top + 8) || (nNpcY >= m_MapCoverArea.bottom - 8))
	// 		{
	// 			pItem = g_DirtyNpcSet.GetNextItem();
	// 			continue;
	// 		}
	// 
	// 		if (-1 != SubWorld[0].FindRegion(pItem->dwRegionID))
	// 			pItem->bInCurRegion = true;
	// 			
	// 
	// 		bool			bValidNpc = true;
	// 		unsigned int	uColor = 0x0;
	// 
	// 		if (bValidNpc)
	// 		{
	// 			FootSpot[nNumSpot].Color.Color_dw = uColor;
	// 			FootSpot[nNumSpot].oPosition.nX = nX + nNpcX - m_MapCoverArea.left - 1;
	// 			FootSpot[nNumSpot].oPosition.nY = nY + nNpcY - m_MapCoverArea.top  - 1;
	// 			FootSpot[nNumSpot].oEndPos.nX = FootSpot[nNumSpot].oPosition.nX + 3;
	// 			FootSpot[nNumSpot].oEndPos.nY = FootSpot[nNumSpot].oPosition.nY + 3;
	// 			if (pItem->btKind == kind_building)
	// 			{
	// 				KRUImage img;
	// 				unsigned int uId = Buildings.GetBuildingIdByNpcSettingId(pItem->nSettingIdx);
	// 				Buildings.GetSmallIconName(uId, img.szImage, FILE_NAME_LENGTH);
	// 				img.nType = ISI_T_SPR;
	// 				img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
	// 				img.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	// 				img.Color.Color_b.a = 0;
	// 				img.nISPosition = IMAGE_IS_POSITION_INIT;
	// 				img.uImage = 0;
	// 				img.nFrame = 0;
	// 				
	// 				img.oPosition.nX = FootSpot[nNumSpot].oPosition.nX;
	// 				img.oPosition.nY = FootSpot[nNumSpot].oPosition.nY;
	// 				img.oPosition.nZ = 0;
	// 				
	// 				img.oEndPos.nX = FootSpot[nNumSpot].oEndPos.nX;
	// 				img.oEndPos.nY = FootSpot[nNumSpot].oEndPos.nY;
	// 				
	// 				g_pRepresent->DrawPrimitives(1, &img, RU_T_IMAGE, true);
	// 			}
	// 			else
	// 			{
	// 				g_pRepresent->DrawPrimitives(1, &FootSpot[nNumSpot], RU_T_SHADOW, true);
	// 			}
	// 			nNumSpot++;
	// 		
	// 
	// 			if (nNumSpot == MAX_NUM_CHARACTER)
	// 			{
	// 				g_pRepresent->DrawPrimitives(MAX_NUM_CHARACTER, &FootSpot[0], RU_T_SHADOW, true);
	// 				nNumSpot = 0;
	// 			}
	// 		}
	// 		
	// 		pItem = g_DirtyNpcSet.GetNextItem();
	// 	}
}
/*#define _SIZHUZHANG_DC_BITMAP_NAME */
/*
*/
void KScenePlaceMapC::PaintMiniMapCharactersOnDC(ChatPoint& point)
{
	#define	    MAX_NUM_CHARACTER	40	//小地图上的点最大显示个数
	KRUImage	FootSpot[MAX_NUM_CHARACTER];
	
	ConfigManager &  mgr = ConfigManager::Singleton();
	
	int isInTeam = Player[CLIENT_PLAYER_INDEX].GetTeamInfo().IsInTeam();
	int selfNpcIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	
	int nNumSpot = 0;
	int nNpcIdx = 0;
	
	vector<MapNpcMgr::NpcInfo>& npcList = MapNpcMgr::getSingleton().loadMap(_mapName);
	
	KUiNewFont newFont;
	newFont.bDefaultFont = false;
	strcpy( newFont.szName, "stzhongs-9");
	newFont.uColor = 0xffffff00;
	int nX = point.x;
	int nY = point.y;
	unsigned long hResource = 0;
	unsigned long hDrawDC = point.hdc;
	for(int i = 0; i < npcList.size(); ++i)
	{
		int nNpcX = npcList[i].x * 2;
		int nNpcY = npcList[i].y * 2;
		if (nNpcX < m_MapCoverArea.left || nNpcX >= m_MapCoverArea.right ||
			nNpcY < m_MapCoverArea.top  || nNpcY >= m_MapCoverArea.bottom)
		{
			continue;
		}
		
		strcpy(newFont.szContext, npcList[i].npcName.c_str());
		
		newFont.nX = nNpcX - m_MapCoverArea.left + nX;
		newFont.nY = nNpcY - m_MapCoverArea.top + nY;
//		CoreDataChanged(GDCNI_DRAWTEXT, (unsigned int)&newFont, 1);
	}
	
	while (nNpcIdx = NpcSet.GetNextIdx(nNpcIdx))
	{
		KNpc& curNpc = Npc[nNpcIdx];
		if (Npc[nNpcIdx].m_RegionIndex == -1)
			continue;
		
		int nNpcX = LOWORD(curNpc.m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H + curNpc.GetMapX() * 2;
		int nNpcY = HIWORD(curNpc.m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V + curNpc.GetMapY();
		if (nNpcX < m_MapCoverArea.left || nNpcX >= m_MapCoverArea.right ||
			nNpcY < m_MapCoverArea.top  || nNpcY >= m_MapCoverArea.bottom)
		{
			continue;
		}
		
		bool bValidNpc = false;
		
		if(nNpcIdx == selfNpcIndex)
		{
			//自己已经画过了
			continue;
		}
		else if(curNpc.m_Kind == kind_player)
		{
			const char* npcZhuhouName  = Npc[nNpcIdx].GetZhuhouName();
			const char* selfZhuhouName = Npc[selfNpcIndex].GetZhuhouName();
			const char* npcShizuName   = Npc[nNpcIdx].GetShizuName();
			const char* selfShizuName  = Npc[selfNpcIndex].GetShizuName();
			
			//双方均有社会关系
			if (selfShizuName [0] && npcShizuName[0])
			{
				if( strcmp(selfShizuName, npcShizuName)==0 )
				{
					if (Npc[nNpcIdx].IsGensMgr())//氏族长
					{
						hResource = point.bitmapHandle[player_the_same_tong_bitmap_idx];
					}//endif
					else
					{//同氏族的
						hResource = point.bitmapHandle[player_the_same_tong_bitmap_idx];
					}//end else
					
					bValidNpc = true;
				}//endif
				
				if(selfZhuhouName[0] && npcZhuhouName[0] && strcmp(selfZhuhouName, npcZhuhouName)==0)
				{
					if (Npc[nNpcIdx].IsKing())//诸侯长
					{
						hResource = point.bitmapHandle[sizhuzhang_bitmap_idx];
					}//endif
					else
					{//同一个诸侯的
						hResource = point.bitmapHandle[player_the_same_tong_bitmap_idx];
					
					}//end else
					
					bValidNpc = true;
				}//endif
				
				if (!bValidNpc)
				{//一般
					if(Npc[nNpcIdx].IsKing())
					{
						hResource = point.bitmapHandle[zhuhouzhang_bitmap_idx];
					}
					else
					if(Npc[nNpcIdx].IsGensMgr())
					{
						hResource = point.bitmapHandle[normal_player_bitmap_idx];
					}
					else
						hResource = point.bitmapHandle[normal_player_bitmap_idx];
					
					bValidNpc = true;
				}//endif
				
			}//endif
			
			if(isInTeam)
			{
				if (g_TeamC[0].FindMemberID(curNpc.m_dwID) >= 0)
				{
					if (g_TeamC[0].GetCaptain() == curNpc.m_dwID)
					{//队长
						hResource = point.bitmapHandle[player_team_bitmap_idx];
						
					}//endif
					else
					{//小队成员
						hResource = point.bitmapHandle[player_team_bitmap_idx];
						
					}//end else
					
					bValidNpc = true;
				}//endif			
			}
			
			if(    curNpc.m_UnaryAttrMgr[nuai_titlecolor] == mgr.GetConfigurableColor(color_pk_punish)
				|| curNpc.m_UnaryAttrMgr[nuai_titlecolor] == mgr.GetConfigurableColor(color_pk_demon))
			{//恶人
				hResource = point.bitmapHandle[badplayer_bitmap_idx];
				
				bValidNpc = true;
			}
		}
		else if(curNpc.m_Kind == kind_dialoger)
		{//任务
			//如果是非战斗NPC
			hResource = point.bitmapHandle[normal_npc_bitmap_idx];
			
			bValidNpc = true;
		}
		else if(curNpc.m_Kind == kind_normal)
		{//
			hResource = point.bitmapHandle[creature_bitmap_idx];
			bValidNpc = true;
		}
		if (Npc[nNpcIdx].IsInWorldCombatInstance() && IsValidCombatID(Npc[nNpcIdx].m_WorldCombatOrg))
		{
			hResource = point.bitmapHandle[npc_type_icon_number-1+Npc[nNpcIdx].m_WorldCombatOrg];
			if(hResource == 0)
				bValidNpc = false;
			bValidNpc = true;
		}//endif
		if (bValidNpc)
		{
			//显示
			FootSpot[nNumSpot].Color.Color_dw = 0xffffffff;
			FootSpot[nNumSpot].nType = ISI_T_BITMAP16;
			FootSpot[nNumSpot].nFrame = 0;
			FootSpot[nNumSpot].oPosition.nX		= nX + nNpcX - m_MapCoverArea.left - 1;
			FootSpot[nNumSpot].oPosition.nY		= nY + nNpcY - m_MapCoverArea.top  - 1;
			FootSpot[nNumSpot].oEndPos.nX		= FootSpot[nNumSpot].oPosition.nX + 3;
			FootSpot[nNumSpot].oEndPos.nY		= FootSpot[nNumSpot].oPosition.nY + 3;
			FootSpot[nNumSpot].uImage			= 0;
			FootSpot[nNumSpot].bRenderFlag		= 1;
			FootSpot[nNumSpot].bRenderStyle		= IMAGE_RENDER_STYLE_WITH_HANDLE;
			FootSpot[nNumSpot].hEffectBitmap  = hResource;
			FootSpot[nNumSpot].hEffectDrawDC = hDrawDC;
			g_pRepresent->DrawPrimitives(1, &FootSpot[nNumSpot], RU_T_IMAGE_ON_DC, true);
			
			nNumSpot++;
			
			if (nNumSpot >= MAX_NUM_CHARACTER)
			{
				nNumSpot = MAX_NUM_CHARACTER - 1;
				break;
			}
		}
	}
}
void KScenePlaceMapC::PaintMiniMapCharacters(int nX, int nY)
{
#define	    MAX_NUM_CHARACTER	40	//小地图上的点最大显示个数
	KRUImage	FootSpot[MAX_NUM_CHARACTER];
	
	ConfigManager &  mgr = ConfigManager::Singleton();
	
	int isInTeam = Player[CLIENT_PLAYER_INDEX].GetTeamInfo().IsInTeam();
	int selfNpcIndex = Player[CLIENT_PLAYER_INDEX].m_nIndex;
	
	int nNumSpot = 0;
	int nNpcIdx = 0;

//	//显示MapInfo填写的NPC的名字
// 	vector<MapNpcMgr::NpcInfo>& npcList = MapNpcMgr::getSingleton().loadMap(_mapName);
// 	
// 	KUiNewFont newFont;
// 	newFont.bDefaultFont = false;
// 	strcpy( newFont.szName, "stzhongs-9");
// 	newFont.uColor = 0xffffff00;
// 	for(int i = 0; i < npcList.size(); ++i)
// 	{
// 		int nNpcX = npcList[i].x * 2;
// 		int nNpcY = npcList[i].y * 2;
// 		if (nNpcX < m_MapCoverArea.left || nNpcX >= m_MapCoverArea.right ||
// 			nNpcY < m_MapCoverArea.top  || nNpcY >= m_MapCoverArea.bottom)
// 		{
// 			continue;
// 		}
// 		
// 		strcpy(newFont.szContext, npcList[i].npcName.c_str());
// 		
// 		newFont.nX = nNpcX - m_MapCoverArea.left + nX;
// 		newFont.nY = nNpcY - m_MapCoverArea.top + nY;
// 		CoreDataChanged(GDCNI_DRAWTEXT, (unsigned int)&newFont, 1);
// 	}
	
	while (nNpcIdx = NpcSet.GetNextIdx(nNpcIdx))
	{
		KNpc& curNpc = Npc[nNpcIdx];
		if (Npc[nNpcIdx].m_RegionIndex == -1)
			continue;
		
		int nNpcX = LOWORD(curNpc.m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H + curNpc.GetMapX() * 2;
		int nNpcY = HIWORD(curNpc.m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V + curNpc.GetMapY();
		if (nNpcX < m_MapCoverArea.left || nNpcX >= m_MapCoverArea.right ||
			nNpcY < m_MapCoverArea.top  || nNpcY >= m_MapCoverArea.bottom)
		{
			continue;
		}
		
		bool bValidNpc = false;
		
		if(nNpcIdx == selfNpcIndex)
		{
			//自己已经画过了
			continue;
		}
		else if(curNpc.m_Kind == kind_player)
		{
			const char* npcZhuhouName  = Npc[nNpcIdx].GetZhuhouName();
			const char* selfZhuhouName = Npc[selfNpcIndex].GetZhuhouName();
			const char* npcShizuName   = Npc[nNpcIdx].GetShizuName();
			const char* selfShizuName  = Npc[selfNpcIndex].GetShizuName();
			
			//双方均有社会关系
			if (selfShizuName [0] && npcShizuName[0])
			{
				if( strcmp(selfShizuName, npcShizuName)==0 )
				{
					if (Npc[nNpcIdx].IsGensMgr())
					{
						const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_SAME_GENS_OWNER);
						if (szImage)
							strcpy(FootSpot[nNumSpot].szImage, szImage);
					}//endif
					else
					{
						const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_THE_SAME_GENS);
						if (szImage)
							strcpy(FootSpot[nNumSpot].szImage, szImage);
					}//end else
					
					bValidNpc = true;
				}//endif
				
				if(selfZhuhouName[0] && npcZhuhouName[0] && strcmp(selfZhuhouName, npcZhuhouName)==0)
				{
					if (Npc[nNpcIdx].IsKing())
					{
						const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_SAME_TONG_OWNER);
						if (szImage)
							strcpy(FootSpot[nNumSpot].szImage, szImage);
					}//endif
					else
					{
						const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_THE_SAME_TONG);
						if (szImage)
							strcpy(FootSpot[nNumSpot].szImage, szImage);
					}//end else
					
					bValidNpc = true;
				}//endif
				
				if (!bValidNpc)
				{
					const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_NOT_THE_SAME_SOCIAL);
					if (szImage)
						strcpy(FootSpot[nNumSpot].szImage, szImage);

					if (Npc[nNpcIdx].IsGensMgr())
					{
						const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_OTHER_GENS_OWNER);
						if (szImage)
							strcpy(FootSpot[nNumSpot].szImage, szImage);
					}//endif

					if (Npc[nNpcIdx].IsKing())
					{
						const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_OTHER_TONG_OWNER);
						if (szImage)
							strcpy(FootSpot[nNumSpot].szImage, szImage);

					}//endif

					bValidNpc = true;
					
				}//endif
				
			}//endif
			
			if(isInTeam)
			{
				if (g_TeamC[0].FindMemberID(curNpc.m_dwID) >= 0)
				{
					if (g_TeamC[0].GetCaptain() == curNpc.m_dwID)
					{
						const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_TEAM_OWNER);
						if (szImage)
							strcpy(FootSpot[nNumSpot].szImage, szImage);
						
					}//endif
					else
					{
						const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_SAME_TEAM);
						if (szImage)
							strcpy(FootSpot[nNumSpot].szImage, szImage);
					}//end else
					
					bValidNpc = true;
				}//endif			
			}
			
			if(    curNpc.m_UnaryAttrMgr[nuai_titlecolor] == mgr.GetConfigurableColor(color_pk_punish)
				|| curNpc.m_UnaryAttrMgr[nuai_titlecolor] == mgr.GetConfigurableColor(color_pk_demon))
			{
				const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_RED_NAME);
				if (szImage)
					strcpy(FootSpot[nNumSpot].szImage, szImage);
				bValidNpc = true;
			}
		}
		else if(curNpc.m_Kind == kind_dialoger)
		{
			//对话NPC
			const char * szImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_mini_map_info, MMI_DIALOGER);			
			if (szImage)
			{
				sprintf(FootSpot[nNumSpot].szImage, szImage);
				bValidNpc = true;
			}

// 			const char * szImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_mini_map_info, MMI_QUEST_NPC_N);
// 			int questState = curNpc.m_DataRes.GetQuestIcon();
// 			if (szImage)
// 				sprintf(FootSpot[nNumSpot].szImage, szImage, questState);
// 			bValidNpc = true;
		}
		else if(curNpc.m_Kind == kind_normal)
		{
			const char * szImage = mgr.GetConfigurableDisplayStyle(style_mini_map_info,MMI_MONSTER);
			if (szImage)
				strcpy(FootSpot[nNumSpot].szImage, szImage);
			bValidNpc = true;
		}
		
		if (Npc[nNpcIdx].IsInWorldCombatInstance() && IsValidCombatID(Npc[nNpcIdx].m_WorldCombatOrg))
		{
			const char * szImage = mgr.GetConfigurableDisplayStyle(style_world_combat_minimap_image,Npc[nNpcIdx].m_WorldCombatOrg);
			if (szImage)
				strcpy(FootSpot[nNumSpot].szImage,szImage);

			bValidNpc = true;
		}//endif

		if (bValidNpc)
		{
			//显示
			FootSpot[nNumSpot].Color.Color_dw = 0xffffffff;
			FootSpot[nNumSpot].nType = ISI_T_SPR;
			FootSpot[nNumSpot].nFrame = 0;
			FootSpot[nNumSpot].oPosition.nX		= nX + nNpcX - m_MapCoverArea.left - 1;
			FootSpot[nNumSpot].oPosition.nY		= nY + nNpcY - m_MapCoverArea.top  - 1;
			FootSpot[nNumSpot].oEndPos.nX		= FootSpot[nNumSpot].oPosition.nX + 3;
			FootSpot[nNumSpot].oEndPos.nY		= FootSpot[nNumSpot].oPosition.nY + 3;
			FootSpot[nNumSpot].uImage			= 0;
			FootSpot[nNumSpot].bRenderFlag		= 1;
			FootSpot[nNumSpot].bRenderStyle		= 0;
			
			g_pRepresent->DrawPrimitives(1, &FootSpot[nNumSpot], RU_T_IMAGE, true);
			
			nNumSpot++;
			
			if (nNumSpot >= MAX_NUM_CHARACTER)
			{
				nNumSpot = MAX_NUM_CHARACTER - 1;
				break;
			}
		}
	}
	
}

void KScenePlaceMapC::PaintSceneMapNpcName(int nX, int nY)
{	
	int nWorldH = m_curSceneMapGridArea.height * REGION_PIXEL_HEIGHT;
	int nWorldW = m_curSceneMapGridArea.width * REGION_PIXEL_WIDTH;

	int beginX = m_curSceneMapGridArea.x * REGION_PIXEL_WIDTH;
	int beginY = m_curSceneMapGridArea.y * REGION_PIXEL_HEIGHT;
	vector<MapNpcMgr::NpcInfo>& npcList = MapNpcMgr::getSingleton().loadMap(_mapName);
	
// 	KUiNewFont newFont;
// 	newFont.bDefaultFont = false;
// 	strcpy( newFont.szName, "stzhongs-9");
// 	newFont.uColor = 0xffffff00;

	for(int i = 0; i < npcList.size(); ++i)
	{
		int nNpcX = (float)(npcList[i].x * 32 - beginX) * m_nSceneMapArea.width / nWorldW + nX + m_nSceneMapArea.x;
		int nNpcY = (float)(npcList[i].y * 64 - beginY) * m_nSceneMapArea.height / nWorldH + nY + m_nSceneMapArea.y;

//		//显示名字
// 		strcpy(newFont.szContext, npcList[i].npcName.c_str());
// 		newFont.nX = nNpcX;
// 		newFont.nY = nNpcY;
// 		CoreDataChanged(GDCNI_DRAWTEXT, (unsigned int)&newFont, 1);

		//显示图标
		KRUImage FootSpot;		
		const char * szImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_mini_map_info, MMI_DIALOGER);
		if (szImage)
		{
			sprintf(FootSpot.szImage, szImage);
			FootSpot.Color.Color_dw = 0xffffffff;
			FootSpot.nType = ISI_T_SPR;
			FootSpot.nFrame = 0;
			FootSpot.oPosition.nX = nNpcX/* - m_MapCoverArea.left */- 1;
			FootSpot.oPosition.nY = nNpcY/* - m_MapCoverArea.top  */- 1;
			FootSpot.oEndPos.nX = FootSpot.oPosition.nX + 3;
			FootSpot.oEndPos.nY = FootSpot.oPosition.nY + 3;
			FootSpot.uImage = 0;
			FootSpot.bRenderFlag = 1;
			FootSpot.bRenderStyle = 0;
			
			g_pRepresent->DrawPrimitives(1, &FootSpot, RU_T_IMAGE, true);
		}
	}
}

void KScenePlaceMapC::PaintSceneMapCharacters(int nX, int nY)
{
	KPlayer& player = GetClientPlayer();
	bool isInTeam = player.GetTeamInfo().IsInTeam();
	int selfNpcIndex = player.m_nIndex;

	int nWorldH = SubWorld[0].m_nWorldRegionHeight * REGION_PIXEL_HEIGHT;
	int nWorldW = SubWorld[0].m_nWorldRegionWidth * REGION_PIXEL_WIDTH;

	if (!IsValidNpc(selfNpcIndex))
		return;

	bool IsInWorldCombatMap = Npc[selfNpcIndex].IsInWorldCombatInstance();

	if (IsInWorldCombatMap && SubWorld[0].GetSyncType() != especial_sync_type_normal)
	{
		KSubWorld::WORLD_MAP_PLAYER_INFO_CACHE & PlayerInfoCache = SubWorld[0].m_PlayerInfoCache;
		
		for (int nIndex = 0 ; nIndex < PlayerInfoCache.m_Num ; nIndex ++ )
		{
			WORLD_PLAYER_INFO & info = PlayerInfoCache.m_PlayerInfos[nIndex];
			int nOrgId               = info.Orgnize;

			if (Npc[selfNpcIndex].m_dwID == info.NpcId)
				continue;

			const char *    orgImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_combat_map_org_image,nOrgId);
			if (orgImage == NULL || orgImage[0] == 0)
				continue;

			KRUImage memberSpot;			
			memberSpot.Color.Color_dw = 0xffffffff;
			memberSpot.nType = ISI_T_SPR;
			memberSpot.nFrame = 0;			
			memberSpot.bRenderFlag = 1;
			memberSpot.bRenderStyle = 0;
			memberSpot.uImage = 0;

			strcpy(memberSpot.szImage, orgImage);
							
			int posX = info.MapX;
			int posY = info.MapY;

			float xposPercent = ((float)(posX - (SubWorld[0].m_nRegionBeginX) * REGION_PIXEL_WIDTH)) / nWorldW;
			float yposPercent = ((float)(posY - (SubWorld[0].m_nRegionBeginY) * REGION_PIXEL_HEIGHT)) / nWorldH;
			posX = m_nSceneMapArea.x + m_nSceneMapArea.width * xposPercent;
			posY = m_nSceneMapArea.y + m_nSceneMapArea.height * yposPercent;
							
			memberSpot.oPosition.nX = nX + posX - 1;
			memberSpot.oPosition.nY = nY + posY - 1;
			memberSpot.oEndPos.nX = memberSpot.oPosition.nX + 3;
			memberSpot.oEndPos.nY = memberSpot.oPosition.nY + 3;
			g_pRepresent->DrawPrimitives(1, &memberSpot, RU_T_IMAGE, true);

		}//end for nIndex

	}
	else
	{
	
		KSubWorld::WAR_MAP_COMMANDER_INFO_CACHE & info = SubWorld[0].m_WarCommanderInfoCache;

		//绘制队友
		if (isInTeam)
		{
			KTeam* pTeam = player.GetTeamInfo().GetTeam();
			if (pTeam != NULL)
			{
				int selfMapId = SubWorld[0].m_SubWorldID;
				
				const char * memberImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_mini_map_info, MMI_SAME_TEAM);
				const char * captainImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_mini_map_info, MMI_TEAM_OWNER);
				if (memberImage && captainImage)
				{
					KRUImage memberSpot;			
					memberSpot.Color.Color_dw = 0xffffffff;
					memberSpot.nType = ISI_T_SPR;
					memberSpot.nFrame = 0;			
					memberSpot.bRenderFlag = 1;
					memberSpot.bRenderStyle = 0;
					int maxMemberCount = pTeam->GetMaxMemberCount();
					for (int memberIndex = 0; memberIndex < maxMemberCount; memberIndex++)
					{
						bool IsCommander = false;
						memberSpot.uImage = 0;

						ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(memberIndex);

						if (pMemberInfo != NULL)
						{
							if (pMemberInfo->NpcId > 0)
							{
								//如果队友是国战指挥员，不绘制
								WAR_COMMANDER_INFO* pCommanderInfo = NULL;	
								if (info.m_Num > 0)
									pCommanderInfo = info.m_PlayerInfos;

								if (pCommanderInfo && info.m_Num > 0 && info.m_Num <= MAX_WAR_COMMANDER_NUM)
								{
									for (int i = 0; i < info.m_Num; ++i)
									{
										if (pMemberInfo->NpcId == pCommanderInfo[i].dNpcId)
										{
											IsCommander = true;
											break;
										}
									}
								}

								if (IsCommander)
									continue;

								int mapId = pMemberInfo->MapId;
								if (mapId == selfMapId)
								{
									if (pTeam->GetCaptain() == pMemberInfo->NpcId)
										strcpy(memberSpot.szImage, captainImage);
									else
										strcpy(memberSpot.szImage, memberImage);
									
									int posX = pMemberInfo->PosX;
									int posY = pMemberInfo->PosY;
									float xposPercent = ((float)(posX - (SubWorld[0].m_nRegionBeginX) * REGION_PIXEL_WIDTH)) / nWorldW;
									float yposPercent = ((float)(posY - (SubWorld[0].m_nRegionBeginY) * REGION_PIXEL_HEIGHT)) / nWorldH;
									posX = m_nSceneMapArea.x + m_nSceneMapArea.width * xposPercent;
									posY = m_nSceneMapArea.y + m_nSceneMapArea.height * yposPercent;
									
									memberSpot.oPosition.nX = nX + posX - 1;
									memberSpot.oPosition.nY = nY + posY - 1;
									memberSpot.oEndPos.nX = memberSpot.oPosition.nX + 3;
									memberSpot.oEndPos.nY = memberSpot.oPosition.nY + 3;
									g_pRepresent->DrawPrimitives(1, &memberSpot, RU_T_IMAGE, true);
								}
							}
						}
					}			
				}
			}
		}

		//绘制国战指挥员
		if ( info.m_Num > 0 && info.m_Num <= MAX_WAR_COMMANDER_NUM )
		{
			for (int nIndex = 0 ; nIndex < info.m_Num ; nIndex ++ )
			{
				WAR_COMMANDER_INFO & commanderInfo = info.m_PlayerInfos[nIndex];
				int                  nDuty         = commanderInfo.nDuty;
				
				if (Npc[selfNpcIndex].m_dwID == commanderInfo.dNpcId)
					continue;
				
				const char *    commanderImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_war_commander_image, nDuty);
				if (commanderImage == NULL || commanderImage[0] == 0)
					continue;
				
				KRUImage memberSpot;			
				memberSpot.Color.Color_dw = 0xffffffff;
				memberSpot.nType = ISI_T_SPR;
				memberSpot.nFrame = 0;			
				memberSpot.bRenderFlag = 1;
				memberSpot.bRenderStyle = 0;
				memberSpot.uImage = 0;
				
				strcpy(memberSpot.szImage, commanderImage);
				
				int posX = commanderInfo.nMpsX;
				int posY = commanderInfo.nMpsY;
				
				float xposPercent = ((float)(posX - (SubWorld[0].m_nRegionBeginX) * REGION_PIXEL_WIDTH)) / nWorldW;
				float yposPercent = ((float)(posY - (SubWorld[0].m_nRegionBeginY) * REGION_PIXEL_HEIGHT)) / nWorldH;
				posX = m_nSceneMapArea.x + m_nSceneMapArea.width * xposPercent;
				posY = m_nSceneMapArea.y + m_nSceneMapArea.height * yposPercent;
				
				memberSpot.oPosition.nX = nX + posX - 1;
				memberSpot.oPosition.nY = nY + posY - 1;
				memberSpot.oEndPos.nX = memberSpot.oPosition.nX + 3;
				memberSpot.oEndPos.nY = memberSpot.oPosition.nY + 3;
				g_pRepresent->DrawPrimitives(1, &memberSpot, RU_T_IMAGE, true);
				
			}//end for nIndex
		}
		
	}

//  #define	MAX_NUM_CHARACTER	40
//  	KRUImage FootSpot[MAX_NUM_CHARACTER];
//  	int	nNumSpot = 0;
//  	int nNpcIdx = 0;
//  
//  	while (nNpcIdx = NpcSet.GetNextIdx(nNpcIdx))
//  	{
//  		KNpc& curNpc = Npc[nNpcIdx];
//  
//  		if (curNpc.m_RegionIndex == -1)
//  			continue;
//  
//  		int nNpcX = 0;
//  		int nNpcY = 0;
//  		
//  		Npc[nNpcIdx].GetMpsPos( &nNpcX, &nNpcY );
//  		
//  		float xposPercent = ((float)(nNpcX - (SubWorld[0].m_nRegionBeginX) * REGION_PIXEL_WIDTH)) / nWorldW;
//  		float yposPercent = ((float)(nNpcY - (SubWorld[0].m_nRegionBeginY) * REGION_PIXEL_HEIGHT)) / nWorldH;
//  		nNpcX = m_nSceneMapArea.x + m_nSceneMapArea.width * xposPercent;
//  		nNpcY = m_nSceneMapArea.y + m_nSceneMapArea.height * yposPercent;
//  
//  		bool bValidNpc = false;
//  	
//  		if(curNpc.m_Index == selfNpcIndex)
//  		{	
//  			//自己不在这里绘制
//  			continue;
//  		}
//  		else if(curNpc.m_Kind == kind_dialoger)
//  		{
//  			//对话NPC
//  			const char * szImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_mini_map_info,MMI_DIALOGER);			
//  			int questState = curNpc.m_DataRes.GetQuestIcon();
//  			if (szImage)
//  				sprintf(FootSpot[nNumSpot].szImage, szImage, questState);
//  
//  			bValidNpc = true;
//  		}
//  		else
//  		{
//  			// to do
//  		}
//  		if (bValidNpc)
//  		{
//  			FootSpot[nNumSpot].Color.Color_dw = 0xffffffff;
//  			FootSpot[nNumSpot].nType = ISI_T_SPR;
//  			FootSpot[nNumSpot].nFrame = 0;
//  			FootSpot[nNumSpot].oPosition.nX		= nX + nNpcX/* - m_MapCoverArea.left */- 1;
//  			FootSpot[nNumSpot].oPosition.nY		= nY + nNpcY/* - m_MapCoverArea.top  */- 1;
//  			FootSpot[nNumSpot].oEndPos.nX		= FootSpot[nNumSpot].oPosition.nX + 3;
//  			FootSpot[nNumSpot].oEndPos.nY		= FootSpot[nNumSpot].oPosition.nY + 3;
//  			FootSpot[nNumSpot].uImage			= 0;
//  			FootSpot[nNumSpot].bRenderFlag		= 1;
//  			FootSpot[nNumSpot].bRenderStyle		= 0;
//  			
//  			g_pRepresent->DrawPrimitives(1, &FootSpot[nNumSpot], RU_T_IMAGE, true);
//  			
//  			nNumSpot++;
//  			
//  			if (nNumSpot >= MAX_NUM_CHARACTER)
//  			{
//  				nNumSpot = MAX_NUM_CHARACTER - 1;
//  				break;
//  			}
//  		}
//  	}
}

//绘制缩略图
void KScenePlaceMapC::PaintMiniMapPicOnDC(int nX,int nY,unsigned long hDrawDC)
{
	KRUImagePart	Img;
	Img.bRenderFlag = 0;
	Img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
	Img.Color.Color_dw = 0;
	Img.nFrame = 0;
	Img.nType = ISI_T_BITMAP16;
	Img.oPosition.nY = nY;
	int nDstY = 0;
	for (int v = m_PaintCell.top; v < m_PaintCell.bottom; v++)
	{
		if (v != m_PaintCell.top)
			Img.oImgLTPos.nY = 0;
		else
			Img.oImgLTPos.nY = m_FirstCellSkipWidth.cy;
		if (v != m_PaintCell.bottom - 1)
			Img.oImgRBPos.nY = MAP_CELL_MAP_HEIGHT;
		else
			Img.oImgRBPos.nY = m_LastCellSkipHeight.cy;

		Img.oPosition.nX = nX;
		int nDstX = 0;
		for (int h = m_PaintCell.left; h < m_PaintCell.right; h++)
		{
			if (h != m_PaintCell.left)
				Img.oImgLTPos.nX = 0;
			else
				Img.oImgLTPos.nX = m_FirstCellSkipWidth.cx;
			if (h != m_PaintCell.right - 1)
				Img.oImgRBPos.nX = MAP_CELL_MAP_WIDTH;
			else
				Img.oImgRBPos.nX = m_LastCellSkipHeight.cx;

			Img.nISPosition = m_ElemsList[v][h].sISPosition;
			strcpy(Img.szImage, m_ElemsList[v][h].szImageName);
			Img.uImage = m_ElemsList[v][h].uImageId;
			Img.hEffectDrawDC = hDrawDC;
			{
				g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE_PART_ON_DC, true);
			}
			m_ElemsList[v][h].sISPosition = Img.nISPosition;
			m_ElemsList[v][h].uImageId = Img.uImage;

			Img.oPosition.nX += Img.oImgRBPos.nX - Img.oImgLTPos.nX;
			nDstX += Img.oImgRBPos.nX - Img.oImgLTPos.nX;
			
		}
		Img.oPosition.nY += Img.oImgRBPos.nY - Img.oImgLTPos.nY;
		nDstY += Img.oImgRBPos.nY - Img.oImgLTPos.nY;
	};

}
void KScenePlaceMapC::PaintMiniMapPic(int nX, int nY)
{
	//_ASSERT(g_pRepresent);

	KRUImagePart	Img;
	Img.bRenderFlag = 0;
	Img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
	Img.Color.Color_dw = 0;
	Img.nFrame = 0;
	Img.nType = ISI_T_BITMAP16;
	Img.oPosition.nY = nY;
	int nDstY = 0;
	for (int v = m_PaintCell.top; v < m_PaintCell.bottom; v++)
	{
		if (v != m_PaintCell.top)
			Img.oImgLTPos.nY = 0;
		else
			Img.oImgLTPos.nY = m_FirstCellSkipWidth.cy;
		if (v != m_PaintCell.bottom - 1)
			Img.oImgRBPos.nY = MAP_CELL_MAP_HEIGHT;
		else
			Img.oImgRBPos.nY = m_LastCellSkipHeight.cy;

		Img.oPosition.nX = nX;
		int nDstX = 0;
		for (int h = m_PaintCell.left; h < m_PaintCell.right; h++)
		{
			if (h != m_PaintCell.left)
				Img.oImgLTPos.nX = 0;
			else
				Img.oImgLTPos.nX = m_FirstCellSkipWidth.cx;
			if (h != m_PaintCell.right - 1)
				Img.oImgRBPos.nX = MAP_CELL_MAP_WIDTH;
			else
				Img.oImgRBPos.nX = m_LastCellSkipHeight.cx;

			Img.nISPosition = m_ElemsList[v][h].sISPosition;
			strcpy(Img.szImage, m_ElemsList[v][h].szImageName);
			Img.uImage = m_ElemsList[v][h].uImageId;

// 			if (m_bSmallMap)
// 			{
// 				KBitmapDataBuffInfo Info;
// 				void* pBuf = g_pRepresent->GetBitmapDataBuffer(Img.szImage, &Info);
// 				m_AlphaBmp.Blt(pBuf, Img.oImgLTPos.nX, Img.oImgLTPos.nY, MAP_CELL_MAP_WIDTH, 
// 					nDstX,	nDstY,
// 					Img.oImgRBPos.nX - Img.oImgLTPos.nX, Img.oImgRBPos.nY - Img.oImgLTPos.nY);
// 				g_pRepresent->ReleaseBitmapDataBuffer(Img.szImage, pBuf);
// 			}
// 			else
			{
				g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE_PART, true);
			}
			
			m_ElemsList[v][h].sISPosition = Img.nISPosition;
			m_ElemsList[v][h].uImageId = Img.uImage;

			Img.oPosition.nX += Img.oImgRBPos.nX - Img.oImgLTPos.nX;
			nDstX += Img.oImgRBPos.nX - Img.oImgLTPos.nX;
		}
		Img.oPosition.nY += Img.oImgRBPos.nY - Img.oImgLTPos.nY;
		nDstY += Img.oImgRBPos.nY - Img.oImgLTPos.nY;
	};

// 	if (m_bSmallMap)
// 		m_AlphaBmp.Draw(nX, nY, m_AlphaBmp.GetWidth(), m_AlphaBmp.GetHeight());
}

//绘制缩略图
void KScenePlaceMapC::PaintSceneMapPic(int nX, int nY)
{
	KRUImage	Img;
	Img.bRenderFlag		= 0;
	Img.bRenderStyle	= IMAGE_RENDER_STYLE_ALPHA;
	Img.Color.Color_dw	= 0;
	Img.nFrame			= 0;
	Img.nType			= ISI_T_BITMAP16;
	Img.oPosition.nX	= nX;
	Img.oPosition.nY	= nY;	
	strcpy( Img.szImage, m_szSceneMapFile );
	Img.uImage			= 0;

	g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE, true);
}

//设置小地图的大小（单位：像素点）
void KScenePlaceMapC::SetSize(int cx, int cy)
{
	if (m_Size.cx != cx || m_Size.cy != cy)
	{
		if (cx > MAP_MAX_SUPPORT_WIDTH)
			cx = MAP_MAX_SUPPORT_WIDTH;
		else if (cx < 0)
			cx = 0;
		if (cy > MAP_MAX_SUPPORT_HEIGHT)
			cy = MAP_MAX_SUPPORT_HEIGHT;
		else if (cy < 0)
			cy = 0;
		int nDLimit = (cx - m_Size.cx) * MAP_SCALE_H / 2;
		m_FocusLimit.left += nDLimit;
		if (m_FocusLimit.right != RIGHT_BOTTOM_NO_LIMIT)
		{
			m_FocusLimit.right -= nDLimit;
			if (m_FocusLimit.right < m_FocusLimit.left)
				m_FocusLimit.right = m_FocusLimit.left;
		}
		nDLimit = (cy - m_Size.cy) * MAP_SCALE_V / 2;
		m_FocusLimit.top += nDLimit;
		if (m_FocusLimit.bottom != RIGHT_BOTTOM_NO_LIMIT)
		{
			m_FocusLimit.bottom -= nDLimit;
			if (m_FocusLimit.bottom < m_FocusLimit.top)
				m_FocusLimit.bottom = m_FocusLimit.top;
		}

// 		if (m_Size.cx < cx && m_Size.cx != 0)
// 			m_bSmallMap = false;
// 		else
// 			m_bSmallMap = true;

		m_Size.cx = cx;
		m_Size.cy = cy;

		SetFocusPosition(m_FocusPosition.x, m_FocusPosition.y, true);
	}
}

//返回值表示是否有地图
int KScenePlaceMapC::GetMapRect(RECT* pRect)
{
	if (pRect)
	{
		pRect->left = m_EntireMapLTPosition.x;
		pRect->top  = m_EntireMapLTPosition.y;
		if (m_pEntireMap)
		{
			pRect->right = m_EntireMapLTPosition.x + m_pEntireMap->nWidth * MAP_SCALE_H;
			pRect->bottom = m_EntireMapLTPosition.y + m_pEntireMap->nHeight * MAP_SCALE_V;
		}
		else
		{
			pRect->right = m_EntireMapLTPosition.x;
			pRect->bottom = m_EntireMapLTPosition.y;
		}
	}
	return m_bHavePicMap;
}


//获得小地图的资讯，这是小地图的真实资讯，不使用场景坐标等
int KScenePlaceMapC::GetMapInfo(KSceneMapInfo *pInfo)
{
	pInfo->nFocusMinH = m_EntireMapLTPosition.x / MAP_SCALE_H;
	pInfo->nFocusMinV = m_EntireMapLTPosition.y / MAP_SCALE_V;
	pInfo->nOrigFocusH= m_FocusPosition.x / MAP_SCALE_H;
	pInfo->nOrigFocusV= m_FocusPosition.y / MAP_SCALE_V;
	return 1;
}


#define SELECT_RANGE 10
//////////////////////////////////////////////////////////////////////////
// nCursorX nCursorY 是鼠标的相对于小地图窗口左上角的坐标
void KScenePlaceMapC::GetCursorInfo(int nCursorX, int nCursorY, char* pInfo, int nInfoSize)
{
// 	if (!pInfo)
// 		return;
// 
// 	pInfo[0] = 0;
// 	int nNpcIdx = 0;
// 
// 	while (nNpcIdx = NpcSet.GetNextIdx(nNpcIdx))
// 	{
// 		if (Npc[nNpcIdx].m_RegionIndex == -1)
// 			continue;
// 		
// 		int nNpcX = LOWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H +
// 						Npc[nNpcIdx].GetMapX() * 2;
// 		int nNpcY = HIWORD(Npc[nNpcIdx].m_dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V +
// 						Npc[nNpcIdx].GetMapY();
// 		if (nNpcX < m_MapCoverArea.left || nNpcX >= m_MapCoverArea.right ||
// 			nNpcY < m_MapCoverArea.top  || nNpcY >= m_MapCoverArea.bottom)
// 		{
// 			continue;
// 		}
// 		
// 		if (Npc[nNpcIdx].m_Kind == kind_building)
// 		{
// 			g_DirtyNpcSet.PushTempItem(nNpcIdx);
// 		}
// 	}
// 
// 	g_DirtyNpcSet.Front();
// 	DirtyNpcItem* pItem = g_DirtyNpcSet.GetNextItem();
// 	while (pItem)
// 	{
// 		int nNpcX = LOWORD(pItem->dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_H +
// 						pItem->nMapX * 2;
// 		int nNpcY = HIWORD(pItem->dwRegionID) * MAP_A_REGION_NUM_MAP_PIXEL_V +
// 						pItem->nMapY;
// 		if (nNpcX < m_MapCoverArea.left || nNpcX >= m_MapCoverArea.right ||
// 			nNpcY < m_MapCoverArea.top  || nNpcY >= m_MapCoverArea.bottom)
// 		{
// 			pItem = g_DirtyNpcSet.GetNextItem();
// 			continue;
// 		}
// 
// 		int nX = nNpcX - m_MapCoverArea.left - 1 + 4;
// 		int nY = nNpcY - m_MapCoverArea.top  - 1 + 8;
// 		if ((abs(nCursorX - nX) < SELECT_RANGE)&&(abs(nCursorY - nY) < SELECT_RANGE))
// 		{
// 			strncpy(pInfo, pItem->szInfo, (nInfoSize < _NAME_LEN ? nInfoSize : _NAME_LEN));
// 			break;
// 		}
// 
// 		pItem = g_DirtyNpcSet.GetNextItem();
// 	}
// 
// 	g_DirtyNpcSet.ClearTemp();
}

void KScenePlaceMapC::PaintMiniMapAutorunDest(int nX, int nY)
{
	int cellX = -1;
	int cellY = -1;
	AutoRobotMgr::Singleton().GetDestCellPos(cellX, cellY);
	if (cellX == -1 || cellY == -1)
		return;

	KSubWorld& subWorld = SubWorld[0];

	//计算目标点在小地图上的位置
	int destPosX = cellX * MAP_A_REGION_NUM_MAP_PIXEL_H / REGION_CELL_WIDTH;
	int destPosY = cellY * MAP_A_REGION_NUM_MAP_PIXEL_V / REGION_CELL_HEIGHT;
	//判断是否超出绘制边界
	if (destPosX < m_MapCoverArea.left || destPosX >= m_MapCoverArea.right ||
		destPosY < m_MapCoverArea.top  || destPosY >= m_MapCoverArea.bottom)
		return;

	KRUImage autorunTargetImg;
	strcpy(autorunTargetImg.szImage, "\\spr\\autorundest.spr");
	autorunTargetImg.Color.Color_dw = 0xffffffff;
	autorunTargetImg.nType = ISI_T_SPR;
	autorunTargetImg.nFrame = (subWorld.m_dwCurrentTime % GAME_FPS) * 8 / GAME_FPS;
	autorunTargetImg.uImage = 0;
	autorunTargetImg.bRenderFlag = 1;
	autorunTargetImg.bRenderStyle = 0;
	autorunTargetImg.oPosition.nX = nX + destPosX - m_MapCoverArea.left + 2;
	autorunTargetImg.oPosition.nY = nY + destPosY - m_MapCoverArea.top + 2;

	if (g_pRepresent)
		g_pRepresent->DrawPrimitives(1, &autorunTargetImg, RU_T_IMAGE, true);
}

void KScenePlaceMapC::PaintSceneMapAutorunDest(int nX, int nY)
{
	int cellX = -1;
	int cellY = -1;
	AutoRobotMgr::Singleton().GetDestCellPos(cellX, cellY);
	if (cellX == -1 || cellY == -1)
		return;

	KSubWorld& subWorld = SubWorld[0];	
	float scaleX = (float)(cellX - subWorld.m_nRegionBeginX * REGION_CELL_WIDTH) / (float)(subWorld.m_nWorldRegionWidth * REGION_CELL_WIDTH);
	float scaleY = (float)(cellY - subWorld.m_nRegionBeginY * REGION_CELL_HEIGHT) / (float)(subWorld.m_nWorldRegionHeight * REGION_CELL_HEIGHT);
	int posX = m_nSceneMapArea.x + scaleX * m_nSceneMapArea.width;
	int posY = m_nSceneMapArea.y + scaleY * m_nSceneMapArea.height;

	KRUImage autorunTargetImg;
	strcpy(autorunTargetImg.szImage, "\\spr\\autorundest.spr");
	autorunTargetImg.Color.Color_dw = 0xffffffff;
	autorunTargetImg.nType = ISI_T_SPR;
	autorunTargetImg.nFrame = (subWorld.m_dwCurrentTime % GAME_FPS) * 8 / GAME_FPS;
	autorunTargetImg.uImage = 0;
	autorunTargetImg.bRenderFlag = 1;
	autorunTargetImg.bRenderStyle = 0;
	autorunTargetImg.oPosition.nX = nX + posX + 2;
	autorunTargetImg.oPosition.nY = nY + posY + 2;

	if (g_pRepresent)
		g_pRepresent->DrawPrimitives(1, &autorunTargetImg, RU_T_IMAGE, true);
}

void KScenePlaceMapC::PaintSceneMapSyncToWorldNpc(int nX, int nY)
{
	const int syncToWorldNpcCount = NpcSet.GetSyncToWorldNpcCount();
	if (syncToWorldNpcCount == 0)
		return;

	int nWorldH = SubWorld[0].m_nWorldRegionHeight * REGION_PIXEL_HEIGHT;
	int nWorldW = SubWorld[0].m_nWorldRegionWidth * REGION_PIXEL_WIDTH;

	KRUImage memberSpot;			
	memberSpot.Color.Color_dw = 0xffffffff;
	memberSpot.nType = ISI_T_SPR;
	memberSpot.nFrame = 0;			
	memberSpot.bRenderFlag = 1;
	memberSpot.bRenderStyle = 0;

	for (int listIndex = 0; listIndex < syncToWorldNpcCount; listIndex++)
	{
		SyncToWorldNpcInfo* pInfo = NpcSet.GetSyncToWorldNpcInfo(listIndex);
		if (pInfo && !IsHideSyncToWorldNpc(pInfo->NpcId))
		{
			const char* pImage = ConfigManager::Singleton().GetConfigurableDisplayStyle(style_sync_to_world_npc_image, pInfo->Mode);
			if (!pImage)
				continue;
			
			strcpy(memberSpot.szImage, pImage);
			memberSpot.uImage = 0;
			
			int posX = pInfo->PosX;
			int posY = pInfo->PosY;
			float xposPercent = ((float)(posX - (SubWorld[0].m_nRegionBeginX) * REGION_PIXEL_WIDTH)) / nWorldW;
			float yposPercent = ((float)(posY - (SubWorld[0].m_nRegionBeginY) * REGION_PIXEL_HEIGHT)) / nWorldH;
			posX = m_nSceneMapArea.x + m_nSceneMapArea.width * xposPercent;
			posY = m_nSceneMapArea.y + m_nSceneMapArea.height * yposPercent;
			
			memberSpot.oPosition.nX = nX + posX - 1;
			memberSpot.oPosition.nY = nY + posY - 1;
			memberSpot.oEndPos.nX = memberSpot.oPosition.nX + 3;
			memberSpot.oEndPos.nY = memberSpot.oPosition.nY + 3;
			g_pRepresent->DrawPrimitives(1, &memberSpot, RU_T_IMAGE, true);
		}
	}	
}

bool KScenePlaceMapC::IsHideSyncToWorldNpc(DWORD npcId) const
{
	KPlayer& player = GetClientPlayer();
	DWORD selfNpcId = Npc[player.GetNpcIndex()].GetId();

	//隐藏自己
	if (npcId == selfNpcId)
		return true;

	//隐藏队友
	if (player.GetTeamInfo().IsInTeam())
	{
		KTeam* pTeam = player.GetTeamInfo().GetTeam();
		if (pTeam != NULL)
		{
			int maxMemberCount = pTeam->GetMaxMemberCount();
			for (int memberIndex = 0; memberIndex < maxMemberCount; memberIndex++)
			{
				ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(memberIndex);
				if (pMemberInfo != NULL)
				{
					if (pMemberInfo->NpcId == npcId)
						return true;
				}
			}
		}
	}

// 	//隐藏已经同步完整数据的NPC
// 	int npcIndex = NpcSet.SearchID(npcId);
// 	if (npcIndex > 0)
// 	{
// 		int kind = Npc[npcIndex].m_Kind;
// 		if (kind_dialoger == kind || kind_guard == kind)
// 			return true;
// 	}

	return false;
}

bool KScenePlaceMapC::IsMapInfoNpc(const char* szNpcName)
{
	vector<MapNpcMgr::NpcInfo>& npcList = MapNpcMgr::getSingleton().loadMap(_mapName);
	const int listSize = npcList.size();
	for(int i = 0; i < listSize; ++i)
	{
		if (g_StrCmp(npcList[i].npcName.c_str(), szNpcName))
			return true;
	}

	return false;
}

KScenePlaceMapC::Rect KScenePlaceMapC::GetGridArea() const
{
	return m_curSceneMapGridArea;
}