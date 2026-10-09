// WndTool.h: interface for the CWndTool class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDTOOL_H__78175E51_D3D5_4137_8ECF_6F0D6D3C3E80__INCLUDED_)
#define AFX_WNDTOOL_H__78175E51_D3D5_4137_8ECF_6F0D6D3C3E80__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Picture.h"

//*********************************************************************
// macro : 计算某个成员变量在类对象中的偏移
//*********************************************************************
#define OFFSETOF_MEMBER(Class, Type, Member)		\
	((LONG)static_cast<Type*>(&(((Class*)0x8)->Member)) - 0x8)
//*********************************************************************
// macro : 根据在类对象中的偏移还原出成员变量
//*********************************************************************
#define MEMBER_ATOFFSET(Class, TheObj, Offset)		\
	(Class*)((char*)TheObj + Offset)

// 窗口位置
struct WindowRect
{
	LONG  lWndOffset;	// 成员窗口对象在类对象中的偏移位置
	BOOL  bShow;		// 是否显示
	RECT  rect;			// 窗口位置
};

struct BMPButton
{
	LONG  lWndOffset;	// 成员窗口对象在类对象中的偏移位置
	BOOL  bEnalbe;		// 是否显示
	POINT  ptPos;		// 窗口位置
	SIZE	size;
	LPCTSTR lpType;
	BYTE  byDrawType;
	DWORD dwUp;
	DWORD dwDown;
	DWORD dwOver;
	DWORD dwDisalbe;
	UINT  uTransColor;
};

struct URLBmpButton
{
	LONG  lWndOffset;	// 成员窗口对象在类对象中的偏移位置
	BOOL  bEnalbe;		// 是否显示
	POINT  ptPos;			// 窗口位置
	SIZE	size;
	LPCTSTR lpType;
	BYTE  byDrawType;
	DWORD dwUp;
	DWORD dwDown;
	DWORD dwOver;
	DWORD dwDisalbe;
	UINT  uTransColor;
	LPCTSTR		pcszURL;
};

// URL链接
struct UrlLink
{
	LONG		lWndOffset;	// 成员变量对象在类对象中的偏移位置
	COLORREF	crDefault;	// 缺省字体颜色
	COLORREF	crOnMouse;	// 鼠标覆盖颜色
	BOOL		bUnderLine;	// 是否有下划线
	int			nFontIncrement;	// 字號增量
	LPCSTR		pcszCtlUrl;	// URL连接
};

enum COLOR
{
	COLOR_BLACK			= RGB(0, 0, 0),			// 黑色
	COLOR_RED			= RGB(255, 0, 0),		// 红色
	COLOR_GREEN			= RGB(0, 255, 0),		// 绿色
	COLOR_BLUE			= RGB(0, 0, 255),		// 蓝色
	COLOR_WHITE			= RGB(255, 255, 255),	// 白色
	COLOR_DEEPGRAY		= RGB(99, 99, 99),		// 深灰
	COLOR_FLATYELLOW	= RGB(117, 129, 136),	// 浅黄
		//COLOR_FLATYELLOW	= RGB(245, 239, 227),	// 浅黄
	COLOR_DEEPRED		= RGB(118, 15, 17),		// 暗红
	COLOR_FLATBLACK		= RGB(78, 52, 32),		// 棕色
	COLOR_BARLABEL		= RGB(33, 48, 33),		// 底栏label的颜色
	URL_BACKGROUND		= RGB(224,212,180),
	URL_HOTCOLOR		= RGB(225,58,40)
};

class CWndTool  
{
	void *m_pThis;
public:
	CWndTool(void *pThis);
	~CWndTool();
	//*********************************************************************
	// function		: ShowWindows
	// description	: 显示WindowRect数组中的所有控件窗口
	// parameter	: pWndRects	窗口位置数组
	// return		: void
	//*********************************************************************
	void ShowWindows(WindowRect *pWndRects);
	//*********************************************************************
	// function		: ShowWindows
	// description	: 设置超级联接控件的URL
	// parameter	: pUrlCtls	控件数组
	// return		: void
	//*********************************************************************
	void ShowBmpButtons(BMPButton *pBmpBtns);
	void ShowUrlBmpButtons(URLBmpButton *pUrlBtns);
	//*********************************************************************
	
	void ShowUrlCtls(UrlLink *pUrlCtls);
	//*********************************************************************
	// function		: ShowBitmap
	// description	: 绘制位图
	// parameter	: pDC
	// parameter	: pBitMap 位图对象
	// parameter	: lLeft	  显示的最左位置
	// parameter	: lTop	  显示的最上位置
	// return		: BOOL
	//*********************************************************************
	BOOL ShowBitmap(CDC *pDC, CBitmap *pBitMap, LONG lLeft, LONG lTop);
	//*********************************************************************
	// function		: ShowPicture
	// description	: 绘制图像
	// parameter	: pDC
	// parameter	: pPicture 图像对象
	// parameter	: lLeft	  显示的最左位置
	// parameter	: lTop	  显示的最上位置
	// return		: BOOL
	//*********************************************************************
	BOOL ShowPicture(CDC *pDC, CPicture *pPicture, LPRECT pRect);
	//*********************************************************************
	// function		: 坐标是否位于位图范围内
	// parameter	: x 起点的横坐标
	// parameter	: y 起点的纵坐标
	// parameter	: pBitmap 位图对象
	// parameter	: pt	落点
	// return		: 如果落点在区域内，返回TRUE，否则返回FALSE
	//*********************************************************************
	BOOL InBmpZoom(long x, long y, CBitmap *pBitmap, const CPoint &pt);
	//*********************************************************************
	// function		: 获取子控件的窗口位置
	// parameter	: pChildCwnd 子窗口对象
	// parameter	: pRects	注册的窗口位置数组
	// return		: RECT* 查找到的窗口位置结构指针，返回没有找到返回NULL
	//*********************************************************************
	RECT *GetClientRect(CWnd *pChildWnd, WindowRect *pRects);
};

#endif // !defined(AFX_WNDTOOL_H__78175E51_D3D5_4137_8ECF_6F0D6D3C3E80__INCLUDED_)
