/////////////////////////////////////////////////////////////////////////////
//  FileName    :   LayoutParser.cpp
//  Creator     :   xiehong
//  Date        :   2006-12-18 18:00:00
//  Comment     :   render declare
//	Changes		:	
/////////////////////////////////////////////////////////////////////////////

#include "LayoutRender.h"

using namespace CEGUI;

LayoutRender::LayoutRender()
{
	_drawPanel = NULL;
}

LayoutRender::~LayoutRender()
{
	
}

void LayoutRender::setDrawPanel(KRenderCache* drawPanel)
{
	_drawPanel = drawPanel;	
}

void LayoutRender::Release()
{
	
}

Font* LayoutRender::getCEFont(const LOFont& font)
{
	Font* ceFont = NULL;
	if(FontManager::getSingleton().isFontPresent(font.family))
	{
		ceFont = FontManager::getSingleton().getFont(font.family);
	}
	else
	{
		ceFont = System::getSingleton().getDefaultFont();
	}
	return ceFont;
}

const Image* LayoutRender::getCEImage(const unsigned short* imagePath)
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

LORect LayoutRender::getWordSize(unsigned short codePoint, const LOFont& font)
{
	static Font* ceFont = getCEFont(font);
	static char fontBank[32] = "\0";
	if(strcmp(fontBank, font.family) != 0)
	{
		strcpy(fontBank, font.family);
		ceFont = getCEFont(font);
	}

	Size wordSize = ceFont->getWordSize(codePoint);

	return LORect(0, 0, wordSize.d_width, wordSize.d_height);
}

int LayoutRender::getLineHeight(const LOFont& font)
{
	int lineHeight = getCEFont(font)->getLineSpacing();
	return lineHeight;
}

int LayoutRender::getTextExtent(const unsigned short* text, const LOFont& font)
{
	Font* ceFont = getCEFont(font);
	if(ceFont == NULL)
	{
		return -1;
	}
	
	float extent = ceFont->getTextExtent(text, font.wordExtSpace);
	return extent;
}

int LayoutRender::getCharAtPixel(const unsigned short* text, int startCharIndex, int pixel,	const LOFont& font)
{
	Font* ceFont = getCEFont(font);
	if(ceFont == NULL)
	{
		return -1;
	}

	int charIndex = ceFont->getCharAtPixel(text, startCharIndex, pixel, font.wordExtSpace);
	return charIndex;
}

void LayoutRender::drawText(const unsigned short* text, 
							const LOFont& font, 
							const LORect& destArea, 
							const LOColor& color, 
							float alpha, 
							const LORect& clipper, 
							float zPos,
							int borderMode,
							bool underLine)
{
	static Font* ceFont = getCEFont(font);
	static char fontBank[32] = "\0";
	if(strcmp(fontBank, font.family) != 0)
	{
		strcpy(fontBank, font.family);
		ceFont = getCEFont(font);
	}

	int baseline = ceFont->getBaseline();
	CEGUI::Point destPos(destArea.getLeft(), destArea.getTop() + baseline);

	Rect clipperRect(clipper.getLeft(), clipper.getTop(), clipper.getRight(), clipper.getBottom());

	colour textColor(color.red, color.green, color.blue, alpha);
	
	if(_drawPanel == NULL)
	{
		ceFont->drawTextLine(text, Vector3(destArea.getLeft(), destArea.getTop() + baseline, zPos), clipperRect, textColor, borderMode > 0 ? true : false, font.wordExtSpace);
	}
	else
	{
		_drawPanel->cacheTextLine(text, destPos, clipperRect, ceFont, textColor, 0xFFFFFFFF, font.wordExtSpace, underLine);
	}
}

LORect LayoutRender::getImageArea(const unsigned short* imageName)
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

LOImageInfo LayoutRender::getImageInfo(const unsigned short* imageName)
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

void LayoutRender::drawImage(const unsigned short* imageName, 
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

	if(_drawPanel == NULL)
	{
		ColourRect colorRect(colour(color.red, color.green, color.blue));
		ceImage->draw(Rect(destArea.getLeft(), destArea.getTop(), destArea.getRight(), destArea.getBottom())
			, zPos, clipperRect, colorRect, TopLeftToBottomRight, alpha, curFrameCount);
	}
	else
	{
		_drawPanel->cacheImage(ceImage, destPos, clipperRect, curFrameCount);
	}
}

