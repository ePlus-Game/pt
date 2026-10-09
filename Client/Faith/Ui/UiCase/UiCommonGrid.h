 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2/26/2007
//      File_base        : UiCommonGrid
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 把一些格子的共有属性
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiCommonGrid_H
#define KUiCommonGrid_H

#include "CEGUI.h"

#include "TLGameObject.h"

using namespace CEGUI;

class KUiCommonGrid
{
	TLGameObject* d_goCtrl;

	bool		d_showCompare;
	void showTip();
protected:
	bool onMouseEnters(const CEGUI::EventArgs& e);
	bool onMouseLeaves(const CEGUI::EventArgs& e);
	bool onMouseMove(const CEGUI::EventArgs& e);
	bool onObjectChanged(const CEGUI::EventArgs& e);

public:
	KUiCommonGrid();
	~KUiCommonGrid();
	void showCompare(bool show){	d_showCompare = show;	}
	void setCtrl(TLGameObject* goCtrl){	d_goCtrl = goCtrl;	};
	void addTip();
};

#endif