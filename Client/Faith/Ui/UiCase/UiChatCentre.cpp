//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 07/03/2006 16:56
//      File_base        : UiChatCentre
//      File_ext         : cpp
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "UiChatCentre.h"
#include "UiToolsControlBar.h"
#include "../UiConfigManager.h"
#include "../KMessageCentre.h"
#include "../UiElem/TLTreeEx.h"
#include "UiErrorMessageBox.h"
#include "UiComMsgBox.h"
#include "ChatDataDef.h"
#include <vector>
#include "UiTeamList.h"
#include "UiChatWindow.h"
#include "UiPlayerMenu.h"
#include "UiTipGenerator.h"
#include "UiLinkedItemTip.h"
#include "UiSystemMessage.h"
#include "../UiSheetMgr.h"
#include "../UiAdapter.h"



#include "chatWindow/chatWnd.h"
#include "chatWindow/OnwerPlayerInfo.h"
#include "chatWindow/PlayerInfoDlg.h"
#include "chatWindow/ChatFriendPanel.h"
#include "chatWindow/ChatFriendPanelManager.h"
#include "CEGUICoordConverter.h"
#include "SocialComDef.h"
#include "chatWindow/ChatMainDlg.h"

#include "ui/UiCase/UiPathHelp.h"
using namespace ClientMapInfo;

using namespace CHAT;

extern iCoreShell*		g_pCoreShell;

ListBoxItemChatRoom::ListBoxItemChatRoom(const CEGUI::String& text, unsigned int item_id, void* item_data, bool disabled, bool auto_delete) :
	ListboxTextItem( text, item_id, item_data, disabled, auto_delete )
{
	d_roomID = -1;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
template<> 
KUiP2RPopmenu* KUiWndSingleton<KUiP2RPopmenu>::ms_Singleton	= NULL;

KUiP2RPopmenu::KUiP2RPopmenu( const CEGUI::String& id_name ):
KUiWndSingleton<KUiP2RPopmenu>( id_name )
{

}

KUiP2RPopmenu::~KUiP2RPopmenu()
{

}

void KUiP2RPopmenu::Show(const std::string& str, int nRoomID )
{
	KUiWndSingleton<KUiP2RPopmenu>::Show();
	if ( ms_Singleton )
	{
		Point newP = MouseCursor::getSingleton().getPosition();
		newP.d_x-=10;
		newP.d_y-=10;
		ms_Singleton->m_pThisWnd->setPosition( Absolute, newP );
		ms_Singleton->d_string = str;
		ms_Singleton->d_room = nRoomID;
	}	
}

void KUiP2RPopmenu::Init( void )
{
	m_pThisWnd->getChild("TaharezLook/P2pMenu/Screen")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiP2RPopmenu::handleScreen, this));

    m_pThisWnd->getChild("TaharezLook/P2pMenu/Admin")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiP2RPopmenu::handleUpAdmin, this));

	m_pThisWnd->getChild("TaharezLook/P2pMenu/Kick")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiP2RPopmenu::handleKicK, this));

	m_pThisWnd->subscribeEvent(PushButton::EventMouseLeaves, Event::Subscriber(&KUiP2RPopmenu::handleClose, this));
	GlobalEventSet::getSingleton().subscribeEvent(Window::EventMouseClick,     Event::Subscriber(&KUiP2RPopmenu::handleMouse, this));

}

bool KUiP2RPopmenu::handleMouse(const EventArgs& args)
{
	const MouseEventArgs & mouseArgs = (const MouseEventArgs &)  args;
    Rect  rect                       = ms_Singleton->m_pThisWnd->getRect(Absolute);
	if (mouseArgs.position.d_x<rect.d_left ||mouseArgs.position.d_x>rect.d_right || mouseArgs.position.d_y<rect.d_top || mouseArgs.position.d_x<rect.d_bottom )
	{
		Hide();
	}//endif

	return false;
}

bool KUiP2RPopmenu::handleKicK( const EventArgs& args )
{
	g_pCoreShell->OperationRequest( GOI_KICK_CHATROOM, d_room, (int)d_string.c_str() );
	KUiP2RPopmenu::Hide();
	return true;
}

bool KUiP2RPopmenu::handleScreen( const EventArgs& args )
{
	KUiP2RPopmenu::Hide();
	return true;
}

bool KUiP2RPopmenu::handleUpAdmin( const EventArgs& args )
{
	g_pCoreShell->OperationRequest( GOI_CHANGE_ROOM_OWNER, d_room, (int)d_string.c_str() );
	KUiP2RPopmenu::Hide();
	return true;
}

bool KUiP2RPopmenu::handleClose( const EventArgs& args )
{
	KUiP2RPopmenu::Hide();
	return true;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
template<> 
KUiPlayerInfo* KUiWndSingleton<KUiPlayerInfo>::ms_Singleton	= NULL;

KUiPlayerInfo::KUiPlayerInfo( const CEGUI::String& id_name ):
KUiWndSingleton<KUiPlayerInfo>( id_name )
{
	m_Name		= NULL;
	m_Level		= NULL;
	m_Organise	= NULL;
	m_Favor		= NULL;
}

KUiPlayerInfo::~KUiPlayerInfo()
{

}

void KUiPlayerInfo::Updatedata( const PlayerInfo& player )
{
	if ( player.szName[0] )
	{
		m_Name->setAnsiText( player.szName );
	}
	else
	{
		m_Name->setAnsiText( "------" );
	}

	char szBuff[32];
	sprintf( szBuff, "%d", player.sLevel );
	m_Level->setAnsiText( szBuff );
	
	
	char szZhiye[16];
	if ( player.sSkillType < 0 )
	{
		switch(player.sMetier)
		{
		case 0:
			strcpy( szZhiye, ROLE_CAREER_JS);
			break;
		case 1:
			strcpy( szZhiye, ROLE_CAREER_DS);
			break;
		case 2:
			strcpy( szZhiye, ROLE_CAREER_YR);
			break;
		default :
			strcpy( szZhiye, ROLE_CAREER_JS);
			break;
		}
	}
	else
	{

		switch(player.sMetier)
		{
		case 0:
			if ( player.sSkillType )
			{
				strcpy( szZhiye, ROLE_CAREER_JS_0);
			}
			else
			{
				strcpy( szZhiye, ROLE_CAREER_JS_1);
			}
			break;
		case 1:
			if ( player.sSkillType )
			{
				strcpy( szZhiye, ROLE_CAREER_DS_0);
			}
			else
			{
				strcpy( szZhiye, ROLE_CAREER_DS_1);
			}
			break;
		case 2:
			if ( player.sSkillType )
			{
				strcpy( szZhiye, ROLE_CAREER_YR_1);
			}
			else
			{
				strcpy( szZhiye, ROLE_CAREER_YR_0);
			}
			break;
		default :
			if ( player.sSkillType )
			{
				strcpy( szZhiye, ROLE_CAREER_JS_1);
			}
			else
			{
				strcpy( szZhiye, ROLE_CAREER_JS_0);
			}
			break;
		}
	}
	m_Favor->setAnsiText( szZhiye );

	if ( player.szZhuhou[0] )
	{
		m_Organise->setAnsiText( player.szZhuhou );
	}
	else
	{
		m_Organise->setAnsiText( "------" );
	}
	

	KUiPlayerInfo::Show();
}

void KUiPlayerInfo::Init( void )
{
	m_pThisWnd->getChild("TaharezLook/FriendInfo/Close")->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiPlayerInfo::handleClose, this));
	m_pThisWnd->getChild("TaharezLook/FriendInfo/Title")->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiPlayerInfo::handleClose, this));

	m_Name		= m_pThisWnd->getChild("TaharezLook/FriendInfo/Name");
	m_Level		= m_pThisWnd->getChild("TaharezLook/FriendInfo/Level");
	m_Organise	= m_pThisWnd->getChild("TaharezLook/FriendInfo/Organise");
	m_Favor		= m_pThisWnd->getChild("TaharezLook/FriendInfo/Favor");
}

bool KUiPlayerInfo::handleClose( const EventArgs& args )
{
	KUiPlayerInfo::Hide();
	return true;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

const unsigned int	SendButtonID = UI_CHATROOM_SEND;
const unsigned int	CloseButtonID = UI_CHATROOM_CLOSE;
const unsigned int	AddButtonID = UI_CHATROOM_ADD;
const unsigned int  ScrollWidth = 30;
KUiChatRoom::KUiChatRoom()
{
	m_nRoomID	= 0;
	d_inputBox = NULL;
	d_faceBtn	= NULL;
	d_facePanel = NULL;
	m_CurBuffIndex =0;
	m_bOwner       =false;
}


KUiChatRoom::~KUiChatRoom()
{
	TreeItemList::iterator it = d_itemList.begin();
	while ( it != d_itemList.end() )
	{
		if ( *it )
		{
			delete *it;
			*it = NULL;
		}
		it++;
	}
}

void KUiChatRoom::setOwnerState(const bool bOwner)
{
    m_bOwner = bOwner;
	if (bOwner)
	{
		m_pThisWnd->getChild(AddButtonID)->enable();
	}//endif
	else
		m_pThisWnd->getChild(AddButtonID)->disable();
}

bool KUiChatRoom::isHaveFocus( void )
{
	if ( d_inputBox )
	{
		return d_inputBox->hasInputFocus();
	}
	else
	{
		return false;
	}
}

void KUiChatRoom::AddEvent()
{
	if (m_pThisWnd == NULL)
	{
		return;
	}

	m_pThisWnd->getChild(SendButtonID)->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatRoom::handleSend, this));

    m_pThisWnd->getChild(CloseButtonID)->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatRoom::handleExit, this));

	m_pThisWnd->getChild(AddButtonID)->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatRoom::handleAdd, this));


	CEGUI::String strChildName = m_pThisWnd->getName() + "/FriendList";
	d_listBox = (TLTree*)m_pThisWnd->getChild(strChildName);
    d_listBox->SetInsectionPixel(3.0f); //Item 之间加入间歇 ，调整
	d_listBox->subscribeEvent(Tree::TR_EventSelectionChanged, Event::Subscriber(&KUiChatRoom::handleClickList, this));
	

	m_pThisWnd->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&KUiChatRoom::handleKeyDown, this));

	strChildName = m_pThisWnd->getName() + "/RecvWnd";
	TLStaticText* pWnd = (TLStaticText* )m_pThisWnd->getChild( strChildName );
	if ( pWnd )
	{
		pWnd->useLayout();
		//裁剪区域必须是相对底板的位置
		Rect textArea = pWnd->getUnclippedInnerRect();
		Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
 		textArea.setPosition(posOff);

		LORect clipper;
		cerectToLorect(&textArea, &clipper);
		pWnd->getLayout()->setClipper(clipper);
		pWnd->subscribeEvent(TLStaticText::EventMouseDoubleClick, 
			Event::Subscriber(&KUiChatRoom::clickText, this));

		pWnd->subscribeEvent(TLStaticText::EventMouseClick, 
			Event::Subscriber(&KUiChatRoom::clickText, this));

		pWnd->subscribeEvent(TLStaticText::EventMouseMove, 
			Event::Subscriber(&KUiChatRoom::hoverText, this));
		
	}

	//滑动条
	strChildName = m_pThisWnd->getName() + "/Scrollbar";
	d_scrollBar = (TLVertScrollbar*)m_pThisWnd->getChild(strChildName);
	d_scrollBar->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged, 
		Event::Subscriber(&KUiChatRoom::handleScroll, this));
    d_scrollBar->setScrollPosition(1.0f);

 	strChildName = m_pThisWnd->getName() + "/SendWnd";
	d_inputBox = (TLEditbox* )m_pThisWnd->getChild( strChildName );
	if ( d_inputBox )
	{
		d_inputBox->subscribeEvent(TLEditbox::EventCharacterKey, Event::Subscriber(&KUiChatRoom::handleKeyInput, this));
		d_inputBox->subscribeEvent(TLEditbox::EventKeyDown, Event::Subscriber(&KUiChatRoom::handleKeyDown, this));
		d_inputBox->useLayout();
		clearText();
		//d_inputBox->activate();
	}

 	strChildName = m_pThisWnd->getName() + "/Face";
	d_faceBtn = (TLButton*)m_pThisWnd->getChild( strChildName );
	if ( d_faceBtn )
	{
		d_faceBtn->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatRoom::handleFaceBtnDown, this));
	}	

	//表情面版排版
	strChildName = m_pThisWnd->getName() + "/facepanel";
	d_facePanel = (TLStaticText*)m_pThisWnd->getChild(strChildName);
    d_facePanel->subscribeEvent(TLStaticText::EventHidden, Event::Subscriber(&KUiChatRoom::handleFaceHide, this));
	d_facePanel->subscribeEvent(TLStaticText::EventMouseDoubleClick, Event::Subscriber(&KUiChatRoom::handleSelectAFace, this));
	d_facePanel->subscribeEvent(TLStaticText::EventMouseButtonDown, Event::Subscriber(&KUiChatRoom::handleSelectAFace, this));
	d_facePanel->useLayout();

	const KUiCfgLoader::FacePanelCfgData& facePanelCfg = KUiCfgLoader::getSingleton().getFaceData();
	
	char facePanelLayout[LAYOUT_TEXT_MAX_LEN] = {0};

	char tempText[COMMON_CLIENT_MSG_LEN_128];
	sprintf(facePanelLayout, "<Layout width=%d>", facePanelCfg.wndWidth);
	strcat(facePanelLayout , "<Seg text-align=left float=none>");
	for(int k = 0; k < facePanelCfg.faceList.size(); ++k)
	{
		sprintf(tempText, "<Obj type=pic gotype=face goid=%d>%s</Obj>",
			facePanelCfg.faceList[k].index,
			facePanelCfg.faceList[k].image);
		strcat(facePanelLayout, tempText);
	}
	strcat(facePanelLayout , "</Seg>");
	strcat(facePanelLayout , "</Layout>");

	d_facePanel->getLayout()->SetText(facePanelLayout);
	d_facePanel->getLayout()->flashLayout();
	d_facePanel->fitLayoutSize();
	d_facePanel->setLayoutOffset(d_facePanel->getLeftFrameWidth(), d_facePanel->getTopFrameHeight());
	d_facePanel->hide();
	m_pThisWnd->removeChildWindow(d_facePanel);
	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_facePanel);
	d_facePanel->setRenderMode(true);
	Point facePanelAbPos;
	facePanelAbPos = d_faceBtn->getUnclippedPixelRect().getPosition();
	facePanelAbPos.d_y -= d_facePanel->getAbsoluteHeight() + 10;//上移10个像素否则不太好看
	d_facePanel->setPosition(Absolute, facePanelAbPos);


	m_pThisWnd->subscribeEvent(TLStaticText::EventHidden,  
			Event::Subscriber(&KUiChatRoom::onHide, this));
    
	setOwnerState(m_bOwner);

#ifndef _DEBUG
	try
#endif
	{
		strChildName = m_pThisWnd->getName() + "/Minimize";
		m_pThisWnd->getChild(strChildName)->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatRoom::handleMinimize, this));
	}
#ifndef _DEBUG
	catch (...)
	{
	}
#endif
}

bool KUiChatRoom::handleMinimize(const EventArgs& args )
{
	KUiChatNotify::Show();
	KUiChatRoom::Hide();
	KUiChatRoomListMenu::GetSingleton().AddPlayerName(m_strTargetName);
	return true;
}

bool KUiChatRoom::handleFaceHide(const EventArgs& args )
{
	d_facePanel->setZLevel(Window::Top);
	return true;
}

bool KUiChatRoom::handleSelectAFace( const EventArgs& args )
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* chanCtrl = (TLStaticText*)mouse->window;
	ILayout* layout = chanCtrl->getLayout();
	
	Point pos = chanCtrl->getUnclippedPixelRect().getPosition();
	Point off = chanCtrl->getLayoutOffset();
	int xPos = mouse->position.d_x - pos.d_x - off.d_x;
	int yPos = mouse->position.d_y - pos.d_y - off.d_y;
	
	LOElemInfo elemInfo;
	if(layout->pickupElem(xPos, yPos, elemInfo) == false)
	{
		return false;
	}

	if(elemInfo.gameObj._objType != LO_GO_FACE)
	{
		return false;
	}

	write(elemInfo);
	
	d_facePanel->hide();
	return true;
}

bool KUiChatRoom::hoverText(const EventArgs& args)
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


bool KUiChatRoom::clickText(const CEGUI::EventArgs& args)
{
	MouseEventArgs* mouse = (MouseEventArgs*)&args;
	TLStaticText* chanCtrl = (TLStaticText*)mouse->window;
	ILayout* lay = chanCtrl->getLayout();

	if(lay == NULL)
		return false;
	
	Point pos = chanCtrl->getUnclippedPixelRect().getPosition();
	Point off = chanCtrl->getLayoutOffset();
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
			if(LeftButton == mouse->button)
			{
  				KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
 				if ( pRoom && pRoom->IsVisible() )
 				{
					pRoom->write(elemInfo);
					pRoom->write(" ");
				}
 				else
				{
					KUiChatInputWnd::GetSingleton().clearText();
					
					KUiChatInputWnd::GetSingleton().write("/");

 					KUiChatInputWnd::GetSingleton().write(elemInfo);
 					KUiChatInputWnd::GetSingleton().write(" ");
 					
 					KUiChatInputWnd::GetSingleton().show();
 				}

			}
			else if(RightButton == mouse->button)
			{
				char* name = NULL;
				unicodeToAnsi(elemInfo.content.get(), name);

				KUiPlayerBaseInfo tagRoleInfo;
				g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&tagRoleInfo, NULL );
				
				if ( 0 == strcmp(tagRoleInfo.Name, name) )
					return false;
				
				KUiPlayerItem tagPlayerID;
				memcpy(tagPlayerID.Name, name, strlen(name) + 1);
				g_pCoreShell->GetGameData(GDI_GET_NPC_ID_BY_NAME, (unsigned int)&tagPlayerID, NULL);
				g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (unsigned int)&tagRoleInfo, (int)tagPlayerID.uId);

				Point parentP = m_pThisWnd->getPosition(Absolute);
				Point newP = MouseCursor::getSingleton().getPosition();
				newP.d_y -= KUiPlayerMenu::GetSingleton().getArea().getHeight() + 20;
				KUiPlayerMenu::GetSingleton().setPos(newP);
				int _tagTeamID = 0;
				if (tagRoleInfo.bTeam)
				{
					_tagTeamID = 10;
				}

				KUiPlayerMenu::GetSingleton().ShowByClickText(name, elemInfo.gameObj._objId[0]);
				delete[] name;
				name = NULL;
			}
			handled = true;
		}
		break;
	case LO_GO_ITEM:
		{
			if(mouse->sysKeys & Control && KUiChatInputWnd::IsVisible())
			{				
 				KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
 				if ( pRoom && pRoom->IsVisible() )
 				{
					pRoom->write(elemInfo);
 				}
 				else
 				{
 					KUiChatInputWnd::GetSingleton().write(elemInfo);
 					KUiChatInputWnd::GetSingleton().show();
 				}
			}
			else
			{
				KUiTipGenerator::TipObject tipObj;
				tipObj.type = KUiTipGenerator::LinkedItem;
				SpliteHashId(elemInfo.gameObj._objId[0], tipObj.ids[0], tipObj.ids[1], tipObj.ids[2]);
				tipObj.ids[3]	= elemInfo.gameObj._objId[1];
				tipObj.ids[4]	= elemInfo.gameObj._objId[2];
				char* layoutDes = KUiTipGenerator::getSinglton().genLayoutDes(tipObj);
				KUiLinkedItemTip::GetSingleton().show(layoutDes, m_pThisWnd->getRect(Absolute), KUiLinkedItemTip::TopRight);
				char* compareLayoutText = KUiTipGenerator::getSinglton().genCompareLayoutDes(tipObj);
				KUiLinkedItemTip::GetSingleton().showCompare(compareLayoutText);
			}
			handled = true;
		}
		break;
	case LO_GO_POSITION:
		{
			if(mouse->sysKeys & Control && KUiChatInputWnd::IsVisible())
			{
 				KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
 				if ( pRoom && pRoom->IsVisible() )
 				{
					pRoom->write(elemInfo);
 				}
 				else
 				{
 					KUiChatInputWnd::GetSingleton().write(elemInfo);
 					KUiChatInputWnd::GetSingleton().show();
 				}
			}
			else
			{	
				if(elemInfo.gameObj._objId[3] == 1)
				{
					const AutoMapInfo* pMapInfo = AutoGoBack::Singleton().GetMapInfo(elemInfo.gameObj._objId[0]);
					if(pMapInfo == 0)
						return true;
					KUiSceneTimeInfo mapInfo = {0};
					g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
					if(strcmp(pMapInfo->mapName,mapInfo.szSceneName) == 0)
					{
						g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)elemInfo.gameObj._objId[1], (int)elemInfo.gameObj._objId[2] * 2);
					}
					else
					{
						KUiChannelCentre::GetSingleton().toSysMsg(ChatString::ChatStringGetString().autoGoInfo);
					}

					handled = true;
					break;

				}
				KUiSceneTimeInfo mapInfo = { 0 };
				
				g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&mapInfo, NULL );
				mapInfo.szSceneName[COMMON_CLIENT_MSG_LEN_32 - 1] = 0;

				if(elemInfo.gameObj._objId[0] == mapInfo.nSceneId)
				{
					g_pCoreShell->OperationRequest(GOI_GOTO_POS, (unsigned)elemInfo.gameObj._objId[1], (int)elemInfo.gameObj._objId[2] * 2);
				}
				else
				{
					KUiChannelCentre::GetSingleton().toSysMsg(MSG_CANT_AUTORUN);
				}
				handled = true;
			}
		}
		break;
	case LO_GO_FACE:
		{
			if(mouse->sysKeys & Control && KUiChatInputWnd::IsVisible())
			{
				KUiChatRoom* pRoom = KUiChatCentre::GetSingleton().getActiveChatRoom();
 				if ( pRoom && pRoom->IsVisible() )
 				{
					pRoom->write(elemInfo);
 				}
 				else
 				{
 					KUiChatInputWnd::GetSingleton().write(elemInfo);
 					KUiChatInputWnd::GetSingleton().show();
 				}
			}
		}
		break;
	default:
		break;
	}
	
	if(KUiChatInputWnd::IsVisible())
	{
		KUiChatInputWnd::GetSingleton().show();
	}
	return handled;
}

void KUiChatRoom::adjustLayoutPos( void )
{
	CEGUI::String strChildName = m_pThisWnd->getName() + "/RecvWnd";
	TLStaticText* pWnd = (TLStaticText* )m_pThisWnd->getChild( strChildName );
	if ( pWnd )
	{
		int layoutHeight = pWnd->getLayout()->getRenderArea().getHeight();
		int clipperHeight = pWnd->getHeight(Absolute) 
			- pWnd->getTopFrameHeight() - pWnd->getBottomFrameHeight();
		
		if(layoutHeight <= clipperHeight)
			return;

		float scrollPos = d_scrollBar->getScrollPosition();
		
		int yPos = (layoutHeight - clipperHeight) * scrollPos;

		pWnd->setLayoutOffset(pWnd->getLeftFrameWidth(), -yPos + pWnd->getTopFrameHeight());
	}
}

bool KUiChatRoom::handleScroll( const EventArgs& args )
{
	WindowEventArgs* scrollCtrl = (WindowEventArgs*)&args;
	adjustLayoutPos();
	return true;
}

bool KUiChatRoom::handleClickList( const EventArgs& args )
{
	TreeItemEx* pItem = static_cast<TreeItemEx *>(static_cast<Tree *>(d_listBox)->getFirstSelectedItem());
	if ( pItem && m_bOwner )
	{
		KUiP2RPopmenu::Show( Utf8ToAnsi( pItem->getText() ), m_nRoomID );
	}
	return true;
}

bool KUiChatRoom::handleSend( const EventArgs& args )
{
	CEGUI::String strChildName = m_pThisWnd->getName() + "/SendWnd";
	Window* pWnd = m_pThisWnd->getChild( strChildName );
	if ( pWnd )
	{
		doSendMessage();
	}

	return true;
}

bool KUiChatRoom::handleExit( const EventArgs& args )
{
	Hide();
	
	return true;
}	

bool KUiChatRoom::handleAdd( const CEGUI::EventArgs& args )
{
	KUiChatCentre::clickAddToChatRoom(m_nRoomID);
	return true;
}	


bool KUiChatRoom::handleKeyDown( const CEGUI::EventArgs& args )
{	
	char d_msg[LAYOUT_TEXT_MAX_LEN + 1];
	int startIndex = 0;
	int endIndex = 0;

	const KeyEventArgs& key = static_cast<const KeyEventArgs&>(args);

	switch(key.scancode)
	{
	case Key::Return:
		{
			doSendMessage();
		}
		break;
	case Key::Delete:
		{
			d_inputBox->getLayout()->getSelection(startIndex, endIndex);
			if(startIndex == endIndex)
				d_inputBox->getLayout()->setSelection(startIndex, startIndex + 1);
			d_inputBox->getLayout()->eraseSelection();
			flashColor();
		}
		break;
	case Key::Backspace:
		{
			d_inputBox->getLayout()->getSelection(startIndex, endIndex);
			if(startIndex == endIndex)
				d_inputBox->getLayout()->setSelection(startIndex - 1, startIndex);
			d_inputBox->getLayout()->eraseSelection();
			flashColor();
		}
		break;
	case Key::ArrowLeft:
		{
			d_inputBox->getLayout()->getSelection(startIndex, endIndex);
			endIndex = endIndex > 0 ? endIndex - 1 : 0;
			if(key.sysKeys & Shift)
			{
				d_inputBox->getLayout()->setSelection(-1 , endIndex);
			}
			else
			{
				d_inputBox->getLayout()->setSelection(endIndex , endIndex);
			}
		}
		break;
	case Key::ArrowRight:
		{
			int startIndex = 0;
			int endIndex = 0;
			d_inputBox->getLayout()->getSelection(startIndex, endIndex);
			endIndex += 1;
			if(key.sysKeys & Shift)
			{
				d_inputBox->getLayout()->setSelection(-1 , endIndex);
			}
			else
			{
				d_inputBox->getLayout()->setSelection(endIndex , endIndex);
			}
		}
		break;
	case Key::Home:
		{
			if(key.sysKeys & Shift)
			{
				d_inputBox->getLayout()->setSelection(-1 , 0);
			}
			else
			{
				d_inputBox->getLayout()->setSelection(0 , 0);
			}
		}
		break;
	case Key::End:
		{
			if(key.sysKeys & Shift)
			{
				d_inputBox->getLayout()->setSelection(-1 , 100000);
			}
			else
			{
				d_inputBox->getLayout()->setSelection(100000 , 100000);
			}
		}
		break;
	case Key::V:
		{
			if(key.sysKeys & Control)
			{
				if(OpenClipboard(NULL) == false)
					break;
				////张鹏程添加，为unicode
				HGLOBAL hData = 0;//GetClipboardData(CF_OEMTEXT);
				UINT format = 0; // 从第一种格式值开始枚举
				while(format = EnumClipboardFormats(format))
				{
					if(format == CF_TEXT)
					{
						hData = GetClipboardData(CF_TEXT);
						if(NULL == hData)
						{
							CloseClipboard();
					         break;
						}
					    char* data = (char*)GlobalLock(hData);
						int len = strlen(data);
						if(len >= LAYOUT_TEXT_MAX_LEN)
						{
							memcpy(d_msg, data, LAYOUT_TEXT_MAX_LEN);
							d_msg[LAYOUT_TEXT_MAX_LEN - 1] = '\0';
						}
						else
						{
							strcpy(d_msg, data);
						}
						GlobalUnlock(hData);
							
						CloseClipboard();
							
						WriteTextFromClipbord(d_msg);
					}
					else
					if(format == CF_UNICODETEXT)
					{
						hData = GetClipboardData(CF_UNICODETEXT);
						if(NULL == hData)
						{
							CloseClipboard();
					         break;
						}
						wchar_t* wData = (wchar_t*)GlobalLock(hData);
						char* data = 0;
						unicodeToAnsi(wData,data);
						GlobalUnlock(hData);		
						CloseClipboard();
						int len = strlen(data);
						if(len >= LAYOUT_TEXT_MAX_LEN)
						{
							memcpy(d_msg, data, LAYOUT_TEXT_MAX_LEN);
							d_msg[LAYOUT_TEXT_MAX_LEN - 1] = '\0';
						}
						else
						{
							strcpy(d_msg, data);
						}
						write(d_msg);
						delete [] data;
					}
				}
			}
		}
		break;
	case Key::C:
		{
			if(key.sysKeys & Control)
			{
				if(OpenClipboard(NULL) == false)
					break;

				if(EmptyClipboard() == false)
				{
					CloseClipboard();
					break;
				}
				
				char* ansiText = NULL;
				
				int len = getSelectionText(ansiText);
				
				HGLOBAL hData = (char*)GlobalAlloc(GMEM_MOVEABLE, len + 1);
				if(NULL == hData)
				{
					CloseClipboard();
					break;
				}
				
				char* pszData = (char*)GlobalLock(hData);
				if(NULL == pszData)
				{
					CloseClipboard();
					break;
				}
				
				memcpy(pszData, ansiText, len);
				pszData[len] = 0;
				GlobalUnlock(hData);
				SetClipboardData(CF_TEXT, hData);

				CloseClipboard();

				delete[] ansiText;
			}
		}
		break;
	case Key::X:
		{
			if(key.sysKeys & Control)
			{
				if(OpenClipboard(NULL) == false)
					break;

				if(EmptyClipboard() == false)
				{
					CloseClipboard();
					break;
				}
				
				char* ansiText = NULL;
				
				int len = getSelectionText(ansiText);
				
				HGLOBAL hData = (char*)GlobalAlloc(GMEM_MOVEABLE, len + 1);
				if(NULL == hData)
				{
					CloseClipboard();
					break;
				}
				char* pszData = (char*)GlobalLock(hData);
				if(NULL == pszData)
				{
					CloseClipboard();
					break;
				}
				
				memcpy(pszData, ansiText, len);
				pszData[len] = 0;
				GlobalUnlock(hData);
				SetClipboardData(CF_TEXT, hData);

				CloseClipboard();

				delete[] ansiText;
				
				d_inputBox->getLayout()->eraseSelection();
			}
		}
		break;
	case Key::Escape:
		{
			clearText();
			Hide();
		}
		break;
	case Key::F1:
	case Key::F2:
	case Key::F3:
	case Key::F4:
	case Key::F5:
	case Key::F6:
	case Key::F7:
	case Key::F8:
	case Key::F9:
	case Key::F10:
	case Key::F11:
	case Key::F12:
	case Key::Tab:
		{
			return false;
		}
		break;
	}
	showText();
	
	d_inputBox->requestRedraw();
	return true;
}
void KUiChatRoom::WriteTextFromClipbord(char* pText)
{
	vector<LOElemInfo*> elemInfoList;
	ChatEditBox::ChatEditGetElemListForClipbord(pText,&elemInfoList);
	if(!elemInfoList.empty())
	{
		int size = elemInfoList.size();
		for(int i = 0; i < size; i++)
		{
			LOElemInfo* pElemInfo = elemInfoList[i];
			write(*pElemInfo);
		}
	}

	///清除///
	while (!elemInfoList.empty())
	{
		LOElemInfo* pElemInfo = elemInfoList.back();
		elemInfoList.pop_back();
		delete pElemInfo;
		pElemInfo = 0;
	}
	elemInfoList.clear();
}
int KUiChatRoom::getSelectionText(char*& text)
{
	static wchar_t copyText[LAYOUT_TEXT_MAX_LEN];
	wchar_t    mySubText[LAYOUT_TEXT_MAX_LEN] = {0};

	copyText[0] = 0;

	int selStart = 0;
	int selEnd = 0;
	d_inputBox->getLayout()->getSelection(selStart, selEnd);
	if(selStart > selEnd)
	{
		int temp = selStart;
		selStart = selEnd;
		selEnd = temp;
	}
				
	LOElemInfo* elemList = NULL;
	int elemCount = d_inputBox->getLayout()->getElemList(elemList);
	int curIndex = 0;
	int copyIndex = 0;
	for(int i = 0; i < elemCount; ++i)
	{
		int elemStart = 0;
		int elemEnd = 0;
		const wchar_t* subText = NULL;
		mySubText[0] = 0;
		if(elemList[i].isShowDes)
		{
			if(curIndex >= selStart && curIndex < selEnd)
			{
				elemStart = 0;
				elemEnd = elemList[i].description.len();
				subText = elemList[i].description.get();
				wcscpy(mySubText,subText);
			}
			++curIndex;
		}
		else if(elemList[i].elemType == LO_IMAGE)
		{
			if(curIndex >= selStart && curIndex < selEnd)
			{
				elemStart = 0;
				elemEnd = elemList[i].content.len();
				subText = elemList[i].content.get();
				wcscpy(mySubText,L"<face>");
				wcscat(mySubText,L"<id>");
				wchar_t temp[256] = {0};
				wsprintfW(temp,L"%d",elemList[i].gameObj._objId[0]);
				wcscat(mySubText,temp);
				wcscat(mySubText,L"</id></face>");
				elemEnd = wcslen(mySubText);
			}
			++curIndex;
		}
		else
		{
			int elemWordCount = elemList[i].content.len();
			if(selStart >= curIndex && selStart < curIndex + elemWordCount)
			{
				elemStart = selStart - curIndex;
			}
			elemEnd = elemWordCount;
			if(selEnd > curIndex && selEnd <= curIndex + elemWordCount )
			{
				elemEnd = selEnd - curIndex;
			}
			subText = elemList[i].content.get();
			wcscpy(mySubText,subText);
			curIndex += elemWordCount;
		}

		for(int j = elemStart; j < elemEnd; ++j)
		{
			copyText[copyIndex++] = mySubText[j];
		}
	}
	delete[] elemList;
	elemList = NULL;

	copyText[copyIndex] = 0;
	return unicodeToAnsi(copyText, text);
}

void KUiChatRoom::doSendMessage()
{
	LOElemInfo* elemList = NULL;
	int elemCount = d_inputBox->getLayout()->getElemList(elemList);
	if(elemCount <= 0)
	{
		return;
	}

	//cacheMessage(elemList, elemCount);
	
	char szMsg[LAYOUT_TEXT_MAX_LEN+1];

	szMsg[0] = 0;
	int textLen = 0;
	char textHead[COMMON_CLIENT_MSG_LEN_64];
	memset(textHead, 0, COMMON_CLIENT_MSG_LEN_64);
	for(int i = 0; i < elemCount; ++i)
	{
		if(elemList[i].gameObj._objType == LO_GO_ITEM)
		{
			sprintf(textHead, "<I=%d|%d|%d|", 
				elemList[i].gameObj._objId[0], 
				elemList[i].gameObj._objId[1], 
				elemList[i].gameObj._objId[2]);
		}
		else if(elemList[i].gameObj._objType == LO_GO_NOTHING)
		{
			strcpy(textHead, "<N=");
		}
		else if(elemList[i].gameObj._objType == LO_GO_FACE)
		{
			sprintf(textHead, "<F=%d", 
				elemList[i].gameObj._objId[0]);
		}
		else if(elemList[i].gameObj._objType == LO_GO_POSITION)
		{
			sprintf(textHead, "<P=%d|%d|%d|", 
				elemList[i].gameObj._objId[0], 
				elemList[i].gameObj._objId[1], 
				elemList[i].gameObj._objId[2]);
		}

		//判断是否超长
		textLen += strlen(textHead);
		if(textLen > LAYOUT_TEXT_MAX_LEN)
		{
			break;
		}

		char* ansiText = NULL;
		int ansiTextLen = 0;
		if(elemList[i].isShowDes)
		{
			ansiTextLen = unicodeToAnsi(elemList[i].description.get(), ansiText);
		}
		else
		{
			ansiTextLen = unicodeToAnsi(elemList[i].content.get(), ansiText);
		}
		for(int j = 0; j < ansiTextLen; ++j)
		{
			if(ansiText[j] == '<')
			{
				ansiText[j] = 1;
			}
			else if(ansiText[j] == '>')
			{
				ansiText[j] = 2;
			}
		}
		//判断是否超长
		textLen += strlen(ansiText);		
		if(textLen > LAYOUT_TEXT_MAX_LEN)
		{
			delete []ansiText ;
			break;
		}

		strcat(szMsg, textHead);
		if(elemList[i].gameObj._objType != LO_GO_FACE)
		{
			strcat(szMsg, ansiText);
		}
		strcat(szMsg, ">");

		delete[] ansiText;
		ansiText = NULL;
	}
	if(strlen(szMsg) > 4)
	{
		if ( m_bRoomChat )
		{
			chatInRoom( m_nRoomID, (BYTE*)szMsg, sizeof(szMsg) );
		}
		else
		{
			chatToSomeone( m_strTargetName, (BYTE*)szMsg, strlen(szMsg) );
		}
	}
	delete [] elemList;
	clearText();

	d_scrollBar->setScrollPosition(1.0f);
	Show(m_bPrivate);
}


void KUiChatRoom::chatToSomeone(const std::string &strReceiver, BYTE *pMsg, int nMsgLen)
{
	g_pCoreShell->OperationRequest( GOI_SEND_CHAT_DATE_P2P, (unsigned int)strReceiver.c_str(), (int)pMsg );	
}

void KUiChatRoom::chatInRoom(DWORD dwRoomId, BYTE *pMsg, int nMsgLen)
{
	g_pCoreShell->OperationRequest( GOI_SEND_CHAT_DATE_P2R, dwRoomId, (int)pMsg );	
}

void KUiChatRoom::setChatMode( bool bIsRoomChat )
{
	m_bRoomChat = bIsRoomChat;
}

bool KUiChatRoom::onHide( const EventArgs& args )
{
	d_facePanel->hide();
	if (KUiP2RPopmenu::IsVisible())
		KUiP2RPopmenu::Hide();
	return true;
}

void KUiChatRoom::recvMsgFromSomeone( BYTE *pMsg )
{
	CEGUI::String strChildName = m_pThisWnd->getName() + "/RecvWnd";
	TLStaticText* pWnd = (TLStaticText* )m_pThisWnd->getChild( strChildName );
	if ( pWnd )
	{
		std::string strMsg;
		
		char szBuff[256];
		sprintf( szBuff, "<Layout width=%f margin-top=0 margin-left=0 margin-right=0 margin-bottom=0>", pWnd->getWidth(Absolute) );
		strMsg = szBuff;		
		
		switch (m_CurBuffIndex)
		{
		case 0:
			{
				m_strMsgBuff[0] += (char*)pMsg;
				
				strMsg += m_strMsgBuff[1]+m_strMsgBuff[0];
				
				if (m_strMsgBuff[0].length()>=2048)
				{
					m_CurBuffIndex=1;	
                    m_strMsgBuff[1]=" ";
				}//end if 
				
			}//end for 0
			break;
			
		case 1:
			{
				m_strMsgBuff[1] += (char*)pMsg;
				
				strMsg += m_strMsgBuff[0]+m_strMsgBuff[1];
				
				if (m_strMsgBuff[1].length()>=2048)
				{
					m_CurBuffIndex=0;
					m_strMsgBuff[0]=" ";
				}//endif 
				
			}//end for 1
			break;
		}//end for switch

		strMsg += "</Layout>";

		ILayout* pLayout = pWnd->getLayout();
		if ( pLayout )
		{
			pLayout->SetText( (char*)strMsg.c_str() );
			pLayout->flashLayout();
			adjustLayoutPos();
			pWnd->requestRedraw();
		}
	}//*/
	Show(m_bPrivate);
}

/*
void KUiChannelCentre::addAMessage(int panelIndex, char* segText)
{
	TLStaticText* thisTextCtrl = d_frameContent[panelIndex];

	list<string>& strList = d_msgList[panelIndex];
	strList.push_back(segText);
	

	d_textLen[panelIndex] += strlen(segText);

	while(d_textLen[panelIndex] > LAYOUT_TEXT_MAX_LEN - 100 && d_msgList[panelIndex].size() > 0)
	{
		string& delStr = *strList.begin();
		d_textLen[panelIndex] -= delStr.length();
		strList.erase(d_msgList[panelIndex].begin());
	}

	if(strList.size() > KUiCfgLoader::getSingleton().getChannelData().maxCentence)
	{
		string& delStr = *strList.begin();
		d_textLen[panelIndex] -= delStr.length();
		strList.erase(d_msgList[panelIndex].begin());
	}

	d_needLayout[panelIndex] = true;
}//*/

void KUiChatRoom::recvMsgFromRoom( BYTE *pMsg )
{
	CEGUI::String strChildName = m_pThisWnd->getName() + "/RecvWnd";
	TLStaticText* pWnd = (TLStaticText* )m_pThisWnd->getChild( strChildName );
	if ( pWnd )
	{
		std::string strMsg;
		
		char szBuff[256];
		sprintf( szBuff, "<Layout width=%f margin-top=0 margin-left=0 margin-right=0 margin-bottom=0>", pWnd->getWidth(Absolute) );
		strMsg = szBuff;		
		
		switch (m_CurBuffIndex)
		{
		case 0:
			{
				m_strMsgBuff[0] += (char*)pMsg;

			    strMsg += m_strMsgBuff[1]+m_strMsgBuff[0];

				if (m_strMsgBuff[0].length()>=2048)
				{
					m_CurBuffIndex=1;	
                    m_strMsgBuff[1]=" ";
				}//end if 

			}//end for 0
			break;
			
		case 1:
			{
				m_strMsgBuff[1] += (char*)pMsg;

				strMsg += m_strMsgBuff[0]+m_strMsgBuff[1];

				if (m_strMsgBuff[1].length()>=2048)
				{
					m_CurBuffIndex=0;
					m_strMsgBuff[0]=" ";
				}//endif 
				
			}//end for 1
			break;
		}//end for switch

		strMsg += "</Layout>";

		ILayout* pLayout = pWnd->getLayout();
		if ( pLayout )
		{
			pLayout->SetText( (char*)strMsg.c_str() );
			pLayout->flashLayout();
			adjustLayoutPos();
			pWnd->requestRedraw();
		}
	}//*/
//Show(m_bPrivate);
}

void KUiChatRoom::addMemberToRoomReq( DWORD dwRoomId, const std::string &strName)
{
 	g_pCoreShell->OperationRequest( GOI_ADDTO_CHATROOM, dwRoomId, (int)strName.c_str() );	
}
 
void KUiChatRoom::kickRoomMember( DWORD dwRoomId, const std::string &strName)
{
	g_pCoreShell->OperationRequest( GOI_KICK_CHATROOM, dwRoomId, (int)strName.c_str() );
}

void KUiChatRoom::onCreateChatRoomNotify( BYTE *pMsg )
{
	m_nRoomID = (int)pMsg;
	if ( d_listBox )
	{
		d_listBox->cleanAllOpenItem();
		KUiPlayerBaseInfo baseInfo;
		g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&baseInfo, NULL );
		TreeItemEx* pItem = (TreeItemEx*)d_listBox->findFirstItemWithText( AnsiToUtf8(baseInfo.Name) );
		if ( !pItem )
		{
			TreeItemEx* item = new TreeItemEx(AnsiToUtf8(baseInfo.Name), m_nRoomID);
			d_itemList.push_back(item);
			d_listBox->addItem( item );
		}

	}
	Show(m_bPrivate);
}

void KUiChatRoom::onJoinRoomNotify( BYTE *pMsg )
{
	PCHAT_JOIN_ROOM pJoinRoom = (PCHAT_JOIN_ROOM)pMsg;
	if (pJoinRoom && d_listBox)
	{
		TreeItemEx* pItem = (TreeItemEx*)d_listBox->findFirstItemWithText( AnsiToUtf8(pJoinRoom->createrName) );
		if ( !pItem )
		{
			TreeItemEx* item = new TreeItemEx(AnsiToUtf8(pJoinRoom->createrName),pJoinRoom->roomId);
			d_itemList.push_back(item);
			d_listBox->addItem( item );
		}
	}
}

void KUiChatRoom::onMemberLeaveRoomNotify( BYTE *pMsg )
{
	PCHAT_LEAVEROOM_NOTIFY pLeave = (PCHAT_LEAVEROOM_NOTIFY)pMsg;
	if ( pLeave && d_listBox)
	{
		TreeItemEx* pItem = (TreeItemEx*)d_listBox->findFirstItemWithText( AnsiToUtf8(pLeave->name) );
		if ( pItem )
		{
			d_listBox->removeItem( pItem );
			TreeItemList::iterator it = d_itemList.begin();
			while ( it != d_itemList.end() )
			{
				if ( *it && *it == pItem )
				{
					delete *it;
					*it = NULL;
					return;
				}
				it++;
			}
		}

	}
}

void KUiChatRoom::onAddRoomMemberNotify( BYTE *pMsg )
{
	PCHATROOM_ADD_MEMBER pAddMember = (PCHATROOM_ADD_MEMBER)pMsg;


	if ( pAddMember && d_listBox )
	{
		TreeItemEx* pItem = (TreeItemEx*)d_listBox->findFirstItemWithText( AnsiToUtf8(pAddMember->name) );
		if ( !pItem )
		{
			TreeItemEx* item = new TreeItemEx(AnsiToUtf8(pAddMember->name), pAddMember->roomId);
			d_itemList.push_back(item);
			d_listBox->addItem( item );
		}

	}
}

void KUiChatRoom::onKickRoomMemberNotify( BYTE *pMsg )
{
	PCHATROOM_KICKMEMBER_NOTIFY pKick = (PCHATROOM_KICKMEMBER_NOTIFY)pMsg;
	if ( pKick && d_listBox)
	{
		TreeItemEx* pItem = (TreeItemEx*)d_listBox->findFirstItemWithText( AnsiToUtf8(pKick->name) );
		if ( pItem )
		{
			d_listBox->removeItem( pItem );
			TreeItemList::iterator it = d_itemList.begin();
			while ( it != d_itemList.end() )
			{
				if ( *it && *it == pItem )
				{
					delete *it;
					*it = NULL;
					return;
				}
				it++;
			}
		}
	}
}

bool KUiChatRoom::handleKeyInput( const CEGUI::EventArgs& args )
{
	int selStart = 0;
	int selEnd = 0;
	d_inputBox->getLayout()->getSelection(selStart, selEnd);

	int delWordNum = selEnd - selStart;

	KeyEventArgs* key = (KeyEventArgs*)&args;
	
	if(key->codepoint < ' ')//如果是输入的控制字符
		return false;

	//\n和\是程序
	if(key->codepoint == '\n' || key->codepoint == '\\')
	{
		return true;
	}

	wchar_t inputWord[2];
	inputWord[0] = key->codepoint;
	inputWord[1] = 0;
	char* ansiWord = NULL;
	unicodeToAnsi(inputWord, ansiWord);
	write(ansiWord);
	delete[] ansiWord;
	ansiWord = NULL;

	return true;
}

void KUiChatRoom::write(LOElemInfo& newElem)
{
	int insertWordCount = 0;
	if(newElem.isShowDes == true || newElem.elemType == LO_IMAGE)
	{
		insertWordCount = 1;
	}
	else
	{
		int insertWordsLen = newElem.content.len();
		for(int i = 0; i <= insertWordsLen; ++i)
		{
			if(newElem.content[i] != '\n' && newElem.content[i] >= ' ')
			{
				newElem.content[insertWordCount++] = newElem.content[i];
			}
		}
		newElem.content[insertWordCount] = 0;
		insertWordCount--;
	}

	int selStart = 0;
	int selEnd = 0;
	d_inputBox->getLayout()->getSelection(selStart, selEnd);
	int delWordCount = selStart > selEnd ? selStart - selEnd : selEnd - selStart;
	
	int maxInsertCount = KUiCfgLoader::getSingleton().getChannelData().inputBoxMaxWordCount - d_inputBox->getLayout()->getWordCount() + delWordCount;
	if(maxInsertCount <= 0)
	{
		return;
	}
	else if(maxInsertCount < insertWordCount)
	{
		newElem.content[maxInsertCount] = 0;
	}
	
	newElem.vAlign = LO_VA_BOTTOM;
	d_inputBox->getLayout()->insertElem(newElem);
	d_inputBox->getLayout()->flashLayout();
	flashColor();
	showText();

	d_inputBox->requestRedraw();
}

bool KUiChatRoom::handleFaceBtnDown( const EventArgs& args )
{
	if(d_facePanel->isVisible())
	{
		d_facePanel->hide();
	}
	else
	{
	//	d_facePanel->setZLevel(Window::Top);
		KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->removeChildWindow(d_facePanel);
	    KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(d_facePanel);
		d_facePanel->show();
		d_facePanel->setZLevel(Window::Top);
	}
	return false;
}

void KUiChatRoom::write(const char* ansiText)
{
	wchar_t* unicodeText = NULL;
	ansiToUnicode(ansiText, unicodeText);
	LOElemInfo newElem;
	newElem.elemType = LO_TEXT;
	newElem.content = unicodeText;
	newElem.description = unicodeText;
	newElem.vAlign = LO_VA_BOTTOM;
	newElem.isShowDes = false;
	
	write(newElem);

	delete[] unicodeText;
	unicodeText = NULL;

	d_inputBox->requestRedraw();
}


void KUiChatRoom::flashColor( void )
{
	//判断是否私聊，如果是则改变其颜色，否则显示当前颜色
	LOElemInfo* elemList;

	int elemCount = d_inputBox->getLayout()->getElemList(elemList);
	if(elemCount <= 0)
	{
		return;
	}

	const wchar_t* firstContent = elemList[0].content.get();
	wchar_t cozeName[COMMON_CLIENT_MSG_LEN_64];
	cozeName[0] = 0;
	swscanf(firstContent, L"/%[^ ] %*[^\0]", cozeName);
	int nameLen = wcslen(cozeName);
	int textLen = wcslen(firstContent);

	if(cozeName[0] != 0 && (nameLen + 2 <= textLen || elemCount > 1))
	{
		const KUiCfgLoader::ChatRoomCfg& cfg = KUiCfgLoader::getSingleton().getChatRoomCfg(); 
		d_inputBox->getLayout()->setColor( cfg.SendColor );
	}

	delete[] elemList;
}

void KUiChatRoom::showText( void )
{
	//这里主要调整layout相对于控件的位置，以保证光标在控件的裁减区域内
	int startIndex = 0;
	int endIndex = 0;
	d_inputBox->getLayout()->getSelection(startIndex, endIndex);

	int caratPosXInLayout = d_inputBox->getLayout()->getPosAtWordIndex(endIndex).x;
	int inputBoxWidth = d_inputBox->getUnclippedPixelRect().getWidth();
	int inputBoxHeight = d_inputBox->getUnclippedPixelRect().getHeight();
	int layoutOffX = d_inputBox->getLayoutOffset().d_x;

	int layoutHeight = d_inputBox->getLayout()->getRenderArea().getHeight();
	if(layoutHeight == 0)
	{
		layoutHeight = 14;//特殊处理，当没有内容的时候则给定一个排版高度
	}
	if(caratPosXInLayout + layoutOffX < 0)
	{
		d_inputBox->setLayoutOffset(-caratPosXInLayout, inputBoxHeight - layoutHeight);
	}
	else if(caratPosXInLayout + layoutOffX > inputBoxWidth)
	{
		d_inputBox->setLayoutOffset(inputBoxWidth - caratPosXInLayout, inputBoxHeight - layoutHeight);
	}
	else
	{
		d_inputBox->setLayoutOffset(layoutOffX, inputBoxHeight - layoutHeight);
	}
}

void KUiChatRoom::clearText( void )
{
	d_inputBox->getLayout()->clearLayout();
	d_inputBox->setLayoutOffset(0, 0);
	d_inputBox->getLayout()->SetText("<Layout width=2500 height=25><Seg text-align=left float=right></Seg></Layout>");

	//裁剪区域必须是相对底板的位置
	Rect textArea = d_inputBox->getUnclippedPixelRect();
	Vector2 posOff = textArea.getPosition() - m_pThisWnd->getUnclippedPixelRect().getPosition();
	textArea.setPosition(posOff);

	LORect clipper;
	cerectToLorect(&textArea, &clipper);
	clipper.setPos(clipper.getLeft(), clipper.getTop() - UI_CHAT_WINDOW_INPUT_LAYOUT_CLIPPER_EXTEND);
	clipper.setHeight(clipper.getHeight() + 100 + UI_CHAT_WINDOW_INPUT_LAYOUT_CLIPPER_EXTEND);
	d_inputBox->getLayout()->setClipper(clipper);
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
template<> 
KUiChatNotify* KUiWndSingleton<KUiChatNotify>::ms_Singleton	= NULL;

KUiChatNotify::KUiChatNotify( const CEGUI::String& id_name ):
KUiWndSingleton<KUiChatNotify>( id_name )
{

}


KUiChatNotify::~KUiChatNotify()
{

}

void KUiChatNotify::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		// Do events wire-up
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatMsgNotify/CallRoomBtn")->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatNotify::handleChatRoom, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatMsgNotify/CallRoomBtn")->subscribeEvent(Window::EventMouseEnters, Event::Subscriber(&KUiChatNotify::handleShowRoomList, ms_Singleton));
	}
}

void KUiChatNotify::Show( void )
{
	KUiWndSingleton<KUiChatNotify>::Show();
	if (ms_Singleton && ms_Singleton->m_pThisWnd)
	{
		((TLStaticImage *)ms_Singleton->m_pThisWnd)->Shake(false,true,4);
	}//endif
}


bool KUiChatNotify::handleChatRoom( const CEGUI::EventArgs& args )
{
	if ( !KUiChatCentre::RecvPrivateChatMsg() )
	{
		if (KUiChatRoomListMenu::GetSingleton().IsNameListEmpty())
		{
			Hide();
		}
	}
	KUiChatRoomListMenu::Hide();
	return true;
}

bool KUiChatNotify::handleShowRoomList(const EventArgs & args)
{
	if (!KUiChatRoomListMenu::GetSingleton().IsVisible() && !KUiChatRoomListMenu::GetSingleton().IsNameListEmpty())
	{
		Point newPos = MouseCursor::getSingleton().getPosition();
		KUiChatRoomListMenu::GetSingleton().SetPos(newPos);
		KUiChatRoomListMenu::Show();
	}
	return true;
}

TreeItemEx::TreeItemEx( const CEGUI::String& text, CEGUI::uint item_id , void* item_data , bool disabled, bool auto_delete ) :
  TLTreeItem( text, item_id, item_data, disabled, auto_delete )
{

}


/************************************************************************/
/*                     Friend center                                    */
/************************************************************************/

const char*	KUiChatCentre::FriendButtonID		= "TaharezLook/ChatCentre/FriendBtn";
const char*	KUiChatCentre::EnemyButtonID		= "TaharezLook/ChatCentre/EnemyBtn";
const char*	KUiChatCentre::ScreenButtonID		= "TaharezLook/ChatCentre/ScreenBtn";
const char*	KUiChatCentre::CharRoomButtonID		= "TaharezLook/ChatCentre/ChatRoomBtn";
const char*	KUiChatCentre::TemporaryButtonID	= "TaharezLook/ChatCentre/TemporaryBtn";
const char*	KUiChatCentre::AddFriendButtonID	= "TaharezLook/ChatCentre/AddFriend";

template<> 
KUiChatCentre* KUiWndSingleton<KUiChatCentre>::ms_Singleton	= NULL;

KUiChatCentre::KUiChatCentre( const CEGUI::String& id_name )
: KUiWndSingleton<KUiChatCentre>( id_name )
, m_pFriendTree(NULL)
, m_pFriendTreeCliper(NULL)
, m_pFriendTreeScrol(NULL)
{
	m_FriendList	= NULL;
	m_EnemyList	= NULL;
	m_ScreenList	= NULL;
	m_ChatRoomList	= NULL;
	m_TemporaryList= NULL;	//*/
	m_pActiveRoom	= NULL;
	m_root = KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT);
}


KUiChatCentre::~KUiChatCentre()
{
	KUiChatRoomSetP2P::iterator it = m_PrivateChatSet.begin();
	for ( ; it != m_PrivateChatSet.end(); ++it )
	{
		if ( (*it).second.pRoom )
		{
			(*it).second.pRoom->ReleaseWnd();
			delete (*it).second.pRoom;
			(*it).second.pRoom = NULL;
		}
	}

	KUiChatRoomSetP2R::iterator cit = m_ChatRoomSet.begin();
    for ( ;cit!=m_ChatRoomSet.end();++cit)
	{
		if ( cit->second.pRoom )
		{   
			cit->second.pRoom->ReleaseWnd();
			delete cit->second.pRoom;
			cit->second.pRoom = NULL;
		}
	}

	if ( m_FriendList )
	{
		static_cast<Tree*>(m_FriendList)->removeAllItem();
	}
	if ( m_EnemyList )
	{
		static_cast<Tree*>(m_EnemyList)	->removeAllItem();
	}
	
	if ( m_ScreenList )
	{
		static_cast<Tree*>(m_ScreenList)->removeAllItem();
	}
	
	if ( m_ChatRoomList )
	{
		static_cast<Tree*>(m_ChatRoomList)->removeAllItem();
	}
	
	if ( m_TemporaryList )
	{
		static_cast<Tree*>(m_TemporaryList)->removeAllItem();	//*/
	}	

	TreeItemList::iterator itFreelist = m_freelist.begin();
	while ( itFreelist != m_freelist.end()	)
	{
		if ( *itFreelist )
		{
			delete *itFreelist;
			*itFreelist = NULL;
		}
		++itFreelist;
	}//*/

	TreeItemList::iterator itFreePopupiter = m_PopupItemList.begin();
	while ( itFreePopupiter != m_PopupItemList.end()	)
	{
		if ( *itFreePopupiter )
		{
			delete *itFreePopupiter;
			*itFreePopupiter = NULL;
		}
		++itFreePopupiter;
	}//*/

	m_PopupItemList.clear();

}

void KUiChatCentre::Init()
{
	if ( ms_Singleton->m_pThisWnd )
	{
		m_IsDraging                    = false;
		ms_Singleton->m_pCurSelectItem = NULL;
		//　初始化所有list
		m_pFriendTreeCliper = m_pThisWnd->getChild("TaharezLook/ChatCentre/FriendListClipper");
		//	m_DragItem          = (TLGameObject *)m_pThisWnd->getChild("TaharezLook/ChatCentre/Drager");  
		m_pFriendTreeScrol  = static_cast<TLVertScrollbar *>(m_pThisWnd->getChild("TaharezLook/ChatCentre/TreeScrollbar"));
		
		//ms_Singleton->m_FriendList		= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/FriendList" );
		ms_Singleton->m_FriendList		= m_pFriendTreeCliper->getChild( "TaharezLook/ChatCentre/FriendListClipper/FriendList" );
		((TLTree *)ms_Singleton->m_FriendList)->setSortingEnabled(false);
		//ms_Singleton->m_EnemyList		= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/EnemyList" );
		ms_Singleton->m_EnemyList		= m_pFriendTreeCliper->getChild( "TaharezLook/ChatCentre/FriendListClipper/EnemyList" );
		((TLTree *)ms_Singleton->m_EnemyList)->setSortingEnabled(false);
		//ms_Singleton->m_ScreenList		= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/ScreenList" );
		ms_Singleton->m_ScreenList		= m_pFriendTreeCliper->getChild( "TaharezLook/ChatCentre/FriendListClipper/ScreenList" );
		((TLTree *)ms_Singleton->m_ScreenList)->setSortingEnabled(false);
		//ms_Singleton->m_TemporaryList	= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/TemporaryList" );
		ms_Singleton->m_TemporaryList	= m_pFriendTreeCliper->getChild( "TaharezLook/ChatCentre/FriendListClipper/TemporaryList" );
		((TLTree *)ms_Singleton->m_TemporaryList)->setSortingEnabled(false);
		
		ms_Singleton->m_ChatRoomList	= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/ChatRoomList" );
		
		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiChatCentre::HandleScrol, ms_Singleton));
		m_pFriendTreeCliper->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiChatCentre::HandleScrol, ms_Singleton));
		
		m_pFriendTreeScrol->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiChatCentre::HandleScrol, ms_Singleton));
		m_pFriendTreeScrol->subscribeEvent(TLVertScrollbar::EventScrollPositionChanged,Event::Subscriber(&KUiChatCentre::handleFriendScroll, ms_Singleton));
		m_FriendList->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiChatCentre::HandleScrol, ms_Singleton));
		m_EnemyList->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiChatCentre::HandleScrol, ms_Singleton));
		m_ScreenList->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiChatCentre::HandleScrol, ms_Singleton));
		m_TemporaryList->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiChatCentre::HandleScrol, ms_Singleton));
		m_ChatRoomList->subscribeEvent(Window::EventMouseWheel, Event::Subscriber(&KUiChatCentre::HandleScrol, ms_Singleton));
		
		//初始化通用弹出对话框
		m_pOkWindow		= (StaticImage*)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/OkWindow" );
		Point pos = m_pOkWindow->getUnclippedPixelRect().getPosition();
		m_pThisWnd->removeChildWindow(m_pOkWindow);
		m_pOkWindow->setRenderMode(false);
		m_root->addChildWindow(m_pOkWindow);
		m_pOkWindow->setPosition(Absolute, pos);
		m_pOkWindow->hide();
		
		//m_pOkWindow->getChild("TaharezLook/ChatCentre/OkWindow/Name")->setVisible(false);
		m_pOkWindow					->subscribeEvent(Window::EventShown, Event::Subscriber(&KUiChatCentre::onOkWndShow, this));
		m_pOkWindow					->subscribeEvent(Window::EventHidden, Event::Subscriber(&KUiChatCentre::onOkWndHide, this));
		m_pOkWindow                 ->getChild("TaharezLook/ChatCentre/OkWindow/Close")->subscribeEvent(Window::EventMouseClick,Event::Subscriber(&KUiChatCentre::onOkWndHide, this));
		m_pOkWindow                 ->setZLevel(Window::Top);
		m_pOkWindow                 ->setRenderMode(false, 3);

		m_OkNormalSize              = ((StaticImage *)(m_pOkWindow))->getImage()->getSize(); 

		m_pOkPopup                  = m_pOkWindow->getChild("TaharezLook/ChatCentre/OkWindow/Popup");
		m_pOkPopup                  ->subscribeEvent(Window::EventMouseClick,Event::Subscriber(&KUiChatCentre::handleOkWindowPopUp , this));
		m_pOkPopup->hide();

		PushButton* OkButton		= (PushButton*)ms_Singleton->m_pOkWindow->getChild("TaharezLook/ChatCentre/OkWindow/Ok");
		PushButton* CancelButton	= (PushButton*)ms_Singleton->m_pOkWindow->getChild("TaharezLook/ChatCentre/OkWindow/Cancel");
		ms_Singleton->m_pName		= (Editbox*)ms_Singleton->m_pOkWindow->getChild("TaharezLook/ChatCentre/OkWindow/Name");
		ms_Singleton->m_pOkWinText	= (StaticText*)ms_Singleton->m_pOkWindow->getChild("TaharezLook/ChatCentre/OkWindow/OkWindowTxt");
		OkButton					->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::OnOk, ms_Singleton));
		CancelButton				->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::OnCancel, ms_Singleton));
		ms_Singleton->m_pName->subscribeEvent(Editbox::EventKeyDown, Event::Subscriber(&KUiChatCentre::handleScreenShortCut, ms_Singleton));
		
		//初始化用户列表弹出菜单项
		ms_Singleton->m_pFriendListMenu	= ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/FriendListMenu" );
		
		ms_Singleton->m_pCommonChat	    = (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/Add");
		ms_Singleton->m_pChat			= (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/Chat");
		ms_Singleton->m_pInvite			= (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/Invite");
		ms_Singleton->m_pFriendDetail	= (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/FriendDetail");
		ms_Singleton->m_pPreventChat	= (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/PreventChat");
		ms_Singleton->m_pDeleteFriend	= (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/DeleteFriend");
		ms_Singleton->m_pPlayerInfo		= (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/PlayerInfo");
		ms_Singleton->m_pInviteShizu    = (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/AddToShizu");
		ms_Singleton->m_pInviteZhuhou   = (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/AddToZhuhou");
		ms_Singleton->m_pOpenRoom		= (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/OpenRoom");
		ms_Singleton->m_pLeaveRoom		= (PushButton*)ms_Singleton->m_pFriendListMenu->getChild("TaharezLook/ChatCentre/FriendListMenu/LeaveRoom");
		ms_Singleton->m_pAddToFriend    = (PushButton*)ms_Singleton->m_pFriendListMenu->getChild( "TaharezLook/ChatCentre/FriendListMenu/AddToFriend" );
		ms_Singleton->m_pAddToSepecificGroup = (PushButton*)ms_Singleton->m_pFriendListMenu->getChild( "TaharezLook/ChatCentre/FriendListMenu/AddToSpecificGroup" );
		
		ms_Singleton->m_pCommonChat		->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickCommonChat, ms_Singleton));
		ms_Singleton->m_pChat			->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickChat, ms_Singleton));
		ms_Singleton->m_pInvite			->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickInvite, ms_Singleton));
		ms_Singleton->m_pFriendDetail	->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickFriendDetail, ms_Singleton));
		ms_Singleton->m_pPreventChat	->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickPreventChat, ms_Singleton));
		ms_Singleton->m_pDeleteFriend	->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickDeleteFriend, ms_Singleton));
		ms_Singleton->m_pPlayerInfo		->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickPlayerInfo, ms_Singleton));
		ms_Singleton->m_pOpenRoom		->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickOpenRoom, ms_Singleton));
		ms_Singleton->m_pLeaveRoom		->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickLeaveRoom, ms_Singleton));
	    ms_Singleton->m_pAddToFriend    ->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickAddToFriend, ms_Singleton));
        ms_Singleton->m_pAddToSepecificGroup->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickAddToSpecificGroup, ms_Singleton));
		ms_Singleton->m_pInviteShizu->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickInviteShizu, ms_Singleton));
		ms_Singleton->m_pInviteZhuhou->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickInviteZhuhou, ms_Singleton));
       

		ms_Singleton->m_pFriendListMenu	->setZLevel(Window::Top);
		ms_Singleton->m_pFriendListMenu ->subscribeEvent(Window::EventShown,Event::Subscriber(&KUiChatCentre::ListMenuShow, ms_Singleton));
		ms_Singleton->m_pFriendListMenu ->subscribeEvent(Window::EventHidden,Event::Subscriber(&KUiChatCentre::ListMenuHide, ms_Singleton));
		
		//初始化用户分组弹出菜单项
		ms_Singleton->m_pFriendGroupMenu	= (Window*)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/FriendGroupMenu" );
		ms_Singleton->m_pAddGroup			= (PushButton*)ms_Singleton->m_pFriendGroupMenu->getChild("TaharezLook/ChatCentre/FriendGroupMenu/AddGroup");
		ms_Singleton->m_pRenameGroup		= (PushButton*)ms_Singleton->m_pFriendGroupMenu->getChild("TaharezLook/ChatCentre/FriendGroupMenu/RenameGroup");
		ms_Singleton->m_pDeleteGroup		= (PushButton*)ms_Singleton->m_pFriendGroupMenu->getChild("TaharezLook/ChatCentre/FriendGroupMenu/DeleteGroup");
		ms_Singleton->m_pAddToGroup			= (PushButton*)ms_Singleton->m_pFriendGroupMenu->getChild("TaharezLook/ChatCentre/FriendGroupMenu/AddToGroup");
		
		
		ms_Singleton->m_pAddGroup			->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickAddGroup, ms_Singleton));
		ms_Singleton->m_pRenameGroup		->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickRenameGroup, ms_Singleton));
		ms_Singleton->m_pDeleteGroup		->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickDeleteGroup, ms_Singleton));
		ms_Singleton->m_pAddToGroup			->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickAddToGroup, ms_Singleton));
		
		ms_Singleton->m_pFriendGroupMenu	->setZLevel(Window::Top);
		ms_Singleton->m_pFriendGroupMenu    ->subscribeEvent(Window::EventShown,Event::Subscriber(&KUiChatCentre::ListMenuShow, ms_Singleton));
		ms_Singleton->m_pFriendGroupMenu    ->subscribeEvent(Window::EventHidden,Event::Subscriber(&KUiChatCentre::ListMenuHide, ms_Singleton));
		
		//初始化按钮
		ms_Singleton->m_pAddFriend	 = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatCentre/AddFriend");
		ms_Singleton->m_pDelFriend	 = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatCentre/DelFriend");
		ms_Singleton->m_pInvFriend	 = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatCentre/InvFriend");
		ms_Singleton->m_pDelEnemy	 = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatCentre/DelEnemy");
		ms_Singleton->m_pAddEnemy	 = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatCentre/AddEnemy");
		ms_Singleton->m_pAddScreen	 = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatCentre/AddScreen");
		ms_Singleton->m_pDelScreen	 = (PushButton*)ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatCentre/DelScreen");
		ms_Singleton->m_pCreateRoom	 = (PushButton*)ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/CreateRoom" );

		ms_Singleton->m_pAddFriend	->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickAddFriendBtn, ms_Singleton));
		ms_Singleton->m_pDelFriend	->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickDelFriendBtn, ms_Singleton));
		ms_Singleton->m_pInvFriend	->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickInvite, ms_Singleton));
		ms_Singleton->m_pAddEnemy	->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickAddEnemyBtn, ms_Singleton));
		ms_Singleton->m_pDelEnemy	->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickDelEnemyBtn, ms_Singleton));
		ms_Singleton->m_pAddScreen	->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickAddScreenBtn, ms_Singleton));
		ms_Singleton->m_pDelScreen	->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickDelScreenBtn, ms_Singleton));
		ms_Singleton->m_pCreateRoom	->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::handleCreateRoom, ms_Singleton));
		ms_Singleton->m_pThisWnd	->getChild("TaharezLook/ChatCentre/Close")->subscribeEvent(PushButton::EventMouseClick, Event::Subscriber(&KUiChatCentre::clickCloseBtn, ms_Singleton));

		// Do events wire-up
		ms_Singleton->m_pThisWnd->getChild(FriendButtonID)->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::showFriendList, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(EnemyButtonID)->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::showEnemyList, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(ScreenButtonID)->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::showScreenList, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(CharRoomButtonID)->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::showChatList, ms_Singleton));
		ms_Singleton->m_pThisWnd->getChild(TemporaryButtonID)->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::showTemporaryList, ms_Singleton));
		
		m_FriendList->subscribeEvent(TLTreeEx::TR_EventNodeRightButtonUp, Event::Subscriber(&KUiChatCentre::clickFListMenu, ms_Singleton));
		m_FriendList->subscribeEvent(TLTreeEx::TR_EventDoubleClick,Event::Subscriber(&KUiChatCentre::freindlistDC, ms_Singleton));

		m_EnemyList->subscribeEvent(TLTreeEx::TR_EventNodeRightButtonUp, Event::Subscriber(&KUiChatCentre::clickFListMenu, ms_Singleton));
		m_EnemyList->subscribeEvent(TLTreeEx::TR_EventDoubleClick,Event::Subscriber(&KUiChatCentre::freindlistDC, ms_Singleton));

		m_ScreenList->subscribeEvent(TLTreeEx::TR_EventNodeRightButtonUp, Event::Subscriber(&KUiChatCentre::clickFListMenu, ms_Singleton));
		
		m_TemporaryList->subscribeEvent(TLTreeEx::TR_EventNodeRightButtonUp, Event::Subscriber(&KUiChatCentre::clickFListMenu, ms_Singleton));
		m_TemporaryList->subscribeEvent(TLTreeEx::TR_EventDoubleClick,Event::Subscriber(&KUiChatCentre::freindlistDC, ms_Singleton));

		m_ChatRoomList->subscribeEvent(TLTreeEx::TR_EventNodeRightButtonUp, Event::Subscriber(&KUiChatCentre::clickFListMenu, ms_Singleton));
		m_ChatRoomList->subscribeEvent(TLTreeEx::EventMouseDoubleClick    , Event::Subscriber(&KUiChatCentre::handleDCList, ms_Singleton));
		
		ms_Singleton->m_pThisWnd->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::handleMouseClick, ms_Singleton));
		m_pFriendTreeCliper->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiChatCentre::handleMouseClick, ms_Singleton));
		//ms_Singleton->ms_Singleton->m_pThisWnd->getChild("TaharezLook/ChatCentre/FriendList")->subscribeEvent(TLTreeEx::EventNodeLeftButtonUp, Event::Subscriber(&KUiChatCentre::closeFListMenu, ms_Singleton));
		m_State=friendPage;
		
		GlobalEventSet::getSingleton().subscribeEvent(Window::EventGlobalDragTreeItemAccept,     Event::Subscriber(&KUiChatCentre::handleAcceptInList, this));
		GlobalEventSet::getSingleton().subscribeEvent(Window::EventGlobalDragTreeItemMove,     Event::Subscriber(&KUiChatCentre::handleDragItemMove, this));
		
		m_Popup1=(TLTreeEx *)ms_Singleton->m_pOkWindow->getChild("TaharezLook/ChatCentre/OkWindow/Popup1");
		m_Popup1->hide();

		m_PopupBack = (TLStaticImage * )ms_Singleton->m_pOkWindow->getChild("TaharezLook/ChatCentre/OkWindow/Popupback");
		m_PopupBack->hide();

		GlobalEventSet::getSingleton().subscribeEvent(Window::EventMouseClick,     Event::Subscriber(&KUiChatCentre::handleMouseClickForPopup, this));

        m_PopupState = PS_INVALID;
}    
}

bool KUiChatCentre::ListMenuShow(const EventArgs& args )
{
	disableAllList();
	return true;
}

bool KUiChatCentre::ListMenuHide(const EventArgs& args )
{
    enableAllList();
	return true;
}

bool KUiChatCentre::handleOkWindowPopUp(const EventArgs& args)
{

	switch (m_PopupState)
	{
	case PS_FRIENDLIST:
		{
			prepareForFriendPopUpList();
		}//end for list
		break;
	
	case PS_GROUPLIST:
		{
            prepareForGroupPopUpList();
		}
		break;

	default:break;
	}//end for switch
	
	if (m_Popup1->getItemCount())
	{
		m_Popup1->scratchWindowDueItems();
		
		Size      PopupSize = m_Popup1->getSize(Absolute);
		Point     Pos       = m_Popup1->getPosition(Absolute);
		
		Size      newSize   = m_OkNormalSize;
		newSize.d_height    = m_Popup1->getPosition(Absolute).d_y + PopupSize.d_height+50;
		
		m_pOkWindow->setSize(Absolute,newSize);
		m_pOkWindow->removeChildWindow(m_Popup1);
		m_pOkWindow->addChildWindow(m_Popup1);
        m_pOkWindow->requestRedraw();
		
	}//endif

	m_PopupBack->setSize(Absolute,m_Popup1->getAbsoluteSize());
	m_PopupBack->setPosition(Absolute,m_Popup1->getAbsolutePosition());
	m_PopupBack->show();
	m_PopupBack->setZLevel(Window::Top);

    m_Popup1->setZLevel(Window::SuperTop);
	m_Popup1->show();

	return true;
}

bool KUiChatCentre::handleMouseClickForPopup(const EventArgs& args)
{
	const MouseEventArgs & mouseArgs = (const MouseEventArgs &)  args;
	
	if (mouseArgs.button == LeftButton)
	{
		Point localPos(CEGUI::CoordConverter::screenToWindow(*m_pThisWnd, mouseArgs.position));
        
		if (ms_Singleton->m_Popup1 && ms_Singleton->m_Popup1->isVisible(true))	
		{
			if (ms_Singleton->m_PopupBack)
				m_PopupBack->hide();

            if (ms_Singleton->m_Popup1->getItemAtCurMouse())
			{
                handleClickInPop1();
			}//endif
			else
			{
				ms_Singleton->m_pOkWindow->setSize(Absolute,m_OkNormalSize);
				ms_Singleton->m_Popup1->hide();
			}//endif

		}//endif

		if (ms_Singleton->m_pFriendGroupMenu && ms_Singleton->m_pFriendGroupMenu->isVisible(true))
		{
			if (!posInRect(localPos,ms_Singleton->m_pFriendGroupMenu->getRect(Absolute)))
			{
				ms_Singleton->m_pFriendGroupMenu->hide();
			}//endif

		}//endif

		if (ms_Singleton->m_pFriendListMenu && ms_Singleton->m_pFriendListMenu->isVisible(true))
		{
			if (!posInRect(localPos,ms_Singleton->m_pFriendListMenu->getRect(Absolute)))
			{
				ms_Singleton->m_pFriendListMenu->hide();
			}//endif
			
		}//endif

	}//endif

	return false;
}

void KUiChatCentre::handleClickInPop1       (void)
{
	TreeItemEx * pItem=(TreeItemEx *)m_Popup1->getFirstSelectedItem();
	
	if (pItem && m_pName)
	{
		m_pName->setText(pItem->getText());
	}//endif

	m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	m_Popup1->hide();
}

void KUiChatCentre::ResetPopupWindow        (void)
{
   m_Popup1->removeAllItem();
}

void KUiChatCentre::AddToPupupWindow(const TreeItemEx * pItem)
{
   int nPopupSize        = m_Popup1->getItemCount();
   int nTotalAvailable   = m_PopupItemList.size();

   TreeItemEx * pNewItem = NULL;

   if (nPopupSize < nTotalAvailable)
   {
       pNewItem = m_PopupItemList[nPopupSize];
	   pNewItem ->setText(pItem->getText());
   }//endif
   else
   {
	   pNewItem = new TreeItemEx( pItem->getText() ) ;
	   m_PopupItemList.push_back(pNewItem);
   }//end else
   
   m_Popup1->addItem(pNewItem);

}

void KUiChatCentre::prepareForFriendPopUpList()
{
	if (m_Popup1 && m_FriendList)
	{
		ResetPopupWindow();
		//m_Popup1->removeAllItem();
		int nCount = ((TLTree *)m_FriendList)->getItemCount();
		if (nCount)
		{
			KUiCfgLoader& cfgLoader = KUiCfgLoader::getSingleton();
			for (int index=0;index<nCount;index++)
			{
				TreeItemEx * pItem = (TreeItemEx *)((TLTree *)m_FriendList)->getItemFromIndex(index);
				
				if (pItem->getItemCount())
				{
					for (int subidx=0;subidx<pItem->getItemCount();subidx++)
					{
						TreeItemEx * pSubItem =(TreeItemEx *) pItem->getTreeItemFromIndex(subidx); 
						AddToPupupWindow((TreeItemEx *)pItem->getTreeItemFromIndex(subidx));
					}//end for subidx
					
				}//endif
				
			}//end for index
		}//endif
	}//endif
}

void KUiChatCentre::prepareForGroupPopUpList()
{
	if (m_Popup1 && m_FriendList)
	{
		//m_Popup1->removeAllItem();
		ResetPopupWindow();

		int nCount = ((TLTree *)m_FriendList)->getItemCount();
		if (nCount)
		{
           for (int nIdx=0;nIdx<nCount;nIdx++)
		   {
			   TLTreeItem * pTreeItem = (TLTreeItem *)((TLTree *)m_FriendList)->getItemFromIndex(nIdx);
			   pTreeItem->setLayer(0);

			   AddToPupupWindow((TreeItemEx *)pTreeItem);
			 //  m_Popup1->addItem(pTreeItem);
		   }//end for nIdx

		}//endif

	}//endif
}

bool KUiChatCentre::handleDragItemMove(const EventArgs& args )
{
	GlobalDragTreeItemArgs & DragArgs = (GlobalDragTreeItemArgs &) args;
	Point e = MouseCursor::getSingleton().getPosition();
    Point localPos(CEGUI::CoordConverter::screenToWindow(*m_pThisWnd, e));
	    
	if (DragArgs.d_ParentTree == ms_Singleton->m_FriendList || DragArgs.d_ParentTree == ms_Singleton->m_EnemyList || DragArgs.d_ParentTree == ms_Singleton->m_ScreenList || DragArgs.d_ParentTree == m_TemporaryList)
	{
		if (posInRect(localPos,ms_Singleton->m_pThisWnd->getChild(FriendButtonID)->getRect(Absolute)))
		{
			((TLButton *)ms_Singleton->m_pThisWnd->getChild(FriendButtonID))->setHoverState(true);
			((TLButton *)ms_Singleton->m_pThisWnd->getChild(FriendButtonID))->setHoverLock(true);
		
			CEGUI::EventArgs args;
			showFriendList(args);
		}//endif
		else
		{
            ((TLButton *)ms_Singleton->m_pThisWnd->getChild(FriendButtonID))->setHoverState(false);
            ((TLButton *)ms_Singleton->m_pThisWnd->getChild(FriendButtonID))->setHoverLock(false);
		}

		if (posInRect(localPos,ms_Singleton->m_pThisWnd->getChild(EnemyButtonID)->getRect(Absolute)))
		{
            ((TLButton *)ms_Singleton->m_pThisWnd->getChild(EnemyButtonID))->setHoverState(true);
			((TLButton *)ms_Singleton->m_pThisWnd->getChild(EnemyButtonID))->setHoverLock(true);

			CEGUI::EventArgs args;
			showEnemyList(args);
		}
		else
		{
            ((TLButton *)ms_Singleton->m_pThisWnd->getChild(EnemyButtonID))->setHoverState(false);
			((TLButton *)ms_Singleton->m_pThisWnd->getChild(EnemyButtonID))->setHoverLock(false);
		}

		if (posInRect(localPos,ms_Singleton->m_pThisWnd->getChild(ScreenButtonID)->getRect(Absolute)))
		{
            ((TLButton *)ms_Singleton->m_pThisWnd->getChild(ScreenButtonID))->setHoverState(true);
			((TLButton *)ms_Singleton->m_pThisWnd->getChild(ScreenButtonID))->setHoverLock(true);

			CEGUI::EventArgs args;
			showScreenList(args);
		}
		else
		{
             ((TLButton *)ms_Singleton->m_pThisWnd->getChild(ScreenButtonID))->setHoverState(false);
			 ((TLButton *)ms_Singleton->m_pThisWnd->getChild(ScreenButtonID))->setHoverLock(false);
		}


	}//endif

	return false;
}

bool KUiChatCentre::handleAcceptInList(const EventArgs& args )
{
	   std::string  tempString;

       GlobalDragTreeItemArgs & ItemArgs = (GlobalDragTreeItemArgs &) args;
    
	   ((TLButton *)ms_Singleton->m_pThisWnd->getChild(ScreenButtonID))->setHoverLock(false);
       ((TLButton *)ms_Singleton->m_pThisWnd->getChild(EnemyButtonID))->setHoverLock(false);
       ((TLButton *)ms_Singleton->m_pThisWnd->getChild(FriendButtonID))->setHoverLock(false);
	   
	   if (ItemArgs.d_ParentTree)
	   {
		   TreeItemEx* pLastItem   =   NULL;
		   int         nRoomID     =      0;

		   //Comes from friend list page
           if (ItemArgs.d_ParentTree==ms_Singleton->m_FriendList)
		   {
              TLTreeEx * pFriendList = (TLTreeEx *)ms_Singleton->m_FriendList;
			 
			  pLastItem              = (TreeItemEx*)pFriendList->findFirstItemWithText(ItemArgs.d_ItemText);
             
			  //Updata for Group change
			  if (pLastItem && pLastItem->getLayer()>0)  //Friend member only, different from group
			  {
				  nRoomID            = pLastItem->getIDEx();

				  if (m_State== friendPage)
				  {
					  TreeItemEx * pDestItemList = (TreeItemEx*)pFriendList->getItemAtCurMouse();
					  
					  if (pDestItemList) //Add to another group
					  {
						  int nParam[2];
						  nParam[0]=pLastItem->getIDEx();
						  nParam[1]=pDestItemList->getIDEx();
						  
						  g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)Utf8ToAnsi(ItemArgs.d_ItemText), (int)&nParam ); 
					      return true;
					  }//endif
					  
				  }//end for case

			  }//endif

		   }//endif
		   else if (ItemArgs.d_ParentTree==ms_Singleton->m_EnemyList)
		   {
			    TLTreeEx * pEnemyList  = (TLTreeEx *)ms_Singleton->m_EnemyList;
                pLastItem              = (TreeItemEx*)pEnemyList->findFirstItemWithText(ItemArgs.d_ItemText);
				
				if (pLastItem)
					nRoomID            = GROUPID_ENEMY;
		   }//end else
		   else if (ItemArgs.d_ParentTree==ms_Singleton->m_ScreenList)
		   {
			   TLTreeEx * pScreen  = (TLTreeEx *)ms_Singleton->m_ScreenList;
			   pLastItem              = (TreeItemEx*)pScreen->findFirstItemWithText(ItemArgs.d_ItemText);
			   
			   if (pLastItem)
					nRoomID            = GROUPID_BLACK;
		   }//end else
		   else if (ItemArgs.d_ParentTree==ms_Singleton->m_TemporaryList)
		   {
			   TLTreeEx * pTemp  = (TLTreeEx *)ms_Singleton->m_TemporaryList;
			   pLastItem              = (TreeItemEx*)pTemp->findFirstItemWithText(ItemArgs.d_ItemText);
			   
			   if (pLastItem)
					nRoomID            = GROUPID_TEMP;
		   }//end else

		   if (ItemArgs.d_ItemText == "")
			   return false;

		   char  szName[32] = "";
		   sprintf(szName,Utf8ToAnsi(ItemArgs.d_ItemText));

		   if (nRoomID == GROUPID_ENEMY )
		   {
			   int nLen = strlen(szName);
			   for (int comp = 0 ; comp < nLen && szName[comp]; comp ++ )
			   {
				   if (szName[comp] == ' ')
				   {
					   szName[comp] = 0;
					   break;
				   }//endif
				   
			   }//endif
			   
		   }//endif
		      
		   if (pLastItem && nRoomID)
		   {
			   Point e = MouseCursor::getSingleton().getPosition();
               Point localPos(CEGUI::CoordConverter::screenToWindow(*m_pThisWnd, e));

			   //Test for the page button
			   if (m_pActiveRoom && m_pActiveRoom->m_bRoomChat && m_pActiveRoom->IsVisible())
			   {
				   Rect roomRect = m_pActiveRoom->m_pThisWnd->getRect(Absolute);
				   
				   if (pLastItem && posInRect(e,roomRect))
				   {
					   tempString = szName;
					   m_pActiveRoom->addMemberToRoomReq(m_pActiveRoom->m_nRoomID,tempString);
					   return true;
				   }//endif
				   
			   }//endif

			   if (m_State== friendPage && ( posInRect(localPos,ms_Singleton->m_pThisWnd->getChild(FriendButtonID)->getRect(Absolute)) 
				   || posInRect(localPos,m_pFriendTreeCliper->getRect(Absolute)) ))
			   {
				   TLTreeEx   * pFriendList = (TLTreeEx *)ms_Singleton->m_FriendList;
				   TreeItemEx * pDestItemList = (TreeItemEx*)pFriendList->getItemAtCurMouse();
				   
				   if (pDestItemList) //Add to another group
				   {
					   int nParam[2];
					   nParam[0]=nRoomID;
					   nParam[1]=pDestItemList->getIDEx();
					   
					   g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)szName, (int)&nParam ); 
					   return true;
				   }//endif
				   else
				   {
					   
					   int nParam[2];
					   nParam[0]=nRoomID;
					   nParam[1]=GROUPID_NONE;
					   
					   //if (nRoomID!=GROUPID_TEMP)
					   g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)szName, (int)&nParam ); 
					   
				   }//endif

				   return true;
			   }
			   else if (m_State== enemyPage && ( posInRect(localPos,ms_Singleton->m_pThisWnd->getChild(EnemyButtonID)->getRect(Absolute)) 
				   || posInRect(localPos,m_pFriendTreeCliper->getRect(Absolute)) ))
			   {
				   int nParam[2];
				   nParam[0]=nRoomID;
				   nParam[1]=GROUPID_ENEMY;
				   
				   //if (nRoomID!=GROUPID_TEMP)
				    g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)szName, (int)&nParam ); 
                   /*else
				   {
					   g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_ADD ,(unsigned int)Utf8ToAnsi(ItemArgs.d_ItemText),GROUPID_NONE );
					    ((TLTreeEx *)m_TemporaryList)->removeItem(pLastItem);   
				   }//end else
                   */
				   return true;

			   }
			   else if (m_State== screenPage && ( posInRect(localPos,ms_Singleton->m_pThisWnd->getChild(ScreenButtonID)->getRect(Absolute)) 
				   || posInRect(localPos,m_pFriendTreeCliper->getRect(Absolute)) ))
			   {
				   int nParam[2];
				   nParam[0]=nRoomID;
				   nParam[1]=GROUPID_BLACK;
				   
				   //if (nRoomID!=GROUPID_TEMP)
				   g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)szName, (int)&nParam ); 
                   /*else
				   {
					   g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_ADD ,(unsigned int)Utf8ToAnsi(ItemArgs.d_ItemText),GROUPID_NONE );
					   ((TLTreeEx *)m_TemporaryList)->removeItem(pLastItem);   
				   }//end else
                   */

				   return true;
			   }
			   
		   }//endif

	   }//endif

	return false;
}

bool   KUiChatCentre::posInRect                 (const Point    & pos ,const Rect & rect )
{
	return (pos.d_x>=rect.d_left && pos.d_x<=rect.d_right && pos.d_y>=rect.d_top && pos.d_y<=rect.d_bottom);
}

bool KUiChatCentre::HandleScrol(const EventArgs& args )
{
    if (ms_Singleton)
	{
		MouseEventArgs* eventArgs = (MouseEventArgs*)&args;
		
		if(m_pFriendTreeScrol->isVisible())
		{
			m_pFriendTreeScrol->setScrollPosition(m_pFriendTreeScrol->getScrollPosition()
				- m_pFriendTreeScrol->getStepSize() * eventArgs->wheelChange);
		}
	}//endif

	return true;
}

void KUiChatCentre::Hide()
{
	if (!ms_Singleton || !ms_Singleton->m_pThisWnd)
		return ;

	if (ms_Singleton->m_pOkWindow && ms_Singleton->m_pOkWindow->isVisible(true))
	{
		ms_Singleton->m_pOkWindow->hide();
		ms_Singleton->m_pOkWindow->setSize(Absolute,ms_Singleton->m_OkNormalSize);
	}//endif
	else
	{
		KUiWndSingleton<KUiChatCentre>::Hide();
		if (ms_Singleton->m_pActiveRoom && ms_Singleton->m_pActiveRoom->IsVisible())
			ms_Singleton->m_pActiveRoom = NULL;
	}
}

void KUiChatCentre::Show()
{
	KUiWndSingleton<KUiChatCentre>::Show();
	
	if ( ms_Singleton->m_pThisWnd )
	{
		ms_Singleton->onRecvFriendListArrive();
		const CEGUI::EventArgs args;
		switch(ms_Singleton->m_State)
		{
		case friendPage:
			ms_Singleton->showFriendList(args);
			break;

		case enemyPage:
			ms_Singleton->showEnemyList(args);
			break;
			
		case chatroomPage:
			ms_Singleton->showChatList(args);
			break;

		case screenPage:
			ms_Singleton->showScreenList(args);
			break;

		case temporaryPage:
			ms_Singleton->showTemporaryList(args);
			break;
		}
	   /*ms_Singleton->m_pFriendTreeCliper->show();
		ms_Singleton->m_pAddFriend->show();
		ms_Singleton->m_pDelFriend->show();
		ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/HideSelf" )->hide();
		//ms_Singleton->m_pThisWnd->getChild( "TaharezLook/ChatCentre/RoleName" )->hide();
		ms_Singleton->m_pFriendListMenu->hide();
		ms_Singleton->m_pFriendGroupMenu->hide();
		*/

	//	ms_Singleton->m_State = friendPage;
	}	
}

void KUiChatCentre::ProcessChatNotify(DWORD dwNotifyID, DWORD dwParam,  BYTE *pMsg )
{
	int n = 0;
	switch( dwNotifyID )
	{
	case PrivateChatNotify:
		ms_Singleton->onRecvMsgFromSomeone( (PCHATMSG_BY_NAME)dwParam );
		KUiNaviationEx::GetSingleton().ActiveButton(6);
		break;
	case RoomChatNotify:
		{
			KUiChatRoomSetP2R::iterator it = ms_Singleton->m_ChatRoomSet.find(dwParam);
			if ( it!= ms_Singleton->m_ChatRoomSet.end() && it->second.pRoom &&  it->second.pRoom->m_nRoomID == dwParam )
			{
				const KUiCfgLoader::ChatRoomCfg& cfg = KUiCfgLoader::getSingleton().getChatRoomCfg(); 
				
				CHATROOMMSG_TO_SOMEONE* pData = (CHATROOMMSG_TO_SOMEONE*)pMsg;
				KChatMsg tagChatMsg;
				tagChatMsg.dwRoomID		= -1;
				tagChatMsg.strSender	= pData->senderName;
				
				char segText[LAYOUT_TEXT_MAX_LEN + 1];
				
				segText[0] = 0;
				strcat(segText, "<Seg float=wrap>");
				//名称
				if(pData->senderName != NULL && pData->senderName[0] != 0)
				{
					strcat(segText, "<Obj type=text vertical-align=bottom show-des=true color=");
					strcat(segText, cfg.p2rNameColor);
					strcat(segText, " font-family=");
					strcat(segText, cfg.p2rNameFont);
					strcat(segText, " des=[");
					strcat(segText, pData->senderName);
					strcat(segText, "]:>");
					strcat(segText, "[");
					strcat(segText, pData->senderName);
					strcat(segText, "]:</Obj></Seg><Seg float=wrap>");
				}
				
				if(pData->msg[0] == '<')
				{
					ms_Singleton->chatTextToLoelem((const char*)&pData->msg, segText,0, false);
				}
				else	//系统消息可能不带标签
				{
					static char sysMsg[COMMON_CLIENT_MSG_LEN_256];
					sprintf(sysMsg, "<N= %s>", (const char*)&pData->msg);
					ms_Singleton->chatTextToLoelem(sysMsg, segText,0, false);
				}
				
				strcat(segText, "</Seg>");
				
				tagChatMsg.strMsg = segText;
				it->second.pRoom->recvMsgFromRoom( (BYTE*)segText );
				ms_Singleton->setActiveChatRoom( it->second.pRoom );
				
				//	AddChatRoom(ms_Singleton->m_ChatRoomSet[n].pRoom->GetId(),dwParam);
				KUiNaviationEx::GetSingleton().ActiveButton(6);
				break;
			}
		}
		break;
	case CreateChatRoomNotify:
		ms_Singleton->onCreateChatRoomNotify( dwParam, pMsg );
		break;
	case AddRoomMemberNotify:
		{
			KUiChatRoomSetP2R::iterator it = ms_Singleton->m_ChatRoomSet.find(dwParam);
			
			if (it!=ms_Singleton->m_ChatRoomSet.end() && it->second.pRoom &&  it->second.pRoom->m_nRoomID == dwParam )
			{
				it->second.pRoom->onAddRoomMemberNotify( pMsg );
				ms_Singleton->setActiveChatRoom( it->second.pRoom);
				return;
			}
		}
		break;		
 	case JoinRoomMemberNotify:
		{
			PCHAT_JOIN_ROOM pJoinRoom = (PCHAT_JOIN_ROOM) pMsg;
			if ( pJoinRoom )
			{
				KUiChatRoomSetP2R::iterator it = ms_Singleton->m_ChatRoomSet.find( dwParam );
				if ( it != ms_Singleton->m_ChatRoomSet.end() && (*it).second.pRoom )
				{
						(*it).second.pRoom->onJoinRoomNotify( pMsg );
					//	ms_Singleton->setActiveChatRoom( (*it).second.pRoom );
					//	(*it).second.pRoom->Show(false);
				}
				else
				{
					ms_Singleton->m_ChatRoomSet[dwParam].pRoom = new KUiChatRoom ;
					if (ms_Singleton->m_ChatRoomSet[dwParam].pRoom)
					{
						KUiPlayerBaseInfo tagRoleInfo;
						g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&tagRoleInfo, NULL );
						char szRoomName[64];
						sprintf( szRoomName, "%s/P2R/%d", tagRoleInfo.Name, dwParam );
						ms_Singleton->m_ChatRoomSet[dwParam].pRoom->CreateWnd( UI_CHATROOM, szRoomName );
						ms_Singleton->m_ChatRoomSet[dwParam].pRoom->Hide();
						ms_Singleton->m_ChatRoomSet[dwParam].pRoom->m_nRoomID = dwParam;
						ms_Singleton->m_ChatRoomSet[dwParam].pRoom->AddEvent();
						ms_Singleton->m_ChatRoomSet[dwParam].pRoom->setChatMode( true );
                        ms_Singleton->m_ChatRoomSet[dwParam].pRoom->onJoinRoomNotify( pMsg );
					//	ms_Singleton->setActiveChatRoom( ms_Singleton->m_ChatRoomSet[dwParam].pRoom);
					//	ms_Singleton->m_ChatRoomSet[dwParam].pRoom->Show(false);
					}
				}//end else

				ms_Singleton->AddChatRoom( pJoinRoom->roomName, pJoinRoom->roomId );
			}
		}
 		break;
	case KickMemberNotify:
		{
			KUiChatRoomSetP2R::iterator it = ms_Singleton->m_ChatRoomSet.find(dwParam);
			if (it!= ms_Singleton->m_ChatRoomSet.end() && it->second.pRoom &&  it->second.pRoom->m_nRoomID == dwParam )
			{
				it->second.pRoom->onKickRoomMemberNotify( pMsg );
				ms_Singleton->setActiveChatRoom( it->second.pRoom);
				ms_Singleton->DelChatRoom( dwParam );
				return;
			}	
		}
		break;
	case LeaveMemberNotify:
		{
			KUiChatRoomSetP2R::iterator it = ms_Singleton->m_ChatRoomSet.find(dwParam);
			if (it!= ms_Singleton->m_ChatRoomSet.end() && it->second.pRoom &&  it->second.pRoom->m_nRoomID == dwParam )
			{
				it->second.pRoom->onMemberLeaveRoomNotify( pMsg );
				ms_Singleton->setActiveChatRoom( it->second.pRoom);
				return;
			}
		}
		break;
	case ChangeRoomOwnerNotify:
		{
			if ( ms_Singleton->m_ChatRoomSet.find(dwParam)!=ms_Singleton->m_ChatRoomSet.end() && ms_Singleton->m_ChatRoomSet[dwParam].pRoom)
			{
				bool bOwner = g_pCoreShell->GetGameData(GDI_CHAT_IS_OWNER,dwParam,0) == FALSE ? false : true;
				ms_Singleton->m_ChatRoomSet[dwParam].pRoom->setOwnerState(bOwner);
			}//endif
		}
		break;
	}	
}

void KUiChatCentre::ProcessFriendNotify( DWORD dwNotifyID, BYTE *pMsg )
{
	int n = 0;
	switch( dwNotifyID )
	{
	case AddFriendReturnNotify:
		{
			ms_Singleton->onRecvFriendListArrive();
		}
		break;
	}	
}

int KUiChatCentre::RecvPrivateChatMsg()
{
	KChatMsgList::iterator it = ms_Singleton->m_ChatMsgList.begin();
	if ( it != ms_Singleton->m_ChatMsgList.end() )
	{
		OpenChatRoomReqP2P( it->strSender,it->strMsg );
		KUiChatRoomListMenu::GetSingleton().DeletePlayerName(it->strSender);
		ms_Singleton->m_ChatMsgList.pop_front();
	}

	return ms_Singleton->m_ChatMsgList.size();
}

#define MAX_LEN_MSG_TO_SEND 1024

void KUiChatCentre::OpenChatRoomReqP2P(const std::string& strTargetName,const std::string& strMsg	)
{
	KUiPlayerBaseInfo tagRoleInfo;
	g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&tagRoleInfo, NULL );
	char szRoomName[64];
	sprintf( szRoomName, "%s/P2P/%d", tagRoleInfo.Name, ms_Singleton->m_PrivateChatSet.size() );
	if ( ms_Singleton->m_PrivateChatSet.find(strTargetName)==ms_Singleton->m_PrivateChatSet.end() )
	{
		KChatRoomPtr tagRoomPtr;
		tagRoomPtr.pRoom = new KUiChatRoom;
		ms_Singleton->m_PrivateChatSet[strTargetName] = tagRoomPtr;
		ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->CreateWnd( UI_PRIVATECHATROOM, szRoomName );
		ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->m_strTargetName = strTargetName;
		ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->setChatMode( false );
		ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->AddEvent();
		ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->Hide();
		ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->setOwnerState(false);
	}

	ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->Show(true);

	if ( !strMsg.empty() )
	{
		char    *     szStrAddress = (char    *)strMsg.c_str();
		unsigned long dwLen        = strMsg.length();

        if (dwLen < MAX_LEN_MSG_TO_SEND )
			ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->recvMsgFromSomeone( (BYTE*)strMsg.c_str() );
	    else
		{
            while (dwLen > MAX_LEN_MSG_TO_SEND)
			{
				char    *     szStrNextAddress = szStrAddress+MAX_LEN_MSG_TO_SEND;
                unsigned long dwStrCur         = MAX_LEN_MSG_TO_SEND;

				//Searching for "</Seg>" Begin...............................

			    while (szStrNextAddress!=szStrAddress + 5)
				{
					if ( *(szStrNextAddress-1)=='>' && *(szStrNextAddress-2)=='g' && *(szStrNextAddress-3)=='e' 
						&& *(szStrNextAddress-4)=='S' && *(szStrNextAddress-5)=='/' )
					{
						
						char          szStrBuff[MAX_LEN_MSG_TO_SEND];
                        memcpy(szStrBuff,szStrAddress,dwStrCur);
                        szStrBuff[dwStrCur] = 0;

						ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->recvMsgFromSomeone( (unsigned char *)szStrBuff );
                        break;
					}//endif 

					szStrNextAddress -- ;
					dwStrCur         -- ;
				}//end for while

				//Searching for "</Seg>" End ..........................................

				if (szStrAddress + 5== szStrNextAddress)
				{
					//Wrong message or too long message ! 
					break;
				}
				else
				{
					dwLen = dwLen - dwStrCur;
					szStrAddress = szStrNextAddress;
				}

			}//end while

			if (dwLen)
				ms_Singleton->m_PrivateChatSet[strTargetName].pRoom->recvMsgFromSomeone( (unsigned char *)szStrAddress );
	    
		}//end else

	}//endif

	ms_Singleton->setActiveChatRoom( ms_Singleton->m_PrivateChatSet[strTargetName].pRoom );
}

void KUiChatCentre::OpenChatRoomReqP2R( void )
{
	char *message = KMessageCentre::GetMessage(friend_message, CREATE_CHATROOM);
	ms_Singleton->m_pOkWindow->setUserString("Operation","CreateRoom");
	ms_Singleton->m_pOkWinText->setText(AnsiToUtf8(message));
	ms_Singleton->m_pFriendGroupMenu->hide();
	ms_Singleton->m_root->removeChildWindow(ms_Singleton->m_pOkWindow);
	ms_Singleton->m_root->addChildWindow(ms_Singleton->m_pOkWindow);
	ms_Singleton->m_pName->show();
	ms_Singleton->m_pName->setText(String(""));
	ms_Singleton->m_pOkWindow->show();
	ms_Singleton->m_pOkPopup->hide();

}

void KUiChatCentre::CloseChatRoomP2R( int nRoom )
{
	if (ms_Singleton->m_ChatRoomSet.find(nRoom)!=ms_Singleton->m_ChatRoomSet.end())
	{
		ms_Singleton->m_ChatRoomSet[nRoom].pRoom->Hide();
		if (ms_Singleton->m_pActiveRoom == ms_Singleton->m_ChatRoomSet[nRoom].pRoom)
			ms_Singleton->m_pActiveRoom = NULL;

	}//endif
}

void KUiChatCentre::OfflineOperation( void )
{
	//玩家下线的时候 清空聊天室的名字表，但创建的窗口不删除，复用
	if (ms_Singleton && ms_Singleton->m_ChatRoomList)
		  ((TLTree*)ms_Singleton->m_ChatRoomList)->removeAllItem();

	if (ms_Singleton && ms_Singleton->m_pActiveRoom)
	{
		ms_Singleton->m_pActiveRoom->Hide();
		ms_Singleton->m_pActiveRoom = NULL;
	}//endif
}

void KUiChatCentre::PkValueChangeNotify(const char * pName,int nPkValue )
{
/*	if (ms_Singleton && ms_Singleton->m_EnemyList)
	{
        int iCount = ((TLTreeEx *)ms_Singleton->m_EnemyList)->getItemCount();
		
		for (int i=0;i<iCount;i++)
		{
           TreeItemEx * pItem = (TreeItemEx *)((TLTreeEx *)ms_Singleton->m_EnemyList)->getItemFromIndex(i);
		   const char * pItemName = Utf8ToAnsi(pItem->getText());
           
		   int iComp = 0;

		   while (pName[iComp] && pItemName[iComp] && pName[iComp] == pItemName[iComp])
		   {
              iComp++;
		   }//end for while

		   if (pName[iComp]==0 && (pItemName[iComp]==0 || pItemName[iComp]==' ')) //Notice Name don't contain ' '
		   {
			   char szBuff[64];
			   sprintf(szBuff,"%s PK:%d",pName,nPkValue);
			   pItem->setText(AnsiToUtf8(szBuff));
			   break;
		   }//endif

		}//end for i
	
	}//endif
*/
}

void KUiChatCentre::CloseAllChatRoom( void )
{
	KUiChatRoomSetP2P::iterator it = ms_Singleton->m_PrivateChatSet.begin();
	for ( ; it != ms_Singleton->m_PrivateChatSet.end(); ++it )
	{
		(*it).second.pRoom->Hide();

		if (ms_Singleton->m_pActiveRoom == (*it).second.pRoom)
		{
			ms_Singleton->m_pActiveRoom = NULL;
		}//endif

	}
	KUiChatRoomSetP2R::iterator cit = ms_Singleton->m_ChatRoomSet.begin();
	for (;cit!=ms_Singleton->m_ChatRoomSet.end();++cit )
	{
		if ( cit->second.pRoom )
		{
			cit->second.pRoom->Hide();
			if (ms_Singleton->m_pActiveRoom == cit->second.pRoom)
			{
				ms_Singleton->m_pActiveRoom = NULL;
			}
		}
	}
}

bool KUiChatCentre::handleCreateRoom( const CEGUI::EventArgs& args	)
{
	KUiChatCentre::OpenChatRoomReqP2R();
	return true;
}

void KUiChatCentre::chatTextToLoelem(const char* text, char* segText, bool bSelf, bool bP2P )
{
	const KUiCfgLoader::ChatRoomCfg& cfg = KUiCfgLoader::getSingleton().getChatRoomCfg();
	if(NULL == text || NULL == segText)
	{
		return;
	}

	int textLen = strlen(text);

	if(text[0] != '<' || text[textLen - 1] != '>')
	{
		return;
	}

	std::vector<char> formatTexts[COMMON_CLIENT_MSG_LEN_256];
	int index = 0;
	for(int i = 0; i < textLen; ++i)
	{
		if(text[i] == '<')
		{
			continue;
		}
		else if(text[i] == '>')
		{
			formatTexts[index].push_back(0);
			index++;
			continue;
		}
		else if(text[i] == 1)
		{
			formatTexts[index].push_back('<');
		}
		else if(text[i] == 2)
		{
			formatTexts[index].push_back('>');
		}
		else
		{
			formatTexts[index].push_back(text[i]);
		}
	}

	char tempText[COMMON_CLIENT_MSG_LEN_32];
	char msgText[COMMON_CLIENT_MSG_LEN_512];
	msgText[0] = 0;
	for(int j = 0; j < index; ++j)
	{
		memset(msgText, 0, COMMON_CLIENT_MSG_LEN_512);

		char* curText = &formatTexts[j][0];
		if(formatTexts[j][0] == 'I')
		{
			int id[3] = {0, 0, 0};
			int retCode = sscanf(curText, "I=%d|%d|%d|%[^\0]", &id[0], &id[1], &id[2], msgText);
			if(retCode != 4)
			{
				continue;
			}
			strcat(segText, "<Obj type=text vertical-align=bottom ");
			strcat(segText, " show-des=true gotype=item des=");
			strcat(segText, msgText);
			strcat(segText, " goid=");
			sprintf(tempText, "%d", id[0]);
			strcat(segText, tempText);
			strcat(segText, " goid1=");
			sprintf(tempText, "%d", id[1]);
			strcat(segText, tempText);
			strcat(segText, " goid2=");
			sprintf(tempText, "%d", id[2]);
			strcat(segText, tempText);
			
			ItemType type;
			SpliteHashId(id[0], type.genre, type.detail, type.particular);
			type.level = id[1];
			int color = g_pCoreShell->GetGameData( GDI_GET_ITEM_QUALITY_BY_TYPE, (unsigned int)&type, NULL );

			strcat(segText, " color=");
			strcat(segText, KUiChanMgr::getSinglton().getItemColor(color));
			strcat(segText, " font-family=");
			strcat(segText, KUiChanMgr::getSinglton().getItemFont());
			strcat(segText, ">");
			strcat(segText, msgText);
			strcat(segText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'N')
		{
			int retCode = sscanf(curText, "N=%[^\0]", msgText);
			if(retCode != 1)
			{
				continue;
			}
			strcat(segText, "<Obj type=text vertical-align=bottom color=");
			if ( bP2P )
			{
				if (bSelf)
				{
					strcat(segText, cfg.p2pSelfChatColor);
				}
				else
				{
					strcat(segText, cfg.p2pTargetChatColor);
				}
			}
			else
			{
				strcat(segText, cfg.p2rMsgColor);
			}			

			strcat(segText, " font-family=");
			if ( bP2P )
			{
				if (bSelf)
				{
					strcat(segText, cfg.p2pSelfChatFont);
				}
				else
				{
					strcat(segText, cfg.p2pTargetChatFont);
				}
			}
			else
			{
					strcat(segText, cfg.p2rMsgFont);
			}
			strcat(segText, ">");

			strcat(segText, msgText);
			strcat(segText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'F')
		{
			int id;
			int retCode = sscanf(curText, "F=%d", &id);
			if(retCode != 1)
			{
				continue;
			}
			sprintf(tempText, "%d", id);

			strcat(segText, "<Obj type=pic vertical-align=bottom color=");
			if ( bP2P )
			{
				if (bSelf)
				{
					strcat(segText, cfg.p2pSelfChatColor);
				}
				else
				{
					strcat(segText, cfg.p2pTargetChatColor);
				}
			}
			else
			{
				strcat(segText, cfg.p2rMsgColor);
			}


			strcat(segText, " gotype=face goid=");
			
			strcat(segText, " color=");
			if (bP2P)
			{
				if (bSelf)
				{
					strcat(segText, cfg.p2pSelfChatColor);
				}
				else
				{
					strcat(segText, cfg.p2pTargetChatColor);
				}

			}
			else
			{
				strcat(segText,cfg.p2rMsgColor);
			}

			//根据goid得到内容和描述
			char imagePath[COMMON_CLIENT_MSG_LEN_64] = {0};
			char description[COMMON_CLIENT_MSG_LEN_32] = {0};
			const KUiCfgLoader::FacePanelCfgData& facePanelCfg  = KUiCfgLoader::getSingleton().getFaceData();
			for(int i = 0; i < facePanelCfg.faceList.size(); ++i)
			{
				if(facePanelCfg.faceList[i].index == id)
				{
					strcpy(imagePath, facePanelCfg.faceList[i].image);
					strcpy(description, facePanelCfg.faceList[i].description);
					break;
				}
			}
			strcat(segText, " des=");
			strcat(segText, description);
			strcat(segText, ">");
			strcat(segText, imagePath);
			strcat(segText, "</Obj>");
		}
		else if(formatTexts[j][0] == 'P')
		{
			int id[4] = {0, 0, 0,0};
			int retCode = sscanf(curText, "P=%d|%d|%d|%d|%[^\0]", &id[0], &id[1], &id[2],&id[3] ,msgText);
			if(retCode != 5)
			{
				continue;
			}
			strcat(segText, "<Obj type=text vertical-align=bottom color=");
			if (bP2P)
			{
				if (bSelf)
				{
					strcat(segText, cfg.p2pSelfChatColor);
				}
				else
				{
					strcat(segText, cfg.p2pTargetChatColor);
				}
			}
			else
			{
				strcat(segText, cfg.p2rMsgColor);
			}


			strcat(segText, " show-des=true gotype=pos des=");
			strcat(segText, msgText);
			strcat(segText, " goid=");
			sprintf(tempText, "%d", id[0]);
			strcat(segText, tempText);
			strcat(segText, " goid1=");
			sprintf(tempText, "%d", id[1]);
			strcat(segText, tempText);
			strcat(segText, " goid2=");
			sprintf(tempText, "%d", id[2]);
			strcat(segText, tempText);
			strcat(segText, " goid3=");
			sprintf(tempText, "%d", id[3]);
			strcat(segText, tempText);
			
			strcat(segText, " color=");
			strcat(segText, KUiCfgLoader::getSingleton().getPosLinkCfg().color);
			strcat(segText, " font-family=");
			strcat(segText, KUiCfgLoader::getSingleton().getPosLinkCfg().font);
			strcat(segText, ">");
			strcat(segText, msgText);
			strcat(segText, "</Obj>");
		}
		else
		{
			continue;
		}
	}//*/
}

void KUiChatCentre::onRecvMsgFromSomeone( PCHATMSG_BY_NAME pMsg )
{
	const KUiCfgLoader::ChatRoomCfg& cfg = KUiCfgLoader::getSingleton().getChatRoomCfg(); 

	KChatMsg tagChatMsg;
	tagChatMsg.dwRoomID		= -1;
	tagChatMsg.strSender	= pMsg->name;


	char segText[LAYOUT_TEXT_MAX_LEN + 1];

	segText[0] = 0;
	strcat(segText, "<Seg float=wrap>");
	//名称
	if(pMsg->name != NULL && pMsg->name[0] != 0)
	{
		strcat(segText, "<Obj type=text vertical-align=bottom show-des=true color=");
		if (pMsg->isRecive)
		{
			strcat(segText, cfg.p2pTargetNameColor);
		}
		else
		{
			strcat(segText, cfg.p2pSelfNameColor);
		}
		
		strcat(segText, " font-family=");
		if (pMsg->isRecive)
		{
			strcat(segText, cfg.p2pTargetNameFont);
		}
		else
		{
			strcat(segText, cfg.p2pSelfNameFont);
		}
		strcat(segText, " des=[");
		if (pMsg->isRecive)
		{
			strcat(segText, pMsg->name);
		}
		else
		{
			strcat(segText, KMessageCentre::GetMessage(friend_message, SELF) );
		}
		
		strcat(segText, "]:>");
		if (pMsg->isRecive)
		{
			strcat(segText, "[");
			strcat(segText, pMsg->name);
		}
		else
		{
			char szBuff[64];
			sprintf( szBuff, "[%s", KMessageCentre::GetMessage(friend_message, SELF) );
			strcat(segText, szBuff);
		}
		strcat(segText, "]:</Obj></Seg><Seg float=wrap>");
	}

	if(pMsg->msg[0] == '<')
	{
		chatTextToLoelem((const char*)&pMsg->msg, segText,!pMsg->isRecive);
	}
	else	//系统消息可能不带标签
	{
		static char sysMsg[COMMON_CLIENT_MSG_LEN_256];
		sprintf(sysMsg, "<N= %s>", (const char*)&pMsg->msg);
		chatTextToLoelem(sysMsg, segText,!pMsg->isRecive);
	}
	
	strcat(segText, "</Seg>");

	tagChatMsg.strMsg = segText;
	
	if ( m_PrivateChatSet.find(pMsg->name)!=m_PrivateChatSet.end() && m_PrivateChatSet[pMsg->name].pRoom->IsVisible() )
	{
		m_PrivateChatSet[pMsg->name].pRoom->recvMsgFromSomeone( (BYTE*)tagChatMsg.strMsg.c_str() );
		ms_Singleton->setActiveChatRoom( m_PrivateChatSet[pMsg->name].pRoom );
	}
	else
	{
	
		KChatMsgList::iterator it = ms_Singleton->m_ChatMsgList.begin();
			
		while ( it != ms_Singleton->m_ChatMsgList.end() )
		{
			if ((*it).strSender == tagChatMsg.strSender)
			{
				(*it).strMsg += tagChatMsg.strMsg;
				break;
			}//endif

			++it;
		}//endif
			
		if (it == ms_Singleton->m_ChatMsgList.end())
			m_ChatMsgList.push_back( tagChatMsg );

		KUiChatRoomListMenu::GetSingleton().AddPlayerName(tagChatMsg.strSender);
		KUiChatNotify::Show();
	}//end else
}

void KUiChatCentre::onCreateChatRoomNotify( DWORD dwRoomID, BYTE *pMsg )
{
	PCHAT_CREATEROOM_RST pRoomInfo = (PCHAT_CREATEROOM_RST)pMsg;
	if ( pRoomInfo )
	{
		KUiPlayerBaseInfo tagRoleInfo;
		g_pCoreShell->GetGameData( GDI_PLAYER_BASE_INFO, (unsigned int)&tagRoleInfo, NULL );
		char szRoomName[64];
		sprintf( szRoomName, "%s/P2R/%d", tagRoleInfo.Name, dwRoomID );
		if ( m_ChatRoomSet.find(dwRoomID)==m_ChatRoomSet.end() )
		{
			KChatRoomPtr tagRoomPtr;
			tagRoomPtr.pRoom = new KUiChatRoom;
			m_ChatRoomSet[dwRoomID] = tagRoomPtr;
			m_ChatRoomSet[dwRoomID].pRoom->CreateWnd( UI_CHATROOM, szRoomName );
			m_ChatRoomSet[dwRoomID].pRoom->Hide();
			tagRoomPtr.pRoom->AddEvent();
			AddChatRoom( pRoomInfo->roomName, pRoomInfo->roomId );
		}
		
		ms_Singleton->setActiveChatRoom( ms_Singleton->m_ChatRoomSet[dwRoomID].pRoom);
		m_ChatRoomSet[dwRoomID].pRoom->onCreateChatRoomNotify( (BYTE*)dwRoomID );
		/*TreeItem *Item = static_cast<TreeItem*>(static_cast<Tree*>(m_FriendList)->getFirstSelectedItem());
		  if ( Item )
		  {
			m_ChatRoomSet[dwRoomID].pRoom->m_strTargetName = Utf8ToAnsi( Item->getText() );
			m_ChatRoomSet[dwRoomID].pRoom->addMemberToRoomReq(m_ChatRoomSet[dwRoomID].pRoom->m_nRoomID, m_ChatRoomSet[dwRoomID].pRoom->m_strTargetName);
			m_ChatRoomSet[dwRoomID].pRoom->setChatMode( true );
			}
		*/
		bool bOwner = g_pCoreShell->GetGameData(GDI_CHAT_IS_OWNER,pRoomInfo->roomId,0) == FALSE ? false : true;
        m_ChatRoomSet[dwRoomID].pRoom->setOwnerState(bOwner);
		m_ChatRoomSet[dwRoomID].pRoom->Show(false);
		setActiveChatRoom(m_ChatRoomSet[dwRoomID].pRoom);
	}
}

/************************************************************************/
/*                          Common function                             */
/************************************************************************/
bool KUiChatCentre::clickAddFriendBtn( const CEGUI::EventArgs& args	)
{
	char *message = KMessageCentre::GetMessage(friend_message, INPUT_ADD_FRIEND_NAME);
	m_pOkWindow->setUserString("Operation","AddFriend");
	m_pOkWinText->setText(AnsiToUtf8(message));
	m_pName->show();
	m_root->removeChildWindow(m_pOkWindow);
	m_root->addChildWindow(m_pOkWindow);
	m_pOkWindow->show();
	m_pOkPopup->hide();
	m_pName->setText(String(""));
	return true;
}

bool KUiChatCentre::clickDelFriendBtn( const CEGUI::EventArgs& args	)
{
	if( m_pCurSelectItem )
	{
		showIsOperation(IS_DELETE_FRIEND);
	}
	else
	{
		char *message = KMessageCentre::GetMessage(friend_message, INPUT_DEL_FRIEND_NAME);
		m_pOkWindow->setUserString("Operation","DelFriend");
		m_pOkWinText->setText(AnsiToUtf8(message));
		m_pName->setEnabled(true);
		m_pName->setText(String(""));
		m_pName->show();
		m_root->removeChildWindow(m_pOkWindow);
		m_root->addChildWindow(m_pOkWindow);
		m_pOkWindow->show();
		m_pOkPopup->hide();
	}
	return true;
}

bool KUiChatCentre::clickCloseBtn( const EventArgs& args	)
{
	KUiWndSingleton<KUiChatCentre>::Hide();
	return true;
}

void KUiChatCentre::HideAllList( void )
{
	if ( m_pThisWnd )
	{
        m_pFriendTreeCliper->hide();
	 	m_FriendList->hide();
		m_FriendList->setEnabled(false);
		m_EnemyList->hide();
		m_EnemyList->setEnabled(false);
		m_ScreenList->hide();
		m_ScreenList->setEnabled(false);
		m_ChatRoomList->hide();
		m_ChatRoomList->setEnabled(false);
		m_TemporaryList->hide();
		m_TemporaryList->setEnabled(false);
		m_pThisWnd->getChild( "TaharezLook/ChatCentre/HideSelf" )->hide();
		m_pThisWnd->getChild( "TaharezLook/ChatCentre/CreateRoom" )->hide();
    	m_pThisWnd->getChild( "TaharezLook/ChatCentre/AddToRoom" )->hide();
		m_pAddFriend->hide();
		m_pDelFriend->hide();
		m_pInvFriend->hide();
		m_pOkWindow->hide();
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
		m_pDelEnemy->hide();
	    m_pAddEnemy->hide();
		m_pAddScreen->hide();
		m_pDelScreen->hide();
		
		static_cast<TLTree *>(m_FriendList)->cleanAllOpenItem();
	}
}

/*!
\brief
	common  popmenu.
*/
bool KUiChatCentre::clickFListMenu( const CEGUI::EventArgs& args )
{
	CEGUI::TreeEventArgs* args_= (CEGUI::TreeEventArgs*)(&args);
	m_pCurSelectItem = (TreeItemEx*)args_->treeItem;
	
	Point parentP = m_pThisWnd->getPosition(Absolute);
	Point newP = MouseCursor::getSingleton().getPosition() - parentP;

	if ( MouseCursor::getSingleton().getPosition().d_x + m_pFriendGroupMenu->getWidth(Absolute) >
		 parentP.d_x + m_pThisWnd->getWidth(Absolute) - ScrollWidth )
	{
		newP.d_x = m_pThisWnd->getWidth(Absolute) - ( m_pFriendGroupMenu->getWidth(Absolute) + ScrollWidth);
	}

	if ( MouseCursor::getSingleton().getPosition().d_y + m_pFriendGroupMenu->getHeight(Absolute) >
		 parentP.d_y + m_pThisWnd->getHeight(Absolute))
	{
		newP.d_y = m_pThisWnd->getHeight(Absolute) - ( m_pFriendGroupMenu->getHeight(Absolute));
	}

	bool bShizuAvailable  = false;
	bool bZhuhouAvailable = false;

	SocietyInfoIndex tagSocietyIdx;
	tagSocietyIdx.TemplateId       = enSUTplId_Tong;
	tagSocietyIdx.Layer            = enSULayer_Gens;
	tagSocietyIdx.Operation        = enSUO_AddSubUnit;
	
	if (ms_Singleton->m_pInviteShizu &&  g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
	{
        bShizuAvailable = true;
	}//endif
	
	tagSocietyIdx.Layer            = enSULayer_Tong;
	
	if (ms_Singleton->m_pInviteShizu && g_pCoreShell->GetGameData( GDI_GET_SOCIETY_RIGHT, (unsigned int)&tagSocietyIdx, NULL ) )
	{
		bZhuhouAvailable = true;
	}

	if(args_->treeItem == NULL && m_State == friendPage )
	{
		m_pAddGroup->enable();
		m_pRenameGroup->disable();
		m_pDeleteGroup->disable();
		m_pAddToGroup->disable();
		m_pFriendListMenu->hide();
		m_pFriendGroupMenu->setPosition(Absolute, newP);
		m_pFriendGroupMenu->show();
	}
	else if( m_State == friendPage && args_->treeItem->getLayer() == 0)
	{
		m_pFriendGroupMenu->show();
		m_pRenameGroup->enable();
		m_pDeleteGroup->enable();
		m_pAddToGroup->enable();
		m_pAddGroup->disable();
		m_pFriendListMenu->hide();
		m_pFriendGroupMenu->setPosition(Absolute, newP);
	}
	else
	{
		switch( m_State )
		{
		case friendPage:
			{
				TreeItemEx *pItem = NULL;
				if ( m_FriendList != NULL )
				{
					pItem = static_cast<TreeItemEx *>(static_cast<Tree *>(m_FriendList)->getFirstSelectedItem());
				}

				if (pItem == NULL)
					return true;

				m_pFriendListMenu->setPosition(Absolute, newP);
				showAllFriendSubManu();

				if (!bShizuAvailable)
					m_pInviteShizu->hide();
				else
					m_pInviteShizu->show();

				if (!bZhuhouAvailable)
					m_pInviteZhuhou->hide();
				else
					m_pInviteZhuhou->show();

				if ( pItem != NULL && !pItem->getIsViewOtherColor() )
				{
					m_pInvite->disable();
					m_pChat->disable();
					m_pPlayerInfo->disable();
					m_pCommonChat->disable();
					m_pInviteShizu->disable();
					m_pInviteZhuhou->disable();
				}//endif
				else
				{
					m_pInvite->enable();
					m_pChat->enable();
					m_pPlayerInfo->enable();
					m_pCommonChat->enable();
					m_pInviteShizu->enable();
					m_pInviteZhuhou->enable();
				}//end else

				m_pOpenRoom->hide();
				m_pLeaveRoom->hide();
			  //m_pChat->enable();		
			  //m_pFriendDetail->disable();
				m_pPreventChat->enable();
				char *msg = KMessageCentre::GetMessage(friend_message, DELELE_FRIEND);
				m_pDeleteFriend->setText(AnsiToUtf8(msg));
				m_pDeleteFriend->enable();
				m_pFriendGroupMenu->hide();
				m_pAddToFriend->hide();
				
			}
			break;
		case enemyPage:
			{
				TreeItemEx *pItem = NULL;
				if ( m_EnemyList != NULL )
				{
					pItem = static_cast<TreeItemEx *>(static_cast<Tree *>(m_EnemyList)->getFirstSelectedItem());
				}
				
				if (pItem == NULL)
					return true;

				showAllFriendSubManu();
				m_pInviteShizu->hide();
				m_pInviteZhuhou->hide();
				m_pOpenRoom->hide();
				m_pLeaveRoom->hide();
				m_pFriendGroupMenu->hide();
				m_pInvite->hide();		
				m_pPlayerInfo->hide();
				m_pPreventChat->hide();
				m_pAddToFriend->hide();
				m_pChat->hide();
				m_pAddToSepecificGroup->hide();

				char *msg = KMessageCentre::GetMessage(friend_message, DELELE_ENEMY);

				m_pDeleteFriend->setText(AnsiToUtf8(msg));
				m_pDeleteFriend->enable();

				if (pItem->getIsViewOtherColor())
					m_pCommonChat->enable();
				else
					m_pCommonChat->disable();

				Point firstItemPos = m_pDeleteFriend->getPosition(Absolute);
				newP.d_y          -= firstItemPos.d_y;

				m_pFriendListMenu->setPosition(Absolute, newP);
			
			}
			break;
		case screenPage:
			{
				TreeItemEx *pItem = NULL;
				if ( m_ScreenList != NULL )
				{
					pItem = static_cast<TreeItemEx *>(static_cast<Tree *>(m_ScreenList)->getFirstSelectedItem());
				}
				
				if (pItem == NULL)
					return true;

				showAllFriendSubManu();
				m_pInviteShizu->hide();
				m_pInviteZhuhou->hide();

				m_pOpenRoom->hide();
				m_pLeaveRoom->hide();
				m_pFriendGroupMenu->hide();
				m_pInvite->hide();		
				m_pPlayerInfo->hide();
				m_pPreventChat->hide();
				m_pAddToFriend->hide();
				m_pChat->hide();
				m_pAddToSepecificGroup->hide();
				
				char *msg = KMessageCentre::GetMessage(friend_message, DEL);
				
				m_pDeleteFriend->setText(AnsiToUtf8(msg));
				m_pDeleteFriend->enable();
                m_pCommonChat->disable();
				
				Point firstItemPos = m_pDeleteFriend->getPosition(Absolute);
				newP.d_y          -= firstItemPos.d_y;
				
				m_pFriendListMenu->setPosition(Absolute, newP);
			}
		    break;
		case chatroomPage:
			{
				TreeItemEx *pItem = NULL;
				if ( m_ChatRoomList != NULL )
				{
					pItem = static_cast<TreeItemEx *>(static_cast<Tree *>(m_ChatRoomList)->getFirstSelectedItem());
				}

				if (pItem==NULL)
					return true;

				showAllFriendSubManu();

				m_pOpenRoom->show();
				m_pLeaveRoom->show();
				
				m_pFriendGroupMenu->hide();
				m_pInvite->hide();		
				m_pPlayerInfo->hide();
				m_pPreventChat->hide();
				m_pAddToFriend->hide();
				m_pChat->hide();
				m_pAddToSepecificGroup->hide();
				m_pDeleteFriend->hide();
				m_pCommonChat->hide();
				m_pInviteShizu->hide();
				m_pInviteZhuhou->hide();


				Point firstItemPos = m_pOpenRoom->getPosition(Absolute);
				newP.d_y          -= firstItemPos.d_y;
				
				m_pFriendListMenu->setPosition(Absolute, newP);

			}
		    break;
		
		case temporaryPage:
			{
				TreeItemEx *pItem = NULL;
				if ( m_TemporaryList != NULL )
				{
					pItem = static_cast<TreeItemEx *>(static_cast<Tree *>(m_ScreenList)->getFirstSelectedItem());
				}
				
				if (pItem == NULL)
					return true;

			/*	showAllFriendSubManu();
				m_pOpenRoom->hide();
				m_pLeaveRoom->hide();
				m_pFriendGroupMenu->hide();
				m_pInvite->hide();		
				m_pPlayerInfo->hide();
				m_pPreventChat->hide();
				m_pChat->hide();
				m_pAddToSepecificGroup->hide();
				m_pDeleteFriend->hide();
				m_pCommonChat->hide();
			*/	
			}
			break;

		default:
		    break;
		}
	}

	return true;
}

void KUiChatCentre::sortListByOnline(TLTreeEx * pList )
{
	if (pList)
	{
        int nTotalCount = pList->getItemCount();
		if (pList == ms_Singleton->m_FriendList)
		{
			//Presort the sub Item
			for (int index = 0; index < nTotalCount ; index ++)
			{
				TreeItemEx * pItem=(TreeItemEx *)pList->getItemFromIndex(index);
				
				if (pItem->getLayer()==1 && pItem->getItemCount())
				{
					int nSubItemCount = pItem->getItemCount();
					int nOnlineBord   = -1;        //The last online index
					
					for (int iSubIndex = 0; iSubIndex<nSubItemCount;iSubIndex++)
					{
						TreeItemEx * pSubItem = (TreeItemEx *)pItem->getTreeItemFromIndex(iSubIndex);
						
						if (pSubItem && pSubItem->getIsViewOtherColor())
						{
							if (nOnlineBord == iSubIndex-1)
							{
								nOnlineBord = iSubIndex;
							}//endif
							else
							{
								TreeItemEx           * pNotOnline = (TreeItemEx *)pItem->getTreeItemFromIndex(nOnlineBord+1);
								TreeItem::LBItemList & sublist    = pItem->getItemList();
								sublist[nOnlineBord+1]  = pSubItem;
								sublist[iSubIndex]      = pNotOnline;
								nOnlineBord ++ ;
							}//end else
							
						}//endif
						
					}//end for iSubIndex
					
				}//endif
				
			}//end for index
		}//end for friend list
		else
		{
			int nOnlineBord     = -1; 	
			for (int iItemIndex = 0; iItemIndex<nTotalCount;iItemIndex++)
			{
				TreeItemEx * pItem = (TreeItemEx *)pList->getItemFromIndex(iItemIndex);
				
				if (pItem && pItem->getIsViewOtherColor())
				{
					if (nOnlineBord == iItemIndex-1)
					{
						nOnlineBord = iItemIndex;
					}//endif
					else
					{
						TreeItemEx           *   pNotOnline       = (TreeItemEx *)pList->getItemFromIndex(nOnlineBord+1);
	
						pList->setItemFromIndex(nOnlineBord+1,pItem);
						pList->setItemFromIndex(iItemIndex,pNotOnline);
						nOnlineBord ++ ;
					}//end else
					
				}//endif
				
			}//end for iSubIndex

		}//end else
		
	}//endif
}

bool KUiChatCentre::closeFListMenu( const CEGUI::EventArgs& args )
{
	MouseEventArgs* pArgs = (MouseEventArgs*)&args;
	if ( m_pFriendGroupMenu->isVisible() && pArgs->button != RightButton )
	{
		m_pFriendGroupMenu->hide();
	}

	if ( m_pFriendListMenu->isVisible() && pArgs->button != RightButton )
	{
		m_pFriendListMenu->hide();
	}

	if ( m_pOkWindow->isVisible() && pArgs->button != RightButton )
	{
		m_pOkWindow->hide();
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	}
	return true;
}

bool KUiChatCentre::clickCommonChat( const EventArgs& args )
{
	//TreeItemEx *     pItem = (TreeItemEx *)((TLTreeEx *)m_FriendList)->getFirstSelectedItem();
	if(m_pCurSelectItem)
	{
		char  szName[32] = "";
		sprintf(szName,Utf8ToAnsi(m_pCurSelectItem->getText()));
		
		if ( m_State == enemyPage )
		{
			int nLen = strlen(szName);
			for (int comp = 0 ; comp < nLen && szName[comp]; comp ++ )
			{
				if (szName[comp] == ' ')
				{
					szName[comp] = 0;
					break;
				}//endif
				
			}//endif
			
		}//endif

		KUiChatInputWnd::GetSingleton().clearText();
		
		KUiChatInputWnd::GetSingleton().write("/");
		
		KUiChatInputWnd::GetSingleton().write(szName);
		KUiChatInputWnd::GetSingleton().write(" ");
		
		KUiChatInputWnd::GetSingleton().show();
	}//endif

    m_pFriendListMenu->hide();

	return true;
}

bool KUiChatCentre::clickChat( const CEGUI::EventArgs& args )
{
	if (m_pCurSelectItem == NULL || m_pFriendListMenu == NULL)
	{
		return false;
	}
	String friendName = m_pCurSelectItem->getText();
	std::string strTest;
	KChatMsgList::iterator it;
	KChatMsgList::iterator tempIt;
	for (it = m_ChatMsgList.begin(); it != m_ChatMsgList.end(); ++it)
	{
		CEGUI::String senderName(AnsiToUtf8(it->strSender.c_str()));
		if (senderName.compare(friendName) == 0)
		{	
			strTest = it->strMsg;
			tempIt = it;
			++it;
			m_ChatMsgList.erase(tempIt);
			break;
		}
	}
	OpenChatRoomReqP2P(Utf8ToAnsi(friendName), strTest);
	m_pFriendListMenu->hide();
	return true;
}

bool KUiChatCentre::clickInvite( const CEGUI::EventArgs& args )
{
	KUiPlayerItem tagPlayer;
	ZeroMemory(&tagPlayer, sizeof(KUiPlayerItem));
	if( m_pCurSelectItem )
	{
		String name = m_pCurSelectItem->getText();
		strncpy( tagPlayer.Name, Utf8ToAnsi( name ), CLIENT_NAME_AND_TITLE_MAX + 1);

		KUiPlayerTeam	TeamInfo;
		TeamInfo.cNumMember = 0;
		g_pCoreShell->TeamOperation(TEAM_OI_GD_INFO, (unsigned int)&TeamInfo, 0);
		if ( ((int)(TeamInfo.cNumMember)) <= MAX_TEAMMEMBER_COUNT )
		{
			if (TeamInfo.cNumMember == 0)
			{
				g_pCoreShell->TeamOperation(TEAM_OI_CREATE, 0, 0);
			}

			g_pCoreShell->TeamOperation( TEAM_OI_INVITE_BY_NAME, (unsigned int)&tagPlayer, NULL );
		}
		else
		{
			char *msg = KMessageCentre::GetMessage(team_message, 1);
			KUiChannelCentre::GetSingleton().toSysMsg(msg);
		}
	}
	m_pFriendListMenu->hide();
	return true;
}

bool KUiChatCentre::clickFriendDetail( const CEGUI::EventArgs& args )
{
	//showFriendInfo();
	m_pFriendListMenu->hide();	
	return true;
}
bool KUiChatCentre::clickPreventChat( const CEGUI::EventArgs& args )
{
	if ( m_pFriendListMenu )
	{
		showIsOperation(IS_ADD_SCREEN);
		m_pFriendListMenu->hide();	
	}
	
	return true;
}

void KUiChatCentre::disableAllList                  ( void                                                          )
{
	if (ms_Singleton->m_FriendList)
	{
        ms_Singleton->m_FriendList->disable();
	}//endif

	if (ms_Singleton->m_ScreenList)
	{
       ms_Singleton->m_ScreenList->disable();
	}//endif
	
	if (ms_Singleton->m_EnemyList)
	{
		ms_Singleton->m_EnemyList->disable();
	}//endif

	if (ms_Singleton->m_ChatRoomList)
	{
		ms_Singleton->m_ChatRoomList->disable();
	}//endif

	if (ms_Singleton->m_TemporaryList)
	{
       ms_Singleton->m_TemporaryList->disable();
	}//endif
}

void KUiChatCentre::enableAllList                   ( void                                                          )
{
	if (ms_Singleton->m_FriendList)
	{
        ms_Singleton->m_FriendList->enable();
	}//endif
	
	if (ms_Singleton->m_ScreenList)
	{
		ms_Singleton->m_ScreenList->enable();
	}//endif
	
	if (ms_Singleton->m_EnemyList)
	{
		ms_Singleton->m_EnemyList->enable();
	}//endif
	
	if (ms_Singleton->m_ChatRoomList)
	{
		ms_Singleton->m_ChatRoomList->enable();
	}//endif
	
	if (ms_Singleton->m_TemporaryList)
	{
		ms_Singleton->m_TemporaryList->enable();
	}//endif
}


bool KUiChatCentre::clickDeleteFriend( const CEGUI::EventArgs& args )
{
	switch( m_State )
	{
	case friendPage:
		{
			if(!m_pCurSelectItem )
			{
				char *message = KMessageCentre::GetMessage(friend_message, SELECT_DEL_FRIEND_NAME);
				m_pOkWindow->setUserString("Operation","DelFriend");
				m_pOkWinText->setText(AnsiToUtf8(message));
				m_pName->hide();
			}
			else
			{
				showIsOperation(IS_DELETE_FRIEND);
			}
		}
		break;
	case enemyPage:
		{
			char *message = NULL;
			if(!m_pCurSelectItem )
			{
				message = KMessageCentre::GetMessage(friend_message, SELECT_DEL_ENEMY_NAME);
				KUiChannelCentre::GetSingleton().toSysMsg(message);
			}
			else
			{
				showIsOperation(IS_DEL_ENEMY_NAME);
			}
		}
		break;
	case screenPage:
		{
			char *message = NULL;
			if(!m_pCurSelectItem )
			{
				message = KMessageCentre::GetMessage(friend_message, SELECT_DEL_ENEMY_NAME);
				KUiChannelCentre::GetSingleton().toSysMsg(message);
			}
			else
			{
				showIsOperation(IS_DEL_SCREEN_NAME);
			}
		}
		break;
	case chatroomPage:
		{
            if (m_pCurSelectItem)
			{
				unsigned long dwRoomID=m_pCurSelectItem->getID();
				g_pCoreShell->OperationRequest( GOI_LEAVE_CHATROOM, dwRoomID, NULL );
				KUiChatCentre::GetSingleton().DelChatRoom( dwRoomID );
			}//endif 
		}
		break;
	case temporaryPage:
		break;
	default:
		break;
	}

	m_pFriendListMenu->hide();

	return true;
}

void KUiChatCentre::showAllFriendSubManu(void )
{
	if (m_pFriendListMenu)
	{
        m_pFriendListMenu->show();
        m_pCommonChat->show();
        m_pChat->show();
        m_pInvite->show();
        m_pPreventChat->show();
        m_pDeleteFriend->show();
        m_pAddToFriend->show();
		m_pAddToSepecificGroup->show();
		m_pPlayerInfo->show();
		m_pInviteShizu->show();
		m_pInviteZhuhou->show();

	}//endif
}

/************************************************************************/
/*							Friend list page                            */
/************************************************************************/
bool KUiChatCentre::showFriendList( const CEGUI::EventArgs& args )
{
	m_State = friendPage;
	HideAllList();

	if(m_pFriendTreeScrol != NULL && m_FriendList != NULL)
	{
		m_pFriendTreeScrol->setScrollPosition(0.0f);
		m_FriendList->setPosition(Absolute, Point(0, 0));
	}

	if ( m_pThisWnd )
	{	
		m_pFriendTreeCliper->show();
		m_FriendList->show();	
		m_FriendList->setEnabled(true);

		m_pThisWnd->getChild( "TaharezLook/ChatCentre/HideSelf" )->hide();
		m_pAddFriend->show();
		m_pCommonChat->enable();
		//m_pDelFriend->show();
		//m_pInvFriend->show();
		m_pCurSelectItem = NULL;
	}		
	if ( m_pFriendGroupMenu->isVisible() )
	{
		m_pFriendGroupMenu->hide();
	}
	if ( m_pFriendListMenu->isVisible() )
	{
		m_pFriendListMenu->hide();
	}
	return true;
}

bool KUiChatCentre::handleDCList( const CEGUI::EventArgs& args )
{
	if ( m_pCurSelectItem )
	{
		int nRoomID = m_pCurSelectItem->getID();
		KUiChatRoomSetP2R::iterator it = m_ChatRoomSet.find(nRoomID);
		if ( it != m_ChatRoomSet.end() )
		{
			if ( it->second.pRoom )
			{
				it->second.pRoom->Show(false);
				setActiveChatRoom(it->second.pRoom);
			}			
		}		
	}

	return true;
}

/*!
\brief
	Friend group popmenu.
*/
void KUiChatCentre::clickAddToChatRoom( int nRoomID )
{
	char *message = KMessageCentre::GetMessage(friend_message, ADD_TO_CHATROOM);
	ms_Singleton->m_pOkWindow->setUserString("Operation","AddToChatRoom");
	ms_Singleton->m_pOkWinText->setText(AnsiToUtf8(message));
	ms_Singleton->m_pFriendGroupMenu->hide();
	ms_Singleton->m_pName->show();
	ms_Singleton->m_pName->setText(String(""));
	
	int nCount = ((TLTree *)ms_Singleton->m_FriendList)->getItemCount();
	ms_Singleton->m_pOkWindow  ->show();
	ms_Singleton->m_PopupState = PS_FRIENDLIST;
	ms_Singleton->m_pOkPopup->hide();

	if (nCount)
	{
		for (int index=0;index<nCount;index++)
		{
			TreeItemEx * pItem = (TreeItemEx *)((TLTree *)ms_Singleton->m_FriendList)->getItemFromIndex(index);
			
			if (pItem->getItemCount())
			{
				ms_Singleton->m_pOkPopup->show();
				break;
			}//endif
			
		}//end for index
	}//end if

	ms_Singleton->m_root->removeChildWindow(ms_Singleton->m_pOkWindow);
	ms_Singleton->m_root->addChildWindow(ms_Singleton->m_pOkWindow);
	ms_Singleton->m_pOkWindow->show();
	ms_Singleton->m_nCurRoomID = nRoomID;
}

bool KUiChatCentre::clickAddGroup( const CEGUI::EventArgs& args )
{
	char *message = KMessageCentre::GetMessage(friend_message, INPUT_NEW_GROUP_NAME);
	m_pOkWindow->setUserString("Operation","AddGroup");
	m_pOkWinText->setText(AnsiToUtf8(message));
	m_pFriendGroupMenu->hide();
	m_root->removeChildWindow(m_pOkWindow);
	m_root->addChildWindow(m_pOkWindow);
	m_pName->show();
	m_pName->setText(String(""));
	m_pOkWindow->show();
	m_pOkPopup->hide();

	return true;
}

bool KUiChatCentre::clickRenameGroup( const CEGUI::EventArgs& args )
{
	char *message = KMessageCentre::GetMessage(friend_message, NEW_NAME_GROUP);
	m_pOkWindow->setUserString("Operation","RenameGroup");
	m_pOkWinText->setText(AnsiToUtf8(message));
	m_pName->show();
	m_pName->setText(String(""));
	m_root->removeChildWindow(m_pOkWindow);
	m_root->addChildWindow(m_pOkWindow);
	m_pOkWindow->show();
	m_pOkPopup->hide();

	m_pFriendGroupMenu->hide();
	return true;
}

bool KUiChatCentre::clickDeleteGroup( const CEGUI::EventArgs& args )
{
	if(m_pCurSelectItem)
	{
		char *message = KMessageCentre::GetMessage(friend_message, IS_DELETE_CROUP);
		m_pOkWindow->setUserString("Operation","DelGroup");
		m_pOkWinText->setText(AnsiToUtf8(message));	
		m_pFriendGroupMenu->hide();
		m_pName->hide();
		//m_pOkWindow->show();
		showIsOperation(IS_DELETE_CROUP);
	}
	else
	{
		char *message = KMessageCentre::GetMessage(friend_message, SELECT_DEL_GROUP_NAME);
		m_pOkWindow->setUserString("Operation","");
		m_pOkWinText->setText(AnsiToUtf8(message));	
		m_pFriendGroupMenu->hide();
		m_pName->hide();
		m_pName->setText(String(""));
		m_root->removeChildWindow(m_pOkWindow);
		m_root->addChildWindow(m_pOkWindow);
		m_pOkWindow->show();
		ms_Singleton->m_pOkPopup->hide();
	}
	m_pFriendGroupMenu->hide();
	return true;
}

bool KUiChatCentre::clickAddToGroup( const CEGUI::EventArgs& args )
{
	char *message = KMessageCentre::GetMessage(friend_message, INPUT_PLAYERNAME_TO_GROUP);
	m_pOkWindow->setUserString("Operation","AddFriendToGroup");
	m_pOkWinText->setText(AnsiToUtf8(message));	
	m_pFriendGroupMenu->hide();
	m_pName->show();
	m_pName->setText(String(""));
	m_root->removeChildWindow(m_pOkWindow);
	m_root->addChildWindow(m_pOkWindow);

	ms_Singleton->m_pOkWindow  ->show();

	ms_Singleton->m_PopupState = PS_FRIENDLIST;
	ms_Singleton->m_pOkPopup->hide();
	int nCount = ((TLTree *)ms_Singleton->m_FriendList)->getItemCount();

	if (nCount)
	{
		for (int index=0;index<nCount;index++)
		{
			TreeItemEx * pItem = (TreeItemEx *)((TLTree *)ms_Singleton->m_FriendList)->getItemFromIndex(index);
			
			if (pItem->getItemCount())
			{
				ms_Singleton->m_pOkPopup->show();
				break;
			}//endif
			
		}//end for index
	}//end if

	return true;
}

bool KUiChatCentre::clickPlayerInfo( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pArgs = (WindowEventArgs*)&args;
	if ( pArgs )
	{
		if ( m_pCurSelectItem )
		{
			ChatFriendPanelManager::ChatFriendManagerGet().chatWndListUpdata = true;
			g_pCoreShell->OperationRequest( GOI_FIND_PLAYER, (unsigned int)Utf8ToAnsi( m_pCurSelectItem->getText() ), NULL );
		}
	}
	m_pFriendGroupMenu->hide();
	m_pFriendListMenu->hide();

	return true;
}

bool KUiChatCentre::clickOpenRoom( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pArgs = (WindowEventArgs*)&args;
	if ( pArgs && m_pCurSelectItem )
	{
		int nRoomID = m_pCurSelectItem->getID();
		KUiChatRoomSetP2R::iterator it = m_ChatRoomSet.find(nRoomID);
		if ( it != m_ChatRoomSet.end() )
		{
			if ( it->second.pRoom )
			{
				it->second.pRoom->Show(false);
				ms_Singleton->setActiveChatRoom(it->second.pRoom);
			}			
		}		
	}
	m_pFriendListMenu->hide();
	return true;
}

bool KUiChatCentre::clickLeaveRoom( const CEGUI::EventArgs& args )
{
	WindowEventArgs* pArgs = (WindowEventArgs*)&args;
	if ( pArgs && m_pCurSelectItem )
	{
		g_pCoreShell->OperationRequest( GOI_LEAVE_CHATROOM, m_pCurSelectItem->getID(), NULL );
		KUiChatCentre::GetSingleton().DelChatRoom( m_pCurSelectItem->getID() );
	}
	m_pFriendListMenu->hide();
	return true;
}

bool KUiChatCentre::clickAddToFriend( const CEGUI::EventArgs& args)
{
	int nRoomId;
	TreeItemEx *  pLastItem;

	m_pFriendListMenu->hide();
	
	switch (m_State)
	{
	case friendPage:
		{
			return true;
		}
		break;
	case enemyPage:
		{
			nRoomId = GROUPID_ENEMY;
			pLastItem = (TreeItemEx *)((TLTreeEx *)m_EnemyList)->getFirstSelectedItem();
			if (!pLastItem) return true;
		}
		break;

	case screenPage:	
		{
            nRoomId = GROUPID_BLACK;
			pLastItem =(TreeItemEx *) ((TLTreeEx *)m_ScreenList)->getFirstSelectedItem();
			if (!pLastItem) return true;
		}
		break;
    case temporaryPage:
		{
			nRoomId = GROUPID_TEMP;
			pLastItem = (TreeItemEx *)((TLTreeEx *)m_ScreenList)->getFirstSelectedItem();
			if (!pLastItem) return true;
		}
		break;
	}

	int nParam[2];
	nParam[0]=nRoomId;
    nParam[1]=GROUPID_NONE;
	g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)Utf8ToAnsi(pLastItem->getText()), (int)&nParam ); 
	
	return true;
}

bool KUiChatCentre::clickAddToSpecificGroup(const CEGUI::EventArgs& args )
{
	m_pFriendListMenu->hide();

	char *message = KMessageCentre::GetMessage(friend_message, INPUT_GOUPNAME_IN);
	m_pOkWindow->setUserString("Operation","AddToSpecificGroup");
	m_pOkWinText->setText(AnsiToUtf8(message));
	m_pFriendGroupMenu->hide();
	m_root->removeChildWindow(m_pOkWindow);
	m_root->addChildWindow(m_pOkWindow);
	m_pName->show();
	m_pName->setText(String(""));
    m_pOkWindow->show();
	m_PopupState = PS_GROUPLIST;
	m_pOkPopup->show();

	return true;
}

bool KUiChatCentre::clickInviteShizu(const CEGUI::EventArgs& args )
{
	String friendName = m_pCurSelectItem->getText();
	
	TongOperParam tagTongOper;
	ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
	tagTongOper.nTemplateID		= enSUTplId_Tong;
	tagTongOper.nLayerID		= enSULayer_Gens;
	tagTongOper.nOperationID	= enSUO_AddSubUnit;
	strcpy(tagTongOper.szName,Utf8ToAnsi(friendName));
	
	IUIMDLDataset* pTongOper = NULL;
	if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
	{
		Hide();
		return false;
	}//endif
	
	pTongOper->updateRecord( enSULayer_Gens, &tagTongOper, sizeof( TongOperParam ) );	
	
	m_pFriendListMenu->hide();

	return true;
}

bool KUiChatCentre::clickInviteZhuhou(const CEGUI::EventArgs& args )
{

	String friendName = m_pCurSelectItem->getText();
	
	TongOperParam tagTongOper;
	ZeroMemory( &tagTongOper, sizeof(tagTongOper) );
	tagTongOper.nTemplateID		= enSUTplId_Tong;
	tagTongOper.nLayerID		= enSULayer_Tong;
	tagTongOper.nOperationID	= enSUO_AddSubUnit;
	strcpy(tagTongOper.szName,Utf8ToAnsi(friendName));
	
	IUIMDLDataset* pTongOper = NULL;
	if ( success_errorcode != m_pUiMDLManager->queryDataSet( tong_operation, &pTongOper ) )
	{
		Hide();
		return false;
	}//endif
	
	pTongOper->updateRecord( enSULayer_Tong, &tagTongOper, sizeof( TongOperParam ) );	
	
	m_pFriendListMenu->hide();

	return true;
}

/*!
\brief
	Friend list OkWindow function. 		
*/
bool KUiChatCentre::OnOk( const CEGUI::EventArgs& args )
{
	char msg[COMMON_CLIENT_MSG_LEN_256] = {0};
	String name = m_pName->getText();

	unsigned int namePtr = 0;

	if( String("") == name )
	{
		namePtr = 0;
		return true;
	}
	else
	{
		namePtr = (unsigned int)Utf8ToAnsi(name);
		char *temp = (char *)namePtr;
		if ( strlen(temp) > MAXSIZE_GROUPNAME )
		{
			namePtr = (unsigned int)Utf8ToAnsi(name.substr(0, MAX_WORD_NUM));
		}
	}

	String userOper = m_pOkWindow->getUserString("Operation");
	if(userOper == "AddGroup")
	{
		//发送添加好友分组命令到游戏服务器
		if ( namePtr )
		{
			g_pCoreShell->OperationRequest( GOI_CHAT_GROUP_NEW, namePtr, NULL );
			m_pOkWindow->hide();
			m_pOkWindow->setSize(Absolute,m_OkNormalSize);
		}
		else
		{
			char *message = KMessageCentre::GetMessage(friend_message, INPUT_NEW_GROUP_NAME);
			KUiChannelCentre::GetSingleton().toSysMsg(message);
			m_pName->setText(String(""));
			m_root->removeChildWindow(m_pOkWindow);
			m_root->addChildWindow(m_pOkWindow);
			m_pName->show();
			m_pOkWindow->show();
			m_pOkPopup->hide();

		}
	}
	else if(userOper == "RenameGroup")
	{
		TreeItemEx*pItem = static_cast<TreeItemEx*>(static_cast<Tree*>(ms_Singleton->m_FriendList)->getFirstSelectedItem());
		if ( pItem )
		{
			if ( namePtr )
			{
				g_pCoreShell->OperationRequest( GOI_CHAT_GROUP_RENAME, (unsigned int)pItem->getIDEx(), namePtr  );
				m_pOkWindow->hide();
				m_pOkWindow->setSize(Absolute,m_OkNormalSize);
			}
			else
			{
				char *message = KMessageCentre::GetMessage(friend_message, NEW_NAME_GROUP);
				KUiChannelCentre::GetSingleton().toSysMsg(message);
				m_pName->setText(String(""));
				m_root->removeChildWindow(m_pOkWindow);
				m_root->addChildWindow(m_pOkWindow);
				m_pName->show();
				m_pOkWindow->show();
				m_pOkPopup->hide();	
			}
		}
	}
	else if(userOper == "AddFriend")
	{
		TreeItem *tmpItem = NULL;
		bool	 isInList = false;
		if ( namePtr )
		{
			tmpItem = (static_cast<Tree*>(m_EnemyList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				char *message = KMessageCentre::GetMessage(friend_message, IS_IN_ENEMY_LIST);
				strcpy( msg, (char *)namePtr);
				strcat( msg, message);
				KUiChannelCentre::GetSingleton().toSysMsg(msg);
				isInList = true;
			}

			tmpItem = (static_cast<Tree*>(m_ScreenList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				char *message = KMessageCentre::GetMessage(friend_message, IS_IN_SCREEN_LIST);
				strcpy( msg, (char *)namePtr);
				strcat( msg, message);
				KUiChannelCentre::GetSingleton().toSysMsg(msg);
				isInList = true;
			}

			tmpItem = (static_cast<Tree*>(m_TemporaryList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				int nParam[2];
				nParam[0]=GROUPID_TEMP;
				nParam[1]=GROUPID_NONE;
				
				g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)Utf8ToAnsi(tmpItem->getText()), (int)&nParam ); 
				isInList = true;
			}//endif

			tmpItem = (static_cast<Tree*>(m_FriendList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				if ( NULL == (static_cast<TLTreeEx*>(m_FriendList))->IsFirstLayer(name) )
				{
					char *message = KMessageCentre::GetMessage(friend_message, IS_IN_FRIEND_LIST);
					strcpy( msg, (char *)namePtr);
					strcat( msg, message);
					KUiChannelCentre::GetSingleton().toSysMsg(msg);
					isInList = true;
				}
			}

			if (!isInList)
			{
				g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_ADD, namePtr, GROUPID_NONE  );
			}

			m_pOkWindow->hide();
			m_pOkWindow->setSize(Absolute,m_OkNormalSize);	
		}
		else
		{
			char *message = KMessageCentre::GetMessage(friend_message, INPUT_ADD_FRIEND_NAME);
			KUiChannelCentre::GetSingleton().toSysMsg(message);
			m_pOkWindow->setText(String(""));
			m_root->removeChildWindow(m_pOkWindow);
			m_root->addChildWindow(m_pOkWindow);
			m_pName->show();
			m_pOkWindow->show();
			m_pOkPopup->hide();
		}
	}
	else if(userOper == "AddFriendToGroup")
	{
		if ( namePtr && m_pCurSelectItem )
		{
			int nParam[2];
			Tree* tree = static_cast<Tree*>(ms_Singleton->m_FriendList);
			if ( tree )
			{
				TreeItemEx* tmpItem = (TreeItemEx*)tree->findFirstItemWithText(name);
				if ( tmpItem )
				{
					nParam[0] = tmpItem->getIDEx();
					nParam[1] = m_pCurSelectItem->getIDEx();
					g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, namePtr, (int)&nParam );
				}
			}				
		}

		m_pOkWindow->hide();
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	}
	else if ( userOper == "DelFriend" )
	{
		if( m_pCurSelectItem )
		{
			unsigned int DeleteItemPtr = (unsigned int)Utf8ToAnsi(m_pCurSelectItem->getText());
			g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, DeleteItemPtr, NULL );
			m_pFriendListMenu->hide();

			m_pCurSelectItem = NULL;
		}
		else
		{
			if (namePtr)
			{
				TreeItem * pITem=((static_cast<Tree*>(m_FriendList))->findFirstItemWithText(AnsiToUtf8((const char *)namePtr)));
				
				if (pITem && pITem->getItemCount()==0)
					g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, namePtr, NULL );

				m_pFriendListMenu->hide();
			}//endif

		}

		m_pOkWindow->hide();
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	}
	else if ( userOper == "DelEnemy" )
	{
		if( m_pCurSelectItem )
		{
			unsigned int DeleteItemPtr = (unsigned int)Utf8ToAnsi(m_pCurSelectItem->getText());
	        if ( ((char *)DeleteItemPtr) != 0 )
			{
				g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, DeleteItemPtr, NULL );
				ms_Singleton->m_pFriendListMenu->hide();
			}

			m_pFriendListMenu->hide();
			m_pCurSelectItem = NULL;
		}
		else
		{
			if (namePtr)
			{
				TreeItem * pITem=((static_cast<Tree*>(m_EnemyList))->findFirstItemWithText(AnsiToUtf8((const char *)namePtr)));
				
				if (pITem && pITem->getItemCount()==0)
					g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, namePtr, NULL );
				
				m_pFriendListMenu->hide();
			}//endif
			
		}

		m_pOkWindow->hide();
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	}
	else if (userOper == "DelScreen")
	{ 
        if( m_pCurSelectItem )
		{
			unsigned int DeleteItemPtr = (unsigned int)Utf8ToAnsi(m_pCurSelectItem->getText());
			if ( ((char *)DeleteItemPtr) != 0 )
			{
				g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, DeleteItemPtr, NULL );
				ms_Singleton->m_pFriendListMenu->hide();
			}
			
			m_pFriendListMenu->hide();
			m_pCurSelectItem = NULL;
		}
		else
		{
			if (namePtr)
			{
				TreeItem * pITem=((static_cast<Tree*>(m_ScreenList))->findFirstItemWithText(AnsiToUtf8((const char *)namePtr)));
				
				if (pITem && pITem->getItemCount()==0)
					g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, namePtr, NULL );
				
				m_pFriendListMenu->hide();
			}//endif
			
		}

		m_pOkWindow->hide();
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	}//endif
	else if ( userOper == "AddScreen" )
	{
		TreeItem *tmpItem = NULL;
		bool	 isInList = false;
		
		if ( namePtr )
		{
			tmpItem = (static_cast<Tree*>(m_EnemyList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				char *message = KMessageCentre::GetMessage(friend_message, IS_IN_ENEMY_LIST);
				strcpy( msg, (char *)namePtr);
				strcat( msg, message);
				KUiChannelCentre::GetSingleton().toSysMsg(msg);
				isInList = true;
			}
			
			tmpItem = (static_cast<Tree*>(m_ScreenList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				char *message = KMessageCentre::GetMessage(friend_message, IS_IN_SCREEN_LIST);
				strcpy( msg, (char *)namePtr);
				strcat( msg, message);
				KUiChannelCentre::GetSingleton().toSysMsg(msg);
				isInList = true;
			}
			
			tmpItem = (static_cast<Tree*>(m_TemporaryList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				int nParam[2];
				nParam[0]=GROUPID_TEMP;
				nParam[1]=GROUPID_NONE;
				
				g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)Utf8ToAnsi(tmpItem->getText()), (int)&nParam ); 
				isInList = true;
			}//endif
			
			tmpItem = (static_cast<Tree*>(m_FriendList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				if ( NULL == (static_cast<TLTreeEx*>(m_FriendList))->IsFirstLayer(name) )
				{
					char *message = KMessageCentre::GetMessage(friend_message, IS_IN_FRIEND_LIST);
					strcpy( msg, (char *)namePtr);
					strcat( msg, message);
					KUiChannelCentre::GetSingleton().toSysMsg(msg);
					isInList = true;
				}
			}
			
			if (!isInList)
			{
				g_pCoreShell->OperationRequest( GOI_CHAT_ADD_BLACK_LIST, (unsigned int)Utf8ToAnsi(name), NULL );
			}//endif
			m_pOkWindow->hide();
			m_pOkWindow->setSize(Absolute,m_OkNormalSize);
		}
	}
	else if ( userOper == "AddToChatRoom" )
	{
	 	g_pCoreShell->OperationRequest( GOI_ADDTO_CHATROOM, m_nCurRoomID, (int)Utf8ToAnsi(name) );	
		m_pOkWindow->hide();
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	}
	else if ( userOper == "CreateRoom" )
	{
		g_pCoreShell->OperationRequest( GOI_CREATE_CHATROOM, (int)Utf8ToAnsi(name), NULL);
		m_pOkWindow->hide();
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	}
	else if (userOper == "AddEnemy")
	{
		TreeItem *tmpItem = NULL;
		bool	 isInList = false;
		if ( namePtr )
		{
			
			tmpItem = (static_cast<Tree*>(m_EnemyList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				char *message = KMessageCentre::GetMessage(friend_message, IS_IN_ENEMY_LIST);
				strcpy( msg, (char *)namePtr);
				strcat( msg, message);
				KUiChannelCentre::GetSingleton().toSysMsg(msg);
				isInList = true;
			}
			
			tmpItem = (static_cast<Tree*>(m_ScreenList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				char *message = KMessageCentre::GetMessage(friend_message, IS_IN_SCREEN_LIST);
				strcpy( msg, (char *)namePtr);
				strcat( msg, message);
				KUiChannelCentre::GetSingleton().toSysMsg(msg);
				isInList = true;
			}
			
			
			tmpItem = (static_cast<Tree*>(m_TemporaryList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				int nParam[2];
				nParam[0]=GROUPID_TEMP;
				nParam[1]=GROUPID_NONE;
				
				g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)Utf8ToAnsi(tmpItem->getText()), (int)&nParam ); 
				isInList = true;
			}
			
			tmpItem = (static_cast<Tree*>(m_FriendList))->findFirstItemWithText(name);
			if ( NULL != tmpItem )
			{
				if ( NULL == (static_cast<TLTreeEx*>(m_FriendList))->IsFirstLayer(name) )
				{
					char *message = KMessageCentre::GetMessage(friend_message, IS_IN_FRIEND_LIST);
					strcpy( msg, (char *)namePtr);
					strcat( msg, message);
					KUiChannelCentre::GetSingleton().toSysMsg(msg);
					isInList = true;
				}
			}

			if (!isInList)
			{
				g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_ADD, namePtr, GROUPID_ENEMY  );
			}
			m_pOkWindow->hide();
			m_pOkWindow->setSize(Absolute,m_OkNormalSize);
		}

	}
	else if (userOper == "AddToSpecificGroup")
	{
		if (m_pCurSelectItem)
		{
            TreeItemEx  * pGroupItem = ( TreeItemEx  *)((TLTreeEx *)m_FriendList)->findFirstItemWithText(name);
			if (pGroupItem)
			{
                int nParam[2];
				nParam[0]=m_pCurSelectItem->getIDEx();
				nParam[1]=pGroupItem->getIDEx();
				
				g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_CHANGE_GROUP, (unsigned int)Utf8ToAnsi(m_pCurSelectItem->getText()), (int)&nParam ); 
			}//endif
	
		}//endif

		m_pOkWindow->hide();
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	}
	else
	{
		// error message
	}

	
	//m_pOkWindow->hide();
	return true;
}

bool KUiChatCentre::OnCancel( const CEGUI::EventArgs& args )
{
	m_pOkWindow->hide();
	m_pOkWindow->setSize(Absolute,m_OkNormalSize);
	return true;
}

bool KUiChatCentre::onOkWndShow( const CEGUI::EventArgs& args )
{	
	//m_pOkWindow->setModalState(true);
	//m_pOkWindow->setZLevel(Window::Top);
	m_pName->activate();
	
    disableAllList();

	return true;
}

bool KUiChatCentre::onOkWndHide( const CEGUI::EventArgs& args )
{
	//m_pOkWindow->setModalState(false);
	//m_pOkWindow->setZLevel(Window::Top);
	m_pOkWindow->hide();
	m_pOkWindow->setSize(Absolute,m_OkNormalSize);

	if (m_Popup1 && m_Popup1->isVisible(true) )
	{
		m_pOkWindow->setSize(Absolute,m_OkNormalSize);
		m_Popup1->hide();
	}//endif

	if (m_PopupBack)
	{
		m_PopupBack->hide();
	}

	enableAllList();
	m_PopupState  = PS_INVALID;
	return true;	
}

/*!
\brief
	Friend list callback function. 		
*/
void KUiChatCentre::onRecvFriendListArrive( void )
{
	if ( ms_Singleton == NULL || (ms_Singleton && ms_Singleton->m_pThisWnd == NULL ) ) 
	{
		return;
	}//endif
	
	static_cast<Tree*>(ms_Singleton->m_FriendList)->removeAllItem();
	ZeroMemory(ms_Singleton->m_GroupNameList, sizeof(char) * MAX_FRIENDGROUP_COUNT * CLIENT_NAME_AND_TITLE_MAX+1);
	int nCroupCount = g_pCoreShell->GetGameData( GDI_CHAT_GROUP_INFO, (unsigned int)ms_Singleton->m_GroupIDList, (int)ms_Singleton->m_GroupNameList );
	
	for ( int nFriendGroupIdx = 0; nFriendGroupIdx < nCroupCount; ++nFriendGroupIdx )
	{
		int exID = ms_Singleton->m_GroupIDList[nFriendGroupIdx];
		if ( ms_Singleton->m_GroupIDList[nFriendGroupIdx] == (int)GROUPID_BLACK )
		{
			onRecvBlackFriendListArrive( ms_Singleton->m_GroupIDList[nFriendGroupIdx]);
			sortListByOnline((TLTreeEx *)ms_Singleton->m_ScreenList);
			continue;
		}
	
		if ( ms_Singleton->m_GroupIDList[nFriendGroupIdx] == (int)GROUPID_TEMP )
		{
			onRecvTemporaryFriendListArrive( ms_Singleton->m_GroupIDList[nFriendGroupIdx] );
			sortListByOnline((TLTreeEx *)ms_Singleton->m_TemporaryList);
			continue;
		}

		if ( ms_Singleton->m_GroupIDList[nFriendGroupIdx] == (int)GROUPID_ENEMY )
		{
			onRecvEmeyFriendListArrive( ms_Singleton->m_GroupIDList[nFriendGroupIdx] );
			sortListByOnline((TLTreeEx *)ms_Singleton->m_EnemyList);
			g_pCoreShell->GetGameData(GDI_CHAT_UPDATE_PK_VALUE,0,0);
			continue;
		}//*/
		
		TreeItemEx* friendGroup = new TreeItemEx( AnsiToUtf8( ms_Singleton->m_GroupNameList[nFriendGroupIdx]) );
		friendGroup->setIsViewOtherColor(true);
		m_freelist.push_back(friendGroup);
		friendGroup->setIDEx( ms_Singleton->m_GroupIDList[nFriendGroupIdx] );
		static_cast<Tree*>(ms_Singleton->m_FriendList)->addItem( (TreeItem*)friendGroup );

		UI_CHAT_OBJINFO  m_FriendNameList[COMMON_CLIENT_MSG_LEN_256]; //
		int nFriendInGroupCount = g_pCoreShell->GetGameData( GDI_CHAT_FRIENDS_IN_A_GROUP, (unsigned int)ms_Singleton->m_GroupIDList[nFriendGroupIdx], (int)m_FriendNameList );
		
		if ( nFriendInGroupCount > COMMON_CLIENT_MSG_LEN_256)
		{
			nFriendInGroupCount = COMMON_CLIENT_MSG_LEN_256;
		}//endif

		std::vector<TreeItemEx *> NotOnLine;
		for ( int nFriendIdx = 0; nFriendIdx < nFriendInGroupCount; ++nFriendIdx )
		{
			String friendName = AnsiToUtf8( ((UI_CHAT_OBJINFO)m_FriendNameList[nFriendIdx]).szName );//
			bool   bOnLine    = ((UI_CHAT_OBJINFO)m_FriendNameList[nFriendIdx]).bOnline;
			TreeItemEx *Friend = new TreeItemEx( friendName );
			Friend->setIsViewOtherColor(bOnLine);
			Friend->setCanDrag(true);

			if( bOnLine )
			{
				friendGroup->addItem( Friend );	
			}//endif
			else
			{
				NotOnLine.push_back(Friend);
			}//end else

			Friend->setIDEx( exID );
			m_freelist.push_back(Friend);
		}//end for nFriendIdx

		std::vector<TreeItemEx *>::iterator itor = NotOnLine.begin();
		KUiCfgLoader& cfgLoader = KUiCfgLoader::getSingleton();
		for ( ; itor != NotOnLine.end(); itor++ )
		{
			friendGroup->addItem( *itor );
			(*itor)->setNormalColor(PropertyHelper::stringToColour(cfgLoader.getFriendListCfg().offLineNormalColor));
			(*itor)->setPushedColor(PropertyHelper::stringToColour(cfgLoader.getFriendListCfg().offLinePushedColor));
			(*itor)->setHoverColor(PropertyHelper::stringToColour(cfgLoader.getFriendListCfg().offLineHoverColor));
		}//end for 

		if ( ms_Singleton->m_FriendList != NULL)
		{
			static_cast<TLTree *>(ms_Singleton->m_FriendList)->setAllOpenItem();
		}//endif 

		sortListByOnline((TLTreeEx *)ms_Singleton->m_FriendList);
	}
}

void	KUiChatCentre::onRecvBlackFriendListArrive( int nGroupID )
{
	commonAddItem(GDI_CHAT_FRIENDS_IN_A_GROUP, static_cast<Tree*>(m_ScreenList), nGroupID );
}

void	KUiChatCentre::onRecvEmeyFriendListArrive( int nGroupID )
{
	commonAddItem(GDI_CHAT_FRIENDS_IN_A_GROUP, static_cast<Tree*>(m_EnemyList), nGroupID );
}

void	KUiChatCentre::onRecvTemporaryFriendListArrive( int nGroupID )
{
	commonAddItem(GDI_CHAT_FRIENDS_IN_A_GROUP, static_cast<Tree*>(m_TemporaryList), nGroupID );
}

/************************************************************************/
/*							Enemy list page                             */
/************************************************************************/
bool KUiChatCentre::showEnemyList( const CEGUI::EventArgs& args )
{
	m_State = enemyPage;
	HideAllList();

	if(m_pFriendTreeScrol != NULL && m_EnemyList != NULL)
	{
		m_pFriendTreeScrol->setScrollPosition(0.0f);
		m_EnemyList->setPosition(Absolute, Point(0, 0));
	}

	if ( m_pThisWnd )
	{	
		m_pFriendTreeCliper->show();
		m_EnemyList->show();
		m_EnemyList->setEnabled(true);
		m_pAddEnemy->show();
		m_pCurSelectItem = NULL;
		
	}
	if ( m_pFriendGroupMenu->isVisible() )
	{
		m_pFriendGroupMenu->hide();
	}
	if ( m_pFriendGroupMenu->isVisible() )
	{
		m_pFriendGroupMenu->hide();
	}

	return true;
}

bool KUiChatCentre::clickAddEnemyBtn(const EventArgs& args )
{
    char *message = KMessageCentre::GetMessage(friend_message, INPUT_ADD_ENEMY_NAME);
	m_pOkWindow->setUserString("Operation","AddEnemy");
	m_pOkWinText->setText(AnsiToUtf8(message));
	m_pName->show();
	m_root->removeChildWindow(m_pOkWindow);
	m_root->addChildWindow(m_pOkWindow);
	m_pOkWindow->show();
	m_pOkPopup->hide();
	m_pName->setText(String(""));
	return true;
}

bool KUiChatCentre::clickDelEnemyBtn( const EventArgs& args	)
{
	if ( m_pCurSelectItem )
	{
		showIsOperation(IS_DEL_ENEMY_NAME);
	}
	else
	{
		char *message = KMessageCentre::GetMessage(friend_message, SELECT_DEL_ENEMY_NAME);
		m_pOkWindow->setUserString("Operation","DelEnemy");
		m_pOkWinText->setText(AnsiToUtf8(message));	
		m_pFriendGroupMenu->hide();
		m_pName->show();
		m_pName->setText(String(""));
		m_root->removeChildWindow(m_pOkWindow);
		m_root->addChildWindow(m_pOkWindow);
		m_pOkWindow->show();
		m_pOkPopup->hide();

	}
	return true;
}

/************************************************************************/
/*							Screen list page                            */
/************************************************************************/
bool KUiChatCentre::showScreenList		( const CEGUI::EventArgs& args		)
{
	m_State = screenPage;
	HideAllList();

	if(m_pFriendTreeScrol != NULL && m_ScreenList != NULL )
	{
		m_pFriendTreeScrol->setScrollPosition(0.0f);
		m_ScreenList->setPosition(Absolute, Point(0,0));
	}

	if ( m_pThisWnd )
	{	
		m_pFriendTreeCliper->show();
		m_ScreenList->show();	
		m_ScreenList->setEnabled(true);
  

		m_pAddScreen->show();
		m_pDelScreen->show();
		m_pCurSelectItem = NULL;
	}
	if ( m_pFriendGroupMenu->isVisible() )
	{
		m_pFriendGroupMenu->hide();
	}
	if ( m_pFriendListMenu->isVisible() )
	{
		m_pFriendListMenu->hide();
	}
	return true;
}

bool KUiChatCentre::clickAddScreenBtn( const EventArgs& args	)
{

	char *message = KMessageCentre::GetMessage(friend_message, INPUT_SCREEN_PLAYER_NAME);
	m_pOkWindow->setUserString("Operation","AddScreen");
	m_pOkWinText->setText(AnsiToUtf8(message));	
	m_pFriendGroupMenu->hide();
	m_pName->show();
	m_root->removeChildWindow(m_pOkWindow);
	m_root->addChildWindow(m_pOkWindow);
	m_pOkWindow->show();
	m_pOkPopup->hide();
	m_pName->setText(String(""));
	return true;
}

bool KUiChatCentre::clickDelScreenBtn( const EventArgs& args	)
{
	if ( m_pCurSelectItem )
	{
		showIsOperation(IS_DEL_SCREEN_NAME);
	}
	else
	{
		char *message = KMessageCentre::GetMessage(friend_message, SELECT_DEL_SCREEN_NAME);
		m_pOkWindow->setUserString("Operation","DelScreen");
		m_pOkWinText->setText(AnsiToUtf8(message));	
		m_pFriendGroupMenu->hide();
		m_pName->show();
		m_pOkWindow->show();
		m_pOkPopup->hide();
		m_root->removeChildWindow(m_pOkWindow);
		m_root->addChildWindow(m_pOkWindow);
		m_pName->setText(String(""));
	}
	return true;
}

/************************************************************************/
/*							Chat room list page                         */
/************************************************************************/
bool KUiChatCentre::showChatList		( const CEGUI::EventArgs& args		)
{
	m_State = chatroomPage;
	HideAllList();
	m_pCurSelectItem = NULL;

	if(m_pFriendTreeScrol != NULL )
	{
		m_pFriendTreeScrol->setScrollPosition(0.0f);
		//m_FriendList->setPosition(Absolute, Point(0,0));
	}

	if ( m_pThisWnd )
	{
		m_ChatRoomList->show();
		m_ChatRoomList->setEnabled(true);
		//m_pThisWnd->getChild( "TaharezLook/ChatCentre/RoleName" )->show();
		m_pThisWnd->getChild( "TaharezLook/ChatCentre/CreateRoom" )->show();
	//	m_pThisWnd->getChild( "TaharezLook/ChatCentre/AddToRoom" )->show();
		m_pCreateRoom->show();
		m_pCurSelectItem = NULL;
	}
	
	if ( m_pFriendGroupMenu->isVisible() )
	{
		m_pFriendGroupMenu->hide();
	}
	if ( m_pFriendListMenu->isVisible() )
	{
		m_pFriendListMenu->hide();
	}
	return true;
}

bool KUiChatCentre::freindlistDC(const EventArgs& args )
{
	CEGUI::TreeEventArgs & treeargs=(CEGUI::TreeEventArgs &)args;
	
	TreeItemEx *     pItem = (TreeItemEx *)treeargs.treeItem;
	
	if (pItem && (m_State==temporaryPage || pItem->getIsViewOtherColor()) && ((m_State==friendPage && pItem->getLayer()==1) || (m_State!=friendPage && m_State!=chatroomPage)))
	{
		char  szName[32] = "";
		sprintf(szName,Utf8ToAnsi(pItem->getText()));
		
		if ( m_State == enemyPage )
		{
			int nLen = strlen(szName);
			for (int comp = 0 ; comp < nLen && szName[comp]; comp ++ )
			{
				if (szName[comp] == ' ')
				{
					szName[comp] = 0;
					break;
				}//endif
				
			}//endif
			
		}//endif

		KUiChatInputWnd::GetSingleton().clearText();
		
		KUiChatInputWnd::GetSingleton().write("/");
		
		KUiChatInputWnd::GetSingleton().write(szName);
		KUiChatInputWnd::GetSingleton().write(" ");
		
		KUiChatInputWnd::GetSingleton().show();
	}//endif
	
	return true;
}

/************************************************************************/
/*							Temporary list page                         */
/************************************************************************/
bool KUiChatCentre::showTemporaryList		( const CEGUI::EventArgs& args		)
{
	m_State = temporaryPage;
	HideAllList();

	if(m_pFriendTreeScrol != NULL && m_TemporaryList != NULL )
	{
		m_pFriendTreeScrol->setScrollPosition(0.0f);
		m_TemporaryList->setPosition(Absolute, Point(0,0));
	}

	if ( m_pThisWnd )
	{
		m_pFriendTreeCliper->show();
        m_TemporaryList->show();
		m_TemporaryList->setEnabled(true);

		m_pCurSelectItem = NULL;
	}
	if ( m_pFriendGroupMenu->isVisible() )
	{
		m_pFriendGroupMenu->hide();
	}
	if ( m_pFriendListMenu->isVisible() )
	{
		m_pFriendListMenu->hide();
	}
	return true;
}


bool KUiChatCentre::handleMouseClick( const EventArgs& args )
{
	//右键菜单消失
	const MouseEventArgs *mouseArgs = (MouseEventArgs *)(&args);
	if ( mouseArgs->button != RightButton )
	{
		if ( m_pFriendGroupMenu->isVisible())
		{
			m_pFriendGroupMenu->hide();
		}
		if ( m_pFriendListMenu->isVisible())
		{
			m_pFriendListMenu->hide();
		}
	}
	//选中item项
	TLTree* tree = NULL;
	switch( m_State )
	{
	case friendPage:
		{
			tree = reinterpret_cast<TLTree*>(m_FriendList);
		}
		break;
	case enemyPage:
		{
			tree = reinterpret_cast<TLTree*>(m_EnemyList);
		}
		break;
	case screenPage:
		{
			tree = reinterpret_cast<TLTree*>(m_ScreenList);
		}
		break;
	case temporaryPage:
		{
			tree = reinterpret_cast<TLTree*>(m_TemporaryList);
		}
		break;
	case chatroomPage:
		{
			tree  = reinterpret_cast<TLTree*>(m_ChatRoomList);
		}
	default:
		break;
	}
	if ( tree != NULL )
	{
		m_pCurSelectItem = (TreeItemEx*)(tree->getFirstSelectedItem());
	}
	static_cast<TLTree*>(m_FriendList)->findSetIsOpen(m_pCurSelectItem);
	return true;
}

//显示确认界面
void KUiChatCentre::showIsOperation( FRIEND_WINDOW_MESSAGE msg )
{
	KUiComMsgBox::GetSingleton().setModalStatus(true);
	KUiComMsgBox::GetSingleton().setComMsgPosition();
	KUiComMsgBox::Show();
	char	*warnMsg = KMessageCentre::GetMessage( friend_message, msg );
	KUiComMsgBox::GetSingleton().setMsg(AnsiToUtf8(warnMsg));
	char	yesButton[COMMON_CLIENT_MSG_LEN_32];
	strcpy(yesButton, KMessageCentre::GetMessage( friend_message, YES ));
	char	*noButton = KMessageCentre::GetMessage( friend_message, NO );
	KUiComMsgBox::GetSingleton().setBtnName( AnsiToUtf8(yesButton), AnsiToUtf8(noButton));
	switch( msg )
	{
	case IS_DELETE_CROUP:
		{
			KUiComMsgBox::GetSingleton().setFristBtnCallback(KUiChatCentre::YesDelGroup);
		}
		break;
	case IS_DELETE_FRIEND:
		{
			KUiComMsgBox::GetSingleton().setFristBtnCallback(KUiChatCentre::IsDelFriend);
		}
		break;
	case IS_DEL_SCREEN_NAME:
		{
			KUiComMsgBox::GetSingleton().setFristBtnCallback(KUiChatCentre::IsDelScreen);
		}
		break;
	case IS_ADD_SCREEN:
		{
			KUiComMsgBox::GetSingleton().setFristBtnCallback(KUiChatCentre::IsAddScreenList);
		}
		break;
	case IS_DEL_ENEMY_NAME:
		{
			KUiComMsgBox::GetSingleton().setFristBtnCallback(KUiChatCentre::IsDelEnemy);
		}
		break;
	default:
		{

		}
		break;
	}
}

//确认要删除分组
void KUiChatCentre::YesDelGroup(void)
{
	if(ms_Singleton->m_pCurSelectItem)
	{
		//发送删除好友列表命令到游戏服务器
		g_pCoreShell->OperationRequest( GOI_CHAT_GROUP_DELETE, ms_Singleton->m_pCurSelectItem->getIDEx(), NULL );
	}
}

//确认要将好友加到黑名单中？
void KUiChatCentre::IsAddScreenList(void)
{
	int nParam[2];
	if ( g_pCoreShell != NULL  && ms_Singleton->m_pCurSelectItem != NULL )
	{
		unsigned int pName = (unsigned int)Utf8ToAnsi(ms_Singleton->m_pCurSelectItem->getText());;
		if ( pName )
		{
			nParam[0] = ms_Singleton->m_pCurSelectItem->getIDEx();
			nParam[1] = GROUPID_BLACK;
			g_pCoreShell->OperationRequest( GOI_CHAT_ADD_BLACK_LIST, pName, NULL );
		}
	}
	else
	{
		char *message = KMessageCentre::GetMessage(friend_message, SELECT_PLAYER_TO_SCREEN);
		KUiChannelCentre::GetSingleton().toSysMsg(message);
	}
}

//确认要删除好友？
void KUiChatCentre::IsDelFriend( void )
{
	if( ms_Singleton->m_pCurSelectItem != NULL && g_pCoreShell != NULL )
	{
		char  szName[32] = "";
		sprintf(szName,Utf8ToAnsi( ms_Singleton->m_pCurSelectItem->getText()));
		
		if ( ms_Singleton->m_State == enemyPage )
		{
			int nLen = strlen(szName);
			for (int comp = 0 ; comp < nLen && szName[comp]; comp ++ )
			{
				if (szName[comp] == ' ')
				{
					szName[comp] = 0;
					break;
				}//endif
				
			}//endif
			
		}//endif

		unsigned int DeleteItemPtr = (unsigned int)szName;
		if ( ((char *)DeleteItemPtr) != 0 )
		{
			g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, DeleteItemPtr, NULL );
			ms_Singleton->m_pFriendListMenu->hide();
		}
	}
	else
	{
		char *message = KMessageCentre::GetMessage(friend_message, SELECT_DEL_FRIEND_NAME);
		KUiChannelCentre::GetSingleton().toSysMsg(message);
	}
}

//确认要删除黑名单中的玩家
void KUiChatCentre::IsDelScreen( void )
{
	if(ms_Singleton->m_pCurSelectItem )
	{
		unsigned int DeleteItemPtr = (unsigned int)Utf8ToAnsi(ms_Singleton->m_pCurSelectItem->getText());
		if ( ((char *)DeleteItemPtr) != 0 )
		{
			g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, DeleteItemPtr, NULL );
			ms_Singleton->m_pFriendListMenu->hide();
		}
	}
}


//
void KUiChatCentre::IsDelEnemy( void )
{
	if(ms_Singleton->m_pCurSelectItem )
	{
		char  szName[32] = "";
		sprintf(szName,Utf8ToAnsi( ms_Singleton->m_pCurSelectItem->getText()));
		
		if ( ms_Singleton->m_State == enemyPage )
		{
			int nLen = strlen(szName);
			for (int comp = 0 ; comp < nLen && szName[comp]; comp ++ )
			{
				if (szName[comp] == ' ')
				{
					szName[comp] = 0;
					break;
				}//endif
				
			}//endif
			
		}//endif

		unsigned int DeleteItemPtr = (unsigned int)szName;
		if ( ((char *)DeleteItemPtr) != 0 )
		{
			g_pCoreShell->OperationRequest( GOI_CHAT_FRIEND_DELETE, DeleteItemPtr, NULL );
			ms_Singleton->m_pFriendListMenu->hide();
		}
	}
}
//通用加载好友列表函数
void KUiChatCentre::commonAddItem(int nID, Tree *pTreeList, unsigned int iGroupId )
{
	pTreeList->removeAllItem();
	UI_CHAT_OBJINFO m_FriendNameList[COMMON_CLIENT_MSG_LEN_256]; 
	int nFriendInGroupCount = g_pCoreShell->GetGameData( nID, (unsigned int)iGroupId, (int)m_FriendNameList );
	
	if ( nFriendInGroupCount > COMMON_CLIENT_MSG_LEN_256 )
	{
		nFriendInGroupCount = COMMON_CLIENT_MSG_LEN_256;
	}

	if (nFriendInGroupCount == 0)
		return ;

	KUiCfgLoader& cfgLoader = KUiCfgLoader::getSingleton();

	for ( int nFriendIdx = 0; nFriendIdx < nFriendInGroupCount; ++nFriendIdx )
	{
		String friendName = AnsiToUtf8( ((UI_CHAT_OBJINFO)m_FriendNameList[nFriendIdx]).szName );//
		bool   bIsOnline  = ((UI_CHAT_OBJINFO)m_FriendNameList[nFriendIdx]).bOnline;
		TreeItemEx *Friend = new TreeItemEx( friendName );//
		Friend->setIsViewOtherColor(bIsOnline);
		Friend->setCanDrag(true);
		
		if ( bIsOnline )
		{
			pTreeList->addItem( (TreeItem*)Friend );
		}
		else
		{
			pTreeList->addItem(Friend);
			Friend->setNormalColor(PropertyHelper::stringToColour(cfgLoader.getFriendListCfg().offLineNormalColor));
			Friend->setPushedColor(PropertyHelper::stringToColour(cfgLoader.getFriendListCfg().offLinePushedColor));
			Friend->setHoverColor(PropertyHelper::stringToColour(cfgLoader.getFriendListCfg().offLineHoverColor));
		}

		Friend->setIDEx( iGroupId );
		m_freelist.push_back(Friend);

	}//end for nFriend
	
}

bool KUiChatCentre::handleScreenShortCut( const EventArgs& args )
{
	return true;
}

KUiChatRoom* KUiChatCentre::getActiveChatRoom( void )
{
	return m_pActiveRoom;
}

void	KUiChatCentre::setActiveChatRoom( KUiChatRoom* pRoom )
{
	if ( pRoom && pRoom->IsVisible() )
	{
		m_pActiveRoom = pRoom;
	}
}

bool KUiChatCentre::handleFriendScroll( const EventArgs& args )
{
	if ( NULL == m_pFriendTreeCliper )
	{
		return false;
	}

	if ( NULL == m_FriendList )
	{
		return false;
	}

	if ( NULL == m_pFriendTreeScrol )
	{
		return false;
	}
	
	WindowEventArgs *arg = (WindowEventArgs *)(&args);
	float  cliperHeight = m_pFriendTreeCliper->getHeight(Absolute);
	switch( m_State )
	{
	case friendPage:
		{
			TLTreeEx *pFriendTree = static_cast<TLTreeEx *>(m_FriendList);
			if ( pFriendTree != NULL && arg != NULL && arg->window != NULL )
			{
				setTreePos( pFriendTree, arg->window );
			}
		}
		break;
	case enemyPage:
		{
			TLTreeEx *pEnemyTree = static_cast<TLTreeEx *>(m_EnemyList);
			if ( pEnemyTree != NULL && arg != NULL && arg->window != NULL )
			{
				setTreePos( pEnemyTree, arg->window );
			}
		}
		break;
	case screenPage:
		{
			TLTreeEx *pScreenTree = static_cast<TLTreeEx *>(m_ScreenList);
			if ( pScreenTree != NULL && arg != NULL && arg->window != NULL )
			{
				setTreePos( pScreenTree, arg->window );
			}
		}
		break;
	case chatroomPage:
		{
		}
		break;
	case temporaryPage:
		{
			TLTreeEx *pTemporaryTree = static_cast<TLTreeEx *>(m_TemporaryList);
			if ( pTemporaryTree != NULL && arg != NULL && arg->window != NULL )
			{
				setTreePos( pTemporaryTree, arg->window );
			}
		}
		break;
	default:
		break;

	}
	/*float  treeHeight	= static_cast<TLTreeEx *>(m_FriendList)->getTreeTotalItemsHeigh();
	float  ypos = 0;
	
	WindowEventArgs *arg = (WindowEventArgs *)(&args);
	if ( arg->window == m_pFriendTreeScrol )
	{
		float scrollPos = m_pFriendTreeScrol->getScrollPosition();
		if ( treeHeight > cliperHeight )
		{
			ypos = (treeHeight - cliperHeight) * scrollPos;
			Point pos;
			pos.d_x = m_FriendList->getPosition(Absolute).d_x;
			pos.d_y = 0 - ypos;
			m_FriendList->setPosition(Absolute, pos);
		}
		else
		{
			m_FriendList->setPosition(Absolute,Point(0, 0));
		}
	}*/
	return true;
}


void	KUiChatCentre::AddChatRoom( const char* szRoomName, int nRoomID )
{
	if ( m_ChatRoomList )
	{
		TreeItemEx* pItem = (TreeItemEx*)((TLTree*)m_ChatRoomList)->findFirstItemWithText( AnsiToUtf8(szRoomName) );

		while (pItem)
		{
		   if (pItem->getID()==nRoomID)
			   break;

           pItem = (TreeItemEx*)((TLTree*)m_ChatRoomList)->findNextItemWithText( AnsiToUtf8(szRoomName),pItem );
		}

		if ( !pItem )
		{
			TreeItemEx* item = new TreeItemEx(AnsiToUtf8(szRoomName),nRoomID);
			m_freelist.push_back(item);
			((TLTree*)m_ChatRoomList)->addItem( item );
		}
	}

}

KChatMsgList & KUiChatCentre::GetChatMsgList()
{
	return m_ChatMsgList;
}

void	KUiChatCentre::DelChatRoom( int nRoomID )
{
	if ( m_ChatRoomList )
	{
		TreeItemEx* pItem = (TreeItemEx*)((TLTree*)m_ChatRoomList)->findFirstItemWithID( nRoomID );
		
		if ( pItem )
		{
			if (pItem==m_pCurSelectItem)
				m_pCurSelectItem=NULL;

			((TLTree*)m_ChatRoomList)->removeItem( pItem );
			delete pItem;
		}

		KUiChatRoomSetP2R::iterator it = m_ChatRoomSet.find( nRoomID );
		if ( it != m_ChatRoomSet.end() )
		{
			if ( it->second.pRoom )
			{
				it->second.pRoom->Hide();
				if (m_pActiveRoom == it->second.pRoom)
					m_pActiveRoom = NULL;
			}
		}
	}
}


//
void KUiChatCentre::setTreePos( TLTreeEx *pTree, Window *pWindow )
{
	float  cliperHeight = m_pFriendTreeCliper->getHeight(Absolute);
	float  treeHeight	= pTree->getTreeTotalItemsHeigh();
	float  ypos = 0;
	
	if ( pWindow == m_pFriendTreeScrol )
	{
		float scrollPos = m_pFriendTreeScrol->getScrollPosition();
		if ( treeHeight > cliperHeight )
		{
			ypos = (treeHeight - cliperHeight) * scrollPos;
			Point pos;
			pos.d_x = pTree->getPosition(Absolute).d_x;
			pos.d_y = 0 - ypos;
			pTree->setPosition(Absolute, pos);
		}
		else
		{
			pTree->setPosition(Absolute,Point(0, 0));
		}
	}
}

template<> 
KUiChatRoomListMenu * KUiWndSingleton<KUiChatRoomListMenu>::ms_Singleton = NULL;

KUiChatRoomListMenu::KUiChatRoomListMenu(const String & id_Name) :
KUiWndSingleton<KUiChatRoomListMenu>(id_Name)
{

}

KUiChatRoomListMenu::~KUiChatRoomListMenu()
{

}

void KUiChatRoomListMenu::Init()
{
#ifndef _DEBUG
	try
	{
#endif
		if (m_pThisWnd != NULL)
		{
			m_ButtonList[KUiChatRoomListMenu::Button0] = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/ChatRoomListMenu/Button0"));
			m_ButtonList[KUiChatRoomListMenu::Button1] = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/ChatRoomListMenu/Button1"));
			m_ButtonList[KUiChatRoomListMenu::Button2] = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/ChatRoomListMenu/Button2"));
			m_ButtonList[KUiChatRoomListMenu::Button3] = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/ChatRoomListMenu/Button3"));
			m_ButtonList[KUiChatRoomListMenu::Button4] = static_cast<TLButton *>(m_pThisWnd->getChild("TaharezLook/ChatRoomListMenu/Button4"));

			m_ButtonList[KUiChatRoomListMenu::Button0]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChatRoomListMenu::Button0_MouseClick, this));
			m_ButtonList[KUiChatRoomListMenu::Button1]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChatRoomListMenu::Button1_MouseClick, this));
			m_ButtonList[KUiChatRoomListMenu::Button2]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChatRoomListMenu::Button2_MouseClick, this));
			m_ButtonList[KUiChatRoomListMenu::Button3]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChatRoomListMenu::Button3_MouseClick, this));
			m_ButtonList[KUiChatRoomListMenu::Button4]->subscribeEvent(TLButton::EventMouseClick, Event::Subscriber(&KUiChatRoomListMenu::Button4_MouseClick, this));

			m_pThisWnd->setRenderMode( false, 3 );
		}
#ifndef _DEBUG
	}
	catch (...)
	{
		for (int i = 0; i < KUiChatRoomListMenu::ButtonCount; ++i)
		{
			m_ButtonList[i] = NULL;
		}
	}
#endif
}

void KUiChatRoomListMenu::DeletePlayerName(std::string playerName)
{
	CEGUI::String tempName = AnsiToUtf8(playerName.c_str());
	RoomNameList::iterator it;
	RoomNameList::iterator tempIt;
	for (it = m_NameList.begin(); it != m_NameList.end(); ++it)
	{
		CEGUI::String externName = *it;
		if (externName.compare(tempName) == 0)
		{
			tempIt = it;
			++it;
			m_NameList.erase(tempIt);
			break;
		}
	}
	RefreshPlayerName();
	ResizeMenu();
}

void KUiChatRoomListMenu::AddPlayerName(std::string playerName)
{
	CEGUI::String tempName = AnsiToUtf8(playerName.c_str());
	RoomNameList::iterator it;
	bool isExtern = false;
	for (it = m_NameList.begin(); it != m_NameList.end(); ++it)
	{
		CEGUI::String externName = *it;
		if (externName.compare(tempName) == 0)
		{
			isExtern = true;
			break;
		}
		else
		{
			isExtern = false;
		}
	}
	if (!isExtern)
	{	
		m_NameList.push_back(tempName);
	}
	RefreshPlayerName();
	ResizeMenu();
}

void KUiChatRoomListMenu::Show()
{
	KUiWndSingleton<KUiChatRoomListMenu>::Show();

	if (ms_Singleton != NULL)
	{
		ms_Singleton->RefreshPlayerName();
		ms_Singleton->ResizeMenu();
	}
}

bool KUiChatRoomListMenu::IsNameListEmpty()
{
	return m_NameList.empty();
}

void KUiChatRoomListMenu::SetPos(Point pos)
{
	m_pThisWnd->setPosition(Absolute, pos);
}

void KUiChatRoomListMenu::RefreshPlayerName()
{
	RoomNameList::iterator it = m_NameList.begin();
	for (int i = 0; i < KUiChatRoomListMenu::ButtonCount; ++i)
	{
		if (it != m_NameList.end())
		{
			CEGUI::String playerName = *it;
			if (m_ButtonList[i] != NULL)
			{
				m_ButtonList[i]->setText(playerName);
				m_ButtonList[i]->show();
			}
			++it;
		}
		else
		{
			if (m_ButtonList[i] != NULL)
			{
				m_ButtonList[i]->hide();
			}
		}
	}
}

void KUiChatRoomListMenu::ResizeMenu()
{
	int showButton = 0;
	for (int i = 0; i < KUiChatRoomListMenu::ButtonCount; ++i)
	{
		if (m_ButtonList[i] != NULL)
		{
			if (m_ButtonList[i]->isVisible())
			{
				++showButton;
			}
		}
	}
	if (m_pThisWnd != NULL)
	{
		if (showButton == 0)
		{
			m_pThisWnd->setSize(Absolute, Size(0, 0));
		}
		else
		{
			m_pThisWnd->setSize(Absolute, Size(135, showButton * 17 + 12));
		}
	}
}

void KUiChatRoomListMenu::ClickButton(TLButton * button)
{
	if (button != NULL)
	{
		RoomNameList::iterator roomNameIt = m_NameList.begin();
		RoomNameList::iterator tempRoomNameIt;
		for (int i = 0; i < KUiChatRoomListMenu::ButtonCount; ++i)
		{
			if (roomNameIt != m_NameList.end())
			{
				CEGUI::String playerName = *roomNameIt;
				if (playerName.compare(button->getText()) == 0)
				{
					tempRoomNameIt = roomNameIt;
					++roomNameIt;
					m_NameList.erase(tempRoomNameIt);
					break;
				}
				else
				{
					++roomNameIt;
				}
			}
		}

		std::string strTest;
		KChatMsgList & chatMsgList = KUiChatCentre::GetSingleton().GetChatMsgList();
		KChatMsgList::iterator chatMsgIt;
		KChatMsgList::iterator tempChatMsgIt;
		for (chatMsgIt = chatMsgList.begin(); chatMsgIt != chatMsgList.end(); ++chatMsgIt)
		{
			CEGUI::String senderName(AnsiToUtf8(chatMsgIt->strSender.c_str()));
			if (senderName.compare(button->getText()) == 0)
			{
				strTest = chatMsgIt->strMsg;
				tempChatMsgIt = chatMsgIt;
				++chatMsgIt;
				chatMsgList.erase(tempChatMsgIt);
				break;
			}
		}
		String senderName = button->getText();
		KUiChatCentre::OpenChatRoomReqP2P(Utf8ToAnsi(senderName), strTest);
	}
}

bool KUiChatRoomListMenu::Button0_MouseClick(const EventArgs & args)
{
	if (m_ButtonList[KUiChatRoomListMenu::Button0] != NULL)
	{
		ClickButton(m_ButtonList[KUiChatRoomListMenu::Button0]);
	}

	Hide();
	return true;
}

bool KUiChatRoomListMenu::Button1_MouseClick(const EventArgs & args)
{
	if (m_ButtonList[KUiChatRoomListMenu::Button1] != NULL)
	{
		ClickButton(m_ButtonList[KUiChatRoomListMenu::Button1]);
	}

	Hide();
	return true;
}

bool KUiChatRoomListMenu::Button2_MouseClick(const EventArgs & args)
{
	if (m_ButtonList[KUiChatRoomListMenu::Button2] != NULL)
	{
		ClickButton(m_ButtonList[KUiChatRoomListMenu::Button2]);
	}

	Hide();
	return true;
}

bool KUiChatRoomListMenu::Button3_MouseClick(const EventArgs & args)
{
	if (m_ButtonList[KUiChatRoomListMenu::Button3] != NULL)
	{
		ClickButton(m_ButtonList[KUiChatRoomListMenu::Button3]);
	}

	Hide();
	return true;
}

bool KUiChatRoomListMenu::Button4_MouseClick(const EventArgs & args)
{
	if (m_ButtonList[KUiChatRoomListMenu::Button4] != NULL)
	{
		ClickButton(m_ButtonList[KUiChatRoomListMenu::Button4]);
	}

	Hide();
	return true;
}
