//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/14/2006
//      File_base        : KUiComMsgBox
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 通用对话框
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiDragItem_H
#define KUiDragItem_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "CoreShell.h"
#include <map>
#include <list>
#include <string>

#include "TLGameObject.h"

using namespace CEGUI;

class KUiDragItem : public KUiWndSingleton<KUiDragItem>
{
protected:
public:
    KUiDragItem( const CEGUI::String& id_name	);
    ~KUiDragItem(								);
	TLGameObject* getObj();
	void initItem();
	bool onMouseMove(const CEGUI::EventArgs& e);
	bool onShow(const CEGUI::EventArgs& e);
	bool onHide(const CEGUI::EventArgs& e);
};


#endif