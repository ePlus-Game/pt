//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/09/2006 16:49
//      File_base        : UiLoginBg
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UILOGINBG_H
#define UILOGINBG_H

#include "CEGUI.h"
#include "../uicommon.h"

enum tagLoginBgStyle
{
	UILOGBG_LOGIN = 0,
	UILOGBG_ROLE,
};

class KUiLoginBackGround : public KUiWndSingleton<KUiLoginBackGround>
{
public:
    KUiLoginBackGround( const CEGUI::String& id_name	);
    ~KUiLoginBackGround(								);

public:
	static void					Show			( void								);
	void						Init			( void								);
	void						SetServerInfo	( CEGUI::String serverInfo );
	void						ShowServerInfo	();
	void						HideServerInfo	();

private:
	bool						handleWeb		( const CEGUI::EventArgs& args		);
	bool						handleRegist	( const CEGUI::EventArgs& args		);
	bool						handlePay		( const CEGUI::EventArgs& args		);
    bool						handleKeyDown	( const CEGUI::EventArgs& args		);
	
private:
    static const unsigned int	WebButtonID;
	static const unsigned int	RegistButtonID;
    static const unsigned int	PayButtonID;
	TLStaticText*				d_serverInfo;
	TLStaticImage*				d_serverInfoBK;
};

#endif