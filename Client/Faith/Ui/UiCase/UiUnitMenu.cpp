#include "UiUnitMenu.h"
#include "Coreshell.h"
#include "chatdatadef.h"
#include "uichatwindow.h"
#include "../UiSheetMgr.h"

extern iCoreShell*		g_pCoreShell;

KUiUnitMenu::KUiUnitMenu()
{
	loadUi();
}

KUiUnitMenu::~KUiUnitMenu()
{

}

KUiUnitMenu& KUiUnitMenu::getSingleton()
{
	static KUiUnitMenu singleton;
	return singleton;
}

void KUiUnitMenu::loadUi()
{
#ifndef _DEBUG
	try
	{
#endif
		_thisWindow = (TLStaticImage*)WindowManager::getSingleton().loadWindowLayout(UI_UNIT_MENU_WINDOW_PATH);
#ifndef _DEBUG
	}
	catch (...)
	{
		_thisWindow = NULL;
		return;
	}
#endif

	KUiSheetMgr::getSinglton().find(UI_DEFAULT_GUISHEET_ROOT)->addChildWindow(_thisWindow);
	
	_button[UMI_CAPTION]	= (TLButton*)_thisWindow->getChild("TaharezLook/UnitMenu/Captain");
	_button[UMI_KICK]		= (TLButton*)_thisWindow->getChild("TaharezLook/UnitMenu/Kick");
	_button[UMI_EXP]		= (TLButton*)_thisWindow->getChild("TaharezLook/UnitMenu/Exp");
	_button[UMI_ADDFRIEND]	= (TLButton*)_thisWindow->getChild("TaharezLook/UnitMenu/AddFriend");
	_button[UMI_WISPER]		= (TLButton*)_thisWindow->getChild("TaharezLook/UnitMenu/Wisper");
	_button[UMI_LEAVE]		= (TLButton*)_thisWindow->getChild("TaharezLook/UnitMenu/Leave");
	
	_button[UMI_WISPER]->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiUnitMenu::wisper, this));
	_button[UMI_CAPTION]->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiUnitMenu::captain, this));
	_button[UMI_KICK]->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiUnitMenu::kick, this));
	_button[UMI_LEAVE]->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiUnitMenu::leave, this));
	_button[UMI_ADDFRIEND]->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiUnitMenu::addFriend, this));
	_button[UMI_EXP]->subscribeEvent(Window::EventMouseClick, Event::Subscriber(&KUiUnitMenu::expSetting, this));

	_thisWindow->setZLevel(Window::Top);
}

void KUiUnitMenu::show(UMNpcInfo npcInfo, int flag, Point windowPos)
{
	if(!_thisWindow)
	{
		return;
	}

	_npcInfo = npcInfo;

	for(int i = 0; i < UMI_COUNT; ++i)
	{
		_button[i]->hide();
	}

	if((flag & UI_UNIT_MENU_WISPER_FLAG) == UI_UNIT_MENU_WISPER_FLAG)
	{
		_button[UMI_WISPER]->show();
	}

	if((flag & UI_UNIT_MENU_CAPTION_FLAG) == UI_UNIT_MENU_CAPTION_FLAG)
	{
		_button[UMI_CAPTION]->show();
	}

	if((flag & UI_UNIT_MENU_KICK_FLAG) == UI_UNIT_MENU_KICK_FLAG)
	{
		_button[UMI_KICK]->show();
	}

	if((flag & UI_UNIT_MENU_LEAVE_FLAG) == UI_UNIT_MENU_LEAVE_FLAG)
	{
		_button[UMI_LEAVE]->show();
	}

	if((flag & UI_UNIT_MENU_ADDFRIEND_FLAG) == UI_UNIT_MENU_ADDFRIEND_FLAG)
	{
		_button[UMI_ADDFRIEND]->show();
	}

	if((flag & UI_UNIT_MENU_EXP_FLAG) == UI_UNIT_MENU_EXP_FLAG)
	{
		_button[UMI_EXP]->show();
	}

	Point pos;
	pos.d_x = _thisWindow->getLeftFrameWidth();
	pos.d_y = _thisWindow->getTopFrameHeight();

	for(int j = 0; j < UMI_COUNT; ++j)
	{
		if(_button[j]->isVisible(true))
		{
			_button[j]->setPosition(Absolute, pos);
			pos.d_y += _button[j]->getAbsoluteHeight();
		}
	}

	pos.d_y += _thisWindow->getBottomFrameHeight();
	_thisWindow->setHeight(Absolute, pos.d_y);

	_thisWindow->show();
	_thisWindow->setPosition(Absolute, windowPos);
}

void KUiUnitMenu::hide()
{
	if(!_thisWindow)
	{
		return;
	}

	_thisWindow->hide();
}

bool KUiUnitMenu::captain(const EventArgs& args)
{
	KUiPlayerItem playerItem;
	strcpy(playerItem.Name, _npcInfo.NpcName.c_str());
	playerItem.uId = _npcInfo.NpcId;
	g_pCoreShell->TeamOperation( TEAM_OI_APPOINT, (unsigned int)&playerItem, NULL );

	_thisWindow->hide();
	return true;
}

bool KUiUnitMenu::kick(const EventArgs& args)
{
	KUiPlayerItem playerItem;
	strcpy(playerItem.Name, _npcInfo.NpcName.c_str());
	playerItem.uId = _npcInfo.NpcId;
	g_pCoreShell->TeamOperation( TEAM_OI_KICK, (unsigned int)&playerItem, NULL );

	_thisWindow->hide();
	return true;
}

bool KUiUnitMenu::leave(const EventArgs& args)
{
	g_pCoreShell->TeamOperation( TEAM_OI_LEAVE, NULL, NULL );
	_thisWindow->hide();
	return true;
}

bool KUiUnitMenu::addFriend(const EventArgs& args)
{
	char name[COMMON_CLIENT_MSG_LEN_32];
	strcpy(name, _npcInfo.NpcName.c_str());
	g_pCoreShell->OperationRequest(GOI_CHAT_FRIEND_ADD, (unsigned int)name, CHAT::GROUPID_NONE);
	return true;
}

bool KUiUnitMenu::expSetting(const EventArgs& args)
{
	_thisWindow->hide();
	return true;
}

bool KUiUnitMenu::wisper(const EventArgs& args)
{
	char name[COMMON_CLIENT_MSG_LEN_32];
	strcpy(name, _npcInfo.NpcName.c_str());

	KUiChatInputWnd::GetSingleton().clearText();
	
	KUiChatInputWnd::GetSingleton().write("/");
	
	KUiChatInputWnd::GetSingleton().write(name);
	KUiChatInputWnd::GetSingleton().write(" ");
	
	KUiChatInputWnd::GetSingleton().show();

	_thisWindow->hide();
	return true;
}