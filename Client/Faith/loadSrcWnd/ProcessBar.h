#ifndef _PROCESS_BAR_H
#define _PROCESS_BAR_H
namespace LOAD_PROCESSBAR
{
//////////GDI_PROCESS_BAR//////////
#define _PROCESSBAR_SRC_PATH              "ui\\imagesets\\image\\"
#define _PROCESSBAR_CFG_FILE              "UiSettings\\loadSrcProcessBar.ini"
#define _PROCESSBAR_ITEM_NAME             "bar"
#define _PROCESSBAR_X                     "processBarX"
#define _PROCESSBAR_Y                     "processBarY"
#define _PROCESSBAR_WIDTH                 "processBarWidth"
#define _PROCESSBAR_HEIGHT                "processBarHeight"
#define _PROCESSBAR_BAR_X                 "barX"
#define _PROCESSBAR_BAR_Y                 "barY"
#define _PROCESSBAR_BAR_WIDTH             "barWidth"
#define _PROCESSBAR_BAR_HEIGHT            "barHeight"
#define _PROCESSBAR_ALPHA_COLOR_R         "alphaProcessBarColor_r"
#define _PROCESSBAR_ALPHA_COLOR_G         "alphaProcessBarColor_g"
#define _PROCESSBAR_ALPHA_COLOR_B         "alphaProcessBarColor_b"
#define _PROCESSBAR_ALPHA_MIX_COLOR_R     "alphaMixProcessBarColor_r"
#define _PROCESSBAR_ALPHA_MIX_COLOR_G     "alphaMixProcessBarColor_g"
#define _PROCESSBAR_ALPHA_MIX_COLOR_B     "alphaMixProcessBarColor_b"
#define _PROCESSBAR_BAR_ALPHA_COLOR_R     "alphaBarColor_r"
#define _PROCESSBAR_BAR_ALPHA_COLOR_G     "alphaBarColor_g"
#define _PROCESSBAR_BAR_ALPHA_COLOR_B      "alphaBarColor_b"
#define _PROCESSBAR_BK_BITMAP              "processBarBitmap"
#define _PROCESSBAR_BAR_BK_BITMAP           "barBitmap"
#define _PROCESSBAR_RGB(r,g,b)     (((r)&255)<<16)+(((g)&255)<<8)+((b)&255)
#define _PROCESSBAR_RGB16BIT(r,g,b) ((b&31)+((g&63)<<5)+((r&31)<<11))
inline void DrawBitmap(HDC dest_hdc,HBITMAP& hBitmap,int width ,int height ,int x = 0,int y = 0 )
{
	HDC hdc=CreateCompatibleDC(dest_hdc);
	BITMAP bitmap;
	GetObject(hBitmap,sizeof(BITMAP),&bitmap);
	HBITMAP hOld = (HBITMAP)SelectObject(hdc,hBitmap);
	StretchBlt(dest_hdc,x,y,width,height,hdc,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);
	SelectObject(hdc,hOld);
	DeleteDC(hdc);
}
typedef class GDIProcessBar
{
public:
	GDIProcessBar();
	~GDIProcessBar();
	GDIProcessBar(HWND hParent);
public:
	void ProcessBarDrawItem(HDC hDestDC,
		                       LPGDIBITMAP pDestBitmap,
		                       LPGDIBITMAP pSrBKcBitmap,
		                       float alpha,
							   const char* infoText);
	BOOL  ProcessBarLoadCfgINI();
	void  ProcessBarSetParentWnd(HWND hParentWnd);
	void  ProcessBarSetCurrentProcess(int length){currentProcess = length;}
	int ProcessBarGetProcessWidth() const { return processBarWidth;}
protected:
	void  ProcessBarDrawItem16(HDC hDestDC,
		                       LPGDIBITMAP pDestBitmap,
		                       LPGDIBITMAP pSrBKcBitmap,
		                       float alpha,
							   const char* infoText);
	void ProcessBarDrawItem32(HDC hDestDC,
		                       LPGDIBITMAP pDestBitmap,
		                       LPGDIBITMAP pSrBKcBitmap,
		                       float alpha,
							   const char* infoText);

protected:
	int processBarX;
	int processBarY;
	int processBarWidth;
	int processBarHeight;
	int barX;
	int barY;
	int barWidth;
	int barHeight;
	DWORD alphaProcessBarColor;
	DWORD alphaMixProcessBarColor;
	DWORD alphaBarColor;
	LPGDIBITMAP pProcessBarBitmap;
	LPGDIBITMAP pBarBitmap;
	HWND  hParentWnd;
	int currentProcess;
}PROCESSBAR,*LPPROCESSBAR;
};
#endif