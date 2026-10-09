//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/15/2006 13:05
//      File_base        : UiLogin
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 文件功能描述
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UILOGIN_H
#define UILOGIN_H

#include "../UiCommon.h"
#include "CEGUI.h"
#include <string>

enum LoginFocus
{
	USERNAME,
	PASSWORD,
};

class KUiLogin : public KUiWndSingleton<KUiLogin>
{
public:
	KUiLogin(  const CEGUI::String& id_name );
	~KUiLogin(								);
public:
	static void					Show( bool bShowActiveKey = false );
	static void					Hide();
	void						Init();	
	void						AutoAccountValidate( const char* szAccName, const char* szPassword );
	void						SetKeyboardEnable( bool bEnable );
	void						SetPassWord( const char * text );
private:
	bool						handleMemory( const CEGUI::EventArgs& args		);
    bool						handleLogin	 ( const CEGUI::EventArgs& args		);
	bool						handleBack	 ( const CEGUI::EventArgs& args		);	
	bool						handleKeyDown( const CEGUI::EventArgs& args		);
	bool						thisWnd_KeyDown( const CEGUI::EventArgs& args		);
	bool						handleMiniKeyDown( const CEGUI::EventArgs& args );
	bool						btnFanhui_MouseClick( const CEGUI::EventArgs& args );
	bool						handleMouseBnDown( const CEGUI::EventArgs& args );
//这里的代码使用数组应该会更好
	bool						handleKey1Down( const CEGUI::EventArgs& args	);
	bool						handleKey2Down( const CEGUI::EventArgs& args	);
	bool						handleKey3Down( const CEGUI::EventArgs& args	);
	bool						handleKey4Down( const CEGUI::EventArgs& args	);
	bool						handleKey5Down( const CEGUI::EventArgs& args	);
	bool						handleKey6Down( const CEGUI::EventArgs& args	);
	bool						handleKey7Down( const CEGUI::EventArgs& args	);
	bool						handleKey8Down( const CEGUI::EventArgs& args	);
	bool						handleKey9Down( const CEGUI::EventArgs& args	);
	bool						handleKey0Down( const CEGUI::EventArgs& args	);
	bool						handleKeyaDown( const CEGUI::EventArgs& args	);
	bool						handleKeybDown( const CEGUI::EventArgs& args	);
	bool						handleKeycDown( const CEGUI::EventArgs& args	);
	bool						handleKeydDown( const CEGUI::EventArgs& args	);
	bool						handleKeyeDown( const CEGUI::EventArgs& args	);
	bool						handleKeyfDown( const CEGUI::EventArgs& args	);
	bool						handleKeygDown( const CEGUI::EventArgs& args	);
	bool						handleKeyhDown( const CEGUI::EventArgs& args	);
	bool						handleKeyiDown( const CEGUI::EventArgs& args	);
	bool						handleKeyjDown( const CEGUI::EventArgs& args	);
	bool						handleKeykDown( const CEGUI::EventArgs& args	);
	bool						handleKeylDown( const CEGUI::EventArgs& args	);
	bool						handleKeymDown( const CEGUI::EventArgs& args	);
	bool						handleKeynDown( const CEGUI::EventArgs& args	);
	bool						handleKeyoDown( const CEGUI::EventArgs& args	);
	bool						handleKeypDown( const CEGUI::EventArgs& args	);
	bool						handleKeyqDown( const CEGUI::EventArgs& args	);
	bool						handleKeyrDown( const CEGUI::EventArgs& args	);
	bool						handleKeysDown( const CEGUI::EventArgs& args	);
	bool						handleKeytDown( const CEGUI::EventArgs& args	);
	bool						handleKeyuDown( const CEGUI::EventArgs& args	);
	bool						handleKeyvDown( const CEGUI::EventArgs& args	);
	bool						handleKeywDown( const CEGUI::EventArgs& args	);
	bool						handleKeyxDown( const CEGUI::EventArgs& args	);
	bool						handleKeyyDown( const CEGUI::EventArgs& args	);
	bool						handleKeyzDown( const CEGUI::EventArgs& args	);
	
	bool						handleKeyTopDotDown( const CEGUI::EventArgs& args	);
	bool						handleKeyDotDown( const CEGUI::EventArgs& args	);
	bool						handleKeyPlusDown( const CEGUI::EventArgs& args	);
	bool						handleKeyEquelDown( const CEGUI::EventArgs& args	);
	bool						handleKeyLbracketDown( const CEGUI::EventArgs& args	);
	bool						handleKeyRbracketDown( const CEGUI::EventArgs& args	);
	bool						handleKeyLslashDown( const CEGUI::EventArgs& args	);
	bool						handleKeyRslashDown( const CEGUI::EventArgs& args	);
	bool						handleKeyQuotesDown( const CEGUI::EventArgs& args	);
	bool						handleKeyCommaDown( const CEGUI::EventArgs& args	);
	bool						handleKeyColonDown( const CEGUI::EventArgs& args	);
	bool						handleKeyEnterDown( const CEGUI::EventArgs& args	);
	bool						handleKeyShiftDown( const CEGUI::EventArgs& args	);
	bool						handleKeyCancelDown( const CEGUI::EventArgs& args	);
	bool						handleKeyBlankDown( const CEGUI::EventArgs& args	);

	bool						HandleIEClick(const CEGUI::EventArgs& args);
	bool						HandleViewClick(const CEGUI::EventArgs& args);

//	bool						handleShow( const CEGUI::EventArgs& args	);
//	bool						handleHide( const CEGUI::EventArgs& args	);
//likun define
	bool						InputCharHelp( const char *pChar );
	bool						ShiftInput(const char *pNoShift, const char *pShift, bool bShiftDown);
	std::string					GetKeyName( const char *keyName );
						
	void						Login(void);
	bool						DoReturn();
private:
	static const unsigned int	LoginButtonID;
	static const unsigned int	ExitButtonID;
//likun define
	bool						d_bMiniKeyBoard;
	bool						d_bShiftDown;
	std::string					d_strUse;
	std::string					d_strPass;
	LoginFocus					d_InputFocus;
	CEGUI::Editbox 				*d_pUserName;
	CEGUI::Editbox				*d_pPassWord;
	HANDLE						m_hThread;
	DWORD						m_ThreadId;
};

#endif