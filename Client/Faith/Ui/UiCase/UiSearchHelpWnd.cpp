//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiSearchHelpWnd
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 查询帮助Tip
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#include "UiSearchHelpWnd.h"
#include "UiElf.h"
#include "../KMessageCentre.h"
#include "../UiConfigManager.h"
#include "CoreShell.h"
#include <string>
#include "UiMapCentre.h"
#include "UiTipGenerator.h"
#include "UiLinkedItemTip.h"
#include "../UiAdapter.h"
#include "UiChatWindow.h"
#include "UiGameSetting.h"
#include <iomanip>
#include <sstream>

using namespace CEGUI;
using namespace std;

extern iCoreShell* g_pCoreShell;

template<> 
KUiSearchHelpWnd* KUiWndSingleton<KUiSearchHelpWnd>::ms_Singleton	= NULL;

KUiSearchHelpWnd::KUiSearchHelpWnd(const CEGUI::String& id_name)
: KUiWndSingleton<KUiSearchHelpWnd>( id_name )
, d_pSimpleHelpTip(NULL)
, d_pSimpleHelpText(NULL)
, d_OperationHelpTime(0)
, d_pSearchHelp(NULL)
, d_pNextSearch(NULL)
, d_pLastSearch(NULL)
, d_pScroll(NULL)
, d_RandomHelpTime(0)
, d_MaxHelpCount(0)
, d_bInnerRequest(false)
{
	d_LastSearchContent.clear();
	d_NextSearchContent.clear();
}

KUiSearchHelpWnd::~KUiSearchHelpWnd()
{

}

void KUiSearchHelpWnd::Init()
{
	getChild();
}

void KUiSearchHelpWnd::getChild()
{
	if ( ms_Singleton && ms_Singleton->m_pThisWnd )
	{
		d_pSimpleHelpTip = m_pThisWnd->getChild("TaharezLook/SearchHelpWnd/SimpleHelpTip");

		m_pThisWnd->removeChildWindow(d_pSimpleHelpTip);
		KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_pSimpleHelpTip);

		d_pSimpleHelpText = static_cast<TLStaticText*>(d_pSimpleHelpTip->getChild("TaharezLook/SearchHelpWnd/SimpleHelpTip/Text"));
		d_pSimpleHelpTip->getChild("TaharezLook/SearchHelpWnd/SimpleHelpTip/Close")->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiSearchHelpWnd::handleCloseSimpleHelp, ms_Singleton) );

		d_pSearchHelp = m_pThisWnd->getChild("TaharezLook/SearchHelpWnd/SearchHelp");
		d_pSearchHelp->getChild("TaharezLook/SearchHelpWnd/SearchHelp/Close")->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiSearchHelpWnd::handleCloseSearchHelp, ms_Singleton) );

		d_pScroll = static_cast<TLVertScrollbar*>(d_pSearchHelp->getChild("TaharezLook/SearchHelpWnd/SearchHelp/Scrollbar"));
		d_pScroll->subscribeEvent( TLVertScrollbar::EventScrollPositionChanged, Event::Subscriber(&KUiSearchHelpWnd::handleScrollbar, ms_Singleton) );
		d_pScroll->subscribeEvent( TLVertScrollbar::EventMouseWheel, Event::Subscriber(&KUiSearchHelpWnd::handleMouseWheel, ms_Singleton) );

		d_pNextSearch = static_cast<TLButton*>(d_pSearchHelp->getChild("TaharezLook/SearchHelpWnd/SearchHelp/NextSearch"));
		d_pLastSearch = static_cast<TLButton*>(d_pSearchHelp->getChild("TaharezLook/SearchHelpWnd/SearchHelp/LastSearch"));
		d_pNextSearch->setEnabled(false);
		d_pLastSearch->setEnabled(false);
		d_pNextSearch->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiSearchHelpWnd::handleNextSearch, ms_Singleton) );
		d_pLastSearch->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiSearchHelpWnd::handleLastSearch, ms_Singleton) );

		d_pSearchResultText = static_cast<TLStaticText*>(d_pSearchHelp->getChild("TaharezLook/SearchHelpWnd/SearchHelp/SearchRes"));
		
		//裁剪区域必须是相对底板的位置
		LORect clipper;
		Rect textArea = d_pSearchResultText->getUnclippedInnerRect();
		Point pos = d_pSearchHelp->getPosition(Absolute) + d_pSearchResultText->getPosition(Absolute);
		textArea.setPosition(pos);
		cerectToLorect(&textArea, &clipper);
		d_pSearchResultText->useLayout();
		d_pSearchResultText->getLayout()->setClipper(clipper);
		
		d_pSearchResultText->subscribeEvent( PushButton::EventMouseClick, Event::Subscriber(&KUiSearchHelpWnd::handleClickText, ms_Singleton) );
		d_pSearchResultText->subscribeEvent( PushButton::EventMouseDoubleClick, Event::Subscriber(&KUiSearchHelpWnd::handleClickText, ms_Singleton) );
		d_pSearchResultText->subscribeEvent( PushButton::EventMouseMove, Event::Subscriber(&KUiSearchHelpWnd::handleHoverText, ms_Singleton) );
		d_pSearchResultText->subscribeEvent( TLVertScrollbar::EventMouseWheel, Event::Subscriber(&KUiSearchHelpWnd::handleMouseWheel, ms_Singleton) );
		d_pSearchResultText->setText("");

		const KUiCfgLoader::ChangeMapParam changeMapParam = KUiCfgLoader::getSingleton().getChangeMapParam();
		d_MaxHelpCount = changeMapParam.tipCount;
	}
}

void KUiSearchHelpWnd::Show()
{
	KUiWndSingleton<KUiSearchHelpWnd>::Show();
	KUiQueryWnd::Show();
}

void KUiSearchHelpWnd::Hide()
{
	KUiWndSingleton<KUiSearchHelpWnd>::Hide();
	if ( NULL != ms_Singleton && NULL != ms_Singleton->d_pSimpleHelpTip )
	{
		ms_Singleton->d_pSimpleHelpTip->hide();
	}
	KUiQueryWnd::Hide();
}

void KUiSearchHelpWnd::ShowSimpleHelp(char* layoutText)
{
	if ( KUiGameSetting::GetSingleton().GetShowSimpleHelp() == false ||
		strlen(layoutText) >= LAYOUT_TEXT_MAX_LEN)
		return;

	//KUiWndSingleton<KUiSearchHelpWnd>::Show();
	KUiElf::Show();
	KUiElf::GetSingleton().SetState( ELF_TIP_SHOW );

	d_pSimpleHelpTip->show();
	if ( d_pSearchHelp->isVisible() )
	{
		d_pSearchHelp->hide();
	}
	d_OperationHelpTime = ::GetTickCount();
	
	d_pSimpleHelpText->useLayout();
	d_pSimpleHelpText->getLayout()->formatText(layoutText);
	d_pSimpleHelpText->getLayout()->SetText(layoutText);
	d_pSimpleHelpText->getLayout()->flashLayout();
	
	if(d_pSimpleHelpText->getLayout()->isHaveContent())
	{
		d_pSimpleHelpText->setText("");
		d_pSimpleHelpText->setLayoutOffset(d_pSimpleHelpText->getLeftFrameWidth(), 
										   d_pSimpleHelpText->getTopFrameHeight());
		d_pSimpleHelpText->fitLayoutSize(false);
	}
	else
	{
		d_pSimpleHelpText->setText(AnsiToUtf8(layoutText));
		d_pSimpleHelpText->setHeight(Absolute, 100);
		d_pSimpleHelpText->setWidth(Absolute, 200);
	}
}

void KUiSearchHelpWnd::ShowSearchHelp()
{
	if ( !KUiGameSetting::GetSingleton().GetShowSearchHelp() )
		return;

	Show();
	KUiElf::Show();	
	KUiElf::GetSingleton().SetState( ELF_SEARCH_SHOW );

	ms_Singleton->d_pSearchHelp->show();
	ms_Singleton->d_pSimpleHelpTip->hide();
}

bool KUiSearchHelpWnd::IsSearchVisible()
{
	return ( d_pSearchHelp->isVisible() && KUiQueryWnd::IsVisible() );
}

void KUiSearchHelpWnd::Breathe()
{
	// 无显示时进行随机提示
	//caol- d_pSearchHelp.visible = false的时候Breathe也进不来啊，这段代码被转移到小精灵的breathe里面了
// 	if ( !d_pSimpleHelpTip->isVisible() && !d_pSearchHelp->isVisible() )
// 	{
// 		if ( (::GetTickCount() - d_RandomHelpTime) > RANDOMHELP_INTERVAL * 1000 )
// 		{
// 			ShowSimpleHelp(KMessageCentre::GetMessage(helpTip_message, g_Random(6)));
// 			d_RandomHelpTime = ::GetTickCount();
// 		}
// 	}

	// 到时间自动隐藏简单帮助
	if ( d_pSimpleHelpTip->isVisible() )
	{
		if ( (::GetTickCount() - d_OperationHelpTime) > TIP_INTERVAL*1000 )
		{
			d_pSimpleHelpTip->hide();
		}
	}
}

bool KUiSearchHelpWnd::handleCloseSimpleHelp( const EventArgs& e )
{	
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if ( d_pSimpleHelpTip->isVisible() && arg->button == LeftButton )
	{
		Hide();
	}

	return true;
}

bool KUiSearchHelpWnd::handleCloseSearchHelp( const EventArgs& e )
{
	MouseEventArgs* arg = (MouseEventArgs*)(&e);
	if ( d_pSearchHelp->isVisible() && arg->button == LeftButton )
	{
		Hide();
	}

	return true;
}

void KUiSearchHelpWnd::QueryRequest( SearchContent content, bool cache )
{
	if ( !KUiGameSetting::GetSingleton().GetShowSearchHelp() && !d_bInnerRequest )
		return;

	int nResultCount;
	char searchResult[MAX_SEARCH_TEXT_LENGTH];
	IQueryManager* queryManager = NULL;
	IQueryResult*  queryResult = NULL;

	if ( content.text.length() < 2 && content.id == INVALID_CONTENT_ID )
	{
		char *szMsg = KMessageCentre::GetMessage(common_message, 8);
		KUiChannelCentre::GetSingleton().toSysMsg(szMsg);
		return;
	}
	
	if ( CreateQueryManager( &queryManager ) != query_succeed )
		return;

	if ( queryManager )
	{
		queryManager->InitTab();
	}
	else
	{
		return;
	}

	sprintf( searchResult, "<Layout width=%d height=10000>", (int)d_pSearchResultText->getWidth(Absolute) );

	int nErr = 0;
	if ( !content.text.empty() )
	{
		nErr = queryManager->QueryRequest(&queryResult, std::string(Utf8ToAnsi(content.text)), content.queryType);
	}
	else
	{
		nErr = queryManager->QueryRequest(&queryResult, content.id, content.queryType);
	}

	if ( nErr == query_succeed && queryResult )
	{
		queryResult->GetQueryResult(&nResultCount, (void*)searchResult,1024, &content.queryResultType );
	}
	else
	{
		return;
	}

	strcat( searchResult, "</Layout>" );

	KUiElf::GetSingleton().SetState( ELF_RESULT_SHOW );
	
	ShowSearchResult( 1, searchResult, content.queryResultType );

	if ( !cache )
	{
		SaveQuery( content );
	}
}

void KUiSearchHelpWnd::SaveQuery( SearchContent content )
{
	if ( d_LastSearchContent.size() > 0 )
	{
		SearchVector::iterator it = d_LastSearchContent.end()-1;
		if ( content.id == (*it).id &&
			 content.text == (*it).text && 
			 content.queryType == (*it).queryType && 
			 content.queryResultType == (*it).queryResultType )
		{
			return;
		}
	}

	if ( d_LastSearchContent.size() < MAX_CACHE_NUM )
	{
		d_LastSearchContent.push_back(content);
	}
	else
	{
		d_LastSearchContent.erase(d_LastSearchContent.begin());
		d_LastSearchContent.push_back(content);
	}

	d_NextSearchContent.clear();
	
	UpdateButton();
}

void KUiSearchHelpWnd::ShowSearchResult( int count, char* result, 
										 QueryResultType queryResultType )
{
	d_pScroll->setScrollPosition(0);
	
	switch(queryResultType)
	{
	case format_string:
		{
			d_pSearchResultText->useLayout();
			d_pSearchResultText->getLayout()->formatText( result );
			d_pSearchResultText->getLayout()->SetText( result );
			d_pSearchResultText->getLayout()->flashLayout();
		}
		break;
	case tip_string:
		{
			d_pSearchResultText->useLayout();
			d_pSearchResultText->getLayout()->formatText( result );
			d_pSearchResultText->getLayout()->SetText( result );
			d_pSearchResultText->getLayout()->flashLayout();
		}
		break;
	case map_id:
		{
		}
	    break;
	case screeneffect_id:
		{
		}
	    break;
	default:
	    break;
	}

	if ( m_pThisWnd )
	{
		float renderHeight = d_pSearchResultText->getHeight(Absolute);
		float totleHeight = d_pSearchResultText->getLayout()->getRenderArea().getHeight();
		float step = renderHeight/(2 * (totleHeight - renderHeight));
		d_pScroll->setStepSize(step);

		((TLStaticImage*)m_pThisWnd)->Shake( true, false, 5 );
	}
}

bool KUiSearchHelpWnd::handleLastSearch( const EventArgs& e )
{
	if ( d_LastSearchContent.size() < 2 )
	{
		return false;
	}

	SearchVector::iterator it = d_LastSearchContent.end()-2;
	QueryRequest( *it, true );
	it++;

	if ( d_NextSearchContent.size() < MAX_CACHE_NUM )
	{
		d_NextSearchContent.push_back(*it);
	}
	else
	{
		d_NextSearchContent.erase(d_NextSearchContent.begin());
		d_NextSearchContent.push_back(*it);
	}

	d_LastSearchContent.erase(it);
	UpdateButton();

	return true;
}

bool KUiSearchHelpWnd::handleNextSearch( const EventArgs& e )
{
	if ( d_NextSearchContent.size() < 1 )
	{
		return false;
	}

	SearchVector::iterator it = d_NextSearchContent.end()-1;

	QueryRequest( *it, true );

	if ( d_LastSearchContent.size() < MAX_CACHE_NUM )
	{
		d_LastSearchContent.push_back(*it);
	}
	else
	{
		d_LastSearchContent.erase(d_LastSearchContent.begin());
		d_LastSearchContent.push_back(*it);
	}

	d_NextSearchContent.erase(it);

	UpdateButton();

	return true;
}


bool KUiSearchHelpWnd::handleScrollbar( const EventArgs& e )
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&e;
	if ( d_pSearchResultText != NULL ) 
	{
		int layoutHeight = d_pSearchResultText->getLayout()->getRenderArea().getHeight();
		int clipperHeight = d_pSearchResultText->getHeight(Absolute);
		
		if(layoutHeight <= clipperHeight)
			return false;
		
		float scrollPos = d_pScroll->getScrollPosition();
		
		int yPos = (layoutHeight - clipperHeight) * scrollPos;
		
		d_pSearchResultText->setLayoutOffset( 0, -yPos );
	}

	return true;
}

bool KUiSearchHelpWnd::handleMouseWheel( const EventArgs& e )
{
	MouseEventArgs* eventArgs = (MouseEventArgs*)&e;

	if(d_pScroll->isVisible())
	{
		d_pScroll->setScrollPosition( d_pScroll->getScrollPosition()
									  - d_pScroll->getStepSize() * eventArgs->wheelChange );
	}
	return true;
}

bool KUiSearchHelpWnd::handleHoverText(const EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* frameCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = frameCtrl->getLayout();

	if(lay == NULL)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		return false;
	}

	Point pos = frameCtrl->getUnclippedPixelRect().getPosition();
	Point off = frameCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;

	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		return false;
	}


	if(elemInfo.gameObj._objType == LO_GO_NOTHING)
	{
		KUiAdapter::SetMouseRes( MOUSE_CURSOR_NORMAL );
		return false;
	}

	KUiAdapter::SetMouseRes( MOUSE_SUPER_LINK_PLAYER + elemInfo.gameObj._objType - 1 );
	
	return true;
}

bool KUiSearchHelpWnd::handleClickText( const EventArgs& e )
{
	MouseEventArgs* mouse = (MouseEventArgs*)&e;
	TLStaticText* frameCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = frameCtrl->getLayout();
	SearchContent content;
	content.id = INVALID_CONTENT_ID;
	content.queryResultType = tip_string;
	
	if(lay == NULL)
		return false;
	
	Point pos = frameCtrl->getUnclippedPixelRect().getPosition();
	Point off = frameCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;
	
	LOElemInfo elemInfo;
	if(lay->pickupElem(xPos, yPos, elemInfo) == false)
	{
		return false;
	}
	
	bool handled = false;
	switch(elemInfo.gameObj._objType)
	{
	case LO_GO_PLAYER:
		{
		}
		break;
	case LO_GO_ITEM:
		{
			if( mouse->sysKeys & Control /*&& KUiChatInputWnd::IsVisible()*/ )
			{				
				KUiChatInputWnd::GetSingleton().write(elemInfo);
				KUiChatInputWnd::GetSingleton().show();
			}
			else
			{
				content.id = elemInfo.gameObj._objId[0];
				content.queryType = item_query;
				QueryRequest( content );
				handled = true;
			}
		}
		break;
	case LO_GO_POSITION:
		{
			KUiSceneTimeInfo mapInfo = { 0 };
			
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
			mapInfo.szSceneName[COMMON_CLIENT_MSG_LEN_32 - 1] = 0;
			
			if(elemInfo.gameObj._objId[0] == mapInfo.nSceneId)
			{
				g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)elemInfo.gameObj._objId[1], (int)elemInfo.gameObj._objId[2]*2);
			}
			else
			{
				char *szMsg = KMessageCentre::GetMessage(common_message, CE_Auto_Path_Not_Support_Over_Map);
				KUiChannelCentre::GetSingleton().toSysMsg(szMsg);
			}
		}
		break;
	case LO_GO_CHANNEL:
		{
		}
		break;
	case LO_GO_FACE:
		{
			int chanId = elemInfo.gameObj._objId[0];
			handled = true;
		}
		break;
	case LO_GO_MAP:
		{
			int mapId = elemInfo.gameObj._objId[0];
			char szMapName[32];
			g_pCoreShell->GetGameData(GDI_GET_MAP_NAME, mapId, (int)szMapName );
			KUiSceneMap::getSinglton().showMap(szMapName);
			handled = true;
		}
		break;
	case LO_GO_SKILL:
		{
			content.id = elemInfo.gameObj._objId[0];
			content.queryType = skill_query;
			QueryRequest( content );
		}
		break;
	case LO_GO_TASK:
		{
			content.id = elemInfo.gameObj._objId[0];
			content.queryType = quest_query;
			QueryRequest( content );
		}
		break;
	case LO_GO_NPC:
		{
			content.id = elemInfo.gameObj._objId[0];
			content.queryType = npc_query;

/*
			KUiSceneTimeInfo mapInfo = { 0 };
			
			g_pCoreShell->SceneMapOperation( GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
			mapInfo.szSceneName[COMMON_CLIENT_MSG_LEN_32 - 1] = 0;
			
// 			if(elemInfo.gameObj._objId[1] == mapInfo.nSceneId)
// 			{
			NpcMapPos pos; 
			g_pCoreShell->GetGameData( GDI_GET_NPC_POS_BY_TABLE_INDEX, (UINT)&pos, elemInfo.gameObj._objId[0] );
			g_pCoreShell->OperationRequest( GOI_GOTO_POS, (unsigned)pos.x, (int)pos.y * 2 );
// 			}
// 			else
// 			{
// 				char *szMsg = KMessageCentre::GetMessage(common_message, CE_Auto_Path_Not_Support_Over_Map);
// 				KUiChannelCentre::GetSingleton().toSysMsg(szMsg);
// 			}
//*/

			QueryRequest( content );
		}
		break;
	default:
		break;
	}
	
	return true;
}

void KUiSearchHelpWnd::UpdateButton()
{
	if ( d_LastSearchContent.size() > 1 )
	{
		d_pLastSearch->setEnabled( true );
	}
	else
	{
		d_pLastSearch->setEnabled( false );
	}

	if ( d_NextSearchContent.size() > 0 )
	{
		d_pNextSearch->setEnabled( true );
	}
	else
	{
		d_pNextSearch->setEnabled( false );
	}
		
	int layoutHeight = d_pSearchResultText->getLayout()->getRenderArea().getHeight();
	int clipperHeight = d_pSearchResultText->getHeight(Absolute);
		
	if(layoutHeight <= clipperHeight)
	{
		d_pScroll->hide();
	} 
	else
	{
		d_pScroll->show();
	}
	
	Hide();
	Show();
}

void KUiSearchHelpWnd::ShowWhatICanDo()
{
	KUiPlayerAttribute attr;
	g_pCoreShell->GetGameData( GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&attr, NULL);

	SearchContent content;

	ostringstream sLevel;
	sLevel<<setfill('0')<<setw(2)<<attr.nLevel;
	d_bInnerRequest = true;
	content.id = INVALID_CONTENT_ID;
	content.text = sLevel.str();
	content.queryType = player_query;
	content.queryResultType = format_string;
	QueryRequest( content );
	Show();
	d_bInnerRequest = false;
}