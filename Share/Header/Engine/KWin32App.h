//-------------------------------------------------------------
//	Purpose   :	 
//	Filename  :	 KWin32App.h
//	Author    :	 Lucifer~yu (Zhang jian yu)
//	CreateTime:	 05/22/2006
//-------------------------------------------------------------
#ifndef KWIN32APP_H
#define KWIN32APP_H

#include "KWin32Frame.h"

struct ENGINE_API KWin32AppParam
{
	HINSTANCE	 hInstance;
	TCHAR		 szAppName[DEFAULT_TITLE_NAME_LEN];
	KWin32Frame* pMainFrame;
	unsigned int uScreenHeight;
	unsigned int uScreenWidth;
	unsigned int uMouseHoverTimeSetting;
	BOOL		 bFullScreen;
	BOOL		 bShowMouse;
	BOOL		 bMulti;
};

class ENGINE_API KWin32App
{
public:
	KWin32App();
	virtual ~KWin32App();

public:
	virtual BOOL			 CreateApplication( KWin32AppParam &rAppParam	);
	virtual BOOL			 RunApplication	  (								);
	virtual void			 StopApplication  (								);

public:
	static HINSTANCE		 m_hInstance;
	static KWin32App*		 m_pWin32App;
	static KWin32Frame*		 m_pMainFrame;
	static HWND				 m_hMainWnd;
	static HWND				 m_hMainDrawWnd;
	static unsigned int		 m_uScreenHeight;
	static unsigned int		 m_uScreenWidth;
	static BOOL				 m_bFullScreen;
	static FILE*			 m_pSout;	
	static FILE*             m_pSerr;	
};

//---------------------------------------------------------------------------
#endif
