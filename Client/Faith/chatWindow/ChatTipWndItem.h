#ifndef _CHAT_WND_ITEM_H
#define _CHAT_WND_ITEM_H
#define CHAT_TIP_ITEM_SHOW_ID   300
#define CHAT_TIP_ITEM_BUTTON_ID 301
typedef class ChatTipWndItem
{
public:
	ChatTipWndItem();
	~ChatTipWndItem();
	static BOOL CALLBACK ChatTipWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void   ChatTipWndAdjustPos();
	void   ChatTipWndCreate(HWND hwnd);
	void   ChatTipWndDrawItem(LPDRAWITEMSTRUCT lpdis);
	void   ChatTipWndDrawTip(HDC hdc);
	void   ChatTipWndClickButton(HWND hwnd,WPARAM wParam,LPARAM lParam);
	void   ChatTipWndProcessPaint(HDC hdc);

	void   ChatTipWndShow(BOOL bShow);
	BOOL   ChatTipWndIsShow() {return isShow;}

	static LRESULT CALLBACK ChatTipColosButtonProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

public:
	WNDPROC  closeButtonProc;
	HWND hTipItemWnd;
	HWND hShowItem;
	ChatButton   closeButton;
	int wndBitmapIdx ;

	int leftBitmapIdx;
	int rightBitmapIdx;
	int topBitmapIdx;
	int endBitmapIdx;
	int leftTopBitmapIdx;
	int rightTopBitmapIdx;
	int leftEndBitmapIdx;
	int rightEndBItmapIdx;
	int leftDist;
	int topDist;
	int rightDist;
	int endDist;
	int width,height;
	int x,y;
//	int startRenderX,startRenderY;
	ILayout* pTipItemLay;
	BOOL isShow;
	WNDPROC tipShowProc;
}TIPITEM,*LPTIPITEM;
#endif