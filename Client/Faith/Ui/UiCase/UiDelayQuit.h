//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2007
//
//      Created_datetime : 03/28/2007 9:50
//      File_base        : UiDelayQuit
//      File_ext         : h
//      Author           : Lucien (LIU Siliang)
//      Description      : 文件功能描述 退出延时
//
//      <Change_list>
//////////////////////////////////////////////////////////////////////

#ifndef _UIDELAYQUIT_H_
#define _UIDELAYQUIT_H_

#include "../UiCommon.h"
#include "CEGUI.h"

using namespace CEGUI;

enum QuitState
{
	TOMAINBEGIN,
	TOSELECTROLE,
};

class KUiDeleyQuit : public KUiWndSingleton<KUiDeleyQuit>
{
	bool			_timerAct;
public:
	KUiDeleyQuit( const CEGUI::String& id_name );
	~KUiDeleyQuit();

public:
	static void		Show();
	void			Init();
	// 显示延时
	void			DisplayDelay();
	// 重置
	void			Reset();
	void			Quit();
	void			QuitToSelectRole();

	void			SetQuitState( QuitState qs ) { m_bState = qs; }
	QuitState		GetQuitState( ) {	return m_bState; }

	void			setTimerAct(bool act){	_timerAct = act;	}
private:
	bool			handleQuit( const CEGUI::EventArgs& args );
	bool			handleCancel( const CEGUI::EventArgs& args );

	bool			btnHireConfig( const CEGUI::EventArgs& args );
	bool			btnHireSalary( const CEGUI::EventArgs& args );
	
private:
	CEGUI::Window*	m_pStaticText;
	int				m_nDisplayTimeControl;
	int				m_nDisplayTime;
	int				m_nTimeToQuit;
	bool			m_bQuitGame;

	QuitState		m_bState;
};

#endif