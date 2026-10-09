 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/06/2006
//      File_base        : KUiDuraAlert
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 耐久不够提示界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiDuraAlert_H
#define KUiDuraAlert_H

#define UI_DURA_ALERT_WINDOW_PATH_1024 "uisettings/layouts1024/DuraAlert.ls"
#define UI_DURA_ALERT_WINDOW_PATH "uisettings/layouts/DuraAlert.ls"

#include "CEGUI.h"
#include "TLStatic.h"
#include "GameDataDef.h"

using namespace CEGUI;

class KUiDuraAlert
{
	TLStaticImage*	_equipRed[itempart_num];
	TLStaticImage*	_equipYellow[itempart_num];
	TLStaticImage*	_wholeBody;
	void load();

	TLStaticImage*	_thisWindow;

	EQUIP_DUR_STATE	_state[itempart_num];
private:
	bool slash(const EventArgs& args);
	bool onWindowShow(const EventArgs& args);
	bool onWindowHide(const EventArgs& args);

	bool onMouseHover(const EventArgs& args);

	void showTip();
public:
	void updateState(EQUIP_DUR_STATE uEquipStatus[]);

    KUiDuraAlert();
    ~KUiDuraAlert();
	static KUiDuraAlert& getSingleton();
	void hide();
};


#endif