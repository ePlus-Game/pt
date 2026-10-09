//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/23/2007 9:50
//      File_base        : UiWaitingMsg
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 登陆等待
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////
#ifndef _UIWAITINGMSG_H_
#define _UIWAITINGMSG_H_

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "CoreUseNameDef.h"
#include "Login/Login.h"
#include <string>

class KUiWaitingMsg : public KUiWndSingleton<KUiWaitingMsg>
{
public:
	KUiWaitingMsg( const CEGUI::String& id_name );
	~KUiWaitingMsg( void );
public:
	virtual void Init( void );
public:
	bool		IsQuiting( void );
	void		SetLoginStatus( 
					LOGIN_BG_INFO_MSG_INDEX eIndex	);
	void		ShowMessage( const char* msg, bool bModel = true );
	void		SetLoginParam( 
					const char* pAccount,
					const KSG_PASSWORD& crPassword,
					const char* pActiveKey );
	bool		BeginLogin( void );
	void		EndLogin( void );
	void		WaitConnect( void );
	void		QuitMsg( void );
	static void LoadThreadLogin(void* pParam);
	bool		IsConnectting( void );
	void		SetShowActiveKey( bool bActivekey )
	{
		m_bNeedActiveKey = bActivekey;
	}

private:
	bool		handleKeyDown(const CEGUI::EventArgs& args);
	bool		onShow( const CEGUI::EventArgs& args );
	bool		onHide( const CEGUI::EventArgs& args );
	bool		onClose( const CEGUI::EventArgs& args );
	void		CombineTextPoint( char* str );
	DWORD		GetRandomTime( void );

private:
	CEGUI::Window*				m_StaticText;
	int							m_loop;
	bool						m_errorMsg;
	std::string					m_username;
	std::string					m_activekey;
	KSG_PASSWORD				m_crPassword;
	HANDLE						m_hThread;
	int							m_timeBegin;
	int							m_timeEnd;
	bool						m_bConnectting;
	bool						m_bConnectSwitch;
	bool						m_bNeedActiveKey;
	bool						m_bModel;
	char						m_AutoConnectText[COMMON_CLIENT_MSG_LEN_256];
	bool						m_bQuiting;
};


#endif



