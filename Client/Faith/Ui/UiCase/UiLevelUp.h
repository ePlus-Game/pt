//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/27/2007 9:50
//      File_base        : UiLevelUp
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 登陆等待
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _UILEVELUP_H_
#define _UILEVELUP_H_

#include "CEGUI.h"
#include "../UiCommon.h"
#include "CoreUseNameDef.h"
#include "layoutinterface.h"
#include "../UiConfigManager.h"

using namespace CEGUI;

class KUiLevelUp : public KUiWndSingleton<KUiLevelUp>
{
public:
	KUiLevelUp( const CEGUI::String& id_name );
	~KUiLevelUp();
public:
	static void Show();
	void Init();

	bool handleMouseEntres( const CEGUI::EventArgs& args );
	bool handleMouseLeaves( const CEGUI::EventArgs& args );
	bool handleMouseClicks( const CEGUI::EventArgs& args );

	void GetPlayerInfo( const char* szName, const int level );

private:
	char				m_szLevelUp[MAX_TEXT_LEN];
	CEGUI::Window*		m_pImage;
};

#endif