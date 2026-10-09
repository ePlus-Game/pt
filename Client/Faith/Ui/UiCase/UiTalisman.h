 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 1/19/2007
//      File_base        : Talisman
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 法宝界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiTalisman_H
#define KUiTalisman_H

#include "CEGUI.h"
#include "../uicommon.h"

#include "TLStatic.h"
#include "TLButton.h"
#include "TLEditbox.h"
#include "TLGameObject.h"
#include "UiCommonGrid.h"
#include "GameDataDef.h"

using namespace CEGUI;

#define CtrlNameLen 128
#define TM_MaxHoleCountPerRow 3
#define TM_HoleRowCount 5
#define TM_LAYER_STATE_COUNT 3
#define UI_TALISMAN_WINDOW_PATH_1024 "uisettings/layouts1024/Talisman.ls"
#define UI_TALISMAN_WINDOW_PATH "uisettings/layouts/Talisman.ls"
class KUiTalisman
{
private:
	enum StringCode
	{
		TM_LEVELUP_COMFIRM = 1,
		TM_LEVELUP_BTN_OK,
		TM_LEVELUP_BTN_CANCEL,
		TM_PLACE_TALISMAN_FAILURE,
		TM_PLACE_INSIDE_BALL_FAILURE,
		TM_ALREADY_EXISIT_INSIDE_BALL,
		TM_PLACE_INSIDE_BALL_COMFIRM,
	};

	TLStaticImage*	_thisWindow;
	
	//法宝相关
	int				d_talismanId;
	TLStaticText*	d_tm_nameText;
	TLStaticText*	d_tm_levelText;
	TLButton*		d_tm_uplevelButton;
	TLStaticText*	d_tm_curyunhunText;
	TLStaticText*	d_tm_maxyunhunText;
	TLEditbox*		d_tm_descriptionText;

	//内丹、孔相关
	TLGameObject*	d_tm_iconImage;
	KUiCommonGrid	d_tm_iconImageGrid;

	TLButton*		d_tm_holeImage[TM_HoleRowCount][TM_MaxHoleCountPerRow];
	TLGameObject*	d_tm_ballImage[TM_HoleRowCount][TM_MaxHoleCountPerRow];
	KUiCommonGrid	d_tm_ballImageGrid[TM_HoleRowCount][TM_MaxHoleCountPerRow];
	//>0表示内丹对应的BUFFid、0表示没有内丹但是有孔，<0表示没有孔
	int				d_tm_holeState[TM_HoleRowCount][TM_MaxHoleCountPerRow];
	
	int				d_tm_canUplevel;
	int             d_tm_canTrans;
	
	//层
	TLStaticImage*	d_layer[TM_HoleRowCount][TM_LAYER_STATE_COUNT];

	//镶嵌
	TM_HOLE_POS		d_curClickHole;
	int				d_curInsideBallId;
	
	TLButton*		d_tm_closeBtn;

	void clear();
	void getChild();
	void getTalismanInfo();
protected:
	bool onClickHole(const EventArgs& e	);
	bool onClickUpgrade(const EventArgs& e	);
	bool onClickIcon(const EventArgs& e	);
	bool onClickClose(const EventArgs& e	);
public:

	void onTalismanPropChange();
	void onTalismanPotentialChange(int curProtential);
	int	getEditTalismanId(){	return d_talismanId;	};
	void setEditTalismanId(int talismanId){	d_talismanId = talismanId;	};
	void insertBall();
	static bool isTalisman(int itemId);
	static bool isInsideBall(int itemId);

	bool isVisible();
	void show();
	void hide();
	void toggle();

	void load();
	KUiTalisman();
	~KUiTalisman();
	static KUiTalisman& getSingleton();
};


#endif