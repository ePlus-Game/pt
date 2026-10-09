#ifndef GDI_RENDER_H
#define GDI_RENDER_H
#include "CEGUI.h"
#include "ui/UiCommon.h"
using namespace CEGUI;
#define FONT_SIZE 256
#define FONT_WEIGHT 500
typedef struct TFont{
	char fontName[256];
	bool isInSystem;
	TFont()
	{
		memset(fontName,0,256);
		isInSystem = FALSE;
	}
}TFONT,*LPTFONT;
int CALLBACK  EnumFontProc(const LPLOGFONT pLogFont,const LPTEXTMETRIC pntme,DWORD fontType,LPARAM lParam);
inline HFONT    CreateGDIFont(HDC hdc,const char* font,int fontHeight,bool isDefaultFont )
{
	WCHAR* pWFont = 0;
	ansiToUnicode(font,pWFont);
	HFONT hFont = 0;
	if(isDefaultFont)
	{
		hFont = CreateFontW(ChatString::ChatStringGetString().chatDefualtFontHeight,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,pWFont);
	}
	else
	{
		hFont = CreateFontW(fontHeight,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,ANSI_CHARSET,OUT_TT_ONLY_PRECIS,CLIP_DEFAULT_PRECIS,ANTIALIASED_QUALITY,FF_ROMAN,pWFont);
	}
	SetTextCharacterExtra(hdc,ChatString::ChatStringGetString().fontExtra);
	delete [] pWFont;
	return hFont;
}
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
///////////////////////////////////////////
int  GetLayoutSelectedText(ILayout* pLayOut,char*&pText);
///////////////////////////////////////////
inline BOOL IsInRect(const POINT& pt,const RECT& rc)
{
	if(pt.x < rc.left || pt.x > rc.right)
		return FALSE;
	if(pt.y < rc.top || pt.y >rc.bottom)
		return FALSE;
	return TRUE;
}


////////////////////////////////////////////
LPVOID CopySPR16ToBitmap16(LPVOID pSpr,LPVOID pScreenBuffer,
						   int destTextureX,int destTextureY,
						   int destTextureWidth,int destTextureHeight,
						   int screenX,int screenY,
						   int sprWidth,int sprHeight,int screenWidth,int srceenHeight);
LPVOID CopySPR16ToBitmap32(LPVOID pSpr,LPVOID pScreenBuffer,
						   int destTextureX,int destTextureY,
						   int destTextureWidth,int destTextureHeight,
						   int screenX,int screenY,
						   int sprWidth,int sprHieght,int screenWidth,int screenHeight);
LPVOID CopySPR8ToBitmap32(LPVOID pSpr,
						  LPVOID pScreenBuffer,
						  void* pPalette,
						  int destTextureX,int destTextureY,
						  int destTextureWidth,int destTextureHeight,
						  int screenX,int screenY,
						  int sprWidth,int sprHeight,int screenWidth,int screenHeight);
LPVOID CopySPR8ToBitmap16(LPVOID pSpr,
						  LPVOID pScreenBuffer,
						  void* pPalette,
						  int destTextureX,int destTextureY,
						  int destTextureWidth,int destTextureHeight,
						  int screenX,int screenY,
						  int sprWidth,int sprHeight,int screenWidth,int screenHeight);
void CopSPRCell16ToBitmap32(LPVOID pSpr,
							  LPVOID dest,
							  int cell_x,int cell_y,
							  int spr_width,int spr_height,
							  int destWidth,int destHeight);
void CopSPRCell8ToBitmap32(LPVOID pSpr,
							  LPVOID dest,LPVOID pPalette,
							  int cell_x,int cell_y,
							  int spr_width,int spr_height,
							  int destWidth,int destHeight);
struct GDIClipperInfo
{
	int			x;			// 裁减后的X坐标
	int			y;			// 裁减后的Y坐标
	int			width;		// 裁减后的宽度
	int			height;		// 裁减后的高度
	int			left;		// 上边界裁剪量
	int			top;		// 左边界裁剪量
	int			right;		// 右边界裁剪量
};
int  MakeClip(int nX, int nY, int nSrcWidth, int nSrcHeight, int nDestWidth, int nDestHeight, GDIClipperInfo* pClipper);

typedef class GDIRender :public ILayoutRender
{
public:
	GDIRender();

	const Image*  getCEImage(const unsigned short* imagePath);
	virtual LORect	getWordSize(
		unsigned short codePoint,
		const LOFont& font)	;
	
	virtual int		getLineHeight(
		const LOFont& font)	;
	
	virtual int		getTextExtent(
		const unsigned short* text,
		const LOFont& font);
	
	//得到一个字符串上指定坐标的字符
	virtual int		getCharAtPixel(
		const unsigned short* text, 
		int startCharIndex,  
		int pixel,
		const LOFont& font)	;
	
	virtual void	drawText(
		const unsigned short* text,
		const LOFont& font,
		const LORect& destArea,
		const LOColor& color,
		float alpha,
		const LORect& clipper,
		float zPos,
		int borderMode,
		bool underline);
	
	
	virtual LORect	getImageArea(
		const unsigned short* imageName);
	
	
	virtual LOImageInfo getImageInfo(
		const unsigned short* imageName);	
	//在指定位置画一幅图片
	virtual void	drawImage(
		const unsigned short* imageName, 
		LORect& destArea,
		const LOColor& color, 
		float alpha,
		LOImageInfo& imageInfo, 
		LORect& clipper, 
		float zPos);
	virtual void Release( ){};
	virtual ~GDIRender(){};
public:
	HDC     hCurrentDC;
	int      fontHeight;
	char    font[256];
	bool    isUseDefualtFont;
	static int     wordSize;
}GDIRENDER,*LPGDIRENDER;
#endif