#include <windows.h>
#include <windowsx.h>
#include "loadSrcWnd/GDILoadBitmap.h"
#include "loadSrcWnd/ProcessBar.h"
using namespace LOAD_PROCESSBAR;
#include "loadSrcWnd/LoadSrcWnd.h"
#include "KWin32App.h"
#include "resource.h"
#include <math.h>
#include "KIniFile.h"
LoadSrcWnd::LoadSrcWnd()

{
	pBkSrcBitmap = 0;
	pScreenBitmap = 0;
	x = 0;
	y =0;
	alpha =0.3f;
	currentProcess = 0.0f;
	hWndRgn = 0;
#ifdef _DEBUG
	bFlag = false;
#else
	bFlag = true;
#endif
}
LoadSrcWnd::~LoadSrcWnd()
{
	if(pScreenBitmap)
	    delete pScreenBitmap;
	if(pBkSrcBitmap)
		delete pBkSrcBitmap;
	if(hWndRgn)
		DeleteObject(hWndRgn);
}

void LoadSrcWnd::LoadSrcWndSetWindowRGN()
{
	if ( bFlag == false )
	{
		return;
	}

	if(hWndRgn)
	SetWindowRgn(hwnd,hWndRgn,TRUE);
}
void LoadSrcWnd::LoadSrcWndCreateWindowRGN(HBITMAP hBitmap,COLORREF colorKey,bool isBitmap16)
{
	if ( bFlag == false )
	{
		return;
	}

	BITMAP bm;;
	GetObject(hBitmap,sizeof(bm),&bm);
	HRGN tempRgn;
	if(isBitmap16 == false)
	{
		UINT* pBitmapBuffer = new UINT[bm.bmWidth*bm.bmHeight];
		GetBitmapBits(hBitmap,bm.bmWidth*bm.bmHeight*sizeof(UINT),pBitmapBuffer);
		hWndRgn = CreateRectRgn(0,0,0,0);
		for(int y = 0;y < bm.bmHeight;y++)
		{
			for(int x = 0; x < bm.bmWidth;x++)
			{
				if(pBitmapBuffer[x + y*bm.bmWidth] != colorKey)
				{
					tempRgn = CreateRectRgn(x,y,x+1,y+1);
					CombineRgn(hWndRgn,hWndRgn,tempRgn,RGN_OR);
					DeleteObject(tempRgn);	
				}
			}

		}
		delete [] pBitmapBuffer;
	}
	else
	{
		USHORT* pBitmapBuffer = new USHORT[bm.bmWidth*bm.bmHeight];
		GetBitmapBits(hBitmap,bm.bmWidth*bm.bmHeight*sizeof(USHORT),pBitmapBuffer);
		hWndRgn = CreateRectRgn(0,0,0,0);
		USHORT colorkey16 = (unsigned short)colorKey;
		for(int y = 0;y < bm.bmHeight;y++)
		{
			for(int x = 0; x < bm.bmWidth;x++)
			{
				if(pBitmapBuffer[x + y*bm.bmWidth] != colorkey16)
				{
					tempRgn = CreateRectRgn(x,y,x+1,y+1);
					CombineRgn(hWndRgn,hWndRgn,tempRgn,RGN_OR);
					DeleteObject(tempRgn);	
				}
			}

		}
		delete [] pBitmapBuffer;
	}
}
void LoadSrcWnd::LoadSrcWndLoadToDestProcess(float dProcess,const char* infoText)
{

	if ( bFlag == false )
	{
		return;
	}

	if(currentProcess>=1.0f)
		return;
	float dp = 0.0f;
	if(currentProcess+dProcess >= 1.0f)
		dp = 1.0f - currentProcess;
	else
		dp = dProcess;
	int showCount = (int)(500.0*dp);
	for(int i = 0; i < showCount;i++)
	{

		float process1 = currentProcess+ dp/((float)showCount-1.0f)*(float)i;
		LoadSrcWndSetProcess(process1,infoText);
	}
	currentProcess+= dp;
}
void LoadSrcWnd::LoadSrcWndLoadCfgINT()
{

	if ( bFlag == false )
	{
		return;
	}

	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH]= {0};
	char szImagePathIndex[]=_PROCESSBAR_SRC_PATH;
	char szImagePath[MAX_PATH];
	strcpy(szPath,_PROCESSBAR_CFG_FILE);
	iniFile.Load(szPath);
	iniFile.GetInteger(_LOAD_SRC_WND_ITEM_NAME,_LOAD_SRC_WND_WIDTH,0,&width);
	iniFile.GetInteger(_LOAD_SRC_WND_ITEM_NAME,_LOAD_SRC_WND_HEIGHT,0,&height);

	iniFile.GetString(_LOAD_SRC_WND_ITEM_NAME,_LOAD_SRC_WND_BKSRC,"",szValue,MAX_PATH);
	memset(szImagePath,0,MAX_PATH);
	strcpy(szImagePath,szImagePathIndex);
	strcat(szImagePath,szValue);
	pBkSrcBitmap = new GDIBITMAP;
	if(!pBkSrcBitmap->GDIBitmapFromFile(szImagePath))
	{
		delete  pBkSrcBitmap;
		return ;
	}
	HBITMAP hTempBitmap = LoadBitmapFromFile(szImagePath);//HBITMAP)LoadImage(NULL,szImagePath,IMAGE_BITMAP,0,0,LR_DEFAULTCOLOR|LR_LOADFROMFILE);
	BOOL isHaveColorKey = FALSE;
	int r,g,b;
	DEVMODE	devMode;
	::EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &devMode);
	iniFile.GetInteger(_LOAD_SRC_WND_ITEM_NAME,_LOAD_SRC_WND_IS_HAVE_COLORKEY,0,&isHaveColorKey);
	if(isHaveColorKey==TRUE)
	{
		iniFile.GetInteger(_LOAD_SRC_WND_ITEM_NAME,_LOAD_SRC_WND_COLORKEY_R,0,&r);
		iniFile.GetInteger(_LOAD_SRC_WND_ITEM_NAME,_LOAD_SRC_WND_COLORKEY_G,0,&g);
		iniFile.GetInteger(_LOAD_SRC_WND_ITEM_NAME,_LOAD_SRC_WND_COLORKEY_B,0,&b);
		if(devMode.dmBitsPerPel == 32)
		{
			LoadSrcWndCreateWindowRGN(hTempBitmap,(r<<16)+(g<<8)+b,false);
		}
		else
		{
			LoadSrcWndCreateWindowRGN(hTempBitmap,((r>>3)<<11)+((g>>2)<<5)+(b>>3),true);
		}
	}
	DeleteObject(hTempBitmap);
	processBar.ProcessBarLoadCfgINI();
	pScreenBitmap = new GDIBITMAP;
	pScreenBitmap->GDIBitmapCreate(pBkSrcBitmap->GDIBitmapGetBitmapInfo().bmWidth,
		                           pBkSrcBitmap->GDIBitmapGetBitmapInfo().bmHeight,
								   pBkSrcBitmap->GDIBitmapGetBitmapInfo().bmBitsPixel);



}
void LoadSrcWnd::LoadSrcWndCreate(HWND hParent)
{

	if ( bFlag == false )
	{
		return;
	}

	if((hwnd=CreateDialog(KWin32App::m_hInstance,
		MAKEINTRESOURCE(IDD_LOAD_SRC_WND),
		hParent,
		(DLGPROC)LoadSrcWnd::LoadSrcWndProc))==0)
		return ;
	SendMessage(hwnd,WM_DRAWLOADSRCWND,0,(LPARAM)this);
	LoadSrcWndAdjustWindow();
	LoadSrcWndSetWindowRGN();
	ShowWindow(hwnd,SW_SHOW);
	SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
//	SendMessage(hwnd,WM_PAINT,0,0);
	
}
void LoadSrcWnd::LoadSrcWndDrawItem(HDC hDest,const char* infoText)
{
	if ( bFlag == false )
	{
		return;
	}

	processBar.ProcessBarDrawItem(hDest,pScreenBitmap,pBkSrcBitmap,alpha,infoText);
}
void LoadSrcWnd::LoadSrcWndDestroy()
{
	if ( bFlag == false )
	{
		return;
	}

	DestroyWindow(hwnd);
}
void LoadSrcWnd::LoadSrcWndSetProcess(float process,const char* infoText)
{
	if ( bFlag == false )
	{
		return;
	}
	if(process>1.0f)
		return;
	float length = (float)processBar.ProcessBarGetProcessWidth()*process;
	alpha = sqrtf(process);
	processBar.ProcessBarSetCurrentProcess((int)length);
	HDC hdc = GetWindowDC(hwnd);
	processBar.ProcessBarDrawItem(hdc,pScreenBitmap,pBkSrcBitmap,alpha,infoText);
}

void LoadSrcWnd::LoadSrcWndAdjustWindow()
{
	if ( bFlag == false )
	{
		return;
	}

	int screenWidth,screenHeight;
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);
	x  = screenWidth/2-width/2;
	y = screenHeight/2 - height/2;
	MoveWindow(hwnd,x,y,width,height,TRUE);

}
BOOL LoadSrcWnd::LoadSrcWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
{

	static LPLOADWRCWND pLoadSrcWnd = 0;
	switch(msg)
	{
	case WM_INITDIALOG:
		return FALSE;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd,&ps);
			if(pLoadSrcWnd)
				pLoadSrcWnd->LoadSrcWndDrawItem(hdc,0);
			EndPaint(hwnd,&ps);
			
		}
		return FALSE;
	case WM_ERASEBKGND:
		{
			if(pLoadSrcWnd)
				pLoadSrcWnd->LoadSrcWndDrawItem((HDC)wParam,0);
		}
		return TRUE;
	case WM_DRAWLOADSRCWND:
		{
			pLoadSrcWnd = (LPLOADWRCWND)lParam;
			LPLOADWRCWND pp = pLoadSrcWnd;

		}
		return 0;
	case WM_DESTROY:
		{
			pLoadSrcWnd = 0;
		}
		break;
	}
	return FALSE;
}