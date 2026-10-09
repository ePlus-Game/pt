 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2006-10-26
//      File_base        : KUiErrorMessageBox
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 游戏世界中的出错信息，例如距离过远等
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiErrorMessageBox_H
#define KUiErrorMessageBox_H

#define UI_ERROR_MSG_MAX_MESSAGE_COUNT 10
#define UI_ERROR_MSG_WINDOW_PATH_1024 "uisettings/layouts1024/ErrorMessage.ls"
#define UI_ERROR_MSG_WINDOW_PATH "uisettings/layouts/ErrorMessage.ls"

#include "TLStatic.h"

using namespace CEGUI;

class KUiErrorMessageBox
{
	float		d_autoCloseTime;
	int			_messageCount;
	TLStaticText* d_messages[UI_ERROR_MSG_MAX_MESSAGE_COUNT];
	
	TLStaticImage*	_thisWindow;
private:
	
	void loadUi();

protected:
	bool onHide(const EventArgs& e);
	bool onShow(const EventArgs& e);
	bool onSheetChanged(const EventArgs& e);
public:
	void show();
	void hide();
	void AddMessage(String message);
	void toSystemMessage(String message);

    KUiErrorMessageBox();
    ~KUiErrorMessageBox();

	static KUiErrorMessageBox& GetSingleton();
	
};


#endif