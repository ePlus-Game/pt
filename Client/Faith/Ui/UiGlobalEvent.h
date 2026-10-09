//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 08/13/2007 11:15
//      File_base        : UiGlobalEvent
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 全局事件
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _UIGLOBALEVENT_H_
#define _UIGLOBALEVENT_H_

#include "CEGUI.h"
#include "KIniFile.h"
#include "CoreUseNameDef.h"

using namespace CEGUI;

class KUIGlobalEvent : public Singleton<KUIGlobalEvent>
{
public:
	KUIGlobalEvent();
	~KUIGlobalEvent();

	bool globalShown(const EventArgs& e);

	bool globalMouseClick(const EventArgs& e);

	bool globalMouseEnter(const EventArgs& e);

	void setLoginInfo(const char *fileName, const char *roleName, bool firstLogin);

	void writeToRoleFile();

private:
	bool globalEvent(const EventArgs& e, const String& section);

private:
	KIniFile		d_UiConfigFile;		//UI使用配置文件
	KIniFile		d_RoleFile;			//角色配置文件

	bool			d_bFirstOpenHelp;	//是否进行帮助
	char			d_fileName[COMMON_CLIENT_MSG_LEN_64];
	char			d_roleName[COMMON_CLIENT_MSG_LEN_64];

};


#endif 
