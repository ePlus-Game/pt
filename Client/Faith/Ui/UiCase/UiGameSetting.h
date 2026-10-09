//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/21/2007 10::46
//      File_base        : UiGameSetting
//      File_ext         : h
//      Author           : likun
//      Description      : 游戏设置界面
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UIGAMESETTING_H
#define UIGAMESETTING_H

#include "..\UiCommon.h"
#include "CEGUI.h"
#include "TLRadioButton.h"
#include "TLMiniHorzScrollbar.h"
#include "TLButton.h"
#include "CoreShell.h"

//render add //取消按钮恢复设置
#define SHOWNPC_FLAG_ID          0
#define SHOWPLAYER_FLAG_ID       1
#define SHOWSHADOW_FLAG_ID       2
#define DRAWGROUND_FLAG_ID       3
#define DRAWLARGEOBJ_FLAG_ID     4
#define DRAWSMALLOBJ_FLAG_ID     5
#define FONTSHADOW_FLAG_ID       6
#define	LOCKSHORTCUT_FLAG_ID	 7
#define NUMBER_GAME_SELECTED     8
//end//
class KUiGameSetting : public KUiWndSingleton<KUiGameSetting>
{
public:
	KUiGameSetting( const CEGUI::String &id_name );
	~KUiGameSetting();
	
	static void Show( void );
	static void	Hide( void );
	void Init( void );
	
	void SetShowSearchHelp( bool b ) { d_bShowSearchHelp = b; }
	bool GetShowSearchHelp() { return d_bShowSearchHelp; }

	void SetShowSimpleHelp( bool b ) { d_bShowSimpleHelp = b; }
	bool GetShowSimpleHelp() { return d_bShowSimpleHelp; }

	void SetShowFirstHelp( bool b ) { d_bShowFirstHelp = b; }
	bool GetShowFirstHelp() { return d_bShowFirstHelp; }

	void SetLockShortcut( bool b ) { d_bLockShortcut = b; }
	bool GetLockShortcut() { return d_bLockShortcut; }

	bool LoadAllGameSetValue( void );	
	void SaveAllGameSetValue( void );

	//render add/
	//获取当前按钮状态///
	void SetCurrentGameSet();
	//恢复上一次设置////////
	void ResumeGameSet();
	////

private:
	//获取并注册控件
	void GetContrlRegister( void );
	
	//音乐音效开关回调函数
	bool handleSlider( const CEGUI::EventArgs &args );
	
	// 全屏窗口切换回调函数
	bool handleScreen( const CEGUI::EventArgs &args );
	
	// 人物绘制开关回调函数
	bool ShowPlayer( const CEGUI::EventArgs &args );
	bool ShowNpc( const CEGUI::EventArgs &args );
	bool ShowShadow( const CEGUI::EventArgs &args );
	
	// 高低分辨率回调函数
	bool handleRadio( const CEGUI::EventArgs &args );
	
	// 场景绘制回调函数
	bool DrawGround( const CEGUI::EventArgs &args );
	bool DrawSmallObj( const CEGUI::EventArgs &args );
	bool DrawLargeObj( const CEGUI::EventArgs &args );
	
	// 字体阴影绘制回调函数
	bool handleFontShadow( const CEGUI::EventArgs &args );

	// 游戏快捷键图标锁定回调函数
	bool handleLockShortcut( const CEGUI::EventArgs &args );
	
	// 界面按钮回调函数
	bool handleClose( const CEGUI::EventArgs &args );
	bool handleCancel( const CEGUI::EventArgs &args );
	bool handleOK( const CEGUI::EventArgs &args );
	
private:
	

	void InitGameSetScrolContrl( KIniFile *pIni, char *pKeyValue, int *pValue, CEGUI::TLMiniHorzScrollbar *pCtrl, float fMaxRange );	
	void InitGameRadioContrl( KIniFile *Ini );
	void InitGameCheckContrl(KIniFile *pIni, char *pKeyValue, int *pValue, CEGUI::Checkbox *pCtrl, bool &bValue, int nDefault );
	
private:
	// 音乐音效开关
	CEGUI::TLMiniHorzScrollbar		*d_pMusicSlider;
	int								d_iMusicValue;
	CEGUI::TLMiniHorzScrollbar		*d_pVoiceSlider;
	int								d_iSoundValue;
	
	// 全屏窗口切换
	CEGUI::TLRadioButton			*d_pFullScrnRadio;
	CEGUI::TLRadioButton			*d_pWindowRadio;
	bool							d_bWinOrFull;
	
	// 人物绘制开关
	CEGUI::Checkbox					*d_pShowPlayer;
	bool							d_bShowPlayer;
	CEGUI::Checkbox					*d_pShowNpc;
	bool							d_bShowNpc;
	CEGUI::Checkbox					*d_pShowShadow;
	bool							d_bShowShadow;
	
	// 高低分辨率
	CEGUI::TLRadioButton			*d_pHeightRadio;
	CEGUI::TLRadioButton			*d_pLowRadio;
	int								d_screenWidth;
	int								d_screenHeight;
// 	int								d_originalScreenWidth;
// 	int								d_originalScreenHeight;
	bool							d_hasNotifiedScreenChange;
	
	// 场景绘制开关
	CEGUI::Checkbox					*d_pDrawGround;
	bool							d_bDrawGround;
	CEGUI::Checkbox					*d_pDrawSmallObj;
	bool							d_bDrawSmallObj;
	CEGUI::Checkbox					*d_pDrawLargeObj;
	bool							d_bDrawLargeObj;
	
	// 字体阴影绘制开关
	CEGUI::Checkbox					*d_pFontShadow;
	bool							d_bFontShadow;

	// 游戏快捷键图标锁定
	CEGUI::Checkbox					*d_pLockShortcut;
	bool							d_bLockShortcut;
	
	// 查询帮助开关
	bool							d_bShowSearchHelp;
	bool							d_bShowSimpleHelp;
	bool							d_bShowFirstHelp;
	
	// 界面按钮
	CEGUI::TLButton					*d_pCloseButton;
	CEGUI::TLButton					*d_pCancelButton;
	CEGUI::TLButton					*d_pOKButton;
	//render add
	bool                          currentGameSet[NUMBER_GAME_SELECTED];
	//end
	
	
};
#endif