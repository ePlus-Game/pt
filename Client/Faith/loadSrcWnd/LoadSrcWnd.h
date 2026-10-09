#ifndef _LOAD_SRC_WND_H
#define _LOAD_SRC_WND_H

#define WM_DRAWLOADSRCWND WM_USER+100  //绘制消息lParam表示一个SrcWnd

#define _LOAD_SRC_WND_WIDTH     "width"
#define _LOAD_SRC_WND_HEIGHT     "height"
#define _LOAD_SRC_WND_BKSRC      "bkSrc"
#define _LOAD_SRC_WND_ITEM_NAME  "processBarWnd"
#define _LOAD_SRC_WND_IS_HAVE_COLORKEY "isHaveColorkey"
#define _LOAD_SRC_WND_COLORKEY_R        "clip_colorkey_r"
#define _LOAD_SRC_WND_COLORKEY_G        "clip_colorkey_g"
#define _LOAD_SRC_WND_COLORKEY_B        "clip_colorkey_b"




typedef class LoadSrcWnd
{
public:
	LoadSrcWnd();
	~LoadSrcWnd();
	void LoadSrcWndLoadCfgINT();
	void LoadSrcWndCreate(HWND hParent);
	void LoadSrcWndDrawItem(HDC hDest,const char* infoText);
	static BOOL LoadSrcWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
	void LoadSrcWndAdjustWindow();
	void LoadSrcWndDestroy();
	void LoadSrcWndSetProcess(float process,const char* infoText);
	void LoadSrcWndLoadToDestProcess(float dProcess,const char* infoText);
	float LoadSrcWndGetCurrentProcess() const {return currentProcess;}
	void  LoadSrcWndCreateWindowRGN(HBITMAP hBitmap,DWORD colorKey ,bool isBitmap16 );
	void LoadSrcWndSetWindowRGN();
protected:
	int x,y;
	int width,height;
	HWND hwnd;
	float alpha;
	float currentProcess;
	LPGDIBITMAP pBkSrcBitmap;
	LPGDIBITMAP pScreenBitmap;
	PROCESSBAR  processBar;
	HRGN        hWndRgn;
	bool		bFlag;
}LOADSRCWND,*LPLOADWRCWND;
#endif