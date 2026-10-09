//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/18/2006 11:07
//      File_base        : UiMiniMap
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef UIMINIMAP_H
#define UIMINIMAP_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "GameDataDef.h"
#include "TLListbox.h"
#include "layoutinterface.h"
#include <vector>
#include <string>

using namespace CEGUI;
using namespace std;

class KUiMiniMap : public KUiWndSingleton<KUiMiniMap>
{
public:
	KUiMiniMap(  const String& id_name );
	~KUiMiniMap(								);
	static bool showPlayer;

public:
	static unsigned int PaintMiniMap( void );
	static void			Show		( void );
	static void			Hide		( void );
	void				Init		( void );
	int             GetMiniMapWndWidth();
	int             GetMiniMapWndHeight();
	int             GetMiniMapWndPaintX();
	int             GetMiniMapWndPaintY();
	Window*         GetParent() const {return m_pThisWnd;}
	bool onClickHidePlayer(const EventArgs& args);
	bool onGM(const EventArgs& args);
	bool onSearchTeam(const EventArgs& args);

private:
	Window*		d_miniMapWnd ;
	Window*		d_miniMapName;
	Window*		d_miniMapPos; 
	char		d_mapName[COMMON_CLIENT_MSG_LEN_32];

	TLButton*	m_NpcSearch;
	TLButton*	d_worldMap;
	TLButton*	d_sceneMap;
	TLButton*	d_hidePlayer;
	TLButton*	d_searchElf;
	TLButton*   d_pathHelp;

	void sendItemLink(const MapPosInfo& mapInfo);
protected:
	bool onClick(const EventArgs& args);
	bool onClickNpcSearch(const EventArgs & args);
	bool onClickWorldMap(const EventArgs& args);
	bool onClickSceneMap(const EventArgs& args);
	bool onClickPathHelpButton(const EventArgs& args);
	bool onSearchElf(const EventArgs& args);
};



/************************************************************************/
/*                            场景地图                                  */
/************************************************************************/

#define UI_SCENE_MAP_WINDOW_PATH "uisettings/layouts/SceneMap.ls"
#define UI_SCENE_MAP_WINDOW_PATH_1024 "uisettings/layouts1024/SceneMap.ls"
#define UI_SCENE_MAP_INVALID_INDEX -1

class KUiSceneMap
{
	char			_tipText[LAYOUT_TEXT_MAX_LEN];
	char			_curMapName[COMMON_CLIENT_MSG_LEN_32];
	char			m_srcMapName[COMMON_CLIENT_MSG_LEN_64];

	TLStaticImage*	_thisWindow;
	TLStaticText*	_postionText;
//	TLButton*		_mapListBtn;
//	TLListbox*		_mapList;

	Point			_mapCtrlOff;
	///////render add
	TLStaticText*   _currentMapName;

public:
	KUiSceneMap();
	~KUiSceneMap();
	
	bool	isVisible();
	void	showCurMap();
	void	showMap(char* mapName);
	void	hide();
	void	toggle();
	
	static KUiSceneMap& getSinglton()
	{
		static KUiSceneMap singlton;
		return singlton;
	}
protected:
	bool	onWindowOpen(const EventArgs& args);
	bool	onWindowClose(const EventArgs& args);

	bool	onMouseMove(const EventArgs& args);
	bool	onClickMap(const EventArgs& args);

	bool	onSelectOneMap(const EventArgs& args);
//	bool	onClickMapListBtn(const EventArgs& args);

	bool	onClickClose(const EventArgs& args);
private:
	void	load();
	void	sendItemLink(const MapPosInfo& mapInfo);
	
	void	setCurMap(const char* mapName, bool bShowChar);
};

#define UI_BIG_MAP_PART_NUM 8
#define UI_BIG_MAP_WINDOW_PATH "uisettings/layouts/BigMap.ls"
#define UI_BIG_MAP_WINDOW_PATH_1024 "uisettings/layouts1024/BigMap.ls"
#define UI_BIG_MAP_INVALID_INDEX -1




/************************************************************************/
/*                            世界地图                                  */
/************************************************************************/

class KUiBigMap
{
	TLStaticImage*	_thisWindow;
	TLStaticImage*	_areaPanel;
	TLStaticImage*	_part[UI_BIG_MAP_PART_NUM];
	TLStaticText*	_tipText;
	bool            b_bigMapShow;

	//地图列表
//	TLButton*		_mapListBtn;
//	TLListbox*		_mapList;
public:

	KUiBigMap();
	~KUiBigMap();

	void			show();
	void			hide();
	bool			isVisible();
	
	static KUiBigMap& getSinglton()
	{
		static KUiBigMap singlton;
		return singlton;
	}
	bool            isBigMapShow() const { return b_bigMapShow;}

protected:
	bool			onWindowOpen(const EventArgs& args);
	bool			onWindowClose(const EventArgs& args);

	bool			onTimer(const EventArgs& arg);
	bool			mouseClickAPart(const EventArgs& arg);
	bool			onClickClose(const EventArgs& arg);

	bool			onSelectOneMap(const EventArgs& args);
//	bool			onClickMapListBtn(const EventArgs& args);
private:
	void			load();
	const Image*	getNormalImage(const char* mapPartName);
	const Image*	getHoverImage(const char* mapPartName);
	const char*		getTip(const char* mapPartName);
};
#endif



