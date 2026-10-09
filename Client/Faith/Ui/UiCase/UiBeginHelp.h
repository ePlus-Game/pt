//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 05/08/2007 19:08
//      File_base        : UiBeginHelp
//      File_ext         : h
//      Author           : likun
//      Description      : 玩家第一次登陆后出现的UI帮助界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef	KUIBEGINHELP_H 
#define KUIBEGINHELP_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include "CoreUseNameDef.h"
class KUiBeginHelp : public KUiWndSingleton<KUiBeginHelp>
{
public:
	KUiBeginHelp( const CEGUI::String& id_name );
	~KUiBeginHelp();

	static void		Show( void );
	void		    Init( void );

	void			setbRoleFirstLogin( bool bFirstLogin, const char *fileName, const char *roleName );
	bool			getbRoleFirstLogin( void ){ return d_bFirstLogin; }

	void			RepeatShow( void );
	void			setAlwayTop( bool bTop);

private:
	bool			handleClick( const CEGUI::EventArgs& args );
	
private:
	bool			d_bFirstLogin;			//登陆的角色索引
	char			d_fileName[COMMON_CLIENT_MSG_LEN_128];
	char			d_roleName[COMMON_CLIENT_MSG_LEN_128];
};
#endif