//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiGlobalEvent
//      File_ext         : cpp
//      Author           : Lucien (LIU Siliang)
//      Description      : 全局事件
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#include "KWin32.h"
#include "KWin32Wnd.h"
#include "UiGlobalEvent.h"
#include "CoreUseNameDef.h"
#include "layoutinterface.h"
#include ".\UiCase\UiSearchHelpWnd.h"
#include ".\UiCase\UiGameSetting.h"

template<> 
KUIGlobalEvent* Singleton<KUIGlobalEvent>::ms_Singleton	= NULL;

KUIGlobalEvent::KUIGlobalEvent()
: d_bFirstOpenHelp(true)
{
	ZeroMemory(d_fileName, COMMON_CLIENT_MSG_LEN_64);
	ZeroMemory(d_roleName, COMMON_CLIENT_MSG_LEN_64);
	
	d_UiConfigFile.Load("uisettings/uicfg.ini");

	GlobalEventSet::getSingleton().subscribeEvent(Window::EventShown, 
		Event::Subscriber(&KUIGlobalEvent::globalShown, this));

	GlobalEventSet::getSingleton().subscribeEvent(Window::EventMouseClick, 
		Event::Subscriber(&KUIGlobalEvent::globalMouseClick, this));

	GlobalEventSet::getSingleton().subscribeEvent(Window::EventMouseEnters, 
		Event::Subscriber(&KUIGlobalEvent::globalMouseEnter, this));
}

KUIGlobalEvent::~KUIGlobalEvent()
{
}

bool KUIGlobalEvent::globalShown(const EventArgs& e)
{
	const WindowEventArgs& key = static_cast<const WindowEventArgs&>(e);
	if ( key.window )
	{
		if ( !(key.window->getWindowShownHelp()) )
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	if ( globalEvent(e, String("UIShow")) )
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool KUIGlobalEvent::globalMouseEnter(const EventArgs& e)
{
	const WindowEventArgs& key = static_cast<const WindowEventArgs&>(e);
	if ( key.window )
	{
		if ( !(key.window->getMouseEntersHelp()) )
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	if ( globalEvent(e, String("UIEnter")) )
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool KUIGlobalEvent::globalMouseClick(const EventArgs& e)
{
	String section;
	const MouseEventArgs& mouse = static_cast<const MouseEventArgs&>(e);
	const WindowEventArgs& key = static_cast<const WindowEventArgs&>(e);
	if ( mouse.button == RightButton )
	{
		if ( key.window )
		{
			if ( !(key.window->getRMouseClickHelp()) )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
		section = "UIRClick";
	}
	else
	{
		if ( key.window )
		{
			if ( !(key.window->getLMouseClickHelp()) )
			{
				return false;
			}
		}
		else
		{
			return false;
		}
		section = "UILClick";
	}

	if ( globalEvent(e, section) )
		return true;
	else 
		return false;

}

void KUIGlobalEvent::setLoginInfo(const char *fileName, const char *roleName, bool firstLogin)
{
	strcpy( d_fileName, fileName );
	strcpy( d_roleName, roleName );

	if ( !d_fileName )
	{
		return;
	}
	if ( d_RoleFile.Load(d_fileName) == FALSE )
	{
		return;
	}
	if ( firstLogin )
	{
		writeToRoleFile();
	}
}

void KUIGlobalEvent::writeToRoleFile()
{
	char keyname[COMMON_CLIENT_MSG_LEN_64];
	BOOL ret = d_UiConfigFile.GetNextKey("EventHelp", "Begin", keyname);
	while ( ret == TRUE )
	{
		d_RoleFile.WriteInteger(d_roleName, keyname, 1);
		ret = d_UiConfigFile.GetNextKey("EventHelp", keyname, keyname);
	}

	d_RoleFile.Save(d_fileName);
}

bool KUIGlobalEvent::globalEvent(const EventArgs& e, const String& section)
{
	if ( !d_bFirstOpenHelp || strcmp(d_roleName, "") == 0 || 
		 (KUiGameSetting::GetSingletonPtr()->GetShowSearchHelp() == false &&
		  KUiGameSetting::GetSingletonPtr()->GetShowSimpleHelp() == false )
		  || KUiGameSetting::GetSingletonPtr()->GetShowFirstHelp() == false)
	{
		return false;
	}

	const WindowEventArgs& key = static_cast<const WindowEventArgs&>(e);

	// 查找窗口记录
	if ( !key.window )
	{
		return false;
	}
	String	windowName = key.window->getName();
	String	name = windowName.erase( 0, (windowName.find("/")+1) );
	BOOL	ret = FALSE;
	String	sectionkey = section + name;

	d_RoleFile.GetInteger(d_roleName, Utf8ToAnsi(sectionkey), 0, &ret);
	
	if ( ret == 0 )
	{
		return false;
	}
	
	// 如果存在记录并且记录为真时显示提示
	char Helptip[LAYOUT_TEXT_MAX_LEN];
	ZeroMemory(Helptip, LAYOUT_TEXT_MAX_LEN);

	d_UiConfigFile.GetString("EventHelp", Utf8ToAnsi(sectionkey), "", Helptip, LAYOUT_TEXT_MAX_LEN);

	KUiSearchHelpWnd::GetSingleton().ShowSimpleHelp(Helptip);

	// 将记录至为FALSE
	d_RoleFile.WriteInteger(d_roleName, Utf8ToAnsi(sectionkey), 0);
	d_RoleFile.Save(d_fileName);

	return true;
}

