// UiIEWindow.h: interface for the KUiIEWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIIEWINDOW_H__07F63374_CF67_4E25_8DAE_9F433FFC3AD7__INCLUDED_)
#define AFX_UIIEWINDOW_H__07F63374_CF67_4E25_8DAE_9F433FFC3AD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../uicommon.h"

class KUiIEWindow : public KUiWndSingleton<KUiIEWindow>
{
public:
	KUiIEWindow(const CEGUI::String& id_name);
	~KUiIEWindow();

public:
	static void		Show			( void );
	static void		Hide			( void );
	void			Init();
	bool			handleKeyDown(const CEGUI::EventArgs& args);
	bool			btnClose_MouseClick(const CEGUI::EventArgs& args);
};

#endif // !defined(AFX_UIIEWINDOW_H__07F63374_CF67_4E25_8DAE_9F433FFC3AD7__INCLUDED_)
