// UiIEWindow.h: interface for the KUiQuestionWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_KUiQuestionWindow_H__07F63374_CF67_4E25_8DAE_9F433FFC3AD7__INCLUDED_)
#define AFX_KUiQuestionWindow_H__07F63374_CF67_4E25_8DAE_9F433FFC3AD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../uicommon.h"
#include "TLQuestionWindow.h"

class KUiQuestionWindow : public KUiWndSingleton<KUiQuestionWindow>
{
public:
	KUiQuestionWindow( const CEGUI::String& id_name );
	~KUiQuestionWindow();

public:
	void			Toggle( void );
	static void		Show( void );
	static void		Hide( void );
	void			Init( void );
	void			ShowQuestion( unsigned int uParam, int nParam );
	bool			BtnClose_Click( const EventArgs& args );
	bool			wndQuestion_AnswerCommited( const EventArgs& args );

private:
	TLQuestionWindow* m_pQuestionWnd;

	int m_currentSec;
};

#endif // !defined(AFX_UIIEWINDOW_H__07F63374_CF67_4E25_8DAE_9F433FFC3AD7__INCLUDED_)
