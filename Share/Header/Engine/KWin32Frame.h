//-------------------------------------------------------------
//	Purpose   :	 
//	Filename  :	 KWin32Frame.h
//	Author    :	 Lucifer~yu (Zhang jian yu)
//	CreateTime:	 05/22/2006
//-------------------------------------------------------------
#ifndef KWIN32FRAME_H
#define KWIN32FRAME_H

#include "KWin32.h"

class KWin32App;

struct KFrameParam 
{
	BOOL			bShowMouse;
	BOOL			bMultiGame;
	unsigned int	uHoverTime;
};

class ENGINE_API KWin32Frame
{
public:
	friend class KWin32App;
	KWin32Frame			();
	virtual ~KWin32Frame();
private:
	//Engine内部调用函数
	BOOL					RegisterClass	   ( HINSTANCE hInstance, TCHAR* szAppName, KFrameParam& rFrameParam );
	HWND					CreateFrame		   ( HINSTANCE hInstance, int uScreenW = 0, int uScreenH = 0		 );
	void					MessageLoop		   (																 );
	void					DestroyFrame	   (																 );
public:
	static	bool			s_minisized;	//xiehong add 2007-8-8
	//系统回调消息函数
	static LRESULT CALLBACK WndProc			   ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam				 );
	//要球应用层重载的函数
	virtual	BOOL			GameInit		   ( 																 );
	virtual BOOL			GameLoop		   (															 	 );
	virtual BOOL			GameExit		   (																 );
	virtual int				HandleInput		   ( UINT uMsg, WPARAM wParam, LPARAM lParam						 );	
	virtual void			SetFocus		   (																 );
private:
	//KWin32Frame内部调用的函数
	static void				MouseEnters		   (																 );
	static void				MouseLeaves		   (																 );
	static void				GenerateMsgHoverMsg(																				);

private:
	//KWin32Frame外部影响的变量
	TCHAR					m_szClass[DEFAULT_TITLE_NAME_LEN];
	TCHAR					m_szTitle[DEFAULT_TITLE_NAME_LEN];
	static BOOL				m_bShowMouse;
	static BOOL				m_bMultiGame;
	static unsigned int		m_uMouseHoverTimeSetting;
	//KWin32Frame内部影响的变量
	static BOOL				m_bActive;
	static unsigned int		m_nLastMousePos;
	static unsigned int		m_uLastMouseStatus;
	static unsigned int		m_uMouseHoverStartTime;
	static BOOL				m_bMouseInWindow;
};

#endif