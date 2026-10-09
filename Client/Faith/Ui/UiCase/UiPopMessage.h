 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/04/2006 15:20
//      File_base        : KUiPopMessage
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 顶部滚动公告
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiPopMessage_H
#define KUiPopMessage_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "chatdatadef.h"
#include "CoreShell.h"
#include <map>
#include <list>
#include <string>

using namespace CEGUI;

class KUiPopMessage
{
	StaticText* d_newWnd;
protected:
	bool onClose(const CEGUI::EventArgs& arg);
public:
	void show(String message, Point pos);
    KUiPopMessage(String wndType);
    ~KUiPopMessage();
};


#endif