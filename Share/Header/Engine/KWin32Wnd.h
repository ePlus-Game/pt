//-------------------------------------------------------------
//	Purpose   :	 
//	Filename  :	 KWin32Wnd.h
//	Author    :	 Lucifer~yu (Zhang jian yu)
//	CreateTime:	 05/22/2006
//-------------------------------------------------------------
#ifndef KWIN32WND_H
#define KWIN32WND_H

#include "KWin32App.h"

ENGINE_API HWND			g_GetMainHWnd		(					);
ENGINE_API HWND			g_GetDrawHWnd		(					);
ENGINE_API void			g_GetClientRect		( LPRECT lpRect		);
ENGINE_API UINT			g_GetScreenHeight	(					);
ENGINE_API UINT			g_GetScreenWidth	(					);
ENGINE_API KWin32App*	g_GetMainApp		(					);
ENGINE_API KWin32Frame*	g_GetMainWnd		(					);
ENGINE_API HINSTANCE    g_GethInstance		(					);

ENGINE_API void		    g_SetFontQuality	( bool bHighQuality	);
ENGINE_API bool		    g_IsHighFontQuality	(					);

ENGINE_API void			g_SetFontBorder		( bool bBorder		);
ENGINE_API bool			g_IsFontWithBorder	(					);

ENGINE_API void			g_SetMainHWnd		( HWND hWnd			);
ENGINE_API void			g_SetDrawHWnd		( HWND hWnd			);
ENGINE_API void			g_SetScreenHeight	( UINT uHeight		);
ENGINE_API void			g_SetScreenWidth	( UINT uWidth		);
ENGINE_API void			g_SetFullScreen		( BOOL bFullWindow	);
ENGINE_API void			g_SetMainApp		( KWin32App*		);
ENGINE_API void			g_SetMainWnd		( KWin32Frame*		);


ENGINE_API void			g_ClientToScreen	( LPRECT lpRect		);
ENGINE_API void			g_ScreenToClient	( LPRECT lpRect		);
ENGINE_API void			g_ScreenToClient	( LPPOINT lpPoint	);
ENGINE_API void			g_ChangeWindowStyle	(					);
ENGINE_API BOOL			g_IsFullScreen		(					);

#define	MOUSE_EVENT_NONE			0	//未发生鼠标活动事件
#define	MOUSE_EVENT_HAPPEND			1	//有鼠标活动事件
#define	MOUSE_HOVER_MSG_SENT		2	//无鼠标活动事件的持续时间超过了设定的时间限制，已发送了WM_NCMOUSEHOVER消息
#define	MOUSE_HOVER_START_TIME_MIN	3	//无鼠标活动事件的持续时间未超过设定的时间限制，此值表示无鼠标活动的开始时间
#ifndef WM_MOUSEHOVER
	#define WM_MOUSEHOVER 0x02A1
#endif

#define SWORD_ICON 101

#endif
