 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 1/16/2007
//      File_base        : KUiCastBar
//      File_ext         : h
//      Author           : Ð»ãp
//      Description      : Ò÷³ªÌõ
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiCastBar_H
#define KUiCastBar_H

#include "CEGUI.h"
#include "../uicommon.h"

#include "TLStatic.h"

using namespace CEGUI;

class KUiCastBar : public KUiWndSingleton<KUiCastBar>
{
#define BoardLeftWidth 10
#define BoardTopWidth 2
#define FidoutTime    1000

	float			d_curPercent;
	float			d_leftTime;
	float			d_progressBarMaxLen;
	float			d_fidoutLeftTime;

	TLStaticImage*	d_progressbarHead;
	TLStaticImage*	d_progressbarBody;
	TLStaticImage*	d_progressbarBody_Normal;
	TLStaticImage*	d_progressbarBody_Complete;
	TLStaticImage*	d_progressbarBody_Cancel;
	TLStaticImage*	d_progressbarTail;
	TLStaticText*	d_time;
	TLStaticText*	d_msg;
	TLStaticImage*	d_progressbarFrame_Normal;
	TLStaticImage*	d_progressbarFrame_HighLight;

protected:
	void refreshMsg(int msgCode					);
	bool onHide(const CEGUI::EventArgs& e);
public:

	enum CastBarCode
	{
		CB_CODE_CANCEL = 0,
	};
    KUiCastBar( const CEGUI::String& id_name	);
    ~KUiCastBar(								);
    void getChild(								);
	bool onNewFrame(const CEGUI::EventArgs& e	);
	void cast(int curPercent, float leftTime, int msgCode = delayed_action_msg_none);
	void delay(int leftTime, int msgCode = delayed_action_msg_none);
	void cancel(int msgCode = delayed_action_msg_none);
	void complete(int msgCode = delayed_action_msg_none);
	void	Init();
};


#endif