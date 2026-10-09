//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 06/14/2006 10:13
//      File_base        : UiAdapter
//      File_ext         : h
//      Author           : Lucifer~yu (Zhang jian yu)
//      Description      : 对 CEGUI 简单的应用封装 
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef UIADAPTER_H
#define UIADAPTER_H

#include "CEGUI.h"
#include "renderers/directx7GUIRenderer/dxdraw7renderer.h"
#include "KTimer.h"
#include <vector>
#include <string>
// LSL
#include "Sounder/UIDXSound.h"

class KUiAdapter
{
public:
	KUiAdapter();
	~KUiAdapter();
public:
	static	void				UiStartGame	  ( void												);//进入游戏运行时	
	static	void				UiEndGame	  ( void												);//离开游戏运行时
	static	void				HideUi		  ( void												);	
	static	void				SetMouseRes	  ( int nMouseRes										);
	static	int					GetMouseRes	  ( void												);
	static	bool				EscHideDialog (	void												);
	static	void				ReFreshUi	  (														);//刷新重画UI
	static	void				InitGameSet	  ( void												);

public:
	int							UiInit		  ( iRepresentShell* pRepresentShell					);//界面系统初始化
	int							UiStart		  (														);//開始界面控制流程
	void						UiPaint		  ( int nGameLoop										);//绘制界面
	void						UiBreathe	  ( void												);
	//add by render //
	void                        UiPaintOldMode( int nGameLoop                                       );
	void                        UiPaintNewMode( int nGameLoop                                        );
	void                        UiPaintNpcHeadInfo(int nGameLoop)                                      ;
	void                        UiPaintBottomOldWindow(int nGameLoop);
	//end//
	void						UiTimePulse	  ( DWORD fTimeElapsed									);
	int							UiExit		  (	void												);//界面系统退出
 	int							UiProcessInput( unsigned int uMsg, unsigned int uParam, int nParam	);//处理输入
	static void					UiCloseNoNpcDlg( void );
	void						SetFocus	  ( void												);//窗口获得焦点时清除系统按键

protected:
	void						PlayTitleMusic( void												);
	void						StopTitleMusic( void												);
	bool						InitMouseRes  ( void												);
	void						InitOldFont	  ( void												);	
	void						MouseLeaves	  (	void												);
	void						MouseEnters	  (	void												);
	
	void						InitMusicVolume( void												);
private:
	// LSL
	CEGUI::ISound*				m_pSound;
	static int					m_nCurMouse;
	CEGUI::Renderer*			m_pRenderer;
	CEGUI::System*				m_pCEGUISystem;
	CEGUI::WindowManager*		m_pWindowManager;
	BOOL						m_bMouseInWindow;
	int							m_nFrameRate;
	DWORD						m_dwPing;
	KTimer						m_Timer;
	static	KUiAdapter*			ms_This;
	float						m_fOldTimeElapsed;
	bool						m_bHideUi;
	std::vector<std::string>	m_mouseRes;
	HIMC						m_hIMC;
	bool						m_bHIMC;
};

#endif