#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <layoutinterface.h>
#include "chatWindow/ChatMainDlg.h"
#include "GDIRender.h"
#include "KColors.h"
#include <string>
#include <vector>
using std::string;
using std::vector;

#define COLOR_COMPLETE_MIXER

int CALLBACK  EnumFontProc(const LPLOGFONT pLogFont,const LPTEXTMETRIC pntme,DWORD fontType,LPARAM lParam)
{

	LPTFONT pTfont = (LPTFONT)lParam;
	if(strcmp(pTfont->fontName,pLogFont->lfFaceName)==0)
	{
		pTfont->isInSystem  = TRUE;
		return 0;
	}
	return 1;
}
int GDIRender::wordSize = 0;	
GDIRender::GDIRender()
{
	hCurrentDC = 0;
	fontHeight = 0;
	isUseDefualtFont = false;
	memset(font,0,256);
}
LORect GDIRender::getWordSize(unsigned short codePoint,
							  const LOFont& font)
{
	if(!hCurrentDC)
		return LORect();
	HFONT hFont = CreateGDIFont(hCurrentDC,GDIRender::font,fontHeight,isUseDefualtFont);
//	SetTextCharacterExtra(hCurrentDC,ChatString::ChatStringGetString().fontExtra);
	HFONT hOldFont = (HFONT)SelectObject(hCurrentDC,hFont);///可以要可以不要，关键看流程///
	LORect rc;
	rc.setPos(0,0);
	unsigned short dText[1] = {codePoint};
	SIZE size;
	GetTextExtentPoint32W(hCurrentDC,dText,1,&size);
	wordSize = size.cy;
	if(abs(wordSize)>100)
		wordSize = 13;
	rc.setWidth(size.cx);
	rc.setHeight(size.cy);

	SelectObject(hCurrentDC,hOldFont);
	DeleteObject(hFont);
	return rc;
}
int GDIRender::getLineHeight(const LOFont& font)
{
	if(hCurrentDC == 0)
		return 0;
	HFONT hFont = CreateGDIFont(hCurrentDC,GDIRender::font,fontHeight,isUseDefualtFont);
	HFONT hOldFont = (HFONT)SelectObject(hCurrentDC,hFont);
	TEXTMETRICW tmw;
	GetTextMetricsW(hCurrentDC,&tmw);
	SelectObject(hCurrentDC,hOldFont);
	DeleteObject(hFont);
	return tmw.tmHeight;
}
int GDIRender::getTextExtent(const unsigned short* text,
							 const LOFont& font)
{
	if(hCurrentDC == 0)
		return 0;
	HFONT hFont = CreateGDIFont(hCurrentDC,GDIRender::font,fontHeight,isUseDefualtFont);
	HFONT hOldFont = (HFONT)SelectObject(hCurrentDC,hFont);///可以要可以不要，关键看流程///
	SIZE  size;
	size.cx=size.cy =0;
	GetTextExtentPoint32W(hCurrentDC,text,wcslen(text),&size);
	SelectObject(hCurrentDC,hOldFont);
	DeleteObject(hFont);
	return size.cx;
}

int GDIRender::getCharAtPixel(const unsigned short* text, 
							  int startCharIndex, int pixel, 
							  const LOFont& font)
{
	int length =  wcslen(text);
	if(startCharIndex >= length)
		return length;
	LORect rc;
	int pixelLength = 0;
	for(int index = startCharIndex;index < length;index ++)
	{
		rc = getWordSize(text[index],font);
//		rc.setWidth(rc.getWidth()+2);
		if((pixel-pixelLength)>=rc.getLeft()&&pixel-pixelLength<=rc.getRight())
		{
			return index;
		}
		pixelLength+=rc.getWidth();
	}
	return length;
}

void GDIRender::drawText(const unsigned short* text, 
						 const LOFont& font, 
						 const LORect& destArea, 
						 const LOColor& color, 
						 float alpha, 
						 const LORect& clipper, 
						 float zPos, 
						 int borderMode,
						 bool underLine)
{
	if(hCurrentDC == 0)
		return ;
	HFONT hFont = CreateGDIFont(hCurrentDC,GDIRender::font,fontHeight,isUseDefualtFont);
	HFONT hOldFont = (HFONT)SelectObject(hCurrentDC,hFont);
	RECT rc;

	rc.left  = clipper.getLeft();
	rc.right = clipper.getRight();
	rc.top   = clipper.getTop();
	rc.bottom = clipper.getBottom();
	int length = wcslen(text);
	WCHAR* temp = new WCHAR[length+1];
	int lenTemp = 0;
	int idxTemp =0;
	for(int idx = 0; idx < length;idx++)
	{
		if(text[idx] !=10)
		{
			temp[idxTemp] = text[idx];
			idxTemp++;
			lenTemp++;
		}
	}
	temp[lenTemp] = 0;
	COLORREF oldColor = GetTextColor(hCurrentDC);
	SetBkMode(hCurrentDC,TRANSPARENT);
	SetTextColor(hCurrentDC,RGB(color.red*255,color.green*255,color.blue*255));
	ExtTextOutW(hCurrentDC,destArea.getLeft(),destArea.getTop(),ETO_CLIPPED,&rc,temp,lenTemp,NULL);

	SelectObject(hCurrentDC,hOldFont);
	delete [] temp;
	temp = 0;
	DeleteObject(hFont);
}

LORect	GDIRender::getImageArea(const unsigned short* imageName)
{
	const Image* ceImage = getCEImage(imageName);
	LORect retRect;
	if(ceImage != NULL)
	{
		int height	= ceImage->getHeight();
		int width	= ceImage->getWidth();
		retRect.setHeight(height);
		retRect.setWidth(width);
	}
	return retRect;
}


const Image* GDIRender::getCEImage(const unsigned short* imagePath)
{
	wchar_t imageSet[128] = {0};
	wchar_t imageName[128] = {0};
	
	swscanf(imagePath, L" set:%127s image:%127s", imageSet, imageName);
	
	const Image* image;
	
	try
	{
		if(0 != imageSet[0])
		{
			image = &ImagesetManager::getSingleton().getImageset(imageSet)->getImage(imageName);
		}
		else
		{
			if(ImagesetManager::getSingleton().isImagesetPresent(imagePath) == true)
			{
				image = &ImagesetManager::getSingleton().getImageset(imagePath)->getImage("full_image");
			}
			else
			{
				image = &ImagesetManager::getSingleton().createImagesetFromImageFile(imagePath, imagePath)->getImage("full_image");
			}			
		}
	}
	catch (UnknownObjectException)
	{
		image = NULL;
	}
	
	return image;
}

LOImageInfo GDIRender::getImageInfo(const unsigned short* imageName)
{
	const Image* ceImage = getCEImage(imageName);
	
	LOImageInfo image;
	if(NULL == ceImage)
	{
		return image;
	}
	
	String imgName = ceImage->getImagesetName();
	Imageset* is = ImagesetManager::getSingleton().getImageset( imgName );
	if(NULL == is)
	{
		return image;
	}
	
	KSprite* spr = is->getSpr();
	
	if(NULL == spr)
	{
		return image;
	}
	image.frameCount = is->getSpr()->GetFrames();
	image.interval = is->getSpr()->GetInterval();
	return image;
}
void	GDIRender::drawImage(const unsigned short* imageName, 
							   LORect& destArea,
							   const LOColor& color, 
							   float alpha,
							   LOImageInfo& imageInfo, 
							   LORect& clipper, 
							   float zPos)	
{
	const Image* ceImage = getCEImage(imageName);
	if(NULL == ceImage)
		return;
	
	CEGUI::Point destPos(destArea.getLeft(), destArea.getTop());
	String imgName = ceImage->getImagesetName();
	Imageset* is = ImagesetManager::getSingleton().getImageset( imgName );
	if(NULL == is)
	{
		return;
	}
	
	KSprite* spr = is->getSpr();	
	if(NULL == spr)
	{
		return ;
	}
	Rect clipperRect(clipper.getLeft(), clipper.getTop(), clipper.getRight(), clipper.getBottom());
	
	int curFrameCount = 0;
	if(imageInfo.frameCount > 1)
	{
		curFrameCount = ((float)(::GetTickCount() - imageInfo.lastDrawTime)) / (float)imageInfo.interval;
		if(curFrameCount >= imageInfo.frameCount)
		{
			curFrameCount = 0;
			imageInfo.lastDrawTime = ::GetTickCount();
		}
	}
	SPRFRAME* sprFame = spr->GetFrameInfo(spr->GetFrames()-1);
	if(sprFame == 0)
		return;
	int width = ceImage->getWidth();
	int height = ceImage->getHeight();
	Rect rc = ceImage->getSourceTextureArea();
	LPVOID pBitmapBuffer = 0;//CopySPR16ToBitmap16(sprFame->Sprite,width*height);
	HBITMAP hBitmap = 0;//CreateBitmap(width,height,1,16,pBitmapBuffer);
	LPVOID pDestBitmapBuffer = 0;
	BITMAP bm;
	ZeroMemory(&bm,sizeof(BITMAP));
	HBITMAP hDestBitmap = (HBITMAP)GetCurrentObject(hCurrentDC,OBJ_BITMAP);	
	if(GetObject(hDestBitmap,sizeof(BITMAP),&bm) == 0)
		return;
	DWORD size = bm.bmWidth*bm.bmHeight;
//	int bytesPerPixel = bm.bmBitsPixel>>3;
//	DWORD destBytes = (size<<(bm.bmBitsPixel>>3)
//	pDestBitmapBuffer = new UCHAR[size<<1];
//	GetBitmapBits(hDestBitmap,size<<1,pDestBitmapBuffer);
//	CopySPT8ToBitmap32(LPVOID pSpr,
//							  LPVOID pScreenBuffer,
//							  void* pPalette,
//							  int destTextureX,int destTextureY,
//							  int destTextureWidth,int destTextureHeight,
//							  int screenX,int screenY,
//							  int sprWidth,int screenWidth,int screenHeight);
	
	int destTextureX = rc.d_left,
		destTextureY = rc.d_top,
		destTextureWidth = width,
		destTextureHeight = height,
		screenX =destArea.getLeft(),
		screenY = destArea.getTop();
	if(screenX+width<=0||screenX>=bm.bmWidth||
		screenY+height<=0||screenY>=bm.bmHeight)
		return;
/*	if(screenX<0)
	{
		screenX =0;
		destTextureX+=(-screenX);
		destTextureWidth-=(-screenX);
	}
	else
	if(screenX+width>bm.bmWidth)
	{

		destTextureWidth-=(screenX+width-bm.bmWidth);
	}

	if(screenY<0)
	{
		screenY = 0;
		destTextureY +=(-screenY);
		destTextureHeight -=(-screenY);
	}
	else
	if(screenY+height>bm.bmHeight)
	{
		destTextureHeight -=(screenY+height-bm.bmHeight);
	}*/



	if(bm.bmBitsPixel == 16)
	{
		pDestBitmapBuffer = new UCHAR[size<<1];
		GetBitmapBits(hDestBitmap,size<<1,pDestBitmapBuffer);
		if(spr->Is16Bit())
		{
			pBitmapBuffer = CopySPR16ToBitmap16(sprFame->Sprite,
											   pDestBitmapBuffer,
											   destTextureX,destTextureY,
											   destTextureWidth,destTextureHeight,
											   screenX,screenY,
											   sprFame->Width,sprFame->Height,
											   bm.bmWidth,bm.bmHeight);
		}
		else
		{
			pBitmapBuffer = CopySPR8ToBitmap16(sprFame->Sprite,
											  pDestBitmapBuffer,
											  spr->Get24Palette(),
											  destTextureX,destTextureY,
											   destTextureWidth,destTextureHeight,
											   screenX,screenY,
											  sprFame->Width,sprFame->Height,
											  bm.bmWidth,bm.bmHeight);
			
		}
		hBitmap = CreateBitmap(width,height,1,16,pBitmapBuffer);
	}
	else
	if(bm.bmBitsPixel == 32)
	{
		pDestBitmapBuffer = new UCHAR[size<<2];
		GetBitmapBits(hDestBitmap,size<<2,pDestBitmapBuffer);
		if(spr->Is16Bit())
		{
			pBitmapBuffer = CopySPR16ToBitmap32(sprFame->Sprite,
											   pDestBitmapBuffer,
											   destTextureX,destTextureY,
											   destTextureWidth,destTextureHeight,
											   screenX,screenY,
											   sprFame->Width,sprFame->Height,
											   bm.bmWidth,bm.bmHeight);
		}
		else
		{
			pBitmapBuffer = CopySPR8ToBitmap32(sprFame->Sprite,
											  pDestBitmapBuffer,
											  spr->Get24Palette(),
											  destTextureX,destTextureY,
											   destTextureWidth,destTextureHeight,
											   screenX,screenY,
											  sprFame->Width,sprFame->Height,
											  bm.bmWidth,bm.bmHeight);
			
		}
		hBitmap = CreateBitmap(width,height,1,32,pBitmapBuffer);
	}
	else
	{
		DeleteObject(hBitmap);
		return;
	}
	DrawBitmap(hCurrentDC,hBitmap,width,height,
		       destArea.getLeft(), destArea.getTop());
	delete [] pBitmapBuffer;
	pBitmapBuffer = 0;
	DeleteObject(hBitmap);
	delete [] pDestBitmapBuffer;
}

LPVOID CopySPR8ToBitmap32(LPVOID pSpr,
						  LPVOID pScreenBuffer,
						  void* pPalette,
						  int destTextureX,int destTextureY,
						  int destTextureWidth,int destTextureHeight,
						  int screenX,int screenY,
						  int sprWidth,int sprHeight,int screenWidth,int screenHeight)
{
	//单帧的
	if(screenX+screenWidth<0||screenX>=screenWidth-1||
		screenY+screenHeight<0||screenY>=screenHeight-1)
		return NULL;
	DWORD numberPixels = destTextureHeight*destTextureWidth;
	UINT* pBitmap32 = new UINT[numberPixels];
	memset(pBitmap32,0,numberPixels<<2);
	int index_y = 0,index_x = 0;
	int screenIndex_x = screenX,screenIndex_y = screenY;
	int top =0,left = 0,right = 0,down =0;
	if(screenX < 0)
	{
		left = -screenX;
		screenIndex_x = 0;
	}
	else
	if(screenX>=screenWidth)
	{
		right = screenX - screenWidth+1;
		screenIndex_x = screenWidth -1;
	}
	if(screenY<0)
	{
		top = -screenY;
		screenIndex_y = 0;
	}
	else
	if(screenY>=screenHeight)
	{
		down = screenY - screenHeight+1;
		
		screenIndex_y = screenHeight -1;
	}
		
	UINT* pDest = (UINT*)pScreenBuffer + screenIndex_x+screenIndex_y*screenWidth;
	UINT* pBitmap = pBitmap32;
	int clipperW = destTextureWidth - left - right;
	int clipperH = destTextureHeight - top - down;
	UINT* pTemp = pBitmap+ left+top*destTextureWidth;
	UINT* pDestTemp = pDest;
	for(int j = 0; j < clipperH ; j++)
	{
		memcpy(pTemp,pDestTemp,clipperW<<2);
		pTemp+=destTextureWidth;
		pDestTemp+=screenWidth;
	}
	UCHAR* pSprBuffer = (UCHAR*)pSpr;

	UINT* srcBitmap32Start = new UINT[numberPixels];
	UINT* srcBitmap32 = srcBitmap32Start;
	ZeroMemory(srcBitmap32,numberPixels<<2);
	CopSPRCell8ToBitmap32(pSpr,srcBitmap32,pPalette,destTextureX,destTextureY,sprWidth,sprHeight,destTextureWidth,destTextureHeight);
	for(index_y = 0;index_y<destTextureHeight;index_y++)
	{
		for(index_x = 0;index_x<destTextureWidth;++index_x)
		{
			UINT color = *srcBitmap32;
			UCHAR alpha = (color>>24);
			if(alpha == 255)
			{
				*pBitmap = color;
			}
			else
			if(alpha>0)
			{
				float alphaf = (float)alpha/255.0f;
				float src_r = (float)((color>>16)&0xff);
				float src_g = (float)((color>>8)&0xff);
				float src_b = (float)((color)&0xff);
				//////////////////////////
				unsigned short destColor = *pBitmap;
				float dest_r = (float)((destColor>>16)&0xff);
				float dest_g =  (float)(((destColor>>8)&0xff));
				float dest_b = (float)((destColor&0xff));
				float rf = alphaf*src_r + (1.0f - alphaf)*dest_r;
				float gf = alphaf*src_g + (1.0f - alphaf)*dest_g;
				float bf = alphaf*src_b + (1.0f - alphaf)*dest_b;
				UCHAR r =(UCHAR)rf;
				UCHAR g = (UCHAR)gf;
				UCHAR b = (UCHAR)bf;
				*pBitmap  = ((r<<16)|(g<<8)|(b)); 
			}
			pBitmap++;
			srcBitmap32++;

		}
	}
	delete [] srcBitmap32Start;
/*	KPAL24 * pPalette24 = (KPAL24*)pPalette;
	
	int pixelCount = 0;
	int pixelAlpha = 0;
	int currentPos = 0;
	int clipperWidth = sprWidth - left - right;
	int clipperHeight = sprHeight - top - down;
	int screencount = 0;
	pBitmap += left+top*destTextureWidth;
	int bitmapCount = 0;
	for(index_y = 0;index_y<clipperHeight;index_y++)
	{
		currentPos = 0;
		screencount = 0;
		bitmapCount = 0;
		while(currentPos<left)
		{
			pixelCount = *pSprBuffer;
			pSprBuffer++;
			pixelAlpha = *pSprBuffer;
			pSprBuffer++;
			currentPos+= pixelCount;
			if(pixelAlpha>0)
			{
				pSprBuffer+=pixelCount;
			}
		}
		while(currentPos<left+clipperWidth)
		{
			pixelCount = *pSprBuffer;
			pSprBuffer++;
			pixelAlpha = *pSprBuffer;
			pSprBuffer++;
			currentPos+=pixelCount;
			if(pixelAlpha>0)
			{
				//绘制了
				float alpha_f = (float)pixelAlpha/255.0f;
				for(int i = 0; i < pixelCount ; i++)
				{
					UCHAR paleIdx = *pSprBuffer;
					float red = (float)pPalette24[paleIdx].Red;
					float green = (float)pPalette24[paleIdx].Green;
					float blue = (float)pPalette24[paleIdx].Blue;
					DWORD destColor = pDest[screencount];
					float dest_red = (float)(destColor>>16);
					float dest_green = (float)((destColor>>8)&0xff);
					float dest_blue = (float)(destColor&0xff);
					red = red*alpha_f+dest_red*(1.0f-alpha_f);
					green = green*alpha_f + dest_green*(1.0f - alpha_f);
					blue = blue*alpha_f + dest_blue*(1.0f - alpha_f);
					UCHAR r = (UCHAR)red;
					UCHAR g = (UCHAR)green;
					UCHAR b = (UCHAR)blue;
					pBitmap[bitmapCount] = (0xff<<24)|(r<<16)|(g<<8)|(b);
					bitmapCount++;
					screencount++;
					pSprBuffer++;
				}
			}
			else
			{
				for(int i = 0; i < pixelCount;i++)
				{
					pBitmap[bitmapCount] = pDest[screencount];
					bitmapCount++;
					screencount++;
				}


			}
		}
		while (currentPos<left+clipperWidth+right)
		{
			pixelCount = *pSprBuffer;
			pSprBuffer++;
			pixelAlpha = *pSprBuffer;
			pSprBuffer++;
			currentPos+= pixelCount;
			if(pixelAlpha>0)
			{
				pSprBuffer+=pixelCount;
			}
		}
		pBitmap+=destTextureWidth;
		pDest+=screenWidth;

	}*/
	return pBitmap32;

   
}
///////////////////////////////////////////
LPVOID CopySPR16ToBitmap32(LPVOID pSpr,LPVOID pScreenBuffer,
						   int destTextureX,int destTextureY,
						   int destTextureWidth,int destTextureHeight,
						   int screenX,int screenY,
						   int sprWidth,int sprHeight,
						   int screenWidth,int screenHeight)
{
	if(screenX+screenWidth<0||screenX>=screenWidth-1||
		screenY+screenHeight<0||screenY>=screenHeight-1)
		return NULL;
	DWORD numberPixels = destTextureHeight*destTextureWidth;
	UINT* pBitmap32 = new UINT[numberPixels];
	memset(pBitmap32,0,numberPixels<<2);
	int index_y = 0,index_x = 0;
	int screenIndex_x = screenX,screenIndex_y = screenY;
	int top =0,left = 0,right = 0,down =0;
	if(screenX < 0)
	{
		left = -screenX;
		screenIndex_x = 0;
	}
	else
	if(screenX>=screenWidth)
	{
		right = screenX - screenWidth+1;
		screenIndex_x = screenWidth -1;
	}
	if(screenY<0)
	{
		top = -screenY;
		screenIndex_y = 0;
	}
	else
	if(screenY>=screenHeight)
	{
		down = screenY - screenHeight+1;
		
		screenIndex_y = screenHeight -1;
	}
		
	UINT* pDest = (UINT*)pScreenBuffer + screenIndex_x+screenIndex_y*screenWidth;
	UINT* pBitmap = pBitmap32;
	int clipperW = destTextureWidth - left - right;
	int clipperH = destTextureHeight - top - down;
	UINT* pTemp = pBitmap+ left+top*destTextureWidth;
	UINT* pDestTemp = pDest;
	for(int j = 0; j < clipperH ; j++)
	{
		memcpy(pTemp,pDestTemp,clipperW<<2);
		pTemp+=destTextureWidth;
		pDestTemp+=screenWidth;
	}
	UINT* srcBitmap32Start = new UINT[numberPixels];
	UINT* srcBitmap32 = srcBitmap32Start;
	ZeroMemory(srcBitmap32,numberPixels<<2);
	CopSPRCell16ToBitmap32(pSpr,srcBitmap32,destTextureX,destTextureY,sprWidth,sprHeight,destTextureWidth,destTextureHeight);
	for(index_y = 0;index_y<destTextureHeight;index_y++)
	{
		for(index_x = 0;index_x<destTextureWidth;++index_x)
		{
			UINT color = *srcBitmap32;
			UCHAR alpha = (color>>24);
			if(alpha == 255)
			{
				*pBitmap = color;
			}
			else
			if(alpha>0)
			{
				float alphaf = (float)alpha/255.0f;
				float src_r = (float)((color>>16)&0xff);
				float src_g = (float)((color>>8)&0xff);
				float src_b = (float)((color)&0xff);
				//////////////////////////
				unsigned short destColor = *pBitmap;
				float dest_r = (float)((destColor>>16)&0xff);
				float dest_g =  (float)(((destColor>>8)&0xff));
				float dest_b = (float)((destColor&0xff));
				float rf = alphaf*src_r + (1.0f - alphaf)*dest_r;
				float gf = alphaf*src_g + (1.0f - alphaf)*dest_g;
				float bf = alphaf*src_b + (1.0f - alphaf)*dest_b;
				UCHAR r =(UCHAR)rf;
				UCHAR g = (UCHAR)gf;
				UCHAR b = (UCHAR)bf;
				*pBitmap  = ((r<<16)|(g<<8)|(b)); 
			}
			pBitmap++;
			srcBitmap32++;

		}
	}
	delete [] srcBitmap32Start;
	return pBitmap32;
}
////////////////////////////////////////////////////////////
LPVOID CopySPR16ToBitmap16(LPVOID pSpr,LPVOID pScreenBuffer,
						   int destTextureX,int destTextureY,
						   int destTextureWidth,int destTextureHeight,
						   int screenX,int screenY,
						   int sprWidth,int sprHeight,
						   int screenWidth,int screenHeight)
{
	if(screenX+screenWidth<0||screenX>=screenWidth-1||
		screenY+screenHeight<0||screenY>=screenHeight-1)
		return NULL;
	DWORD numberPixels = destTextureHeight*destTextureWidth;
	USHORT* pBitmap32 = new USHORT[numberPixels];
	memset(pBitmap32,0,numberPixels<<1);
	int index_y = 0,index_x = 0;
	int screenIndex_x = screenX,
		screenIndex_y = screenY;
	int top =0,left = 0,right = 0,down =0;
	if(screenX < 0)
	{
		left = -screenX;
		screenIndex_x = 0;
	}
	else
	if(screenX>=screenWidth)
	{
		right = screenX - screenWidth+1;
		screenIndex_x = screenWidth -1;
	}
	if(screenY<0)
	{
		top = -screenY;
		screenIndex_y = 0;
	}
	else
	if(screenY>=screenHeight)
	{
		down = screenY - screenHeight+1;
		
		screenIndex_y = screenHeight -1;
	}
		
	USHORT* pDest = (USHORT*)pScreenBuffer + screenIndex_x+screenIndex_y*screenWidth;
	USHORT* pBitmap = pBitmap32;
	int clipperW = destTextureWidth - left - right;
	int clipperH = destTextureHeight - top - down;
	USHORT* pTemp = pBitmap+ left+top*destTextureWidth;
	USHORT* pDestTemp = pDest;
	for(int j = 0; j < clipperH ; j++)
	{
		memcpy(pTemp,pDestTemp,clipperW<<1);
		pTemp+=destTextureWidth;
		pDestTemp+=screenWidth;
	}
	UINT* srcBitmap32Start = new UINT[numberPixels];
	UINT* srcBitmap32 = srcBitmap32Start;
	ZeroMemory(srcBitmap32,numberPixels<<2);
	CopSPRCell16ToBitmap32(pSpr,srcBitmap32,destTextureX,destTextureY,sprWidth,sprHeight,destTextureWidth,destTextureHeight);
	for(index_y = 0;index_y<destTextureHeight;index_y++)
	{
		for(index_x = 0;index_x<destTextureWidth;++index_x)
		{
			UINT color = *srcBitmap32;
			UCHAR alpha = (color>>24);
			if(alpha == 255)
			{
				UCHAR red = (((color>>16)&0xff)>>3);
				UCHAR green = (((color>>8)&0xff)>>2);
				UCHAR blue = ((color&0xff)>>3);
				*pBitmap = ((red<<11)|(green<<5)|(blue));
			}
			else
			if(alpha>0)
			{
				float alphaf = (float)alpha/255.0f;
				float src_r = (float)((color>>16)&0xff);
				float src_g = (float)((color>>8)&0xff);
				float src_b = (float)((color)&0xff);
				//////////////////////////
				unsigned short destColor = *pBitmap;
				float dest_r = (float)((destColor>>11)<<3);
				float dest_g =  (float)(((destColor>>5)&63)<<2);
				float dest_b = (float)((destColor&31)<<3);
				float rf = alphaf*src_r + (1.0f - alphaf)*dest_r;
				float gf = alphaf*src_g + (1.0f - alphaf)*dest_g;
				float bf = alphaf*src_b + (1.0f - alphaf)*dest_b;
				UCHAR r =(UCHAR)rf;
				UCHAR g = (UCHAR)gf;
				UCHAR b = (UCHAR)bf;
				r>>=3;g>>=2;b>>=3;
				*pBitmap  = (r<<11)+(g<<5)+b; 
			}
			pBitmap++;
			srcBitmap32++;

		}
	}
	delete [] srcBitmap32Start;
	return pBitmap32;
}

LPVOID CopySPR8ToBitmap16(LPVOID pSpr,
						  LPVOID pScreenBuffer,
						  void* pPalette,
						  int destTextureX,int destTextureY,
						  int destTextureWidth,int destTextureHeight,
						  int screenX,int screenY,
						  int sprWidth,int sprHeight,
						  int screenWidth,int screenHeight)
{
	if(screenX+screenWidth<0||screenX>=screenWidth-1||
		screenY+screenHeight<0||screenY>=screenHeight-1)
		return NULL;
	DWORD numberPixels = destTextureHeight*destTextureWidth;
	USHORT* pBitmap32 = new USHORT[numberPixels];
	memset(pBitmap32,0,numberPixels<<1);
	int index_y = 0,index_x = 0;
	int screenIndex_x = screenX,screenIndex_y = screenY;
	int top =0,left = 0,right = 0,down =0;
	if(screenX < 0)
	{
		left = -screenX;
		screenIndex_x = 0;
	}
	else
	if(screenX>=screenWidth)
	{
		right = screenX - screenWidth+1;
		screenIndex_x = screenWidth -1;
	}
	if(screenY<0)
	{
		top = -screenY;
		screenIndex_y = 0;
	}
	else
	if(screenY>=screenHeight)
	{
		down = screenY - screenHeight+1;
		
		screenIndex_y = screenHeight -1;
	}
		
	USHORT* pDest = (USHORT*)pScreenBuffer + screenIndex_x+screenIndex_y*screenWidth;
	USHORT* pBitmap = pBitmap32;
	int clipperW = destTextureWidth - left - right;
	int clipperH = destTextureHeight - top - down;
	USHORT* pTemp = pBitmap+ left+top*destTextureWidth;
	USHORT* pDestTemp = pDest;
	for(int j = 0; j < clipperH ; j++)
	{
		memcpy(pTemp,pDestTemp,clipperW<<1);
		pTemp+=destTextureWidth;
		pDestTemp+=screenWidth;
	}

	UINT* srcBitmap32Start = new UINT[numberPixels];
	UINT* srcBitmap32 = srcBitmap32Start;
	ZeroMemory(srcBitmap32,numberPixels<<2);
	CopSPRCell8ToBitmap32(pSpr,srcBitmap32,pPalette,destTextureX,destTextureY,sprWidth,sprHeight,destTextureWidth,destTextureHeight);
	for(index_y = 0;index_y<destTextureHeight;index_y++)
	{
		for(index_x = 0;index_x<destTextureWidth;++index_x)
		{
			UINT color = *srcBitmap32;
			UCHAR alpha = (color>>24);
			if(alpha == 255)
			{
				UCHAR red = (((color>>16)&0xff)>>3);
				UCHAR green = (((color>>8)&0xff)>>2);
				UCHAR blue = ((color&0xff)>>3);
				*pBitmap = ((red<<11)|(green<<5)|(blue));
			}
			else
			if(alpha>0)
			{
				float alphaf = (float)alpha/255.0f;
				float src_r = (float)((color>>16)&0xff);
				float src_g = (float)((color>>8)&0xff);
				float src_b = (float)((color)&0xff);
				//////////////////////////
				unsigned short destColor = *pBitmap;
				float dest_r = (float)((destColor>>11)<<3);
				float dest_g =  (float)(((destColor>>5)&63)<<2);
				float dest_b = (float)((destColor&31)<<3);
				float rf = alphaf*src_r + (1.0f - alphaf)*dest_r;
				float gf = alphaf*src_g + (1.0f - alphaf)*dest_g;
				float bf = alphaf*src_b + (1.0f - alphaf)*dest_b;
				UCHAR r =(UCHAR)rf;
				UCHAR g = (UCHAR)gf;
				UCHAR b = (UCHAR)bf;
				r>>=3;g>>=2;b>>=3;
				*pBitmap  = (r<<11)+(g<<5)+b; 
			}
			pBitmap++;
			srcBitmap32++;

		}
	}
	delete [] srcBitmap32Start;
	/*
	UCHAR* pSprBuffer = (UCHAR*)pSpr;

	KPAL24 * pPalette24 = (KPAL24*)pPalette;
	
	int pixelCount = 0;
	int pixelAlpha = 0;
	int currentPos = 0;
	int clipperWidth = sprWidth - left - right;
	int clipperHeight = sprHeight - top - down;
	int screencount = 0;
	pBitmap += left+top*destTextureWidth;
	int bitmapCount = 0;
	for(index_y = 0;index_y<clipperHeight;index_y++)
	{
		currentPos = 0;
		screencount = 0;
		bitmapCount = 0;
		while(currentPos<left)
		{
			pixelCount = *pSprBuffer;
			pSprBuffer++;
			pixelAlpha = *pSprBuffer;
			pSprBuffer++;
			currentPos+= pixelCount;
			if(pixelAlpha>0)
			{
				pSprBuffer+=pixelCount;
			}
		}
		while(currentPos<left+clipperWidth)
		{
			pixelCount = *pSprBuffer;
			pSprBuffer++;
			pixelAlpha = *pSprBuffer;
			pSprBuffer++;
			currentPos+=pixelCount;
			if(pixelAlpha>0)
			{
				//绘制了
				float alpha_f = (float)pixelAlpha/255.0f;
				for(int i = 0; i < pixelCount ; i++)
				{
					UCHAR paleIdx = *pSprBuffer;
					float red = (float)pPalette24[paleIdx].Red;
					float green = (float)pPalette24[paleIdx].Green;
					float blue = (float)pPalette24[paleIdx].Blue;
					USHORT destColor = pDest[screencount];
					float dest_red = (float)((destColor>>11)<<3);
					float dest_green = (float)(((destColor>>5)&63)<<2);
					float dest_blue = (float)(destColor&31);
					red = red*alpha_f+dest_red*(1.0f-alpha_f);
					green = green*alpha_f + dest_green*(1.0f - alpha_f);
					blue = blue*alpha_f + dest_blue*(1.0f - alpha_f);
					UCHAR r = (UCHAR)red;
					UCHAR g = (UCHAR)green;
					UCHAR b = (UCHAR)blue;
					r>>=3;g>>=2;b>>=3;
					pBitmap[bitmapCount] = (r<<11)|(g<<5)|(b);
					bitmapCount++;
					screencount++;
					pSprBuffer++;
				}
			}
			else
			{
				for(int i = 0; i < pixelCount;i++)
				{
					pBitmap[bitmapCount] = pDest[screencount];
					bitmapCount++;
					screencount++;
				}


			}
		}
		while (currentPos<left+clipperWidth+right)
		{
			pixelCount = *pSprBuffer;
			pSprBuffer++;
			pixelAlpha = *pSprBuffer;
			pSprBuffer++;
			currentPos+= pixelCount;
			if(pixelAlpha>0)
			{
				pSprBuffer+=pixelCount;
			}
		}
		pBitmap+=destTextureWidth;
		pDest+=screenWidth;

	}*/
	return pBitmap32;
}


int GetLayoutSelectedText(ILayout* pLayOut,char*&pText)
{
//	static wchar_t copyText[COMMON_CLIENT_MSG_LEN_512];
	vector<wchar_t> copyString;
//	copyText[0] = 0;


	int selStart = 0;
	int selEnd = 0;
	pLayOut->getSelection(selStart, selEnd);
	if(selStart == selEnd)
		return 0;
	if(selStart > selEnd)
	{
		int temp = selStart;
		selStart = selEnd;
		selEnd = temp;
	}
				
	LOElemInfo* elemList = NULL;
	int elemCount = pLayOut->getElemList(elemList);
	int curIndex = 0;
	wchar_t mySubText[LAYOUT_TEXT_MAX_LEN] = {0};
	for(int i = 0; i < elemCount; ++i)
	{
		int elemStart = 0;
		int elemEnd = 0;
		const wchar_t* subText = NULL;
		mySubText[0] = 0;
		if(elemList[i].isShowDes)
		{
			if(curIndex >= selStart && curIndex < selEnd)
			{
				elemStart = 0;
				elemEnd = elemList[i].description.len();
				subText = elemList[i].description.get();
				wcscpy(mySubText,subText);
			}
			++curIndex;
		}
		else if(elemList[i].elemType == LO_IMAGE)
		{
			if(curIndex >= selStart && curIndex < selEnd)
			{
				memset(mySubText,0,LAYOUT_TEXT_MAX_LEN);
				elemStart = 0;
				elemEnd = elemList[i].content.len();
				subText = elemList[i].content.get();
				if(wcslen(subText)>LAYOUT_TEXT_MAX_LEN-500)
					return 0;
				wcscpy(mySubText,L"<face>");
				wcscat(mySubText,L"<id>");
				wchar_t temp[256] = {0};
				wsprintfW(temp,L"%d",elemList[i].gameObj._objId[0]);
				wcscat(mySubText,temp);
				wcscat(mySubText,L"</id></face>");
				elemEnd = wcslen(mySubText);
			}
			++curIndex;
		}
		else
		{
			int elemWordCount = elemList[i].content.len();
			elemEnd = elemWordCount;
		    if(selStart >= curIndex && selStart < curIndex + elemWordCount)
			{
				elemStart = selStart - curIndex;
			}

			if(selEnd > curIndex && selEnd <= curIndex + elemWordCount )
			{
				elemEnd = selEnd - curIndex;
			}

			if(selEnd <= curIndex || selStart >= curIndex + elemWordCount)
			{
				elemStart = 0;
				elemEnd = 0;
			}
			subText = elemList[i].content.get();
			curIndex += elemWordCount;
			wcscpy(mySubText,subText);
		}
		for(int j = elemStart; j < elemEnd; ++j)
		{
			copyString.push_back(mySubText[j]);
		}
	}
	delete[] elemList;
	elemList = NULL;
	int size = copyString.size();
	wchar_t*  pWText = new wchar_t[size+1];
	for(int k = 0; k < size;k++ )
		pWText[k] = copyString[k];
	pWText[size] = 0;
	int len = unicodeToAnsi(pWText,pText);
	delete [] pWText;
	return len;
}
int  MakeClip(int nX, int nY, int nSrcWidth, int nSrcHeight, int nDestWidth, int nDestHeight, GDIClipperInfo* pClipper)
{
	// 初始化裁减量
	pClipper->x = nX;
	pClipper->y = nY;
	pClipper->width = nSrcWidth;
	pClipper->height = nSrcHeight;
	pClipper->top = 0;
	pClipper->left = 0;
	pClipper->right = 0;

	// 上边界裁减
	if (pClipper->y < 0)
	{
		pClipper->y = 0;
		pClipper->top = -nY;
		pClipper->height += nY;
	}
	if (pClipper->height <= 0)
		return 0;
	
	// 下边界裁减
	if (pClipper->height > nDestHeight - pClipper->y)
		pClipper->height = nDestHeight - pClipper->y;
	if (pClipper->height <= 0)
		return 0;

	// 左边界裁减
	if (pClipper->x < 0)
	{
		pClipper->x = 0;
		pClipper->left = -nX;
		pClipper->width += nX;
	}
	if (pClipper->width <= 0)
		return 0;

	// 右边界裁减
	if (pClipper->width > nDestWidth - pClipper->x)
	{
		pClipper->right = pClipper->width + pClipper->x - nDestWidth;
		pClipper->width -= pClipper->right;
	}
	if (pClipper->width <= 0)
		return 0;
	
	return 1;
}
void CopSPRCell16ToBitmap32(LPVOID pSpr,
							  LPVOID dest,
							  int cell_x,int cell_y,
							  int spr_width,int spr_height,
							  int destWidth,int destHeight)
{
	
	GDIClipperInfo clipper;
	if(MakeClip(-cell_x,-cell_y,spr_width,spr_height,destWidth,destHeight,&clipper)==0)
		return;
	unsigned char* pSrcBuffer = (unsigned char* )pSpr;
	unsigned int* pDest  = (unsigned int* ) dest;
	pDest += clipper.y*destWidth+clipper.x;

	int topClipPixels = clipper.top*spr_width;
	int pixelCount = 0;
	int pixelAlpha = 0;
	//先定到要绘制区域的y//
	while(topClipPixels>0)
	{
		pixelCount = *pSrcBuffer;
		pSrcBuffer++;
		pixelAlpha = *pSrcBuffer;
		pSrcBuffer++;
		topClipPixels-=pixelCount;
		if(pixelAlpha>0)
			pSrcBuffer+=(pixelCount<<1);
	}
	///接下来就一步一步绘制了
	int copyHeight = clipper.height;
	for(int i = 0 ; i < copyHeight;i++)
	{
		unsigned int* pDestStartLine  = (unsigned int*)pDest;
		int currentPos = 0;
		int targetPos = clipper.left;
		//先定位到开始绘制的位置/
		while(currentPos<targetPos)
		{
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			int destAlpha = (255 - pixelAlpha);
			currentPos+=pixelCount;
			int loopCount = currentPos  - targetPos;
			if(loopCount>0)
			{
				//到达开始位置时候，上一个排列点已经超出开始位置 了，这个时候需要处理
				if(loopCount > clipper.width)
				{
					//超出了被绘制的区域了
					if(pixelAlpha>0)
					{

						pSrcBuffer+=((pixelCount - loopCount)<<1);
						unsigned short * pSrcTemp = (unsigned short*)pSrcBuffer;
						if(pixelAlpha == 255)
						{
							for(int i = 0; i < clipper.width; i++)
							{
						/*		unsigned short color = pSrcTemp[i];
								UCHAR red = (((color)>>11)<<3);
								UCHAR green = (((color>>5)&63)<<2);
								UCHAR blue = ((color&31)<<3);
								pDest[i] = ((255<<24)|(red<<16)|(green<<8)|(blue));*/
								pDest[i] = ((255<<24)|g_RGB565BIT_TO_ARGB32BIT(pSrcTemp[i]));
							}
						}
						else
						{
							for(int i = 0; i < clipper.width;i++)
							{
								unsigned int src_color = g_RGB565BIT_TO_ARGB32BIT(pSrcTemp[i]);
								pDest[i] = ((((UCHAR)pixelAlpha)<<24)|src_color);
								
							}
						}
						pSrcBuffer+=((loopCount)<<1);
					}
					pDest+=clipper.width;
				}
				else
				{
					if(pixelAlpha>0)
					{
						pSrcBuffer+=((pixelCount - loopCount)<<1);
						unsigned short * pSrcTemp = (unsigned short*)pSrcBuffer;
						if(pixelAlpha == 255)
						{
							for(int i = 0; i < loopCount; i++)
							{
								/*		unsigned short color = pSrcTemp[i];
								UCHAR red = (((color)>>11)<<3);
								UCHAR green = (((color>>5)&63)<<2);
								UCHAR blue = ((color&31)<<3);
								pDest[i] = ((255<<24)|(red<<16)|(green<<8)|(blue));*/
								pDest[i] = ((255<<24)|g_RGB565BIT_TO_ARGB32BIT(pSrcTemp[i]));
							}
						}
						else
						{

							for(int i = 0; i < loopCount;i++)
							{
								unsigned int src_color = g_RGB565BIT_TO_ARGB32BIT(pSrcTemp[i]);
								pDest[i] = ((((UCHAR)pixelAlpha)<<24)|src_color);
							}
						
						}
						pSrcBuffer+=(loopCount<<1);
					}
					pDest+=loopCount;

				}
				   

			}
			else
			{
				if(pixelAlpha>0)
					pSrcBuffer += (pixelCount<<1);
			}
		}
		//////////////////////////////
		////下一步绘制//////////////////////////////////////////////////////////////////////////
		targetPos +=clipper.width;
		while(currentPos < targetPos)
		{
			//绘制中间的了
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			int destAlpha = (255 - pixelAlpha);
			currentPos += pixelCount;
			int loopCount = 0;
			if(currentPos > targetPos)
				loopCount = pixelCount - (currentPos - targetPos);
			else
				loopCount = pixelCount;
			//绘制loopcount//
			if(pixelAlpha>0)
			{
				unsigned short * pSrcTemp = (unsigned short*)pSrcBuffer;
				if(pixelAlpha == 255)
				{
					for(int i = 0; i < loopCount;i++)
					{
						/*		unsigned short color = pSrcTemp[i];
								UCHAR red = (((color)>>11)<<3);
								UCHAR green = (((color>>5)&63)<<2);
								UCHAR blue = ((color&31)<<3);
								pDest[i] = ((255<<24)|(red<<16)|(green<<8)|(blue));*/
								pDest[i] = ((255<<24)|g_RGB565BIT_TO_ARGB32BIT(pSrcTemp[i]));
					}
				}
				else
				{
					for(int i = 0; i < loopCount; i++)
					{
						unsigned int src_color = g_RGB565BIT_TO_ARGB32BIT(pSrcTemp[i]);
					     pDest[i] = ((((UCHAR)pixelAlpha)<<24)|src_color);
					//	pDest[i] = pSrcTable[pSrcTemp[i]] + pDestTable[pDest[i]];
					}
						
				}
				pSrcBuffer+=(pixelCount<<1);
			}		
			pDest += loopCount;
		}
		//末尾处理
		targetPos+=clipper.right;
		while(currentPos<targetPos)
		{
			pixelCount=*pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			currentPos+=pixelCount;
			if(pixelAlpha>0)
				pSrcBuffer+=(pixelCount<<1);
			
		}
		pDest = pDestStartLine + destWidth;
	}

}
void CopSPRCell8ToBitmap32(LPVOID pSpr,
							  LPVOID dest,LPVOID pPalette,
							  int cell_x,int cell_y,
							  int spr_width,int spr_height,
							  int destWidth,int destHeight)
{
	GDIClipperInfo clipper;
	if(MakeClip(-cell_x,-cell_y,spr_width,spr_height,destWidth,destHeight,&clipper)==0)
		return;
	unsigned char* pSrcBuffer = (unsigned char* )pSpr;
	unsigned int* pDest  = (unsigned int* ) dest;
	pDest += clipper.y*destWidth+clipper.x;

	KPAL24* pal = (KPAL24*)pPalette;
	int topClipPixels = clipper.top*spr_width;
	int pixelCount = 0;
	int pixelAlpha = 0;
	//先定到要绘制区域的y//
	while(topClipPixels>0)
	{
		pixelCount = *pSrcBuffer;
		pSrcBuffer++;
		pixelAlpha = *pSrcBuffer;
		pSrcBuffer++;
		topClipPixels-=pixelCount;
		if(pixelAlpha>0)
			pSrcBuffer+=(pixelCount);
	}
	///接下来就一步一步绘制了
	int copyHeight = clipper.height;
	for(int i = 0 ; i < copyHeight;i++)
	{
		unsigned int* pDestStartLine  = (unsigned int*)pDest;
		int currentPos = 0;
		int targetPos = clipper.left;
		//先定位到开始绘制的位置/
		while(currentPos<targetPos)
		{
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			int destAlpha = (255 - pixelAlpha);
			currentPos+=pixelCount;
			int loopCount = currentPos  - targetPos;
			if(loopCount>0)
			{
				//到达开始位置时候，上一个排列点已经超出开始位置 了，这个时候需要处理
				if(loopCount > clipper.width)
				{
					//超出了被绘制的区域了
					if(pixelAlpha>0)
					{

						pSrcBuffer+=((pixelCount - loopCount));
						unsigned char * pSrcTemp = (unsigned char*)pSrcBuffer;
						if(pixelAlpha == 255)
						{
							for(int i = 0; i < clipper.width; i++)
							{
								UCHAR idx = pSrcTemp[i];
								UCHAR red = pal[idx].Red;
								UCHAR green = pal[idx].Green;
								UCHAR blue = pal[idx].Blue;
								pDest[i] = ((pixelAlpha<<24)|(red<<16)|(green<<8)|(blue));
							}
						}
						else
						{
							for(int i = 0; i < clipper.width;i++)
							{
								UCHAR idx = pSrcTemp[i];
								UCHAR red = pal[idx].Red;
								UCHAR green = pal[idx].Green;
								UCHAR blue = pal[idx].Blue;
								pDest[i] = ((((UCHAR)pixelAlpha)<<24)|(red<<16)|(green<<8)|(blue));
								
							}
						}
						pSrcBuffer+=((loopCount));
					}
					pDest+=clipper.width;
				}
				else
				{
					if(pixelAlpha>0)
					{
						pSrcBuffer+=((pixelCount - loopCount));
						unsigned char * pSrcTemp = (unsigned char*)pSrcBuffer;
						if(pixelAlpha == 255)
						{
							for(int i = 0; i < loopCount; i++)
							{
								UCHAR idx = pSrcTemp[i];
								UCHAR red = pal[idx].Red;
								UCHAR green = pal[idx].Green;
								UCHAR blue = pal[idx].Blue;
								pDest[i] = ((((UCHAR)pixelAlpha)<<24)|(red<<16)|(green<<8)|(blue));
							}
						}
						else
						{

							for(int i = 0; i < loopCount;i++)
							{
								UCHAR idx = pSrcTemp[i];
								UCHAR red = pal[idx].Red;
								UCHAR green = pal[idx].Green;
								UCHAR blue = pal[idx].Blue;
								pDest[i] = ((pixelAlpha<<24)|(red<<16)|(green<<8)|(blue));
							}
						
						}
						pSrcBuffer+=(loopCount);
					}
					pDest+=loopCount;

				}
				   

			}
			else
			{
				if(pixelAlpha>0)
					pSrcBuffer += (pixelCount);
			}
		}
		//////////////////////////////
		////下一步绘制//////////////////////////////////////////////////////////////////////////
		targetPos +=clipper.width;
		while(currentPos < targetPos)
		{
			//绘制中间的了
			pixelCount = *pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			int destAlpha = (255 - pixelAlpha);
			currentPos += pixelCount;
			int loopCount = 0;
			if(currentPos > targetPos)
				loopCount = pixelCount - (currentPos - targetPos);
			else
				loopCount = pixelCount;
			//绘制loopcount//
			if(pixelAlpha>0)
			{
				unsigned char * pSrcTemp = (unsigned char*)pSrcBuffer;
				if(pixelAlpha == 255)
				{
					for(int i = 0; i < loopCount;i++)
					{
						UCHAR idx = pSrcTemp[i];
						UCHAR red = pal[idx].Red;
						UCHAR green = pal[idx].Green;
						UCHAR blue = pal[idx].Blue;
						pDest[i] = ((((UCHAR)pixelAlpha)<<24)|(red<<16)|(green<<8)|(blue));
					}
				}
				else
				{
					for(int i = 0; i < loopCount; i++)
					{
						UCHAR idx = pSrcTemp[i];
						UCHAR red = pal[idx].Red;
						UCHAR green = pal[idx].Green;
						UCHAR blue = pal[idx].Blue;
						pDest[i] = ((((UCHAR)pixelAlpha)<<24)|(red<<16)|(green<<8)|(blue));
					}
						
				}
				pSrcBuffer+=(pixelCount);
			}		
			pDest += loopCount;
		}
		//末尾处理
		targetPos+=clipper.right;
		while(currentPos<targetPos)
		{
			pixelCount=*pSrcBuffer;
			pSrcBuffer++;
			pixelAlpha = *pSrcBuffer;
			pSrcBuffer++;
			currentPos+=pixelCount;
			if(pixelAlpha>0)
				pSrcBuffer+=(pixelCount);
			
		}
		pDest = pDestStartLine + destWidth;
	}
}