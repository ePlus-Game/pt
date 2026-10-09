 //////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 09/04/2006 15:20
//      File_base        : KUiTopMessage
//      File_ext         : h
//      Author           : 谢鉷
//      Description      : 顶部滚动公告
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef KUiTopMessage_H
#define KUiTopMessage_H

#include "CEGUI.h"
#include "../uicommon.h"
#include "GameDataDef.h"

using namespace CEGUI;

class KUiTopMessage : public KUiWndSingleton<KUiTopMessage>
{
public:

	void Init();
	void setStyle(CommonStyle& style);
	void setStyle(CommonStyle2& style);
	void setText(String message);
	void resetText();
    KUiTopMessage(const String& name);
    ~KUiTopMessage();
	
private:
	String	d_defaultText;
	Font*	d_defaultFont;
	colour	d_defaultColor;
	int		d_defaultSpeed;
	int		d_loopTime;

	bool updateSelf(const EventArgs& e);
};


#endif