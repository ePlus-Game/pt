 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/25/2006
//      File_base        : KUiSplitItemBox
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 拆分物品对话框
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiSplitItemBox_H
#define KUiSplitItemBox_H

#include "CEGUI.h"
#include "../uicommon.h"

#include "CoreShell.h"
#include "TLGameObject.h"
#include "TLEditbox.h"
#include "TLButton.h"
#include "TLMiniHorzScrollbar.h"
#include "UiCommonGrid.h"

#define		UI_SPLIT_ITEM_BOX_PATH	"uisettings/layouts/SplitItemBox.ls"

using namespace CEGUI;

class KUiSplitItemBox
{
	TLStaticImage*			_thisWindow;

	//左右显示拆分个数的图标
	TLGameObject*			_sour;
	TLGameObject*			_dest;
	KUiCommonGrid			_sourGrid;
	KUiCommonGrid			_destGrid;
	KObjAtContRegion		_sourRegion;
	KObjAtContRegion		_destRegion;
	
	//调节拆分个数的控件
	TLMiniHorzScrollbar*	_slider;
	TLEditbox*				_count;
	TLButton*				_increase;
	TLButton*				_decrease;

	//确定取消按钮
	TLButton*				_ok;
	TLButton*				_cancel;
	int						_total;

	void	load();
	void	show();
protected:
	bool	onWindowOpen	(const EventArgs& args);
	bool	onWindowClose	(const EventArgs& args);
	bool	onScroll		(const EventArgs& arg);
	bool	onOk			(const EventArgs& arg);
	bool	onCancel		(const EventArgs& arg);
public:
	void	open(KObjAtContRegion& destPos, TLGameObject::GameObject& destObjInfo);
    
	KUiSplitItemBox();
    ~KUiSplitItemBox();
	
	static KUiSplitItemBox& getSingleton();
};


#endif