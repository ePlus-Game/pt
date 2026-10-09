// ***************************************************************************************
// 场景地图的小地图
// Copyright : Kingsoft 2003
// Author    : wooy(wu yue)
// CreateTime: 2003-7-8
// ---------------------------------------------------------------------------------------
// 考虑效率因素，决定障碍小地图显示的范围要包含在场景地图架载的范围之中。场景模块架载地图的范围是
// 7*7=49个区域，障碍小地图的最大显示范围对应为6*6=36个区域。当小地图的缩小比率为16分之一的情况
// 下，则对应的小地图显示图形的大小就限定在不大于192*192像素。
// ***************************************************************************************
#pragma once
#include "UiImage.h"

class KAlphaBmp
{
public:
	KAlphaBmp();
	~KAlphaBmp();

	BOOL	Init(const char* pszFilePath);
	BYTE*	GetColorBuffer();
	BYTE*	GetAlphaBuffer();
	int		GetWidth();
	int		GetHeight();
	
	void	Blt(void* pSrcColor, int nSrcX, int nSrcY, int nSrcPitch, int nDstX, int nDstY, int nWidth, int nHeight);
	void	Draw(int nX, int nY, int nWidth, int nHeight);
private:
	
	BYTE*	m_pColorBuffer;
	BYTE*	m_pAlphaBuffer;
	int		m_nWidth;
	int		m_nHeight;

	char			m_szImageName[32];
	unsigned int	m_uImageId;
	short			m_nISPosition;
};

class KScenePlaceMapC
{
public:
	//对象运行时的一些参数
	enum SCENE_PLACE_MAP_WORK_PARAM_PUB
	{
        MAP_SCALE_H = 16,				//地图在横向上的比例
		MAP_SCALE_V = 32,				//地图在纵向上的比例
	};
	
	struct Rect
	{
		int x;
		int y;
		int width;
		int height;
	};

	char _mapName[COMMON_CLIENT_MSG_LEN_64];
public:
	KScenePlaceMapC();
	~KScenePlaceMapC();

	
	char* mapName() { return _mapName; }
	BOOL HaveSceneMap() { return m_bHaveSceneMap; }
	void Terminate();															//结束对象功能。释放对象的全部数据与动态构造的资源。
	void Load(KIniFile* pSetting, const char* pszScenePlaceRootPath, const char* mapName);			//读取设置
	void Free();																//清除
	void SetFocusPosition(int nX, int nY, bool bChangedRegion);					//设置场景地图的焦点
	void SetShowElemsFlag(unsigned int uShowElemsFlag);							//设置场景地图的包含的元素
	void SetSize(int cx, int cy);												//设置小地图的大小（单位：像素点）
	void gotoPosition(int x, int y);											//自动寻路到指定位置	
	int	GetMapRect(RECT* pRect);												//获取地图覆盖的范围,返回值表示是否有地图
	int GetMapInfo(KSceneMapInfo *pInfo);										//获得小地图的资讯，这是小地图的真实资讯，不使用场景坐标等
	void GetCursorInfo(int nCursorX, int nCursorY, char* pInfo, int nInfoSize); //获得鼠标信息
	int GetSceneWidth() const;													//获得场景宽度
	int GetSceneHeight() const;													//获得场景高度
	Rect GetSceneMapArea() const;												//获得场景地图区域
	RECT GetMiniMapArea() const;												//获得小地图区域
	Rect GetGridArea() const;													//获得场景地图的格子区域
	void PaintMiniMap(int nx, int ny, int nbShowCharacter = TRUE);				//绘制小地图
	void PaintMiniMapOnDC(ChatPoint& point,int nbShowCharacter);
	void PaintSceneMap(bool isCurrentMap ,int nbShowCharacter = TRUE, int nx = -1, int ny = -1);	//绘制场景地图
	bool IsHideSyncToWorldNpc(DWORD npcId) const;								//是否隐藏世界同步NPC
	bool IsMapInfoNpc(const char* szNpcName);									//是否是MapInfoNPC

	BOOL KScenePlaceMapC::isMapExist(const char* mapName);
private:
	bool Initialize();															//初始化
	void FillCellsPicInfo();
	void CalcPicLayout();

	void PaintMiniMapPic(int nX, int nY);										//绘制小地图背景图片
	void PaintMiniMapPicOnDC(int nX,int nY,unsigned long hDrawDC);
	void PaintMiniMapCharactersOnDC(ChatPoint& point);
	void PaintMiniMapCharacters(int nX, int nY);								//绘制小地图角色
	void PaintMiniMapAutorunDest(int nX, int nY);								//绘制小地图自动寻路目的地标识
	void PaintSceneMapPic(int nX, int nY);										//绘制场景地图背景图片
	void PaintSceneMapCharacters(int nX, int nY);								//绘制场景地图角色
	void PaintSceneMapNpcName(int nX, int nY);									//绘制场景地图角色
	void PaintSceneMapAutorunDest(int nX, int nY);								//绘制场景地图自动寻路目的地标识
	void PaintDirtyNpcSet(int nX, int nY);										//绘制脏NPC集（用于在显示建筑物等位置不易改变的NPC）
	void PaintSceneMapSyncToWorldNpc(int nX, int nY);							//绘制场景地图世界同步NPC

private:

	//对象运行时的一些参数(仅内部使用)
	enum SCENE_PLACE_MAP_WORK_PARAM
	{
		MAP_CELL_MAP_WIDTH = 128,		//地图块在地图中横向的宽度（单位：像素点）
		MAP_CELL_MAP_HEIGHT = 128,		//地图块在地图中纵向的宽度（单位：像素点）
		MAP_CELL_SCENE_WIDTH = 2048,	//地图块在实际场景中横向的宽度（单位：像素点）= MAP_SCALE_H * MAP_CELL_MAP_WIDTH
		MAP_CELL_SCENE_HEIGHT = 4096,	//地图块在实际场景中纵向的宽度（单位：像素点）= MAP_SCALE_V * MAP_CELL_MAP_HEIGHT
		MAP_CELL_NUM_REGION_H = 4,		//每个地图块在横向包含的场景区域的数目
		MAP_CELL_NUM_REGION_V = 4,		//每个地图块在纵向包含的场景区域的数目

 		MAP_CELL_MAX_RANGE = 5,			//模块中同时绘制的地图块的横/纵向最大跨度（单位：块）
		MAP_CELL_CENTRE_INDEX = 2,		//焦点所在的地图格的横或纵向索引

//		MAP_CELL_FOCUS_INDEX_MIN = 2,	//焦点所在的地图格的横或纵向索引许可最小值
//		MAP_CELL_FOCUS_INDEX_MAX = 2,	//焦点所在的地图格的横或纵向索引许可最大值

		MAP_MAX_SUPPORT_WIDTH  = (MAP_CELL_MAX_RANGE - 1) * MAP_CELL_MAP_WIDTH,		//小地图的最大支持显示宽（单位：像素点）
		MAP_MAX_SUPPORT_HEIGHT = (MAP_CELL_MAX_RANGE - 1) * MAP_CELL_MAP_HEIGHT,	//小地图的最大支持显示高（单位：像素点）

		MAP_A_REGION_NUM_MAP_PIXEL_H = 32,	//场景一个区域在地图上的横向跨度（单位：像素点）
		MAP_A_REGION_NUM_MAP_PIXEL_V = 32,	//场景一个区域在地图上的纵向跨度（单位：像素点）

	};

	struct	MAP_CELL
	{
		 char			szImageName[32];
		 unsigned int	uImageId;
		 short			sISPosition;
		 short			sReserved;
	};

private:
	int m_nSceneMapWidth;			//场景地图宽度
	int m_nSceneMapHeight;			//场景地图高度	
	Rect m_nSceneMapArea;			//场景地图区域

	int		m_bHavePicMap;			//此场景是否有所略图形式的小地图
	int		m_bHaveSceneMap;		//此场景是否有场景地图
	bool	m_bInited;				//是否执行过初始化
	
	char	m_szEntireMapFile[128];	//地图图文件名称
	char	m_szSceneMapFile[128];	//场景地图文件路径
	KSGImageContent*	m_pEntireMap;//完整的缩略地图

	POINT	m_EntireMapLTPosition;	//完整地图的左上角的场景坐标

	POINT	m_FocusPosition;		//焦点坐标
	POINT	m_PicLoadedLTPosition;	//缩略地图加载部分左上点(在整个场景中的对应)坐标
	RECT	m_FocusLimit;			//焦点位置的限定范围

	SIZE			m_Size;			//地图的显示大小
	unsigned int	m_uMapShowElems;//地图中要显示的内容
	MAP_CELL		m_ElemsList[MAP_CELL_MAX_RANGE][MAP_CELL_MAX_RANGE];

	//--绘制显示时参数预先计算与保留的--
	RECT		m_MapCoverArea;		//地图绘制部分覆盖的范围（以地图像素点为单位，场景坐标点0,0对应点为原点）
	RECT		m_PaintCell;			//要绘制的地图块索引范围
	SIZE		m_FirstCellSkipWidth;	//绘制的横/纵头一块要跳过的宽/高度
	SIZE		m_LastCellSkipHeight;	//绘制的横/纵最末那块的结束处的宽/高度

	unsigned int	m_uSelfColor;			// 主角颜色
	unsigned int	m_uTeammateColor;		// 队友颜色
	unsigned int	m_uPlayerColor;			// 其他玩家颜色
	unsigned int	m_uFightNpcColor;		// 战斗npc颜色
	unsigned int	m_uNormalNpcColor;		// 普通npc颜色

	Rect			m_curSceneMapGridArea;	//当前显示的地图的格子坐标范围
	KAlphaBmp		m_AlphaBmp;

	KUiImageRef		m_sceneMapSelfImage;
//	bool			m_bSmallMap;
};

inline int KScenePlaceMapC::GetSceneWidth() const
{
	return m_nSceneMapWidth;
}

inline int KScenePlaceMapC::GetSceneHeight() const
{
	return m_nSceneMapHeight;
}

inline KScenePlaceMapC::Rect KScenePlaceMapC::GetSceneMapArea() const
{
	return m_nSceneMapArea;
}

inline RECT KScenePlaceMapC::GetMiniMapArea() const
{
	return m_MapCoverArea;
}
