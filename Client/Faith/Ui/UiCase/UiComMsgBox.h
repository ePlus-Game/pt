 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/06/2006
//      File_base        : KUiComMsgBox
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 通用对话框
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiComMsgBox_H
#define KUiComMsgBox_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "CoreShell.h"
#include <map>
#include <list>
#include <string>
#include "TLButton.h"
#include "TLEditbox.h"

using namespace CEGUI;

class KUiComMsgBox : public KUiWndSingleton<KUiComMsgBox>
{
	CEGUI::Window*		d_msgText;
	TLEditbox*			d_editText;
	TLStaticText*		d_msgLayoutText;
	TLButton*			d_fristBtn;
	TLButton*			d_secondBtn;
	TLButton*			d_thirdBtn;
	StaticImage*		d_icon;

	CEGUI::Window*		d_msgEditBg;
	//CEGUI::Window*		d_msgInput;

	String d_style;
	bool   d_bModalStatus;
	//llikun ComMsgBox的位置
	Point	d_position;
	char	d_layoutTextHead[COMMON_CLIENT_MSG_LEN_64];

	void getChild();
	void (*d_callback1)();
	void (*d_callback2)();
	void (*d_callback3)();
	
public:
	void init();
	enum Style
	{
		Normal,
		Question,
		Plaint,
		Warning,
		Mistake,
		TargetItem,
		UseItem,
		DongJie,
		JinYan,
	};
	void setMsg(String msg, bool bInput = false);
	void setLayoutMsg(char* msg, bool hasHead = false);
	void setStyle(Style newStyle);
	void setBtnName(String fristbtnName = "", String secondBtnName = "", String thirdName = "");
	void setTopMost();
	static void Show();
	static void Hide();
	void Init();

    KUiComMsgBox( const CEGUI::String& id_name	);
    ~KUiComMsgBox(								);
	static void ConfirmUseYiBuItem( void );
	static void CannelUseYiBuItem( void );

	bool onFristBtnDown(const CEGUI::EventArgs& e);
	bool onSecondBtnDown(const CEGUI::EventArgs& e);
	bool onThirdBtnDown(const CEGUI::EventArgs& e);
	void setFristBtnCallback(void (*func)());
	void setSecondBtnCallback(void (*func)());
	void setThirdBtnCallback(void (*func)());
	//设置对话框为模态对话框状态
	void setModalStatus( const bool bModal );
	//设置对话框位置
	void setComMsgPosition();
	const String& getEditText( void );
};


#endif