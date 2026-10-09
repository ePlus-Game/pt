#include "KWin32.h"
#include "KIniFile.h"
#include <windows.h>
#include <windowsx.h>
#include "loadSrcWnd/GDILoadBitmap.h"
#include "loadSrcWnd/ProcessBar.h"
using namespace LOAD_PROCESSBAR;



GDIProcessBar::GDIProcessBar()
{
	 processBarX = 0;
	processBarY = 0;
	processBarWidth = 0;
	processBarHeight = 0;
	barX = 0;
	barY = 0;
	barWidth = 0;
	barHeight = 0;
	alphaProcessBarColor = 0;
	alphaMixProcessBarColor = 0;
	alphaBarColor = 0;
	pProcessBarBitmap = 0;
	pBarBitmap = 0;
	hParentWnd = 0;
	currentProcess = 0;
	 
}
GDIProcessBar::GDIProcessBar(HWND hParent)
{
	 processBarX = 0;
	processBarY = 0;
	processBarWidth = 0;
	processBarHeight = 0;
	barX = 0;
	barY = 0;
	barWidth = 0;
	barHeight = 0;
	alphaProcessBarColor = 0;
	alphaMixProcessBarColor = 0;
	alphaBarColor = 0;
	pProcessBarBitmap = 0;
	pBarBitmap = 0;
	hParentWnd = 0;
	currentProcess = 0;
	hParentWnd = hParent;
}
GDIProcessBar::~GDIProcessBar()
{
	if(pProcessBarBitmap)
	{
		delete pProcessBarBitmap;
		pProcessBarBitmap = 0;
	}
	if(pBarBitmap)
	{
		delete pBarBitmap;
		pBarBitmap = 0;
	}
}

void GDIProcessBar::ProcessBarDrawItem(HDC hDestDC, 
									   LPGDIBITMAP pDestBitmap,
									   LPGDIBITMAP pSrBKcBitmap,
									   float alpha,
									   const char* infoText)
{
	if(pDestBitmap == 0||pSrBKcBitmap == 0)
		return ;

	if(pDestBitmap->GDIBitmapGetBitmapInfo().bmBitsPixel == 16)
		ProcessBarDrawItem16(hDestDC,pDestBitmap,pSrBKcBitmap,alpha,infoText);
	else
	if(pDestBitmap->GDIBitmapGetBitmapInfo().bmBitsPixel == 32)
		ProcessBarDrawItem32(hDestDC,pDestBitmap,pSrBKcBitmap,alpha,infoText);
}
void GDIProcessBar::ProcessBarDrawItem32(HDC hDestDC, LPGDIBITMAP pDestBitmap, LPGDIBITMAP pSrBKcBitmap, float alpha,const char* infoText)
{
	UINT* pDestBitmapBuffer = (UINT*)pDestBitmap->GDIBitmapGetBItmapBuffer();
	const BITMAP& destBitmapInfo = pDestBitmap->GDIBitmapGetBitmapInfo();
	UINT* pSrcBkBitmapBuffer = (UINT*)pSrBKcBitmap->GDIBitmapGetBItmapBuffer();	
	const BITMAP& srcBitmapInfo = pSrBKcBitmap->GDIBitmapGetBitmapInfo();
	memcpy(pDestBitmapBuffer,pSrcBkBitmapBuffer,(srcBitmapInfo.bmWidth*srcBitmapInfo.bmHeight)<<2);

	UINT* pProcessBarBitmapBuffer = (UINT*)pProcessBarBitmap->GDIBitmapGetBItmapBuffer();
	const BITMAP& processBarBitmapInfo = pProcessBarBitmap->GDIBitmapGetBitmapInfo();
	UINT* pBarBitmapBuffer = (UINT*)pBarBitmap->GDIBitmapGetBItmapBuffer();
	const BITMAP& barBitmapInfo = pBarBitmap->GDIBitmapGetBitmapInfo();
	UINT* pDestBitmapBuffer1 = pDestBitmapBuffer+processBarX+processBarY*processBarBitmapInfo.bmWidth;
	for(int y = 0; y < processBarBitmapInfo.bmHeight;y++)
	{
		for(int x = 0; x < processBarBitmapInfo.bmWidth;x++)
		{
			if(pProcessBarBitmapBuffer[x] == alphaProcessBarColor )
				continue;
			if(pProcessBarBitmapBuffer[x] == alphaMixProcessBarColor)
			{
				if(x>currentProcess)
				{
					continue;
				}
				DWORD destColor = pDestBitmapBuffer1[x];
				int dest_r = (destColor>>16);
				int dest_g = (((destColor)>>8)&255);
				int dest_b = ((destColor)&255);

				DWORD srcColor = pBarBitmapBuffer[x-barX+(y-barY)*barBitmapInfo.bmWidth];
				int src_r = (srcColor>>16);
				int src_g = (((srcColor)>>8)&255);
				int src_b = ((srcColor)&255);
				float mix_r = (float)dest_r*(1.0f-alpha)+(float)src_r*alpha;
				float mix_g = (float)dest_g*(1.0f-alpha)+(float)src_g*alpha;
				float mix_b = (float)dest_b*(1.0f-alpha)+(float)src_b*alpha;
				pDestBitmapBuffer1[x] = _PROCESSBAR_RGB((int)mix_r,(int)mix_g,(int)mix_b);
				continue;
			}
			pDestBitmapBuffer1[x] = pProcessBarBitmapBuffer[x];
		}
		pDestBitmapBuffer1+=destBitmapInfo.bmWidth;
		pProcessBarBitmapBuffer+=processBarBitmapInfo.bmWidth;
	}
	HBITMAP hDestBitmap = CreateBitmap(destBitmapInfo.bmWidth,destBitmapInfo.bmHeight,1,32,pDestBitmapBuffer);
	HDC hdc1 = CreateCompatibleDC(hDestDC);
	HBITMAP hOld = SelectBitmap(hdc1,hDestBitmap);
	if(infoText)
	{
		SetBkMode(hdc1,TRANSPARENT);
		RECT rc;
		rc.left = processBarX + processBarWidth/4;
		rc.right = processBarX + processBarWidth/4*3;
		rc.top = processBarY + processBarHeight + 10;
		rc.bottom = rc.top + 25;
		DrawText(hdc1,infoText,-1,&rc,DT_CENTER|DT_NOCLIP|DT_VCENTER);
		SetBkMode(hdc1,OPAQUE);
	}
	BitBlt(hDestDC,0,0,destBitmapInfo.bmWidth,destBitmapInfo.bmHeight,hdc1,0,0,SRCCOPY);
	SelectBitmap(hdc1,hOld);
	DeleteObject(hDestBitmap);
	DeleteDC(hdc1);
}
void GDIProcessBar::ProcessBarDrawItem16(HDC hDestDC,
		                       LPGDIBITMAP pDestBitmap,
		                       LPGDIBITMAP pSrBKcBitmap,
		                       float alpha,const char* infoText)
{
	USHORT* pDestBitmapBuffer = (USHORT*)pDestBitmap->GDIBitmapGetBItmapBuffer();
	const BITMAP& destBitmapInfo = pDestBitmap->GDIBitmapGetBitmapInfo();
	USHORT* pSrcBkBitmapBuffer = (USHORT*)pSrBKcBitmap->GDIBitmapGetBItmapBuffer();	
	const BITMAP& srcBitmapInfo = pSrBKcBitmap->GDIBitmapGetBitmapInfo();
	memcpy(pDestBitmapBuffer,pSrcBkBitmapBuffer,(srcBitmapInfo.bmWidth*srcBitmapInfo.bmHeight)<<1);

	USHORT* pProcessBarBitmapBuffer = (USHORT*)pProcessBarBitmap->GDIBitmapGetBItmapBuffer();
	const BITMAP& processBarBitmapInfo = pProcessBarBitmap->GDIBitmapGetBitmapInfo();
	USHORT* pBarBitmapBuffer = (USHORT*)pBarBitmap->GDIBitmapGetBItmapBuffer();
	const BITMAP& barBitmapInfo = pBarBitmap->GDIBitmapGetBitmapInfo();
	USHORT* pDestBitmapBuffer1 = pDestBitmapBuffer+processBarX+processBarY*processBarBitmapInfo.bmWidth;
	for(int y = 0; y < processBarBitmapInfo.bmHeight;y++)
	{
		for(int x = 0; x < processBarBitmapInfo.bmWidth;x++)
		{
			if(pProcessBarBitmapBuffer[x] == alphaProcessBarColor )
				continue;
			if(pProcessBarBitmapBuffer[x] == alphaMixProcessBarColor)
			{
				if(x>currentProcess)
				{
					continue;
				}
				USHORT destColor = pDestBitmapBuffer1[x];
				int dest_r = ((destColor>>11)<<3);
				int dest_g = ((((destColor)>>5)&63)<<2);
				int dest_b = (((destColor)&31)<<3);

				USHORT srcColor = pBarBitmapBuffer[x-barX+(y-barY)*barBitmapInfo.bmWidth];
				int src_r = ((srcColor>>11)<<3);
				int src_g = ((((srcColor)>>5)&63)<<2);
				int src_b = (((srcColor)&31)<<3);
				float mix_r = (float)dest_r*(1.0f-alpha)+(float)src_r*alpha;
				float mix_g = (float)dest_g*(1.0f-alpha)+(float)src_g*alpha;
				float mix_b = (float)dest_b*(1.0f-alpha)+(float)src_b*alpha;
				int mixR = (int) mix_r;
				int mixG = (int) mix_g;
				int mixB = (int) mix_b;
				mixR>>=3;
				mixG>>=2;
				mixB>>=3;
				pDestBitmapBuffer1[x] = _PROCESSBAR_RGB16BIT(mixR,mixG,mixB);
				continue;
			}
			pDestBitmapBuffer1[x] = pProcessBarBitmapBuffer[x];
		}
		pDestBitmapBuffer1+=destBitmapInfo.bmWidth;
		pProcessBarBitmapBuffer+=processBarBitmapInfo.bmWidth;
	}
	HBITMAP hDestBitmap = CreateBitmap(destBitmapInfo.bmWidth,destBitmapInfo.bmHeight,1,16,pDestBitmapBuffer);
	HDC hdc1 = CreateCompatibleDC(hDestDC);
	HBITMAP hOld = SelectBitmap(hdc1,hDestBitmap);
	if(infoText)
	{
		SetBkMode(hdc1,TRANSPARENT);
		RECT rc;
		rc.left = processBarX + processBarWidth/4;
		rc.right = processBarX + processBarWidth/4*3;
		rc.top = processBarY + processBarHeight + 1;
		rc.bottom = rc.top + 10;
		DrawText(hdc1,infoText,-1,&rc,DT_CENTER|DT_NOCLIP|DT_VCENTER);
		SetBkMode(hdc1,OPAQUE);
	}
	BitBlt(hDestDC,0,0,destBitmapInfo.bmWidth,destBitmapInfo.bmHeight,hdc1,0,0,SRCCOPY);
	SelectBitmap(hdc1,hOld);
	DeleteObject(hDestBitmap);
	DeleteDC(hdc1);
}

BOOL GDIProcessBar::ProcessBarLoadCfgINI()
{
	KIniFile iniFile;
	TCHAR  szPath[MAX_PATH] = {0},szValue[MAX_PATH];
	char szImagePathIndex[]=_PROCESSBAR_SRC_PATH;
	char szImagePath[MAX_PATH];
	strcpy(szPath,_PROCESSBAR_CFG_FILE);
	iniFile.Load(szPath);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_X,0,&processBarX);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_Y,0,&processBarY);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_WIDTH,0,&processBarWidth);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_HEIGHT,0,&processBarHeight);

	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_BAR_X,0,&barX);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_BAR_Y,0,&barY);

	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_BAR_WIDTH,0,&barWidth);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_BAR_HEIGHT,0,&barHeight);


	iniFile.GetString(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_BK_BITMAP,"",szValue,MAX_PATH);
	memset(szImagePath,0,MAX_PATH);
	strcpy(szImagePath,szImagePathIndex);
	strcat(szImagePath,szValue);
	pProcessBarBitmap = new GDIBITMAP;
	if(!pProcessBarBitmap->GDIBitmapFromFile(szImagePath))
	{
		delete  pProcessBarBitmap;
		pProcessBarBitmap = 0;
		return FALSE;
	}
	pBarBitmap = new GDILoadBitmap;

	iniFile.GetString(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_BAR_BK_BITMAP,"",szValue,MAX_PATH);
	memset(szImagePath,0,MAX_PATH);
	strcpy(szImagePath,szImagePathIndex);
	strcat(szImagePath,szValue);
	if(!pBarBitmap->GDIBitmapFromFile(szImagePath))
	{
		delete  pProcessBarBitmap;
		delete pBarBitmap;
		pProcessBarBitmap= 0;
		pBarBitmap = 0;
		return FALSE;
	}
	BOOL is16Bitmap = FALSE;
	if(pProcessBarBitmap->GDIBitmapGetBitmapInfo().bmBitsPixel == 16)
		is16Bitmap = TRUE;
	int r,g,b;
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_ALPHA_COLOR_R,0,&r);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_ALPHA_COLOR_G,0,&g);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_ALPHA_COLOR_B,0,&b);

	if(is16Bitmap)
	{
		r>>=3;
		g>>=2;
		b>>=3;
		alphaProcessBarColor = _PROCESSBAR_RGB16BIT(r,g,b);
	}
	else
		alphaProcessBarColor = _PROCESSBAR_RGB(r,g,b);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_ALPHA_MIX_COLOR_R,0,&r);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_ALPHA_MIX_COLOR_G,0,&g);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_ALPHA_MIX_COLOR_B,0,&b);

	if(is16Bitmap)
	{
		r>>=3;
		g>>=2;
		b>>=3;
		alphaMixProcessBarColor = _PROCESSBAR_RGB16BIT(r,g,b);
	}
	else
		alphaMixProcessBarColor = _PROCESSBAR_RGB(r,g,b);
	
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_BAR_ALPHA_COLOR_R,0,&r);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_BAR_ALPHA_COLOR_G,0,&g);
	iniFile.GetInteger(_PROCESSBAR_ITEM_NAME,_PROCESSBAR_BAR_ALPHA_COLOR_B,0,&b);

	if(is16Bitmap)
		alphaBarColor = _PROCESSBAR_RGB16BIT(r,g,b);
	else
		alphaBarColor = _PROCESSBAR_RGB(r,g,b);
	return TRUE;	
}

void GDIProcessBar::ProcessBarSetParentWnd(HWND hParentWnd)
{
	this->hParentWnd = hParentWnd;
}

