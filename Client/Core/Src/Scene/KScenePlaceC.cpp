// 场景地图（客户端版）
// Copyright : Kingsoft 2002
// Author    : Wooy (wu yue)
// CreateTime: 2002-11-11
// ---------------------------------------------------------------------------------------
// ***************************************************************************************
#include "KCore.h"
#include "KEngine.h"
#include "KWin32Wnd.h"
#include "KScenePlaceC.h"
#include <crtdbg.h>
#include "KIpotLeaf.h"
#include "SceneMath.h"
#include "../CoreShell.h"
#include "../ImgRef.h"
#include "iRepresentShell.h"
#include "Text.h"
#include "ObstacleDef.h"
#include <math.h>
#include "../ImgRef.h"
#include "KWeather.h"
#include "KOption.h"
#include "KPlayer.h"
#include "CoreDrawGameObj.h"
#include "KSubWorld.h"
#include "MapNpcMgr.h"
//--> Rocker 2004/08/19 切换地表到采用bitmap16_alpha模式
bool gbSwitchPaintAlphaType = false;

BOOL	g_bFrameCutAlert = TRUE;

static int LastFrameTime = 0;
static BOOL ShowCover = TRUE;

#ifdef SWORDONLINE_SHOW_DBUG_INFO
#ifdef _SHOW_OBJTARGET
	bool		g_bShowGameInfo = true;		//是否显示游戏（场景）信息
	int			g_nMapIndex = 0;			//场景索引数值
	int			g_bShowObstacle = true;
#else
	bool		g_bShowGameInfo = false;	//是否显示游戏（场景）信息
	int			g_nMapIndex = 0;			//场景索引数值
	int			g_bShowObstacle = false;
#endif
#endif

KScenePlaceC	g_ScenePlace;
//====是否预绘制地表层====
static bool		l_bPrerenderGround = true;

//摆放场景地图文件的目录
#define	ALL_PALCE_ROOT_FOLDER	"\\Maps"
//判断区域索引坐标是否落在以焦点区域为中心的一个范围内
#define INSIDE_AREA(h, v, range)	\
	( ((h) - m_FocusRegion.x) * ((h) - m_FocusRegion.x) <= (range * range) &&   \
		((v) - m_FocusRegion.y) * ((v) - m_FocusRegion.y) <= (range * range) )

#define GET_IN_PROCESS_AREA_REGION(h, v)		\
	( m_pInProcessAreaRegions[((v) - m_FocusRegion.y + 1) * SPWP_PROCESS_RANGE + (h) - m_FocusRegion.x + 1])

//***********************************************************************************************
// EnvironmentLight类的实现
//DWORD ChaZhiColor(KLColor &cLight1, KLColor &cLight2, float f2)
//{
//	if(f2 < 0.0f || f2 > 1.0f)
//		return 0xff000000;
//
//	float f1 = 1.0f - f2;
//	unsigned int r, g, b;
//	r = (unsigned int)(cLight2.r * f2 + cLight1.r * f1);
//	g = (unsigned int)(cLight2.g * f2 + cLight1.g * f1);
//	b = (unsigned int)(cLight2.b * f2 + cLight1.b * f1);
//	return 0xff000000 | (r<<16) | (g<<8) | b;
//}

//EnvironmentLight::EnvironmentLight()
//{
//	for(int i=0; i<7; i++)
//		m_cLight[i].r = m_cLight[i].g = m_cLight[i].b = 0x00000040;
//
//	m_cLight[0].r = 0x18, m_cLight[0].g = 0x18, m_cLight[0].b = 0x18;
//	m_cLight[1].r = 0x18, m_cLight[1].g = 0x18, m_cLight[1].b = 0x28;
//	m_cLight[2].r = 0x38, m_cLight[2].g = 0x38, m_cLight[2].b = 0x3b;
//	m_cLight[3].r = 0x40, m_cLight[3].g = 0x40, m_cLight[3].b = 0x40;
//	m_cLight[4].r = 0x58, m_cLight[4].g = 0x58, m_cLight[4].b = 0x58;
//	m_cLight[5].r = 0x38, m_cLight[5].g = 0x30, m_cLight[5].b = 0x28;
//	m_cLight[6].r = 0x30, m_cLight[6].g = 0x2a, m_cLight[6].b = 0x28;
//}

// 设置第nIdx个颜色
//void EnvironmentLight::SetLight(const KLColor &cLight, int nIdx)
//{
//	m_cLight[nIdx] = cLight;
//}

// 设置第nIdx个颜色
//void EnvironmentLight::SetLight(BYTE r, BYTE g, BYTE b, int nIdx)
//{
//	m_cLight[nIdx].r = r;
//	m_cLight[nIdx].g = g;
//	m_cLight[nIdx].b = b;
//}

// 设置所有7个颜色
//void EnvironmentLight::SetLight(KLColor *pLight)
//{
//	for(int i=0; i<7; i++)
//		m_cLight[i] = pLight[i];
//}

// 取得距一天开始nMinutes分钟时的环境光颜色
//DWORD EnvironmentLight::GetEnvironmentLight(int nMinutes)
//{
//	if(nMinutes < 660)
//	{
//		if(nMinutes < 300)
//		{
//			if(nMinutes >= 0 && nMinutes < 60)
//				return m_cLight[0].GetColor();
//			else if(nMinutes >= 60 && nMinutes < 180)
//				return ChaZhiColor(m_cLight[0], m_cLight[1], (nMinutes - 60) / 120.0f);
//			else if(nMinutes >= 180 && nMinutes < 300)
//				return m_cLight[1].GetColor();
//		}
//		else
//		{
//			if(nMinutes >= 300 && nMinutes < 420)
//				return ChaZhiColor(m_cLight[1], m_cLight[2], (nMinutes - 300) / 120.0f);
//			else if(nMinutes >= 420 && nMinutes < 540)
//				return ChaZhiColor(m_cLight[2], m_cLight[3], (nMinutes - 420) / 120.0f);
//			else if(nMinutes >= 540 && nMinutes < 660)
//				return ChaZhiColor(m_cLight[3], m_cLight[4], (nMinutes - 540) / 120.0f);
//		}
//	}
//	else
//	{
//		if(nMinutes < 1020)
//		{
//			if(nMinutes >= 660 && nMinutes < 780)
//				return m_cLight[4].GetColor();
//			else if(nMinutes >= 780 && nMinutes < 900)
//				return ChaZhiColor(m_cLight[4], m_cLight[3], (nMinutes - 780) / 120.0f);
//			else if(nMinutes >= 900 && nMinutes < 1020)
//				return ChaZhiColor(m_cLight[3], m_cLight[5], (nMinutes - 900) / 120.0f);
//		}
//		else
//		{
//			if(nMinutes >= 1020 && nMinutes < 1140)
//				return ChaZhiColor(m_cLight[5], m_cLight[6], (nMinutes - 1020) / 120.0f);
//			else if(nMinutes >= 1140 && nMinutes < 1260)
//				return ChaZhiColor(m_cLight[6], m_cLight[0], (nMinutes - 1140) / 120.0f);
//			else if(nMinutes >= 1260 && nMinutes < 1440)
//				return m_cLight[0].GetColor();
//		}
//	}
//	return 0xff000000;
//}

//*************************************************************************************************

//##ModelId=3DDB39150334
POINT KScenePlaceC::m_RangePosTable[SPWP_MAX_NUM_REGIONS] =
{
	{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0},
	{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}, {6, 1},
	{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}, {6, 2},
	{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}, {6, 3},
	{0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}, {6, 4},
	{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}, {6, 5},
	{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}, {6, 6},
};

//##ModelId=3DDB39BA029B
int	KScenePlaceC::m_PRIIdxTable[SPWP_MAX_NUM_REGIONS] =
{
	24, 31, 30, 23, 16, 17, 18, 
	25, 32, 39, 38, 37, 36, 29,
	22, 15, 8,  9,  10, 11, 12,
	19, 26, 33, 40, 47, 46, 45,
	44, 43, 42, 35, 28, 21, 14,
	7,  0,  1,  2,  3,  4,  5,
	6,  13, 20, 27, 34, 41, 48,
};

const KPrevLoadPosItem KScenePlaceC::m_PrevLoadPosOffset[3][3] = 
{
    {
        { 5, {{-2,  0}, {-2, -1}, {-2, -2}, {-1, -2}, { 0, -2}} },  // 左上角(-1, -1)的对应需要预加载的Region的数目和相对坐标
        { 3, {{-1, -2}, { 0, -2}, { 1, -2}, { 0,  0}, { 0,  0}} },  // 正上方( 0, -1)的对应需要预加载的Region的数目和相对坐标
        { 5, {{ 0, -2}, { 1, -2}, { 2, -2}, { 2, -1}, { 2,  0}} },  // 右上角( 1, -1)的对应需要预加载的Region的数目和相对坐标
    },
    {
        { 3, {{-2, -1}, {-2,  0}, {-2,  1}, { 0,  0}, { 0,  0}} },  // 正左方(-1,  0)的对应需要预加载的Region的数目和相对坐标
        { 0, {{ 0,  0}, { 0,  0}, { 0,  0}, { 0,  0}, { 0,  0}} },  // 原  点( 0,  0)的对应需要预加载的Region的数目和相对坐标
        { 3, {{ 2, -1}, { 2,  0}, { 2,  1}, { 0,  0}, { 0,  0}} },  // 正右方( 1,  0)的对应需要预加载的Region的数目和相对坐标
    },
    {
        { 5, {{-2,  0}, {-2,  1}, {-2,  2}, {-1,  2}, { 0,  2}} },  // 左下角(-1,  1)的对应需要预加载的Region的数目和相对坐标
        { 3, {{-1,  2}, { 0,  2}, { 1,  2}, { 0,  0}, { 0,  0}} },  // 正下方( 0,  1)的对应需要预加载的Region的数目和相对坐标
        { 5, {{ 0,  2}, { 1,  2}, { 2,  2}, { 2,  1}, { 2,  0}} },  // 右下角( 1,  1)的对应需要预加载的Region的数目和相对坐标
    },
};


//##ModelId=3DBE3B53008C
KScenePlaceC::KScenePlaceC()
{
	m_bShowSceneMap	= false;
	m_nFocusOffSetX = 0;
	m_nFocusOffSetY = 0;
	m_hLoadAndPreprocessThread = NULL;
	m_bInited = false;
	m_bLoading = false;
	m_bEnableWeather = true;
	m_bFollowWithMap = false;
	m_bCurrentMap = false;
	m_FocusPosition.x = m_FocusPosition.y = 0;//SPWP_FARAWAY_COORD;
	m_FocusRegion.x = m_FocusRegion.y = 0;
	m_FocusMoveOffset.cx = m_FocusMoveOffset.cy = 0;
	m_bPreprocessEvent = false;
	m_hLoadRegionEvent = NULL;
	m_hSwitchLoadFinishedEvent = NULL;
	m_nFirstToLoadIndex = -1;
	m_szPlaceRootPath[0] = 0;
	m_szSceneName[0] = 0;
	m_nSceneId = m_nCurrentSceneID = SPWP_NO_SCENE;
	m_RepresentArea.left = m_RepresentArea.right = m_RepresentArea.top = m_RepresentArea.bottom = 0;
	m_RepresentExactHalfSize.cx = m_RepresentExactHalfSize.cy = 0;
	m_MapFocusOffset.x = m_MapFocusOffset.y = 0;

	for (int i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_pRegions[i] = &m_RegionObjs[i];
	memset(&m_pInProcessAreaRegions, 0, sizeof(m_pInProcessAreaRegions));
	memset(&m_RegionGroundImages, 0, sizeof(m_RegionGroundImages));
	memset(&m_DistanceViewerImage, 0, sizeof(m_DistanceViewerImage));
	m_nNumGroundImagesAvailable = 0;

	m_bRenderGround = false;
	m_pfunRegionLoadedCallback = NULL;
	m_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;
	
	m_pObjsAbove = NULL;
	m_nNumObjsAbove = 0;
	
	m_pWeather = NULL;
}

//##ModelId=3DD17A770383
KScenePlaceC::~KScenePlaceC()
{
	Terminate();
}

//##ModelId=3DCAAE3703A6
void KScenePlaceC::ClosePlace()
{
	if (m_szPlaceRootPath[0] == 0)
		return;

	m_nSceneId = m_nCurrentSceneID = SPWP_NO_SCENE;
	m_Map.Free();

	m_DistanceViewer.Release();
	m_CoverViewer.Clear();
	m_DistanceCover.Clear();

	SetLoadingStatus(false);

	ResetEvent(m_hSwitchLoadFinishedEvent);
	ResetEvent(m_hLoadRegionEvent);	//这要放在修改m_FocusRegion之前，要不子线程就可能在两句代码执行之间退出结束了。

	EnterCriticalSection(&m_RegionListAdjustCritical);
	EnterCriticalSection(&m_LoadCritical);
	m_nFirstToLoadIndex = -1;
	for (int i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
		m_pInProcessAreaRegions[i] = NULL;
	LeaveCriticalSection(&m_LoadCritical);
	LeaveCriticalSection(&m_RegionListAdjustCritical);

	EnterCriticalSection(&m_ProcessCritical);
	ClearPreprocess(true);
	for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_RegionObjs[i].Clear();
	LeaveCriticalSection(&m_ProcessCritical);

	m_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;
	m_szPlaceRootPath[0] = 0;
	m_szSceneName[0] = 0;
	m_bPreprocessEvent = false;
	m_bRenderGround = false;

	if(m_pWeather)
	{
		delete m_pWeather;
		m_pWeather = NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// Added By Rocker 2004.3.15
	//FreeStaticImages();
}

//##ModelId=3DCAA6A703DB
bool KScenePlaceC::Initialize()
{
	if (m_bInited)
		return true;

	m_hSwitchLoadFinishedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hLoadRegionEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (m_hSwitchLoadFinishedEvent && m_hLoadRegionEvent)
	{
		InitializeCriticalSection(&m_RegionListAdjustCritical);
		InitializeCriticalSection(&m_LoadCritical);
		InitializeCriticalSection(&m_ProcessCritical);

		DWORD	ThreadId;
		m_hLoadAndPreprocessThread = CreateThread(NULL, 0,
			LoadThreadEntrance, this, 0, &ThreadId);

		if (m_hLoadAndPreprocessThread)
		{
			m_bInited = true;
			return true;
		}

		DeleteCriticalSection(&m_ProcessCritical);
		DeleteCriticalSection(&m_LoadCritical);
		DeleteCriticalSection(&m_RegionListAdjustCritical);
	}

	CloseHandle(m_hLoadRegionEvent);
	m_hLoadRegionEvent = NULL;
	CloseHandle(m_hSwitchLoadFinishedEvent);
	m_hSwitchLoadFinishedEvent = NULL;
	return false;
}

bool KScenePlaceC::OpenPlace(int nPlaceIndex, int nCityType)
{
	if (m_bInited == false)
		return false;

	ShowCover = TRUE;

#ifndef _SERVER
	
	int i = 0;
	g_pRepresentShell->FreeAllImage();
	m_nNumGroundImagesAvailable = 0;
	for ( i = 0; i < m_nNumGroundImagesAvailable; i++)
	{
		m_RegionGroundImages[i].szImage[0] = 0;
		m_RegionGroundImages[i].uImage = 0;
	}//*/
	m_Map.Terminate();
#endif

	ClosePlace();

	KIniFile	Ini;
	char Buff[256];

	if(!GetPlacePathFromIndex(nPlaceIndex, nCityType, m_szSceneName, sizeof(m_szSceneName), m_szPlaceRootPath, sizeof(m_szPlaceRootPath)))
	{
		return false;
	}

	if(!LoadPlaceMiniMap())
	{
		return false;
	}

	m_nSceneId = m_nCurrentSceneID = nPlaceIndex;

	sprintf(Buff, "%s.wor", m_szPlaceRootPath);
	Ini.Load(Buff);

	// 取得场景的环境光信息
	int nIsInDoor;
	Ini.GetInteger("MAIN", "IsInDoor", 0, &nIsInDoor);
	m_ObjectsTree.SetIsIndoor(nIsInDoor != 0);
	
	Ini.Clear();

#ifdef SWORDONLINE_SHOW_DBUG_INFO
	g_nMapIndex = nPlaceIndex;		//场景索引数值
#endif

	m_FocusRegion.x = m_FocusRegion.y = -SPWP_LOAD_EXTEND_RANGE;
	m_FocusMoveOffset.cx = m_FocusMoveOffset.cy = 0;
	int	nImageIndex = 0;
	for ( i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		m_RegionObjs[i].ToLoad(-m_RangePosTable[i].x, -m_RangePosTable[i].y);

	if (g_pRepresent && m_nNumGroundImagesAvailable == 0 && l_bPrerenderGround)
	{
		for (int i = 0; m_nNumGroundImagesAvailable < SPWP_NUM_REGIONS_IN_PROCESS_AREA && i < 2147483647; i++)
		{	// 注By Rocker：不断创建2147483647次或直到创建成功9个地表渲染纹理为止
			KRUImage* pImage = &m_RegionGroundImages[m_nNumGroundImagesAvailable];
			pImage->bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
			pImage->nType = ISI_T_BITMAP16;
			sprintf(pImage->szImage, "_*PlaceGround*_#~%d~#_", i);
			pImage->uImage = g_pRepresent->CreateImage(
				pImage->szImage, KScenePlaceRegionC::RWPP_AREGION_WIDTH,
				KScenePlaceRegionC::RWPP_AREGION_HEIGHT / 2, ISI_T_BITMAP16);

			if (pImage->uImage)
				m_nNumGroundImagesAvailable ++;
		}
	}

	LoadPlaceMultiScrollInfo();
	
	SetLoadingStatus(true);

//	PreLoadStaticImages();

	
	return true;
}


//##根据给出的场景索引获取场景文件名字和场景的名字
//--> Rocker 2004/10/08
//int KScenePlaceC::GetPlacePathFromIndex(int nIndex/*[in]*/, char *pSceneName/*[out]*/, int nSceneNameLength/*[in]*/, char* pszPath/*[out]*/, int nPathLength/*[in]*/)
int KScenePlaceC::GetPlacePathFromIndex(int nIndex/*[in]*/, int nCityType, char *pSceneName/*[out]*/, int nSceneNameLength/*[in]*/, char* pszPath/*[out]*/, int nPathLength/*[in]*/)
//<-- End
{
	KIniFile	Ini;
	char		Index[16];
	char		Buff[128];

	if (Ini.Load("\\settings\\MapList.ini") == FALSE)
		return false;
	
	itoa(nIndex, Index, 10);
	if (Ini.GetString("List", Index, "", Buff, sizeof(Buff)) == FALSE)
		return false;
	
	sprintf(pszPath, "%s\\%s", ALL_PALCE_ROOT_FOLDER, Buff);
	
	strcat(Index, "_name");
	if (!Ini.GetString("List", Index, "", pSceneName, nSceneNameLength))
	{
		char* pName = strchr(Buff, '\\');
		if (pName)
		{
			while(strchr(pName, '\\'))
				pName = strchr(pName, '\\') + 1;
			strcpy(pSceneName, pName);
		}
		else
			strcpy(pSceneName, Buff);
	}
	
	Ini.Clear();
	int nValue = sprintf(pszPath, "%s\\%s.wor",ALL_PALCE_ROOT_FOLDER, Buff);
	
	if (Ini.Load(pszPath) == FALSE)
	{
		pszPath[0] = 0;
		return false;
	}
	pszPath[nValue - 4] = 0;

	return true;
}

//##根据给出的场景索引获取场景多重卷轴配置文件的名字
// --> Rocker Edit Start 2005/11/29
// <-- Rocker End
int KScenePlaceC::GetPlaceMultiScrollIniPath(int nIndex, char *pszIniPath)
{
	// --> Rocker Edit Start 2005/11/29
	if (g_GetScreenWidth() == 1024)
	{
		return sprintf(pszIniPath, "%s\\ScrollSetting\\%d_Scroll_1024.ini", ALL_PALCE_ROOT_FOLDER, nIndex);
	}
	// <-- Rocker End
	
	return sprintf(pszIniPath, "%s\\ScrollSetting\\%d_Scroll.ini", ALL_PALCE_ROOT_FOLDER, nIndex);
}


//##获取场景的多重卷轴的信息
int KScenePlaceC::LoadPlaceMultiScrollInfo()
{
	char szIniPath[128];
	KIniFile Ini;

	m_DistanceCoverList.Clear();
	m_DistanceViewer.Release();
	m_DistanceCover.Clear();
	m_DistanceList.Clear();
	m_CoverViewer.Clear();
	m_CoverList.Clear();

	GetPlaceMultiScrollIniPath(m_nCurrentSceneID, szIniPath);
	if(Ini.Load(szIniPath))
	{
		int nValue;
		tagSCROLL_INFO *pElement;
		char szSection[32];


		for(int i = 0;;i++)
		{
			pElement = NULL;
			itoa(i, szSection, 10);
			Ini.GetInteger(szSection, "Type", 0, &nValue);
			if(nValue == 1 || nValue == 2 || nValue == 3)
			{
				pElement = new tagSCROLL_INFO;
				if(pElement)
				{
					pElement->nID = i;
					pElement->nIsInThisArea = 0;
					pElement->nIsLoaded = 0;
					Ini.GetInteger(szSection, "AreaLeft", 0, (int *)(&pElement->rectArea.left));
					Ini.GetInteger(szSection, "AreaTop", 0, (int *)(&pElement->rectArea.top));
					Ini.GetInteger(szSection, "AreaRight", 0, (int *)(&pElement->rectArea.right));
					Ini.GetInteger(szSection, "AreaBottom", 0, (int *)(&pElement->rectArea.bottom));
					pElement->rectArea.left *= KScenePlaceRegionC::RWPP_AREGION_WIDTH;
					pElement->rectArea.right *= KScenePlaceRegionC::RWPP_AREGION_WIDTH;
					pElement->rectArea.top *= KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
					pElement->rectArea.bottom *= KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

					Ini.GetInteger(szSection, "PaintRectLeft", 0, (int *)(&pElement->rectPaintRect.left));
					Ini.GetInteger(szSection, "PaintRectTop", 0, (int *)(&pElement->rectPaintRect.top));
					Ini.GetInteger(szSection, "PaintRectRight", 800, (int *)(&pElement->rectPaintRect.right));
					Ini.GetInteger(szSection, "PaintRectBottom", 600, (int *)(&pElement->rectPaintRect.bottom));

					if(nValue == 1) //远景
					{
						Ini.GetInteger(szSection, "SceneCenterPointX", 0, (int *)(&pElement->rectViewArea.left));
						Ini.GetInteger(szSection, "SceneCenterPointY", 0, (int *)(&pElement->rectViewArea.top));
						Ini.GetInteger(szSection, "PicCenterPointX", 0, (int *)(&pElement->rectViewArea.right));
						Ini.GetInteger(szSection, "PicCenterPointY", 0, (int *)(&pElement->rectViewArea.bottom));

						Ini.GetInteger(szSection, "Rating", 2, &pElement->nRate);

						m_DistanceList.Add(pElement);
					}
					else //覆盖
					{
						Ini.GetInteger(szSection, "ViewAreaLeft", 0, (int *)(&pElement->rectViewArea.left));
						Ini.GetInteger(szSection, "ViewAreaTop", 0, (int *)(&pElement->rectViewArea.top));
						Ini.GetInteger(szSection, "ViewAreaRight", 0, (int *)(&pElement->rectViewArea.right));
						Ini.GetInteger(szSection, "ViewAreaBottom", 0, (int *)(&pElement->rectViewArea.bottom));
						pElement->rectViewArea.left *= KScenePlaceRegionC::RWPP_AREGION_WIDTH;
						pElement->rectViewArea.right *= KScenePlaceRegionC::RWPP_AREGION_WIDTH;
						pElement->rectViewArea.top *= KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
						pElement->rectViewArea.bottom *= KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

						Ini.GetInteger(szSection, "Speed", 2, &pElement->nRate);
						Ini.GetInteger(szSection, "Count", 1, &pElement->nCount);
						Ini.GetInteger(szSection, "Angle", 0, &pElement->nAngle);

						if(nValue == 2)
						{
							m_CoverList.Add(pElement);
						}
						else
						{
							m_DistanceCoverList.Add(pElement);
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}
	return 1;
}


//##判断不同画面层是否要显示
/**
 * @brief 判断不同画面层是否要显示
 * @param nX nY 场景焦点坐标
 */

//<-- End
void KScenePlaceC::HandleMultiScroll(int nX, int nY)
{
	tagSCROLL_INFO *pNode;
	//先判断背景层
	pNode = m_DistanceList.Begin();
	bool bCurrentType = false;
	while(pNode)
	{
		if (pNode->rectArea.left < nX && pNode->rectArea.right > nX &&
			pNode->rectArea.top < nY  && pNode->rectArea.bottom > nY)
		{

			//--> Rocker 2004/08/18
			bCurrentType = true;
			//<-- End
			if(!pNode->nIsLoaded)
			{
				pNode->nIsInThisArea = TRUE;
				if(pNode->nIsLoaded = LoadAScroll(pNode->nID))
				{
					m_DistanceViewer.SetScrollStep(pNode->nRate, pNode->nRate << 1);

					m_DistanceViewer.SetPaintRect(pNode->rectPaintRect.top, pNode->rectPaintRect.left,
												  pNode->rectPaintRect.bottom, pNode->rectPaintRect.right);

					m_DistanceViewer.SetCenterPoint(pNode->rectViewArea.left, pNode->rectViewArea.top,
													pNode->rectViewArea.right, pNode->rectViewArea.bottom);

					m_DistanceViewer.LetMePaint(TRUE);
				}
			}
		}
		else
		{
			pNode->nIsLoaded = 0;
			if(pNode->nIsInThisArea)
			{
				m_DistanceViewer.LetMePaint(FALSE);
				m_DistanceViewer.Release();
			}
			pNode->nIsInThisArea = 0;
		}
		pNode = m_DistanceList.Next();
	}
	//再判断远景覆盖层
	pNode = m_DistanceCoverList.Begin();
	while(pNode)
	{
		if (pNode->rectArea.left < nX && pNode->rectArea.right > nX &&
			pNode->rectArea.top < nY  && pNode->rectArea.bottom > nY)
		{
			if(!pNode->nIsLoaded)
			{
				pNode->nIsInThisArea = TRUE;
				m_DistanceCover.Clear();
				if(pNode->nIsLoaded = LoadAScroll(pNode->nID))
				{
					m_DistanceCover.Mode(KCoverViewLayer::enumCM_1X_SPEED_TO_GROUND);
					m_DistanceCover.SetCoverRect(pNode->rectArea.top, pNode->rectArea.left, pNode->rectArea.bottom, pNode->rectArea.right);
					m_DistanceCover.GenerateItem(pNode->nAngle, (float)(pNode->nRate), pNode->nCount, m_FocusPosition.x, m_FocusPosition.y);
					m_DistanceCover.LetMePaint(TRUE);
				}
			}
			m_DistanceCover.HeartBeat(m_FocusPosition.x, m_FocusPosition.y);
		}
		else
		{
			pNode->nIsLoaded = 0;
			if(pNode->nIsInThisArea)
			{
				m_DistanceCover.LetMePaint(FALSE);
				m_DistanceCover.Clear();
			}
			pNode->nIsInThisArea = 0;
		}
		pNode = m_DistanceCoverList.Next();
	}
	//再判断覆盖层
	pNode = m_CoverList.Begin();
	while(pNode)
	{
		if (pNode->rectArea.left < nX && pNode->rectArea.right > nX &&
			pNode->rectArea.top < nY  && pNode->rectArea.bottom > nY)
		{
			if(!pNode->nIsLoaded)
			{
				pNode->nIsInThisArea = TRUE;
				m_CoverViewer.Clear();
				if(pNode->nIsLoaded = LoadAScroll(pNode->nID))
				{
					m_CoverViewer.Mode(KCoverViewLayer::enumCM_2X_SPEED_TO_GROUND);
					m_CoverViewer.SetCoverRect(pNode->rectArea.top, pNode->rectArea.left, pNode->rectArea.bottom, pNode->rectArea.right);
					m_CoverViewer.GenerateItem(pNode->nAngle, (float)(pNode->nRate), pNode->nCount, m_FocusPosition.x, m_FocusPosition.y);
					m_CoverViewer.LetMePaint(TRUE);
				}
			}
			m_CoverViewer.HeartBeat(m_FocusPosition.x, m_FocusPosition.y);
		}
		else
		{
			pNode->nIsLoaded = 0;
			if(pNode->nIsInThisArea)
			{
				m_CoverViewer.LetMePaint(FALSE);
			}
			pNode->nIsInThisArea = 0;
		}
		pNode = m_CoverList.Next();
	}

	//--> Rocker 2004/08/18 为了优化非卷轴区域的显示速度
	if (bCurrentType != gbSwitchPaintAlphaType)
	{
		for (int i=0; i<SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
		{
			// 重新创建地表数据
			if (bCurrentType)
			{
				m_RegionGroundImages[i].nType = ISI_T_BITMAP16_ALPHA;
				g_pRepresent->CreateImage(m_RegionGroundImages[i].szImage, 
					KScenePlaceRegionC::RWPP_AREGION_WIDTH, KScenePlaceRegionC::RWPP_AREGION_HEIGHT, 
					ISI_T_BITMAP16_ALPHA);
			}else
			{
				m_RegionGroundImages[i].nType = ISI_T_BITMAP16;
				g_pRepresent->CreateImage(m_RegionGroundImages[i].szImage, 
					KScenePlaceRegionC::RWPP_AREGION_WIDTH, KScenePlaceRegionC::RWPP_AREGION_HEIGHT, 
					ISI_T_BITMAP16);
			}
		}
		PrerenderGround(true);
		gbSwitchPaintAlphaType = bCurrentType;
	}
	//<-- End
}


//##载入一个多重卷轴资讯
int KScenePlaceC::LoadAScroll(int nID)
{
	char szIniPath[128];
	KIniFile Ini;
	int nRet = 0;
	
	m_CoverViewer.Clear();
	m_DistanceViewer.Release();
	m_DistanceCover.Clear();
	
	GetPlaceMultiScrollIniPath(m_nCurrentSceneID, szIniPath);
	if(Ini.Load(szIniPath))
	{
		int  nValue;
		char szValue[128];
		char szSection[32];
		itoa(nID, szSection, 10);
		Ini.GetInteger(szSection, "Type", 0, &nValue);
		switch(nValue)
		{
		case 1://1是远景层
			Ini.GetString(szSection, "Image", "", szValue, sizeof(szValue));
			m_DistanceViewer.Release();
			nRet = m_DistanceViewer.Load(szValue);
			strcpy(m_DistanceViewerImage,szValue);
			break;
		case 2://2是覆盖层
			{
				char szKeyBase[8];
				char szKey[32];
				strcpy(szKeyBase, "Image");
				m_CoverViewer.Clear();
				for(int i = 0;;i++)
				{
					sprintf(szKey, "%s%d", szKeyBase, i);
					Ini.GetString(szSection, szKey, "", szValue, sizeof(szValue));
					if(!szValue[0])
					{
						break;
					}
					nRet = m_CoverViewer.Load(szValue);
				}
			}
			break;
		case 3://3是远景覆盖层
			{
				char szKeyBase[8];
				char szKey[32];
				strcpy(szKeyBase, "Image");
				m_DistanceCover.Clear();
				for(int i = 0;;i++)
				{
					sprintf(szKey, "%s%d", szKeyBase, i);
					Ini.GetString(szSection, szKey, "", szValue, sizeof(szValue));
					if(!szValue[0])
					{
						break;
					}
					nRet = m_DistanceCover.Load(szValue);
				}
			}
			break;
		default:
			break;
		}
	}
	return nRet;
}

//##临时转换一张小地图
int KScenePlaceC::TempChangeMiniMap(int nIndex)
{
	KIniFile Ini;
	char szName[128], szPath[128];

	GetPlacePathFromIndex(nIndex, -1, szName, sizeof(szName), szPath, sizeof(szPath));
	sprintf(szName, "%s.wor", szPath);
	if(!Ini.Load(szName))
	{
		return false;
	}
	else
	{
		m_nbShowCharacter = FALSE;
		m_nCurrentSceneID = nIndex;
		m_Map.Load(&Ini, szPath, m_szSceneName);
		return true;
	}
}


//##读取这个场景所对应的小地图
int KScenePlaceC::LoadPlaceMiniMap()
{
	KIniFile Ini;
	char Buff[256];

	sprintf(Buff, "%s.wor", m_szPlaceRootPath);
	if(!Ini.Load(Buff))
	{
		return false;
	}
	else
	{
		m_nCurrentSceneID = m_nSceneId;
		m_Map.Load(&Ini, m_szPlaceRootPath, m_szSceneName);
		m_nbShowCharacter = TRUE;
		return true;
	}
}

//##读取任意一张地图
int KScenePlaceC::LoadSceneMap(char* mapName, BOOL bShowCharacter)
{
	KIniFile Ini;
	char Buff[256];

	sprintf(Buff, "\\Maps\\%s.wor", mapName);
	if(!Ini.Load(Buff))
	{
		return false;
	}
	else
	{
		m_nCurrentSceneID = m_nSceneId;
		sprintf(Buff, "\\Maps\\%s", mapName);
		m_Map.Load(&Ini, Buff, mapName);
		m_nbShowCharacter = bShowCharacter;
		return true;
	}
}

BOOL KScenePlaceC::isHaveSceneMap(char* mapName)
{
	return m_Map.isMapExist(mapName);
}

//##ModelId=3DBCE7B70358
void KScenePlaceC::SetFocusPosition(int nX, int nY, int nZ, bool bSyncWorld )
{
	if (m_bInited == false || m_szPlaceRootPath[0] == 0)
		return;

	// 焦点默认偏移
	nX += m_nFocusOffSetX;
	nY += m_nFocusOffSetY;	

	if(m_FocusPosition.x == nX && m_FocusPosition.y == nY)
		return;

	if (m_bFollowWithMap)
	{
		m_OrigFocusPosition.x = nX;
		m_OrigFocusPosition.y = nY;
		return;
	}

	if(m_pWeather)
		m_pWeather->SetFocusPos(nX, nY);

	//忽略z轴坐标
	m_FocusPosition.x = nX;
	m_FocusPosition.y = nY;

	POINT	pos;
	pos.x = m_FocusPosition.x / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	pos.y = m_FocusPosition.y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	if (g_pRepresent)
	{
		g_pRepresent->LookAt(nX, nY, 0);
	}

	m_RepresentArea.right  -= m_RepresentArea.left;
	m_RepresentArea.bottom -= m_RepresentArea.top;
	m_RepresentArea.left = m_FocusPosition.x - m_RepresentExactHalfSize.cx;
	m_RepresentArea.top  = m_FocusPosition.y - m_RepresentExactHalfSize.cy;
	m_RepresentArea.right  += m_RepresentArea.left;
	m_RepresentArea.bottom += m_RepresentArea.top;

	if (pos.x == m_FocusRegion.x && pos.y == m_FocusRegion.y)
	{
		m_Map.SetFocusPosition(m_FocusPosition.x + m_MapFocusOffset.x,
			m_FocusPosition.y + m_MapFocusOffset.y, false);
		return;
	}

    #ifdef _DEBUG
    char szDebugString[128];
    sprintf(
        szDebugString, 
        "Change From %d, %d To %d, %d Region!\n", 
        m_FocusRegion.x, 
        m_FocusRegion.y, 
        pos.x,
        pos.y
    );
    OutputDebugString(szDebugString);
    #endif

    POINT OffsetPos;
    // 如果根据移动后Region为相对位移进行的预读
    OffsetPos.x = pos.x - m_FocusRegion.x + 1;
    OffsetPos.y = pos.y - m_FocusRegion.y + 1; 
    if (
        (OffsetPos.x >= 0) &&
        (OffsetPos.x <  3) &&
        (OffsetPos.y >= 0) &&
        (OffsetPos.y <  3)
    )
    {
        const KPrevLoadPosItem *pcPosOffsetItem = NULL;
        pcPosOffsetItem = &m_PrevLoadPosOffset[OffsetPos.y][OffsetPos.x];

        m_PreLoadPosItem.m_nNum = 0;

        int i = 0;

        EnterCriticalSection(&m_RegionListAdjustCritical);
        for (i = 0; i < (pcPosOffsetItem->m_nNum); i++)
        {
            // 如果根据移动后Region为相对位移进行的预读
            m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].x = 
                pos.x + pcPosOffsetItem->m_Pos[i].x;   
            
            m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].y = 
                pos.y + pcPosOffsetItem->m_Pos[i].y;

            // 如果根据移动前Region为相对位移进行的预读
            //m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].x = 
            //    m_FocusRegion.x + pcPosOffsetItem->m_Pos[i].x;   
            //
            //m_PreLoadPosItem.m_Pos[m_PreLoadPosItem.m_nNum].y = 
            //    m_FocusRegion.y + pcPosOffsetItem->m_Pos[i].y;
                
            m_PreLoadPosItem.m_nNum++;   
        }
        LeaveCriticalSection(&m_RegionListAdjustCritical);
        
        #ifdef _DEBUG
        for (i = 0; i < m_PreLoadPosItem.m_nNum; i++)
        {
            char szDebugString[128];
            sprintf(
                szDebugString, 
                "Preload %d, %d Region!\n", 
                m_PreLoadPosItem.m_Pos[i].x, 
                m_PreLoadPosItem.m_Pos[i].y 
            );
            OutputDebugString(szDebugString);
        }
        #endif

    }
    else
    {
        // 如果是超过预加载的范围，清空，不需要加载
        m_PreLoadPosItem.m_nNum = 0;
    }

	m_Map.SetFocusPosition(m_FocusPosition.x + m_MapFocusOffset.x,
		m_FocusPosition.y + m_MapFocusOffset.y, true);

	m_FocusMoveOffset.cx += pos.x - m_FocusRegion.x;
	m_FocusMoveOffset.cy += pos.y - m_FocusRegion.y;
	m_FocusRegion.x   = pos.x;
	m_FocusRegion.y = pos.y;

	m_ObjectsTree.SetLightenAreaLeftTopPos(
		(m_FocusRegion.x  - 1) * KScenePlaceRegionC::RWPP_AREGION_WIDTH,
		(m_FocusRegion.y  - 1) * KScenePlaceRegionC::RWPP_AREGION_HEIGHT);

	ClearPreprocess(false);
	if (m_FocusMoveOffset.cx >= SPWP_TRIGGER_RANGE  ||
		m_FocusMoveOffset.cx <= -SPWP_TRIGGER_RANGE ||
		m_FocusMoveOffset.cy >= SPWP_TRIGGER_RANGE  ||
		m_FocusMoveOffset.cy <= -SPWP_TRIGGER_RANGE)
	{

		if (m_FocusMoveOffset.cx >= SPWP_TRIGGER_LOADING_RANGE ||
			m_FocusMoveOffset.cx <= -SPWP_TRIGGER_LOADING_RANGE ||
			m_FocusMoveOffset.cy >= SPWP_TRIGGER_LOADING_RANGE  ||
			m_FocusMoveOffset.cy <= -SPWP_TRIGGER_LOADING_RANGE)
		{
			SetLoadingStatus(true);
		}

		ChangeLoadArea( bSyncWorld );

		m_FocusMoveOffset.cx = 0;
		m_FocusMoveOffset.cy = 0;
	}
	ChangeProcessArea();
}

void KScenePlaceC::SetRepresentAreaSize(int nWidth, int nHeight)
{
	m_RepresentExactHalfSize.cx = nWidth / 2 + SPWP_REPRESENT_RECT_WINDAGE_X;
	m_RepresentExactHalfSize.cy = nHeight + SPWP_REPRESENT_RECT_WINDAGE_T * 2;
	m_RepresentArea.right = m_RepresentArea.left + nWidth +
		SPWP_REPRESENT_RECT_WINDAGE_X + SPWP_REPRESENT_RECT_WINDAGE_X;
	m_RepresentArea.bottom = m_RepresentArea.top + 
		(nHeight + SPWP_REPRESENT_RECT_WINDAGE_T + SPWP_REPRESENT_RECT_WINDAGE_B) * 2;
}

void KScenePlaceC::GetFocusPosition(int& nX, int& nY, int& nZ)
{
	nX = m_FocusPosition.x;
	nY = m_FocusPosition.y;
	nZ = 0;
}

//##ModelId=3DCD58AC00BC
void KScenePlaceC::Terminate()
{
	if (m_bInited == false)
		return ;
	ClosePlace();

	m_Map.Terminate();

	//触发子线程退出执行
	m_FocusRegion.x = SPWP_FARAWAY_COORD;
	SetEvent(m_hLoadRegionEvent);
	//等待子线程关闭
	DWORD	dwExitCode;
	if (GetExitCodeThread(m_hLoadAndPreprocessThread, &dwExitCode) && dwExitCode == STILL_ACTIVE)
		WaitForSingleObject(m_hLoadAndPreprocessThread, INFINITE);
	CloseHandle(m_hLoadAndPreprocessThread);
	m_hLoadAndPreprocessThread = NULL;
	
	DeleteCriticalSection(&m_ProcessCritical);
	DeleteCriticalSection(&m_LoadCritical);
	DeleteCriticalSection(&m_RegionListAdjustCritical);

	CloseHandle(m_hSwitchLoadFinishedEvent);
	m_hSwitchLoadFinishedEvent = NULL;
	CloseHandle(m_hLoadRegionEvent);
	m_hLoadRegionEvent = NULL;

	m_bPreprocessEvent = false;

	for (int i = 0; i < m_nNumGroundImagesAvailable; i++)
	{
		if (g_pRepresent)
		{
			g_pRepresent->FreeImage(m_RegionGroundImages[i].szImage);
		}
		m_RegionGroundImages[i].szImage[0] = 0;
		m_RegionGroundImages[i].uImage = 0;
	}
	m_nNumGroundImagesAvailable = 0;
	m_bInited = false;
}

// 预加载地图上的图素
void KScenePlaceC::PreLoadProcess()
{
    if (m_PreLoadPosItem.m_nNum == 0)
        return;

    int i = 0;
    int j = 0;
    m_nPrevLoadFileCount = 0;

    EnterCriticalSection(&m_RegionListAdjustCritical);
    for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
    {
        int nRegionX = 0;
        int nRegionY = 0;
        
        if (!m_pRegions[i])
            continue;

        m_pRegions[i]->GetRegionIndex(nRegionX, nRegionY);

        for (j = 0; j < m_PreLoadPosItem.m_nNum; j++)
        {
            if (
                (m_PreLoadPosItem.m_Pos[j].x == nRegionX) &&
                (m_PreLoadPosItem.m_Pos[j].y == nRegionY)
            )
            {
                break;
            }
        }

        if (j >= m_PreLoadPosItem.m_nNum)
            continue;   // 如果没有找到匹配的项，则跳到下一个

        
        KBuildinObj *pObjsList = NULL;
        unsigned uNumObjs      = 0;

        m_pRegions[i]->GetBIOSBuildinObjs(pObjsList, uNumObjs); 

        for (NULL; uNumObjs > 0; uNumObjs--, pObjsList++)
        {
            memcpy(
                m_PrevLoadFileNameAndFrames[m_nPrevLoadFileCount].szName, 
                pObjsList->szImage, 
                MAX_RESOURCE_FILE_NAME_LEN
            );   
            m_PrevLoadFileNameAndFrames[m_nPrevLoadFileCount].nFrame = pObjsList->nFrame;

            m_nPrevLoadFileCount++;
            if (m_nPrevLoadFileCount >= MAX_PREV_LOAD_FILE_COUNT)
                break;
        }
        if (m_nPrevLoadFileCount >= MAX_PREV_LOAD_FILE_COUNT)
            break;
    }
    m_PreLoadPosItem.m_nNum = 0;
    LeaveCriticalSection(&m_RegionListAdjustCritical);
}


//##ModelId=3DCB6BC90345
void KScenePlaceC::LoadProcess()
{
    DWORD dwRetCode = 0;

	while(true)
	{
        dwRetCode = WaitForSingleObject(m_hLoadRegionEvent, 1000);
        if (dwRetCode == WAIT_OBJECT_0)
        {
			if (m_FocusRegion.x == SPWP_FARAWAY_COORD)
			{
				//CFS_FILELOGS::WriteLog("SPWP_FARAWAY_COORD ok \n" );
				break;
			}
			KScenePlaceRegionC*	pRegion = NULL;
			EnterCriticalSection(&m_RegionListAdjustCritical);
			if (m_nFirstToLoadIndex >= 0)
				pRegion = m_pRegions[m_nFirstToLoadIndex];
			LeaveCriticalSection(&m_RegionListAdjustCritical);

			//CFS_FILELOGS::WriteLog(" KScenePlaceC::LoadProcess() m_FocusRegion.x = %d m_FocusRegion.y = %d \n", m_FocusRegion.x, m_FocusRegion.y );

			if (pRegion)
			{
				//CFS_FILELOGS::WriteLog(" KScenePlaceC::LoadProcess() load m_nFirstToLoadIndex = %d ok \n", m_nFirstToLoadIndex );
				//CFS_FILELOGS::WriteLog(" KScenePlaceC::LoadProcess() pRegion->m_RegionIndex.x = %d pRegion->m_RegionIndex.y = %d \n", pRegion->m_RegionIndex.x, pRegion->m_RegionIndex.y );
				EnterCriticalSection(&m_LoadCritical);
				pRegion->Load(m_szPlaceRootPath);
				LeaveCriticalSection(&m_LoadCritical);
				g_DebugLog("[Scene]Enter ARegionLoaded");
				ARegionLoaded(pRegion);
				g_DebugLog("[Scene]Leave ARegionLoaded");
			}
			else
			{
				//CFS_FILELOGS::WriteLog(" KScenePlaceC::LoadProcess() load m_nFirstToLoadIndex = %d fail \n", m_nFirstToLoadIndex );
			}
		}
        else if (dwRetCode == WAIT_TIMEOUT)
        {
			//CFS_FILELOGS::WriteLog(" KScenePlaceC::LoadProcess() load m_nFirstToLoadIndex WAIT_TIMEOUT \n" );
            if (m_nSceneId == SPWP_NO_SCENE)
                continue;

            if (m_nFirstToLoadIndex >= 0)
                continue;   
        }
		else
		{
			//CFS_FILELOGS::WriteLog(" KScenePlaceC::LoadProcess() load error fail \n" );
		}
	}
}

//##ModelId=3DCCD131018C
DWORD WINAPI KScenePlaceC::LoadThreadEntrance(void* pParam)
{
	if (pParam)
		((KScenePlaceC*)pParam)->LoadProcess();
	return 0;
}

//##ModelId=3DBDBC7200B4
void KScenePlaceC::SetRegionsToLoad()
{
	if (m_FocusMoveOffset.cx == 0 && m_FocusMoveOffset.cy == 0)
		return;

	EnterCriticalSection(&m_RegionListAdjustCritical);

	//可能正在加载中的区域
	KScenePlaceRegionC* pMayLoadingRegion = NULL;
	if (m_nFirstToLoadIndex >= 0)
		pMayLoadingRegion = m_pRegions[m_nFirstToLoadIndex];

	KScenePlaceRegionC* pTempRegions[SPWP_MAX_NUM_REGIONS];
	int nFirst = 0;
	int nLast = SPWP_MAX_NUM_REGIONS - 1;
	int i, nBourn, nX, nY;
	
	if (m_nFirstToLoadIndex < 0)
		nBourn = SPWP_MAX_NUM_REGIONS;
	else
		nBourn	= m_nFirstToLoadIndex;

	for (i = 0; i < nBourn; i++)
	{
		m_pRegions[i]->GetRegionIndex(nX, nY);
		if (INSIDE_AREA(nX, nY, SPWP_LOAD_EXTEND_RANGE))
			pTempRegions[nFirst++] = m_pRegions[i];
		else
			pTempRegions[nLast--] = m_pRegions[i];
	}

	//设置新的下个起始加载区域在区域指针列表中的索引
	m_nFirstToLoadIndex = (nFirst < SPWP_MAX_NUM_REGIONS) ? nFirst : -1;

	for (i = nBourn; i < SPWP_MAX_NUM_REGIONS; i++)
	{
		m_pRegions[i]->GetRegionIndex(nX, nY);
		if (INSIDE_AREA(nX, nY, SPWP_LOAD_EXTEND_RANGE))
			pTempRegions[nFirst++] = m_pRegions[i];
		else
			pTempRegions[nLast--] = m_pRegions[i];
	}

	int nNewLoadIdx[SPWP_MAX_NUM_REGIONS];
	int nCount = 0;

	if (m_FocusMoveOffset.cx * m_FocusMoveOffset.cx > SPWP_LOAD_EXTEND_RANGE * SPWP_LOAD_EXTEND_RANGE * 4 ||
		m_FocusMoveOffset.cy * m_FocusMoveOffset.cy > SPWP_LOAD_EXTEND_RANGE * SPWP_LOAD_EXTEND_RANGE * 4 )
	{
		
		memcpy(nNewLoadIdx, m_PRIIdxTable, sizeof(m_PRIIdxTable));
		nCount = SPWP_MAX_NUM_REGIONS;
	}
	else
	{
		int nBeginX, nEndX, nBeginY, nEndY;
		if (m_FocusMoveOffset.cx > 0)
		{
			nBeginX = SPWP_LOAD_EXTEND_RANGE * 2 + 1 - m_FocusMoveOffset.cx;
			nEndX = SPWP_LOAD_EXTEND_RANGE * 2;
		}
		else
		{
			nBeginX = 0;
			nEndX = -m_FocusMoveOffset.cx - 1;
		}
		
		if (m_FocusMoveOffset.cy > 0)
		{
			nBeginY = SPWP_LOAD_EXTEND_RANGE * 2 + 1 - m_FocusMoveOffset.cy;
			nEndY = SPWP_LOAD_EXTEND_RANGE * 2;
		}
		else
		{
			nBeginY = 0;
			nEndY = -m_FocusMoveOffset.cy - 1;
		}

		for (i = 0; i < SPWP_MAX_NUM_REGIONS; i++)
		{
			if (
				(m_RangePosTable[m_PRIIdxTable[i]].x >= nBeginX 
				&& m_RangePosTable[m_PRIIdxTable[i]].x <= nEndX)
				||
				(m_RangePosTable[m_PRIIdxTable[i]].y >= nBeginY 
				&& m_RangePosTable[m_PRIIdxTable[i]].y <= nEndY)
				)
			{
				nNewLoadIdx[nCount++] = m_PRIIdxTable[i];
			}
		}
	}

	//CFS_FILELOGS::WriteLog("\nKScenePlaceC::SetRegionsToLoad() m_nFirstToLoadIndex = %d nCount = %d !\n", m_nFirstToLoadIndex, nCount );
	_ASSERT((nFirst + nCount == SPWP_MAX_NUM_REGIONS));
	if ( (nFirst + nCount != SPWP_MAX_NUM_REGIONS) ) 
	{
		//CFS_FILELOGS::WriteLog("m_nFirstToLoadIndex = %d nFirst = %d, nCount = %d !\n", m_nFirstToLoadIndex, nFirst, nCount );
	}

	for (i = 0; i < nCount; i++)
	{
		if (pMayLoadingRegion == pTempRegions[nFirst + i])
		{
			//确保没有区域对象正在执行加载，如果有则等待这个区域对象加载结束
			EnterCriticalSection(&m_LoadCritical);
			LeaveCriticalSection(&m_LoadCritical);
			//运行到此处，此时可能为“一个区域刚加载完毕，但是还未及更新区域
			//指针列表的下个加载区域索引。”的情况
			//下面的处理把它当作还未加载处理。
		}

		pTempRegions[nFirst + i]->ToLoad(m_FocusRegion.x + m_RangePosTable[nNewLoadIdx[i]].x - SPWP_LOAD_EXTEND_RANGE,
			m_FocusRegion.y + m_RangePosTable[nNewLoadIdx[i]].y - SPWP_LOAD_EXTEND_RANGE);
	}
	memcpy(m_pRegions, pTempRegions, sizeof(m_pRegions));
	LeaveCriticalSection(&m_RegionListAdjustCritical);

	if (nCount)
		SetEvent(m_hLoadRegionEvent);
}

//##ModelId=3DCAA6B90196
unsigned int KScenePlaceC::AddObject(unsigned int uGenre, int nId, int x, int y, int z,
									 int eLayerParam)
{
	POINT	ri;
	KIpotRuntimeObj* pLeaf = NULL;

	ri.x = x / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	if (eLayerParam && INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_OBJ_RADIUS))
	{
		pLeaf = (KIpotRuntimeObj*)malloc(sizeof(KIpotRuntimeObj));
		if (pLeaf)
		{
			pLeaf->eLeafType = pLeaf->IPOTL_T_RUNTIME_OBJ;
			pLeaf->uGenre = uGenre;
			pLeaf->nId = nId;
			pLeaf->oPosition.x = x;
			pLeaf->oPosition.y = y + POINT_LEAF_Y_ADJUST_VALUE;
			pLeaf->nPositionZ = z;
			pLeaf->pAheadBrother = NULL;
			pLeaf->pBrother = NULL;
			pLeaf->pLChild = NULL;
			pLeaf->pRChild = NULL;
			pLeaf->pParentLeaf = NULL;
			pLeaf->pParentBranch = NULL;
			pLeaf->eLayerParam = eLayerParam;

			EnterCriticalSection(&m_ProcessCritical);
			m_ObjectsTree.AddLeafPoint(pLeaf);
			LeaveCriticalSection(&m_ProcessCritical);
		}
	}

	return ((unsigned int)pLeaf);
}

//##ModelId=3DCAA7000085
unsigned int KScenePlaceC::MoveObject(unsigned int uGenre, int nId,  int x, int y, int z,
									  unsigned int& uRtoid, int eLayerParam)
{
	if (uRtoid == 0)
	{
		uRtoid = AddObject(uGenre, nId, x, y, z, eLayerParam);
		return uRtoid;
	}

	KIpotRuntimeObj* pLeaf = (KIpotRuntimeObj*)uRtoid;
	POINT	ri;
	ri.x = x / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = y / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

	if (eLayerParam && INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_OBJ_RADIUS))
	{
		pLeaf->uGenre = uGenre;
		pLeaf->nId = nId;
		pLeaf->nPositionZ = z;
		pLeaf->eLayerParam = eLayerParam;
		if (pLeaf->oPosition.x != x || pLeaf->oPosition.y != y + POINT_LEAF_Y_ADJUST_VALUE)
		{
			EnterCriticalSection(&m_ProcessCritical);
			m_ObjectsTree.PluckRto(pLeaf);
			pLeaf->oPosition.x = x;
			pLeaf->oPosition.y = y + POINT_LEAF_Y_ADJUST_VALUE;
			m_ObjectsTree.AddLeafPoint(pLeaf);
			LeaveCriticalSection(&m_ProcessCritical);
		}
	}
	else
	{
		EnterCriticalSection(&m_ProcessCritical);
		m_ObjectsTree.PluckRto(pLeaf);
		LeaveCriticalSection(&m_ProcessCritical);
		free(pLeaf);
		pLeaf = NULL;
		uRtoid = 0;
	}
	return uRtoid;
}

//##ModelId=3DCAA70603E3
void KScenePlaceC::RemoveObject(unsigned int uGenre, int nId, unsigned int& uRtoid)
{
	if (uRtoid)
	{
		EnterCriticalSection(&m_ProcessCritical);
		m_ObjectsTree.PluckRto((KIpotRuntimeObj*)uRtoid);
		LeaveCriticalSection(&m_ProcessCritical);
		free ((KIpotRuntimeObj*)uRtoid);
		uRtoid = 0;
	}
}

void KScenePlaceC::Breathe()
{
	if (m_bLoading)
	{
		WaitForSingleObject(m_hSwitchLoadFinishedEvent, SPWP_SWITCH_SCENE_TIMEOUT);
		m_bRenderGround = true;
	}

	if (m_bPreprocessEvent)
	{
		m_bPreprocessEvent = false;
		Preprocess();
	}
	HandleMultiScroll(m_FocusPosition.x, m_FocusPosition.y);
}

//##ModelId=3DCD7F0A0071
void KScenePlaceC::Paint()
{
	IR_UpdateTime();
	int curFrameTime = IR_GetCurrentTime();
	if (m_bInited == false || m_szPlaceRootPath[0] == 0)
		return;

	if (m_bRenderGround )
	{
		m_bRenderGround = false;
		PrerenderGround(false);
	}

	EnterCriticalSection(&m_ProcessCritical);

	if ( m_DistanceViewerImage[0] && (219 == m_nCurrentSceneID || m_nCurrentSceneID == 218 || m_nCurrentSceneID == 217 || m_nCurrentSceneID == 213 || m_nCurrentSceneID == 2 || m_nCurrentSceneID == 60 || m_nCurrentSceneID == 201 || (m_nCurrentSceneID >= 77 && m_nCurrentSceneID <= 83) ))
	{
		KRUImage	Img;
		memset(&Img,0,sizeof(Img));

		Img.bRenderStyle = IMAGE_RENDER_STYLE_OPACITY;
		Img.bRenderFlag = 0;
		Img.Color.Color_dw = 0;
		Img.nFrame = 0;
		Img.nType = ISI_T_BITMAP16;
		Img.Color.Color_b.a = 255;
		Img.uImage = 0;
		strcpy(Img.szImage, m_DistanceViewerImage);

		g_pRepresent->DrawPrimitives(1, &Img, RU_T_IMAGE, true);
	}

	unsigned int i=0;

	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
		{			
			m_pInProcessAreaRegions[i]->PaintGround();
		}
		else
		{
			//CFS_FILELOGS::WriteLog("KScenePlaceC::Paint() m_pInProcessAreaRegions[%d] is NULL! \n", i );
		}
	}//*/

	if ( Option.IsDrawGround() )
	{
		m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_COVER_GROUND);
	}

	CoreDraw::gCurrentPlayerPaintedNum = 0;

	m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_OBJECT);

	if ( Option.IsDrawLargeObj() )
	{
		for (i = 0; i < m_nNumObjsAbove; i++)
		{
			KScenePlaceRegionC::PaintAboveHeadObj(m_pObjsAbove[i], &m_RepresentArea);
		}
	}
	m_ObjectsTree.Paint(&m_RepresentArea, IPOT_RL_INFRONTOF_ALL);

	LastFrameTime = curFrameTime;
	LeaveCriticalSection(&m_ProcessCritical);

	//====显示游戏（场景）信息====
#ifdef SWORDONLINE_SHOW_DBUG_INFO
	if (g_bShowObstacle)
	{
		for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
		{
			if (m_pInProcessAreaRegions[i])
			{
				m_pInProcessAreaRegions[i]->PaintObstacle();
			}
		}
	}

	if (g_bShowGameInfo)
	{
		KRUShadow	Shadow;
		Shadow.oPosition.nX = 0;
		Shadow.oPosition.nY = 360;
		Shadow.oEndPos.nX = 268;
		Shadow.oEndPos.nY = 440;
		Shadow.Color.Color_dw = 0x16000000;

		g_pRepresent->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
		char	szInfo[120];
		KOutputTextParam	Param;
		Param.BorderColor = 0;
		Param.nX = Shadow.oPosition.nX + 2;
		Param.nY = Shadow.oPosition.nY + 2;
		Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;
		Param.Color = 0xffffffff;
		Param.nNumLine = 1;
		Param.nSkipLine = 0;
		int nLen = sprintf(szInfo, "Map<color=Green>[%d]<color=White>:%s", g_nMapIndex, m_szSceneName);
		nLen = TEncodeText(szInfo, nLen);
		g_pRepresent->OutputRichText(12, &Param, szInfo, nLen);
		nLen = sprintf(szInfo, "Focus:<color=Green>%d,%d<color=White>-%d,%d",
			m_FocusRegion.x, m_FocusRegion.y, m_FocusPosition.x, m_FocusPosition.y);
		nLen = TEncodeText(szInfo, nLen);
		Param.nY += 12;
		g_pRepresent->OutputRichText(12, &Param, szInfo, nLen);
	}
#endif

// <-- Rocker End


}

// nX nY 像素点坐标
BOOL	KScenePlaceC::AddObstacle(int nX, int nY, int nObstacleKind)
{
	POINT	ri;
	ri.x = nX / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = nY / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

	if (INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_RADIUS))
	{
		KScenePlaceRegionC* pRegion = GET_IN_PROCESS_AREA_REGION(ri.x, ri.y);
		if (pRegion)
			return pRegion->AddObstacle(nX, nY, nObstacleKind);
	}
	return FALSE;
}

// nX nY 像素点坐标
BOOL	KScenePlaceC::ClearObstacle(int nX, int nY)
{
	POINT	ri;
	ri.x = nX / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = nY / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

	if (INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_RADIUS))
	{
		KScenePlaceRegionC* pRegion = GET_IN_PROCESS_AREA_REGION(ri.x, ri.y);
		if (pRegion)
			return pRegion->ClearObstacle(nX, nY);
	}
	return FALSE;
}

//##ModelId=3DCE68BB0238
void KScenePlaceC::ChangeLoadArea( bool bSyncWorld )
{
	SetRegionsToLoad();
}

//##ModelId=3DBF946D0053
void KScenePlaceC::ChangeProcessArea()
{
	KRUImage*	pImage = NULL;
	/*
	RECT	rc;
	rc.left = (m_FocusRegion.x - 1) * KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	rc.top  = (m_FocusRegion.y - 1) *  KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	rc.right  = rc.left + SPWP_PROCESS_RANGE * KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	rc.bottom = rc.top  + SPWP_PROCESS_RANGE * KScenePlaceRegionC::RWPP_AREGION_HEIGHT;//*/

	EnterCriticalSection(&m_ProcessCritical);
	EnterCriticalSection(&m_RegionListAdjustCritical);
	int h, v;
	for (int i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
		{
			m_pInProcessAreaRegions[i]->GetRegionIndex(h, v);
			if (INSIDE_AREA(h, v, SPWP_PROCESS_RADIUS) == 0)
			{
				m_pInProcessAreaRegions[i]->LeaveProcessArea();				
			}
			m_pInProcessAreaRegions[i] = NULL;
		}
	}
	
	int nNum;
	if (m_nFirstToLoadIndex < 0)
		nNum = SPWP_MAX_NUM_REGIONS;
	else
		nNum = m_nFirstToLoadIndex;
	for (i = 0; i < nNum; i++)
	{
		m_pRegions[i]->GetRegionIndex(h, v);
		if (INSIDE_AREA(h, v, SPWP_PROCESS_RADIUS))
		{
			GET_IN_PROCESS_AREA_REGION(h, v) = m_pRegions[i];
			pImage = m_pRegions[i]->GetPrerenderGroundImage();
			if (pImage == NULL && l_bPrerenderGround)
			{
				pImage = GetFreeGroundImage();
				//_ASSERT(pImage);
			}
			m_pRegions[i]->EnterProcessArea(pImage);
		}
	}

	if (m_nFirstToLoadIndex < 0 || m_nFirstToLoadIndex >= SPWP_PROCESS_PRERENDER_REGION_COUNTER_TRIGGER)
	{
		m_bPreprocessEvent = true;
		m_bRenderGround = true;
	}

	LeaveCriticalSection(&m_RegionListAdjustCritical);
	LeaveCriticalSection(&m_ProcessCritical);
}

void KScenePlaceC::PrerenderGround(bool bForce)
{
	EnterCriticalSection(&m_RegionListAdjustCritical);
	for (int i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
			m_pInProcessAreaRegions[i]->PrerenderGround(bForce);
	}
	LeaveCriticalSection(&m_RegionListAdjustCritical);
}

//##ModelId=3DBFA1460230
void KScenePlaceC::Preprocess()
{
	unsigned int i, j, dx, dy, nTotalLineObj = 0;

	struct
	{
		KIpotBuildinObj*	pObjsPoint;
		KIpotBuildinObj*	pObjsLine;
		KIpotBuildinObj*	pObjsTree;
		KBuildinObj*		pObjsAbove;
		unsigned int		nNumObjsPoint;
		unsigned int		nNumObjsLine;
		unsigned int		nNumObjsTree;
		unsigned int		nNumObjsAbove;
	}RegionRtoData[SPWP_NUM_REGIONS_IN_PROCESS_AREA_1024] = { 0 };

	EnterCriticalSection(&m_ProcessCritical);

	ClearPreprocess(false);

	m_ObjectsTree.SetPermanentBranchPos(
		m_FocusPosition.x - KScenePlaceRegionC::RWPP_AREGION_WIDTH * 2,
		m_FocusPosition.x + KScenePlaceRegionC::RWPP_AREGION_WIDTH * 2,
		m_FocusPosition.y - KScenePlaceRegionC::RWPP_AREGION_HEIGHT * 2);

	//--------获取内建对象的列表----------
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		if (m_pInProcessAreaRegions[i])
		{
			m_pInProcessAreaRegions[i]->GetBuildinObjs(
				RegionRtoData[i].pObjsPoint, RegionRtoData[i].nNumObjsPoint,
				RegionRtoData[i].pObjsLine, RegionRtoData[i].nNumObjsLine,
				RegionRtoData[i].pObjsTree, RegionRtoData[i].nNumObjsTree);
			nTotalLineObj += RegionRtoData[i].nNumObjsLine;

			RegionRtoData[i].nNumObjsAbove = m_pInProcessAreaRegions[i]->
				GetAboveHeadLayer(RegionRtoData[i].pObjsAbove);
			m_nNumObjsAbove += RegionRtoData[i].nNumObjsAbove;
		}
	}
	
	// --> Rocker Edit Start 2005/12/07 如果在1024界面下，找到外围一圈区域并加入场景树
	if (g_GetScreenWidth() == 1024)
	{
		int nNum;
		if (m_nFirstToLoadIndex < 0)
			nNum = SPWP_MAX_NUM_REGIONS;
		else
			nNum = m_nFirstToLoadIndex;
		int n = SPWP_NUM_REGIONS_IN_PROCESS_AREA;
		for (i = 0; i < nNum; i++)
		{
			int h,v;
			m_pRegions[i]->GetRegionIndex(h, v);
			if ((h - m_FocusRegion.x == 2 || h - m_FocusRegion.x == -2)&&
				(v - m_FocusRegion.y <= 2 && v - m_FocusRegion.y >= -2))
			{
				m_pRegions[i]->EnterProcessArea(NULL);
				m_pRegions[i]->GetBuildinObjs(
					RegionRtoData[n].pObjsPoint, RegionRtoData[n].nNumObjsPoint,
					RegionRtoData[n].pObjsLine, RegionRtoData[n].nNumObjsLine,
					RegionRtoData[n].pObjsTree, RegionRtoData[n].nNumObjsTree);
				nTotalLineObj += RegionRtoData[n].nNumObjsLine;

				RegionRtoData[n].nNumObjsAbove = m_pRegions[i]->GetAboveHeadLayer(RegionRtoData[n].pObjsAbove);
				m_nNumObjsAbove += RegionRtoData[n].nNumObjsAbove;
				n++;
				if (n >= SPWP_NUM_REGIONS_IN_PROCESS_AREA_1024)
					break;
			}
		}
	}
	// <-- Rocker End

	//--------处理高空对象---------
	if (m_nNumObjsAbove)
	{
		m_pObjsAbove = (KBuildinObj**)malloc(sizeof(KBuildinObj*) * m_nNumObjsAbove);
		m_nNumObjsAbove = 0;
		if (m_pObjsAbove)
		{
			for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
			{
				while(RegionRtoData[i].nNumObjsAbove)
				{
					RegionRtoData[i].nNumObjsAbove--;
					KBuildinObj* pBio = &RegionRtoData[i].pObjsAbove[RegionRtoData[i].nNumObjsAbove];
					for (j = 0; j < m_nNumObjsAbove; j++)
					{
						KBuildinObj* pBio2 = m_pObjsAbove[j];
						if ((pBio->oPos1.y < pBio2->oPos1.y) ||
							(pBio->oPos1.y == pBio2->oPos1.y &&
								(pBio->oPos1.z < pBio2->oPos1.z)))
						{
							break;
						}
					}
					for (unsigned int k = m_nNumObjsAbove; k > j; k--)
						m_pObjsAbove[k] = m_pObjsAbove[k - 1];
					m_pObjsAbove[j] = &RegionRtoData[i].pObjsAbove[RegionRtoData[i].nNumObjsAbove];
					m_nNumObjsAbove ++;
				}
			}
		}			
	}

	//--------处理树方式排序的对象---------
	class TreeObjSet : public KNode
	{
	public:
		float	fAngleXY;
		float	fNodicalY;
		int		nLength2;
		POINT	oLP1, oLP2;
		KIpotBuildinObj*	pObjs;
	};

	KList		List;
	TreeObjSet	*pNode1 = NULL, *pNode2 = NULL;
	KIpotBuildinObj* pObj = NULL;

	//---把同在一条直线上的连在一起--
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		for (j = 0; j < RegionRtoData[i].nNumObjsTree; j++)
		{
			pObj = &RegionRtoData[i].pObjsTree[j];
			
			pNode1 = (TreeObjSet*)List.GetHead();
			while (pNode1)
			{
				if (SM_IsLineLinkable(pObj->pBio->fAngleXY,
					pObj->pBio->fNodicalY,
					pNode1->fAngleXY, pNode1->fNodicalY))
				{
					break;
				}
				pNode1 = (TreeObjSet*)pNode1->GetNext();
			};

			if (pNode1 == NULL)
			{
				if ((pNode1 = new TreeObjSet) == NULL)
				{
					i = SPWP_NUM_REGIONS_IN_PROCESS_AREA;
					break;
				}
				pNode1->pObjs = pObj;
				pNode1->oLP1 = pObj->oPosition;
				pNode1->oLP2 = pObj->oEndPos;
				pNode1->fAngleXY  = pObj->fAngleXY  = pObj->pBio->fAngleXY;
				pNode1->fNodicalY = pObj->fNodicalY = pObj->pBio->fNodicalY;
				List.AddHead(pNode1);
				continue;
			}
			
			pObj->fAngleXY  = pNode1->fAngleXY;
			pObj->fNodicalY = pNode1->fNodicalY;

			int x = pObj->oPosition.x;

			if (x <  pNode1->pObjs->oPosition.x)
			{
				pObj->pBrother = pNode1->pObjs;
				pNode1->pObjs = pObj;
			}
			else
			{
				KIpotBuildinObj* pSortStop = pNode1->pObjs;
				while(pSortStop->pBrother)
				{
					if (x < ((KIpotBuildinObj*)pSortStop->pBrother)->oPosition.x)
						break;
					pSortStop = (KIpotBuildinObj*)pSortStop->pBrother;
				};
				pObj->pBrother = pSortStop->pBrother;
				pSortStop->pBrother = pObj;
			}
			if (pNode1->oLP1.x > pObj->oPosition.x)
				pNode1->oLP1 = pObj->oPosition;
			if (pNode1->oLP2.x < pObj->oEndPos.x)
				pNode1->oLP2 = pObj->oEndPos;
		}
	}
	
	//----把线条组合按长度排序----
	pNode1 = (TreeObjSet*)List.GetHead();
	while(pNode1)
	{
		dx = pNode1->oLP1.x - pNode1->oLP2.x;
		dy = pNode1->oLP1.y - pNode1->oLP2.y;
		pNode1->nLength2 = dx * dx + dy * dy;
		pNode1 = (TreeObjSet*)pNode1->GetNext();
	};

	KList		List2;
	while(pNode1 = (TreeObjSet*)List.GetHead())
	{
		pNode2 = (TreeObjSet*)pNode1->GetNext();
		while(pNode2)
		{
			if (pNode1->nLength2 < pNode2->nLength2)
				pNode1 = pNode2;
			pNode2 = (TreeObjSet*)pNode2->GetNext();
		};
		pNode1->Remove();
		List2.AddTail(pNode1);
	};
	
	//----把按树方式排序的对象加入对象树-----
	while(pNode1 = (TreeObjSet*)List2.RemoveHead())
	{
		while(pObj = pNode1->pObjs)
		{
			pNode1->pObjs = (KIpotBuildinObj*)pObj->pBrother;
			pObj->pBrother = NULL;
			m_ObjectsTree.AddBranch(pObj);
		}
		delete pNode1;	
	};

// 	// --> Rocker Edit Start 2005/12/07
	int nLineProcessRegionCount = SPWP_NUM_REGIONS_IN_PROCESS_AREA;
	if (g_GetScreenWidth() == 1024)
	{
		nLineProcessRegionCount = SPWP_NUM_REGIONS_IN_PROCESS_AREA_1024;
	}
// 	// <-- Rocker End
	
	//----把线方式排序的对象进行根据线长度排序-----
	if (nTotalLineObj)
	{
		struct LineObjItem//---处理线方式排序的对象-----
		{
			int		nLength2;
			KIpotBuildinObj*	pObj;
		};
		LineObjItem* pNodeList = (LineObjItem*)malloc(sizeof(LineObjItem) * nTotalLineObj);
		nTotalLineObj = 0;
		if (pNodeList)
		{
			//for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
			// --> Rocker Edit Start 2005/12/07
			for (i = 0; i < nLineProcessRegionCount; i++)
			// <-- Rocker End
			{
				for (j = 0; j < RegionRtoData[i].nNumObjsLine; j++)
				{
					KIpotBuildinObj* pObj = &RegionRtoData[i].pObjsLine[j];
					dx = pObj->oEndPos.x - pObj->oPosition.x;
					dy = pObj->oEndPos.y - pObj->oPosition.y;
					int nLength2 = dx * dx + dy * dy;

					for (unsigned int k = 0; k < nTotalLineObj; k++)
					{
						if (nLength2 > pNodeList[k].nLength2)
						{
							for (unsigned int j = nTotalLineObj; j > k; j--)
								pNodeList[j] = pNodeList[j - 1];
							pNodeList[k].pObj = pObj;
							pNodeList[k].nLength2 = nLength2;
							break;
						}
					}
					if (k == nTotalLineObj)
					{
						pNodeList[nTotalLineObj].pObj = pObj;
						pNodeList[nTotalLineObj].nLength2 = nLength2;
					}
					nTotalLineObj++;
				}
			}
			//----把线方式排序的对象加入对象树-----
			for (i = 0; i < nTotalLineObj; i++)
			{
				m_ObjectsTree.AddLeafLine(pNodeList[i].pObj);
			}
			free(pNodeList);
			pNodeList = NULL;
		}
	}

	//----把点方式排序的对象加入对象树-----
	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
	{
		for (j = 0; j < RegionRtoData[i].nNumObjsPoint; j++)
		{
			m_ObjectsTree.AddLeafPoint(&RegionRtoData[i].pObjsPoint[j]);
		}
	}

	
	//----把场景内建光源加入树-----
//	KBuildInLightInfo* pLights = NULL;
//	for (i = 0; i < SPWP_NUM_REGIONS_IN_PROCESS_AREA; i++)
//	{
//
//		if (m_pInProcessAreaRegions[i])
//		{
//			j = m_pInProcessAreaRegions[i]->GetBuildinLights(pLights);
//			if (j)
//				m_ObjectsTree.AddBuildinLight(pLights, j);			
//		}
//	}

	RECT	KeepArea;
	KeepArea.left = (m_FocusRegion.x - 1) * KScenePlaceRegionC::RWPP_AREGION_WIDTH - SPWP_RTO_HALF_RANGE;
	KeepArea.right = KeepArea.left + KScenePlaceRegionC::RWPP_AREGION_WIDTH * SPWP_PROCESS_RANGE + SPWP_RTO_HALF_RANGE * 2;
	KeepArea.top = (m_FocusRegion.y - 1) * KScenePlaceRegionC::RWPP_AREGION_HEIGHT - SPWP_RTO_HALF_RANGE;
	KeepArea.bottom = KeepArea.top + KScenePlaceRegionC::RWPP_AREGION_HEIGHT * SPWP_PROCESS_RANGE + SPWP_RTO_HALF_RANGE * 2;
	m_ObjectsTree.StrewRtoLeafs(KeepArea);

	LeaveCriticalSection(&m_ProcessCritical);
}

//##ModelId=3DCCBD7B0239
void KScenePlaceC::ClearPreprocess(int bIncludeRto)
{
	if (m_pObjsAbove)
	{
		free(m_pObjsAbove);
		m_pObjsAbove = NULL;
	}
	m_nNumObjsAbove = 0;

	EnterCriticalSection(&m_ProcessCritical);
	if (bIncludeRto == false)
		m_ObjectsTree.Fell();
	else
		m_ObjectsTree.Clear();
	LeaveCriticalSection(&m_ProcessCritical);
}

void KScenePlaceC::ProjectDistToSpaceDist(int& nXDistance, int& nYDistance)
{
	nYDistance = nYDistance + nYDistance;
}

void KScenePlaceC::ViewPortCoordToSpaceCoord(int& nX, int& nY, int nZ)
{
	if (g_pRepresent)
		g_pRepresent->ViewPortCoordToSpaceCoord(nX, nY, nZ);
	else
	{
		nX = nX + m_RepresentArea.left + SPWP_REPRESENT_RECT_WINDAGE_X;
		nY = (nY + ((nZ * 887) >> 10)) * 2 + m_RepresentArea.top  + SPWP_REPRESENT_RECT_WINDAGE_T * 2;
	}
}

void KScenePlaceC::GetRegionLeftTopPos(int nRegionX, int nRegionY, int& nLeft, int& nTop)
{
	nLeft = nRegionX * KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	nTop  = nRegionY * KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
}

void KScenePlaceC::ARegionLoaded(KScenePlaceRegionC* pRegion)
{
	_ASSERT(pRegion);
	//如果刚加载完毕的区域属于预处理范围则触发预处理信号
	int	h, v;

	EnterCriticalSection(&m_RegionListAdjustCritical);
	int nCount;
	if (m_nFirstToLoadIndex >= 0)		
	{
		nCount = m_nFirstToLoadIndex;
		if (m_pRegions[m_nFirstToLoadIndex] == pRegion)
		{
			m_nFirstToLoadIndex++;
			//CFS_FILELOGS::WriteLog("KScenePlaceC::ARegionLoaded() change m_nFirstToLoadIndex = %d\n\n", m_nFirstToLoadIndex );
			if (m_nFirstToLoadIndex == SPWP_MAX_NUM_REGIONS)
				m_nFirstToLoadIndex = -1;
		}
	}
	else
	{
		nCount = SPWP_MAX_NUM_REGIONS - 1;
	}
	for (h = 0; h < nCount; h++)
	{
		pRegion->SetNestRegion(m_pRegions[h]);
		m_pRegions[h]->SetNestRegion(pRegion);
	}
	LeaveCriticalSection(&m_RegionListAdjustCritical);
	if (m_nFirstToLoadIndex < 0)
		ResetEvent(m_hLoadRegionEvent);

	if (nCount >= SPWP_PROCESS_PRERENDER_REGION_COUNTER_TRIGGER)
	{
		SetLoadingStatus(false);
		m_bRenderGround = true;
	}

	KRUImage* pImage = NULL;
	pRegion->GetRegionIndex(h, v);
	if (INSIDE_AREA(h, v, SPWP_PROCESS_RADIUS))
	{
		EnterCriticalSection(&m_ProcessCritical);
		if (l_bPrerenderGround)
			pImage = GetFreeGroundImage();
//		_ASSERT(pImage);
		pRegion->EnterProcessArea(pImage);
		GET_IN_PROCESS_AREA_REGION(h, v) = pRegion;

		if (nCount >= 8)
			m_bPreprocessEvent = true;
		LeaveCriticalSection(&m_ProcessCritical);
	}

	//处理highlight的special object
	if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT &&
		h == m_nHLSpecialObjectRegionX && v == m_nHLSpecialObjectRegionY)
	{
		EnterCriticalSection(&m_ProcessCritical);
		pRegion->SetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
		LeaveCriticalSection(&m_ProcessCritical);
	}

	if (m_pfunRegionLoadedCallback)
		m_pfunRegionLoadedCallback(h, v);
}

KRUImage* KScenePlaceC::GetFreeGroundImage()
{
	for (int i = 0; i < m_nNumGroundImagesAvailable; i++)
	{
		if (m_RegionGroundImages[i].GROUND_IMG_OCCUPY_FLAG == false)
			return (&m_RegionGroundImages[i]);
	}

	return NULL;
}

long KScenePlaceC::GetObstacleInfo(int nX, int nY)
{
	POINT	ri;
	ri.x = nX / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = nY / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;

	if (INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_RADIUS))
	{
		KScenePlaceRegionC* pRegion = GET_IN_PROCESS_AREA_REGION(ri.x, ri.y);
		if (pRegion)
			return pRegion->GetObstacleInfo(nX, nY);
	}
	return Obstacle_Normal;
}

long KScenePlaceC::GetTrapInfo(int nX, int nY)
{
	POINT	ri;
	ri.x = nX / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = nY / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	
	if (INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_RADIUS))
	{
		KScenePlaceRegionC* pRegion = GET_IN_PROCESS_AREA_REGION(ri.x, ri.y);
		if (pRegion)
			return pRegion->GetTrapInfo(nX, nY);
	}
	return 0;
}

long KScenePlaceC::GetObstacleInfoMin(int nX, int nY, int nOffX, int nOffY)
{
	POINT	ri;
	ri.x = nX / KScenePlaceRegionC::RWPP_AREGION_WIDTH;
	ri.y = nY / KScenePlaceRegionC::RWPP_AREGION_HEIGHT;
	
	if (INSIDE_AREA(ri.x, ri.y, SPWP_PROCESS_RADIUS))
	{
		KScenePlaceRegionC* pRegion = GET_IN_PROCESS_AREA_REGION(ri.x, ri.y);
		if (pRegion)
			return pRegion->GetObstacleInfoMin(nX, nY, nOffX, nOffY);
	}
	return Obstacle_Normal;
}

void KScenePlaceC::RepresentShellReset()
{
	m_bRenderGround = true;
	for (int i = 0; i < m_nNumGroundImagesAvailable; i++)
		m_RegionGroundImages[i].GROUND_IMG_OK_FLAG = false;
}

//设置场景中一个区域被加载完毕后的回调函数
void  KScenePlaceC::SetRegionLoadedCallback(funScenePlaceRegionLoadedCallback pfunCallback)
{
	m_pfunRegionLoadedCallback = pfunCallback;
}

void  KScenePlaceC::SetHightLightSpecialObject(int nRegionX, int nRegionY, int nBioIndex)
{
	KScenePlaceRegionC* pRegion = NULL;
	EnterCriticalSection(&m_RegionListAdjustCritical);
	{
		if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT)
		{
			pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
			if (pRegion)
			{
				EnterCriticalSection(&m_ProcessCritical);
				pRegion->UnsetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
				LeaveCriticalSection(&m_ProcessCritical);
			}
		}

		m_nHLSpecialObjectRegionX = nRegionX;
		m_nHLSpecialObjectRegionY = nRegionY;
		m_nHLSpecialObjectBioIndex = nBioIndex;

		if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT)
		{
			pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
			if (pRegion)
			{
				EnterCriticalSection(&m_ProcessCritical);
				pRegion->SetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
				LeaveCriticalSection(&m_ProcessCritical);
			}
		}
	}
	LeaveCriticalSection(&m_RegionListAdjustCritical);
}

void  KScenePlaceC::UnsetHightLightSpecialObject()
{
	if (m_nHLSpecialObjectBioIndex != SPWP_NO_HL_SPECAIL_OBJECT)
	{
		EnterCriticalSection(&m_RegionListAdjustCritical);
		KScenePlaceRegionC* pRegion = GetLoadedRegion(m_nHLSpecialObjectRegionX, m_nHLSpecialObjectRegionY);
		if (pRegion)
		{
			EnterCriticalSection(&m_ProcessCritical);
			pRegion->UnsetHightLightSpecialObject(m_nHLSpecialObjectBioIndex);
			LeaveCriticalSection(&m_ProcessCritical);
		}
		LeaveCriticalSection(&m_RegionListAdjustCritical);
		m_nHLSpecialObjectBioIndex = SPWP_NO_HL_SPECAIL_OBJECT;
	}
}

KScenePlaceRegionC*	KScenePlaceC::GetLoadedRegion(int h, int v)
{
	EnterCriticalSection(&m_RegionListAdjustCritical);
	int nCount;
	KScenePlaceRegionC* pRegion = NULL;
	if (m_nFirstToLoadIndex < 0)
		nCount = SPWP_MAX_NUM_REGIONS;
	else
		nCount = m_nFirstToLoadIndex;
	for (int i = 0; i < nCount; i++)
	{
		int rh, rv;
		m_pRegions[i]->GetRegionIndex(rh, rv);
		if (rh == h && rv == v)
		{
			pRegion = m_pRegions[i];
			break;
		}
	}
	LeaveCriticalSection(&m_RegionListAdjustCritical);
	return pRegion;
}

void KScenePlaceC::GetSceneNameAndFocus(char* pszName, int& nId, int& nX, int& nY)
{
	if (pszName != NULL)
	{
		strcpy(pszName, m_szSceneName);
	}
	nId = m_nSceneId;
	nX = (m_FocusPosition.x) / 32;
	nY = (m_FocusPosition.y) / 64;
}

void KScenePlaceC::ChangeWeather(int nWeatherID)
{
}

void KScenePlaceC::EnableWeather(int nbEnable)
{
}

void KScenePlaceC::SetLoadingStatus(bool bLoading)
{
	if ((!m_bLoading) != (!bLoading))
	{
		m_bLoading = bLoading;
//		CoreDataChanged(GDCNI_SWITCHING_SCENEPLACE, m_nCurrentSceneID, m_bLoading);
		if (m_bLoading == false)
			SetEvent(m_hSwitchLoadFinishedEvent);
	}
}

void KScenePlaceC::getMapInfoAtMiniPos(MapPosInfo* mapInfo, Position* destPos)
{
	int x = destPos->x + m_Map.GetMiniMapArea().left;
	int y = destPos->y + m_Map.GetMiniMapArea().top;

	mapInfo->pos.x = x / 2;
	mapInfo->pos.y = y / 2;
	mapInfo->mapId = SubWorld[0].m_SubWorldID;

	sscanf(m_szPlaceRootPath, "\\Maps\\%s", mapInfo->mapName);
}

void KScenePlaceC::getMapInfoAtPos(MapPosInfo* mapInfo, Position* destPos)
{
	int x = destPos->x - m_Map.GetSceneMapArea().x;
	int y = destPos->y - m_Map.GetSceneMapArea().y;

	int nWorldH = SubWorld[0].m_nWorldRegionHeight * REGION_PIXEL_HEIGHT;
	int nWorldW = SubWorld[0].m_nWorldRegionWidth * REGION_PIXEL_WIDTH;

	mapInfo->pos.x = ((float)x / m_Map.GetSceneMapArea().width * nWorldW + SubWorld[0].m_nRegionBeginX * REGION_PIXEL_WIDTH) * 0.03125;
	mapInfo->pos.y = ((float)y / m_Map.GetSceneMapArea().height * nWorldH + SubWorld[0].m_nRegionBeginY * REGION_PIXEL_HEIGHT) * 0.015625;
	mapInfo->mapId = SubWorld[0].m_SubWorldID;

	sscanf(m_szPlaceRootPath, "\\Maps\\%s", mapInfo->mapName);
}

void KScenePlaceC::getNpcNameAtPos(vector<string>* npcNames, Position* pos)
{	
	int nNpcIdx = 0;
	pos->x = pos->x - m_Map.GetSceneMapArea().x;
	pos->y = pos->y - m_Map.GetSceneMapArea().y;
	
	int nWorldH = m_Map.GetGridArea().height * REGION_PIXEL_HEIGHT;
	int nWorldW = m_Map.GetGridArea().width * REGION_PIXEL_WIDTH;

	KPlayer& player = GetClientPlayer();

	if (player.GetTeamInfo().IsInTeam())
	{
		KTeam* pTeam = player.GetTeamInfo().GetTeam();
		if (pTeam != NULL)
		{
			int selfMapId = SubWorld[0].m_SubWorldID;
			
			int maxMemberCount = pTeam->GetMaxMemberCount();
			for (int memberIndex = 0; memberIndex < maxMemberCount; memberIndex++)
			{
				ClientTeamMemberInfo* pMemberInfo = pTeam->GetMemberInfo(memberIndex);
				if (pMemberInfo != NULL)
				{
					if (pMemberInfo->NpcId > 0)
					{	
						int mapId = pMemberInfo->MapId;
						if (mapId == selfMapId)
						{
							int posX = pMemberInfo->PosX;
							int posY = pMemberInfo->PosY;
							
							float xposPercent = ((float)(posX - m_Map.GetGridArea().x * REGION_PIXEL_WIDTH)) / nWorldW;
							float yposPercent = ((float)(posY - m_Map.GetGridArea().y * REGION_PIXEL_HEIGHT)) / nWorldH;
							posX = m_Map.GetSceneMapArea().width * xposPercent;
							posY = m_Map.GetSceneMapArea().height * yposPercent;
							
							if(posX < pos->x - 8 || posX > pos->x + 8 || posY < pos->y - 8 || posY > pos->y + 8)
							{
								continue;
							}
							
							npcNames->push_back(pMemberInfo->Name);
						}
					}
				}
			}	
		}
	}

// 	while (nNpcIdx = NpcSet.GetNextIdx(nNpcIdx))
// 	{
// 		if (Npc[nNpcIdx].m_RegionIndex == -1)
// 			continue;
// 
//  		int nNpcX = 0;
// 		int nNpcY = 0;
// 
// 		Npc[nNpcIdx].GetMpsPos( &nNpcX, &nNpcY );
// 
// 		float xposPercent = ((float)(nNpcX - SubWorld[0].m_nRegionBeginX * REGION_PIXEL_WIDTH)) / nWorldW;
// 		float yposPercent = ((float)(nNpcY - SubWorld[0].m_nRegionBeginY * REGION_PIXEL_HEIGHT)) / nWorldH;
// 		nNpcX = m_Map.GetSceneMapArea().width * xposPercent;
// 		nNpcY = m_Map.GetSceneMapArea().height * yposPercent;
// 
// 		if(nNpcX < pos->x - 8 || nNpcX > pos->x + 8 || nNpcY < pos->y - 8 || nNpcY > pos->y + 8)
// 		{
// 			continue;	
// 		}
// 
// 		bool atPos = false;
// 		if (Npc[nNpcIdx].m_Kind == kind_dialoger)
// 		{
// 			//MapInfoNpc不在这里显示
// 			if (m_Map.IsMapInfoNpc(Npc[nNpcIdx].Name))
// 				continue;
// 
// 			atPos = true;
// 		}
// 		else if (Npc[nNpcIdx].m_Kind == kind_player)// && nNpcIdx != Player[CLIENT_PLAYER_INDEX].m_nIndex)
// 		{
// // 			if(Player[CLIENT_PLAYER_INDEX].m_cTeam.m_nFlag &&				//主角是组队状态
// // 				((DWORD)g_TeamC[0].GetCaptain() == Npc[nNpcIdx].m_dwID		//是主角所在队伍的队长
// // 				|| g_TeamC[0].FindMemberID(Npc[nNpcIdx].m_dwID) >= 0))		//是主角所在队伍的队员
// // 			{
// // 				atPos = true;	
// // 			}
// 			if(Player[CLIENT_PLAYER_INDEX].GetNpcIndex() == nNpcIdx)
// 			{
// 				atPos = true;
// 			}
// 		}
// 
// 		if(atPos)
// 		{
// 			npcNames->push_back(Npc[nNpcIdx].Name);
// 		}
// 
// 		//++nNpcIdx;
// 	}
	
	//世界同步NPC
	const int syncToWorldNpcCount = NpcSet.GetSyncToWorldNpcCount();
	for (int syncToWorldNpcIndex = 0; syncToWorldNpcIndex < syncToWorldNpcCount; syncToWorldNpcIndex++)
	{
		SyncToWorldNpcInfo* pInfo = NpcSet.GetSyncToWorldNpcInfo(syncToWorldNpcIndex);
		if (pInfo && !m_Map.IsHideSyncToWorldNpc(pInfo->NpcId))
		{
			int posX = pInfo->PosX;
			int posY = pInfo->PosY;
			
			float xposPercent = ((float)(posX - m_Map.GetGridArea().x * REGION_PIXEL_WIDTH)) / nWorldW;
			float yposPercent = ((float)(posY - m_Map.GetGridArea().y * REGION_PIXEL_HEIGHT)) / nWorldH;
			posX = m_Map.GetSceneMapArea().width * xposPercent;
			posY = m_Map.GetSceneMapArea().height * yposPercent;
			
			if(posX < pos->x - 8 || posX > pos->x + 8 || posY < pos->y - 8 || posY > pos->y + 8)
			{
				continue;
			}

			npcNames->push_back(pInfo->Name);
		}
	}

	pos->x = ((float)pos->x / m_Map.GetSceneMapArea().width * nWorldW + m_Map.GetGridArea().x * REGION_PIXEL_WIDTH) * 0.03125;
	pos->y = ((float)pos->y / m_Map.GetSceneMapArea().height * nWorldH + m_Map.GetGridArea().y * REGION_PIXEL_HEIGHT) * 0.015625;

	vector<MapNpcMgr::NpcInfo>& npcList = MapNpcMgr::getSingleton().loadMap(m_Map.mapName());
	const int listSize = npcList.size();
	for(int i = 0; i < listSize; ++i)
	{
		if(npcList[i].x < pos->x - 8 || npcList[i].x > pos->x + 8 || npcList[i].y < pos->y - 8 || npcList[i].y > pos->y + 8)
		{
			continue;
		}

		npcNames->push_back(npcList[i].npcName);
	}
}

void KScenePlaceC::PaintMiniMap(int nX, int nY)
{
	m_Map.PaintMiniMap(nX, nY, m_nbShowCharacter);
}

void KScenePlaceC::PaintMiniMapOnDC(ChatPoint& chatPoint)
{
	m_Map.PaintMiniMapOnDC(chatPoint,m_nbShowCharacter);
}
void KScenePlaceC::PaintSceneMap()
{
	if ( m_bShowSceneMap )
	{
		m_Map.PaintSceneMap(m_bCurrentMap,m_nbShowCharacter);
	}
}

void KScenePlaceC::gotoPosition(int x, int y)
{
	if ( m_bShowSceneMap )
	{
		m_Map.gotoPosition(x, y);
	}
}

void KScenePlaceC::SetMapParam(unsigned int uShowElems, int nSize)
{
	m_Map.SetShowElemsFlag(uShowElems);
	if (uShowElems)
		m_Map.SetSize((nSize & 0xffff), (nSize >> 16));
}

//设置场景的地图的焦点(单位:场景坐标)
void KScenePlaceC::SetMapFocusPositionOffset(int nOffsetX, int nOffsetY)
{
	if (m_bFollowWithMap == false)
	{
		m_MapFocusOffset.x = nOffsetX;
		m_MapFocusOffset.y = nOffsetY;
		m_Map.SetFocusPosition(m_FocusPosition.x + m_MapFocusOffset.x,
			m_FocusPosition.y + m_MapFocusOffset.y, true);
	}
	else
	{
		m_bFollowWithMap = false;
		SetFocusPosition(m_OrigFocusPosition.x + nOffsetX, m_OrigFocusPosition.y + nOffsetY , 0);
		m_bFollowWithMap = true;
	}
}

//获取场景的小地图信息
int KScenePlaceC::GetMapInfo(KSceneMapInfo* pInfo)
{
	RECT	MapRc;
	int nRet = m_Map.GetMapRect(&MapRc);
	if (pInfo)
	{
		int nHalfShowWidth  = m_RepresentExactHalfSize.cx * 2; //- SPWP_REPRESENT_RECT_WINDAGE_X
		int nHalfShowHeight = m_RepresentExactHalfSize.cy * 2; //- SPWP_REPRESENT_RECT_WINDAGE_T
		pInfo->nFocusMinH = MapRc.left + nHalfShowWidth;
		pInfo->nFocusMinV = MapRc.top  + nHalfShowHeight;
		pInfo->nFocusMaxH = MapRc.right  - nHalfShowWidth;
		pInfo->nFocusMaxV = MapRc.bottom - nHalfShowHeight;
		if (pInfo->nFocusMaxH < pInfo->nFocusMinH)
			pInfo->nFocusMaxH = pInfo->nFocusMinH;
		if (pInfo->nFocusMaxV < pInfo->nFocusMinV)
			pInfo->nFocusMaxV = pInfo->nFocusMinV;

		if (m_bFollowWithMap == false)
		{
			pInfo->nOrigFocusH = m_FocusPosition.x;
			pInfo->nOrigFocusV = m_FocusPosition.y;
			pInfo->nFocusOffsetH = m_MapFocusOffset.x;
			pInfo->nFocusOffsetV = m_MapFocusOffset.y;
		}
		else
		{
			pInfo->nOrigFocusH = m_OrigFocusPosition.x;
			pInfo->nOrigFocusV = m_OrigFocusPosition.y;
			pInfo->nFocusOffsetH = m_FocusPosition.x - m_OrigFocusPosition.x;
			pInfo->nFocusOffsetV = m_FocusPosition.y - m_OrigFocusPosition.y;
		}
		pInfo->nScallH = KScenePlaceMapC::MAP_SCALE_H;
		pInfo->nScallV = KScenePlaceMapC::MAP_SCALE_V;
	}
	return nRet;
}

//设置是否跟随地图的移动而移动
void  KScenePlaceC::FollowMapMove(int nbEnable)
{
	if ((!m_bFollowWithMap) != (!nbEnable))
	{
		if (m_bFollowWithMap = (nbEnable != 0))
		{
			m_OrigFocusPosition.x = m_FocusPosition.x;
			m_OrigFocusPosition.y = m_FocusPosition.y;
			int	x = m_MapFocusOffset.x;
			int y = m_MapFocusOffset.y;
			m_MapFocusOffset.x = 0;
			m_MapFocusOffset.y = 0;
			SetMapFocusPositionOffset(x, y);
		}
		else
		{
			m_MapFocusOffset.x = m_FocusPosition.x - m_OrigFocusPosition.x;
			m_MapFocusOffset.y = m_FocusPosition.y - m_OrigFocusPosition.y;
			SetFocusPosition(m_OrigFocusPosition.x, m_OrigFocusPosition.y, 0);
		}
	}
}

//获取场景的小地图信息
int KScenePlaceC::GetLittleMapInfo(KSceneMapInfo* pInfo)
{
	return m_Map.GetMapInfo(pInfo);
}

// <Add name="Adt.X" time="2005/10/13">
int KScenePlaceC::GetLittleMapRect(RECT* pRect)
{
	return m_Map.GetMapRect(pRect);
}
// </Add>

int KScenePlaceC::SetLittleMapFocusPosition(int nX, int nY)
{
	m_Map.SetFocusPosition(nX, nY, TRUE);
	return 1;
}

// --> Rocker Edit Start 2005/11/10
void KScenePlaceC::GetLittleMapCursorInfo(int nCursorX, int nCursorY, char* pInfo, int nInfoSize)
{
	m_Map.GetCursorInfo(nCursorX, nCursorY, pInfo, nInfoSize);
}
// <-- Rocker End

//////////////////////////////////////////////////////////////////////////
// Added By Rocker 2004.3.15
ImageBin::ImageBin()
{
	m_ImageIDList.clear();
}

ImageBin::~ImageBin()
{

}

void ImageBin::PreLoadImage(const char* pszImageFile,					// 图形资源字符串
							unsigned int nType,							// 图形资源类型ISI_T_BITMAP16, ISI_T_SPR
							MemoryResidentParam* pResidentParam)		// 图形资源驻留内存的参数
{
	if(g_pRepresent->PreLoadImage(pszImageFile, nType, pResidentParam))
	{
		unsigned int id = g_FileName2Id((char*)pszImageFile);
		m_ImageIDList.push_back(id);
	}
}

void ImageBin::FreeAll()
{
	for(int i=0; i<m_ImageIDList.size(); i++)
	{
		if (g_pRepresent)
			g_pRepresent->FreeImageByID(m_ImageIDList[i]);
	}

	m_ImageIDList.clear();
}
