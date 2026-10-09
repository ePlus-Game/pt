//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 08/18/2006 11:06
//      File_base        : UiMiniMap
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "KWin32.h"
#include "KIniFile.h"
#include "KWin32Wnd.h"
#include "UiMapCentre.h"
#include "coreshell.h"
#include "gamedatadef.h"
#include "iRepresentShell.h"
#include "KRepresentUnit.h"
#include "../../Login/Login.h"
#include "vector"
#include "UiItemTip.h"
#include "../UiConfigManager.h"
#include "UiChatWindow.h"
#include "UiBubble.h"
#include "..\UiAdapter.h"
#include "..\UiSheetMgr.h"
#include "UiChatCentre.h"
#include "UiSearchHelpWnd.h"
#include "UiChangeMapWnd.h"
#include "UiGameSetting.h"
#include "../UiSheetMgr.h"
#include "UiErrorMessageBox.h"
#include "Ui/UiCase/UiPathHelp.h"
#include "chatWindow/ChatMiniMap.h"
#include "UiIBShop.h"
#include "UiCreditShop.h"
#include "UiElf.h"
//#include "UiCreditShop.h"
#include "UiNpcNavigation.h"
#include "UiGMCommunication.h"
#include "UiTeamViewer.h"
#include "UiQuestionWindow.h"
#include "UiRankButton.h"
using namespace std;

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*			g_pCoreShell;

using namespace CEGUI;

/************************************************************************/
/*                                                                      */
/************************************************************************/
template<> 
KUiMiniMap* KUiWndSingleton<KUiMiniMap>::ms_Singleton	= NULL;

KUiMiniMap::KUiMiniMap( const CEGUI::String& id_name ):
KUiWndSingleton<KUiMiniMap>( id_name )
{
	ZeroMemory( &d_mapName, COMMON_CLIENT_MSG_LEN_32*sizeof( char ) );
}
bool KUiMiniMap::showPlayer = false;
KUiMiniMap::~KUiMiniMap()
{

}

unsigned int KUiMiniMap::PaintMiniMap( void )
{
	if ( g_LoginLogic.GetStatus() == LL_S_IN_GAME && ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		Point absPoint = ms_Singleton->d_miniMapWnd->getAbsolutePosition();
		Point absFPoint	 = ms_Singleton->m_pThisWnd->getAbsolutePosition();
//		ChatMiniMap::GetSingle().SetPaintPos(absPoint.d_x,absPoint.d_y);
		//		ChatMiniMap::GetSingle().Update();
		if(ms_Singleton->m_pThisWnd->isVisible()
			&& !KUiSceneMap::getSinglton().isVisible()
			&& !KUiBigMap::getSinglton().isVisible())
		{
			g_pCoreShell->SceneMapOperation(GSMOI_PAINT_MINI_MAP, absFPoint.d_x + absPoint.d_x, absFPoint.d_y + absPoint.d_y );
		}
		
		KUiSceneTimeInfo tagSceneMapTime = { 0 };
		
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&tagSceneMapTime, NULL );
		tagSceneMapTime.szSceneName[COMMON_CLIENT_MSG_LEN_32-1] = 0;
		
		memcpy( ms_Singleton->d_mapName, tagSceneMapTime.szSceneName, COMMON_CLIENT_MSG_LEN_32*sizeof(char) );
		ms_Singleton->d_mapName[COMMON_CLIENT_MSG_LEN_32-1] = 0;
		char szBuf[COMMON_CLIENT_MSG_LEN_32];
		sprintf( szBuf, "(%d,%d)", tagSceneMapTime.nScenePos0, tagSceneMapTime.nScenePos1 );
		String mapName = AnsiToUtf8( ms_Singleton->d_mapName );
		//mapName += szBuf;
		ms_Singleton->d_miniMapName->setText( mapName );			
		ms_Singleton->d_miniMapPos->setText(szBuf);
	}

	if(g_LoginLogic.GetStatus() == LL_S_IN_GAME && KUiSceneMap::getSinglton().isVisible())
	{
		g_pCoreShell->SceneMapOperation(GSMOI_PAINT_SCENE_MAP, NULL, NULL );
	}

	return 0;
}

int KUiMiniMap::GetMiniMapWndWidth()
{
	return (int)d_miniMapWnd->getAbsoluteWidth();
}
int KUiMiniMap::GetMiniMapWndHeight()
{
	return (int)d_miniMapWnd->getAbsoluteHeight();
}
int KUiMiniMap::GetMiniMapWndPaintX()
{
	Point absPoint = ms_Singleton->d_miniMapWnd->getAbsolutePosition();
	Point absFPoint	 = ms_Singleton->m_pThisWnd->getAbsolutePosition();
	return absFPoint.d_x + absPoint.d_x;
}
int KUiMiniMap::GetMiniMapWndPaintY()
{
	Point absPoint = ms_Singleton->d_miniMapWnd->getAbsolutePosition();
	Point absFPoint	 = ms_Singleton->m_pThisWnd->getAbsolutePosition();
	return absFPoint.d_y + absPoint.d_y;
}
void KUiMiniMap::Init( void )
{
	if(NULL == m_pThisWnd)
	{
		return;
	}
	
	//face设置在supperbottom层
	m_pThisWnd->setZLevel(Window::SuperBottom);
	// Do events wire-up
	Point absPoint = m_pThisWnd->getAbsolutePosition();
	Size absSize = m_pThisWnd->getAbsoluteSize();
	absPoint.d_x = g_GetScreenWidth() - absSize.d_width;
	m_pThisWnd->setPosition( Absolute, absPoint );
	d_miniMapWnd = m_pThisWnd->getChild("TaharezLook/MiniMap/MiniMapWnd");
	d_miniMapName = m_pThisWnd->getChild("TaharezLook/MiniMap/MiniMapName");
	d_miniMapPos = m_pThisWnd->getChild("TaharezLook/MiniMap/MiniMapPos");

	d_miniMapWnd->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMiniMap::onClick, this));
	
	d_worldMap = (TLButton*)m_pThisWnd->getChild("TaharezLook/MiniMap/world");
	d_sceneMap = (TLButton*)m_pThisWnd->getChild("TaharezLook/MiniMap/area");
	d_hidePlayer = (TLButton*)m_pThisWnd->getChild("TaharezLook/MiniMap/HidePlayer");
	d_searchElf = (TLButton*)m_pThisWnd->getChild("TaharezLook/MiniMap/find");
	d_pathHelp = (TLButton*)m_pThisWnd->getChild("TaharezLook/MiniMap/pathHelp");
	Window* gmBtn = (TLButton*)m_pThisWnd->getChild("TaharezLook/MiniMap/GM");
	Window* searchTeam = (TLButton*)m_pThisWnd->getChild("TaharezLook/MiniMap/SearchTeam");
	d_pathHelp->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMiniMap::onClickPathHelpButton, this));
	d_worldMap->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMiniMap::onClickWorldMap, this));
	d_sceneMap->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMiniMap::onClickSceneMap, this));
	d_hidePlayer->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMiniMap::onClickHidePlayer, this));
	gmBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMiniMap::onGM, this));
	searchTeam->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMiniMap::onSearchTeam, this));
	d_searchElf->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiMiniMap::onSearchElf, this));
}

void KUiMiniMap::Show()
{
	KUiWndSingleton<KUiMiniMap>::Show();
	KUiRankButton::GetSingleton().MoveToRightEdge();
	KUiIBNavigation::GetSingleton().MoveToRightEdge();
	KUiCreditShopNavigation::GetSingleton().MoveToRightEdge();
	KUiElf::GetSingleton().MoveToRightEdge();
	if (g_pCoreShell)
	{
		Size absSize = ms_Singleton->m_pThisWnd->getChild("TaharezLook/MiniMap/MiniMapWnd")->getAbsoluteSize();
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
			SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
			( (int)(absSize.d_width)| ((int)(absSize.d_height) << 16)));	
	}
}

void KUiMiniMap::Hide()
{
	KUiWndSingleton<KUiMiniMap>::Hide();
	KUiIBNavigation::GetSingleton().MoveToRightEdge();
	KUiCreditShopNavigation::GetSingleton().MoveToRightEdge();
	KUiElf::GetSingleton().MoveToRightEdge();
	KUiRankButton::GetSingleton().MoveToRightEdge();
//	if (g_pCoreShell)
//		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING, SCENE_PLACE_MAP_ELEM_NONE, 0);
}

bool KUiMiniMap::onClick(const CEGUI::EventArgs& args)
{
	if(!m_pThisWnd->isVisible())
		return false;
	
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	Point dest = mouse->position - d_miniMapWnd->getUnclippedPixelRect().getPosition();

	//先显示新位置
	TLStaticImage* posImage = (TLStaticImage*)d_miniMapWnd->getChild("TaharezLook/MiniMap/MiniMapWnd/MousePos");
	posImage->setPosition(Absolute, dest - Point(posImage->getWidth(Absolute) / 2, posImage->getHeight(Absolute) / 2));
	posImage->play();

	Position destPos;
	destPos.x = dest.d_x;
	destPos.y = dest.d_y;

	MapPosInfo mapInfo;
	g_pCoreShell->SceneMapOperation(GSMOI_GET_MAP_INFO_AT_MINI_POS, (unsigned)&mapInfo, (int)&destPos);

	KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
	if(mouse->sysKeys & Control && KUiChatInputWnd::IsVisible() || (pRoom && pRoom->IsVisible() && (mouse->sysKeys & Control)))
	{
		if(g_pCoreShell->GetGameData(GDI_CAN_GOTO_POS, (unsigned)mapInfo.pos.x, (int)mapInfo.pos.y * 2))
		{
			sendItemLink(mapInfo);
		}
		else
		{
			KUiChannelCentre::GetSingleton().toSysMsg(MSG_AUTORUN_NOWAY);
		}
	}
	else
	{
		g_pCoreShell->OperationRequest( GOI_GOTO_POS, (unsigned)mapInfo.pos.x, (int)mapInfo.pos.y * 2);
	}
	mouse->handled = true;
	return true;
}

bool KUiMiniMap::onClickWorldMap(const EventArgs& args)
{
	KUiBigMap::getSinglton().show();	
	return true;
}

bool KUiMiniMap::onClickSceneMap(const EventArgs& args)
{
	KUiSceneMap::getSinglton().toggle();
	return true;
}

bool KUiMiniMap::onClickPathHelpButton(const EventArgs& args)
{
	if(KUiPathHelp::GetSingleton().IsVisible())
		KUiPathHelp::Hide();
	else
		KUiPathHelp::Show();
	return true;
}
bool KUiMiniMap::onClickHidePlayer(const EventArgs& args)
{
	g_pCoreShell->OperationRequest(GOI_SHOW_PLAYERS_BODY, 0, showPlayer);
	showPlayer = !showPlayer;
	if ( d_hidePlayer )
	{
		((RadioButton*)d_hidePlayer)->setSelected( !showPlayer );
	}	
	ChatMiniMap::isShowPlayer = showPlayer;
	return true;
}

bool KUiMiniMap::onSearchElf(const EventArgs& args)
{
// 	if(KUiSearchHelpWnd::GetSingleton().IsSearchVisible())
// 	{
// 		KUiSearchHelpWnd::Hide();
// 	}
// 	else
// 	{
// 		KUiSearchHelpWnd::GetSingleton().ShowSearchHelp();
// 	}
	if ( KUiNpcNavigation::getSingleton().isVisible() )
	{
		KUiNpcNavigation::getSingleton().hide();
	}
	else
	{
		KUiNpcNavigation::getSingleton().toggle();
	}
	

// 	if ( KUiCreditShop::IsVisible() )
// 	{
// 		KUiCreditShop::Hide();
// 	}
// 	else
// 	{
// 		KUiCreditShop::Show();
// 	}
	return true;
}

bool KUiMiniMap::onGM(const EventArgs& args)
{
	if(!KUiGMCommunication::getSingleton().isVisible())
	{
		KUiGMCommunication::getSingleton().setYourMsg("");
		KUiGMCommunication::getSingleton().show();
		KUiGMCommunication::getSingleton().showCommit();
	}
	else
	{
		KUiGMCommunication::getSingleton().hide();
	}

	return true;
}

bool KUiMiniMap::onSearchTeam(const EventArgs& args)
{
	KUiTeamViewer::ToggleVisibility();
	return true;
}

void KUiMiniMap::sendItemLink(const MapPosInfo& mapInfo)
{	
	LOElemInfo itemElem;
	wchar_t* itemContent = NULL;
	char linkName[COMMON_CLIENT_MSG_LEN_128];

	sprintf(linkName, "%s(%d,%d)", mapInfo.mapName, mapInfo.pos.x, mapInfo.pos.y);
	ansiToUnicode(linkName, itemContent);
	wchar_t itemDescription[COMMON_CLIENT_MSG_LEN_64];
	itemDescription[0] = 0;
	wcscat(itemDescription, L"[");
	wcscat(itemDescription, itemContent);
	wcscat(itemDescription, L"]");
	
	itemElem.elemType = LO_TEXT;
	itemElem.isShowDes = true;
	itemElem.gameObj._objType = LO_GO_POSITION;
	itemElem.gameObj._objId[0] = mapInfo.mapId;
	itemElem.gameObj._objId[1] = mapInfo.pos.x;
	itemElem.gameObj._objId[2] = mapInfo.pos.y;
	itemElem.content = itemContent;
	itemElem.description = itemDescription;
	KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
	if ( pRoom && pRoom->IsVisible() )
	{
		pRoom->write(itemElem);
	}
	else
	{
		KUiChatInputWnd::GetSingleton().write(itemElem);
		KUiChatInputWnd::GetSingleton().show();
	}
	
	
	delete[] itemContent;
	itemContent = NULL;
	
	
}


/************************************************************************/
/*                            场景地图                                  */
/************************************************************************/

KUiSceneMap::KUiSceneMap()
{
	load();
	m_srcMapName[0] = 0;
	_curMapName[0] = 0;
}

KUiSceneMap::~KUiSceneMap()
{

}

void KUiSceneMap::load()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SCENE_MAP_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_SCENE_MAP_WINDOW_PATH);
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif
	
//	_thisWindow->setRenderMode(false,2);
//	_thisWindow->setZLevel(Window::t)
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	_thisWindow->hide();
	
	_thisWindow->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiSceneMap::onWindowOpen, this));
	_thisWindow->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiSceneMap::onWindowClose, this));
	_thisWindow->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiSceneMap::onClickMap, this));
	_thisWindow->subscribeEvent(Window::EventMouseMove, Event::Subscriber(&KUiSceneMap::onMouseMove, this));
	
	_postionText = (TLStaticText*)_thisWindow->getChild("TaharezLook/SceneMap/Position");
	_currentMapName = (TLStaticText*)_thisWindow->getChild("TaharezLook/SceneMap/SceneMapName");

//	_mapList = (TLListbox*)_thisWindow->getChild("TaharezLook/SceneMap/MapList");
	const KUiCfgLoader::SceneMapCfg& mapCfg = KUiCfgLoader::getSingleton().getSceneMapCfg();
//	for(int i = 0; i < mapCfg.mapList.size(); ++i)
//	{
//		const char* mapName = mapCfg.mapList[i].text;
//		ListboxTextItem* item = new ListboxTextItem(AnsiToUtf8(mapName));
//		_mapList->addItem(item);
//	}
//	_mapList->subscribeEvent(Window::EventSelectionChanged, Event::Subscriber(&KUiSceneMap::onSelectOneMap, this));
//	_mapList->hide();

//	_mapListBtn = (TLButton*)_thisWindow->getChild("TaharezLook/SceneMap/MapListBtn");
//	_mapListBtn->disable();
//	_mapListBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiSceneMap::onClickMapListBtn, this));
	
	_thisWindow->getChild("TaharezLook/SceneMap/Close")->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiSceneMap::onClickClose, this));

	//场景地图挂在sheet2
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT_2)->addChildWindow(_thisWindow);
}

void KUiSceneMap::showCurMap()
{
	if(!_thisWindow)
	{
		return;
	}

	MapPosInfo mapInfo;
	g_pCoreShell->SceneMapOperation(GSMOI_GET_MAP_INFO_AT_SCENE_POS, (unsigned)&mapInfo, (int)&Position());

	//当前地图名
	char* curMapName = mapInfo.mapName;
	
	showMap(curMapName);
}

void KUiSceneMap::showMap(char* mapName)
{
	if(!_thisWindow || KUiQuestionWindow::IsVisible())
	{
		return;
	}
	
	if(g_pCoreShell->SceneMapOperation(GSMOI_HAVE_SCENE_MAP, (unsigned int)mapName, NULL))
	{	
		//控件相对于地图的偏移（负数）
		int mapWidth	= g_pCoreShell->SceneMapOperation(GSMOI_GET_SCENE_MAP_WIDTH, NULL, NULL);
		int mapHeight	= g_pCoreShell->SceneMapOperation(GSMOI_GET_SCENE_MAP_HEIGHT, NULL, NULL);
		_mapCtrlOff.d_x = _thisWindow->getPosition(Absolute).d_x - (g_GetScreenWidth() - mapWidth) * 0.5;
		_mapCtrlOff.d_y = _thisWindow->getPosition(Absolute).d_y - (g_GetScreenHeight() - mapHeight) * 0.5;


		g_pCoreShell->SceneMapOperation(GSMOI_SHOW_SCENE_MAP, NULL, NULL);

		//打开的时候会显示当前地图，而不是上一次关闭之前的地图
		setCurMap(mapName, true);

		_thisWindow->show();
	}
	else
	{
		if(KUiBigMap::getSinglton().isVisible())
		{
			KUiBigMap::getSinglton().hide();
		}
		else
		{
			KUiBigMap::getSinglton().show();
		}
	}
}

void KUiSceneMap::hide()
{	
	if(!_thisWindow)
	{
		return;
	}
	MapPosInfo mapInfo;
	g_pCoreShell->SceneMapOperation(GSMOI_GET_MAP_INFO_AT_SCENE_POS, (unsigned)&mapInfo, (int)&Position());
	setCurMap(mapInfo.mapName, true);
	g_pCoreShell->SceneMapOperation(GSMOI_HIDE_SCENE_MAP, NULL, NULL);
	_thisWindow->hide();
//	KUiMiniMap::Show();
}

bool KUiSceneMap::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}
	return _thisWindow->isVisible();
}

void KUiSceneMap::toggle()
{
	if(!_thisWindow)
	{
		return;
	}

	int bShow = g_pCoreShell->SceneMapOperation( GSMOI_IS_SCENE_MAP_SHOW, NULL, NULL );
	if(bShow > 0)
	{
		hide();
		KUiItemTip::Hide();
	}
	else
	{
		if(KUiChangeMapWnd::IsVisible())
		{
			return;
		}
		showCurMap();
		KUiAdapter::UiCloseNoNpcDlg();
	}
}

bool KUiSceneMap::onWindowOpen(const EventArgs& args)
{
	KUiBigMap::getSinglton().hide();
	KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT_2);
	return true;
}

bool KUiSceneMap::onWindowClose(const EventArgs& args)
{
	KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT);
	if(g_pCoreShell)
	{
		Size absSize = KUiMiniMap::GetSingleton().GetParent()->getChild("TaharezLook/MiniMap/MiniMapWnd")->getAbsoluteSize();
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
		SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
				( (int)(absSize.d_width)| ((int)(absSize.d_height) << 16)));
	}
	return true;
}

bool KUiSceneMap::onClickMap(const CEGUI::EventArgs& args)
{	
	if(!_thisWindow->isVisible())
		return false;
	
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	
	if(RightButton == mouse->button)
	{
		hide();
		KUiBigMap::getSinglton().show();
		return true;
	}

	//检查是不是当前地图///////////////
	MapPosInfo mapInfo;
	g_pCoreShell->SceneMapOperation(GSMOI_GET_MAP_INFO_AT_SCENE_POS, (unsigned)&mapInfo, (int)&Position());
	if(strcmp(mapInfo.mapName,_curMapName)!=0)
	{
		Point dest = mouse->position - _thisWindow->getPosition(Absolute);
		
		//先显示新位置
		TLStaticImage* posImage = (TLStaticImage*)_thisWindow->getChild("TaharezLook/SceneMap/MousePos");
		posImage->setPosition(Absolute, dest - Point(posImage->getWidth(Absolute) / 2, posImage->getHeight(Absolute) / 2));
		posImage->play();
		
		dest += _mapCtrlOff;
		
		Position destPos;
		destPos.x = dest.d_x;
		destPos.y = dest.d_y;
		KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
		if(mouse->sysKeys & Control || (pRoom && pRoom->IsVisible() && (mouse->sysKeys & Control)))
		{
			KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(MSG_CANT_AUTORUN));
		}
		else
		{
			strcpy(mapInfo.mapName, _curMapName);
			Position destPos;
			destPos.x = dest.d_x;
			destPos.y = dest.d_y;
			vector<string> npcNames;
			g_pCoreShell->SceneMapOperation(GSMOI_GET_PLAYER_NAME_POS_AT_POS, (unsigned)&npcNames, (int)&destPos);
			mapInfo.pos.x = destPos.x;
			mapInfo.pos.y = destPos.y;
			if (0 == g_pCoreShell->OperationRequest(GOI_AUTO_FIND_MAP_WAY, (unsigned int)&mapInfo, 0))
			{
				KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(MSG_AUTORUN_NOWAY));
			}
		}
		
		mouse->handled = true;
		return true;
	}
	else
	{
		Point dest = mouse->position - _thisWindow->getPosition(Absolute);

		//先显示新位置
		TLStaticImage* posImage = (TLStaticImage*)_thisWindow->getChild("TaharezLook/SceneMap/MousePos");
		posImage->setPosition(Absolute, dest - Point(posImage->getWidth(Absolute) / 2, posImage->getHeight(Absolute) / 2));
		posImage->play();

		dest += _mapCtrlOff;

		Position destPos;
		destPos.x = dest.d_x;
		destPos.y = dest.d_y;
		KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
		if(mouse->sysKeys & Control || (pRoom && pRoom->IsVisible() && (mouse->sysKeys & Control)))
		{
			MapPosInfo mapInfo;
			g_pCoreShell->SceneMapOperation(GSMOI_GET_MAP_INFO_AT_SCENE_POS, (unsigned)&mapInfo, (int)&destPos);
			
			if(g_pCoreShell->GetGameData(GDI_CAN_GOTO_POS, (unsigned)mapInfo.pos.x, (int)mapInfo.pos.y * 2))
			{
				sendItemLink(mapInfo);
				hide();
			}
			else
			{
				KUiErrorMessageBox::GetSingleton().AddMessage(AnsiToUtf8(MSG_AUTORUN_NOWAY));
			}
		}
		else
		{
			g_pCoreShell->SceneMapOperation(GSMOI_GO_TO_POS, (unsigned)dest.d_x, (int)dest.d_y);
		}
		
		mouse->handled = true;
		return true;
	}
}

void KUiSceneMap::sendItemLink(const MapPosInfo& mapInfo)
{	
	if(!_thisWindow)
	{
		return;
	}

	LOElemInfo itemElem;
	wchar_t* itemContent = NULL;
	char linkName[COMMON_CLIENT_MSG_LEN_128];

	sprintf(linkName, "%s(%d,%d)", mapInfo.mapName, mapInfo.pos.x, mapInfo.pos.y);
	ansiToUnicode(linkName, itemContent);
	wchar_t itemDescription[COMMON_CLIENT_MSG_LEN_64];
	itemDescription[0] = 0;
	wcscat(itemDescription, L"[");
	wcscat(itemDescription, itemContent);
	wcscat(itemDescription, L"]");
	
	itemElem.elemType = LO_TEXT;
	itemElem.isShowDes = true;
	itemElem.gameObj._objType = LO_GO_POSITION;
	itemElem.gameObj._objId[0] = mapInfo.mapId;
	itemElem.gameObj._objId[1] = mapInfo.pos.x;
	itemElem.gameObj._objId[2] = mapInfo.pos.y;
	itemElem.content = itemContent;
	itemElem.description = itemDescription;
	KUiChatInputWnd::GetSingleton().write(itemElem);
	
	delete[] itemContent;
	itemContent = NULL;
	
	KUiChatInputWnd::GetSingleton().show();
}

bool KUiSceneMap::onMouseMove(const CEGUI::EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;

	Point dest = mouse->position - _thisWindow->getPosition(Absolute) + _mapCtrlOff;

	Position destPos;
	destPos.x = dest.d_x;
	destPos.y = dest.d_y;
	vector<string> npcNames;
	g_pCoreShell->SceneMapOperation(GSMOI_GET_PLAYER_NAME_POS_AT_POS, (unsigned)&npcNames, (int)&destPos);

	//显示鼠标坐标的代码
	static char posText[COMMON_CLIENT_MSG_LEN_64];
	sprintf(posText, "x:%d , y:%d", destPos.x, destPos.y);
	//为了效率，英文就不转UTF8了
	_postionText->setText(posText);

	if(npcNames.size() == 0)
	{
		KUiItemTip::Hide();
		return true;
	}

	sprintf(_tipText, "<Layout width=150 margin-top=%d margin-left=%d margin-right=%d margin-bottom=%d>", 
		KUiCfgLoader::getSingleton().getSceneMapCfg().topMargin,
		KUiCfgLoader::getSingleton().getSceneMapCfg().leftMargin,
		KUiCfgLoader::getSingleton().getSceneMapCfg().RightMargin,
		KUiCfgLoader::getSingleton().getSceneMapCfg().bottomMargin);

	for(int i = 0; i < npcNames.size(); ++i)
	{
		strcat(_tipText, "<Seg float=wrap><Obj color=");
		strcat(_tipText, KUiCfgLoader::getSingleton().getSceneMapCfg().tipColor);
		strcat(_tipText, " font-family=");
		strcat(_tipText, KUiCfgLoader::getSingleton().getSceneMapCfg().tipFont);
		strcat(_tipText, ">");
		strcat(_tipText, npcNames[i].c_str());
		strcat(_tipText, "</Obj></Seg>");
	}
	//坐标
	strcat(_tipText, "<Seg float=wrap><Obj color=");
	strcat(_tipText, KUiCfgLoader::getSingleton().getSceneMapCfg().tipColor);
	strcat(_tipText, " font-family=");
	strcat(_tipText, KUiCfgLoader::getSingleton().getSceneMapCfg().tipFont);
	strcat(_tipText, ">");
	char tempText[COMMON_CLIENT_MSG_LEN_32];
	sprintf(tempText, "(%d,%d)", destPos.x, destPos.y);
	strcat(_tipText, tempText);
	strcat(_tipText, "</Obj></Seg>");
	strcat(_tipText, "</Layout>");
	KUiItemTip::GetSingleton();
	KUiItemTip::GetSingleton().show(_tipText, Rect(mouse->position, Size(1, 1)));

	mouse->handled = true;
	return true;
}

/*bool KUiSceneMap::onSelectOneMap(const EventArgs& args)
{
//	_mapList->hide();

//	ListboxItem* item = _mapList->getFirstSelectedItem();
//	if(NULL == item)
//	{
//		return false;
//	}
//	int index = _mapList->getItemIndex(item);

//	const KUiCfgLoader::SceneMapCfg& mapCfg = KUiCfgLoader::getSingleton().getSceneMapCfg();

//	setCurMap(mapCfg.mapList[index].name, false);

//	return true;
}*/

bool KUiSceneMap::onClickClose(const EventArgs& args)
{
	hide();
	if(KUiBigMap::getSinglton().isBigMapShow())
	{
		KUiBigMap::getSinglton().show();
	}
	return true;
}

/*bool KUiSceneMap::onClickMapListBtn(const EventArgs& args)
{
	if(_mapList->isVisible())
	{
		_mapList->hide();
	}
	else
	{
		_mapList->show();
	}
	return true;
}*/

void KUiSceneMap::setCurMap(const char* mapName, bool bShowChar )
{	
	if(!_thisWindow)
	{
		return;
	}
	strcpy(_curMapName, mapName);
	int rst = g_pCoreShell->SceneMapOperation(GSMOI_SCENE_CHANGE_MAP, (unsigned int)mapName, bShowChar);
	if(!rst)
	{
		return;
	}
	_currentMapName->setText(AnsiToUtf8(mapName));
	//修改选择地图的按钮文字为当前显示地图的名字
/*	const KUiCfgLoader::SceneMapCfg& mapCfg = KUiCfgLoader::getSingleton().getSceneMapCfg();
	for(int i = 0; i < mapCfg.mapList.size(); ++i)
	{
		if(!strcmp(mapName, mapCfg.mapList[i].name))
		{
			_mapListBtn->setText(AnsiToUtf8(mapCfg.mapList[i].text));
			break;
		}
	}*/
}



/************************************************************************/
/*                            世界地图                                  */
/************************************************************************/


KUiBigMap::KUiBigMap()
{
	load();
}

KUiBigMap::~KUiBigMap()
{

}

void KUiBigMap::show()
{
	if(!_thisWindow || KUiQuestionWindow::IsVisible())
	{
		return;
	}
	_thisWindow->show();
	_tipText->hide();
//	_mapList->hide();
}

void KUiBigMap::hide()
{	
	if(!_thisWindow)
	{
		return;
	}
	_thisWindow->hide();
}

bool KUiBigMap::isVisible()
{
	if(!_thisWindow)
	{
		return false;
	}
	return _thisWindow->isVisible();
}

bool KUiBigMap::onWindowOpen(const EventArgs& args)
{
	b_bigMapShow = true;
	_thisWindow->beginUpdate();
	if(KUiSceneMap::getSinglton().isVisible())
	{
		KUiSceneMap::getSinglton().hide();
	}
	KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT_2);
	return true;
}

bool KUiBigMap::onWindowClose(const EventArgs& args)
{
	b_bigMapShow = false;
	_thisWindow->stopUpdate();
	KUiSheetMgr::getSinglton().switchSheet(UI_DEFAULT_GUISHEET_ROOT);
	return true;
}

void KUiBigMap::load()
{
#ifndef _DEBUG
	try
	{
#endif
		if(g_GetScreenWidth() == 1024 && g_GetScreenHeight() == 768)
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_BIG_MAP_WINDOW_PATH_1024);
		}
		else
		{
			_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_BIG_MAP_WINDOW_PATH);
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif

	_thisWindow->setZLevel(Window::SuperTop);

	_thisWindow->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiBigMap::onWindowOpen, this));
	_thisWindow->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiBigMap::onWindowClose, this));

	_areaPanel = (TLStaticImage*)_thisWindow->getChild("TaharezLook/BigMap/Area");
	_thisWindow->subscribeEvent(Window::EventNewFrame, Event::Subscriber(&KUiBigMap::onTimer, this));

		
	for(int i = 0; i < UI_BIG_MAP_PART_NUM; ++i)
	{
		_part[i] = (TLStaticImage*)_areaPanel->getChild(String("TaharezLook/BigMap/Area/Part") + PropertyHelper::intToString(i + 1));
		_part[i]->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&KUiBigMap::mouseClickAPart, this));
		const char* mapName = Utf8ToAnsi(_part[i]->getText());
		_part[i]->setImage(getNormalImage(mapName));
	}

	_tipText = (TLStaticText*)_thisWindow->getChild("TaharezLook/BigMap/TipText");
	_tipText->useLayout();
	_tipText->setZLevel(Window::Top);
	b_bigMapShow = false;

/*	_mapList = (TLListbox*)_thisWindow->getChild("TaharezLook/BigMap/MapList");
	const KUiCfgLoader::SceneMapCfg& mapCfg = KUiCfgLoader::getSingleton().getSceneMapCfg();
	for(int mapIndex = 0; mapIndex < mapCfg.mapList.size(); ++mapIndex)
	{
		const char* mapName = mapCfg.mapList[mapIndex].text;
		ListboxTextItem* item = new ListboxTextItem(AnsiToUtf8(mapName));
//		_mapList->addItem(item);
	}
	_mapList->subscribeEvent(Window::EventSelectionChanged, Event::Subscriber(&KUiBigMap::onSelectOneMap, this));
	_mapList->setZLevel(Window::Top);

	_mapListBtn = (TLButton*)_thisWindow->getChild("TaharezLook/BigMap/MapListBtn");
	_mapListBtn->hide();
	_mapListBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiBigMap::onClickMapListBtn, this));*/

	TLButton* closeBtn = (TLButton*)_thisWindow->getChild("TaharezLook/BigMap/Close");
	closeBtn->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiBigMap::onClickClose, this));

	_thisWindow->hide();
	
	//场景地图挂在sheet2
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT_2)->addChildWindow(_thisWindow);
}

bool KUiBigMap::onClickClose(const EventArgs& arg)
{
	_thisWindow->hide();
	return true;
}

bool KUiBigMap::onTimer(const EventArgs& arg)
{
//  	FrameEventArgs* timeEvent = (FrameEventArgs*)&arg;
//  	static int elapse = 0;
//  	elapse += timeEvent->elapse;
//  	if(elapse < 100)
//  	{
//  		return true;
//  	}
//  	elapse = 0;

	Point mouse = MouseCursor::getSingleton().getPosition();
	const char* tipText = NULL;
	Window*	hoverCtrl = NULL;
	for(int i = 0; i < UI_BIG_MAP_PART_NUM; ++i)
	{
		TLStaticImage* hitCtrl = (TLStaticImage*)_part[i];

		Rect ctrlArea = hitCtrl->getUnclippedPixelRect();
		if(!ctrlArea.isPointInRect(mouse))
		{
			const char* mapName = Utf8ToAnsi(hitCtrl->getText());
			hitCtrl->setImage(getNormalImage(mapName));
			_tipText->hide();
			continue;
		}

		Point ctrlPos = ctrlArea.getPosition();
		int alpha = hitCtrl->getAlphaAtPixel(0, mouse.d_x - ctrlPos.d_x, mouse.d_y - ctrlPos.d_y);
		if(alpha > 0)
		{
			const char* mapName = Utf8ToAnsi(hitCtrl->getText());
			hitCtrl->setImage(getHoverImage(mapName));

			static string lastMapName = mapName;
			static DWORD lastShowTipTime = GetTickCount();
			if(lastMapName == mapName)
			{
				if(GetTickCount() - lastShowTipTime > KUiCfgLoader::getSingleton().getBigMapCfg().tipFidInTime)
				{
					tipText = getTip(mapName);
					hoverCtrl = hitCtrl;
				}
			}
			else
			{
				lastMapName = mapName;
				lastShowTipTime = GetTickCount();
			}
		}
		else
		{
			const char* mapName = Utf8ToAnsi(hitCtrl->getText());
			hitCtrl->setImage(getNormalImage(mapName));
		}
	}

	if(tipText == NULL)
	{
		_tipText->hide();
		KUiItemTip::Hide();
	}
	else
	{
		Rect area = hoverCtrl->getUnclippedInnerRect();
 		KUiItemTip::GetSingleton().show(const_cast<char*>(tipText), area, KUiItemTip::Right);
// 		_tipText->getLayout()->SetText(const_cast<char*>(tipText));
// 		_tipText->getLayout()->flashLayout();
// 		_tipText->show();
	}
	return true;
}


bool KUiBigMap::mouseClickAPart(const EventArgs& arg)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&arg;

	int index = UI_BIG_MAP_INVALID_INDEX;
	for(int i = 0; i < UI_BIG_MAP_PART_NUM; ++i)
	{
		TLStaticImage* hitCtrl = (TLStaticImage*)_part[i];

		Rect ctrlArea = hitCtrl->getUnclippedPixelRect();
		if(!ctrlArea.isPointInRect(mouse->position))
		{
			continue;
		}

		Point ctrlPos = hitCtrl->getUnclippedPixelRect().getPosition();
		int alpha = hitCtrl->getAlphaAtPixel(0, mouse->position.d_x - ctrlPos.d_x, mouse->position.d_y - ctrlPos.d_y);
		if(alpha == 0)
		{
			continue;
		}
		
		if(RightButton == mouse->button)
		{
			return true;
		}
		
		char* mapName = (char*)Utf8ToAnsi(hitCtrl->getText());
		
		hide();

		KUiSceneMap::getSinglton().showMap(mapName);
	}
	return false;
}

const Image* KUiBigMap::getNormalImage(const char* mapPartName)
{	
	const KUiCfgLoader::BigMapCfg& cfg = KUiCfgLoader::getSingleton().getBigMapCfg();
	for(int i = 0; i < cfg.part.size(); ++i)
	{
		const char* name = cfg.part[i].name;
		if(!strcmp(cfg.part[i].name, mapPartName))
		{
			const char* imagePath = cfg.part[i].normalImage;
			return getImage(imagePath);
		}
	}

	return NULL;
}

const Image* KUiBigMap::getHoverImage(const char* mapPartName)
{
	const KUiCfgLoader::BigMapCfg& cfg = KUiCfgLoader::getSingleton().getBigMapCfg();
	for(int i = 0; i < cfg.part.size(); ++i)
	{
		if(!strcmp(cfg.part[i].name, mapPartName))
		{
			const char* imagePath = cfg.part[i].hoverImage;
			return getImage(imagePath);
		}
	}

	return NULL;
}

const char* KUiBigMap::getTip(const char* mapPartName)
{
	const KUiCfgLoader::BigMapCfg& cfg = KUiCfgLoader::getSingleton().getBigMapCfg();
	for(int i = 0; i < cfg.part.size(); ++i)
	{
		if(!strcmp(cfg.part[i].name, mapPartName))
		{
			return cfg.part[i].tipText;
		}
	}

	return NULL;
}

/*bool KUiBigMap::onSelectOneMap(const EventArgs& args)
{
//	_mapList->hide();

	ListboxItem* item = _mapList->getFirstSelectedItem();
	if(NULL == item)
	{
		return false;
	}
	int index = _mapList->getItemIndex(item);

	const KUiCfgLoader::SceneMapCfg& mapCfg = KUiCfgLoader::getSingleton().getSceneMapCfg();

	hide();
	KUiSceneMap::getSinglton().show();
	KUiSceneMap::getSinglton().setCurMap(mapCfg.mapList[index].name, false);

	return true;
}*/

/*bool KUiBigMap::onClickMapListBtn(const EventArgs& args)
{
	if(_mapList->isVisible())
	{
		_mapList->hide();
	}
	else
	{
		_mapList->show();
	}
	return true;
}*/