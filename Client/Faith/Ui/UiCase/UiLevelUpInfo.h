//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/27/2007 9:50
//      File_base        : UiLevelUpInfo
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 登陆等待
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _UILEVELUPINFO_H_
#define _UILEVELUPINFO_H_

#include "CEGUI.h"
#include "../UiCommon.h"
#include "CoreUseNameDef.h"
#include "GameDataDef.h"
#include "TLStatic.h"
#include "../UiConfigManager.h"

using namespace CEGUI;

class KUiLevelUpInfo : public KUiWndSingleton<KUiLevelUpInfo>
{
public:
	KUiLevelUpInfo( const CEGUI::String& id_name );
	~KUiLevelUpInfo();
public:
	static void Show();
	void Init();

	bool handleClose( const CEGUI::EventArgs& args );

	// 得到升级的信息
	void GetLevelUpInfo(const LevelUpAdd* plevelupAdd);

private:
	// 拼字太麻烦
	void PrintText( const char* color, const char* font, const char* sourceText, char* destText );

private:
	TLStaticText*	d_AttributeText;
	TLStaticText*	d_tipText;

	KUiCfgLoader::LevelUpData m_LevelUpCfg;
	
};

#endif