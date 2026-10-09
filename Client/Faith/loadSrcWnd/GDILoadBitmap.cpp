#include <windows.h>
#include <windowsx.h>
#include "loadSrcWnd/GDILoadBitmap.h"
#include "KWin32App.h"
#include "KPakFile.h"
#include "KPakList.h"

BOOL  Flip_Bitmap(UCHAR* bitmap_buffer,
		          int bytes_per_line,int height)
{
	UCHAR* tempBuffer=new UCHAR[bytes_per_line*height];
	if(tempBuffer==0)
		return FALSE;
	memcpy(tempBuffer,bitmap_buffer,bytes_per_line*height);
	for(int index=0;index<height;index++)
	{
		memcpy(bitmap_buffer+index*bytes_per_line,tempBuffer+(height-index-1)*bytes_per_line,bytes_per_line);
	}
	delete [] tempBuffer;
	return TRUE;
}


HBITMAP LoadBitmap32FromFile(const char* fileName)
{
	#define _RGB16BIT565(r,g,b) ((b&31)+((g&63)<<5)+((r&31)<<11))
	KPakFile pack;
	if(!pack.Open(fileName))
	{
#ifdef _DEBUG
		MessageBox(NULL,"image has not been loaded","NULL",MB_OK);
#endif
		return 0;
	}
	BITMAPFILEHEADER bmfileheader;
	BITMAPINFOHEADER bminfoheader;
	pack.Read(&bmfileheader,sizeof(BITMAPFILEHEADER));
	if(bmfileheader.bfType!=0x4D42)
	{
#ifdef _DEBUG
		MessageBox(NULL,"image has not been loaded","NULL",MB_OK);
#endif
		
		return 0;
	}
	pack.Read(&bminfoheader,sizeof(BITMAPINFOHEADER));
	if(bminfoheader.biBitCount!=32)
	{
#ifdef _DEBUG
		MessageBox(NULL,"the image is not the bitmap with color bpp 32","NULL",MB_OK);
#endif
		return 0;
	}
	if(bminfoheader.biSizeImage == 0)
	{
#ifdef _DEBUG
		MessageBox(NULL,"image has not been loaded","NULL",MB_OK);
#endif
		return 0;
	}
	void* pBitmapBuffer = new byte[bminfoheader.biSizeImage];
	void* pFinalBitmapBuffer = 0;
	pack.Seek(-((long)bminfoheader.biSizeImage),FILE_END);
	pack.Read(pBitmapBuffer,bminfoheader.biSizeImage);
	pack.Close();
	Flip_Bitmap((UCHAR*)pBitmapBuffer,bminfoheader.biWidth*(bminfoheader.biBitCount>>3),bminfoheader.biHeight);
	if(bminfoheader.biBitCount == 32)
	{
		HBITMAP hBitmap = CreateBitmap(bminfoheader.biWidth,bminfoheader.biHeight,1,32,pBitmapBuffer);
 		delete [] pBitmapBuffer;
		pBitmapBuffer = 0;
		return hBitmap;

	}
	delete [] pBitmapBuffer ;
	return 0;
}
HBITMAP   LoadBitmapFromFile(const char* fileName)
{
#define _RGB16BIT565(r,g,b) ((b&31)+((g&63)<<5)+((r&31)<<11))
	KPakFile pack;
//		BOOL AA =g_pPakList->Open("Package.ini");
	if(!pack.Open(fileName))
	{
#ifdef _DEBUG
		MessageBox(NULL,"image has not been loaded","NULL",MB_OK);
#endif
		return 0;
	}
	BITMAPFILEHEADER bmfileheader;
	BITMAPINFOHEADER bminfoheader;
	pack.Read(&bmfileheader,sizeof(BITMAPFILEHEADER));
	if(bmfileheader.bfType!=0x4D42)
	{
#ifdef _DEBUG
		MessageBox(NULL,"image has not been loaded","NULL",MB_OK);
#endif
		
		return 0;
	}
	pack.Read(&bminfoheader,sizeof(BITMAPINFOHEADER));
	if(bminfoheader.biBitCount!=16&&
	   bminfoheader.biBitCount!=24&&
	   bminfoheader.biBitCount!=32)
	{
#ifdef _DEBUG
		MessageBox(NULL,"image has not been loaded","NULL",MB_OK);
#endif
		return 0;
	}
	if(bminfoheader.biSizeImage == 0)
	{
#ifdef _DEBUG
		MessageBox(NULL,"image has not been loaded","NULL",MB_OK);
#endif
		return 0;
	}
	void* pBitmapBuffer = new byte[bminfoheader.biSizeImage];
	void* pFinalBitmapBuffer = 0;
	pack.Seek(-((long)bminfoheader.biSizeImage),FILE_END);
	pack.Read(pBitmapBuffer,bminfoheader.biSizeImage);
	pack.Close();
	Flip_Bitmap((UCHAR*)pBitmapBuffer,bminfoheader.biWidth*(bminfoheader.biBitCount>>3),bminfoheader.biHeight);
	DEVMODE	devMode;
/*	HWND hWnd = GetDesktopWindow();
	HDC hdc = GetWindowDC(hWnd);
	int colorBpp = GetDeviceCaps(hdc,BITSPIXEL);
	char buffer[256] = {0};
	sprintf(buffer,"%d",colorBpp);
	MessageBox(NULL,buffer,buffer,MB_OK);*/
	::EnumDisplaySettings( NULL, ENUM_CURRENT_SETTINGS, &devMode);
//	ReleaseDC(hWnd,hdc);
//	if(KWin32App::m_bFullScreen)
//		devMode.dmBitsPerPel = 32;
	if(devMode.dmBitsPerPel == 16)
	{
		if(bminfoheader.biBitCount == 16)
		{
			HBITMAP hBitmap = CreateBitmap(bminfoheader.biWidth,bminfoheader.biHeight,1,16,pBitmapBuffer);
 			delete [] pBitmapBuffer;
			pBitmapBuffer = 0;
			return hBitmap;
		}
		if(bminfoheader.biBitCount == 32)
		{
			pFinalBitmapBuffer = new USHORT[bminfoheader.biWidth*bminfoheader.biHeight];
			USHORT* pDest = (USHORT*)pFinalBitmapBuffer;
			UINT*   psrc = (UINT*) pBitmapBuffer;
			for(int y = 0; y < bminfoheader.biHeight ; y++)
			{

				for(int x = 0; x < bminfoheader.biWidth; x++)
				{
					DWORD src_color = psrc[x];
					UCHAR r = (UCHAR)(((src_color>>16)&0xff)>>3);
					UCHAR g = (UCHAR)(((src_color>>8)&0xff)>>2);
					UCHAR b = (CHAR)((src_color&0xff)>>3);
					USHORT color = (r<<11)+(g<<5)+(b);
					pDest[x] = color;
				}
				pDest += bminfoheader.biWidth;
				psrc += bminfoheader.biWidth;
			}
			delete [] pBitmapBuffer;
			HBITMAP hBitmap = CreateBitmap(bminfoheader.biWidth,bminfoheader.biHeight,1,16,pFinalBitmapBuffer);
			delete [] pFinalBitmapBuffer;
			return hBitmap;
		}
		if(bminfoheader.biBitCount == 24)
		{			
			pFinalBitmapBuffer = new USHORT[bminfoheader.biHeight*bminfoheader.biWidth];
			UCHAR*  psrc = (UCHAR*)pBitmapBuffer;
			USHORT* pDest = (USHORT*) pFinalBitmapBuffer;
			for(int y = 0;y < bminfoheader.biHeight;y++)
			{
				for(int x = 0; x < bminfoheader.biWidth; x++)
				{

					int b = psrc[3*x+y*bminfoheader.biWidth*3+0];
					int g = psrc[3*x+y*bminfoheader.biWidth*3+1];
					int r = psrc[3*x+y*bminfoheader.biWidth*3+2];
					r>>=3;
					g>>=2;
					b>>=3;
					pDest[x] =_RGB16BIT565(r,g,b);// (r<<11)+(g<<5)+b;
				}
//				psrc+=bminfoheader.biWidth*3;
				pDest+=bminfoheader.biWidth;
			}
			delete [] pBitmapBuffer;
			HBITMAP hBitmap = CreateBitmap(bminfoheader.biWidth,bminfoheader.biHeight,1,16,pFinalBitmapBuffer);
			delete [] pFinalBitmapBuffer;
			pFinalBitmapBuffer = 0;
			return hBitmap;
		}
		delete [] pBitmapBuffer ;
		return 0;
	}
	if(devMode.dmBitsPerPel == 32)
	{
		if(bminfoheader.biBitCount == 16)
		{
			pFinalBitmapBuffer = new UINT[bminfoheader.biWidth*bminfoheader.biHeight];
			USHORT* psrc = (USHORT*) pBitmapBuffer;
			UINT*   pDest = (UINT*) pFinalBitmapBuffer;
			for(int y = 0;y < bminfoheader.biHeight;y++)
			{
				for(int x = 0; x < bminfoheader.biWidth; x++)
				{
					USHORT src_color = psrc[x];
					UCHAR r = (((src_color>>11)&63)<<3);
					UCHAR g = (((src_color>>8)&31)<<2);
					UCHAR b = ((src_color&31)<<3);
					int alpha = 255;
					DWORD color = (r<<16) + (g<<8) + b+(alpha<<24);
					pDest[x] = color;
				}
				pDest+=bminfoheader.biWidth;
				psrc += bminfoheader.biWidth;
			}
			delete [] pBitmapBuffer;
			HBITMAP hBitmap = CreateBitmap(bminfoheader.biWidth,bminfoheader.biHeight,1,32,pFinalBitmapBuffer);
			delete [] pFinalBitmapBuffer;
			return hBitmap;

		}
		if(bminfoheader.biBitCount == 24)
		{
			pFinalBitmapBuffer = new UINT[bminfoheader.biWidth*bminfoheader.biHeight];
			if(pFinalBitmapBuffer == 0)
			{
				MessageBox(NULL,"","",MB_OK);
			}
			UCHAR* psrc = (UCHAR*) pBitmapBuffer;
			UINT*   pDest = (UINT*) pFinalBitmapBuffer;
			for(int y = 0;y < bminfoheader.biHeight;y++)
			{
				for(int x = 0; x < bminfoheader.biWidth; x++)
				{
					UCHAR b = psrc[3*x+0];
				    UCHAR g = psrc[3*x+1];
				    UCHAR r = psrc[3*x+2];
					DWORD color = (r<<16) + (g<<8) + b;
					pDest[x] = color;
				}
				pDest+=bminfoheader.biWidth;
				psrc += bminfoheader.biWidth*3;
			}
			delete [] pBitmapBuffer;
			HBITMAP hBitmap = CreateBitmap(bminfoheader.biWidth,bminfoheader.biHeight,1,32,pFinalBitmapBuffer);
			delete [] pFinalBitmapBuffer;
			return hBitmap;

		}
		if(bminfoheader.biBitCount == 32)
		{
			HBITMAP hBitmap = CreateBitmap(bminfoheader.biWidth,bminfoheader.biHeight,1,32,pBitmapBuffer);
 			delete [] pBitmapBuffer;
			pBitmapBuffer = 0;
			return hBitmap;

		}
		delete [] pBitmapBuffer;
		return 0;
	}
	delete [] pBitmapBuffer ;
	return 0;
}
GDILoadBitmap::GDILoadBitmap()
{
	pBitmapBuffer = 0;
}
GDILoadBitmap::~GDILoadBitmap()
{
	if(pBitmapBuffer)
		delete[] pBitmapBuffer;
}
void GDILoadBitmap::GDIBitmapCreate(int width,int height,int colorBpp)
{
	if(colorBpp!=16&&colorBpp!=32)
		return;
	bitmapInfo.bmBitsPixel = colorBpp;
	bitmapInfo.bmWidth = width;
	bitmapInfo.bmHeight = height;
	bitmapInfo.bmWidthBytes = width*(colorBpp>>3);
	bitmapInfo.bmPlanes =1;
	bitmapInfo.bmType  = 0;
	if(colorBpp == 16)
	{
		pBitmapBuffer = new USHORT[width*height];
		memset(pBitmapBuffer,0,(width*height)<<1);
	}
	else
	{
		pBitmapBuffer = new UINT[width*height];
		memset(pBitmapBuffer,0,(width*height)<<2);
	}
}
BOOL GDILoadBitmap::GDIBitmapFromFile(const char* fileName)
{

 	HBITMAP hTempBitmap =LoadBitmapFromFile(fileName);
	GetObject(hTempBitmap,sizeof(bitmapInfo),&bitmapInfo);
	DWORD numPixels = bitmapInfo.bmWidth*bitmapInfo.bmHeight;
	DWORD numBytes = 0;
	if(bitmapInfo.bmBitsPixel == 32)
	{
		numBytes = (numPixels<<2);
		pBitmapBuffer = new UINT[numPixels];
		memset(pBitmapBuffer,0,numPixels<<2);
	}
	else
	if(bitmapInfo.bmBitsPixel == 16)
	{
		numBytes = (numPixels<<1);
		pBitmapBuffer = new USHORT[numPixels];
		memset(pBitmapBuffer,0,numPixels<<1);
	}
	else
	{
		DeleteObject(hTempBitmap);
		return FALSE;
	}
	GetBitmapBits(hTempBitmap,numBytes,pBitmapBuffer);
	DeleteObject(hTempBitmap);
	return TRUE;
}

const BITMAP& GDILoadBitmap::GDIBitmapGetBitmapInfo() const
{
	return bitmapInfo;
}
const LPVOID GDILoadBitmap::GDIBitmapGetBItmapBuffer() const
{
	return pBitmapBuffer;
}
void GDILoadBitmap::GDIBitmapDestroy()
{
	if(pBitmapBuffer)
	{
		delete [] pBitmapBuffer;
		pBitmapBuffer = 0;
	}
	memset(&bitmapInfo,0,sizeof(BITMAP));
}


