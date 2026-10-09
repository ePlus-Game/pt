#ifndef UIPATHHELP_H
#define UIPATHHELP_H
#include "CEGUI.h"
#include "../uicommon.h"
#include "CoreShell.h"
#include "GameDataDef.h"
#include "Ui/UiElem/TLVertScrollbar.h"
#include "ui/UiElem/TLCheckbox.h"
#include <vector>
#include "AutoTrust.h"
using std::vector;
#include <list>
using std::list;

#define UI_PATH_HELP_H "uisettings/layouts/pathHelp.ls"
#define UI_PATH_HELP_INI "uisettings/pathhelp.ini"
#define UI_START_PLACE_LIST_BOX_ITEM_NAME "TaharezLook/pathHelp/StartListBox/item"
#define UI_END_PLACE_LIST_BOX_ITEM_NAME "TaharezLook/pathHelp/EndListBox/item"

namespace ClientMapInfo
{

enum MapNpcType
{
		npcTyp_None=0,
		npcTyp_Sell_Medicne,
		npcTyp_Sell_Eq,
		npcTyp_Sell_Weapon,
};
struct MapNpcInfo
{
	MapNpcType type;
	int npcMapId;
	char npcName[AUTO_ITEM_NAME_LENGTH];
	int  npcPosX;
	int npcPosY;
	MapNpcInfo::MapNpcInfo()
	{
		type = npcTyp_None;
		npcMapId = 0;
		memset(npcName,0,AUTO_ITEM_NAME_LENGTH);
		npcPosX = 0;
		npcPosY = 0;
	}
	MapNpcInfo::MapNpcInfo(const MapNpcInfo& npc)
	{
		type = npc.type;
		npcMapId = npc.npcMapId;
		strcpy(npcName,npc.npcName);
		npcPosX = npc.npcPosX;
		npcPosY = npc.npcPosY;
	}
	const MapNpcInfo& operator =(const MapNpcInfo& npc)
	{
		type = npc.type;
		npcMapId = npc.npcMapId;
		strcpy(npcName,npc.npcName);
		npcPosX = npc.npcPosX;
		npcPosY = npc.npcPosY;
		return *this;
	}
};
struct AutoMapInfo
{
	int isHaveTypeNpc(MapNpcType type);
	AutoMapInfo();
	~AutoMapInfo();
	int mapId;//地图ID
	int carryNumbers;//传送点数目
	char mapName[AUTO_ITEM_NAME_LENGTH];//传送点名字
	vector<int> carryIndexList;//传送点索引
	int levelLimited ;//
	///////////////
	vector<MapNpcInfo> npcList;
//	vector<int> backCarryList;
};
struct CarryInfo
{
	CarryInfo();
	~CarryInfo();
	int carryID;
	char carryName[AUTO_ITEM_NAME_LENGTH];
	int levelLimited;
	int targetCarry;
	int currentMapIndex;
	int carryType;
	int carryPosX;
	int carryPosY;
};

class AutoGoBack
{
public:
	AutoGoBack();
	~AutoGoBack();
    AutoMapInfo* GetCurrentMapInfo();
    AutoMapInfo*       GetMapInfo(int id);
	const CarryInfo*   GetCarryInfo(int id);
	void               SetCurrentMapInfo(int id);
	void               SetCurrentMapInfo(const char* name);
	int                GetCurrentMapIndex() const { return currentMapID;}
	static AutoGoBack& Singleton();
	bool               GotoTargetMap(int mapId);
	bool               GotoSellItem();
	int           GetMapIndex(int mapId);
	bool FindPath(int srcMapID,int targetMapId);
private:
	struct pathInfo{
		int mapID;
		int parentMapID;
	};
	bool IsInPenList(int id);
	bool IsInCloseList(int id);
	bool FindPathInCloseList(int id,pathInfo& info);
	void CreateCarryPath();
public:
	vector<AutoMapInfo*> mapInfoList;
	vector<CarryInfo*>   carryInfoList;
	int currentMapID;
	list<pathInfo>               findOpenList;
	list<pathInfo>          findCloseList;
	list<int>               pathList;
	list<int>               mapPathList;
};
}
enum FindResultError
{
	ResultError_not_find = 0,
	ResultError_not_select_start_place,
	ResultError_not_select_end_place,
	ResultError_START_END_THE_SAME,
	ResultError_Success,
};
#define FIND_PATH_UI_START  0xff000000L
#define FIND_PATH_UI_END    0xfff00000L
class KUiPathHelp : public KUiWndSingleton<KUiPathHelp>
{
public:
	KUiPathHelp(const CEGUI::String& string_id_name);
	~KUiPathHelp();
	void    Init(void);
	static void    Show();
	static void    Hide();
private:
	void AddChildToListBox();

	bool HandleSelectedStartListChaoge               (const CEGUI::EventArgs& args);
	bool HandleSelectedStartListBeijiang             (const CEGUI::EventArgs& args);
	bool HandleSelectedStartListBeihai               (const CEGUI::EventArgs& args);
	bool HandleSelectedStartListNanman               (const CEGUI::EventArgs& args);
	bool HandleSelectedStartListXizhou               (const CEGUI::EventArgs& args);
	bool HandleSelectedStartListDonglu               (const CEGUI::EventArgs& args);
	bool HandleSelectedStartList                     (const CEGUI::EventArgs& args);
	void SetItemFunStart(Checkbox* pWindow,unsigned int id);
	void SetCheckedOnlyOneStart(unsigned int currentID);


	bool HandleSelectedEndListChaoge                 (const CEGUI::EventArgs& args);
	bool HandleSelectedEndListBeijiang               (const CEGUI::EventArgs& args);
	bool HandleSelectedEndListBeihai                 (const CEGUI::EventArgs& args);
	bool HandleSelectedEndListNanman                 (const CEGUI::EventArgs& args);
	bool HandleSelectedEndListXizhou                 (const CEGUI::EventArgs& args);
	bool HandleSelectedEndListDonglu                 (const CEGUI::EventArgs& args);
	bool HandleSelectedEndList                       (const CEGUI::EventArgs& args);
	void SetItemFunEnd(Checkbox* pWindow,unsigned int id);
	void SetCheckedOnlyOneEnd(unsigned int currentID);

	bool HandleOkButton(const CEGUI::EventArgs& args);
	void FindPath();
	void SetResultText(FindResultError errorMode);

	bool HandleClickResultText(const CEGUI::EventArgs& args);
	bool HandleCloseButton(const CEGUI::EventArgs& args);
	bool HandleStartScrollBarPos(const CEGUI::EventArgs& args);
	bool HandleEndScrollBarPos(const CEGUI::EventArgs& args);
	bool handleResultScrollBarPos(const CEGUI::EventArgs& args);
private:
	TLStaticText* pStartPlaceName;
	TLStaticText* pEndPlaceName;
	TLStaticText*   pResultText;

	TLVertScrollbar* pStartScrollbar;
	TLVertScrollbar* pEndScrollbar;
	TLVertScrollbar* pResultScrollBar;


	TLStaticImage*   pStartPlacListBox;
	TLStaticImage*   pEndPlaceListBox;
	TLStaticImage*   pResultBox;

	TLStaticText*   pResultPathText;
	PushButton*     pOkButton;


	vector<Window*> startPlaceItemList;
	vector<Window*> endPlaceItemList;

	char     pathResultNone[COMMON_CLIENT_MSG_LEN_256];
	char     pathResultNoStart[COMMON_CLIENT_MSG_LEN_256];
	char     pathResultNoEnd[COMMON_CLIENT_MSG_LEN_256];
	char     pathResultNotFind[COMMON_CLIENT_MSG_LEN_256];
	char     pathResultTheSame[COMMON_CLIENT_MSG_LEN_256];
	char     pathResultGotEnd[COMMON_CLIENT_MSG_LEN_256];
	
	char     showTextFormat[COMMON_CLIENT_MSG_LEN_256];
	char     showText[COMMON_CLIENT_MSG_LEN_256];
	char     showText1[COMMON_CLIENT_MSG_LEN_256];

	int      startListLength;
	int      endListLength;

	int      itemWidth,itemHeight,startPx,startPy;
	int     numberCheckedBoxes;

};
#endif